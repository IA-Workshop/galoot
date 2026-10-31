"""Writes test/reference/golden.hpp: exact values that tests compare the
library with, each derived here from its definition. Snippet candidate.

The values come from the two reference scripts beside this one, which share
no code with the library: clifford_reference.py multiplies in any symmetric
integer form by diagonalizing it over fractions, and conventions_check.py
holds the oriented blade names and the rigid(3) complement table of Lengyel's
rigid geometric algebra (rigidgeometricalgebra.org). Where a convention is
stated both as a definition and as a closed formula or table, the script
computes both and stops if they differ, so each value is written only when two
routes agree.

Usage, with the interpreter isolated as for every reference script:

  python3 -I test/reference/golden.py > test/reference/golden.hpp
      Regenerate the committed header.
  python3 -I test/reference/golden.py --check test/reference/golden.hpp
      Exit 1, printing a unified diff, if the file differs in any byte from
      what the script writes now; exit 0 if it is the same.

A blade is a bitmask in which bit i - 1 stands for generator e_i. The
per-blade tables are written for n = 4 and n = 5: the sizes of rigid(3) and
conformal(3), and one even and one odd n, which the left complement tells
apart.
"""

import difflib
import itertools
import os
import re
import sys
import textwrap
from fractions import Fraction

# Isolated mode leaves the script's own directory off sys.path, and the two
# reference scripts live there.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import clifford_reference as ref
import conventions_check as conv

BLADE_TABLE_SIZES = (4, 5)
MAX_N = 6  # largest n of the signature tables

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
    """Blades ordered by grade, then lexicographically by index list:
    e12, e13, e14, e23, e24, e34. The order is checked pairwise."""
    order = sorted(range(1 << n),
                   key=lambda m: (ref.popcount(m), ref.indices(m)))
    for a, b in itertools.pairwise(order):
        ka, kb = ref.popcount(a), ref.popcount(b)
        agree(ka < kb or (ka == kb and ref.indices(a) < ref.indices(b)), True,
              f"order of {a} before {b}")
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
    """Each oriented name in rigid(3) as a mask and a sign. The sign from
    conventions_check.parse, the parity of the order written, is compared
    with the product of the generators in that order in R(4, 0, 0)."""
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
        terms = sorted(alg.mul(unit(a), unit(b)).items(),
                       key=lambda kv: (ref.popcount(kv[0]),
                                       ref.indices(kv[0])))
        for _, c in terms:
            agree(c.denominator, 1, "integer coefficient")
        products.append((a, b, [(int(c), m) for m, c in terms]))
    agree(products[1][2], [(-1, 0), (1, e4 | e5)], "e4 e5 = -1 + e45")
    return products


def diagonal_signatures(n):
    """Every R(p, q, r) with p + q + r = n: the first p generators square to
    +1, the next q to -1, and the last r to 0."""
    out = []
    for p in range(n, -1, -1):
        for q in range(n - p, -1, -1):
            r = n - p - q
            g = ref.r_pqr(p, q, r)
            squares = [g[i][i] for i in range(n)]
            agree(squares, [1] * p + [-1] * q + [0] * r, f"R({p},{q},{r})")
            out.append((p, q, r, squares))
    return out


def rigid_squares(d):
    """rigid(d) is R(d, 0, 1): e1 to ed Euclidean, e(d+1) null."""
    g = ref.rigid(d)
    agree(g, ref.r_pqr(d, 0, 1), f"rigid({d})")
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


def open_namespace(out, name, text):
    # No line break inside a signature such as R(d+1, 1, 0): its spaces are
    # held as U+00A0 while the text is wrapped.
    held = re.sub(r"\([^()]*\)", lambda m: m.group(0).replace(" ", " "),
                  text)
    out.append("")
    out.extend(line.replace(" ", " ")
               for line in textwrap.wrap(held, width=77, initial_indent="// ",
                                         subsequent_indent="// ",
                                         break_on_hyphens=False))
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
  std::array<term, 2> terms;
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
};"""


def generate():
    out = HEADER_START.splitlines()

    open_namespace(out, "blade_names",
                   "The canonical name of each blade lists the indices of its "
                   "generators in increasing order. Indexed by mask.")
    for n in BLADE_TABLE_SIZES:
        names = canonical_names(n)
        emit_array(out, "named_blade", f"n{n}",
                   [f'{{{mask_literal(m, n)}, "{names[m]}"}}'
                    for m in range(1 << n)])
    close_namespace(out, "blade_names")

    open_namespace(out, "wedge_basis",
                   "In conformal(3) the blade e45 is the wedge e4 ^ e5, not "
                   "the product e4 e5, so e4 e5 = -1 + e45. Every product of "
                   "two of e4, e5, and e45.")
    rows = []
    for a, b, terms in wedge_basis_products():
        padded = terms + [(0, 0)] * (2 - len(terms))
        body = ", ".join(f"{{{c}, {mask_literal(m, 5)}}}" for c, m in padded)
        rows.append(f"{{{mask_literal(a, 5)}, {mask_literal(b, 5)}, "
                    f"{len(terms)}, {{{{{body}}}}}}}")
    emit_array(out, "product", "conformal_3_products", rows)
    close_namespace(out, "wedge_basis")

    open_namespace(out, "diagonal_signatures",
                   "In R(p, q, r) the first p generators square to +1, the "
                   "next q to -1, and the last r to 0. Every R(p, q, r) with "
                   "n = p + q + r generators, for n from 1 to 6.")
    for n in range(1, MAX_N + 1):
        emit_array(out, f"diagonal_signature<{n}>", f"n{n}",
                   [f"{{{p}, {q}, {r}, {{{', '.join(map(str, s))}}}}}"
                    for p, q, r, s in diagonal_signatures(n)])
    close_namespace(out, "diagonal_signatures")

    open_namespace(out, "rigid",
                   "rigid(d) is R(d, 0, 1): e1 to ed are Euclidean and e(d+1) "
                   "is the null projective generator. The squares of its "
                   "generators for d from 1 to 5.")
    for d in range(1, MAX_N):
        declare(out, f"std::array<int, {d + 1}>", f"d{d}_squares",
                f"{{{', '.join(map(str, rigid_squares(d)))}}};")
    close_namespace(out, "rigid")

    check_conformal_basis_against_conventions_check()
    open_namespace(out, "conformal",
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
                   "The oriented names of rigid(3), from Lengyel's rigid "
                   "geometric algebra (rigidgeometricalgebra.org): a name "
                   "lists generators in the order of their product, so "
                   "e31 = -e13 and e423 = +e234.")
    emit_array(out, "oriented_name", "rigid_3",
               [f'{{"{name}", {{{mask_literal(m, 4)}, {s}}}}}'
                for name, m, s in oriented_names()])
    close_namespace(out, "oriented_names")

    open_namespace(out, "result_order",
                   "A result with no declared type lists its components by "
                   "grade and, within a grade, lexicographically by index "
                   "list: e12, e13, e14, e23, e24, e34. Every blade in that "
                   "order.")
    for n in BLADE_TABLE_SIZES:
        emit_array(out, "mask", f"n{n}",
                   [mask_literal(m, n) for m in result_order(n)])
    close_namespace(out, "result_order")

    tables = {}
    for n in BLADE_TABLE_SIZES:
        right, left = complements(n)
        if n == 4:
            check_complements_against_rigid_3_table(right, left)
        tables[n] = (right, left, grade_signs(n, right, left))

    open_namespace(out, "grade_signs",
                   "The reverse multiplies a blade of grade k by "
                   "(-1)^(k(k-1)/2), the antireverse does the same with the "
                   "antigrade n - k, and the grade involution multiplies by "
                   "(-1)^k. Indexed by mask.")
    for n in BLADE_TABLE_SIZES:
        signs = tables[n][2]
        emit_array(out, "blade_signs", f"n{n}",
                   [f"{{{mask_literal(m, n)}, {', '.join(map(str, signs[m]))}}}"
                    for m in range(1 << n)])
    close_namespace(out, "grade_signs")

    open_namespace(out, "complements",
                   "The right complement satisfies u ^ right(u) = antiscalar "
                   "and the left complement left(u) ^ u = antiscalar. "
                   "Indexed by mask.")
    for n in BLADE_TABLE_SIZES:
        right, left, _ = tables[n]
        emit_array(out, "complement_pair", f"n{n}",
                   [f"{{{mask_literal(m, n)}, "
                    f"{{{mask_literal(right[m][0], n)}, {right[m][1]}}}, "
                    f"{{{mask_literal(left[m][0], n)}, {left[m][1]}}}}}"
                    for m in range(1 << n)])
    close_namespace(out, "complements")

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
