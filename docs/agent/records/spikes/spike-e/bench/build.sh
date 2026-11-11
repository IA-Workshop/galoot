#!/usr/bin/env bash
# Builds the three benchmark programs, the exact check, and the assembly of
# probe.cpp with the compile flags of the gcc-measure and clang-measure
# presets, each compile through tools/timed into runs.tsv here, then runs
# the check and the programs' own check, and counts the assembly. Run from
# the repository's root with the job pool's environment set:
#   eval "$(tools/jobpool env)"
#   bash docs/agent/records/spikes/spike-e/bench/build.sh
# Programs go to build/spike-e-bench/<compiler>/.
set -euo pipefail
here=docs/agent/records/spikes/spike-e/bench
export TIMED_LOG="$PWD/$here/runs.tsv"
flags=(-DGALOOT_CHECKING=0 -Iinclude -Itest -Wall -Wextra -Wpedantic
  -Wconversion -Wshadow -Werror -ffp-contract=off -O2 -std=c++23)
mkdir -p "$here/out"
for compiler in gcc clang; do
  case $compiler in
    gcc) cxx=g++ preset=gcc-measure ;;
    clang) cxx=clang++ preset=clang-measure ;;
  esac
  out=build/spike-e-bench/$compiler
  mkdir -p "$out"
  for program in transform_point transform_line transform_plane check; do
    tools/timed build "$preset" "spike-e-bench:$program:$compiler" -- \
      "$cxx" "${flags[@]}" -fuse-ld=lld "$here/$program.cpp" -o "$out/$program"
  done
  tools/timed asm "$preset" "spike-e-bench:probe:$compiler" -- \
    "$cxx" "${flags[@]}" -S "$here/probe.cpp" -o "$here/probe.$compiler.s"
  tools/timed check "$preset" "spike-e-bench:check:$compiler" -- \
    "$out/check" > "$here/out/check.$compiler.txt"
  for program in transform_point transform_line transform_plane; do
    tools/timed check "$preset" "spike-e-bench:$program-check:$compiler" -- \
      "$out/$program" check 20261110 >> "$here/out/check.$compiler.txt"
  done
done
python3 -I "$here/count_asm.py" "$here/probe.gcc.s" "$here/probe.clang.s" \
  > "$here/out/asm-counts.md"
