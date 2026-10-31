"""Fault injection for galoot/core/bits.hpp. Snippet candidate.

Usage: python3 -I docs/agent/snippets/bits_fault_injection.py <worktree>
       <preset> [--runtime-only]

Written to show that the bits tests can fail. Last checked at 106069e with
gcc-debug, in both modes: every mutation caught.

With --runtime-only, the static_assert lines of test/core/bits_test.cpp are
removed for the run (and restored), so each mutation must be caught by a
run-time test or by a compile-fail test rather than by a constant evaluation.

Applies each mutation below to include/galoot/core/bits.hpp in <worktree>,
builds the core.bits targets and runs ctest -L '^core\\.bits$' through
tools/timed, records whether the mutation was caught (build failed or a test
failed), and restores both files from the text it read first. It refuses to
run (exit 2) if either file has uncommitted changes. Needs the job-slot pool in
MAKEFLAGS. Exits 1 if any mutation survives.
"""

import os
import subprocess
import sys

MUTATIONS = [
    ("reorder_counts_equal", "for (a >>= 1U; a != 0; a >>= 1U)",
     "for (; a != 0; a >>= 1U)"),
    # These two keep every variable in use, so -Werror does not end the build.
    ("reorder_always_plus", "return swaps % 2 == 0 ? 1 : -1;",
     "return swaps % 2 == 0 ? 1 : 1;"),
    ("right_sign_swapped", "return reorder_sign(a, complement_mask(a, n));",
     "return reorder_sign(complement_mask(a, n), a);"),
    ("left_sign_swapped", "return reorder_sign(complement_mask(a, n), a);",
     "return reorder_sign(a, complement_mask(a, n));"),
    ("reverse_k_plus_1", "return (k * (k - 1) / 2) % 2 == 0 ? 1 : -1;",
     "return (k * (k + 1) / 2) % 2 == 0 ? 1 : -1;"),
    ("antireverse_uses_grade", "const int m = antigrade(a, n);",
     "const int m = static_cast<int>(n) - antigrade(a, n);"),
    ("involution_mod_4", "return grade(a) % 2 == 0 ? 1 : -1;",
     "return grade(a) % 4 == 0 ? 1 : -1;"),
    ("complement_drops_top", "return ~a & detail::low_bits(n);",
     "return ~a & (detail::low_bits(n) >> 1U);"),
    ("count_rejects_32", "return n >= 1 && n <= mask_bits;",
     "return n >= 1 && n < mask_bits;"),
    # n is unsigned, so n >= 0 would end the build on -Wtype-limits instead.
    ("count_accepts_0", "return n >= 1 && n <= mask_bits;",
     "return n <= mask_bits;"),
    # Still uses both parameters, so -Wunused-parameter does not end the build.
    ("mask_check_dropped", "return (a & ~low_bits(n)) == 0;",
     "return (a & ~low_bits(n)) == 0 || a != 0;"),
]


def run(worktree, args):
    return subprocess.run(args, cwd=worktree, capture_output=True,
                          text=True).returncode


def main():
    worktree, preset = os.path.abspath(sys.argv[1]), sys.argv[2]
    runtime_only = sys.argv[3:] == ["--runtime-only"]
    header = os.path.join(worktree, "include/galoot/core/bits.hpp")
    test = os.path.join(worktree, "test/core/bits_test.cpp")
    timed = os.path.join(worktree, "tools/timed")

    # The files are rewritten and then restored from the text read here, so
    # uncommitted edits would be at risk only if a restore failed; refusing a
    # dirty tree keeps that from losing work.
    dirty = subprocess.run(
        ["git", "-C", worktree, "status", "--porcelain", "--", header, test],
        capture_output=True, text=True, check=True).stdout
    if dirty:
        print(f"refused: uncommitted changes in\n{dirty}", file=sys.stderr)
        sys.exit(2)
    with open(header) as f:
        original = f.read()
    with open(test) as f:
        original_test = f.read()

    survivors = 0
    try:
        if runtime_only:
            lines = original_test.splitlines(keepends=True)
            kept = [line for line in lines
                    if not line.startswith("static_assert(")]
            print(f"removed {len(lines) - len(kept)} static_assert lines")
            with open(test, "w") as f:
                f.writelines(kept)
        for name, old, new in MUTATIONS:
            if original.count(old) != 1:
                print(f"{name}: pattern found {original.count(old)} times")
                survivors += 1
                continue
            with open(header, "w") as f:
                f.write(original.replace(old, new))
            built = run(worktree, [
                timed, "build", preset, f"mutation:{name}", "--", "cmake",
                "--build", "--preset", preset, "--target", "core_bits_test",
                "core_bits_assert_fail"]) == 0
            if not built:
                print(f"{name}: caught (build failed)")
                continue
            tested = run(worktree, [
                timed, "test", preset, f"mutation:{name}", "--", "ctest",
                "--preset", preset, "-L", r"^core\.bits$"]) == 0
            if tested:
                print(f"{name}: SURVIVED")
                survivors += 1
            else:
                failed_log = os.path.join(
                    worktree, "build", preset,
                    "Testing/Temporary/LastTestsFailed.log")
                with open(failed_log) as f:
                    failed = [line.split(":", 1)[1].strip() for line in f]
                print(f"{name}: caught by {len(failed)} tests: "
                      + ", ".join(failed))
    finally:
        with open(header, "w") as f:
            f.write(original)
        with open(test, "w") as f:
            f.write(original_test)
    sys.exit(1 if survivors else 0)


if __name__ == "__main__":
    main()
