"""Rebuild runs.tsv after a merge that the union driver doubled. Snippet
candidate.

Why: WU1 extended the header of docs/agent/records/performance/runs.tsv
once, in place, and wrote `--` into the new cells of every earlier row
(decision 0020, item 3). That rewrote every line, so a merge with a branch
that only appended rows becomes one conflict, and the union merge driver
(.gitattributes) keeps both versions: two headers, and the old rows twice.
This script writes the file as it should be: the side with the current
header, then the rows the other side appended since the merge base, each row
padded with `--` to the current header (plan 0002, risk 10; lessons-learned
0001).

Usage: runs_tsv_merge.py <base> <side with the new header> <appending side>

Right after the merge commit, from the worktree root, with m=HEAD:

  f=docs/agent/records/performance/runs.tsv
  base=$(git merge-base "$m^1" "$m^2")
  # Merging develop into a branch that has the new header (develop appended):
  python3 -I docs/agent/snippets/runs_tsv_merge.py <(git show "$base:$f") \
    <(git show "$m^1:$f") <(git show "$m^2:$f") > "$PWD/$f.new"
  # Merging such a branch into develop, the sides swap: pass "$m^2:$f" as
  # the side with the new header and "$m^1:$f" as the appending side.
  mv "$PWD/$f.new" "$PWD/$f"

then check that the file has one header line and one field count, and
commit. It refuses (exit 1) if a row has more cells than the header, or if
the other side changed a row instead of appending.

Written for: the merge of feature/build (WU1) into develop, and each branch
that merges develop afterward while it still has old-format rows. Last
checked at the merge of develop into feature/build, 2026-10-31.
"""

import sys


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read().splitlines()


def main(base_path, new_path, appending_path):
    base, ours, theirs = read(base_path), read(new_path), read(appending_path)
    header = ours[0].split("\t")

    def padded(row):
        cells = row.split("\t")
        if len(cells) > len(header):
            sys.exit(f"row has more cells than the header: {row}")
        return "\t".join(cells + ["--"] * (len(header) - len(cells)))

    base_rows = set(padded(r) for r in base[1:])
    if any(padded(r) not in base_rows for r in theirs[1:len(base)]):
        sys.exit("the other side changed existing rows; merge by hand")
    out = [ours[0]] + [padded(r) for r in ours[1:]]
    out += [padded(r) for r in theirs[len(base):]]
    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit("usage: runs_tsv_merge.py <base> <new-header side> <appending side>")
    main(*sys.argv[1:])
