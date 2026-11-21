#!/usr/bin/env bash
# Builds the three units at -O0 (so that the inline functions stay out of
# line and are merged by the linker) and links them in both orders.
set -u
inc=$1; out=$2; cd "$(dirname "$0")"
for cxx in g++ clang++; do
  $cxx -std=c++23 -O0 -I"$inc" -DGALOOT_CHECKING=1 -c checked.cpp -o "$out/checked.o"
  $cxx -std=c++23 -O0 -I"$inc" -DGALOOT_CHECKING=0 -c unchecked.cpp -o "$out/unchecked.o"
  $cxx -std=c++23 -O0 -I"$inc" -DGALOOT_CHECKING=0 -c main.cpp -o "$out/main.o"
  $cxx "$out/checked.o" "$out/unchecked.o" "$out/main.o" -o "$out/checked_first"
  $cxx "$out/unchecked.o" "$out/checked.o" "$out/main.o" -o "$out/unchecked_first"
  for order in checked_first unchecked_first; do
    echo "== $cxx, link order $order"
    "$out/$order"; echo "exit status $?"
  done
done
