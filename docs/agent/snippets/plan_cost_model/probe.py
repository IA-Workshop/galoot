#!/usr/bin/env -S python3 -I
"""Measure the constant-evaluation cost of each phase of the plan and map
generators, and of whole plans and maps, as the smallest limit under which
a unit compiles. Snippet.

Usage, from the repository root, with the job pool exported
(eval "$(tools/jobpool env)"):

  probe.py cases <set> <cases.tsv>
      Writes the cases of a set (seeds from 20261102 for the random ones):
        train       probes of every phase of a plan, all four products
        train-gen   more probes of count and fill, all four products
        train-map   probes of the phases of a map
        test        probes of every phase of plans and maps that no fit sees
        plans       whole plans of full<n>, every product and form, and
                    random sets
        maps        whole maps of full<n>, every map, and random sets
        wide        whole plans of the sets programs write, n up to 16
  probe.py verify <cases.tsv> <out.tsv> <compiler> [jobs]
      Compiles each case once with a static_assert that the C++ phase
      result equals the mirror's (a whole plan or map: only those of at most
      2,000 terms), and writes one row per case: ok, or the compiler's error.
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
constant evaluation, as make_product_plan and make_unary_map do; its
baseline is the same unit without that evaluation, measured too, so that a
phase cheaper than the rest of the unit is reported as hidden rather than
mistaken for its cost. A plan case reads product_plan, which runs every
phase, and a map case reads unary_map.

The threshold counts what the compiler counts in the largest evaluation of
the unit (harness.py). Every compile is -fsyntax-only: constant evaluation
happens in the front end.

Written for research note 0017 and refitted by plan 0015; last checked at
the revision named in the snippets index.
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
SEED_2 = 20261102
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
    """The case's form as mirror.Form: from_gram writes every entry."""
    if case["form"].startswith("gram="):
        rows = case["form"][5:].split("/")
        return mirror.Form([[int(v) for v in row.split(".")] for row in rows])
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
                     for row in gram_of(case).values)
    return f"core::from_gram({{{rows}}})"


def set_expr(blades):
    inner = ", ".join(f"{{{m}U, {s}}}" for m, s in blades)
    return f"core::blade_set<{len(blades)}>{{{{{{{inner}}}}}}}"


def is_map(case):
    return case["op"] in mirror.UNARY


# The cases and the mirror keep the short names; plan.hpp spells two of the
# products out.
ENUMERATOR = {"gp": "geometric_product", "antigp": "geometric_antiproduct"}


def op_expr(op):
    if op in mirror.UNARY:
        return f"d::UnaryOperation::{op}"
    return f"d::BinaryOperation::{ENUMERATOR.get(op, op)}"


def plan_array(plan):
    inner = ",\n    ".join(f"d::plan_term{{{a}, {b}, {m}U, {c}}}"
                           for a, b, m, c in plan)
    return f"std::array<d::plan_term, {len(plan)}>{{{{\n    {inner}}}}}"


def plan_literal(plan):
    return f"[[maybe_unused]] constexpr auto lit = {plan_array(plan)};\n"


def blades_literal(masks):
    return ", ".join(f"{{{m}U, 1}}" for m in masks)


def typed_literal(result, typed):
    R, T = len(result), len(typed)
    terms = ", ".join(f"d::typed_term{{{a_}, {b_}, {o}, {c}}}"
                      for a_, b_, o, c in typed)
    return (f"d::typed_plan<{R}, {T}>{{"
            f"core::blade_set<{R}>{{{{{{{blades_literal(result)}}}}}}}, "
            f"std::array<d::typed_term, {T}>{{{{{terms}}}}}}}")


def map_literal(result, typed):
    R, K = len(result), len(typed)
    terms = ", ".join(f"d::map_term{{{a_}, {o}, {c}}}"
                      for a_, _, o, c in typed)
    return (f"d::typed_map<{R}, {K}>{{"
            f"core::blade_set<{R}>{{{{{{{blades_literal(result)}}}}}}}, "
            f"std::array<d::map_term, {K}>{{{{{terms}}}}}}}")


def found_literal(result, T):
    return (f"d::collected_result<{T}>{{{{{{{blades_literal(result)}}}}}, "
            f"{len(result)}}}")


def none_found(fields):
    return " && ".join(f"!probe.{f}" for f in fields)


PLAN_DEFECTS = ("zero_coefficient", "beyond_first", "beyond_second",
                "beyond_n", "out_of_order")
TYPED_DEFECTS = ("result_out_of_order", "result_sign", "beyond_result",
                 "result_unused", "out_of_order")
MAP_DEFECTS = ("result_out_of_order", "result_sign", "beyond_result",
               "result_unused", "not_per_input", "wrong_blade",
               "complement_sign")
VERIFY_LARGEST = 2000


def unit(case, verify=False):
    """The source of a case. With verify, the unit also asserts that the
    C++ result equals the mirror's, or is None for a whole plan or map too
    large to write out."""
    op, n = case["op"], case["n"]
    a, b = case["a"], case["b"]
    text = PREAMBLE + ("struct alg { static constexpr auto value = "
                       f"{metric_expr(case)}; }};\n")
    kind = case["kind"]
    if kind in ("plan", "map"):
        first = case.get("a_expr") or set_expr(a)
        if kind == "plan":
            second = case.get("b_expr") or set_expr(b)
            read = (f"d::product_plan<{op_expr(op)}, alg, {first}, "
                    f"{second}>")
        else:
            read = f"d::unary_map<{op_expr(op)}, alg, {first}>"
        text += f"static_assert({read}.terms.size() > 0);\n"
        if verify:
            if kind == "plan":
                _, result, typed, _ = mirror.phases(op, gram_of(case), a, b)
                expected = typed_literal(result, typed)
            else:
                _, result, typed, _ = mirror.map_phases(op, n, a)
                expected = map_literal(result, typed)
            if len(typed) > VERIFY_LARGEST:
                return None
            text += f"static_assert({read} == {expected});\n"
        return text

    phase = kind.removeprefix("baseline-")
    probe = not kind.startswith("baseline-")
    text += f"[[maybe_unused]] constexpr auto A = {set_expr(a)};\n"
    if is_map(case):
        terms, result, typed, _ = mirror.map_phases(op, n, a)
        K, R = len(terms), len(result)
        if phase == "map_fill":
            line = f"d::filled_map<{K}>({op_expr(op)}, {n}, A.blades)"
            check = f"probe == {plan_array(terms)}"
        else:
            text += plan_literal(terms)
            if phase == "collect":
                line = "d::collected(lit)"
                check = (f"probe.size == {R} && probe.blades == "
                         f"{found_literal(result, K)}.blades")
            elif phase == "map_typed":
                text += (f"[[maybe_unused]] constexpr auto found = "
                         f"{found_literal(result, K)};\n")
                line = f"d::typed_map_of<{R}>(lit, found)"
                check = f"probe == {map_literal(result, typed)}"
            elif phase == "map_defects":
                text += (f"[[maybe_unused]] constexpr auto mapped = "
                         f"{map_literal(result, typed)};\n")
                line = f"d::map_defects_of({op_expr(op)}, {n}, A, mapped)"
                check = none_found(MAP_DEFECTS)
            else:
                raise ValueError(phase)
    else:
        plan, result, typed, _ = mirror.phases(op, gram_of(case), a, b)
        capacity = mirror.plan_capacity(op, a, b, n,
                                        mirror.collections.Counter())
        R, T = len(result), len(plan)
        text += f"[[maybe_unused]] constexpr auto B = {set_expr(b)};\n"
        if phase == "capacity":
            line = f"d::plan_capacity({op_expr(op)}, A.blades, B.blades, {n})"
            check = f"probe == {capacity}"
        elif phase == "count":
            line = (f"d::generate_plan<{op_expr(op)}, {capacity}>"
                    "(alg::value, A.blades, B.blades, {})")
            check = f"probe == {T}"
        elif phase == "fill":
            line = (f"d::filled_plan<{op_expr(op)}, {T}, {capacity}>"
                    "(alg::value, A.blades, B.blades)")
            check = f"probe == {plan_array(plan)}"
        else:
            text += plan_literal(plan)
            if phase == "defects":
                line = f"d::defects_of(lit, {len(a)}, {len(b)}, {n})"
                check = none_found(PLAN_DEFECTS)
            elif phase == "collect":
                line = "d::collected(lit)"
                check = (f"probe.size == {R} && probe.blades == "
                         f"{found_literal(result, T)}.blades")
            elif phase == "typed":
                text += (f"[[maybe_unused]] constexpr auto found = "
                         f"{found_literal(result, T)};\n")
                line = f"d::typed<{R}>(lit, found)"
                check = f"probe == {typed_literal(result, typed)}"
            elif phase == "typing":
                text += (f"[[maybe_unused]] constexpr auto typed_lit = "
                         f"{typed_literal(result, typed)};\n")
                line = "d::typed_defects_of(typed_lit.result, typed_lit.terms)"
                check = none_found(TYPED_DEFECTS)
            else:
                raise ValueError(phase)
    if probe:
        text += f"[[maybe_unused]] constexpr auto probe = {line};\n"
        if verify:
            text += f"static_assert({check});\n"
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


PRODUCTS = ("gp", "wedge", "antigp", "antiwedge")


def phase_cases(prefix, rng, count, ops=PRODUCTS, phases=PHASE_PROBES,
                sizes=(3, 6), largest_events=None):
    """Random inputs for the phases of a plan: forms, sizes, and
    operations. With largest_events, an input whose generation takes more
    of the mirror's events than that is left out, so that no probe costs
    minutes a compile."""
    cases = []
    for i in range(count):
        n = rng.randint(*sizes)
        op = rng.choice(ops)
        f = rng.choice(("r", "rigid", "conformal", "dense", "random"))
        f = random_gram(rng, n, rng.choice((0.2, 0.5))) if f == "random" else f
        top = 1 << n
        ka = rng.randint(2, min(top, 24))
        kb = rng.randint(2, min(top, 24))
        a, b = random_set(rng, n, ka), random_set(rng, n, kb)
        base = {"op": op, "form": f, "n": n, "a": a, "b": b}
        plan, _, _, ev = mirror.phases(op, gram_of(base), a, b)
        if not plan:
            continue
        if largest_events and sum(ev["count"].values()) > largest_events:
            continue
        for phase in phases:
            for kind in (phase, f"baseline-{phase}"):
                cases.append({**base, "name": f"{prefix}{i}-{kind}",
                              "kind": kind})
    return cases


def map_phase_cases(prefix, rng, count, phases=mirror.MAP_PHASES):
    """Random inputs for the phases of a map: the operation, n, and a set of
    distinct masks with random signs, sometimes in canonical order."""
    cases = []
    for i in range(count):
        n = rng.randint(2, 7)
        op = rng.choice(mirror.UNARY)
        a = random_set(rng, n, rng.randint(2, min(1 << n, 40)))
        if rng.random() < 0.3:
            a.sort(key=lambda blade: mirror.canonical_key(blade[0]))
        base = {"op": op, "form": "r", "n": n, "a": a, "b": []}
        for phase in phases:
            for kind in (phase, f"baseline-{phase}"):
                cases.append({**base, "name": f"{prefix}{i}-{kind}",
                              "kind": kind})
    return cases


def full_case(name, kind, op, f, n, b=True):
    s = mirror.full(n)
    case = {"name": name, "kind": kind, "op": op, "form": f, "n": n, "a": s,
            "b": s if b else [], "a_expr": f"core::full<{n}>"}
    if b:
        case["b_expr"] = f"core::full<{n}>"
    return case


def plan_cases():
    """Whole plans: full<n> times full<n> for every product and form, and
    random sets."""
    cases = []
    for op in PRODUCTS:
        for f in ("r", "rigid", "conformal", "dense"):
            for n in range(3, 8):
                if f == "dense" and mirror.BASE[op] == "gp" and n > 6:
                    continue
                cases.append(full_case(f"full-{op}-{f}-n{n}", "plan", op, f,
                                       n))
    rng = random.Random(SEED_2 + 1)
    for i in range(24):
        n = rng.randint(4, 9)
        op = rng.choice(PRODUCTS)
        f = rng.choice(("r", "rigid", "conformal", "dense", "random"))
        f = random_gram(rng, n, 0.3) if f == "random" else f
        top = 1 << n
        a = random_set(rng, n, rng.randint(4, min(top, 96)))
        b = random_set(rng, n, rng.randint(4, min(top, 96)))
        if mirror.phases(op, gram_of({"form": f, "n": n}), a, b)[0]:
            cases.append({"name": f"random-{i}", "kind": "plan", "op": op,
                          "form": f, "n": n, "a": a, "b": b})
    return cases


def map_cases():
    """Whole maps: every map of full<n>, n = 3 to 10, and random sets."""
    cases = [full_case(f"map-{op}-n{n}", "map", op, "r", n, b=False)
             for op in mirror.UNARY for n in range(3, 11)]
    rng = random.Random(SEED_2 + 7)
    for i in range(12):
        n = rng.randint(4, 12)
        op = rng.choice(mirror.UNARY)
        a = random_set(rng, n, rng.randint(4, min(1 << n, 300)))
        cases.append({"name": f"randommap-{i}", "kind": "map", "op": op,
                      "form": "r", "n": n, "a": a, "b": []})
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


def right_complements(blades, n):
    """The right complements of oriented blades: s e_A to s right(e_A)."""
    out = []
    for mask, sign in blades:
        c = mirror.complement(mask, n)
        out.append((c, sign * mirror.reorder_sign(mask, c,
                                                  mirror.collections.Counter(),
                                                  "x")))
    return out


def wide_cases():
    """Plans of the sets programs write, at n up to 16: the rigid(3) pairs
    embedded with e4 as e_n, the sparse sets and their right complements,
    and random small sets."""
    cases = []
    pairs = [("point", "point"), ("line", "plane"), ("motor", "point"),
             ("point", "motor"), ("line", "line"), ("motor", "motor"),
             ("plane", "plane")]
    for n in (4, 8, 12, 16):
        en = 1 << (n - 1)
        sparse_a = [(0, 1), (1, 1), (en, 1), (en | 1, 1)]
        sparse_b = [(1, 1), (2, 1), (en, 1)]
        for op in PRODUCTS:
            for x, y in pairs:
                a, b = rigid_set(RIGID_NAMES[x], n), rigid_set(RIGID_NAMES[y], n)
                plan, _, _, _ = mirror.phases(op, mirror.form("rigid", n), a, b)
                if plan:
                    cases.append({"name": f"rigid-{op}-{x}-{y}-n{n}",
                                  "kind": "plan", "op": op, "form": "rigid",
                                  "n": n, "a": a, "b": b})
            for label, a, b in (
                    ("sparse", sparse_a, sparse_b),
                    ("cosparse", right_complements(sparse_a, n),
                     right_complements(sparse_b, n))):
                plan, _, _, _ = mirror.phases(op, mirror.form("rigid", n), a, b)
                if plan:
                    cases.append({"name": f"{label}-{op}-n{n}", "kind": "plan",
                                  "op": op, "form": "rigid", "n": n, "a": a,
                                  "b": b})
    rng = random.Random(SEED_2 + 4)
    for i in range(16):
        n = rng.randint(10, 16)
        op = rng.choice(PRODUCTS)
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
        row["a"] = decode_set(row["a"] or "")
        row["b"] = decode_set(row["b"] or "")
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
        phases = cost.phases_of(case) if kind in ("plan", "map") else (kind,)
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


def verify(cases_path, out_path, compiler, jobs):
    """Compiles each case once, with no limit to reach, and the
    static_asserts of unit(case, verify=True)."""
    out_path = pathlib.Path(out_path).resolve()
    units = out_path.with_suffix(".units")
    units.mkdir(parents=True, exist_ok=True)
    pool = harness.Pool()
    cases = [c for c in read_cases(cases_path)
             if not c["kind"].startswith("baseline-")]
    limit = [f"{harness.LIMIT_FLAG[compiler]}{harness.CEILING[compiler]}"]

    def one(case):
        text = unit(case, verify=True)
        if text is None:
            return case, "skipped (too large to write out)"
        source = units / f"{case['name']}.cpp"
        source.write_text(text, encoding="utf-8")
        status, err = harness.compile_once(
            pool, compiler, "galoot-O0", source,
            f"cost-model-verify:{case['name']}:{compiler}", limit)
        if status == 0:
            return case, "ok"
        return case, "FAILS: " + " ".join(err.split())[:300]

    failed = 0
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("name\tcompiler\tresult\n")
        with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
            for case, result in ex.map(one, cases):
                failed += result.startswith("FAILS")
                f.write(f"{case['name']}\t{compiler}\t{result}\n")
    print(f"{len(cases)} cases, {failed} where the C++ and the mirror differ")
    return failed == 0


def main(argv):
    if len(argv) >= 3 and argv[0] == "cases":
        which = argv[1]
        if which == "train":
            cases = phase_cases("train", random.Random(SEED_2), 60)
        elif which == "train-gen":
            cases = phase_cases("traingen", random.Random(SEED_2 + 3), 60,
                                phases=("count", "fill"), sizes=(3, 8),
                                largest_events=1_000_000)
        elif which == "train-map":
            cases = map_phase_cases("trainmap", random.Random(SEED_2 + 5), 60)
        elif which == "test":
            cases = (phase_cases("test", random.Random(SEED_2 + 2), 20) +
                     map_phase_cases("testmap", random.Random(SEED_2 + 6), 16))
        elif which == "plans":
            cases = plan_cases()
        elif which == "maps":
            cases = map_cases()
        elif which == "wide":
            cases = wide_cases()
        else:
            sys.exit(__doc__)
        write_cases(argv[2], cases)
        print(len(cases), "cases")
    elif len(argv) >= 4 and argv[0] == "verify":
        ok = verify(argv[1], argv[2], argv[3],
                    int(argv[4]) if len(argv) > 4 else 8)
        sys.exit(0 if ok else 1)
    elif len(argv) >= 6 and argv[0] == "run":
        run(argv[1], argv[2], argv[3], argv[4], argv[5],
            int(argv[6]) if len(argv) > 6 else 8,
            argv[7] if len(argv) > 7 else None)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
