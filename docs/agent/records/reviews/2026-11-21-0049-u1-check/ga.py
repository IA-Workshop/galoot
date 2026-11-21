"""Small exact Clifford algebra over a diagonal metric, written for this check.

Blades are bitmasks (bit i-1 is e_i). Coefficients are any ring values that
support + and * (Fraction, int, or sympy expressions). Definitions follow the
conventions specification sections 1 to 4, implemented from scratch here.
"""
from fractions import Fraction
import itertools


def popcount(m):
    return bin(m).count("1")


def reorder_sign(a, b):
    # Sign of e_A e_B -> e_{A xor B} from counting swaps (canonical order).
    s = 0
    a >>= 1
    while a:
        s += popcount(a & b)
        a >>= 1
    return -1 if s & 1 else 1


class Alg:
    def __init__(self, squares):
        self.sq = list(squares)
        self.n = len(squares)
        self.full = (1 << self.n) - 1

    def blades(self):
        return range(1 << self.n)

    def gp_blade(self, a, b):
        c = reorder_sign(a, b)
        common = a & b
        for i in range(self.n):
            if common >> i & 1:
                c *= self.sq[i]
        return c, a ^ b

    def gp(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                c, m = self.gp_blade(a, b)
                if c == 0:
                    continue
                out[m] = out.get(m, 0) + c * ca * cb
        return clean(out)

    def wedge(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                if a & b:
                    continue
                out[a | b] = out.get(a | b, 0) + reorder_sign(a, b) * ca * cb
        return clean(out)

    def right(self, x):
        # u ^ right(u) = 1-bar (antiscalar) with coefficient +1.
        out = {}
        for a, c in x.items():
            b = self.full ^ a
            out[b] = out.get(b, 0) + reorder_sign(a, b) * c
        return clean(out)

    def left(self, x):
        out = {}
        for a, c in x.items():
            b = self.full ^ a
            out[b] = out.get(b, 0) + reorder_sign(b, a) * c
        return clean(out)

    def agp(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def grade_map(self, x, f):
        return clean({m: f(popcount(m)) * c for m, c in x.items()})

    def rev(self, x):
        return self.grade_map(x, lambda k: (-1) ** (k * (k - 1) // 2))

    def antirev(self, x):
        n = self.n
        return self.grade_map(x, lambda k: (-1) ** ((n - k) * (n - k - 1) // 2))

    def ginv(self, x):
        return self.grade_map(x, lambda k: (-1) ** k)

    def aginv(self, x):
        n = self.n
        return self.grade_map(x, lambda k: (-1) ** (n - k))

    def parity(self, v):
        ps = {(self.n - popcount(m)) % 2 for m, c in v.items() if c != 0}
        assert len(ps) == 1, "mixed parity"
        return "odd" if ps.pop() else "even"

    def transform_full(self, x, v):
        # Conventions 4.4, before dropping blades outside x's layout.
        if self.parity(v) == "even":
            return self.agp(self.agp(v, x), self.antirev(v))
        return neg(self.agp(self.agp(v, self.aginv(x)), self.antirev(v)))


def clean(d):
    out = {}
    for m, c in d.items():
        try:
            c = c.expand()
        except AttributeError:
            pass
        if c != 0:
            out[m] = c
    return out


def neg(x):
    return clean({m: -c for m, c in x.items()})


def scale(x, s):
    return clean({m: s * c for m, c in x.items()})


def add(x, y):
    out = dict(x)
    for m, c in y.items():
        out[m] = out.get(m, 0) + c
    return clean(out)


def sub(x, y):
    return add(x, neg(y))


def vec(*cs):
    return clean({1 << i: c for i, c in enumerate(cs)})


def blade(*idx):
    m = 0
    for i in idx:
        m |= 1 << (i - 1)
    # sign of the listed order relative to canonical order
    s = 1
    lst = list(idx)
    for i in range(len(lst)):
        for j in range(i + 1, len(lst)):
            if lst[i] > lst[j]:
                s = -s
    return m, s


def mvb(*terms):
    # terms: (coef, (indices...)); () is the scalar
    out = {}
    for c, idx in terms:
        m, s = blade(*idx)
        out[m] = out.get(m, 0) + s * c
    return clean(out)


def show(x, n):
    if not x:
        return "0"
    parts = []
    for m in sorted(x, key=lambda m: (popcount(m), [i for i in range(n) if m >> i & 1])):
        name = "".join(str(i + 1) for i in range(n) if m >> i & 1) or "1"
        parts.append(f"{x[m]}*e{name}" if m else f"{x[m]}")
    return " + ".join(parts)
