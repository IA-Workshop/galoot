#!/usr/bin/env bash
# Builds m05 under each compile option and runs it at two scales (ordinary
# values, and v small enough that the second product underflows), with and
# without flush-to-zero, and in each rounding direction.
#   m05_run.sh <galoot include dir> <scratch dir>
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"
options=("" "-fassociative-math -fno-signed-zeros -fno-trapping-math"
         "-freciprocal-math" "-fno-signed-zeros"
         "-march=x86-64-v3 -ffp-contract=fast")
for cxx in g++ clang++; do
  for opt in "${options[@]}"; do
    # shellcheck disable=SC2086
    if ! $cxx -std=c++23 -O2 -I"$inc" -DGALOOT_CHECKING=1 -frounding-math \
        $opt "$here/m05_check_environment.cpp" -o "$out/m05" 2>"$out/m05.log"; then
      echo "== $cxx -O2 $opt: build failed: $(grep -m1 error "$out/m05.log")"
      continue
    fi
    echo "== $cxx -O2 -frounding-math $opt"
    for family in r3 r4; do
      for scales in "1 1" "1e-160 1e-1"; do
        for ftz in 0 1; do
          # shellcheck disable=SC2086
          echo "   $("$out/m05" $family $scales $ftz near)"
        done
      done
      if [ -z "$opt" ]; then
        for mode in up down zero; do
          echo "   $("$out/m05" $family 1 1 0 $mode)"
        done
      fi
    done
  done
done
