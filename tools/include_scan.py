"""Check the core's headers for what they include and how they are named
(constitution C13; STYLE S6; plan 0001, DoD-6). Snippet candidate.

Usage: python3 -I tools/include_scan.py <include-root>

Reads the core's headers under <include-root>: galoot/core.hpp and every
header under galoot/core/. Prints one line per finding, as
<path>[:<line>]: <finding>, and exits 1 if there is any, else 0:

- an include that names neither a C++23 standard header nor an existing core
  header (quoted includes resolve against the including file's directory);
- a cycle in the core's include graph;
- an include guard not named from the path (galoot/core/bits.hpp has
  GALOOT_CORE_BITS_HPP), as an #ifndef and #define that open the file;
- a header name that is not snake_case.hpp.

Namespaces (S6) are left to review.
"""

import os
import re
import sys

# The C++ standard library headers of C++23: ISO/IEC 14882:2024 [headers],
# Table 24 (C++ library headers) and Table 25 (C++ headers for C library
# facilities). Transcribed from the C++23 working draft N4950, not from the
# published standard, and checked only against the headers that libstdc++ 16
# installs (plan 0002, section 4.6). The deprecated headers of Annex D
# (<codecvt>, <strstream>, and the <*.h> C headers) are left out: the core
# does not use them.
STANDARD_HEADERS = frozenset("""
algorithm any array atomic barrier bit bitset charconv chrono compare complex
concepts condition_variable coroutine deque exception execution expected
filesystem flat_map flat_set format forward_list fstream functional future
generator initializer_list iomanip ios iosfwd iostream istream iterator latch
limits list locale map mdspan memory memory_resource mutex new numbers numeric
optional ostream print queue random ranges ratio regex scoped_allocator
semaphore set shared_mutex source_location span spanstream sstream stack
stacktrace stdexcept stdfloat stop_token streambuf string string_view
syncstream system_error thread tuple type_traits typeindex typeinfo
unordered_map unordered_set utility valarray variant vector version
cassert cctype cerrno cfenv cfloat cinttypes climits clocale cmath csetjmp
csignal cstdarg cstddef cstdint cstdio cstdlib cstring ctime cuchar cwchar
cwctype
""".split())

INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]')
NAME = re.compile(r"^[a-z][a-z0-9_]*\.hpp$")


def core_headers(root):
    """Returns the core's headers as paths relative to root, sorted."""
    found = []
    if os.path.isfile(os.path.join(root, "galoot", "core.hpp")):
        found.append("galoot/core.hpp")
    for directory, _, files in os.walk(os.path.join(root, "galoot", "core")):
        for name in files:
            if name.endswith(".hpp"):
                path = os.path.join(directory, name)
                found.append(os.path.relpath(path, root).replace(os.sep, "/"))
    return sorted(found)


def expected_guard(header):
    return re.sub(r"[^A-Z0-9]", "_", header.upper())


def check_guard(header, lines, findings):
    directives = [(n, line.split()) for n, line in enumerate(lines, 1)
                  if line.startswith("#")]
    guard = expected_guard(header)
    opening = [words[:2] for _, words in directives[:2]]
    if opening != [["#ifndef", guard], ["#define", guard]]:
        line = directives[0][0] if directives else 1
        findings.append(f"{header}:{line}: include guard should be {guard}")


def scan(root):
    headers = core_headers(root)
    known = set(headers)
    findings = []
    graph = {}

    for header in headers:
        name = header.rsplit("/", 1)[-1]
        if not NAME.match(name):
            findings.append(f"{header}: header name is not snake_case.hpp")
        with open(os.path.join(root, header), encoding="utf-8") as f:
            lines = f.read().splitlines()
        check_guard(header, lines, findings)

        graph[header] = []
        for number, line in enumerate(lines, 1):
            match = INCLUDE.match(line)
            if not match:
                continue
            delimiter, target = match.groups()
            if delimiter == '"':
                base = os.path.dirname(header)
                target = os.path.normpath(os.path.join(base, target))
            if target in known:
                graph[header].append(target)
            elif delimiter == "<" and target in STANDARD_HEADERS:
                pass
            else:
                findings.append(
                    f"{header}:{number}: {delimiter}{match.group(2)}"
                    f"{'>' if delimiter == '<' else delimiter} is not a C++23 "
                    "standard header or a core header")

    findings.extend(cycles(graph))
    return findings


def cycles(graph):
    """Reports each include cycle once, from its first header in sort order."""
    findings = []
    state = {}  # header -> "open" while on the path, "done" after
    path = []

    def visit(header):
        state[header] = "open"
        path.append(header)
        for target in graph[header]:
            if state.get(target) == "open":
                loop = path[path.index(target):] + [target]
                findings.append(f"{loop[0]}: include cycle: "
                                + " -> ".join(loop))
            elif target not in state:
                visit(target)
        path.pop()
        state[header] = "done"

    for header in sorted(graph):
        if header not in state:
            visit(header)
    return findings


def main(argv):
    if len(argv) != 1:
        sys.exit("usage: include_scan.py <include-root>")
    findings = scan(argv[0])
    for finding in findings:
        print(finding)
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
