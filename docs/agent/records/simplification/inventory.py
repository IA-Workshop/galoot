"""Inventory of the public interface of include/galoot/, from Clang's own view of the headers.

Usage, from the repository root:
    python3 -I docs/agent/records/simplification/inventory.py [include-dir] > inventory.tsv

Prints one row per declaration: tier, kind, qualified name, file:line, signature.
The summary goes to stderr.

Method (see README.md, "Public names and declarations"):
- A translation unit includes core.hpp, core/format.hpp and presets/rga3.hpp.
- clang-query runs in its default traversal (AsIs); implicit declarations and
  template instantiations made while the headers are parsed are excluded by the
  matcher. Set TRAVERSAL=IgnoreUnlessSpelledInSource to cross-check tier "ns"
  (that traversal does not reach the formatter specialization in namespace std).
- Tier "ns": named declarations whose semantic context is ::galoot::core or
  ::galoot::presets::rga3 and whose parent in the AST is that namespace. A
  template is listed once (the FunctionTemplateDecl, ClassTemplateDecl,
  VarTemplateDecl or TypeAliasTemplateDecl), not again for the pattern it
  describes. Explicit and partial specializations are listed with their own kind.
- Tier "member": public, non-implicit declarations whose parent is a class or
  class template of tier "ns" (constructors, member functions, nested types,
  static and non-static data members).
- Tier "std": declarations in namespace std that the headers write (the
  formatter specialization).
"""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else "include")
PUB = 'namespaceDecl(anyOf(hasName("::galoot::core"), hasName("::galoot::presets::rga3")))'
IN_LIB = 'isExpansionInFileMatching("galoot/(core|presets)")'
NOT_INST = ("unless(isImplicit()), unless(varDecl(isTemplateInstantiation())), "
            "unless(functionDecl(isTemplateInstantiation())), "
            "unless(cxxRecordDecl(isTemplateInstantiation()))")
QUERIES = {
    "ns": f"namedDecl({IN_LIB}, hasDeclContext({PUB}), hasParent(namespaceDecl()), "
          f"unless(namespaceDecl()), {NOT_INST})",
    "member": f"namedDecl({IN_LIB}, hasParent(cxxRecordDecl(hasDeclContext({PUB}), "
              "unless(isTemplateInstantiation()))), "
              f"isPublic(), {NOT_INST})",
    "std": f'namedDecl({IN_LIB}, hasDeclContext(namespaceDecl(hasName("::std"))), {NOT_INST})',
}
TRAVERSAL = os.environ.get("TRAVERSAL", "AsIs")
FLAGS = {"referenced", "used", "constexpr", "consteval", "implicit", "invalid", "inline",
         "imported", "hidden", "class", "struct", "union", "typename", "static", "cinit",
         "callinit", "listinit", "explicit", "default", "delete", "trivial", "mutable",
         "external-linkage", "implicit-inline", "module", "private", "public", "protected",
         "definition", "constinit"}
DECL = re.compile(r"^(\w+) 0x[0-9a-f]+ (?:parent 0x[0-9a-f]+ )?(prev 0x[0-9a-f]+ )?"
                  r"<([^>]*)> (?:(?:line|col):\S+ )?(.*)$")


def run(matcher):
    with tempfile.TemporaryDirectory() as d:
        tu = os.path.join(d, "all.cpp")
        q = os.path.join(d, "q.query")
        with open(tu, "w") as f:
            f.write("#include <galoot/core.hpp>\n#include <galoot/core/format.hpp>\n"
                    "#include <galoot/presets/rga3.hpp>\n")
        with open(q, "w") as f:
            f.write(f"set traversal {TRAVERSAL}\nset output dump\nm {matcher}.bind(\"api\")\n")
        out = subprocess.run(["clang-query", "-f", q, tu, "--", "-std=c++23", f"-I{ROOT}"],
                             capture_output=True, text=True, check=True).stdout
    return out.splitlines()


def location(rng, last):
    m = re.search(r"([^<,\s]+\.hpp):(\d+):\d+", rng)
    if m:
        return os.path.relpath(m.group(1), os.path.dirname(ROOT)), m.group(2)
    m = re.search(r"line:(\d+):", rng)
    return last[0], (m.group(1) if m else last[1])


def parse(lines, tier):
    rows, last = [], ("?", "?")
    for i, l in enumerate(lines):
        if l != 'Binding for "api":':
            continue
        m = DECL.match(lines[i + 1])
        if not m:
            sys.exit(f"inventory.py: cannot read the declaration line: {lines[i + 1][:200]}")
        kind, prev, rng, rest = m.groups()
        if prev:
            kind += " (redeclaration)"
        f, ln = location(rng, last)
        last = (f, ln)
        name, sig = "", ""
        q = rest.find("'")
        words = (rest[:q] if q >= 0 else rest).split()
        for w in words:
            if w not in FLAGS:
                name = w
                break
        if q >= 0:
            sig = rest[q:].split("':'")[0].strip("'") if "':'" in rest[q:] else rest[q:].split("' ")[0].strip("'")
        if kind.startswith("FunctionTemplateDecl"):
            for c in lines[i + 2:]:
                if re.match(r"[|`]-(FunctionDecl|CXXMethodDecl|CXXConstructorDecl) ", c):
                    cm = re.search(r"'([^']*)'", c)
                    sig = cm.group(1) if cm else ""
                    break
                if not c.startswith(("|", "`", " ")):
                    break
        if tier == "member":
            name = re.sub(r"<.*", "", name)
        rows.append((tier, kind, name, f"{f}:{ln}", sig))
    return rows


def body_of(path, line):
    """The text of the definition that starts at path:line, comment lines dropped."""
    text = open(os.path.join(os.path.dirname(ROOT), path)).read().split("\n")
    body, depth, started = [], 0, False
    for s in text[int(line) - 1:int(line) + 119]:
        if s.strip().startswith("//"):
            continue
        body.append(s)
        depth += s.count("{") - s.count("}")
        started = started or "{" in s
        if started and depth <= 0:
            break
        if not started and s.rstrip().endswith(";"):
            break
    return re.sub(r'"(?:[^"\\]|\\.)*"', '""', " ".join(body))


def rejects(path, line):
    """Whether a function exists only to reject a call: its body asserts false,
    calls a detail::reject_ helper, or holds nothing but static_asserts and a
    placeholder return."""
    b = body_of(path, line)
    if re.search(r"static_assert\(\s*false|detail::reject_\w+\s*<", b):
        return True
    m = re.search(r"\)\s*(?:noexcept\s*)?\{(.*)\}\s*$", b)
    if not m:
        return False
    inner = re.sub(r"static_assert\s*\((?:[^;])*\)\s*;", "", m.group(1))
    inner = re.sub(r"\s+", " ", inner).strip()
    return inner == "" or bool(re.fullmatch(r"return (\w+|[\w:<>, ]+\{\}|\{\});", inner))


def main():
    rows = []
    for tier, matcher in QUERIES.items():
        rows += parse(run(matcher), tier)
    out = []
    for tier, kind, name, loc, sig in rows:
        role = ""
        if "Function" in kind or "Constructor" in kind or "Method" in kind:
            role = "rejects" if rejects(*loc.rsplit(":", 1)) else "operation"
        out.append((tier, kind, name, loc, role, sig))
    print("tier\tkind\tname\tlocation\trole\tsignature")
    for r in out:
        print("\t".join(r))
    for tier in QUERIES:
        t = [r for r in out if r[0] == tier and "redeclaration" not in r[1]]
        names = {("/presets/" in r[3], r[2]) for r in t}
        rej = sum(r[4] == "rejects" for r in t)
        spec = sum("Specialization" in r[1] for r in t)
        print(f"{tier}: {len(t)} declarations ({rej} only reject a call, {spec} explicit or "
              f"partial specializations), {len(names)} distinct names; "
              f"{sum('redeclaration' in r[1] for r in out if r[0] == tier)} redeclarations not counted",
              file=sys.stderr)


main()
