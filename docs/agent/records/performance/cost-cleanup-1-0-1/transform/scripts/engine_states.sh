#!/usr/bin/env bash
# usage: engine_states.sh <program> <out.tsv>
# The engine states after each test case's last draw, seeds 0 to 50: how
# many there are and the SHA-256 of all of them in order. The seeds run
# through seed_runs.sh.
set -u
S=$(dirname "$(readlink -f "$0")")
raw=$(dirname "$1")/engine-raw
bash "$S/seed_runs.sh" "$1" "$raw" || { echo "seed runs failed"; exit 1; }
printf 'seed\texit\tstates\tsha256_of_states\tassertions\n' > "$2"
for k in $(seq 0 50); do
  o=$(cat "$raw/seed-$k.out"); e=$(cat "$raw/seed-$k.out.exit")
  st=$(grep 'MESSAGE: engine state:' <<<"$o" | sed 's/.*MESSAGE: engine state: *//')
  n=$(grep -c 'MESSAGE: engine state:' <<<"$o")
  h=$(printf '%s' "$st" | sha256sum | cut -d' ' -f1)
  as=$(grep -m1 '^\[doctest\] assertions:' <<<"$o" | tr -s ' ')
  printf '%s\t%s\t%s\t%s\t%s\n' "$k" "$e" "$n" "$h" "$as" >> "$2"
done
