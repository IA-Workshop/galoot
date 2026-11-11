"""Checks that a unit's operations compile without a call into the library.

Usage:
  python3 -I kernels_inline.py <function>... -- <command>...

Runs the command, which writes x86-64 assembly to its standard output (a
compile with -S -o -), and checks two things:

- every named function is defined in the output: some label is its mangled
  name, _Z, the length of the name, then the name, as the compilers mangle a
  function at namespace scope, so that a function the compiler dropped or
  renamed cannot pass by having no code to check;
- no call or jump, in any of its forms (call, callq, jmp, jmpq, and a tail
  call written as jmp), has a symbol of namespace galoot as its target,
  whether named directly, through the PLT (sym@PLT), or through the global
  offset table (*sym@GOTPCREL(%rip)). Such a symbol's mangled name starts
  with _ZN, then any of the qualifiers K, V, r, R and O that a member
  function's cv- and ref-qualification adds, then 6galoot; or, for a lambda
  or another entity local to a function of the library, with _ZZN and the
  same. A call to any other symbol, such as one of the standard library, and
  a jump to a local label (.L...) are allowed.

Prints each offending line with the function it is in, and each function
that is missing. Exits 1 if there is any, or if the command fails; 0
otherwise.
"""

import re
import subprocess
import sys

# A label at the start of a line: a symbol, then a colon, then nothing but a
# comment.
LABEL = re.compile(r"^([A-Za-z_.$][\w.$@]*):\s*(?:#.*)?$")
# An instruction that transfers control, with its operand.
BRANCH = re.compile(r"^\s+(call|callq|jmp|jmpq)\s+(\S+)")
# The mangled name of a symbol in namespace galoot, or of an entity local to
# one of its functions, anywhere in an operand: after a * for an indirect
# call, and before a suffix such as @PLT.
LIBRARY = re.compile(r"(?<![\w.$])_ZZ?N[KVrRO]*6galoot")


def check(assembly, functions):
    """The offending lines and the missing functions of `assembly`."""
    labels = set()
    offending = []
    current = "(no function)"
    for number, line in enumerate(assembly.splitlines(), start=1):
        label = LABEL.match(line)
        if label:
            labels.add(label.group(1))
            if not label.group(1).startswith(".L"):
                current = label.group(1)
            continue
        branch = BRANCH.match(line)
        if branch and LIBRARY.search(branch.group(2)):
            offending.append(f"line {number}, in {current}: {line.strip()}")

    missing = []
    for function in functions:
        prefix = f"_Z{len(function)}{function}"
        if not any(label.startswith(prefix) for label in labels):
            missing.append(function)
    return offending, missing


def main(argv):
    if "--" not in argv:
        sys.exit(__doc__)
    split = argv.index("--")
    functions, command = argv[:split], argv[split + 1:]
    if not functions or not command:
        sys.exit(__doc__)

    result = subprocess.run(command, capture_output=True, text=True,
                            check=False)
    if result.returncode != 0:
        print(f"the command failed with exit status {result.returncode}:")
        print(result.stderr, end="")
        sys.exit(1)

    offending, missing = check(result.stdout, functions)
    for line in offending:
        print(f"a call into the library: {line}")
    for function in missing:
        print(f"not defined in the output: {function}")
    if offending or missing:
        sys.exit(1)
    print(f"{len(functions)} functions defined, none calls into the library")


if __name__ == "__main__":
    main(sys.argv[1:])
