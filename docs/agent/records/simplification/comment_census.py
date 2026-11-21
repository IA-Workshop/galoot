"""Comment lines of include/galoot/ classified two ways at once: by the keyword
heuristic of the knowledge-base census and by the plan's position classes.

Usage, from the repository root:
    python3 -I docs/agent/records/simplification/comment_census.py

The heuristic (the same rules and word lists as the census in
knowledge-base/complexity-reduction/comments-and-self-documentation.md; its
script is math-tools exercises/cpp/cr-complexity/py/comment_kinds.py): a block
is a run of // lines, or of /// lines; it is "lint" if it starts with a NOLINT or
IWYU directive, "contract" if it is /// and holds a heading word such as
Requires or Cost, "why" if it holds a causal or contrastive word anywhere,
"cross-ref" if it names a file or says "see", and "other" otherwise. Every line
of a block takes the block's class. Position: "prologue" before the first
#include, "detail" inside a detail namespace, "public" elsewhere (the plan's
comments.py rules). The totals by heuristic class must match the census's
published table at the plan's revision.
"""
import collections
import glob
import re

WHY = re.compile(r"\b(because|so that|otherwise|since|why|instead|rather than|avoid|would|must|"
                 r"cannot|without|unless|so the|to keep|lest)\b", re.I)
CONTRACT = re.compile(r"\b(Requires|Returns|Cost|Limit|Throws|Precondition|Postcondition|Rejects)\b")
REF = re.compile(r"\b[\w/.-]+\.(md|hpp|cpp|py|cmake)\b|\bsee\b", re.I)
FILES = (sorted(glob.glob("include/galoot/core/*.hpp")) + ["include/galoot/core.hpp"]
         + sorted(glob.glob("include/galoot/presets/**/*.hpp", recursive=True)))

count = collections.Counter()
for f in FILES:
    lines = open(f).read().split("\n")
    position, seen_include, in_detail = [], False, False
    for line in lines:
        s = line.strip()
        seen_include = seen_include or s.startswith("#include")
        if re.match(r"namespace (galoot::core::)?detail\b", s):
            in_detail = True
        if s.startswith(("} // namespace detail", "} // namespace galoot::core::detail")):
            in_detail = False
        position.append("prologue" if not seen_include else "detail" if in_detail else "public")
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if not s.startswith("//"):
            i += 1
            continue
        doc, j, block = s.startswith("///"), i, []
        while j < len(lines) and lines[j].strip().startswith("//") and lines[j].strip().startswith("///") == doc:
            block.append(lines[j].strip().lstrip("/ "))
            j += 1
        t = " ".join(block)
        kind = ("lint" if t.startswith(("NOLINT", "IWYU")) else
                "contract" if doc and CONTRACT.search(t) else
                "why" if WHY.search(t) else "cross-ref" if REF.search(t) else "other")
        for n in range(i, j):
            count[("///" if doc else "//", kind, position[n])] += 1
        i = j

print("comment\tclass\tposition\tlines")
for k in sorted(count):
    print("\t".join(k) + f"\t{count[k]}")
by_class = collections.Counter()
for (c, kind, _), n in count.items():
    by_class[(c, kind)] += n
print("\nBy class (the census's table): " + ", ".join(f"{c} {k} {n}" for (c, k), n in sorted(by_class.items())))
print(f"All comment lines: {sum(count.values())}")
