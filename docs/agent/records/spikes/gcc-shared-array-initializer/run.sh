#!/usr/bin/env bash
# Builds and runs reproducer.cpp in each variant on both compilers at -O0 and
# -O2, each compile through tools/timed, and prints what each program prints.
# From the worktree root: bash docs/agent/records/spikes/gcc-shared-array-initializer/run.sh
set -u
here=$(cd "$(dirname "$0")" && pwd)
out=$(mktemp -d)
for cxx in g++ clang++; do
  for v in 1 2 3 4; do
    for o in -O0 -O2; do
      tools/timed probe - "gcc-shared-init:$cxx:V$v$o" -- \
        "$cxx" -std=c++23 "$o" -DV="$v" "$here/reproducer.cpp" -o "$out/a" > /dev/null 2>&1
      echo "$cxx V=$v $o: $("$out/a")"
    done
  done
done
rm -r -- "$out"
