#!/usr/bin/env python3
"""Exact checks of the versor transform: which sandwich form is the
outermorphism of a versor's action on vectors, by the parity of the versor's
antigrades; which parts of the sandwich a rigid(3) layout drops; the hand
values of the transform; and the formulas of the rigid wiki's Projections
page.

Run, with the interpreter isolated and no bytecode written into the
repository:

  python3 -I -B check.py [REPO]

REPO is the Galoot checkout to read (default: the checkout that holds this
script). The script reads REPO/test/reference/clifford_reference.py for the
geometric product of a form that is not diagonal, and to cross-check its own
product on the diagonal forms. It writes nothing. It prints one line per
form and per group and exits 1 if any check failed, 0 if all passed.

What is computed by what:

- The exterior algebra (wedge, reorder signs), the complements (from
  u ^ right(u) = I and left(u) ^ u = I), the grade
  maps, the metric exomorphism (Gram minors by the Leibniz expansion), and
  the geometric product of a diagonal form are written here. They share no
  code with clifford_reference.py, golden.py, conventions_check.py, or the
  library.
- On a diagonal form the product table written here is compared with
  clifford_reference.py's on every blade pair; on the other forms (the
  conformal ones and R(4,1,0) given by a Gram matrix) clifford_reference.py's
  table is the product.
- Coefficients are Fractions, or polynomials over the Fractions (class Poly)
  where a claim is checked for generic, symbolic inputs. A polynomial check
  is an identity in the variables, so it is a proof for that form and
  layout; a check over random Fractions is a sample.

Notation in the output: I is the antiscalar e12...n; V % x % V~ is the
sandwich V antiproduct x antiproduct antireverse(V), and rev(V) the reverse;
x^ is the grade involution and x' the antigrade involution (each blade of
antigrade m times (-1)^m). The rule is V % x % V~ for V of even antigrades
and -V % x' % V~ for V of odd antigrades.
"""

import itertools
import os
import pathlib
import random
import sys
from fractions import Fraction

DEFAULT_REPO = str(pathlib.Path(__file__).resolve().parents[5])
SEED = 20261107
TRIALS = 10       # random versors per form and per number of hyperplanes
COMPOSITIONS = 3  # random pairs per combination of parities
ENTRY = 3         # random integer entries are drawn from [-ENTRY, ENTRY]


class Checker:
    def __init__(self):
        self.counts = {}
        self.failures = []

    def ok(self, group, condition, what):
        self.counts[group] = self.counts.get(group, 0) + 1
        if not condition:
            self.failures.append(f"{group}: {what}")
        return condition


# --- Polynomials over the Fractions ------------------------------------------

class Poly:
    """A polynomial: a dict from a monomial, a sorted tuple of variable
    names with repetition, to a nonzero Fraction."""

    __slots__ = ("terms",)
    __hash__ = None

    def __init__(self, terms=None):
        self.terms = {m: c for m, c in (terms or {}).items() if c}

    @staticmethod
    def var(name):
        return Poly({(name,): Fraction(1)})

    @staticmethod
    def lift(x):
        return x if isinstance(x, Poly) else Poly({(): Fraction(x)})

    def __add__(self, other):
        r = dict(self.terms)
        for m, c in Poly.lift(other).terms.items():
            r[m] = r.get(m, 0) + c
        return Poly(r)

    __radd__ = __add__

    def __neg__(self):
        return Poly({m: -c for m, c in self.terms.items()})

    def __sub__(self, other):
        return self + -Poly.lift(other)

    def __rsub__(self, other):
        return Poly.lift(other) - self

    def __mul__(self, other):
        if not isinstance(other, Poly):
            return Poly({m: c * other for m, c in self.terms.items()})
        r = {}
        for m1, c1 in self.terms.items():
            for m2, c2 in other.terms.items():
                m = tuple(sorted(m1 + m2))
                r[m] = r.get(m, 0) + c1 * c2
        return Poly(r)

    __rmul__ = __mul__

    def __bool__(self):
        return bool(self.terms)

    def __eq__(self, other):
        return not (self - other)

    def __repr__(self):
        if not self.terms:
            return "0"
        return " + ".join(f"{c}*{'*'.join(m) or '1'}"
                          for m, c in sorted(self.terms.items()))


def variables(prefix, count):
    return [Poly.var(f"{prefix}{i}") for i in range(count)]


# --- Exterior algebra, written here ------------------------------------------

def popcount(m):
    return bin(m).count("1")


def bits(m):
    return [i for i in range(m.bit_length()) if m >> i & 1]


def reorder_sign(a, b):
    """-1 to the number of pairs i in A, j in B with i > j: the sign that
    sorts the generators of e_A followed by those of e_B. For disjoint A and
    B it is the sign of e_A ^ e_B against e_(A|B); for orthogonal generators
    it is also the sign of the product, with shared generators squared."""
    inversions = sum(1 for i in bits(a) for j in bits(b) if i > j)
    return -1 if inversions & 1 else 1


def clean(x):
    return {m: c for m, c in x.items() if c}


def add(*xs):
    r = {}
    for x in xs:
        for m, c in x.items():
            r[m] = r.get(m, 0) + c
    return clean(r)


def scale(x, s):
    return clean({m: c * s for m, c in x.items()})


def wedge(a, b):
    r = {}
    for ma, ca in a.items():
        for mb, cb in b.items():
            if not ma & mb:
                r[ma | mb] = r.get(ma | mb, 0) + ca * cb * reorder_sign(ma, mb)
    return clean(r)


def wedge_all(xs):
    r = {0: Fraction(1)}
    for x in xs:
        r = wedge(r, x)
    return r


def leibniz_det(m):
    n = len(m)
    total = Fraction(0)
    for perm in itertools.permutations(range(n)):
        inversions = sum(1 for i, j in itertools.combinations(perm, 2)
                         if i > j)
        term = Fraction(-1 if inversions & 1 else 1)
        for row, col in enumerate(perm):
            term *= m[row][col]
        total += term
    return total


def diag(*squares):
    n = len(squares)
    return [[squares[i] if i == j else 0 for j in range(n)] for i in range(n)]


def conformal(d):
    g = diag(*([1] * d + [0, 0]))
    g[d][d + 1] = g[d + 1][d] = -1
    return g


def r_pqr(p, q, r):
    return diag(*([1] * p + [-1] * q + [0] * r))


class Form:
    """The algebra of a symmetric Gram matrix: its geometric product,
    antiproduct, complements, and grade maps."""

    def __init__(self, label, gram, ref, ck):
        self.label, self.g = label, gram
        self.n = n = len(gram)
        self.size = 1 << n
        self.full = self.size - 1
        reference = ref.Algebra(gram).table()
        if all(gram[i][j] == 0 for i in range(n) for j in range(n) if i != j):
            table = {}
            for a in range(self.size):
                for b in range(self.size):
                    c = Fraction(1)
                    for i in bits(a & b):
                        c *= gram[i][i]
                    c *= reorder_sign(a, b)
                    table[(a, b)] = clean({a ^ b: c})
            ck.ok("product tables", table == reference,
                  f"{label}: own diagonal table against clifford_reference")
        else:
            table = reference
        self.table = {k: tuple(v.items()) for k, v in table.items()}
        blades = range(self.size)
        self.eps_right = [reorder_sign(a, self.full ^ a) for a in blades]
        self.eps_left = [reorder_sign(self.full ^ a, a) for a in blades]
        self.G = self.exomorphism()

    def exomorphism(self):
        """G e_A = sum over |B| = |A| of det g[A, B] e_B."""
        out = {}
        for a in range(self.size):
            rows = bits(a)
            terms = {}
            for cols in itertools.combinations(range(self.n), len(rows)):
                minor = leibniz_det([[Fraction(self.g[r][c]) for c in cols]
                                     for r in rows])
                if minor:
                    terms[sum(1 << c for c in cols)] = minor
            out[a] = terms
        return out

    def apply(self, matrix, x):
        r = {}
        for a, ca in x.items():
            for b, cb in matrix[a].items():
                r[b] = r.get(b, 0) + ca * cb
        return clean(r)

    def gp(self, x, y):
        r = {}
        for a, ca in x.items():
            for b, cb in y.items():
                for m, t in self.table[(a, b)]:
                    r[m] = r.get(m, 0) + ca * cb * t
        return clean(r)

    def right(self, x):
        return {self.full ^ a: c * self.eps_right[a] for a, c in x.items()}

    def left(self, x):
        return {self.full ^ a: c * self.eps_left[a] for a, c in x.items()}

    def antigp(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def antiwedge(self, x, y):
        return self.left(wedge(self.right(x), self.right(y)))

    def antigrade(self, a):
        return self.n - popcount(a)

    def reverse(self, x):
        return {a: c * rev_sign(popcount(a)) for a, c in x.items()}

    def antireverse(self, x):
        return {a: c * rev_sign(self.antigrade(a)) for a, c in x.items()}

    def grade_involution(self, x):
        return {a: c * (-1) ** popcount(a) for a, c in x.items()}

    def antigrade_involution(self, x):
        return {a: c * (-1) ** self.antigrade(a) for a, c in x.items()}

    def weight(self, x):
        return self.left(self.apply(self.G, self.right(x)))

    def weight_dual(self, x):
        return self.right(self.weight(x))

    def bulk_dual(self, x):
        return self.right(self.apply(self.G, x))

    def antiscalar(self, c=1):
        return {self.full: Fraction(c)}


def rev_sign(k):
    return -1 if (k * (k - 1) // 2) & 1 else 1


# --- Candidate sandwich forms ------------------------------------------------

INVOLUTIONS = ("none", "grade", "antigrade")
CONJUGATES = ("antireverse", "reverse")
CANDIDATES = [(s, inv, conj) for conj in CONJUGATES for inv in INVOLUTIONS
              for s in (1, -1)]


def candidate_name(c):
    s, inv, conj = c
    x = {"none": "x", "grade": "x^", "antigrade": "x'"}[inv]
    v = {"antireverse": "V~", "reverse": "rev(V)"}[conj]
    return f"{'+' if s > 0 else '-'}V % {x} % {v}"


RULE = {0: (1, "none", "antireverse"), 1: (-1, "antigrade", "antireverse")}


def sandwich_maps(f, v):
    """For each conjugate, the matrix A -> V % e_A % conj(V)."""
    conj = {"antireverse": f.antireverse(v), "reverse": f.reverse(v)}
    maps = {}
    for name, w in conj.items():
        maps[name] = {a: f.antigp(f.antigp(v, {a: Fraction(1)}), w)
                      for a in range(f.size)}
    return maps


def candidate_matrix(f, maps, c):
    s, inv, conj = c

    def sign(a):
        if inv == "grade":
            return (-1) ** popcount(a)
        if inv == "antigrade":
            return (-1) ** f.antigrade(a)
        return 1

    return {a: scale(maps[conj][a], s * sign(a)) for a in range(f.size)}


def antigrade_parity(f, v):
    parities = {f.antigrade(a) & 1 for a in v}
    return parities.pop() if len(parities) == 1 else None


def random_vector(f, rng):
    return clean({1 << i: Fraction(rng.randint(-ENTRY, ENTRY))
                  for i in range(f.n)})


def random_hyperplane(f, rng):
    """A hyperplane (antigrade 1) as the wedge of n - 1 random vectors,
    returned with those vectors, which lie in it."""
    while True:
        vs = [random_vector(f, rng) for _ in range(f.n - 1)]
        h = wedge_all(vs)
        lam = f.antigp(h, f.antireverse(h)).get(f.full, 0)
        if h and lam:
            return h, vs


def random_versor(f, rng, count):
    while True:
        hs = [random_hyperplane(f, rng) for _ in range(count)]
        v = {f.full: Fraction(1)}
        for h, _ in hs:
            v = f.antigp(v, h)
        if v:
            return v, hs


def outermorphism_holds(f, matrix, lam, rng):
    """lam^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak), k = 1 to n, on one
    random tuple of vectors per k."""
    for k in range(1, f.n + 1):
        vs = [random_vector(f, rng) for _ in range(k)]
        lhs = scale(f.apply(matrix, wedge_all(vs)), lam ** (k - 1))
        rhs = wedge_all([f.apply(matrix, v) for v in vs])
        if lhs != rhs:
            return False
    return True


def mirror_fixed(f, matrix, lam, inside, rng):
    """T(a) = lam a for the vectors that span the mirror, and T(b) != lam b
    for a random vector b off it."""
    if any(f.apply(matrix, a) != scale(a, lam) for a in inside):
        return False
    h = wedge_all(inside)
    while True:
        b = random_vector(f, rng)
        if wedge(b, h):
            return f.apply(matrix, b) != scale(b, lam)


def compose_matrices(f, outer, inner):
    return {a: f.apply(outer, inner[a]) for a in range(f.size)}


def check_parity(ck, f, rng):
    """Classify the twelve candidate forms on random versors of f."""
    samples = {1: [], 2: [], 3: []}
    outer = {(p, c): True for p in (0, 1) for c in CANDIDATES}
    mirror = {c: True for c in CANDIDATES}
    kind = {(p, c): set() for p in (0, 1) for c in CANDIDATES}
    for count in (1, 2, 3):
        for _ in range(TRIALS):
            v, hs = random_versor(f, rng, count)
            p = antigrade_parity(f, v)
            ck.ok("versors", p == count % 2,
                  f"{f.label}: {count} hyperplanes give antigrade parity {p}")
            norm = f.antigp(v, f.antireverse(v))
            lam = norm.get(f.full, 0)
            ck.ok("versors", lam != 0 and norm == {f.full: lam},
                  f"{f.label}: V % V~ = lambda antiscalar, "
                  f"{count} hyperplanes")
            maps = sandwich_maps(f, v)
            rule = candidate_matrix(f, maps, RULE[p])
            twisted = {a: scale(rule[a], (-1) ** popcount(a))
                       for a in range(f.size)}
            samples[count].append((v, p))
            for c in CANDIDATES:
                m = candidate_matrix(f, maps, c)
                kind[(p, c)].add("rule" if m == rule else
                                 "rule x^" if m == twisted else "other")
                if not outermorphism_holds(f, m, lam, rng):
                    outer[(p, c)] = False
                if count == 1 and not mirror_fixed(f, m, lam, hs[0][1], rng):
                    mirror[c] = False

    # The rule and its twist by the grade involution against the claims.
    for p in (0, 1):
        ck.ok("parity", outer[(p, RULE[p])],
              f"{f.label}: the rule is an outermorphism, parity {p}")
        for c in CANDIDATES:
            if outer[(p, c)]:
                ck.ok("parity", kind[(p, c)] <= {"rule", "rule x^"},
                      f"{f.label}: {candidate_name(c)} passes the "
                      f"outermorphism check but is neither the rule nor "
                      f"the rule after x^, parity {p}")
    ck.ok("parity", mirror[RULE[1]],
          f"{f.label}: the odd rule fixes its mirror")
    for c in CANDIDATES:
        if mirror[c] and outer[(1, c)]:
            ck.ok("parity", kind[(1, c)] == {"rule"},
                  f"{f.label}: {candidate_name(c)} fixes the mirror and "
                  f"passes, but is not the rule")

    # Composition: T_(W % V) = T_W o T_V, for the rule and for the two
    # families that follow the rule by x^ at one parity.
    families = {
        "rule": {0: False, 1: False},
        "odd twisted": {0: False, 1: True},
        "even twisted": {0: True, 1: False},
    }

    def family_matrix(twist, maps, p):
        m = candidate_matrix(f, maps, RULE[p])
        if twist[p]:
            m = {a: scale(m[a], (-1) ** popcount(a)) for a in range(f.size)}
        return m

    composes = {name: True for name in families}
    for i in range(COMPOSITIONS):
        for c1, c2 in itertools.product((1, 2), repeat=2):
            v, pv = samples[c1][i]
            w, pw = samples[c2][-1 - i]
            wv = f.antigp(w, v)
            pwv = antigrade_parity(f, wv)
            ck.ok("composition", pwv == (pv + pw) % 2,
                  f"{f.label}: antigrade parities add under the antiproduct")
            mv, mw, mwv = (sandwich_maps(f, u) for u in (v, w, wv))
            for name, twist in families.items():
                lhs = family_matrix(twist, mwv, pwv)
                rhs = compose_matrices(f, family_matrix(twist, mw, pw),
                                       family_matrix(twist, mv, pv))
                if lhs != rhs:
                    composes[name] = False
    ck.ok("composition", composes["rule"], f"{f.label}: the rule composes")
    ck.ok("composition", composes["odd twisted"],
          f"{f.label}: the family with odd versors followed by x^ composes")
    ck.ok("composition", not composes["even twisted"],
          f"{f.label}: the family with even versors followed by x^ "
          f"composes, and should not")

    def passing(p, need_mirror):
        return [candidate_name(c) for c in CANDIDATES
                if outer[(p, c)] and (mirror[c] or not need_mirror)]

    # The sign s of s V % x^ % V~ for an odd V, from the outermorphism
    # check alone; the mirror check must then agree.
    odd_grade_sign = [c[0] for c in CANDIDATES
                      if c[1:] == ("grade", "antireverse") and outer[(1, c)]]
    ck.ok("parity", len(odd_grade_sign) == 1,
          f"{f.label}: signs of s V % x^ % V~ that pass: {odd_grade_sign}")
    ck.ok("parity", all(mirror[(s, "grade", "antireverse")]
                        for s in odd_grade_sign),
          f"{f.label}: the sign found fixes the mirror")
    return {
        "even": passing(0, False),
        "odd outer": passing(1, False),
        "odd outer+mirror": passing(1, True),
        "odd sign with x^": odd_grade_sign,
        "composes": composes,
    }


# --- The antigrade lemma and the dropped parts -------------------------------

def check_antigrade_lemma(ck, f, rng):
    """For V of one antigrade parity, not necessarily a versor, and x of
    antigrade k, V % x % V~ has only antigrades congruent to k mod 4."""
    for _ in range(TRIALS):
        p = rng.randint(0, 1)
        v = clean({a: Fraction(rng.randint(-ENTRY, ENTRY))
                   for a in range(f.size) if f.antigrade(a) % 2 == p})
        k = rng.randint(0, f.n)
        x = clean({a: Fraction(rng.randint(-ENTRY, ENTRY))
                   for a in range(f.size) if f.antigrade(a) == k})
        y = f.antigp(f.antigp(v, x), f.antireverse(v))
        ck.ok("antigrade lemma", all((f.antigrade(a) - k) % 4 == 0 for a in y),
              f"{f.label}: antigrades of V % x % V~ for x of antigrade {k}")


def parse(name):
    """Lengyel's oriented name to (mask, sign): "e423" is e4 e2 e3 = +e234;
    "e1234" in rigid(3) is the antiscalar."""
    if name == "1":
        return 0, 1
    idx = [int(ch) for ch in name[1:]]
    inversions = sum(1 for i, j in itertools.combinations(idx, 2) if i > j)
    return sum(1 << (i - 1) for i in idx), -1 if inversions & 1 else 1


def build(pairs):
    """Sum of coefficient times named blade, from (name, coefficient)."""
    r = {}
    for name, c in pairs:
        mask, sign = parse(name)
        r[mask] = r.get(mask, 0) + c * sign
    return clean(r)


def coeffs(x, names):
    out = []
    for name in names:
        mask, sign = parse(name)
        out.append(x.get(mask, 0) * sign)
    return out


LAYOUTS = {
    "point": ("e1", "e2", "e3", "e4"),
    "line": ("e41", "e42", "e43", "e23", "e31", "e12"),
    "plane": ("e423", "e431", "e412", "e321"),
    "motor": ("e41", "e42", "e43", "e1234", "e23", "e31", "e12", "1"),
    "flector": ("e1", "e2", "e3", "e4", "e423", "e431", "e412", "e321"),
}


def reach(f, xs, ys):
    """Blades the antiproduct plan of layouts xs and ys can reach: the
    complements of the blades of right(x) right(y) with a nonzero table
    entry."""
    out = set()
    for a in xs:
        for b in ys:
            for m, t in f.table[(f.full ^ a, f.full ^ b)]:
                if t:
                    out.add(f.full ^ m)
    return out


def transform(f, v, x):
    p = antigrade_parity(f, v)
    s, inv, _ = RULE[p]
    xi = f.antigrade_involution(x) if inv == "antigrade" else x
    return scale(f.antigp(f.antigp(v, xi), f.antireverse(v)), s)


def check_dropped(ck, rigid3):
    """Generic, symbolic V of the motor and flector layouts on generic x of
    each layout: which blades the plan reaches outside x's, and whether any
    of them is nonzero as a polynomial."""
    f = rigid3
    rows = []
    for vname in ("motor", "flector"):
        vnames = LAYOUTS[vname]
        v = build(zip(vnames, variables("v", len(vnames))))
        vmasks = {parse(n)[0] for n in vnames}
        for xname, xnames in LAYOUTS.items():
            x = build(zip(xnames, variables("x", len(xnames))))
            xmasks = {parse(n)[0] for n in xnames}
            reached = reach(f, reach(f, vmasks, xmasks), vmasks)
            y = transform(f, v, x)
            dropped = sorted(reached - xmasks)
            nonzero = [m for m in dropped if y.get(m)]
            ck.ok("dropped parts", not nonzero,
                  f"{vname} on {xname}: nonzero dropped blades {nonzero}")
            ck.ok("dropped parts", set(y) <= reached,
                  f"{vname} on {xname}: result outside the reached blades")
            rows.append(f"{vname} on {xname}: reaches {len(reached)}, "
                        f"drops {len(dropped)}, "
                        f"nonzero dropped {len(nonzero)}")
    # The lemma, symbolically: V generic of either parity (the motor and
    # flector layouts are all even and all odd blades), x generic of one
    # antigrade.
    for vname in ("motor", "flector"):
        vnames = LAYOUTS[vname]
        v = build(zip(vnames, variables("v", len(vnames))))
        for k in range(f.n + 1):
            masks = [a for a in range(f.size) if f.antigrade(a) == k]
            x = dict(zip(masks, variables("x", len(masks))))
            y = f.antigp(f.antigp(v, x), f.antireverse(v))
            ck.ok("antigrade lemma",
                  all((f.antigrade(a) - k) % 4 == 0 for a in y),
                  f"rigid(3), generic {vname}, x of antigrade {k}")
    return rows


# --- Hand values -------------------------------------------------------------

def check_hand_values(ck, ref):
    rows = []
    r4 = Form("R(4,0,0)", r_pqr(4, 0, 0), ref, ck)
    v = {0: Fraction(1), r4.full: Fraction(1)}
    y = r4.antigp(r4.antigp(v, {0: Fraction(1)}), r4.antireverse(v))
    norm = r4.antigp(v, r4.antireverse(v))
    ck.ok("hand values", y == {0: 2, r4.full: 2},
          f"R(4,0,0): (1 + I) % 1 % (1 + I)~ = {y}")
    ck.ok("hand values", norm == {0: 2, r4.full: 2},
          f"R(4,0,0): V % V~ = {norm}")
    rows.append(f"R(4,0,0), V = 1 + I, x = 1: V % x % V~ = "
                f"{y.get(0, 0)} + {y.get(r4.full, 0)} I, the I part dropped; "
                f"V % V~ = {norm.get(0, 0)} + {norm.get(r4.full, 0)} I")

    # V % V~ a multiple of the antiscalar is not enough in six generators.
    r6 = Form("R(6,0,0)", r_pqr(6, 0, 0), ref, ck)
    v = {0: Fraction(1), r6.full: Fraction(1)}
    norm = r6.antigp(v, r6.antireverse(v))
    y = r6.antigp(r6.antigp(v, {1: Fraction(1)}), r6.antireverse(v))
    ck.ok("hand values", norm == {r6.full: 2}, f"R(6,0,0): V % V~ = {norm}")
    ck.ok("hand values", y and all(popcount(a) != 1 for a in y),
          f"R(6,0,0): V % e1 % V~ = {y} has a vector part")
    rows.append(f"R(6,0,0), V = 1 + I: V % V~ = {norm.get(r6.full, 0)} I, "
                f"yet V % e1 % V~ = "
                + " + ".join(f"{c} e{''.join(str(i + 1) for i in bits(a))}"
                             for a, c in sorted(y.items())))

    f = Form("rigid(3)", r_pqr(3, 0, 1), ref, ck)
    xyzw = LAYOUTS["point"]

    def point(x, y, z, w=1):
        return build(zip(xyzw, map(Fraction, (x, y, z, w))))

    def moved(v, p):
        return coeffs(transform(f, v, p), xyzw)

    s, c = Fraction(3, 5), Fraction(4, 5)
    cases = [
        ("rotor (3/5) e43 + (4/5) I on (1, 0, 0, 1)",
         build([("e43", s), ("e1234", c)]), point(1, 0, 0),
         [Fraction(7, 25), Fraction(24, 25), 0, 1]),
        ("rotor 3 e43 + 4 I on (1, 0, 0, 1)",
         build([("e43", 3), ("e1234", 4)]), point(1, 0, 0), [7, 24, 0, 25]),
        ("translator e23 + 2 e31 - e12 + I on (5, 7, -1, 1)",
         build([("e23", 1), ("e31", 2), ("e12", -1), ("e1234", 1)]),
         point(5, 7, -1), [7, 11, -3, 1]),
        ("mirror e412 on (1, 2, 3, 1)", build([("e412", 1)]), point(1, 2, 3),
         [1, 2, -3, 1]),
    ]
    rotor = cases[0][1]
    twice = f.antigp(rotor, rotor)
    cases.append(("rotor (3/5) e43 + (4/5) I composed with itself on "
                  "(1, 0, 0, 1)", twice, point(1, 0, 0),
                  [Fraction(-527, 625), Fraction(336, 625), 0, 1]))
    irotor = cases[1][1]
    cases.append(("rotor 3 e43 + 4 I composed with itself on (1, 0, 0, 1)",
                  f.antigp(irotor, irotor), point(1, 0, 0),
                  [-527, 336, 0, 625]))
    for what, v, p, expect in cases:
        got = moved(v, p)
        ck.ok("hand values", got == expect, f"rigid(3): {what} = {got}")
        rows.append(f"rigid(3): {what} -> ({', '.join(str(g) for g in got)})")
    ck.ok("hand values",
          moved(twice, point(1, 0, 0)) ==
          moved(rotor, transform(f, rotor, point(1, 0, 0))),
          "rigid(3): the composed rotor equals the rotor applied twice")

    f2 = Form("rigid(2)", r_pqr(2, 0, 1), ref, ck)
    mirror = {0b110: Fraction(1)}  # e23
    p = {1: Fraction(1), 2: Fraction(2), 4: Fraction(1)}
    got = transform(f2, mirror, p)
    ck.ok("hand values", got == {1: -1, 2: 2, 4: 1},
          f"rigid(2): mirror e23 on (1, 2, 1) = {got}")
    rows.append("rigid(2): mirror e23 on (1, 2, 1) -> ("
                + ", ".join(str(got.get(m, 0)) for m in (1, 2, 4)) + ")")
    return rows


# --- The Projections page ----------------------------------------------------

def check_projections(ck, ref):
    """The ten formulas of the rigid wiki's Projections page (revision 352),
    transcribed from its LaTeX into Galoot's terms, as polynomial identities
    in rigid(3)."""
    f = Form("rigid(3)", r_pqr(3, 0, 1), ref, ck)
    V = Poly.var
    px, py, pz, pw = (V(s) for s in ("px", "py", "pz", "pw"))
    gx, gy, gz, gw = (V(s) for s in ("gx", "gy", "gz", "gw"))
    lvx, lvy, lvz = (V(s) for s in ("lvx", "lvy", "lvz"))
    lmx, lmy, lmz = (V(s) for s in ("lmx", "lmy", "lmz"))
    p = build([("e1", px), ("e2", py), ("e3", pz), ("e4", pw)])
    g = build([("e423", gx), ("e431", gy), ("e412", gz), ("e321", gw)])
    line = build([("e41", lvx), ("e42", lvy), ("e43", lvz),
                  ("e23", lmx), ("e31", lmy), ("e12", lmz)])
    e4, e321 = build([("e4", 1)]), build([("e321", 1)])
    gn2 = gx * gx + gy * gy + gz * gz
    lv2 = lvx * lvx + lvy * lvy + lvz * lvz
    lm2 = lmx * lmx + lmy * lmy + lmz * lmz
    g_dot_p = gx * px + gy * py + gz * pz
    g_dot_lv = gx * lvx + gy * lvy + gz * lvz
    vxm = (lvy * lmz - lvz * lmy, lvz * lmx - lvx * lmz, lvx * lmy - lvy * lmx)

    def proj(a, b):
        return f.antiwedge(b, wedge(a, f.weight_dual(b)))

    def antiproj(a, b):
        return wedge(b, f.antiwedge(a, f.weight_dual(b)))

    rows = [
        ("projection of point p onto plane g", proj(p, g),
         add(scale(p, gn2),
             build([("e1", -(g_dot_p + gw * pw) * gx),
                    ("e2", -(g_dot_p + gw * pw) * gy),
                    ("e3", -(g_dot_p + gw * pw) * gz)]))),
        ("projection of point p onto line l", proj(p, line),
         build([("e1", (lvx * px + lvy * py + lvz * pz) * lvx + vxm[0] * pw),
                ("e2", (lvx * px + lvy * py + lvz * pz) * lvy + vxm[1] * pw),
                ("e3", (lvx * px + lvy * py + lvz * pz) * lvz + vxm[2] * pw),
                ("e4", lv2 * pw)])),
        ("projection of line l onto plane g", proj(line, g),
         build([("e41", gn2 * lvx - g_dot_lv * gx),
                ("e42", gn2 * lvy - g_dot_lv * gy),
                ("e43", gn2 * lvz - g_dot_lv * gz),
                ("e23", (gx * lmx + gy * lmy + gz * lmz) * gx
                 - (gy * lvz - gz * lvy) * gw),
                ("e31", (gx * lmx + gy * lmy + gz * lmz) * gy
                 - (gz * lvx - gx * lvz) * gw),
                ("e12", (gx * lmx + gy * lmy + gz * lmz) * gz
                 - (gx * lvy - gy * lvx) * gw)])),
        ("antiprojection of plane g onto point p", antiproj(g, p),
         build([("e423", pw * pw * gx), ("e431", pw * pw * gy),
                ("e412", pw * pw * gz), ("e321", -g_dot_p * pw)])),
        ("antiprojection of line l onto point p", antiproj(line, p),
         build([("e41", pw * pw * lvx), ("e42", pw * pw * lvy),
                ("e43", pw * pw * lvz),
                ("e23", (py * lvz - pz * lvy) * pw),
                ("e31", (pz * lvx - px * lvz) * pw),
                ("e12", (px * lvy - py * lvx) * pw)])),
        ("antiprojection of plane g onto line l", antiproj(g, line),
         build([("e423", lv2 * gx - g_dot_lv * lvx),
                ("e431", lv2 * gy - g_dot_lv * lvy),
                ("e412", lv2 * gz - g_dot_lv * lvz),
                ("e321", gx * lmy * lvz - gx * lmz * lvy + gy * lmz * lvx
                 - gy * lmx * lvz + gz * lmx * lvy - gz * lmy * lvx)])),
        ("projection of the origin onto plane g", proj(e4, g),
         build([("e1", -gx * gw), ("e2", -gy * gw), ("e3", -gz * gw),
                ("e4", gn2)])),
        ("projection of the origin onto line l", proj(e4, line),
         build([("e1", vxm[0]), ("e2", vxm[1]), ("e3", vxm[2]),
                ("e4", lv2)])),
        ("antiprojection of the horizon onto point p, bulk dual",
         wedge(p, f.antiwedge(e321, f.bulk_dual(p))),
         build([("e423", px * pw), ("e431", py * pw), ("e412", pz * pw),
                ("e321", -(px * px + py * py + pz * pz))])),
        ("antiprojection of the horizon onto line l, bulk dual",
         wedge(line, f.antiwedge(e321, f.bulk_dual(line))),
         build([("e423", lmy * lvz - lmz * lvy),
                ("e431", lmz * lvx - lmx * lvz),
                ("e412", lmx * lvy - lmy * lvx), ("e321", lm2)])),
    ]
    out = []
    for what, got, page in rows:
        if got == page:
            relation = "agrees"
        elif got == scale(page, -1):
            relation = "is the negation of the page's row"
        else:
            relation = "DIFFERS"
        out.append(f"{what}: {relation}")
        # The horizon row for a point is the one row whose sign the page
        # has reversed; with the left bulk dual it would agree.
        if what.startswith("antiprojection of the horizon onto point"):
            ck.ok("projections page", relation.startswith("is the negation"),
                  what)
            left_dual = f.left(f.apply(f.G, p))
            ck.ok("projections page",
                  wedge(p, f.antiwedge(e321, left_dual)) == page,
                  f"{what}: with the left bulk dual")
        else:
            ck.ok("projections page", relation == "agrees", what)

    # What the antiprojection formulas say, as identities.
    a = antiproj(g, p)
    ck.ok("projections page", f.antiwedge(p, a) == {},
          "the antiprojection of a plane onto a point contains the point")
    ck.ok("projections page", antiproj(a, p) == scale(a, pw * pw),
          "antiprojecting the result onto p again scales it by pw^2")
    a = antiproj(line, p)
    ck.ok("projections page", wedge(p, a) == {},
          "the antiprojection of a line onto a point contains the point")
    # A genuine line, the join of two points, lies in the plane.
    qx, qy, qz, qw = (V(s) for s in ("qx", "qy", "qz", "qw"))
    q = build([("e1", qx), ("e2", qy), ("e3", qz), ("e4", qw)])
    pq = wedge(p, q)
    ck.ok("projections page", f.antiwedge(pq, antiproj(g, pq)) == {},
          "the antiprojection of a plane onto the line p ^ q contains it")
    return out


def main(argv):
    repo = argv[0] if argv else DEFAULT_REPO
    sys.path.insert(0, os.path.join(repo, "test", "reference"))
    import clifford_reference as ref

    ck = Checker()
    rng = random.Random(SEED)
    forms = [
        ("rigid(2)", r_pqr(2, 0, 1)), ("R(3,0,0)", r_pqr(3, 0, 0)),
        ("rigid(3)", r_pqr(3, 0, 1)), ("R(2,2,0)", r_pqr(2, 2, 0)),
        ("conformal(2)", conformal(2)), ("rigid(4)", r_pqr(4, 0, 1)),
        ("conformal(3)", conformal(3)), ("R(4,1,0)", r_pqr(4, 1, 0)),
    ]
    ck.ok("forms", conformal(2) == ref.conformal(2), "conformal(2)")
    ck.ok("forms", conformal(3) == ref.conformal(3), "conformal(3)")
    ck.ok("forms", r_pqr(3, 0, 1) == ref.rigid(3), "rigid(3)")
    print(f"python {sys.version.split()[0]}; seed {SEED}; {TRIALS} versors "
          f"per form and number of hyperplanes (1, 2, 3); {COMPOSITIONS} "
          f"pairs per pair of parities; entries in [-{ENTRY}, {ENTRY}]")
    for label, g in forms:
        f = Form(label, g, ref, ck)
        for a in range(f.size):
            x = {a: Fraction(1)}
            ck.ok("complements", f.left(f.right(x)) == x,
                  f"{label}: left(right(e_A)) = e_A")
            ck.ok("complements", wedge(x, f.right(x)) == f.antiscalar(),
                  f"{label}: e_A ^ right(e_A) = I")
            ck.ok("involutions",
                  f.right(f.antigrade_involution(x)) ==
                  f.grade_involution(f.right(x)),
                  f"{label}: right(x') = right(x)^")
            ck.ok("involutions",
                  f.right(f.antireverse(x)) == f.reverse(f.right(x)),
                  f"{label}: right(antireverse x) = reverse(right x)")
            ck.ok("involutions",
                  f.antigrade_involution(x) ==
                  scale(f.grade_involution(x), (-1) ** f.n),
                  f"{label}: x' = (-1)^n x^")
        r = check_parity(ck, f, rng)
        check_antigrade_lemma(ck, f, rng)
        print(f"{label} | n {f.n} | even: {', '.join(r['even'])} | "
              f"odd, outermorphism: {', '.join(r['odd outer'])} | "
              f"odd, and mirror fixed: {', '.join(r['odd outer+mirror'])} | "
              f"odd sign s in s V % x^ % V~ by the outermorphism check: "
              f"{r['odd sign with x^']} | "
              f"composes: rule {r['composes']['rule']}, odd twisted "
              f"{r['composes']['odd twisted']}, even twisted "
              f"{r['composes']['even twisted']}")

    for row in check_dropped(ck, Form("rigid(3)", r_pqr(3, 0, 1), ref, ck)):
        print(row)
    for row in check_hand_values(ck, ref):
        print(row)
    for row in check_projections(ck, ref):
        print(row)

    width = max(len(k) for k in ck.counts)
    for group, count in ck.counts.items():
        print(f"{group:<{width}}  {count:6d} checks")
    print(f"total {sum(ck.counts.values())} checks")
    if ck.failures:
        print("FAILED (seed %d):" % SEED)
        print("\n".join(ck.failures[:40]))
        return 1
    print("all checks passed (seed %d)" % SEED)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
