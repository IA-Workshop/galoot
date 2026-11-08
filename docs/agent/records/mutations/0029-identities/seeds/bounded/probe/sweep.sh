#!/usr/bin/env bash
# sweep.sh <probe> <first> <last>: for each seed, prints the seed, the
# probe's MAXBITS, and its exit status.
for s in $(seq $2 $3); do out=$($1 --rand-seed=$s 2>&1); st=$?; b=$(grep -o 'MAXBITS [0-9]*' <<<"$out" | cut -d' ' -f2); echo "$s ${b:-NA} $st"; done
