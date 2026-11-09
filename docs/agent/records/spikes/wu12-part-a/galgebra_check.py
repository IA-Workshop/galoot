"""A second route for the product-dependent statements of research note 0026
section A: the geometric product and the antiproduct under a change of basis,
computed with galgebra's products, which share no code with Galoot or with
test/reference/clifford_reference.py.

For each change P (row i writes e_i in the e'_j, P G' P^T = G), on every pair
of basis blades: phi(a b) = phi(a) phi(b); phi(a v b) = phi(a) v' phi(b) / det P;
and, where P^-T G' P^-1 = G, phi(a antigp b) = phi(a) antigp' phi(b) / det P,
with the antiproducts built from galgebra's product and wedge and the
complements u ^ right(u) = antiscalar solved here from galgebra's wedge. phi
is the wedge of the images of the generators, formed with galgebra's wedge
(not from minors). The four changes include two with |det P| != 1.

Run with the math-tools environment (galgebra 0.6.0, SymPy 1.14.0):
  .venv/bin/python -I docs/agent/records/spikes/wu12-part-a/galgebra_check.py
"""

import contextlib
import io
import itertools
import sys
from fractions import Fraction as F

import sympy
from galgebra.ga import Ga


def grade(m):
    return bin(m).count("1")


class GA:
    def __init__(self, g, names):
        with contextlib.redirect_stdout(io.StringIO()):
            self.ga = Ga(" ".join(names), g=sympy.Matrix(g))
        self.n = len(g)
        self.gens = list(self.ga.mv())
        self.top = (1 << self.n) - 1
        self.masks = sorted(range(1 << self.n),
                            key=lambda m: (grade(m), [i for i in range(self.n) if m >> i & 1]))
        self.blade = {}
        for m in range(1 << self.n):
            x = self.ga.mv(sympy.Integer(1), "scalar")
            for i in range(self.n):
                if m >> i & 1:
                    x = x ^ self.gens[i]
            self.blade[m] = x
        # u ^ right(u) = antiscalar: right(e_A) = s e_(top ^ A) with s read off.
        self.rsign = {}
        for m in range(1 << self.n):
            w = self.coords(self.blade[m] ^ self.blade[self.top ^ m])
            self.rsign[m] = w[self.top]

    def coords(self, x):
        if not isinstance(x.obj, sympy.Expr) and x.obj == 0:
            return {}
        cs = x.blade_coefs()
        out = {}
        for m, c in zip(self.masks, cs):
            c = sympy.nsimplify(sympy.expand(c))
            if c != 0:
                out[m] = F(int(c.p), int(c.q))
        return out

    def mv(self, d):
        x = self.ga.mv(0, "scalar")
        for m, c in d.items():
            x = x + sympy.Rational(c.numerator, c.denominator) * self.blade[m]
        return x

    def right(self, d):
        return {self.top ^ m: c * self.rsign[m] for m, c in d.items()}

    def left(self, d):
        # left(u) ^ u = antiscalar; left is the inverse of right.
        out = {}
        for m, c in d.items():
            src = self.top ^ m
            out[src] = c / self.rsign[src]
        return out

    def gp(self, a, b):
        return self.coords(self.mv(a) * self.mv(b))

    def wedge(self, a, b):
        return self.coords(self.mv(a) ^ self.mv(b))

    def antigp(self, a, b):
        return self.left(self.gp(self.right(a), self.right(b)))

    def antiwedge(self, a, b):
        return self.left(self.wedge(self.right(a), self.right(b)))


def scale(d, f):
    return {m: c * f for m, c in d.items() if c * f != 0}


def run(name, g, gt, p):
    n = len(g)
    s = GA(g, [f"e{i + 1}" for i in range(n)])
    t = GA(gt, [f"f{i + 1}" for i in range(n)])
    pm = sympy.Matrix([[sympy.Rational(F(v).numerator, F(v).denominator) for v in row] for row in p])
    assert pm * sympy.Matrix(gt) * pm.T == sympy.Matrix(g), name
    d = F(str(pm.det()))
    q = pm.inv().T
    contragredient = q * sympy.Matrix(gt) * q.T == sympy.Matrix(g)
    images = [sum((pm[i, j] * t.gens[j] for j in range(n)), t.ga.mv(0, "scalar")) for i in range(n)]

    def phi(dct):
        x = t.ga.mv(0, "scalar")
        for m, c in dct.items():
            w = t.ga.mv(sympy.Integer(1), "scalar")
            for i in range(n):
                if m >> i & 1:
                    w = w ^ images[i]
            x = x + sympy.Rational(c.numerator, c.denominator) * w
        return t.coords(x)

    bad = {"gp": 0, "antiwedge": 0, "antigp": 0}
    for a, b in itertools.product(range(1 << n), repeat=2):
        x, y = {a: F(1)}, {b: F(1)}
        if phi(s.gp(x, y)) != t.gp(phi(x), phi(y)):
            bad["gp"] += 1
        if phi(s.antiwedge(x, y)) != scale(t.antiwedge(phi(x), phi(y)), 1 / d):
            bad["antiwedge"] += 1
        if phi(s.antigp(x, y)) != scale(t.antigp(phi(x), phi(y)), 1 / d):
            bad["antigp"] += 1
    pairs = 1 << (2 * n)
    print(f"{name}: det P = {d}, contragredient isometry {contragredient}; failures of "
          f"{pairs} pairs: gp {bad['gp']}, antiwedge/det {bad['antiwedge']}, "
          f"antigp/det {bad['antigp']}")
    ok = bad["gp"] == 0 and bad["antiwedge"] == 0
    ok = ok and ((bad["antigp"] == 0) == contragredient)
    return ok


def main():
    h = F(1, 2)
    ok = True
    ok &= run("conformal(2) to R(3,1,0)",
              [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 0, -1], [0, 0, -1, 0]],
              [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, -1]],
              [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, -h, h], [0, 0, 1, 1]])
    ok &= run("Witt, one mode, to R(1,1,0)", [[0, 1], [1, 0]], [[1, 0], [0, -1]],
              [[h, h], [1, -1]])
    ok &= run("rigid(3), e4 to 2 e4'", [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 0]],
              [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 0]],
              [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 2]])
    ok &= run("M R(2,1,0) M^T, det M = 3, to R(2,1,0)",
              [[5, 2, 1], [2, 0, -1], [1, -1, 0]], [[1, 0, 0], [0, 1, 0], [0, 0, -1]],
              [[1, 2, 0], [0, 1, 1], [1, 0, 1]])
    print("all agree" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
