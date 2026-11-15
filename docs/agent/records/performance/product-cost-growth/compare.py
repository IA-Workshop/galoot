#!/usr/bin/env -S python3 -I
"""Compare the products, signs, and plans of the core headers in the
checkout with those of an older revision, by building and running
compare.cpp with both compilers.

Usage, from the repository root:
  compare.py <old-revision> <out-dir>

The old revision's include/galoot is extracted with git archive into
<out-dir>/old and renamed so that one program can include both copies: the
namespace galoot becomes galoot_old, the include paths galoot/ become
galoot_old/, and the macros GALOOT_ become GALOOT_OLD_. Refuses to run when
include/ has uncommitted changes, so that the result belongs to a revision.
compare.cpp is built at -O2 with checking on in both copies and run; each
compile and run goes through tools/timed, with selections compare:<compiler>
and compare-run:<compiler>. The output of each run is written to
<out-dir>/compare-<revision>-<old-revision>-<compiler>.txt. Exits 1 when a
build fails or a run reports a mismatch.
"""

import io
import pathlib
import re
import shutil
import subprocess
import sys
import tarfile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
FLAGS = ["-std=c++23", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-DGALOOT_CHECKING=1", "-DGALOOT_OLD_CHECKING=1"]


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True,
                          text=True, check=True).stdout.strip()


def extract_old(revision, out):
    old = out / "old"
    if old.exists():
        shutil.rmtree(old)
    archive = subprocess.run(["git", "archive", revision, "include/galoot"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(old, filter="data")
    source = old / "include" / "galoot"
    target = old / "include" / "galoot_old"
    source.rename(target)
    for header in target.rglob("*.hpp"):
        text = header.read_text(encoding="utf-8")
        text = re.sub(r"\bgaloot\b", "galoot_old", text)
        text = text.replace("GALOOT_", "GALOOT_OLD_")
        header.write_text(text, encoding="utf-8")
    return old / "include"


def main(argv):
    if len(argv) != 2:
        sys.exit(__doc__)
    old_revision, out = argv[0], pathlib.Path(argv[1]).resolve()
    if git("status", "--porcelain", "--", "include"):
        sys.exit("include/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    head = git("rev-parse", "--short=7", "HEAD")
    old_short = git("rev-parse", "--short=7", old_revision)
    old_include = extract_old(old_revision, out)

    failed = False
    for name, compiler in COMPILERS.items():
        program = out / f"compare-{name}"
        build = subprocess.run(
            [str(ROOT / "tools" / "timed"), "compare", "-",
             f"compare:{name}:{head}-{old_short}", "--", compiler, *FLAGS,
             f"-I{ROOT / 'include'}", f"-I{old_include}", "-o", str(program),
             str(HERE / "compare.cpp")],
            cwd=ROOT, capture_output=True, text=True)
        if build.returncode != 0:
            print(build.stderr[:4000], file=sys.stderr)
            failed = True
            continue
        run = subprocess.run(
            [str(ROOT / "tools" / "timed"), "compare", "-",
             f"compare-run:{name}:{head}-{old_short}", "--", str(program)],
            cwd=ROOT, capture_output=True, text=True)
        text = (f"# {compiler} {' '.join(FLAGS)}\n# HEAD {head}, old "
                f"{old_short}, exit {run.returncode}\n{run.stdout}"
                + "".join(f"# stderr: {line}\n"
                          for line in run.stderr.splitlines()))
        (out / f"compare-{head}-{old_short}-{name}.txt").write_text(
            text, encoding="utf-8")
        print(text, end="")
        failed = failed or run.returncode != 0
        program.unlink()
    shutil.rmtree(out / "old")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main(sys.argv[1:])
