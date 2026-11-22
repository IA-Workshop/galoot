#!/usr/bin/env bash
# mk.sh <name>: a fresh scratch clone of the real repository and a copy of the
# close-out script whose cd and W= lines point into the scratch directory.
set -euo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal
REAL=/home/brianm/projects/utils/math-core/galoot
name=$1
C=$S/clones/$name
X=$S/scripts/close-out-$name.sh
[ ! -e "$C" ] || { echo "mk: $C exists" >&2; exit 1; }
git clone -q --no-hardlinks "$REAL" "$C"
git -C "$C" branch -q --no-track master origin/master
git -C "$C" checkout -q main
git -C "$C" remote set-url origin https://github.com/IA-Workshop/galoot.git
orig=$C/docs/agent/records/release/close-out-1.0.1.sh
sed -e "s|^cd /home/brianm/projects/utils/math-core/galoot\$|cd $C|" \
    -e "s|^W=/home/brianm/projects/utils/math-core/galoot-master\$|W=$S/$name-master|" \
    "$orig" > "$X"
chmod +x "$X"
d=$(diff "$orig" "$X" | grep -c '^[<>]' || true)
[ "$d" = 4 ] || { echo "mk: copy differs in $d lines, not 4" >&2; diff "$orig" "$X" >&2; exit 1; }
for r in main master 1.0.0 "1.0.0^{commit}"; do
  a=$(git -C "$REAL" rev-parse "$r"); b=$(git -C "$C" rev-parse "$r")
  [ "$a" = "$b" ] || { echo "mk: $r differs: real $a, clone $b" >&2; exit 1; }
done
echo "mk: $name ready: clone $C, script $X"
