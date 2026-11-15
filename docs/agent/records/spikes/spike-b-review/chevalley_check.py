"""Reviewer's own check of the integrality claim, written before running the
repository script. Route: the Chevalley action rho_i = eps_i + iota_i on the
exterior algebra, with wedge monomials as the basis. No diagonalization.

Blades are sorted index tuples. Multivectors are dicts blade -> Fraction.
"""
import itertools
import random
import sys
from fractions import Fraction

sys.setrecursionlimit(10000)


def wedge_vec(i, B):
    """e_i ^ e_B as (sign, blade) or None."""
    if i in B:
        return None
    pos = sum(1 for b in B if b < i)
    return (-1) ** pos, tuple(sorted(B + (i,)))


def contract_vec(i, B, g):
    """e_i _| e_B = sum_k (-1)^(k-1) g(i, b_k) e_{B \\ b_k}."""
    out = {}
    for k, b in enumerate(B):
        c = Fraction(g[i][b]) * (-1) ** k
        if c:
            rest = B[:k] + B[k + 1:]
            out[rest] = out.get(rest, 0) + c
    return out


def rho(i, x, g):
    """e_i x for x in the exterior algebra (Clifford product via Chevalley)."""
    out = {}
    for B, c in x.items():
        w = wedge_vec(i, B)
        if w:
            s, C = w
            out[C] = out.get(C, 0) + s * c
        for C, d in contract_vec(i, B, g).items():
            out[C] = out.get(C, 0) + c * d
    return {k: v for k, v in out.items() if v}


def blade_times(A, x, g):
    """e_A x by the recursion e_A x = e_a (e_A' x) - (e_a _| e_A') x."""
    if not A:
        return dict(x)
    a, Ap = A[0], A[1:]
    out = rho(a, blade_times(Ap, x, g), g)
    for C, c in contract_vec(a, Ap, g).items():
        for D, d in blade_times(C, x, g).items():
            out[D] = out.get(D, 0) - c * d
    return {k: v for k, v in out.items() if v}


def product(A, B, g):
    return blade_times(A, {B: Fraction(1)}, g)


def blades(n):
    for k in range(n + 1):
        yield from itertools.combinations(range(n), k)


def antisym(A, g):
    """(1/k!) sum_sigma sgn(sigma) e_sigma(1) ... e_sigma(k), via rho."""
    total = {}
    for perm in itertools.permutations(A):
        inv = sum(1 for p, q in itertools.combinations(perm, 2) if p > q)
        x = {(): Fraction(1)}
        for i in reversed(perm):
            x = rho(i, x, g)
        for C, c in x.items():
            total[C] = total.get(C, 0) + (-1) ** inv * c
    kf = 1
    for k in range(2, len(A) + 1):
        kf *= k
    return {k: v / kf for k, v in total.items() if v}


def check_algebra(n, g, label):
    """Defining relation, associativity on all triples, antisym = monomial,
    and the denominator bound 2^((|A|+|B|-|C|)/2) when 2g is integral."""
    bl = list(blades(n))
    tab = {(A, B): product(A, B, g) for A in bl for B in bl}
    # 1. defining relation
    for i in range(n):
        for j in range(n):
            s = {}
            for C, c in tab[((i,), (j,))].items():
                s[C] = s.get(C, 0) + c
            for C, c in tab[((j,), (i,))].items():
                s[C] = s.get(C, 0) + c
            s = {k: v for k, v in s.items() if v}
            exp = {(): 2 * Fraction(g[i][j])} if g[i][j] else {}
            assert s == exp, (label, "defining relation", i, j, s)
    # 2. associativity on every basis triple via the table
    def mul(x, y):
        out = {}
        for A, a in x.items():
            for B, b in y.items():
                for C, c in tab[(A, B)].items():
                    out[C] = out.get(C, 0) + a * b * c
        return {k: v for k, v in out.items() if v}
    for A, B, C in itertools.product(bl, repeat=3):
        l = mul(tab[(A, B)], {C: Fraction(1)})
        r = mul({A: Fraction(1)}, tab[(B, C)])
        assert l == r, (label, "assoc", A, B, C)
    # 3. wedge monomial = antisymmetrized product
    for A in bl:
        assert antisym(A, g) == {A: Fraction(1)}, (label, "antisym", A)
    # integrality / denominators
    worst = 1
    all_int = True
    bound_ok = True
    for (A, B), terms in tab.items():
        for C, c in terms.items():
            den = c.denominator
            worst = max(worst, den)
            if den != 1:
                all_int = False
            m = (len(A) + len(B) - len(C)) // 2
            if den > 2 ** m or den & (den - 1):
                bound_ok = False
    return tab, all_int, worst, bound_ok


def main():
    rng = random.Random(20261031)
    # integer forms: degenerate, indefinite, non-diagonal, n = 2..5
    for trial in range(40):
        n = 2 + trial % 4
        g = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(i, n):
                g[i][j] = g[j][i] = rng.randint(-4, 4)
        if trial % 5 == 0:  # force degenerate: last row = copy of first
            g[n - 1] = list(g[0]);
            for i in range(n):
                g[i][n - 1] = g[0][i]
            g[n - 1][n - 1] = g[0][0]
        tab, all_int, worst, _ = check_algebra(n, g, f"int{trial}")
        assert all_int, (g, worst)
    print("integer forms: 40 forms n=2..5, checks 1-3 pass, all coefficients integer")

    # half-integer forms: g = h/2 with h random integer
    nonint = 0
    for trial in range(24):
        n = 2 + trial % 4
        g = [[Fraction(0)] * n for _ in range(n)]
        for i in range(n):
            for j in range(i, n):
                g[i][j] = g[j][i] = Fraction(rng.randint(-4, 4), 2)
        tab, all_int, worst, bound_ok = check_algebra(n, g, f"half{trial}")
        assert bound_ok, (g, worst)
        if not all_int:
            nonint += 1
    print(f"half-integer forms: 24 forms, checks 1-3 pass, {nonint} forms with "
          "non-integer coefficients, every denominator a power of 2 within "
          "2^((|A|+|B|-|C|)/2)")

    # explicit counterexample and sharpness of the bound
    g = [[0, Fraction(1, 2)], [Fraction(1, 2), 0]]
    print("g = [[0,1/2],[1/2,0]]: e1 e2 =", product((0,), (1,), g))
    n = 6
    g = [[Fraction(0)] * n for _ in range(n)]
    for k in range(0, n, 2):
        g[k][k + 1] = g[k + 1][k] = Fraction(1, 2)
    print("three hyperbolic planes g_{2k-1,2k}=1/2: e135 e246 =",
          product((0, 2, 4), (1, 3, 5), g))


if __name__ == "__main__":
    main()
