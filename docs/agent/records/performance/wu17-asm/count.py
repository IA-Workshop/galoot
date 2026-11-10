"""Counts the arithmetic, branches and calls of each operation in the
assembly that run.sh writes.

    python3 -I count.py <record directory> <benchmark assembly directory>

It also counts each probe.clang.<variant>.s found in the second directory.

An operation is a function of probe.cpp together with every function of the
same file it reaches by a call or a jump to a symbol, so that a kernel the
compiler kept out of line is counted with the operation that runs it. A
packed instruction on two doubles (xmm, SSE2) counts as two scalar
operations, one on four (ymm) as four. The negation column counts xorpd and
xorps instructions that do not clear a register with itself (trace.py
counts negated lanes, including those done in general registers); a
compare is a ucomisd, comisd or cmp*sd/pd. A branch is any conditional
jump, counted apart when it goes to the stack protector's failure call (a
guard branch, which tests the stack canary, not blade data); a call is a
call or a jump to a symbol, with its target.

For each benchmark program the script lists the calls and symbol jumps
from the program's own functions (bench::, main and its lambdas, the
reference kernels) to each other, to the library's functions, and to
operator new and malloc, so a
kernel kept out of line in the timed loop shows there too.

The output is a Markdown table per assembly file, written to standard
output.
"""

import pathlib
import re
import subprocess
import sys

OPERATIONS = [
    "wedge_point_point",
    "wedge_point_point_point",
    "antiwedge_line_plane",
    "transform_point",
    "transform_line",
    "transform_plane",
    "compose_motors",
]

LABEL = re.compile(r"^([A-Za-z_.$][\w.$@]*):")
INSTRUCTION = re.compile(r"^\s+([a-z][a-z0-9]*)\b\s*(.*?)\s*(?:#.*)?$")
CONDITIONAL_JUMP = re.compile(r"^j(?!mp$)[a-z]+$")


def lanes(mnemonic, operands):
    """The number of doubles an arithmetic instruction works on."""
    if mnemonic.endswith(("sd", "ss")):
        return 1
    return 4 if "%ymm" in operands else 2


def functions(path):
    """Maps each function defined in an assembly file to its instructions."""
    names = set()
    text = path.read_text().splitlines()
    for line in text:
        match = re.match(r"^\s+\.type\s+([^,]+),\s*@function", line)
        if match:
            names.add(match.group(1))
    body = {}
    current = None
    for line in text:
        label = LABEL.match(line)
        if label:
            if label.group(1) in names:
                current = label.group(1)
                body[current] = []
            elif current is not None:
                body[current].append((":", label.group(1)))
            continue
        if current is None:
            continue
        if re.match(r"^\s+\.size\s+" + re.escape(current) + r",", line):
            current = None
            continue
        instruction = INSTRUCTION.match(line)
        if instruction and not instruction.group(1).startswith("."):
            body[current].append((instruction.group(1), instruction.group(2)))
    return body


def targets(instructions):
    """The symbols an instruction list calls or jumps to."""
    found = []
    for mnemonic, operands in instructions:
        if mnemonic in ("call", "callq", "jmp", "jmpq") and not (
            operands.startswith(".L")
        ):
            found.append((mnemonic, operands.split("@")[0].lstrip("*")))
    return found


def guarded(instructions, label):
    """Whether a jump to label goes to a block whose first call is the
    stack protector's failure handler."""
    for position, (mnemonic, operands) in enumerate(instructions):
        if mnemonic == ":" and operands == label:
            for later, rest in instructions[position + 1:]:
                if later in ("call", "callq"):
                    return rest.startswith("__stack_chk_fail")
                if later in ("ret", "retq", "jmp", "jmpq"):
                    return False
    return False


def counts(instructions):
    tally = {
        "instructions": 0,
        "mul": 0,
        "mul_scalar": 0,
        "mul_packed": 0,
        "add_sub": 0,
        "add_sub_scalar": 0,
        "add_sub_packed": 0,
        "neg": 0,
        "compare": 0,
        "branch": 0,
        "guard_branch": 0,
        "memory": 0,
    }
    for index, (mnemonic, operands) in enumerate(instructions):
        if mnemonic == ":":
            continue
        tally["instructions"] += 1
        bare = mnemonic[1:] if mnemonic.startswith("v") else mnemonic
        if bare in ("mulsd", "mulpd"):
            tally["mul"] += lanes(bare, operands)
            tally["mul_scalar" if bare == "mulsd" else "mul_packed"] += 1
        elif bare in ("addsd", "subsd", "addpd", "subpd"):
            tally["add_sub"] += lanes(bare, operands)
            scalar = bare.endswith("sd")
            tally["add_sub_scalar" if scalar else "add_sub_packed"] += 1
        elif bare in ("xorpd", "xorps"):
            parts = [part.strip() for part in operands.split(",")]
            if not (len(parts) == 2 and parts[0] == parts[1]):
                tally["neg"] += 1
        elif bare in ("ucomisd", "comisd") or bare.startswith("cmp") and (
            bare.endswith("sd") or bare.endswith("pd")
        ):
            tally["compare"] += 1
        elif CONDITIONAL_JUMP.match(mnemonic):
            guard = guarded(instructions, operands)
            tally["guard_branch" if guard else "branch"] += 1
        if "(" in operands:
            tally["memory"] += 1
    return tally


def demangle(names):
    result = subprocess.run(
        ["c++filt"], input="\n".join(names), capture_output=True, text=True,
        check=True,
    )
    return result.stdout.splitlines()


def short(name):
    """A demangled name cut to its function's qualified name, without its
    return type, template arguments, parameters or clone suffix."""
    text = demangle([name])[0].split(" [clone")[0]
    if text == name:
        return nested_name(name)
    depth = 0
    out = []
    for character in text:
        if character in "<(":
            if depth == 0 and character == "<":
                out.append("<>")
            depth += 1
        elif character in ">)":
            depth -= 1
        elif depth == 0:
            out.append(character)
    words = "".join(out).split()
    while len(words) > 1 and words[-1] == "const":
        words.pop()
    return with_kernel(words[-1], name)


def with_kernel(qualified, name):
    """A run_product's name with the kernel it runs."""
    for kernel in ("first_kernel", "product_kernel"):
        if f"{len(kernel)}{kernel}" in name and qualified.startswith(
                "galoot::core::detail::run_product"):
            return "galoot::core::detail::run_product<" + kernel + ">"
    return qualified


def nested_name(name):
    """The qualified name of a mangled name c++filt cannot read (its
    template arguments are class-type constants), from the length-prefixed
    identifiers after _ZN, with the kernel a run_product runs."""
    match = re.match(r"^_Z(N?)(.*)$", name)
    if not match:
        return name.split("@")[0]
    rest, parts = match.group(2), []
    while (m := re.match(r"^(\d+)", rest)):
        length = int(m.group(1))
        start = len(m.group(1))
        parts.append(rest[start:start + length])
        rest = rest[start + length:]
        if not match.group(1):
            break
    return with_kernel("::".join(parts), name)


def closure(body, root):
    seen = [root]
    stack = [root]
    while stack:
        for _, target in targets(body[stack.pop()]):
            if target in body and target not in seen:
                seen.append(target)
                stack.append(target)
    return seen


HEADER = (
    "| operation | instructions | mul | of which mulsd, mulpd | add/sub "
    "| of which scalar, packed | neg (xor) | compares | branches "
    "| guard branches | memory operands | calls and symbol jumps |\n"
    "| --- | ---: | ---: | --- | ---: | --- | ---: | ---: | ---: | ---: "
    "| ---: | --- |"
)


def row(label, tally, calls):
    return (
        f"| {label} | {tally['instructions']} | {tally['mul']} "
        f"| {tally['mul_scalar']}, {tally['mul_packed']} | {tally['add_sub']} "
        f"| {tally['add_sub_scalar']}, {tally['add_sub_packed']} "
        f"| {tally['neg']} | {tally['compare']} | {tally['branch']} "
        f"| {tally['guard_branch']} | {tally['memory']} | {calls} |"
    )


def probe_table(path):
    body = functions(path)
    print(f"### {path.name}\n")
    print(HEADER)
    for operation in OPERATIONS:
        root = next(name for name in body if f"{len(operation)}{operation}R"
                    in name)
        reached = closure(body, root)
        instructions = [i for name in reached for i in body[name]]
        calls = [
            f"{short(name)} {mnemonic} {short(target)}"
            for name in reached
            for mnemonic, target in targets(body[name])
        ]
        print(row(operation, counts(instructions), "; ".join(calls) or "none"))
        if len(reached) > 1:
            for name in reached:
                print(row(f"(part) {short(name)}", counts(body[name]), ""))
    print()


def bench_table(path):
    """The calls and symbol jumps of a benchmark program between its own
    functions (bench::, main and its lambdas) and the library, and into
    operator new and malloc."""
    body = functions(path)
    rows = set()

    def ours(callee):
        return callee.startswith(
            ("galoot::", "bench::", "main", "reference_kernels::"))

    for name, instructions in body.items():
        caller = short(name)
        if not ours(caller):
            continue
        for mnemonic, target in targets(instructions):
            callee = short(target)
            if target.startswith("_Znw"):
                callee = "operator new"
            elif callee == "malloc":
                pass
            elif not ours(callee):
                continue
            rows.add((caller, mnemonic.rstrip("q"), callee))
    print(f"- `{path.name}`: " + (
        "; ".join(f"{a} {b} {c}" for a, b, c in sorted(rows)) or "none"))
    for name, instructions in body.items():
        callee = short(name)
        if callee.startswith(("galoot::", "reference_kernels::")):
            reached = closure(body, name)
            tally = counts([i for n in reached for i in body[n]])
            print(f"  - out of line: {callee}, {tally['instructions']} "
                  f"instructions with what it calls, {tally['mul']} mul "
                  f"({tally['mul_scalar']} mulsd, {tally['mul_packed']} "
                  f"mulpd), {tally['add_sub']} add/sub, "
                  f"{tally['branch']} branches, "
                  f"{tally['guard_branch']} guard branches")


def main():
    record = pathlib.Path(sys.argv[1])
    scratch = pathlib.Path(sys.argv[2])
    for compiler in ("gcc", "clang"):
        path = record / f"probe.{compiler}.s"
        if path.exists():
            probe_table(path)
    for path in sorted(scratch.glob("probe.clang.*.s")):
        probe_table(path)
    print("### Benchmark programs\n")
    for compiler in ("gcc", "clang"):
        for operation in OPERATIONS:
            path = scratch / f"{operation}_bench.{compiler}.s"
            if path.exists():
                bench_table(path)
    print()


if __name__ == "__main__":
    main()
