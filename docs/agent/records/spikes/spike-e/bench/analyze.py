#!/usr/bin/env -S python3 -I
"""Tables for the README from drive.py's rows, written to out/tables.md.

    python3 -I analyze.py <bench directory>

Reads out/rows.tsv (the three environment sizes) and out/sweep/rows.tsv
(the batch sizes), using for each (environment, round, compiler, operation,
loop, objects, form) the pair drive.py used (the last attempt), and
writes:

- per compiler, the median and range over the rounds of nanoseconds,
  cycles, and instructions per transform of each form and loop at
  environment size 0, and the nanosecond medians at all three sizes;
- the criterion of drive.py (medians apart by more than twice the larger
  range, the same form faster at every size) applied to nanoseconds, as
  fixed before the run, and to cycles per transform, which do not depend
  on the clock rate;
- for the sweep, cycles per transform of each form at each batch size, and
  the smallest size from which each collected or constrained form is
  faster than each other form at that size and at every larger size.
"""

import csv
import statistics
import sys
from pathlib import Path

FORMS = ("galoot", "a", "b", "bp", "c")
OPS = ("transform_point", "transform_line", "transform_plane")
LOOPS = ("throughput", "latency", "batch")
ENVS = (0, 1352, 2768)
PAIRS = (("b", "a"), ("bp", "a"), ("c", "a"), ("c", "b"), ("bp", "b"),
         ("a", "galoot"), ("b", "galoot"), ("c", "galoot"))


def used(rows):
    pairs = {}
    for row in rows:
        key = (int(row["env"]), int(row["round"]), row["compiler"],
               row["operation"], row["loop"], int(row["objects"]),
               row["form"])
        a = int(row["attempt"])
        if key not in pairs or pairs[key]["attempt"] < a:
            pairs[key] = {"attempt": a}
        if pairs[key]["attempt"] == a:
            pairs[key][row["kind"]] = row
    return pairs


def per_op(pair, field):
    s, l = pair["short"], pair["long"]
    return ((float(l[field]) - float(s[field]))
            / (int(l["transforms"]) - int(s["transforms"])))


def groups(path):
    with open(path, encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    g = {}
    for (env, _, c, o, l, n, form), p in used(rows).items():
        d = g.setdefault((c, o, l, n, form, env),
                         {"ns": [], "cyc": [], "inst": []})
        d["ns"].append(float(p["long"]["ns_per_op"]))
        d["cyc"].append(per_op(p, "cycles_u"))
        d["inst"].append(per_op(p, "inst_p_u"))
    return g


def med(v):
    return statistics.median(v)


def rng(v):
    return max(v) - min(v)


def verdict(g, c, o, l, n, x, y, field):
    """'x' or 'y' when the faster form is apart beyond the criterion at
    every size, else '-'; with the ratio of medians at size 0."""
    faster = set()
    beyond = True
    for env in ENVS:
        gx, gy = g.get((c, o, l, n, x, env)), g.get((c, o, l, n, y, env))
        if not gx or not gy:
            return "-", None
        mx, my = med(gx[field]), med(gy[field])
        limit = 2 * max(rng(gx[field]), rng(gy[field]))
        beyond = beyond and abs(mx - my) > limit
        faster.add(x if mx < my else y)
    gx, gy = g[c, o, l, n, x, 0], g[c, o, l, n, y, 0]
    ratio = med(gx[field]) / med(gy[field])
    return (faster.pop() if beyond and len(faster) == 1 else "-"), ratio


def main(argv):
    here = Path(argv[0])
    g = groups(here / "out" / "rows.tsv")
    out = []
    for c in ("gcc", "clang"):
        out.append(f"### {'GCC 16' if c == 'gcc' else 'Clang 23'}\n")
        out.append("Per transform, median [range] over 11 rounds at "
                   "environment size 0; ns at sizes 0 / 1352 / 2768.\n")
        out.append("| Operation | Loop | Form | ns, sizes 0 / 1352 / 2768 | "
                   "cycles [range] | instructions |")
        out.append("| --- | --- | --- | --- | --- | ---: |")
        for o in OPS:
            for l in LOOPS:
                n = 1024 if l == "batch" else 1
                for form in FORMS:
                    ns = " / ".join(f"{med(g[c, o, l, n, form, e]['ns']):.2f}"
                                    for e in ENVS)
                    d = g[c, o, l, n, form, 0]
                    out.append(f"| {o.split('_')[1]} | {l} | {form} | {ns} | "
                               f"{med(d['cyc']):.1f} [{rng(d['cyc']):.1f}] | "
                               f"{med(d['inst']):.1f} |")
        out.append("")
    out.append("### The criterion, pair by pair\n")
    out.append("Ratio of medians (first form over second) at size 0, in ns "
               "and in cycles; the form named is faster beyond the criterion "
               "at all three sizes, '-' when not.\n")
    out.append("| Compiler | Operation | Loop | Pair | ns ratio | ns verdict "
               "| cycles ratio | cycles verdict |")
    out.append("| --- | --- | --- | --- | ---: | --- | ---: | --- |")
    for c in ("gcc", "clang"):
        for o in OPS:
            for l in LOOPS:
                n = 1024 if l == "batch" else 1
                for x, y in PAIRS:
                    vn, rn = verdict(g, c, o, l, n, x, y, "ns")
                    vc, rc = verdict(g, c, o, l, n, x, y, "cyc")
                    out.append(f"| {c} | {o.split('_')[1]} | {l} | {x} / {y} "
                               f"| {rn:.3f} | {vn} | {rc:.3f} | {vc} |")
    out.append("")

    sweep = here / "out" / "sweep" / "rows.tsv"
    if sweep.exists():
        s = groups(sweep)
        sizes = sorted({k[3] for k in s})
        forms = [f for f in FORMS if any(k[4] == f for k in s)]
        out.append("### Sweep: cycles per transform against objects per "
                   "motor (batch loop, size 0, median of 5)\n")
        out.append("| Compiler | Operation | Form | " +
                   " | ".join(str(n) for n in sizes) + " | 1024 (run) |")
        out.append("| --- | --- | --- | " + " | ".join("---:" for _ in sizes)
                   + " | ---: |")
        for c in ("gcc", "clang"):
            for o in OPS:
                for form in forms:
                    cells = [f"{med(s[c, o, 'batch', n, form, 0]['cyc']):.1f}"
                             for n in sizes]
                    cells.append(f"{med(g[c, o, 'batch', 1024, form, 0]['cyc']):.1f}")
                    out.append(f"| {c} | {o.split('_')[1]} | {form} | "
                               + " | ".join(cells) + " |")
        out.append("")
        out.append("Break-even: the smallest batch size from which the first "
                   "form takes fewer cycles per transform (medians) than the "
                   "second at that size and every larger one, the run's 1,024 "
                   "included.\n")
        out.append("| Compiler | Operation | b against a | b against galoot "
                   "| c against a | b against c |")
        out.append("| --- | --- | --- | --- | --- | --- |")

        def cyc(c, o, n, form):
            if n == 1024:
                return med(g[c, o, "batch", 1024, form, 0]["cyc"])
            return med(s[c, o, "batch", n, form, 0]["cyc"])

        for c in ("gcc", "clang"):
            for o in OPS:
                cells = []
                for x, y in (("b", "a"), ("b", "galoot"), ("c", "a"),
                             ("b", "c")):
                    all_n = sizes + [1024]
                    found = "never"
                    for i, n in enumerate(all_n):
                        if all(cyc(c, o, m, x) < cyc(c, o, m, y)
                               for m in all_n[i:]):
                            found = str(n)
                            break
                    cells.append(found)
                out.append(f"| {c} | {o.split('_')[1]} | " + " | ".join(cells)
                           + " |")
        out.append("")
    (here / "out" / "tables.md").write_text("\n".join(out) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
