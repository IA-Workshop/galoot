#!/usr/bin/env bash
# usage: evidence.sh <preset> <tag> <component> [<program>]
# Records the registration, the test cases, and the per-seed summary of the
# test program of core.<component> (transform_identities or identities) for
# the equivalence comparison. The seeds run through seed_runs.sh.
set -u
p=$1; tag=$2; c=$3
S=$(dirname "$(readlink -f "$0")")
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--transform
out=$wt/docs/agent/records/performance/cost-cleanup-1-0-1/transform/evidence-$tag/$p/$c
prog=${4:-$wt/build/$p/test/core_${c}_test}
raw=/tmp/claude-1000/-home-brianm-projects-utils-math-core/20d19078-8b10-4646-bf3c-dde482e5309c/scratchpad/raw-$tag-$p-$c
mkdir -p "$out"
cd "$wt"
ctest --test-dir build/$p -N -R "^core\.$c\$" \
  | sed -e "s|$wt|<worktree>|g" > "$out/ctest-listing.txt"
ctest --test-dir build/$p --show-only=json-v1 -R "^core\.$c\$" \
  | python3 -I -c '
import json, sys
d = json.load(sys.stdin)
for t in d["tests"]:
    labels = next(p["value"] for p in t["properties"] if p["name"] == "LABELS")
    print(t["name"], " ".join(sorted(labels)), sep="\t")
' > "$out/ctest-labels.tsv"
"$prog" --count > "$out/count.txt" 2>&1
"$prog" --list-test-cases > "$out/list-test-cases.txt" 2>&1
bash "$S/seed_runs.sh" "$prog" "$raw" || { echo "seed runs failed"; exit 1; }
printf 'seed\texit\ttest_cases\tassertions\tmessage\n' > "$out/seeds.tsv"
for k in $(seq 0 50); do
  o=$(cat "$raw/seed-$k.out"); e=$(cat "$raw/seed-$k.out.exit")
  tc=$(grep -m1 '^\[doctest\] test cases:' <<<"$o" | tr -s ' ')
  as=$(grep -m1 '^\[doctest\] assertions:' <<<"$o" | tr -s ' ')
  msg=$(grep 'MESSAGE:' <<<"$o" | sed 's/.*MESSAGE: *//' | paste -sd'|')
  printf '%s\t%s\t%s\t%s\t%s\n' "$k" "$e" "$tc" "$as" "$msg" >> "$out/seeds.tsv"
done
echo "evidence $p $tag $c written"
