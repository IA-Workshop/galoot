#!/usr/bin/env bash
# usage: engine_states.sh <program> <out.tsv>: the engine state after the last draw, seeds 0 to 50.
set -u
printf 'seed\texit\tsha256_of_state\tmessage\n' > "$2"
for k in $(seq 0 50); do
  o=$("$1" --rand-seed=$k 2>&1); e=$?
  st=$(grep 'MESSAGE: engine state:' <<<"$o" | sed 's/.*MESSAGE: engine state: *//')
  [ -n "$st" ] || st=MISSING
  h=$(printf '%s' "$st" | sha256sum | cut -d' ' -f1)
  msg=$(grep 'MESSAGE:' <<<"$o" | grep -v 'engine state' | sed 's/.*MESSAGE: *//')
  printf '%s\t%s\t%s\t%s\n' "$k" "$e" "$h" "$msg" >> "$2"
done
