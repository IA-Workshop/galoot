"""Independent reviewer check of the transform's signs (diagonal metrics only).

Multivectors are dicts mask -> Fraction. Shares no code with the library or
its reference scripts.
"""
from fractions import Fraction as Fr
import itertools, random, sys


def reorder_sign(a, b):
    # sign of moving generators of b past generators of a to sort a*b
    s = 0
    a >>= 1
    while a:
        s += bin(a & b).count("1")
        a >>= 1
    return -1 if s & 1 else 1


class Alg:
    def __init__(self, sq):
        self.sq = sq
        self.n = len(sq)
        self.I = (1 << self.n) - 1

    def gp_blade(self, a, b):
        s = reorder_sign(a, b)
        for i in range(self.n):
            if a & b & (1 << i):
                s *= self.sq[i]
        return s, a ^ b

    def gp(self, x, y):
        r = {}
        for a, ca in x.items():
            for b, cb in y.items():
                s, m = self.gp_blade(a, b)
                if s:
                    r[m] = r.get(m, 0) + s * ca * cb
        return {k: v for k, v in r.items() if v}

    def wedge(self, x, y):
        r = {}
        for a, ca in x.items():
            for b, cb in y.items():
                if a & b:
                    continue
                m = a | b
                r[m] = r.get(m, 0) + reorder_sign(a, b) * ca * cb
        return {k: v for k, v in r.items() if v}

    def right(self, x):
        # u ^ right(u) = I
        return {self.I ^ a: reorder_sign(a, self.I ^ a) * c for a, c in x.items()}

    def left(self, x):
        # left(u) ^ u = I
        return {self.I ^ a: reorder_sign(self.I ^ a, a) * c for a, c in x.items()}

    def agp(self, x, y):
        return self.left(self.gp(self.right(x), self.right(y)))

    def antiwedge(self, x, y):
        return self.left(self.wedge(self.right(x), self.right(y)))

    def gr(self, m):
        return bin(m).count("1")

    def ag(self, m):
        return self.n - self.gr(m)

    def antireverse(self, x):
        return {a: (-1 if (self.ag(a) * (self.ag(a) - 1) // 2) & 1 else 1) * c
                for a, c in x.items()}

    def reverse(self, x):
        return {a: (-1 if (self.gr(a) * (self.gr(a) - 1) // 2) & 1 else 1) * c
                for a, c in x.items()}

    def ginv(self, x):
        return {a: (-1 if self.gr(a) & 1 else 1) * c for a, c in x.items()}

    def aginv(self, x):
        return {a: (-1 if self.ag(a) & 1 else 1) * c for a, c in x.items()}

    def parity(self, v):
        ps = {self.ag(a) & 1 for a in v}
        assert len(ps) <= 1, "mixed"
        return ps.pop() if ps else 0

    def transform(self, x, v):
        p = self.parity(v)
        xi = self.aginv(x) if p else x
        r = self.agp(self.agp(v, xi), self.antireverse(v))
        return scale(r, -1) if p else r


def scale(x, s):
    return {k: s * c for k, c in x.items() if s * c}


def add(x, y):
    r = dict(x)
    for k, c in y.items():
        r[k] = r.get(k, 0) + c
    return {k: c for k, c in r.items() if c}


def eq(x, y):
    return add(x, scale(y, -1)) == {}


def vec(alg, cs):
    return {1 << i: Fr(c) for i, c in enumerate(cs) if c}


def mask(s):
    m = 0
    for ch in s:
        m |= 1 << (int(ch) - 1)
    return m


def blade(alg, idx, c=1):
    """Oriented blade e_{idx} as written (e.g. '41'), as canonical dict."""
    if idx == "":
        return {0: Fr(c)}
    x = {0: Fr(c)}
    for ch in idx:
        x = alg.wedge(x, {1 << (int(ch) - 1): Fr(1)})
    return x


def rand_hyperplane(alg, rng):
    # wedge of n-1 random vectors
    h = {0: Fr(1)}
    for _ in range(alg.n - 1):
        h = alg.wedge(h, vec(alg, [rng.randint(-5, 5) for _ in range(alg.n)]))
    return h


results = []


def check(name, cond):
    results.append((name, bool(cond)))
    print(("PASS " if cond else "FAIL ") + name)
