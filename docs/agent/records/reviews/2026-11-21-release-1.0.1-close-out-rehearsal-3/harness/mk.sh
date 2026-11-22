#!/usr/bin/env bash
# mk.sh <name>: a fresh scratch clone of the real repository (local master
# from origin/master, main checked out, origin URL set to the GitHub URL) and
# a copy of the close-out script in which only the cd and W= lines change.
set -euo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-3
REAL=/home/brianm/projects/utils/math-core/galoot
name=$1
C=$S/clones/$name
X=$S/scripts/close-out-$name.sh
[ ! -e "$C" ] || { echo "mk: $C exists" >&2; exit 1; }
git clone -q --no-hardlinks "$REAL" "$C"
git -C "$C" branch -q --no-track master origin/master
git -C "$C" checkout -q main
git -C "$C" remote set-url origin https://github.com/IA-Workshop/galoot.git
O=$C/docs/agent/records/release/close-out-1.0.1.sh
sed -e "s|^cd /home/brianm/projects/utils/math-core/galoot\$|cd $C|" \
    -e "s|^W=/home/brianm/projects/utils/math-core/galoot-master\$|W=$S/$name-master|" "$O" > "$X"
chmod +x "$X"
# The copy must differ in exactly two lines, the cd line and the W= line.
d=$(diff "$O" "$X" | grep '^[<>]' | sed 's/^\([<>]\) \(cd\|W=\).*/\1 \2/' | LC_ALL=C sort | tr '\n' ' ' || true)
[ "$d" = "< W= < cd > W= > cd " ] || { echo "mk: copy differs in other lines: $d" >&2; diff "$O" "$X" >&2; exit 1; }
[ "$(git -C "$C" rev-parse main:docs/agent/records/release/close-out-1.0.1.sh)" = 3ea57051e6c90c0035a66d0c733ac5fc74c0e320 ] || { echo "mk: blob differs" >&2; exit 1; }
for r in main master 1.0.0 "1.0.0^{commit}"; do
  a=$(git -C "$REAL" rev-parse "$r"); b=$(git -C "$C" rev-parse "$r")
  [ "$a" = "$b" ] || { echo "mk: $r differs: real $a, clone $b" >&2; exit 1; }
done
echo "mk: $name ready: main $(git -C "$C" rev-parse --short main), master $(git -C "$C" rev-parse --short master), 1.0.0 $(git -C "$C" rev-parse --short 1.0.0); script blob 3ea57051; copy differs in the cd and W= lines only"
