#!/usr/bin/env bash
# Counts the errors of every failing compile among the audit's reproducers
# three ways, to show where a count of lines differs from a count of errors:
#   lines   the lines of the compiler's output that contain "error", which is
#           what the three run.sh scripts printed before this correction;
#   errors  the lines that start with a location and "error:", which is what
#           they print now;
#   own     the compiler's own count: the number in Clang's closing "N errors
#           generated.", and for GCC the results of level "error" in the SARIF
#           form of the same diagnostics (-fdiagnostics-format=sarif-stderr).
# Usage: count_check.sh <galoot include dir> <scratch dir>
# Each case is one small compile, called directly. Clang stops after 20
# errors unless told otherwise, so it runs with -ferror-limit=0 here; no case
# reaches 20 on Clang.
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd); audit=$(dirname "$here")
mkdir -p "$out"
diagnostic='^[^[:space:]]+: (fatal )?error: '

check() {  # check <compiler> <source, relative to the audit directory> [flags...]
  local cxx=$1 src=$2; shift 2
  local log="$out/count.log" own
  local limit=(); [ "$cxx" = clang++ ] && limit=(-ferror-limit=0)
  "$cxx" -std=c++23 -I"$inc" "${limit[@]}" "$@" "$audit/$src" >"$log" 2>&1
  if [ "$cxx" = clang++ ]; then
    own=$(sed -n -E 's/^([0-9]+) errors? generated\.$/\1/p' "$log")
  else
    own=$("$cxx" -std=c++23 -I"$inc" -fdiagnostics-format=sarif-stderr "$@" \
      "$audit/$src" 2>&1 >/dev/null | python3 -I -c '
import json, sys
results = json.load(sys.stdin)["runs"][0]["results"]
print(sum(1 for r in results if r.get("level") == "error"))')
  fi
  # Of the flags, only the -D ones tell two cases of one source apart.
  local defines=() flag
  for flag in "$@"; do
    case $flag in -D*) defines+=("$flag") ;; esac
  done
  printf '%-8s %-50s %-26s lines %2s  errors %2s  own %2s\n' "$cxx" "$src" \
    "${defines[*]}" "$(grep -c 'error' "$log")" \
    "$(grep -cE "$diagnostic" "$log")" "${own:-0}"
}

syntax=(-fsyntax-only)
warn=(-Wall -Wextra -fsyntax-only)
for cxx in g++ clang++; do
  check "$cxx" cpp/repro/r01_one_blade_explicit_fail.cpp "${syntax[@]}"
  check "$cxx" cpp/repro/r02_many_blades_explicit_fail.cpp "${syntax[@]}"
  check "$cxx" cpp/repro/r09_get_derived_fail.cpp "${syntax[@]}"
  check "$cxx" cpp/repro/r10_scale_derived_fail.cpp "${syntax[@]}"
  check "$cxx" cpp/repro/r12_orthogonal_change_overflow_fail.cpp "${syntax[@]}" -DW=79
  check "$cxx" cpp/repro/r15_translation_bool_fail.cpp "${syntax[@]}"
  check "$cxx" cpp/repro/r21_tab_separator_fail.cpp "${syntax[@]}"
  check "$cxx" numerical/p01_narrow_unsigned.cpp "${warn[@]}" -DCONSTANT_CASE
  for n in 2 4 5; do
    check "$cxx" numerical/p05_coefficient_edges.cpp "${warn[@]}" -DCASE=$n
  done
  check "$cxx" numerical/p06_change_extreme_rows.cpp "${warn[@]}" -DCASE=1
  for n in 1 3 4 5; do
    check "$cxx" numerical/p07_diagnostics.cpp "${warn[@]}" -DCASE=$n
  done
  check "$cxx" merge/m01_uint16_coefficient.cpp "${syntax[@]}" -DCONSTANT_CASE
  check "$cxx" merge/m04_target_mismatch_fail.cpp -O2 -march=x86-64-v3 \
    -DGALOOT_FORCE_INLINE=1 -c -o /dev/null
  check "$cxx" correction/c01_overflow_cascade_fail.cpp "${syntax[@]}"
done
# The limits of wide algebras: Clang's alone, then GCC's alone.
check clang++ cpp/repro/r17_wide_algebra_capacity.cpp "${syntax[@]}" -DGEN=22
check clang++ cpp/repro/r20_wide_vectors_wedge.cpp "${syntax[@]}" -DGEN=20
for n in 22 24 32; do
  check clang++ numerical/p09_wide_algebras.cpp "${warn[@]}" -DWIDE_N=$n -DWIDE_OP=4
done
for n in 24 32; do
  check clang++ numerical/p09_wide_algebras.cpp "${warn[@]}" -DWIDE_N=$n -DWIDE_OP=6
done
check g++ cpp/repro/r20_wide_vectors_wedge.cpp "${syntax[@]}" -DGEN=31
