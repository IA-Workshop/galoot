#!/usr/bin/env bash
# usage: seed_runs.sh <program> <dir>
# Runs <program> at --rand-seed=0 to 50, one Ninja job per seed drawing from
# the shared job pool (MAKEFLAGS), writing <dir>/seed-<k>.out with the
# output and <dir>/seed-<k>.out.exit with the exit status.
set -u
prog=$1; dir=$2
mkdir -p "$dir"
{
  printf 'rule run\n  command = "%s" --rand-seed=$seed > $out 2>&1; echo $$? > $out.exit\n' "$prog"
  for k in $(seq 0 50); do
    printf 'build seed-%s.out: run\n  seed = %s\n' "$k" "$k"
  done
} > "$dir/build.ninja"
rm -f "$dir"/seed-*.out "$dir"/seed-*.out.exit
ninja -C "$dir" > "$dir/ninja.log" 2>&1
