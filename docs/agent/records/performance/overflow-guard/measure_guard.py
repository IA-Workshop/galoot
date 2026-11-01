#!/usr/bin/env -S python3 -I
"""Measure what the coefficient guards of blade_product.hpp cost in constant
evaluation.

Usage, from the repository root: measure_guard.py <out-dir> [repeats] [cpu]

Each compile constant-evaluates the full product table of one n = 5 form, all
32 x 32 products of blades with a term list of capacity 16, and checks the
number of terms in a static_assert. Two forms: conformal(3), with at most two
terms per product and 1,024 terms in all, and seeded random form 3 of
clifford_reference.py, dense, with up to 14 terms per product and 5,326 in
all. Each is compiled with the header as committed and with a copy whose
add_checked and multiply_checked return x + y and x * y with no check, under
<out-dir>/unguarded/include. The flags are Galoot's test flags with
-fsyntax-only and the constant-evaluation limits raised, so that every point
measures cost rather than failing at a limit.

Every compile runs through tools/timed, which appends its row (wall time,
peak memory, retired instructions on both kinds of core) to the run log,
pinned to `cpu` (default 6) with taskset. The selection names the point:
overflow-guard:<compiler>:<form>:<guarded|unguarded>:<repeat>.
"""

import pathlib
import shutil
import subprocess
import sys

COMPILERS = {
    "gcc": ["g++", "-fconstexpr-ops-limit=1099511627776"],
    "clang": ["clang++", "-fconstexpr-steps=0"],
}
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-DGALOOT_CHECKING=1", "-O0", "-fsyntax-only"]
FORMS = {
    "conformal3": ("galoot::core::conformal<3>()", 1024),
    "random3": ("galoot::core::from_gram({{3, 1, -3, -1, -2}, "
                "{1, -3, -2, 1, 0}, {-3, -2, -2, 3, 0}, {-1, 1, 3, 3, 2}, "
                "{-2, 0, 0, 2, -1}})", 5326),
}

UNIT = """#include <cstddef>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

namespace {{

constexpr auto g = {form};

consteval std::size_t table_terms() {{
  std::size_t count = 0;
  for (galoot::core::mask_t a = 0; a < 32; ++a) {{
    for (galoot::core::mask_t b = 0; b < 32; ++b) {{
      count += galoot::core::detail::blade_product<16>(g, a, b).size();
    }}
  }}
  return count;
}}

static_assert(table_terms() == {terms});

}} // namespace
"""

# Each guard's check, removed with the comment and constant that serve it,
# so that the unguarded copy does only the arithmetic.
GUARD_EDITS = [
    ("""  // Neither bound can overflow: max - y for 0 < y <= max, and -max - y for
  // -max <= y <= 0, both lie within -max to max.
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  const bool fits = y > 0 ? x <= max - y : x >= -max - y;
  GALOOT_ASSERT(fits, "a product coefficient does not fit coefficient_type");
  return x + y;""", "  return x + y;"),
    ("""  // For |x| > 0, |x| |y| <= max exactly when |y| <= max / |x| rounded down.
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  const coefficient_type magnitude_x = x < 0 ? -x : x;
  const coefficient_type magnitude_y = y < 0 ? -y : y;
  const bool fits = magnitude_x == 0 || magnitude_y <= max / magnitude_x;
  GALOOT_ASSERT(fits, "a product coefficient does not fit coefficient_type");
  return x * y;""", "  return x * y;"),
]


def unguarded_include(root, out):
    include = out / "unguarded" / "include"
    if include.exists():
        shutil.rmtree(include)
    shutil.copytree(root / "include", include)
    header = include / "galoot" / "core" / "blade_product.hpp"
    text = header.read_text(encoding="utf-8")
    for anchor, replacement in GUARD_EDITS:
        assert text.count(anchor) == 1, anchor
        text = text.replace(anchor, replacement)
    header.write_text(text, encoding="utf-8")
    return include


def main(argv):
    root = pathlib.Path(subprocess.run(
        ["git", "rev-parse", "--show-toplevel"], capture_output=True,
        text=True, check=True).stdout.strip())
    out = pathlib.Path(argv[0]).resolve()
    repeats = int(argv[1]) if len(argv) > 1 else 3
    cpu = argv[2] if len(argv) > 2 else "6"
    out.mkdir(parents=True, exist_ok=True)
    includes = {"guarded": root / "include",
                "unguarded": unguarded_include(root, out)}
    status = 0
    for repeat in range(1, repeats + 1):
        for form, (expression, terms) in FORMS.items():
            source = out / f"table_{form}.cpp"
            source.write_text(UNIT.format(form=expression, terms=terms),
                              encoding="utf-8")
            for compiler, (program, *limits) in COMPILERS.items():
                for guard, include in includes.items():
                    selection = (f"overflow-guard:{compiler}:{form}:{guard}:"
                                 f"{repeat}")
                    command = [str(root / "tools" / "timed"), "measure", "-",
                               selection, "--", "taskset", "-c", cpu, program,
                               *FLAGS, *limits, f"-I{include}", str(source)]
                    result = subprocess.run(command, cwd=root,
                                            capture_output=True, text=True)
                    print(f"{selection}\texit {result.returncode}", flush=True)
                    if result.returncode != 0:
                        print(result.stderr[-2000:], file=sys.stderr)
                        status = 1
    return status


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
