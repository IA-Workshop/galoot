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
