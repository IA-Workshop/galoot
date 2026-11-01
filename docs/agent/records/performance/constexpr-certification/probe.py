#!/usr/bin/env -S python3 -I
"""Measure the constant-evaluation cost of each phase of the product-plan
generator, and of whole plans, as the smallest limit under which a unit
compiles.

Usage, from the repository root, with the job pool exported
(eval "$(tools/jobpool env)"):

  probe.py cases <set> <cases.tsv>
      Writes the cases of a set: train (phase probes, for fitting),
      train-gp (more probes of the geometric product's count and fill), test
      (phase probes the fit never sees), plans (whole plans of full and
      random sets), wide (whole plans of the sets programs write, n up to
      16), with seeds from 20261101 for the random ones.
  probe.py run <cases.tsv> <out.tsv> <compiler> <config> <cache> [jobs]
          [weights.tsv]
      Finds the threshold of each case, starting each search at the
      prediction of the weights when they are given (cost.py), which only
      saves compiles: the search ends at the measured threshold whatever
      the start. <cache> is warm (the compiler's
      defaults) or cold (GCC with -fconstexpr-cache-depth=1, which caches no
      constant call; for Clang, which has no such cache, the same as warm).
      Rows already in <out.tsv> are kept, so an interrupted run resumes.

Each case is one unit (written under <out.tsv>.units/). A phase probe holds
the inputs of one phase as literals and evaluates that phase once, as its own
constant evaluation, as make_product_plan does; its baseline is the same unit
without that evaluation, measured too, so that a phase cheaper than the rest
of the unit is reported as hidden rather than mistaken for its cost. A plan
case reads product_plan, which runs every phase.

The threshold counts what the compiler counts in the largest evaluation of
the unit (harness.py). Every compile is -fsyntax-only: constant evaluation
happens in the front end.
"""

import concurrent.futures
import importlib.util
import pathlib
import random
import sys

HERE = pathlib.Path(__file__).resolve().parent


def load(name):
    spec = importlib.util.spec_from_file_location(name, HERE / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


harness = load("harness")
mirror = load("mirror")

SEED = 20261101
PREAMBLE = ("#include <array>\n#include <cstddef>\n\n"
            "#include <galoot/core/blade_set.hpp>\n"
            "#include <galoot/core/metric.hpp>\n"
            "#include <galoot/core/plan.hpp>\n\n"
            "namespace core = galoot::core;\n"
            "namespace d = galoot::core::detail;\n")


# A case is a dict: name, kind (a phase name or "plan" or "baseline-<phase>"),
# op, form, n, a and b (lists of (mask, sign)), and for plan cases the set
# expressions if they are named sets ("full").

def encode_set(blades):
    return ",".join(f"{m}:{s}" for m, s in blades)


def decode_set(text):
    return [tuple(int(v) for v in item.split(":")) for item in text.split(",")
            if item]


def gram_of(case):
    if case["form"].startswith("gram="):
        rows = case["form"][5:].split("/")
        return [[int(v) for v in row.split(".")] for row in rows]
    return mirror.form(case["form"], case["n"])


def metric_expr(case):
    n, f = case["n"], case["form"]
    if f == "r":
        return f"core::R<{n}, 0, 0>()"
    if f == "rigid":
        return f"core::R<{n - 1}, 0, 1>()"
    if f == "conformal":
        return f"core::conformal<{n - 2}>()"
    rows = ", ".join("{" + ", ".join(str(v) for v in row) + "}"
                     for row in gram_of(case))
    return f"core::from_gram({{{rows}}})"


def set_expr(blades):
    inner = ", ".join(f"{{{m}U, {s}}}" for m, s in blades)
    return f"core::blade_set<{len(blades)}>{{{{{{{inner}}}}}}}"


def op_expr(op):
    return f"d::BinaryOperation::{op}"


def plan_literal(plan):
    inner = ",\n    ".join(f"d::plan_term{{{a}, {b}, {m}U, {c}}}"
                           for a, b, m, c in plan)
    return (f"[[maybe_unused]] constexpr std::array<d::plan_term, {len(plan)}> lit{{{{\n    "
            f"{inner}}}}};\n")


def unit(case):
    """The source of a case."""
    op, n = case["op"], case["n"]
    a, b = case["a"], case["b"]
    text = PREAMBLE + ("struct alg { static constexpr auto value = "
                       f"{metric_expr(case)}; }};\n")
    kind = case["kind"]
    if kind == "plan":
        first = case.get("a_expr") or set_expr(a)
        second = case.get("b_expr") or set_expr(b)
        return text + (f"static_assert(d::product_plan<{op_expr(op)}, alg, "
                       f"{first}, {second}>.terms.size() > 0);\n")
    phase = kind.removeprefix("baseline-")
    probe = not kind.startswith("baseline-")
    plan, result, typed, _ = mirror.phases(op, gram_of(case), a, b)
    capacity = mirror.plan_capacity(op, a, b, mirror.collections.Counter())
    text += f"[[maybe_unused]] constexpr auto A = {set_expr(a)};\n"
    text += f"[[maybe_unused]] constexpr auto B = {set_expr(b)};\n"
    if phase in ("capacity", "count", "fill"):
        line = {
            "capacity": f"d::plan_capacity({op_expr(op)}, A.blades, B.blades)",
            "count": (f"d::generate_plan<{op_expr(op)}, {capacity}>"
                      "(alg::value, A.blades, B.blades, {})"),
            "fill": (f"d::filled_plan<{op_expr(op)}, {len(plan)}, "
                     f"{capacity}>(alg::value, A.blades, B.blades)"),
        }[phase]
        if probe:
            text += f"[[maybe_unused]] constexpr auto probe = {line};\n"
        return text
    text += plan_literal(plan)
    R, T = len(result), len(plan)
    if phase == "defects":
        line = f"d::defects_of(lit, {len(a)}, {len(b)}, {n})"
    elif phase == "collect":
        line = "d::collected(lit)"
    elif phase == "typed":
        blades = ", ".join(f"{{{m}U, 1}}" for m in result)
        text += (f"[[maybe_unused]] constexpr d::collected_result<{T}> found{{{{{{{blades}}}}}"
                 f", {R}}};\n")
        line = f"d::typed<{R}>(lit, found)"
    elif phase == "typing":
        blades = ", ".join(f"{{{m}U, 1}}" for m in result)
        terms = ", ".join(f"d::typed_term{{{a_}, {b_}, {o}, {c}}}"
                          for a_, b_, o, c in typed)
        text += (f"[[maybe_unused]] constexpr d::typed_plan<{R}, {T}> typed_lit{{"
                 f"core::blade_set<{R}>{{{{{{{blades}}}}}}}, "
                 f"std::array<d::typed_term, {T}>{{{{{terms}}}}}}};\n")
        line = "d::typed_defects_of(typed_lit.result, typed_lit.terms)"
    else:
        raise ValueError(phase)
    if probe:
        text += f"[[maybe_unused]] constexpr auto probe = {line};\n"
    return text


PHASE_PROBES = ("capacity", "count", "fill", "defects", "collect", "typed",
                "typing")


def random_set(rng, n, size, signs=True):
    masks = rng.sample(range(1 << n), size)
    return [(m, rng.choice((1, -1)) if signs else 1) for m in masks]


def random_gram(rng, n, density):
    g = [[0] * n for _ in range(n)]
    for i in range(n):
        g[i][i] = rng.choice((1, -1, 0, 2))
        for j in range(i):
            if rng.random() < density:
                g[i][j] = g[j][i] = rng.choice((1, -1, 2))
    return "gram=" + "/".join(".".join(str(v) for v in row) for row in g)


def phase_cases(prefix, rng, count, ops=("gp", "wedge"),
                phases=PHASE_PROBES):
    """Random inputs for the phases: forms, sizes, and operations."""
    cases = []
    for i in range(count):
        n = rng.randint(3, 6)
        op = rng.choice(ops)
        f = rng.choice(("r", "rigid", "conformal", "dense", "random"))
        f = random_gram(rng, n, rng.choice((0.2, 0.5))) if f == "random" else f
        if f == "conformal" and n < 3:
            f = "r"
        top = 1 << n
        ka = rng.randint(2, min(top, 24))
        kb = rng.randint(2, min(top, 24))
        a, b = random_set(rng, n, ka), random_set(rng, n, kb)
        base = {"op": op, "form": f, "n": n, "a": a, "b": b}
        plan, _, _, _ = mirror.phases(op, gram_of(base), a, b)
        if not plan:
            continue
        for phase in phases:
            for kind in (phase, f"baseline-{phase}"):
                cases.append({**base, "name": f"{prefix}{i}-{kind}",
                              "kind": kind})
    return cases


def plan_cases():
    """Whole plans: the full sets of the recorded table, the typed rigid(3)
    pairs, sparse pairs, and random sets."""
    cases = []
    for op in ("gp", "wedge"):
        for f in ("r", "rigid", "conformal", "dense"):
            for n in range(3, 8):
                if f == "dense" and op == "gp" and n > 6:
                    continue
                s = mirror.full(n)
                cases.append({"name": f"full-{op}-{f}-n{n}", "kind": "plan",
                              "op": op, "form": f, "n": n, "a": s, "b": s,
                              "a_expr": f"core::full<{n}>",
                              "b_expr": f"core::full<{n}>"})
    rng = random.Random(SEED + 1)
    for i in range(24):
        n = rng.randint(4, 9)
        op = rng.choice(("gp", "wedge"))
        f = rng.choice(("r", "rigid", "conformal", "dense", "random"))
        f = random_gram(rng, n, 0.3) if f == "random" else f
        top = 1 << n
        a = random_set(rng, n, rng.randint(4, min(top, 96)))
        b = random_set(rng, n, rng.randint(4, min(top, 96)))
        cases.append({"name": f"random-{i}", "kind": "plan", "op": op,
                      "form": f, "n": n, "a": a, "b": b})
    return cases


RIGID_NAMES = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}


def rigid_set(names, n):
    """A rigid(3) set with e4 read as e_n, signs from the index order."""
    blades = []
    for name in names.split():
        if name == "1":
            blades.append((0, 1))
            continue
        idx = [n if ch == "4" else int(ch) for ch in name[1:]]
        inversions = sum(1 for p in range(len(idx))
                         for q in range(p + 1, len(idx)) if idx[p] > idx[q])
        blades.append((sum(1 << (i - 1) for i in idx),
                       -1 if inversions % 2 else 1))
    return blades


def wide_cases():
    """Plans of the sets programs write, at n up to 16: the rigid(3) pairs
    embedded with e4 as e_n, the sparse pairs, and random small sets."""
    cases = []
    pairs = [("point", "point"), ("line", "plane"), ("motor", "point"),
             ("point", "motor"), ("line", "line"), ("motor", "motor")]
    for n in (4, 8, 12, 16):
        for op in ("gp", "wedge"):
            for x, y in pairs:
                a, b = rigid_set(RIGID_NAMES[x], n), rigid_set(RIGID_NAMES[y], n)
                plan, _, _, _ = mirror.phases(op, mirror.form("rigid", n), a, b)
                if plan:
                    cases.append({"name": f"rigid-{op}-{x}-{y}-n{n}",
                                  "kind": "plan", "op": op, "form": "rigid",
                                  "n": n, "a": a, "b": b})
            en = 1 << (n - 1)
            cases.append({"name": f"sparse-{op}-n{n}", "kind": "plan",
                          "op": op, "form": "rigid", "n": n,
                          "a": [(0, 1), (1, 1), (en, 1), (en | 1, 1)],
                          "b": [(1, 1), (2, 1), (en, 1)]})
    rng = random.Random(SEED + 4)
    for i in range(16):
        n = rng.randint(10, 16)
        op = rng.choice(("gp", "wedge"))
        f = rng.choice(("r", "rigid", "conformal", "random"))
        f = random_gram(rng, n, 0.15) if f == "random" else f
        a = random_set(rng, n, rng.randint(2, 12))
        b = random_set(rng, n, rng.randint(2, 12))
        plan, _, _, _ = mirror.phases(op, gram_of({"form": f, "n": n}), a, b)
        if plan:
            cases.append({"name": f"wide-{i}", "kind": "plan", "op": op,
                          "form": f, "n": n, "a": a, "b": b})
    return cases


FIELDS = ("name", "kind", "op", "form", "n", "a", "b", "a_expr", "b_expr")


def write_cases(path, cases):
    with open(path, "w", encoding="utf-8") as f:
        f.write("\t".join(FIELDS) + "\n")
        for c in cases:
            row = {**c, "a": encode_set(c["a"]), "b": encode_set(c["b"])}
            f.write("\t".join(str(row.get(k) or "-") for k in FIELDS) + "\n")


def read_cases(path):
    lines = pathlib.Path(path).read_text(encoding="utf-8").splitlines()
    header = lines[0].split("\t")
    cases = []
    for line in lines[1:]:
        row = dict(zip(header, line.split("\t")))
        row = {k: (None if v == "-" else v) for k, v in row.items()}
        row["n"] = int(row["n"])
        row["a"], row["b"] = decode_set(row["a"]), decode_set(row["b"])
        cases.append(row)
    return cases


OUT_FIELDS = ("name", "compiler", "config", "cache", "threshold", "kind",
              "where", "compiles")


def guesses(cases, weights_path):
    """Predicted thresholds, to start the searches near them."""
    cost = load("cost")
    weights, rows, names = cost.read_weights(weights_path)
    found = {}
    for case in cases:
        kind = case["kind"].removeprefix("baseline-")
        if case["kind"].startswith("baseline-"):
            continue
        ev = cost.events_of(case)
        phases = mirror.PHASES if kind == "plan" else (kind,)
        values = [cost.predict_phase(weights, rows, names, p, ev[p])[0]
                  for p in phases if p in weights]
        if values:
            found[case["name"]] = max(1, round(max(values)))
    return found


def run(cases_path, out_path, compiler, config, cache, jobs,
        weights_path=None):
    out_path = pathlib.Path(out_path).resolve()
    units = out_path.with_suffix(".units")
    units.mkdir(parents=True, exist_ok=True)
    done = set()
    if out_path.exists():
        for line in out_path.read_text(encoding="utf-8").splitlines()[1:]:
            done.add(line.split("\t")[0])
    else:
        out_path.write_text("\t".join(OUT_FIELDS) + "\n", encoding="utf-8")
    extra = ["-fconstexpr-cache-depth=1"] if (
        compiler == "gcc" and cache == "cold") else []
    pool = harness.Pool()
    cases = [c for c in read_cases(cases_path) if c["name"] not in done]
    if weights_path:
        for case in cases:
            case["guess"] = guesses([case], weights_path).get(case["name"])

    def one(case):
        source = units / f"{case['name']}.cpp"
        source.write_text(unit(case), encoding="utf-8")
        guess = int(case.get("guess") or 1 << 14)
        r = harness.threshold(pool, compiler, config, source,
                              f"{case['name']}:{cache}", guess, extra)
        if r["kind"] == "other" and r["threshold"] == "fails":
            sys.stderr.write(f"{case['name']} fails:\n{r['stderr'][:2000]}\n")
        return case, r

    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        for case, r in ex.map(one, cases):
            row = {"name": case["name"], "compiler": compiler,
                   "config": config, "cache": cache, **r}
            with open(out_path, "a", encoding="utf-8") as f:
                f.write("\t".join(str(row[k]) for k in OUT_FIELDS) + "\n")
            print(case["name"], r["threshold"], r["kind"], r["where"],
                  r["compiles"], flush=True)


def main(argv):
    if len(argv) >= 3 and argv[0] == "cases":
        which = argv[1]
        if which == "train":
            cases = phase_cases("train", random.Random(SEED), 40)
        elif which == "train-gp":
            cases = phase_cases("traingp", random.Random(SEED + 3), 60,
                                ("gp",), ("count", "fill"))
        elif which == "test":
            cases = phase_cases("test", random.Random(SEED + 2), 16)
        elif which == "plans":
            cases = plan_cases()
        elif which == "wide":
            cases = wide_cases()
        else:
            sys.exit(__doc__)
        write_cases(argv[2], cases)
        print(len(cases), "cases")
    elif len(argv) >= 6 and argv[0] == "run":
        run(argv[1], argv[2], argv[3], argv[4], argv[5],
            int(argv[6]) if len(argv) > 6 else 8,
            argv[7] if len(argv) > 7 else None)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
