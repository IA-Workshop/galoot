#!/usr/bin/env -S python3 -I
"""Measure WU8b's additions to mv.hpp: where the default limits stop apply,
what apply, a conversion, and truncate cost a program, how the cost of a
fixed use through apply grows with n, and what the headers cost every
includer, before and after.

Usage, from the worktree root, with the job pool exported and TIMED_LOG set
to the absolute path of the campaign's log:
  measure_mv_b.py limits <out-dir> [cpu]
  measure_mv_b.py uses <out-dir> [repeats] [cpu]
  measure_mv_b.py growth <before-revision> <out-dir> [repeats] [cpu]
  measure_mv_b.py headers <before-revision> <out-dir> [repeats] [cpu]

Every compile runs as `taskset -c <cpu> tools/timed measure ...` (default
CPU 10, a performance core), so its row in the campaign's log is in pinned
mode, with the instructions retired, the wall time, the peak memory, and the
busy time of the sibling thread and of other processes on the CPU. The
headers after are the checkout's include/, which must have no uncommitted
changes; the headers before are extracted from <before-revision> with git
archive.

limits   apply<Op> on full<n> times full<n>, and apply<Op> of each unary map
         on full<n>, at the largest n whose plan or map builds at the default
         limits (the ceilings of product_plan's and unary_map's comments),
         on run-time inputs, in the three configurations of the plan
         limits: Galoot's test flags at -O0 with checking and at -O2
         without, and a bare -std=c++23 -pedantic-errors -O2. One trial
         build at each point.
uses     Units with REPS = 0, 64, and 256 uses, at -O0 and -O2, over
         distinct four-blade layouts set<i> of R(15,0,1):
         construct  set<i> made from four values read at run time;
         plan       the same, and a term of the plan of set<i> times set<i>
                    read at run time, so the plan is generated;
         product    the same, and apply of that product;
         repeat     one product applied in each of REPS functions;
         pair       construct, and a value of the layout set<i> reversed;
         convert    construct, and the value converted to that layout;
         half       construct, and a value of set<i>'s first two blades;
         truncate   construct, and the value truncated to those two blades.
         Per use, from the slope from 64 to 256: a new product's plan, plan
         - construct; its kernel, product - plan; an applied existing
         product, repeat; a conversion, convert - pair; a truncation,
         truncate - half.
growth   Plan 0013's fixed use, four typed rigid(3) products in R(n-1,0,1)
         with e4 as e_n, at n = 4, 8, 12, 16, -O0 with the test flags: a
         baseline with the sets and no product; the plans' terms read at run
         time (before and after); and the same products applied to
         multivectors read at run time (after).
headers  An empty unit including galoot/core.hpp or galoot/core/mv.hpp, at
         -O0 with the test flags, before and after, interleaved.
"""

import io
import os
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
KEYS = ("exit", "inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
        "pinned_other_s")
REPS = (0, 64, 256)
SIZES = (4, 8, 12, 16)

PREAMBLE = """#include <array>
#include <cstddef>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace gc = galoot::core;
using gc::detail::BinaryOperation;
using gc::detail::UnaryOperation;

"""


def log_path():
    log = os.environ.get("TIMED_LOG", "")
    if not log.startswith("/"):
        sys.exit("TIMED_LOG must be the absolute path of the campaign's log")
    return pathlib.Path(log)


def read_last_row():
    lines = log_path().read_text(encoding="utf-8").splitlines()
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
        sys.exit(f"the last row of the log is not {selection}")
    if row.get("mode") != "pinned":
        sys.exit(f"{selection} was not pinned: mode {row.get('mode')}")
    obj.unlink(missing_ok=True)
    error = next((line.strip().replace(f"{ROOT}/", "")
                  for line in result.stderr.splitlines() if "error" in line),
                 "-")
    found = {key: row.get(key, "unavailable") for key in KEYS[1:]}
    return {"exit": result.returncode, **found, "error": error[:200]}


def write_rows(path, header, rows):
    path.write_text("\t".join(header) + "\n" + "".join(
        "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")


def clean_checkout():
    status = subprocess.run(
        ["git", "status", "--porcelain", "--", "include"], cwd=ROOT,
        capture_output=True, text=True, check=True).stdout
    if status.strip():
        sys.exit("include/ has uncommitted changes:\n" + status)


def extract(revision, dest):
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    dest.mkdir(parents=True, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(dest, filter="data")
    return dest / "include"


def form_literal(form, n):
    if form == "r":
        return f"gc::R<{n}, 0, 0>()"
    if form == "conformal":
        return f"gc::conformal<{n - 2}>()"
    if form == "rigid":
        return f"gc::R<{n - 1}, 0, 1>()"
    rows = ", ".join("{" + ", ".join("2" if i == j else "1" for j in range(n))
                     + "}" for i in range(n))
    return f"gc::from_gram({{{rows}}})"


def limit_unit(op, form, n, unary):
    text = (PREAMBLE + "struct alg {\n"
            f"  static constexpr auto value = {form_literal(form, n)};\n"
            "};\n"
            f"using M = gc::mv<alg, gc::full<{n}>, double>;\n"
            "double use(const volatile double* v) {\n"
            "  M x{};\n"
            "  for (std::size_t i = 0; i < M::component_count; ++i) {\n"
            "    x.coefficients[i] = v[i % 4];\n"
            "  }\n")
    if unary:
        text += (f"  const auto r = gc::detail::apply<UnaryOperation::{op}>(x);"
                 "\n")
    else:
        text += (f"  const auto r = gc::detail::apply<BinaryOperation::{op}>"
                 "(x, x);\n")
    return text + "  return r.coefficients[0];\n}\n"


# The ceilings that product_plan's and unary_map's comments state: the
# largest n whose plan or map of full<n> builds at the default limits.
LIMIT_POINTS = {
    "clang": [("gp", "r", 5, False), ("gp", "rigid", 5, False),
              ("gp", "conformal", 5, False), ("gp", "dense", 4, False),
              ("wedge", "r", 5, False), ("antigp", "r", 5, False),
              ("antiwedge", "r", 5, False), ("reverse", "r", 7, True),
              ("antireverse", "r", 7, True),
              ("grade_involution", "r", 7, True),
              ("right_complement", "r", 8, True),
              ("left_complement", "r", 8, True)],
    "gcc": [("gp", "r", 6, False), ("gp", "rigid", 6, False),
            ("gp", "conformal", 6, False), ("gp", "dense", 5, False),
            ("wedge", "r", 6, False), ("antigp", "r", 6, False),
            ("antiwedge", "r", 6, False), ("reverse", "r", 9, True),
            ("antireverse", "r", 9, True), ("grade_involution", "r", 9, True),
            ("right_complement", "r", 9, True),
            ("left_complement", "r", 9, True)],
}


def limits(out, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    rows = []
    for compiler, points in LIMIT_POINTS.items():
        for config in CONFIGS:
            for op, form, n, unary in points:
                unit = f"{op}-{form}-n{n}"
                source = units / f"limits-{unit}.cpp"
                source.write_text(limit_unit(op, form, n, unary),
                                  encoding="utf-8")
                r = compile_pinned(f"mvb-limits:{compiler}:{config}:{unit}",
                                   compiler, CONFIGS[config], source,
                                   ROOT / "include", cpu)
                rows.append({"compiler": compiler, "config": config,
                             "unit": unit, **r})
                print(compiler, config, unit, r["exit"], r["inst_p_u"],
                      r["wall_s"], r["maxrss_kib"], r["error"], flush=True)
    write_rows(out / "limits.tsv",
               ["compiler", "config", "unit", *KEYS, "error"], rows)


def set_literal(masks):
    blades = ", ".join(f"{{.mask = {m}, .sign = 1}}" for m in masks)
    return (f"gc::blade_set<{len(masks)}>{{"
            f"std::to_array<gc::oriented_blade>({{{blades}}})}}")


def sets_text(reps):
    text = ("struct alg {\n  static constexpr auto value = "
            "gc::R<15, 0, 1>();\n};\n")
    for i in range(reps):
        masks = [4 * i + k for k in range(1, 5)]
        text += (f"inline constexpr auto set{i} = {set_literal(masks)};\n"
                 f"inline constexpr auto reversed{i} = "
                 f"{set_literal(masks[::-1])};\n"
                 f"inline constexpr auto half{i} = "
                 f"{set_literal(masks[:2])};\n")
    return text


def use_body(kind, i, layout):
    make = (f"  const gc::mv<alg, {layout}, double> x{{v[0], v[1], v[2], "
            "v[3]};\n")
    if kind == "construct":
        return make + "  return x.coefficients[3];\n"
    if kind == "plan":
        return make + (
            "  return x.coefficients[3] + static_cast<double>(gc::detail::"
            f"product_plan<BinaryOperation::gp, alg, {layout}, {layout}>"
            ".terms[0].coefficient);\n")
    if kind in ("product", "repeat"):
        return make + (
            "  return x.coefficients[3] + static_cast<double>(gc::detail::"
            f"product_plan<BinaryOperation::gp, alg, {layout}, {layout}>"
            ".terms[0].coefficient) + gc::detail::apply<BinaryOperation::gp>"
            "(x, x).coefficients[0];\n")
    if kind == "pair":
        return make + (f"  const gc::mv<alg, reversed{i}, double> y{{}};\n"
                       "  return x.coefficients[3] + y.coefficients[0];\n")
    if kind == "convert":
        return make + (f"  const gc::mv<alg, reversed{i}, double> y{{x}};\n"
                       "  return x.coefficients[3] + y.coefficients[0];\n")
    if kind == "half":
        return make + (f"  const gc::mv<alg, half{i}, double> y{{}};\n"
                       "  return x.coefficients[3] + y.coefficients[0];\n")
    assert kind == "truncate"
    return make + (f"  const auto y = gc::truncate<half{i}>(x);\n"
                   "  return x.coefficients[3] + y.coefficients[0];\n")


USE_KINDS = ("construct", "plan", "product", "repeat", "pair", "convert",
             "half", "truncate")


def uses_unit(kind, reps):
    text = PREAMBLE + sets_text(1 if kind == "repeat" else reps)
    for i in range(reps):
        layout = "set0" if kind == "repeat" else f"set{i}"
        body = use_body(kind, i, layout)
        if kind == "repeat":
            body = body.replace("v[3]}", f"v[{i % 4}]}}")
        text += f"double use{i}(const volatile double* v) {{\n{body}}}\n"
    return text


def uses(out, repeats, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for config in ("galoot-O0", "galoot-O2"):
                for kind in USE_KINDS:
                    for reps in REPS:
                        source = units / f"uses-{kind}-{reps}.cpp"
                        source.write_text(uses_unit(kind, reps),
                                          encoding="utf-8")
                        r = compile_pinned(
                            f"mvb-uses:{compiler}:{config}:{kind}:{reps}:"
                            f"r{repeat}", compiler, CONFIGS[config], source,
                            ROOT / "include", cpu)
                        if r["exit"] != 0:
                            sys.exit(f"{source} failed: {r['error']}")
                        rows.append({"compiler": compiler, "config": config,
                                     "kind": kind, "reps": reps,
                                     "repeat": repeat, **r})
                        print(compiler, config, kind, reps, repeat, r["exit"],
                              r["inst_p_u"], r["wall_s"], r["maxrss_kib"],
                              flush=True)
    write_rows(out / "uses.tsv", ["compiler", "config", "kind", "reps",
                                  "repeat", *KEYS], rows)
    (out / "uses-summary.txt").write_text(uses_summary(rows),
                                          encoding="utf-8")


def mean(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return statistics.mean(values)


def spread(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return max(values) - min(values)


def uses_summary(rows):
    lines = ["Retired instructions per use, slope from 64 to 256 uses, mean "
             "of the repeats", ""]
    for compiler in COMPILERS:
        for config in ("galoot-O0", "galoot-O2"):
            def slope(kind):
                return (mean(rows, compiler=compiler, config=config,
                             kind=kind, reps=256) -
                        mean(rows, compiler=compiler, config=config,
                             kind=kind, reps=64)) / 192
            per = {kind: slope(kind) for kind in USE_KINDS}
            lines.append(
                f"{compiler} {config}: plan of a new product "
                f"{per['plan'] - per['construct']:.3e}, its kernel "
                f"{per['product'] - per['plan']:.3e}, an existing product "
                f"applied again {per['repeat']:.3e} (a function each, with "
                f"its construction), conversion "
                f"{per['convert'] - per['pair']:.3e}, truncate "
                f"{per['truncate'] - per['half']:.3e}")
    return "\n".join(lines) + "\n"


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
    mask = sum(1 << (i - 1) for i in indices)
    return mask, -1 if inversions % 2 else 1


def rigid_sets(n):
    text = (f"struct alg {{ static constexpr auto value = "
            f"gc::R<{n - 1}, 0, 1>(); }};\n")
    for name, names in RIGID_NAMES.items():
        blades = [oriented(b, n) for b in names.split()]
        inner = ", ".join(f"{{{mask}U, {sign}}}" for mask, sign in blades)
        text += (f"[[maybe_unused]] constexpr auto {name} = "
                 f"gc::blade_set<{len(blades)}>{{{{{{{inner}}}}}}};\n")
    return text


def growth_unit(kind, n):
    text = PREAMBLE + rigid_sets(n)
    uses = []
    if kind == "plans":
        for a, b in PAIRS:
            plan = (f"gc::detail::product_plan<BinaryOperation::gp, alg, {a}, "
                    f"{b}>.terms")
            uses.append(f"static_cast<double>({plan}[static_cast<std::size_t>"
                        f"(argc) % {plan}.size()].coefficient)")
    elif kind == "apply":
        for a, b in PAIRS:
            uses.append(f"gc::detail::apply<BinaryOperation::gp>(make<{a}>(v), "
                        f"make<{b}>(v)).coefficients[0]")
        text += ("template <auto Layout>\n"
                 "gc::mv<alg, Layout, double> make(const volatile double* v) {"
                 "\n  gc::mv<alg, Layout, double> x{};\n"
                 "  for (std::size_t i = 0; i < x.coefficients.size(); ++i) {\n"
                 "    x.coefficients[i] = v[i % 4];\n  }\n  return x;\n}\n")
    body = " + ".join(uses) if uses else "0.0"
    return (text + "int main(int argc, char**) {\n"
            "  const volatile double v[4] = {1.0, 2.0, 3.0, 4.0};\n"
            "  (void)v;\n"
            f"  return static_cast<int>({body}) + argc;\n}}\n")


def growth(before_revision, out, repeats, cpu):
    clean_checkout()
    out.mkdir(parents=True, exist_ok=True)
    trees = {"before": extract(before_revision, out / "before"),
             "after": ROOT / "include"}
    units = out / "units"
    units.mkdir(exist_ok=True)
    rows = []
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for kind in ("baseline", "plans", "apply"):
                for tree, include in trees.items():
                    if kind == "apply" and tree == "before":
                        continue
                    source = units / f"{tree}-n{n}-{kind}.cpp"
                    source.write_text(growth_unit(kind, n), encoding="utf-8")
                    for compiler in COMPILERS:
                        r = compile_pinned(
                            f"mvb-growth:{compiler}:{tree}:n{n}:{kind}:"
                            f"{repeat}", compiler, CONFIGS["galoot-O0"],
                            source, include, cpu)
                        if r["exit"] != 0:
                            sys.exit(f"{source} failed: {r['error']}")
                        rows.append({"compiler": compiler, "tree": tree,
                                     "n": n, "kind": kind, "repeat": repeat,
                                     **r})
                        print(compiler, tree, n, kind, repeat, r["inst_p_u"],
                              r["sibling_busy_s"], flush=True)
    write_rows(out / "growth.tsv",
               ["compiler", "tree", "n", "kind", "repeat", *KEYS], rows)
    (out / "growth-summary.txt").write_text(growth_summary(rows),
                                            encoding="utf-8")
    shutil.rmtree(out / "before")


def growth_summary(rows):
    lines = ["Mean retired instructions (M) of the fixed use, above the "
             "baseline of the same tree and n; spread of the repeats in "
             "parentheses", ""]
    for compiler in COMPILERS:
        for tree, kind in (("before", "plans"), ("after", "plans"),
                           ("after", "apply")):
            parts = []
            for n in SIZES:
                use = mean(rows, compiler=compiler, tree=tree, n=n, kind=kind)
                base = mean(rows, compiler=compiler, tree=tree, n=n,
                            kind="baseline")
                parts.append(f"n={n} {(use - base) / 1e6:.1f} "
                             f"({spread(rows, compiler=compiler, tree=tree, n=n, kind=kind) / 1e6:.1f})")
            lines.append(f"{compiler} {tree} {kind}: " + ", ".join(parts))
        for tree in ("before", "after"):
            parts = [f"n={n} "
                     f"{mean(rows, compiler=compiler, tree=tree, n=n, kind='baseline') / 1e6:.1f}"
                     for n in SIZES]
            lines.append(f"{compiler} {tree} baseline: " + ", ".join(parts))
    return "\n".join(lines) + "\n"


def headers(before, out, repeats, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    trees = {"before": extract(before, out / "before"),
             "after": ROOT / "include"}
    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for header in ("galoot/core.hpp", "galoot/core/mv.hpp"):
                name = header.replace("/", "_").replace(".hpp", "")
                source = units / f"header-{name}.cpp"
                source.write_text(f"#include <{header}>\n", encoding="utf-8")
                for tree, include in trees.items():
                    r = compile_pinned(
                        f"mvb-headers:{compiler}:{tree}:{name}:r{repeat}",
                        compiler, CONFIGS["galoot-O0"], source, include, cpu)
                    rows.append({"compiler": compiler, "tree": tree,
                                 "header": header, "repeat": repeat, **r})
                    print(compiler, tree, header, repeat, r["exit"],
                          r["inst_p_u"], flush=True)
    write_rows(out / "headers.tsv", ["compiler", "tree", "header", "repeat",
                                     *KEYS], rows)
    shutil.rmtree(out / "before")


def main(argv):
    if not argv:
        sys.exit(__doc__)
    mode, rest = argv[0], argv[1:]
    if mode == "limits" and len(rest) >= 1:
        limits(pathlib.Path(rest[0]).resolve(),
               rest[1] if len(rest) > 1 else "10")
    elif mode == "uses" and len(rest) >= 1:
        uses(pathlib.Path(rest[0]).resolve(),
             int(rest[1]) if len(rest) > 1 else 3,
             rest[2] if len(rest) > 2 else "10")
    elif mode in ("growth", "headers") and len(rest) >= 2:
        run = growth if mode == "growth" else headers
        run(rest[0], pathlib.Path(rest[1]).resolve(),
            int(rest[2]) if len(rest) > 2 else 3,
            rest[3] if len(rest) > 3 else "10")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
