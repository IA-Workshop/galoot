#!/usr/bin/env -S python3 -I
"""Measure what geometric-product plans cost to generate, and where the
compilers' default limits on constant evaluation stop them.

Usage, from the repository root, with the job pool exported:
  measure_plan.py limits <out-dir>
  measure_plan.py growth <out-dir> [repeats] [cpu]
  measure_plan.py locals <out-dir> [repeats] [cpu]
  measure_plan.py evals <out-dir>

Every compile runs through tools/timed, pinned to `cpu` (default 6) with
taskset, so its wall time, peak memory, and retired instructions are in the
run log; this script copies them into <out-dir>/<mode>.tsv. Instructions are
the counts of both kinds of core added. Each selection starts with
plan-<mode>: and names the point.

limits  Each unit is compiled with the compilers' default limits in three
        configurations: Galoot's test flags at -O0 (with checking) and at -O2
        (without), both with -D_GLIBCXX_ASSERTIONS, and a bare
        -std=c++23 -pedantic-errors. The units are the typed rigid(3) pairs,
        the conformal(3) vector pair, the sparse pairs at n = 8, 12, and 16,
        and full<n> times full<n> for R(n,0,0), conformal(n - 2), R(n-1,0,1),
        and a dense form with 2 on the diagonal and 1 elsewhere, from n = 2
        up to the first n that fails. The record holds each exit status and
        the first diagnostic line that names a limit.
growth  Fixed use as n grows: the four typed rigid(3) pairs (point x point,
        line x plane, motor x point, point x motor) embedded in R(n-1,0,1),
        e4 becoming e_n, at n = 4, 8, and 16. Distinct uses at n = 4: units
        with 1, 4, and 16 plans of the point, line, plane, and motor sets.
        Each has a baseline unit with the same algebra and sets and no plan.
        Galoot's test flags at -O0, compiled with -c, so each plan's table is
        emitted.
locals  The form of make_gp_plan's locals: the header as it is, with static
        constexpr locals, against a copy with plain constexpr locals, for the
        typed pairs and full<4> times full<4> in rigid(3).
evals   One evaluation per argument set: Clang -ftime-trace of
        test/core/plan_test.cpp and of a unit with two uses of one plan in two
        functions and one other plan; counts the instantiations of
        make_gp_plan and the evaluations of gp_plan's initializer and of the
        fill.
"""

import collections
import json
import pathlib
import re
import shutil
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare": ["-std=c++23", "-pedantic-errors"],
}
LIMIT_TEXT = ("step limit", "operation count exceeds", "constexpr-ops-limit",
              "constexpr-loop-limit", "constexpr-depth", "not a constant")

PREAMBLE = """#include <cstddef>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/plan.hpp>

namespace core = galoot::core;
using core::detail::gp_plan;
"""

RIGID_NAMES = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}
TYPED_PAIRS = [("point", "point", 15), ("line", "plane", 15),
               ("motor", "point", 28), ("point", "motor", 28)]


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


def rigid_sets(n):
    lines = [f"struct alg {{ static constexpr auto value = "
             f"core::R<{n - 1}, 0, 1>(); }};",
             "static_assert(core::metric_like<alg>);"]
    for name, names in RIGID_NAMES.items():
        lines.append(f"[[maybe_unused]] constexpr auto {name} = "
                     f"{set_literal(names, n)};")
    return "\n".join(lines) + "\n"


def plans_unit(n, pairs):
    """A unit that uses each plan of `pairs` in a static_assert and at run
    time, so that its table is emitted."""
    text = PREAMBLE + rigid_sets(n)
    uses = []
    for a, b, size in pairs:
        plan = f"gp_plan<alg, {a}, {b}>"
        if size is not None:
            text += f"static_assert({plan}.size() == {size});\n"
        uses.append(f"{plan}[static_cast<std::size_t>(argc) % {plan}.size()]"
                    ".coefficient")
    body = " + ".join(uses) if uses else "0"
    text += ("int main([[maybe_unused]] int argc, char**) {\n"
             "  return static_cast<int>("
             f"{body});\n}}\n")
    return text


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


def full_unit(form, n):
    return (PREAMBLE + f"struct alg {{ static constexpr auto value = "
            f"{form_literal(form, n)}; }};\n"
            f"static_assert(gp_plan<alg, core::full<{n}>, core::full<{n}>>"
            ".size() > 0);\n")


def sparse_unit(n):
    en = 1 << (n - 1)
    first = (f"core::blade_set<4>{{{{{{{{0U, 1}}, {{1U, 1}}, {{{en}U, 1}}, "
             f"{{{en | 1}U, 1}}}}}}}}")
    second = (f"core::blade_set<3>{{{{{{{{1U, 1}}, {{2U, 1}}, "
              f"{{{en}U, 1}}}}}}}}")
    return (PREAMBLE + f"struct alg {{ static constexpr auto value = "
            f"core::R<{n - 1}, 0, 1>(); }};\n"
            f"static_assert(gp_plan<alg, {first}, {second}>.size() == 10);\n")


def vectors_unit():
    return (PREAMBLE + "struct alg { static constexpr auto value = "
            "core::conformal<3>(); };\n"
            "static_assert(gp_plan<alg, core::of_grade<5, 1>, "
            "core::of_grade<5, 1>>.size() == 25);\n")


def typed_unit():
    pairs = TYPED_PAIRS + [("line", "line", 27)]
    return plans_unit(4, pairs)


def read_last_row():
    with open(ROOT / "docs/agent/records/performance/runs.tsv",
              encoding="utf-8") as f:
        lines = f.read().splitlines()
    header = lines[0].split("\t")
    return dict(zip(header, lines[-1].split("\t")))


def compile_unit(selection, compiler, flags, source, include, cpu, extra=()):
    obj = source.with_suffix(f".{compiler}.o")
    command = [str(ROOT / "tools" / "timed"), "measure", "-", selection, "--",
               "taskset", "-c", cpu, COMPILERS[compiler], *flags, *extra,
               f"-I{include}", "-c", "-o", str(obj), str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    row = read_last_row()
    if row.get("selection") != selection:
        sys.exit(f"the last run row is not {selection}")

    def number(field):
        value = row.get(field, "")
        return int(value) if value.isdigit() else None

    inst = [number("inst_p_u"), number("inst_e_u")]
    instructions = sum(i for i in inst if i is not None) if any(
        i is not None for i in inst) else "unavailable"
    limit = next((line.strip().replace(f"{ROOT}/", "")
                  for line in result.stderr.splitlines()
                  if any(t in line for t in LIMIT_TEXT)), "-")
    return {"exit": result.returncode, "wall_s": row.get("wall_s"),
            "peak_kib": row.get("maxrss_kib"), "instructions": instructions,
            "limit": limit[:200], "stderr": result.stderr}


def write_rows(path, header, rows):
    path.write_text("\t".join(header) + "\n" + "".join(
        "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")


def limits(out, cpu):
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    fixed = {"typed": typed_unit(), "conformal3-vectors": vectors_unit(),
             "sparse8": sparse_unit(8), "sparse12": sparse_unit(12),
             "sparse16": sparse_unit(16)}
    rows = []
    for compiler in COMPILERS:
        for config, flags in CONFIGS.items():
            for unit, text in fixed.items():
                source = units / f"limits-{unit}.cpp"
                source.write_text(text, encoding="utf-8")
                r = compile_unit(f"plan-limits:{compiler}:{config}:{unit}",
                                 compiler, flags, source, ROOT / "include",
                                 cpu)
                rows.append({"compiler": compiler, "config": config,
                             "unit": unit, **r})
                print(rows[-1]["unit"], compiler, config, r["exit"], r["limit"],
                      flush=True)
            for form in ("r", "conformal", "rigid", "dense"):
                for n in range(3 if form == "conformal" else 2, 9):
                    unit = f"full-{form}-n{n}"
                    source = units / f"limits-{unit}.cpp"
                    source.write_text(full_unit(form, n), encoding="utf-8")
                    r = compile_unit(
                        f"plan-limits:{compiler}:{config}:{unit}", compiler,
                        flags, source, ROOT / "include", cpu)
                    rows.append({"compiler": compiler, "config": config,
                                 "unit": unit, **r})
                    print(unit, compiler, config, r["exit"], r["limit"],
                          flush=True)
                    if r["exit"] != 0:
                        break
    write_rows(out / "limits.tsv", ["compiler", "config", "unit", "exit",
                                    "wall_s", "peak_kib", "instructions",
                                    "limit"], rows)


def growth(out, repeats, cpu):
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    names = list(RIGID_NAMES)
    cases = []
    for n in (4, 8, 16):
        cases.append(("fixed", n, 0, []))
        cases.append(("fixed", n, 4, TYPED_PAIRS))
    every = [(a, b, None) for a in names for b in names]
    cases.append(("distinct", 4, 0, []))
    cases.append(("distinct", 4, 1, TYPED_PAIRS[:1]))
    cases.append(("distinct", 4, 4, TYPED_PAIRS))
    cases.append(("distinct", 4, 16, every))
    rows = []
    for repeat in range(1, repeats + 1):
        for kind, n, count, pairs in cases:
            unit = f"{kind}-n{n}-plans{count}"
            source = units / f"growth-{unit}.cpp"
            source.write_text(plans_unit(n, pairs), encoding="utf-8")
            for compiler in COMPILERS:
                r = compile_unit(f"plan-growth:{compiler}:{unit}:{repeat}",
                                 compiler, CONFIGS["galoot-O0"], source,
                                 ROOT / "include", cpu)
                if r["exit"] != 0:
                    sys.exit(f"{unit} failed on {compiler}:\n{r['stderr']}")
                rows.append({"compiler": compiler, "kind": kind, "n": n,
                             "plans": count, "repeat": repeat, **r})
                print(unit, compiler, repeat, r["instructions"], flush=True)
    write_rows(out / "growth.tsv", ["compiler", "kind", "n", "plans",
                                    "repeat", "exit", "wall_s", "peak_kib",
                                    "instructions"], rows)


def plain_locals_include(out):
    include = out / "plain-locals" / "include"
    if include.exists():
        shutil.rmtree(include)
    shutil.copytree(ROOT / "include", include)
    header = include / "galoot" / "core" / "plan.hpp"
    text = header.read_text(encoding="utf-8")
    start = text.index("consteval auto make_gp_plan() {")
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert body.count("static constexpr ") == 7, body.count("static constexpr ")
    header.write_text(text[:start] + body.replace("static constexpr ",
                                                  "constexpr ")
                      + text[end:], encoding="utf-8")
    return include


def locals_form(out, repeats, cpu):
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    includes = {"static": ROOT / "include",
                "plain": plain_locals_include(out)}
    sources = {"typed": typed_unit(), "full4-rigid": full_unit("rigid", 4)}
    rows = []
    for repeat in range(1, repeats + 1):
        for unit, text in sources.items():
            source = units / f"locals-{unit}.cpp"
            source.write_text(text, encoding="utf-8")
            for compiler in COMPILERS:
                for form, include in includes.items():
                    r = compile_unit(
                        f"plan-locals:{compiler}:{unit}:{form}:{repeat}",
                        compiler, CONFIGS["galoot-O0"], source, include, cpu)
                    if r["exit"] != 0:
                        sys.exit(f"{unit} failed:\n{r['stderr']}")
                    rows.append({"compiler": compiler, "unit": unit,
                                 "locals": form, "repeat": repeat, **r})
                    print(unit, compiler, form, r["instructions"], flush=True)
    shutil.rmtree(out / "plain-locals")
    write_rows(out / "locals.tsv", ["compiler", "unit", "locals", "repeat",
                                    "exit", "wall_s", "peak_kib",
                                    "instructions"], rows)


TWO_USES = PREAMBLE + rigid_sets(4) + """
std::size_t first() { return gp_plan<alg, point, point>.size(); }
core::coefficient_type second() {
  return gp_plan<alg, point, point>[3].coefficient;
}
core::coefficient_type third() {
  return gp_plan<alg, line, line>[3].coefficient;
}
int main() {
  return static_cast<int>(first()) + static_cast<int>(second() + third());
}
"""


def evals(out, cpu):
    trace_dir = out / "evals"
    trace_dir.mkdir(parents=True, exist_ok=True)
    two_uses = trace_dir / "two_uses.cpp"
    two_uses.write_text(TWO_USES, encoding="utf-8")
    units = {"plan_test": ROOT / "test" / "core" / "plan_test.cpp",
             "two_uses": two_uses}
    lines = ["unit\tinstantiations\tgp_plan_evaluations\tfill_evaluations"]
    for unit, source in units.items():
        copy = trace_dir / f"{unit}.cpp"
        if copy != source:
            shutil.copy(source, copy)
            shutil.copy(source.parent / "signatures.hpp",
                        trace_dir / "signatures.hpp")
        r = compile_unit(f"plan-evals:clang:{unit}", "clang",
                         CONFIGS["galoot-O0"], copy, ROOT / "include", cpu,
                         extra=["-ftime-trace", "-ftime-trace-granularity=0",
                                f"-I{ROOT / 'include' / 'ext'}"])
        if r["exit"] != 0:
            sys.exit(f"{unit} failed:\n{r['stderr']}")
        trace = copy.with_suffix(".clang.json")
        events = [e for e in json.loads(trace.read_text())["traceEvents"]
                  if e.get("ph") == "X"]
        # The fill is the local `plan` of make_gp_plan, so only an
        # evaluation of a `plan` within an instantiation of make_gp_plan
        # counts; the test's own variables of that name do not.
        counts = collections.Counter()
        instantiations = [
            (e["ts"], e["ts"] + e["dur"]) for e in events
            if e["name"] == "InstantiateFunction" and re.match(
                r"galoot::core::detail::make_gp_plan<",
                e.get("args", {}).get("detail", ""))]
        counts["instantiations"] = len(instantiations)
        for e in events:
            detail = e.get("args", {}).get("detail", "")
            if e["name"] != "EvaluateAsInitializer":
                continue
            if detail == "galoot::core::detail::gp_plan":
                counts["gp_plan"] += 1
            elif detail == "plan" and any(start <= e["ts"] < end
                                          for start, end in instantiations):
                counts["fill"] += 1
        lines.append(f"{unit}\t{counts['instantiations']}\t"
                     f"{counts['gp_plan']}\t{counts['fill']}")
        print(lines[-1], flush=True)
        trace.unlink()
        copy.with_suffix(".clang.o").unlink()
    (out / "evals.tsv").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)
    mode, out = argv[0], pathlib.Path(argv[1]).resolve()
    out.mkdir(parents=True, exist_ok=True)
    repeats = int(argv[2]) if len(argv) > 2 else 3
    cpu = argv[3] if len(argv) > 3 else "6"
    if mode == "limits":
        limits(out, cpu)
    elif mode == "growth":
        growth(out, repeats, cpu)
    elif mode == "locals":
        locals_form(out, repeats, cpu)
    elif mode == "evals":
        evals(out, cpu)
    else:
        sys.exit(__doc__)
    shutil.rmtree(out / "units", ignore_errors=True)
    for obj in out.rglob("*.o"):
        obj.unlink()


if __name__ == "__main__":
    main(sys.argv[1:])
