#!/usr/bin/env -S python3 -I
"""Compare the verdicts and killing tests of tools/mutate with an old
driver's recorded table, mutant by mutant.

Usage, from the root: compare_runs.py rga3 | blade_product

rga3: 0023-rga3/out/<preset>.tsv (the old driver: a mutant is killed when
its build fails or a test fails) against out/mutants-<preset>.tsv.
blade_product: 0011-blade-product/results-b142837-<preset>.tsv (column
killed) against out/mutants-<preset>.tsv, for the mutants both hold.
Prints, per build preset, each verdict that differs and each test that one
table lists as failed and the other not, with whether the new row lists it
as not run (its program did not build); exits 1 if a verdict differs.
"""

import csv
import pathlib
import sys

RECORDS = pathlib.Path(__file__).resolve().parent.parent
csv.field_size_limit(1 << 24)


def table(path, skip=0):
    with open(path, encoding="utf-8") as f:
        lines = list(csv.reader(f, delimiter="\t"))[skip:]
    return {line[0]: dict(zip(lines[0], line)) for line in lines[1:]}


def names(cell, separator=" "):
    return set() if cell in ("-", "") else set(cell.split(separator))


def main(which):
    status = 0
    for preset in ("gcc-debug", "clang-debug"):
        if which == "rga3":
            old = table(RECORDS / f"0023-rga3/out/{preset}.tsv")
            new = table(RECORDS / f"0023-rga3/out/mutants-{preset}.tsv")
            killed = {m: r["failed_tests"] != "-" or r["build_exit"] != "0"
                      for m, r in old.items()}
            failed = {m: names(r["failed_tests"]) for m, r in old.items()}
        else:
            old = table(RECORDS / "0011-blade-product/"
                        f"results-b142837-{preset}.tsv", skip=1)
            new = table(RECORDS / f"0011-blade-product/out/mutants-{preset}.tsv")
            killed = {m: r["killed"] == "killed" for m, r in old.items()}
            failed = {m: names(r["failed_tests"], ",") for m, r in old.items()}
        shared = [m for m in old if m in new]
        print(f"{preset}: {len(shared)} mutants in both tables")
        for m in shared:
            row = new[m]
            if killed[m] != (row["verdict"] == "killed"):
                print(f"  {m}: verdict {row['verdict']}, recorded "
                      f"{'killed' if killed[m] else 'not killed'}")
                status = 1
            now = {k for k in names(row["killers"]) if ":" not in k}
            not_run = names(row["not_run"])
            for test in sorted(failed[m] - now):
                print(f"  {m}: {test} failed in the record, not now"
                      + (" (not run: its program did not build)"
                         if test in not_run else ""))
            for test in sorted(now - failed[m]):
                print(f"  {m}: {test} fails now, not in the record")
    return status


if __name__ == "__main__":
    if sys.argv[1:] not in (["rga3"], ["blade_product"]):
        sys.exit(__doc__)
    sys.exit(main(sys.argv[1]))
