#!/usr/bin/env bash
# usage: evidence.sh <preset> <tag> [<program>]
# Records the registration, the test cases, and the per-seed output of
# core_identities_rounding_test for the equivalence comparison.
set -u
p=$1; tag=$2
wt=/home/brianm/projects/utils/math-core/galoot-wt/cost-cleanup--rounding
out=$wt/docs/agent/records/performance/cost-cleanup-1-0-1/rounding/evidence-$tag/$p
prog=${3:-$wt/build/$p/test/core_identities_rounding_test}
mkdir -p "$out"
cd "$wt"
ctest --test-dir build/$p -N -R '^core\.(identities_rounding|transform_identities)$' \
  | sed -e "s|$wt|<worktree>|g" > "$out/ctest-listing.txt"
ctest --test-dir build/$p --show-only=json-v1 -R '^core\.identities_rounding$' \
  | python3 -I -c '
import json, sys
d = json.load(sys.stdin)
for t in d["tests"]:
    labels = next(p["value"] for p in t["properties"] if p["name"] == "LABELS")
    print(t["name"], " ".join(sorted(labels)), sep="\t")
' > "$out/ctest-labels.tsv"
"$prog" --count > "$out/count.txt" 2>&1
"$prog" --list-test-cases > "$out/list-test-cases.txt" 2>&1
printf 'seed\texit\ttest_cases\tassertions\tmessage\tlogged\n' > "$out/seeds.tsv"
for k in $(seq 0 50); do
  o=$("$prog" --rand-seed=$k 2>&1); e=$?
  tc=$(grep -m1 '^\[doctest\] test cases:' <<<"$o" | tr -s ' ')
  as=$(grep -m1 '^\[doctest\] assertions:' <<<"$o" | tr -s ' ')
  msg=$(grep 'MESSAGE:' <<<"$o" | sed 's/.*MESSAGE: *//' | paste -sd'|')
  logged=$(grep '^ *logged:' <<<"$o" | sed 's/^ *//' | paste -sd'|')
  printf '%s\t%s\t%s\t%s\t%s\t%s\n' "$k" "$e" "$tc" "$as" "$msg" "$logged" >> "$out/seeds.tsv"
done
echo "evidence $p $tag written"
