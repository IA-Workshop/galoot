"""Compile user.cpp for each case on both compilers and summarize."""
import re
import subprocess
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
include = Path(sys.argv[1]).resolve()
cases = [int(c) for c in sys.argv[2].split(",")]
flagsets = {
    "project": "-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off".split(),
    "plain": "-std=c++23 -Wall".split(),
}
out = here / "out"
out.mkdir(exist_ok=True)
rows = ["compiler\tflags\tcase\tstatus\tlines\terrors\tfirst_error_at\tlongest\tfirst_error"]
err = re.compile(r"(^|: )(fatal )?error: ")
for case in cases:
    for cc, name in (("g++", "gcc"), ("clang++", "clang")):
        for fs, flags in flagsets.items():
            cmd = [cc, *flags, "-fsyntax-only", "-fno-diagnostics-color",
                   "-I", "include", f"-DCASE={case}", str(here / "user.cpp")]
            p = subprocess.run(cmd, cwd=include.parent, capture_output=True, text=True)
            text = p.stderr.replace(str(here) + "/", "")
            (out / f"{name}-{fs}-case{case}.txt").write_text(text)
            lines = text.splitlines()
            errs = [i for i, l in enumerate(lines) if err.search(l) and not l.lstrip().startswith("•")]
            first = errs[0] + 1 if errs else 0
            fe = lines[errs[0]] if errs else ""
            fe = re.sub(r"^\S+:\d+:\d+: ", "", fe)
            longest = max((len(l) for l in lines), default=0)
            rows.append(f"{name}\t{fs}\t{case}\t{p.returncode}\t{len(lines)}\t{len(errs)}\t{first}\t{longest}\t{fe[:300]}")
(out / "summary.tsv").write_text("\n".join(rows) + "\n")
