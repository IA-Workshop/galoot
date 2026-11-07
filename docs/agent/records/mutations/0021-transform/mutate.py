#!/usr/bin/env -S python3 -I
"""Mutation runs for compose and transform (transform.hpp) and antiproject
(dual.hpp).

  mutate.py <preset> [mutant...]

Applies one mutant at a time to its header, builds the test programs that
judge it with ninja's -k 0 so that each is tried, runs the tests labeled
core.transform and core.dual (the programs, the programs built in each
checking mode, and every compile-fail test of both labels), and restores
the header. Every build and test goes through tools/timed with the
selection mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a call the test makes that has no overload), or a compile-fail test or a
run-time check that fails. An error that comes from a warning made an error
does not count; the record lists each compile error's first line so that
this can be read off.

Writes out/<preset>.tsv: mutant, build exit, the programs that failed to
build with the first error line of each, and the failed tests.

Usage, from the root of a worktree that no other build uses, with the job
pool exported.
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
TARGETS = ["core_transform_test", "core_transform_identities_test",
           "core_transform_mode_1", "core_transform_mode_0", "core_dual_test",
           "core_dual_identities_test"]
LABELS = r"^core\.(transform|dual)$"

T = "transform.hpp"
D = "dual.hpp"
CATCH_ALL = "requires detail::mismatched_operands<X, Y>"
NO_ALGEBRA = ("requires multivector<X> && multivector<Y> && "
              "(!detail::same_scalar_v<std::remove_cvref_t<X>, "
              "std::remove_cvref_t<Y>>)")
NO_SCALAR = ("requires multivector<X> && multivector<Y> && "
             "(!detail::same_algebra_v<std::remove_cvref_t<X>, "
             "std::remove_cvref_t<Y>>)")
FIRST_SIGN = "odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;"
FIRST_SIGN_GRADE = "(static_cast<void>(n), odd) && grade(X.blades[t.b].mask) % 2 != 0 ? -1 : 1;"
ODD_SIGN = ("const coefficient_type odd = "
            "antigrade_parity<Alg, V> == Parity::odd ? -1 : 1;")
CHECK_CALL = ("        detail::check_transform_dropped(\n"
              "            detail::transform_outer<Alg, V, X, false>(middle, v.coefficients));\n")
TRANSFORM_CHECK = ("detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n"
                   "                                 Alg, V, X, T> &&\n"
                   "      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n"
                   "                                 Alg, detail::sandwich_middle_layout<Alg, V, X>,\n"
                   "                                 V, T>;")
FIRST_PLAN_CHECK = ("detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n"
                    "                                 Alg, V, X, T>;")
SECOND_PLAN_CHECK = ("detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n"
                     "                                 Alg, detail::sandwich_middle_layout<Alg, V, X>,\n"
                     "                                 V, T>;")
COMPOSE_CHECK = ("detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n"
                 "                                 Alg, A, B, T>;")
COMPOSE_PARITY = ("      detail::antigrade_parity<Alg, A> != detail::Parity::mixed &&\n"
                  "      detail::antigrade_parity<Alg, B> != detail::Parity::mixed;")
ANTIPROJECT = "return wedge(b, antiwedge(a, weight_dual(b)));"
ANTIPROJECT_LAYOUT = "wedge_layout<Alg, B, antiwedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;"
SECOND_LOOP = ("  std::size_t k = 0;\n  for (const typed_term& t : second.terms) {\n"
               "    if (kept_output<Alg, V, X>(t.out) != Kept) {\n      continue;\n    }\n")
FIRST_LOOP = ("  std::size_t k = 0;\n  for (const typed_term& t : first.terms) {\n"
              "    if (!All && !read[t.out]) {\n      continue;\n    }\n")


def check_of(name):
    return f"constexpr bool scalar_holds_every_coefficient_of_the_{name} =\n      "


MUTANTS = {
    # The parity of a versor.
    "parity_by_grade": (T, [("if (antigrade(blade.mask, n) % 2 == 0) {", "if (static_cast<void>(n), grade(blade.mask) % 2 == 0) {", None)]),
    "parity_accepts_mixed": (T, [("    return Parity::mixed;\n", "    return Parity::odd;\n", None)]),
    "parity_rejects_empty": (T, [("return has_odd ? Parity::odd : Parity::even;",
                                  "return has_odd ? Parity::odd : has_even ? Parity::even : Parity::mixed;", None)]),
    # The forms of the transform.
    "odd_sign_minus_for_every_n": (T, [(FIRST_SIGN, FIRST_SIGN_GRADE, None)]),
    "odd_sign_plus_for_every_n": (T, [(FIRST_SIGN, FIRST_SIGN_GRADE, None),
                                      (ODD_SIGN, "const coefficient_type odd = 1;", None)]),
    "reverse_for_antireverse": (T, [("antireverse_sign(V.blades[b].mask, n)", "(static_cast<void>(n), reverse_sign(V.blades[b].mask))", None)]),
    "no_involution_for_odd": (T, [(FIRST_SIGN, "(static_cast<void>(n), static_cast<void>(odd), 1);", None)]),
    "x_orientation_dropped": (T, [("Kept ? X.blades[out].sign : 1;", "1;", None)]),
    "second_stage_drops_a_kept_term": (T, [
        (SECOND_LOOP, SECOND_LOOP + "    if (Kept && !skipped) {\n      skipped = true;\n      continue;\n    }\n", None),
        ("  std::array<kernel_term, second_term_count<Alg, V, X, Kept>()> terms{};\n",
         "  std::array<kernel_term, second_term_count<Alg, V, X, Kept>() - (Kept ? 1 : 0)> terms{};\n  bool skipped = false;\n", None)]),
    "first_stage_drops_a_read_term": (T, [
        (FIRST_LOOP, FIRST_LOOP + "    if (!All && !skipped) {\n      skipped = true;\n      continue;\n    }\n", None),
        ("  std::array<kernel_term, first_term_count<Alg, V, X, All>()> terms{};\n",
         "  std::array<kernel_term, first_term_count<Alg, V, X, All>() - (All ? 0 : 1)> terms{};\n  bool skipped = false;\n", None)]),
    "compose_operands_swapped": (T, [("compose_layout<Alg, A, B>>(q2, q1);", "compose_layout<Alg, A, B>>(q1, q2);", None)]),
    # The exact check.
    "exact_check_removed": (T, [(CHECK_CALL, "", None)]),
    "exact_check_reads_kept": (T, [(CHECK_CALL, "        detail::check_transform_dropped(out.coefficients);\n", None)]),
    "exact_check_twice": (T, [(CHECK_CALL, CHECK_CALL + CHECK_CALL, None)]),
    "exact_check_truncate_message": (T, [('"transform: the result leaves the blades of x"',
                                          '"truncate: a dropped coefficient is not zero"', None)]),
    # The compile-time checks of transform and compose.
    "transform_parity_check_removed": (T, [("detail::antigrade_parity<Alg, V> != detail::Parity::mixed;", "true;", None)]),
    "transform_check_removed": (T, [(TRANSFORM_CHECK, "true;", None)]),
    "transform_checks_first_plan_only": (T, [(TRANSFORM_CHECK, FIRST_PLAN_CHECK, None)]),
    "transform_checks_second_plan_only": (T, [(TRANSFORM_CHECK, SECOND_PLAN_CHECK, None)]),
    "compose_parity_check_removed": (T, [(COMPOSE_PARITY, "      true;", None)]),
    "compose_parity_checks_first_only": (T, [(COMPOSE_PARITY, "      detail::antigrade_parity<Alg, A> != detail::Parity::mixed;", None)]),
    "compose_check_removed": (T, [(check_of("composition") + COMPOSE_CHECK, check_of("composition") + "true;", None)]),
    "compose_checks_reversed_plan": (T, [(check_of("composition") + COMPOSE_CHECK, check_of("composition") +
        "detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,\n                                 Alg, B, A, T>;", None)]),
    "compose_layout_is_input": (T, [("inline constexpr auto compose_layout = geometric_antiproduct_layout<Alg, A, B>;",
                                     "inline constexpr auto compose_layout = A;", None)]),
    "transform_catch_alls_without_algebra_condition": (T, [(CATCH_ALL, NO_ALGEBRA, [0, 1])]),
    "transform_catch_alls_without_scalar_condition": (T, [(CATCH_ALL, NO_SCALAR, [0, 1])]),
    # antiproject.
    "antiproject_operands_swapped": (D, [(ANTIPROJECT, "return wedge(a, antiwedge(b, weight_dual(a)));", None),
        (ANTIPROJECT_LAYOUT, "wedge_layout<Alg, A, antiwedge_layout<Alg, B, weight_dual_layout<Alg, A>>>;", None)]),
    "antiproject_uses_bulk_dual": (D, [(ANTIPROJECT, "return wedge(b, antiwedge(a, bulk_dual(b)));", None),
        (ANTIPROJECT_LAYOUT, "wedge_layout<Alg, B, antiwedge_layout<Alg, A, bulk_dual_layout<Alg, B>>>;", None)]),
    "antiproject_is_project": (D, [(ANTIPROJECT, "return antiwedge(b, wedge(a, weight_dual(b)));", None),
        (ANTIPROJECT_LAYOUT, "antiwedge_layout<Alg, B, wedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;", None)]),
    "antiproject_check_removed": (D, [(check_of("antiprojection") + "detail::weight_coefficients_exact<Alg, B, T>;",
                                       check_of("antiprojection") + "true;", None)]),
    "antiproject_checks_first_operand": (D, [(check_of("antiprojection") + "detail::weight_coefficients_exact<Alg, B, T>;",
                                              check_of("antiprojection") + "detail::weight_coefficients_exact<Alg, A, T>;", None)]),
    "antiproject_checks_bulk": (D, [(check_of("antiprojection") + "detail::weight_coefficients_exact<Alg, B, T>;",
                                     check_of("antiprojection") + "detail::bulk_coefficients_exact<Alg, B, T>;", None)]),
    "antiproject_layout_is_input": (D, [("inline constexpr auto antiproject_layout =\n    " + ANTIPROJECT_LAYOUT,
                                         "inline constexpr auto antiproject_layout = A;", None)]),
    "antiproject_catch_all_without_algebra_condition": (D, [(CATCH_ALL, NO_ALGEBRA, [3])]),
    "antiproject_catch_all_without_scalar_condition": (D, [(CATCH_ALL, NO_SCALAR, [3])]),
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
                       LABELS])
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
