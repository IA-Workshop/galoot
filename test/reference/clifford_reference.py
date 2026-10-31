"""Exact geometric-product tables for any symmetric integer Gram matrix.

This is an independent reference (VVUQ V1, kind 3) for the library's product
generator. It takes a route the library does not: it diagonalizes the form
over fractions.Fraction (a symmetric LDL^T with pivoting), multiplies in the
orthogonal basis by the single-term sign rule, and changes back to the stored
basis. The library instead works in the stored basis with integer arithmetic.

Blades are bitmasks with bit i-1 for e_i. In a metric that is not diagonal the
blade of a mask is the wedge monomial e_a1 ^ ... ^ e_ak (decision 0003,
CONVENTIONS 1.1), so e4 e5 = -1 + e45 in conformal(3).

Usage, with the interpreter isolated as for every reference script:

  python3 -I test/reference/clifford_reference.py
      Validate and survey the Spike B signature set; exit nonzero on failure.
  python3 -I test/reference/clifford_reference.py table SIGNATURE
      Print the full product table, one blade pair per line.

SIGNATURE is one of: "conformal D", "R P Q R0", "rigid D", "random K" (the
K-th seeded random form of the survey, K = 0 to 9), or "gram ROWS" with rows
separated by ";" and entries by "," (for example "gram 0,-1;-1,0").
"""

import itertools
import random
import sys
from fractions import Fraction

SEED = 20261031
RANDOM_FORMS = 10
RANDOM_MAX_N = 5
RANDOM_ENTRY = 3  # Gram entries are drawn from [-3, 3].


# --- Signatures --------------------------------------------------------------

def diagonal_gram(squares):
    n = len(squares)
    return [[squares[i] if i == j else 0 for j in range(n)] for i in range(n)]


def r_pqr(p, q, r):
    """CONVENTIONS 1.2: p squares of +1, then q of -1, then r of 0."""
    return diagonal_gram([1] * p + [-1] * q + [0] * r)


def rigid(d):
    """CONVENTIONS 1.3: rigid(d) is R(d, 0, 1)."""
    return r_pqr(d, 0, 1)


def conformal(d):
    """CONVENTIONS 1.4: e(d+1) and e(d+2) null, e(d+1) . e(d+2) = -1."""
    g = diagonal_gram([1] * d + [0, 0])
    g[d][d + 1] = g[d + 1][d] = -1
    return g


def random_forms(seed=SEED, count=RANDOM_FORMS):
    """Seeded symmetric integer forms; n cycles through 2 to RANDOM_MAX_N.

    Cycling n, rather than drawing it, makes every size appear in a short
    list; the entries are drawn.
    """
    rng = random.Random(seed)
    forms = []
    for k in range(count):
        n = 2 + k % (RANDOM_MAX_N - 1)
        g = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(i, n):
                g[i][j] = g[j][i] = rng.randint(-RANDOM_ENTRY, RANDOM_ENTRY)
        forms.append(g)
    return forms


def parse_signature(words):
    kind, args = words[0], words[1:]
    if kind == "conformal":
        return f"conformal({args[0]})", conformal(int(args[0]))
    if kind == "R":
        p, q, r = (int(a) for a in args)
        return f"R({p},{q},{r})", r_pqr(p, q, r)
    if kind == "rigid":
        return f"rigid({args[0]})", rigid(int(args[0]))
    if kind == "random":
        k = int(args[0])
        return f"random[{k}] (seed {SEED})", random_forms()[k]
    if kind == "gram":
        rows = " ".join(args).split(";")
        g = [[int(x) for x in row.split(",")] for row in rows]
        return "gram", g
    raise SystemExit(f"unknown signature kind {kind!r}")


# --- Linear algebra over Fraction -------------------------------------------

def mat_mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(len(b)))
             for j in range(len(b[0]))] for i in range(len(a))]


def transpose(a):
    return [list(row) for row in zip(*a)]


def inverse(a):
    n = len(a)
    m = [[Fraction(x) for x in row] + [Fraction(int(i == j)) for j in range(n)]
         for i, row in enumerate(a)]
    for k in range(n):
        p = next(i for i in range(k, n) if m[i][k] != 0)
        m[k], m[p] = m[p], m[k]
        pivot = m[k][k]
        m[k] = [x / pivot for x in m[k]]
        for i in range(n):
            if i != k and m[i][k] != 0:
                f = m[i][k]
                m[i] = [x - f * y for x, y in zip(m[i], m[k])]
    return [row[n:] for row in m]


def det(a):
    n = len(a)
    if n == 0:
        return Fraction(1)
    m = [[Fraction(x) for x in row] for row in a]
    result = Fraction(1)
    for k in range(n):
        p = next((i for i in range(k, n) if m[i][k] != 0), None)
        if p is None:
            return Fraction(0)
        if p != k:
            m[k], m[p] = m[p], m[k]
            result = -result
        result *= m[k][k]
        for i in range(k + 1, n):
            f = m[i][k] / m[k][k]
            m[i] = [x - f * y for x, y in zip(m[i], m[k])]
    return result


def diagonalize(g):
    """Return (T, d) with T g T^T = diag(d), by symmetric elimination.

    Row i of T holds the coordinates of the orthogonal vector f_i in the
    stored basis e. A zero pivot is replaced first by a later nonzero diagonal
    entry (swap) and otherwise by f_k + f_j for some j with g(f_k, f_j) != 0,
    whose square 2 g(f_k, f_j) is nonzero. A row with nothing to pair with is
    in the radical and keeps square 0.
    """
    n = len(g)
    a = [[Fraction(x) for x in row] for row in g]
    t = [[Fraction(int(i == j)) for j in range(n)] for i in range(n)]

    def swap(i, j):
        a[i], a[j] = a[j], a[i]
        for row in a:
            row[i], row[j] = row[j], row[i]
        t[i], t[j] = t[j], t[i]

    def add_to(k, j, f):
        """f_k += f * f_j, as a congruence on a."""
        a[k] = [x + f * y for x, y in zip(a[k], a[j])]
        for row in a:
            row[k] += f * row[j]
        t[k] = [x + f * y for x, y in zip(t[k], t[j])]

    for k in range(n):
        if a[k][k] == 0:
            j = next((j for j in range(k + 1, n) if a[j][j] != 0), None)
            if j is not None:
                swap(k, j)
            else:
                j = next((j for j in range(k + 1, n) if a[k][j] != 0), None)
                if j is None:
                    continue
                add_to(k, j, Fraction(1))
        for i in range(k + 1, n):
            if a[i][k] != 0:
                add_to(i, k, -a[i][k] / a[k][k])

    check_d = mat_mul(mat_mul(t, [[Fraction(x) for x in r] for r in g]),
                      transpose(t))
    for i in range(n):
        for j in range(n):
            assert check_d[i][j] == (a[i][i] if i == j else 0), "not diagonal"
    return t, [a[i][i] for i in range(n)]


# --- Blades and multivectors -------------------------------------------------

def popcount(x):
    return bin(x).count("1")


def indices(mask):
    return [i for i in range(mask.bit_length()) if mask >> i & 1]


def reorder_sign(a, b):
    """Sign of f_A f_B relative to the sorted blade f_(A xor B)."""
    swaps = 0
    a >>= 1
    while a:
        swaps += popcount(a & b)
        a >>= 1
    return -1 if swaps & 1 else 1


def clean(x):
    return {m: c for m, c in x.items() if c != 0}


def blade_name(mask):
    return "1" if mask == 0 else "e" + "".join(str(i + 1) for i in indices(mask))


def change_of_basis(m, n):
    """Map of each blade mask A to sum_S det(m[A, S]) S, the outermorphism.

    If vector i of one basis is sum_j m[i][j] times vector j of another, the
    wedge monomial on A is the sum over |S| = |A| of the minor on rows A and
    columns S times the wedge monomial on S. Wedges need no metric.
    """
    out = {}
    for a in range(1 << n):
        rows = indices(a)
        terms = {}
        for cols in itertools.combinations(range(n), len(rows)):
            minor = det([[m[r][c] for c in cols] for r in rows])
            if minor != 0:
                terms[sum(1 << c for c in cols)] = minor
        out[a] = terms
    return out


class Algebra:
    """The Clifford algebra of a symmetric integer Gram matrix g."""

    def __init__(self, g):
        n = len(g)
        assert all(g[i][j] == g[j][i] for i in range(n) for j in range(n)), \
            "Gram matrix is not symmetric"
        self.n = n
        self.g = g
        self.t, self.d = diagonalize(g)
        self.to_f = change_of_basis(inverse(self.t), n)  # e_A in f blades
        self.to_e = change_of_basis(self.t, n)           # f_S in e blades

    def f_product(self, s, t):
        """Single-term rule in the orthogonal basis: f_S f_T = c f_(S^T)."""
        c = Fraction(reorder_sign(s, t))
        for i in indices(s & t):
            c *= self.d[i]
        return c

    def mul(self, x, y):
        """Product of two multivectors given on wedge-monomial blades e_A."""
        fx, fy = self.to_f_mv(x), self.to_f_mv(y)
        fz = {}
        for s, cs in fx.items():
            for t, ct in fy.items():
                c = cs * ct * self.f_product(s, t)
                if c != 0:
                    fz[s ^ t] = fz.get(s ^ t, 0) + c
        return self.to_e_mv(clean(fz))

    def to_f_mv(self, x):
        out = {}
        for a, c in x.items():
            for s, m in self.to_f[a].items():
                out[s] = out.get(s, 0) + c * m
        return clean(out)

    def to_e_mv(self, x):
        out = {}
        for s, c in x.items():
            for a, m in self.to_e[s].items():
                out[a] = out.get(a, 0) + c * m
        return clean(out)

    def table(self):
        """Every blade pair (A, B) mapped to the terms of e_A e_B."""
        size = 1 << self.n
        return {(a, b): self.mul({a: Fraction(1)}, {b: Fraction(1)})
                for a in range(size) for b in range(size)}


# --- Validation --------------------------------------------------------------

def mv_mul_by_table(table, x, y):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            for m, c in table[(a, b)].items():
                out[m] = out.get(m, 0) + ca * cb * c
    return clean(out)


def validate(alg, table):
    """Return the list of failed checks; empty means every check passed.

    1. Defining relations: e_i e_j + e_j e_i = 2 g_ij for all i, j.
    2. Associativity on every triple of basis blades.
    3. Wedge-monomial basis: each blade e_A equals the antisymmetrized product
       (1/k!) sum_sigma sgn(sigma) e_a(sigma 1) ... e_a(sigma k). Checks 1 and
       2 hold in any basis, so this one pins the basis that 0003 names.
    """
    failures = []
    n, size = alg.n, 1 << alg.n

    for i in range(n):
        for j in range(n):
            s = clean({m: table[(1 << i, 1 << j)].get(m, 0) +
                       table[(1 << j, 1 << i)].get(m, 0)
                       for m in range(size)})
            if s != clean({0: Fraction(2 * alg.g[i][j])}):
                failures.append(f"anticommutator e{i + 1} e{j + 1}")

    for a, b, c in itertools.product(range(size), repeat=3):
        left = mv_mul_by_table(table, table[(a, b)], {c: Fraction(1)})
        right = mv_mul_by_table(table, {a: Fraction(1)}, table[(b, c)])
        if left != right:
            failures.append(
                f"associativity {blade_name(a)} {blade_name(b)} {blade_name(c)}")

    for a in range(size):
        idx = indices(a)
        total = {}
        for perm in itertools.permutations(idx):
            inversions = sum(1 for p, q in itertools.combinations(perm, 2)
                             if p > q)
            prod = {0: Fraction(1)}
            for i in perm:
                prod = mv_mul_by_table(table, prod, {1 << i: Fraction(1)})
            for m, c in prod.items():
                total[m] = total.get(m, 0) + (-c if inversions & 1 else c)
        k_fact = 1
        for k in range(2, len(idx) + 1):
            k_fact *= k
        if clean({m: c / k_fact for m, c in total.items()}) != {a: 1}:
            failures.append(f"wedge monomial {blade_name(a)}")
    return failures


def is_dyadic(q):
    den = q.denominator
    return den & (den - 1) == 0


def survey(name, g):
    alg = Algebra(g)
    table = alg.table()
    coeffs = [c for terms in table.values() for c in terms.values()]
    multi = sum(1 for terms in table.values() if len(terms) > 1)
    return {
        "name": name,
        "n": alg.n,
        "det": det(g),
        "gram": g,
        "d": alg.d,
        "pairs": len(table),
        "multi": multi,
        "max_terms": max(len(t) for t in table.values()),
        "nonzero_terms": len(coeffs),
        "integer": all(c.denominator == 1 for c in coeffs),
        "dyadic": all(is_dyadic(c) for c in coeffs),
        "max_abs": max((abs(c) for c in coeffs), default=0),
        "d_dyadic": all(is_dyadic(x) for x in alg.d),
        "failures": validate(alg, table),
    }


def fmt(q):
    return str(q.numerator) if q.denominator == 1 else str(q)


def print_table(name, g):
    alg = Algebra(g)
    print(f"# {name}, n = {alg.n}, Gram {g}")
    print(f"# orthogonal squares d = [{', '.join(fmt(x) for x in alg.d)}]")
    print("# e_A e_B -> coefficient blade, ... (blades are wedge monomials)")
    for (a, b), terms in alg.table().items():
        body = ", ".join(f"{fmt(c)} {blade_name(m)}"
                         for m, c in sorted(terms.items(),
                                            key=lambda kv: (popcount(kv[0]),
                                                            indices(kv[0]))))
        print(f"{blade_name(a)} {blade_name(b)} -> {body or '0'}")


def main(argv):
    if argv and argv[0] == "table":
        print_table(*parse_signature(argv[1:]))
        return 0

    signatures = [("conformal(3)", conformal(3)), ("R(1,3,0)", r_pqr(1, 3, 0)),
                  ("rigid(3)", rigid(3)),
                  # Degenerate and not diagonal (plan 0001, section 5).
                  ("degenerate", [[0, 1, 0], [1, 0, 0], [0, 0, 0]])]
    signatures += [(f"random[{k}]", g) for k, g in enumerate(random_forms())]

    ok = True
    print(f"seed {SEED}; random forms: {RANDOM_FORMS}, 2 <= n <= "
          f"{RANDOM_MAX_N}, entries in [-{RANDOM_ENTRY}, {RANDOM_ENTRY}]")
    print("name | n | Gram | det | orthogonal squares | "
          "multi-term pairs / pairs | max terms | integer | dyadic | max |c| | "
          "checks")
    for name, g in signatures:
        r = survey(name, g)
        ok = ok and not r["failures"]
        gram = ";".join(",".join(str(x) for x in row) for row in r["gram"])
        d = ",".join(fmt(x) for x in r["d"])
        status = "pass" if not r["failures"] else \
            f"FAIL: {', '.join(r['failures'][:5])}"
        print(f"{r['name']} | {r['n']} | {gram} | {fmt(r['det'])} | {d} | "
              f"{r['multi']} / {r['pairs']} | {r['max_terms']} | "
              f"{r['integer']} | {r['dyadic']} | {r['max_abs']} | {status}")
    print("all reference checks passed" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
