#!/usr/bin/env bash
# Runs the reproducers of the correction of the merged register (the sample
# check's findings E1 and M1 to M4) and prints what each gave. Each is one
# small compile, called with g++ and clang++ directly, outside the test suite
# and the job pool.
#   run.sh <galoot include dir> <scratch dir>
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"
strip() { sed -e "s|$inc/||g" -e "s|$here/||g" -e "s|$out/||g"; }
diagnostic='^[^[:space:]]+: (fatal )?error: '

main() {
  echo "#### c01: an overflowing product coefficient, this correction's input (E1, A38)"
  for cxx in g++ clang++; do
    log=$($cxx -std=c++23 -fsyntax-only -I"$inc" \
      "$here/c01_overflow_cascade_fail.cpp" 2>&1)
    echo "== $cxx: $(grep -cE "$diagnostic" <<<"$log") errors in $(wc -l <<<"$log") lines of output; $(grep -c 'error' <<<"$log") lines contain \"error\""
    grep -E "$diagnostic" <<<"$log" | head -3 | strip | cut -c1-160
    grep -E 'errors? generated|errors seen above' <<<"$log" | strip | cut -c1-160
    echo "   the library's message is in the output: $(grep -c 'a product coefficient does not fit coefficient_type' <<<"$log") time(s)"
  done

  echo "#### count_check: every failing compile of the audit, counted three ways (E1)"
  "$here/count_check.sh" "$inc" "$out/count"

  echo "#### c02: the float16 padding and the floor it puts under a tolerance (M1, A22)"
  g++ -std=c++23 -Wall -Wextra -I"$inc" -DGALOOT_CHECKING=1 \
    "$here/c02_float16_floor.cpp" -o "$out/c02" && {
    "$out/c02"
    for s in 0.05 0.03; do
      "$out/c02" $s 2>&1 | strip
      echo "   exit status ${PIPESTATUS[0]}"
    done
  }

  echo "#### c03: canonical_name with a generator beyond e9 (M2, A06)"
  for cxx in g++ clang++; do
    for checking in 0 1; do
      echo "== $cxx -O0 -fsanitize=undefined, GALOOT_CHECKING=$checking"
      $cxx -std=c++23 -Wall -Wextra -O0 -fsanitize=undefined -I"$inc" \
        -DGALOOT_CHECKING=$checking "$here/c03_canonical_name_mixed.cpp" \
        -o "$out/c03" && "$out/c03" 2>&1 | strip
    done
  done

  echo "#### c04: rotation about an l that is not a line (M4, A49)"
  for cxx in g++ clang++; do
    for opt in -O0 -O2; do
      echo "== $cxx $opt, GALOOT_CHECKING=1"
      $cxx -std=c++23 -Wall -Wextra $opt -I"$inc" -DGALOOT_CHECKING=1 \
        "$here/c04_rotation_not_a_line.cpp" -o "$out/c04" && "$out/c04"
      echo "   exit status $?"
    done
  done

  echo "#### c05: the bit functions with a mask beyond e_n (A05)"
  for cxx in g++ clang++; do
    for checking in 0 1; do
      echo "== $cxx -O0 -fsanitize=undefined, GALOOT_CHECKING=$checking"
      $cxx -std=c++23 -Wall -Wextra -O0 -fsanitize=undefined -I"$inc" \
        -DGALOOT_CHECKING=$checking "$here/c05_bits_mask_beyond_n.cpp" \
        -o "$out/c05" && "$out/c05" 2>&1 | strip
    done
  done
}
# The shell's report of an aborted program names this script's path, a line
# and a process number; they are replaced so that two runs compare equal.
main 2>&1 | sed -E -e "s|$here/||g" \
  -e 's/(run\.sh: line) [0-9]+: +[0-9]+ (Aborted)/\1 N: \2/'
