#!/usr/bin/env bash
# Builds cprobe.cpp in 16 variants (GCC and Clang; -ffp-contract=off, on,
# fast, and fast without vectorization; GALOOT_CHECKING 0 and 1), each
# compile timed into runs.tsv beside this script, runs each on the same
# cases, and compares the outputs.
#
#   bash build.sh <scratch dir> <python with python-flint>
#
# Run from the root of a checkout. The binaries and their outputs go to
# <scratch dir>; compare.txt and verify.txt are written beside this script.
set -euo pipefail
out=$1
py=$2
here=docs/agent/records/numerical/0040-contraction-check
mkdir -p "$out"
cases=3000
seed=7
for cxx in g++ clang++; do
  c=gcc
  novec="-fno-tree-vectorize -fno-tree-slp-vectorize"
  if [[ $cxx == clang++ ]]; then
    c=clang
    novec="-fno-vectorize -fno-slp-vectorize"
  fi
  for mode in off on fast fastnv; do
    flags="-ffp-contract=$mode"
    if [[ $mode == fastnv ]]; then
      flags="-ffp-contract=fast $novec"
    fi
    for ck in 0 1; do
      b=$c-$mode-c$ck
      # shellcheck disable=SC2086 # $flags holds several options
      tools/timed --log $here/runs.tsv probe - "contraction-check:cprobe-$b" -- \
        $cxx -std=c++23 -O2 -march=x86-64-v3 $flags -DGALOOT_CHECKING=$ck \
        -Iinclude $here/cprobe.cpp -o "$out/$b"
      "$out/$b" $cases $seed >"$out/$b.out" 2>"$out/$b.err"
    done
  done
done
python3 $here/compare.py "$out" >$here/compare.txt
files=()
for f in "$out"/*.out; do
  files+=("$f")
done
"$py" -B $here/verify.py $seed $cases "${files[@]}" >$here/verify.txt
