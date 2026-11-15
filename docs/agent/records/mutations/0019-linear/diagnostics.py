#!/usr/bin/env -S python3 -I
"""Sizes of the WU10b diagnostics: for each new compile-fail source of the
sums, the scalings, part, and the scalar's ring operations, and for the two
existing sources whose message changed, compile with the test flags and
-fsyntax-only on GCC and Clang, and print the length of the first error
line, the longest line, the line count, and the number of lines that hold
"error:". Every compile goes through tools/timed with the selection
diagnostics:<compiler>:<source>.

Usage, from the worktree root: diagnostics.py <out-file>
"""

import pathlib
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
FLAGS = ["-DGALOOT_CHECKING=1", "-Wall", "-Wextra", "-Wpedantic",
         "-Wconversion", "-Wshadow", "-Werror", "-ffp-contract=off",
         "-D_GLIBCXX_ASSERTIONS", "-O0", "-std=c++23", "-fsyntax-only"]


def measure(compiler, source):
    name = source.name
    result = subprocess.run(
        [str(ROOT / "tools/timed"), "diagnostics", "-",
         f"diagnostics:{compiler}:{name}", "--", compiler, *FLAGS,
         f"-I{ROOT / 'include'}", f"-I{ROOT / 'test'}", str(source)],
        cwd=ROOT, capture_output=True, text=True)
    lines = [line.replace(f"{ROOT}/", "")
             for line in result.stderr.splitlines()
             if not line.startswith("timed:")]
    errors = [line for line in lines if "error:" in line]
    first = errors[0] if errors else ""
    return (len(first), max((len(line) for line in lines), default=0),
            len(lines), len(errors), first)


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    core = ROOT / "test/core"
    # linear_divide_assert_fail.cpp is a program that runs, not a source
    # that must fail to compile.
    sources = [s for s in sorted(core.glob("linear_*_fail.cpp"))
               if s.name != "linear_divide_assert_fail.cpp"]
    sources += sorted(core.glob("mv_part_*_fail.cpp"))
    sources += sorted(core.glob("mv_scalar_lacks_*_fail.cpp"))
    sources += [core / f"mv_{name}_fail.cpp" for name in (
        "scalar_without_arithmetic", "scalar_without_multiplication",
        "scalar_without_multiplication_or_negation",
        "scalar_without_negation_or_construction", "not_ring_scalar")]
    rows = ["source\tcompiler\tfirst_line\tlongest\tlines\terror_lines\t"
            "first_error"]
    for source in sources:
        for compiler in ("g++", "clang++"):
            first, longest, count, errors, text = measure(compiler, source)
            rows.append(f"{source.name}\t{compiler}\t{first}\t{longest}\t"
                        f"{count}\t{errors}\t{text}")
    pathlib.Path(sys.argv[1]).write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
