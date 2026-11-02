#!/usr/bin/env -S python3 -I
"""Measure what linear.hpp, part, and the reworded scalar checks cost to
compile, before and after.

Usage, from the repository root:
  measure.py <before-revision> <out-dir> [repeats] [cpu]

The "before" sources (include/ and test/) are extracted with git archive
from <before-revision> into <out-dir>/before; the "after" sources are the
checkout's, which must have no uncommitted changes under include/ or test/.
Every compile runs as `taskset -c <cpu> tools/timed measure ...`, so that its
row in runs.tsv is in pinned mode, with the instructions retired on that
core, the wall time, the peak memory, and the busy time of the sibling
thread and of other processes on the same CPU. Compiles are interleaved,
repeat by repeat and unit by unit, so that drift on the machine affects
every unit alike. Flags are those of the gcc-debug and clang-debug test
builds, with -c.

Units (W10 and P2): a unit that includes only galoot/core.hpp, and
test/core/smoke_test.cpp, which includes core.hpp and doctest; the test
programs of the components that include mv.hpp, whose header changed
(mv_test.cpp, product_test.cpp, unary_test.cpp, identities_test.cpp), each
with its own revision's headers and tests; and linear_test.cpp, after only.

Writes <out-dir>/rows.tsv (every compile) and <out-dir>/summary.txt (the mean
and spread of each unit's instructions, wall time, and peak memory).
"""

import io
import pathlib
import shutil
import statistics
import subprocess
import sys
import tarfile

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
FLAGS = ["-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS", "-DGALOOT_CHECKING=1",
         "-DGALOOT_EXPECTED_CHECKING=1", "-Wall", "-Wextra", "-Wpedantic",
         "-Wconversion", "-Wshadow", "-Werror", "-ffp-contract=off",
         "-D_GLIBCXX_ASSERTIONS", "-O0", "-g", "-std=c++23"]
BOTH = ["smoke", "mv", "product", "unary", "identities"]


def extract(revision, out):
    tree = out / "before"
    if tree.exists():
        shutil.rmtree(tree)
    archive = subprocess.run(["git", "archive", revision, "include", "test"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(tree, filter="data")
    return tree


def read_last_row():
    with open(ROOT / "docs/agent/records/performance/runs.tsv",
              encoding="utf-8") as f:
        lines = f.read().splitlines()
    header = lines[0].split("\t")
    return dict(zip(header, lines[-1].split("\t")))


def compile_pinned(selection, compiler, source, tree, cpu, objects):
    obj = objects / (selection.replace(":", "_").replace("/", "_") + ".o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", COMPILERS[compiler], *FLAGS,
               f"-I{tree / 'include'}", f"-I{tree / 'test'}", "-isystem",
               str(tree / "include" / "ext"), "-c", "-o", str(obj),
               str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f"{selection} failed:\n{result.stderr[:4000]}")
    row = read_last_row()
    if row.get("selection") != selection or row.get("mode") != "pinned":
        sys.exit(f"the last run row is not a pinned row of {selection}")
    return {"selection": selection, "inst": int(row["inst_p_u"]),
            "wall": float(row["wall_s"]), "rss": int(row["maxrss_kib"]),
            "sibling": row["sibling_busy_s"], "other": row["pinned_other_s"]}


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    before_revision = sys.argv[1]
    out = pathlib.Path(sys.argv[2]).resolve()
    repeats = int(sys.argv[3]) if len(sys.argv) > 3 else 3
    cpu = sys.argv[4] if len(sys.argv) > 4 else "4"
    dirty = subprocess.run(["git", "status", "--porcelain", "include", "test"],
                           cwd=ROOT, capture_output=True, text=True).stdout
    if dirty.strip():
        sys.exit("include/ or test/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    objects = out / "objects"
    objects.mkdir(exist_ok=True)
    before = extract(before_revision, out)
    units = out / "units"
    units.mkdir(exist_ok=True)
    (units / "core_only.cpp").write_text("#include <galoot/core.hpp>\n")

    # (name, source, tree)
    jobs = [
        ("core_only/before", units / "core_only.cpp", before),
        ("core_only/after", units / "core_only.cpp", ROOT),
    ]
    for suite in BOTH:
        jobs.append((f"{suite}/before",
                     before / f"test/core/{suite}_test.cpp", before))
        jobs.append((f"{suite}/after", ROOT / f"test/core/{suite}_test.cpp",
                     ROOT))
    jobs.append(("linear/after", ROOT / "test/core/linear_test.cpp", ROOT))

    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for name, source, tree in jobs:
                selection = f"linear-cost:{compiler}:{name}"
                row = compile_pinned(selection, compiler, source, tree, cpu,
                                     objects)
                row.update(compiler=compiler, name=name, repeat=repeat)
                rows.append(row)
                print(f"{selection}\t{row['inst']}\t{row['wall']}", flush=True)

    with open(out / "rows.tsv", "w", encoding="utf-8") as f:
        keys = ["compiler", "name", "repeat", "inst", "wall", "rss", "sibling",
                "other"]
        f.write("\t".join(keys) + "\n")
        for row in rows:
            f.write("\t".join(str(row[k]) for k in keys) + "\n")

    def stats(compiler, name):
        sel = [r for r in rows if r["compiler"] == compiler
               and r["name"] == name]
        inst = [r["inst"] / 1e6 for r in sel]
        return (statistics.mean(inst), max(inst) - min(inst),
                statistics.mean(r["wall"] for r in sel),
                max(r["rss"] for r in sel))

    head = subprocess.run(["git", "rev-parse", "--short=12", "HEAD"], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip()
    lines = [f"after: {head}; before: {before_revision}; {repeats} repeats "
             f"pinned to CPU {cpu}; instructions retired on that core, mean "
             "and spread in M; wall time mean in s; peak memory max in KiB"]
    for compiler in COMPILERS:
        for name, _, _ in jobs:
            mean, spread, wall, rss = stats(compiler, name)
            line = (f"{compiler} {name}: {mean:.1f} M (spread {spread:.1f}), "
                    f"{wall:.3f} s, {rss} KiB")
            if name.endswith("/after") and not name.startswith("linear"):
                base = stats(compiler, name.replace("/after", "/before"))[0]
                line += f"; change {mean - base:+.1f} M " \
                        f"({100 * (mean / base - 1):+.2f}%)"
            lines.append(line)
    (out / "summary.txt").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
