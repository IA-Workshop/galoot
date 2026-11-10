#!/usr/bin/env bash
# Writes the assembly of probe.cpp and of the seven benchmark programs with
# the compile flags of the gcc-measure and clang-measure presets (the flags
# CMake gives a test/bench source in those presets, with -S), then three
# Clang variants of probe.cpp used only to find causes, then the counts of
# count.py and the symbolic trace of trace.py. Run from the repository's
# root:
#   bash <this directory>/run.sh [scratch directory]
# Only probe.gcc.s and probe.clang.s are kept with the record; the
# benchmark programs' assembly and the variants go to the scratch directory
# (default build/wu17-asm). The counts and traces go to out/.
set -euo pipefail
here=$(realpath --relative-to=. "$(dirname "${BASH_SOURCE[0]}")")
scratch=${1:-build/wu17-asm}
mkdir -p "$scratch" "$here/out"
export TIMED_LOG="$PWD/$here/runs.tsv"
flags=(-DGALOOT_CHECKING=0 -Itest -Iinclude -Wall -Wextra -Wpedantic
  -Wconversion -Wshadow -Werror -ffp-contract=off -O2 -std=c++23 -S)
operations=(wedge_point_point wedge_point_point_point antiwedge_line_plane
  transform_point transform_line transform_plane compose_motors)
for compiler in gcc clang; do
  case $compiler in
    gcc) cxx=g++ preset=gcc-measure ;;
    clang) cxx=clang++ preset=clang-measure ;;
  esac
  tools/timed asm "$preset" "wu17-asm:probe:$compiler" -- \
    "$cxx" "${flags[@]}" "$here/probe.cpp" -o "$here/probe.$compiler.s"
  for operation in "${operations[@]}"; do
    tools/timed asm "$preset" "wu17-asm:bench_$operation:$compiler" -- \
      "$cxx" "${flags[@]}" "test/bench/${operation}_bench.cpp" \
      -o "$scratch/${operation}_bench.$compiler.s"
  done
done

# Variants: without the SLP vectorizer; without its copyable-element
# transform; with a raised inlining threshold.
declare -A variants=(
  [no-slp]="-fno-slp-vectorize"
  [no-copyable]="-mllvm -slp-copyable-elements=false"
  [inline-2000]="-mllvm -inline-threshold=2000"
)
for variant in no-slp no-copyable inline-2000; do
  # shellcheck disable=SC2086
  tools/timed asm - "wu17-asm:probe:clang:$variant" -- \
    clang++ "${flags[@]}" ${variants[$variant]} "$here/probe.cpp" \
    -o "$scratch/probe.clang.$variant.s"
done

python3 -I "$here/count.py" "$here" "$scratch" > "$here/out/count.md"
python3 -I "$here/trace.py" "$here" test/bench/reference_kernels.hpp \
  > "$here/out/trace.md"
python3 -I "$here/trace.py" "$here" test/bench/reference_kernels.hpp \
  "$scratch"/probe.clang.{no-slp,no-copyable,inline-2000}.s \
  > "$here/out/trace-variants.md"
