#!/usr/bin/env bash
# usage: measured_build.sh <tag> ; builds core_identities_rounding_test on clang-measure then gcc-debug under the timing lock
set -u
tag=$1
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--rounding
rec=$wt/docs/agent/records/performance/cost-cleanup-1-0-1/rounding
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
cd "$wt"
eval "$(tools/jobpool env)"
LOCK_OWNER="galoot cost-cleanup U1 $tag $(date -u +%FT%TZ)" bash $pe/lock.sh take 7200 || { echo "lock not taken"; exit 1; }
conditions() {
  { date -u +%FT%TZ; uptime; tools/jobpool status; free -m;
    top -b -n 1 -w 200 | sed -n 1,25p; } > "$rec/conditions-$tag-$1.txt" 2>&1
}
conditions start
for p in clang-measure gcc-debug; do
  MEMGUARD_MIN_GB=20 bash $pe/memguard.sh "$rec/memguard-$tag-$p.tsv" -- \
    tools/timed build $p core_identities_rounding_test -- \
    ninja -C build/$p core_identities_rounding_test > "$rec/build-$tag-$p.log" 2>&1
  echo "$p build exit $?"
done
conditions end
bash $pe/lock.sh release
echo done
