#!/usr/bin/env bash
# Compile test/bench/kernels_inline.cpp against two include directories with
# the flags of rga3.kernels_inline (an optimized build without the checking
# mode, the warnings of every build) and compare the disassembly. The
# forced-inline mode is left unset, as in that test, and also set to 0.
#
# Usage: compare_codegen.sh <old include dir> <new include dir> <out dir>
# Run from the repository root. Prints one line per compiler and mode and
# exits 1 when a disassembly differs or a compile fails.
set -u
old=$1 new=$2 out=$3
src=test/bench/kernels_inline.cpp
mkdir -p "$out"
status=0
for c in g++ clang++; do
  for fi in unset 0; do
    d=(); [[ $fi == 0 ]] && d=(-DGALOOT_FORCE_INLINE=0)
    tag=$c-fi$fi
    for side in old new; do
      inc=$old; [[ $side == new ]] && inc=$new
      $c -I "$inc" -std=c++23 -DGALOOT_CHECKING=0 "${d[@]}" -O2 \
        -ffp-contract=off -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
        -Werror -c -o "$out/$tag-$side.o" "$src" || status=1
      objdump -d --no-show-raw-insn "$out/$tag-$side.o" |
        sed 1,3d > "$out/$tag-$side.dis"
    done
    if cmp -s "$out/$tag-old.dis" "$out/$tag-new.dis"; then
      e=identical
    else
      e=DIFFERENT; status=1
    fi
    echo "$tag: $(grep -c . "$out/$tag-new.dis") lines; disassembly $e"
  done
done
exit $status
