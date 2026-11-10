#!/usr/bin/env bash
# Cheap gates at each commit of the delta, in a detached worktree.
set -u
G=/home/brianm/projects/utils/math-core/galoot
W=$G/.claude/worktrees/wu17-part-c-delta
T=$G/.claude/worktrees/wu17-part-c-delta-steps
LOG=$W/docs/agent/records/performance/runs.tsv
git -C $G worktree add --detach $T 4bb9195 >/dev/null 2>&1
cd $T
for c in 4bb9195 5295358 bc8a7e2 5354189 38a711f 0dfc35a; do
  git checkout -q --detach $c
  r=()
  t() { local name=$1; shift; $W/tools/timed --log $LOG check - "$name" -- "$@" >/dev/null 2>&1; r+=("$name=$?"); }
  t markers python3 -I tools/test_references.py markers test
  t golden-facts python3 -I tools/test_references.py golden test/reference/golden.hpp test
  t golden-check python3 -I test/reference/golden.py --check test/reference/golden.hpp
  t trace-gate python3 -I tools/test_references.py trace docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md test/reference/golden.hpp test
  t citations python3 -I tools/test_references.py citations --exclude tools/citation_patterns.txt --exclude test/tools/test_references/markers_bad.cpp --exclude test/tools/test_references/markers_bad.expected --exclude test/tools/test_references/trace_bad_table.expected --exclude test/tools/test_references/trace_empty_table.expected --exclude test/tools/test_references/citations_bad --exclude test/tools/test_references/citations_bad.expected include/galoot cmake README.md test tools CMakeLists.txt CMakePresets.json .clang-format .clang-tidy
  t format clang-format --dry-run -Werror test/core/transform_counts_test.cpp
  echo "$c ${r[*]}"
done
cd $G
git worktree remove $T
