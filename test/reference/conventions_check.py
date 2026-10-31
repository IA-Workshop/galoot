"""Exact checks of the sign conventions in docs/conventions.md.

This is an independent reference (VVUQ V1, kind 3): a small Clifford algebra
over rationals that shares no code with the library. Run it with
`python3 -I test/reference/conventions_check.py`; it exits nonzero on any
failure.

Blades are bitmasks with bit i-1 for e_i. Names follow Lengyel's oriented
notation, so "e31" is the mask {1, 3} with sign -1.
"""

import itertools
import random
import sys
from fractions import Fraction

N = 4
SQUARES = (1, 1, 1, 0)  # rigid(3) = R(3,0,1): e4 is the null vector
FULL = (1 << N) - 1
SEED = 20261031
TRIALS = 200

failures = []


def check(condition, what):
    if not condition:
        failures.append(what)


def popcount(x):
    return bin(x).count("1")


def reorder_sign(a, b):
    """Sign of e_A e_B relative to the sorted blade e_(A xor B)."""
    swaps = 0
    a >>= 1
    while a:
        swaps += popcount(a & b)
        a >>= 1
    return -1 if swaps & 1 else 1


def square(mask):
    s = 1
    for i in range(N):
        if mask >> i & 1:
            s *= SQUARES[i]
    return s


def clean(x):
    return {m: c for m, c in x.items() if c}


def add(*xs):
    r = {}
    for x in xs:
        for m, c in x.items():
            r[m] = r.get(m, 0) + c
    return clean(r)


def scale(x, s):
    return clean({m: c * s for m, c in x.items()})


def gp(a, b):
    r = {}
    for ma, ca in a.items():
        for mb, cb in b.items():
            c = ca * cb * reorder_sign(ma, mb) * square(ma & mb)
            r[ma ^ mb] = r.get(ma ^ mb, 0) + c
    return clean(r)


def wedge(a, b):
    r = {}
    for ma, ca in a.items():
        for mb, cb in b.items():
            if not ma & mb:
                r[ma | mb] = r.get(ma | mb, 0) + ca * cb * reorder_sign(ma, mb)
    return clean(r)


# u ^ right(u) = antiscalar and left(u) ^ u = antiscalar fix the signs.
def right(x):
    return {FULL ^ m: c * reorder_sign(m, FULL ^ m) for m, c in x.items()}


def left(x):
    return {FULL ^ m: c * reorder_sign(FULL ^ m, m) for m, c in x.items()}


def antigp(a, b):
    return left(gp(right(a), right(b)))


def antiwedge(a, b):
    return left(wedge(right(a), right(b)))


def grade_sign(k):
    return -1 if (k * (k - 1) // 2) & 1 else 1


def reverse(x):
    return {m: c * grade_sign(popcount(m)) for m, c in x.items()}


def antireverse(x):
    return {m: c * grade_sign(N - popcount(m)) for m, c in x.items()}


def involute(x):
    return {m: c * (-1) ** popcount(m) for m, c in x.items()}


def bulk(x):
    return {m: c for m, c in x.items() if square(m)}


def weight(x):
    return {m: c for m, c in x.items() if square(FULL ^ m)}


def dot(a, b):
    return sum(c * b.get(m, 0) * square(m) for m, c in a.items())


def parse(name):
    """Lengyel name to (mask, sign): "e423" is e4 e2 e3 = +e234."""
    if name == "1":
        return 0, 1
    idx = [int(ch) for ch in name[1:]]
    inversions = sum(1 for i, j in itertools.combinations(idx, 2) if i > j)
    return sum(1 << (i - 1) for i in idx), -1 if inversions & 1 else 1


def blade(name, c=1):
    if name.startswith("-"):
        return blade(name[1:], -c)
    mask, sign = parse(name)
    return clean({mask: Fraction(c) * sign})


def coeffs(x, names):
    out = []
    for name in names:
        mask, sign = parse(name)
        out.append(x.get(mask, 0) * sign)
    return out


def point(x, y, z, w=1):
    return add(blade("e1", x), blade("e2", y), blade("e3", z), blade("e4", w))


def sandwich(v, x):
    return antigp(antigp(v, x), antireverse(v))


def random_mv(rng, masks=range(1 << N)):
    return {m: Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for m in masks}


def random_point(rng):
    return point(*(Fraction(rng.randint(-5, 5)) for _ in range(3)))


def check_complements():
    table = {
        "1": ("e1234", "e1234"), "e1": ("e423", "-e423"),
        "e2": ("e431", "-e431"), "e3": ("e412", "-e412"),
        "e4": ("e321", "-e321"), "e23": ("-e41", "-e41"),
        "e31": ("-e42", "-e42"), "e12": ("-e43", "-e43"),
        "e41": ("-e23", "-e23"), "e42": ("-e31", "-e31"),
        "e43": ("-e12", "-e12"), "e423": ("-e1", "e1"),
        "e431": ("-e2", "e2"), "e412": ("-e3", "e3"),
        "e321": ("-e4", "e4"), "e1234": ("1", "1"),
    }
    for u, (r, l) in table.items():
        check(right(blade(u)) == blade(r), f"right complement of {u}")
        check(left(blade(u)) == blade(l), f"left complement of {u}")


def check_identities(rng):
    for _ in range(TRIALS):
        a, b, c = random_mv(rng), random_mv(rng), random_mv(rng)
        check(left(right(a)) == a, "left of right complement is identity")
        check(gp(gp(a, b), c) == gp(a, gp(b, c)), "gp is associative")
        check(antigp(antigp(a, b), c) == antigp(a, antigp(b, c)),
              "antigp is associative")
        for comp in (right, left):
            check(comp(wedge(a, b)) == antiwedge(comp(a), comp(b)),
                  f"De Morgan, wedge, {comp.__name__}")
            check(comp(gp(a, b)) == antigp(comp(a), comp(b)),
                  f"De Morgan, gp, {comp.__name__}")
        check(right(bulk(right(bulk(a)))) == {}, "double bulk dual is 0")
    for k in range(N + 1):
        masks = [m for m in range(1 << N) if popcount(m) == k]
        for _ in range(TRIALS // 4):
            a, b = random_mv(rng, masks), random_mv(rng, masks)
            expect = clean({FULL: dot(a, b)})
            check(wedge(a, right(bulk(b))) == expect,
                  f"a ^ bulk_dual(b) = (a . b) antiscalar, grade {k}")
            # Left duals differ by (-1)^(gr ag); in 4D that negates odd
            # grades, which is the dual-sign disagreement of the spec.
            left_sign = (-1) ** (k * (N - k))
            check(wedge(a, left(bulk(b))) == scale(expect, left_sign),
                  f"left bulk dual sign at grade {k}")


def check_objects():
    xyzw = ("e1", "e2", "e3", "e4")
    line_names = ("e41", "e42", "e43", "e23", "e31", "e12")
    plane_names = ("e423", "e431", "e412", "e321")

    # Line p ^ q has direction q - p on e41, e42, e43 for unit weights.
    line = wedge(point(1, 2, 3), point(2, 2, 5))
    check(coeffs(line, line_names)[:3] == [1, 0, 2], "line direction q - p")

    # Plane (gx, gy, gz, gw): p v g = gx x + gy y + gz z + gw w.
    g = add(blade("e423", 2), blade("e431", -1), blade("e412", 3),
            blade("e321", 5))
    check(antiwedge(point(1, 1, 1), g) == {0: Fraction(2 - 1 + 3 + 5)},
          "plane incidence form")

    # Points counterclockwise about +z give the plane with normal +z.
    tri = wedge(wedge(point(0, 0, 0), point(1, 0, 0)), point(0, 1, 0))
    check(coeffs(tri, plane_names) == [0, 0, 1, 0], "plane orientation")

    # Translator t.(e23, e31, e12) + antiscalar moves points by 2t.
    t = add(blade("e23", 1), blade("e31", -2), blade("e12", 3),
            blade("e1234"))
    moved = coeffs(sandwich(t, point(5, 7, -1)), xyzw)
    check(moved == [7, 3, 5, 1], "translator moves by 2t")

    # Rotor l sin(phi) + antiscalar cos(phi) turns by 2 phi,
    # counterclockwise seen from the tip of the direction of l.
    s, c = Fraction(3, 5), Fraction(4, 5)
    rot = add(blade("e43", s), blade("e1234", c))
    turned = coeffs(sandwich(rot, point(1, 0, 0)), xyzw)
    check(turned == [c * c - s * s, 2 * s * c, 0, 1], "rotor turns by 2 phi")

    # Projection g v (p ^ weight_dual(g)) of a point on a unit plane
    # keeps a positive weight.
    plane_z2 = add(blade("e412"), blade("e321", -2))
    foot = antiwedge(plane_z2, wedge(point(1, 1, 7), right(weight(plane_z2))))
    check(coeffs(foot, xyzw) == [1, 1, 2, 1], "projection on a plane")


def check_versors(rng):
    """A versor's transform must equal the outermorphism of its point map."""
    s, c = Fraction(3, 5), Fraction(4, 5)
    motor = antigp(add(blade("e43", s), blade("e1234", c)),
                   add(blade("e23", 1), blade("e31", 2), blade("e12", -1),
                       blade("e1234")))
    mirror = blade("e412")
    flectors = (mirror, antigp(motor, mirror))

    def motor_transform(x):
        return sandwich(motor, x)

    def flector_transform(f):
        return lambda x: scale(sandwich(f, involute(x)), -1)

    transforms = [("motor", motor_transform)]
    transforms += [(f"flector {i}", flector_transform(f))
                   for i, f in enumerate(flectors)]
    for name, transform in transforms:
        for _ in range(TRIALS // 4):
            a, b, d = (random_point(rng) for _ in range(3))
            ta, tb, td = transform(a), transform(b), transform(d)
            check(coeffs(ta, ("e4",)) == [1], f"{name} keeps point weight")
            check(transform(wedge(a, b)) == wedge(ta, tb),
                  f"{name} outermorphism on lines")
            check(transform(wedge(wedge(a, b), d)) == wedge(wedge(ta, tb), td),
                  f"{name} outermorphism on planes")

    # Applying q1 and then q2 is one transform by q2 v q1.
    q1 = add(blade("e23", 2), blade("e1234"))
    for _ in range(TRIALS // 4):
        x = wedge(random_point(rng), random_point(rng))
        check(sandwich(antigp(motor, q1), x) ==
              sandwich(motor, sandwich(q1, x)), "motor composition order")

    # The single-sign form -F v x v antireverse(F) negates point weights.
    p = point(1, 2, 3)
    check(coeffs(scale(sandwich(mirror, p), -1), ("e4",)) == [-1],
          "-F v p v F~ negates the weight of a point")
    check(coeffs(flector_transform(mirror)(p), ("e1", "e2", "e3")) ==
          [1, 2, -3], "mirror z = 0 reflects z")


def check_conformal_basis():
    # Diagonal R(4,1,0) has e4^2 = +1, e5^2 = -1. The null basis of
    # conformal(3) is origin = (e5 - e4)/2 and infinity = e5 + e4.
    g = ((1, 0), (0, -1))
    origin = (Fraction(-1, 2), Fraction(1, 2))
    infinity = (Fraction(1), Fraction(1))

    def form(u, v):
        return sum(u[i] * g[i][j] * v[j] for i in range(2) for j in range(2))

    check(form(origin, origin) == 0, "origin is null")
    check(form(infinity, infinity) == 0, "infinity is null")
    check(form(origin, infinity) == -1, "origin . infinity = -1")


def main():
    rng = random.Random(SEED)
    check_complements()
    check_identities(rng)
    check_objects()
    check_versors(rng)
    check_conformal_basis()
    if failures:
        print("FAILED (seed %d):" % SEED)
        for what in sorted(set(failures)):
            print("  " + what)
        return 1
    print("all convention checks passed (seed %d)" % SEED)
    return 0


if __name__ == "__main__":
    sys.exit(main())
