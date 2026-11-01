#!/usr/bin/env -S python3 -I
"""Measure the compile cost of a fixed antiproduct use embedded in
R(n-1,0,1), n = 4, 8, 12, 16, through the conjugated plan of plan.hpp and
through the reduced plan of reduced_antiproduct.hpp.

Usage, from the repository root:
  measure_growth.py <wu7b-revision> <out-dir> [repeats] [cpu]

The headers are those of <wu7b-revision> (the branch that holds the
antiproduct plans), extracted with git archive. The use is the four typed
rigid(3) pairs of the WU7b growth record (point x point, line x plane,
motor x point, point x motor), e4 read as e_n, through six units: a
baseline with the sets and no plan, the geometric product, the conjugated
geometric antiproduct, the reduced one with C and gamma computed per plan,
the baseline plus the per-algebra coupling data alone, and the reduced one
with C and gamma from that data. Every unit includes the same
headers. Each unit reads every term at run time, so that its table is
emitted. Galoot's test flags at -O0, -c. Every compile runs as
`taskset -c <cpu> tools/timed measure ...` (default CPU 10), so that its
row in runs.tsv holds the instructions retired on that core. The compiles
are interleaved repeat by repeat.

Before the timing runs, one unit per compiler checks at compile time that
the reduced plan equals the conjugated plan for the four pairs at every n.
Writes <out-dir>/growth.tsv, <out-dir>/summary.txt, and
<out-dir>/agreement.txt.
"""

import io
import pathlib
import shutil
import statistics
import subprocess
import sys
import tarfile

HERE = pathlib.Path(__file__).resolve().parent
ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True, cwd=HERE).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-O0", "-DGALOOT_CHECKING=1"]
SIZES = (4, 8, 12, 16)
KEYS = ("inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
        "pinned_other_s")
RIGID = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}
PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
         ("point", "motor")]


def oriented(name, n):
    if name == "1":
        return 0, 1
    indices = [n if c == "4" else int(c) for c in name[1:]]
    inversions = sum(1 for p in range(len(indices))
                     for q in range(p + 1, len(indices))
                     if indices[p] > indices[q])
    return sum(1 << (i - 1) for i in indices), -1 if inversions % 2 else 1


def preamble(n):
    text = ("#include <cstddef>\n\n#include \"reduced_antiproduct.hpp\"\n\n"
            "namespace core = galoot::core;\n"
            "namespace detail = galoot::core::detail;\n"
            "struct alg { static constexpr auto value = "
            f"core::R<{n - 1}, 0, 1>(); }};\n")
    for name, names in RIGID.items():
        blades = [oriented(x, n) for x in names.split()]
        inner = ", ".join(f"{{{m}U, {s}}}" for m, s in blades)
        text += (f"[[maybe_unused]] constexpr core::blade_set<{len(blades)}> "
                 f"{name}{{{{{{{inner}}}}}}};\n")
    return text


def plan(use, a, b):
    if use in ("reduced", "cached"):
        return f"antiproduct_spike::{use}_plan<alg, {a}, {b}>.terms"
    op = {"gp": "gp", "antigp": "antigp"}[use]
    return (f"detail::product_plan<detail::BinaryOperation::{op}, alg, "
            f"{a}, {b}>.terms")


def unit(n, use):
    if use == "coupling":
        # The baseline plus the per-algebra coupling data alone.
        return (preamble(n) + "static_assert(antiproduct_spike::coupling<alg>"
                ".component[0] != 0);\n" + unit(n, "baseline").split(
                    preamble(n))[1])
    reads = [] if use == "baseline" else [plan(use, a, b) for a, b in PAIRS]
    reads = [f"{t}[static_cast<std::size_t>(argc) % {t}.size()].coefficient"
             for t in reads]
    body = " + ".join(reads) if reads else "0"
    return (preamble(n) + "int main([[maybe_unused]] int argc, char**) {\n"
            f"  return static_cast<int>({body});\n}}\n")


def agreement_unit():
    text = ("#include \"reduced_antiproduct.hpp\"\n\n"
            "namespace core = galoot::core;\n"
            "namespace detail = galoot::core::detail;\n")
    for n in SIZES:
        body = preamble(n).split("namespace detail = galoot::core::detail;\n")[1]
        text += f"namespace n{n} {{\n{body}"
        for a, b in PAIRS:
            for use in ("reduced", "cached"):
                text += (f"static_assert({plan(use, a, b)[:-6]} == "
                         f"{plan('antigp', a, b)[:-6]});\n")
        text += "}\n"
    return text


def row_of(selection):
    lines = (ROOT / "docs/agent/records/performance/runs.tsv").read_text(
        encoding="utf-8").splitlines()
    header = lines[0].split("\t")
    for line in reversed(lines[1:]):
        row = dict(zip(header, line.split("\t")))
        if row.get("selection") == selection:
            return row
    return {}


def compile_pinned(selection, compiler, source, include, cpu):
    obj = source.with_suffix(f".{compiler}.o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", COMPILERS[compiler], *FLAGS,
               f"-I{include}", f"-I{HERE}", "-c", "-o", str(obj), str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    obj.unlink(missing_ok=True)
    if result.returncode != 0:
        sys.exit(f"{selection} failed:\n{result.stderr[:4000]}")
    row = row_of(selection)
    if row.get("mode") != "pinned":
        sys.exit(f"{selection}: row missing or not pinned: {row}")
    return {key: row.get(key, "unavailable") for key in KEYS}


def extract(revision, out):
    tree = out / "tree"
    if tree.exists():
        shutil.rmtree(tree)
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(tree, filter="data")
    return tree / "include"


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)
    revision = subprocess.run(["git", "rev-parse", "--short=7", argv[0]],
                              cwd=ROOT, capture_output=True, text=True,
                              check=True).stdout.strip()
    out = pathlib.Path(argv[1]).resolve()
    repeats = int(argv[2]) if len(argv) > 2 else 3
    cpu = argv[3] if len(argv) > 3 else "10"
    out.mkdir(parents=True, exist_ok=True)
    include = extract(revision, out)
    units = out / "units"
    units.mkdir(exist_ok=True)

    agreement = []
    source = units / "agreement.cpp"
    source.write_text(agreement_unit(), encoding="utf-8")
    for compiler in COMPILERS:
        r = compile_pinned(f"antiproduct-agree:{compiler}", compiler, source,
                           include, cpu)
        agreement.append(f"{compiler}: reduced and cached plans == conjugated plan for "
                         f"the four pairs at n = {', '.join(map(str, SIZES))}"
                         f" (compiled, {int(r['inst_p_u']) / 1e6:.0f} M)")
    (out / "agreement.txt").write_text(
        f"headers of {revision}\n" + "\n".join(agreement) + "\n",
        encoding="utf-8")

    rows = []
    uses = ("baseline", "gp", "antigp", "reduced", "coupling", "cached")
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for use in uses:
                source = units / f"{use}-n{n}.cpp"
                source.write_text(unit(n, use), encoding="utf-8")
                for compiler in COMPILERS:
                    r = compile_pinned(
                        f"antiproduct-growth:{compiler}:{use}:n{n}:{repeat}",
                        compiler, source, include, cpu)
                    rows.append({"compiler": compiler, "use": use, "n": n,
                                 "repeat": repeat, **r})
                    print(compiler, use, n, repeat, r["inst_p_u"],
                          r["sibling_busy_s"], flush=True)
    header = ["compiler", "use", "n", "repeat", *KEYS]
    (out / "growth.tsv").write_text(
        "\t".join(header) + "\n" + "".join(
            "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")

    def mean(compiler, use, n):
        values = [int(r["inst_p_u"]) for r in rows if r["compiler"] == compiler
                  and r["use"] == use and r["n"] == n]
        return statistics.mean(values), max(values) - min(values)

    lines = [f"headers of {revision}; instructions retired on the pinned core,"
             " mean of the repeats, in M; each use is its unit minus the "
             "baseline at the same n"]
    for compiler in COMPILERS:
        for use in uses[1:]:
            parts, first = [], None
            for n in SIZES:
                full, spread = mean(compiler, use, n)
                base, base_spread = mean(compiler, "baseline", n)
                cost = (full - base) / 1e6
                first = cost if first is None else first
                parts.append(f"n={n} {cost:.1f} M ({100 * (cost / first - 1):+.1f}%,"
                             f" spread {max(spread, base_spread) / 1e6:.1f}, "
                             f"baseline {base / 1e6:.1f} M)")
            lines.append(f"{compiler} {use}: " + "; ".join(parts))
    text = "\n".join(lines) + "\n"
    (out / "summary.txt").write_text(text, encoding="utf-8")
    print(text, end="")
    shutil.rmtree(out / "tree")
    shutil.rmtree(units)


if __name__ == "__main__":
    main(sys.argv[1:])
