"""Record the diagnostics a first-time user sees for each likely WU8 mistake.

Compiles program.cpp once per (compiler, spelling, form, case), serially, with
the project's warning flags and -Werror, as -fsyntax-only from this directory
so that the paths in the texts are relative and the texts do not depend on
where the checkout lives. Each text is kept in out/<compiler>-<spelling>-
<form>-case<N>.txt, and one row per compile goes to out/summary.tsv.

Usage, from anywhere:  python3 -I run.py
"""

import re
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "out"
INCLUDE = "../../../../../include"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-fsyntax-only",
         "-fno-diagnostics-color", "-I", INCLUDE]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
SPELLINGS = "MN"
FORMS = "RPST"
CASES = range(12)

# Text that shows the user's own terms, searched for in each output.
TERMS = {
    "e14": '"e14"', "e1": '"e1"', "e5": '"e5"',
    "names": "e41 e42 e43 e23 e31 e12",
    "alias": r"\b(Point|Line|Motor|Vector4)\b",
    "algebras": r"rga3.*r400|r400.*rga3",
}
ERROR = re.compile(r"\berror:")


def compile_one(cxx, spelling, form, case):
    command = [cxx, *FLAGS, f"-DAPX_FORM='{form}'",
               f"-DAPX_SPELL='{spelling}'", f"-DAPX_CASE={case}",
               "program.cpp"]
    run = subprocess.run(command, cwd=HERE, capture_output=True, text=True)
    return run.returncode, run.stderr + run.stdout


def summarize(text):
    lines = text.splitlines()
    errors = [i for i, line in enumerate(lines) if ERROR.search(line)]
    first = lines[errors[0]] if errors else ""
    first = re.sub(r"^[^ ]*: (fatal )?error: ", "", first)
    found = [name for name, pattern in TERMS.items()
             if re.search(pattern, text)]
    return len(lines), len(errors), errors[0] + 1 if errors else 0, first, found


def main():
    OUT.mkdir(exist_ok=True)
    rows = ["compiler\tspelling\tform\tcase\tstatus\tlines\terror_lines\t"
            "first_error_at\tuser_terms\tfirst_error"]
    for compiler, cxx in COMPILERS.items():
        for spelling in SPELLINGS:
            for form in FORMS:
                for case in CASES:
                    status, text = compile_one(cxx, spelling, form, case)
                    name = f"{compiler}-{spelling}-{form}-case{case}.txt"
                    (OUT / name).write_text(text)
                    lines, errors, at, first, found = summarize(text)
                    rows.append(f"{compiler}\t{spelling}\t{form}\t{case}\t"
                                f"{status}\t{lines}\t{errors}\t{at}\t"
                                f"{','.join(found)}\t{first}")
    (OUT / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
