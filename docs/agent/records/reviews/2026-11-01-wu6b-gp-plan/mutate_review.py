"""The reviewer's mutants of plan.hpp, run with the author's driver.

Reads docs/agent/records/mutations/0011-gp-plan/mutate.py, replaces its list
of mutations with the one below, and runs it, so the build, the CTest
selection (the core.plan label), the --runtime mode, and the restore are the
author's. The results in out/ came from two runs: the first list without the
last three entries, and then the last three (an earlier form of
r_capacity_first_operand_only left a variable unused and was killed only by
-Werror; it was rewritten).

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/reviews/2026-11-01-wu6b-gp-plan/mutate_review.py \
      <preset> [--runtime] [mutation...]
"""
import pathlib
import subprocess

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
DRIVER = pathlib.Path(ROOT) / "docs/agent/records/mutations/0011-gp-plan/mutate.py"

MUTATIONS = [

    # The capacity make_gp_plan passes to the generator.
    ("r_make_capacity_one",
     "static constexpr std::size_t capacity = plan_capacity(A.blades, B.blades);",
     "static constexpr std::size_t capacity = 1;"),
    ("r_make_capacity_two",
     "static constexpr std::size_t capacity = plan_capacity(A.blades, B.blades);",
     "static constexpr std::size_t capacity = 2;"),
    # Set checks before generation.
    ("r_range_or", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(A, n) || all_in_algebra(B, n)"),
    ("r_range_n_plus_one", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(A, n < mask_bits ? n + 1 : n) && "
     "all_in_algebra(B, n < mask_bits ? n + 1 : n)"),
    ("r_range_skip_last",
     "  for (const oriented_blade& blade : set.blades) {\n"
     "    if (!in_algebra(blade.mask, n)) {",
     "  for (std::size_t i = 0; i + 1 < K; ++i) {\n"
     "    const oriented_blade& blade = set.blades[i];\n"
     "    if (!in_algebra(blade.mask, n)) {"),
    ("r_sign_check_or", "signs_valid<A> && signs_valid<B>",
     "signs_valid<A> || signs_valid<B>"),
    ("r_guard_or", "if constexpr (in_range && valid_signs) {",
     "if constexpr (in_range || valid_signs) {"),
    ("r_guard_range_only", "if constexpr (in_range && valid_signs) {",
     "if constexpr (in_range) {"),
    ("r_guard_signs_only", "if constexpr (in_range && valid_signs) {",
     "if constexpr (valid_signs) {"),
    ("r_n_is_mask_bits",
     "static constexpr std::size_t n = Alg::value.generator_count;",
     "static constexpr std::size_t n = mask_bits;"),
    # Sign folding.
    ("r_sign_second_from_first_place",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[a].sign) * bs[0].sign;"),
    ("r_sign_first_from_last_place",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "static_cast<coefficient_type>(as[as.size() - 1].sign) * bs[b].sign;"),
    ("r_sign_negated",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "-(static_cast<coefficient_type>(as[a].sign) * bs[b].sign);"),
    ("r_sign_by_equality",
     "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;",
     "as[a].sign == bs[b].sign ? coefficient_type{1} : coefficient_type{-1};"),
    # Output placement.
    ("r_write_at_k", "if (count < out.size()) {\n          out[count]",
     "if (k < out.size()) {\n          out[k]"),
    ("r_fill_one_short", "filled_gp<count, capacity>",
     "filled_gp<(count > 0 ? count - 1 : 0), capacity>"),
    # Capacity over the pairs.
    ("r_capacity_ge", "if (pair > capacity) {", "if (pair >= capacity) {"),
    # Checks of a generated plan.
    ("r_defects_skip_first",
     "for (std::size_t k = 0; k < plan.size(); ++k) {\n"
     "    const plan_term& t = plan[k];",
     "for (std::size_t k = 1; k < plan.size(); ++k) {\n"
     "    const plan_term& t = plan[k];"),
    ("r_defects_skip_last",
     "for (std::size_t k = 0; k < plan.size(); ++k) {\n"
     "    const plan_term& t = plan[k];",
     "for (std::size_t k = 0; k + 1 < plan.size(); ++k) {\n"
     "    const plan_term& t = plan[k];"),
    ("r_order_numeric", "canonical_less(before.out, t.out)",
     "before.out < t.out"),
    ("r_order_new_a_needs_b", "const bool in_order = before.a < t.a ||",
     "const bool in_order = (before.a < t.a && before.b <= t.b) ||"),
    ("r_defects_sizes_swapped", "defects_of(plan, A.size(), B.size(), n);",
     "defects_of(plan, B.size(), A.size(), n);"),
    ("r_defects_n_widest", "defects_of(plan, A.size(), B.size(), n);",
     "defects_of(plan, A.size(), B.size(), mask_bits);"),
    # Locals: the count plain, and every local plain (the form before the fix).
    ("r_count_plain", "    static constexpr std::size_t count =\n",
     "    constexpr std::size_t count =\n"),
    ("r_capacity_first_operand_only", "product_capacity(x.mask, y.mask)",
     "product_capacity(x.mask, y.mask & x.mask)"),
    ("r_gp_plan_unconstrained",
     "template <metric_like Alg, blade_set A, blade_set B>\ninline constexpr auto gp_plan",
     "template <class Alg, blade_set A, blade_set B>\ninline constexpr auto gp_plan"),
    ("r_all_locals_plain_count", "    static constexpr std::size_t capacity = plan_capacity",
     "    constexpr std::size_t capacity = plan_capacity"),
]

source = DRIVER.read_text(encoding="utf-8")
start = source.index("MUTATIONS = [")
end = source.index("\n]\n", start) + 3
namespace = {"__name__": "__main__", "MUTATIONS": MUTATIONS}
exec(compile(source[:start] + "MUTATIONS = MUTATIONS\n" + source[end:],
             str(DRIVER), "exec"), namespace)
