"""Compare each compile-fail test's output run alone with its output in a
parallel run of the whole suite. Usage:
alone_vs_together.py <preset> <test list> <out dir>
Runs from the worktree root with the pool exported, every ctest under tools/timed."""
import json
import os
import re
import subprocess
import sys

preset, test_list, out = sys.argv[1:4]
os.makedirs(out, exist_ok=True)
names = open(test_list).read().split()
listing = json.loads(subprocess.run(
    ["ctest", "--preset", preset, "--show-only=json-v1"],
    capture_output=True, text=True, check=True).stdout)
number = {t["name"]: i + 1 for i, t in enumerate(listing["tests"])}
BOOKKEEPING = re.compile(r"^(Test command|Working Directory|Test timeout|"
                         r"Environment variables|Test Pass Reason|"
                         r"Test Passed|Test Failed)")


def lines_of(text, n):
    prefix = f"{n}: "
    kept = [line[len(prefix):] for line in text.splitlines()
            if line.startswith(prefix)]
    return [line for line in kept if not BOOKKEEPING.match(line.strip())]


together = subprocess.run(
    ["tools/timed", "test", preset, "all-verbose", "--",
     "ctest", "--preset", preset, "-V"], capture_output=True, text=True)
open(f"{out}/together.log", "w").write(together.stdout)
same = 0
for name in names:
    alone = subprocess.run(
        ["tools/timed", "test", preset, f"alone:{name}", "--",
         "ctest", "--preset", preset, "-V", "-R", f"^{re.escape(name)}$"],
        capture_output=True, text=True)
    a = lines_of(alone.stdout, number[name])
    t = lines_of(together.stdout, number[name])
    if a == t and a:
        same += 1
    else:
        print(f"{name}: DIFFERENT ({len(a)} lines alone, {len(t)} together)")
        open(f"{out}/{name}.alone", "w").write("\n".join(a))
        open(f"{out}/{name}.together", "w").write("\n".join(t))
print(f"{preset}: {same} of {len(names)} tests give the same output alone and "
      f"in the parallel run (exit of the parallel run {together.returncode})")
