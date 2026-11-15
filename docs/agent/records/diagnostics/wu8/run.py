"""Record the diagnostics of the three likeliest mistakes with WU8a's mv.hpp,
and compare them with the api-design spike's texts for the same cases.

Compiles program.cpp once per (compiler, spelling, case), serially, with the
flags of the spike (the project's warnings, -Werror, -fsyntax-only, no
color) from this directory, so that paths in the texts are relative, as in
the spike's texts. Each text goes to out/<compiler>-<spelling>-case<N>.txt.
Then out/summary.tsv gets one row per text, for the spike's spelling M in
forms R and P ("before") and for this program's spellings I and L
("after"): its line count, its number of error lines, the line of its first
error, and the length of its longest line in characters. The after texts
name the headers through the relative include path, which the spike's
fixture.hpp, beside its program, did not; longest_installed is the longest
line with that prefix removed, as it reads when the headers are installed,
and equals longest_line for the spike's texts.

Usage, from anywhere:  python3 -I run.py
"""

import re
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "out"
SPIKE = HERE.parent.parent / "spikes" / "api-design" / "out"
INCLUDE = "../../../../../include"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-fsyntax-only",
         "-fno-diagnostics-color", "-I", INCLUDE]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
SPELLINGS = "IL"
CASES = (0, 1, 5, 6, 8)
ERROR = re.compile(r"\berror:")


def compile_one(cxx, spelling, case):
    command = [cxx, *FLAGS, f"-DWU8_SPELL='{spelling}'", f"-DWU8_CASE={case}",
               "program.cpp"]
    run = subprocess.run(command, cwd=HERE, capture_output=True, text=True)
    return run.returncode, run.stderr + run.stdout


def measure(text):
    lines = text.splitlines()
    errors = [i for i, line in enumerate(lines) if ERROR.search(line)]
    longest = max((len(line) for line in lines), default=0)
    installed = max((len(line.replace(INCLUDE + "/", "")) for line in lines),
                    default=0)
    return (len(lines), len(errors), errors[0] + 1 if errors else 0, longest,
            installed)


def main():
    OUT.mkdir(exist_ok=True)
    rows = ["compiler\tsource\tcase\tstatus\tlines\terror_lines\t"
            "first_error_at\tlongest_line\tlongest_installed"]
    for compiler in COMPILERS:
        for form in "RP":
            for case in CASES[1:]:
                path = SPIKE / f"{compiler}-M-{form}-case{case}.txt"
                lines, errors, at, longest, installed = measure(
                    path.read_text())
                status = 1 if errors else 0
                rows.append(f"{compiler}\tspike-M-{form}\t{case}\t{status}\t"
                            f"{lines}\t{errors}\t{at}\t{longest}\t{installed}")
    for compiler, cxx in COMPILERS.items():
        for spelling in SPELLINGS:
            for case in CASES:
                status, text = compile_one(cxx, spelling, case)
                (OUT / f"{compiler}-{spelling}-case{case}.txt").write_text(text)
                lines, errors, at, longest, installed = measure(text)
                rows.append(f"{compiler}\twu8a-{spelling}\t{case}\t{status}\t"
                            f"{lines}\t{errors}\t{at}\t{longest}\t{installed}")
    (OUT / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
