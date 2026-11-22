#!/usr/bin/env bash
# Configure every configure preset from an old and a new source tree and
# compare the cache and the compile commands of each. The build tree's and
# the source tree's paths are replaced by <build> and <source> on both
# sides. A preset's GALOOT_FORCE_INLINE value is printed from the new tree.
#
# Usage: compare.sh <old source dir> <new source dir> <scratch dir>
# An old source directory comes from, for example,
#   git archive <revision> | tar -x -C <dir>
# Prints one line per preset: whether the compile commands and the cache
# entries (comments and blank lines left out) are identical, and the mode.
# Exits 1 when a configuration fails or either comparison differs.
set -u
old=$(realpath "$1") new=$(realpath "$2") scratch=$(realpath -m "$3")
mkdir -p "$scratch"
status=0
presets=$(cd "$new" && cmake --list-presets=configure | sed -n 's/^  "\(.*\)" *-.*/\1/p')
for p in $presets; do
  for side in old new; do
    src=${!side}
    build=$scratch/$side/$p
    rm -rf "$build"
    (cd "$src" && cmake --preset "$p" -B "$build") >"$scratch/$side-$p.log" 2>&1 \
      || { echo "$p: $side configure failed"; status=1; continue 2; }
    sed "s#$build#<build>#g; s#$src#<source>#g" "$build/compile_commands.json" \
      >"$scratch/$side-$p.commands"
    sed "s#$build#<build>#g; s#$src#<source>#g" "$build/CMakeCache.txt" \
      | grep -v '^#\|^//\|^$' | sort >"$scratch/$side-$p.cache"
  done
  line="$p:"
  if cmp -s "$scratch/old-$p.commands" "$scratch/new-$p.commands"; then
    line="$line commands identical"; else line="$line commands DIFFER"; status=1; fi
  if cmp -s "$scratch/old-$p.cache" "$scratch/new-$p.cache"; then
    line="$line, cache identical"; else line="$line, cache DIFFERS"; status=1; fi
  echo "$line, $(grep '^GALOOT_FORCE_INLINE' "$scratch/new-$p.cache")"
done
exit $status
