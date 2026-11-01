"""Mutation runs for the unary maps and the antiproduct plans of plan.hpp:
apply one mutation to plan.hpp or bits.hpp, build the plan test, run the
core.plan selection (the doctest program, the compile-fail tests, and on
Clang the two evaluation checks), and restore the file. A mutant is killed
when the build fails or any test fails; the build status is recorded before
the tests, because CTest runs whatever program is on disk. Each mutation's
anchor must occur exactly once in its file.

For a failed build the record gives what killed it: the lines of plan.hpp
and plan_test.cpp whose static_assert failed, the messages of the
generation-time checks of plan.hpp that fired ("unary map: ..." and
"product plan: ..."), and the first error line. The kill is classified as
`static_assert` when a static_assert failed, `warning` when the first error
is a warning made an error (a mutant killed only by a warning is not killed
by a test, and is rewritten), `constant` for any other error in a constant
evaluation, and `test` when the build passed and a test failed. The program
is deleted before each build, so that a failed build cannot leave an old
program behind, and CTest is not run after a failed build: the mutant is
killed already, and CTest 4.4 run in parallel under the shared job pool can
keep a slot it took for a test whose program is missing. After the last
mutant every file is restored and the program is built again.

With --runtime, every static_assert of the test file that reads a plan or
a map becomes static_assert(true || static_cast<bool>(...)), and the checks
of a generated plan, of its typing, and of a map in the header become
`true || ...`, so that the doctest program builds on a mutant they would
reject, and the record lists the test cases that fail at run time. The
static_asserts of the test on type properties stay, and so do the checks of
the sets before generation, because the compile-fail tests judge them.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0013-plan-maps/mutate.py \\
      <preset> [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
PLAN = f"{ROOT}/include/galoot/core/plan.hpp"
BITS = f"{ROOT}/include/galoot/core/bits.hpp"
TEST = f"{ROOT}/test/core/plan_test.cpp"
PROGRAM = "core_plan_test"

RUNTIME_HEADER_EDITS = [
    # Five checks of a generated plan and seven of a map share the name.
    ("static_assert(!defects.", "static_assert(true || !defects.", 12),
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
    assert count > 80, count
    return "".join(out)


def plain_locals(function, count):
    def edit(text):
        start = text.index(f"consteval auto {function}() {{")
        end = text.index("\n}\n", start)
        body = text[start:end]
        assert body.count("static constexpr ") == count, \
            body.count("static constexpr ")
        return text[:start] + body.replace("static constexpr ", "constexpr ") \
            + text[end:]
    return edit


MAP_TWICE_ANCHOR = \
    "inline constexpr auto unary_map = make_unary_map<Op, Alg, S>();"
MAP_TWICE = ("inline constexpr auto unary_map =\n"
             "    (static_cast<void>(make_unary_map<Op, Alg, S>()),\n"
             "     make_unary_map<Op, Alg, S>());")


def map_twice(text):
    assert text.count(MAP_TWICE_ANCHOR) == 1
    return text.replace(MAP_TWICE_ANCHOR, MAP_TWICE)


MAP_TERMS_UNUSED = ("  for (const bool is_used : used) {\n"
                    "    found.result_unused = found.result_unused || !is_used;"
                    "\n  }\n  return found;\n}\n\ntemplate <std::size_t K>")
MAP_SET_RANGE = ("static_assert(in_range,\n"
                 "                \"unary map: the blade set names a generator "
                 "beyond e_n\");")
MAP_SET_SIGN = ("static_assert(valid_signs,\n"
                "                \"unary map: the blade set holds a sign other "
                "than +1 or -1\");")
MAP_GUARD = ("  if constexpr (in_range && valid_signs) {\n"
             "    static constexpr auto generated = filled_map")

# (name, file, anchor or edit function, replacement)
MUTATIONS = [
    # The conjugation of the antiproducts.
    ("run_reversal_dropped", PLAN,
     "const term& base_term = terms.terms[terms.size() - 1 - k];",
     "const term& base_term = terms.terms[k];"),
    ("output_right_complement", PLAN,
     "left_complement_sign(base_term.blade, N) * base_term.coefficient,",
     "right_complement_sign(base_term.blade, N) * base_term.coefficient,"),
    ("output_sign_dropped", PLAN,
     "left_complement_sign(base_term.blade, N) * base_term.coefficient,",
     "base_term.coefficient,"),
    ("output_not_complemented", PLAN,
     "        .blade = complement_mask(base_term.blade, N),",
     "        .blade = base_term.blade,"),
    ("input_left_complement", PLAN,
     "        .coefficient = right_complement_sign(a, N),\n    };\n  }\n}",
     "        .coefficient = left_complement_sign(a, N),\n    };\n  }\n}"),
    ("input_sign_dropped", PLAN,
     "        .coefficient = right_complement_sign(a, N),\n    };\n  }\n}",
     "        .coefficient = 1,\n    };\n  }\n}"),
    ("left_right_swapped", PLAN,
     lambda text: swap_conjugation(text), None),
    ("first_not_complemented", PLAN,
     "      const term first = factor_of<Op, N>(as[a].mask);",
     "      const term first =\n"
     "          {.blade = as[a].mask, .coefficient = 1};"),
    ("second_not_complemented", PLAN,
     "      const term second = factor_of<Op, N>(bs[b].mask);",
     "      const term second =\n"
     "          {.blade = bs[b].mask, .coefficient = 1};"),
    ("antigp_base_wedge", PLAN,
     "  if (op == BinaryOperation::antigp) {\n    return BinaryOperation::gp;",
     "  if (op == BinaryOperation::antigp) {\n    return BinaryOperation::wedge;"),
    ("antiwedge_base_gp", PLAN,
     "    return BinaryOperation::wedge;\n  }\n  return op;",
     "    return BinaryOperation::gp;\n  }\n  return op;"),
    ("antigp_capacity_uncomplemented", PLAN,
     "return product_capacity(complement_mask(a, n), complement_mask(b, n));",
     "return product_capacity(a, b) + 0 * n;"),
    ("antigp_capacity_one", PLAN,
     "return product_capacity(complement_mask(a, n), complement_mask(b, n));",
     "return 1 + 0 * n;"),
    # Equivalent in output, a cost: the antiwedge sized as the antiproduct.
    ("antiwedge_capacity_antigp", PLAN,
     "  if (op == BinaryOperation::antigp) {\n    return product_capacity(",
     "  if (op == BinaryOperation::antigp ||\n"
     "      op == BinaryOperation::antiwedge) {\n    return product_capacity("),
    # The pair rule's constraint.
    ("pair_terms_unconstrained", PLAN,
     "  requires(Op == BinaryOperation::gp || Op == BinaryOperation::wedge)\n",
     ""),
    # The images of one blade.
    ("unary_complement_signs_swapped", PLAN,
     lambda text: swap_map_signs(text), None),
    ("antireverse_with_grade", PLAN,
     "return {.blade = a, .coefficient = antireverse_sign(a, n)};",
     "return {.blade = a, .coefficient = reverse_sign(a)};"),
    ("involution_as_reverse", PLAN,
     "return {.blade = a, .coefficient = grade_involution_sign(a)};",
     "return {.blade = a, .coefficient = reverse_sign(a)};"),
    ("reverse_as_involution", PLAN,
     "return {.blade = a, .coefficient = reverse_sign(a)};",
     "return {.blade = a, .coefficient = grade_involution_sign(a)};"),
    ("right_complement_keeps_mask", PLAN,
     "    return {\n        .blade = complement_mask(a, n),\n"
     "        .coefficient = right_complement_sign(a, n),",
     "    return {\n        .blade = a,\n"
     "        .coefficient = right_complement_sign(a, n),"),
    ("left_complement_as_right", PLAN,
     "      .coefficient = left_complement_sign(a, n),\n  };",
     "      .coefficient = right_complement_sign(a, n),\n  };"),
    ("unary_range_removed", PLAN,
     "  GALOOT_ASSERT(in_algebra(a, n), \"blade has a generator beyond e_n\");\n"
     "  if (op == UnaryOperation::reverse) {",
     "  if (op == UnaryOperation::reverse) {"),
    ("unary_count_removed", PLAN,
     "  GALOOT_ASSERT(valid_count(n), \"n must be from 1 to 32\");\n"
     "  GALOOT_ASSERT(in_algebra(a, n),",
     "  GALOOT_ASSERT(in_algebra(a, n),"),
    # generate_map and the typing of a map.
    ("map_sign_not_folded", PLAN,
     ".coefficient = set[a].sign * image.coefficient,",
     ".coefficient = image.coefficient,"),
    ("map_position_zero", PLAN,
     "        .a = a,\n        .b = 0,", "        .a = 0,\n        .b = 0,"),
    ("map_second_position_one", PLAN,
     "        .a = a,\n        .b = 0,", "        .a = a,\n        .b = 1,"),
    ("map_result_reversed", PLAN,
     "    out.result.blades[r] = found.blades[r];",
     "    out.result.blades[r] = found.blades[R - 1 - r];"),
    ("map_result_sign_negative", PLAN,
     "    out.result.blades[r] = found.blades[r];",
     "    out.result.blades[r] = {.mask = found.blades[r].mask, .sign = -1};"),
    ("map_position_off_by_one", PLAN,
     "        .out = terms[k].out,\n        .coefficient = terms[k].coefficient,",
     "        .out = terms[k].out + 1,\n"
     "        .coefficient = terms[k].coefficient,"),
    ("map_coefficient_negated", PLAN,
     "        .out = terms[k].out,\n        .coefficient = terms[k].coefficient,",
     "        .out = terms[k].out,\n        .coefficient = -terms[k].coefficient,"),
    ("map_result_extra_blade", PLAN,
     "static constexpr auto mapped = typed_map_of<found.size>(generated, found);",
     "static constexpr auto mapped =\n"
     "        typed_map_of<found.size + 1>(generated, found);"),
    ("map_generated_in_wider_algebra", PLAN,
     "static constexpr auto generated = filled_map<S.size()>(Op, n, S.blades);",
     "static constexpr auto generated =\n"
     "        filled_map<S.size()>(Op, n + 1, S.blades);"),
    # The checks of a map.
    ("map_range_check_removed", PLAN, MAP_SET_RANGE,
     MAP_SET_RANGE.replace("static_assert(in_range,",
                           "static_assert(true || in_range,")),
    ("map_sign_check_removed", PLAN, MAP_SET_SIGN,
     MAP_SET_SIGN.replace("static_assert(valid_signs,",
                          "static_assert(true || valid_signs,")),
    ("map_generation_unguarded", PLAN, MAP_GUARD,
     MAP_GUARD.replace("if constexpr (in_range && valid_signs) {",
                       "if constexpr (true) {")),
    ("map_order_check_removed", PLAN,
     "static_assert(!defects.result_out_of_order,",
     "static_assert(true || !defects.result_out_of_order,"),
    ("map_sign_check_off", PLAN, "static_assert(!defects.result_sign,",
     "static_assert(true || !defects.result_sign,"),
    ("map_beyond_check_removed", PLAN, "static_assert(!defects.beyond_result,",
     "static_assert(true || !defects.beyond_result,"),
    ("map_unused_check_removed", PLAN, "static_assert(!defects.result_unused,",
     "static_assert(true || !defects.result_unused,"),
    ("map_per_input_check_removed", PLAN,
     "static_assert(!defects.not_per_input,",
     "static_assert(true || !defects.not_per_input,"),
    ("map_blade_check_removed", PLAN, "static_assert(!defects.wrong_blade,",
     "static_assert(true || !defects.wrong_blade,"),
    ("map_closed_form_check_removed", PLAN,
     "static_assert(!defects.complement_sign,",
     "static_assert(true || !defects.complement_sign,"),
    # map_defects_of at its bounds.
    ("map_result_order_by_mask", PLAN,
     "!canonical_less(map.result.blades[r - 1].mask,\n"
     "                                                  map.result.blades[r].mask);",
     "map.result.blades[r - 1].mask >= map.result.blades[r].mask;"),
    ("map_result_repeat_allowed", PLAN,
     "!canonical_less(map.result.blades[r - 1].mask,\n"
     "                                                  map.result.blades[r].mask);",
     "canonical_less(map.result.blades[r].mask,\n"
     "                                                 map.result.blades[r - 1].mask);"),
    ("map_result_sign_negative_only", PLAN,
     "map.result.blades[r].sign != 1;", "map.result.blades[r].sign < 0;"),
    ("map_beyond_inclusive", PLAN, "    if (t.out >= R) {",
     "    if (t.out > R) {"),
    ("map_unused_ignored", PLAN, MAP_TERMS_UNUSED,
     MAP_TERMS_UNUSED.replace("found.result_unused || !is_used;",
                              "found.result_unused && !is_used;")),
    ("map_per_input_above_only", PLAN, "found.not_per_input || t.a != k;",
     "found.not_per_input || t.a > k;"),
    ("map_blade_kinds_swapped", PLAN,
     "complement ? complement_mask(input.mask, n) : input.mask;",
     "complement ? input.mask : complement_mask(input.mask, n);"),
    ("map_complement_right_only", PLAN,
     "  const bool complement = op == UnaryOperation::right_complement ||\n"
     "                          op == UnaryOperation::left_complement;",
     "  const bool complement = op == UnaryOperation::right_complement;"),
    ("map_left_factor_dropped", PLAN,
     "op == UnaryOperation::left_complement && g * ag % 2 != 0 ? -1 : 1;",
     "op == UnaryOperation::left_complement && g * ag % 2 != 0 ? 1 : 1;"),
    # The antigrade is still read, so that -Wunused-variable does not kill
    # the mutant in place of a test.
    ("map_left_factor_by_grade", PLAN,
     "op == UnaryOperation::left_complement && g * ag % 2 != 0 ? -1 : 1;",
     "op == UnaryOperation::left_complement && (g + 0 * ag) % 2 != 0 ? -1 : 1;"),
    ("map_closed_form_unsigned", PLAN,
     "          static_cast<coefficient_type>(input.sign) *\n"
     "          closed_form_right_sign(input.mask) * left;",
     "          static_cast<coefficient_type>(1) *\n"
     "          closed_form_right_sign(input.mask) * left;"),
    # The closed form itself.
    ("closed_form_k_minus_one", PLAN, "int exponent = k * (k + 1) / 2;",
     "int exponent = k * (k - 1) / 2;"),
    ("closed_form_index_from_zero", PLAN,
     "exponent += std::countr_zero(rest) + 1;",
     "exponent += std::countr_zero(rest);"),
    # bits.hpp: the signs the maps and the conjugation are built from.
    ("reorder_sign_skips_lowest", BITS,
     "  for (; a != 0; a &= a - 1) {\n"
     "    const auto i = static_cast<unsigned>(std::countr_zero(a));",
     "  for (a &= a - 1; a != 0; a &= a - 1) {\n"
     "    const auto i = static_cast<unsigned>(std::countr_zero(a));"),
    ("reverse_sign_k_plus_one", BITS,
     "  return (k * (k - 1) / 2) % 2 == 0 ? 1 : -1;\n}",
     "  return (k * (k + 1) / 2) % 2 == 0 ? 1 : -1;\n}"),
    ("bits_left_right_swapped", BITS,
     lambda text: swap_bits_complements(text), None),
    # Type properties.
    ("map_term_not_trivially_copyable", PLAN,
     "  [[nodiscard]] constexpr bool operator==(const map_term&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const map_term&) const = "
     "default;\n  constexpr ~map_term() {}\n"),
    ("map_term_not_aggregate", PLAN, "struct map_term {\n",
     "struct map_term {\n  map_term() = default;\n"),
    ("typed_map_not_trivially_copyable", PLAN,
     "  [[nodiscard]] constexpr bool operator==(const typed_map&) const = "
     "default;\n",
     "  [[nodiscard]] constexpr bool operator==(const typed_map&) const = "
     "default;\n  constexpr ~typed_map() {}\n"),
    ("typed_map_not_aggregate", PLAN, "struct typed_map {\n",
     "struct typed_map {\n  typed_map() = default;\n"),
    ("typed_map_not_structural", PLAN,
     "  std::array<map_term, K> terms;\n",
     "  mutable std::array<map_term, K> terms;\n"),
    ("unary_map_unconstrained", PLAN,
     "template <UnaryOperation Op, metric_like Alg, blade_set S>\n"
     "// clang-tidy reads",
     "template <UnaryOperation Op, class Alg, blade_set S>\n"
     "// clang-tidy reads"),
    # The form of the locals and of the initializer. Equivalent in result;
    # the evaluation check, on Clang only, judges them.
    ("map_locals_plain", PLAN, plain_locals("make_unary_map", 7), None),
    ("map_locals_plain_twice", PLAN,
     lambda text: map_twice(plain_locals("make_unary_map", 7)(text)), None),
    ("map_twice_static_locals", PLAN, map_twice, None),
    ("plan_locals_plain", PLAN, plain_locals("make_product_plan", 10), None),
]


def swap_conjugation(text):
    """The conjugation with the left complement on the inputs and the right
    on the outputs, in place of right on the inputs and left on the
    outputs."""
    a = "        .coefficient = right_complement_sign(a, N),\n    };\n  }\n}"
    b = "left_complement_sign(base_term.blade, N) * base_term.coefficient,"
    assert text.count(a) == 1 and text.count(b) == 1
    text = text.replace(a, a.replace("right_complement_sign",
                                     "left_complement_sign"))
    return text.replace(b, b.replace("left_complement_sign",
                                     "right_complement_sign"))


def swap_map_signs(text):
    """unary_term with the right and left complement signs exchanged."""
    a = "        .coefficient = right_complement_sign(a, n),\n    };\n  }"
    b = "      .coefficient = left_complement_sign(a, n),\n  };"
    assert text.count(a) == 1 and text.count(b) == 1
    text = text.replace(a, a.replace("right_", "left_"))
    return text.replace(b, b.replace("left_", "right_"))


def swap_bits_complements(text):
    """right_complement_sign and left_complement_sign of bits.hpp exchanged,
    so every complement in the library takes the other one's sign."""
    a = "  return reorder_sign(a, complement_mask(a, n));"
    b = "  return reorder_sign(complement_mask(a, n), a);"
    assert text.count(a) == 1 and text.count(b) == 1
    text = text.replace(a, "@SWAP@")
    text = text.replace(b, a)
    return text.replace("@SWAP@", b)


STATIC_ASSERT_LINE = re.compile(
    r"(plan_test\.cpp|plan\.hpp):(\d+):\d+: (?:fatal )?error: "
    r"(?:static assertion|static_assert)")
CHECK_MESSAGE = re.compile(r"(?:unary map|product plan): [^\"\n\[]*")


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


def kill_kind(build, test_exit, lines, errors):
    if build.returncode == 0:
        return "test" if test_exit != 0 else "-"
    if lines:
        return "static_assert"
    if errors and "[-W" in errors[0]:
        return "warning"
    return "constant"


def main():
    preset = sys.argv[1]
    runtime = "--runtime" in sys.argv[2:]
    only = {a for a in sys.argv[2:] if a != "--runtime"}
    unknown = only - {name for name, _, _, _ in MUTATIONS}
    assert not unknown, unknown
    originals = {path: open(path, encoding="utf-8").read()
                 for path in (PLAN, BITS, TEST)}
    bases = dict(originals)
    if runtime:
        bases[PLAN] = edited(originals[PLAN], RUNTIME_HEADER_EDITS)
        with open(PLAN, "w", encoding="utf-8") as f:
            f.write(bases[PLAN])
        with open(TEST, "w", encoding="utf-8") as f:
            f.write(without_plan_static_asserts(originals[TEST]))
    binary_dir = f"{ROOT}/build/{preset}/test"
    print("mutation\tfile\tpreset\tbuild_exit\ttest_exit\tfailed_tests\t"
          "killed\tkind\tstatic_assert_lines\tchecks_fired\tfailed_cases\t"
          "first_build_error")
    try:
        for name, path, anchor, replacement in MUTATIONS:
            if only and name not in only:
                continue
            base = bases[path]
            if runtime and not callable(anchor) and base.count(anchor) == 0:
                # A check that --runtime turns off cannot be removed again.
                assert originals[path].count(anchor) == 1, (name, anchor)
                print(f"{name}\t{path.split('/')[-1]}\t{preset}\t-\t-\t-\t"
                      "skipped\t-\t-\t-\t-\tthe check is off in this mode",
                      flush=True)
                continue
            with open(path, "w", encoding="utf-8") as f:
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
            with open(path, "w", encoding="utf-8") as f:
                f.write(base)
            failed = [line.split(" - ")[1].split()[0]
                      for line in output.splitlines() if " - core." in line]
            killed = build.returncode != 0 or test_exit != 0
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            lines = sorted({f"{m.group(1)}:{m.group(2)}" for m in
                            STATIC_ASSERT_LINE.finditer(build.stdout)})
            fired = sorted({m.group(0).strip().rstrip("'").strip()
                            for line in errors
                            for m in CHECK_MESSAGE.finditer(line)})
            print(f"{name}\t{path.split('/')[-1]}\t{preset}\t"
                  f"{build.returncode}\t{test_exit}\t{','.join(failed) or '-'}\t"
                  f"{'killed' if killed else 'SURVIVED'}\t"
                  f"{kill_kind(build, test_exit, lines, errors)}\t"
                  f"{','.join(lines) or '-'}\t{' | '.join(fired) or '-'}\t"
                  f"{' | '.join(failed_cases(output)) or '-'}\t"
                  f"{first_error[:200]}",
                  flush=True)
    finally:
        for path, text in originals.items():
            with open(path, "w", encoding="utf-8") as f:
                f.write(text)
        run(["tools/timed", "build", preset, "mutation:restored", "--", "cmake",
             "--build", "--preset", preset, "--target", PROGRAM])


main()
