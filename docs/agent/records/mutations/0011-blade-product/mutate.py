"""Mutation runs for blade_product.hpp: apply one mutation, build the blade
product test and its abort program, run the core.blade_product selection
(the doctest program, the compile-fail tests, and the run-time aborts), and
restore the header. A mutant is killed when the build fails or any test
fails; the build status is recorded before the tests, because CTest runs
whatever program is on disk. Each mutation's anchor must occur exactly once.

For a failed build the record gives the first error line and the lines of
blade_product_test.cpp whose static_assert failed, so that a kill by a
static_assert can be told from a kill by another error. The two programs are
deleted before each build, so that a failed build leaves none for CTest to
run.

With --runtime, the static_asserts of the test file that check products are
turned off for the run (each becomes `true || ...`), so that the doctest
program builds on a mutant they would reject, and the record lists the test
cases that fail at run time. That shows which run-time checks kill each
mutant, such as the wedge-monomial check for the dropped correction.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0011-blade-product/mutate.py \\
      <preset> [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
HEADER = f"{ROOT}/include/galoot/core/blade_product.hpp"
TEST = f"{ROOT}/test/core/blade_product_test.cpp"
PROGRAMS = ("core_blade_product_test", "core_blade_product_assert_fail")

# The static_asserts of the test file that a behavioral mutant can fail, each
# turned off in --runtime mode, with the number of times its anchor occurs.
RUNTIME_EDITS = [
    ("static_assert(table_mismatches(core::conformal<3>(),",
     "static_assert(true || table_mismatches(core::conformal<3>(),", 1),
    ("static_assert(added(", "static_assert(true || added(", 3),
    ("static_assert(core::detail::blade_product<1>(widest, top, top)",
     "static_assert(true || core::detail::blade_product<1>(widest, top, top)",
     1),
]

MUTATIONS = [
    # Signs of the recursion.
    ("wedge_sign_side", "multiply_checked(reorder_sign(generator, blade), c)",
     "multiply_checked(reorder_sign(blade, generator), c)"),
    ("wedge_sign_dropped", "multiply_checked(reorder_sign(generator, blade), c)",
     "c"),
    ("contraction_first_sign", "    coefficient_type sign = 1;\n",
     "    coefficient_type sign = -1;\n"),
    ("contraction_sign_constant", "      sign = -sign;\n    }\n  }\n}",
     "    }\n  }\n}"),
    ("correction_sign", "  coefficient_type sign = -1;\n",
     "  coefficient_type sign = 1;\n"),
    ("correction_sign_constant", "    sign = -sign;\n  }\n}\n", "  }\n}\n"),
    ("correction_dropped",
     "      add_blade_product(out, g, rest & ~(mask_t{1} << j), b,\n",
     "      if (false) add_blade_product(out, g, rest & ~(mask_t{1} << j), b,\n"),
    # Equivalent on every symmetric form, which the header requires.
    ("contraction_transposed", "g.gram[i][j]", "g.gram[j][i]"),
    ("correction_transposed", "g.gram[low][j]", "g.gram[j][low]"),
    # The structure of the recursion.
    ("lowest_generator_highest",
     "const auto low = static_cast<std::size_t>(std::countr_zero(a));\n"
     "  const mask_t rest = a & (a - 1);",
     "const auto low = static_cast<std::size_t>(std::bit_width(a) - 1);\n"
     "  const mask_t rest = a & ~(mask_t{1} << low);"),
    ("scalar_scale_dropped", "    out.add(b, scale);\n", "    out.add(b, 1);\n"),
    ("generator_scale_dropped", "multiply_checked(x.terms[k].coefficient, scale)",
     "x.terms[k].coefficient"),
    # term_list::add.
    ("zero_kept", "    if (coefficient == 0) {\n      return;\n    }\n", ""),
    ("zero_sum_kept", "      if (terms[at].coefficient == 0) {",
     "      if (false) {"),
    ("trailing_place_kept", "        --length;\n        terms[length] = {};\n",
     "        --length;\n"),
    ("merge_dropped", "if (at < length && terms[at].blade == blade) {",
     "if (false) {"),
    ("append", "while (at < length && canonical_less(terms[at].blade, blade)) {",
     "while (at < length) {"),
    ("numeric_order", "canonical_less(terms[at].blade, blade)",
     "terms[at].blade < blade"),
    # Guards.
    ("capacity_check_removed", "GALOOT_ASSERT(length < Capacity,",
     "GALOOT_ASSERT(true || length < Capacity,"),
    ("capacity_off_by_one", "GALOOT_ASSERT(length < Capacity,",
     "GALOOT_ASSERT(length <= Capacity,"),
    ("product_guard_removed",
     "const bool fits = magnitude_x == 0 || magnitude_y <= max / magnitude_x;",
     "const bool fits = true;"),
    ("product_guard_strict", "magnitude_y <= max / magnitude_x",
     "magnitude_y < max / magnitude_x"),
    ("product_magnitude_x", "magnitude_x = x < 0 ? -x : x;", "magnitude_x = x;"),
    ("product_magnitude_y", "magnitude_y = y < 0 ? -y : y;", "magnitude_y = y;"),
    ("sum_guard_removed",
     "const bool fits = y > 0 ? x <= max - y : x >= -max - y;",
     "const bool fits = true;"),
    ("sum_guard_upper_strict", "x <= max - y :", "x < max - y :"),
    ("sum_guard_lower_strict", "x >= -max - y;", "x > -max - y;"),
    ("sum_guard_lower_minimum", "x >= -max - y;", "x >= -max - y - 1;"),
    # Equivalent when x lies within -max to max, which add_checked requires.
    ("sum_guard_branch", "y > 0 ? x <= max - y", "y >= 0 ? x <= max - y"),
    ("range_check_removed", "GALOOT_ASSERT(in_range,",
     "GALOOT_ASSERT(true || in_range,"),
    ("range_shift_off_by_one", "in_range = ((a | b) >> N) == 0;",
     "in_range = ((a | b) >> (N + 1)) == 0;"),
    ("range_widest_shifted", "if constexpr (N < mask_bits) {",
     "if constexpr (N <= mask_bits) {"),
    ("capacity_formula", "static_cast<unsigned>(generators - 1)",
     "static_cast<unsigned>(generators)"),
    ("capacity_scalar", "             ? 1\n", "             ? 2\n"),
    # The aggregate and trivially copyable properties.
    ("term_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const term&) const = default;\n",
     "  [[nodiscard]] constexpr bool operator==(const term&) const = default;\n"
     "  constexpr ~term() {}\n"),
    ("term_list_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const term_list&) const = default;\n",
     "  [[nodiscard]] constexpr bool operator==(const term_list&) const = default;\n"
     "  constexpr ~term_list() {}\n"),
    ("term_not_aggregate", "struct term {\n",
     "struct term {\n  term() = default;\n"),
    ("term_list_not_aggregate", "struct term_list {\n",
     "struct term_list {\n  term_list() = default;\n"),
    ("term_list_not_structural", "  std::size_t length;\n",
     "  mutable std::size_t length;\n"),
]

STATIC_ASSERT_LINE = re.compile(
    r"blade_product_test\.cpp:(\d+):\d+: error: (?:static assertion|static_assert)")


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def failed_cases(output):
    """The names of the doctest cases that report a failure."""
    names = []
    for line in output.splitlines():
        if line.startswith("TEST CASE:"):
            name = line[len("TEST CASE:"):].strip()
            if name not in names:
                names.append(name)
    return names


def main():
    preset = sys.argv[1]
    runtime = "--runtime" in sys.argv[2:]
    only = {a for a in sys.argv[2:] if a != "--runtime"}
    original = open(HEADER, encoding="utf-8").read()
    original_test = open(TEST, encoding="utf-8").read()
    if runtime:
        edited = original_test
        for anchor, replacement, count in RUNTIME_EDITS:
            assert edited.count(anchor) == count, (anchor, edited.count(anchor))
            edited = edited.replace(anchor, replacement)
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(edited)
    binary_dir = f"{ROOT}/build/{preset}/test"
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled\t"
          "static_assert_lines\tfailed_cases\tfirst_build_error")
    try:
        for name, anchor, replacement in MUTATIONS:
            if only and name not in only:
                continue
            assert original.count(anchor) == 1, (name, original.count(anchor))
            with open(HEADER, "w", encoding="utf-8") as f:
                f.write(original.replace(anchor, replacement))
            for program in PROGRAMS:
                subprocess.run(["rm", "-f", "--", f"{binary_dir}/{program}"],
                               check=True)
            build = run(["tools/timed", "build", preset, f"mutation:{name}",
                         "--", "cmake", "--build", "--preset", preset,
                         "--target", "core_blade_product_test",
                         "core_blade_product_assert_fail"])
            test = run(["tools/timed", "test", preset, f"mutation:{name}", "--",
                        "ctest", "--preset", preset, "-L",
                        r"^core\.blade_product$", "--output-on-failure"])
            failed = [line.split(" - ")[1].split()[0]
                      for line in test.stdout.splitlines() if " - core." in line]
            killed = build.returncode != 0 or test.returncode != 0
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            lines = sorted({int(m.group(1)) for m in
                            STATIC_ASSERT_LINE.finditer(build.stdout)})
            print(f"{name}\t{preset}\t{build.returncode}\t{test.returncode}\t"
                  f"{','.join(failed) or '-'}\t"
                  f"{'killed' if killed else 'SURVIVED'}\t"
                  f"{','.join(map(str, lines)) or '-'}\t"
                  f"{' | '.join(failed_cases(test.stdout)) or '-'}\t"
                  f"{first_error[:200]}",
                  flush=True)
    finally:
        with open(HEADER, "w", encoding="utf-8") as f:
            f.write(original)
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(original_test)


main()
