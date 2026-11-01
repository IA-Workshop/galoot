"""Mutation runs for the typed product plans of plan.hpp: apply one mutation,
build the plan test, run the core.plan selection (the doctest program, the
compile-fail tests, and on Clang the evaluation check), and restore the
header. A mutant is killed when the build fails or any test fails; the
build status is recorded before the tests, because CTest runs whatever
program is on disk. Each mutation's anchor must occur exactly once.

For a failed build the record gives the first error line and the lines of
plan.hpp and plan_test.cpp whose static_assert failed, so that a kill by a
static_assert can be told from a kill by another error. The program is
deleted before each build, so that a failed build cannot leave an old program
behind, and CTest is not run after a failed build: the mutant is killed
already, and CTest 4.4 run in parallel under the shared job pool can keep a
slot it took for a test whose program is missing. After the last mutant the
header and the test are restored and the program is built again.

With --runtime, every static_assert of the test file that reads a plan
becomes static_assert(true || static_cast<bool>(...)), and the checks of a
generated plan and of its typing in the header become `true || ...`, so that
the doctest program builds on a mutant they would reject, and the record
lists the test cases that fail at run time. The static_asserts of the test
on type properties (aggregate, trivially copyable) stay, and so do the
checks of the two sets before generation, because the compile-fail tests
judge them.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0013-plan-results/mutate.py \\
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

RUNTIME_HEADER_EDITS = [
    ("static_assert(!defects.", "static_assert(true || !defects.", 5),
    ("static_assert(!typing.", "static_assert(true || !typing.", 5),
]
TYPE_PROPERTIES = ("std::is_aggregate_v", "std::is_trivially_copyable_v")


def without_plan_static_asserts(text):
    """The test file with each static_assert that reads a plan made true."""
    out, at, count = [], 0, 0
    while True:
        start = text.find("static_assert(", at)
        if start < 0:
            out.append(text[at:])
            break
        open_paren = start + len("static_assert")
        depth, end = 0, open_paren
        while True:
            c = text[end]
            if c == '"':
                end = text.index('"', end + 1)
            elif c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
                if depth == 0:
                    break
            end += 1
        inner = text[open_paren + 1:end]
        out.append(text[at:open_paren + 1])
        if inner.lstrip().startswith(TYPE_PROPERTIES):
            out.append(inner)
        else:
            out.append(f"true || static_cast<bool>({inner})")
            count += 1
        out.append(")")
        at = end + 1
    assert count > 50, count
    return "".join(out)


def plain_locals(text):
    start = text.index("consteval auto make_product_plan() {")
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert body.count("static constexpr ") == 10, body.count("static constexpr ")
    return text[:start] + body.replace("static constexpr ", "constexpr ") + \
        text[end:]


TWICE_ANCHOR = "inline constexpr auto product_plan = make_product_plan<Op, Alg, A, B>();"
TWICE = ("inline constexpr auto product_plan =\n"
         "    (static_cast<void>(make_product_plan<Op, Alg, A, B>()),\n"
         "     make_product_plan<Op, Alg, A, B>());")


def twice(text):
    assert text.count(TWICE_ANCHOR) == 1
    return text.replace(TWICE_ANCHOR, TWICE)


SIGN = "static_cast<coefficient_type>(as[a].sign) * bs[b].sign;"
MASK_ORDER = ("      const bool in_order = before.a < t.a ||\n"
              "                            (before.a == t.a && before.b < t.b) ||")
TYPED_ORDER = "          before.a < t.a || (before.a == t.a && before.b < t.b) ||"

MUTATIONS = [
    # The wedge's pair rule.
    ("wedge_shared_kept",
     "    if ((a & b) == 0) {\n      out.add(a | b, reorder_sign(a, b));\n    }\n",
     "    out.add(a | b, reorder_sign(a, b));\n"),
    ("wedge_sign_plus", "out.add(a | b, reorder_sign(a, b));",
     "out.add(a | b, 1);"),
    ("wedge_sign_reversed", "out.add(a | b, reorder_sign(a, b));",
     "out.add(a | b, reorder_sign(b, a));"),
    ("wedge_sign_by_grade", "out.add(a | b, reorder_sign(a, b));",
     "out.add(a | b, reorder_sign(a, b) * (grade(a) % 2 == 0 ? 1 : -1));"),
    ("wedge_range_removed", "GALOOT_ASSERT(in_algebra(a | b, N),",
     "GALOOT_ASSERT(true,"),
    ("wedge_range_first_only", "in_algebra(a | b, N)", "in_algebra(a, N)"),
    ("wedge_range_second_only", "in_algebra(a | b, N)", "in_algebra(b, N)"),
    ("wedge_as_gp",
     "  if constexpr (Op == BinaryOperation::gp) {\n    return blade_product",
     "  if constexpr (true) {\n    return blade_product"),
    ("pair_rule_always_gp",
     "pair_terms<Op, Capacity>(g, as[a].mask, bs[b].mask)",
     "pair_terms<BinaryOperation::gp, Capacity>(g, as[a].mask, bs[b].mask)"),
    # Capacities: the wedge sized for the product is a cost, which the
    # capacity's own test sees; the product sized for the wedge is wrong.
    # The operation is still read, so that -Wunused-parameter does not
    # kill the mutant in place of a test.
    ("wedge_capacity_gp",
     "return op == BinaryOperation::gp ? product_capacity(a, b) : 1;",
     "return op == BinaryOperation::gp ? product_capacity(a, b)\n"
     "                                  : product_capacity(b, a);"),
    ("capacities_swapped", "return op == BinaryOperation::gp ?",
     "return op == BinaryOperation::wedge ?"),
    # Sign folding of the generator.
    ("sign_fold_dropped", SIGN,
     "static_cast<coefficient_type>(as[a].sign * as[a].sign) *\n"
     "          (bs[b].sign * bs[b].sign);"),
    ("sign_first_only", SIGN, "static_cast<coefficient_type>(as[a].sign);"),
    ("sign_second_only", SIGN, "static_cast<coefficient_type>(bs[b].sign);"),
    ("sign_by_sum", SIGN,
     "static_cast<coefficient_type>(as[a].sign) + bs[b].sign;"),
    # Positions and output blades of the generator.
    ("positions_swapped", ".a = a,\n              .b = b,",
     ".a = b,\n              .b = a,"),
    ("a_off_by_one", ".a = a,", ".a = a + 1,"),
    ("b_off_by_one", ".b = b,", ".b = b + 1,"),
    ("out_shifted", ".out = terms.terms[k].blade,",
     ".out = terms.terms[k].blade << 1U,"),
    ("out_xor", ".out = terms.terms[k].blade,",
     ".out = as[a].mask ^ bs[b].mask,"),
    ("loops_swapped",
     "  for (std::size_t a = 0; a < as.size(); ++a) {\n"
     "    for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "      const term_list<Capacity> terms =",
     "  for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "    for (std::size_t a = 0; a < as.size(); ++a) {\n"
     "      const term_list<Capacity> terms ="),
    ("terms_reversed",
     "for (std::size_t k = 0; k < terms.size(); ++k) {\n        if (count",
     "for (std::size_t r = 0, k = terms.size() - 1; r < terms.size();"
     " ++r, --k) {\n        if (count"),
    ("last_pair_skipped",
     "    for (std::size_t b = 0; b < bs.size(); ++b) {\n"
     "      const term_list<Capacity> terms =",
     "    for (std::size_t b = 0; b + 1 < bs.size(); ++b) {\n"
     "      const term_list<Capacity> terms ="),
    ("count_plus_one", "  return count;\n}", "  return count + 1;\n}"),
    ("write_bound_inclusive", "if (count < out.size()) {",
     "if (count <= out.size()) {"),
    ("capacity_start_zero", "  std::size_t capacity = 1;\n",
     "  std::size_t capacity = 0;\n"),
    ("capacity_halved", "        capacity = pair;\n",
     "        capacity = pair > 1 ? pair / 2 : pair;\n"),
    ("fill_negated",
     "  generate_plan<Op, Capacity>(g, as, bs, plan);\n  return plan;",
     "  generate_plan<Op, Capacity>(g, as, bs, plan);\n"
     "  for (plan_term& t : plan) {\n    t.coefficient = -t.coefficient;\n  }\n"
     "  return plan;"),
    # Checks of the sets before generation.
    ("range_first_only", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(A, n)"),
    ("range_second_only", "all_in_algebra(A, n) && all_in_algebra(B, n)",
     "all_in_algebra(B, n)"),
    ("range_check_removed", "static_assert(in_range,",
     "static_assert(true || in_range,"),
    ("sign_check_first_only", "signs_valid<A> && signs_valid<B>",
     "signs_valid<A>"),
    ("sign_check_second_only", "signs_valid<A> && signs_valid<B>",
     "signs_valid<B>"),
    ("sign_check_removed", "static_assert(valid_signs,",
     "static_assert(true || valid_signs,"),
    ("generation_unguarded", "if constexpr (in_range && valid_signs) {",
     "if constexpr (true) {"),
    # Checks of a generated plan, kept from the mask-level plan.
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
    ("order_allows_repeat", "canonical_less(before.out, t.out)",
     "!canonical_less(t.out, before.out)"),
    ("order_same_b", MASK_ORDER,
     MASK_ORDER.replace("before.b < t.b", "before.b <= t.b")),
    ("order_same_a", MASK_ORDER,
     MASK_ORDER.replace("before.a < t.a ||", "before.a <= t.a ||")),
    # Result typing.
    ("result_by_mask",
     "while (at < size && canonical_less(out[at].mask, t.out)) {",
     "while (at < size && out[at].mask < t.out) {"),
    ("result_extra_scalar",
     "  std::size_t size = 0;\n  for (const plan_term& t : plan) {",
     "  std::size_t size = 0;\n  if (!plan.empty()) {\n"
     "    out[0] = {.mask = 0, .sign = 1};\n    size = 1;\n  }\n"
     "  for (const plan_term& t : plan) {"),
    ("result_sign_from_coefficient",
     "    out[at] = {.mask = t.out, .sign = 1};",
     "    out[at] = {.mask = t.out, .sign = t.coefficient < 0 ? -1 : 1};"),
    ("result_repeats_kept",
     "    if (at < size && out[at].mask == t.out) {\n      continue;\n    }\n",
     ""),
    ("result_room_check_removed", "GALOOT_ASSERT(size < out.size(),",
     "GALOOT_ASSERT(true,"),
    ("result_room_inclusive", "GALOOT_ASSERT(size < out.size(),",
     "GALOOT_ASSERT(size <= out.size(),"),
    ("result_clears_rest", "    ++size;\n  }\n  return size;\n}",
     "    ++size;\n  }\n  for (std::size_t j = size; j < out.size(); ++j) {\n"
     "    out[j] = {};\n  }\n  return size;\n}"),
    ("position_off_by_one", "        .out = at,", "        .out = at + 1,"),
    ("position_missing_zero",
     "    while (at < result.size() && result[at].mask != plan[k].out) {\n"
     "      ++at;\n    }\n",
     "    while (at < result.size() && result[at].mask != plan[k].out) {\n"
     "      ++at;\n    }\n    if (at == result.size()) {\n      at = 0;\n    }\n"),
    ("typing_past_output",
     "for (std::size_t k = 0; k < plan.size() && k < out.size(); ++k) {",
     "for (std::size_t k = 0; k < plan.size(); ++k) {"),
    ("typed_result_short",
     "  for (std::size_t r = 0; r < R; ++r) {\n"
     "    typed_plan_of.result.blades[r] = found.blades[r];",
     "  for (std::size_t r = 0; r + 1 < R; ++r) {\n"
     "    typed_plan_of.result.blades[r] = found.blades[r];"),
    ("typed_negated",
     "  type_terms(plan, typed_plan_of.result.blades, typed_plan_of.terms);\n"
     "  return typed_plan_of;",
     "  type_terms(plan, typed_plan_of.result.blades, typed_plan_of.terms);\n"
     "  for (typed_term& t : typed_plan_of.terms) {\n"
     "    t.coefficient = -t.coefficient;\n  }\n"
     "  return typed_plan_of;"),
    # Checks of the typing.
    ("typing_order_check_removed",
     "static_assert(!typing.result_out_of_order,",
     "static_assert(true || !typing.result_out_of_order,"),
    ("typing_sign_check_removed", "static_assert(!typing.result_sign,",
     "static_assert(true || !typing.result_sign,"),
    ("typing_beyond_check_removed", "static_assert(!typing.beyond_result,",
     "static_assert(true || !typing.beyond_result,"),
    ("typing_unused_check_removed", "static_assert(!typing.result_unused,",
     "static_assert(true || !typing.result_unused,"),
    ("typing_terms_check_removed", "static_assert(!typing.out_of_order,",
     "static_assert(true || !typing.out_of_order,"),
    ("typed_result_order_by_mask",
     "!canonical_less(result.blades[r - 1].mask, result.blades[r].mask)",
     "result.blades[r - 1].mask >= result.blades[r].mask"),
    ("typed_result_repeat_allowed",
     "!canonical_less(result.blades[r - 1].mask, result.blades[r].mask)",
     "canonical_less(result.blades[r].mask, result.blades[r - 1].mask)"),
    ("typed_sign_test", "result.blades[r].sign != 1",
     "result.blades[r].sign == 0"),
    ("typed_beyond_inclusive", "    if (t.out < R) {", "    if (t.out <= R) {"),
    ("typed_unused_ignored",
     "    found.result_unused = found.result_unused || !is_used;",
     "    found.result_unused = found.result_unused && !is_used;"),
    ("typed_order_allows_equal", "before.out < t.out", "before.out <= t.out"),
    ("typed_order_same_b", TYPED_ORDER,
     TYPED_ORDER.replace("before.b < t.b", "before.b <= t.b")),
    ("typed_order_same_a", TYPED_ORDER,
     TYPED_ORDER.replace("before.a < t.a ||", "before.a <= t.a ||")),
    # Type properties.
    ("typed_term_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const typed_term&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const typed_term&) const = "
     "default;\n  constexpr ~typed_term() {}\n"),
    ("typed_term_not_aggregate", "struct typed_term {\n",
     "struct typed_term {\n  typed_term() = default;\n"),
    ("typed_plan_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const typed_plan&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const typed_plan&) const = "
     "default;\n  constexpr ~typed_plan() {}\n"),
    ("typed_plan_not_aggregate", "struct typed_plan {\n",
     "struct typed_plan {\n  typed_plan() = default;\n"),
    ("typed_plan_not_structural",
     "  std::array<typed_term, T> terms;\n",
     "  mutable std::array<typed_term, T> terms;\n"),
    ("plan_term_not_trivially_copyable",
     "  [[nodiscard]] constexpr bool operator==(const plan_term&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const plan_term&) const = "
     "default;\n  constexpr ~plan_term() {}\n"),
    ("plan_term_not_aggregate", "struct plan_term {\n",
     "struct plan_term {\n  plan_term() = default;\n"),
    # The form of the locals and of the initializer. Equivalent in result;
    # the evaluation check, on Clang only, judges them.
    ("locals_plain", plain_locals, None),
    ("locals_plain_twice", lambda text: twice(plain_locals(text)), None),
    ("twice_static_locals", twice, None),
]

STATIC_ASSERT_LINE = re.compile(
    r"(plan_test\.cpp|plan\.hpp):(\d+):\d+: (?:fatal )?error: "
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


def mutated(base, anchor, replacement):
    if callable(anchor):
        return anchor(base)
    assert base.count(anchor) == 1, (anchor, base.count(anchor))
    return base.replace(anchor, replacement)


def main():
    preset = sys.argv[1]
    runtime = "--runtime" in sys.argv[2:]
    only = {a for a in sys.argv[2:] if a != "--runtime"}
    unknown = only - {name for name, _, _ in MUTATIONS}
    assert not unknown, unknown
    original = open(HEADER, encoding="utf-8").read()
    original_test = open(TEST, encoding="utf-8").read()
    base = edited(original, RUNTIME_HEADER_EDITS) if runtime else original
    if runtime:
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(without_plan_static_asserts(original_test))
    binary_dir = f"{ROOT}/build/{preset}/test"
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled\t"
          "static_assert_lines\tfailed_cases\tfirst_build_error")
    try:
        for name, anchor, replacement in MUTATIONS:
            if only and name not in only:
                continue
            if runtime and not callable(anchor) and base.count(anchor) == 0:
                # A check that --runtime turns off cannot be removed again.
                assert original.count(anchor) == 1, (name, anchor)
                print(f"{name}\t{preset}\t-\t-\t-\tskipped\t-\t-\t"
                      "the check is off in this mode", flush=True)
                continue
            with open(HEADER, "w", encoding="utf-8") as f:
                f.write(mutated(base, anchor, replacement))
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
