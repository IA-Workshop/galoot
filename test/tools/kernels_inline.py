"""Checks that a unit's operations compile without a call into the library.

Usage:
  python3 -I kernels_inline.py <function>... -- <command>...
  python3 -I kernels_inline.py --alone <source> <function>... -- <command>...

Runs the command, which writes x86-64 assembly in the AT&T syntax to its
standard output (a compile with -S -o -), and checks three things:

- the output is x86-64 assembly: some instruction names a register of
  x86-64 (%rax, %eax, %r8d, %xmm0 and the like), or every instruction is a
  return, so that the output of another target, whose calls this check
  would not read, is refused rather than passed (the build registers the
  tests only for an x86-64 target; this refusal is a second guard);
- every named function is defined in the output: some label is its mangled
  name, _Z, the length of the name, then the name, as the compilers mangle a
  function at namespace scope, so that a function the compiler dropped or
  renamed cannot pass by having no code to check;
- no call (call, callq, calll), jump (jmp, jmpq) or conditional jump (jne
  and the other j<cc>), with or without a notrack or bnd prefix, has a
  symbol of namespace galoot as its target: named directly, through the
  PLT (sym@PLT), through the global offset table (*sym@GOTPCREL(%rip)), or
  through a register that an earlier lea or mov in the same function loaded
  with the symbol's address (sym(%rip), or $sym as an immediate), into the
  register or a narrower part of it. Such a symbol's
  mangled name starts with _ZN, then any of the qualifiers K, V, r, R, O
  and H that a member function's cv-, ref- and explicit-object
  qualification add, then 6galoot; or, for a lambda or another entity local
  to a function of the library, with _ZZN and the same. A call to any other
  symbol, such as one of the standard library, and a jump to a local label
  (.L...) are allowed. Not read: a call through a register loaded in any
  other way (copied from another register, or computed), a call through
  memory other than the global offset table, and a branch with a prefix
  other than notrack or bnd.

With --alone, the command is a compile without its source, and each named
function is compiled in a unit of its own: the text of the source without
the paragraphs that define the named functions, then the paragraph that
defines this one. A paragraph is a run of lines between blank lines; it
defines a function when its first line that is not a comment starts a
declaration of the function's name followed by "(". A compiler weighs the
growth of a unit against its size, so an operation that inlines among many
others can stay a call when it is alone.

Prints each offending line with the function it is in, and each function
that is missing. Exits 1 if there is any, if the output is not x86-64
assembly, or if a command fails; 0 otherwise.
"""

import pathlib
import re
import subprocess
import sys
import tempfile

# A label at the start of a line: a symbol, then a colon, then nothing but a
# comment.
LABEL = re.compile(r"^([A-Za-z_.$][\w.$@]*):\s*(?:#.*)?$")
# An instruction that transfers control, with its operand.
BRANCH = re.compile(r"^\s+(?:(?:notrack|bnd)\s+)?(call[lq]?|j[a-z]+)\s+(\S+)")
# An instruction that loads a register from an operand: lea or mov, in any
# of their suffixed forms, with the register last.
LOAD = re.compile(r"^\s+(?:lea|mov)[a-z]*\s+(\S+),\s*(%\w+)\s*(?:#.*)?$")
# The mangled name of a symbol in namespace galoot, or of an entity local to
# one of its functions, anywhere in an operand: after a * for an indirect
# call, after a $ for an immediate, and before a suffix such as @PLT.
LIBRARY = re.compile(r"(?<![\w.])_ZZ?N[KVrROH]*6galoot[\w.$]*")
# A register of x86-64, of any width, as an instruction's operand names it.
X86_64 = re.compile(r"%(?:[re]?(?:[abcd]x|[sd]i|[sb]p|ip)|[abcd][lh]|[sd]il|"
                    r"[sb]pl|r(?:[89]|1[0-5])[dwb]?|[xyz]mm\d+|k[0-7])\b")
# An instruction that only returns.
RETURN = re.compile(r"^\s+(?:rep\s+)?retq?\s*(?:#.*)?$")
# The 64-bit register that holds a narrower one: %eax, %ax and %al are parts
# of %rax, %r8d of %r8.
WIDE = {f"%{low}": f"%r{base}" for base in ("ax", "bx", "cx", "dx")
        for low in (f"e{base}", base, f"{base[0]}l", f"{base[0]}h")}
WIDE |= {f"%{low}": f"%r{base}" for base in ("si", "di", "bp", "sp")
         for low in (f"e{base}", base, f"{base}l")}
WIDE |= {f"%r{n}{suffix}": f"%r{n}" for n in range(8, 16)
         for suffix in ("d", "w", "b")}


def wide(register):
    """The 64-bit register that `register` names or is a part of."""
    return WIDE.get(register, register)


def check(assembly, functions):
    """The offending lines and the missing functions of `assembly`, and
    whether it is x86-64 assembly."""
    labels = set()
    offending = []
    current = "(no function)"
    loaded = {}
    x86_64 = False
    only_returns = True
    for number, line in enumerate(assembly.splitlines(), start=1):
        label = LABEL.match(line)
        if label:
            labels.add(label.group(1))
            if not label.group(1).startswith(".L"):
                current = label.group(1)
                loaded = {}
            continue
        instruction = line.strip() and not line.lstrip().startswith(
            (".", "#"))
        if instruction:
            x86_64 = x86_64 or X86_64.search(line) is not None
            only_returns = only_returns and RETURN.match(line) is not None
        load = LOAD.match(line)
        if load:
            symbol = LIBRARY.search(load.group(1))
            if symbol:
                loaded[wide(load.group(2))] = symbol.group(0)
            else:
                loaded.pop(wide(load.group(2)), None)
            continue
        branch = BRANCH.match(line)
        if not branch:
            continue
        operand = branch.group(2)
        where = f"line {number}, in {current}: {line.strip()}"
        if LIBRARY.search(operand):
            offending.append(where)
        elif operand.startswith("*%") and wide(operand[1:]) in loaded:
            offending.append(f"{where}, through {operand[1:]}, which holds "
                             f"{loaded[wide(operand[1:])]}")

    missing = []
    for function in functions:
        prefix = f"_Z{len(function)}{function}"
        if not any(label.startswith(prefix) for label in labels):
            missing.append(function)
    return offending, missing, x86_64 or only_returns


def run(command, functions, context=""):
    """Runs one compile and prints its findings; True if it passes."""
    result = subprocess.run(command, capture_output=True, text=True,
                            check=False)
    if result.returncode != 0:
        print(f"{context}the command failed with exit status "
              f"{result.returncode}:")
        print(result.stderr, end="")
        return False

    offending, missing, x86_64 = check(result.stdout, functions)
    if not x86_64:
        print(f"{context}the output is not x86-64 assembly: no instruction "
              "names a register of x86-64, so its calls cannot be read")
        return False
    for line in offending:
        print(f"{context}a call into the library: {line}")
    for function in missing:
        print(f"{context}not defined in the output: {function}")
    return not offending and not missing


def defines(paragraph, function):
    """Whether the paragraph defines the function: its first line that is
    not a comment declares the function's name followed by (."""
    for line in paragraph.splitlines():
        if not line.lstrip().startswith("//"):
            return re.search(rf"(?:^|\s){re.escape(function)}\(", line) \
                is not None
    return False


def alone(source, functions, command):
    """Compiles each function in a unit of its own; True if all pass."""
    text = pathlib.Path(source).read_text(encoding="utf-8")
    paragraphs = re.split(r"\n\s*\n", text)
    owners = {}
    for function in functions:
        found = [p for p in paragraphs if defines(p, function)]
        if len(found) != 1:
            print(f"the source defines no function {function}"
                  if not found else
                  f"the source defines {function} in {len(found)} paragraphs")
            return False
        owners[function] = found[0]
    common = [p for p in paragraphs if p not in owners.values()]

    passed = True
    with tempfile.TemporaryDirectory() as scratch:
        for function in functions:
            unit = pathlib.Path(scratch) / f"{function}.cpp"
            unit.write_text("\n\n".join([*common, owners[function]]) + "\n",
                            encoding="utf-8")
            passed &= run([*command, str(unit)], [function],
                          f"{function}, compiled alone: ")
    return passed


def main(argv):
    source = None
    if argv[:1] == ["--alone"]:
        if len(argv) < 2:
            sys.exit(__doc__)
        source, argv = argv[1], argv[2:]
    if "--" not in argv:
        sys.exit(__doc__)
    split = argv.index("--")
    functions, command = argv[:split], argv[split + 1:]
    if not functions or not command:
        sys.exit(__doc__)

    if source is None:
        passed = run(command, functions)
        summary = (f"{len(functions)} functions defined, none calls into the "
                   "library")
    else:
        passed = alone(source, functions, command)
        summary = (f"{len(functions)} functions compiled alone, none calls "
                   "into the library")
    if not passed:
        sys.exit(1)
    print(summary)


if __name__ == "__main__":
    main(sys.argv[1:])
