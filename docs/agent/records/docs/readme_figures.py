"""Print the figures that README.md states beyond the checker's sums.

Usage, from the repository root:
    python3 -I docs/agent/records/docs/readme_figures.py [--compare REV] [TABLE]

The checker (check_comment_moves.py) prints the sums by header, kind,
position, the survey's four classes and waits_for, and the boundary report;
this script reads the same table and the headers at the same revision and
prints the rest: the stages, the candidates against plan 0048 section 5.4,
the pointer estimates, the restated totals, and the counts the README's prose
quotes. --compare REV also lists the rows whose class or waits_for differ
from the table at that git revision, grouped by the change.

A stage counts the comment lines left once its conditions are met: every
line of a keep row; two lines of a summary row (its one sentence), or all of
it if shorter; nothing of a move or delete row whose condition is met; and
every line of a row whose condition is not yet met. Pointer lines are not in
the stages; the pointer estimates are added after.
"""
import argparse
import collections
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import check_comment_moves as check  # noqa: E402

REVISION = "01dce7f"

# Plan 0048 section 5.4, column (b): the comment lines the plan expected to
# remain in each of its four candidate classes, one sentence and a pointer.
PLAN_B = {"prologue": 90, "/// detail": 110, "//": 330, "/// public": 340}
CANDIDATE = {"prologue": "D1", "/// detail": "D2", "//": "D3", "/// public": "D4"}

# restated-targets.md: code and blank lines of its scenario rows, the
# all-lines figure less the comment figure (5,940 - 1,620 and 5,190 - 870
# without C1; 5,020 - 870 with it).
CODE_AND_BLANK = 4320
CODE_AND_BLANK_C1 = 4150

STAGES = (
    ("Deletions that wait for nothing, and B9", {"none", "B9"}),
    ("Plan 0049's D1 and D3 pages received, S11 as ratified", {"D1", "D3"}),
    ("The S11 amendment applied", {"the S11 amendment", "D3 and the S11 amendment"}),
)

GUIDE_REPEATS = ("docs/guide/core.md", "docs/guide/rga3.md", "docs/guide/README.md")


def read_table(text):
    rows = text.rstrip("\n").split("\n")
    columns = rows[0].split("\t")
    records = []
    for number, raw in enumerate(rows[1:], 2):
        r = dict(zip(columns, raw.split("\t")))
        r["number"] = number
        r["line_list"] = check.parse_lines(r["lines"])
        records.append(r)
    return records


def group(r, text):
    """Plan 0048's four classes: prologue, /// public, /// detail, //."""
    if r["position"] == "prologue":
        return "prologue"
    doc = text[r["line_list"][0] - 1].strip().startswith("///")
    return f"/// {r['position']}" if doc else "//"


def left(r, met):
    n = len(r["line_list"])
    if r["waits_for"] not in met:
        return n
    return {"keep": n, "summary": min(n, 2), "move": 0, "delete": 0}[r["class"]]


def percent(part, whole):
    return f"{round(100 * part / whole)}%"


def doc_blocks(headers, texts):
    """Public /// blocks by scope (namespace or member) and by whether the
    declaration after them only rejects a call, as inventory.tsv's role says."""
    inventory = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                             "../simplification/baseline/inventory.tsv")
    declared = collections.defaultdict(list)
    for raw in open(inventory).read().rstrip("\n").split("\n")[1:]:
        _, _, _, location, role, _ = raw.split("\t")
        h, _, line = location.rpartition(":")
        declared[h].append((int(line), role))
    counts = collections.Counter()
    for h in headers:
        lines = texts[h].split("\n")
        _, _, position = check.census(texts[h])
        stack, head, scope = [], "", {}
        for number, line in enumerate(lines, 1):
            scope[number] = [kind for kind, _ in stack]
            if line.strip().startswith("#"):
                continue
            for c in check.code_text(line):
                if c == "{":
                    inside = any(kind == "other" for kind, _ in stack)
                    stack.append(("other" if inside else check.scope_kind(head), number))
                    head = ""
                elif c == "}":
                    stack.pop()
                    head = ""
                elif c == ";":
                    head = ""
                else:
                    head += c
            head += " "
        blocks, run = [], []
        for number, line in enumerate(lines, 1):
            if line.strip().startswith("///"):
                run.append(number)
            elif run:
                blocks.append(run)
                run = []
        for i, b in enumerate(blocks):
            if position[b[0]] != "public":
                continue
            where = "member" if "class" in scope[b[0]] else "namespace"
            after = blocks[i + 1][0] if i + 1 < len(blocks) else len(lines) + 1
            roles = [role for line, role in sorted(declared.get(h, []))
                     if b[-1] < line < after]
            counts[(where, roles[0] == "rejects" if roles else False)] += 1
    return counts


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--compare", metavar="REV")
    parser.add_argument("table", nargs="?",
                        default="docs/agent/records/docs/comment-moves.tsv")
    args = parser.parse_args()

    records = read_table(open(args.table).read())
    headers = sorted({r["header"] for r in records})
    texts = {h: check.git("show", f"{REVISION}:{h}") for h in headers}
    lines_of = {h: t.split("\n") for h, t in texts.items()}
    full = [r for r in records if r["position"] != "trailing"]
    total = sum(len(r["line_list"]) for r in full)
    out = []

    def say(label, value):
        out.append(f"{label}\t{value}")

    say("rows", len(records))
    say("full-line comment lines", total)
    say("trailing comments", sum(len(r["line_list"]) for r in records
                                 if r["position"] == "trailing"))
    say("places kept in full (KEPT_IN_FULL)", len(check.KEPT_IN_FULL))
    say("boundary report entries", len(check.boundary_report(texts, records)))

    # What the rows wait for, with the delete lines of each.
    waits = collections.Counter()
    deletes = collections.Counter()
    for r in full:
        key = r["waits_for"]
        if key == "none":
            key = "none (kept)" if r["class"] == "keep" else "none (delete or summary now)"
        waits[key] += len(r["line_list"])
        if r["class"] == "delete":
            deletes[key] += len(r["line_list"])
    for key in sorted(waits):
        say(f"waits for {key}: lines, of which delete", f"{waits[key]}, {deletes[key]}")

    # Kinds the prose quotes.
    kind = collections.Counter()
    for r in full:
        kind[r["kind"]] += len(r["line_list"])
    say("narration lines", kind["narration"])
    weight = sum(kind[k] for k in ("public contract", "internal contract",
                                   "reason", "equation or convention"))
    say("contracts, reasons and equations", f"{weight} of {total}, {percent(weight, total)}")

    # Stages.
    met = set()
    stage_left = []
    by_group = []
    for name, conditions in STAGES:
        met |= conditions
        per = collections.Counter()
        for r in full:
            per[group(r, lines_of[r["header"]])] += left(r, met)
        stage_left.append(sum(per.values()))
        by_group.append(per)
        say(f"stage: {name}", f"{stage_left[-1]}, "
            f"{percent(stage_left[-1] - total, total)}")

    # The candidates of plan 0048 section 5.4.
    after_d3, after_amendment = by_group[1], by_group[2]
    for g in ("prologue", "/// detail", "//", "/// public"):
        say(f"candidate {CANDIDATE[g]} ({g}): after D3, after the amendment, "
            "gap to the plan's (b)",
            f"{after_d3[g]}, {after_amendment[g]}, {after_amendment[g] - PLAN_B[g]}")
    plan_class = collections.Counter()
    for r in full:
        plan_class[(group(r, lines_of[r["header"]]), r["kind"], r["class"])] += len(r["line_list"])
    for g in ("prologue", "//"):
        for c in ("delete", "move"):
            say(f"{g} lines that {c}",
                sum(v for (gg, _, cc), v in plan_class.items() if gg == g and cc == c))
    say("// marker lines kept", plan_class[("//", "marker", "keep")])

    # Pointers.
    blocks = doc_blocks(headers, texts)
    namespace = blocks[("namespace", False)] + blocks[("namespace", True)]
    rejecting = blocks[("namespace", True)]
    members = blocks[("member", False)] + blocks[("member", True)]
    say("public /// blocks at namespace scope, on rejecting overloads, on members",
        f"{namespace}, {rejecting}, {members}")
    every = namespace + members
    per_entry = namespace - rejecting
    say("pointer lines: on every declaration, one per reference entry (ruled)",
        f"{every}, {per_entry}")
    end = stage_left[2] + per_entry
    say("end state with one pointer per reference entry",
        f"{end}, {percent(end - total, total)}")
    say("restated all lines: before D3, after the amendment, the same with C1",
        f"{CODE_AND_BLANK + stage_left[0]}, {CODE_AND_BLANK + end}, "
        f"{CODE_AND_BLANK_C1 + end}")

    # Preconditions kept in full on public /// declarations.
    by_line = {(r["header"], l): r for r in full for l in r["line_list"]}
    public_places = 0
    for h, spec in check.KEPT_IN_FULL:
        first = check.parse_lines(spec)[0]
        if group(by_line[(h, first)], lines_of[h]) == "/// public":
            public_places += 1
    say("places kept in full on public declarations (///, outside detail)", public_places)

    # The tighter pass: every kept /// detail row cut to two lines, and the
    # performance-enum-size suppressions with their reasons.
    cut = sum(max(0, len(r["line_list"]) - 2) for r in full
              if r["class"] == "keep" and group(r, lines_of[r["header"]]) == "/// detail")
    enum = 0
    for h, text in lines_of.items():
        for number, line in enumerate(text, 1):
            if "NOLINTNEXTLINE(performance-enum-size)" in line:
                enum += 1 + (text[number - 2].strip().startswith("//"))
    say("tighter pass: detail rows cut to two lines, enum-size lines, end state",
        f"{cut}, {enum}, {end - cut - enum}")

    # Targets the prose quotes.
    repeats = collections.Counter(r["target"] for r in records
                                  if r["class"] == "delete" and r["target"] in GUIDE_REPEATS)
    say("delete rows citing core.md, rga3.md, README.md",
        f"{sum(repeats.values())}: " + ", ".join(str(repeats[p]) for p in GUIDE_REPEATS))
    to_reference = [r for r in full if r["class"] == "move"
                    and r["target"].startswith("docs/guide/reference/")]
    say("lines moving to reference pages, of which public contract",
        f"{sum(len(r['line_list']) for r in to_reference)}, "
        f"{sum(len(r['line_list']) for r in to_reference if r['kind'] == 'public contract')}")
    say("lines moving to the diagnostics page",
        sum(len(r["line_list"]) for r in full
            if r["class"] == "move" and r["target"] == "docs/guide/diagnostics.md"))

    print("\n".join(out))

    if args.compare:
        before = read_table(check.git("show", f"{args.compare}:{args.table}"))
        old = {(r["header"], r["lines"]): r for r in before}
        changes = collections.defaultdict(list)
        for r in records:
            o = old.get((r["header"], r["lines"]))
            if o is None:
                changes[("new row", "")].append(r)
            elif (o["class"], o["waits_for"]) != (r["class"], r["waits_for"]):
                changes[(f"{o['class']}, {o['waits_for']}",
                         f"{r['class']}, {r['waits_for']}")].append(r)
        print(f"\nRows changed since {args.compare}:")
        for (was, now), rs in sorted(changes.items()):
            print(f"{was} -> {now}\t{len(rs)} rows, "
                  f"{sum(len(r['line_list']) for r in rs)} lines\t"
                  + ", ".join(f"{r['header'].split('/')[-1]}:{r['lines']}" for r in rs))


if __name__ == "__main__":
    main()
