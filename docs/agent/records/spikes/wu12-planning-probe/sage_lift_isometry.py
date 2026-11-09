"""Planning probe: Sage lift_isometry on the CONVENTIONS 1.4 change of basis.

Run with the math-tools Sage environment (passagemath-modules 10.8.13):
  /home/brianm/projects/utils/math-core/math-tools/toolset/env/py-sage/.venv/bin/python -I \
    docs/agent/records/spikes/wu12-planning-probe/sage_lift_isometry.py

conformal(d) has e(d+1) = origin, e(d+2) = infinity, e(d+1).e(d+2) = -1; its
diagonal counterpart R(d+1,1,0). P (rows: conformal generators written in the
diagonal ones): origin = (1/2)(e(d+2)' - e(d+1)'), infinity = e(d+2)' + e(d+1)'.
Sage's lift_isometry(m) sends e_i to sum_j m[j, i] e'_j, so m = P^T, and its
codomain Gram matrix is m^{-T} G m^{-1}.
"""
from itertools import combinations
from sage.all__sagemath_modules import QQ, CliffordAlgebra, QuadraticForm, matrix, vector

def wedge_basis(Cl, n):
    e = Cl.gens()
    order = [S for k in range(n + 1) for S in combinations(range(n), k)]
    X = {(): Cl.one()}
    for S in order[1:]:
        i, rest = S[0], S[1:]
        X[S] = (e[i] * X[rest] + (-1) ** len(rest) * X[rest] * e[i]) / 2
    position = {S: r for r, S in enumerate(order)}
    def coords(x):
        out = [QQ(0)] * len(order)
        for key, c in x.monomial_coefficients().items():
            out[position[tuple(key)]] = QQ(c)
        return vector(QQ, out)
    T = matrix(QQ, [list(coords(X[S])) for S in order]).transpose()
    return order, X, coords, T

for d in (1, 2, 3):
    n = d + 2
    G = matrix(QQ, n, n, 0)
    for i in range(d): G[i, i] = 1
    G[d, d + 1] = G[d + 1, d] = -1
    P = matrix(QQ, n, n, 0)
    for i in range(d): P[i, i] = 1
    P[d, d] = QQ(-1) / 2; P[d, d + 1] = QQ(1) / 2
    P[d + 1, d] = 1; P[d + 1, d + 1] = 1
    m = P.transpose()
    Cl = CliffordAlgebra(QuadraticForm(QQ, 2 * G))
    phi = Cl.lift_isometry(m, "a")
    Gt = phi.codomain()._quadratic_form.Gram_matrix()
    want_Gt = matrix(QQ, n, n, 0)
    for i in range(d + 1): want_Gt[i, i] = 1
    want_Gt[d + 1, d + 1] = -1
    order, X, coords, T = wedge_basis(Cl, n)
    _, Xt, coords_t, Tt = wedge_basis(phi.codomain(), n)
    bad = 0
    for A in order:
        c = Tt.solve_right(coords_t(phi(X[A])))
        for bi, B in enumerate(order):
            want = (P.matrix_from_rows_and_columns(list(A), list(B)).det() if A else 1) if len(A) == len(B) else 0
            bad += c[bi] != want
    # homomorphism on wedge monomials, products computed in each algebra by Sage
    hom_bad = sum(phi(X[A] * X[B]) != phi(X[A]) * phi(X[B]) for A in order for B in order)
    print(f"conformal({d}): codomain Gram is R({d+1},1,0): {Gt == want_Gt}; "
          f"wedge-coordinate images vs minors of P: {bad} mismatches of {len(order)**2}; "
          f"homomorphism failures {hom_bad} of {len(order)**2}; det P = {P.det()}")
