"""Shows that the regeneration check of golden.hpp fails on a changed copy,
so that a hand edit or a stale header cannot pass it.

Usage: python3 -I golden_check_test.py <golden.py> <golden.hpp> <scratch dir>

Writes two altered copies of the committed header into the scratch
directory, each differing from it in one place, and runs
`golden.py --check` on each. Exits 1 unless every copy is rejected with exit
status 1 and a diff that shows the altered line:

- one sign flipped: the first right complement whose sign is -1 becomes +1;
- one blank line added at the end, a change no compiler would notice.
"""

import os
import subprocess
import sys


def flip_first_negative_complement(text):
    start = text.index("namespace complements {")
    line_start = text.index("{0b", start)
    line_end = text.index("\n", line_start)
    line = text[line_start:line_end]
    while ", -1}, {" not in line:
        line_start = line_end + 1
        line_end = text.index("\n", line_start)
        line = text[line_start:line_end]
    altered = line.replace(", -1}, {", ", 1}, {", 1)
    return text[:line_start] + altered + text[line_end:], f"-{altered}"


def add_blank_line(text):
    return text + "\n", "-"


def main(golden_py, golden_hpp, scratch):
    os.makedirs(scratch, exist_ok=True)
    with open(golden_hpp, encoding="utf-8", newline="") as f:
        committed = f.read()

    failed = False
    for name, alter in (("flipped-sign", flip_first_negative_complement),
                        ("added-blank-line", add_blank_line)):
        # The diff runs from the altered copy to the regenerated text, so the
        # altered line appears as a removal.
        text, expected_diff_line = alter(committed)
        path = os.path.join(scratch, f"golden-{name}.hpp")
        with open(path, "w", encoding="utf-8", newline="") as f:
            f.write(text)
        run = subprocess.run(
            [sys.executable, "-I", golden_py, "--check", path],
            capture_output=True, text=True, check=False)
        print(f"--- {name}: exit {run.returncode}")
        print(run.stdout + run.stderr)
        shown = expected_diff_line in run.stdout.splitlines()
        if run.returncode != 1 or not shown:
            print(f"golden_check_test: {name} was not rejected with its diff")
            failed = True
    return 1 if failed else 0


if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.exit("usage: golden_check_test.py <golden.py> <golden.hpp> "
                 "<scratch dir>")
    sys.exit(main(*sys.argv[1:]))
