#!/usr/bin/env bash
# usage: merge_checks.sh <out.txt>
# The checks for the merge into feature/cost-cleanup-1-0-1: in each of the
# eleven presets, configure the tree if it is missing, build
# core_transform_identities_test and core_identities_test, and run
# ctest -L '^core\.(transform_identities|identities)$', each through
# tools/timed under the selection gate:transform-subfeature.
set -u
out=$1
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--transform
cd "$wt"
eval "$(tools/jobpool env)"
sel=gate:transform-subfeature
status=0
{
  echo "# Checks for the merge into feature/cost-cleanup-1-0-1, at $(git rev-parse --short HEAD), $(date -u +%FT%TZ)"
  echo "# preset: build core_transform_identities_test core_identities_test; ctest -L '^core\.(transform_identities|identities)\$'; selection $sel"
} > "$out"
for p in gcc-debug gcc-release gcc-asan clang-debug clang-release clang-asan \
         lint gcc-measure gcc-measure-o3 clang-measure clang-measure-o3; do
  if [ ! -d build/$p ]; then
    tools/timed configure $p - -- cmake --preset $p > /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/merge-config-$p.log 2>&1 || { echo "$p: configure failed" >> "$out"; status=1; continue; }
  fi
  if ! tools/timed build $p $sel -- ninja -C build/$p core_transform_identities_test core_identities_test > /tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/merge-build-$p.log 2>&1; then
    echo "$p: build failed" >> "$out"; status=1; continue
  fi
  r=$(tools/timed test $p $sel -- ctest --test-dir build/$p -L '^core\.(transform_identities|identities)$' --output-on-failure 2>&1)
  e=$?
  echo "$p: $(grep -m1 'tests passed' <<<"$r") | $(grep 'timed:' <<<"$r")" >> "$out"
  [ $e -eq 0 ] || { status=1; echo "$r" | tail -30 >> "$out"; }
done
echo "exit $status" >> "$out"
exit $status
