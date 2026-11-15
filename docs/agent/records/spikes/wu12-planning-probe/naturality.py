"""Planning probe: which operations commute with an exact change of basis.

A change of basis from Cl(G) to Cl(G') is an invertible rational matrix P
whose row i writes the i-th source generator in the target's generators,
with P G' P^T = G. It acts on wedge monomials by the minors of P
(test/reference/clifford_reference.py, change_of_basis). For each operation
op, the probe looks for a factor f among +-1, det P^(+-1), det P^(+-2) with
phi(op(x, y)) = f op'(phi(x), phi(y)) on 20 seeded random multivectors with
integer coefficients in [-3, 3]. Products come from the reference's own route
(LDL^T, the single-term rule, and minors), so nothing here uses the library.

This is sampled evidence for a hypothesis (V9), not a derivation.

Run from the repository root:
  python3 -I docs/agent/records/spikes/wu12-planning-probe/naturality.py test/reference
"""

import random
import sys
from fractions import Fraction as F

sys.path.insert(0, sys.argv[1])
import clifford_reference as cr  # noqa: E402


def grade(mask):
    return bin(mask).count("1")


class Algebra:
    def __init__(self, g):
        self.g = [[F(v) for v in row] for row in g]
        self.n = len(g)
        self.reference = cr.Algebra(self.g)
        self.top = (1 << self.n) - 1

    def gp(self, x, y):
        return cr.clean(self.reference.mul(x, y))

    def wedge(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                if not a & b:
                    out[a | b] = out.get(a | b, 0) + cr.reorder_sign(a, b) * ca * cb
        return cr.clean(out)

    # u ^ right(u) = 1 and left(u) ^ u = 1 for each basis blade u.
    def right(self, x):
        return cr.clean({self.top ^ a: c * cr.reorder_sign(a, self.top ^ a) for a, c in x.items()})

    def left(self, x):
        return cr.clean({self.top ^ a: c * cr.reorder_sign(self.top ^ a, a) for a, c in x.items()})

    def reverse(self, x):
        return {a: c * (-1) ** (grade(a) * (grade(a) - 1) // 2) for a, c in x.items()}

    def antireverse(self, x):
        m = self.n
        return {a: c * (-1) ** ((m - grade(a)) * (m - grade(a) - 1) // 2) for a, c in x.items()}

    def grade_involution(self, x):
        return {a: c * (-1) ** grade(a) for a, c in x.items()}

    def antigp(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def antiwedge(self, x, y):
        return self.left(self.wedge(self.right(x), self.right(y)))

    # The bulk G x is left(reverse(x) 1), as the library computes it.
    def bulk(self, x):
        return self.left(self.gp(self.reverse(x), {self.top: F(1)}))

    def weight(self, x):
        return self.left(self.bulk(self.right(x)))

    def bulk_dual(self, x):
        return self.right(self.bulk(x))

    def weight_dual(self, x):
        return self.right(self.weight(x))

    def dot(self, x, y):
        gy = self.bulk(y)
        return cr.clean({0: sum(x.get(a, 0) * c for a, c in gy.items())})

    def antidot(self, x, y):
        wy = self.weight(y)
        return cr.clean({self.top: sum(x.get(a, 0) * c for a, c in wy.items())})


UNARY = ["reverse", "antireverse", "grade_involution", "right", "left", "bulk", "weight", "bulk_dual",
         "weight_dual"]
BINARY = ["gp", "wedge", "antigp", "antiwedge", "dot", "antidot"]


def probe(name, g, g_target, p, trials=20, seed=7):
    n = len(g)
    source, target = Algebra(g), Algebra(g_target)
    pm = [[F(v) for v in row] for row in p]
    isometry = cr.mat_mul(cr.mat_mul(pm, target.g), cr.transpose(pm))
    assert isometry == source.g, f"{name}: P G' P^T is not G"
    d = cr.det(pm)
    images = cr.change_of_basis(pm, n)

    def phi(x):
        out = {}
        for a, c in x.items():
            for b, k in images[a].items():
                out[b] = out.get(b, 0) + c * k
        return cr.clean(out)

    rng = random.Random(seed)

    def draw():
        return cr.clean({a: F(rng.randint(-3, 3)) for a in range(1 << n)})

    # Whether the contragredient P^-T is also an isometry: P^-T G' P^-1 = G.
    contragredient = cr.transpose(cr.inverse(pm))
    dual_isometry = cr.mat_mul(cr.mat_mul(contragredient, target.g), cr.transpose(contragredient)) == source.g
    print(f"{name}: det P = {d}; P^-T G' P^-1 = G: {dual_isometry}")
    for op in UNARY + BINARY:
        found = None
        for factor in [F(1), F(-1), d, 1 / d, d * d, 1 / (d * d), -d, -1 / d]:
            agree = True
            for _ in range(trials):
                x, y = draw(), draw()
                if op in UNARY:
                    lhs, rhs = phi(getattr(source, op)(x)), getattr(target, op)(phi(x))
                else:
                    lhs, rhs = phi(getattr(source, op)(x, y)), getattr(target, op)(phi(x), phi(y))
                if lhs != cr.clean({k: factor * v for k, v in rhs.items()}):
                    agree = False
                    break
            if agree:
                found = factor
                break
        verdict = f"factor {found}" if found is not None else "not preserved"
        print(f"  {op:16s} {verdict}")


def main():
    half = F(1, 2)
    # CONVENTIONS 1.4: origin = (1/2)(e(d+2)' - e(d+1)'), infinity = e(d+2)' + e(d+1)'.
    probe("conformal(1) to R(2,1,0)", [[1, 0, 0], [0, 0, -1], [0, -1, 0]],
          [[1, 0, 0], [0, 1, 0], [0, 0, -1]], [[1, 0, 0], [0, -half, half], [0, 1, 1]])
    probe("conformal(2) to R(3,1,0)", [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 0, -1], [0, 0, -1, 0]],
          [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, -1]],
          [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, -half, half], [0, 0, 1, 1]])
    probe("conformal(1) to R(2,1,0), other scaling", [[1, 0, 0], [0, 0, -1], [0, -1, 0]],
          [[1, 0, 0], [0, 1, 0], [0, 0, -1]], [[1, 0, 0], [0, -1, 1], [0, half, half]])
    # The scaled Witt form of one mode from R(1,1,0): a = e1' + e2', b = (1/2)(e1' - e2').
    probe("Witt, one mode, to R(1,1,0)", [[0, 1], [1, 0]], [[1, 0], [0, -1]], [[1, 1], [half, -half]])
    probe("R(2,0,0) rotated by (3/5, 4/5)", [[1, 0], [0, 1]], [[1, 0], [0, 1]],
          [[F(3, 5), F(4, 5)], [F(-4, 5), F(3, 5)]])
    probe("R(1,1,0) boosted by (5/3, 4/3)", [[1, 0], [0, -1]], [[1, 0], [0, -1]],
          [[F(5, 3), F(4, 3)], [F(4, 3), F(5, 3)]])
    probe("R(1,0,1), the null shear e1 to e1 + e2", [[1, 0], [0, 0]], [[1, 0], [0, 0]], [[1, 1], [0, 1]])
    probe("rigid(3) with its null generator first, to rigid(3)",
          [[0, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]],
          [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 0]],
          [[0, 0, 0, 1], [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0]])
    # The reference's own LDL^T of random form 1: T G T^T = D, so P = T^-1 maps Cl(G) to Cl(D).
    g = [[2, 0, -3], [0, 1, -3], [-3, -3, -2]]
    t, squares = cr.diagonalize([[F(v) for v in row] for row in g])
    diagonal = [[squares[i] if i == j else F(0) for j in range(3)] for i in range(3)]
    probe("random 1 to its LDL^T diagonal", g, diagonal, cr.inverse(t))


if __name__ == "__main__":
    main()
