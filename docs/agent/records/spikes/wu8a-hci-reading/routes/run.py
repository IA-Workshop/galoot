"""Compile routes/program.cpp under each route of mv_routes.hpp, on GCC and
Clang with the project's flags, and record what changes.

Serially, from this directory, -fsyntax-only. Each text goes to
out/<compiler>-<route>-case<N>.txt and one row per compile to
out/summary.tsv: exit status, lines, top-level error lines, the longest line
as installed, whether masks or internals show, whether the user's type or
layout names show, and the first error line.

Usage, from anywhere:  python3 -I run.py
"""

import re
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "out"
INCLUDE = "../../../../../../include"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-fsyntax-only",
         "-fno-diagnostics-color", "-I", INCLUDE]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
ROUTES = {
    "none": [],
    "count_base": ["-DROUTE_COUNT_BASE=1"],
    "type_pack": ["-DROUTE_TYPE_PACK=1"],
    "named_checks": ["-DROUTE_NAMED_CHECKS=1"],
    "named_class": ["-DROUTE_NAMED_CLASS=1", "-DNAMED_CLASS=1"],
    "all": ["-DROUTE_COUNT_BASE=1", "-DROUTE_TYPE_PACK=1",
            "-DROUTE_NAMED_CHECKS=1", "-DROUTE_NAMED_CLASS=1",
            "-DNAMED_CLASS=1"],
}
CASES = range(8)
TOP_ERROR = re.compile(r"^\S.*\berror:")
MACHINERY = re.compile(r"oriented_blade\{|\{\{\{\{\d+, -?\d+\}|__array_traits"
                       r"|detail::")
USER_NAMES = re.compile(r"\b(Point|Line|Motor|line_layout|point_layout|"
                        r"motor_layout)\b")


def strip_path(line):
    return re.sub(r"^\S*/", "", line.replace(INCLUDE + "/", ""))


def main():
    OUT.mkdir(exist_ok=True)
    rows = ["compiler\troute\tcase\tstatus\tlines\terrors\tlongest_installed"
            "\tmachinery\tuser_names\tfirst_error"]
    for compiler, cxx in COMPILERS.items():
        for route, defines in ROUTES.items():
            for case in CASES:
                command = [cxx, *FLAGS, *defines, f"-DCASE={case}",
                           "program.cpp"]
                run = subprocess.run(command, cwd=HERE, capture_output=True,
                                     text=True)
                text = run.stderr + run.stdout
                (OUT / f"{compiler}-{route}-case{case}.txt").write_text(text)
                lines = text.splitlines()
                errors = [ln for ln in lines if TOP_ERROR.search(ln)]
                longest = max((len(ln.replace(INCLUDE + "/", ""))
                               for ln in lines), default=0)
                rows.append("\t".join(str(v) for v in (
                    compiler, route, case, run.returncode, len(lines),
                    len(errors), longest,
                    "yes" if MACHINERY.search(text) else "no",
                    "yes" if USER_NAMES.search(text) else "no",
                    strip_path(errors[0])[:160] if errors else "")))
    (OUT / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
