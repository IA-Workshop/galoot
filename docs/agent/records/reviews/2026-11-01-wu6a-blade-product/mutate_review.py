"""The W5 reviewer's own mutants of blade_product.hpp, run by the driver of
records/mutations/0011-blade-product/mutate.py (copied unchanged apart from
the MUTATIONS list). Mutation runs for blade_product.hpp: apply one mutation, build the blade
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
  python3 -I docs/agent/records/reviews/2026-11-01-wu6a-blade-product/mutate_review.py \\
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
    ("r_range_b_unchecked", "return ((a | b) >> N) == 0;", "return (a >> N) == 0;"),
    ("r_range_and", "return ((a | b) >> N) == 0;", "return ((a & b) >> N) == 0;"),
    ("r_range_skip_below_top", "if constexpr (N < mask_bits) {", "if constexpr (N + 1 < mask_bits) {"),
    ("r_range_widest_false", "      return true;\n    }\n  }();", "      return false;\n    }\n  }();"),
    ("r_contraction_toggle_on_nonzero",
     "                multiply_checked(multiply_checked(sign, entry), c));\n      }\n      sign = -sign;",
     "                multiply_checked(multiply_checked(sign, entry), c));\n        sign = -sign;\n      }"),
    ("r_correction_toggle_on_nonzero",
     "                        multiply_checked(multiply_checked(sign, entry), scale));\n    }\n    sign = -sign;",
     "                        multiply_checked(multiply_checked(sign, entry), scale));\n      sign = -sign;\n    }"),
    ("r_correction_over_a", "if (((rest >> j) & 1U) == 0U) {", "if (((a >> j) & 1U) == 0U) {"),
    ("r_contraction_removes_i", "out.add(blade & ~(mask_t{1} << j),", "out.add(blade & ~generator,"),
    ("r_correction_keeps_j", "add_blade_product(out, g, rest & ~(mask_t{1} << j), b,", "add_blade_product(out, g, rest, b,"),
    ("r_wedge_always", "if ((blade & generator) == 0) {", "if (true) {"),
    ("r_wedge_never", "if ((blade & generator) == 0) {", "if (false) {"),
    ("r_unguarded_scale", "const coefficient_type c = multiply_checked(x.terms[k].coefficient, scale);",
     "const coefficient_type c = x.terms[k].coefficient * scale;"),
    ("r_unguarded_contraction", "multiply_checked(multiply_checked(sign, entry), c)", "sign * entry * c"),
    ("r_unguarded_correction", "multiply_checked(multiply_checked(sign, entry), scale)", "sign * entry * scale"),
    ("r_unguarded_sum", "terms[at].coefficient = add_checked(terms[at].coefficient, coefficient);",
     "terms[at].coefficient += coefficient;"),
    ("r_unguarded_wedge_sign", "multiply_checked(reorder_sign(generator, blade), c)", "reorder_sign(generator, blade) * c"),
    ("r_unguarded_inner_contraction", "multiply_checked(multiply_checked(sign, entry), c)", "multiply_checked(sign * entry, c)"),
    ("r_unguarded_inner_correction", "multiply_checked(multiply_checked(sign, entry), scale)", "multiply_checked(sign * entry, scale)"),
    ("r_product_zero_test_on_y", "magnitude_x == 0 ||", "magnitude_y == 0 ||"),
    ("r_product_divides_by_y", "magnitude_y <= max / magnitude_x;", "magnitude_x <= max / magnitude_y;"),
    ("r_product_returns_magnitude", "  return x * y;\n", "  return magnitude_x * y;\n"),
    ("r_sum_lower_bound_fixed", "x >= -max - y;", "x >= -max;"),
    ("r_sum_upper_bound_fixed", "x <= max - y :", "x <= max :"),
    ("r_removal_reads_past_end", "for (std::size_t j = at; j + 1 < length; ++j) {", "for (std::size_t j = at; j < length; ++j) {"),
    ("r_removal_start_late", "for (std::size_t j = at; j + 1 < length; ++j) {", "for (std::size_t j = at + 1; j + 1 < length; ++j) {"),
    ("r_insert_shift_short", "for (std::size_t j = length; j > at; --j) {", "for (std::size_t j = length; j > at + 1; --j) {"),
    ("r_search_past_equal", "while (at < length && canonical_less(terms[at].blade, blade)) {",
     "while (at < length && !canonical_less(blade, terms[at].blade)) {"),
    ("r_capacity_xor", "const int generators = grade(a | b);", "const int generators = grade(a ^ b);"),
    ("r_capacity_sum_of_grades", "const int generators = grade(a | b);", "const int generators = grade(a) + grade(b);"),
    ("r_rest_scaled_twice", "add_blade_product(rest_product, g, rest, b, 1);", "add_blade_product(rest_product, g, rest, b, scale);"),
    ("r_factors_swapped", "  add_blade_product(out, g, a, b, 1);\n  return out;", "  add_blade_product(out, g, b, a, 1);\n  return out;"),
    ("r_partial_sum_excursion", "    out.add(b, scale);\n    return;",
     "    out.add(b ^ 1U, 1);\n    out.add(b, scale);\n    out.add(b ^ 1U, -1);\n    return;"),
    ("r_zero_removal_keeps_length", "        --length;\n        terms[length] = {};\n", "        terms[length - 1] = {};\n"),
    ("r_runtime_diverges", "  add_blade_product(out, g, a, b, 1);\n  return out;",
     "  add_blade_product(out, g, a, b, 1);\n  if !consteval {\n    if (a == 0b1111U && b == 0b1111U && N == 4) {\n      out.add(0b1U, 1);\n    }\n  }\n  return out;"),
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
