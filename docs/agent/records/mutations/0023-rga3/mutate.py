#!/usr/bin/env -S python3 -I
"""Mutation runs for rotation and translation (galoot/rga3/motions.hpp).

  mutate.py <preset> [mutant...]

Applies one mutant at a time to motions.hpp, builds rga3_motions_test with
ninja's -k 0, runs every test labeled rga3.motions (the program and every
compile-fail test of the label), and restores the header. Every build and
test goes through tools/timed with the selection mutant:<name>.

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
HEADER = ROOT / "include/galoot/rga3/motions.hpp"
OUT = pathlib.Path(__file__).resolve().parent / "out"
TARGETS = ["rga3_motions_test"]
LABELS = r"^rga3\.motions$"

# A layout other than the motor's, the core's grade order, for the mutants
# of the return type.
OTHER = 'core::mv<algebra, core::blades<algebra, "1 e41 e42 e43 e23 e31 e12 e1234">, T>'

# The head of each catch-all, from its template line; DELETE removes the
# declaration through its closing brace.
NON_FLOATING = "template <class T, class S>\n  requires(!std::floating_point<T>)\n"
ANGLE_TYPE = "template <std::floating_point T, class S>\n  requires(!std::same_as<S, T>)\n"
AXIS = "template <core::multivector X, class S>\n  requires(!std::same_as<X, Line<typename X::scalar_type>>)\n"
INTEGER = "template <std::integral T>\nMotor<T> translation("
MIXED = ("template <core::ring_scalar X, core::ring_scalar Y, core::ring_scalar Z>\n"
         "  requires(!(std::same_as<X, Y> && std::same_as<Y, Z>))\n")
DELETE = object()

MUTANTS = {
    # rotation's values.
    "rotation_full_angle": [("const T half = theta / T{2};", "const T half = theta;")],
    "rotation_negative_angle": [("const T half = theta / T{2};", "const T half = -theta / T{2};")],
    "rotation_sin_cos_swapped": [("const T s = std::sin(half);\n  const T c = std::cos(half);",
                                  "const T s = std::cos(half);\n  const T c = std::sin(half);")],
    "rotation_layout": [("[[nodiscard]] inline Motor<T> rotation(", "[[nodiscard]] inline auto rotation("),
                        ("  return Motor<T>{\n      v[0] * s,", f"  return {OTHER}{{\n      v[0] * s,")],
    "rotation_any_scalar": [("template <std::floating_point T>\n[[nodiscard]] inline",
                             "template <class T>\n[[nodiscard]] inline")],
    # translation's values.
    "translation_full_d": [("dx / T{2}, dy / T{2}, dz / T{2}", "dx, dy, dz")],
    "translation_negative_half_d": [("dx / T{2}, dy / T{2}, dz / T{2}", "-dx / T{2}, -dy / T{2}, -dz / T{2}")],
    "translation_d_permuted": [("dx / T{2}, dy / T{2}, dz / T{2}", "dy / T{2}, dz / T{2}, dx / T{2}")],
    "translation_layout": [("[[nodiscard]] constexpr Motor<T> translation(", "[[nodiscard]] constexpr auto translation("),
                           ("  return Motor<T>{\n      T{0}, T{0}", f"  return {OTHER}{{\n      T{{0}}, T{{0}}")],
    "translation_accepts_integers": [("template <core::ring_scalar T>\n  requires(!std::integral<T>)\n",
                                      "template <core::ring_scalar T>\n")],
    # Each catch-all's constraint removed: the requires-clause, and then
    # also the type-constraints of its template parameters.
    "non_floating_catch_all_unconstrained": [(NON_FLOATING, "template <class T, class S>\n")],
    "angle_type_catch_all_unconstrained": [(ANGLE_TYPE, "template <std::floating_point T, class S>\n")],
    "angle_type_catch_all_any_type": [(ANGLE_TYPE, "template <class T, class S>\n")],
    "axis_catch_all_unconstrained": [(AXIS, "template <core::multivector X, class S>\n")],
    "axis_catch_all_any_type": [(AXIS, "template <class X, class S>\n")],
    "integer_catch_all_any_type": [(INTEGER, "template <class T>\nMotor<T> translation(")],
    "mixed_catch_all_unconstrained": [(MIXED, "template <core::ring_scalar X, core::ring_scalar Y, core::ring_scalar Z>\n")],
    "mixed_catch_all_any_type": [(MIXED, "template <class X, class Y, class Z>\n")],
    # Each catch-all removed.
    "non_floating_catch_all_removed": [(NON_FLOATING, DELETE)],
    "angle_type_catch_all_removed": [(ANGLE_TYPE, DELETE)],
    "axis_catch_all_removed": [(AXIS, DELETE)],
    "integer_catch_all_removed": [(INTEGER, DELETE)],
    "mixed_catch_all_removed": [(MIXED, DELETE)],
}


def mutate(text, old, new):
    parts = text.split(old)
    if len(parts) != 2:
        sys.exit(f"anchor occurs {len(parts) - 1} times: {old!r}")
    if new is DELETE:
        start = text.index(old)
        end = text.index("\n}\n", start) + len("\n}\n")
        return text[:start] + text[end:]
    return new.join(parts)


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
    original = HEADER.read_text()
    text = original
    for old, new in MUTANTS[name]:
        text = mutate(text, old, new)
    HEADER.write_text(text)
    try:
        build = timed("build", preset, f"mutant:{name}",
                      ["ninja", "-C", f"build/{preset}", "-k", "0", *TARGETS])
        tests = timed("test", preset, f"mutant:{name}",
                      ["ctest", "--test-dir", f"build/{preset}", "-L",
                       LABELS])
    finally:
        HEADER.write_text(original)
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

    # Rebuild the unmutated program, so that the tree matches the header.
    rebuilt = timed("build", preset, "mutant:restore",
                    ["ninja", "-C", f"build/{preset}", *TARGETS])
    if rebuilt.returncode != 0:
        sys.exit("the restored header does not build")


if __name__ == "__main__":
    main()
