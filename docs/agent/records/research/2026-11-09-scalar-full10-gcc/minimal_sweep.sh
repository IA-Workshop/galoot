#!/usr/bin/env bash
# For minimal.cpp at KEYS=256 with -O0 -D_GLIBCXX_ASSERTIONS: finds the
# smallest -fconstexpr-ops-limit that compiles, by bisection, then compiles
# at each of the <width> limits just below it and records which diagnostic
# each gives. Run from the worktree root: minimal_sweep.sh <width>
# Rows: results/minimal-sweep.tsv (limit, exit status, outcome), where outcome
# is pass, ops (GCC's "operation count exceeds limit"), expect (the
# "__builtin_expect(...) is not a constant expression" of libstdc++'s
# assertion), or other.
set -u
dir=docs/agent/records/research/2026-11-09-scalar-full10-gcc
width=$1
table=$dir/results/minimal-sweep.tsv
: >"$table"
probe() {
  local limit=$1 out status outcome
  out=$(tools/timed --log "$dir/runs.tsv" compile - "scalar-full10:minimal-sweep:$limit" -- \
    g++ -std=c++23 -fsyntax-only -O0 -D_GLIBCXX_ASSERTIONS -DKEYS=256 \
    "-fconstexpr-ops-limit=$limit" "$dir/minimal.cpp" 2>&1)
  status=$?
  if ((status == 0)); then outcome=pass
  elif grep -q "operation count exceeds limit" <<<"$out"; then outcome=ops
  elif grep -q "__builtin_expect.*is not a constant expression" <<<"$out"; then outcome=expect
  else outcome=other; fi
  printf '%s\t%s\t%s\n' "$limit" "$status" "$outcome" >>"$table"
  [[ $outcome == pass ]]
}
lo=1000 hi=33554432
while ((hi - lo > 1)); do
  mid=$(((lo + hi) / 2))
  if probe "$mid"; then hi=$mid; else lo=$mid; fi
done
echo "bisection threshold $hi"
for ((l = hi - width; l < hi; ++l)); do probe "$l"; done
for d in 1 2 5 10 100 1000; do probe $((hi + d)); done
cut -f3 "$table" | sort | uniq -c
