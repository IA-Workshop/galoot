"""tools.catalogue: which tests pin each message of a header tree, read from
CTest listings given as files or as build trees, the check of a catalogue's
test names, and the check that every message is pinned or listed as
internal with its reason, and that a test pinning a part of a longer
message forbids the longer one.

Usage: python3 -I catalogue_test.py <tools/catalogue> <scratch directory>

The fixtures are in catalogue/ beside this script: include/ is a header
tree of messages, one header in a subdirectory and a file that is not a
header; tests.json is a CTest listing of tests that require, forbid, or
only partly name them. The scratch directory is wiped and gets the tree
listing, which is tests.json without t.semicolon, and stand-ins for ctest:
one that prints it for that tree and these arguments only, and one that
fails. Every expected value is worked out by hand from the fixtures. Prints
one line per failed check and exits 1 if there is any.
"""

import json
import os
import shutil
import subprocess
import sys

FIXTURE = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       "catalogue")
INCLUDE = os.path.join(FIXTURE, "include")
LISTING = os.path.join(FIXTURE, "tests.json")
SUMMARY = "13 messages: 9 pinned by a test, 4 listed as internal\n"
NOT_LISTED = "not pinned by a test and not listed as internal: "

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


def fixture(name):
    return os.path.join(FIXTURE, name)


def write_executable(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)
    os.chmod(path, 0o755)


def main(argv):
    if len(argv) != 2:
        print(__doc__.split("\n\n")[1], file=sys.stderr)
        return 2
    tool, scratch = (os.path.abspath(a) for a in argv)
    shutil.rmtree(scratch, ignore_errors=True)
    tree = os.path.join(scratch, "tree")
    os.makedirs(tree)
    with open(LISTING, encoding="utf-8") as f:
        listing = json.load(f)
    listing["tests"] = [t for t in listing["tests"]
                        if t["name"] != "t.semicolon"]
    tree_listing = os.path.join(scratch, "tree.json")
    with open(tree_listing, "w", encoding="utf-8") as f:
        json.dump(listing, f)
    ctest = os.path.join(scratch, "ctest")
    write_executable(ctest, f"""#!/bin/sh
if [ $# -ne 3 ] || [ "$1" != --test-dir ] || [ "$2" != '{tree}' ] \\
    || [ "$3" != --show-only=json-v1 ]; then
  echo "stand-in ctest: unexpected arguments: $*" >&2
  exit 3
fi
cat '{tree_listing}'
""")
    failing_ctest = os.path.join(scratch, "failing-ctest")
    write_executable(failing_ctest, """#!/bin/sh
echo "stand-in ctest: no tests were found" >&2
exit 1
""")

    def run(*arguments):
        return subprocess.run([sys.executable, "-I", tool, *arguments],
                              capture_output=True, text=True, check=False)

    with open(fixture("report.expected"), encoding="utf-8") as f:
        report = f.read()

    # The report: a column per listing, the second read from a build tree
    # through ctest.
    result = run("report", INCLUDE, f"json={LISTING}", f"tree={tree}",
                 "--ctest", ctest)
    check("report: output", result.stdout, report)
    check("report: exit", result.returncode, 0)

    # The test names of a catalogue that are absent from a listing.
    result = run("report", INCLUDE, f"json={LISTING}", f"tree={tree}",
                 "--ctest", ctest, "--check", fixture("catalogue_good.md"))
    check("check good: output", result.stdout, report
          + "\nTest names in catalogue_good.md: 1, absent from a listing: 0\n")
    check("check good: exit", result.returncode, 0)
    result = run("report", INCLUDE, f"json={LISTING}", f"tree={tree}",
                 "--ctest", ctest, "--check", fixture("catalogue_wrong.md"))
    check("check wrong: output", result.stdout, report
          + "\nTest names in catalogue_wrong.md: 2, absent from a listing: 2\n"
          "- json: core.absent\n- tree: core.absent\n")
    check("check wrong: exit", result.returncode, 1)

    # Every message pinned or listed as internal, from a listing file and
    # from a build tree.
    result = run("pinned", INCLUDE, LISTING, fixture("internal_good.txt"))
    check("pinned good: output", result.stdout, SUMMARY)
    check("pinned good: exit", result.returncode, 0)
    result = run("pinned", INCLUDE, tree, fixture("internal_good.txt"),
                 "--ctest", ctest)
    check("pinned tree: output", result.stdout,
          NOT_LISTED + "fixture.hpp:8, nested/deeper.hpp:3: "
          "second: holds; a semicolon\n")
    check("pinned tree: exit", result.returncode, 1)
    result = run("pinned", INCLUDE, LISTING, fixture("internal_missing.txt"))
    check("pinned missing: output", result.stdout,
          NOT_LISTED + "fixture.hpp:10: "
          "fourth: only in a test that is meant to fail\n")
    check("pinned missing: exit", result.returncode, 1)
    result = run("pinned", INCLUDE, LISTING, fixture("internal_drifted.txt"))
    check("pinned drifted: output", result.stdout,
          NOT_LISTED + "fixture.hpp:9: third: only forbidden\n"
          "listed as internal but in no header: third: only forbiden\n")
    check("pinned drifted: exit", result.returncode, 1)
    result = run("pinned", INCLUDE, LISTING, fixture("internal_pinned.txt"))
    check("pinned pinned: output", result.stdout,
          "listed as internal but pinned by t.run_time: fifth: run time\n")
    check("pinned pinned: exit", result.returncode, 1)

    # t.nested requires "dot: nested", a part of "antidot: nested", so it
    # must forbid a literal of the longer message that the shorter lacks:
    # with no FORBID, with one the shorter holds too, or with the literal
    # forbidden on the first error line only, it would pass on the longer.
    for name, forbid in (
            ("no forbid", []),
            ("forbid in both", ["FORBID", "nested"]),
            ("forbid first", ["FORBID_FIRST", "antidot: nested"])):
        with open(LISTING, encoding="utf-8") as f:
            variant = json.load(f)
        for test in variant["tests"]:
            if test["name"] == "t.nested":
                test["command"][6:8] = forbid
        path = os.path.join(scratch, "twin.json")
        with open(path, "w", encoding="utf-8") as f:
            json.dump(variant, f)
        result = run("pinned", INCLUDE, path, fixture("internal_good.txt"))
        check(f"pinned twin, {name}: output", result.stdout,
              'not forbidding the longer message: t.nested requires '
              '"dot: nested", a part of "antidot: nested"\n')
        check(f"pinned twin, {name}: exit", result.returncode, 1)

    # Refusals: a malformed list, a failing ctest, and no listing at all.
    for name, line, text in (
            ("internal_no_reason.txt", 3,
             'a block must start with "reason: " and the reason'),
            ("internal_twice.txt", 8,
             "listed twice: third: only forbidden")):
        result = run("pinned", INCLUDE, LISTING, fixture(name))
        check(f"{name}: output", result.stdout, "")
        check(f"{name}: exit", result.returncode, 2)
        if f"{name}:{line}: {text}" not in result.stderr:
            failures.append(f"{name}: stderr lacks line {line}'s refusal: "
                            f"{result.stderr!r}")
    result = run("pinned", INCLUDE, tree, fixture("internal_good.txt"),
                 "--ctest", failing_ctest)
    check("failing ctest: output", result.stdout, "")
    check("failing ctest: exit", result.returncode, 2)
    for text in ("exited 1", "stand-in ctest: no tests were found"):
        if text not in result.stderr:
            failures.append(f"failing ctest: stderr lacks {text!r}: "
                            f"{result.stderr!r}")
    result = run("report", INCLUDE)
    check("no listing: exit", result.returncode, 2)

    for failure in failures:
        print(failure)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
