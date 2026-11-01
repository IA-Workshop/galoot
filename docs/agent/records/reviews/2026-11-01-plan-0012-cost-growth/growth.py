#!/usr/bin/env -S python3 -I
"""Pinned compile cost of a fixed use as n grows, and of the header and test
units, with two header sets (before and after a change), and with a probe copy of
the new headers in which metric_like keeps only its type test.

Usage:
  growth.py <root> <old-include> <new-include> <out-dir> <cpu> [repeats]

<root> is the checkout whose tools/timed records the runs (its runs.tsv gets
one pinned row per compile). Every compile is
`taskset -c <cpu> <root>/tools/timed measure - <selection> -- <cxx> ...`
with Galoot's test flags at -O0 and -c.

Units of the fixed use, at n = 4, 8, 12, 16, in R(n-1,0,1):
  plans0-sa  the algebra, static_assert(metric_like<alg>), four blade sets
  plans1-sa  the same and the plan of point x point
  plans4-sa  the same and the plans of point x point, line x plane,
             motor x point, point x motor
  plans0, plans4  as plans0-sa and plans4-sa without the static_assert
The sets are rigid(3)'s point, line, plane, and motor with e4 read as e_n,
each blade's sign the parity of its written index list.

Writes growth.tsv, header.tsv, and summary.txt in <out-dir>.
"""

import pathlib
import re
import shutil
import statistics
import subprocess
import sys

FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-O0", "-DGALOOT_CHECKING=1"]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
SIZES = (4, 8, 12, 16)
SETS = {
    "point": ["e1", "e2", "e3", "e4"],
    "line": ["e41", "e42", "e43", "e23", "e31", "e12"],
    "plane": ["e423", "e431", "e412", "e321"],
    "motor": ["1", "e41", "e42", "e43", "e23", "e31", "e12", "e1234"],
}
PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
         ("point", "motor")]
VARIANTS = {"plans0-sa": (0, True), "plans1-sa": (1, True),
            "plans4-sa": (4, True), "plans0": (0, False),
            "plans4": (4, False)}
HEADERS = {"bits.hpp": "galoot/core/bits.hpp",
           "blade_product.hpp": "galoot/core/blade_product.hpp",
           "core.hpp": "galoot/core.hpp"}
TESTS = ("bits_test", "blade_product_test", "blade_set_test", "plan_test")


def blade(name, n):
    if name == "1":
        return 0, 1
    idx = [n if c == "4" else int(c) for c in name[1:]]
    swaps = sum(1 for p, x in enumerate(idx) for y in idx[p + 1:] if x > y)
    return sum(1 << (i - 1) for i in idx), (-1) ** swaps


def unit(n, plans, sa):
    lines = ["#include <cstddef>",
             "#include <galoot/core/blade_set.hpp>",
             "#include <galoot/core/metric.hpp>",
             "#include <galoot/core/plan.hpp>",
             "namespace core = galoot::core;",
             f"struct alg {{ static constexpr auto value = "
             f"core::R<{n - 1}, 0, 1>(); }};"]
    if sa:
        lines.append("static_assert(core::metric_like<alg>);")
    for name, names in SETS.items():
        body = ", ".join(f"{{{m}U, {s}}}" for m, s in
                         (blade(x, n) for x in names))
        lines.append(f"[[maybe_unused]] constexpr core::blade_set<{len(names)}>"
                     f" {name}{{{{{{{body}}}}}}};")
    uses = []
    for a, b in PAIRS[:plans]:
        p = f"core::detail::gp_plan<alg, {a}, {b}>"
        uses.append(f"{p}[static_cast<std::size_t>(argc) % {p}.size()]"
                    ".coefficient")
    lines.append("int main([[maybe_unused]] int argc, char**) { return "
                 f"static_cast<int>({' + '.join(uses) or '0'}); }}")
    return "\n".join(lines) + "\n"


def last_row(root):
    text = (root / "docs/agent/records/performance/runs.tsv").read_text(
        encoding="utf-8").splitlines()
    return dict(zip(text[0].split("\t"), text[-1].split("\t")))


def compile_one(root, cpu, selection, compiler, source, include, extra=()):
    obj = source.parent / f"{source.stem}.{compiler}.o"
    cmd = ["taskset", "-c", cpu, str(root / "tools/timed"), "measure", "-",
           selection, "--", COMPILERS[compiler], *FLAGS, f"-I{include}",
           *extra, "-c", "-o", str(obj), str(source)]
    r = subprocess.run(cmd, cwd=root, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f"{selection} failed:\n{r.stderr[:3000]}")
    row = last_row(root)
    if row.get("selection") != selection or row.get("mode") != "pinned":
        sys.exit(f"{selection}: row missing or not pinned: {row}")
    obj.unlink(missing_ok=True)
    return {k: row.get(k, "unavailable") for k in
            ("inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
             "pinned_other_s")}


def make_probe(new_include, out):
    probe = out / "probe" / "include"
    if probe.parent.exists():
        shutil.rmtree(probe.parent)
    shutil.copytree(new_include, probe)
    path = probe / "galoot/core/metric.hpp"
    text = path.read_text(encoding="utf-8")
    pattern = re.compile(
        r"(concept metric_like =\n\s+detail::is_metric_v<std::remove_cvref_t"
        r"<decltype\(Alg::value\)>>) &&\n.*?;\n", re.S)
    text, count = pattern.subn(r"\1;\n", text)
    assert count == 1, "metric_like not found"
    path.write_text(text, encoding="utf-8")
    return probe


def stats(rows, **match):
    v = [int(r["inst_p_u"]) for r in rows
         if all(str(r[k]) == str(x) for k, x in match.items())]
    return statistics.mean(v), max(v) - min(v)


def summarize(growth, header, trees):
    out = ["Instructions retired on the pinned core, mean of the repeats; "
           "spread is max - min.", ""]
    for c in COMPILERS:
        for t in trees:
            for plans_v, base_v in (("plans4-sa", "plans0-sa"),
                                    ("plans1-sa", "plans0-sa"),
                                    ("plans4", "plans0")):
                parts, first = [], None
                for n in SIZES:
                    a, sa_ = stats(growth, compiler=c, tree=t, n=n,
                                   variant=plans_v)
                    b, sb = stats(growth, compiler=c, tree=t, n=n,
                                  variant=base_v)
                    d = a - b
                    first = d if first is None else first
                    parts.append(f"n{n} {d / 1e6:.1f}M ({100 * (d / first - 1):+.1f}%,"
                                 f" spr {max(sa_, sb) / 1e6:.1f}M, base "
                                 f"{b / 1e6:.1f}M)")
                out.append(f"{c} {t} {plans_v}-{base_v}: " + "; ".join(parts))
        out.append("")
    for c in COMPILERS:
        for u in [*HEADERS, *TESTS]:
            b, sb = stats(header, compiler=c, tree="old", unit=u)
            a, sa_ = stats(header, compiler=c, tree="new", unit=u)
            out.append(f"{c} {u}: old {b / 1e6:.1f}M (spr {sb / 1e6:.1f}M), "
                       f"new {a / 1e6:.1f}M (spr {sa_ / 1e6:.1f}M), "
                       f"{(a - b) / 1e6:+.1f}M ({100 * (a / b - 1):+.2f}%)")
    return "\n".join(out) + "\n"


def write(path, rows):
    keys = list(rows[0])
    path.write_text("\t".join(keys) + "\n" + "".join(
        "\t".join(str(r[k]) for k in keys) + "\n" for r in rows),
        encoding="utf-8")


def main(argv):
    if len(argv) < 5:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    old, new = pathlib.Path(argv[1]).resolve(), pathlib.Path(argv[2]).resolve()
    out = pathlib.Path(argv[3]).resolve()
    cpu = argv[4]
    repeats = int(argv[5]) if len(argv) > 5 else 3
    out.mkdir(parents=True, exist_ok=True)
    trees = {"old": old, "new": new, "probe": make_probe(new, out)}
    units = out / "units"
    units.mkdir(exist_ok=True)
    test_dir = root / "test"
    extra = [f"-isystem{root / 'include/ext'}", f"-I{test_dir}",
             "-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS"]

    growth, header = [], []
    for rep in range(1, repeats + 1):
        for n in SIZES:
            for v, (plans, sa) in VARIANTS.items():
                src = units / f"n{n}-{v}.cpp"
                src.write_text(unit(n, plans, sa), encoding="utf-8")
                for c in COMPILERS:
                    for t, inc in trees.items():
                        sel = f"review-0012-growth:{c}:{t}:n{n}:{v}:{rep}"
                        r = compile_one(root, cpu, sel, c, src, inc)
                        growth.append({"compiler": c, "tree": t, "n": n,
                                       "variant": v, "repeat": rep, **r})
                        print(sel, r["inst_p_u"], r["sibling_busy_s"],
                              r["pinned_other_s"], flush=True)
        for u in [*HEADERS, *TESTS]:
            if u in HEADERS:
                src = units / f"{u.replace('.', '_')}.cpp"
                src.write_text(f"#include <{HEADERS[u]}>\n", encoding="utf-8")
                ex = ()
            else:
                src = test_dir / "core" / f"{u}.cpp"
                ex = extra
            for c in COMPILERS:
                for t in ("old", "new"):
                    sel = f"review-0012-header:{c}:{t}:{u}:{rep}"
                    if u in TESTS:
                        # Object beside the scratch units, not in the tree.
                        copy = units / f"{u}.cpp"
                        shutil.copy(src, copy)
                        r = compile_one(root, cpu, sel, c, copy, trees[t],
                                        [*ex, f"-I{test_dir / 'core'}"])
                    else:
                        r = compile_one(root, cpu, sel, c, src, trees[t], ex)
                    header.append({"compiler": c, "tree": t, "unit": u,
                                   "repeat": rep, **r})
                    print(sel, r["inst_p_u"], r["sibling_busy_s"],
                          r["pinned_other_s"], flush=True)
    write(out / "growth.tsv", growth)
    write(out / "header.tsv", header)
    (out / "summary.txt").write_text(summarize(growth, header, trees),
                                     encoding="utf-8")
    print((out / "summary.txt").read_text(encoding="utf-8"))
    shutil.rmtree(units)
    shutil.rmtree(out / "probe")


if __name__ == "__main__":
    main(sys.argv[1:])
