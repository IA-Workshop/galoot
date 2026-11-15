#!/usr/bin/env -S python3 -I
"""Measure what transform's rounding check costs to compile and to run.

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

Units: test/core/smoke_test.cpp, a unit that includes only galoot/core.hpp,
and test/core/transform_test.cpp (before and after), and
test/core/transform_rounding_test.cpp (after only, in the checking build and
again with GALOOT_CHECKING 0). Then the run cost of checking mode: cost.cpp
of records/spikes/transform-rounding, built at -O2 -ffp-contract=off with
GALOOT_CHECKING 1 and 0, each run pinned with <count> transforms.

Writes <out-dir>/rows.tsv (every compile and run) and <out-dir>/summary.txt
(the mean and spread of each unit's instructions, wall time, and peak
memory).
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


def flags(checking):
    if checking:
        return FLAGS
    return [f.replace("CHECKING=1", "CHECKING=0") for f in FLAGS]


def pinned(selection, cpu, command):
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", *command]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    row = read_last_row()
    return result, row


def compile_pinned(selection, compiler, source, tree, cpu, objects,
                   checking=True):
    obj = objects / (selection.replace(":", "_").replace("/", "_") + ".o")
    result, row = pinned(selection, cpu, [
        COMPILERS[compiler], *flags(checking), f"-I{tree / 'include'}",
        f"-I{tree / 'test'}", "-isystem", str(tree / "include" / "ext"), "-c",
        "-o", str(obj), str(source)])
    if result.returncode != 0:
        sys.exit(f"{selection} failed:\n{result.stderr[:4000]}")
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
    count = sys.argv[5] if len(sys.argv) > 5 else "10000000"
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

    rounding = ROOT / "test/core/transform_rounding_test.cpp"
    # (name, source, tree, checking)
    jobs = [
        ("suites/smoke/before", before / "test/core/smoke_test.cpp", before,
         True),
        ("suites/smoke/after", ROOT / "test/core/smoke_test.cpp", ROOT, True),
        ("suites/core_only/before", units / "core_only.cpp", before, True),
        ("suites/core_only/after", units / "core_only.cpp", ROOT, True),
        ("suites/transform/before", before / "test/core/transform_test.cpp",
         before, True),
        ("suites/transform/after", ROOT / "test/core/transform_test.cpp", ROOT,
         True),
        ("suites/transform_rounding/after", rounding, ROOT, True),
        ("suites/transform_rounding/after_unchecked", rounding, ROOT, False),
    ]

    # The run cost: cost.cpp in each checking mode, built once (not measured)
    # and run pinned in every repeat.
    cost_source = ROOT / "docs/agent/records/spikes/transform-rounding/cost.cpp"
    programs = {}
    for compiler in COMPILERS:
        for mode in ("1", "0"):
            program = objects / f"cost_{compiler}_{mode}"
            subprocess.run(
                [str(ROOT / "tools" / "timed"), "build", "-",
                 f"transform-rounding-cost:build:{compiler}:{mode}", "--",
                 COMPILERS[compiler], "-std=c++23", "-O2", "-ffp-contract=off",
                 f"-DGALOOT_CHECKING={mode}", f"-I{ROOT / 'include'}",
                 "-o", str(program), str(cost_source)], cwd=ROOT, check=True)
            programs[(compiler, mode)] = program

    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for name, source, tree, checking in jobs:
                selection = f"transform-rounding-cost:{compiler}:{name}"
                row = compile_pinned(selection, compiler, source, tree, cpu,
                                     objects, checking)
                row.update(compiler=compiler, name=name, repeat=repeat)
                rows.append(row)
                print(f"{selection}\t{row['inst']}\t{row['wall']}", flush=True)
            for mode in ("1", "0"):
                name = f"run/cost/checking_{mode}"
                selection = f"transform-rounding-cost:{compiler}:{name}"
                result, row = pinned(selection, cpu, [
                    str(programs[(compiler, mode)]), count, "1"])
                if result.returncode != 0 or row.get("selection") != selection:
                    sys.exit(f"{selection} failed")
                rows.append({"selection": selection,
                             "inst": int(row["inst_p_u"]),
                             "wall": float(row["wall_s"]),
                             "rss": int(row["maxrss_kib"]),
                             "sibling": row["sibling_busy_s"],
                             "other": row["pinned_other_s"],
                             "compiler": compiler, "name": name,
                             "repeat": repeat})
                print(f"{selection}\t{row['inst_p_u']}\t{row['wall_s']}",
                      flush=True)

    with open(out / "rows.tsv", "w", encoding="utf-8") as f:
        f.write("compiler\tname\trepeat\tinst\twall_s\tmaxrss_kib\t"
                "sibling_busy_s\tpinned_other_s\n")
        for r in rows:
            f.write(f"{r['compiler']}\t{r['name']}\t{r['repeat']}\t"
                    f"{r['inst']}\t{r['wall']}\t{r['rss']}\t{r['sibling']}\t"
                    f"{r['other']}\n")
    lines = []
    names = [job[0] for job in jobs] + ["run/cost/checking_1",
                                        "run/cost/checking_0"]
    for compiler in COMPILERS:
        for name in names:
            mine = [r for r in rows if r["compiler"] == compiler and
                    r["name"] == name]
            inst = [r["inst"] / 1e6 for r in mine]
            wall = [r["wall"] for r in mine]
            rss = [r["rss"] / 1024 for r in mine]
            lines.append(
                f"{compiler}\t{name}\tinst {statistics.mean(inst):,.1f} M "
                f"(range {max(inst) - min(inst):,.1f} M)\twall "
                f"{statistics.mean(wall):.2f} s\trss {max(rss):,.0f} MiB")
    (out / "summary.txt").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
