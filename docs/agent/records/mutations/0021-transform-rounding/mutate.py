#!/usr/bin/env -S python3 -I
"""Mutation runs for the rounding check of transform (transform.hpp).

  mutate.py <preset> [mutant...]

Applies one mutant at a time to transform.hpp, builds the test programs that
judge it with ninja's -k 0 so that each is tried, runs the tests labeled
core.transform (the unit tests, the programs built in each checking mode,
and every compile-fail test, the rounding tests among them), and restores
the header. Every build and test goes through tools/timed with the
selection mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code (a static_assert of the test, or
a constant evaluation the test asks for), or a compile-fail test or a
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
TARGETS = ["core_transform_rounding_test", "core_transform_rounding_mode_1",
           "core_transform_rounding_mode_0", "core_transform_test",
           "core_transform_mode_1", "core_transform_mode_0",
           "core_transform_identities_test"]
LABELS = r"^core\.transform$"

T = "transform.hpp"
TOLERANCE = "    tolerances[j] = static_cast<T>(gamma * magnitude[j]);\n"
WITHIN = ("    const bool within =\n"
          "        size <= tolerances[j] && tolerances[j] <= std::numeric_limits<T>::max();\n")
LOOP = ("  for (std::size_t j = 0; j < D; ++j) {\n"
        "    const T size = dropped[j] < T{} ? static_cast<T>(-dropped[j]) : dropped[j];\n")
ROUNDED_CALL = ("          detail::check_transform_rounded(\n"
                "              dropped, detail::transform_dropped_tolerances<Alg, V, X>(\n"
                "                           v.coefficients, x.coefficients));\n")
MIDDLE = ("  const auto middle =\n"
          "      padded_magnitude<T, sandwich_middle_layout<Alg, V, X>.size()>(\n"
          "          first_kernel<Alg, V, X, true>, abs_v, absolute_values(x));\n")
FAST_MATH = ("#if defined(__FAST_MATH__) && GALOOT_CHECKING == 1\n"
             "#error \"transform: checking mode needs IEEE 754 arithmetic, not -ffast-math\"\n"
             "#endif\n")

MUTANTS = {
    # The tolerance.
    "tolerance_halved": (T, [(TOLERANCE, "    tolerances[j] = static_cast<T>(gamma * magnitude[j] / T{2});\n", None)]),
    "tolerance_zero": (T, [(TOLERANCE, "    tolerances[j] = static_cast<T>(gamma * magnitude[j] * T{0});\n", None)]),
    "margin_0": (T, [("inline constexpr int transform_rounding_margin = 1;", "inline constexpr int transform_rounding_margin = 0;", None)]),
    "margin_2": (T, [("inline constexpr int transform_rounding_margin = 1;", "inline constexpr int transform_rounding_margin = 2;", None)]),
    # The count of roundings.
    "k_undercounted_second_stage": (T, [("    int term = x[t.a] + y[t.b] + 1;", "    int term = x[t.a] + y[t.b] + (x[t.a] > 0 ? 0 : 1);", None)]),
    "k_coefficient_product_uncounted": (T, [("    if (t.coefficient != 1 && t.coefficient != -1) {\n      ++term;\n    }\n", "", None)]),
    "k_first_term_counted": (T, [("      out[t.out] = term;\n", "      out[t.out] = term + 1;\n", None)]),
    # The magnitude.
    "magnitude_from_computed_middle": (T, [(MIDDLE, "  const auto middle =\n      absolute_values(transform_middle<Alg, V, X, true>(v, x));\n", None)]),
    "padding_removed": (T, [("  constexpr T padding = std::numeric_limits<T>::min();", "  constexpr T padding = T{};", None)]),
    "magnitude_coefficient_ignored": (T, [("      term = static_cast<T>(static_cast<T>(c * term) + padding);", "      term = static_cast<T>(static_cast<T>(T{1} * term) + padding + (c * T{0}));", None)]),
    # The comparison.
    "nan_dropped_passes": (T, [(LOOP, LOOP.replace("  for (std::size_t j = 0; j < D; ++j) {\n", "  for (std::size_t j = 0; j < D; ++j) {\n    if (dropped[j] != dropped[j]) {\n      continue;\n    }\n"), None)]),
    "tolerance_finite_removed": (T, [(WITHIN, "    const bool within = size <= tolerances[j];\n", None)]),
    "rounding_check_removed": (T, [(ROUNDED_CALL, "          static_cast<void>(dropped);\n", None)]),
    "float_unchecked": (T, [("                  detail::rounding_checked_scalar<T>) {", "                  (detail::rounding_checked_scalar<T> && !std::is_same_v<T, float>)) {", None)]),
    "exact_check_removed": (T, [("          detail::check_transform_dropped(dropped);\n", "          static_cast<void>(dropped);\n", None)]),
    "fast_math_error_removed": (T, [(FAST_MATH, "", None)]),
    # The guard of the margin's hypothesis, which no transform that builds reaches.
    "margin_hypothesis_removed": (T, [("  static_assert(margin_suffices,", "  static_assert(margin_suffices || true,", None)]),
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
