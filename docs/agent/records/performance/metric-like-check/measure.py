#!/usr/bin/env -S python3 -I
"""Pinned compile cost of metric_like, written several ways, as n grows.

Usage:
  measure.py <root> <variants-dir> <out-dir> <cpu> [repeats]

<root> is the checkout whose tools/timed records the runs (its runs.tsv gets
one pinned row per compile). <variants-dir> is the output of variants.py.
Every compile is
`taskset -c <cpu> <root>/tools/timed measure - <selection> -- <cxx> ...`
with Galoot's test flags at -O0 and -c. Repeats are interleaved, so that drift
affects every variant alike.

Two workloads, in R(n-1,0,1):

  concept units, metric.hpp alone, n = 4, 8, 12, 16, 32:
    c-base     the tag, and no use of the concept
    c-sa       static_assert(metric_like<alg>)
    c-use1     one class template constrained by metric_like, used once
    c-use2     that and a function template constrained by metric_like
    c-sa-use2  the static_assert and both templates
  plan units, n = 4, 8, 12, 16 (the four typed rigid(3) pairs, e4 read as e_n,
  each blade's sign the parity of its written index list):
    plans0     the algebra and four blade sets
    plans4     the same and four plans
    plans0-sa, plans4-sa  the same with static_assert(metric_like<alg>)

Writes rows.tsv (one row per compile) and summary.txt in <out-dir>.
"""

import pathlib
import statistics
import subprocess
import sys

FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-O0", "-DGALOOT_CHECKING=1"]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
CONCEPT_SIZES = (4, 8, 12, 16, 32)
PLAN_SIZES = (4, 8, 12, 16)
CONCEPT_TREES = ("base", "type", "single", "single-guarded", "cached-orig",
                 "cached", "cached-flat")
PLAN_TREES = ("base", "type", "single", "cached", "cached-flat")
CONCEPT_VARIANTS = {"c-base": (False, 0), "c-sa": (True, 0),
                    "c-use1": (False, 1), "c-use2": (False, 2),
                    "c-sa-use2": (True, 2)}
PLAN_VARIANTS = {"plans0": (0, False), "plans4": (4, False),
                 "plans0-sa": (0, True), "plans4-sa": (4, True)}
SETS = {
    "point": ["e1", "e2", "e3", "e4"],
    "line": ["e41", "e42", "e43", "e23", "e31", "e12"],
    "plane": ["e423", "e431", "e412", "e321"],
    "motor": ["1", "e41", "e42", "e43", "e23", "e31", "e12", "e1234"],
}
PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
         ("point", "motor")]


def algebra(n):
    # Above the default bound of 16 generators the factory needs MaxN.
    bound = f", {n}" if n > 16 else ""
    return (f"struct alg {{ static constexpr auto value = "
            f"core::R<{n - 1}, 0, 1{bound}>(); }};")


def concept_unit(n, sa, uses):
    lines = ["#include <cstddef>",
             "#include <galoot/core/metric.hpp>",
             "namespace core = galoot::core;",
             algebra(n)]
    if sa:
        lines.append("static_assert(core::metric_like<alg>);")
    lines += ["template <core::metric_like A> struct use {",
              "  static constexpr std::size_t n = A::value.generator_count;",
              "};",
              "template <core::metric_like A> constexpr std::size_t twice() {",
              "  return 2 * A::value.generator_count;",
              "}"]
    terms = ["use<alg>::n", "twice<alg>()"][:uses]
    lines.append("int main() { return static_cast<int>("
                 f"{' + '.join(terms) or '0'}); }}")
    return "\n".join(lines) + "\n"


def blade(name, n):
    if name == "1":
        return 0, 1
    idx = [n if c == "4" else int(c) for c in name[1:]]
    swaps = sum(1 for p, x in enumerate(idx) for y in idx[p + 1:] if x > y)
    return sum(1 << (i - 1) for i in idx), (-1) ** swaps


def plan_unit(n, plans, sa):
    lines = ["#include <cstddef>",
             "#include <galoot/core/blade_set.hpp>",
             "#include <galoot/core/metric.hpp>",
             "#include <galoot/core/plan.hpp>",
             "namespace core = galoot::core;",
             algebra(n)]
    if sa:
        lines.append("static_assert(core::metric_like<alg>);")
    for name, names in SETS.items():
        body = ", ".join(f"{{{m}U, {s}}}" for m, s in
                         (blade(x, n) for x in names))
        lines.append(f"[[maybe_unused]] constexpr core::blade_set<{len(names)}>"
                     f" {name}{{{{{{{body}}}}}}};")
    uses = []
    for a, b in PAIRS[:plans]:
        p = (f"core::detail::product_plan<core::detail::BinaryOperation::gp,"
             f" alg, {a}, {b}>.terms")
        uses.append(f"{p}[static_cast<std::size_t>(argc) % {p}.size()]"
                    ".coefficient")
    lines.append("int main([[maybe_unused]] int argc, char**) { return "
                 f"static_cast<int>({' + '.join(uses) or '0'}); }}")
    return "\n".join(lines) + "\n"


def row_of(root, selection):
    # The latest row with this selection, so that rows other runs append at
    # the same time cannot be taken for it.
    text = (root / "docs/agent/records/performance/runs.tsv").read_text(
        encoding="utf-8").splitlines()
    keys = text[0].split("\t")
    for line in reversed(text[1:]):
        row = dict(zip(keys, line.split("\t")))
        if row.get("selection") == selection:
            return row
    return {}


def compile_one(root, cpu, selection, compiler, source, include):
    obj = source.parent / f"{source.stem}.{compiler}.o"
    cmd = ["taskset", "-c", cpu, str(root / "tools/timed"), "measure", "-",
           selection, "--", COMPILERS[compiler], *FLAGS, f"-I{include}",
           "-c", "-o", str(obj), str(source)]
    r = subprocess.run(cmd, cwd=root, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f"{selection} failed:\n{r.stderr[:3000]}")
    row = row_of(root, selection)
    if row.get("selection") != selection or row.get("mode") != "pinned":
        sys.exit(f"{selection}: row missing or not pinned: {row}")
    obj.unlink(missing_ok=True)
    return {k: row.get(k, "unavailable") for k in
            ("utc_start", "inst_p_u", "wall_s", "maxrss_kib",
             "sibling_busy_s", "pinned_other_s")}


def mean_spread(rows, **match):
    v = [int(r["inst_p_u"]) for r in rows
         if all(str(r[k]) == str(x) for k, x in match.items())]
    return statistics.mean(v), max(v) - min(v)


def summarize(rows):
    out = ["Retired instructions on the pinned core, mean of the repeats, in "
           "millions; spread (max - min) in parentheses.", ""]
    out.append("Concept units: each variant minus c-base of the same tree.")
    for c in COMPILERS:
        for t in CONCEPT_TREES:
            for v in CONCEPT_VARIANTS:
                if v == "c-base":
                    continue
                parts = []
                for n in CONCEPT_SIZES:
                    a, sa = mean_spread(rows, work="concept", compiler=c,
                                        tree=t, n=n, variant=v)
                    b, sb = mean_spread(rows, work="concept", compiler=c,
                                        tree=t, n=n, variant="c-base")
                    parts.append(f"n{n} {(a - b) / 1e6:.2f} "
                                 f"({max(sa, sb) / 1e6:.2f})")
                out.append(f"{c} {t} {v}: " + "; ".join(parts))
        out.append("")
    out.append("Concept units: c-base itself.")
    for c in COMPILERS:
        for t in CONCEPT_TREES:
            parts = []
            for n in CONCEPT_SIZES:
                b, sb = mean_spread(rows, work="concept", compiler=c, tree=t,
                                    n=n, variant="c-base")
                parts.append(f"n{n} {b / 1e6:.2f} ({sb / 1e6:.2f})")
            out.append(f"{c} {t} c-base: " + "; ".join(parts))
    out.append("")
    out.append("Plan units: plans4 minus plans0 (and with the static_assert), "
               "growth from n = 4 to 16.")
    for c in COMPILERS:
        for t in PLAN_TREES:
            for p, b_ in (("plans4", "plans0"), ("plans4-sa", "plans0-sa"),
                          ("plans4-sa", "plans0")):
                parts, first = [], None
                for n in PLAN_SIZES:
                    a, sa = mean_spread(rows, work="plan", compiler=c, tree=t,
                                        n=n, variant=p)
                    b, sb = mean_spread(rows, work="plan", compiler=c, tree=t,
                                        n=n, variant=b_)
                    d = a - b
                    first = d if first is None else first
                    parts.append(f"n{n} {d / 1e6:.1f} ({max(sa, sb) / 1e6:.1f})")
                out.append(f"{c} {t} {p} - {b_}: " + "; ".join(parts) +
                           f"; growth {(d - first) / 1e6:+.1f}")
        out.append("")
    worst = max(float(r["sibling_busy_s"]) for r in rows
                if r["sibling_busy_s"] not in ("--", "unavailable"))
    other = max(float(r["pinned_other_s"]) for r in rows
                if r["pinned_other_s"] not in ("--", "unavailable"))
    out.append(f"Largest sibling-thread busy time in one compile: {worst} s; "
               f"largest other work on the pinned CPU: {other} s.")
    return "\n".join(out) + "\n"


def write(path, rows):
    keys = list(rows[0])
    path.write_text("\t".join(keys) + "\n" + "".join(
        "\t".join(str(r[k]) for k in keys) + "\n" for r in rows),
        encoding="utf-8")


def main(argv):
    if len(argv) < 4:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    variants = pathlib.Path(argv[1]).resolve()
    out = pathlib.Path(argv[2]).resolve()
    cpu = argv[3]
    repeats = int(argv[4]) if len(argv) > 4 else 3
    out.mkdir(parents=True, exist_ok=True)
    units = variants / "units"
    units.mkdir(exist_ok=True)

    jobs = []
    for n in CONCEPT_SIZES:
        for v, (sa, uses) in CONCEPT_VARIANTS.items():
            src = units / f"concept-n{n}-{v}.cpp"
            src.write_text(concept_unit(n, sa, uses), encoding="utf-8")
            jobs += [("concept", n, v, t, src) for t in CONCEPT_TREES]
    for n in PLAN_SIZES:
        for v, (plans, sa) in PLAN_VARIANTS.items():
            src = units / f"plan-n{n}-{v}.cpp"
            src.write_text(plan_unit(n, plans, sa), encoding="utf-8")
            jobs += [("plan", n, v, t, src) for t in PLAN_TREES]

    rows = []
    for rep in range(1, repeats + 1):
        for work, n, v, t, src in jobs:
            for c in COMPILERS:
                sel = f"metric-like-cost:{c}:{t}:{work}:n{n}:{v}:{rep}"
                r = compile_one(root, cpu, sel, c, src,
                                variants / t / "include")
                rows.append({"work": work, "compiler": c, "tree": t, "n": n,
                             "variant": v, "repeat": rep, **r})
                print(sel, r["inst_p_u"], r["sibling_busy_s"],
                      r["pinned_other_s"], flush=True)
        write(out / "rows.tsv", rows)
    (out / "summary.txt").write_text(summarize(rows), encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
