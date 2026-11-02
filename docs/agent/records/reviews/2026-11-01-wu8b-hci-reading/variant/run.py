import re, subprocess, sys
from pathlib import Path
here = Path(__file__).resolve().parent
settings = {
    "branch": [],
    "lossy": ["-DVARIANT_LOSSY_EXPLICIT="],
    "both": ["-DVARIANT_LOSSY_EXPLICIT=", "-DVARIANT_IMPLICIT_LOSSLESS=1"],
    "array": ["-DVARIANT_ARRAY_EXPLICIT="],
}
flags = "-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -fno-diagnostics-color".split()
out = here / "out"; out.mkdir(exist_ok=True)
err = re.compile(r"(^|: )(fatal )?error: ")
rows = ["setting\tcompiler\tcase\tstatus\tlines\terrors\tfirst_error_at\tlongest\tfirst_error\trun"]
for setting, extra in settings.items():
    for case in [0, 1, 3, 4, 5, 7, 22, 70, 71, 72, 73]:
        for cc, name in (("g++", "gcc"), ("clang++", "clang")):
            exe = out / f"{setting}-{name}-{case}"
            cmd = [cc, *flags, *extra, "-Iinclude", f"-DCASE={case}", "variant.cpp", "-o", str(exe)]
            p = subprocess.run(cmd, cwd=here, capture_output=True, text=True)
            (out / f"{setting}-{name}-case{case}.txt").write_text(p.stderr)
            lines = p.stderr.splitlines()
            errs = [i for i, l in enumerate(lines) if err.search(l) and not l.lstrip().startswith("•")]
            fe = re.sub(r"^\S+:\d+:\d+: ", "", lines[errs[0]]) if errs else ""
            run = ""
            if p.returncode == 0:
                run = subprocess.run([str(exe)], capture_output=True, text=True).stdout.strip()
                exe.unlink()
            rows.append(f"{setting}\t{name}\t{case}\t{p.returncode}\t{len(lines)}\t{len(errs)}\t{errs[0]+1 if errs else 0}\t{max((len(l) for l in lines), default=0)}\t{fe[:200]}\t{run}")
(out / "summary.tsv").write_text("\n".join(rows) + "\n")
