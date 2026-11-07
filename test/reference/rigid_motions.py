"""Rigid motions and incidences of three-dimensional space by plain linear
algebra over fractions: 3 by 3 and 4 by 4 matrices, cross products, and
determinants.

This is the second route to the rigid golden values. It knows nothing of
geometric algebra and imports nothing from the other reference scripts: a
point is a homogeneous column (x, y, z, w), a plane a row (g_x, g_y, g_z,
g_w) that vanishes on the points it contains, a line a pair of a direction
and a moment, and a motion a matrix. golden.py compares each value with the
one the geometric-algebra route of conventions_check.py gives, and writes it
only if the two agree.

Every function is exact, and each one checks what it assumes (a unit axis, an
orthogonal matrix, a cosine and sine on the unit circle) and raises
ValueError otherwise. Run it with `python3 -I test/reference/rigid_motions.py`
to check its own constructions; it prints one line and exits nonzero on a
failure.

The two routes are not independent in every respect. The overall signs of
line_meets_plane, planes_meet, and three_planes_meet were chosen to match the
library's convention, not derived from it, and planes_meet checks itself only
up to scale. So the agreement of the two routes confirms these meets only up
to sign: a sign error shared by both would go unnoticed. The incidence zeros
do not depend on these signs. The orientation of plane_through is the
exception: self_check pins it to counterclockwise points about +z giving the
normal +z.
"""

import itertools
import math
import sys
from fractions import Fraction


def frac_matrix(rows):
    return tuple(tuple(Fraction(x) for x in row) for row in rows)


def identity(n):
    return frac_matrix([[int(i == j) for j in range(n)] for i in range(n)])


def mat_mul(a, b):
    return tuple(tuple(sum(a[i][k] * b[k][j] for k in range(len(b)))
                       for j in range(len(b[0]))) for i in range(len(a)))


def mat_vec(a, v):
    return tuple(sum(a[i][k] * v[k] for k in range(len(v)))
                 for i in range(len(a)))


def transpose(a):
    return tuple(zip(*a))


def scale(k, a):
    return tuple(tuple(k * x for x in row) for row in a)


def mat_add(*ms):
    return tuple(tuple(sum(m[i][j] for m in ms) for j in range(len(ms[0][0])))
                 for i in range(len(ms[0])))


def determinant(m):
    """By the Leibniz formula: the sum over permutations of the signed
    products of one entry from each row."""
    n = len(m)
    total = Fraction(0)
    for perm in itertools.permutations(range(n)):
        inversions = sum(1 for i, j in itertools.combinations(perm, 2)
                         if i > j)
        term = Fraction(-1 if inversions & 1 else 1)
        for row, col in enumerate(perm):
            term *= m[row][col]
        total += term
    return total


def rank(rows):
    """The rank of a matrix, by Gaussian elimination over fractions."""
    work = [list(map(Fraction, row)) for row in rows]
    r = 0
    for col in range(len(work[0]) if work else 0):
        pivot = next((i for i in range(r, len(work)) if work[i][col]), None)
        if pivot is None:
            continue
        work[r], work[pivot] = work[pivot], work[r]
        for i in range(len(work)):
            if i != r and work[i][col]:
                f = work[i][col] / work[r][col]
                work[i] = [x - f * y for x, y in zip(work[i], work[r])]
        r += 1
    return r


def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0])


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def skew(u):
    """The matrix [u]x with [u]x v = u x v."""
    return frac_matrix([[0, -u[2], u[1]], [u[2], 0, -u[0]], [-u[1], u[0], 0]])


def require(condition, what):
    if not condition:
        raise ValueError(what)


def require_orthogonal(r, det):
    require(mat_mul(transpose(r), r) == identity(3), "R^T R = I")
    require(determinant(r) == det, f"det R = {det}")


# 3 by 3 orthogonal matrices.

def rotation(axis, cos, w):
    """The rotation by the angle theta about the direction axis,
    counterclockwise seen from the tip of axis, by Rodrigues' formula
    R = cos I + sin [u]x + (1 - cos) u u^T with u = axis / |axis|.

    The axis need not have unit length, so that an axis such as (1, 1, 1),
    whose length is irrational, still gives a rational matrix: the caller
    passes cos = cos(theta) and w = sin(theta) / |axis|, and R is
    cos I + w [axis]x + (1 - cos) / |axis|^2 axis axis^T."""
    axis = tuple(map(Fraction, axis))
    cos, w = Fraction(cos), Fraction(w)
    norm2 = dot(axis, axis)
    require(norm2 != 0, "a rotation axis is not zero")
    require(cos * cos + w * w * norm2 == 1, "cos^2 + sin^2 = 1")
    outer = frac_matrix([[a * b for b in axis] for a in axis])
    r = mat_add(scale(cos, identity(3)), scale(w, skew(axis)),
                scale((1 - cos) / norm2, outer))
    require_orthogonal(r, 1)
    return r


def mirror(normal):
    """The reflection in the plane through the origin with this normal,
    I - 2 n n^T / |n|^2."""
    n = tuple(map(Fraction, normal))
    norm2 = dot(n, n)
    require(norm2 != 0, "a mirror normal is not zero")
    r = mat_add(identity(3),
                scale(Fraction(-2) / norm2,
                      frac_matrix([[a * b for b in n] for a in n])))
    require_orthogonal(r, -1)
    return r


INVERSION = scale(Fraction(-1), identity(3))

# cos(2 pi / n) and sin^2(2 pi / n) for the rotations a crystal allows.
CRYSTAL_TURNS = {1: (Fraction(1), Fraction(0)), 2: (Fraction(-1), Fraction(0)),
                 3: (Fraction(-1, 2), Fraction(3, 4)),
                 4: (Fraction(0), Fraction(1)),
                 6: (Fraction(1, 2), Fraction(3, 4))}


def exact_sqrt(q):
    """The square root of a fraction that is the square of one."""
    q = Fraction(q)
    require(q >= 0, "a square is not negative")
    root = Fraction(math.isqrt(q.numerator), math.isqrt(q.denominator))
    require(root * root == q, f"{q} is the square of a fraction")
    return root


def crystal_rotation(axis, n):
    """The rotation by 2 pi / n about axis, for n in 1, 2, 3, 4, 6. It is
    rational only when sin(2 pi / n) / |axis| is, which for n = 3 and 6 needs
    an axis such as (1, 1, 1)."""
    cos, sin2 = CRYSTAL_TURNS[n]
    return rotation(axis, cos, exact_sqrt(sin2 / dot(axis, axis)))


def generator_matrix(generator):
    """The matrix of one generator of a point group: ("rotation", axis, n),
    ("rotoinversion", axis, n), the rotation followed by the inversion,
    ("mirror", normal), or ("inversion",)."""
    kind = generator[0]
    if kind == "rotation":
        return crystal_rotation(generator[1], generator[2])
    if kind == "rotoinversion":
        return mat_mul(INVERSION, crystal_rotation(generator[1], generator[2]))
    if kind == "mirror":
        return mirror(generator[1])
    if kind == "inversion":
        return INVERSION
    raise ValueError(f"unknown generator {kind}")


def matrix_group(generators):
    """Every product of the generator matrices, with the identity, as a
    sorted list. Each element is checked to be orthogonal."""
    gens = [generator_matrix(g) for g in generators]
    found = {identity(3)}
    frontier = [identity(3)]
    while frontier:
        new = []
        for x in frontier:
            for g in gens:
                y = mat_mul(x, g)
                if y not in found:
                    found.add(y)
                    new.append(y)
        frontier = new
    for x in found:
        require(mat_mul(transpose(x), x) == identity(3), "R^T R = I")
    return sorted(found)


def group_properties(elements):
    """What a table of the point groups states of each: its order, whether
    it is abelian and cyclic, whether it contains the inversion
    (centrosymmetric), whether every element is a proper rotation
    (enantiomorphic), and whether a nonzero direction is fixed by every
    element (polar)."""
    order = len(elements)
    abelian = all(mat_mul(a, b) == mat_mul(b, a)
                  for a, b in itertools.combinations(elements, 2))

    def element_order(x):
        k, y = 1, x
        while y != identity(3):
            y, k = mat_mul(y, x), k + 1
        return k

    cyclic = any(element_order(x) == order for x in elements)
    centro = INVERSION in elements
    proper = all(determinant(x) == 1 for x in elements)
    fixed = [tuple(x[i][j] - int(i == j) for j in range(3))
             for x in elements for i in range(3)]
    polar = rank(fixed) < 3
    return {"order": order, "abelian": abelian, "cyclic": cyclic,
            "centrosymmetric": centro, "enantiomorphic": proper,
            "polar": polar}


# 4 by 4 homogeneous matrices, acting on columns (x, y, z, w).

def homogeneous(r, t):
    """[[R, t], [0, 1]]."""
    return tuple(tuple(r[i]) + (Fraction(t[i]),) for i in range(3)) + \
        ((Fraction(0),) * 3 + (Fraction(1),),)


def translation(t):
    return homogeneous(identity(3), t)


def rotation_x(cos, sin):
    require(cos * cos + sin * sin == 1, "cos^2 + sin^2 = 1")
    return homogeneous(frac_matrix([[1, 0, 0], [0, cos, -sin], [0, sin, cos]]),
                       (0, 0, 0))


def rotation_z(cos, sin):
    require(cos * cos + sin * sin == 1, "cos^2 + sin^2 = 1")
    return homogeneous(frac_matrix([[cos, -sin, 0], [sin, cos, 0], [0, 0, 1]]),
                       (0, 0, 0))


def joint(cos_theta, sin_theta, d, a, cos_alpha, sin_alpha):
    """Rz(theta) Tz(d) Tx(a) Rx(alpha), compared with the closed form of
    that product."""
    m = mat_mul(mat_mul(mat_mul(rotation_z(cos_theta, sin_theta),
                                translation((0, 0, d))),
                        translation((a, 0, 0))),
                rotation_x(cos_alpha, sin_alpha))
    ct, st, ca, sa = cos_theta, sin_theta, cos_alpha, sin_alpha
    closed = frac_matrix([[ct, -st * ca, st * sa, a * ct],
                          [st, ct * ca, -ct * sa, a * st],
                          [0, sa, ca, d],
                          [0, 0, 0, 1]])
    require(m == closed, "Rz Tz Tx Rx equals its closed form")
    return m


def chain(joints):
    """The product of the joint matrices, the first joint leftmost."""
    m = identity(4)
    for j in joints:
        m = mat_mul(m, joint(*j))
    return m


def rotation_about_line(point, direction, cos, sin):
    """The rotation by theta about the line through point with the unit
    direction, counterclockwise seen from the tip of the direction:
    x goes to point + R (x - point)."""
    direction = tuple(map(Fraction, direction))
    require(dot(direction, direction) == 1, "the direction is a unit vector")
    r = rotation(direction, cos, sin)
    p = tuple(map(Fraction, point))
    rp = mat_vec(r, p)
    return homogeneous(r, tuple(p[i] - rp[i] for i in range(3)))


# Points, lines, and planes.

def line_through(p, q):
    """The line through the homogeneous points p and q, by its Pluecker
    coordinates: the direction w_p q - w_q p and the moment p x q of the
    first three coordinates."""
    direction = tuple(p[3] * q[i] - q[3] * p[i] for i in range(3))
    return direction + cross(p[:3], q[:3])


def plane_through(p, q, r):
    """The plane through three points: the linear form that takes the value
    det[x; p; q; r] at a point x, whose coefficients are the cofactors
    det[e_j; p; q; r]."""
    return tuple(determinant([[int(i == j) for i in range(4)], p, q, r])
                 for j in range(4))


def line_meets_plane(p, q, g):
    """The point where the line through p and q meets the plane g,
    (g . p) q - (g . q) p, which g takes to zero."""
    gp, gq = dot(g, p), dot(g, q)
    x = tuple(gp * q[i] - gq * p[i] for i in range(4))
    require(dot(g, x) == 0, "the meet lies on the plane")
    return x


def planes_meet(g, h):
    """The line where the planes g and h meet, with direction h_n x g_n and
    moment h_w g_n - g_w h_n, where g_n is the normal and g_w the offset.
    Checked against the line through two points found on both planes: its
    coordinates are a multiple of these."""
    gn, hn = g[:3], h[:3]
    direction = cross(hn, gn)
    moment = tuple(h[3] * gn[i] - g[3] * hn[i] for i in range(3))
    line = direction + moment
    if any(direction):
        # A point on both planes: the one where the third plane through the
        # origin with normal `direction` meets them.
        x = three_planes_meet(g, h, direction + (Fraction(0),))
        y = tuple(x[i] + x[3] * (direction + (Fraction(0),))[i]
                  for i in range(4))
        require(dot(g, y) == 0 and dot(h, y) == 0,
                "a second point on both planes")
        through = line_through(x, y)
        require(rank([line, through]) == 1, "the line lies on both planes")
    return line


def three_planes_meet(g, h, k):
    """The point where three planes meet, with coordinates
    det[g; h; k; e_j], so that every plane e takes the value det[g; h; k; e]
    at it, which is zero for g, h, and k."""
    x = tuple(determinant([g, h, k, [int(i == j) for i in range(4)]])
              for j in range(4))
    for plane in (g, h, k):
        require(dot(plane, x) == 0, "the meet lies on each plane")
    return x


def lines_meet(p, q, a, b):
    """The scalar meet of the lines through p, q and through a, b,
    det[p; q; a; b], which is zero exactly when the four points are
    coplanar."""
    return determinant([p, q, a, b])


def self_check():
    """The constructions above against cases worked by hand."""
    quarter = rotation((0, 0, 1), 0, 1)
    require(mat_vec(quarter, (1, 0, 0)) == (0, 1, 0),
            "a quarter turn about z takes x to y")
    third = crystal_rotation((1, 1, 1), 3)
    require(mat_vec(third, (1, 0, 0)) == (0, 1, 0),
            "a third of a turn about (1, 1, 1) takes x to y")
    require(mat_vec(mirror((0, 0, 1)), (1, 2, 3)) == (1, 2, -3),
            "the mirror z = 0")
    plane = plane_through((0, 0, 0, 1), (1, 0, 0, 1), (0, 1, 0, 1))
    require(plane == (0, 0, 1, 0),
            "counterclockwise points about +z give the normal +z")
    line = line_through((1, 2, 3, 1), (2, 2, 5, 1))
    require(line[:3] == (1, 0, 2), "direction q - p")
    turn = rotation_about_line((1, 2, 0), (0, 0, 1), 0, 1)
    require(mat_vec(turn, (2, 2, 0, 1)) == (1, 3, 0, 1),
            "a quarter turn about the line through (1, 2, 0) along z")
    require(len(matrix_group([("rotation", (0, 0, 1), 4),
                              ("mirror", (0, 0, 1)),
                              ("mirror", (1, 0, 0))])) == 16,
            "4/mmm has order 16")


def main():
    try:
        self_check()
    except ValueError as e:
        print(f"rigid_motions.py: a self-check failed: {e}")
        return 1
    print("rigid_motions.py: every self-check passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
