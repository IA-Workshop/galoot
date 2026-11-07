"""Extract the C++ examples of a Markdown file and check what they print, so
that every example a reader copies from the README compiles and runs.

Usage:
  python3 -I tools/readme_examples.py extract <readme> <out-dir>
  python3 -I tools/readme_examples.py check <readme>
  python3 -I tools/readme_examples.py run <example> <program> [<arg>...]

An example is a fenced code block whose info string is cpp:

  ```cpp
  int main() {}
  ```

A fence is three or more backticks or tildes, and the block ends at a line
of the same character, at least as many, and nothing else. The fence may be
indented, as in a list item; that indentation is removed from the lines of
the block.

`extract` writes the n-th example to <out-dir>/example_<n>.cpp, with a #line
directive so that the compiler reports its lines as lines of <readme>, and
prints the path of each file written, one per line. A file whose text has
not changed is left alone, so that the build does not compile it again, and
an example_<n>.cpp beyond the last example is removed. When the README is
malformed it writes nothing, removes every example, prints the defect, and
exits 1.

`check` exits 1, naming the defect, when <readme> has no example, so that a
fence lost from it fails rather than leaving an example nobody compiles. It
also fails on a fence that is never closed, and on a block marked as C++
with another info string, such as c++ or cxx, which would be shown as C++
but not extracted.

`run` runs <program> with its arguments and passes when the program exits
0 and, if <example> holds lines of the form `// prints: <text>`, its output
is exactly those texts, one line each, in order.
"""

import os
import re
import subprocess
import sys

FENCE = re.compile(r"^(\s*)(`{3,}|~{3,})(.*)$")
PRINTS = re.compile(r"^\s*// prints: (.*)$")
EXAMPLE_FILE = re.compile(r"^example_(\d+)\.cpp$")

# Info strings that mark a block as C++ in Markdown renderers but are not
# the one this script extracts.
OTHER_CPP_NAMES = frozenset({"c++", "cc", "cpp", "cxx", "h++", "hpp"})


def examples(path):
    """Return the examples of the Markdown file at path as a list of
    (first line number, text), or raise ValueError naming a defect."""
    with open(path, encoding="utf-8") as f:
        lines = f.read().splitlines()

    found = []
    fence = None
    for number, line in enumerate(lines, start=1):
        match = FENCE.match(line)
        if fence is None:
            if not match:
                continue
            indent, marker, info = match.groups()
            if marker[0] == "`" and "`" in info:
                continue  # Inline code that opens a line, not a fence.
            words = info.split()
            name = words[0] if words else ""
            if name != "cpp" and name.lower() in OTHER_CPP_NAMES:
                raise ValueError(f"{path}:{number}: the block is marked "
                                 f"{name}; mark C++ examples cpp so that "
                                 f"they are compiled")
            fence = (indent, marker, name == "cpp", number, [])
            continue
        indent, marker, is_cpp, start, body = fence
        if (match and match.group(2)[0] == marker[0]
                and len(match.group(2)) >= len(marker)
                and not match.group(3).strip()):
            if is_cpp:
                found.append((start + 1, "\n".join(body) + "\n"))
            fence = None
        else:
            body.append(line[len(indent):] if line.startswith(indent)
                        else line.lstrip())
    if fence is not None:
        raise ValueError(f"{path}:{fence[3]}: the fence opened here is "
                         f"never closed")
    return found


def remove_examples(out_dir, keep):
    """Remove each example_<n>.cpp in out_dir with n above keep."""
    for name in os.listdir(out_dir):
        match = EXAMPLE_FILE.match(name)
        if match and int(match.group(1)) > keep:
            os.remove(os.path.join(out_dir, name))


def extract(readme, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    try:
        found = examples(readme)
    except ValueError as defect:
        remove_examples(out_dir, 0)
        print(defect, file=sys.stderr)
        return 1

    source = os.path.abspath(readme).replace("\\", "\\\\").replace('"', '\\"')
    for n, (first_line, text) in enumerate(found, start=1):
        target = os.path.join(out_dir, f"example_{n}.cpp")
        content = f'#line {first_line} "{source}"\n{text}'
        try:
            with open(target, encoding="utf-8") as f:
                unchanged = f.read() == content
        except FileNotFoundError:
            unchanged = False
        if not unchanged:
            with open(target, "w", encoding="utf-8") as f:
                f.write(content)
        print(target)
    remove_examples(out_dir, len(found))
    return 0


def check(readme):
    try:
        found = examples(readme)
    except ValueError as defect:
        print(defect)
        return 1
    if not found:
        print(f"{readme}: no ```cpp block, so no example is compiled; a "
              f"C++ example needs a fence marked cpp")
        return 1
    print(f"{readme}: {len(found)} ```cpp block(s)")
    return 0


def run(example, command):
    with open(example, encoding="utf-8") as f:
        expected = [m.group(1) for m in map(PRINTS.match, f) if m]
    result = subprocess.run(command, stdout=subprocess.PIPE, text=True,
                            check=False)
    sys.stdout.write(result.stdout)
    if result.returncode != 0:
        print(f"{example}: the program exited {result.returncode}, not 0")
        return 1
    actual = result.stdout.splitlines()
    if expected and actual != expected:
        print(f"{example}: the program printed {actual!r}, but its "
              f"// prints: comments say {expected!r}")
        return 1
    return 0


def main(argv):
    if len(argv) == 3 and argv[0] == "extract":
        return extract(argv[1], argv[2])
    if len(argv) == 2 and argv[0] == "check":
        return check(argv[1])
    if len(argv) >= 3 and argv[0] == "run":
        return run(argv[1], argv[2:])
    sys.exit("usage: readme_examples.py extract <readme> <out-dir>\n"
             "       readme_examples.py check <readme>\n"
             "       readme_examples.py run <example> <program> [<arg>...]")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
