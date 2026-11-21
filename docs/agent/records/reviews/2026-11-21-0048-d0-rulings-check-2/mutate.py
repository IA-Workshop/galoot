"""Run the author's checker on two altered copies of the table.

Usage: python3 -I mutate.py WORKTREE OUTDIR

WORKTREE is a checkout of records/comment-table-rulings; the checker runs from
its root. OUTDIR receives the altered tables and the checker's output.
1. demoted: mv.hpp:1281-1283, a place kept in full, made a delete. Must fail.
2. reverted: transform.hpp:59-65, one of the eleven rows, set back to wait for
   nothing. The checker does not judge contract text, so this passes.
"""
import pathlib
import subprocess
import sys

tree, out = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
out.mkdir(parents=True, exist_ok=True)
table = (tree / "docs/agent/records/docs/comment-moves.tsv").read_text().split("\n")


def altered(header, lines, change):
    result, hits = [], 0
    for line in table:
        f = line.split("\t")
        if len(f) > 8 and f[0] == header and f[1] == lines:
            change(f)
            hits += 1
        result.append("\t".join(f))
    assert hits == 1
    return "\n".join(result)


def demote(f):
    assert f[4] == "keep"
    f[4], f[6], f[7] = "delete", "docs/guide/core.md", "Reading and writing coefficients"


def revert(f):
    f[8] = "none"


cases = {
    "demoted": altered("include/galoot/core/mv.hpp", "1281-1283", demote),
    "reverted": altered("include/galoot/core/transform.hpp", "59-65", revert),
}
for name, text in cases.items():
    path = out / f"{name}.tsv"
    path.write_text(text)
    run = subprocess.run(
        [sys.executable, "-I", "docs/agent/records/docs/check_comment_moves.py", str(path)],
        cwd=tree, capture_output=True, text=True)
    (out / f"{name}.out").write_text(run.stdout + run.stderr)
    problems = [l for l in run.stdout.splitlines() if l.startswith("row ")]
    print(f"{name}: exit {run.returncode}; {len(problems)} problem lines")
    for l in problems:
        print("  " + l)
