#!/usr/bin/env -S python3 -I
"""Where Clang spends the part of a fixed use's cost that still grows with n:
-ftime-trace of the units of measure_growth.py (the four typed pairs in
R(n-1,0,1), and the baseline with no plan) at each n, with the checkout's
headers.

Usage, from the repository root:
  trace_remainder.py <out-dir> [repeats] [cpu]

Each compile is pinned as in measure_growth.py and traced with
-ftime-trace-granularity=0. For each unit the script records the retired
instructions of the compile, Clang's "Total ..." events (the time per kind of
work, summed over nested events), and the number and summed duration of the
events of each kind that evaluate constants or instantiate templates, with
the names of the costliest. Writes <out-dir>/trace.tsv (one row per unit,
repeat, and event kind) and <out-dir>/trace-summary.txt (the means over the
repeats, and the difference between the plans unit and its baseline at each
n).
"""

import collections
import json
import pathlib
import statistics
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
sys.path.insert(0, str(ROOT / "docs/agent/records/performance/plan-growth"))
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import measure_plan as mp
from measure_growth import FLAGS, SIZES, git

COUNTED = ("EvaluateAsInitializer", "EvaluateAsConstantExpr", "EvaluateAsRValue",
           "EvaluateAsBooleanCondition", "EvaluateForOverflow",
           "InstantiateFunction", "InstantiateClass", "ParseClass",
           "CodeGen Function", "OptFunction", "RunPass")


def trace_one(selection, source, cpu):
    obj = source.with_suffix(".o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", "clang++", *FLAGS,
               f"-I{ROOT / 'include'}", "-ftime-trace",
               "-ftime-trace-granularity=0", "-c", "-o", str(obj),
               str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(f"{selection} failed:\n{result.stderr[:4000]}")
    row = mp.read_last_row()
    if row.get("selection") != selection or row.get("mode") != "pinned":
        sys.exit(f"the last run row is not a pinned {selection}")
    trace = obj.with_suffix(".json")
    events = [e for e in json.loads(trace.read_text())["traceEvents"]
              if e.get("ph") == "X"]
    trace.unlink()
    obj.unlink()
    totals = {e["name"]: e["dur"] for e in events
              if e["name"].startswith("Total ")}
    counted = collections.defaultdict(lambda: [0, 0])
    detail = collections.defaultdict(lambda: [0, 0])
    for e in events:
        if e["name"] in COUNTED:
            counted[e["name"]][0] += 1
            counted[e["name"]][1] += e["dur"]
            name = e.get("args", {}).get("detail", "")[:90]
            detail[(e["name"], name)][0] += 1
            detail[(e["name"], name)][1] += e["dur"]
    return int(row["inst_p_u"]), totals, counted, detail


def main(argv):
    if not argv:
        sys.exit(__doc__)
    out = pathlib.Path(argv[0]).resolve()
    repeats = int(argv[1]) if len(argv) > 1 else 3
    cpu = argv[2] if len(argv) > 2 else "6"
    if git("status", "--porcelain", "--", "include"):
        sys.exit("include/ has uncommitted changes")
    out.mkdir(parents=True, exist_ok=True)
    units = out / "trace-units"
    units.mkdir(exist_ok=True)
    head = git("rev-parse", "--short=7", "HEAD")

    rows = []
    means = collections.defaultdict(list)
    details = {}
    for repeat in range(1, repeats + 1):
        for n in SIZES:
            for plans, pairs in ((0, []), (4, mp.TYPED_PAIRS)):
                source = units / f"n{n}-plans{plans}.cpp"
                source.write_text(mp.plans_unit(n, pairs), encoding="utf-8")
                instructions, totals, counted, detail = trace_one(
                    f"trace-0012:clang:n{n}:plans{plans}:{repeat}", source, cpu)
                key = (n, plans)
                means[(key, "instructions", "")].append(instructions)
                rows.append((n, plans, repeat, "instructions", "", instructions))
                for name, dur in totals.items():
                    means[(key, name, "us")].append(dur)
                    rows.append((n, plans, repeat, name, "us", dur))
                for name, (count, dur) in counted.items():
                    means[(key, name, "count")].append(count)
                    means[(key, name, "us")].append(dur)
                    rows.append((n, plans, repeat, name, "count", count))
                    rows.append((n, plans, repeat, name, "us", dur))
                if repeat == 1:
                    details[key] = detail
                print(n, plans, repeat, instructions, flush=True)

    with open(out / "trace.tsv", "w", encoding="utf-8") as f:
        f.write("n\tplans\trepeat\tevent\tunit\tvalue\n")
        for r in rows:
            f.write("\t".join(map(str, r)) + "\n")

    lines = [f"Clang 23 -ftime-trace at {head}, mean of {repeats} pinned "
             "repeats; plans - baseline at each n"]
    names = sorted({(k[1], k[2]) for k in means})
    header = "event\tunit\t" + "\t".join(f"n={n}" for n in SIZES)
    lines.append(header)
    for name, unit in names:
        cells = []
        for n in SIZES:
            full = means.get(((n, 4), name, unit))
            base = means.get(((n, 0), name, unit))
            if full is None:
                cells.append("-")
                continue
            value = statistics.mean(full) - (statistics.mean(base)
                                             if base else 0)
            cells.append(f"{value:.0f}")
        lines.append(f"{name}\t{unit}\t" + "\t".join(cells))
    lines.append("\nCostliest events of the plans unit, first repeat: "
                 "kind, detail, count, microseconds at n = 4 and at n = 16")
    small, large = details[(SIZES[0], 4)], details[(SIZES[-1], 4)]
    for key in sorted(large, key=lambda k: -large[k][1])[:40]:
        lines.append(f"{key[0]}\t{key[1]}\t{small.get(key, [0, 0])[0]}\t"
                     f"{large[key][0]}\t{small.get(key, [0, 0])[1]}\t"
                     f"{large[key][1]}")
    (out / "trace-summary.txt").write_text("\n".join(lines) + "\n",
                                           encoding="utf-8")
    print("\n".join(lines))
    for p in units.iterdir():
        p.unlink()
    units.rmdir()


if __name__ == "__main__":
    main(sys.argv[1:])
