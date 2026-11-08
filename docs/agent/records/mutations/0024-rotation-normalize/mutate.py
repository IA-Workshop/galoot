#!/usr/bin/env -S python3 -I
"""Mutants of the normalization in rotation (galoot/presets/rga3/motions.hpp).

  mutate.py <preset> [mutant...]

Applies one mutant at a time to the header, builds rga3_motions_test and
the two programs of the checking-mode tests, runs the CTests rga3.motions
and rga3.rotation_checking_* and rga3.rotation_unchecked_*, and restores
the header. Every build and test goes through tools/timed with the
selection mutant:<name>. A mutant is killed when the build fails in the
test's own code or a check fails. Writes out/<preset>.tsv: mutant, build
exit, test exit, the failed CTests and doctest test cases.

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
HEADER = ROOT / "include/galoot/presets/rga3/motions.hpp"
OUT = pathlib.Path(__file__).resolve().parent / "out"

LENGTH = "const T length = std::sqrt(((v[0] * v[0]) + (v[1] * v[1])) + (v[2] * v[2]));"
SINE = "const T s = std::sin(half) / length;"
DIRECTION = "v[0] * s, v[1] * s, v[2] * s, c,"
MOMENT = "v[3] * s, v[4] * s, v[5] * s, T{0},"
COSINE = "const T c = std::cos(half);"
ASSERT = """  // A square root is never subnormal, so a length that is not normal is
  // zero, infinite, or NaN.
  GALOOT_ASSERT(std::isnormal(length), "rotation: the direction of the axis "
                                       "must have a finite nonzero length");
"""
TARGETS = ["rga3_motions_test", "rga3_rotation_mode_1", "rga3_rotation_mode_0"]
TESTS = r"^rga3\.(motions|rotation_(checking|unchecked)_.*)$"

MUTANTS = {
    # The division left out, and the length and its check with it, so that
    # no unused or undeclared variable stops the build: the rotor of l as
    # given.
    "division_omitted": [(LENGTH, ""), (ASSERT, ""),
                         (SINE, "const T s = std::sin(half);")],
    # Only the moment divided, the direction times the plain sine.
    "moment_only": [(SINE, SINE + "\n  const T r = std::sin(half);"),
                    (DIRECTION, "v[0] * r, v[1] * r, v[2] * r, c,")],
    # Only the direction divided, the moment times the plain sine.
    "direction_only": [(SINE, SINE + "\n  const T r = std::sin(half);"),
                       (MOMENT, "v[3] * r, v[4] * r, v[5] * r, T{0},")],
    # Wrong norms: the squared length, the length of all six coefficients,
    # a direction component left out, and multiplying by the length.
    "norm_squared": [(LENGTH, "const T length = ((v[0] * v[0]) + (v[1] * v[1])) + (v[2] * v[2]);")],
    "norm_with_moment": [(LENGTH, "const T length = std::sqrt(((v[0] * v[0]) + (v[1] * v[1])) + (v[2] * v[2]) + (v[3] * v[3]) + (v[4] * v[4]) + (v[5] * v[5]));")],
    "norm_without_z": [(LENGTH, "const T length = std::sqrt((v[0] * v[0]) + (v[1] * v[1]));")],
    "times_length": [(SINE, "const T s = std::sin(half) * length;")],
    # Added after the blind review: its three mutants, the cosine divided
    # too, |v_z| as the length, and the 1-norm, and the check of the length
    # removed.
    "cos_divided": [(COSINE, "const T c = std::cos(half) / length;")],
    "length_abs_vz": [(LENGTH, "const T length = std::abs(v[2]);")],
    "length_l1": [(LENGTH, "const T length = (std::abs(v[0]) + std::abs(v[1])) + std::abs(v[2]);")],
    "assert_removed": [(ASSERT, "")],
}


def timed(preset, name, step, command):
    return subprocess.run(
        [str(ROOT / "tools/timed"), step, preset, f"mutant:{name}", "--"] + command,
        cwd=ROOT, capture_output=True, text=True)


def main():
    preset = sys.argv[1]
    names = sys.argv[2:] or list(MUTANTS)
    original = HEADER.read_text(encoding="utf-8")
    OUT.mkdir(exist_ok=True)
    rows = []
    try:
        for name in names:
            text = original
            for old, new in MUTANTS[name]:
                assert text.count(old) == 1, (name, old)
                text = text.replace(old, new)
            HEADER.write_text(text, encoding="utf-8")
            build = timed(preset, name, "build", [
                "cmake", "--build", "--preset", preset, "--target"] + TARGETS)
            test_exit = "-"
            failed = ""
            if build.returncode == 0:
                test = timed(preset, name, "test", [
                    "ctest", "--preset", preset, "-R", TESTS,
                    "--output-on-failure"])
                test_exit = str(test.returncode)
                cases = sorted(set(re.findall(r"^TEST CASE:\s+(.*)$", test.stdout, re.M)))
                tests = sorted(set(re.findall(
                    r"^\s*\d+ - (\S+) \(", test.stdout, re.M)))
                failed = "; ".join(tests + cases)
            else:
                errors = re.findall(r"error: .*", build.stdout + build.stderr)
                failed = errors[0] if errors else "build failed"
            rows.append((name, str(build.returncode), test_exit, failed))
            print("\t".join(rows[-1]), flush=True)
    finally:
        HEADER.write_text(original, encoding="utf-8")
    with open(OUT / f"{preset}.tsv", "w", encoding="utf-8") as f:
        f.write("mutant\tbuild_exit\ttest_exit\tfailed\n")
        for row in rows:
            f.write("\t".join(row) + "\n")


if __name__ == "__main__":
    main()
