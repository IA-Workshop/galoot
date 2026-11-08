#!/usr/bin/env -S python3 -I
"""Sizes of the diagnostics of WU15's misuses: the R7 gap probes in gaps/
and the compile-fail sources of the formatter and of part 3's catch-alls
(test/core/format_*_fail.cpp, and those named in PART3 when present).
Each is compiled with the test flags and -fsyntax-only on GCC and Clang,
and the script records the line count, the number of lines that hold
"error:", the length of the longest line, and the first error line. Every
compile goes through tools/timed with the selection
diagnostics:<label>:<compiler>:<source>.

Usage, from the worktree root:
  diagnostics.py <label> <out-file> [<include dir>]

<label> names the revision measured, such as before or after; the include
directory defaults to the worktree's own.
"""

import pathlib
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
HERE = pathlib.Path(__file__).resolve().parent
FLAGS = ["-DGALOOT_CHECKING=1", "-Wall", "-Wextra", "-Wpedantic",
         "-Wconversion", "-Wshadow", "-Werror", "-ffp-contract=off",
         "-D_GLIBCXX_ASSERTIONS", "-O0", "-std=c++23", "-fsyntax-only"]
# Part 3's compile-fail sources, by glob under test/core.
PART3 = ("mv_source_*_fail.cpp", "mv_other_*_fail.cpp",
         "linear_*number*_fail.cpp", "linear_*scalar_sum*_fail.cpp",
         "blade_set_blades_not_metric_like_fail.cpp",
         "blade_set_names_zero_fail.cpp")


def measure(label, compiler, source, include):
    result = subprocess.run(
        [str(ROOT / "tools/timed"), "diagnostics", "-",
         f"diagnostics:{label}:{compiler}:{source.name}", "--", compiler,
         *FLAGS, f"-I{include}", f"-I{ROOT / 'test'}", str(source)],
        cwd=ROOT, capture_output=True, text=True)
    lines = [line.replace(f"{ROOT}/", "").replace(f"{include}/", "")
             for line in result.stderr.splitlines()
             if not line.startswith("timed:")]
    errors = [line for line in lines if "error:" in line]
    return (len(lines), len(errors),
            max((len(line) for line in lines), default=0),
            errors[0] if errors else "")


def main():
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    label = sys.argv[1]
    out = pathlib.Path(sys.argv[2])
    include = pathlib.Path(sys.argv[3] if len(sys.argv) == 4 else
                           ROOT / "include").resolve()
    core = ROOT / "test/core"
    sources = sorted(HERE.glob("gaps/*_fail.cpp"))
    sources += sorted(core.glob("format_*_fail.cpp"))
    for pattern in PART3:
        sources += sorted(core.glob(pattern))
    rows = ["label\tsource\tcompiler\tlines\terror_lines\tlongest\t"
            "first_error"]
    for source in sources:
        for compiler in ("g++", "clang++"):
            count, errors, longest, first = measure(label, compiler, source,
                                                    include)
            rows.append(f"{label}\t{source.name}\t{compiler}\t{count}\t"
                        f"{errors}\t{longest}\t{first[:300]}")
    out.write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
