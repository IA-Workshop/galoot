#!/usr/bin/env bash
# Preprocess one unit with the old and the new headers in every valid mode
# and diff the results. The include directory's path, which __FILE__ puts
# into each GALOOT_ASSERT expansion, is replaced by <include> on both sides.
#
# Usage: compare.sh <old include dir> <new include dir> <out dir>
# An old include directory comes from, for example,
#   git archive <revision> include | tar -x -C <dir>
# which gives <dir>/include. Prints one line per compiler and mode: whether
# the -E -P outputs are identical, and the macros -dM loses (<) or gains (>).
# Exits 1 when an output differs or a compile fails.
set -u
old=$1 new=$2 out=$3
src=$(dirname "$0")/unit.cpp
mkdir -p "$out"
status=0
for c in g++ clang++; do
  for chk in 0 1; do
    for fi in unset bare 0 1; do
      case $fi in
        unset) d=() ;;
        bare) d=(-DGALOOT_FORCE_INLINE) ;;
        *) d=(-DGALOOT_FORCE_INLINE=$fi) ;;
      esac
      tag=$c-chk$chk-fi$fi
      for side in old new; do
        inc=$old; [[ $side == new ]] && inc=$new
        $c -std=c++23 -I "$inc" -DGALOOT_CHECKING=$chk "${d[@]}" -E -P "$src" | sed "s|$inc/|<include>/|g" > "$out/$tag-$side.i" || status=1
        $c -std=c++23 -I "$inc" -DGALOOT_CHECKING=$chk "${d[@]}" -dM -E "$src" | sort > "$out/$tag-$side.dm" || status=1
      done
      if cmp -s "$out/$tag-old.i" "$out/$tag-new.i"; then e=identical; else e=DIFFERENT; status=1; fi
      dm=$(diff "$out/$tag-old.dm" "$out/$tag-new.dm" | grep '^[<>]' | sed 's/^\(.\) #define \([A-Za-z0-9_]*\).*/\1\2/' | tr '\n' ' ')
      echo "$tag: -E -P $e; -dM changes: $dm"
    done
  done
done
exit $status
