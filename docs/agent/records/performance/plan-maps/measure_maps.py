#!/usr/bin/env -S python3 -I
"""Measure where the compilers' default limits on constant evaluation stop
the antiproduct plans and the unary maps, and how the cost of a fixed use
grows with the number of generators.

Usage, from the repository root, with the job pool exported:
  measure_maps.py limits <out-dir> [cpu]
  measure_maps.py growth <before-revision> <out-dir> [repeats] [cpu]
  measure_maps.py sparse <out-dir> [repeats] [cpu]

Every compile runs as `taskset -c <cpu> tools/timed measure ...` (default
CPU 10, a performance core), so its row in runs.tsv is in pinned mode, with
the instructions retired on that core, the wall time, the peak memory, and
the busy time of the sibling thread and of other processes on the CPU.

limits  In the three configurations of the typed-plan limits (Galoot's test
        flags at -O0 with checking and at -O2 without, and a bare
        -std=c++23 -pedantic-errors -O2): product_plan of the geometric
        antiproduct and the antiwedge over the typed rigid(3) pairs, the
        conformal(3) vectors, the sparse sets {1, e1, e_n, e1n} x
        {e1, e2, e_n} of R(n-1,0,1) and their right complements, at n = 8,
        12, and 16, and full<n> x full<n> for R(n,0,0), conformal(n - 2),
        R(n-1,0,1), and a dense form with 2 on the diagonal and 1 elsewhere,
        from n = 2 up to the first n that fails; and unary_map of each of the
        five operations over the four rigid(3) sets and over full<n> of
        R(n,0,0) from n = 2 up to the first n that fails or n = 12. Writes
        <out-dir>/limits.tsv with each exit status and the first diagnostic
        line that names a limit.
growth  Fixed uses embedded in R(n-1,0,1), e4 becoming e_n, at n = 4, 8, 12,
        and 16, each with a baseline unit of the same algebra and sets and
        no plan or map: the four typed rigid(3) pairs (point x point,
        line x plane, motor x point, point x motor) through the geometric
        product, with "before" the headers of <before-revision> and "after"
        the checkout's include/, which must have no uncommitted changes; the
        same four pairs through the geometric antiproduct, after only; and
        the right complement and the reverse of the four rigid(3) sets,
        after only. No unit asserts metric_like itself, so its one
        evaluation per unit falls in the plans, as in a program that only
        uses the algebra. Galoot's test flags at -O0, -c, each unit reading
        every term at run time so that its table is emitted. The compiles
        are interleaved repeat by repeat. Writes <out-dir>/growth.tsv and
        <out-dir>/summary.txt.
sparse  The sparse sets {1, e1, e_n, e1n} x {e1, e2, e_n} of R(n-1,0,1), at
        n = 4, 8, 12, and 16, through the geometric product and through the
        geometric antiproduct, each unit reading every term at run time,
        with a baseline of the same algebra and sets and no plan; the
        checkout's include/, Galoot's flags at -O0. Writes
        <out-dir>/sparse.tsv and <out-dir>/summary.txt.
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
              "constant expression", "error:")
ANTI = ("antigp", "antiwedge")
MAPS = ("reverse", "antireverse", "grade_involution", "right_complement",
        "left_complement")
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
ANTI_TYPED_PAIRS = {
    "antigp": [("point", "point"), ("plane", "plane"), ("motor", "point"),
               ("motor", "motor")],
    "antiwedge": [("plane", "plane"), ("line", "plane"), ("plane", "line")],
}


def preamble():
    return ("#include <cstddef>\n\n"
            "#include <galoot/core/blade_set.hpp>\n"
            "#include <galoot/core/metric.hpp>\n"
            "#include <galoot/core/plan.hpp>\n\n"
            "namespace core = galoot::core;\n")


def plan_terms(tree, op, alg, a, b):
    """The expression of a plan's array of terms in the headers of `tree`."""
    if tree == "before":
        assert op == "gp"
        return (f"core::detail::product_plan<core::detail::BinaryOperation::gp, "
                f"{alg}, {a}, {b}>.terms")
    return (f"core::detail::product_plan<core::detail::BinaryOperation::{op}, "
            f"{alg}, {a}, {b}>.terms")


def map_terms(op, alg, s):
    return (f"core::detail::unary_map<core::detail::UnaryOperation::{op}, "
            f"{alg}, {s}>.terms")


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


def set_literal(blades):
    inner = ", ".join(f"{{{mask}U, {sign}}}" for mask, sign in blades)
    return f"core::blade_set<{len(blades)}>{{{{{{{inner}}}}}}}"


def algebra(value):
    return f"struct alg {{ static constexpr auto value = {value}; }};\n"


def rigid_sets(n):
    text = algebra(f"core::R<{n - 1}, 0, 1>()")
    for name, names in RIGID_NAMES.items():
        blades = [oriented(x, n) for x in names.split()]
        text += (f"[[maybe_unused]] constexpr auto {name} = "
                 f"{set_literal(blades)};\n")
    return text


def uses_unit(n, uses):
    """A unit of the rigid(3) sets in R(n-1,0,1) that reads every term of each
    array of terms in `uses` at run time, so that its table is emitted."""
    text = preamble() + rigid_sets(n)
    reads = [f"{t}[static_cast<std::size_t>(argc) % {t}.size()].coefficient"
             for t in uses]
    body = " + ".join(reads) if reads else "0"
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


def full_plan_unit(op, form, n):
    full = f"core::full<{n}>"
    return (preamble() + algebra(form_literal(form, n)) +
            f"static_assert({plan_terms('after', op, 'alg', full, full)}"
            ".size() > 0);\n")


def full_map_unit(op, n):
    full = f"core::full<{n}>"
    return (preamble() + algebra(f"core::R<{n}, 0, 0>()") +
            f"static_assert({map_terms(op, 'alg', full)}.size() == "
            f"{1 << n});\n")


def complement(mask, sign, n):
    """The right complement of sign e_mask in n generators, as (mask, sign)."""
    other = ((1 << n) - 1) ^ mask
    swaps = sum(1 for i in range(n) if mask >> i & 1
                for j in range(i) if other >> j & 1)
    return other, sign * (-1 if swaps % 2 else 1)


def sparse_sets(n, complemented):
    en = 1 << (n - 1)
    first = [(0, 1), (1, 1), (en, 1), (en | 1, 1)]
    second = [(1, 1), (2, 1), (en, 1)]
    if complemented:
        first = [complement(m, s, n) for m, s in first]
        second = [complement(m, s, n) for m, s in second]
    return set_literal(first), set_literal(second)


def sparse_unit(op, n, complemented):
    """The antiwedge of the low-grade sparse sets has no terms (every pair of
    their complements shares a generator), so the unit asks only for the
    plan's size, which needs the plan generated."""
    first, second = sparse_sets(n, complemented)
    plan = plan_terms('after', op, 'alg', first, second)
    return (preamble() + algebra(f"core::R<{n - 1}, 0, 1>()") +
            f"static_assert(sizeof({plan}) == sizeof({plan}[0]) * "
            f"{plan}.size() || {plan}.empty());\n")


def vectors_unit(op):
    vectors = "core::of_grade<5, 1>"
    check = "> 0" if op == "antigp" else "== 0"
    return (preamble() + algebra("core::conformal<3>()") +
            f"static_assert({plan_terms('after', op, 'alg', vectors, vectors)}"
            f".size() {check});\n")


def typed_unit(op):
    return uses_unit(4, [plan_terms("after", op, "alg", a, b)
                         for a, b in ANTI_TYPED_PAIRS[op]])


def typed_map_unit(op):
    return uses_unit(4, [map_terms(op, "alg", s) for s in RIGID_NAMES])


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
        r = compile_pinned(f"maps-limits:{compiler}:{config}:{op}:{unit}",
                           compiler, CONFIGS[config], source, ROOT / "include",
                           cpu)
        rows.append({"compiler": compiler, "config": config, "op": op,
                     "unit": unit, **r})
        print(compiler, config, op, unit, r["exit"], r["inst_p_u"],
              r["limit"], flush=True)
        return r["exit"]

    for compiler in COMPILERS:
        for config in CONFIGS:
            for op in ANTI:
                fixed = {"typed": typed_unit(op),
                         "conformal3-vectors": vectors_unit(op)}
                for n in (8, 12, 16):
                    fixed[f"sparse{n}"] = sparse_unit(op, n, False)
                    fixed[f"sparse-complements{n}"] = sparse_unit(op, n, True)
                for unit, text in fixed.items():
                    measure(compiler, config, op, unit, text)
                for form in ("r", "conformal", "rigid", "dense"):
                    for n in range(3 if form == "conformal" else 2, 10):
                        if measure(compiler, config, op, f"full-{form}-n{n}",
                                   full_plan_unit(op, form, n)) != 0:
                            break
            for op in MAPS:
                measure(compiler, config, op, "typed", typed_map_unit(op))
                for n in range(2, 13):
                    if measure(compiler, config, op, f"full-n{n}",
                               full_map_unit(op, n)) != 0:
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


# The uses of the growth units: (use, tree, the arrays of terms read).
def growth_uses():
    uses = [("baseline", "before", []), ("baseline", "after", [])]
    uses.append(("gp", "before", [plan_terms("before", "gp", "alg", a, b)
                                  for a, b in TYPED_PAIRS]))
    uses.append(("gp", "after", [plan_terms("after", "gp", "alg", a, b)
                                 for a, b in TYPED_PAIRS]))
    uses.append(("antigp", "after", [plan_terms("after", "antigp", "alg", a, b)
                                     for a, b in TYPED_PAIRS]))
    uses.append(("maps", "after",
                 [map_terms(op, "alg", s) for op in ("right_complement",
                                                    "reverse")
                  for s in RIGID_NAMES]))
    return uses


def mean_of(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return statistics.mean(values), max(values) - min(values)


def summary(rows, before, after):
    lines = [f"before {before}, after {after}; instructions retired on the "
             "pinned core, mean of the repeats, in M; each use is the unit "
             "minus the baseline of its tree at the same n"]
    for compiler in COMPILERS:
        for use, tree, _ in growth_uses():
            if use == "baseline":
                continue
            parts, first = [], None
            for n in SIZES:
                full, spread_full = mean_of(rows, compiler=compiler, tree=tree,
                                            n=n, use=use)
                base, spread_base = mean_of(rows, compiler=compiler, tree=tree,
                                            n=n, use="baseline")
                cost = (full - base) / 1e6
                first = cost if first is None else first
                parts.append(f"n={n} {cost:.1f} M "
                             f"({100 * (cost / first - 1):+.1f}%, spread "
                             f"{max(spread_full, spread_base) / 1e6:.1f}, "
                             f"baseline {base / 1e6:.1f} M)")
            lines.append(f"{compiler} {use} {tree}: " + "; ".join(parts))
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
            for use, tree, uses in growth_uses():
                source = units / f"{use}-{tree}-n{n}.cpp"
                source.write_text(uses_unit(n, uses), encoding="utf-8")
                for compiler in COMPILERS:
                    r = compile_pinned(
                        f"maps-growth:{compiler}:{use}:{tree}:n{n}:{repeat}",
                        compiler, CONFIGS["galoot-O0"], source, trees[tree],
                        cpu)
                    if r["exit"] != 0:
                        sys.exit(f"{source} failed:\n{r['stderr'][:4000]}")
                    rows.append({"compiler": compiler, "use": use,
                                 "tree": tree, "n": n, "repeat": repeat, **r})
                    print(compiler, use, tree, n, repeat, r["inst_p_u"],
                          r["sibling_busy_s"], flush=True)
    write_rows(out / "growth.tsv",
               ["compiler", "use", "tree", "n", "repeat", *KEYS], rows)
    (out / "summary.txt").write_text(summary(rows, before, after),
                                     encoding="utf-8")
    print((out / "summary.txt").read_text(encoding="utf-8"))
    shutil.rmtree(out / "before")


def sparse_growth(out, repeats, cpu):
    if git("status", "--porcelain", "--", "include", "test"):
        sys.exit("include/ or test/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    units = out / "units"
    units.mkdir(exist_ok=True)
    after = git("rev-parse", "--short=7", "HEAD")
    rows = []
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            first, second = sparse_sets(n, False)
            for use in ("baseline", "gp", "antigp"):
                reads = [] if use == "baseline" else [
                    plan_terms("after", use, "alg", first, second)]
                body = " + ".join(
                    f"{t}[static_cast<std::size_t>(argc) % {t}.size()]"
                    ".coefficient" for t in reads) or "0"
                text = (preamble() + algebra(f"core::R<{n - 1}, 0, 1>()") +
                        "int main([[maybe_unused]] int argc, char**) {\n"
                        f"  return static_cast<int>({body});\n}}\n")
                source = units / f"sparse-{use}-n{n}.cpp"
                source.write_text(text, encoding="utf-8")
                for compiler in COMPILERS:
                    r = compile_pinned(
                        f"maps-sparse:{compiler}:{use}:n{n}:{repeat}",
                        compiler, CONFIGS["galoot-O0"], source,
                        ROOT / "include", cpu)
                    if r["exit"] != 0:
                        sys.exit(f"{source} failed:\n{r['stderr'][:4000]}")
                    rows.append({"compiler": compiler, "use": use,
                                 "tree": "after", "n": n, "repeat": repeat,
                                 **r})
                    print(compiler, use, n, repeat, r["inst_p_u"], flush=True)
    write_rows(out / "sparse.tsv",
               ["compiler", "use", "tree", "n", "repeat", *KEYS], rows)
    lines = [f"headers of {after}; instructions retired on the pinned core, "
             "mean of the repeats, in M; each use is the unit minus the "
             "baseline at the same n"]
    for compiler in COMPILERS:
        for use in ("gp", "antigp"):
            costs = []
            for n in SIZES:
                full, spread = mean_of(rows, compiler=compiler, use=use, n=n)
                base, base_spread = mean_of(rows, compiler=compiler,
                                            use="baseline", n=n)
                costs.append((n, (full - base) / 1e6,
                              max(spread, base_spread) / 1e6))
            parts = "; ".join(f"n={n} {c:.1f} M (spread {s:.1f})"
                              for n, c, s in costs)
            lines.append(f"{compiler} {use}: {parts}; n=16 / n=4 = "
                         f"{costs[-1][1] / costs[0][1]:.2f}")
    (out / "summary.txt").write_text("\n".join(lines) + "\n",
                                     encoding="utf-8")
    print((out / "summary.txt").read_text(encoding="utf-8"))


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
    elif mode == "sparse":
        out = pathlib.Path(argv[1]).resolve()
        sparse_growth(out, int(argv[2]) if len(argv) > 2 else 3,
                      argv[3] if len(argv) > 3 else "10")
    else:
        sys.exit(__doc__)
    shutil.rmtree(out / "units", ignore_errors=True)


if __name__ == "__main__":
    main(sys.argv[1:])
