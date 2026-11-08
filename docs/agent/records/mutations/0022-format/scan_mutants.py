#!/usr/bin/env -S python3 -I
"""Mutants of the misuse catalogue's scan, each judged by its self-test.

  scan_mutants.py <scratch dir>

Copies records/diagnostics/catalogue/scan.py and its fixtures into the
scratch directory, applies one edit at a time, and runs `--self-test`. A
mutant is killed when the self-test fails. Prints one line per mutant and
exits 1 if any survives.
"""

import pathlib
import shutil
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
CATALOGUE = HERE.parent.parent / "diagnostics" / "catalogue"

MUTANTS = {
    "forbid_values_required": ('mode = "forbid"', 'mode = "require"'),
    "semicolon_not_translated": ('argument.replace("\\x01", ";")', "argument"),
    "will_fail_counted": ('if properties.get("WILL_FAIL"):', "if False:"),
    "handler_prefix_kept": ("if literal.startswith(ASSERT_PREFIX):",
                            "if False:"),
    "ambiguous_literal_pins_first": (
        "    return max(whole, key=len) if whole else None",
        "    return found[0] if found else None"),
    "nested_literal_pins_shortest": ("return max(whole, key=len)",
                                     "return min(whole, key=len)"),
    "macro_definition_read": (
        'if bare[text_start:match.start()].lstrip().startswith("#define"):',
        "if False:"),
    "comments_read": ("        bare = strip_comments(code)",
                      "        bare = code"),
    "error_directive_ignored": ("        for match in ERROR_DIRECTIVE.finditer(bare):",
                                "        for match in []:"),
}


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    scratch = pathlib.Path(sys.argv[1]).resolve() / "scan-mutants"
    if scratch.exists():
        shutil.rmtree(scratch)
    shutil.copytree(CATALOGUE / "fixtures", scratch / "fixtures")
    original = (CATALOGUE / "scan.py").read_text(encoding="utf-8")
    survivors = 0
    for name, (old, new) in MUTANTS.items():
        if original.count(old) != 1:
            sys.exit(f"{name}: the anchor occurs {original.count(old)} times")
        (scratch / "scan.py").write_text(original.replace(old, new),
                                         encoding="utf-8")
        result = subprocess.run(
            [sys.executable, "-I", str(scratch / "scan.py"), "--self-test"],
            capture_output=True, text=True)
        killed = result.returncode != 0
        survivors += not killed
        print(f"{name}\t{'killed' if killed else 'SURVIVED'}")
    shutil.rmtree(scratch)
    return 1 if survivors else 0


if __name__ == "__main__":
    sys.exit(main())
