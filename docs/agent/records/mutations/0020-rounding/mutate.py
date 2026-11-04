#!/usr/bin/env -S python3 -I
"""Mutation runs for the rounding tests over double and the tracking scalar.

  mutate.py <preset> [mutant...]

Applies one mutant at a time to a core header or to test/support/tracked.hpp,
builds the programs that judge it (core_rounding_test, support_tracked_test)
with ninja's -k 0 so that every program is tried, runs the tests labeled
core.rounding or support.tracked, and restores the file. Every build and
test goes through tools/timed with the selection mutant:<name>.

A mutant is killed when a test fails: a program that does not compile
because of an error in the test's own code, or a test that fails at run
time. An error that comes from a warning made an error does not count; the
record lists each compile error's first line so that this can be read off.
When a program does not build, CTest runs its previous build, so the failed
list can omit that program's test; the build failures column shows it.

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
OUT = pathlib.Path(__file__).resolve().parent / "out"
TARGETS = ["core_rounding_test", "support_tracked_test"]

# A helper placed before run_product, for the float mutant: each product of
# a floating type is rounded to float before it is accumulated.
FLOAT_PRODUCT = """template <class T>
constexpr T product_through_float(const T& a, const T& b) {
  if constexpr (std::is_floating_point_v<T>) {
    return static_cast<T>(static_cast<float>(a * b));
  } else {
    return static_cast<T>(a * b);
  }
}

"""
RUN_PRODUCT = """template <const auto& Kernel, std::size_t R, class T, std::size_t KX,
          std::size_t KY, std::size_t... K>
constexpr std::array<T, R> run_product("""

# For the nudge mutants, from the blind review: after every addition or
# subtraction of the product and map kernels, at run time over double, a
# result above 2^-9 in magnitude is moved N doubles away from zero.
NUDGE = """#include <cmath>
namespace galoot::core::detail {
template <class T> constexpr T review_nudge(T v, int n) {
  if constexpr (std::is_same_v<T, double>) {
    if (!std::is_constant_evaluated()) {
      if (v > 0x1p-9 || v < -0x1p-9) {
        for (int i = 0; i < n; ++i) {
          v = std::nextafter(v, v < 0 ? -1e300 : 1e300);
        }
      }
    }
  }
  return v;
}
}
"""


def nudge(n):
    return ("include/galoot/core/mv.hpp", [
        ("#include <galoot/core/assert.hpp>",
         NUDGE + "#include <galoot/core/assert.hpp>", None),
        ("out = static_cast<T>(out + value);",
         f"out = review_nudge(static_cast<T>(out + value), {n});", None),
        ("out = static_cast<T>(out - value);",
         f"out = review_nudge(static_cast<T>(out - value), {n});", None)])


# name: (file, [(old, new, occurrences)]). occurrences lists which
# occurrences of old, counted from 0, are replaced; None means old occurs
# exactly once.
MUTANTS = {
    # The four of the plan.
    "products_rounded_through_float": ("include/galoot/core/mv.hpp", [
        (RUN_PRODUCT, FLOAT_PRODUCT + RUN_PRODUCT, None),
        ("static_cast<T>(x[Kernel[K].a] * y[Kernel[K].b])",
         "product_through_float(x[Kernel[K].a], y[Kernel[K].b])", None)]),
    "divide_multiplies_by_reciprocal": ("include/galoot/core/linear.hpp", [(
        "static_cast<T>(x.coefficients[i] / s);",
        "static_cast<T>(\n"
        "        x.coefficients[i] * (static_cast<T>(coefficient_type{1}) / s));",
        None)]),
    "tracked_sum_adds_no_rounding": ("test/support/tracked.hpp", [(
        "r.roundings = detail::larger(a.roundings, b.roundings) + 1;",
        "r.roundings = detail::larger(a.roundings, b.roundings);", [0])]),
    "margin_removed": ("test/support/tracked.hpp", [(
        "inline constexpr int rounding_margin = 2;",
        "inline constexpr int rounding_margin = 0;", None)]),
    # Additional, by the implementer.
    "tracked_product_counts_the_larger": ("test/support/tracked.hpp", [(
        "r.roundings = a.roundings + b.roundings + 1;",
        "r.roundings = detail::larger(a.roundings, b.roundings) + 1;", None)]),
    "exact_results_checked_by_tolerance": ("test/support/tracked.hpp", [(
        "  if (roundings == 0) {\n    return exact_rational(d) == exact;\n  }\n",
        "", None)]),
    # A first term of coefficient -1 multiplied by -1 instead of negated:
    # the same double, but a rounding the exact operations must not count.
    "negation_by_multiplication": ("include/galoot/core/mv.hpp", [(
        "      out = static_cast<T>(-value);",
        "      out = static_cast<T>(static_cast<T>(C) * value);", None)]),
    # From the blind review: one and two doubles of extra error after each
    # sum in the kernels. One survived the check as first written.
    "kernel_sums_nudged_one_ulp": nudge(1),
    "kernel_sums_nudged_two_ulps": nudge(2),
    # Added with the review's fixes: a result of one rounding checked by the
    # margin's tolerance instead of equality with the nearest double, and a
    # result of several checked without the ratio of its exact error.
    "one_rounding_checked_by_tolerance": ("test/support/tracked.hpp", [(
        "  if (roundings == 1) {\n    return d == nearest;\n  }\n", "", None)]),
    "ratio_check_removed": ("test/support/tracked.hpp", [(
        " &&\n         bound_ratio(d, exact, roundings, magnitude) <= ratio_limit;",
        ";", None)]),
}


def mutate(text, old, new, occurrences):
    parts = text.split(old)
    if occurrences is None:
        if len(parts) != 2:
            sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
        return new.join(parts)
    if max(occurrences) >= len(parts) - 1:
        sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
    out = parts[0]
    for k, part in enumerate(parts[1:]):
        out += (new if k in occurrences else old) + part
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
    relative, edits = MUTANTS[name]
    path = ROOT / relative
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
                       r"^(core\.rounding|support\.tracked)$"])
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

    # Rebuild the unmutated programs, so that the tree matches the sources.
    rebuilt = timed("build", preset, "mutant:restore",
                    ["ninja", "-C", f"build/{preset}", *TARGETS])
    if rebuilt.returncode != 0:
        sys.exit("the restored sources do not build")


if __name__ == "__main__":
    main()
