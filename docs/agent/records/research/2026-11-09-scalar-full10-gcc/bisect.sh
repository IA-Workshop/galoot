#!/usr/bin/env bash
# Finds the smallest -fconstexpr-ops-limit under which a phase of the plan of
# scalar times full<GENERATORS> no longer fails, by bisection between a
# failing and a passing limit, with gcc-debug's flags. A phase "fails" at a
# limit when the compile fails and its diagnostics name the phase's function.
# Run from the worktree root:
#   bisect.sh <tag> <phase-regex> <low-failing> <high-passing> [extra flags...]
# Each compile is a row of runs.tsv; each probe's outcome is a line of
# results/bisect-<tag>.tsv (limit, exit status, whether the phase failed).
set -u
dir=docs/agent/records/research/2026-11-09-scalar-full10-gcc
tag=$1 phase=$2 lo=$3 hi=$4
shift 4
debug=(-DGALOOT_CHECKING=1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
  -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0)
table=$dir/results/bisect-$tag.tsv
probe() {
  local limit=$1 out status failed
  shift
  out=$(tools/timed --log "$dir/runs.tsv" compile - "scalar-full10:bisect-$tag:$limit" -- \
    g++ -I include -std=c++23 -fsyntax-only "${debug[@]}" "$@" \
    "-fconstexpr-ops-limit=$limit" "$dir/product.cpp" 2>&1)
  status=$?
  failed=0
  if ((status != 0)) && grep -Eq "$phase" <<<"$out"; then failed=1; fi
  printf '%s\t%s\t%s\n' "$limit" "$status" "$failed" >>"$table"
  return $failed
}
extra=("$@")
while ((hi - lo > 1)); do
  mid=$(((lo + hi) / 2))
  if probe "$mid" "${extra[@]}"; then hi=$mid; else lo=$mid; fi
done
echo "threshold $hi (fails at $lo)"
# Confirm above the threshold: a pass found below the true count would fail
# at some of these (note 0017, section 3.3).
for d in 1 2 3 7 15 31 63 127; do probe $((hi + d)) "${extra[@]}" || echo "fails at +$d"; done
for p in 1 10 100 1000; do probe $((hi + hi * p / 10000)) "${extra[@]}" || echo "fails at +$p/10000"; done
