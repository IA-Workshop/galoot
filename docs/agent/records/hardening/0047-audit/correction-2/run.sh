#!/usr/bin/env bash
# Runs the reproducers of the second correction of the merged register (the
# findings N1 to N4 of the second sample check) and prints what each gave.
# Each is one small compile, called with g++ and clang++ directly, outside
# the test suite and the job pool. The compiles of d01 at 21 and 22
# generators are the heaviest; run the whole script under memguard.sh.
#   run.sh <galoot include dir> <scratch dir>
# A failing compile is counted as correction/count_check.sh counts it: the
# lines that start with a location and "error:", and the compiler's own count
# (Clang's closing "N errors generated.", run with -ferror-limit=0; for GCC,
# the results of level "error" in its SARIF output).
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"
strip() { sed -e "s|$inc/||g" -e "s|$here/||g" -e "s|$out/||g"; }
diagnostic='^[^[:space:]]+: (fatal )?error: '

# compile <compiler> <source> [flags...]: prints "builds", or the two counts
# and the first error, cut to 150 characters.
compile() {
  local cxx=$1 src=$2; shift 2
  local log="$out/compile.log" own
  local limit=(); [ "$cxx" = clang++ ] && limit=(-ferror-limit=0)
  if "$cxx" -std=c++23 -I"$inc" "${limit[@]}" "$@" "$here/$src" \
    >"$log" 2>&1; then
    echo "builds"
    return
  fi
  if [ "$cxx" = clang++ ]; then
    own=$(sed -n -E 's/^([0-9]+) errors? generated\.$/\1/p' "$log")
  else
    own=$("$cxx" -std=c++23 -I"$inc" -fdiagnostics-format=sarif-stderr "$@" \
      "$here/$src" 2>&1 >/dev/null | python3 -I -c '
import json, sys
results = json.load(sys.stdin)["runs"][0]["results"]
print(sum(1 for r in results if r.get("level") == "error"))')
  fi
  echo "refused: errors $(grep -cE "$diagnostic" "$log"), own ${own:-0}"
  grep -E "$diagnostic" "$log" | head -1 | strip | sed -E 's/^[^ ]+: //' |
    cut -c1-150 | sed 's/^/     first: /'
  grep -o -E 'cannot allocate array; evaluated array bound [0-9]+ exceeds the limit \([0-9]+\)' \
    "$log" | head -1 | sed 's/^/     note: /'
}

main() {
  echo "#### d01: Clang's refusals at 22 generators follow the pair's span (N1, A21)"
  for cxx in clang++ g++; do
    for c in $(seq 1 12); do
      echo "== $cxx GEN=22 CASE=$c: $(compile "$cxx" d01_wide_capacity_by_pair.cpp \
        -fsyntax-only -DGEN=22 -DCASE=$c)"
    done
  done
  for c in 2 5 6 7; do
    echo "== clang++ GEN=21 CASE=$c: $(compile clang++ d01_wide_capacity_by_pair.cpp \
      -fsyntax-only -DGEN=21 -DCASE=$c)"
  done

  echo "#### d02: a hand-written metric that metric_like refuses, through blades (N2, A39)"
  for cxx in g++ clang++; do
    for c in 1 2 3 4; do
      echo "== $cxx CASE=$c: $(compile "$cxx" d02_hand_metric_blades_fail.cpp \
        -fsyntax-only -DCASE=$c)"
    done
  done

  echo "#### d03: the motor rotation returns about an l that is not a line (N3, A49)"
  for cxx in g++ clang++; do
    for opt in -O0 -O2; do
      echo "== $cxx $opt, GALOOT_CHECKING=1"
      $cxx -std=c++23 -Wall -Wextra $opt -I"$inc" -DGALOOT_CHECKING=1 \
        "$here/d03_rotation_motor_norms.cpp" -o "$out/d03" && "$out/d03"
      echo "   exit status $?"
    done
  done

  echo "#### d04: a derived class beside an operand of another scalar or algebra (N4, A50)"
  for cxx in g++ clang++; do
    for form in 1 2 3 4 5; do
      for op in $(seq 1 16); do
        echo "== $cxx FORM=$form OP=$op: $(compile "$cxx" \
          d04_derived_mismatched_operands_fail.cpp -fsyntax-only \
          -DFORM=$form -DOP=$op)"
      done
    done
  done
}
main 2>&1
