#!/usr/bin/env bash
# For each local branch, count run-log rows it holds that the given tip lacks,
# and say whether the branch is an ancestor of the tip.
set -euo pipefail
tip=$1
log=docs/agent/records/performance/runs.tsv
tmp=$(mktemp -d)
git show "$tip:$log" | sort -u > "$tmp/tip"
printf 'branch\tahead_of_tip\tmerged\trows_tip_lacks\n'
git for-each-ref --format='%(refname:short)' refs/heads | while read -r b; do
  if git merge-base --is-ancestor "$b" "$tip"; then m=yes; else m=no; fi
  ahead=$(git rev-list --count "$tip..$b")
  if git cat-file -e "$b:$log" 2>/dev/null; then
    n=$(git show "$b:$log" | sort -u | comm -23 - "$tmp/tip" | wc -l)
  else n=nolog; fi
  printf '%s\t%s\t%s\t%s\n' "$b" "$ahead" "$m" "$n"
done
rm -rf "$tmp"
