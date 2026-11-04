#!/usr/bin/env -S python3 -I
"""Mutation runs for the bulk, the weight, the duals, the dot, the antidot,
and the projection of dual.hpp.

  mutate.py <preset> [mutant...]

Applies one mutant at a time to include/galoot/core/dual.hpp, builds the
two test programs that judge it (core_dual_test, core_dual_identities_test)
with ninja's -k 0 so that both are tried, runs the tests labeled core.dual
(the two programs and every dual compile-fail test), and restores the
header. Every build and test goes through tools/timed with the selection
mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a call the test makes that has no overload), or a compile-fail test or a
run-time check that fails. An error that comes from a warning made an error
does not count; the record lists each compile error's first line so that
this can be read off.

Writes out/<preset>.tsv: mutant, build exit, the programs that failed to
build with the first error line of each, and the failed tests.

Usage, from the worktree root with the job pool exported.
"""

import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
CORE = ROOT / "include/galoot/core"
OUT = pathlib.Path(__file__).resolve().parent / "out"
TARGETS = ["core_dual_test", "core_dual_identities_test"]

H = "dual.hpp"
BULK_CHECK = "detail::bulk_coefficients_exact<Alg, S, T>;"
WEIGHT_CHECK = "detail::weight_coefficients_exact<Alg, S, T>;"
CATCH_ALL = "requires detail::mismatched_operands<X, Y>"
NO_ALGEBRA = ("requires multivector<X> && multivector<Y> && "
              "(!detail::same_scalar_v<std::remove_cvref_t<X>, "
              "std::remove_cvref_t<Y>>)")
NO_SCALAR = ("requires multivector<X> && multivector<Y> && "
             "(!detail::same_algebra_v<std::remove_cvref_t<X>, "
             "std::remove_cvref_t<Y>>)")
SCALAR_MASK = "plan.result.blades[t.out].mask == 0"
DOT_CHECK = "detail::kernel_held_exactly<T>(kernel);"
ANTIDOT_KERNEL = ("detail::scalar_part_kernel<Alg, right_complement_layout<Alg, A>,\n"
                  "                                     right_complement_layout<Alg, B>>);")
PROJECT_CHECK = "detail::weight_coefficients_exact<Alg, B, T>;"
GP_BULK_DUAL = "return geometric_product(reverse(x), detail::unit_antiscalar<Alg, T>());"


def check_of(name):
    return f"constexpr bool scalar_holds_every_coefficient_of_the_{name} =\n      "


MUTANTS = {
    # The duals.
    "bulk_dual_uses_left_complement": (H, [(GP_BULK_DUAL, "return left_complement(bulk(x));", None)]),
    "bulk_dual_omits_reverse": (H, [(GP_BULK_DUAL, "return geometric_product(x, detail::unit_antiscalar<Alg, T>());", None)]),
    "bulk_dual_antiscalar_first": (H, [(GP_BULK_DUAL, "return geometric_product(detail::unit_antiscalar<Alg, T>(), reverse(x));", None)]),
    "weight_dual_is_bulk_dual": (H, [
        ("return bulk(right_complement(x));", "return right_complement(bulk(x));", None),
        ("inline constexpr auto weight_dual_layout =\n    bulk_layout<Alg, right_complement_layout<Alg, S>>;",
         "inline constexpr auto weight_dual_layout =\n    right_complement_layout<Alg, bulk_layout<Alg, S>>;", None)]),
    "weight_calls_bulk": (H, [
        ("return left_complement(bulk(right_complement(x)));", "return bulk(x);", None),
        ("inline constexpr auto weight_layout =\n    left_complement_layout<Alg,\n                           bulk_layout<Alg, right_complement_layout<Alg, S>>>;",
         "inline constexpr auto weight_layout = bulk_layout<Alg, S>;", None)]),
    "weight_right_complement_outside": (H, [
        ("return left_complement(bulk(right_complement(x)));",
         "return right_complement(bulk(right_complement(x)));", None)]),
    "left_weight_dual_uses_right_complement": (H, [
        ("return left_complement(weight(x));", "return right_complement(weight(x));", None)]),
    # The dot and the antidot.
    "dot_omits_reverse": (H, [("a.coefficients, reverse(b).coefficients,", "a.coefficients, b.coefficients,", None)]),
    "kernel_keeps_antiscalar_terms": (H, [
        (SCALAR_MASK, "plan.result.blades[t.out].mask == low_bits(Alg::value.generator_count)", [0, 1])]),
    "kernel_keeps_every_term": (H, [(SCALAR_MASK, "t.out < plan.result.size()", [0, 1])]),
    "kernel_adds_first_term_to_zero": (H, [("  mark_first_terms<1>(terms);\n", "", None)]),
    "antidot_without_complements": (H, [
        ("return left_complement(dot(right_complement(a), right_complement(b)));",
         "return left_complement(dot(a, b));", None),
        ("left_complement_layout<Alg, dot_layout<Alg, right_complement_layout<Alg, A>,\n"
         "                                           right_complement_layout<Alg, B>>>;",
         "left_complement_layout<Alg, dot_layout<Alg, A, B>>;", None)]),
    "antidot_right_complement_outside": (H, [
        ("return left_complement(dot(right_complement(a), right_complement(b)));",
         "return right_complement(dot(right_complement(a), right_complement(b)));", None)]),
    # The projection.
    "project_operands_swapped": (H, [
        ("return antiwedge(b, wedge(a, weight_dual(b)));", "return antiwedge(a, wedge(b, weight_dual(a)));", None),
        ("antiwedge_layout<Alg, B, wedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;",
         "antiwedge_layout<Alg, A, wedge_layout<Alg, B, weight_dual_layout<Alg, A>>>;", None)]),
    "project_uses_bulk_dual": (H, [
        ("return antiwedge(b, wedge(a, weight_dual(b)));", "return antiwedge(b, wedge(a, bulk_dual(b)));", None),
        ("antiwedge_layout<Alg, B, wedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;",
         "antiwedge_layout<Alg, B, wedge_layout<Alg, A, bulk_dual_layout<Alg, B>>>;", None)]),
    # The checks: each dropped, and each checking another operation's plan.
    "bulk_check_dropped": (H, [(check_of("bulk") + BULK_CHECK, check_of("bulk") + "true;", None)]),
    "bulk_checks_weight": (H, [(check_of("bulk") + BULK_CHECK, check_of("bulk") + WEIGHT_CHECK, None)]),
    "weight_check_dropped": (H, [(check_of("weight") + WEIGHT_CHECK, check_of("weight") + "true;", None)]),
    "weight_checks_bulk": (H, [(check_of("weight") + WEIGHT_CHECK, check_of("weight") + BULK_CHECK, None)]),
    "bulk_dual_check_dropped": (H, [(check_of("bulk_dual") + BULK_CHECK, check_of("bulk_dual") + "true;", None)]),
    "bulk_dual_checks_weight": (H, [(check_of("bulk_dual") + BULK_CHECK, check_of("bulk_dual") + WEIGHT_CHECK, None)]),
    "weight_dual_check_dropped": (H, [(check_of("weight_dual") + WEIGHT_CHECK, check_of("weight_dual") + "true;", None)]),
    "weight_dual_checks_bulk": (H, [(check_of("weight_dual") + WEIGHT_CHECK, check_of("weight_dual") + BULK_CHECK, None)]),
    "left_bulk_dual_check_dropped": (H, [(check_of("left_bulk_dual") + BULK_CHECK, check_of("left_bulk_dual") + "true;", None)]),
    "left_bulk_dual_checks_weight": (H, [(check_of("left_bulk_dual") + BULK_CHECK, check_of("left_bulk_dual") + WEIGHT_CHECK, None)]),
    "left_weight_dual_check_dropped": (H, [(check_of("left_weight_dual") + WEIGHT_CHECK, check_of("left_weight_dual") + "true;", None)]),
    "left_weight_dual_checks_bulk": (H, [(check_of("left_weight_dual") + WEIGHT_CHECK, check_of("left_weight_dual") + BULK_CHECK, None)]),
    "dot_check_dropped": (H, [(check_of("dot") + DOT_CHECK, check_of("dot") + "true;", None)]),
    "dot_checks_antidot": (H, [(check_of("dot") + DOT_CHECK,
        check_of("dot") + "detail::kernel_held_exactly<T>(detail::scalar_part_kernel<Alg, right_complement_layout<Alg, A>, right_complement_layout<Alg, B>>);", None)]),
    "dot_checks_whole_product": (H, [(check_of("dot") + DOT_CHECK,
        check_of("dot") + "detail::coefficients_exact<detail::BinaryOperation::geometric_product, Alg, A, B, T>;", None)]),
    "antidot_check_dropped": (H, [(check_of("antidot") + "detail::kernel_held_exactly<T>(\n          " + ANTIDOT_KERNEL,
        check_of("antidot") + "true;", None)]),
    "antidot_checks_dot": (H, [(check_of("antidot") + "detail::kernel_held_exactly<T>(\n          " + ANTIDOT_KERNEL,
        check_of("antidot") + "detail::kernel_held_exactly<T>(detail::scalar_part_kernel<Alg, A, B>);", None)]),
    "project_check_dropped": (H, [(check_of("projection") + PROJECT_CHECK, check_of("projection") + "true;", None)]),
    "project_checks_bulk_dual": (H, [(check_of("projection") + PROJECT_CHECK,
        check_of("projection") + "detail::bulk_coefficients_exact<Alg, B, T>;", None)]),
    # The catch-alls of dot, antidot, and project.
    "catch_alls_without_algebra_condition": (H, [(CATCH_ALL, NO_ALGEBRA, [0, 1, 2])]),
    "catch_alls_without_scalar_condition": (H, [(CATCH_ALL, NO_SCALAR, [0, 1, 2])]),
    # Each layout name returns the input's layout.
    "bulk_layout_is_input": (H, [("inline constexpr auto bulk_layout = left_complement_layout<\n    Alg, geometric_product_layout<Alg, S, detail::antiscalar_layout<Alg>>>;",
                                   "inline constexpr auto bulk_layout = S;", None)]),
    "weight_layout_is_input": (H, [("inline constexpr auto weight_layout =\n    left_complement_layout<Alg,\n                           bulk_layout<Alg, right_complement_layout<Alg, S>>>;",
                                     "inline constexpr auto weight_layout = S;", None)]),
    "bulk_dual_layout_is_input": (H, [("inline constexpr auto bulk_dual_layout =\n    geometric_product_layout<Alg, S, detail::antiscalar_layout<Alg>>;",
                                        "inline constexpr auto bulk_dual_layout = S;", None)]),
    "weight_dual_layout_is_input": (H, [("inline constexpr auto weight_dual_layout =\n    bulk_layout<Alg, right_complement_layout<Alg, S>>;",
                                          "inline constexpr auto weight_dual_layout = S;", None)]),
    "left_bulk_dual_layout_is_input": (H, [("inline constexpr auto left_bulk_dual_layout =\n    left_complement_layout<Alg, bulk_layout<Alg, S>>;",
                                             "inline constexpr auto left_bulk_dual_layout = S;", None)]),
    "left_weight_dual_layout_is_input": (H, [("inline constexpr auto left_weight_dual_layout =\n    left_complement_layout<Alg, weight_layout<Alg, S>>;",
                                               "inline constexpr auto left_weight_dual_layout = S;", None)]),
    "dot_layout_is_input": (H, [("inline constexpr auto dot_layout = detail::make_scalar_part_layout<Alg, A, B>();",
                                  "inline constexpr auto dot_layout = A;", None)]),
    "antidot_layout_is_input": (H, [("inline constexpr auto antidot_layout =\n    left_complement_layout<Alg, dot_layout<Alg, right_complement_layout<Alg, A>,\n                                           right_complement_layout<Alg, B>>>;",
                                      "inline constexpr auto antidot_layout = A;", None)]),
    "project_layout_is_input": (H, [("inline constexpr auto project_layout =\n    antiwedge_layout<Alg, B, wedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;",
                                      "inline constexpr auto project_layout = A;", None)]),
}


def mutate(text, old, new, occurrences):
    parts = text.split(old)
    if occurrences is None:
        if len(parts) != 2:
            sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
        return new.join(parts)
    out = parts[0]
    for k, part in enumerate(parts[1:]):
        out += (new if k in occurrences else old) + part
    if max(occurrences) >= len(parts) - 1:
        sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
    return out


def timed(step, preset, selection, command):
    return subprocess.run(
        [str(ROOT / "tools/timed"), step, preset, selection, "--", *command],
        cwd=ROOT, capture_output=True, text=True)


def failed_builds(output):
    """The object files that failed and the first error line of each."""
    failures = []
    lines = output.splitlines()
    for k, line in enumerate(lines):
        if line.startswith("FAILED: "):
            source = re.search(r"(\w+)_test\.cpp\.o", line)
            first = next((rest.strip() for rest in lines[k + 1:]
                          if "error:" in rest), "-")
            first = first.replace(f"{ROOT}/", "")[:400]
            failures.append(f"{source.group(1) if source else line}: {first}")
    return failures


def run(preset, name):
    header, edits = MUTANTS[name]
    path = CORE / header
    original = path.read_text()
    text = original
    for old, new, occurrences in edits:
        text = mutate(text, old, new, occurrences)
    path.write_text(text)
    try:
        build = timed("build", preset, f"mutant:{name}",
                      ["ninja", "-C", f"build/{preset}", "-k", "0", *TARGETS])
        tests = timed("test", preset, f"mutant:{name}",
                      ["ctest", "--test-dir", f"build/{preset}", "-L",
                       r"^core\.dual$"])
    finally:
        path.write_text(original)
    failed = re.findall(r"^\s*\d+ - (\S+) \(", tests.stdout, re.MULTILINE)
    return [name, str(build.returncode),
            " | ".join(failed_builds(build.stdout)) or "-",
            " ".join(failed) or "-"]


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    preset = sys.argv[1]
    names = sys.argv[2:] or list(MUTANTS)
    OUT.mkdir(exist_ok=True)
    record = OUT / f"{preset}.tsv"
    # A run of some mutants replaces their rows and keeps the others.
    rows = {}
    if record.exists():
        for line in record.read_text(encoding="utf-8").splitlines()[1:]:
            rows[line.split("\t")[0]] = line.split("\t")
    for name in names:
        rows[name] = run(preset, name)
        print("\t".join(rows[name]), flush=True)
    header = ["mutant", "build_exit", "build_failures", "failed_tests"]
    ordered = [rows[name] for name in MUTANTS if name in rows]
    with open(record, "w", encoding="utf-8") as f:
        f.write("".join("\t".join(r) + "\n" for r in [header, *ordered]))

    # Rebuild the unmutated programs, so that the tree matches the headers.
    rebuilt = timed("build", preset, "mutant:restore",
                    ["ninja", "-C", f"build/{preset}", *TARGETS])
    if rebuilt.returncode != 0:
        sys.exit("the restored headers do not build")


if __name__ == "__main__":
    main()
