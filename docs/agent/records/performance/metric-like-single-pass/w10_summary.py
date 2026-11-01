#!/usr/bin/env -S python3 -I
"""The W10 rows of two revisions, side by side.

Usage:
  w10_summary.py <compile.tsv> <runs.tsv> <before-rev> <after-rev> <selection-prefix>...

From the compile record: per preset (gcc-debug, clang-debug), the number of
compiles, the sum of retired instructions (both core kinds), and the
instructions and peak memory of metric_test.cpp and plan_test.cpp. From the
run log: the wall time of the build and test rows whose selection starts
with one of the given prefixes (the fresh trees' rows). Revisions match by
prefix.
"""

import csv
import sys

UNITS = ("test/core/metric_test.cpp", "test/core/plan_test.cpp")


def rows(path):
    with open(path, encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def number(value):
    try:
        return int(value)
    except ValueError:
        return 0


def main(argv):
    if len(argv) < 5:
        sys.exit(__doc__)
    compiles, runs = rows(argv[0]), rows(argv[1])
    revisions, prefixes = argv[2:4], argv[4:]
    for preset in ("gcc-debug", "clang-debug"):
        for rev in revisions:
            mine = [r for r in compiles
                    if r["revision"].startswith(rev) and r["preset"] == preset]
            total = sum(number(r["inst_p_u"]) + number(r["inst_e_u"])
                        for r in mine)
            print(f"{preset} {rev}: {len(mine)} compiles, "
                  f"{total / 1e9:.3f} G instructions")
            for unit in UNITS:
                for r in mine:
                    if r["source"].endswith(unit):
                        inst = number(r["inst_p_u"]) + number(r["inst_e_u"])
                        print(f"  {unit}: {inst / 1e9:.3f} G, "
                              f"{number(r['maxrss_kib']) / 1024:.0f} MiB, "
                              f"{r['wall_s']} s")
            for r in runs:
                if (r["revision"].startswith(rev) and r["preset"] == preset
                        and r["step"] in ("build", "test")
                        and any(r["selection"].startswith(p)
                                for p in prefixes)):
                    print(f"  {r['step']} {r['selection']}: {r['wall_s']} s, "
                          f"exit {r['exit']}")


if __name__ == "__main__":
    main(sys.argv[1:])
