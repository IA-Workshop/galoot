#!/usr/bin/env bash
# Preprocess one unit with the old and the new headers in every valid mode
# and diff the results, for a change that deletes or moves lines of a
# header. As in compare.sh, the include directory's path becomes <include>;
# here the line number that __LINE__ puts after it in each GALOOT_ASSERT
# expansion also becomes N, so a check below a deletion compares equal.
#
# Usage: compare_lines.sh <old include dir> <new include dir> <out dir>
# Prints one line per compiler and mode: the counts of lines the normalized
# -E -P output loses (<) and gains (>), and the macros -dM loses or gains.
# The diffs are left in <out dir>/<tag>.diff. Exits 1 when a compile fails.
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
        $c -std=c++23 -I "$inc" -DGALOOT_CHECKING=$chk "${d[@]}" -E -P "$src" |
          sed -E "s|$inc/|<include>/|g; s|(\"<include>/[^\"]*\"), *[0-9]+|\1, N|g" \
          > "$out/$tag-$side.i" || status=1
        $c -std=c++23 -I "$inc" -DGALOOT_CHECKING=$chk "${d[@]}" -dM -E "$src" | sort > "$out/$tag-$side.dm" || status=1
      done
      diff "$out/$tag-old.i" "$out/$tag-new.i" > "$out/$tag.diff"
      lost=$(grep -c '^<' "$out/$tag.diff")
      gained=$(grep -c '^>' "$out/$tag.diff")
      dm=$(diff "$out/$tag-old.dm" "$out/$tag-new.dm" | grep '^[<>]' | sed 's/^\(.\) #define \([A-Za-z0-9_]*\).*/\1\2/' | tr '\n' ' ')
      echo "$tag: -E -P loses $lost, gains $gained; -dM changes: $dm"
    done
  done
done
exit $status
