"""Exhaustive check of what a change of basis preserves (research note 0026,
section A), on every basis blade and every pair of basis blades.

A change of basis from Cl(G) to Cl(G') is an invertible rational matrix P
whose row i writes e_i in the generators e'_j, with P G' P^T = G. phi is its
outermorphism, phi(e_A) = sum_B det P[A, B] e'_B, and psi is the
outermorphism of the contragredient Q = P^-T. For each change below the
script checks, on every blade (unary) or pair of blades (binary), each
statement of note 0026 section A:

  A1  phi is a homomorphism of the geometric product and the wedge, keeps
      grades, sends 1 to 1' and the antiscalar to det P times the antiscalar.
  A2  the reverse, antireverse, grade and antigrade involutions, and the
      dot are preserved with factor 1.
  A3  phi(right(x)) = det P right'(psi(x)), the same with left (any
      invertible P, no metric).
  A4  bulk'(phi(x)) = psi(bulk(x)) (an isometry).
  A5  the antiwedge with 1/det P; the bulk dual and the left bulk dual with
      det P; phi(weight(x)) = det(P)^2 weight'(psi(x)).
  A6  the geometric antiproduct, the weight dual, the left weight dual, and
      the antidot are preserved, each with factor 1/det P, exactly when
      P^-T G' P^-1 = G; the transform with 1/det(P)^2, compose with
      1/det P, project and antiproject with 1/det(P)^2, under the same
      condition.
  A7  the right and left complements are preserved (factor det P) exactly
      when P is orthogonal, P P^T = I; the bulk (factor 1) exactly when
      P P^T G = G; the weight (factor 1) exactly when G' P^T P = G'.

Where |det P| != 1 it also checks that the wrong power of det P fails, so
that the factors are told apart. The products come from
test/reference/clifford_reference.py (diagonalize, multiply by the
single-term rule, change back); the complements, the metric exomorphism
(Gram minors), and the duals are written here from their definitions in
CONVENTIONS 3 and 4 and in dual.hpp's comment.

Run from the repository root, with any CPython 3:
  python3 -I docs/agent/records/spikes/wu12-part-a/preservation_check.py test/reference
"""

import itertools
import random
import sys
from fractions import Fraction as F

sys.path.insert(0, sys.argv[1] if len(sys.argv) > 1 else "test/reference")
import clifford_reference as cr  # noqa: E402

failures = []


def check(ok, what):
    if not ok:
        failures.append(what)


def grade(m):
    return bin(m).count("1")


def mat(rows):
    return [[F(v) for v in row] for row in rows]


def identity(n):
    return [[F(int(i == j)) for j in range(n)] for i in range(n)]


class Algebra:
    def __init__(self, g):
        self.g = mat(g)
        self.n = len(g)
        self.top = (1 << self.n) - 1
        self.ref = cr.Algebra(self.g)
        self.exo = cr.change_of_basis(self.g, self.n)  # e_A -> det g[A, B]

    def gp(self, x, y):
        return cr.clean(self.ref.mul(x, y))

    def wedge(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                if not a & b:
                    out[a | b] = out.get(a | b, 0) + cr.reorder_sign(a, b) * ca * cb
        return cr.clean(out)

    def right(self, x):
        return cr.clean({self.top ^ a: c * cr.reorder_sign(a, self.top ^ a)
                         for a, c in x.items()})

    def left(self, x):
        return cr.clean({self.top ^ a: c * cr.reorder_sign(self.top ^ a, a)
                         for a, c in x.items()})

    def sign_map(self, x, f):
        return cr.clean({a: c * f(grade(a)) for a, c in x.items()})

    def reverse(self, x):
        return self.sign_map(x, lambda k: (-1) ** (k * (k - 1) // 2))

    def antireverse(self, x):
        n = self.n
        return self.sign_map(x, lambda k: (-1) ** ((n - k) * (n - k - 1) // 2))

    def involution(self, x):
        return self.sign_map(x, lambda k: (-1) ** k)

    def antiinvolution(self, x):
        n = self.n
        return self.sign_map(x, lambda k: (-1) ** (n - k))

    def bulk(self, x):
        return apply(self.exo, x)

    def weight(self, x):
        return self.left(self.bulk(self.right(x)))

    def bulk_dual(self, x):
        return self.right(self.bulk(x))

    def weight_dual(self, x):
        return self.right(self.weight(x))

    def left_bulk_dual(self, x):
        return self.left(self.bulk(x))

    def left_weight_dual(self, x):
        return self.left(self.weight(x))

    def antigp(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def antiwedge(self, x, y):
        return self.left(self.wedge(self.right(x), self.right(y)))

    def dot(self, x, y):
        gy = self.bulk(y)
        return cr.clean({0: sum(x.get(a, 0) * c for a, c in gy.items())})

    def antidot(self, x, y):
        wy = self.weight(y)
        return cr.clean({self.top: sum(x.get(a, 0) * c for a, c in wy.items())})

    def transform(self, x, v):
        """4.4 without the truncation to x's blades: V x V~ for even V and
        -V x' V~ for odd V, x' the antigrade involution, antiproducts."""
        parities = {(self.n - grade(a)) % 2 for a in v}
        assert len(parities) == 1
        if parities == {0}:
            return self.antigp(self.antigp(v, x), self.antireverse(v))
        inner = self.antigp(self.antigp(v, self.antiinvolution(x)),
                            self.antireverse(v))
        return scale(inner, -1)

    def compose(self, q2, q1):
        return self.antigp(q2, q1)

    def project(self, a, b):
        return self.antiwedge(b, self.wedge(a, self.weight_dual(b)))

    def antiproject(self, a, b):
        return self.wedge(b, self.antiwedge(a, self.weight_dual(b)))


def apply(images, x):
    out = {}
    for a, c in x.items():
        for b, k in images[a].items():
            out[b] = out.get(b, 0) + c * k
    return cr.clean(out)


def scale(x, f):
    return cr.clean({a: f * c for a, c in x.items()})


def blades(n):
    return [{a: F(1)} for a in range(1 << n)]


UNARY_1 = ["reverse", "antireverse", "involution", "antiinvolution"]
BINARY_1 = ["gp", "wedge", "dot"]


def run(name, g, g_target, p):
    n = len(g)
    s, t = Algebra(g), Algebra(g_target)
    pm = mat(p)
    check(cr.mat_mul(cr.mat_mul(pm, t.g), cr.transpose(pm)) == s.g,
          f"{name}: P G' P^T = G")
    d = cr.det(pm)
    q = cr.transpose(cr.inverse(pm))
    phi_images = cr.change_of_basis(pm, n)
    psi_images = cr.change_of_basis(q, n)

    def phi(x):
        return apply(phi_images, x)

    def psi(x):
        return apply(psi_images, x)

    contragredient = cr.mat_mul(cr.mat_mul(q, t.g), cr.transpose(q)) == s.g
    orthogonal = cr.mat_mul(pm, cr.transpose(pm)) == identity(n)
    bulk_condition = cr.mat_mul(cr.mat_mul(pm, cr.transpose(pm)), s.g) == s.g
    weight_condition = cr.mat_mul(cr.mat_mul(t.g, cr.transpose(pm)), pm) == t.g
    bs = blades(n)
    pairs = list(itertools.product(bs, repeat=2))

    def unary(op, factor, op_t=None, via=None):
        """phi(op(x)) == factor * op'(via(x)) on every blade."""
        via = via or phi
        op_t = op_t or op
        return all(phi(getattr(s, op)(x)) == scale(getattr(t, op_t)(via(x)), factor)
                   for x in bs)

    def binary(op, factor):
        return all(phi(getattr(s, op)(x, y)) ==
                   scale(getattr(t, op)(phi(x), phi(y)), factor)
                   for x, y in pairs)

    # A1
    for x in bs:
        (a,) = x
        check(all(grade(b) == grade(a) for b in phi(x)), f"{name}: grade of {a}")
    check(phi({0: F(1)}) == {0: F(1)}, f"{name}: phi(1)")
    check(phi({s.top: F(1)}) == {t.top: d}, f"{name}: phi(antiscalar) = det P")
    # A2 and the products
    for op in UNARY_1:
        check(unary(op, 1), f"{name}: {op} factor 1")
    for op in BINARY_1:
        check(binary(op, 1), f"{name}: {op} factor 1")
    # A3, any invertible P
    for x in bs:
        check(phi(s.right(x)) == scale(t.right(psi(x)), d), f"{name}: A3 right")
        check(phi(s.left(x)) == scale(t.left(psi(x)), d), f"{name}: A3 left")
        check(psi(s.right(x)) == scale(t.right(phi(x)), 1 / d),
              f"{name}: A3 right with roles swapped")
    # A4
    check(all(t.bulk(phi(x)) == psi(s.bulk(x)) for x in bs), f"{name}: A4")
    # A5
    check(binary("antiwedge", 1 / d), f"{name}: antiwedge factor 1/det P")
    check(unary("bulk_dual", d), f"{name}: bulk dual factor det P")
    check(unary("left_bulk_dual", d), f"{name}: left bulk dual factor det P")
    check(all(phi(s.weight(x)) == scale(t.weight(psi(x)), d * d) for x in bs),
          f"{name}: A5 weight through psi")
    if abs(d) != 1:
        check(not binary("antiwedge", d), f"{name}: antiwedge with det P fails")
        check(not unary("bulk_dual", 1 / d), f"{name}: bulk dual with 1/det P fails")
    # A6: each of these holds with 1/det P exactly when the contragredient is
    # an isometry
    for op in ["weight_dual", "left_weight_dual"]:
        check(unary(op, 1 / d) == contragredient, f"{name}: {op} iff condition")
    for op in ["antigp", "antidot"]:
        check(binary(op, 1 / d) == contragredient, f"{name}: {op} iff condition")
    if contragredient:
        check(binary("compose", 1 / d), f"{name}: compose 1/det P")
        check(binary("project", 1 / (d * d)), f"{name}: project 1/det P^2")
        check(binary("antiproject", 1 / (d * d)), f"{name}: antiproject 1/det P^2")
        rng = random.Random(20261109)
        for parity in (0, 1):
            masks = [a for a in range(1 << n) if (n - grade(a)) % 2 == parity]
            for _ in range(3):
                v = cr.clean({a: F(rng.randint(-3, 3)) for a in masks}) or {masks[0]: F(1)}
                check(all(phi(s.transform(x, v)) ==
                          scale(t.transform(phi(x), phi(v)), 1 / (d * d))
                          for x in bs), f"{name}: transform 1/det P^2")
        if abs(d) != 1:
            check(not binary("antigp", d), f"{name}: antigp with det P fails")
            check(not unary("weight_dual", d), f"{name}: weight dual with det P fails")
    # A7
    check(unary("right", d) == orthogonal, f"{name}: right iff orthogonal")
    check(unary("left", d) == orthogonal, f"{name}: left iff orthogonal")
    check(unary("bulk", 1) == bulk_condition, f"{name}: bulk iff P P^T G = G")
    check(unary("weight", 1) == weight_condition, f"{name}: weight iff G' P^T P = G'")
    print(f"{name}: n = {n}, det P = {d}, P^-T G' P^-1 = G: {contragredient}, "
          f"orthogonal: {orthogonal}, P P^T G = G: {bulk_condition}, "
          f"G' P^T P = G': {weight_condition}")


def squarefree(m):
    """The squarefree part of a nonzero integer, with its sign."""
    sign, m = (-1 if m < 0 else 1), abs(m)
    out, k = 1, 2
    while k * k <= m:
        while m % (k * k) == 0:
            m //= k * k
        if m % k == 0:
            out *= k
            m //= k
        k += 1
    return sign * out * m


def ldlt_change(g):
    """P and the integer diagonal D for the reference's LDL^T T G T^T = diag(d),
    each square d = a/b scaled to the squarefree part s of a b by the row
    factor c = b/m, where a b = s m^2, as plan 0030's R5 asks."""
    t, d = cr.diagonalize(mat(g))
    n = len(g)
    rows, squares = [], []
    for i in range(n):
        if d[i] == 0:
            c, sq = F(1), 0
        else:
            ab = d[i].numerator * d[i].denominator
            s = squarefree(ab)
            m = int(round((ab // s) ** 0.5))
            assert m * m * s == ab
            c, sq = F(d[i].denominator, m), s
            assert c * c * d[i] == sq
        rows.append([c * v for v in t[i]])
        squares.append(sq)
    p = cr.inverse(rows)  # row i of P writes e_i in the scaled f_j
    diagonal = [[squares[i] if i == j else 0 for j in range(n)] for i in range(n)]
    return p, diagonal


def r(p_, q_, z_):
    return cr.r_pqr(p_, q_, z_)


def main():
    half = F(1, 2)
    for dd in (1, 2, 3):
        n = dd + 2
        p = [[F(int(i == j)) for j in range(n)] for i in range(dd)]
        p.append([F(0)] * dd + [-half, half])
        p.append([F(0)] * dd + [F(1), F(1)])
        run(f"conformal({dd}) to R({dd + 1},1,0)", cr.conformal(dd), r(dd + 1, 1, 0), p)
    witt1 = [[0, 1], [1, 0]]
    run("Witt, one mode, to R(1,1,0)", witt1, r(1, 1, 0), [[half, half], [1, -1]])
    witt2 = [[0, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 0, 1, 0]]
    run("Witt, two modes, to R(2,2,0)", witt2, r(2, 2, 0),
        [[half, 0, half, 0], [1, 0, -1, 0], [0, half, 0, half], [0, 1, 0, -1]])
    run("R(2,0,0) rotated by (3/5, 4/5)", r(2, 0, 0), r(2, 0, 0),
        [[F(3, 5), F(4, 5)], [F(-4, 5), F(3, 5)]])
    run("R(1,1,0) boosted by (5/3, 4/3)", r(1, 1, 0), r(1, 1, 0),
        [[F(5, 3), F(4, 3)], [F(4, 3), F(5, 3)]])
    run("R(1,0,1), the null shear e1 to e1 + e2", r(1, 0, 1), r(1, 0, 1), [[1, 1], [0, 1]])
    run("rigid(3) with its null generator first, to rigid(3)",
        [[0, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]], cr.rigid(3),
        [[0, 0, 0, 1], [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0]])
    run("rigid(3), e4 to 2 e4'", cr.rigid(3), cr.rigid(3),
        [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 2]])
    run("rigid(3), e1 to e1' + e4', e4 to e4'/3", cr.rigid(3), cr.rigid(3),
        [[1, 0, 0, 1], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, F(1, 3)]])
    # Not an isometry (e4 would square to 1), so every check that needs one
    # must fail: a control of the checks themselves.
    before = len(failures)
    run("control: rigid(3), e4 to e1' + e4'/3", cr.rigid(3), cr.rigid(3),
        [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [1, 0, 0, F(1, 3)]])
    caught = failures[before:]
    del failures[before:]
    check(any("P G' P^T = G" in f for f in caught) and any("gp factor 1" in f for f in caught)
          and any("A4" in f for f in caught), "control: a non-isometry is caught")
    run("R(0,0,1), e1 to 3 e1'", r(0, 0, 1), r(0, 0, 1), [[3]])
    run("R(2,0,1), e3 to e3'/2, then rotated", r(2, 0, 1), r(2, 0, 1),
        [[F(3, 5), F(4, 5), 0], [F(-4, 5), F(3, 5), 0], [0, 0, half]])
    run("diag(4, 1) to R(2,0,0), e1 to 2 e1'", [[4, 0], [0, 1]], r(2, 0, 0), [[2, 0], [0, 1]])
    run("degenerate form to R(1,1,1)", [[0, 1, 0], [1, 0, 0], [0, 0, 0]], r(1, 1, 1),
        [[half, half, 0], [1, -1, 0], [0, 0, 1]])
    # A dense nondegenerate source made from R(2,1,0) by an integer M, det M = 3.
    m = [[1, 2, 0], [0, 1, 1], [1, 0, 1]]
    g = [[int(v) for v in row] for row in cr.mat_mul(cr.mat_mul(mat(m), mat(r(2, 1, 0))),
                                                     cr.transpose(mat(m)))]
    run("M R(2,1,0) M^T, det M = 3, to R(2,1,0)", g, r(2, 1, 0), m)
    forms = [("random %d" % k, f) for k, f in enumerate(cr.random_forms())]
    forms += [("degenerate", [[0, 1, 0], [1, 0, 0], [0, 0, 0]]),
              ("radical off the axes", [[1, 1, 0], [1, 0, 1], [0, 1, -1]]),
              ("all ones", [[1, 1, 1], [1, 1, 1], [1, 1, 1]])]
    for label, g in forms:
        p, diagonal = ldlt_change(g)
        run(f"{label} to its scaled LDL^T diagonal {[diagonal[i][i] for i in range(len(g))]}",
            g, diagonal, p)
    if failures:
        print(f"FAILED: {len(failures)}")
        for f_ in failures[:40]:
            print("  " + f_)
        return 1
    print("every statement holds on every blade and pair of blades")
    return 0


if __name__ == "__main__":
    sys.exit(main())
