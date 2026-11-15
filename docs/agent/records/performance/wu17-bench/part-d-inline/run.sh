#!/usr/bin/env bash
# Writes the -O2 assembly of the seven benchmark programs with the flags of
# the gcc-measure and clang-measure presets and counts the calls in it.
# Run from the repository's root:
#   bash <this directory>/run.sh <scratch directory>
set -euo pipefail
here=$(dirname "${BASH_SOURCE[0]}")
scratch=$1
mkdir -p "$scratch"
flags=(-DGALOOT_CHECKING=0 -Itest -Iinclude -Wall -Wextra -Wpedantic
  -Wconversion -Wshadow -Werror -ffp-contract=off -O2 -std=c++23 -S)
for cxx in g++ clang++; do
  for operation in wedge_point_point wedge_point_point_point \
      antiwedge_line_plane transform_point transform_line transform_plane \
      compose_motors; do
    tools/timed asm - "part-d-inline:$operation:$cxx" -- \
      "$cxx" "${flags[@]}" "test/bench/${operation}_bench.cpp" \
      -o "$scratch/$operation.$cxx.s"
  done
done
python3 -I "$here/calls.py" "$scratch"/*.s
python3 -I "$here/loops.py" "$scratch"/*.s
