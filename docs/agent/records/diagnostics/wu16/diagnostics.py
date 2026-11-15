#!/usr/bin/env -S python3 -I
"""The diagnostics of the rga3 motions: for each compile-fail source of
test/presets/rga3 (rotation over a scalar without sin and cos, with an angle of
another type, or about a multivector that is not a Line; translation over
an integer scalar or over scalars of two types), compile with the test
flags and -fsyntax-only on GCC and Clang, and print the length of the
first error line, the longest line, the line count, the number of lines
that hold "error:", and the first error line itself. Every compile goes
through tools/timed with the selection diagnostics:<compiler>:<source>.

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
    result = subprocess.run(
        [str(ROOT / "tools/timed"), "diagnostics", "-",
         f"diagnostics:{compiler}:{source.name}", "--", compiler, *FLAGS,
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
    sources = sorted((ROOT / "test/presets/rga3").glob("*_fail.cpp"))
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
