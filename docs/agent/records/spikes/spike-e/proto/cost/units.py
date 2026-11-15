"""Write the compile-cost units of the collection pass from cases.json (which
dump.cpp prints), and the tools/compiles spec that measures them.

    python3 -I units.py

For each case, into units/:
  <case>.baseline.cpp  the algebra and the two layouts, read by one trivial
                       evaluation
  <case>.plan.cpp      the transform's two kernels: the library's plans of
                       both antiproducts and the kernels drawn from them
  <case>.pass.cpp      the plan, and the collection pass on its kernels
  <case>.literal.cpp   the kernels as literals, then the pass on them: the
                       pass's own evaluations, as a phase probe holds its
                       inputs as literals
  <case>.kernels.cpp   the kernels as literals only: the literal unit's
                       baseline
and units.toml, the spec of `tools/compiles cost` for the baseline, plan,
and pass units of every case, from the debug trees, with units-gcc.toml
for the one unit Clang refuses at its default step limit.
"""

import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
UNITS = HERE / "units"

ALGEBRA = {
    "rigid3": "core::rigid<3>()",
    "conformal3": "core::conformal<3>()",
}


def blade_set(blades):
    inner = ", ".join(f"{{{m}U, {s}}}" for m, s in blades)
    return f"core::blade_set<{len(blades)}>{{{{{{{inner}}}}}}}"


def kernel(name, terms):
    rows = ",\n".join(f"    {{{a}, {b}, {o}, {c}, {'true' if f else 'false'}}}"
                      for a, b, o, c, f in terms)
    return (f"constexpr std::array<detail::kernel_term, {len(terms)}> {name}"
            f"{{{{\n{rows},\n}}}};\n")


def prelude(case, include_pass, layouts=True):
    lines = ["#include <array>", "#include <cstddef>", "",
             "#include <galoot/core.hpp>", ""]
    if include_pass:
        lines += ['#include "../../collect.hpp"', ""]
    lines += ["namespace core = galoot::core;",
              "namespace detail = galoot::core::detail;", "",
              "struct alg {",
              f"  static constexpr auto value = {ALGEBRA[case['algebra']]};",
              "};", ""]
    if layouts:
        lines += [f"constexpr auto versor = {blade_set(case['versor'])};",
                  f"constexpr auto object = {blade_set(case['object'])};", ""]
    return "\n".join(lines) + "\n"


BASELINE = """\
static_assert(versor.size() + object.size() > 0);
"""

PLAN = """\
static_assert(detail::first_kernel<alg, versor, object, false>.size() > 0);
static_assert(detail::second_kernel<alg, versor, object, true>.size() > 0);
"""

PASS = """\
static_assert(
    collect::collected_transform<alg, versor, object>.products.size() > 0);
"""


def literal(case, with_pass):
    v, x, m = len(case["versor"]), len(case["object"]), len(case["middle"])
    text = prelude(case, with_pass, layouts=False)
    text += kernel("first", case["first"]) + "\n"
    text += kernel("second", case["second"]) + "\n"
    if with_pass:
        text += (f"constexpr auto summary =\n    collect::summarize<{v}, {x}, "
                 f"{m}>(first, second);\n"
                 f"constexpr auto result =\n    collect::make_collected<"
                 f"summary, {v}, {x}>();\n"
                 "static_assert(result.products.size() > 0);\n")
    else:
        text += "static_assert(first.size() + second.size() > 0);\n"
    return text


CLANG_REFUSES = {("cga3_even_grade2", "pass")}


def main():
    cases = json.loads((HERE / "cases.json").read_text())
    UNITS.mkdir(exist_ok=True)
    spec = ['# Written by units.py; do not edit.',
            'compilers = ["gcc", "clang"]',
            'flags_from = "core.assert_consteval"', ""]
    gcc_only = ['# Written by units.py; do not edit.', 'compilers = ["gcc"]',
                'flags_from = "core.assert_consteval"', ""]
    for name, case in cases.items():
        units = {
            "baseline": prelude(case, False) + BASELINE,
            "plan": prelude(case, False) + PLAN,
            "pass": prelude(case, True) + PLAN + PASS,
            "literal": literal(case, True),
            "kernels": literal(case, False),
        }
        for kind, text in units.items():
            (UNITS / f"{name}.{kind}.cpp").write_text(text)
        for kind in ("baseline", "plan", "pass"):
            source = (UNITS / f"{name}.{kind}.cpp").relative_to(
                HERE.parents[6])
            unit = ["[[unit]]", f'name = "{name}/{kind}"',
                    f'source = "{source}"', 'trees = ["after"]',
                    'kind = "debug"', ""]
            # Clang refuses this one at its default step limit.
            if (name, kind) in CLANG_REFUSES:
                gcc_only += unit
            else:
                spec += unit
    (HERE / "units.toml").write_text("\n".join(spec))
    (HERE / "units-gcc.toml").write_text("\n".join(gcc_only))
    print(f"{len(cases)} cases, {5 * len(cases)} units")


if __name__ == "__main__":
    main()
