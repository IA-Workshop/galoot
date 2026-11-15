#!/usr/bin/env -S python3 -I
"""Read the rows of tools/compiles cost for the growth grid and write the
cost of each use, its unit minus the baseline unit of the same n.

Usage:
  analyze.py <out-dir>

Reads <out-dir>/rows.tsv and writes <out-dir>/growth.tsv: per compiler and
unit, the mean of the retired instructions over the repeats and their range,
the mean minus the baseline's mean at the same n (the use), the larger of
the two ranges, the use's change from n = 4, the mean wall time, and the
largest peak memory.
"""

import csv
import statistics
import sys
from pathlib import Path


def main(argv):
    out = Path(argv[1])
    with open(out / "rows.tsv", encoding="utf-8") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    groups = {}
    for row in rows:
        g = groups.setdefault((row["compiler"], row["unit"]),
                              {"inst": [], "wall": [], "rss": []})
        g["inst"].append(int(row["inst_u"]))
        g["wall"].append(float(row["wall_s"]))
        g["rss"].append(int(row["maxrss_kib"]))

    def mean(key):
        return statistics.mean(groups[key]["inst"])

    def spread(key):
        values = groups[key]["inst"]
        return max(values) - min(values)

    lines = []
    for (compiler, unit), g in sorted(groups.items()):
        n, use = unit.split("/")
        base = (compiler, f"{n}/baseline")
        cost = mean((compiler, unit)) - mean(base)
        limit = max(spread((compiler, unit)), spread(base))
        first = (compiler, f"n4/{use}")
        if use != "baseline" and first in groups:
            start = mean(first) - mean((compiler, "n4/baseline"))
            change = f"{100 * (cost - start) / start:+.1f}%" if start else "-"
        else:
            change = "-"
        lines.append([compiler, unit, str(len(g["inst"])),
                      f"{mean((compiler, unit)) / 1e6:.1f}",
                      f"{spread((compiler, unit)) / 1e6:.1f}",
                      f"{cost / 1e6:.1f}" if use != "baseline" else "-",
                      f"{limit / 1e6:.1f}" if use != "baseline" else "-",
                      change, f"{statistics.mean(g['wall']):.3f}",
                      str(max(g["rss"]))])
    with open(out / "growth.tsv", "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", lineterminator="\n")
        writer.writerow(["compiler", "unit", "repeats", "inst_mean_M",
                         "inst_range_M", "use_M", "larger_range_M",
                         "use_change_from_n4", "wall_mean_s",
                         "maxrss_max_kib"])
        writer.writerows(lines)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
