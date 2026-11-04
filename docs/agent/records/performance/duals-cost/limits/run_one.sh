#!/usr/bin/env bash
# Compiles one unit of the limit search and prints a result row: the unit,
# the compiler, the optimization level, the exit status, and the first
# error. -O0 leaves libstdc++'s assertions on, as libstdc++ defines them by
# itself there; -O2 has them off, as in the clang-measure preset.
#
# Usage, from the repository root: run_one.sh <unit.cpp> <g++|clang++> <O0|O2>
# The unit's row goes to runs.tsv beside this script, and its output to
# out/<unit>.<compiler>.<level>.txt.
set -u
tu=$(realpath "$1"); cc=$2; opt=$3
dir=$(cd "$(dirname "$0")" && pwd)
root=$(git rev-parse --show-toplevel) || exit 9
cd "$root" || exit 9
name=$(basename "$tu" .cpp)
mkdir -p "$dir/out"
log=$dir/out/$name.$cc.$opt.txt
timeout 600 tools/timed --log "$dir/runs.tsv" probe - "limit:$name:$cc:$opt" -- \
  "$cc" -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror \
  -ffp-contract=off "-$opt" -Iinclude -c -o /dev/null "$tu" > "$log" 2>&1
status=$?
first=$(grep -m1 -E "error:|fatal" "$log" | sed 's/.*error: //' | cut -c1-120)
echo -e "$name\t$cc\t$opt\t$status\t$first"
