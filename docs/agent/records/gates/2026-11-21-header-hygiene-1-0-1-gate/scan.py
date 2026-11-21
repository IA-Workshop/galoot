#!/usr/bin/env -S python3 -I
"""Scan the headers for names that clang-tidy's misc-confusable-identifiers
reports against a user's namespace-scope name.

Names: every one- and two-character identifier that does not start with an
underscore, and every three-character one built from l I 1 O 0 r s n x.
Each is declared as `int <name> = 0;` in the global namespace of a unit that
includes <galoot/core/format.hpp>, <galoot/presets/rga3.hpp> and
<galoot/core.hpp>, in that order. The check reports one clash per look-alike
group, so names are dealt into units such that no two names of one unit share
a skeleton under the ASCII look-alikes (I and 1 as l, 0 as O, rn as m); a
user's own pair then cannot hide a header's name. A name that does not
compile in the global namespace beside the headers (a keyword, or a C library
function such as j0) is left out and listed.

Usage, from the repository root: scan.py <out dir>
Writes <out dir>/clashes.tsv (one row per reported pair with a side under
include/galoot/) and <out dir>/summary.txt. Exit 0 always; the summary is
the verdict.
"""

import collections
import itertools
import pathlib
import re
import string
import subprocess
import sys

out = pathlib.Path(sys.argv[1])
out.mkdir(parents=True, exist_ok=True)
units_dir = out / "units"
units_dir.mkdir(exist_ok=True)

first = string.ascii_letters
rest = string.ascii_letters + string.digits + "_"
names = list(first)
names += [a + b for a in first for b in rest]
three = "lI1O0rsnx"
names += ["".join(t) for t in itertools.product(three, repeat=3)
          if not t[0].isdigit()]
names = sorted(set(names))

header = ("#include <galoot/core/format.hpp>\n"
          "#include <galoot/presets/rga3.hpp>\n"
          "#include <galoot/core.hpp>\n")
flags = ["-std=c++23", "-I", "include"]

# Drop names that cannot be declared beside the headers.
probe = units_dir / "probe.cpp"
bad = set()
while True:
    probe.write_text(header + "".join(f"int {n} = 0;\n" for n in names
                                      if n not in bad))
    r = subprocess.run(["clang++", *flags, "-fsyntax-only", "-ferror-limit=0",
                        str(probe)], capture_output=True, text=True)
    lines = probe.read_text().splitlines()
    found = set()
    for m in re.finditer(rf"^{re.escape(str(probe))}:(\d+):\d+: error:",
                         r.stderr, re.M):
        decl = re.match(r"int (\w+) = 0;", lines[int(m.group(1)) - 1])
        if decl:
            found.add(decl.group(1))
    if r.returncode == 0:
        break
    if not found:
        sys.exit("probe fails with no name to drop:\n" + r.stderr[:2000])
    bad |= found
names = [n for n in names if n not in bad]


def skeleton(n):
    return n.replace("rn", "m").replace("I", "l").replace("1", "l") \
            .replace("0", "O")


groups = collections.defaultdict(list)
for n in names:
    groups[skeleton(n)].append(n)
count = max(len(g) for g in groups.values())
units = [[] for _ in range(count)]
for g in groups.values():
    for i, n in enumerate(g):
        units[i].append(n)

diag = re.compile(r"^(.*?):(\d+):(\d+): (?:warning|error): '(\w+)' is "
                  r"confusable with '(\w+)' \[misc-confusable-identifiers")
note = re.compile(r"^(.*?):(\d+):(\d+): note: ")
rows = set()
for i, unit in enumerate(units):
    src = units_dir / f"unit{i:02}.cpp"
    src.write_text(header + "".join(f"int {n} = 0;\n" for n in unit))
    r = subprocess.run(["clang-tidy", "--checks=-*,misc-confusable-identifiers",
                        "--header-filter=(^|/)include/galoot/", "--quiet",
                        str(src), "--", *flags],
                       capture_output=True, text=True)
    text = r.stdout.splitlines()
    for k, line in enumerate(text):
        m = diag.match(line)
        if not m:
            continue
        other = None
        for later in text[k + 1:]:
            n2 = note.match(later)
            if n2:
                other = f"{n2.group(1)}:{n2.group(2)}"
                break
            if diag.match(later):
                break
        here = f"{m.group(1)}:{m.group(2)}"
        if "include/galoot/" in here or (other and "include/galoot/" in other):
            rows.add((m.group(4), here, m.group(5), other or "-", src.name))

rows = sorted(rows)
with open(out / "clashes.tsv", "w") as f:
    f.write("name\tat\tconfusable_with\tother_declaration\tunit\n")
    for row in rows:
        f.write("\t".join(row) + "\n")
with open(out / "summary.txt", "w") as f:
    f.write(f"names declared: {len(names)} in {count} units\n")
    f.write(f"names left out (do not compile beside the headers): "
            f"{' '.join(sorted(bad))}\n")
    f.write(f"pairs with a side under include/galoot/: {len(rows)}\n")
    for row in rows:
        f.write("  " + " | ".join(row) + "\n")
print((out / "summary.txt").read_text(), end="")
