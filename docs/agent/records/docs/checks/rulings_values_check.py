"""Exact check of the values that the design records gained with the rulings.

The notation key and the authoring rules show one line with the antiproduct
symbol and one with its C++ operator; the misconception record and the
conventions card quote the values that tell transform from the classical
sandwich for a rotor that is not a quarter turn; the authoring rules set the
second cold reader a rotor in another plane. Each value is derived by hand
in the record that quotes it. This script recomputes each from the
definitions, with the small algebra of transform_sandwich_check.py beside it,
which shares no code with the library. It prints one line per value and exits
nonzero if any differs.

Run: python3 -I rulings_values_check.py
"""

import importlib.util
import pathlib
import sys

_here = pathlib.Path(__file__).resolve().parent
_spec = importlib.util.spec_from_file_location(
    "transform_sandwich_check", _here / "transform_sandwich_check.py"
)
tsc = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(tsc)

E1, E2, E3 = 0b001, 0b010, 0b100
E12, E13, E23, E123 = 0b011, 0b101, 0b110, 0b111

failures = []


def check(label, got, expected):
    ok = got == expected
    print(f"{label:<66} {'holds' if ok else 'FAIL'}")
    if not ok:
        print(f"    got {got}, expected {expected}")
        failures.append(label)


def vec(x, y, z):
    return tsc.Algebra.clean({E1: x, E2: y, E3: z})


def main():
    e3 = tsc.Algebra((1, 1, 1))

    # The mirrors of chapter 6 as planes: right(e1) = e23 and
    # right(e1 + e2) = e23 + e31, where e31 = -e13.
    g = e3.right(vec(1, 0, 0))
    h = e3.right(vec(1, 1, 0))
    check("right(e1) = e23", g, {E23: 1})
    check("right(e1 + e2) = e23 + e31", h, {E23: 1, E13: -1})

    # Running-text line: (e23 + e31) antiproduct e23 = e123 - e3.
    v = e3.anti(h, g)
    check("(e23 + e31) anti e23 = e123 - e3", v, {E123: 1, E3: -1})
    # It is the complement of the rotor b a = 1 - e12.
    r = e3.gp(vec(1, 1, 0), vec(1, 0, 0))
    check("right(1 - e12) = e123 - e3", e3.right(r), v)

    # Text-block line: (e123 - e3) % e3 % (e123 + e3) = 2 e3.
    check("antireverse(e123 - e3) = e123 + e3", e3.antireverse(v),
          {E123: 1, E3: 1})
    left_first = e3.anti(e3.anti(v, {E3: 1}), {E123: 1, E3: 1})
    right_first = e3.anti(v, e3.anti({E3: 1}, {E123: 1, E3: 1}))
    check("(e123 - e3) % e3 % (e123 + e3) = 2 e3, grouped from the left",
          left_first, {E3: 2})
    check("the same, grouped from the right", right_first, {E3: 2})
    check("transform(e3, e123 - e3) = 2 e3",
          tsc.full_transform(e3, {E3: 1}, v), {E3: 2})

    # Text-block line with both products: a % b = left(right(a) * right(b)),
    # on one pair of multivectors that are not blades. This script defines
    # the antiproduct so, which makes the line a guard of the script alone;
    # rulings_values_probe.cpp checks the same pair against the library.
    a = {0: 1, E1: 2, E23: -3, E123: 1}
    b = {E2: 1, E13: 4, E3: -1}
    check("a % b = left(right(a) * right(b)), one pair",
          e3.anti(a, b), e3.left(e3.gp(e3.right(a), e3.right(b))))

    # The conventions card's row: s = 2 - e12 on (2, -3, 5).
    s = e3.gp(vec(2, 1, 0), vec(1, 0, 0))
    check("s = (2, 1, 0)(1, 0, 0) = 2 - e12", s, {0: 2, E12: -1})
    x = vec(2, -3, 5)
    check("s v reverse(s) = (18, -1, 25)", tsc.classical(e3, x, s),
          vec(18, -1, 25))
    check("transform(v, s) = (6, 17, -25)", tsc.full_transform(e3, x, s),
          vec(6, 17, -25))
    check("right(s) = 2 e123 - e3", e3.right(s), {E123: 2, E3: -1})
    check("transform(v, right(s)) = (18, -1, 25)",
          tsc.full_transform(e3, x, e3.right(s)), vec(18, -1, 25))

    # The quarter turn's coincidence, which the pages name: r = 1 - e12 on
    # (1, 0, 0) gives (0, 2, 0) both ways, and on (0, 0, 1) it does not.
    check("r v reverse(r) on (1, 0, 0) = (0, 2, 0)",
          tsc.classical(e3, vec(1, 0, 0), r), vec(0, 2, 0))
    check("transform(v, r) on (1, 0, 0) = (0, 2, 0)",
          tsc.full_transform(e3, vec(1, 0, 0), r), vec(0, 2, 0))
    check("transform(v, r) on (0, 0, 1) = (0, 0, -2)",
          tsc.full_transform(e3, vec(0, 0, 1), r), vec(0, 0, -2))

    # The rest of T1 and T2: the classical value of T1 and the complemented
    # form of both.
    check("r v reverse(r) on (0, 0, 1) = (0, 0, 2)",
          tsc.classical(e3, vec(0, 0, 1), r), vec(0, 0, 2))
    check("transform(v, right(r)) on (0, 0, 1) = (0, 0, 2)",
          tsc.full_transform(e3, vec(0, 0, 1), e3.right(r)), vec(0, 0, 2))
    check("transform(v, right(r)) on (1, 0, 0) = (0, 2, 0)",
          tsc.full_transform(e3, vec(1, 0, 0), e3.right(r)), vec(0, 2, 0))

    # The second cold reader's task: a rotor in another plane, with cosine
    # 4/5 and sine 3/5 and factor 10, that no page under test prints.
    # t = (1, 0, 3)(0, 0, 1) = 3 + e13 on (4, -1, 2).
    t = e3.gp(vec(1, 0, 3), vec(0, 0, 1))
    check("t = (1, 0, 3)(0, 0, 1) = 3 + e13", t, {0: 3, E13: 1})
    y = vec(4, -1, 2)
    check("t v reverse(t) on (4, -1, 2) = (44, -10, -8)",
          tsc.classical(e3, y, t), vec(44, -10, -8))
    check("transform(v, t) on (4, -1, 2) = (-20, 10, -40)",
          tsc.full_transform(e3, y, t), vec(-20, 10, -40))
    check("right(t) = 3 e123 - e2", e3.right(t), {E123: 3, E2: -1})
    check("transform(v, right(t)) on (4, -1, 2) = (44, -10, -8)",
          tsc.full_transform(e3, y, e3.right(t)), vec(44, -10, -8))

    print("all values hold" if not failures else f"FAILURES: {failures}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
