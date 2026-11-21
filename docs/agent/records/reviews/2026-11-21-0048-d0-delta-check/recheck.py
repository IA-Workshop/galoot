"""Independent rechecks of the comment table at 01dce7f.

Usage, from the repository root:
    python3 -I docs/agent/records/reviews/2026-11-21-0048-d0-delta-check/recheck.py stages
    python3 -I docs/agent/records/reviews/2026-11-21-0048-d0-delta-check/recheck.py sample
    python3 -I docs/agent/records/reviews/2026-11-21-0048-d0-delta-check/recheck.py mutate SCRATCH_DIR

stages  recomputes the stage figures and the D1 to D4 columns of the table's
        README by its stated rule: a keep row counts its length, a summary row
        whose condition is met counts two lines, a row whose condition is not
        met counts its length, and nothing else counts.
sample  draws the 40 delete rows that wait for nothing (seed 20261122) from
        those not named in the first check's record, and prints each row's
        header text with three lines after it.
mutate  writes altered copies of the table and its README into SCRATCH_DIR
        and runs the table's own checker on each, to see which demotions of a
        kept precondition it reports.
"""
import collections
import csv
import os
import random
import subprocess
import sys

DOCS = "docs/agent/records/docs/"
TABLE = DOCS + "comment-moves.tsv"
REVISION = "01dce7f"


def rows():
    out = list(csv.DictReader(open(TABLE), delimiter="\t"))
    for i, r in enumerate(out):
        r["row"] = i + 2  # line number in the file
    return out


def lines(spec):
    out = []
    for part in spec.split(","):
        a, _, b = part.partition("-")
        out += list(range(int(a), int(b or a) + 1))
    return out


_texts = {}


def text(header):
    if header not in _texts:
        _texts[header] = subprocess.run(
            ["git", "show", f"{REVISION}:{header}"], capture_output=True,
            text=True, check=True).stdout.split("\n")
    return _texts[header]


def stages():
    def group(r, l):
        if r["position"] == "prologue":
            return "D1"
        if text(r["header"])[l - 1].strip().startswith("///"):
            return "D4" if r["position"] == "public" else "D2"
        return "D3"

    base = {"none", "B9"}
    plan = [("now", None), ("nothing and B9", base),
            ("detail ruling (a)", base | {"the detail ruling"}),
            ("D1 and D3", base | {"the detail ruling", "D1", "D3",
                                  "D3 and the detail ruling"}),
            ("S11 amendment", base | {"the detail ruling", "D1", "D3",
                                      "D3 and the detail ruling",
                                      "the S11 amendment",
                                      "D3 and the S11 amendment"})]
    full = [r for r in rows() if r["position"] != "trailing"]
    for name, met in plan:
        total = collections.Counter()
        for r in full:
            ls = lines(r["lines"])
            if met is None or r["class"] == "keep" or r["waits_for"] not in met:
                kept = ls
            elif r["class"] == "summary":
                kept = ls[:2]
            else:
                kept = []
            for l in kept:
                total[group(r, l)] += 1
        print(f"{name}\t{sum(total.values())}\t{dict(sorted(total.items()))}")


NAMED_IN_FIRST_CHECK = {
    ("core/assert.hpp", "95-97"), ("core/assert.hpp", "112-114"),
    ("core/blade_set.hpp", "11-14"), ("core/transform.hpp", "589"),
    ("core/unary.hpp", "18-24"), ("core/transform.hpp", "24-29"),
    ("core/dual.hpp", "60-70"), ("core/plan.hpp", "63-66"),
    ("core/plan.hpp", "624-627"), ("core/basis_change.hpp", "35-42"),
    ("core/basis_change.hpp", "43-50"), ("core/basis_change.hpp", "51-65"),
    ("core/basis_change.hpp", "83-102"), ("core/transform.hpp", "713-729"),
    ("core/transform.hpp", "751-763"), ("core/plan.hpp", "609-621"),
    ("core/mv.hpp", "883-888"), ("core/bits.hpp", "132"),
    ("core/blade_set.hpp", "577"), ("core/plan.hpp", "302-307"),
    ("core/transform.hpp", "492-495"), ("core/unary.hpp", "25-27")}


def sample():
    pool = [r for r in rows()
            if r["class"] == "delete" and r["waits_for"] == "none"
            and (r["header"][len("include/galoot/"):], r["lines"])
            not in NAMED_IN_FIRST_CHECK]
    random.seed(20261122)
    chosen = sorted(random.sample(pool, 40), key=lambda r: r["row"])
    print(f"pool {len(pool)}")
    for r in chosen:
        ls = lines(r["lines"])
        t = text(r["header"])
        print(f"### row {r['row']} {r['header']}:{r['lines']} -> "
              f"{r['target']} | {r['heading']}")
        for i in range(ls[0], min(ls[-1] + 3, len(t)) + 1):
            print(f"{i:5} {t[i - 1]}")


def mutate(scratch):
    os.makedirs(scratch, exist_ok=True)
    table = open(TABLE).read()
    readme = open(DOCS + "README.md").read()

    def demote(header, spec):
        match = [l for l in table.split("\n")
                 if l.startswith(f"include/galoot/{header}\t{spec}\t")]
        assert len(match) == 1, (header, spec)
        f = match[0].split("\t")
        f[4], f[6], f[7], f[8] = ("move", "docs/guide/reference/blade-helpers.md",
                                  "x", "D3 and the S11 amendment")
        return match[0], "\t".join(f)

    def drop(line_start):
        match = [l for l in readme.split("\n") if l.startswith(line_start)]
        assert len(match) == 1, line_start
        return match[0] + "\n", ""

    cases = [
        ("baseline", None, []),
        ("demote antigrade 80-81", demote("core/bits.hpp", "80-81"), []),
        ("demote antigrade 80-81 and move it to the second README table",
         demote("core/bits.hpp", "80-81"),
         [drop("| `bits.hpp:80-81`"),
          ("| `blade_set.hpp:129-130` |",
           "| `bits.hpp:80-81` | antigrade | defined |\n| `blade_set.hpp:129-130` |")]),
        ("demote get 1281-1283 and drop it from the README",
         demote("core/mv.hpp", "1281-1283"), [drop("| `mv.hpp:1281-1283`")]),
        ("demote low_bits 45-47 and drop it from the README",
         demote("core/bits.hpp", "45-47"),
         [("| `bits.hpp:45-47`, `:58-60` |", "| `bits.hpp:58-60` |")]),
        ("demote in_algebra 58-60 and drop it from the README",
         demote("core/bits.hpp", "58-60"),
         [("| `bits.hpp:45-47`, `:58-60` |", "| `bits.hpp:45-47` |")]),
        ("demote linear 407-411 and drop it from the README",
         demote("core/linear.hpp", "407-411"), [drop("| `linear.hpp:407-411`")]),
        ("demote assert 30-32 and drop it from the README",
         demote("core/assert.hpp", "30-32"), [drop("| `assert.hpp:30-32`")]),
        ("demote collect_result 302-304", demote("core/plan.hpp", "302-304"), []),
        ("demote is_null 126", demote("core/metric.hpp", "126"), []),
        ("demote coefficient_type 49-57 and drop it from the README",
         demote("core/blade_product.hpp", "49-57"),
         [drop("| `blade_product.hpp:49-57`")]),
        ("demote antireverse_sign 167-168", demote("core/bits.hpp", "167-168"), []),
        ("demote antireverse_sign 167-168 and drop it from the README",
         demote("core/bits.hpp", "167-168"), [drop("| `bits.hpp:167-168`")]),
        ("demote multiply_checked 81-83 and drop it from the README",
         demote("core/blade_product.hpp", "81-83"),
         [drop("| `blade_product.hpp:82-83`")]),
        ("demote term_list::add 132-140 and drop 139-140 from the README",
         demote("core/blade_product.hpp", "132-140"),
         [drop("| `blade_product.hpp:139-140`")]),
    ]
    for n, (name, table_edit, readme_edits) in enumerate(cases):
        t, r = table, readme
        if table_edit:
            assert t.count(table_edit[0]) == 1
            t = t.replace(*table_edit)
        for old, new in readme_edits:
            assert r.count(old) == 1, (name, old)
            r = r.replace(old, new)
        tp = os.path.join(scratch, f"case{n}.tsv")
        rp = os.path.join(scratch, f"case{n}.md")
        open(tp, "w").write(t)
        open(rp, "w").write(r)
        p = subprocess.run(["python3", "-I", DOCS + "check_comment_moves.py",
                            "--readme", rp, tp], capture_output=True, text=True)
        verdict = "reported" if p.returncode else "passes"
        print(f"{name}: {verdict}")
        for l in p.stdout.split("\n"):
            if l.startswith("Sentences") or not p.returncode:
                break
            if l:
                print("    " + l)


if __name__ == "__main__":
    {"stages": stages, "sample": sample,
     "mutate": lambda: mutate(sys.argv[2])}[sys.argv[1]]()
