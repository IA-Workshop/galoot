"""Count the arithmetic of each function of probe.cpp in its assembly.

    python3 -I count_asm.py probe.gcc.s probe.clang.s > out/asm-counts.md

A function is counted together with every function of the same file that it
reaches by a call or a jump to a symbol, so that code a compiler kept out of
line is counted with the function that runs it. Lanes: a scalar double
instruction is one operation, a packed one on xmm two. mul: mulsd, mulpd;
add: addsd, subsd, addpd, subpd (an addition of a value to itself, the
doubling, is among them); neg: xorpd/xorps of two different registers;
instr: every instruction; calls: call instructions and jumps to a symbol.
"""

import re
import subprocess
import sys

LABEL = re.compile(r"^([A-Za-z_.$][\w.$@]*):")
INSTRUCTION = re.compile(r"^\s+([a-z][a-z0-9]*)\b\s*(.*?)\s*(?:#.*)?$")


def functions(path):
    """{label: [(mnemonic, operands)]} for every label of the file."""
    out, current = {}, None
    with open(path) as f:
        for line in f:
            m = LABEL.match(line)
            if m and not m.group(1).startswith(".L"):
                current = m.group(1)
                out.setdefault(current, [])
                continue
            m = INSTRUCTION.match(line)
            if m and current and not m.group(1).startswith("."):
                out[current].append((m.group(1), m.group(2)))
    return out


def demangle(names):
    r = subprocess.run(["c++filt"], input="\n".join(names), text=True,
                       capture_output=True, check=True)
    return dict(zip(names, r.stdout.splitlines()))


def reached(code, start):
    seen, stack = [], [start]
    while stack:
        name = stack.pop()
        if name in seen or name not in code:
            continue
        seen.append(name)
        for mnemonic, operands in code[name]:
            if mnemonic in ("call", "callq", "jmp", "jmpq") and operands in code \
                    and not operands.startswith(".L"):
                stack.append(operands)
    return seen


def count(code, names):
    c = {"mul": 0, "add": 0, "neg": 0, "instr": 0, "calls": 0}
    for name in names:
        for mnemonic, operands in code[name]:
            c["instr"] += 1
            two = mnemonic.endswith("pd")
            if mnemonic in ("mulsd", "mulpd"):
                c["mul"] += 2 if two else 1
            elif mnemonic in ("addsd", "subsd", "addpd", "subpd"):
                c["add"] += 2 if two else 1
            elif mnemonic in ("xorpd", "xorps"):
                regs = [r.strip() for r in operands.split(",")]
                if len(set(regs)) > 1:
                    c["neg"] += 1
            elif mnemonic in ("call", "callq") or (
                    mnemonic in ("jmp", "jmpq") and not operands.startswith(".L")):
                c["calls"] += 1
    return c


def main(paths):
    rows = {}
    order = []
    for path in paths:
        compiler = "gcc" if ".gcc." in path else "clang"
        code = functions(path)
        names = demangle(list(code))
        for label, text in names.items():
            m = re.match(r"^(?:std::array<double, \d+ul?> )?"
                         r"((?:point|line|plane)_[a-z]+(?:_setup|_apply)?)\(",
                         text)
            if not m:
                continue
            # A function the compiler also emitted as a clone keeps the
            # first label found.
            key = m.group(1)
            if (key, compiler) in rows:
                continue
            rows[key, compiler] = count(code, reached(code, label))
            if key not in order:
                order.append(key)
    print("| function | GCC mul | add | neg | instr | calls "
          "| Clang mul | add | neg | instr | calls |")
    print("| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: "
          "| ---: | ---: |")
    for key in order:
        cells = [key]
        for compiler in ("gcc", "clang"):
            c = rows.get((key, compiler))
            cells += ([str(c[k]) for k in ("mul", "add", "neg", "instr",
                                           "calls")]
                      if c else ["-"] * 5)
        print("| " + " | ".join(cells) + " |")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
