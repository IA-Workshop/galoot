"""Independent checks of the comment table after the owner's rulings.

Run from the repository root of a checkout of records/comment-table-rulings
(or of a branch made from it):

    python3 -I docs/agent/records/reviews/2026-11-21-0048-d0-rulings-check/recheck.py changed
    python3 -I .../recheck.py namespaces
    python3 -I .../recheck.py figures
    python3 -I .../recheck.py mutate SCRATCH_DIR

changed     rows whose class, target, heading or waits_for differ from main
namespaces  a brace scanner of its own (not the census rule) that says for
            each row whether its lines are inside a detail namespace; prints
            the rows where that disagrees with the position column, and the
            detail rows that still wait for the S11 amendment
figures     the README's sums, stage figures and quoted counts, recomputed
            from the table and the headers at 01dce7f
mutate      writes altered copies of the table and README to SCRATCH_DIR and
            runs the author's checker on each; every one must fail
"""
import collections
import csv
import io
import os
import subprocess
import sys

REV = "01dce7f"
TABLE = "docs/agent/records/docs/comment-moves.tsv"
README = "docs/agent/records/docs/README.md"
CHECKER = "docs/agent/records/docs/check_comment_moves.py"
CLASSES = ("keep", "summary", "move", "delete")


def git(*args):
    return subprocess.run(["git", *args], capture_output=True, text=True,
                          check=True).stdout


def load(text):
    return list(csv.DictReader(io.StringIO(text), delimiter="\t"))


def lines_of(r):
    out = []
    for part in r["lines"].split(","):
        a, _, b = part.partition("-")
        out.extend(range(int(a), int(b or a) + 1))
    return out


_texts = {}


def header(h):
    if h not in _texts:
        _texts[h] = git("show", f"{REV}:{h}").split("\n")
    return _texts[h]


def strip_code(line, state):
    """Code of a line with comments and literals removed; state carries /* */."""
    out, i, n = [], 0, len(line)
    while i < n:
        if state["block"]:
            j = line.find("*/", i)
            if j < 0:
                return "".join(out)
            state["block"], i = False, j + 2
            continue
        if line.startswith("//", i):
            break
        if line.startswith("/*", i):
            state["block"], i = True, i + 2
            continue
        c = line[i]
        if c in "\"'":
            i += 1
            while i < n and line[i] != c:
                i += 2 if line[i] == "\\" else 1
            i += 1
            out.append(" ")
            continue
        out.append(c)
        i += 1
    return "".join(out)


def namespaces(lines):
    """For each line (0-based), the names of the namespaces open at its start.
    A namespace alias (namespace x = y;) opens nothing."""
    import re
    state, stack, head, result = {"block": False}, [], "", []
    for line in lines:
        result.append(tuple(s for s in stack if s))
        code = strip_code(line, state)
        if code.lstrip().startswith("#"):
            continue
        for piece in re.split(r"([{};])", code):
            if piece == "{":
                m = re.search(r"\bnamespace\s*([\w:]*)\s*$", head.strip())
                stack.append((m.group(1) or "<anonymous>") if m else None)
                head = ""
            elif piece == "}":
                if stack:
                    stack.pop()
                head = ""
            elif piece == ";":
                head = ""
            else:
                head += piece
    return result


def cmd_changed():
    old = {(r["header"], r["lines"]): r for r in load(git("show", f"main:{TABLE}"))}
    new = load(open(TABLE).read())
    assert {(r["header"], r["lines"]) for r in new} == set(old)
    for r in new:
        o = old[(r["header"], r["lines"])]
        diff = [f for f in ("class", "target", "heading", "waits_for") if o[f] != r[f]]
        if diff or o["note"] != r["note"]:
            print(f"{r['header']}:{r['lines']} ({r['position']}, {r['kind']})")
            for f in diff:
                print(f"    {f}: {o[f]!r} -> {r[f]!r}")
            if not diff:
                print("    note only")


def cmd_namespaces():
    rows = load(open(TABLE).read())
    maps = {}
    for number, r in enumerate(rows, 2):
        if r["position"] == "trailing":
            continue
        h = r["header"]
        if h not in maps:
            maps[h] = namespaces(header(h))
        inside = [any("detail" in n for n in maps[h][l - 1]) for l in lines_of(r)]
        mine = "detail" if all(inside) else "not detail" if not any(inside) else "mixed"
        if r["position"] != "prologue" and (r["position"] == "detail") != (mine == "detail"):
            print(f"position differs: table row {number} {h}:{r['lines']} is "
                  f"{r['position']}, the scanner says {mine}")
        if mine != "not detail" and "S11" in r["waits_for"]:
            print(f"detail row waiting for the amendment: {number} {h}:{r['lines']} "
                  f"{r['kind']}, {r['class']}, {r['waits_for']}")


def plan_group(r, line):
    if r["position"] == "prologue":
        return "D1 prologues"
    if not header(r["header"])[line - 1].strip().startswith("///"):
        return "D3 //"
    return "D2 /// detail" if r["position"] == "detail" else "D4 /// public"


MET = (set(), {"none", "B9"}, {"none", "B9", "D1", "D3"},
       {"none", "B9", "D1", "D3", "the S11 amendment", "D3 and the S11 amendment"})


def left_at(full, stage):
    """Kept lines by plan group once the conditions of the stage are met: keep
    rows, two lines of each acted-on summary row, every row not yet acted on."""
    g = collections.Counter()
    for r in full:
        lines = lines_of(r)
        acted = r["waits_for"] in MET[stage]
        if r["class"] == "keep" or not acted:
            for l in lines:
                g[plan_group(r, l)] += 1
        elif r["class"] == "summary":
            g[plan_group(r, lines[0])] += min(2, len(lines))
    return g


def cmd_figures():
    rows = load(open(TABLE).read())
    full = [r for r in rows if r["position"] != "trailing"]
    n = lambda rs: sum(len(lines_of(r)) for r in rs)
    print("rows", len(rows), "full-line lines", n(full),
          "trailing", n([r for r in rows if r["position"] == "trailing"]))
    for axis in ("header", "kind"):
        print(f"\nby {axis}: keep summary move delete all")
        t = collections.defaultdict(collections.Counter)
        for r in full:
            t[r[axis]][r["class"]] += len(lines_of(r))
        for k in sorted(t):
            print(f"  {k}: {[t[k][c] for c in CLASSES]} {sum(t[k].values())}")
        total = sum(t.values(), collections.Counter())
        print(f"  all: {[total[c] for c in CLASSES]} {sum(total.values())}")
    print("\nby waits_for: lines, of which delete")
    w = collections.defaultdict(lambda: [0, 0])
    for r in full:
        k = r["waits_for"]
        if k == "none":
            k += " (kept)" if r["class"] == "keep" else " (delete or summary now)"
        w[k][0] += len(lines_of(r))
        w[k][1] += len(lines_of(r)) if r["class"] == "delete" else 0
    for k, v in sorted(w.items()):
        print(f"  {k}: {v}")
    print("\nstages (kept lines by plan group, total, change)")
    for s in range(4):
        g = left_at(full, s)
        total = sum(g.values())
        print(f"  stage {s}: {dict(sorted(g.items()))} {total} {(total - 3208) / 3208:+.1%}")
    end = sum(left_at(full, 3).values())
    print(f"  with 120 pointers: {end + 120} {(end + 120 - 3208) / 3208:+.1%}")
    print(f"  restated all lines: {4320 + sum(left_at(full, 1).values())}, "
          f"{4320 + end + 120}, with C1 {4150 + end + 120}")
    kinds = collections.Counter()
    for r in full:
        kinds[r["kind"]] += len(lines_of(r))
    c = kinds["public contract"] + kinds["internal contract"] + kinds["reason"] \
        + kinds["equation or convention"]
    print(f"\ncontracts, reasons, equations: {c} ({c / 3208:.1%}); narration {kinds['narration']}")
    print("prologue lines deleted:", sum(1 for r in full if r["position"] == "prologue"
                                          and r["class"] == "delete" for _ in lines_of(r)))
    print("// lines deleted, moved, marker kept:",
          *(sum(1 for r in full for l in lines_of(r) if plan_group(r, l) == "D3 //" and p(r))
            for p in (lambda r: r["class"] == "delete", lambda r: r["class"] == "move",
                      lambda r: r["class"] == "keep" and r["kind"] == "marker")))
    print("detail /// keep rows cut to two lines saves:",
          sum(max(0, len(lines_of(r)) - 2) for r in full if r["class"] == "keep"
              and plan_group(r, lines_of(r)[0]) == "D2 /// detail"))
    print("enum-size markers and reasons:",
          n([r for r in full if "performance-enum-size" in r["descriptor"]
             or r["descriptor"].startswith("An int, so a compiler prints")]))
    ref = [r for r in full if r["class"] == "move" and r["target"].startswith("docs/guide/reference/")]
    print("to reference pages:", n(ref), "of which public contract:",
          n([r for r in ref if r["kind"] == "public contract"]))
    print("to the diagnostics page:",
          n([r for r in full if r["class"] == "move" and r["target"] == "docs/guide/diagnostics.md"]))
    cites = collections.Counter(r["target"] for r in full if r["class"] == "delete" and r["target"]
                                in ("docs/guide/core.md", "docs/guide/rga3.md", "docs/guide/README.md"))
    print("delete rows citing core.md, rga3.md, README.md:", sum(cites.values()), dict(cites))
    old = load(git("show", f"main:{TABLE}"))
    for value in ("the detail ruling", "D3 and the detail ruling"):
        rs = [r for r in old if r["waits_for"] == value]
        print(f"on main, waiting for {value!r}: {len(rs)} rows, {n(rs)} lines,",
              {c: n([r for r in rs if r["class"] == c]) for c in CLASSES})


def cmd_mutate(scratch):
    os.makedirs(scratch, exist_ok=True)
    table = open(TABLE).read().split("\n")
    readme = open(README).read()

    def altered(key, values):
        out = []
        for line in table:
            f = line.split("\t")
            if len(f) > 2 and f[0].endswith(key[0]) and f[1] == key[1]:
                for i, v in values.items():
                    f[i] = v
            out.append("\t".join(f))
        return "\n".join(out)

    rotation = ("motions.hpp", "48-61")
    to_move = {4: "move", 6: "docs/guide/reference/rga3.md", 7: "rotation",
               8: "D3 and the S11 amendment"}
    first_table_line = next(l for l in readme.split("\n")
                            if l.startswith("| `presets/rga3/motions.hpp:48-61`"))
    no_line = readme.replace(first_table_line + "\n", "")
    in_second = no_line.replace("| `presets/rga3/motions.hpp:62-68` |",
                                "| `presets/rga3/motions.hpp:48-61` | moved | here |\n"
                                "| `presets/rga3/motions.hpp:62-68` |")
    old_wait = "\n".join(l.replace("\tnone\t", "\tthe detail ruling\t", 1)
                         if "plan.hpp\t103-104" in l else l for l in table)
    cases = {
        "rotation to move": (altered(rotation, to_move), readme),
        "rotation to delete": (altered(rotation, {4: "delete", 6: "docs/guide/rga3.md",
                                                  7: "Rotation and translation",
                                                  8: "the S11 amendment"}), readme),
        "rotation to move, README line deleted": (altered(rotation, to_move), no_line),
        "rotation to move, README line in the second table": (altered(rotation, to_move), in_second),
        "README line deleted alone": ("\n".join(table), no_line),
        "min() / -1 to summary": (altered(("linear.hpp", "407-411"),
                                          {4: "summary", 6: "docs/guide/reference/multivectors.md",
                                           7: "operator/", 8: "D3 and the S11 amendment"}), readme),
        "a row waits for the detail ruling again": (old_wait, readme),
    }
    for number, (name, (t, r)) in enumerate(cases.items(), 1):
        tp, rp = (os.path.join(scratch, f"case{number}.{e}") for e in ("tsv", "md"))
        open(tp, "w").write(t)
        open(rp, "w").write(r)
        run = subprocess.run([sys.executable, "-I", CHECKER, "--readme", rp, tp],
                             capture_output=True, text=True)
        problems = [l for l in run.stdout.split("\n")
                    if l and not l.startswith(("include/", "header\t", "Sentences"))]
        print(f"{name}: exit {run.returncode}; {problems[0] if problems else 'no problem printed'}")


if __name__ == "__main__":
    command = sys.argv[1] if len(sys.argv) > 1 else ""
    if command == "mutate":
        cmd_mutate(sys.argv[2])
    else:
        {"changed": cmd_changed, "namespaces": cmd_namespaces, "figures": cmd_figures}[command]()
