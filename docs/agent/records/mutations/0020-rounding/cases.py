#!/usr/bin/env -S python3 -I
"""Which test cases each mutant of mutate.py fails, on gcc-debug.

  cases.py <path of mutate.py>

Applies each mutant, builds the two programs through tools/timed (selection
mutant-cases:<name>), runs them directly, prints the names of the test cases
that report an error, and restores the file. Usage, from the worktree root
with the job pool exported.
"""
import importlib.util, subprocess, sys, re, pathlib
spec = importlib.util.spec_from_file_location("mutate", sys.argv[1])
m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
root = m.ROOT
for name in m.MUTANTS:
    rel, edits = m.MUTANTS[name]
    path = root / rel
    orig = path.read_text(); text = orig
    for old, new, occ in edits:
        text = m.mutate(text, old, new, occ)
    path.write_text(text)
    try:
        b = m.timed("build", "gcc-debug", f"mutant-cases:{name}", ["ninja", "-C", "build/gcc-debug", *m.TARGETS])
        cases = []
        for prog in ["build/gcc-debug/test/core_rounding_test", "build/gcc-debug/test/support_tracked_test"]:
            r = subprocess.run([str(root / prog)], capture_output=True, text=True)
            out = r.stdout
            blocks = out.split("=" * 79)
            for blk in blocks:
                if "ERROR" in blk:
                    t = re.search(r"TEST CASE:\s+(.*)", blk)
                    if t and t.group(1) not in cases: cases.append(t.group(1).strip())
        print(name, "|", "; ".join(cases))
    finally:
        path.write_text(orig)
subprocess.run(["ninja", "-C", str(root / "build/gcc-debug"), *m.TARGETS], capture_output=True)
