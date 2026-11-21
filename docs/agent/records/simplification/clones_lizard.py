"""Clone detection on include/galoot/ with lizard's duplicate extension, at several minimum lengths.

Usage, from the repository root (lizard is fetched into uv's cache, not installed):
    uvx --from lizard==1.26.1 python -I docs/agent/records/simplification/clones_lizard.py

lizard tokenizes each header with comments dropped, renames identifiers and
keywords consistently within each scope (the first new name in a scope becomes
v0, the next v1, and so on), writes every constant as 1 and every minus as plus,
and reports runs of at least N tokens that occur more than once. Its rate is the
share of all tokens that lie in such a run. N = 70 is what `lizard -Eduplicate`
uses. The script prints the rate for several N and, for N = 70, the clusters and
the duplicated lines per header.
"""
import collections
import glob
import os

from lizard_core.analysis import analyze_files
from lizard_core.extensions import get_extensions

os.chdir("include/galoot")
files = sorted(glob.glob("core/*.hpp")) + ["core.hpp"] + sorted(glob.glob("presets/**/*.hpp", recursive=True))
exts = get_extensions(["duplicate"])
list(analyze_files(files, exts=exts))
dup = next(e for e in exts if type(e).__module__.endswith("lizardduplicate"))

print("minimum_tokens\trate_percent\tclusters\tcopies")
for n in (30, 50, 70, 100, 150):
    clusters = list(dup.get_duplicates(min_duplicate_tokens=n))
    print(f"{n}\t{100 * dup.duplicate_rate():.2f}\t{len(clusters)}\t{sum(len(c) for c in clusters)}")

print("\nClusters at 70 tokens (file:first ~ last line of each copy)")
lines = collections.defaultdict(set)
for c in dup.get_duplicates(min_duplicate_tokens=70):
    print("  " + "; ".join(str(s) for s in c))
    for s in c:
        lines[s.file_name].update(range(s.start_line, s.end_line + 1))
print("\nLines inside a copy, per header, at 70 tokens")
for f in files:
    if lines[f]:
        print(f"  {f}\t{len(lines[f])}")
print(f"  total\t{sum(len(v) for v in lines.values())}")
