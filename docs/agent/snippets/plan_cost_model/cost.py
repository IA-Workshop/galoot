#!/usr/bin/env -S python3 -I
"""Fit, test, and apply the cost model of the plan and map generators.
Snippet.

The model: the count of one constant evaluation is a weighted sum of the
event counts that mirror.py gives for its phase, plus a constant,
    cost(phase, input) = w0 + sum over events e of w_e * count_e(input),
with weights that depend on the phase, the compiler, its options, and (for
GCC) whether constant calls are cached, but not on the input. A unit's
threshold is the largest cost of its evaluations (harness.py).

Usage:
  cost.py fit <cases.tsv>:<thresholds.tsv>... <weights.tsv>
      Fits the weights of each phase from the phase probes of the files,
      exactly, over the rationals. Prints each phase's weights, its rank,
      and its largest residual. A phase probe whose threshold does not
      exceed its baseline's measures the baseline, not the phase, and is
      left out.
  cost.py check <cases.tsv> <thresholds.tsv> <weights.tsv>
      Predicts each case of the file from the weights alone and prints the
      prediction against the measured threshold: phase probes phase by
      phase, whole plans and maps as their largest phase. Marks a case
      whose events lie outside what the fit saw, where the prediction is an
      extrapolation.
  cost.py ceilings <weights.tsv> <limit> [largest n]
      For full<n> times full<n> under each product and form, and the map of
      full<n> under each map: T, R, the largest phase, its predicted
      threshold, and whether it is at most <limit>.
  cost.py predict <weights.tsv> <op> <form> <n> <a> [<b>]
      Predicts every phase of one plan, or of one map when op names a map
      and b is left out; a and b are "full", "grade:<k>", or lists
      mask:sign,...; form is r, rigid, conformal, dense, or gram=<rows
      separated by />, entries by .>.
  cost.py certify <weights.tsv> <limit> <op> <form> <n> <a> [<b>]
      Whether the plan or map is certified to evaluate under <limit>: every
      phase predicted, inside the span of the fit, and at most the limit,
      and, for GCC weights, no loop of the generator as long as GCC's
      per-loop limit. "not certified" does not mean that it fails: the
      certificate is a sufficient condition.
  cost.py figures <weights.tsv>
      The figures that the library's comments state and the thresholds of
      its reference units (FIGURES below), each with its largest phase and
      the next largest.
  cost.py comments <header> <figures.txt>...
      Checks that every count in the /// comments of the header is the
      threshold of a row of the figures files, one of the compilers'
      default limits, or an entry of NON_COST below (a size or flag value,
      each with the record it comes from); counts written as "<digits>
      million" are read too. Prints each that is not and exits 1 if there
      is one.
  cost.py selfcheck <weights.tsv> <figures.txt> --train <c>:<t>...
          --check <c>:<t>...
      Refits from the training files and compares every weight and
      training row with <weights.tsv>; predicts every measured case of
      every file and compares it with its threshold, a case with no
      measured threshold counting as a difference; and compares a fresh
      `figures` run with <figures.txt>. Prints each difference and exits 1
      if there is one.

For GCC, the threshold is the count plus one; for Clang it is the count
(harness.py). Costs here are thresholds, so a unit compiles under a limit L
exactly when every predicted threshold is at most L. On GCC the model is
fitted with its call cache off (-fconstexpr-cache-depth=1), so its
predictions are upper bounds of the counts with the cache on.

Written for research note 0017 and extended to the antiproducts and the
unary maps by plan 0015; last checked at the revision named in the snippets
index. Any limit search on GCC must be the checked search of harness.py: a
bisection alone can land on a limit that GCC evades and report a threshold
too low.
"""

import collections
import importlib.util
import re
import pathlib
import sys
from fractions import Fraction

HERE = pathlib.Path(__file__).resolve().parent


def load(name):
    spec = importlib.util.spec_from_file_location(name, HERE / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


mirror = load("mirror")
probe = load("probe")


def read_thresholds(path):
    lines = pathlib.Path(path).read_text(encoding="utf-8").splitlines()
    header = lines[0].split("\t")
    rows = {}
    for line in lines[1:]:
        row = dict(zip(header, line.split("\t")))
        rows[row["name"]] = row
    return rows


ALL_PHASES = mirror.PHASES + tuple(p for p in mirror.MAP_PHASES
                                   if p not in mirror.PHASES)


def events_of(case):
    """The events of every phase of the case's plan or map."""
    if probe.is_map(case):
        _, _, _, ev = mirror.map_phases(case["op"], case["n"], case["a"])
    else:
        _, _, _, ev = mirror.phases(case["op"], probe.gram_of(case),
                                    case["a"], case["b"])
    return ev


def phases_of(case):
    """The phases the model covers for the case's generator."""
    return mirror.MAP_PHASES if probe.is_map(case) else mirror.PHASES


def solve(rows, ys):
    """Least squares over the rationals by Gaussian elimination of the
    normal equations, with dependent columns given weight zero. Returns the
    weights and the indices of the independent columns."""
    k = len(rows[0])
    m = [[sum(Fraction(r[i]) * r[j] for r in rows) for j in range(k)] +
         [sum(Fraction(r[i]) * y for r, y in zip(rows, ys))]
         for i in range(k)]
    pivots = []
    row = 0
    for col in range(k):
        p = next((r for r in range(row, k) if m[r][col] != 0), None)
        if p is None:
            continue
        m[row], m[p] = m[p], m[row]
        for r in range(k):
            if r != row and m[r][col] != 0:
                f = m[r][col] / m[row][col]
                m[r] = [a - f * b for a, b in zip(m[r], m[row])]
        pivots.append(col)
        row += 1
    w = [Fraction(0)] * k
    for r, col in enumerate(pivots):
        w[col] = m[r][k] / m[r][col]
    return w, pivots


def in_span(basis_rows, vector):
    """Whether `vector` is a rational combination of `basis_rows`."""
    rows = [list(map(Fraction, r)) for r in basis_rows]
    v = list(map(Fraction, vector))
    # Reduce v by an echelon form of rows.
    echelon = []
    for r in rows:
        for e, col in echelon:
            if r[col] != 0:
                f = r[col] / e[col]
                r = [a - f * b for a, b in zip(r, e)]
        col = next((i for i, x in enumerate(r) if x != 0), None)
        if col is not None:
            echelon.append((r, col))
    for e, col in echelon:
        if v[col] != 0:
            f = v[col] / e[col]
            v = [a - f * b for a, b in zip(v, e)]
    return all(x == 0 for x in v)


def phase_of(case):
    return case["kind"]


def training_rows(cases, thresholds):
    """For each phase: (event vectors, thresholds), from probes that exceed
    their baselines."""
    data = collections.defaultdict(list)
    hidden = collections.Counter()
    for case in cases:
        kind = case["kind"]
        if kind in ("plan", "map") or kind.startswith("baseline-"):
            continue
        t = thresholds.get(case["name"])
        base = thresholds.get(case["name"].replace(f"-{kind}",
                                                   f"-baseline-{kind}"))
        if not t or not base or t["threshold"] == "fails":
            continue
        if int(t["threshold"]) <= int(base["threshold"]):
            hidden[kind] += 1
            continue
        data[kind].append((events_of(case)[kind], int(t["threshold"])))
    return data, hidden


def fit_lines(pairs):
    """The lines of a weights file fitted from (cases, thresholds) file
    pairs, and a report of each phase."""
    data = collections.defaultdict(list)
    hidden = collections.Counter()
    for cases_path, thresholds_path in pairs:
        part, part_hidden = training_rows(probe.read_cases(cases_path),
                                          read_thresholds(thresholds_path))
        for phase, samples in part.items():
            data[phase].extend(samples)
        hidden.update(part_hidden)
    lines = ["phase\tevent\tweight"]
    report = []
    for phase in ALL_PHASES:
        samples = data.get(phase, [])
        if not samples:
            report.append(f"{phase}: no samples ({hidden[phase]} hidden)")
            continue
        names = sorted({e for ev, _ in samples for e in ev})
        rows = [[1] + [ev.get(e, 0) for e in names] for ev, _ in samples]
        ys = [y for _, y in samples]
        w, pivots = solve(rows, ys)
        residuals = [y - sum(a * b for a, b in zip(r, w))
                     for r, y in zip(rows, ys)]
        worst = max(abs(r) for r in residuals)
        report.append(f"{phase}: {len(samples)} samples ({hidden[phase]} "
                      f"hidden), {len(names) + 1} columns, rank "
                      f"{len(pivots)}, largest residual {float(worst):.3f}")
        for i, name in enumerate(["const"] + names):
            if w[i] != 0:
                report.append(f"    {name:28} {w[i]}")
            lines.append(f"{phase}\t{name}\t{w[i]}")
        # The rows themselves, so that check can test the span.
        for r in rows:
            lines.append(f"{phase}\t#row\t" + ",".join(map(str, r)))
        lines.append(f"{phase}\t#names\t" + ",".join(names))
    return lines, report


def pair_list(texts):
    return [tuple(t.split(":", 1)) for t in texts]


def fit(pair_texts, weights_path):
    lines, report = fit_lines(pair_list(pair_texts))
    print("\n".join(report))
    pathlib.Path(weights_path).write_text("\n".join(lines) + "\n",
                                          encoding="utf-8")


def read_weights(path):
    weights = collections.defaultdict(dict)
    rows = collections.defaultdict(list)
    names = {}
    for line in pathlib.Path(path).read_text(encoding="utf-8").splitlines()[1:]:
        phase, event, value = line.split("\t")
        if event == "#row":
            rows[phase].append([Fraction(v) for v in value.split(",")])
        elif event == "#names":
            names[phase] = value.split(",")
        else:
            weights[phase][event] = Fraction(value)
    return weights, rows, names


def predict_phase(weights, rows, names, phase, ev):
    """The predicted threshold of one phase, and whether its events lie in
    the span of the training rows (False: an extrapolation, or an event the
    fit never saw)."""
    w = weights[phase]
    unseen = [e for e in ev if e not in w and ev[e]]
    value = w.get("const", 0) + sum(w.get(e, 0) * c for e, c in ev.items())
    vector = [1] + [ev.get(e, 0) for e in names[phase]]
    covered = not unseen and in_span(rows[phase], vector)
    return value, covered


def check(cases_path, thresholds_path, weights_path):
    cases = probe.read_cases(cases_path)
    thresholds = read_thresholds(thresholds_path)
    weights, rows, names = read_weights(weights_path)
    worst = collections.defaultdict(float)
    print("case\tphase\tmeasured\tpredicted\trelative error\tcovered\twhere")
    for case in cases:
        kind = case["kind"]
        if kind.startswith("baseline-"):
            continue
        t = thresholds.get(case["name"])
        if not t or t["threshold"] == "fails":
            continue
        measured = int(t["threshold"])
        ev = events_of(case)
        if kind in ("plan", "map"):
            preds = {p: predict_phase(weights, rows, names, p, ev[p])
                     for p in phases_of(case) if p in weights}
            phase = max(preds, key=lambda p: preds[p][0])
            value, covered = preds[phase]
        else:
            base = thresholds.get(case["name"].replace(
                f"-{kind}", f"-baseline-{kind}"))
            if base and measured <= int(base["threshold"]):
                continue
            phase = kind
            value, covered = predict_phase(weights, rows, names, kind, ev[kind])
        error = (measured - float(value)) / measured
        key = kind if kind in ("plan", "map") else phase
        worst[key] = max(worst[key], abs(error))
        print(f"{case['name']}\t{phase}\t{measured}\t{float(value):.0f}\t"
              f"{error:+.5f}\t{'yes' if covered else 'NO'}\t{t['where']}")
    for key, value in sorted(worst.items()):
        print(f"# largest relative error, {key}: {value:.5f}")


def parse_set(text, n):
    if text == "full":
        return mirror.full(n)
    if text.startswith("grade:"):
        k = int(text[6:])
        return [(m, s) for m, s in mirror.full(n) if mirror.grade(m) == k]
    return probe.decode_set(text)


def make_case(op, form_name, n, a_text, b_text=None):
    n = int(n)
    return {"op": op, "form": form_name, "n": n, "a": parse_set(a_text, n),
            "b": [] if b_text is None else parse_set(b_text, n),
            "kind": "map" if op in mirror.UNARY else "plan"}


def sizes_of(case):
    """The number of terms and of result blades."""
    if probe.is_map(case):
        terms, result, _, _ = mirror.map_phases(case["op"], case["n"],
                                                case["a"])
    else:
        terms, result, _, _ = mirror.phases(case["op"], probe.gram_of(case),
                                            case["a"], case["b"])
    return len(terms), len(result)


def predictions(weights, rows, names, case):
    """(phase, predicted threshold, covered) for each phase, largest
    first."""
    ev = events_of(case)
    found = [(p, *predict_phase(weights, rows, names, p, ev[p]))
             for p in phases_of(case) if p in weights]
    return sorted(found, key=lambda x: -x[1])


def predict(weights_path, op, form_name, n, a_text, b_text=None):
    weights, rows, names = read_weights(weights_path)
    case = make_case(op, form_name, n, a_text, b_text)
    t, r = sizes_of(case)
    print(f"T = {t} terms, R = {r} result blades")
    for phase, value, covered in predictions(weights, rows, names, case):
        print(f"{phase:12} {float(value):16.0f}"
              f"{'' if covered else '  (extrapolated)'}")


def ceilings(weights_path, limit, largest=8):
    weights, rows, names = read_weights(weights_path)
    limit = int(limit)
    print(f"limit {limit}; phases predicted from {weights_path}")
    print("op\tform\tn\tT\tR\tlargest phase\tpredicted\tunder the limit")
    grid = [(op, f, n) for op in probe.PRODUCTS
            for f in ("r", "rigid", "conformal", "dense")
            for n in range(3, int(largest) + 1)]
    grid += [(op, "r", n) for op in mirror.UNARY
             for n in range(3, int(largest) + 3)]
    for op, form_name, n in grid:
        b = None if op in mirror.UNARY else "full"
        case = make_case(op, form_name, n, "full", b)
        if probe.is_map(case) or form_name != "dense" or n <= 6:
            t, r = sizes_of(case)
            phase, value, covered = predictions(weights, rows, names, case)[0]
            print(f"{op}\t{form_name}\t{n}\t{t}\t{r}\t{phase}"
                  f"{'' if covered else ' (x)'}\t{float(value):.0f}\t"
                  f"{'yes' if value <= limit else 'no'}", flush=True)


# GCC refuses a loop of this many iterations (-fconstexpr-loop-limit); the
# generators' longest loops run once per term or per blade of a set.
GCC_LOOP_LIMIT = 1 << 18


def certify(weights_path, limit, op, form_name, n, a_text, b_text=None):
    weights, rows, names = read_weights(weights_path)
    limit = int(limit)
    case = make_case(op, form_name, n, a_text, b_text)
    t, r = sizes_of(case)
    reasons = []
    found = predictions(weights, rows, names, case)
    for phase, value, covered in found:
        if not covered:
            reasons.append(f"{phase} is outside what the fit saw")
        if value > limit:
            reasons.append(f"{phase} predicted {float(value):.0f} > {limit}")
    longest = max(t, len(case["a"]), len(case["b"]))
    if "gcc" in pathlib.Path(weights_path).name and longest >= GCC_LOOP_LIMIT:
        reasons.append(f"a loop of {longest} iterations reaches GCC's loop "
                       "limit")
    print(f"T = {t}, R = {r}, largest phase {found[0][0]} predicted "
          f"{float(found[0][1]):.0f} of {limit}")
    print("certified" if not reasons else "not certified: " +
          "; ".join(reasons))
    return not reasons


def sparse_sets(n):
    """{1, e1, e_n, e1n} and {e1, e2, e_n}, as test/core writes them."""
    en = 1 << (n - 1)
    return ([(0, 1), (1, 1), (en, 1), (en | 1, 1)],
            [(1, 1), (2, 1), (en, 1)])


def figure_cases():
    """The cases of FIGURES as (label, case)."""
    out = []
    for label, op, form_name, n, a_text, b_text in FIGURES:
        if a_text == "sparse":
            a, b = sparse_sets(n)
            case = {"op": op, "form": form_name, "n": n, "a": a, "b": b,
                    "kind": "plan"}
        else:
            case = make_case(op, form_name, n, a_text, b_text)
        out.append((label, case))
    return out


# The figures stated in the comments of product_plan and unary_map and the
# reference units of the canary test: (label, op, form, n, a, b).
FIGURES = [
    ("gp-r-5", "gp", "r", 5, "full", "full"),
    ("gp-r-6", "gp", "r", 6, "full", "full"),
    ("gp-r-7", "gp", "r", 7, "full", "full"),
    ("gp-dense-4", "gp", "dense", 4, "full", "full"),
    ("gp-dense-5", "gp", "dense", 5, "full", "full"),
    ("wedge-r-5", "wedge", "r", 5, "full", "full"),
    ("wedge-r-6", "wedge", "r", 6, "full", "full"),
    ("wedge-r-7", "wedge", "r", 7, "full", "full"),
    ("antigp-r-5", "antigp", "r", 5, "full", "full"),
    ("antigp-r-6", "antigp", "r", 6, "full", "full"),
    ("antigp-conformal-5", "antigp", "conformal", 5, "full", "full"),
    ("antigp-dense-4", "antigp", "dense", 4, "full", "full"),
    ("antigp-dense-5", "antigp", "dense", 5, "full", "full"),
    ("antiwedge-r-5", "antiwedge", "r", 5, "full", "full"),
    ("antiwedge-r-6", "antiwedge", "r", 6, "full", "full"),
    ("antiwedge-r-7", "antiwedge", "r", 7, "full", "full"),
    ("gp-grades-3-7", "gp", "r", 8, "grade:3", "grade:7"),
    ("gp-vectors-full-6", "gp", "r", 6, "grade:1", "full"),
    ("sparse-gp-4", "gp", "rigid", 4, "sparse", None),
    ("sparse-gp-16", "gp", "rigid", 16, "sparse", None),
    ("sparse-antigp-4", "antigp", "rigid", 4, "sparse", None),
    ("sparse-antigp-16", "antigp", "rigid", 16, "sparse", None),
    ("reverse-7", "reverse", "r", 7, "full", None),
    ("reverse-8", "reverse", "r", 8, "full", None),
    ("reverse-9", "reverse", "r", 9, "full", None),
    ("reverse-10", "reverse", "r", 10, "full", None),
    ("right_complement-8", "right_complement", "r", 8, "full", None),
    ("right_complement-9", "right_complement", "r", 9, "full", None),
    ("right_complement-10", "right_complement", "r", 10, "full", None),
]


def figures_text(weights_path):
    weights, rows, names = read_weights(weights_path)
    lines = [f"# Predicted thresholds from {pathlib.Path(weights_path).name}",
             "label\tT\tR\tlargest phase\tthreshold\tnext phase\t"
             "next threshold\tcovered"]
    for label, case in figure_cases():
        t, r = sizes_of(case)
        found = predictions(weights, rows, names, case)
        (p1, v1, _), (p2, v2, _) = found[0], found[1]
        covered = all(c for _, _, c in found)
        lines.append(f"{label}\t{t}\t{r}\t{p1}\t{v1}\t{p2}\t{v2}\t"
                     f"{'yes' if covered else 'NO'}")
    return "\n".join(lines) + "\n"


# Numbers the comments state that are limits, not counts.
LIMITS = (1 << 20, 1 << 25, GCC_LOOP_LIMIT)

# Counts the comments state that are sizes or flag values, not costs of the
# model, each with the record it comes from.
SCALAR_FULL10 = "records/research/2026-11-09-scalar-full10-gcc.md"
NON_COST = (
    (1024, SCALAR_FULL10),
    (4096, SCALAR_FULL10),
    (134217728, SCALAR_FULL10),
    (89_000_000, SCALAR_FULL10),
)

COUNT = re.compile(r"\b\d{1,3}(?:,\d{3})+\b|\b\d{4,}\b|\b\d+ million\b")


def count_value(text):
    """The integer a count in a comment states: "1,024" is 1024 and
    "89 million" is 89000000."""
    if text.endswith(" million"):
        return int(text.split()[0]) * 1_000_000
    return int(text.replace(",", ""))


def listed_counts(non_cost):
    """The values of the list `non_cost` of (count, source) entries; an entry
    without a source is refused."""
    values = set()
    for entry in non_cost:
        value, source = entry
        if not isinstance(source, str) or not source.strip():
            raise ValueError(f"non-cost count {value} has no source record")
        values.add(value)
    return values


def comments(header_path, figures_paths, non_cost=NON_COST):
    """Checks that every count in the /// comments of `header_path` is the
    threshold of some row of the figures files, a limit, or a count of the
    list `non_cost`."""
    known = set(LIMITS) | listed_counts(non_cost)
    for path in figures_paths:
        for line in pathlib.Path(path).read_text(
                encoding="utf-8").splitlines()[2:]:
            known.add(int(line.split("\t")[4]))
    unknown = []
    for number, line in enumerate(pathlib.Path(header_path).read_text(
            encoding="utf-8").splitlines(), 1):
        if not line.lstrip().startswith("///"):
            continue
        for text in COUNT.findall(line):
            if count_value(text) not in known:
                unknown.append(f"{header_path}:{number}: {text}")
    for u in unknown:
        print(u)
    print(f"{len(unknown)} counts in the comments are not in the figures")
    return not unknown


def selfcheck(weights_path, figures_path, train, measured):
    problems = []

    # The fit: the committed weights are what the training data give.
    lines, _ = fit_lines(train)
    committed = pathlib.Path(weights_path).read_text(
        encoding="utf-8").splitlines()
    if lines != committed:
        a, b = set(lines), set(committed)
        for line in sorted(a - b)[:10]:
            problems.append(f"refit gives {line!r}, not in {weights_path}")
        for line in sorted(b - a)[:10]:
            problems.append(f"{weights_path} has {line!r}, not in the refit")
        if a == b:
            problems.append(f"{weights_path} lists its lines in another order")

    # The predictions: every measured case, exactly.
    weights, rows, names = read_weights(weights_path)
    checked = algebra_checks = 0
    for cases_path, thresholds_path in train + measured:
        thresholds = read_thresholds(thresholds_path)
        for case in probe.read_cases(cases_path):
            kind = case["kind"]
            t = thresholds.get(case["name"])
            # Every case of a committed case file was measured, baselines
            # too, so a missing or failed measurement is a difference.
            if not t or t["threshold"] == "fails":
                problems.append(f"{case['name']}: no measured threshold in "
                                f"{thresholds_path}")
                continue
            if kind.startswith("baseline-"):
                continue
            measured_value = int(t["threshold"])
            if kind in ("plan", "map"):
                _, value, _ = predictions(weights, rows, names, case)[0]
                # For small sets in a large algebra, the unit's largest
                # evaluation can be the check of the algebra's metric, which
                # the model does not cover; the compiler then stops in
                # metric.hpp.
                if measured_value > value and "metric.hpp" in t["where"]:
                    algebra_checks += 1
                    continue
            else:
                base = thresholds.get(case["name"].replace(
                    f"-{kind}", f"-baseline-{kind}"))
                if base and measured_value <= int(base["threshold"]):
                    continue
                value, _ = predict_phase(weights, rows, names, kind,
                                         events_of(case)[kind])
            checked += 1
            if value != measured_value:
                problems.append(f"{case['name']}: predicted {value}, "
                                f"measured {measured_value}")

    # The figures.
    fresh = figures_text(weights_path)
    committed_figures = pathlib.Path(figures_path).read_text(encoding="utf-8")
    if fresh != committed_figures:
        for new, old in zip(fresh.splitlines(), committed_figures.splitlines()):
            if new != old:
                problems.append(f"figures: {old!r} is now {new!r}")
        if len(fresh.splitlines()) != len(committed_figures.splitlines()):
            problems.append("figures: the number of lines differs")

    for p in problems:
        print(p)
    print(f"{checked} measured cases predicted; {algebra_checks} whole units "
          "whose largest evaluation is the metric's check, not compared; "
          f"{len(problems)} differences")
    return not problems


def split_selfcheck(args):
    train, measured, target = [], [], None
    for a in args:
        if a == "--train":
            target = train
        elif a == "--check":
            target = measured
        elif target is None:
            sys.exit(__doc__)
        else:
            target.append(tuple(a.split(":", 1)))
    return train, measured


def main(argv):
    if len(argv) >= 3 and argv[0] == "fit":
        fit(argv[1:-1], argv[-1])
    elif len(argv) == 4 and argv[0] == "check":
        check(*argv[1:])
    elif len(argv) in (3, 4) and argv[0] == "ceilings":
        ceilings(*argv[1:])
    elif len(argv) in (7, 8) and argv[0] == "certify":
        sys.exit(0 if certify(*argv[1:]) else 1)
    elif len(argv) in (6, 7) and argv[0] == "predict":
        predict(*argv[1:])
    elif len(argv) >= 3 and argv[0] == "comments":
        sys.exit(0 if comments(argv[1], argv[2:]) else 1)
    elif len(argv) == 2 and argv[0] == "figures":
        sys.stdout.write(figures_text(argv[1]))
    elif len(argv) >= 5 and argv[0] == "selfcheck":
        train, measured = split_selfcheck(argv[3:])
        sys.exit(0 if selfcheck(argv[1], argv[2], train, measured) else 1)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
