"""Independent recomputation of the comment table's figures.

Usage: python3 -I figures.py REPO REV_TABLE [REV_HEADERS]
Reads comment-moves.tsv at REV_TABLE and the headers at REV_HEADERS
(default 01dce7f) through git show, and prints the figures.
"""
import collections
import csv
import io
import subprocess
import sys

repo, rev_table = sys.argv[1], sys.argv[2]
rev_headers = sys.argv[3] if len(sys.argv) > 3 else "01dce7f"


def show(rev, path):
    return subprocess.run(["git", "-C", repo, "show", f"{rev}:{path}"],
                          check=True, capture_output=True, text=True).stdout


rows = list(csv.DictReader(io.StringIO(
    show(rev_table, "docs/agent/records/docs/comment-moves.tsv")),
    delimiter="\t"))


def expand(spec):
    out = []
    for part in spec.split(","):
        if "-" in part:
            a, b = part.split("-")
            out += range(int(a), int(b) + 1)
        else:
            out.append(int(part))
    return out


texts = {}
for r in rows:
    if r["header"] not in texts:
        texts[r["header"]] = show(rev_headers, r["header"]).splitlines()

full = [r for r in rows if r["position"] != "trailing"]
for r in full:
    r["n"] = len(expand(r["lines"]))
    r["slashes"] = [texts[r["header"]][l - 1].lstrip().startswith("///")
                    for l in expand(r["lines"])]

total = sum(r["n"] for r in full)
print("rows", len(rows), "full-line lines", total)

# By kind and class.
kc = collections.Counter()
for r in full:
    kc[(r["kind"], r["class"])] += r["n"]
for k in sorted({k for k, _ in kc}):
    vals = [kc[(k, c)] for c in ("keep", "summary", "move", "delete")]
    print(f"kind {k}: {vals} all {sum(vals)}")
cls = collections.Counter()
for r in full:
    cls[r["class"]] += r["n"]
print("class sums", dict(cls))

# By what rows wait for.
w = collections.Counter()
wd = collections.Counter()
for r in full:
    key = r["waits_for"]
    if key == "none":
        key = "none (kept)" if r["class"] == "keep" else "none (delete or summary now)"
    w[key] += r["n"]
    if r["class"] == "delete":
        wd[key] += r["n"]
for k in sorted(w):
    print(f"waits {k}: {w[k]} delete {wd[k]}")


def candidate(r, i):
    if r["position"] == "prologue":
        return "D1"
    if r["slashes"][i]:
        return "D2" if r["position"] == "detail" else "D4"
    return "D3"


def kept_lines(r, met):
    # Lines this row leaves in a header once the conditions in met are met.
    if r["class"] == "keep" or r["waits_for"] not in met:
        return r["n"]
    if r["class"] == "summary":
        return 2
    return 0


stages = [{"none", "B9"}, {"none", "B9", "D1", "D3"},
          {"none", "B9", "D1", "D3", "the S11 amendment",
           "D3 and the S11 amendment"}]
for met in stages:
    left = sum(kept_lines(r, met) for r in full)
    print(f"stage {sorted(met)}: {left} ({100 * (left - total) / total:.1f}%)")

# Candidates after D3 and after the amendment. A summary row's two kept
# lines are charged to the candidate of its first line.
for met, name in ((stages[1], "after D3"), (stages[2], "after amendment")):
    cand = collections.Counter()
    for r in full:
        k = kept_lines(r, met)
        if k == r["n"]:
            for i in range(r["n"]):
                cand[candidate(r, i)] += 1
        else:
            cand[candidate(r, 0)] += k
    print(name, dict(sorted(cand.items())), sum(cand.values()))

# Prologue deletes by condition.
pd = collections.Counter()
for r in full:
    if r["position"] == "prologue" and r["class"] == "delete":
        pd[r["waits_for"]] += r["n"]
print("prologue deletes", dict(pd), sum(pd.values()))

# Rows a unit may act on before the amendment, not keep, example, marker or
# narration, by position.
act = [r for r in full if r["waits_for"] in {"none", "B9", "D1", "D3"}
       and r["class"] != "keep"
       and r["kind"] not in {"example", "marker", "narration"}]
print("actable contract-candidate rows", len(act),
      dict(collections.Counter(r["position"] for r in act)))

end = sum(kept_lines(r, stages[2]) for r in full)
s1 = sum(kept_lines(r, stages[0]) for r in full)
print("restated all lines before D3", 4320 + s1,
      "after amendment with 120 pointers", 4320 + end + 120,
      "with C1", 4150 + end + 120,
      "end with pointers", end + 120,
      f"({100 * (end + 120 - total) / total:.1f}%)")
