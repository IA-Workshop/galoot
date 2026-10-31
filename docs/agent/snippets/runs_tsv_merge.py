"""Rebuild a run or compile record that a union merge doubled. Snippet.

The records under docs/agent/records/ merge with git's union driver
(.gitattributes), which keeps the lines of both sides wherever both changed
the same region. When one side rewrote lines the other kept, as happened
once when every old row was padded with `--`, the whole file is one
conflict and the merged file holds two headers and the old rows twice. This
writes the file as it should be: the header that extends the other side's,
this side's rows, then the rows the other side appended since the merge
base. Rows are copied as they are and never padded; a reader treats the
cells a row lacks as `--` (`tools/measure read`).

Usage, from the worktree root, during `git merge --no-commit` and before the
merge is committed, so that no commit holds the doubled file:

  f=docs/agent/records/performance/runs.tsv
  base=$(git merge-base HEAD MERGE_HEAD)
  python3 -I docs/agent/snippets/runs_tsv_merge.py \
    <(git show "$base:$f" 2>/dev/null) <(git show "HEAD:$f") \
    <(git show "MERGE_HEAD:$f") > "$PWD/$f.new"
  mv "$PWD/$f.new" "$PWD/$f"
  tools/measure read "$PWD/$f" > /dev/null && git add "$f"

The base may lack the file. The script refuses, with exit 1 and nothing
written, when neither side's header extends the other's, when either side
changed or removed a row of the base instead of appending (a row that only
gained trailing `--` cells counts as unchanged), or when a row has more
cells than the header. `tools/measure read` then checks the result for a
repeated header and over-long rows.

Written for merges across the one-time extensions of the run and compile
record headers. Last checked on 2026-10-31 at 1082f97 (feature/recorder),
by scratch merges that doubled a record with padded rows and that added the
record on both sides of a base without it.
"""

import sys


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read().splitlines()


def unchanged(row, base_row):
    """Whether row is base_row, possibly with trailing `--` cells added."""
    if row == base_row:
        return True
    added = row[len(base_row):].split("\t")
    return row.startswith(base_row) and added[0] == "" and all(
        cell == "--" for cell in added[1:])


def main(base_path, ours_path, theirs_path):
    base, ours, theirs = read(base_path), read(ours_path), read(theirs_path)
    base_rows = base[1:]

    ours_columns, theirs_columns = ours[0].split("\t"), theirs[0].split("\t")
    if ours_columns[:len(theirs_columns)] == theirs_columns:
        header = ours[0]
    elif theirs_columns[:len(ours_columns)] == ours_columns:
        header = theirs[0]
    else:
        sys.exit("neither side's header extends the other's; merge by hand")

    for side, lines in (("this side", ours), ("the other side", theirs)):
        kept = lines[1:len(base_rows) + 1]
        if len(kept) < len(base_rows) or not all(
                unchanged(row, base_row)
                for row, base_row in zip(kept, base_rows, strict=True)):
            sys.exit(f"{side} changed or removed rows of the merge base; "
                     "merge by hand")

    out = [header] + ours[1:] + theirs[len(base_rows) + 1:]
    width = len(header.split("\t"))
    for row in out[1:]:
        if len(row.split("\t")) > width:
            sys.exit(f"row has more cells than the header: {row}")
    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit("usage: runs_tsv_merge.py <base> <this side> <other side>")
    main(*sys.argv[1:])
