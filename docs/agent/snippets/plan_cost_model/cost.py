#!/usr/bin/env -S python3 -I
"""Fit and test the cost model of the product-plan generator.

The model: the count of one constant evaluation is a weighted sum of the
event counts that mirror.py gives for its phase, plus a constant,
    cost(phase, input) = w0 + sum over events e of w_e * count_e(input),
with weights that depend on the phase, the compiler, its options, and (for
GCC) whether constant calls are cached, but not on the input. A unit's
threshold is the largest cost of its evaluations (harness.py).

Usage:
  cost.py fit <cases.tsv> <thresholds.tsv> <weights.tsv>
      Fits the weights of each phase from phase probes, exactly, over the
      rationals. Prints each phase's weights, its rank, and its largest
      residual. A phase probe whose threshold does not exceed its
      baseline's measures the baseline, not the phase, and is left out.
  cost.py check <cases.tsv> <thresholds.tsv> <weights.tsv>
      Predicts each case of the file from the weights alone and prints the
      prediction against the measured threshold: phase probes phase by
      phase, plan cases as the largest phase. Marks a case whose events lie
      outside what the fit saw, where the prediction is an extrapolation.
  cost.py ceilings <weights.tsv> <limit> [largest n]
      For full<n> times full<n> in each form and operation: the predicted
      threshold of each phase, the largest, and whether it is at most
      <limit>; then the same with the two typing phases replaced by a
      binary search (estimated, see binary_search_estimate).
  cost.py predict <weights.tsv> <op> <form> <n> <a> <b>
      Predicts the cost of every phase of one plan; a and b are "full",
      "grade:<k>", or lists mask:sign,...; form is r, rigid, conformal,
      dense, or gram=<rows separated by />, entries by .>.
  cost.py certify <weights.tsv> <limit> <relative> <absolute> <op> <form>
          <n> <a> <b>
      Whether the plan is certified to evaluate under <limit>: every phase
      predicted, inside the span of the fit, and at most
      limit / (1 + relative) - absolute, and no loop of the generator longer
      than GCC's per-loop limit. "not certified" does not mean that the plan
      fails: the certificate is a sufficient condition.

For GCC, the threshold is the count plus one; for Clang it is the count
(harness.py). Costs here are thresholds, so a unit compiles under a limit L
exactly when every predicted threshold is at most L.
"""

import collections
import importlib.util
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


def events_of(case):
    _, _, _, ev = mirror.phases(case["op"], probe.gram_of(case), case["a"],
                                case["b"])
    return ev


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
        if kind == "plan" or kind.startswith("baseline-"):
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


def fit(cases_path, thresholds_path, weights_path):
    cases = probe.read_cases(cases_path)
    thresholds = read_thresholds(thresholds_path)
    data, hidden = training_rows(cases, thresholds)
    lines = ["phase\tevent\tweight"]
    for phase in mirror.PHASES:
        samples = data.get(phase, [])
        if not samples:
            print(f"{phase}: no samples ({hidden[phase]} hidden)")
            continue
        names = sorted({e for ev, _ in samples for e in ev})
        rows = [[1] + [ev.get(e, 0) for e in names] for ev, _ in samples]
        ys = [y for _, y in samples]
        w, pivots = solve(rows, ys)
        residuals = [y - sum(a * b for a, b in zip(r, w))
                     for r, y in zip(rows, ys)]
        worst = max(abs(r) for r in residuals)
        print(f"{phase}: {len(samples)} samples ({hidden[phase]} hidden), "
              f"{len(names) + 1} columns, rank {len(pivots)}, largest "
              f"residual {float(worst):.3f}")
        for i, name in enumerate(["const"] + names):
            if w[i] != 0:
                print(f"    {name:28} {w[i]}")
            lines.append(f"{phase}\t{name}\t{w[i]}")
        # The rows themselves, so that check can test the span.
        for r in rows:
            lines.append(f"{phase}\t#row\t" + ",".join(map(str, r)))
        lines.append(f"{phase}\t#names\t" + ",".join(names))
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
        if kind == "plan":
            preds = {p: predict_phase(weights, rows, names, p, ev[p])
                     for p in mirror.PHASES if p in weights}
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
        key = "plan" if kind == "plan" else phase
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


def predict(weights_path, op, form_name, n, a_text, b_text):
    weights, rows, names = read_weights(weights_path)
    n = int(n)
    case = {"op": op, "form": form_name, "n": n, "a": parse_set(a_text, n),
            "b": parse_set(b_text, n)}
    plan, result, _, ev = mirror.phases(op, probe.gram_of(case), case["a"],
                                        case["b"])
    print(f"T = {len(plan)} terms, R = {len(result)} result blades")
    for phase in mirror.PHASES:
        if phase in weights:
            value, covered = predict_phase(weights, rows, names, phase,
                                           ev[phase])
            print(f"{phase:10} {float(value):16.0f}"
                  f"{'' if covered else '  (extrapolated)'}")


def binary_search_estimate(weights, plan, result):
    """Estimated thresholds of collect_result and type_terms if each found
    its place by binary search over the canonical list instead of a linear
    scan, as (low, high).

    A binary search over s places makes floor(log2 s) + 1 comparisons. Each
    comparison is taken to cost what one step of the present scan costs,
    the weight of a canonical_less call in collect_result (low), or half as
    much again, for the midpoint and the two bounds a binary search keeps
    (high). What a term costs besides its scan, and the shifts of an
    insertion, are kept at their fitted weights. These are estimates of code
    that does not exist; only measurement of that code can confirm them."""
    w_collect, w_typed = weights["collect"], weights["typed"]
    step = max(w_collect.get("collect.cl_index", 0),
               w_collect.get("collect.cl_grade", 0))
    present_c = collections.Counter()
    present_t = collections.Counter()
    out = mirror.collect_result(plan, present_c)
    mirror.typed_copy(out, present_t)
    mirror.type_terms(plan, out, present_t)
    fixed_c = (w_collect.get("const", 0) +
               w_collect.get("collect.new_end", 0) * present_c["collect.new_end"]
               + w_collect.get("collect.new_mid", 0) * present_c["collect.new_mid"]
               + w_collect.get("collect.cl_equal", 0) * present_c["collect.repeat"]
               + w_collect.get("collect.shift", 0) * present_c["collect.shift"])
    comparisons_c = 0
    size = 0
    seen = set()
    for _, _, mask, _ in plan:
        comparisons_c += size.bit_length()
        if mask not in seen:
            seen.add(mask)
            size += 1
    fixed_t = (w_typed.get("const", 0) +
               w_typed.get("type.term", 0) * len(plan) +
               w_typed.get("typed.blade", 0) * len(result))
    comparisons_t = len(plan) * len(result).bit_length()
    return ((fixed_c + step * comparisons_c, fixed_c + Fraction(3, 2) * step
             * comparisons_c),
            (fixed_t + step * comparisons_t, fixed_t + Fraction(3, 2) * step
             * comparisons_t))


def ceilings(weights_path, limit, largest=8):
    weights, rows, names = read_weights(weights_path)
    limit = int(limit)
    print(f"limit {limit}; phases predicted from {weights_path}")
    print("op\tform\tn\tT\tR\tlargest phase\tpredicted\tbuilds\t"
          "binary-search typing: largest phase, low, high, builds")
    for op in ("gp", "wedge"):
        for form_name in ("r", "rigid", "conformal", "dense"):
            for n in range(3, int(largest) + 1):
                if form_name == "dense" and op == "gp" and n > 6:
                    continue
                s = mirror.full(n)
                plan, result, _, ev = mirror.phases(
                    op, mirror.form(form_name, n), s, s)
                preds = {p: predict_phase(weights, rows, names, p, ev[p])
                         for p in mirror.PHASES if p in weights}
                top = max(preds, key=lambda p: preds[p][0])
                value = preds[top][0]
                (cl, ch), (tl, th) = binary_search_estimate(weights, plan,
                                                            result)
                rest = {p: v for p, (v, _) in preds.items()
                        if p not in ("collect", "typed")}
                low = max([*rest.values(), cl, tl])
                high = max([*rest.values(), ch, th])
                low_phase = max({**rest, "collect": cl, "typed": tl}.items(),
                                key=lambda kv: kv[1])[0]
                verdict = ("yes" if high <= limit else
                           "no" if low > limit else "unsure")
                covered = "" if all(c for _, c in preds.values()) else " (x)"
                print(f"{op}\t{form_name}\t{n}\t{len(plan)}\t{len(result)}\t"
                      f"{top}{covered}\t{float(value):.0f}\t"
                      f"{'yes' if value <= limit else 'no'}\t{low_phase}, "
                      f"{float(low):.0f}, {float(high):.0f}, {verdict}",
                      flush=True)


# GCC refuses a loop of this many iterations (-fconstexpr-loop-limit); the
# generator's longest loops run once per term.
GCC_LOOP_LIMIT = 1 << 18


def certify(weights_path, limit, relative, absolute, op, form_name, n, a_text,
            b_text):
    weights, rows, names = read_weights(weights_path)
    limit, n = int(limit), int(n)
    allowed = Fraction(limit) / (1 + Fraction(relative)) - int(absolute)
    case = {"op": op, "form": form_name, "n": n, "a": parse_set(a_text, n),
            "b": parse_set(b_text, n)}
    plan, result, _, ev = mirror.phases(op, probe.gram_of(case), case["a"],
                                        case["b"])
    reasons = []
    worst = 0
    for phase in mirror.PHASES:
        value, covered = predict_phase(weights, rows, names, phase, ev[phase])
        worst = max(worst, value)
        if not covered:
            reasons.append(f"{phase} is outside what the fit saw")
        if value > allowed:
            reasons.append(f"{phase} predicted {float(value):.0f} > "
                           f"{float(allowed):.0f}")
    longest = max(len(plan), len(case["a"]), len(case["b"]))
    if "gcc" in pathlib.Path(weights_path).name and longest >= GCC_LOOP_LIMIT:
        reasons.append(f"a loop of {longest} iterations reaches GCC's loop "
                       "limit")
    print(f"T = {len(plan)}, R = {len(result)}, largest phase predicted "
          f"{float(worst):.0f}, allowed {float(allowed):.0f} of {limit}")
    print("certified" if not reasons else "not certified: " +
          "; ".join(reasons))
    return not reasons


def main(argv):
    if len(argv) == 4 and argv[0] == "fit":
        fit(*argv[1:])
    elif len(argv) == 4 and argv[0] == "check":
        check(*argv[1:])
    elif len(argv) in (3, 4) and argv[0] == "ceilings":
        ceilings(*argv[1:])
    elif len(argv) == 10 and argv[0] == "certify":
        sys.exit(0 if certify(*argv[1:]) else 1)
    elif len(argv) == 7 and argv[0] == "predict":
        predict(*argv[1:])
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
