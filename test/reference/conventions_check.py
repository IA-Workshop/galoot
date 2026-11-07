"""Exact checks of Galoot's sign conventions: blade order and names,
complements and duals, products and their De Morgan laws, and the rigid and
conformal objects and transforms.

This is an independent reference: a small Clifford algebra over rationals that
shares no code with the library, from which each convention is checked
exactly. Run it with
`python3 -I test/reference/conventions_check.py`. It prints the sign it
finds for a single mirror in each form it tries, then one line that names
the seed and says whether every check passed, and it exits nonzero on any
failure.

Blades are bitmasks with bit i-1 for e_i. Names follow Lengyel's oriented
notation, so "e31" is the mask {1, 3} with sign -1.

Most checks run in rigid(3). Those that need another diagonal form
R(p, q, r) run inside `with diagonal_form(squares):`, which rebinds N,
SQUARES, and FULL for the enclosed block only.
"""

import contextlib
import itertools
import math
import random
import sys
from fractions import Fraction

N = 4
SQUARES = (1, 1, 1, 0)  # rigid(3) = R(3,0,1): e4 is the null vector
FULL = (1 << N) - 1
SEED = 20261031
TRIALS = 200

failures = []


@contextlib.contextmanager
def diagonal_form(squares):
    """Run the enclosed checks in the diagonal form with these squares."""
    global N, SQUARES, FULL
    saved = N, SQUARES, FULL
    N, SQUARES, FULL = len(squares), tuple(squares), (1 << len(squares)) - 1
    try:
        yield
    finally:
        N, SQUARES, FULL = saved


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


def antigrade_involute(x):
    """Each blade of antigrade m times (-1)^m."""
    return {m: c * (-1) ** (N - popcount(m)) for m, c in x.items()}


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


def random_mv(rng, masks=None):
    if masks is None:
        masks = range(1 << N)
    return {m: Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for m in masks}


def random_vector(rng):
    return clean({1 << i: Fraction(rng.randint(-5, 5)) for i in range(N)})


def wedge_all(xs):
    r = {0: Fraction(1)}
    for x in xs:
        r = wedge(r, x)
    return r


def determinant(m):
    """Leibniz expansion; the empty matrix has determinant 1."""
    total = 0
    for perm in itertools.permutations(range(len(m))):
        inversions = sum(1 for i, j in itertools.combinations(perm, 2)
                         if i > j)
        term = -1 if inversions & 1 else 1
        for row, col in enumerate(perm):
            term *= m[row][col]
        total += term
    return total


def exomorphism(x):
    """The metric exomorphism G: e_A goes to the sum over |B| = |A| of
    det g[A, B] e_B, with g the diagonal Gram matrix of SQUARES."""
    g = [[SQUARES[i] if i == j else 0 for j in range(N)] for i in range(N)]
    r = {}
    for a, c in x.items():
        rows = [i for i in range(N) if a >> i & 1]
        for cols in itertools.combinations(range(N), len(rows)):
            minor = determinant([[g[i][j] for j in cols] for i in rows])
            b = sum(1 << j for j in cols)
            r[b] = r.get(b, 0) + c * minor
    return clean(r)


def random_point(rng):
    return point(*(Fraction(rng.randint(-5, 5)) for _ in range(3)))


# Right and left complements of each oriented name of rigid(3).
COMPLEMENT_TABLE = {
    "1": ("e1234", "e1234"), "e1": ("e423", "-e423"),
    "e2": ("e431", "-e431"), "e3": ("e412", "-e412"),
    "e4": ("e321", "-e321"), "e23": ("-e41", "-e41"),
    "e31": ("-e42", "-e42"), "e12": ("-e43", "-e43"),
    "e41": ("-e23", "-e23"), "e42": ("-e31", "-e31"),
    "e43": ("-e12", "-e12"), "e423": ("-e1", "e1"),
    "e431": ("-e2", "e2"), "e412": ("-e3", "e3"),
    "e321": ("-e4", "e4"), "e1234": ("1", "1"),
}


def check_complements():
    for u, (r, l) in COMPLEMENT_TABLE.items():
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
            # grades, so a library that uses left duals for points and
            # planes gets the opposite sign of a ^ dual(b) there. This check
            # tells the two choices apart.
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


# Diagonal forms R(p, q, r) for the bulk and the weight: Euclidean,
# indefinite, all negative, and degenerate with one to three null generators.
DIAGONAL_FORMS = {
    "R(3,0,0)": (1, 1, 1), "R(2,1,0)": (1, 1, -1),
    "R(1,3,0)": (1, -1, -1, -1), "R(0,2,0)": (-1, -1),
    "R(2,2,0)": (1, 1, -1, -1), "rigid(2)": (1, 1, 0),
    "rigid(3)": (1, 1, 1, 0), "rigid(4)": (1, 1, 1, 1, 0),
    "R(2,1,1)": (1, 1, -1, 0), "R(2,0,2)": (1, 1, 0, 0),
    "R(1,1,2)": (1, -1, 0, 0), "R(1,0,3)": (1, 0, 0, 0),
    "R(0,1,3)": (-1, 0, 0, 0),
}


def check_bulk_weight_diagonal():
    """In a diagonal form the bulk keeps the blades whose square is nonzero,
    each times the product of the squares of its generators, and the weight
    keeps the blades whose complement's square is nonzero, each times the
    product of the squares of the generators it lacks. Checked on every
    blade against the definitions: the bulk is G x and the weight is
    left(G right(x)), with G the metric exomorphism."""
    for name, squares in DIAGONAL_FORMS.items():
        with diagonal_form(squares):
            nulls = [i for i in range(N) if SQUARES[i] == 0]
            neither = []
            for m in range(1 << N):
                e = {m: Fraction(1)}
                own = 1
                for i in range(N):
                    if m >> i & 1:
                        own *= SQUARES[i]
                lacked = 1
                for i in range(N):
                    if not m >> i & 1:
                        lacked *= SQUARES[i]
                blade_square = gp(e, e).get(0, 0)
                complement_square = gp(right(e), right(e)).get(0, 0)
                rule_bulk = scale(e, own) if blade_square else {}
                rule_weight = scale(e, lacked) if complement_square else {}
                check(exomorphism(e) == rule_bulk,
                      f"{name}: bulk of {m:b} by the diagonal rule")
                check(left(exomorphism(right(e))) == rule_weight,
                      f"{name}: weight of {m:b} by the diagonal rule")
                if not rule_bulk and not rule_weight:
                    neither.append(m)
                # In rigid(d) the factors are 1: the bulk is the part without
                # the null generator and the weight the part with it.
                if name.startswith("rigid"):
                    check(exomorphism(e) == bulk(e)
                          and left(exomorphism(right(e))) == weight(e)
                          and bool(bulk(e)) != bool(m >> (N - 1) & 1)
                          and bool(weight(e)) == bool(m >> (N - 1) & 1),
                          f"{name}: bulk and weight of {m:b} split by e{N}")
            check(bool(neither) == (len(nulls) >= 2),
                  f"{name}: blades in neither the bulk nor the weight "
                  f"exist exactly when two or more generators are null")


def check_antiprojection(rng):
    """The antiprojection of a onto b, b ^ (a v weight_dual(b)), in
    rigid(3): onto a unitized point, and the general formulas of the rigid
    wiki's Projections page."""
    line_names = ("e41", "e42", "e43", "e23", "e31", "e12")
    plane_names = ("e423", "e431", "e412", "e321")

    def antiproject(a, b):
        return wedge(b, antiwedge(a, right(weight(b))))

    def draw():
        return [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                for _ in range(6)]

    for _ in range(TRIALS // 4):
        gx, gy, gz, gw, _, _ = draw()
        vx, vy, vz, mx, my, mz = draw()
        px, py, pz, pw, _, _ = draw()
        g = add(blade("e423", gx), blade("e431", gy), blade("e412", gz),
                blade("e321", gw))
        line = add(*(blade(n, c) for n, c in
                     zip(line_names, (vx, vy, vz, mx, my, mz))))
        p = point(px, py, pz)
        moved = antiproject(g, p)
        # The plane through p with the normal of g.
        check(coeffs(moved, plane_names) ==
              [gx, gy, gz, -(gx * px + gy * py + gz * pz)],
              "antiprojection of a plane onto a unitized point")
        check(antiwedge(p, moved) == {}, "it contains the point")
        check(antiproject(moved, p) == moved, "it is unchanged when repeated")
        # The line through p with the direction of the line.
        moved = antiproject(line, p)
        check(coeffs(moved, line_names) ==
              [vx, vy, vz, py * vz - pz * vy, pz * vx - px * vz,
               px * vy - py * vx],
              "antiprojection of a line onto a unitized point")
        check(wedge(p, moved) == {}, "it contains the point")
        # The page's general rows, with a weight that is not 1.
        q = point(px, py, pz, pw)
        check(coeffs(antiproject(g, q), plane_names) ==
              [pw * pw * gx, pw * pw * gy, pw * pw * gz,
               -(gx * px + gy * py + gz * pz) * pw],
              "antiprojection of a plane onto a point, general weight")
        check(coeffs(antiproject(line, q), line_names) ==
              [pw * pw * vx, pw * pw * vy, pw * pw * vz,
               (py * vz - pz * vy) * pw, (pz * vx - px * vz) * pw,
               (px * vy - py * vx) * pw],
              "antiprojection of a line onto a point, general weight")
        lv2 = vx * vx + vy * vy + vz * vz
        g_lv = gx * vx + gy * vy + gz * vz
        check(coeffs(antiproject(g, line), plane_names) ==
              [lv2 * gx - g_lv * vx, lv2 * gy - g_lv * vy,
               lv2 * gz - g_lv * vz,
               gx * my * vz - gx * mz * vy + gy * mz * vx - gy * mx * vz
               + gz * mx * vy - gz * my * vx],
              "antiprojection of a plane onto a line")


def mirror_transform(v):
    """The transform by an odd versor, -V v x' v antireverse(V), where x' is
    the antigrade involution."""
    return lambda x: scale(sandwich(v, antigrade_involute(x)), -1)


def is_outermorphism(transform, lam, rng):
    """lam^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) for k = 1 to N."""
    for k in range(1, N + 1):
        vs = [random_vector(rng) for _ in range(k)]
        lhs = scale(transform(wedge_all(vs)), lam ** (k - 1))
        if lhs != wedge_all([transform(v) for v in vs]):
            return False
    return True


# Forms for the sign of an odd versor, by n.
ODD_SIGN_FORMS = {
    "rigid(2)": (1, 1, 0), "R(3,0,0)": (1, 1, 1), "rigid(3)": (1, 1, 1, 0),
    "R(4,0,0)": (1, 1, 1, 1), "rigid(4)": (1, 1, 1, 1, 0),
    "R(5,0,0)": (1, 1, 1, 1, 1),
}


def check_odd_sign(rng):
    """Find the sign s for which a single mirror V acts as
    s V v x^ v antireverse(V), from the outermorphism property, then check
    that it fixes its mirror and that it equals -V v x' v antireverse(V).
    Returns the sign found for each form."""
    found = {}
    for name, squares in ODD_SIGN_FORMS.items():
        with diagonal_form(squares):
            for _ in range(TRIALS // 20):
                x = clean(random_mv(rng))
                # The right complement turns the antigrade involution
                # into the grade involution, and x' = (-1)^n x^.
                check(right(antigrade_involute(x)) == involute(right(x)),
                      f"{name}: right(x') = right(x)^")
                check(antigrade_involute(x) == scale(involute(x), (-1) ** N),
                      f"{name}: x' = (-1)^n x^")
            signs = set()
            for _ in range(TRIALS // 20):
                inside = [random_vector(rng) for _ in range(N - 1)]
                v = wedge_all(inside)
                lam = antigp(v, antireverse(v)).get(FULL, 0)
                if not lam:
                    continue
                check(antigp(v, antireverse(v)) == {FULL: lam},
                      f"{name}: V v antireverse(V) = lambda antiscalar")
                passing = [
                    s for s in (1, -1) if is_outermorphism(
                        lambda x, s=s: scale(sandwich(v, involute(x)), s),
                        lam, rng)]
                check(len(passing) == 1,
                      f"{name}: one sign makes the mirror an outermorphism")
                signs.update(passing)
                t = mirror_transform(v)
                check(is_outermorphism(t, lam, rng),
                      f"{name}: -V v x' v antireverse(V) is an outermorphism")
                check(all(t(a) == scale(a, lam) for a in inside),
                      f"{name}: the mirror fixes its vectors times lambda")
                x = random_mv(rng)
                check(all(t(x) == scale(sandwich(v, involute(x)), s)
                          for s in passing),
                      f"{name}: -V v x' v antireverse(V) = s V v x^ v "
                      f"antireverse(V) with the sign found")
            check(len(signs) == 1, f"{name}: the sign found is the same "
                                   f"for every mirror")
            found[name] = signs.pop() if len(signs) == 1 else None

    # An even versor of rigid(2), with odd grades and even antigrades, under
    # V v x v antireverse(V).
    with diagonal_form((1, 1, 0)):
        for _ in range(TRIALS // 20):
            rx, ry, rz, rw = (Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                              for _ in range(4))
            v = add(blade("e1", rx), blade("e2", ry), blade("e3", rz),
                    blade("e123", rw))
            lam = antigp(v, antireverse(v)).get(FULL, 0)
            check(lam and antigp(v, antireverse(v)) == {FULL: lam},
                  "rigid(2): even versor, V v antireverse(V) = lambda")
            check(is_outermorphism(lambda x: sandwich(v, x), lam, rng),
                  "rigid(2): an even versor acts as an outermorphism")
        # The mirror e23 is the line x = 0.
        moved = mirror_transform(blade("e23"))(
            add(blade("e1"), blade("e2", 2), blade("e3")))
        check(coeffs(moved, ("e1", "e2", "e3")) == [-1, 2, 1],
              "rigid(2): the mirror e23 sends (1, 2, 1) to (-1, 2, 1)")
    moved = mirror_transform(blade("e412"))(point(1, 2, 3))
    check(coeffs(moved, ("e1", "e2", "e3", "e4")) == [1, 2, -3, 1],
          "rigid(3): the mirror e412 sends (1, 2, 3, 1) to (1, 2, -3, 1)")
    return found


# The coordinates of the rigid objects of rigid(3), in Lengyel's oriented
# names and in the order of his published formulas. A motor is a weight
# (r_x, r_y, r_z, r_w) and a bulk (u_x, u_y, u_z, u_w); a flector is a point
# and a plane.
POINT_NAMES = ("e1", "e2", "e3", "e4")
LINE_NAMES = ("e41", "e42", "e43", "e23", "e31", "e12")
PLANE_NAMES = ("e423", "e431", "e412", "e321")
MOTOR_NAMES = ("e41", "e42", "e43", "e1234", "e23", "e31", "e12", "1")
FLECTOR_NAMES = ("e1", "e2", "e3", "e4", "e423", "e431", "e412", "e321")

# Sines and cosines of half angles with rational values, from Pythagorean
# triples; the last turns by more than a half turn.
HALF_ANGLES = tuple((Fraction(s), Fraction(c)) for s, c in (
    ("3/5", "4/5"), ("5/13", "12/13"), ("8/17", "15/17"), ("7/25", "24/25"),
    ("4/5", "-3/5")))


def on_names(names, values):
    return add(*(blade(n, v) for n, v in zip(names, values)))


def cross(a, b):
    return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]]


def translator(d):
    """The motor that moves by d: half of d on e23, e31, e12, plus the
    antiscalar."""
    return add(on_names(("e23", "e31", "e12"), [Fraction(x, 2) for x in d]),
               blade("e1234"))


def rotor(line, s, c):
    """line sin(phi) + antiscalar cos(phi), given s = sin(phi) and
    c = cos(phi); it turns by 2 phi about the line."""
    return add(scale(line, s), blade("e1234", c))


def rigid_transform(v, x):
    """The transform of x by a versor of rigid(3): V v x v antireverse(V) for
    an even V, and -V v x^ v antireverse(V) for an odd one."""
    parities = {popcount(m) & 1 for m in v}
    if len(parities) != 1:
        raise ValueError("a versor has blades of one parity")
    if parities == {0}:
        return sandwich(v, x)
    return scale(sandwich(v, involute(x)), -1)


def rigid_affine(v):
    """The 3 by 4 matrix [R | t] of the rigid motion of a versor V with
    V v antireverse(V) = lambda antiscalar, lambda > 0: column j < 3 is the
    image of the direction e_(j+1) and column 3 the image of the origin, each
    divided by lambda."""
    lam = antigp(v, antireverse(v))
    if set(lam) != {FULL} or lam[FULL] <= 0:
        raise ValueError("V v antireverse(V) is not a positive antiscalar")
    images = [coeffs(rigid_transform(v, blade(n)), POINT_NAMES)
              for n in POINT_NAMES]
    for j, image in enumerate(images):
        if image[3] != (lam[FULL] if j == 3 else 0):
            raise ValueError("a versor moved the weight of a point")
    return [[images[j][i] / lam[FULL] for j in range(4)] for i in range(3)]


def normalized(v):
    """V divided by its first nonzero coefficient in order of mask, so that
    the versors of one motion, which differ by a nonzero factor, become one."""
    first = v[min(v)]
    return clean({m: c / first for m, c in v.items()})


def versor_closure(generators):
    """Every product of the generators under the antiproduct, each normalized,
    with the antiscalar as the identity."""
    identity = normalized(blade("e1234"))
    found = {tuple(sorted(identity.items())): identity}
    frontier = [identity]
    while frontier:
        new = []
        for x in frontier:
            for g in generators:
                y = normalized(antigp(x, g))
                key = tuple(sorted(y.items()))
                if key not in found:
                    found[key] = y
                    new.append(y)
        frontier = new
    return list(found.values())


# cot^2(pi / n) for the turns by 2 pi / n that a crystal allows: the rotor
# of such a turn has the half angle pi / n.
CRYSTAL_HALF_TURNS = {2: Fraction(0), 3: Fraction(1, 3), 4: Fraction(1),
                      6: Fraction(3)}


def crystal_versor(generator):
    """A versor, up to a positive factor, of one generator of a point group.
    ("rotation", axis, n) turns by 2 pi / n about the line through the
    origin along axis: the rotor axis sin(pi/n) / |axis| + antiscalar
    cos(pi/n), scaled to axis + r antiscalar on e41, e42, e43 with
    r = |axis| cot(pi/n), which must be rational. ("rotoinversion", axis, n)
    is that rotor times e4, ("mirror", normal) is the plane through the
    origin with that normal, and ("inversion",) is e4, the origin."""
    kind = generator[0]
    if kind == "inversion":
        return blade("e4")
    if kind == "mirror":
        return on_names(PLANE_NAMES, list(generator[1]) + [0])
    axis = [Fraction(x) for x in generator[1]]
    r2 = sum(x * x for x in axis) * CRYSTAL_HALF_TURNS[generator[2]]
    r = Fraction(math.isqrt(r2.numerator), math.isqrt(r2.denominator))
    if r * r != r2:
        raise ValueError(f"|axis| cot(pi/n) is irrational for {generator}")
    turn = add(on_names(("e41", "e42", "e43"), axis), blade("e1234", r))
    if kind == "rotation":
        return turn
    if kind == "rotoinversion":
        return antigp(turn, blade("e4"))
    raise ValueError(f"unknown generator {kind}")


def check_motor_flector_layouts(rng):
    """The motor layout holds exactly the blades of even grade and the
    flector layout those of odd grade, so the antiproduct of two motors or two
    flectors is on the motor layout and that of a motor and a flector, in
    either order, is on the flector layout. Checked on random coefficients,
    not only on versors."""
    motor_masks = {parse(n)[0] for n in MOTOR_NAMES}
    flector_masks = {parse(n)[0] for n in FLECTOR_NAMES}
    check(motor_masks == {m for m in range(1 << N) if popcount(m) % 2 == 0},
          "the motor layout is the blades of even grade")
    check(flector_masks == {m for m in range(1 << N) if popcount(m) % 2},
          "the flector layout is the blades of odd grade")

    def draw(names):
        return on_names(names, [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                                for _ in names])

    pairs = ((MOTOR_NAMES, MOTOR_NAMES, motor_masks),
             (FLECTOR_NAMES, FLECTOR_NAMES, motor_masks),
             (MOTOR_NAMES, FLECTOR_NAMES, flector_masks),
             (FLECTOR_NAMES, MOTOR_NAMES, flector_masks))
    for a_names, b_names, result in pairs:
        for _ in range(TRIALS // 4):
            product = antigp(draw(a_names), draw(b_names))
            check(set(product) <= result,
                  f"antiproduct of {a_names[0]}... and {b_names[0]}... "
                  f"stays on its layout")


def check_line_moment(rng):
    """For points p and q of weights w_p and w_q, p ^ q has direction
    w_p q - w_q p on e41, e42, e43 and moment p x q on e23, e31, e12."""
    for _ in range(TRIALS):
        p = [Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for _ in range(4)]
        q = [Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for _ in range(4)]
        line = coeffs(wedge(point(*p), point(*q)), LINE_NAMES)
        check(line[:3] == [p[3] * q[i] - q[3] * p[i] for i in range(3)],
              "line direction w_p q - w_q p")
        check(line[3:] == cross(p[:3], q[:3]), "line moment p x q")


def check_incidence(rng):
    """Joins and meets of points, lines, and planes in coordinates, each a
    polynomial identity checked on random rationals: the plane through three
    unitized points, a line meeting a plane, two planes meeting in a line,
    three planes meeting in a point, and the scalar meet of two lines."""
    def draw(k):
        return [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                for _ in range(k)]

    def dot3(a, b):
        return sum(x * y for x, y in zip(a, b))

    for _ in range(TRIALS):
        p, q, r = (point(*draw(3)) for _ in range(3))
        pc, qc, rc = (coeffs(x, POINT_NAMES) for x in (p, q, r))
        n = cross([qc[i] - pc[i] for i in range(3)],
                  [rc[i] - pc[i] for i in range(3)])
        check(coeffs(wedge(wedge(p, q), r), PLANE_NAMES) ==
              n + [-dot3(n, pc)],
              "the plane p ^ q ^ r of unitized points is (n, -n . p) with "
              "n = (q - p) x (r - p)")

        g, h, k = (draw(4) for _ in range(3))
        gv, hv, kv = (on_names(PLANE_NAMES, x) for x in (g, h, k))
        v = [qc[i] - pc[i] for i in range(3)]
        m = cross(pc[:3], qc[:3])
        meet = coeffs(antiwedge(wedge(p, q), gv), POINT_NAMES)
        check(meet == [a + g[3] * b for a, b in zip(cross(m, g[:3]), v)] +
              [-dot3(v, g[:3])],
              "the line p ^ q meets the plane g in (m x g_n + g_w v, "
              "-v . g_n)")
        line = coeffs(antiwedge(gv, hv), LINE_NAMES)
        check(line[:3] == cross(h[:3], g[:3]),
              "g v h has direction h_n x g_n")
        check(line[3:] == [h[3] * g[i] - g[3] * h[i] for i in range(3)],
              "g v h has moment h_w g_n - g_w h_n")
        three = coeffs(antiwedge(antiwedge(gv, hv), kv), POINT_NAMES)
        check(three == [determinant([g, h, k, [Fraction(int(i == j))
                                               for i in range(4)]])
                        for j in range(4)],
              "(g v h) v k is the point with coordinates det[g; h; k; e_j]")
        check(all(dot3(x, three) == 0 for x in (g, h, k)),
              "(g v h) v k lies on g, h, and k")
        a, b = (draw(4) for _ in range(2))
        check(antiwedge(wedge(p, q), wedge(point(*a), point(*b))) ==
              clean({0: determinant([pc, qc, a, b])}),
              "(p ^ q) v (a ^ b) = det[p; q; a; b]")


def check_translation(rng):
    """translation(d) is half of d on e23, e31, e12 plus the antiscalar. It
    moves points by d, leaves a direction (a vector of weight 0) and the
    direction of a line unchanged, and is unitized for every d."""
    for _ in range(TRIALS // 4):
        d = [Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for _ in range(3)]
        t = translator(d)
        check(antigp(t, antireverse(t)) == blade("e1234"),
              "a translator is unitized")
        p = [Fraction(rng.randint(-9, 9), rng.randint(1, 5)) for _ in range(3)]
        check(coeffs(sandwich(t, point(*p)), POINT_NAMES) ==
              [p[i] + d[i] for i in range(3)] + [1],
              "translation(d) moves points by d")
        direction = point(*p, w=0)
        check(sandwich(t, direction) == direction,
              "translation(d) leaves a direction unchanged")
        line = wedge(point(*p), random_point(rng))
        check(coeffs(sandwich(t, line), LINE_NAMES)[:3] ==
              coeffs(line, LINE_NAMES)[:3],
              "translation(d) leaves the direction of a line unchanged")
    moved = coeffs(sandwich(translator((2, 4, -6)), point(1, 2, 3)),
                   POINT_NAMES)
    check(moved == [3, 6, -3, 1],
          "translation (2, 4, -6) moves (1, 2, 3, 1) to (3, 6, -3, 1)")


def rotation_matrices(c2, s2):
    """The right-handed rotations by the angle with cosine c2 and sine s2
    about x, y, and z, which turn counterclockwise seen from the tip of the
    axis."""
    return {"e41": [[1, 0, 0], [0, c2, -s2], [0, s2, c2]],
            "e42": [[c2, 0, s2], [0, 1, 0], [-s2, 0, c2]],
            "e43": [[c2, -s2, 0], [s2, c2, 0], [0, 0, 1]]}


def check_rotation(rng):
    """rotation(l, theta) is l sin(theta/2) + antiscalar cos(theta/2). With
    s and c the sine and cosine of a rational half angle, it turns points about
    the x, y, and z axes as the right-handed rotation matrices whose cosine
    is c^2 - s^2 and sine 2sc, and about a line through a point a as
    a + R(p - a). The rotor is unitized."""
    for s, c in HALF_ANGLES:
        matrices = rotation_matrices(c * c - s * s, 2 * s * c)
        for axis, matrix in matrices.items():
            r = rotor(blade(axis), s, c)
            check(antigp(r, antireverse(r)) == blade("e1234"),
                  "a rotor of a unitized line is unitized")
            for _ in range(TRIALS // 50):
                p = [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                     for _ in range(3)]
                turned = coeffs(sandwich(r, point(*p)), POINT_NAMES)
                check(turned == [sum(matrix[i][j] * p[j] for j in range(3))
                                 for i in range(3)] + [1],
                      f"rotation about {axis} turns as the right-handed "
                      f"matrix")
        # About the line through a = (1, 2, 0) with direction z, so that
        # the line is a ^ (a + e3), with moment a x e3 = (2, -1, 0).
        a = [Fraction(1), Fraction(2), Fraction(0)]
        line = wedge(point(*a), point(a[0], a[1], a[2] + 1))
        check(coeffs(line, LINE_NAMES) == [0, 0, 1, 2, -1, 0],
              "the line through (1, 2, 0) along z")
        r = rotor(line, s, c)
        for _ in range(TRIALS // 50):
            p = [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                 for _ in range(3)]
            rel = [p[i] - a[i] for i in range(3)]
            turned = coeffs(sandwich(r, point(*p)), POINT_NAMES)
            check(turned == [a[i] + sum(matrices["e43"][i][j] * rel[j]
                                        for j in range(3))
                             for i in range(3)] + [1],
                  "rotation about a line off the origin turns about it")
    # The scaled rotor 3 e43 + 4 antiscalar, with lambda = 25, turns
    # (1, 0, 0, 1) into 25 (7/25, 24/25, 0, 1).
    check(coeffs(sandwich(add(blade("e43", 3), blade("e1234", 4)),
                          point(1, 0, 0)), POINT_NAMES) == [7, 24, 0, 25],
          "3 e43 + 4 antiscalar turns (1, 0, 0, 1) into (7, 24, 0, 25)")


def joint_motor(theta, d, a, alpha):
    """The motor of one joint, Rz(theta) Tz(d) Tx(a) Rx(alpha) as a product
    of matrices acting on column points, so Rx(alpha) applies first. Each
    angle is given as the sine and cosine of its half."""
    rz = rotor(blade("e43"), *theta)
    rx = rotor(blade("e41"), *alpha)
    return antigp(antigp(antigp(rz, translator((0, 0, d))),
                         translator((a, 0, 0))), rx)


def check_joint_chain(rng):
    """A chain of four joints, each Rz(theta) Tz(d) Tx(a) Rx(alpha) with
    rational half angles, composes to a unitized motor that moves lines and
    planes as the points that span them, and applying the joints one at a
    time from the last to the first equals one transform by the product."""
    for _ in range(TRIALS // 20):
        joints = [(rng.choice(HALF_ANGLES), Fraction(rng.randint(-6, 6)),
                   Fraction(rng.randint(-6, 6)), rng.choice(HALF_ANGLES))
                  for _ in range(4)]
        motors = [joint_motor(*j) for j in joints]
        chain = motors[0]
        for q in motors[1:]:
            chain = antigp(chain, q)
        check(antigp(chain, antireverse(chain)) == blade("e1234"),
              "a chain of unitized joints is unitized")
        check(set(chain) <= {parse(n)[0] for n in MOTOR_NAMES},
              "a chain of joints is on the motor layout")
        p, q, r = (random_point(rng) for _ in range(3))
        tp, tq, tr = (sandwich(chain, x) for x in (p, q, r))
        check(sandwich(chain, wedge(p, q)) == wedge(tp, tq),
              "a chain moves a line as the points that span it")
        check(sandwich(chain, wedge(wedge(p, q), r)) ==
              wedge(wedge(tp, tq), tr),
              "a chain moves a plane as the points that span it")
        stepwise = p
        for q_ in reversed(motors):
            stepwise = sandwich(q_, stepwise)
        check(stepwise == tp, "the joints apply from the last to the first")


def check_motor_and_flector_pages(rng):
    """The formulas of the Motor and Flector pages of Lengyel's rigid
    geometric algebra wiki (rigidgeometricalgebra.org), recomputed from the
    definitions: the layouts' geometric constraint; the motors built from two
    planes, two lines, and two points; the Motor page's transformation rows,
    factorization, and matrix; and the Flector page's factorization and
    matrix, and e4 as the inversion through the origin. The Flector page's
    rows for points, lines, and planes use the single-sign transform and are
    checked by check_versors and recorded as a disagreement, so they are not
    repeated here."""
    def draw(k):
        return [Fraction(rng.randint(-9, 9), rng.randint(1, 5))
                for _ in range(k)]

    def dot3(a, b):
        return sum(x * y for x, y in zip(a, b))

    def lin(*terms):
        return [sum(k * v[i] for k, v in terms) for i in range(3)]

    for _ in range(TRIALS // 4):
        planes = [on_names(PLANE_NAMES, draw(4)) for _ in range(3)]
        q = coeffs(antigp(planes[0], planes[1]), MOTOR_NAMES)
        f = coeffs(antigp(antigp(planes[0], planes[1]), planes[2]),
                   FLECTOR_NAMES)
        check(sum(q[i] * q[i + 4] for i in range(4)) == 0,
              "a motor meets the geometric constraint")
        check(sum(f[i] * f[i + 4] for i in range(4)) == 0,
              "a flector meets the geometric constraint")

        g, h = draw(4), draw(4)
        gx, gy, gz, gw = g
        hx, hy, hz, hw = h
        check(coeffs(antigp(on_names(PLANE_NAMES, h),
                            antireverse(on_names(PLANE_NAMES, g))),
                     MOTOR_NAMES) ==
              [gy * hz - gz * hy, gz * hx - gx * hz, gx * hy - gy * hx,
               gx * hx + gy * hy + gz * hz, hx * gw - gx * hw,
               hy * gw - gy * hw, hz * gw - gz * hw, 0],
              "Motor page: h v antireverse(g)")
        k = coeffs(wedge(random_point(rng), random_point(rng)), LINE_NAMES)
        l = coeffs(wedge(random_point(rng), random_point(rng)), LINE_NAMES)
        kvx, kvy, kvz, kmx, kmy, kmz = k
        lvx, lvy, lvz, lmx, lmy, lmz = l
        check(coeffs(antigp(on_names(LINE_NAMES, l),
                            antireverse(on_names(LINE_NAMES, k))),
                     MOTOR_NAMES) ==
              [lvz * kvy - lvy * kvz, lvx * kvz - lvz * kvx,
               lvy * kvx - lvx * kvy, lvx * kvx + lvy * kvy + lvz * kvz,
               lvz * kmy - lvy * kmz + lmz * kvy - lmy * kvz,
               lvx * kmz - lvz * kmx + lmx * kvz - lmz * kvx,
               lvy * kmx - lvx * kmy + lmy * kvx - lmx * kvy,
               lvx * kmx + lvy * kmy + lvz * kmz + lmx * kvx + lmy * kvy
               + lmz * kvz],
              "Motor page: l v antireverse(k)")
        p, p2 = draw(4), draw(4)
        check(coeffs(antigp(point(*p2), antireverse(point(*p))),
                     MOTOR_NAMES) ==
              [0, 0, 0, p[3] * p2[3]] +
              [p2[i] * p[3] - p[i] * p2[3] for i in range(3)] + [0],
              "Motor page: q v antireverse(p)")

        # A unitized motor with every component nonzero, and a unitized
        # flector made from it.
        s1, c1 = rng.choice(HALF_ANGLES)
        s2, c2 = rng.choice(HALF_ANGLES)
        motor = antigp(antigp(translator(draw(3)),
                              rotor(blade("e41"), s1, c1)),
                       rotor(blade("e42"), s2, c2))
        vx, vy, vz, vw, mx, my, mz, mw = coeffs(motor, MOTOR_NAMES)
        v, m = [vx, vy, vz], [mx, my, mz]
        x = draw(4)
        moved = coeffs(sandwich(motor, point(*x)), POINT_NAMES)
        a = lin((1, cross(v, x[:3])), (x[3], m))
        check(moved == lin((1, x[:3]), (2 * vw, a), (2, cross(v, a)),
                           (-2 * mw * x[3], v)) + [x[3]],
              "Motor page: the point row")
        lv, lm = l[:3], l[3:]
        a, b, c = cross(v, lv), cross(v, lm), cross(m, lv)
        bc = lin((1, b), (1, c))
        check(coeffs(sandwich(motor, on_names(LINE_NAMES, l)), LINE_NAMES) ==
              lin((1, lv), (2 * vw, a), (2, cross(v, a))) +
              lin((1, lm), (2 * mw, a), (2 * vw, bc), (2, cross(v, bc)),
                  (2, cross(m, a))),
              "Motor page: the line row")
        a = cross(v, g[:3])
        check(coeffs(sandwich(motor, on_names(PLANE_NAMES, g)), PLANE_NAMES) ==
              lin((1, g[:3]), (2 * vw, a), (2, cross(v, a))) +
              [gw + 2 * (dot3(lin((1, cross(m, g[:3])), (mw, g[:3])), v)
                         - vw * dot3(m, g[:3]))],
              "Motor page: the plane row")
        t = antigp(motor, antireverse(on_names(MOTOR_NAMES[:4],
                                               [vx, vy, vz, vw])))
        check(t == translator([2 * (vy * mz - vz * my + mx * vw - vx * mw),
                               2 * (vz * mx - vx * mz + my * vw - vy * mw),
                               2 * (vx * my - vy * mx + mz * vw - vz * mw)]),
              "Motor page: the factor T = Q v antireverse(R)")
        matrix = [[1 - 2 * (vy * vy + vz * vz), 2 * vx * vy - 2 * vz * vw,
                   2 * vz * vx + 2 * vy * vw,
                   2 * (vy * mz - vz * my) + 2 * (vw * mx - vx * mw)],
                  [2 * vx * vy + 2 * vz * vw, 1 - 2 * (vz * vz + vx * vx),
                   2 * vy * vz - 2 * vx * vw,
                   2 * (vz * mx - vx * mz) + 2 * (vw * my - vy * mw)],
                  [2 * vz * vx - 2 * vy * vw, 2 * vy * vz + 2 * vx * vw,
                   1 - 2 * (vx * vx + vy * vy),
                   2 * (vx * my - vy * mx) + 2 * (vw * mz - vz * mw)]]
        check(rigid_affine(motor) == matrix, "Motor page: the matrix A + B")

        flector = antigp(motor, blade("e412"))
        px, py, pz, pw, fx, fy, fz, fw = coeffs(flector, FLECTOR_NAMES)
        check(antigp(flector, antireverse(flector)) == blade("e1234"),
              "a motor times a unitized plane is a unitized flector")
        plane = on_names(PLANE_NAMES, [fx, fy, fz, fw])
        check(coeffs(antigp(flector, antireverse(plane)), MOTOR_NAMES) ==
              [-fx * pw, -fy * pw, -fz * pw, 1 - pw * pw, fy * pz - fz * py,
               fz * px - fx * pz, fx * py - fy * px, 0],
              "Flector page: the factor F v antireverse(g), unscaled")
        matrix = [[2 * (fy * fy + fz * fz) - 1, -2 * fx * fy + 2 * fz * pw,
                   -2 * fz * fx - 2 * fy * pw,
                   2 * (px * pw - fx * fw) + 2 * (fy * pz - fz * py)],
                  [-2 * fx * fy - 2 * fz * pw, 2 * (fz * fz + fx * fx) - 1,
                   -2 * fy * fz + 2 * fx * pw,
                   2 * (py * pw - fy * fw) + 2 * (fz * px - fx * pz)],
                  [-2 * fz * fx + 2 * fy * pw, -2 * fy * fz - 2 * fx * pw,
                   2 * (fx * fx + fy * fy) - 1,
                   2 * (pz * pw - fz * fw) + 2 * (fx * py - fy * px)]]
        check(rigid_affine(flector) == matrix,
              "Flector page: the matrix A + B, which keeps the weight")
    check(rigid_affine(blade("e4")) ==
          [[-1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0]],
          "Flector page: e4 is the inversion through the origin")


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
    check_bulk_weight_diagonal()
    check_antiprojection(rng)
    signs = check_odd_sign(rng)
    check_motor_flector_layouts(rng)
    check_line_moment(rng)
    check_incidence(rng)
    check_translation(rng)
    check_rotation(rng)
    check_joint_chain(rng)
    check_motor_and_flector_pages(rng)
    print("sign s of a mirror V acting as s V v x^ v antireverse(V), found "
          "by the outermorphism property: " +
          ", ".join(f"{name} (n = {len(ODD_SIGN_FORMS[name])}) "
                    f"{'+' if s == 1 else '-' if s == -1 else '?'}1"
                    for name, s in signs.items()))
    if failures:
        print("FAILED (seed %d):" % SEED)
        for what in sorted(set(failures)):
            print("  " + what)
        return 1
    print("all convention checks passed (seed %d)" % SEED)
    return 0


if __name__ == "__main__":
    sys.exit(main())
