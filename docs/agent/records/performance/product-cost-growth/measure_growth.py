#!/usr/bin/env -S python3 -I
"""Measure, before and after a change to the core headers, what a fixed use
costs to compile as the number of generators grows, and what the headers
cost the units that include them.

Usage, from the repository root:
  measure_growth.py <before-revision> <out-dir> [repeats] [cpu]

The headers of <before-revision> are extracted with git archive into
<out-dir>/before; the "after" headers are the checkout's include/, which
must have no uncommitted changes. Every compile runs as
`taskset -c <cpu> tools/timed measure ...`, so that its row in runs.tsv is
in pinned mode, with the instructions retired on that core, the wall time,
the peak memory, and the busy time of the sibling thread and of other
processes on the same CPU. The compiles are interleaved, repeat by repeat
and unit by unit, so that drift on the machine affects both header sets
alike.

growth  The four typed rigid(3) pairs (point x point, line x plane,
        motor x point, point x motor) embedded in R(n-1,0,1), e4 becoming
        e_n, at n = 4, 8, 12, and 16, each with a baseline unit that has the
        same algebra and sets and no plan; the units of measure_plan.py, with
        Galoot's test flags at -O0 and -c.
header  Units that include bits.hpp, blade_product.hpp, or core.hpp and use
        nothing, and test/core/bits_test.cpp and blade_product_test.cpp,
        with the same flags.

Writes <out-dir>/growth.tsv, <out-dir>/header.tsv, and <out-dir>/summary.txt.
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
sys.path.insert(0, str(ROOT / "docs/agent/records/performance/plan-growth"))
import measure_plan as mp

FLAGS = mp.CONFIGS["galoot-O0"]
SIZES = (4, 8, 12, 16)
HEADER_UNITS = {
    "bits.hpp": "#include <galoot/core/bits.hpp>\n",
    "blade_product.hpp": "#include <galoot/core/blade_product.hpp>\n",
    "core.hpp": "#include <galoot/core.hpp>\n",
}
TEST_UNITS = ("bits_test", "blade_product_test")


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True,
                          text=True, check=True).stdout.strip()


def extract(revision, out):
    tree = out / "before"
    if tree.exists():
        shutil.rmtree(tree)
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(tree, filter="data")
    return tree / "include"


def compile_pinned(selection, compiler, source, include, cpu, extra=()):
    obj = source.with_suffix(f".{compiler}.o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", mp.COMPILERS[compiler], *FLAGS,
               f"-I{include}", *extra, "-c", "-o", str(obj), str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f"{selection} failed:\n{result.stderr[:4000]}")
    row = mp.read_last_row()
    if row.get("selection") != selection:
        sys.exit(f"the last run row is not {selection}")
    if row.get("mode") != "pinned":
        sys.exit(f"{selection} was not pinned: mode {row.get('mode')}")
    obj.unlink()
    return {key: row.get(key, "unavailable") for key in (
        "inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
        "pinned_other_s")}


def write_rows(path, header, rows):
    path.write_text("\t".join(header) + "\n" + "".join(
        "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")


def mean_of(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return statistics.mean(values), max(values) - min(values), len(values)


def summary(growth, header, before, after):
    lines = [f"before {before}, after {after}; instructions retired on the "
             "pinned core, mean of the repeats, with the spread (max - min)"]
    lines.append("\nFixed use as n grows: the four typed pairs' plans, the "
                 "unit minus its baseline")
    for compiler in mp.COMPILERS:
        for tree in ("before", "after"):
            parts = []
            first = None
            for n in SIZES:
                full, spread_full, _ = mean_of(growth, compiler=compiler,
                                               tree=tree, n=n, plans=4)
                base, spread_base, _ = mean_of(growth, compiler=compiler,
                                               tree=tree, n=n, plans=0)
                plans = (full - base) / 1e9
                first = plans if first is None else first
                parts.append(f"n={n} {plans:.3f} G ({100 * (plans / first - 1):+.1f}%, "
                             f"spread {max(spread_full, spread_base) / 1e6:.1f} M, "
                             f"baseline {base / 1e9:.3f} G)")
            lines.append(f"{compiler} {tree}: " + "; ".join(parts))
    lines.append("\nHeader and test units")
    for compiler in mp.COMPILERS:
        for unit in [*HEADER_UNITS, *TEST_UNITS]:
            b, sb, _ = mean_of(header, compiler=compiler, tree="before",
                               unit=unit)
            a, sa, _ = mean_of(header, compiler=compiler, tree="after",
                               unit=unit)
            lines.append(f"{compiler} {unit}: before {b / 1e9:.4f} G "
                         f"(spread {sb / 1e6:.1f} M), after {a / 1e9:.4f} G "
                         f"(spread {sa / 1e6:.1f} M), change "
                         f"{(a - b) / 1e6:+.1f} M ({100 * (a / b - 1):+.2f}%)")
    return "\n".join(lines) + "\n"


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)
    before_revision, out = argv[0], pathlib.Path(argv[1]).resolve()
    repeats = int(argv[2]) if len(argv) > 2 else 3
    cpu = argv[3] if len(argv) > 3 else "6"
    if git("status", "--porcelain", "--", "include", "test"):
        sys.exit("include/ or test/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    trees = {"before": extract(before_revision, out),
             "after": ROOT / "include"}
    before = git("rev-parse", "--short=7", before_revision)
    after = git("rev-parse", "--short=7", "HEAD")
    units = out / "units"
    units.mkdir(exist_ok=True)
    test_flags = [f"-I{ROOT / 'include' / 'ext'}", f"-I{ROOT / 'test'}"]

    growth, header = [], []
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for plans, pairs in ((0, []), (4, mp.TYPED_PAIRS)):
                source = units / f"n{n}-plans{plans}.cpp"
                source.write_text(mp.plans_unit(n, pairs), encoding="utf-8")
                for compiler in mp.COMPILERS:
                    for tree, include in trees.items():
                        r = compile_pinned(
                            f"growth-0012:{compiler}:{tree}:n{n}:"
                            f"plans{plans}:{repeat}", compiler, source,
                            include, cpu)
                        growth.append({"compiler": compiler, "tree": tree,
                                       "n": n, "plans": plans,
                                       "repeat": repeat, **r})
                        print(growth[-1], flush=True)
        for unit in [*HEADER_UNITS, *TEST_UNITS]:
            if unit in HEADER_UNITS:
                source = units / f"{unit.replace('.', '_')}.cpp"
                source.write_text(HEADER_UNITS[unit], encoding="utf-8")
                extra = ()
            else:
                source = ROOT / "test" / "core" / f"{unit}.cpp"
                extra = test_flags
            for compiler in mp.COMPILERS:
                for tree, include in trees.items():
                    if unit in TEST_UNITS:
                        copy = units / f"{unit}-{tree}.cpp"
                        shutil.copy(source, copy)
                        shutil.copy(source.parent / "signatures.hpp",
                                    units / "signatures.hpp")
                        compiled = copy
                    else:
                        compiled = source
                    r = compile_pinned(
                        f"header-0012:{compiler}:{tree}:{unit}:{repeat}",
                        compiler, compiled, include, cpu, extra)
                    header.append({"compiler": compiler, "tree": tree,
                                   "unit": unit, "repeat": repeat, **r})
                    print(header[-1], flush=True)

    keys = ["inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
            "pinned_other_s"]
    write_rows(out / "growth.tsv",
               ["compiler", "tree", "n", "plans", "repeat", *keys], growth)
    write_rows(out / "header.tsv",
               ["compiler", "tree", "unit", "repeat", *keys], header)
    (out / "summary.txt").write_text(summary(growth, header, before, after),
                                     encoding="utf-8")
    print((out / "summary.txt").read_text(encoding="utf-8"))
    shutil.rmtree(units)
    shutil.rmtree(out / "before")


if __name__ == "__main__":
    main(sys.argv[1:])
