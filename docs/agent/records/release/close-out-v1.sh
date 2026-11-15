#!/usr/bin/env bash
# Closes out Galoot core 1.0.0 from the main checkout on `develop`:
# applies the prepared CONVENTIONS 1.18 text, runs the two gates, merges
# `develop` into `master` in a temporary worktree, and tags 1.0.0.
# Nothing is pushed. Any failed check stops the script before the next step.
# `close-out-v1.sh --dry-run` applies the text to a scratch copy and stops.
set -euo pipefail
cd /home/brianm/projects/utils/math-core/galoot
T=ff1d49f
CONV=docs/agent/governance/CONVENTIONS.md
TEXT=docs/agent/records/release/conventions-1.18-owner-text.md

apply_edits() {  # $1 = file to edit
  python3 -I - "$TEXT" "$1" "$(date +%F)" <<'PY'
import re, sys
text_path, conv_path, today = sys.argv[1:4]
text = open(text_path, encoding="utf-8").read()
blocks = {}
for n in "1234":
    m = re.search(r"^## %s\. .*?\n.*?^```\n(.*?)\n```\n" % n, text, re.S | re.M)
    blocks[n] = m.group(1)
conv = open(conv_path, encoding="utf-8").read()

def replace_once(s, old, new):
    assert s.count(old) == 1, "expected exactly one match of: " + old[:60]
    return s.replace(old, new)

conv = replace_once(conv, "Status: version 1.17, in which", blocks["1"])
conv = replace_once(conv, "The design study is the most canonical reference for Galoot.", blocks["2"])
lines = conv.split("\n")
i = [k for k, l in enumerate(lines) if l.startswith("| Conformal duals |")]
assert len(i) == 1
lines.insert(i[0] + 1, blocks["3"])
h = [k for k, l in enumerate(lines) if l == "## Revision history"]
assert len(h) == 1
k = h[0] + 1
while not lines[k].startswith("|"):
    k += 1
while k < len(lines) and lines[k].startswith("|"):
    k += 1
row = blocks["4"].replace("| 1.18 | 2026-11-12 |", "| 1.18 | %s |" % today)
lines.insert(k, row)
open(conv_path, "w", encoding="utf-8").write("\n".join(lines))
PY
}

if [ "${1:-}" = "--dry-run" ]; then
  s=$(mktemp -d); cp "$CONV" "$s/CONVENTIONS.md"; apply_edits "$s/CONVENTIONS.md"
  diff -u "$CONV" "$s/CONVENTIONS.md" | grep -c '^[+-][^+-]' || true
  grep -c "^Status: version 1.18" "$s/CONVENTIONS.md"
  echo "dry run ok: $s/CONVENTIONS.md"; exit 0
fi

[ "$(git branch --show-current)" = develop ] || { echo "not on develop"; exit 1; }
[ -z "$(git status --short)" ] || { echo "working tree not clean"; exit 1; }
git merge-base --is-ancestor "$T" develop
[ -z "$(git diff --stat "$T" develop -- . ':!docs/agent')" ]
[ "$(git diff --name-only "$T" develop -- docs/agent/governance)" = "" ]

apply_edits "$CONV"
[ "$(git status --short)" = " M $CONV" ]
git commit -q -m "CONVENTIONS 1.18: the motor coefficient names row and the sources sentence" -- "$CONV"
tools/timed trace - gate:trace@conventions-1.18 -- python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md "$CONV" test/reference/golden.hpp test
tools/timed check - gate:runlog@conventions-1.18 -- tools/runlog check
git commit -q -m "Records: trace gate and run-log check after CONVENTIONS 1.18" -- docs/agent/records/performance/runs.tsv

# Release checks (plan 0042, section 8).
[ -z "$(git diff --stat "$T" develop -- . ':!docs/agent')" ]
[ "$(git diff --name-only "$T" develop -- docs/agent/governance)" = "$CONV" ]
[ "$(git show develop:$CONV | sed -n 's/^Status: version \([0-9.]*\),.*/\1/p')" = 1.18 ]
C=$(git rev-parse --short=12 "$(git log -1 --format=%H develop -- "$CONV")")
n=$(git show develop:docs/agent/records/performance/runs.tsv | awk -F'\t' -v c="$C" '$2 == c && $4 == "trace" && $8 == 0' | wc -l)
[ "$n" -ge 1 ]
git merge-base --is-ancestor master develop

W=/home/brianm/projects/utils/math-core/galoot-master
git worktree add "$W" master
git -C "$W" merge --no-ff develop -m "Release galoot core 1.0.0"
[ -z "$(git diff develop master --stat)" ]
git tag -a 1.0.0 master -m "galoot core 1.0.0, CONVENTIONS 1.18"
git worktree remove "$W"
echo "done: master is at $(git rev-parse --short master), tag 1.0.0 -> $(git rev-parse --short 1.0.0^{commit}). Nothing was pushed."
