#!/usr/bin/env bash
# Builds the forced and the free unit with the same flags but different
# GALOOT_FORCE_INLINE, links them in both orders, and runs the program. For
# each mode it also prints how many weak (inline) symbols of galoot the two
# objects both define, which are the functions the linker merges, keeping one
# unit's definition for both.
#   run.sh <galoot include dir> <scratch dir>
set -u
inc=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$out"
modes=("-O0" "-O2" "-O2 -march=x86-64-v3 -ffp-contract=off"
       "-O2 -march=x86-64-v3 -ffp-contract=fast")
for cxx in g++ clang++; do
  for mode in "${modes[@]}"; do
    # shellcheck disable=SC2086
    $cxx -std=c++23 $mode -I"$inc" -DGALOOT_FORCE_INLINE=1 -DUNIT=forced_unit \
      -c "$here/unit.cpp" -o "$out/forced.o"
    # shellcheck disable=SC2086
    $cxx -std=c++23 $mode -I"$inc" -DGALOOT_FORCE_INLINE=0 -DUNIT=free_unit \
      -c "$here/unit.cpp" -o "$out/free.o"
    # shellcheck disable=SC2086
    $cxx -std=c++23 $mode -I"$inc" -c "$here/main.cpp" -o "$out/main.o"
    weak() { nm "$1" | awk '$2 == "W" && $3 ~ /galoot/ {print $3}' | sort; }
    shared=$(comm -12 <(weak "$out/forced.o") <(weak "$out/free.o") | wc -l)
    echo "== $cxx $mode: weak galoot symbols, forced $(weak "$out/forced.o" | wc -l), free $(weak "$out/free.o" | wc -l), in both $shared"
    $cxx "$out/forced.o" "$out/free.o" "$out/main.o" -o "$out/forced_first"
    $cxx "$out/free.o" "$out/forced.o" "$out/main.o" -o "$out/free_first"
    for order in forced_first free_first; do
      echo "   link order $order: $("$out/$order")"
    done
  done
done
