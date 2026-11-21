"""List the rows a unit may act on before the S11 amendment, and draw the sample.

Usage: python3 -I sample.py REPO [REV] [SEED]

A row may be acted on before the amendment when it waits for nothing, for B9,
for D1 or for D3 alone, and is not a keep row. The rows read in full are those
of kind public contract or file prologue in core's mv.hpp, product.hpp,
transform.hpp and basis_change.hpp; the sample is drawn from all the others.
"""
import csv
import io
import random
import subprocess
import sys

repo = sys.argv[1]
rev = sys.argv[2] if len(sys.argv) > 2 else "7b99a2f"
seed = int(sys.argv[3]) if len(sys.argv) > 3 else 20261121

text = subprocess.run(
    ["git", "-C", repo, "show", f"{rev}:docs/agent/records/docs/comment-moves.tsv"],
    check=True, capture_output=True, text=True).stdout
rows = list(csv.DictReader(io.StringIO(text), delimiter="\t"))
early = [r for r in rows
         if r["waits_for"] in {"none", "B9", "D1", "D3"} and r["class"] != "keep"]
focus_headers = tuple(f"include/galoot/core/{h}" for h in
                      ("mv.hpp", "product.hpp", "transform.hpp", "basis_change.hpp"))
focus = [r for r in early if r["header"] in focus_headers
         and r["kind"] in {"public contract", "file prologue"}]
rest = [r for r in early if r not in focus]
print(f"rows acted on before the amendment: {len(early)}; "
      f"read in full: {len(focus)}; the rest: {len(rest)}; seed {seed}")


def show(r):
    return "\t".join((r["header"].removeprefix("include/galoot/"), r["lines"],
                      r["position"], r["kind"], r["class"], r["waits_for"],
                      r["descriptor"]))


print("\nread in full:")
for r in focus:
    print(show(r))
print("\nsample:")
for r in sorted(random.Random(seed).sample(rest, 25),
                key=lambda r: (r["header"], int(r["lines"].split(",")[0].split("-")[0]))):
    print(show(r))
