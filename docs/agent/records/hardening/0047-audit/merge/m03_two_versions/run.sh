#!/usr/bin/env bash
# Two versions of Galoot in one program. Extracts the headers of the 1.0.0
# tag and of the 1.0.1 candidate (the branch feature/header-hygiene-1-0-1 at
# the commit below) with git archive, and makes a third copy of 1.0.0 whose
# only change is the message of complement_mask's check, standing for a
# later version whose inline functions behave differently.
#   run.sh <galoot repository> <scratch dir>
set -u
repo=$1; out=$2; here=$(cd "$(dirname "$0")" && pwd)
candidate=62374f1a4ed1a401dac6bc130a4822492446fcbc
rm -rf "$out/v100" "$out/v101" "$out/edited"
mkdir -p "$out/v100" "$out/v101" "$out/edited"
git -C "$repo" archive 1.0.0 include | tar -x -C "$out/v100"
git -C "$repo" archive "$candidate" include | tar -x -C "$out/v101"
git -C "$repo" archive 1.0.0 include | tar -x -C "$out/edited"
sed -i 's/"n must be from 1 to 32"/"n must be from 1 to 32 (edited copy)"/' \
  "$out/edited/include/galoot/core/bits.hpp"
grep -c 'edited copy' "$out/edited/include/galoot/core/bits.hpp" |
  sed 's/^/edited copy: messages changed: /'

for cxx in g++ clang++; do
  for order in "v100 v101" "v101 v100"; do
    set -- $order
    $cxx -std=c++23 -DFIRST="$out/$1/include" -DSECOND="$out/$2/include" \
      "$here/one_unit.cpp" -o "$out/one_unit" 2>"$out/one_unit.log"
    echo "== $cxx, one unit, $1 then $2: $("$out/one_unit"), $(grep -c 'warning\|error' "$out/one_unit.log") diagnostics"
  done
  for pair in "v100 v101" "v100 edited"; do
    set -- $pair
    for opt in -O0 "-O2 -flto -Wodr"; do
      # shellcheck disable=SC2086
      $cxx -std=c++23 $opt -DGALOOT_CHECKING=1 -I"$out/$1/include" \
        -DUNIT=old_unit -c "$here/unit.cpp" -o "$out/old.o"
      # shellcheck disable=SC2086
      $cxx -std=c++23 $opt -DGALOOT_CHECKING=1 -I"$out/$2/include" \
        -DUNIT=new_unit -c "$here/unit.cpp" -o "$out/new.o"
      # shellcheck disable=SC2086
      $cxx -std=c++23 $opt -I"$out/$1/include" -c "$here/main.cpp" \
        -o "$out/main.o"
      both=$(comm -12 <(nm "$out/old.o" | awk '$2 == "W" {print $3}' | sort) \
        <(nm "$out/new.o" | awk '$2 == "W" {print $3}' | sort) |
        c++filt | grep -c galoot)
      for link in "old new" "new old"; do
        set -- $link
        # shellcheck disable=SC2086
        $cxx $opt "$out/$1.o" "$out/$2.o" "$out/main.o" -o "$out/prog" \
          2>"$out/link.log"
        msg=$("$out/prog" x 2>&1 | grep -o 'assertion failed: .*(' | head -1)
        echo "== $cxx $opt, $pair, link $link: galoot functions in both objects $both, link diagnostics $(grep -c 'warning\|error' "$out/link.log"), n = 3 gives [$("$out/prog")], n = 40 gives [${msg:-no abort}]"
      done
      set -- $pair
    done
  done
done
