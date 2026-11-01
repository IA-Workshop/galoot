"""Mutation runs for plan.hpp: apply one mutation, build the plan test, run
the core.plan selection (the doctest program and the compile-fail tests),
and restore the header. A mutant is killed when the build fails or any test
fails; the build status is recorded before the tests, because CTest runs
whatever program is on disk. Each mutation's anchor must occur exactly once.

For a failed build the record gives the first error line and the lines of
plan.hpp and plan_test.cpp whose static_assert failed, so that a kill by a
static_assert can be told from a kill by another error. The program is
deleted before each build, so that a failed build cannot leave an old program
behind, and CTest is not run after a failed build: the mutant is killed
already, and CTest 4.4 run in parallel under the shared job pool can keep a
slot it took for a test whose program is missing. After the last mutant the
header is restored and the program is built again.

With --runtime, the static_asserts of the test file that check plans, and the
checks of a generated plan in the header, are turned off for the run (each
becomes `true || ...`), so that the doctest program builds on a mutant they
would reject, and the record lists the test cases that fail at run time. The
checks of the two sets before generation stay, because the compile-fail
tests are what judge them.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0011-gp-plan/mutate.py \\
      <preset> [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
HEADER = f"{ROOT}/include/galoot/core/plan.hpp"
TEST = f"{ROOT}/test/core/plan_test.cpp"
PROGRAM = "core_plan_test"

# The static_asserts turned off in --runtime mode, with the number of times
# each anchor occurs.
RUNTIME_TEST_EDITS = [
    ("static_assert(gp_plan<", "static_assert(true || gp_plan<", 8),
    ("static_assert(follows_diagonal_rule(",
     "static_assert(true || follows_diagonal_rule(", 3),
    ("static_assert(folds_signs(", "static_assert(true || folds_signs(", 2),
    ("static_assert(\n    equals_generation<",
     "static_assert(\n    true || equals_generation<", 3),
]
RUNTIME_HEADER_EDITS = [
    ("static_assert(!defects.", "static_assert(true || !defects.", 5),
]

MUTATIONS = [
    # Sign folding: both inputs' signs.
    # Each sign times itself is 1, so the signs still read but fold nothing.
    ("sign_fold_dropped",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[a].sign * as[a].sign) *\n"
     "          (bs[b].sign * bs[b].sign);"),
    ("sign_first_only",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[a].sign);"),
    ("sign_second_only",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(bs[b].sign);"),
    ("sign_first_twice",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[a].sign) * as[a].sign;"),
    ("sign_by_sum",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[a].sign) + bs[b].sign;"),
    # Positions and output blades.
    ("positions_swapped", ".a = a,\n              .b = b,",
     ".a = b,\n              .b = a,"),
    ("a_off_by_one", ".a = a,", ".a = a + 1,"),
    ("b_off_by_one", ".b = b,", ".b = b + 1,"),
    ("out_shifted", ".out = product.terms[k].blade,",
     ".out = product.terms[k].blade << 1U,"),
    ("out_xor", ".out = product.terms[k].blade,",
     ".out = as[a].mask ^ bs[b].mask,"),
    # Order of terms.
    ("loops_swapped",
     "  for (std::size_t a = 0; a < as.size(); ++a) {\n"
     "    for (std::size_t b = 0; b < bs.size(); ++b) {",
     "  for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "    for (std::size_t a = 0; a < as.size(); ++a) {"),
    ("terms_reversed", "for (std::size_t k = 0; k < product.size(); ++k) {",
     "for (std::size_t r = 0, k = product.size() - 1; r < product.size();"
     " ++r, --k) {"),
    ("first_blade_skipped", "for (std::size_t a = 0; a < as.size(); ++a) {",
     "for (std::size_t a = 1; a < as.size(); ++a) {"),
    ("last_pair_skipped", "for (std::size_t b = 0; b < bs.size(); ++b) {",
     "for (std::size_t b = 0; b + 1 < bs.size(); ++b) {"),
    # Repeated blades: a place whose mask the first place has is skipped.
    ("repeat_skipped",
     "    for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "      const term_list<Capacity> product =",
     "    if (a > 0 && as[a].mask == as[0].mask) {\n      continue;\n    }\n"
     "    for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "      const term_list<Capacity> product ="),
    # The counting pass and partial output.
    ("count_plus_one", "  return count;\n}", "  return count + 1;\n}"),
    ("count_only_written", "          };\n        }\n        ++count;\n",
     "          };\n          ++count;\n        }\n"),
    ("write_any_nonempty", "if (count < out.size()) {", "if (!out.empty()) {"),
    ("write_bound_inclusive", "if (count < out.size()) {",
     "if (count <= out.size()) {"),
    # plan_capacity: empty sets and capacity edges.
    ("capacity_start_zero", "  std::size_t capacity = 1;\n",
     "  std::size_t capacity = 0;\n"),
    ("capacity_last_pair", "      if (pair > capacity) {", "      if (true) {"),
    ("capacity_intersection", "product_capacity(x.mask, y.mask)",
     "product_capacity(x.mask & y.mask, x.mask & y.mask)"),
    ("capacity_halved", "        capacity = pair;\n",
     "        capacity = pair > 1 ? pair / 2 : pair;\n"),
    # A fill that differs from generate_gp in a way no check of a generated
    # plan sees.
    ("fill_negated", "  generate_gp<Capacity>(g, as, bs, plan);\n  return plan;",
     "  generate_gp<Capacity>(g, as, bs, plan);\n"
     "  for (plan_term& t : plan) {\n    t.coefficient = -t.coefficient;\n  }\n"
     "  return plan;"),
    ("fill_sets_swapped", "  generate_gp<Capacity>(g, as, bs, plan);",
     "  generate_gp<Capacity>(g, bs, as, plan);"),
    # Checks of the sets before generation.
    ("range_first_only", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(A, n)"),
    ("range_second_only", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(B, n)"),
    ("range_check_removed", "static_assert(in_range,",
     "static_assert(true || in_range,"),
    ("range_first_blade_only",
     "    if (!in_algebra(blade.mask, n)) {\n      return false;\n    }\n",
     "    if (!in_algebra(blade.mask, n)) {\n      return false;\n    }\n"
     "    return true;\n"),
    ("range_rejects_empty", "    }\n  }\n  return true;\n}",
     "    }\n  }\n  return K > 0;\n}"),
    ("sign_check_first_only", "signs_valid<A> && signs_valid<B>",
     "signs_valid<A>"),
    ("sign_check_second_only", "signs_valid<A> && signs_valid<B>",
     "signs_valid<B>"),
    ("sign_check_removed", "static_assert(valid_signs,",
     "static_assert(true || valid_signs,"),
    ("generation_unguarded", "if constexpr (in_range && valid_signs) {",
     "if constexpr (true) {"),
    # Checks of a generated plan. Removing one is equivalent on a correct
    # generator; the mutants above that break the generator fire them.
    ("zero_check_removed", "static_assert(!defects.zero_coefficient,",
     "static_assert(true || !defects.zero_coefficient,"),
    ("first_check_removed", "static_assert(!defects.beyond_first,",
     "static_assert(true || !defects.beyond_first,"),
    ("second_check_removed", "static_assert(!defects.beyond_second,",
     "static_assert(true || !defects.beyond_second,"),
    ("beyond_n_check_removed", "static_assert(!defects.beyond_n,",
     "static_assert(true || !defects.beyond_n,"),
    ("order_check_removed", "static_assert(!defects.out_of_order,",
     "static_assert(true || !defects.out_of_order,"),
    ("first_bound_strict", "t.a >= first", "t.a > first"),
    ("second_bound_strict", "t.b >= second", "t.b > second"),
    ("zero_test_wrong", "t.coefficient == 0;", "t.coefficient == 1;"),
    ("beyond_n_test_one_more", "!in_algebra(t.out, n);",
     "!in_algebra(t.out, n == mask_bits ? n : n + 1);"),
    ("order_allows_repeat", "canonical_less(before.out, t.out)",
     "!canonical_less(t.out, before.out)"),
    ("order_same_b", "(before.a == t.a && before.b < t.b)",
     "(before.a == t.a && before.b <= t.b)"),
    ("order_same_a", "before.a < t.a ||", "before.a <= t.a ||"),
    # The form of the locals: equivalent in result, a cost measured apart.
    ("locals_plain", "    static constexpr auto plan =\n",
     "    constexpr auto plan =\n"),
    # The aggregate and trivially copyable properties.
    ("plan_term_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const plan_term&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const plan_term&) const = "
     "default;\n  constexpr ~plan_term() {}\n"),
    ("plan_term_not_aggregate", "struct plan_term {\n",
     "struct plan_term {\n  plan_term() = default;\n"),
]

STATIC_ASSERT_LINE = re.compile(
    r"(plan_test\.cpp|plan\.hpp):(\d+):\d+: error: "
    r"(?:static assertion|static_assert)")


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def failed_cases(output):
    names = []
    for line in output.splitlines():
        if line.startswith("TEST CASE:"):
            name = line[len("TEST CASE:"):].strip()
            if name not in names:
                names.append(name)
    return names


def edited(text, edits):
    for anchor, replacement, count in edits:
        assert text.count(anchor) == count, (anchor, text.count(anchor))
        text = text.replace(anchor, replacement)
    return text


def main():
    preset = sys.argv[1]
    runtime = "--runtime" in sys.argv[2:]
    only = {a for a in sys.argv[2:] if a != "--runtime"}
    original = open(HEADER, encoding="utf-8").read()
    original_test = open(TEST, encoding="utf-8").read()
    base = edited(original, RUNTIME_HEADER_EDITS) if runtime else original
    if runtime:
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(edited(original_test, RUNTIME_TEST_EDITS))
    binary_dir = f"{ROOT}/build/{preset}/test"
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled\t"
          "static_assert_lines\tfailed_cases\tfirst_build_error")
    try:
        for name, anchor, replacement in MUTATIONS:
            if only and name not in only:
                continue
            if runtime and base.count(anchor) == 0:
                # A check that --runtime turns off cannot be removed again.
                assert original.count(anchor) == 1, (name, anchor)
                print(f"{name}\t{preset}\t-\t-\t-\tskipped\t-\t-\t"
                      "the check is off in this mode", flush=True)
                continue
            assert base.count(anchor) == 1, (name, base.count(anchor))
            with open(HEADER, "w", encoding="utf-8") as f:
                f.write(base.replace(anchor, replacement))
            subprocess.run(["rm", "-f", "--", f"{binary_dir}/{PROGRAM}"],
                           check=True)
            build = run(["tools/timed", "build", preset, f"mutation:{name}",
                         "--", "cmake", "--build", "--preset", preset,
                         "--target", PROGRAM])
            if build.returncode == 0:
                test = run(["tools/timed", "test", preset, f"mutation:{name}",
                            "--", "ctest", "--preset", preset, "-L",
                            r"^core\.plan$", "--output-on-failure"])
                test_exit, output = test.returncode, test.stdout
            else:
                test_exit, output = "-", ""
            failed = [line.split(" - ")[1].split()[0]
                      for line in output.splitlines() if " - core." in line]
            killed = build.returncode != 0 or test_exit != 0
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            lines = sorted({f"{m.group(1)}:{m.group(2)}" for m in
                            STATIC_ASSERT_LINE.finditer(build.stdout)})
            print(f"{name}\t{preset}\t{build.returncode}\t{test_exit}\t"
                  f"{','.join(failed) or '-'}\t"
                  f"{'killed' if killed else 'SURVIVED'}\t"
                  f"{','.join(lines) or '-'}\t"
                  f"{' | '.join(failed_cases(output)) or '-'}\t"
                  f"{first_error[:200]}",
                  flush=True)
    finally:
        with open(HEADER, "w", encoding="utf-8") as f:
            f.write(original)
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(original_test)
        run(["tools/timed", "build", preset, "mutation:restored", "--", "cmake",
             "--build", "--preset", preset, "--target", PROGRAM])


main()
