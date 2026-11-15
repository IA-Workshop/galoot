"""rigid(3) = R(3,0,1) as exact polynomial algebra, from the definitions.

Written from the definitions of the conventions (generators, complements,
antiproduct, antireverse, oriented layouts) with sympy polynomials as
coefficients. It shares no code with the library; its geometric product table
is compared with test/reference/clifford_reference.py in derive.py.

A multivector is a dict {mask: sympy expression}; bit i-1 of a mask stands
for e_i, and the blade of a mask is the wedge of its generators in increasing
order. e4 is null.
"""

import sympy as sp

N = 4
FULL = (1 << N) - 1
SQUARE = {0: 1, 1: 1, 2: 1, 3: 0}  # g(e_i, e_i) by bit position


def pop(x):
    return bin(x).count("1")


def indices(mask):
    return [i for i in range(N) if (mask >> i) & 1]


def reorder_sign(a, b):
    """Sign of e_a e_b = s e_(a^b) when the generators are distinct, by
    counting the transpositions that put the list a then b in order."""
    swaps = 0
    for i in indices(a):
        swaps += pop(b & ((1 << i) - 1))
    return -1 if swaps % 2 else 1


def blade_gp(a, b):
    """The geometric product of two basis blades: (sign, mask), or (0, 0)."""
    s = reorder_sign(a, b)
    for i in indices(a & b):
        s *= SQUARE[i]
    return (s, a ^ b) if s != 0 else (0, 0)


def right_comp(a):
    """(sign, mask) with e_a ^ (sign e_mask) = e_1234."""
    c = FULL ^ a
    return reorder_sign(a, c), c


def left_comp(a):
    """(sign, mask) with (sign e_mask) ^ e_a = e_1234."""
    c = FULL ^ a
    return reorder_sign(c, a), c


def clean(x):
    out = {}
    for k, v in x.items():
        v = sp.expand(v)
        if v != 0:
            out[k] = v
    return out


def add(x, y):
    out = dict(x)
    for k, v in y.items():
        out[k] = out.get(k, 0) + v
    return clean(out)


def scale(c, x):
    return clean({k: c * v for k, v in x.items()})


def gp(x, y):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            s, m = blade_gp(a, b)
            if s:
                out[m] = out.get(m, 0) + s * ca * cb
    return clean(out)


def wedge(x, y):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            if a & b:
                continue
            out[a | b] = out.get(a | b, 0) + reorder_sign(a, b) * ca * cb
    return clean(out)


def right(x):
    out = {}
    for a, c in x.items():
        s, m = right_comp(a)
        out[m] = out.get(m, 0) + s * c
    return clean(out)


def left(x):
    out = {}
    for a, c in x.items():
        s, m = left_comp(a)
        out[m] = out.get(m, 0) + s * c
    return clean(out)


def antiproduct(x, y):
    return left(gp(right(x), right(y)))


def antiwedge(x, y):
    return left(wedge(right(x), right(y)))


def antireverse(x):
    out = {}
    for a, c in x.items():
        ag = N - pop(a)
        out[a] = c * (-1 if (ag * (ag - 1) // 2) % 2 else 1)
    return clean(out)


def grade_involution(x):
    return clean({a: c * (-1) ** pop(a) for a, c in x.items()})


def transform(x, v, odd=False):
    """V x V~ with the antiproduct; an odd V gives -V x^ V~ (n = 4)."""
    if odd:
        return scale(-1, antiproduct(antiproduct(v, grade_involution(x)),
                                     antireverse(v)))
    return antiproduct(antiproduct(v, x), antireverse(v))


# --- Layouts: names, masks, orientations (the presets' conventions) ---------

def m_(*idx):
    r = 0
    for i in idx:
        r |= 1 << (i - 1)
    return r


# (name, mask, orientation sign): coefficient c on the oriented name stands
# for c * sign * e_mask.
POINT = [("x", m_(1), 1), ("y", m_(2), 1), ("z", m_(3), 1), ("w", m_(4), 1)]
LINE = [("vx", m_(1, 4), -1), ("vy", m_(2, 4), -1), ("vz", m_(3, 4), -1),
        ("mx", m_(2, 3), 1), ("my", m_(1, 3), -1), ("mz", m_(1, 2), 1)]
PLANE = [("gx", m_(2, 3, 4), 1), ("gy", m_(1, 3, 4), -1),
         ("gz", m_(1, 2, 4), 1), ("gw", m_(1, 2, 3), -1)]
MOTOR = [("Qvx", m_(1, 4), -1), ("Qvy", m_(2, 4), -1), ("Qvz", m_(3, 4), -1),
         ("Qvw", m_(1, 2, 3, 4), 1), ("Qmx", m_(2, 3), 1),
         ("Qmy", m_(1, 3), -1), ("Qmz", m_(1, 2), 1), ("Qmw", 0, 1)]


def symbols(layout, suffix=""):
    return [sp.Symbol(n + suffix) for n, _, _ in layout]


def build(layout, syms):
    """The multivector with the given coefficients on the oriented layout."""
    return clean({m: s * c for (n, m, s), c in zip(layout, syms)})


def read(layout, x):
    """The coefficients of x on the oriented layout; asserts that x has no
    other blade."""
    used = {m for _, m, _ in layout}
    extra = {k: v for k, v in x.items() if k not in used}
    return [sp.expand(x.get(m, 0) * s) for _, m, s in layout], extra
