#!/usr/bin/env python3
"""Exact checks of the identities behind the bulk dual, the weight dual, the
dot and the antidot, and of the exomorphisms namespace of golden.hpp.

Run, with the interpreter isolated and no bytecode written into the
repository:

  python3 -I -B check.py [REPO]

REPO is the Galoot checkout to read (default: the checkout that holds this
script). The
script reads REPO/test/reference/clifford_reference.py for its geometric
product and REPO/test/reference/golden.hpp for the values it compares. It
writes nothing. It exits 1 on the first failed group, 0 if all pass.

What is computed by what:

- Exterior algebra (wedge, reorder signs), complements, contractions, the
  reverse, determinants (Leibniz expansion over permutations), and the
  metric exomorphism are written here and share no code with
  clifford_reference.py or golden.py.
- G is computed twice here: from Gram minors with the Leibniz determinant,
  and as the outermorphism, the wedge of the images g e_i of the generators.
- The geometric product is computed twice: by clifford_reference.py
  (diagonalize the form, multiply in the orthogonal basis, change back), and
  here by the Chevalley recursion e_a e_B' = iota_a(e_B') + e_a ^ e_B' with
  e_A = e_a ^ e_A' = e_a e_A' - iota_a(e_A'). The two must agree on every
  blade pair, and every identity that involves a product is checked with
  both.
- The forms are written here and compared with clifford_reference.py's
  constructors and with the Gram matrices in golden.hpp.
"""

import itertools
import os
import re
import pathlib
import sys
from fractions import Fraction

DEFAULT_REPO = str(pathlib.Path(__file__).resolve().parents[5])


# --- Forms, written here -----------------------------------------------------

def diag(*squares):
    n = len(squares)
    return [[squares[i] if i == j else 0 for j in range(n)] for i in range(n)]


def conformal(d):
    g = diag(*([1] * d + [0, 0]))
    g[d][d + 1] = g[d + 1][d] = -1
    return g


FORMS = [
    ("rigid(3) = R(3,0,1)", diag(1, 1, 1, 0)),
    ("conformal(3)", conformal(3)),
    ("R(1,1,0)", diag(1, -1)),
    ("R(2,0,2)", diag(1, 1, 0, 0)),
    ("degenerate [[0,1,0],[1,0,0],[0,0,0]]", [[0, 1, 0], [1, 0, 0], [0, 0, 0]]),
    ("Witt [[0,1],[1,0]]", [[0, 1], [1, 0]]),
    ("R(3,0,0)", diag(1, 1, 1)),
]


# --- Exterior algebra, written here ------------------------------------------

def bits(m):
    return [i for i in range(m.bit_length()) if m >> i & 1]


def grade(m):
    return bin(m).count("1")


def name(m):
    return "1" if m == 0 else "e" + "".join(str(i + 1) for i in bits(m))


def wedge_sign(a, b):
    """e_A ^ e_B = sign e_(A u B) for disjoint A, B; 0 if they meet. The
    sign is (-1)^(number of pairs i in A, j in B with i > j), the
    transpositions that sort the concatenated index list."""
    if a & b:
        return 0
    inversions = sum(1 for i in bits(a) for j in bits(b) if i > j)
    return -1 if inversions % 2 else 1


def clean(x):
    return {m: c for m, c in x.items() if c != 0}


def add(*xs):
    out = {}
    for x in xs:
        for m, c in x.items():
            out[m] = out.get(m, 0) + c
    return clean(out)


def scale(c, x):
    return clean({m: c * v for m, v in x.items()})


def blade(m, c=1):
    return {m: Fraction(c)}


def wedge(x, y):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            s = wedge_sign(a, b)
            if s:
                out[a | b] = out.get(a | b, 0) + s * ca * cb
    return clean(out)


def leibniz_det(rows):
    k = len(rows)
    total = Fraction(0)
    for perm in itertools.permutations(range(k)):
        inv = sum(1 for p, q in itertools.combinations(perm, 2) if p > q)
        term = Fraction(-1 if inv % 2 else 1)
        for i in range(k):
            term *= rows[i][perm[i]]
        total += term
    return total


class Form:
    def __init__(self, label, g):
        self.label, self.g, self.n = label, g, len(g)
        self.size = 1 << self.n
        self.full = self.size - 1
        self.one = blade(self.full)
        n = self.n
        self.det = leibniz_det(g)
        # Complements from their definitions, by the wedge written here.
        self.right, self.left = {}, {}
        for u in range(self.size):
            c = self.full ^ u
            self.right[u] = (c, wedge_sign(u, c))   # u ^ (s e_c) = 1-bar
            self.left[u] = (c, wedge_sign(c, u))    # (s e_c) ^ u = 1-bar
        # G by Gram minors (Leibniz) and by the outermorphism.
        self.G = {}
        for a in range(self.size):
            rows = bits(a)
            img = {}
            for cols in itertools.combinations(range(n), len(rows)):
                minor = leibniz_det([[g[r][c] for c in cols] for r in rows])
                if minor:
                    img[sum(1 << c for c in cols)] = minor
            self.G[a] = img
        self.G_outer = {}
        for a in range(self.size):
            x = blade(0)
            for i in bits(a):
                x = wedge(x, {1 << j: Fraction(g[i][j]) for j in range(n)
                              if g[i][j]})
            self.G_outer[a] = x
        self.cache = {}

    # Linear maps given on blades.
    def apply(self, table, x):
        return add(*[scale(c, table[m]) for m, c in x.items()])

    def comp(self, which, x):
        tab = self.right if which == "right" else self.left
        return clean({tab[m][0]: c * tab[m][1] for m, c in x.items()})

    def Gm(self, x):
        return self.apply(self.G, x)

    def antiG(self, x):
        return self.comp("left", self.Gm(self.comp("right", x)))

    def reverse(self, x):
        """The reverse of the wedge monomial e_a1 ^ ... ^ e_ak is
        e_ak ^ ... ^ e_a1, computed with the wedge written here."""
        out = {}
        for m, c in x.items():
            r = blade(0)
            for i in reversed(bits(m)):
                r = wedge(r, blade(1 << i))
            (mm, s), = r.items()
            assert mm == m
            out[m] = c * s
        return clean(out)

    # Contraction by a generator: the antiderivation with iota_i e_j = g_ij.
    def iota(self, i, x):
        out = {}
        for m, c in x.items():
            for pos, j in enumerate(bits(m)):
                if self.g[i][j]:
                    t = m ^ (1 << j)
                    out[t] = out.get(t, 0) + (-1) ** pos * self.g[i][j] * c
        return clean(out)

    def contract(self, x, y):
        """Left contraction of y by x: for a blade e_A with a1 < ... < ak,
        e_A _| y = iota_a1(iota_a2(... iota_ak(y)))."""
        out = {}
        for m, c in x.items():
            z = dict(y)
            for i in reversed(bits(m)):
                z = self.iota(i, z)
            out = add(out, scale(c, z))
        return out

    # Geometric product by the Chevalley recursion, written here.
    def vec_mul(self, i, y):
        return add(self.iota(i, y), wedge(blade(1 << i), y))

    def blade_mul(self, a, b):
        key = (a, b)
        if key in self.cache:
            return self.cache[key]
        if a == 0:
            out = blade(b)
        else:
            i = bits(a)[0]
            rest = a ^ (1 << i)             # e_A = e_i ^ e_rest, sign +1
            out = self.vec_mul(i, self.blade_mul(rest, b))
            for m, c in self.iota(i, blade(rest)).items():
                out = add(out, scale(-c, self.blade_mul(m, b)))
        self.cache[key] = out
        return out

    def mul(self, x, y):
        return add(*[scale(cx * cy, self.blade_mul(a, b))
                     for a, cx in x.items() for b, cy in y.items()])


def bil(form, table_fn, a, b):
    """a^T M b in blade coordinates, for the linear map M = table_fn."""
    mb = table_fn(b)
    return sum((c * mb.get(m, 0) for m, c in a.items()), Fraction(0))


# --- The checks ---------------------------------------------------------------

class Checker:
    def __init__(self):
        self.counts = {}
        self.failures = []

    def eq(self, group, got, want, what):
        self.counts[group] = self.counts.get(group, 0) + 1
        if got != want:
            if len(self.failures) < 20:
                self.failures.append(f"{group}: {what}: {got!r} != {want!r}")


def check_form(ck, f, ref_alg):
    n, size, one = f.n, f.size, f.one
    lab = f.label
    ref_mul = ref_alg.mul
    products = (("chevalley", f.mul), ("clifford_reference", ref_mul))

    # Foundations.
    for a in range(size):
        ck.eq("G minors = G outermorphism", f.G[a], f.G_outer[a],
              f"{lab} {name(a)}")
        ck.eq("reverse sign", f.reverse(blade(a)),
              blade(a, (-1) ** (grade(a) * (grade(a) - 1) // 2)),
              f"{lab} {name(a)}")
        r, l = f.comp("right", blade(a)), f.comp("left", blade(a))
        ck.eq("u ^ right(u) = 1-bar", wedge(blade(a), r), one, f"{lab} {name(a)}")
        ck.eq("left(u) ^ u = 1-bar", wedge(l, blade(a)), one, f"{lab} {name(a)}")
        ck.eq("left o right = right o left = id",
              (f.comp("left", r), f.comp("right", l)), (blade(a), blade(a)),
              f"{lab} {name(a)}")
        k = grade(a)
        ck.eq("left = (-1)^(gr ag) right", l, scale((-1) ** (k * (n - k)), r),
              f"{lab} {name(a)}")
        for b in range(size):
            ck.eq("G symmetric", f.G[a].get(b, 0), f.G[b].get(a, 0),
                  f"{lab} {name(a)} {name(b)}")
            # left is the transpose of right (both signed permutations).
            ck.eq("left = transpose of right",
                  f.comp("left", blade(a)).get(b, 0),
                  f.comp("right", blade(b)).get(a, 0),
                  f"{lab} {name(a)} {name(b)}")
    for i in range(n):
        for b in range(size):
            ck.eq("F: e_i y = iota_i y + e_i ^ y (reference product)",
                  ref_mul(blade(1 << i), blade(b)), f.vec_mul(i, blade(b)),
                  f"{lab} e{i + 1} {name(b)}")
    for a in range(size):
        for b in range(size):
            ck.eq("Chevalley product = reference product",
                  f.blade_mul(a, b), ref_mul(blade(a), blade(b)),
                  f"{lab} {name(a)} {name(b)}")

    # Lemmas of the proof of (a).
    for a in range(size):
        for pname, mul in products:
            ck.eq("L1: X 1-bar = X _| 1-bar", mul(blade(a), one),
                  f.contract(blade(a), one), f"{lab} {name(a)} ({pname})")
        for i in range(n):
            gv = f.Gm(blade(1 << i))
            ck.eq("L2: iota_v right(Y) = right(Y ^ G v)",
                  f.iota(i, f.comp("right", blade(a))),
                  f.comp("right", wedge(blade(a), gv)),
                  f"{lab} v=e{i + 1} Y={name(a)}")
            ck.eq("L3: e_i ^ (B _| 1-bar) = (iota_i B) _| 1-bar",
                  wedge(blade(1 << i), f.contract(blade(a), one)),
                  f.contract(f.iota(i, blade(a)), one),
                  f"{lab} e{i + 1} B={name(a)}")
        ck.eq("e_A _| 1-bar = right(G reverse(e_A))",
              f.contract(blade(a), one),
              f.comp("right", f.Gm(f.reverse(blade(a)))), f"{lab} {name(a)}")

    # (a) reverse(x) 1-bar = right(G x).
    for a in range(size):
        want = f.comp("right", f.Gm(blade(a)))
        for pname, mul in products:
            ck.eq("(a) reverse(x) 1-bar = right(G x)",
                  mul(f.reverse(blade(a)), one), want,
                  f"{lab} {name(a)} ({pname})")
        # bulk = left(reverse(x) 1-bar)
        ck.eq("(a) G x = left(reverse(x) 1-bar)",
              f.comp("left", ref_mul(f.reverse(blade(a)), one)),
              f.Gm(blade(a)), f"{lab} {name(a)}")

    # (b) right(antiG x) = G right(x), and antiG = right G left as well.
    for a in range(size):
        x = blade(a)
        ck.eq("(b) right(antiG x) = G right(x)", f.comp("right", f.antiG(x)),
              f.Gm(f.comp("right", x)), f"{lab} {name(a)}")
        ck.eq("antiG = left G right = right G left", f.antiG(x),
              f.comp("right", f.Gm(f.comp("left", x))), f"{lab} {name(a)}")

    # (c) <e_A reverse(e_B)>_0 = det g[A, B] for equal grades, 0 otherwise.
    for a in range(size):
        for b in range(size):
            if grade(a) == grade(b):
                want = leibniz_det([[f.g[r][c] for c in bits(b)]
                                    for r in bits(a)])
            else:
                want = Fraction(0)
            for pname, mul in products:
                got = mul(blade(a), f.reverse(blade(b))).get(0, Fraction(0))
                ck.eq("(c) <e_A ~e_B>_0 = det g[A,B]", got, want,
                      f"{lab} {name(a)} {name(b)} ({pname})")
            ck.eq("(c) det g[A,B] = (G e_A)_B", want, f.G[a].get(b, 0),
                  f"{lab} {name(a)} {name(b)}")

    # (d) a^T antiG b = right(a)^T G right(b); left(c) = c 1-bar; antidot.
    ck.eq("(d) left(c) = c 1-bar", f.comp("left", blade(0, Fraction(3, 7))),
          scale(Fraction(3, 7), one), lab)
    for a in range(size):
        for b in range(size):
            ea, eb = blade(a), blade(b)
            lhs = bil(f, f.antiG, ea, eb)
            rhs = bil(f, f.Gm, f.comp("right", ea), f.comp("right", eb))
            ck.eq("(d) a^T antiG b = right(a)^T G right(b)", lhs, rhs,
                  f"{lab} {name(a)} {name(b)}")
            ra, rb = f.comp("right", ea), f.comp("right", eb)
            dot = clean({0: ref_mul(ra, f.reverse(rb)).get(0, Fraction(0))})
            ck.eq("(d) antidot(a,b) = left(dot(right a, right b))",
                  scale(lhs, one), f.comp("left", dot),
                  f"{lab} {name(a)} {name(b)}")

    # (e) G antiG = antiG G = det(g) I; double dual; a ^ b-star.
    for a in range(size):
        x = blade(a)
        k = grade(a)
        sgn = (-1) ** (k * (n - k))
        ck.eq("(e) G antiG = det(g) I", f.Gm(f.antiG(x)), scale(f.det, x),
              f"{lab} {name(a)}")
        ck.eq("(e) antiG G = det(g) I", f.antiG(f.Gm(x)), scale(f.det, x),
              f"{lab} {name(a)}")
        star = lambda y: f.comp("right", f.Gm(y))
        ck.eq("(e) u** = (-1)^(gr ag) det(g) u", star(star(x)),
              scale(sgn * f.det, x), f"{lab} {name(a)}")
    unequal_nonzero = 0
    for a in range(size):
        for b in range(size):
            ea, eb = blade(a), blade(b)
            star_b = f.comp("right", f.Gm(eb))
            if grade(a) != grade(b):
                if wedge(ea, star_b):
                    unequal_nonzero += 1
                continue
            k = grade(a)
            dot_ref = ref_mul(ea, f.reverse(eb)).get(0, Fraction(0))
            dot_mat = bil(f, f.Gm, ea, eb)
            ck.eq("(e) a . b: <a ~b>_0 = a^T G b", dot_ref, dot_mat,
                  f"{lab} {name(a)} {name(b)}")
            ck.eq("(e) a ^ b* = (a . b) 1-bar", wedge(ea, star_b),
                  scale(dot_mat, one), f"{lab} {name(a)} {name(b)}")
            ck.eq("(e) a ^ left(G b) = (-1)^(gr ag) (a . b) 1-bar",
                  wedge(ea, f.comp("left", f.Gm(eb))),
                  scale((-1) ** (k * (n - k)) * dot_mat, one),
                  f"{lab} {name(a)} {name(b)}")
    return unequal_nonzero


# --- golden.hpp ----------------------------------------------------------------

GOLDEN_FORMS = [("rigid_3", diag(1, 1, 1, 0)), ("conformal_2", conformal(2)),
                ("conformal_3", conformal(3)),
                ("degenerate_3", [[0, 1, 0], [1, 0, 0], [0, 0, 0]])]


def parse_golden(text):
    """The Gram matrices of gp_tables and the arrays of exomorphisms."""
    grams = {}
    for m in re.finditer(r"inline constexpr std::array<std::array<int, \d+>, "
                         r"\d+> (\w+)_gram = \{\{\n(.*?)\n\}\};", text, re.S):
        grams[m.group(1)] = [[int(v) for v in row.strip(" {},").split(",")]
                             for row in m.group(2).splitlines()]
    start = text.index("namespace exomorphisms {")
    end = text.index("} // namespace exomorphisms")
    body = text[start:end]
    images = {}
    width = int(re.search(r"struct blade_image \{.*?std::array<term, (\d+)> "
                          r"terms;", text, re.S).group(1))
    for m in re.finditer(r"std::array<blade_image, (\d+)>\s+(\w+) = \{\{\n"
                         r"(.*?)\n\}\};", body, re.S):
        rows = []
        for line in m.group(3).splitlines():
            r = re.fullmatch(r"\s*\{0b([01]+), (\d+), \{\{(.*)\}\}\},", line)
            terms = re.findall(r"\{(-?\d+), 0b([01]+)\}", r.group(3))
            rows.append((int(r.group(1), 2), int(r.group(2)),
                         [(int(c), int(mk, 2)) for c, mk in terms]))
        assert len(rows) == int(m.group(1))
        images[m.group(2)] = rows
    return grams, images, width


def check_golden(ck, golden_path, ref):
    with open(golden_path, encoding="utf-8") as fh:
        text = fh.read()
    grams, images, width = parse_golden(text)
    forms = GOLDEN_FORMS + [("random_1", ref.random_forms()[1])]
    ck.eq("golden: namespaces present", sorted(images),
          sorted(nm for nm, _ in forms), "exomorphisms arrays")
    longest = 0
    for nm, g in forms:
        ck.eq("golden: Gram matrix", grams.get(nm), g, nm)
        f = Form(nm, grams.get(nm, g))
        rows = images[nm]
        ck.eq("golden: one row per blade, by mask", [r[0] for r in rows],
              list(range(f.size)), nm)
        for a, size, terms in rows:
            ck.eq("golden: row width", len(terms), width, f"{nm} {name(a)}")
            live, pad = terms[:size], terms[size:]
            ck.eq("golden: padding is {0, 0}", pad, [(0, 0)] * len(pad),
                  f"{nm} {name(a)}")
            ck.eq("golden: no zero term", [t for t in live if t[0] == 0], [],
                  f"{nm} {name(a)}")
            ck.eq("golden: order by grade then index list",
                  [m for _, m in live],
                  sorted([m for _, m in live], key=lambda m: (grade(m), bits(m))),
                  f"{nm} {name(a)}")
            ck.eq("golden: G e_A = minors (Leibniz)",
                  {m: Fraction(c) for c, m in live}, f.G[a], f"{nm} {name(a)}")
            longest = max(longest, size)
    ck.eq("golden: width is the longest image", width, longest, "blade_image")


def main(argv):
    repo = argv[0] if argv else DEFAULT_REPO
    sys.path.insert(0, os.path.join(repo, "test", "reference"))
    import clifford_reference as ref

    ck = Checker()
    # Our forms against clifford_reference's constructors.
    ck.eq("forms", FORMS[0][1], ref.rigid(3), "rigid(3)")
    ck.eq("forms", FORMS[1][1], ref.conformal(3), "conformal(3)")
    ck.eq("forms", FORMS[2][1], ref.r_pqr(1, 1, 0), "R(1,1,0)")
    ck.eq("forms", FORMS[3][1], ref.r_pqr(2, 0, 2), "R(2,0,2)")
    ck.eq("forms", FORMS[6][1], ref.r_pqr(3, 0, 0), "R(3,0,0)")
    ck.eq("forms", conformal(2), ref.conformal(2), "conformal(2)")

    print(f"python {sys.version.split()[0]}; repo {repo}")
    print("form | n | det g | blade pairs | pairs of unequal grade "
          "with a ^ b* != 0")
    for label, g in FORMS:
        f = Form(label, g)
        unequal = check_form(ck, f, ref.Algebra(g))
        print(f"{label} | {f.n} | {f.det} | {f.size ** 2} | {unequal}")

    # The worked example: e12 . e12 = +1 in R(3,0,0), <e12 e12>_0 = -1.
    f = Form("R(3,0,0)", diag(1, 1, 1))
    e12 = blade(0b011)
    sq = ref.Algebra(f.g).mul(e12, e12).get(0, Fraction(0))
    dot = ref.Algebra(f.g).mul(e12, f.reverse(e12)).get(0, Fraction(0))
    ck.eq("example", (sq, dot, f.G[0b011]), (-1, 1, {0b011: 1}),
          "<e12 e12>_0, <e12 ~e12>_0, G e12 in R(3,0,0)")
    print(f"R(3,0,0): <e12 e12>_0 = {sq}, e12 . e12 = <e12 ~e12>_0 = {dot}")

    check_golden(ck, os.path.join(repo, "test", "reference", "golden.hpp"), ref)

    width = max(len(k) for k in ck.counts)
    for group, count in ck.counts.items():
        print(f"{group:<{width}}  {count:6d} checks")
    print(f"total {sum(ck.counts.values())} checks")
    if ck.failures:
        print("FAILED:")
        print("\n".join(ck.failures))
        return 1
    print("all checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
