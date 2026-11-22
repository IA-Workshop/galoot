#!/usr/bin/env bash
# usage: measured_build.sh <tag>
# Builds core_transform_identities_test and core_identities_test on
# clang-measure, then gcc-debug, under the timing lock and the memory guard.
set -u
tag=$1
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--transform
rec=$wt/docs/agent/records/performance/cost-cleanup-1-0-1/transform
pe=/home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common
cd "$wt"
eval "$(tools/jobpool env)"
LOCK_OWNER="galoot cost-cleanup U2 $tag $(date -u +%FT%TZ)" bash $pe/lock.sh take 7200 || { echo "lock not taken"; exit 1; }
conditions() {
  { date -u +%FT%TZ; uptime; tools/jobpool status; free -m;
    top -b -n 1 -w 200 | sed -n 1,25p; } > "$rec/conditions-$tag-$1.txt" 2>&1
}
conditions start
for p in clang-measure gcc-debug; do
  for t in core_transform_identities_test core_identities_test; do
    MEMGUARD_MIN_GB=20 bash $pe/memguard.sh "$rec/memguard-$tag-$p-$t.tsv" -- \
      tools/timed build $p $t -- \
      ninja -C build/$p $t > "$rec/build-$tag-$p-$t.log" 2>&1
    echo "$p $t build exit $?"
  done
done
conditions end
bash $pe/lock.sh release
echo done
