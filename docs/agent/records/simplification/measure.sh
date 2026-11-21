#!/usr/bin/env bash
# Reruns every baseline of this directory on the current tree and writes the
# outputs to the directory given (default: baseline/ beside this script).
# Run from the repository root:
#   docs/agent/records/simplification/measure.sh [out-dir]
# Needs python3, clang-query and g++; the clone detector needs uv (lizard is
# fetched once into uv's cache, from PyPI, and nothing is installed).
set -euo pipefail
here=docs/agent/records/simplification
survey=docs/agent/records/performance/simplification-survey
out=${1:-$here/baseline}
mkdir -p "$out"
git rev-parse HEAD > "$out/revision.txt"
git diff --quiet HEAD -- include || echo "include/ has uncommitted changes" >> "$out/revision.txt"

# Lines per header: the plan's script (lines, blank, ///, //, code).
python3 -I "$survey/metrics.py" > "$out/lines.tsv"
# The same code-line count by GCC's own comment removal, per header.
for f in include/galoot/core/*.hpp include/galoot/core.hpp include/galoot/presets/rga3.hpp \
         include/galoot/presets/rga3/*.hpp; do
  printf '%s\t%s\n' "$f" "$(g++ -std=c++23 -fpreprocessed -dD -E -P -x c++ "$f" 2>/dev/null | grep -c '[^[:space:]]')"
done > "$out/code-lines-gcc.tsv"

# Comment classes: the plan's position classes, and the knowledge-base census crossed with them.
python3 -I "$survey/comments.py" > "$out/comments.txt"
python3 -I "$here/comment_census.py" > "$out/comment-census.tsv"

# Public names and declarations, and every namespace-scope entity with its line range.
python3 -I "$here/inventory.py" > "$out/inventory.tsv" 2> "$out/inventory-summary.txt"
python3 -I "$here/entities.py" > "$out/entities.tsv"

# Duplication: the plan's measure traced to the candidate units, and lizard's clone detector.
python3 -I "$here/duplication_by_site.py" > "$out/duplication-by-site.txt"
uvx --quiet --from lizard==1.26.1 python -I "$here/clones_lizard.py" > "$out/clones-lizard.txt"

# The candidate list checked against the entity ranges, with the lines each unit opens.
python3 -I "$here/check_candidates.py" "$out/entities.tsv" > "$out/candidates-check.txt"

cat "$out/inventory-summary.txt"
tail -1 "$out/candidates-check.txt"
