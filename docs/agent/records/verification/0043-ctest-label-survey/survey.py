#!/usr/bin/env python3
"""Survey the CTest labels of a configured tree.

usage: survey.py <ctest-json-v1 file> <tree name>

The input is the output of `ctest --test-dir <tree> --show-only=json-v1`. The
survey checks, for every test, that
  A. it has at least one label;
  B. it carries a label equal to the module its name starts with (the text
     before the first dot);
  C. it carries a label of the form <module>.<component>;
  D. no label appears twice in its list;
  E. its name has a module prefix (a dot after at least one character).
It then prints the number of tests, the number that fail each check, the
labels that belong to no module of the test's own (neither the module nor
<module>.<component>) with their counts, and the number of tests per module. Run it with `python3 -I`.
"""
import collections
import json
import sys


def main():
    path, tree = sys.argv[1], sys.argv[2]
    tests = json.load(open(path))["tests"]
    failed = collections.defaultdict(list)
    extra = collections.Counter()
    modules = collections.Counter()
    for t in tests:
        name = t["name"]
        labels = []
        for p in t.get("properties", []):
            if p["name"] == "LABELS":
                labels = p["value"]
        module = name.split(".", 1)[0] if "." in name[1:] else None
        if module is None:
            failed["E no module prefix"].append(name)
        if not labels:
            failed["A no label"].append(name)
        if module is not None and module not in labels:
            failed["B no module label"].append(name)
        if module is not None and not any(
            l.startswith(module + ".") for l in labels
        ):
            failed["C no component label"].append(name)
        if len(labels) != len(set(labels)):
            failed["D repeated label"].append(name)
        modules[module] += 1
        for l in labels:
            if l != module and not l.startswith((module or "?") + "."):
                extra[l] += 1
    print(f"tree {tree}: {len(tests)} tests")
    for key in ("A no label", "B no module label", "C no component label",
                "D repeated label", "E no module prefix"):
        print(f"  {key}: {len(failed[key])}", *failed[key][:5])
    print("  labels outside the test's own module:")
    for l, n in sorted(extra.items()):
        print(f"    {l}: {n}")
    print("  tests per module:")
    for m, n in sorted(modules.items(), key=lambda kv: str(kv[0])):
        print(f"    {m}: {n}")


main()
