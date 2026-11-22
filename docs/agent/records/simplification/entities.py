"""Line ranges of every namespace-scope entity in include/galoot/, public and detail.

Usage, from the repository root:
    python3 -I docs/agent/records/simplification/entities.py [include-dir] > entities.tsv

Prints: header, first line, last line, namespace (core, detail, rga3, std), kind,
name. The range is the declaration's own source range as Clang reports it: from
`template` (or the first token) to the closing brace or semicolon, without the
comment above it. Members are inside their class's range and are not listed.
candidates.tsv takes its line ranges from this output.
"""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else "include")
MATCHER = ('namedDecl(isExpansionInFileMatching("galoot/(core|presets)"), hasParent(namespaceDecl()), '
           'unless(namespaceDecl()), unless(isImplicit()), unless(varDecl(isTemplateInstantiation())), '
           'unless(functionDecl(isTemplateInstantiation())), unless(cxxRecordDecl(isTemplateInstantiation())))')
DECL = re.compile(r"^(\w+) 0x[0-9a-f]+ (?:parent 0x[0-9a-f]+ )?(prev 0x[0-9a-f]+ )?<([^>]*)> "
                  r"(?:(?:line|col):\S+ )?(.*)$")
FLAGS = {"referenced", "used", "constexpr", "consteval", "implicit", "invalid", "inline", "imported",
         "hidden", "class", "struct", "union", "typename", "static", "cinit", "callinit", "listinit",
         "explicit", "definition", "constinit", "external-linkage", "implicit-inline"}

with tempfile.TemporaryDirectory() as d:
    tu, q = os.path.join(d, "all.cpp"), os.path.join(d, "q.query")
    open(tu, "w").write("#include <galoot/core.hpp>\n#include <galoot/core/format.hpp>\n"
                        "#include <galoot/presets/rga3.hpp>\n")
    open(q, "w").write(f"set output dump\nm {MATCHER}.bind(\"e\")\n")
    out = subprocess.run(["clang-query", "-f", q, tu, "--", "-std=c++23", f"-I{ROOT}"],
                         capture_output=True, text=True, check=True).stdout.splitlines()

# The namespace of each match is read from the enclosing namespace's own lines.
NS = {}
for path in sorted({os.path.join(dp, f) for dp, _, fs in os.walk(os.path.join(ROOT, "galoot")) for f in fs}):
    stack = []
    for n, s in enumerate(open(path).read().split("\n"), 1):
        m = re.match(r"namespace ([\w:]+) \{", s.strip())
        if m:
            stack.append(m.group(1).split("::")[-1])
        elif re.match(r"\} // namespace", s.strip()) and stack:
            stack.pop()
        NS[(path, n)] = stack[-1] if stack else "std"

seen = set()
for i, l in enumerate(out):
    if l != 'Binding for "e":':
        continue
    m = DECL.match(out[i + 1])
    if not m:
        sys.exit(f"entities.py: cannot read: {out[i + 1][:200]}")
    kind, prev, rng, rest = m.groups()
    fm = re.match(r"([^,]+\.hpp):(\d+):\d+(?:, (?:line:(\d+):\d+|col:\d+))?", rng)
    path, first, last = fm.group(1), int(fm.group(2)), int(fm.group(3) or fm.group(2))
    words = rest.split("'")[0].split()
    name = next((w for w in words if w not in FLAGS), "")
    key = (path, first, kind)
    if key in seen:
        continue
    seen.add(key)
    ns = NS.get((path, first), "?")
    rel = os.path.relpath(path, os.path.dirname(ROOT))
    print(f"{rel}\t{first}\t{last}\t{ns}\t{kind}{' (redeclaration)' if prev else ''}\t{name}")
