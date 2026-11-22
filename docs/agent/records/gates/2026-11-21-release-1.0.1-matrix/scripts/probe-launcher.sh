#!/usr/bin/env bash
# The user-space instructions of the git calls the compile launcher makes for
# each recorded compile (tools/measure: rev-parse, status and branch, with the
# status read before and after the command), and of tools/measure run as the
# compile launcher on a command that does nothing, five times each, in the
# release worktree, under the timing lock. A size for the part of a build row
# that the compile rows do not count.
set -u
wt=/home/brianm/projects/utils/math-core/galoot-wt/release-1.0.1
out=$wt/docs/agent/records/gates/2026-11-21-release-1.0.1-matrix/out/probe-launcher.txt
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
cd "$wt" || exit 1
LOCK_OWNER="galoot release 1.0.1 matrix (launcher probe) $(date -u +%FT%TZ)" \
  bash $pe/lock.sh take 3600 || { echo "lock not taken"; exit 1; }
count() {  # count label command...
  local l=$1; shift
  for i in 1 2 3 4 5; do
    # Both core types' counters, a core type that never ran the command as 0.
    printf '%s\t%s\n' "$l" "$(perf stat -x, -e instructions:u -- "$@" 2>&1 >/dev/null |
      awk -F, '/instructions/ { s += ($1 ~ /^[0-9]+$/) ? $1 : 0 } END { print s }')"
  done
}
{
  date -u +%FT%TZ; uptime; git rev-parse --short=12 HEAD
  count rev-parse git rev-parse --short=12 HEAD
  count status git --no-optional-locks status --porcelain --untracked-files=no -- \
    ':(top)' ':(top,exclude)docs/agent/records/performance'
  count branch git branch --show-current
  scratch=$(mktemp -d)
  count launcher-true tools/measure tu probe true "$scratch/compile.tsv" -- true
  rm -rf -- "$scratch"
} > "$out" 2>&1
bash $pe/lock.sh release
cat "$out"
