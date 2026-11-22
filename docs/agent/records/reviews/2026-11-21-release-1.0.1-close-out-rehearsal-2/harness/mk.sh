#!/usr/bin/env bash
# mk.sh <name> [old]: a fresh scratch clone of the real repository and a copy
# of the close-out script whose cd and W= lines point into the scratch
# directory. With "old", also a copy of the first version (1655ef33) of the
# script, changed the same way, for a comparison dry run.
set -euo pipefail
S=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/close-out-rehearsal-2
REAL=/home/brianm/projects/utils/math-core/galoot
name=$1
C=$S/clones/$name
X=$S/scripts/close-out-$name.sh
[ ! -e "$C" ] || { echo "mk: $C exists" >&2; exit 1; }
git clone -q --no-hardlinks "$REAL" "$C"
git -C "$C" branch -q --no-track master origin/master
git -C "$C" checkout -q main
git -C "$C" remote set-url origin https://github.com/IA-Workshop/galoot.git
copy() {  # copy <original> <copy>: only the cd and W= lines change
  sed -e "s|^cd /home/brianm/projects/utils/math-core/galoot\$|cd $C|" \
      -e "s|^W=/home/brianm/projects/utils/math-core/galoot-master\$|W=$S/$name-master|" "$1" > "$2"
  chmod +x "$2"
  local d; d=$( { diff "$1" "$2" || true; } | grep '^[<>]' | sed 's/^\([<>]\) \(cd\|W=\).*/\1 \2/' | LC_ALL=C sort | tr '\n' ' ')
  [ "$d" = "< W= < cd > W= > cd " ] || { echo "mk: copy differs in other lines: $d" >&2; diff "$1" "$2" >&2; exit 1; }
}
copy "$C/docs/agent/records/release/close-out-1.0.1.sh" "$X"
if [ "${2:-}" = old ]; then
  git -C "$C" show 1655ef33:docs/agent/records/release/close-out-1.0.1.sh > "$S/scripts/orig-old-$name.sh"
  copy "$S/scripts/orig-old-$name.sh" "$S/scripts/close-out-old-$name.sh"
fi
for r in main master 1.0.0 "1.0.0^{commit}"; do
  a=$(git -C "$REAL" rev-parse "$r"); b=$(git -C "$C" rev-parse "$r")
  [ "$a" = "$b" ] || { echo "mk: $r differs: real $a, clone $b" >&2; exit 1; }
done
echo "mk: $name ready: main $(git -C "$C" rev-parse --short main), master $(git -C "$C" rev-parse --short master), 1.0.0 $(git -C "$C" rev-parse --short 1.0.0) (all as in the real repository)"
