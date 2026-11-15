"""The hand mutants of `cost.py comments` (plan 0040 part D1), each run
against comments_test.py.

Usage, from the root of a checkout:
  python3 -I <this file> docs/agent/snippets/plan_cost_model

tools/mutate judges mutants by CTest tests, and comments_test.py is not
one, so these run here. Each mutant is one edit of cost.py, made in a
temporary copy of the snippet's directory under the checkout's build/,
which git ignores, never in the snippet itself, and comments_test.py runs
in that copy; the copy must be inside the checkout because harness.py
asks git for the repository's root. Prints one tab-separated row per
mutant: its name, killed or survived, the tests run, and the tests that
failed. The first row is a control with no edit, which must survive.
Exit 1 if the control is killed or any other mutant survives, else 0.
"""

import pathlib
import shutil
import subprocess
import sys
import tempfile

MUTANTS = {
    "control_unmutated": ("HEADING = re.compile", "HEADING = re.compile"),
    "section_ignored": ("        read = section_lines(path, lines, section)",
                        "        read = list(enumerate(lines, 1))"),
    "missing_section_accepted": ("    if times_found == 0:\n        raise",
                                 "    if False:\n        raise"),
    "twice_accepted": ("    if times_found > 1:\n        raise",
                       "    if False:\n        raise"),
    "empty_section_accepted": ("    if section is not None and not found:",
                               "    if False:"),
    "markdown_without_section_accepted": ('    elif path.endswith(".md"):',
                                          "    elif False:"),
    "fence_ignored": (
        "        heading = None if fence or marker else HEADING.match(line)",
        "        heading = HEADING.match(line)"),
    "section_never_ends": (
        "            if level is not None and depth <= level:",
        "            if False:"),
    "subsection_ends_section": ("depth <= level:", "depth >= 1:"),
}


def main(argv):
    if len(argv) != 1:
        sys.exit(__doc__)
    snippet = pathlib.Path(argv[0])
    text = (snippet / "cost.py").read_text(encoding="utf-8")
    problems = 0
    build = pathlib.Path("build")
    build.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(dir=build) as scratch:
        copy = pathlib.Path(scratch) / "plan_cost_model"
        shutil.copytree(snippet, copy)
        for name, (old, new) in MUTANTS.items():
            if text.count(old) != 1:
                sys.exit(f"{name}: the anchor occurs {text.count(old)} times")
            (copy / "cost.py").write_text(text.replace(old, new),
                                          encoding="utf-8")
            result = subprocess.run(
                [sys.executable, "-I", str(copy / "comments_test.py")],
                capture_output=True, text=True, check=False)
            lines = result.stderr.splitlines()
            failed = [line.split()[1] for line in lines
                      if line.startswith(("FAIL:", "ERROR:"))]
            ran = next((line for line in lines if line.startswith("Ran ")),
                       "Ran ?")
            verdict = "killed" if result.returncode else "survived"
            expected = "survived" if name == "control_unmutated" else "killed"
            problems += verdict != expected
            print(f"{name}\t{verdict}\t{ran}\t{','.join(failed) or '-'}")
    sys.exit(1 if problems else 0)


if __name__ == "__main__":
    main(sys.argv[1:])
