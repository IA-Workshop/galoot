#!/usr/bin/env -S python3 -I
"""Measure where the compilers' default limits on constant evaluation stop
typed product plans, and how the cost of a fixed use grows with the number
of generators before and after the plans were typed.

Usage, from the repository root, with the job pool exported:
  measure_results.py limits <out-dir> [cpu]
  measure_results.py growth <before-revision> <out-dir> [repeats] [cpu]

Every compile runs as `taskset -c <cpu> tools/timed measure ...` (default
CPU 10, a performance core), so its row in runs.tsv is in pinned mode, with
the instructions retired on that core, the wall time, the peak memory, and
the busy time of the sibling thread and of other processes on the CPU.

limits  product_plan of both operations, compiled with the default limits in
        three configurations: Galoot's test flags at -O0 with checking and at
        -O2 without, and a bare -std=c++23 -pedantic-errors -O2 (no
        _GLIBCXX_ASSERTIONS, since libstdc++ defines it only without
        optimization). The units: the typed rigid(3) pairs, the conformal(3)
        vector pair, the sparse pairs at n = 8, 12, and 16, and full<n> times
        full<n> for R(n,0,0), conformal(n - 2), R(n-1,0,1), and a dense form
        with 2 on the diagonal and 1 elsewhere, from n = 2 up to the first n
        that fails. Writes <out-dir>/limits.tsv with each exit status and the
        first diagnostic line that names a limit.
growth  The fixed use of the four typed rigid(3) pairs (point x point,
        line x plane, motor x point, point x motor) embedded in R(n-1,0,1),
        e4 becoming e_n, at n = 4, 8, 12, and 16, each with a baseline unit
        that has the same algebra and sets and no plan. "before" is the
        headers of <before-revision>, extracted with git archive, read
        through gp_plan; "after" is the checkout's include/, which must have
        no uncommitted changes, read through product_plan of the geometric
        product. No unit asserts metric_like itself, so its one evaluation
        per unit falls in the plans, as in a program that only uses the
        algebra. Galoot's test flags at -O0, -c, each unit reading every term
        of each plan at run time so that its table is emitted. The compiles
        are interleaved repeat by repeat, so that drift on the machine
        affects both header sets alike. Writes <out-dir>/growth.tsv and
        <out-dir>/summary.txt.
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
GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare-O2": ["-std=c++23", "-pedantic-errors", "-O2"],
}
LIMIT_TEXT = ("step limit", "operation count exceeds", "constexpr-ops-limit",
              "constexpr-loop-limit", "constexpr-depth", "not a constant",
              "constant expression")
OPERATIONS = ("gp", "wedge")
SIZES = (4, 8, 12, 16)
KEYS = ("exit", "inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
        "pinned_other_s")

RIGID_NAMES = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}
TYPED_PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
               ("point", "motor")]


def preamble():
    return ("#include <cstddef>\n\n"
            "#include <galoot/core/blade_set.hpp>\n"
            "#include <galoot/core/metric.hpp>\n"
            "#include <galoot/core/plan.hpp>\n\n"
            "namespace core = galoot::core;\n")


def terms(tree, op, alg, a, b):
    """The expression of a plan's array of terms in the headers of `tree`."""
    if tree == "before":
        assert op == "gp"
        return f"core::detail::gp_plan<{alg}, {a}, {b}>"
    return (f"core::detail::product_plan<core::detail::BinaryOperation::{op}, "
            f"{alg}, {a}, {b}>.terms")


def oriented(name, n):
    """The mask and sign of a rigid(3) name with e4 read as e_n."""
    if name == "1":
        return 0, 1
    indices = [n if c == "4" else int(c) for c in name[1:]]
    inversions = sum(1 for p in range(len(indices))
                     for q in range(p + 1, len(indices))
                     if indices[p] > indices[q])
    mask = sum(1 << (i - 1) for i in indices)
    return mask, -1 if inversions % 2 else 1


def set_literal(names, n):
    blades = [oriented(name, n) for name in names.split()]
    inner = ", ".join(f"{{{mask}U, {sign}}}" for mask, sign in blades)
    return f"core::blade_set<{len(blades)}>{{{{{{{inner}}}}}}}"


def algebra(value):
    return f"struct alg {{ static constexpr auto value = {value}; }};\n"


def rigid_sets(n):
    text = algebra(f"core::R<{n - 1}, 0, 1>()")
    for name, names in RIGID_NAMES.items():
        text += (f"[[maybe_unused]] constexpr auto {name} = "
                 f"{set_literal(names, n)};\n")
    return text


def uses_unit(tree, op, n, pairs):
    """A unit of rigid(3) sets in R(n-1,0,1) that reads every term of each
    plan of `pairs` at run time, so that its table is emitted."""
    text = preamble() + rigid_sets(n)
    uses = []
    for a, b in pairs:
        plan = terms(tree, op, "alg", a, b)
        uses.append(f"{plan}[static_cast<std::size_t>(argc) % {plan}.size()]"
                    ".coefficient")
    body = " + ".join(uses) if uses else "0"
    return (text + "int main([[maybe_unused]] int argc, char**) {\n"
            f"  return static_cast<int>({body});\n}}\n")


def form_literal(form, n):
    if form == "r":
        return f"core::R<{n}, 0, 0>()"
    if form == "conformal":
        return f"core::conformal<{n - 2}>()"
    if form == "rigid":
        return f"core::R<{n - 1}, 0, 1>()"
    rows = ", ".join("{" + ", ".join("2" if i == j else "1" for j in range(n))
                     + "}" for i in range(n))
    return f"core::from_gram({{{rows}}})"


def full_unit(op, form, n):
    full = f"core::full<{n}>"
    return (preamble() + algebra(form_literal(form, n)) +
            f"static_assert({terms('after', op, 'alg', full, full)}.size() "
            "> 0);\n")


def sparse_unit(op, n):
    en = 1 << (n - 1)
    first = (f"core::blade_set<4>{{{{{{{{0U, 1}}, {{1U, 1}}, {{{en}U, 1}}, "
             f"{{{en | 1}U, 1}}}}}}}}")
    second = (f"core::blade_set<3>{{{{{{{{1U, 1}}, {{2U, 1}}, "
              f"{{{en}U, 1}}}}}}}}")
    count = 10 if op == "gp" else 8
    return (preamble() + algebra(f"core::R<{n - 1}, 0, 1>()") +
            f"static_assert({terms('after', op, 'alg', first, second)}.size() "
            f"== {count});\n")


def vectors_unit(op):
    vectors = "core::of_grade<5, 1>"
    count = 25 if op == "gp" else 20
    return (preamble() + algebra("core::conformal<3>()") +
            f"static_assert({terms('after', op, 'alg', vectors, vectors)}"
            f".size() == {count});\n")


def typed_unit(op):
    pairs = TYPED_PAIRS + [("line", "line")]
    if op == "gp":
        return uses_unit("after", op, 4, pairs)
    # A line wedge a plane has no terms, so it is asserted empty rather than
    # read; the point wedge a line takes its place among the uses.
    pairs = [p for p in pairs if p != ("line", "plane")] + [("point", "line")]
    empty = terms("after", op, "alg", "line", "plane")
    return (uses_unit("after", op, 4, pairs) +
            f"static_assert({empty}.empty());\n")


def read_last_row():
    with open(ROOT / "docs/agent/records/performance/runs.tsv",
              encoding="utf-8") as f:
        lines = f.read().splitlines()
    header = lines[0].split("\t")
    return dict(zip(header, lines[-1].split("\t")))


def compile_pinned(selection, compiler, flags, source, include, cpu):
    obj = source.with_suffix(f".{compiler}.o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", COMPILERS[compiler], *flags,
               f"-I{include}", "-c", "-o", str(obj), str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    row = read_last_row()
    if row.get("selection") != selection:
        sys.exit(f"the last run row is not {selection}")
    if row.get("mode") != "pinned":
        sys.exit(f"{selection} was not pinned: mode {row.get('mode')}")
    obj.unlink(missing_ok=True)
    limit = next((line.strip().replace(f"{ROOT}/", "")
                  for line in result.stderr.splitlines()
                  if any(t in line for t in LIMIT_TEXT)), "-")
    found = {key: row.get(key, "unavailable") for key in KEYS[1:]}
    return {"exit": result.returncode, **found, "limit": limit[:200],
            "stderr": result.stderr}


def write_rows(path, header, rows):
    path.write_text("\t".join(header) + "\n" + "".join(
        "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")


def limits(out, cpu):
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    rows = []

    def measure(compiler, config, op, unit, text):
        source = units / f"limits-{op}-{unit}.cpp"
        source.write_text(text, encoding="utf-8")
        r = compile_pinned(f"results-limits:{compiler}:{config}:{op}:{unit}",
                           compiler, CONFIGS[config], source, ROOT / "include",
                           cpu)
        rows.append({"compiler": compiler, "config": config, "op": op,
                     "unit": unit, **r})
        print(compiler, config, op, unit, r["exit"], r["inst_p_u"],
              r["limit"], flush=True)
        return r["exit"]

    for compiler in COMPILERS:
        for config in CONFIGS:
            for op in OPERATIONS:
                fixed = {"typed": typed_unit(op),
                         "conformal3-vectors": vectors_unit(op)}
                for n in (8, 12, 16):
                    fixed[f"sparse{n}"] = sparse_unit(op, n)
                for unit, text in fixed.items():
                    measure(compiler, config, op, unit, text)
                for form in ("r", "conformal", "rigid", "dense"):
                    for n in range(3 if form == "conformal" else 2, 10):
                        if measure(compiler, config, op, f"full-{form}-n{n}",
                                   full_unit(op, form, n)) != 0:
                            break
    write_rows(out / "limits.tsv",
               ["compiler", "config", "op", "unit", *KEYS, "limit"], rows)


def extract(revision, out):
    tree = out / "before"
    if tree.exists():
        shutil.rmtree(tree)
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(tree, filter="data")
    return tree / "include"


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True,
                          text=True, check=True).stdout.strip()


def mean_of(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return statistics.mean(values), max(values) - min(values)


def summary(rows, before, after):
    lines = [f"before {before} (gp_plan), after {after} (product_plan of "
             "gp); instructions retired on the pinned core, mean of the "
             "repeats, spread (max - min) of the two units in M",
             "the four typed pairs' plans: the unit minus its baseline"]
    for compiler in COMPILERS:
        for tree in ("before", "after"):
            parts, first = [], None
            for n in SIZES:
                full, spread_full = mean_of(rows, compiler=compiler, tree=tree,
                                            n=n, plans=4)
                base, spread_base = mean_of(rows, compiler=compiler, tree=tree,
                                            n=n, plans=0)
                plans = (full - base) / 1e6
                first = plans if first is None else first
                parts.append(f"n={n} {plans:.1f} M "
                             f"({100 * (plans / first - 1):+.1f}%, spread "
                             f"{max(spread_full, spread_base) / 1e6:.1f}, "
                             f"baseline {base / 1e6:.1f} M)")
            lines.append(f"{compiler} {tree}: " + "; ".join(parts))
    for compiler in COMPILERS:
        for n in SIZES:
            b = mean_of(rows, compiler=compiler, tree="before", n=n,
                        plans=4)[0] - mean_of(rows, compiler=compiler,
                                              tree="before", n=n, plans=0)[0]
            a = mean_of(rows, compiler=compiler, tree="after", n=n,
                        plans=4)[0] - mean_of(rows, compiler=compiler,
                                              tree="after", n=n, plans=0)[0]
            lines.append(f"{compiler} n={n}: typing adds {(a - b) / 1e6:+.1f} M "
                         f"({100 * (a / b - 1):+.1f}%) to the four plans")
    return "\n".join(lines) + "\n"


def growth(before_revision, out, repeats, cpu):
    if git("status", "--porcelain", "--", "include", "test"):
        sys.exit("include/ or test/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    trees = {"before": extract(before_revision, out),
             "after": ROOT / "include"}
    before = git("rev-parse", "--short=7", before_revision)
    after = git("rev-parse", "--short=7", "HEAD")
    units = out / "units"
    units.mkdir(exist_ok=True)
    rows = []
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for plans, pairs in ((0, []), (4, TYPED_PAIRS)):
                for tree, include in trees.items():
                    source = units / f"{tree}-n{n}-plans{plans}.cpp"
                    source.write_text(uses_unit(tree, "gp", n, pairs),
                                      encoding="utf-8")
                    for compiler in COMPILERS:
                        r = compile_pinned(
                            f"results-growth:{compiler}:{tree}:n{n}:"
                            f"plans{plans}:{repeat}", compiler,
                            CONFIGS["galoot-O0"], source, include, cpu)
                        if r["exit"] != 0:
                            sys.exit(f"{source} failed:\n{r['stderr'][:4000]}")
                        rows.append({"compiler": compiler, "tree": tree,
                                     "n": n, "plans": plans, "repeat": repeat,
                                     **r})
                        print(compiler, tree, n, plans, repeat, r["inst_p_u"],
                              r["sibling_busy_s"], flush=True)
    write_rows(out / "growth.tsv",
               ["compiler", "tree", "n", "plans", "repeat", *KEYS], rows)
    (out / "summary.txt").write_text(summary(rows, before, after),
                                     encoding="utf-8")
    print((out / "summary.txt").read_text(encoding="utf-8"))
    shutil.rmtree(out / "before")


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)
    mode = argv[0]
    if mode == "limits":
        out = pathlib.Path(argv[1]).resolve()
        out.mkdir(parents=True, exist_ok=True)
        limits(out, argv[2] if len(argv) > 2 else "10")
    elif mode == "growth" and len(argv) >= 3:
        out = pathlib.Path(argv[2]).resolve()
        growth(argv[1], out, int(argv[3]) if len(argv) > 3 else 3,
               argv[4] if len(argv) > 4 else "10")
    else:
        sys.exit(__doc__)
    shutil.rmtree(out / "units", ignore_errors=True)


if __name__ == "__main__":
    main(sys.argv[1:])
