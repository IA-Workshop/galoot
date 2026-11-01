#!/usr/bin/env -S python3 -I
"""Test whether the part of a fixed use's cost that still grows with n
comes from the metric_like check, which reads all n^2 entries of the Gram
matrix, and whether it grows with the number of plans.

Usage, from the repository root:
  remainder_probe.py <out-dir> [repeats] [cpu] [--without-concept-assert]

Two copies of the checkout's headers: "as-is", and "type-only", in which
metric_like keeps only its first condition (that Alg::value is a metric)
and drops the three that read the Gram matrix. Units, from
measure_plan.py, in R(n-1,0,1) at n = 4, 8, 12, and 16: the baseline with
no plan, one plan (point x point), and the four typed pairs. Each compile is
pinned as in measure_growth.py. The type-only copy is a probe, not a
proposal: it removes a check the interface promises.

The units of measure_plan.py state static_assert(core::metric_like<alg>),
so the baseline evaluates the concept once before any plan. With
--without-concept-assert that line is removed from every unit, so that the
first evaluation of the concept falls in the plans, as it does in a program
that only uses the algebra.

Writes <out-dir>/probe.tsv and <out-dir>/probe-summary.txt: for each
compiler and copy, the cost of one plan and of four plans over the
baseline at each n.
"""

import pathlib
import shutil
import statistics
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
sys.path.insert(0, str(ROOT / "docs/agent/records/performance/plan-growth"))
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import measure_plan as mp
from measure_growth import SIZES, compile_pinned, git, write_rows

CONCEPT = """concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    requires { typename std::bool_constant<Alg::value.entries_in_range()>; } &&
    (Alg::value.generator_count >= 1) && Alg::value.is_symmetric() &&
    Alg::value.entries_in_range();
"""
CONCEPT_ASSERT = "static_assert(core::metric_like<alg>);\n"
TYPE_ONLY = """concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>>;
"""


def type_only_copy(out):
    include = out / "type-only" / "include"
    if include.exists():
        shutil.rmtree(include)
    shutil.copytree(ROOT / "include", include)
    header = include / "galoot" / "core" / "metric.hpp"
    text = header.read_text(encoding="utf-8")
    assert text.count(CONCEPT) == 1
    header.write_text(text.replace(CONCEPT, TYPE_ONLY), encoding="utf-8")
    return include


def main(argv):
    without_assert = "--without-concept-assert" in argv
    argv = [a for a in argv if a != "--without-concept-assert"]
    if not argv:
        sys.exit(__doc__)
    out = pathlib.Path(argv[0]).resolve()
    repeats = int(argv[1]) if len(argv) > 1 else 3
    cpu = argv[2] if len(argv) > 2 else "6"
    if git("status", "--porcelain", "--", "include"):
        sys.exit("include/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    trees = {"as-is": ROOT / "include", "type-only": type_only_copy(out)}
    units = out / "probe-units"
    units.mkdir(exist_ok=True)
    head = git("rev-parse", "--short=7", "HEAD")
    cases = {0: [], 1: mp.TYPED_PAIRS[:1], 4: mp.TYPED_PAIRS}

    rows = []
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for plans, pairs in cases.items():
                source = units / f"n{n}-plans{plans}.cpp"
                text = mp.plans_unit(n, pairs)
                if without_assert:
                    assert text.count(CONCEPT_ASSERT) == 1
                    text = text.replace(CONCEPT_ASSERT, "")
                source.write_text(text, encoding="utf-8")
                for compiler in mp.COMPILERS:
                    for tree, include in trees.items():
                        r = compile_pinned(
                            f"probe-0012:{compiler}:{tree}:n{n}:plans{plans}:"
                            f"{repeat}", compiler, source, include, cpu)
                        rows.append({"compiler": compiler, "tree": tree,
                                     "n": n, "plans": plans,
                                     "repeat": repeat, **r})
                        print(rows[-1], flush=True)
    keys = ["inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
            "pinned_other_s"]
    write_rows(out / "probe.tsv",
               ["compiler", "tree", "n", "plans", "repeat", *keys], rows)

    def mean(**match):
        return statistics.mean(int(r["inst_p_u"]) for r in rows
                               if all(r[k] == v for k, v in match.items()))

    lines = [f"Headers at {head}; instructions on the pinned core, mean of "
             f"{repeats} repeats; plans minus the baseline, in M; units "
             + ("without" if without_assert else "with")
             + " static_assert(core::metric_like<alg>)"]
    for compiler in mp.COMPILERS:
        for tree in trees:
            for plans in (1, 4):
                cells = []
                for n in SIZES:
                    value = (mean(compiler=compiler, tree=tree, n=n,
                                  plans=plans)
                             - mean(compiler=compiler, tree=tree, n=n,
                                    plans=0)) / 1e6
                    cells.append(f"n={n} {value:.1f}")
                lines.append(f"{compiler} {tree} {plans} plan(s): "
                             + ", ".join(cells))
            base = [mean(compiler=compiler, tree=tree, n=n, plans=0) / 1e6
                    for n in SIZES]
            lines.append(f"{compiler} {tree} baseline: "
                         + ", ".join(f"n={n} {b:.1f}"
                                     for n, b in zip(SIZES, base)))
    (out / "probe-summary.txt").write_text("\n".join(lines) + "\n",
                                           encoding="utf-8")
    print("\n".join(lines))
    shutil.rmtree(units)
    shutil.rmtree(out / "type-only")


if __name__ == "__main__":
    main(sys.argv[1:])
