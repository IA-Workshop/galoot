"""Writes test/reference/golden.hpp: exact values that tests compare the
library with, each derived here from its definition.

The values come from the three reference scripts beside this one, which
share no code with the library: clifford_reference.py multiplies in any
symmetric integer form by diagonalizing it over fractions,
conventions_check.py holds the oriented blade names and the rigid(3)
complement table of Lengyel's rigid geometric algebra
(rigidgeometricalgebra.org) and computes in rigid(3) by geometric algebra,
and rigid_motions.py computes rigid motions and incidences by matrices,
cross products, and determinants, with no geometric algebra. Where a
convention is stated both as a definition and as a closed formula, a table,
or a worked example, the script computes both and stops if they differ: the
complements, grade signs, oriented names, antiscalars, wedge-basis products,
conformal change of basis, result order, and metric exomorphisms are written
only when two routes agree. The rigid joins, meets, incidences, rotations,
chains of joints, and point groups are each computed by
conventions_check.py and by rigid_motions.py and written only when the two
agree exactly; the orders of the point groups must also equal the published
ones. The diagonal and rigid signatures have no second route; their squares
are written from the definition as clifford_reference.py states it.

Usage, with the interpreter isolated as for every reference script:

  python3 -I test/reference/golden.py > test/reference/golden.hpp
      Regenerate the committed header.
  python3 -I test/reference/golden.py --check test/reference/golden.hpp
      Exit 1, printing a unified diff, if the file differs in any byte from
      what the script writes now; exit 0 if it is the same.

A blade is a bitmask in which bit i - 1 stands for generator e_i. The
per-blade tables of names and of the result order are written for n = 4 and
n = 5: the sizes of rigid(3) and conformal(3), and one even and one odd n,
which the left complement tells apart. The complements and the grade signs
are also written for n = 3, the size of the other two forms whose product
tables are written, so that a product of complements in every one of those
forms can be composed from golden values alone.
"""

import difflib
import itertools
import os
import random
import re
import sys
import textwrap
from fractions import Fraction

# Isolated mode leaves the script's own directory off sys.path, and the two
# reference scripts live there.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import clifford_reference as ref
import conventions_check as conv
import rigid_motions as rm

BLADE_TABLE_SIZES = (4, 5)
# The complement and grade-sign tables cover the size of every form of
# GP_TABLE_FORMS as well.
SIGN_TABLE_SIZES = (3, 4, 5)
MAX_N = 6  # largest n of the signature tables

# The forms whose full product tables are written out, by the name of their
# golden values: a diagonal degenerate form, two non-diagonal ones with two
# null generators, of an even and an odd number of generators, a degenerate
# non-diagonal one whose first pivot is zero, and a dense one with terms of up
# to four blades and coefficients up to 31.
GP_TABLE_FORMS = (
    ("rigid_3", "rigid(3)", ref.rigid(3)),
    ("conformal_2", "conformal(2)", ref.conformal(2)),
    ("conformal_3", "conformal(3)", ref.conformal(3)),
    ("degenerate_3", "the degenerate form", [[0, 1, 0], [1, 0, 0], [0, 0, 0]]),
    ("random_1", "seeded random form 1", ref.random_forms()[1]),
)

# The worked example of the result order: the bivectors of n = 4.
ORDER_EXAMPLE = ("e12", "e13", "e14", "e23", "e24", "e34")

# Lengyel's oriented names in rigid(3): a name lists generators in the order
# of their product, so e31 = e3 e1 = -e13.
RIGID_3_NAMES = ("1", "e1", "e2", "e3", "e4", "e23", "e31", "e12", "e41",
                 "e42", "e43", "e423", "e431", "e412", "e321", "e1234")


class Disagreement(Exception):
    """Two routes to one value gave different answers."""


def agree(a, b, what):
    if a != b:
        raise Disagreement(f"{what}: {a!r} != {b!r}")


def unit(mask):
    return {mask: Fraction(1)}


def single_term(x, what):
    """The (mask, sign) of a multivector that is +1 or -1 times one blade."""
    if len(x) != 1:
        raise Disagreement(f"{what}: {x!r} is not one signed blade")
    (mask, c), = x.items()
    if c not in (1, -1):
        raise Disagreement(f"{what}: coefficient {c} is not +1 or -1")
    return mask, int(c)


def ordered_product(alg, generators):
    """e_g1 e_g2 ... e_gk, multiplied left to right, for 0-based g."""
    x = unit(0)
    for g in generators:
        x = alg.mul(x, unit(1 << g))
    return x


def complements(n):
    """Right and left complements of every blade, from their definitions:
    u ^ right(u) = antiscalar and left(u) ^ u = antiscalar.

    The wedge of two blades with no common generator equals their geometric
    product in any diagonal form, so it is taken as the product in
    R(n, 0, 0). Each complement is the signed blade on the other generators
    that makes the wedge the antiscalar.
    """
    alg = ref.Algebra(ref.r_pqr(n, 0, 0))
    full = (1 << n) - 1
    right, left = {}, {}
    for u in range(1 << n):
        other = full ^ u
        r_mask, r_sign = single_term(alg.mul(unit(u), unit(other)), "wedge")
        l_mask, l_sign = single_term(alg.mul(unit(other), unit(u)), "wedge")
        agree((r_mask, l_mask), (full, full), f"complement mask of {u}")
        # sign * (u ^ other) = antiscalar, and 1 / sign = sign.
        right[u] = (other, r_sign)
        left[u] = (other, l_sign)

    # Two consequences of the definitions: left(u) = (-1)^(gr ag) right(u),
    # and left(right(u)) = u.
    for u in range(1 << n):
        gr = ref.popcount(u)
        agree(left[u][1], right[u][1] * (-1) ** (gr * (n - gr)),
              f"left = (-1)^(gr ag) right, blade {u}, n = {n}")
        mask, sign = right[u]
        agree((left[mask][0], sign * left[mask][1]), (u, 1),
              f"left(right(u)) = u, blade {u}, n = {n}")
    return right, left


def check_complements_against_rigid_3_table(right, left):
    """The n = 4 complements against conventions_check.py, both its
    functions and its table in Lengyel's oriented names."""
    for u in range(16):
        agree(conv.right(unit(u)), {right[u][0]: right[u][1]},
              f"right complement of {u} against conventions_check.py")
        agree(conv.left(unit(u)), {left[u][0]: left[u][1]},
              f"left complement of {u} against conventions_check.py")
    before = len(conv.failures)
    conv.check_complements()
    agree(conv.failures[before:], [], "conventions_check.check_complements")


def grade_signs(n, right, left):
    """The signs that the reverse, antireverse, and grade involution give
    each blade.

    The reverse of e_a1...e_ak is the product e_ak...e_a1. The antireverse
    is the reverse conjugated by the complement, left(reverse(right(u))).
    The grade involution negates every generator. Each is compared with its
    closed form: (-1)^(k(k-1)/2) for grade k, the same with the antigrade
    n - k, and (-1)^k.
    """
    alg = ref.Algebra(ref.r_pqr(n, 0, 0))
    signs = {}
    for u in range(1 << n):
        idx = ref.indices(u)
        gr, ag = len(idx), n - len(idx)

        mask, reverse = single_term(ordered_product(alg, idx[::-1]), "reverse")
        agree(mask, u, "reverse keeps the blade")
        agree(reverse, (-1) ** (gr * (gr - 1) // 2), f"reverse of {u}")

        r_mask, r_sign = right[u]
        rev_r = single_term(ordered_product(alg, ref.indices(r_mask)[::-1]),
                            "reverse")[1]
        l_mask, l_sign = left[r_mask]
        agree(l_mask, u, "left(right(u)) keeps the blade")
        antireverse = r_sign * rev_r * l_sign
        agree(antireverse, (-1) ** (ag * (ag - 1) // 2),
              f"antireverse of {u}")

        negated = unit(0)
        for g in idx:
            negated = alg.mul(negated, {1 << g: Fraction(-1)})
        mask, involution = single_term(negated, "grade involution")
        agree(mask, u, "grade involution keeps the blade")
        agree(involution, (-1) ** gr, f"grade involution of {u}")

        signs[u] = (reverse, antireverse, involution)
    return signs


def result_order(n):
    """Blades ordered by grade, then lexicographically by index list. The
    bivectors of n = 4 are compared with the worked example of the
    convention, ORDER_EXAMPLE."""
    order = sorted(range(1 << n),
                   key=lambda m: (ref.popcount(m), ref.indices(m)))
    if n == 4:
        bivectors = [ref.blade_name(m) for m in order if ref.popcount(m) == 2]
        agree(bivectors, list(ORDER_EXAMPLE), "order of the bivectors of n = 4")
    return order


def canonical_names(n):
    """The canonical name of a mask lists its indices in increasing order, so
    the mask {1, 2, 4} is e124. Compared with conventions_check.parse, which
    reads a name back into a mask and the sign of its order."""
    names = {m: ref.blade_name(m) for m in range(1 << n)}
    for m, name in names.items():
        if m:
            agree(conv.parse(name), (m, 1), f"canonical name {name}")
    return names


def oriented_names():
    """Each oriented name in rigid(3) as a mask and a sign. The list of names
    is compared with the names of conventions_check.py's complement table.
    The sign from conventions_check.parse, the parity of the order written,
    is compared with the product of the generators in that order in
    R(4, 0, 0)."""
    agree(list(RIGID_3_NAMES), list(conv.COMPLEMENT_TABLE),
          "oriented names against conventions_check.py")
    alg = ref.Algebra(ref.r_pqr(4, 0, 0))
    out = []
    for name in RIGID_3_NAMES:
        mask, sign = conv.parse(name)
        if name != "1":
            written = [int(ch) - 1 for ch in name[1:]]
            agree(single_term(ordered_product(alg, written), name),
                  (mask, sign), f"oriented name {name}")
        out.append((name, mask, sign))
    return out


def wedge_basis_products():
    """In a metric that is not diagonal, the blade of a mask is the wedge of
    its generators, not their geometric product: in conformal(3),
    e45 = e4 ^ e5 and so e4 e5 = -1 + e45. Every product of e4, e5, and
    e45, from clifford_reference.py, whose whole conformal(3) table is first
    validated by its defining relations, associativity, and wedge-basis
    checks."""
    alg = ref.Algebra(ref.conformal(3))
    failures = ref.validate(alg, alg.table())
    agree(failures, [], "clifford_reference.validate on conformal(3)")
    e4, e5 = 1 << 3, 1 << 4
    blades = (e4, e5, e4 | e5)
    products = []
    for a, b in itertools.product(blades, repeat=2):
        products.append((a, b, canonical_terms(alg.mul(unit(a), unit(b)))))
    agree(products[1][2], [(-1, 0), (1, e4 | e5)], "e4 e5 = -1 + e45")
    return products


def canonical_terms(x):
    """The terms of a multivector as (coefficient, mask), ordered by grade
    and then by index list, each coefficient checked to be an integer."""
    terms = sorted(x.items(),
                   key=lambda kv: (ref.popcount(kv[0]), ref.indices(kv[0])))
    for _, c in terms:
        agree(c.denominator, 1, "integer coefficient")
    return [(int(c), m) for m, c in terms]


def gp_table(name, g):
    """Every product e_A e_B of two blades in the form g, from
    clifford_reference.py, with det g. The table is first validated by
    clifford_reference.validate: the defining relations, associativity on
    every triple of blades, and the wedge-monomial check. The pseudoscalar
    square is then compared with (-1)^(n(n-1)/2) det g, with det g computed
    by elimination, a second route to that one entry."""
    alg = ref.Algebra(g)
    table = alg.table()
    agree(ref.validate(alg, table), [],
          f"clifford_reference.validate on {name}")
    n = alg.n
    det = ref.det(g)
    agree(det.denominator, 1, f"det g of {name} is an integer")
    full = (1 << n) - 1
    expected = {0: (-1) ** (n * (n - 1) // 2) * det} if det != 0 else {}
    agree(table[(full, full)], expected,
          f"pseudoscalar square of {name} against (-1)^(n(n-1)/2) det g")
    products = [(a, b, canonical_terms(table[(a, b)]))
                for a in range(1 << n) for b in range(1 << n)]
    return n, int(det), products


def gram_minor(g, rows, cols):
    """det g[A, B]: the determinant of the rows A and columns B of g, with
    clifford_reference.det; the empty minor is 1."""
    return ref.det([[g[r][c] for c in cols] for r in rows])


def exomorphism(name, g, n, right, left, signs):
    """The metric exomorphism G of the form g on every blade: G e_A is the
    sum over the blades B of the grade of A of det g[A, B] e_B, which on
    vectors is G e_i = sum_j g_ij e_j and on a wedge of vectors is the wedge
    of their images.

    A second route gives every entry: the scalar part of e_A reverse(e_B),
    from clifford_reference.py's product table, is the Gram determinant
    det g[A, B] when A and B have one grade and zero across grades. The
    script stops if the two differ anywhere. Then, with the antiexomorphism
    composed as left(G right(u)) from this script's complements, both G of
    the antiexomorphism and the antiexomorphism of G are det g times the
    identity on every blade."""
    alg = ref.Algebra(g)
    table = alg.table()
    size = 1 << n
    images = {}
    for a in range(size):
        rows = ref.indices(a)
        image = {}
        for cols in itertools.combinations(range(n), len(rows)):
            minor = gram_minor(g, rows, cols)
            if minor != 0:
                image[sum(1 << c for c in cols)] = minor
        images[a] = image
        for b in range(size):
            scalar_part = table[(a, b)].get(0, Fraction(0)) * signs[b][0]
            agree(scalar_part, image.get(b, Fraction(0)),
                  f"G of {ref.blade_name(a)} on {ref.blade_name(b)} in "
                  f"{name}: Gram minor against the scalar part of e_A ~e_B")

    def apply(images_of, x):
        out = {}
        for m, c in x.items():
            for t, d in images_of[m].items():
                out[t] = out.get(t, 0) + c * d
        return ref.clean(out)

    def complement(table_of, x):
        return {table_of[m][0]: c * table_of[m][1] for m, c in x.items()}

    anti = {a: complement(left, apply(images, complement(right, unit(a))))
            for a in range(size)}
    det = ref.det(g)
    for a in range(size):
        identity = ref.clean({a: det})
        agree(apply(images, anti[a]), identity,
              f"G of the antiexomorphism of {ref.blade_name(a)} in {name}")
        agree(apply(anti, images[a]), identity,
              f"the antiexomorphism of G of {ref.blade_name(a)} in {name}")
    return [(a, canonical_terms(images[a])) for a in range(size)]


def image_row(a, terms, n, width):
    padded = terms + [(0, 0)] * (width - len(terms))
    body = ", ".join(f"{{{c}, {mask_literal(m, n)}}}" for c, m in padded)
    return f"{{{mask_literal(a, n)}, {len(terms)}, {{{{{body}}}}}}}"


def product_row(a, b, terms, n, width):
    padded = terms + [(0, 0)] * (width - len(terms))
    body = ", ".join(f"{{{c}, {mask_literal(m, n)}}}" for c, m in padded)
    return (f"{{{mask_literal(a, n)}, {mask_literal(b, n)}, {len(terms)}, "
            f"{{{{{body}}}}}}}")


def diagonal_signatures(n):
    """Every R(p, q, r) with p + q + r = n: the first p generators square to
    +1, the next q to -1, and the last r to 0, from clifford_reference.py.
    One route only."""
    out = []
    for p in range(n, -1, -1):
        for q in range(n - p, -1, -1):
            r = n - p - q
            g = ref.r_pqr(p, q, r)
            out.append((p, q, r, [g[i][i] for i in range(n)]))
    return out


def rigid_squares(d):
    """rigid(d) is R(d, 0, 1): e1 to ed Euclidean, e(d+1) null, from
    clifford_reference.py. One route only."""
    g = ref.rigid(d)
    return [g[i][i] for i in range(d + 1)]


def conformal_signature(d):
    """The Gram matrix of conformal(d) and its change of basis to
    R(d+1, 1, 0).

    In conformal(d), e1 to ed are Euclidean, e(d+1) is the origin and
    e(d+2) the point at infinity, both null, with e(d+1) . e(d+2) = -1. Row
    i of the change of basis P gives e_i in the generators of R(d+1, 1, 0):
    e_i itself for i <= d, origin = (e(d+2) - e(d+1)) / 2, and
    infinity = e(d+2) + e(d+1). P must carry the diagonal form D to the
    conformal one, P D P^T = G.
    """
    n = d + 2
    g = ref.conformal(d)
    for i in range(n):
        for j in range(n):
            expect = (1 if i == j and i < d else
                      -1 if {i, j} == {d, d + 1} else 0)
            agree(g[i][j], expect, f"conformal({d}) entry {i + 1},{j + 1}")

    half = Fraction(1, 2)
    p = [[Fraction(int(i == j)) for j in range(n)] for i in range(d)]
    p.append([Fraction(0)] * d + [-half, half])
    p.append([Fraction(0)] * d + [Fraction(1), Fraction(1)])
    diag = ref.r_pqr(d + 1, 1, 0)
    carried = ref.mat_mul(ref.mat_mul(p, diag), ref.transpose(p))
    agree(carried, [[Fraction(x) for x in row] for row in g],
          f"P D P^T = G for conformal({d})")
    return g, p, [diag[i][i] for i in range(n)]


def check_conformal_basis_against_conventions_check():
    before = len(conv.failures)
    conv.check_conformal_basis()
    agree(conv.failures[before:], [], "conventions_check.check_conformal_basis")


def antiscalar(n):
    """The antiscalar is e12...n with sign +1: the product of all the
    generators in increasing order."""
    full = (1 << n) - 1
    alg = ref.Algebra(ref.r_pqr(n, 0, 0))
    agree(single_term(ordered_product(alg, range(n)), "antiscalar"),
          (full, 1), f"antiscalar for n = {n}")
    return full, ref.blade_name(full)


def mask_literal(mask, n):
    return f"0b{mask:0{n}b}"


def rational_literal(q):
    return f"{{{q.numerator}, {q.denominator}}}"


def declare(out, type_name, name, init):
    """A constexpr variable, with its name on a second line if one line would
    pass 80 columns."""
    line = f"inline constexpr {type_name} {name} = {init}"
    if len(line) <= 80:
        out.append(line)
    else:
        out.append(f"inline constexpr {type_name}")
        out.append(f"    {name} = {init}")


def emit_array(out, type_name, name, rows):
    declare(out, f"std::array<{type_name}, {len(rows)}>", name, "{{")
    out.extend(f"    {row}," for row in rows)
    out.append("}};")


NO_BREAK = "\N{NO-BREAK SPACE}"


def open_namespace(out, name, fact, text):
    """Opens a namespace of golden values with a comment that describes them
    and, on its last line, states the fact they pin. Tests that use the
    namespace state the same fact in their reference markers."""
    if len(fact) > 71:
        raise ValueError(f"fact of {name} is longer than one comment line")
    # No line break inside a signature such as R(d+1, 1, 0): its spaces are
    # held as no-break spaces while the text is wrapped.
    held = re.sub(r"\([^()]*\)", lambda m: m.group(0).replace(" ", NO_BREAK),
                  text)
    out.append("")
    out.extend(line.replace(NO_BREAK, " ")
               for line in textwrap.wrap(held, width=77, initial_indent="// ",
                                         subsequent_indent="// ",
                                         break_on_hyphens=False))
    out.append(f"// Fact: {fact}")
    out.append(f"namespace {name} {{")


def close_namespace(out, name):
    out.append(f"}} // namespace {name}")


HEADER_START = """\
// Exact values that tests compare the library with. Generated by
// test/reference/golden.py, which derives each value from its definition by
// a route that shares no code with the library; do not edit. To regenerate:
//   python3 -I test/reference/golden.py > test/reference/golden.hpp
// The generator lays out one entry per line, so clang-format is off.
// clang-format off
#ifndef GALOOT_TEST_REFERENCE_GOLDEN_HPP
#define GALOOT_TEST_REFERENCE_GOLDEN_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace golden {

/// A blade mask, with bit i - 1 for generator e_i.
using mask = std::uint32_t;

/// A blade with an orientation: its mask and a sign of +1 or -1.
struct signed_blade {
  mask blade;
  int sign;
};

/// A blade mask and its name.
struct named_blade {
  mask blade;
  std::string_view name;
};

/// A name in oriented notation and the signed blade it stands for.
struct oriented_name {
  std::string_view name;
  signed_blade blade;
};

/// One term of a product: an exact coefficient times a blade.
struct term {
  int coefficient;
  mask blade;
};

/// The product a b of two blades: its first `size` terms, ordered by grade
/// and then by index list; the remaining terms are {0, 0}.
struct product {
  mask a;
  mask b;
  std::size_t size;
  std::array<term, @PRODUCT_TERMS@> terms;
};

/// An exact rational number, with a positive denominator.
struct rational {
  std::int64_t numerator;
  std::int64_t denominator;
};

/// A diagonal signature R(p, q, r) and the squares of its N generators.
template <std::size_t N>
struct diagonal_signature {
  int p;
  int q;
  int r;
  std::array<int, N> squares;
};

/// The signs that the grade operations give a blade.
struct blade_signs {
  mask blade;
  int reverse;
  int antireverse;
  int involution;
};

/// The right and left complements of a blade.
struct complement_pair {
  mask blade;
  signed_blade right;
  signed_blade left;
};

/// The image of a blade under a linear map: its first `size` terms, ordered
/// by grade and then by index list; the remaining terms are {0, 0}.
struct blade_image {
  mask blade;
  std::size_t size;
  std::array<term, @IMAGE_TERMS@> terms;
};

// The rigid objects of rigid(3), by their coefficients on Lengyel's oriented
// blades.

/// A point (x, y, z, w) on e1, e2, e3, e4.
using point = std::array<rational, 4>;

/// A line: its direction on e41, e42, e43 and its moment on e23, e31, e12.
using line = std::array<rational, 6>;

/// A plane: its normal on e423, e431, e412 and its offset on e321.
using plane = std::array<rational, 4>;

/// A rigid motion as the top three rows [R | t] of a 4 by 4 matrix that
/// acts on points as columns (x, y, z, w).
using affine = std::array<std::array<rational, 4>, 3>;

/// A 3 by 3 matrix, row by row.
using matrix3 = std::array<std::array<rational, 3>, 3>;

/// A versor of rigid(3): a motor, on e41 e42 e43 e1234 e23 e31 e12 1, or,
/// when odd is true, a flector, on e1 e2 e3 e4 e423 e431 e412 e321.
struct versor {
  bool odd;
  std::array<rational, 8> coefficients;
};

/// The line p ^ q through two points.
struct line_join {
  point p;
  point q;
  line pq;
};

/// The plane p ^ q ^ r through three points.
struct plane_join {
  point p;
  point q;
  point r;
  plane pqr;
};

/// The point (p ^ q) v g where the line through p and q meets the plane g.
struct line_plane_meet {
  point p;
  point q;
  plane g;
  point meet;
};

/// The line g v h where two planes meet.
struct plane_plane_meet {
  plane g;
  plane h;
  line meet;
};

/// The point (g v h) v k where three planes meet.
struct three_plane_meet {
  plane g;
  plane h;
  plane k;
  point meet;
};

/// The scalar p v g, zero when the point p lies on the plane g.
struct point_on_plane {
  point p;
  plane g;
  rational value;
};

/// The plane p ^ (a ^ b), zero when p lies on the line through a and b.
struct point_on_line {
  point p;
  point a;
  point b;
  plane join;
};

/// The scalar (a ^ b) v (c ^ d), zero when the two lines are coplanar.
struct line_on_line {
  point a;
  point b;
  point c;
  point d;
  rational value;
};

/// The rotor axis sin(phi) + antiscalar cos(phi) of a unitized line, and
/// the motion it performs, a turn by 2 phi about the line.
struct rotation_case {
  line axis;
  rational sin;
  rational cos;
  affine motion;
};

/// One joint Rz(theta) Tz(d) Tx(a) Rx(alpha), each angle given by the sine
/// and cosine of its half.
struct joint {
  rational theta_sin;
  rational theta_cos;
  rational d;
  rational a;
  rational alpha_sin;
  rational alpha_cos;
};

/// A chain of `size` joints, the first leftmost in the product of their
/// matrices; the remaining joints are zero. motion is the product, and
/// images are the chain points it moves.
struct joint_chain {
  std::size_t size;
  std::array<joint, @MAX_JOINTS@> joints;
  affine motion;
  std::array<point, @CHAIN_POINTS@> images;
};

/// A crystallographic point group by its Hermann-Mauguin symbol: its order,
/// its first `generator_count` generators (the rest are zero), and the index
/// of its first element in the array of elements.
struct point_group {
  std::string_view name;
  std::size_t order;
  std::size_t generator_count;
  std::array<versor, 3> generators;
  std::size_t first;
};"""


# The rigid values come from two routes that share no code: geometric algebra
# in conventions_check.py, and linear algebra in rigid_motions.py. Each
# value below is computed by both and written only if they agree exactly.

RIGID_SEED = 20261107
MAX_JOINTS = 6
CHAIN_POINTS = 10
# Every numerator and denominator written for the chains, and every
# coefficient of each partial product of their motors, stays below this, so
# that a test recomputing them in a rational of two 64-bit integers can form
# a product of two of them without overflow.
CHAIN_MAGNITUDE = 1 << 31


class Arr(list):
    """A std::array initializer: written with double braces."""


class Agg(tuple):
    """A struct initializer: written with single braces."""


def literal(x):
    if isinstance(x, Arr):
        return "{{" + ", ".join(literal(v) for v in x) + "}}"
    if isinstance(x, Agg):
        return "{" + ", ".join(literal(v) for v in x) + "}"
    if isinstance(x, bool):
        return "true" if x else "false"
    if isinstance(x, Fraction):
        return rational_literal(x)
    return str(x)


def literal_lines(x, indent):
    """x on one line at this indent if it fits in 80 columns, or else with
    its braces on their own lines and one element per line."""
    pad = " " * indent
    one = literal(x)
    if len(pad) + len(one) + 1 <= 80 or not isinstance(x, (Arr, Agg)):
        return [pad + one]
    opening, closing = ("{{", "}}") if isinstance(x, Arr) else ("{", "}")
    lines = [pad + opening]
    if not any(isinstance(v, (Arr, Agg)) for v in x):
        # Scalars fill each line.
        inner = " " * (indent + 4)
        for v in x:
            item = literal(v) + ","
            if lines[-1] != pad + opening and \
                    len(lines[-1]) + 1 + len(item) <= 80:
                lines[-1] += " " + item
            else:
                lines.append(inner + item)
        lines.append(pad + closing)
        return lines
    for v in x:
        inner = literal_lines(v, indent + 4)
        inner[-1] += ","
        lines += inner
    lines.append(pad + closing)
    return lines


def emit_values(out, type_name, name, values):
    declare(out, f"std::array<{type_name}, {len(values)}>", name, "{{")
    for v in values:
        lines = literal_lines(v, 4)
        lines[-1] += ","
        out.extend(lines)
    out.append("}};")


def fractions(values):
    return Arr(Fraction(v) for v in values)


def ga_point(p):
    return conv.point(*p[:3], w=p[3])


def ga_plane(g):
    return conv.on_names(conv.PLANE_NAMES, g)


def draw_point(rng, weights=(1, 1, 1, 2, -1, Fraction(1, 2), 3)):
    """A point with small rational coordinates and a weight from the list."""
    return tuple(Fraction(rng.randint(-6, 6), rng.choice((1, 1, 2, 3)))
                 for _ in range(3)) + (Fraction(rng.choice(weights)),)


def draw_plane(rng):
    return tuple(Fraction(rng.randint(-6, 6), rng.choice((1, 1, 2)))
                 for _ in range(4))


def rigid_incidences():
    """Joins, meets, and incidences of points, lines, and planes on seeded
    rational inputs, from conventions_check.py's wedge and antiwedge and from
    rigid_motions.py's cross products and determinants."""
    rng = random.Random(RIGID_SEED)
    out = {"lines": [], "planes": [], "line_plane": [], "plane_plane": [],
           "three_planes": [], "point_plane": [], "point_line": [],
           "line_line": []}
    for _ in range(8):
        p, q, r = (draw_point(rng) for _ in range(3))
        line = conv.coeffs(conv.wedge(ga_point(p), ga_point(q)),
                           conv.LINE_NAMES)
        agree(tuple(line), rm.line_through(p, q), f"line through {p}, {q}")
        out["lines"].append(Agg((fractions(p), fractions(q), fractions(line))))
        plane = conv.coeffs(conv.wedge(conv.wedge(ga_point(p), ga_point(q)),
                                       ga_point(r)), conv.PLANE_NAMES)
        agree(tuple(plane), rm.plane_through(p, q, r),
              f"plane through {p}, {q}, {r}")
        out["planes"].append(Agg((fractions(p), fractions(q), fractions(r),
                                  fractions(plane))))

        g, h, k = (draw_plane(rng) for _ in range(3))
        meet = conv.coeffs(conv.antiwedge(conv.wedge(ga_point(p), ga_point(q)),
                                          ga_plane(g)), conv.POINT_NAMES)
        agree(tuple(meet), rm.line_meets_plane(p, q, g),
              f"line {p}, {q} meets plane {g}")
        out["line_plane"].append(Agg((fractions(p), fractions(q),
                                      fractions(g), fractions(meet))))
        meet = conv.coeffs(conv.antiwedge(ga_plane(g), ga_plane(h)),
                           conv.LINE_NAMES)
        agree(tuple(meet), rm.planes_meet(g, h), f"planes {g}, {h} meet")
        out["plane_plane"].append(Agg((fractions(g), fractions(h),
                                       fractions(meet))))
        meet = conv.coeffs(conv.antiwedge(conv.antiwedge(ga_plane(g),
                                                         ga_plane(h)),
                                          ga_plane(k)), conv.POINT_NAMES)
        agree(tuple(meet), rm.three_planes_meet(g, h, k),
              f"planes {g}, {h}, {k} meet")
        out["three_planes"].append(Agg((fractions(g), fractions(h),
                                        fractions(k), fractions(meet))))

    def scalar(x):
        agree(set(x) <= {0}, True, "a scalar")
        return x.get(0, Fraction(0))

    # Incident and not incident, alternately: a point on the plane through
    # it and two others, a point on the line through two others (a
    # combination of them), and a fourth point on the plane of three.
    for i in range(8):
        a, b, c, d = (draw_point(rng) for _ in range(4))
        incident = i % 2 == 0
        p = a
        g = rm.plane_through(a, b, c) if incident else draw_plane(rng)
        value = scalar(conv.antiwedge(ga_point(p), ga_plane(g)))
        agree(value, rm.dot(g, p), f"point {p} on plane {g}")
        if incident:
            agree(value, 0, "a point on a plane through it")
        out["point_plane"].append(Agg((fractions(p), fractions(g), value)))

        s, t = Fraction(rng.randint(1, 4)), Fraction(rng.randint(-4, -1))
        p = tuple(s * a[j] + t * b[j] for j in range(4)) if incident else c
        join = conv.coeffs(conv.wedge(ga_point(p), conv.wedge(ga_point(a),
                                                              ga_point(b))),
                           conv.PLANE_NAMES)
        agree(tuple(join), rm.plane_through(p, a, b),
              f"point {p} and line {a}, {b}")
        agree(not any(join), rm.rank([p, a, b]) < 3, "a point on a line")
        agree(not any(join), incident, "a point on a line, as drawn")
        out["point_line"].append(Agg((fractions(p), fractions(a),
                                      fractions(b), fractions(join))))

        if incident:
            d = tuple(s * a[j] + t * b[j] + c[j] for j in range(4))
        value = scalar(conv.antiwedge(conv.wedge(ga_point(a), ga_point(b)),
                                      conv.wedge(ga_point(c), ga_point(d))))
        agree(value, rm.lines_meet(a, b, c, d), f"lines {a}, {b}, {c}, {d}")
        agree(value == 0, incident, "coplanar lines, as drawn")
        out["line_line"].append(Agg((fractions(a), fractions(b),
                                     fractions(c), fractions(d), value)))
    return out


def rigid_rotations():
    """Rotors with rational half angles about the x, y, and z axes and
    about lines off the origin, and the matrices of their motions: from
    conventions_check.py's sandwich, and from rigid_motions.py's Rodrigues
    formula with the full angle, cos = c^2 - s^2 and sin = 2sc."""
    axes = (((0, 0, 0), (1, 0, 0)), ((0, 0, 0), (0, 1, 0)),
            ((0, 0, 0), (0, 0, 1)), ((1, 2, 0), (0, 0, 1)),
            ((0, 1, -1), (1, 0, 0)),
            ((2, -1, 1), (Fraction(2, 3), Fraction(1, 3), Fraction(2, 3))),
            ((1, 0, 3), (Fraction(3, 13), Fraction(4, 13), Fraction(12, 13))))
    halves = ((Fraction(3, 5), Fraction(4, 5)),
              (Fraction(5, 13), Fraction(12, 13)),
              (Fraction(4, 5), Fraction(-3, 5)))
    cases = []
    for (a, u), (s, c) in itertools.product(axes, halves):
        a = tuple(map(Fraction, a))
        u = tuple(map(Fraction, u))
        line = conv.wedge(conv.point(*a),
                          conv.point(*(a[i] + u[i] for i in range(3))))
        motion = conv.rigid_affine(conv.rotor(line, s, c))
        matrix = rm.rotation_about_line(a, u, c * c - s * s, 2 * s * c)
        agree([list(row) for row in matrix[:3]], motion,
              f"rotation about {a} + t {u} by half angle ({s}, {c})")
        cases.append(Agg((fractions(conv.coeffs(line, conv.LINE_NAMES)), s, c,
                          Arr(fractions(row) for row in motion))))
    return cases


def rigid_chains():
    """Seeded chains of three to six joints, each Rz(theta) Tz(d) Tx(a)
    Rx(alpha): the composed motor's action on the chain points, from
    conventions_check.py, against the product of 4 by 4 matrices from
    rigid_motions.py. Half angles come from a few Pythagorean triples, and
    some are zero or a half turn, so that the values stay small."""
    rng = random.Random(RIGID_SEED + 1)
    halves = ((Fraction(0), Fraction(1)), (Fraction(1), Fraction(0)),
              (Fraction(3, 5), Fraction(4, 5)),
              (Fraction(4, 5), Fraction(3, 5)),
              (Fraction(-3, 5), Fraction(4, 5)),
              (Fraction(5, 13), Fraction(12, 13)))
    points = [tuple(Fraction(rng.randint(-5, 5)) for _ in range(3)) +
              (Fraction(1),) for _ in range(CHAIN_POINTS - 2)]
    # A point of weight 2 and a direction, of weight 0.
    points += [(Fraction(1), Fraction(-2), Fraction(3), Fraction(2)),
               (Fraction(2), Fraction(1), Fraction(-1), Fraction(0))]

    def small(x, what):
        for v in x:
            if max(abs(v.numerator), v.denominator) >= CHAIN_MAGNITUDE:
                raise Disagreement(f"{what}: {v} is too large for the tests")

    chains = []
    def draw(size):
        return [(rng.choice(halves), Fraction(rng.randint(-4, 4)),
                 Fraction(rng.randint(-4, 4)), rng.choice(halves))
                for _ in range(size)]

    def bound(joints):
        """The product of the squared denominators of the half angles,
        which every coefficient of the chain's matrix has as a multiple of
        its denominator."""
        out = 1
        for (_, tc), _, _, (_, ac) in joints:
            out *= tc.denominator ** 2 * ac.denominator ** 2
        return out

    for size in (3, 4, 5, 6):
        joints = draw(size)
        while bound(joints) >= CHAIN_MAGNITUDE:
            joints = draw(size)
        motor = conv.blade("e1234")
        for theta, d, a, alpha in joints:
            motor = conv.antigp(motor, conv.joint_motor(theta, d, a, alpha))
            small(motor.values(), f"a partial product of chain {size}")
        agree(conv.antigp(motor, conv.antireverse(motor)), conv.blade("e1234"),
              f"chain {size} is unitized")
        matrix = rm.chain([(tc * tc - ts * ts, 2 * ts * tc, d, a,
                            ac * ac - as_ * as_, 2 * as_ * ac)
                           for (ts, tc), d, a, (as_, ac) in joints])
        images = []
        for p in points:
            moved = conv.coeffs(conv.sandwich(motor, ga_point(p)),
                                conv.POINT_NAMES)
            agree(tuple(moved), rm.mat_vec(matrix, p),
                  f"chain {size} moves {p} as its matrix")
            small(moved, f"an image of chain {size}")
            images.append(fractions(moved))
        agree(conv.rigid_affine(motor), [list(row) for row in matrix[:3]],
              f"chain {size} as an affine matrix")
        rows = [Agg((ts, tc, d, a, as_, ac))
                for (ts, tc), d, a, (as_, ac) in joints]
        rows += [Agg((Fraction(0),) * 6)] * (MAX_JOINTS - size)
        motion = Arr(fractions(row) for row in matrix[:3])
        for row in motion:
            small(row, f"the matrix of chain {size}")
        chains.append(Agg((size, Arr(rows), motion, Arr(images))))
    return [fractions(p) for p in points], chains


# The 32 crystallographic point groups, by Hermann-Mauguin symbol, with
# the order, abstract group, and point symmetry that the table of the
# Wikipedia article "Crystallographic point group" (revision 1378079624 of
# 2026-10-02) gives for each, and generators in one setting. That table is
# a secondary account; the International Tables for Crystallography were not
# read. The tetragonal and cubic groups have their fourfold axis along z;
# the trigonal and hexagonal groups have their three- and sixfold axis along
# (1, 1, 1), where those rotations have rational matrices and versors, with
# twofold axes and mirror normals along (1, -1, 0). A generator is
# ("rotation", axis, n), ("rotoinversion", axis, n), ("mirror", normal), or
# ("inversion",).
XA, YA, ZA = (1, 0, 0), (0, 1, 0), (0, 0, 1)
DIAG, PERP = (1, 1, 1), (1, -1, 0)
POINT_GROUPS = (
    ("1", 1, "Z1", "enantiomorphic polar", ()),
    ("-1", 2, "Z2", "centrosymmetric", (("inversion",),)),
    ("2", 2, "Z2", "enantiomorphic polar", (("rotation", ZA, 2),)),
    ("m", 2, "Z2", "polar", (("mirror", ZA),)),
    ("2/m", 4, "V", "centrosymmetric",
     (("rotation", ZA, 2), ("mirror", ZA))),
    ("222", 4, "V", "enantiomorphic",
     (("rotation", XA, 2), ("rotation", YA, 2))),
    ("mm2", 4, "V", "polar", (("mirror", XA), ("mirror", YA))),
    ("mmm", 8, "V x Z2", "centrosymmetric",
     (("mirror", XA), ("mirror", YA), ("mirror", ZA))),
    ("4", 4, "Z4", "enantiomorphic polar", (("rotation", ZA, 4),)),
    ("-4", 4, "Z4", "non-centrosymmetric", (("rotoinversion", ZA, 4),)),
    ("4/m", 8, "Z4 x Z2", "centrosymmetric",
     (("rotation", ZA, 4), ("mirror", ZA))),
    ("422", 8, "D8", "enantiomorphic",
     (("rotation", ZA, 4), ("rotation", XA, 2))),
    ("4mm", 8, "D8", "polar", (("rotation", ZA, 4), ("mirror", XA))),
    ("-42m", 8, "D8", "non-centrosymmetric",
     (("rotoinversion", ZA, 4), ("rotation", XA, 2))),
    ("4/mmm", 16, "D8 x Z2", "centrosymmetric",
     (("rotation", ZA, 4), ("mirror", ZA), ("mirror", XA))),
    ("3", 3, "Z3", "enantiomorphic polar", (("rotation", DIAG, 3),)),
    ("-3", 6, "Z6", "centrosymmetric", (("rotation", DIAG, 3), ("inversion",))),
    ("32", 6, "D6", "enantiomorphic",
     (("rotation", DIAG, 3), ("rotation", PERP, 2))),
    ("3m", 6, "D6", "polar", (("rotation", DIAG, 3), ("mirror", PERP))),
    ("-3m", 12, "D12", "centrosymmetric",
     (("rotation", DIAG, 3), ("inversion",), ("rotation", PERP, 2))),
    ("6", 6, "Z6", "enantiomorphic polar", (("rotation", DIAG, 6),)),
    ("-6", 6, "Z6", "non-centrosymmetric", (("rotoinversion", DIAG, 6),)),
    ("6/m", 12, "Z6 x Z2", "centrosymmetric",
     (("rotation", DIAG, 6), ("mirror", DIAG))),
    ("622", 12, "D12", "enantiomorphic",
     (("rotation", DIAG, 6), ("rotation", PERP, 2))),
    ("6mm", 12, "D12", "polar", (("rotation", DIAG, 6), ("mirror", PERP))),
    ("-6m2", 12, "D12", "non-centrosymmetric",
     (("rotoinversion", DIAG, 6), ("mirror", PERP))),
    ("6/mmm", 24, "D12 x Z2", "centrosymmetric",
     (("rotation", DIAG, 6), ("mirror", DIAG), ("mirror", PERP))),
    ("23", 12, "A4", "enantiomorphic",
     (("rotation", ZA, 2), ("rotation", DIAG, 3))),
    ("m-3", 24, "A4 x Z2", "centrosymmetric",
     (("rotation", ZA, 2), ("rotation", DIAG, 3), ("inversion",))),
    ("432", 24, "S4", "enantiomorphic",
     (("rotation", ZA, 4), ("rotation", DIAG, 3))),
    ("-43m", 24, "S4", "non-centrosymmetric",
     (("rotoinversion", ZA, 4), ("rotation", DIAG, 3))),
    ("m-3m", 48, "S4 x Z2", "centrosymmetric",
     (("rotation", ZA, 4), ("rotation", DIAG, 3), ("inversion",))),
)
CYCLIC = {"Z1", "Z2", "Z3", "Z4", "Z6"}
ABELIAN = CYCLIC | {"V", "V x Z2", "Z4 x Z2", "Z6 x Z2"}


def point_groups():
    """Each group closed twice: its versors under the antiproduct, from
    conventions_check.py, and its matrices under multiplication, from
    rigid_motions.py. The two must give the same set of matrices, of the
    tabulated order, and the matrices the tabulated properties."""
    agree(len(POINT_GROUPS), 32, "the number of crystal classes")
    groups, elements = [], []
    for name, order, abstract, symmetry, generators in POINT_GROUPS:
        versors = [conv.crystal_versor(g) for g in generators]
        closure = conv.versor_closure(versors)
        motions = []
        for v in closure:
            motion = conv.rigid_affine(v)
            agree([row[3] for row in motion], [0, 0, 0],
                  f"{name}: every element fixes the origin")
            motions.append(tuple(tuple(row[:3]) for row in motion))
        agree(len(set(motions)), len(closure),
              f"{name}: distinct versors are distinct motions")
        matrices = rm.matrix_group(generators)
        agree(sorted(motions), matrices, f"{name}: the two closures")
        agree(rm.group_properties(matrices),
              {"order": order, "abelian": abstract in ABELIAN,
               "cyclic": abstract in CYCLIC,
               "centrosymmetric": symmetry == "centrosymmetric",
               "enantiomorphic": "enantiomorphic" in symmetry,
               "polar": "polar" in symmetry},
              f"{name}: the tabulated order and properties")
        rows = []
        for v in versors:
            odd = bool(conv.popcount(min(v)) & 1)
            names = conv.FLECTOR_NAMES if odd else conv.MOTOR_NAMES
            rows.append(Agg((odd, fractions(conv.coeffs(v, names)))))
        rows += [Agg((False, Arr([Fraction(0)] * 8)))] * (3 - len(rows))
        groups.append(Agg((f'"{name}"', order, len(versors), Arr(rows),
                           len(elements))))
        elements += [Arr(fractions(row) for row in m) for m in matrices]
    return groups, elements


def generate():
    wedge_products = wedge_basis_products()
    gp_tables = [(name, title, g, *gp_table(title, g))
                 for name, title, g in GP_TABLE_FORMS]
    # The product type holds as many terms as the longest product written.
    width = max(len(terms) for _, _, terms in wedge_products)
    for _, _, _, _, _, products in gp_tables:
        width = max(width, max(len(terms) for _, _, terms in products))

    tables = {}
    for n in SIGN_TABLE_SIZES:
        right, left = complements(n)
        if n == 4:
            check_complements_against_rigid_3_table(right, left)
        tables[n] = (right, left, grade_signs(n, right, left))

    # G of every form whose product table is written, with as many terms per
    # image as the longest one.
    exomorphisms = [(name, n, exomorphism(title, g, n, *tables[n]))
                    for name, title, g, n, _, _ in gp_tables]
    image_width = max(len(terms) for _, _, images in exomorphisms
                      for _, terms in images)
    out = (HEADER_START.replace("@PRODUCT_TERMS@", str(width))
           .replace("@IMAGE_TERMS@", str(image_width))
           .replace("@MAX_JOINTS@", str(MAX_JOINTS))
           .replace("@CHAIN_POINTS@", str(CHAIN_POINTS)).splitlines())

    open_namespace(out, "blade_names",
                   "bit i - 1 of a blade mask stands for e_i",
                   "The canonical name of each blade lists the indices of its "
                   "generators in increasing order. Indexed by mask.")
    for n in BLADE_TABLE_SIZES:
        names = canonical_names(n)
        emit_array(out, "named_blade", f"n{n}",
                   [f'{{{mask_literal(m, n)}, "{names[m]}"}}'
                    for m in range(1 << n)])
    close_namespace(out, "blade_names")

    open_namespace(out, "wedge_basis",
                   "e45 = e4 ^ e5 in conformal(3)",
                   "In conformal(3) the blade e45 is the wedge e4 ^ e5, not "
                   "the product e4 e5, so e4 e5 = -1 + e45. Every product of "
                   "two of e4, e5, and e45.")
    emit_array(out, "product", "conformal_3_products",
               [product_row(a, b, terms, 5, width)
                for a, b, terms in wedge_products])
    close_namespace(out, "wedge_basis")

    open_namespace(out, "gp_tables",
                   "e_i e_j + e_j e_i = 2 g_ij",
                   "Every product e_A e_B of two blades, in the wedge-monomial "
                   "basis, for five forms: rigid(3); conformal(2); "
                   "conformal(3); the "
                   "degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]]; and "
                   "seeded random form 1 of clifford_reference.py "
                   f"(seed {ref.SEED}). Each table was validated before it was "
                   "written by the defining relations e_i e_j + e_j e_i = "
                   "2 g_ij, associativity on every triple of blades, and the "
                   "check that each blade is the antisymmetrized product of "
                   "its generators. Each form has its Gram matrix and det g; "
                   "its products are listed in order of a, then b.")
    for name, _, g, n, det, products in gp_tables:
        emit_array(out, f"std::array<int, {n}>", f"{name}_gram",
                   [f"{{{', '.join(map(str, row))}}}" for row in g])
        declare(out, "int", f"{name}_det", f"{det};")
        emit_array(out, "product", name,
                   [product_row(a, b, terms, n, width)
                    for a, b, terms in products])
    close_namespace(out, "gp_tables")

    open_namespace(out, "diagonal_signatures",
                   "R(p, q, r) squares p generators to +1, q to -1, and r to 0",
                   "In R(p, q, r) the first p generators square to +1, the "
                   "next q to -1, and the last r to 0. Every R(p, q, r) with "
                   "n = p + q + r generators, for n from 1 to 6.")
    for n in range(1, MAX_N + 1):
        emit_array(out, f"diagonal_signature<{n}>", f"n{n}",
                   [f"{{{p}, {q}, {r}, {{{', '.join(map(str, s))}}}}}"
                    for p, q, r, s in diagonal_signatures(n)])
    close_namespace(out, "diagonal_signatures")

    open_namespace(out, "rigid",
                   "rigid(d) is R(d, 0, 1)",
                   "rigid(d) is R(d, 0, 1): e1 to ed are Euclidean and e(d+1) "
                   "is the null projective generator. The squares of its "
                   "generators for d from 1 to 5.")
    for d in range(1, MAX_N):
        declare(out, f"std::array<int, {d + 1}>", f"d{d}_squares",
                f"{{{', '.join(map(str, rigid_squares(d)))}}};")
    close_namespace(out, "rigid")

    check_conformal_basis_against_conventions_check()
    open_namespace(out, "conformal",
                   "conformal(d) has null e(d+1) and e(d+2) with "
                   "e(d+1) . e(d+2) = -1",
                   "In conformal(d), e1 to ed are Euclidean, e(d+1) is the "
                   "origin and e(d+2) the point at infinity, both null, with "
                   "e(d+1) . e(d+2) = -1. For d from 1 to 4: the Gram matrix; "
                   "the squares of the diagonal counterpart R(d+1, 1, 0); and "
                   "the change of basis, whose row i is e_i of conformal(d) in "
                   "the generators of R(d+1, 1, 0), with origin = "
                   "(e(d+2) - e(d+1)) / 2 and infinity = e(d+2) + e(d+1).")
    for d in range(1, MAX_N - 1):
        g, p, squares = conformal_signature(d)
        n = d + 2
        emit_array(out, f"std::array<int, {n}>", f"d{d}_gram",
                   [f"{{{', '.join(map(str, row))}}}" for row in g])
        declare(out, f"std::array<int, {n}>", f"d{d}_diagonal_squares",
                f"{{{', '.join(map(str, squares))}}};")
        emit_array(out, f"std::array<rational, {n}>", f"d{d}_in_diagonal",
                   [f"{{{{{', '.join(rational_literal(q) for q in row)}}}}}"
                    for row in p])
    close_namespace(out, "conformal")

    open_namespace(out, "units",
                   "the antiscalar is e12...n with sign +1",
                   "The scalar unit is 1, and the antiscalar is e12...n with "
                   "sign +1. The antiscalars are indexed by n - 1, for n from "
                   "1 to 6.")
    out.append("inline constexpr signed_blade scalar = {0b0, 1};")
    rows = []
    for n in range(1, MAX_N + 1):
        full, name = antiscalar(n)
        rows.append(f'{{{mask_literal(full, n)}, "{name}"}}')
    emit_array(out, "named_blade", "antiscalars", rows)
    close_namespace(out, "units")

    open_namespace(out, "oriented_names",
                   "an oriented name lists generators in the order of their "
                   "product",
                   "The oriented names of rigid(3), from Lengyel's rigid "
                   "geometric algebra (rigidgeometricalgebra.org): a name "
                   "lists generators in the order of their product, so "
                   "e31 = -e13 and e423 = +e234.")
    emit_array(out, "oriented_name", "rigid_3",
               [f'{{"{name}", {{{mask_literal(m, 4)}, {s}}}}}'
                for name, m, s in oriented_names()])
    close_namespace(out, "oriented_names")

    open_namespace(out, "result_order",
                   "results are ordered by grade, then by index list",
                   "A result with no declared type lists its components by "
                   "grade and, within a grade, lexicographically by index "
                   "list: e12, e13, e14, e23, e24, e34. Every blade in that "
                   "order.")
    for n in BLADE_TABLE_SIZES:
        emit_array(out, "mask", f"n{n}",
                   [mask_literal(m, n) for m in result_order(n)])
    close_namespace(out, "result_order")

    open_namespace(out, "grade_signs",
                   "the reverse of a grade-k blade is (-1)^(k(k-1)/2) times it",
                   "The reverse multiplies a blade of grade k by "
                   "(-1)^(k(k-1)/2), the antireverse does the same with the "
                   "antigrade n - k, and the grade involution multiplies by "
                   "(-1)^k. Indexed by mask.")
    for n in SIGN_TABLE_SIZES:
        signs = tables[n][2]
        emit_array(out, "blade_signs", f"n{n}",
                   [f"{{{mask_literal(m, n)}, {', '.join(map(str, signs[m]))}}}"
                    for m in range(1 << n)])
    close_namespace(out, "grade_signs")

    open_namespace(out, "complements",
                   "u ^ right(u) = antiscalar and left(u) ^ u = antiscalar",
                   "The right complement satisfies u ^ right(u) = antiscalar "
                   "and the left complement left(u) ^ u = antiscalar. "
                   "Indexed by mask.")
    for n in SIGN_TABLE_SIZES:
        right, left, _ = tables[n]
        emit_array(out, "complement_pair", f"n{n}",
                   [f"{{{mask_literal(m, n)}, "
                    f"{{{mask_literal(right[m][0], n)}, {right[m][1]}}}, "
                    f"{{{mask_literal(left[m][0], n)}, {left[m][1]}}}}}"
                    for m in range(1 << n)])
    close_namespace(out, "complements")

    open_namespace(out, "exomorphisms",
                   "the metric exomorphism sends e_A to the sum of "
                   "det g[A, B] e_B",
                   "The metric exomorphism G sends each blade e_A to the sum, "
                   "over the blades e_B of its grade, of the Gram minor "
                   "det g[A, B] times e_B: it applies g to a vector, and to a "
                   "wedge of vectors it gives the wedge of their images. For "
                   "the five forms of gp_tables, indexed by mask. "
                   "Each entry was computed from the Gram minors and checked "
                   "against a second route, the scalar part of e_A ~e_B in "
                   "the product table, and G was checked to compose with the "
                   "antiexomorphism left(G right(u)) to det g times the "
                   "identity in either order.")
    for name, n, images in exomorphisms:
        emit_array(out, "blade_image", name,
                   [image_row(a, terms, n, image_width)
                    for a, terms in images])
    close_namespace(out, "exomorphisms")

    rm.self_check()
    incidences = rigid_incidences()
    open_namespace(out, "rigid_lines",
                   "the line p ^ q has direction w_p q - w_q p and moment "
                   "p x q",
                   "In rigid(3), the line p ^ q through points p and q of "
                   "weights w_p and w_q has the direction w_p q - w_q p on "
                   "e41, e42, e43 and the moment p x q of their first three "
                   "coordinates on e23, e31, e12. Seeded points of several "
                   "weights; each line was computed by the wedge and by "
                   "these cross products, which agree.")
    emit_values(out, "line_join", "joins", incidences["lines"])
    close_namespace(out, "rigid_lines")

    open_namespace(out, "rigid_planes",
                   "the plane p ^ q ^ r takes the value det[x; p; q; r] at a "
                   "point x",
                   "In rigid(3), the plane p ^ q ^ r through three points has "
                   "the coefficients det[e_j; p; q; r] on e423, e431, e412, "
                   "e321, so that x v (p ^ q ^ r) = det[x; p; q; r]. Seeded "
                   "points of several weights; each plane was computed by "
                   "the wedge and by these determinants, which agree.")
    emit_values(out, "plane_join", "joins", incidences["planes"])
    close_namespace(out, "rigid_planes")

    open_namespace(out, "rigid_meets",
                   "the meets of lines and planes are the points and lines "
                   "common to them",
                   "In rigid(3): the line through p and q meets the plane g "
                   "in (p ^ q) v g = (g . p) q - (g . q) p; the planes g and "
                   "h meet in the line g v h with direction h_n x g_n and "
                   "moment h_w g_n - g_w h_n, for normals g_n, h_n and "
                   "offsets g_w, h_w; and three planes meet in the point "
                   "(g v h) v k with coordinates det[g; h; k; e_j]. Seeded "
                   "cases; each meet was computed by the antiwedge and by "
                   "these formulas, and lies on what it meets.")
    emit_values(out, "line_plane_meet", "line_plane", incidences["line_plane"])
    emit_values(out, "plane_plane_meet", "plane_plane",
                incidences["plane_plane"])
    emit_values(out, "three_plane_meet", "three_planes",
                incidences["three_planes"])
    close_namespace(out, "rigid_meets")

    open_namespace(out, "rigid_incidence",
                   "p v g, p ^ l, and l v k are zero exactly when the "
                   "objects are incident",
                   "In rigid(3): p v g = g . p, zero when the point p lies "
                   "on the plane g; p ^ (a ^ b), zero when p lies on the "
                   "line through a and b; and (a ^ b) v (c ^ d) = "
                   "det[a; b; c; d], zero when the two lines are coplanar. "
                   "Seeded cases, every other one incident by construction; "
                   "each value was computed by the wedge and antiwedge and "
                   "by these dot products, ranks, and determinants, which "
                   "agree.")
    emit_values(out, "point_on_plane", "point_plane", incidences["point_plane"])
    emit_values(out, "point_on_line", "point_line", incidences["point_line"])
    emit_values(out, "line_on_line", "line_line", incidences["line_line"])
    close_namespace(out, "rigid_incidence")

    open_namespace(out, "rigid_rotations",
                   "the rotor l sin(phi) + antiscalar cos(phi) turns by 2 phi "
                   "about l",
                   "In rigid(3), the rotor l sin(phi) + antiscalar cos(phi) "
                   "of a unitized line l turns by 2 phi about l, "
                   "counterclockwise seen from the tip of its direction. "
                   "Rational half angles about the x, y, and z axes and "
                   "about four lines off the origin; each motion was "
                   "computed by the antiproduct sandwich and by Rodrigues' "
                   "formula R = cos I + sin [u]x + (1 - cos) u u^T with "
                   "cos = c^2 - s^2 and sin = 2sc, as x goes to "
                   "a + R (x - a) for a point a of the line, which agree.")
    emit_values(out, "rotation_case", "rotations", rigid_rotations())
    close_namespace(out, "rigid_rotations")

    chain_points, chains = rigid_chains()
    open_namespace(out, "joint_chains",
                   "a chain of joint motors moves points as its product of "
                   "4x4 matrices",
                   "Seeded chains of three to six joints in rigid(3), each "
                   "Rz(theta) Tz(d) Tx(a) Rx(alpha) with rational half "
                   "angles. The joint's motor is the antiproduct of the "
                   "rotors and translators in that order, and the chain's "
                   "motor the antiproduct of its joints, the first leftmost. "
                   "Each image of a chain point was computed by the motor's "
                   "sandwich and by the product of 4 by 4 matrices, which "
                   "agree; the motor is unitized.")
    emit_values(out, "point", "points", chain_points)
    emit_values(out, "joint_chain", "chains", chains)
    close_namespace(out, "joint_chains")

    groups, elements = point_groups()
    open_namespace(out, "point_groups",
                   "the 32 crystal classes close under compose with their "
                   "published orders",
                   "The 32 crystallographic point groups, by Hermann-Mauguin "
                   "symbol: each order, generators as versors of rigid(3), "
                   "and every element as a 3 by 3 matrix, sorted. The "
                   "tetragonal and cubic groups have their fourfold axis "
                   "along z, and the trigonal and hexagonal groups their "
                   "three- and sixfold axis along (1, 1, 1), so that every "
                   "value is rational. Each group was closed twice, its "
                   "versors under the antiproduct and its matrices under "
                   "multiplication; the two give the same matrices, in the "
                   "number that published tables give.")
    emit_values(out, "point_group", "groups", groups)
    emit_values(out, "matrix3", "elements", elements)
    close_namespace(out, "point_groups")

    out += ["", "} // namespace golden", "", "// clang-format on",
            "#endif // GALOOT_TEST_REFERENCE_GOLDEN_HPP"]
    return "\n".join(out) + "\n"


def main(argv):
    try:
        text = generate()
    except Disagreement as e:
        print(f"golden.py: two routes disagree, nothing written: {e}",
              file=sys.stderr)
        return 1
    except ValueError as e:
        print(f"golden.py: a reference script rejected its own result, "
              f"nothing written: {e}", file=sys.stderr)
        return 1
    if not argv:
        sys.stdout.write(text)
        return 0
    if len(argv) == 2 and argv[0] == "--check":
        with open(argv[1], encoding="utf-8", newline="") as f:
            committed = f.read()
        if committed == text:
            print(f"{argv[1]}: matches the regenerated golden values")
            return 0
        diff = difflib.unified_diff(
            committed.splitlines(keepends=True), text.splitlines(keepends=True),
            fromfile=argv[1], tofile="regenerated")
        sys.stdout.writelines(diff)
        print(f"{argv[1]}: differs from the regenerated golden values; "
              "regenerate it with python3 -I test/reference/golden.py")
        return 1
    sys.exit("usage: golden.py [--check FILE]")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
