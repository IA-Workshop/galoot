#!/usr/bin/env bash
# The merge gate of feature/header-hygiene-1-0-1 (plan 0044 section 9): the
# five presets one at a time, each step under memguard, then format-check,
# the trace gate and tools/runlog check. No timing lock: this is not a
# measurement; the W10 rows run separately under the lock.
set -u
rev=${1:?revision}
sel="gate:all@$rev"
wt=/home/brianm/projects/utils/math-core/galoot-wt/header-hygiene
out=out
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
mkdir -p "$out"
cd "$wt" || exit 1
[ "$(git rev-parse --short=7 HEAD)" = "$rev" ] || { echo "HEAD is not $rev"; exit 1; }
export MEMGUARD_MIN_GB=15 MEMGUARD_MAX_GB=60
pool() {
  local s; s=$(tools/jobpool status | head -1)
  if [[ $s == "not running"* ]]; then
    echo "$(date -u +%T) pool not running; ensure"; tools/jobpool ensure
    s=$(tools/jobpool status | head -1)
  fi
  echo "$s"
}
conditions() {
  { date -u +%FT%TZ; uptime; tools/jobpool status; free -m;
    top -b -n 1 -w 200 | sed -n 1,25p; } > "$out/conditions-$1.txt" 2>&1
}
step() {  # step preset command...
  local s=$1 p=$2; shift 2
  echo "$(date -u +%T) $p $s start; pool: $(pool)"
  eval "$(tools/jobpool env)"
  bash $pe/memguard.sh "$out/memguard-$p-$s.tsv" -- \
    tools/timed "$s" "$p" "$sel" -- "$@" > "$out/$s-$p.log" 2>&1
  local rc=$?
  echo "$(date -u +%T) $p $s exit $rc; pool: $(tools/jobpool status | head -1)"
  return $rc
}
echo "$(date -u +%FT%TZ) gate start at $rev"
conditions start
for p in gcc-debug clang-debug gcc-asan clang-measure lint; do
  tree=$wt/build/$p
  if [ -e "$tree" ]; then echo "$p: existing tree (incremental)"; else echo "$p: empty tree"; fi
  n0=$(wc -l < "$tree/compile.tsv" 2>/dev/null || echo 0)
  echo "$p compile.tsv rows before: $n0"
  conditions "$p-start"
  step configure "$p" cmake --preset "$p"
  step build "$p" cmake --build --preset "$p"
  cp "$tree/compile.tsv" "$out/compile-$p-after-build.tsv" 2>/dev/null
  step test "$p" ctest --preset "$p"
  conditions "$p-end"
done
echo "$(date -u +%T) format-check; pool: $(pool)"
eval "$(tools/jobpool env)"
tools/timed build lint "gate:format-check@$rev" -- \
  cmake --build --preset lint --target format-check > "$out/format-check.log" 2>&1
echo "format-check exit $?"
tools/timed trace - "gate:trace@$rev" -- python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test > "$out/trace.log" 2>&1
echo "trace exit $?"
conditions end
tools/timed check - "gate:runlog@$rev" -- tools/runlog check > "$out/runlog.log" 2>&1
echo "runlog exit $?"
git status --short
echo "$(date -u +%FT%TZ) done; pool: $(tools/jobpool status | head -1)"
