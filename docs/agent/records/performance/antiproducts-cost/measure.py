#!/usr/bin/env -S python3 -I
"""Measure what the public antiproducts and the new headers cost to compile.

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

suites  W10: test/core/smoke_test.cpp and a unit that includes only
        galoot/core.hpp (before and after), test/core/product_test.cpp
        (before and after, each with its own revision's headers), and
        test/core/unary_test.cpp and identities_test.cpp (after only).
growth  The antiproduct's fixed use as n grows: the four typed rigid(3)
        pairs (point x point, line x plane, motor x point, point x motor)
        embedded in R(n-1,0,1), e4 becoming e_n, at n = 4 and 16, as mv
        values of double through the public operators % and *, each used at
        run time so that its code is emitted; and a baseline unit with the
        same algebra, layouts, and values and no product.

Writes <out-dir>/rows.tsv (every compile) and <out-dir>/summary.txt (the mean
and spread of each unit's instructions, wall time, and peak memory; growth
units also minus their baseline).
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
SIZES = (4, 16)

RIGID_NAMES = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}
PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
         ("point", "motor")]


def oriented(name, n):
    """The mask and sign of a rigid(3) name with e4 read as e_n."""
    if name == "1":
        return 0, 1
    indices = [n if c == "4" else int(c) for c in name[1:]]
    inversions = sum(1 for p in range(len(indices))
                     for q in range(p + 1, len(indices))
                     if indices[p] > indices[q])
    return sum(1 << (i - 1) for i in indices), -1 if inversions % 2 else 1


def growth_unit(n, op):
    """A unit with the four typed pairs in R(n-1,0,1); op is %, *, or None
    for the baseline, which builds the same values and no product."""
    lines = ["#include <galoot/core.hpp>", "",
             "namespace core = galoot::core;",
             f"struct alg {{ static constexpr auto value = "
             f"core::R<{n - 1}, 0, 1>(); }};"]
    for name, names in RIGID_NAMES.items():
        blades = [oriented(b, n) for b in names.split()]
        inner = ", ".join(f"{{{m}U, {s}}}" for m, s in blades)
        lines.append(f"constexpr core::blade_set<{len(blades)}> {name}_layout"
                     f"{{{{{{{inner}}}}}}};")
        lines.append(f"using {name.capitalize()} = "
                     f"core::mv<alg, {name}_layout, double>;")
    lines.append("double use(double s) {")
    for name in RIGID_NAMES:
        count = len(RIGID_NAMES[name].split())
        values = ", ".join("s" for _ in range(count))
        lines.append(f"  const {name.capitalize()} {name}{{{values}}};")
    terms = []
    for a, b in PAIRS:
        if op is None:
            terms.append(f"{a}.coefficients[0] + {b}.coefficients[0]")
        else:
            terms.append(f"({a} {op} {b}).coefficients[0]")
    lines.append("  return " + " + ".join(terms) + ";")
    lines.append("}")
    return "\n".join(lines) + "\n"


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
    for n in SIZES:
        for op, tag in (("%", "percent"), ("*", "times"), (None, "baseline")):
            (units / f"growth_{tag}_n{n}.cpp").write_text(growth_unit(n, op))

    # (name, source, tree)
    jobs = [
        ("suites/smoke/before", before / "test/core/smoke_test.cpp", before),
        ("suites/smoke/after", ROOT / "test/core/smoke_test.cpp", ROOT),
        ("suites/core_only/before", units / "core_only.cpp", before),
        ("suites/core_only/after", units / "core_only.cpp", ROOT),
        ("suites/product/before", before / "test/core/product_test.cpp",
         before),
        ("suites/product/after", ROOT / "test/core/product_test.cpp", ROOT),
        ("suites/unary/after", ROOT / "test/core/unary_test.cpp", ROOT),
        ("suites/identities/after", ROOT / "test/core/identities_test.cpp",
         ROOT),
    ]
    for n in SIZES:
        for tag in ("percent", "times", "baseline"):
            jobs.append((f"growth/{tag}/n{n}", units / f"growth_{tag}_n{n}.cpp",
                         ROOT))

    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for name, source, tree in jobs:
                selection = f"antiproducts-cost:{compiler}:{name}"
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
            if name.startswith("growth/") and "baseline" not in name:
                n = name.rsplit("/", 1)[1]
                base = stats(compiler, f"growth/baseline/{n}")[0]
                line += f"; use {mean - base:.1f} M"
            lines.append(line)
        for tag in ("percent", "times"):
            uses = [stats(compiler, f"growth/{tag}/n{n}")[0]
                    - stats(compiler, f"growth/baseline/n{n}")[0]
                    for n in SIZES]
            lines.append(f"{compiler} growth of {tag} from n = {SIZES[0]} to "
                         f"{SIZES[-1]}: {100 * (uses[-1] / uses[0] - 1):+.1f}%")
    (out / "summary.txt").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
