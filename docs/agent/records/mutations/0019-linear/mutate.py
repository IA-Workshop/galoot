#!/usr/bin/env -S python3 -I
"""Mutation runs for the sums, the scaling, the negation, part, and the
scalar's ring-operation messages.

  mutate.py <preset> [mutant...]

Applies one mutant at a time to a core header, builds the programs that
judge it (core_linear_test, core_linear_divide_assert_fail, core_mv_test,
core_mv_part_checking) with ninja's -k 0 so that every program is tried,
runs the tests labeled core.linear or core.mv (the programs, their
compile-fail tests, and the run-time checks), and restores the header.
Every build and test goes through tools/timed with the selection
mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a call the test makes that has no overload), or a compile-fail test or a
run-time check that fails. An error that comes from a warning made an error
does not count; the record lists each compile error's first line so that
this can be read off. When a program does not build, CTest runs its
previous build, so the failed list can omit that program's test; the build
failures column shows it.

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
TARGETS = ["core_linear_test", "core_linear_divide_assert_fail",
           "core_mv_test", "core_mv_part_checking"]

PART_BODY = """[[nodiscard]] constexpr mv<Alg, To, T> part(const mv<Alg, From, T>& x) {
  mv<Alg, To, T> kept;
  kept.coefficients =
      detail::run_map<detail::identity_kernel<From, To>, To.size()>(
          x.coefficients,
          std::make_index_sequence<detail::identity_kernel<From, To>.size()>{});
  return kept;
}"""

# name: (header, [(old, new, occurrences)]). occurrences lists which
# occurrences of old, counted from 0, are replaced; None means old occurs
# exactly once.
MUTANTS = {
    # Every term of a sum is added to the zero already in its place.
    "sum_adds_first_term_to_zero": ("linear.hpp", [
        ("(accumulate<KX[I].coefficient, KX[I].first>(",
         "(accumulate<KX[I].coefficient, false>(", None),
        ("(accumulate<KY[J].coefficient, KY[J].first>(",
         "(accumulate<KY[J].coefficient, false>(", None)]),
    # The second operand's terms ignore the sign of the operation.
    "difference_takes_y_with_plus": ("linear.hpp", [(
        ".coefficient = Sign * B.blades[k].sign * Target.blades[at].sign,",
        ".coefficient = B.blades[k].sign * Target.blades[at].sign,", None)]),
    # A blade that only y holds gets the coefficient 0.
    "sum_drops_blades_only_y_holds": ("linear.hpp", [(
        ".coefficient = Sign * B.blades[k].sign * Target.blades[at].sign,",
        ".coefficient = position_of(A, B.blades[k].mask) == A.size()\n"
        "                           ? 0\n"
        "                           : Sign * B.blades[k].sign *\n"
        "                                 Target.blades[at].sign,", None)]),
    "scale_left_multiplies_on_the_right": ("linear.hpp", [(
        "static_cast<T>(s * x.coefficients[i]);",
        "static_cast<T>(x.coefficients[i] * s);", None)]),
    "divide_multiplies_by_reciprocal": ("linear.hpp", [(
        "static_cast<T>(x.coefficients[i] / s);",
        "static_cast<T>(\n"
        "        x.coefficients[i] * (static_cast<T>(coefficient_type{1}) / s));",
        None)]),
    "divide_zero_check_removed": ("linear.hpp", [(
        '    GALOOT_ASSERT(s != T{}, "a / s: an integer scalar cannot divide by '
        'zero");\n', "", None)]),
    # The rejecting overloads of the scalings take a scalar of any type.
    "scalar_catch_all_any_type": ("linear.hpp", [(
        "multivector<X> && !multivector_or_derived<S> && ring_scalar<S> &&",
        "multivector<X> && !multivector_or_derived<S> &&", None)]),
    "part_calls_truncate": ("mv.hpp", [(
        PART_BODY,
        "[[nodiscard]] constexpr mv<Alg, To, T> part(const mv<Alg, From, T>& "
        "x) {\n  return truncate<To>(x);\n}", None)]),
    # A scalar without addition is reported as one without subtraction.
    "ring_fault_reports_next_message": ("mv.hpp", [(
        "    return ScalarFault::no_addition;",
        "    return ScalarFault::no_subtraction;", None)]),
    # Additional, by the implementer: negation as a subtraction from zero,
    # and a sum of two layouts that keeps the operands' orientations.
    "negation_subtracts_from_zero": ("linear.hpp", [(
        "static_cast<T>(-x.coefficients[i]);",
        "static_cast<T>(T{} - x.coefficients[i]);", None)]),
    "sum_layout_keeps_orientations": ("linear.hpp", [(
        "    merge<detail::with_plus_signs(A), detail::with_plus_signs(B)>;",
        "    merge<A, B>;", None)]),
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
            source = re.search(r"(\w+)\.cpp\.o", line)
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
                       r"^core\.(linear|mv)$"])
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
