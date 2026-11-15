"""Extract the examples of a Markdown page and check them, so that every
example a reader copies from the README or the guide compiles and runs, every
rejection it shows is the library's own, and every page is under test.

Usage:
  python3 -I tools/doc_examples.py extract [--info cmake] <page> <out-dir>
  python3 -I tools/doc_examples.py check <page>
  python3 -I tools/doc_examples.py run <example> <program> [<arg>...]
  python3 -I tools/doc_examples.py pages <docs-dir> <registered-page>...

An example is a fenced code block whose info string is cpp, a whole program:

  ```cpp
  int main() {}
  ```

A block marked cpp rejected is a whole translation unit that must fail to
compile, and it holds exactly one line `// rejected: <message>` that names
the message the compile must fail with:

  ```cpp rejected
  // rejected: the message
  static_assert(false, "the message");
  ```

A block marked cmake is a whole CMakeLists.txt of a project that uses the
library. Blocks with other info strings, such as text, are not extracted.

A fence is three or more backticks or tildes, and the block ends at a line
of the same character, at least as many, and nothing else. The fence may be
indented, as in a list item; that indentation is removed from the lines of
the block.

`extract` writes the n-th cpp block to <out-dir>/example_<n>.cpp and the
n-th cpp rejected block to <out-dir>/rejected_<n>.cpp, each with a #line
directive so that the compiler reports its lines as lines of <page>; with
--info cmake it writes the n-th cmake block to <out-dir>/example_<n>.cmake
instead. It prints the path of each file written, one per line. A file whose
text has not changed is left alone, so that the build does not compile it
again, and a file of the same kind beyond the last block is removed. When
the page is malformed it writes nothing, removes every file of that kind,
prints the defect, and exits 1.

`check` exits 1, naming the defect, when <page> has no cpp or cpp rejected
block, so that a fence lost from it fails rather than leaving an example
nobody compiles. It also fails on a fence that is never closed, on a block
marked as C++ with another info string, such as c++ or cxx, which would be
shown as C++ but not extracted, and on a cpp rejected block without exactly
one `// rejected:` line.

`run` runs <program> with its arguments and passes when the program exits
0 and, if <example> holds lines of the form `// prints: <text>`, its output
is exactly those texts, one line each, in order.

`pages` exits 1, naming each one, when a Markdown file under <docs-dir>,
outside its agent/ directory, is not among the registered pages, so that a
new page nobody registered fails rather than going untested.
"""

import os
import re
import subprocess
import sys

FENCE = re.compile(r"^(\s*)(`{3,}|~{3,})(.*)$")
PRINTS = re.compile(r"^\s*// prints: (.*)$")
REJECTED = re.compile(r"^\s*// rejected: \S")

# The file each kind of block is written to, and the kinds each --info
# extracts.
FILE_NAME = {"cpp": "example_{}.cpp", "rejected": "rejected_{}.cpp",
             "cmake": "example_{}.cmake"}
FILE_PATTERN = {"cpp": re.compile(r"^example_(\d+)\.cpp$"),
                "rejected": re.compile(r"^rejected_(\d+)\.cpp$"),
                "cmake": re.compile(r"^example_(\d+)\.cmake$")}
EXTRACTED = {"cpp": ("cpp", "rejected"), "cmake": ("cmake",)}

# Info strings that mark a block as C++ in Markdown renderers but are not
# the one this script extracts.
OTHER_CPP_NAMES = frozenset({"c++", "cc", "cpp", "cxx", "h++", "hpp"})


def kind_of(info):
    """The kind of a block from its info string: cpp, rejected, cmake, or
    None for a block that is not extracted."""
    words = info.split()
    name = words[0] if words else ""
    if name == "cpp":
        return "rejected" if words[1:2] == ["rejected"] else "cpp"
    if name == "cmake":
        return "cmake"
    return None


def blocks(path):
    """Return the extracted blocks of the Markdown file at path as a list
    of (kind, first line number, text), or raise ValueError naming a
    defect."""
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
            fence = (indent, marker, kind_of(info), number, [])
            continue
        indent, marker, kind, start, body = fence
        if (match and match.group(2)[0] == marker[0]
                and len(match.group(2)) >= len(marker)
                and not match.group(3).strip()):
            if kind == "rejected":
                count = sum(1 for b in body if REJECTED.match(b))
                if count != 1:
                    raise ValueError(
                        f"{path}:{start}: a cpp rejected block needs one line "
                        f"// rejected: <message>, naming the message its "
                        f"compile must fail with, and this one has {count}")
            if kind is not None:
                found.append((kind, start + 1, "\n".join(body) + "\n"))
            fence = None
        else:
            body.append(line[len(indent):] if line.startswith(indent)
                        else line.lstrip())
    if fence is not None:
        raise ValueError(f"{path}:{fence[3]}: the fence opened here is "
                         f"never closed")
    return found


def remove_files(out_dir, kind, keep):
    """Remove each file of kind in out_dir numbered above keep."""
    for name in os.listdir(out_dir):
        match = FILE_PATTERN[kind].match(name)
        if match and int(match.group(1)) > keep:
            os.remove(os.path.join(out_dir, name))


def extract(page, out_dir, info):
    os.makedirs(out_dir, exist_ok=True)
    kinds = EXTRACTED[info]
    try:
        found = [b for b in blocks(page) if b[0] in kinds]
    except ValueError as defect:
        for kind in kinds:
            remove_files(out_dir, kind, 0)
        print(defect, file=sys.stderr)
        return 1

    source = os.path.abspath(page).replace("\\", "\\\\").replace('"', '\\"')
    counts = dict.fromkeys(kinds, 0)
    for kind, first_line, text in found:
        counts[kind] += 1
        target = os.path.join(out_dir, FILE_NAME[kind].format(counts[kind]))
        content = text if kind == "cmake" else (
            f'#line {first_line} "{source}"\n{text}')
        try:
            with open(target, encoding="utf-8") as f:
                unchanged = f.read() == content
        except FileNotFoundError:
            unchanged = False
        if not unchanged:
            with open(target, "w", encoding="utf-8") as f:
                f.write(content)
        print(target)
    for kind in kinds:
        remove_files(out_dir, kind, counts[kind])
    return 0


def check(page):
    try:
        found = blocks(page)
    except ValueError as defect:
        print(defect)
        return 1
    programs = sum(1 for b in found if b[0] == "cpp")
    rejected = sum(1 for b in found if b[0] == "rejected")
    if not programs and not rejected:
        print(f"{page}: no ```cpp block, so no example is compiled; a "
              f"C++ example needs a fence marked cpp")
        return 1
    print(f"{page}: {programs} ```cpp block(s) and {rejected} ```cpp "
          f"rejected block(s)")
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


def pages(docs_dir, registered):
    if not os.path.isdir(docs_dir):
        print(f"{docs_dir}: no such directory")
        return 1
    known = {os.path.realpath(p) for p in registered}
    found, missing = 0, []
    for directory, subdirectories, files in os.walk(docs_dir):
        if directory == docs_dir and "agent" in subdirectories:
            subdirectories.remove("agent")  # The maintainers' notes.
        for name in files:
            if name.endswith(".md"):
                path = os.path.join(directory, name)
                found += 1
                if os.path.realpath(path) not in known:
                    missing.append(path)
    for path in sorted(missing):
        print(f"{path}: no test registers this page, so its examples are "
              f"not compiled; register it with galoot_add_doc_examples in "
              f"test/documentation.cmake")
    if missing:
        return 1
    print(f"{found} pages, each registered")
    return 0


def main(argv):
    if len(argv) == 3 and argv[0] == "extract":
        return extract(argv[1], argv[2], "cpp")
    if len(argv) == 5 and argv[:3] == ["extract", "--info", "cmake"]:
        return extract(argv[3], argv[4], "cmake")
    if len(argv) == 2 and argv[0] == "check":
        return check(argv[1])
    if len(argv) >= 3 and argv[0] == "run":
        return run(argv[1], argv[2:])
    if len(argv) >= 2 and argv[0] == "pages":
        return pages(argv[1], argv[2:])
    sys.exit("usage: doc_examples.py extract [--info cmake] <page> <out-dir>\n"
             "       doc_examples.py check <page>\n"
             "       doc_examples.py run <example> <program> [<arg>...]\n"
             "       doc_examples.py pages <docs-dir> <registered-page>...")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
