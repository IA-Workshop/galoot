"""Writes test/bench/reference_kernels.hpp: straight-line kernels of seven
operations of rigid(3), against which the benchmarks time the library's
operations and which the smoke tests compare with them.

Each kernel is the arithmetic of the operation written out as a person would
write it by hand, one expression per output: the products of one coefficient
of each operand, each with its sign, added in a fixed order. The terms come
from the definitions, computed here with conventions_check.py's algebra of
rigid(3), which shares no code with the library:

- A product of two objects has one term for each pair of a blade of the first
  operand and a blade of the second whose product is not zero, in order of
  the first operand's blade and then the second's, as the layouts list them.
  The sign of a term folds in the orientations of both operands' blades and
  of the result's (e41 = -e14, so a coefficient on e41 is minus the one on
  e14).
- point ^ point ^ point is (p ^ q) ^ r, the first join kept on the bivectors
  in increasing order (e12, e13, e14, e23, e24, e34) with sign +1.
- transform(x, Q) is the sandwich Q v x v antireverse(Q) in two stages. The
  first, Q v x, is kept on its blades in increasing order (grade, then index
  list), and only where the second stage reads it. The second multiplies it
  by Q with the sign of antireverse folded into each term, and keeps the
  terms that land on x's blades, where the rest of the sandwich is zero.
- compose(Q2, Q1) is the antiproduct Q2 v Q1, read on the motor's layout.

Each kernel is declared [[gnu::always_inline]], so that the benchmarks time
it inline, as straight-line code, and not as a call.

Within an output the first term is assigned, negated when its sign is -1, and
each later one added or subtracted. So over double, for the same operands, a
kernel and any code that adds the same products in the same order give the
same values; a sign moved from a sum into its terms changes only the sign of
a zero result, since rounding to nearest is symmetric.

Before writing, the script evaluates each kernel's terms on seeded rational
operands and stops, writing nothing, unless the result equals the operation
computed on whole multivectors by conventions_check.py; the joins and the
meet must also equal the closed forms of rigid_motions.py, which uses no
geometric algebra (Pluecker coordinates, cofactors, and (g . p) q - (g . q) p
for the line through p and q).

Usage, with the interpreter isolated as for every reference script:

  python3 -I test/reference/reference_kernels.py \\
          > test/bench/reference_kernels.hpp
      Regenerate the committed header.
  python3 -I test/reference/reference_kernels.py \\
          --check test/bench/reference_kernels.hpp
      Exit 1, printing a unified diff, if the file differs in any byte from
      what the script writes now; exit 0 if it is the same.
"""

import difflib
import os
import random
import sys
from fractions import Fraction

# Isolated mode leaves the script's own directory off sys.path, and the
# reference scripts live there.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import conventions_check as conv
import rigid_motions as rm

SEED = 20261110
TRIALS = 50

POINT = conv.POINT_NAMES
LINE = conv.LINE_NAMES
PLANE = conv.PLANE_NAMES
MOTOR = conv.MOTOR_NAMES


class Disagreement(Exception):
    """A kernel's terms do not give the value of its operation."""


def layout(names):
    """(mask, orientation) of each name, in the order given."""
    return [conv.parse(name) for name in names]


def canonical(masks):
    """Masks in increasing order: by grade, then by index list."""
    return sorted(masks, key=lambda m: (conv.popcount(m),
                                        [i for i in range(4) if m >> i & 1]))


def blade_product(product, a, b):
    """(sign, mask) of the product of the unit blades a and b, or None."""
    x = product({a: 1}, {b: 1})
    if not x:
        return None
    (mask, sign), = x.items()
    return int(sign), mask


def terms(product, first, second):
    """Every term (a, b, mask, sign) of the product of two layouts, each a
    list of (mask, orientation), in order of a, then b."""
    out = []
    for a, (ma, sa) in enumerate(first):
        for b, (mb, sb) in enumerate(second):
            p = blade_product(product, ma, mb)
            if p is not None:
                out.append((a, b, p[1], p[0] * sa * sb))
    return out


def onto(found, target):
    """The terms moved to positions of the target layout, each sign
    multiplied by the orientation of its blade there."""
    place = {mask: (i, sign) for i, (mask, sign) in enumerate(target)}
    return [(a, b, place[m][0], c * place[m][1]) for a, b, m, c in found]


def middle(found):
    """The blades the terms reach, in increasing order with sign +1."""
    return [(m, 1) for m in canonical({m for _, _, m, _ in found})]


def antireverse_sign(mask):
    return conv.grade_sign(conv.N - conv.popcount(mask))


def sandwich_stages(x):
    """The two kernels of transform(x, Q): the first stage's terms onto
    positions of the middle, those the second reads, and the second's onto
    positions of x."""
    v = layout(MOTOR)
    first = terms(conv.antigp, v, x)
    mid = middle(first)
    second = [(a, b, m, c * antireverse_sign(v[b][0]))
              for a, b, m, c in terms(conv.antigp, mid, v)]
    kept = [t for t in second if t[2] in {mask for mask, _ in x}]
    read = {a for a, _, _, _ in kept}
    first = [t for t in onto(first, mid) if t[2] in read]
    return first, onto(kept, x), mid


def run(found, size, x, y):
    """The outputs the terms give for operand values x and y."""
    out = [None] * size
    for a, b, o, c in found:
        v = x[a] * y[b]
        if out[o] is None:
            out[o] = v if c == 1 else -v
        else:
            out[o] = out[o] + v if c == 1 else out[o] - v
    return [Fraction(0) if v is None else v for v in out]


# --- The C++ text ------------------------------------------------------------

def indexed(name):
    """The C++ text of coefficient i of the array `name`."""
    return lambda i: f"{name}[{i}]"


def expression(found, output, x, y):
    """The sum for one output, its terms in order; x and y give the text of
    each operand's coefficients."""
    text = ""
    for a, b, o, c in found:
        if o != output:
            continue
        product = f"{x(a)} * {y(b)}"
        if not text:
            text = product if c == 1 else f"-({product})"
        else:
            text += f" {'+' if c == 1 else '-'} {product}"
    return text or "T{}"


def returned(found, names, x, y):
    lines = [f"      {expression(found, i, x, y)}, // {name}"
             for i, name in enumerate(names)]
    return ["  return {"] + lines + ["  };"]


def blade_name(mask):
    """The name of a value of the middle of a kernel: its blade's indices in
    increasing order, or `scalar`."""
    if mask == 0:
        return "scalar"
    return "e" + "".join(str(i + 1) for i in range(4) if mask >> i & 1)


def signature(name, result, operands):
    params = ",\n".join(f"    const std::array<T, {size}>& {p}"
                        for p, size in operands)
    return ["template <class T>",
            f"[[gnu::always_inline]] constexpr std::array<T, {result}> {name}(",
            params + ") {"]


def kernel_text(name, comment, result_names, operands, body):
    lines = [f"// {line}" if line else "//" for line in comment]
    lines += signature(name, len(result_names), operands)
    lines += body
    lines += ["}"]
    return "\n".join(lines) + "\n"


def product_kernel(name, comment, product, first, second, result):
    found = onto(terms(product, layout(first), layout(second)), layout(result))
    body = returned(found, result, indexed("x"), indexed("y"))
    return found, kernel_text(name, comment, result,
                              [("x", len(first)), ("y", len(second))], body)


def join_of_three():
    """(p ^ q) ^ r, with p ^ q on the canonical bivectors."""
    pq = terms(conv.wedge, layout(POINT), layout(POINT))
    mid = middle(pq)
    second = onto(terms(conv.wedge, mid, layout(POINT)), layout(PLANE))
    pq = onto(pq, mid)
    names = [blade_name(m) for m, _ in mid]
    body = [f"  const T {names[i]} = "
            f"{expression(pq, i, indexed('p'), indexed('q'))};"
            for i in range(len(mid))]
    body += returned(second, PLANE, names.__getitem__, indexed("r"))
    comment = [
        "p ^ q ^ r, the plane through three points, on e423 e431 e412 e321:",
        "first p ^ q on the bivectors e12 e13 e14 e23 e24 e34, then that",
        "join with r.",
    ]
    text = kernel_text("wedge_point_point_point", comment, PLANE,
                       [("p", 4), ("q", 4), ("r", 4)], body)
    return (pq, second, len(mid)), text


def transform_kernel(name, object_name, names_of_x):
    first, second, mid = sandwich_stages(layout(names_of_x))
    names = [blade_name(m) for m, _ in mid]
    body = [f"  const T {names[i]} = "
            f"{expression(first, i, indexed('q'), indexed('x'))};"
            for i in sorted({a for a, _, _, _ in second})]
    body += returned(second, names_of_x, names.__getitem__, indexed("q"))
    comment = [
        f"transform(x, Q) for {object_name} x, Q v x v antireverse(Q), on x's",
        f"layout {' '.join(names_of_x)}: first Q v x on the blades the",
        "second stage reads, then that times Q with antireverse's signs.",
    ]
    text = kernel_text(name, comment, names_of_x,
                       [("x", len(names_of_x)), ("q", 8)], body)
    return (first, second, len(mid)), text


# --- Checks against the definitions ----------------------------------------

def agree(a, b, what):
    if list(a) != list(b):
        raise Disagreement(f"{what}: {a!r} != {b!r}")


def draw(rng, size):
    return [Fraction(rng.randint(-9, 9), rng.randint(1, 8))
            for _ in range(size)]


def check_products(rng, pp, ppp, lg, qq):
    for _ in range(TRIALS):
        p, q, r, g = draw(rng, 4), draw(rng, 4), draw(rng, 4), draw(rng, 4)
        a, b = draw(rng, 8), draw(rng, 8)
        mp, mq, mr = (conv.on_names(POINT, x) for x in (p, q, r))
        mg = conv.on_names(PLANE, g)

        line = run(pp, 6, p, q)
        agree(line, conv.coeffs(conv.wedge(mp, mq), LINE), "p ^ q")
        agree(line, rm.line_through(p, q), "p ^ q against Pluecker")

        first, second, size = ppp
        plane = run(second, 4, run(first, size, p, q), r)
        agree(plane, conv.coeffs(conv.wedge(conv.wedge(mp, mq), mr), PLANE),
              "p ^ q ^ r")
        agree(plane, rm.plane_through(p, q, r), "p ^ q ^ r against cofactors")

        meet = run(lg, 4, line, g)
        agree(meet, conv.coeffs(conv.antiwedge(conv.wedge(mp, mq), mg), POINT),
              "line v plane")
        agree(meet, rm.line_meets_plane(p, q, g), "line v plane against "
              "(g . p) q - (g . q) p")

        agree(run(qq, 8, a, b), conv.coeffs(
            conv.antigp(conv.on_names(MOTOR, a), conv.on_names(MOTOR, b)),
            MOTOR), "compose")


def check_transform(rng, stages, names):
    first, second, size = stages
    for _ in range(TRIALS):
        x, q = draw(rng, len(names)), draw(rng, 8)
        mq = conv.on_names(MOTOR, q)
        whole = conv.sandwich(mq, conv.on_names(names, x))
        agree(run(second, len(names), run(first, size, q, x), q),
              conv.coeffs(whole, names), f"transform on {' '.join(names)}")


# --- The header --------------------------------------------------------------

HEAD = """\
// Straight-line kernels of seven operations of rigid(3), the arithmetic of
// each written out by hand, for the benchmarks to time the library against.
// Generated by test/reference/reference_kernels.py, which derives each term
// from the definitions by a route that shares no code with the library; do
// not edit. To regenerate, from the repository's root:
//   python3 -I test/reference/reference_kernels.py
//       > test/bench/reference_kernels.hpp
//
// Layouts, as the rga3 preset stores them: a point on e1 e2 e3 e4, a line on
// e41 e42 e43 e23 e31 e12, a plane on e423 e431 e412 e321, and a motor on
// e41 e42 e43 e1234 e23 e31 e12 1, where e41 = -e14, e31 = -e13, and
// e321 = -e123. Within an output the terms come in order of the first
// operand's coefficient, then the second's.
//
// Each kernel is always inlined (gnu::always_inline, which GCC and Clang
// accept), so that a benchmark times it as the straight-line code it is
// written as: left to their own heuristics, both compilers kept the
// kernels of the transforms and of compose out of line, as a call in the
// timed loop.
//
// The generator lays out one term list per line, so clang-format is off.
// clang-format off
#ifndef GALOOT_TEST_BENCH_REFERENCE_KERNELS_HPP
#define GALOOT_TEST_BENCH_REFERENCE_KERNELS_HPP

#include <array>

namespace reference_kernels {
"""

TAIL = """\
} // namespace reference_kernels

// clang-format on
#endif // GALOOT_TEST_BENCH_REFERENCE_KERNELS_HPP
"""


def generate():
    rng = random.Random(SEED)
    pp, pp_text = product_kernel(
        "wedge_point_point",
        ["x ^ y, the line through two points, on e41 e42 e43 e23 e31 e12."],
        conv.wedge, POINT, POINT, LINE)
    ppp, ppp_text = join_of_three()
    lg, lg_text = product_kernel(
        "antiwedge_line_plane",
        ["x v y, the point where a line meets a plane, on e1 e2 e3 e4."],
        conv.antiwedge, LINE, PLANE, POINT)
    qq, qq_text = product_kernel(
        "compose_motors",
        ["compose(x, y) = x v y, the motor that applies y and then x, on",
         "e41 e42 e43 e1234 e23 e31 e12 1."],
        conv.antigp, MOTOR, MOTOR, MOTOR)
    check_products(rng, pp, ppp, lg, qq)

    texts = [pp_text, ppp_text, lg_text]
    for name, object_name, names in (("transform_point", "a point", POINT),
                                     ("transform_line", "a line", LINE),
                                     ("transform_plane", "a plane", PLANE)):
        stages, text = transform_kernel(name, object_name, names)
        check_transform(rng, stages, names)
        texts.append(text)
    texts.append(qq_text)

    return HEAD + "\n" + "\n".join(texts) + "\n" + TAIL


def main(argv):
    try:
        text = generate()
    except Disagreement as e:
        print(f"reference_kernels.py: a kernel disagrees with its operation, "
              f"nothing written: {e}", file=sys.stderr)
        return 1
    if not argv:
        sys.stdout.write(text)
        return 0
    if len(argv) == 2 and argv[0] == "--check":
        with open(argv[1], encoding="utf-8", newline="") as f:
            committed = f.read()
        if committed == text:
            print(f"{argv[1]}: matches the regenerated kernels")
            return 0
        diff = difflib.unified_diff(
            committed.splitlines(keepends=True),
            text.splitlines(keepends=True),
            fromfile=argv[1], tofile="regenerated")
        sys.stdout.writelines(diff)
        print(f"{argv[1]}: differs from the regenerated kernels; regenerate "
              "it with python3 -I test/reference/reference_kernels.py")
        return 1
    sys.exit("usage: reference_kernels.py [--check FILE]")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
