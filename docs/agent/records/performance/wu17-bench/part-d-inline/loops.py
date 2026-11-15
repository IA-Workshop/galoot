"""Counts, in each loop of each function of a benchmark program's assembly,
the scalar and packed multiplications of doubles and the calls.

    python3 -I loops.py <file.s>...

A loop is the run of instructions from a label to a jump back to it in the
same function; loops are listed innermost first, by the line of their
label, and only those in the program's own functions (bench::... and
main) with a multiplication or a call. A packed mulpd on xmm registers
multiplies two doubles, on ymm four.
"""

import re
import subprocess
import sys

LABEL = re.compile(r"^([A-Za-z_.$][\w.$@]*):")
INSTRUCTION = re.compile(r"^\s+([a-z][a-z0-9]*)\b\s*([^#]*)")


def demangle(names):
    run = subprocess.run(["c++filt"], input="\n".join(names),
                         capture_output=True, text=True, check=True)
    return run.stdout.splitlines()


def short(name):
    """A demangled name without its template arguments and parameters."""
    while True:
        shorter = re.sub(r"<[^<>]*>", "", name)
        if shorter == name:
            break
        name = shorter
    return name.split("(")[0].split(" ")[-1] + ("(lambda)" if "lambda" in
                                                  name else "")


def loops_of(path):
    lines = open(path, encoding="utf-8").read().splitlines()
    functions = {m.group(1) for line in lines
                 if (m := re.match(r"\s+\.type\s+([^,]+),\s*@function",
                                   line))}
    found = []
    current, body = None, []
    for line in lines + [""]:
        label = LABEL.match(line)
        if label and label.group(1) in functions:
            # A new function, or the cold part of one (GCC's name.cold),
            # which may begin before the hot part's .size: end the one
            # being read.
            if current is not None:
                found.extend(scan(current, body))
            current, body = label.group(1), []
            continue
        if current is None:
            continue
        if re.match(r"\s+\.size\s+" + re.escape(current) + ",", line):
            found.extend(scan(current, body))
            current = None
            continue
        if label:
            body.append((":", label.group(1)))
        elif (m := INSTRUCTION.match(line)) and not m.group(1).startswith("."):
            body.append((m.group(1), m.group(2).strip()))
    return found


def scan(function, body):
    where = {operand: i for i, (op, operand) in enumerate(body) if op == ":"}
    out = []
    for end, (op, operand) in enumerate(body):
        if op.startswith("j") and operand in where and where[operand] < end:
            part = body[where[operand]:end]
            mulsd = sum(1 for o, _ in part if o in ("mulsd", "vmulsd"))
            mulpd = sum(1 for o, a in part if o in ("mulpd", "vmulpd"))
            calls = [a.split("@")[0] for o, a in part
                     if o in ("call", "callq")]
            if mulsd or mulpd or calls:
                out.append((function, where[operand], end - where[operand],
                            mulsd, mulpd, calls))
    return out


for path in sys.argv[1:]:
    found = loops_of(path)
    names = demangle([f for f, *_ in found] +
                     [c for *_, calls in found for c in calls])
    table = dict(zip([f for f, *_ in found] +
                     [c for *_, calls in found for c in calls], names))
    print(f"{path.split('/')[-1]}:")
    for function, start, size, mulsd, mulpd, calls in sorted(
            found, key=lambda x: (x[0], x[2])):
        name = short(table[function])
        if not name.startswith(("bench::", "main")):
            continue
        called = sorted({short(table[c]) for c in calls})
        print(f"  {name} loop at {start}, {size} "
              f"instructions: mulsd {mulsd}, mulpd {mulpd}, calls "
              f"{', '.join(called) or 'none'}")
