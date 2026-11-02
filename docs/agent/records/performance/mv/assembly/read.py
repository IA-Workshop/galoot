"""The first assembly read of the straight-line code that applies a plan.

Compiles kernels.cpp at -O2 with the test flags on GCC and Clang to Intel-
syntax assembly, keeps each text in <out>/<compiler>.s, and writes
<out>/summary.tsv: for each function, its instructions by kind, counting a
packed instruction once per lane of two doubles:
- mul: multiplications (mulsd, mulpd, and their v forms);
- addsub: additions and subtractions (addsd, subsd, addpd, subpd, and the v
  forms);
- negate: sign flips (xorpd, xorps), which take a constant mask;
- convert: conversions (cvt*), such as integer to floating;
- branch: jumps other than the final return, and call: calls;
- constant: references to a constant label (.LC*, .LCPI*), the only data a
  body could load besides its arguments;
- stores: stores to memory, and zero_stores: stores of zero, the trace a
  zero fill would leave;
- other: every other instruction but the return.

Usage, from the worktree root:
  python3 -I docs/agent/records/performance/mv/assembly/read.py <out>
"""

import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[5]
FLAGS = ["-std=c++23", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-S", "-masm=intel",
         "-fno-asynchronous-unwind-tables", "-I", str(ROOT / "include")]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
KINDS = ("mul", "addsub", "negate", "convert", "branch", "call", "constant",
         "stores", "zero_stores", "other")
PACKED = re.compile(r"^v?(mul|add|sub)pd$")
SCALAR = re.compile(r"^v?(mul|add|sub)sd$")


def functions(text):
    """Yields (mangled name, instruction lines) for each function."""
    name, body = None, []
    for line in text.splitlines():
        label = re.match(r'^"?([A-Za-z_][\w.$]*)"?:', line)
        if label and not label.group(1).startswith("."):
            if name:
                yield name, body
            name, body = label.group(1), []
        elif name and line.startswith("\t") and not line.startswith("\t."):
            body.append(line.strip())
        elif name and line.startswith("\t.size"):
            yield name, body
            name, body = None, []
    if name:
        yield name, body


def count(body):
    counts = dict.fromkeys(KINDS, 0)
    for line in body:
        op = line.split()[0]
        args = line[len(op):]
        if op in ("ret", "endbr64"):
            continue
        if re.search(r"\.LC(PI)?\w*", args):
            counts["constant"] += 1
        if SCALAR.match(op) or PACKED.match(op):
            lanes = 2 if PACKED.match(op) else 1
            kind = "mul" if "mul" in op else "addsub"
            counts[kind] += lanes
        elif op in ("xorpd", "xorps", "vxorpd", "vxorps") and ".LC" in args:
            counts["negate"] += 1
        elif op.startswith(("cvt", "vcvt")):
            counts["convert"] += 1
        elif op.startswith("j"):
            counts["branch"] += 1
        elif op == "call":
            counts["call"] += 1
        else:
            counts["other"] += 1
        destination = args.split(",")[0].lower()
        if op.startswith(("mov", "vmov")) and "ptr" in destination and \
                "[" in destination:
            counts["stores"] += 1
            source = args.split(",")[-1].strip()
            if source == "0":
                counts["zero_stores"] += 1
    return counts


def main():
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    rows = ["compiler\tfunction\t" + "\t".join(KINDS)]
    for compiler, cxx in COMPILERS.items():
        text = subprocess.run([cxx, *FLAGS, "-o", "-", str(HERE / "kernels.cpp")],
                              capture_output=True, text=True, check=True).stdout
        (out / f"{compiler}.s").write_text(text)
        for mangled, body in functions(text):
            name = subprocess.run(["c++filt", mangled], capture_output=True,
                                  text=True, check=True).stdout.strip()
            short = name.split("(")[0]
            if "6detail11run_product" in mangled or "6detail7run_map" in mangled:
                # A kernel left out of line, which c++filt 2.47 cannot
                # demangle: its name without arguments.
                short = ("detail::run_product" if "run_product" in mangled
                         else "detail::run_map")
            elif not short.startswith(("library_", "hand_", "written_")):
                continue
            counts = count(body)
            rows.append(f"{compiler}\t{short}\t" +
                        "\t".join(str(counts[k]) for k in KINDS))
    (out / "summary.tsv").write_text("\n".join(rows) + "\n")


if __name__ == "__main__":
    main()
