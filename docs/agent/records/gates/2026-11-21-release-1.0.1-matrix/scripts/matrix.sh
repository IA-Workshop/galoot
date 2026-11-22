#!/usr/bin/env bash
# The release matrix of core 1.0.1 (plan 0045, unit R6), at the release
# commit R = 578aa15. Phase "presets": all eleven presets from empty trees, one
# at a time, each step under memguard, with the debug trees' compile rows
# copied into records/performance/compile.tsv by decision 0080's step (after
# the build and again after the suite), then format-check; the timing lock is
# held for this phase. Phase "checks": conventions_check, the plan cost model
# checks, jobserver_check and the 50-seed campaign. Phase "bench": the
# benchmarks once, pinned to CPU 4, under the timing lock. Phase "close": the
# trace gate and tools/runlog check.
# Usage: matrix.sh presets|checks|bench|close
set -u
R=578aa15
sel="gate:all@$R"
wt=/home/brianm/projects/utils/math-core/galoot-wt/release-1.0.1
rec=$wt/docs/agent/records/gates/2026-11-21-release-1.0.1-matrix
out=$rec/out
logs=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/r6
seeds=$wt/docs/agent/records/gates/2026-11-21-release-1.0.1-seeds.tsv
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
mkdir -p "$out" "$logs"
cd "$wt" || exit 1
# Only records may differ between R and the checkout.
[ -z "$(git diff --stat "$R" HEAD -- . ':!docs/agent')" ] || { echo "code differs from $R"; exit 1; }
[ -z "$(git status --porcelain --untracked-files=no -- . ':!docs/agent/records/performance')" ] ||
  { echo "tracked changes outside records/performance"; exit 1; }
export MEMGUARD_MIN_GB=15 MEMGUARD_MAX_GB=60
source "$rec/scripts/copy_compile_rows.sh"

pool() {  # print the pool's status; start it only if it is not running
  local s; s=$(tools/jobpool status | head -1)
  if [[ $s == "not running"* ]]; then
    echo "$(date -u +%T) pool not running; tools/jobpool ensure" >&2
    tools/jobpool ensure >&2
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
    tools/timed "$s" "$p" "$sel" -- "$@" > "$logs/$s-$p.log" 2>&1
  local rc=$?
  echo "$(date -u +%T) $p $s exit $rc; pool: $(tools/jobpool status | head -1)"
  return $rc
}
copy_rows() {  # copy_rows preset when
  local tree=$wt/build/$1
  cp "$tree/compile.tsv" "$out/compile-$1-after-$2.tsv" 2>/dev/null || return 0
  case $1 in
    gcc-debug | clang-debug)
      ( copy_compile_rows "$tree" docs/agent/records/performance/compile.tsv ) 2>&1 |
        sed "s/^/$1 after $2: /" ;;
  esac
}

case ${1:-} in
presets)
  LOCK_OWNER="galoot release 1.0.1 matrix (presets) $(date -u +%FT%TZ)" \
    bash $pe/lock.sh take 3600 || { echo "lock not taken"; exit 1; }
  echo "$(date -u +%FT%TZ) lock taken at $(git rev-parse --short=12 HEAD); pool: $(pool)"
  conditions start
  for p in gcc-debug clang-debug gcc-release gcc-asan clang-release clang-asan \
           lint gcc-measure gcc-measure-o3 clang-measure clang-measure-o3; do
    tree=$wt/build/$p
    [ -e "$tree" ] && { echo "$tree exists; not an empty tree; skipped"; continue; }
    conditions "$p-start"
    step configure "$p" cmake --preset "$p"
    step build "$p" cmake --build --preset "$p"
    copy_rows "$p" build
    step test "$p" ctest --preset "$p"
    copy_rows "$p" test
    grep -E "tests passed|tests failed" "$logs/test-$p.log" | tail -1
    conditions "$p-end"
  done
  echo "$(date -u +%T) format-check; pool: $(pool)"
  eval "$(tools/jobpool env)"
  tools/timed build lint "gate:format-check@$R" -- \
    cmake --build --preset lint --target format-check > "$logs/format-check.log" 2>&1
  echo "format-check exit $?"
  conditions end-presets
  bash $pe/lock.sh release
  echo "$(date -u +%FT%TZ) lock released; pool: $(tools/jobpool status | head -1)"
  ;;
checks)
  conditions start-checks
  eval "$(tools/jobpool env)"
  run() {  # run name command...
    local n=$1; shift
    tools/timed check - "$n@$R" -- "$@" > "$logs/$n.log" 2>&1
    echo "$(date -u +%T) $n exit $?: $(tail -1 "$logs/$n.log")"
  }
  S=docs/agent/snippets/plan_cost_model
  D=docs/agent/records/performance/plan-cost-model/out
  run conventions_check python3 -I test/reference/conventions_check.py
  run gate:comments_test python3 -I $S/comments_test.py
  run gate:cost-comments-plan python3 -I $S/cost.py comments include/galoot/core/plan.hpp \
    $D/figures-clang.txt $D/figures-gcc.txt
  run gate:cost-comments-limits python3 -I $S/cost.py comments docs/guide/limits.md \
    --section "Steps and operations of each plan" $D/figures-clang.txt $D/figures-gcc.txt
  for c in clang:warm gcc:cold; do
    cc=${c%:*} t=${c#*:}
    run gate:cost-selfcheck-$cc python3 -I $S/cost.py selfcheck \
      $D/weights-$cc-galoot-O0.tsv $D/figures-$cc.txt \
      --train $D/train-cases.tsv:$D/train-$cc-galoot-O0-$t.tsv \
              $D/train-gen-cases.tsv:$D/train-gen-$cc-galoot-O0-$t.tsv \
              $D/train-map-cases.tsv:$D/train-map-$cc-galoot-O0-$t.tsv \
      --check $D/test-cases.tsv:$D/test-$cc-galoot-O0-$t.tsv \
              $D/plans-cases.tsv:$D/plans-$cc-galoot-O0-$t.tsv \
              $D/maps-cases.tsv:$D/maps-$cc-galoot-O0-$t.tsv \
              $D/wide-cases.tsv:$D/wide-$cc-galoot-O0-$t.tsv
  done
  run gate:jobserver-check python3 -I docs/agent/snippets/jobserver_check.py
  echo "$(date -u +%FT%TZ) seeds start; pool: $(pool)"
  for prog in core_identities_test core_dual_identities_test core_transform_identities_test \
              core_identities_rounding_test core_basis_change_test core_basis_change_identities_test; do
    bin=$wt/build/gcc-debug/test/$prog
    [ -x "$bin" ] || { echo "missing $bin"; continue; }
    fails=0
    for k in $(seq 1 50); do
      eval "$(tools/jobpool env)"
      tools/timed --log "$seeds" test gcc-debug "gate:seed:$k:$prog@$R" -- \
        "$bin" --rand-seed=$k > "$logs/seed-$prog-$k.log" 2>&1 || {
        fails=$((fails + 1)); echo "seed $k $prog FAILED"; }
    done
    echo "$(date -u +%T) $prog: 50 seeds, $fails failed"
  done
  conditions end-checks
  ;;
bench)
  LOCK_OWNER="galoot release 1.0.1 matrix (benchmarks) $(date -u +%FT%TZ)" \
    bash $pe/lock.sh take 3600 || { echo "lock not taken"; exit 1; }
  echo "$(date -u +%FT%TZ) lock taken; pool: $(pool); $(uptime)"
  conditions start-bench
  head -1 /proc/stat > "$out/bench-procstat-before.txt"; grep -E '^cpu(4|5) ' /proc/stat >> "$out/bench-procstat-before.txt"
  eval "$(tools/jobpool env)"
  for p in gcc-measure clang-measure; do
    taskset -c 4 tools/timed test "$p" "gate:bench@$R" -- \
      ctest --test-dir "build/$p" -L '^bench$' -V > "$logs/bench-$p.log" 2>&1
    echo "$(date -u +%T) bench $p exit $?: $(grep -E 'tests passed|tests failed' "$logs/bench-$p.log" | tail -1)"
  done
  head -1 /proc/stat > "$out/bench-procstat-after.txt"; grep -E '^cpu(4|5) ' /proc/stat >> "$out/bench-procstat-after.txt"
  conditions end-bench
  bash $pe/lock.sh release
  echo "$(date -u +%FT%TZ) lock released; pool: $(tools/jobpool status | head -1); $(uptime)"
  ;;
close)
  tools/timed trace - "gate:trace@$R" -- python3 -I tools/test_references.py trace \
    docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
    test/reference/golden.hpp test > "$logs/trace.log" 2>&1
  echo "trace exit $?: $(tail -1 "$logs/trace.log")"
  tools/timed check - "gate:runlog@$R" -- tools/runlog check > "$logs/runlog.log" 2>&1
  echo "runlog exit $?: $(tail -1 "$logs/runlog.log")"
  ;;
*) echo "usage: matrix.sh presets|checks|bench|close"; exit 2 ;;
esac
echo "$(date -u +%FT%TZ) phase ${1} done"
