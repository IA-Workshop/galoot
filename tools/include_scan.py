"""Check the headers of the core and the rga3 model for what they include
and how they are named. The core must build from its headers alone, with
nothing but the standard library, and the model from the standard library
and the core's headers, so a stray include is a defect even when the machine
happens to provide the header.

Usage: python3 -I tools/include_scan.py <include-root> [<module>...]

Reads the headers of each module named, core or rga3, or of both when none
is: galoot/<module>.hpp and every header under galoot/<module>/. Prints one
line per finding, as <path>[:<line>]: <finding>, and exits 1 if there is
any, else 0:

- an include that names neither a C++23 standard header nor an existing
  header of a module the including module may use: the core uses only its
  own headers, and the model the core's and its own (quoted includes
  resolve against the including file's directory);
- an include of an existing header of a module that the including module may
  not use, such as a core header that includes a model header;
- an #include_next, whose target depends on the order of the user's include
  paths, and a computed #include, whose target this scan cannot know;
- a cycle among the headers of one module (no cycle can cross modules,
  since the core includes no model header);
- an include guard not named from the path (galoot/core/bits.hpp has
  GALOOT_CORE_BITS_HPP), as an #ifndef and #define that open the file;
- a header name that is not snake_case.hpp, and a file under
  galoot/<module>/ that is not a .hpp header.

Namespaces are left to review.
"""

import os
import re
import sys

# The C++ standard library headers of C++23: ISO/IEC 14882:2024 [headers],
# Table 24 (C++ library headers) and Table 25 (C++ headers for C library
# facilities). Transcribed from the C++23 working draft N4950, not from the
# published standard, and checked only against the headers that libstdc++ 16
# installs. The deprecated headers of Annex D (<codecvt>, <strstream>, and the
# <*.h> C headers) are left out: the core does not use them.
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

DIRECTIVE = re.compile(r"^\s*#\s*(include_next|include)\b\s*(.*)$")
HEADER_NAME = re.compile(r'^([<"])([^>"]+)[>"]')
NAME = re.compile(r"^[a-z][a-z0-9_]*\.hpp$")


# Each module: how findings name it, the modules whose headers it may
# include besides the standard library's, and the text of the finding for an
# include that names none of them.
MODULES = {
    "core": ("the core", ("core",),
             "a C++23 standard header or a core header"),
    "rga3": ("the rga3 model", ("core", "rga3"),
             "a C++23 standard header, a core header, or an rga3 header"),
}


def module_files(root, module):
    """Returns the module's headers and the other files under
    galoot/<module>/, each as sorted paths relative to root."""
    headers, others = [], []
    if os.path.isfile(os.path.join(root, "galoot", module + ".hpp")):
        headers.append(f"galoot/{module}.hpp")
    for directory, _, files in os.walk(os.path.join(root, "galoot", module)):
        for name in files:
            path = os.path.join(directory, name)
            path = os.path.relpath(path, root).replace(os.sep, "/")
            (headers if name.endswith(".hpp") else others).append(path)
    return sorted(headers), sorted(others)


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


def scan(root, modules):
    """Returns the findings for the headers of the modules named."""
    owner = {}  # every header of every module on disk -> its module
    files = {}
    for module in MODULES:
        files[module] = module_files(root, module)
        owner.update((header, module) for header in files[module][0])

    findings = []
    graph = {}
    for module in modules:
        headers, others = files[module]
        findings += [f"{path}: not a .hpp header" for path in others]
        for header in headers:
            graph[header] = scan_header(root, header, module, owner,
                                        findings)

    findings.extend(cycles(graph))
    return findings


def scan_header(root, header, module, owner, findings):
    """Appends the header's findings and returns the headers of its own
    module that it includes."""
    name_of, uses, allowed = MODULES[module]
    name = header.rsplit("/", 1)[-1]
    if not NAME.match(name):
        findings.append(f"{header}: header name is not snake_case.hpp")
    with open(os.path.join(root, header), encoding="utf-8") as f:
        lines = f.read().splitlines()
    check_guard(header, lines, findings)

    edges = []
    for number, line in enumerate(lines, 1):
        match = DIRECTIVE.match(line)
        if not match:
            continue
        directive, operand = match.groups()
        if directive == "include_next":
            findings.append(f"{header}:{number}: #include_next depends "
                            "on the order of the user's include paths")
            continue
        name = HEADER_NAME.match(operand)
        if not name:
            operand = operand.split("//")[0].strip()
            findings.append(f"{header}:{number}: computed #include "
                            f"{operand} cannot be checked")
            continue
        delimiter, target = name.groups()
        spelled = (f"{delimiter}{target}"
                   f"{'>' if delimiter == '<' else delimiter}")
        if delimiter == '"':
            base = os.path.dirname(header)
            target = os.path.normpath(os.path.join(base, target))
        if target in owner and owner[target] in uses:
            if owner[target] == module:
                edges.append(target)
        elif target in owner:
            findings.append(f"{header}:{number}: {spelled} belongs to "
                            f"{MODULES[owner[target]][0]}, which {name_of} "
                            "may not include")
        elif delimiter == "<" and target in STANDARD_HEADERS:
            pass
        else:
            findings.append(f"{header}:{number}: {spelled} is not {allowed}")
    return edges


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
    if not argv or any(m not in MODULES for m in argv[1:]):
        sys.exit("usage: include_scan.py <include-root> "
                 f"[{' | '.join(MODULES)}]...")
    findings = scan(argv[0], argv[1:] or list(MODULES))
    for finding in findings:
        print(finding)
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
