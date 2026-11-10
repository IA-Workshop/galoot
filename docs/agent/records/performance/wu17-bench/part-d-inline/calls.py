"""Counts, in each benchmark program's assembly, the calls and symbol jumps
to the program's own form lambdas (main's lambda #1 and #2 on GCC, $_0 and
$_1 on Clang), to reference_kernels, and to functions of galoot::core.

    python3 -I calls.py <file.s>...

Prints one line per file: each kind of target and its number of calls. A
reference kernel or a form lambda in the list is a call of the program's
own in the timed loop; a galoot::core target is the library's. A target
whose demangled name begins with its return type, such as
std::array<...> galoot::core::detail::..., is listed under the first word
of that name, which the line shows as it comes.
"""
import re
import subprocess
import sys


def qualified(demangled):
    """The qualified name of a demangled function: its template arguments,
    parameters and return type removed."""
    text = demangled
    while True:
        shorter = re.sub(r"<[^<>]*>", "", text)
        if shorter == text:
            break
        text = shorter
    return text.split("(")[0].split(" ")[-1]


for path in sys.argv[1:]:
    targets = re.findall(r"^\s+(?:call|callq|jmp|jmpq)\s+\*?([^\s#]+)", open(path).read(), re.M)
    targets = [t.split("@")[0] for t in targets if not t.startswith(".L")]
    dem = subprocess.run(["c++filt"], input="\n".join(targets), capture_output=True, text=True).stdout.splitlines()
    found = {}
    for mangled, d in zip(targets, dem):
        m = re.match(r"main::(\$_\d|\{lambda\(.*?\)#\d\})::operator\(\)", d)
        library = qualified(d).startswith("galoot::core::")
        if m:
            name = m.group(1)
            key = "main lambda " + (name if name.startswith("$")
                                    else "#" + name.split("#")[-1][:-1])
        elif "reference_kernels::" in d:
            key = "reference_kernels"
        elif library:
            key = qualified(d)
        elif mangled.startswith("_ZN6galoot4core"):
            # A name c++filt cannot demangle: read its nested names, each
            # written as its length and its letters.
            parts, rest = [], mangled[3:]
            while (n := re.match(r"\d+", rest)) and len(parts) < 4:
                size = int(n.group(0))
                start = len(n.group(0))
                parts.append(rest[start:start + size])
                rest = rest[start + size:]
            key = "::".join(parts)
        else:
            continue
        found[key] = found.get(key, 0) + 1
    print(f"{path.split('/')[-2]}/{path.split('/')[-1]}: {found or 'none'}")
