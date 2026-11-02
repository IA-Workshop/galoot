"""Record the diagnostics of the mistakes that the reviews of WU8a left for
WU8b, before and after WU8b, on GCC and Clang.

"Before" compiles program.cpp against the headers of a revision given as
the first argument (develop's merge of WU8a), extracted with git archive
into a temporary directory; "after" compiles it against the headers of this
worktree. Each text goes to out/<compiler>-<before|after>-case<N>.txt, and
out/summary.tsv gets one row per text: its exit status, its line count, its
number of error lines, the line of its first error, its longest line in
characters, and that longest line with the include path removed
("installed"), as a user who installs the headers reads it. The flags are
those of the WU8a record beside this directory. The paths in the texts are
relative to this directory for "after" and to the temporary tree for
"before"; the installed length removes both.

Usage, from the worktree root:
  python3 -I docs/agent/records/diagnostics/wu8/wu8b/run.py <before-revision>
"""

import re
import subprocess
import sys
import tarfile
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
OUT = HERE / "out"
AFTER = "../../../../../../include"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-fsyntax-only",
         "-fno-diagnostics-color"]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
CASES = (0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22)
ERROR = re.compile(r"\berror:")


def compile_one(cxx, include, case):
    command = [cxx, *FLAGS, "-I", include, f"-DWU8B_CASE={case}",
               "program.cpp"]
    run = subprocess.run(command, cwd=HERE, capture_output=True, text=True)
    return run.returncode, run.stderr + run.stdout


def measure(text, include):
    lines = text.splitlines()
    errors = [i for i, line in enumerate(lines) if ERROR.search(line)]
    longest = max((len(line) for line in lines), default=0)
    installed = max((len(line.replace(include + "/", "")) for line in lines),
                    default=0)
    return (len(lines), len(errors), errors[0] + 1 if errors else 0, longest,
            installed)


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    revision = sys.argv[1]
    OUT.mkdir(exist_ok=True)
    rows = ["compiler\theaders\tcase\tstatus\tlines\terror_lines\t"
            "first_error_at\tlongest_line\tlongest_installed"]
    with tempfile.TemporaryDirectory() as before:
        root = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                              cwd=HERE, capture_output=True, text=True,
                              check=True).stdout.strip()
        archive = subprocess.run(
            ["git", "archive", "--format=tar", revision, "include"],
            cwd=root, capture_output=True, check=True).stdout
        archive_path = Path(before) / "include.tar"
        archive_path.write_bytes(archive)
        with tarfile.open(archive_path) as tar:
            tar.extractall(before, filter="data")
        trees = {"before": f"{before}/include", "after": AFTER}
        for compiler, cxx in COMPILERS.items():
            for headers, include in trees.items():
                for case in CASES:
                    status, text = compile_one(cxx, include, case)
                    text = text.replace(before + "/", "<before>/")
                    shown = include.replace(before + "/", "<before>/")
                    name = f"{compiler}-{headers}-case{case}.txt"
                    (OUT / name).write_text(text)
                    lines, errors, at, longest, installed = measure(text,
                                                                    shown)
                    rows.append(f"{compiler}\t{headers}\t{case}\t{status}\t"
                                f"{lines}\t{errors}\t{at}\t{longest}\t"
                                f"{installed}")
    (OUT / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
