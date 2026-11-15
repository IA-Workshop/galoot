"""Compile each case of first_user.cpp on GCC and Clang, under the project's
flags and under a plain user's flags, and record what a user reads.

Serially, from this directory, -fsyntax-only, so that paths in the texts are
relative. Each text goes to out/<compiler>-<flags>-case<N>.txt and one row
per compile to out/summary.tsv: exit status, lines, top-level error lines,
top-level warning lines, the line number of the first error, the first error line itself (path
prefix removed), the longest line as installed (include prefix removed), and
whether the text shows masks or library internals.

Usage, from anywhere:  python3 -I run.py [case ...]
"""

import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "out"
INCLUDE = "../../../../../include"
FLAG_SETS = {
    "project": ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
                "-Wshadow", "-Werror", "-ffp-contract=off"],
    "plain": ["-std=c++23", "-Wall"],
}
COMMON = ["-fsyntax-only", "-fno-diagnostics-color", "-I", INCLUDE]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
CASES = range(49)
# Top-level diagnostics only: GCC nests the errors of a constraint failure
# under a note, indented and marked with a bullet, and those are not counted.
ERROR = re.compile(r"^\S.*\berror:")
WARNING = re.compile(r"^\S.*\bwarning:")
# Text a user should not need to read: masks printed as numbers, the
# standard library's array internals, and the library's detail namespace.
MACHINERY = {
    "masks": re.compile(r"oriented_blade\{|\{\{\{\{\d+, -?\d+\}"),
    "array_traits": re.compile(r"__array_traits"),
    "detail": re.compile(r"detail::"),
}


def compile_one(cxx, flags, case, source):
    command = [cxx, *FLAG_SETS[flags], *COMMON, f"-DCASE={case}", source]
    run = subprocess.run(command, cwd=HERE, capture_output=True, text=True)
    return run.returncode, run.stderr + run.stdout


def strip_path(line):
    return re.sub(r"^\S*/", "", line.replace(INCLUDE + "/", ""))


def measure(text):
    lines = text.splitlines()
    errors = [i for i, line in enumerate(lines) if ERROR.search(line)]
    warnings = [i for i, line in enumerate(lines) if WARNING.search(line)]
    first = errors[0] if errors else (warnings[0] if warnings else None)
    first_text = strip_path(lines[first]) if first is not None else ""
    installed = max((len(line.replace(INCLUDE + "/", "")) for line in lines),
                    default=0)
    shown = ",".join(k for k, rx in MACHINERY.items() if rx.search(text))
    return (len(lines), len(errors), len(warnings),
            errors[0] + 1 if errors else 0, installed, shown or "-",
            first_text.replace("\t", " "))


def main():
    OUT.mkdir(exist_ok=True)
    cases = [int(a) for a in sys.argv[1:]] or list(CASES)
    rows = ["compiler\tflags\tcase\tstatus\tlines\terror_lines\t"
            "warning_lines\tfirst_error_at\tlongest_installed\tmachinery\t"
            "first_diagnostic"]
    for compiler, cxx in COMPILERS.items():
        for flags in FLAG_SETS:
            for case in cases:
                status, text = compile_one(cxx, flags, case, "first_user.cpp")
                (OUT / f"{compiler}-{flags}-case{case}.txt").write_text(text)
                rows.append("\t".join(str(v) for v in (
                    compiler, flags, case, status, *measure(text))))
    (OUT / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
