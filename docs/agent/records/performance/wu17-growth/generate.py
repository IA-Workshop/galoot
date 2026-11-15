#!/usr/bin/env -S python3 -I
"""Write the units of the growth grid as a spec for tools/compiles cost.

Usage, from anywhere:
  generate.py > units.toml

Every unit includes <galoot/core.hpp>, declares the algebra R(n - 1, 0, 1)
as core::rigid<n - 1>(), and the four rigid(3) layouts, point, line, plane,
and motor, embedded in it: each name of rigid(3) with e4 read as e_n, so
that e41 is e_n e1, its sign the sign of the permutation that sorts the
generators. The layouts are written as blade_set literals of masks and
signs, since blades<Alg, "..."> takes one digit per generator. Each unit
defines one function with external linkage that reads its operands from
memory at run time and returns the sum of every coefficient of every result,
so the compiler emits every operation it instantiates.

Fixed use against n = 4, 8, 16, one unit per operation and n:
  baseline     the operands only, no operation
  wedge        p ^ q and (p ^ q) ^ r, for points p, q, r
  antiwedge    l & g, for a line l and a plane g
  product      the geometric product of point and point, line and plane,
               motor and point, point and motor
  antiproduct  compose(Q2, Q1), the geometric antiproduct of two motors
  antiproduct4 the geometric antiproduct of point and point, plane and
               plane, motor and point, motor and motor
  transform    transform of a point, a line, and a plane by a motor
  dual         bulk_dual of a point, a line, and a plane
  involution   reverse of a point, a line, a plane, and a motor

Distinct operations at n = 4: units distinct1, distinct4, and distinct16
with the first 1, 4, and 16 entries of DISTINCT, each a different operation
or operand types, against the baseline at n = 4.

The units have trees = ["after"] and kind = "debug", so tools/compiles
compiles each with the flags of the gcc-debug and clang-debug trees, taken
from the compile command of the test FLAGS_FROM.
"""

import sys

SIZES = (4, 8, 16)
FLAGS_FROM = "core.assert_consteval"

RIGID = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "e41 e42 e43 e1234 e23 e31 e12 1",
}

# Each use: the expressions whose results are summed, over the operands
# p, q, r (points), l (line), g, h (planes), Q1, Q2 (motors).
USES = {
    "baseline": [],
    "wedge": ["core::wedge(p, q)", "core::wedge(core::wedge(p, q), r)"],
    "antiwedge": ["core::antiwedge(l, g)"],
    "product": ["core::geometric_product(p, q)",
                "core::geometric_product(l, g)",
                "core::geometric_product(Q1, p)",
                "core::geometric_product(p, Q1)"],
    "antiproduct": ["core::compose(Q2, Q1)"],
    "antiproduct4": ["core::geometric_antiproduct(p, q)",
                     "core::geometric_antiproduct(g, h)",
                     "core::geometric_antiproduct(Q1, p)",
                     "core::geometric_antiproduct(Q2, Q1)"],
    "transform": ["core::transform(p, Q1)", "core::transform(l, Q1)",
                  "core::transform(g, Q1)"],
    "dual": ["core::bulk_dual(p)", "core::bulk_dual(l)",
             "core::bulk_dual(g)"],
    "involution": ["core::reverse(p)", "core::reverse(l)",
                   "core::reverse(g)", "core::reverse(Q1)"],
}

# Sixteen distinct operations: no two have the same operation and operand
# types.
DISTINCT = [
    "core::wedge(p, q)",
    "core::antiwedge(l, g)",
    "core::transform(p, Q1)",
    "core::compose(Q2, Q1)",
    "core::wedge(l, r)",
    "core::antiwedge(g, h)",
    "core::transform(l, Q1)",
    "core::transform(g, Q1)",
    "core::geometric_product(Q1, p)",
    "core::geometric_product(p, Q1)",
    "core::geometric_antiproduct(Q1, p)",
    "core::bulk_dual(p)",
    "core::reverse(Q1)",
    "core::bulk_dual(l)",
    "core::geometric_product(p, q)",
    "core::antiwedge(g, l)",
]

OPERANDS = [("Point", "p"), ("Point", "q"), ("Point", "r"), ("Line", "l"),
            ("Plane", "g"), ("Plane", "h"), ("Motor", "Q1"), ("Motor", "Q2")]


def oriented(name, n):
    """The mask and sign of a rigid(3) blade name with e4 read as e_n."""
    if name == "1":
        return 0, 1
    indices = [n if c == "4" else int(c) for c in name[1:]]
    inversions = sum(1 for a in range(len(indices))
                     for b in range(a + 1, len(indices))
                     if indices[a] > indices[b])
    return sum(1 << (i - 1) for i in indices), -1 if inversions % 2 else 1


def layout(names, n):
    blades = ", ".join(f"{{{mask}U, {sign}}}" for mask, sign in
                       (oriented(x, n) for x in names.split()))
    return f"core::blade_set<{len(names.split())}>{{{{{{{blades}}}}}}}"


def unit(n, expressions):
    lines = ["#include <cstddef>", "", "#include <galoot/core.hpp>", "",
             "namespace core = galoot::core;", "", "namespace {", "",
             "struct alg {",
             f"  static constexpr auto value = core::rigid<{n - 1}>();",
             "};", ""]
    for name, names in RIGID.items():
        lines.append(f"constexpr auto {name}_layout = {layout(names, n)};")
    lines.append("")
    for name in RIGID:
        lines.append(f"using {name.capitalize()} = "
                     f"core::mv<alg, {name}_layout, double>;")
    lines += ["",
              "template <class Mv>",
              "Mv read(const double*& in) {",
              "  Mv x;",
              "  for (double& c : x.coefficients) {",
              "    c = *in++;",
              "  }",
              "  return x;",
              "}", "",
              "template <class Mv>",
              "double sum(const Mv& x) {",
              "  double s = 0;",
              "  for (const double c : x.coefficients) {",
              "    s += c;",
              "  }",
              "  return s;",
              "}", "",
              "} // namespace", "",
              "double use(const double* in);", "",
              "double use(const double* in) {"]
    for type_name, variable in OPERANDS:
        lines.append(f"  const auto {variable} = read<{type_name}>(in);")
    terms = [f"sum({v})" for _, v in OPERANDS]
    terms += [f"sum({e})" for e in expressions]
    lines.append("  return " + " +\n         ".join(terms) + ";")
    lines.append("}")
    return "\n".join(lines) + "\n"


def toml_unit(name, text):
    assert "'''" not in text
    return (f'[[unit]]\nname = "{name}"\ntext = \'\'\'\n{text}\'\'\'\n'
            'trees = ["after"]\nkind = "debug"\n')


def main():
    out = ["# Written by generate.py; do not edit.",
           'compilers = ["gcc", "clang"]', f'flags_from = "{FLAGS_FROM}"', ""]
    for n in SIZES:
        for use, expressions in USES.items():
            out.append(toml_unit(f"n{n}/{use}", unit(n, expressions)))
    for k in (1, 4, 16):
        out.append(toml_unit(f"n4/distinct{k}", unit(4, DISTINCT[:k])))
    sys.stdout.write("\n".join(out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
