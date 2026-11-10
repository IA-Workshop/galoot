#!/usr/bin/env bash
# Compiles one case and saves its diagnostics. Run from the worktree root:
#   docs/agent/records/research/2026-11-09-scalar-full10-gcc/run.sh <name> <compiler> <source> [flags...]
# The output goes to results/<name>.txt; the row to this directory's runs.tsv.
set -u
dir=docs/agent/records/research/2026-11-09-scalar-full10-gcc
name=$1 cxx=$2 src=$3
shift 3
mkdir -p "$dir/results"
out="$dir/results/$name.txt"
base=(-I include -std=c++23 -fsyntax-only)
{
  echo "\$ $cxx ${base[*]} $* $src"
  tools/timed --log "$dir/runs.tsv" compile - "scalar-full10:$name" -- \
    "$cxx" "${base[@]}" "$@" "$src" 2>&1
  echo "exit status: ${PIPESTATUS[0]}"
} >"$out" 2>&1
tail -n 1 "$out"
