#!/usr/bin/env -S python3 -I
"""Check that the specs ported from two old drivers mutate exactly as the
drivers did: for every mutant of 0023-rga3/mutants.toml and of
0011-blade-product/mutants.toml (and its run-time edits), the diff that
`tools/mutate --check` prints equals the diff of the old driver's mutated
text against the file at HEAD, made with the same difflib call. With one
original, equal unified diffs mean equal mutated texts.

The old drivers' own functions make their texts: 0023's suite() and
mutate(), and 0011's MUTATIONS and RUNTIME_EDITS applied as its main() does
(each anchor replaced once, the run-time edits with str.replace).

Usage, from the root of a clean checkout: compare_ports.py
Prints one line per difference and exits 1 if there is any.
"""

import difflib
import pathlib
import runpy
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
RECORDS = ROOT / "docs/agent/records/mutations"


def diff(file, before, after):
    lines = [f"== {{}} {file}"]
    for line in difflib.unified_diff(
            before.splitlines(keepends=True), after.splitlines(keepends=True),
            f"a/{file}", f"b/{file}"):
        lines.append(line.rstrip("\n"))
    return lines


def blocks(spec, *options):
    """The --check output of spec, by the name of each block."""
    result = subprocess.run(
        [sys.executable, "-I", str(ROOT / "tools/mutate"), str(spec),
         "--check", *options], cwd=ROOT, capture_output=True, text=True)
    if result.returncode:
        sys.exit(f"{spec}: {result.stderr}")
    found, name = {}, None
    for line in result.stdout.splitlines():
        if line.startswith("== "):
            name = line.split()[1]
            found[name] = [f"== {{}} {line.split()[2]}"]
        else:
            found[name].append(line)
    return found


def old_rga3():
    old = runpy.run_path(str(RECORDS / "0023-rga3/mutate.py"), run_name="old")
    expected = {}
    for name in [*old["MUTANTS"], *old["OBJECT_MUTANTS"]]:
        header, edits, _, _ = old["suite"](name)
        original = text = header.read_text(encoding="utf-8")
        for anchor, replacement in edits:
            text = old["mutate"](text, anchor, replacement)
        expected[name] = diff(header.relative_to(ROOT), original, text)
    return expected


def old_blade_product():
    # The driver runs main() on import; its tables are read without it.
    source = (RECORDS / "0011-blade-product/mutate.py").read_text(
        encoding="utf-8").replace("\nmain()\n", "\n")
    old = {"__name__": "old"}
    exec(compile(source, "mutate.py", "exec"), old)
    header = pathlib.Path(old["HEADER"])
    test = pathlib.Path(old["TEST"])
    original = header.read_text(encoding="utf-8")
    expected = {}
    for name, anchor, replacement in old["MUTATIONS"]:
        assert original.count(anchor) == 1, name
        expected[name] = diff(header.relative_to(ROOT), original,
                              original.replace(anchor, replacement))
    text = test.read_text(encoding="utf-8")
    edited = text
    for anchor, replacement, count in old["RUNTIME_EDITS"]:
        assert edited.count(anchor) == count, anchor
        edited = edited.replace(anchor, replacement)
    return expected, (test.relative_to(ROOT), text, edited)


def compare(label, expected, actual):
    problems = []
    if list(expected) != list(actual):
        problems.append(f"{label}: mutants {list(actual)} differ from the "
                        f"old driver's {list(expected)}")
    for name in expected:
        if actual.get(name) != expected[name]:
            problems.append(f"{label}: {name}: the diff differs")
    return problems


def main():
    problems = compare("0023-rga3", old_rga3(),
                       blocks(RECORDS / "0023-rga3/mutants.toml"))
    expected, (test, text, edited) = old_blade_product()
    actual = blocks(RECORDS / "0011-blade-product/mutants.toml")
    problems += compare("0011-blade-product", expected, actual)

    # The three run-time edits of the old driver are the first three of the
    # port's four; the spec with only those three reproduces them exactly.
    three = blocks(RECORDS / "0026-tools/blade-product-three-edits.toml",
                   "--runtime")
    if three.pop("runtime", None) != diff(test, text, edited):
        problems.append("the three run-time edits differ from the old "
                        "driver's")
    problems += compare("three-edits", expected,
                        {name: block for name, block in three.items()})
    for problem in problems:
        print(problem)
    print(f"{len(expected) + 50} mutants and the run-time edits compared, "
          f"{len(problems)} differences")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
