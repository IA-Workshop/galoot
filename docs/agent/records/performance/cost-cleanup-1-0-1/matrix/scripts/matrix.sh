#!/usr/bin/env bash
# The merge matrix of feature/cost-cleanup-1-0-1: all eleven presets from empty
# trees, one at a time, each step under memguard, then format-check, the trace
# gate and tools/runlog check. Holds the timing lock for the whole run.
set -u
rev=05229a7
sel="gate:all@$rev"
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup
out=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/matrix
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
mkdir -p "$out"
cd "$wt" || exit 1
[ "$(git rev-parse --short=7 HEAD)" = "$rev" ] || { echo "HEAD is not $rev"; exit 1; }
eval "$(tools/jobpool env)"
export MEMGUARD_MIN_GB=15 MEMGUARD_MAX_GB=60
conditions() {
  { date -u +%FT%TZ; uptime; tools/jobpool status; free -m;
    top -b -n 1 -w 200 | sed -n 1,25p; } > "$out/conditions-$1.txt" 2>&1
}
step() {  # step preset command...
  local s=$1 p=$2; shift 2
  bash $pe/memguard.sh "$out/memguard-$p-$s.tsv" -- \
    tools/timed "$s" "$p" "$sel" -- "$@" > "$out/$s-$p.log" 2>&1
  local rc=$?
  echo "$(date -u +%T) $p $s exit $rc; pool: $(tools/jobpool status | head -1)"
  return $rc
}
LOCK_OWNER="galoot cost-cleanup-1-0-1 merge matrix $(date -u +%FT%TZ)" \
  bash $pe/lock.sh take 7200 || { echo "lock not taken"; exit 1; }
echo "$(date -u +%FT%TZ) lock taken; pool: $(tools/jobpool status | head -1)"
conditions start
for p in gcc-debug clang-debug gcc-release gcc-asan clang-release clang-asan \
         lint gcc-measure gcc-measure-o3 clang-measure clang-measure-o3; do
  tree=$wt/build/$p
  [ -e "$tree" ] && { echo "$tree exists; not an empty tree"; continue; }
  conditions "$p-start"
  step configure "$p" cmake --preset "$p"
  step build "$p" cmake --build --preset "$p"
  cp "$tree/compile.tsv" "$out/compile-$p-after-build.tsv" 2>/dev/null
  step test "$p" ctest --preset "$p"
  cp "$tree/compile.tsv" "$out/compile-$p-after-test.tsv" 2>/dev/null
  conditions "$p-end"
done
tools/timed build lint "gate:format-check@$rev" -- \
  cmake --build --preset lint --target format-check > "$out/format-check.log" 2>&1
echo "format-check exit $?"
tools/timed trace - "gate:trace@$rev" -- python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test > "$out/trace.log" 2>&1
echo "trace exit $?"
conditions end
bash $pe/lock.sh release
echo "$(date -u +%FT%TZ) lock released; pool: $(tools/jobpool status | head -1)"
tools/timed check - "gate:runlog@$rev" -- tools/runlog check > "$out/runlog.log" 2>&1
echo "runlog exit $?"
git status --short
echo done
