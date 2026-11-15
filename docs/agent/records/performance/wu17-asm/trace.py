"""Executes each operation of probe.<compiler>.s symbolically, lane by lane,
and checks what it computes against the straight-line reference kernels.

    python3 -I trace.py <record directory> <reference_kernels.hpp> [file.s...]

With files named, it traces those instead of probe.gcc.s and probe.clang.s
(for a probe compiled with other options).

Every double the code loads, computes or stores is a node of an expression
graph: an operand's coefficient, a constant from the file's data, or a
product, sum, difference or negation of nodes. The interpreter knows the
SSE2 and general-register instructions these functions use and stops on
any other, so nothing is skipped silently. It follows calls and jumps to
functions of the same file, and takes no stack-protector failure branch (a
branch to a block that calls __stack_chk_fail); any other conditional
branch stops it, since the operations are meant to have none.

For each operation it reports, per arithmetic kind, the lanes the code
computes (a packed instruction has two), the lanes whose node reaches a
stored result (live), and among the live ones the padding lanes: a product
of two constants, or a sum or difference with a constant zero, whose value
the result does not depend on. It then expands each stored result into a
polynomial in the operands' coefficients and compares it with the reference
kernel's, also expanded, term by term with exact rational coefficients.
"""

import fractions
import pathlib
import re
import struct
import sys

OPERATIONS = {
    "wedge_point_point": 6,
    "wedge_point_point_point": 4,
    "antiwedge_line_plane": 4,
    "transform_point": 4,
    "transform_line": 6,
    "transform_plane": 4,
    "compose_motors": 8,
}
SIGN = 0x8000000000000000


class Graph:
    def __init__(self):
        self.key = {}
        self.nodes = []

    def node(self, *key):
        if key[0] in ("mul", "add"):
            key = (key[0], *sorted(key[1:]))
        if key not in self.key:
            self.key[key] = len(self.nodes)
            self.nodes.append(key)
        return self.key[key]

    def const(self, bits):
        return self.node("c", bits)

    def constant(self, index):
        """The value of a node whose leaves are all constants, else None."""
        kind, *args = self.nodes[index]
        if kind == "c":
            return struct.unpack("<d", struct.pack("<Q", args[0]))[0]
        if kind == "in":
            return None
        values = [self.constant(a) for a in args]
        if any(v is None for v in values):
            return None
        if kind == "neg":
            return -values[0]
        return {"mul": values[0] * values[1], "add": values[0] + values[1],
                "sub": values[0] - values[1]}[kind]


def parse(path):
    """Functions (name -> list of (mnemonic, operands) and label entries)
    and constant data (label -> list of 8-byte words)."""
    lines = path.read_text().splitlines()
    names = {m.group(1) for line in lines
             if (m := re.match(r"^\s+\.type\s+([^,]+),\s*@function", line))}
    functions, data, aliases = {}, {}, {}
    current, label = None, None
    raw = []
    for line in lines:
        line = line.split("#")[0].rstrip()
        m = re.match(r"^([A-Za-z_.$][\w.$@]*):", line)
        if m:
            name = m.group(1)
            if name in names:
                current = name
                functions[current] = []
                label = None
            elif current is not None and name.startswith(".L"):
                functions[current].append((":", name))
            else:
                label, raw = name, []
                data[label] = raw
            continue
        m = re.match(r"^\s+(\.?[a-z][a-z0-9]*)\s*(.*)$", line)
        if not m:
            continue
        mnemonic, operands = m.group(1), m.group(2).strip()
        if current is not None and re.match(
                r"\.size\s+" + re.escape(current) + ",", mnemonic + " "
                + operands):
            current = None
            continue
        if mnemonic == ".set":
            alias, target = [part.strip() for part in operands.split(",")]
            aliases[alias] = target
            continue
        if mnemonic == ".section" or mnemonic == ".text":
            current = None
        if current is not None and not mnemonic.startswith("."):
            functions[current].append((mnemonic, operands))
        elif current is None and label is not None:
            if mnemonic == ".quad":
                raw.extend(int(operands, 0).to_bytes(8, "little", signed=int(
                    operands, 0) < 0))
            elif mnemonic == ".long":
                raw.extend(int(operands, 0).to_bytes(4, "little", signed=int(
                    operands, 0) < 0))
            elif mnemonic == ".zero":
                raw.extend(bytes(int(operands, 0)))
            elif mnemonic == ".double":
                raw.extend(struct.pack("<d", float(operands)))
    words = {k: [int.from_bytes(bytes(v[i:i + 8]), "little")
                 for i in range(0, len(v) - len(v) % 8, 8)]
             for k, v in data.items() if v}
    for alias, target in aliases.items():
        words[alias] = words[target]
    return functions, words


def split(operands):
    parts, depth, cur = [], 0, ""
    for ch in operands:
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur.strip())
    return parts


class Machine:
    def __init__(self, functions, words, graph):
        self.functions, self.words, self.g = functions, words, graph
        self.zero = graph.const(0)
        self.xmm = {f"%xmm{i}": [self.zero, self.zero] for i in range(16)}
        self.gpr = {}
        self.memory = {}
        self.records = []  # (kind, lane node, mnemonic)

    # Addresses: a pointer is (base, offset).
    def address(self, operand):
        m = re.match(r"^([-\w.$]*)\((%\w+)\)$", operand)
        if not m:
            raise ValueError(f"address {operand}")
        offset, base = m.group(1), m.group(2)
        if base == "%rip":
            return ("data", offset), 0
        pointer = self.gpr[base]
        value = int(offset, 0) if offset else 0
        return pointer[0], pointer[1] + value

    def load(self, operand, width):
        base, offset = self.address(operand)
        if base[0] == "data":
            ws = self.words[base[1]]
            return [self.g.const(ws[i]) for i in range(width)]
        out = []
        for i in range(width):
            key = (base, offset + 8 * i)
            if key not in self.memory:
                if base in self.inputs:
                    self.memory[key] = self.g.node(
                        "in", self.inputs[base], (offset + 8 * i) // 8)
                else:
                    raise ValueError(f"read of unwritten {key}")
            out.append(self.memory[key])
        return out

    def store(self, operand, lanes):
        base, offset = self.address(operand)
        for i, lane in enumerate(lanes):
            self.memory[(base, offset + 8 * i)] = lane

    def source(self, operand, width=2):
        if operand.startswith("%xmm"):
            return list(self.xmm[operand])
        return self.load(operand, width)

    def arith(self, kind, mnemonic, a, b):
        node = self.g.node(kind, a, b)
        self.records.append((kind, node, mnemonic))
        return node

    def negate(self, mnemonic, lane, mask):
        kind, *args = self.g.nodes[mask]
        if kind != "c" or args[0] not in (0, SIGN):
            raise ValueError(
                f"xor with a non-sign constant {self.g.nodes[mask]}")
        if args[0] == 0:
            return lane
        node = self.g.node("neg", lane)
        self.records.append(("neg", node, mnemonic))
        return node

    def run(self, name):
        body = self.functions[name]
        labels = {op: i for i, (m, op) in enumerate(body) if m == ":"}
        pc = 0
        while True:
            mnemonic, operands = body[pc]
            pc += 1
            if mnemonic == ":":
                continue
            ops = split(operands)
            if mnemonic in ("ret", "retq"):
                self.gpr["%rsp"] = (self.gpr["%rsp"][0],
                                    self.gpr["%rsp"][1] + 8)
                return
            if mnemonic in ("jmp", "jmpq"):
                if ops[0] in labels:
                    pc = labels[ops[0]]
                    continue
                self.call(ops[0])
                self.gpr["%rsp"] = (self.gpr["%rsp"][0],
                                    self.gpr["%rsp"][1] + 8)
                return
            if mnemonic in ("call", "callq"):
                if ops[0].startswith("__stack_chk_fail"):
                    raise ValueError("reached the stack protector's failure")
                self.call(ops[0])
                continue
            if re.match(r"^j(?!mp)[a-z]+$", mnemonic):
                target = labels[ops[0]]
                following = [m for m in body[target:] if m[0] in (
                    "call", "callq", "ret", "retq", "jmp", "jmpq")]
                guard = following and following[0][1].startswith(
                    "__stack_chk_fail")
                if guard:
                    continue
                raise ValueError(f"conditional branch {mnemonic} {operands}")
            self.step(mnemonic, ops)

    def call(self, target):
        if target not in self.functions:
            raise ValueError(f"call to {target}")
        self.gpr["%rsp"] = (self.gpr["%rsp"][0], self.gpr["%rsp"][1] - 8)
        self.run(target)

    def step(self, m, ops):
        g, x = self.g, self.xmm
        if m in ("pushq", "push"):
            self.gpr["%rsp"] = (self.gpr["%rsp"][0], self.gpr["%rsp"][1] - 8)
            self.memory[self.gpr["%rsp"]] = self.gpr.get(ops[0], "opaque")
            return
        if m in ("popq", "pop"):
            self.gpr[ops[0]] = self.memory[self.gpr["%rsp"]]
            self.gpr["%rsp"] = (self.gpr["%rsp"][0], self.gpr["%rsp"][1] + 8)
            return
        if m in ("subq", "addq") and ops[1] == "%rsp":
            n = int(ops[0].lstrip("$"), 0)
            delta = -n if m == "subq" else n
            self.gpr["%rsp"] = (self.gpr["%rsp"][0],
                                self.gpr["%rsp"][1] + delta)
            return
        if m in ("leaq",):
            self.gpr[ops[1]] = self.address(ops[0])
            return
        if m in ("cmpq", "cmpl", "testq", "testl", "nop", "endbr64"):
            return
        if m == "movabsq":
            self.gpr[ops[1]] = ("int", int(ops[0].lstrip("$"), 0) % 2**64)
            return
        vector = any(o.startswith("%xmm") for o in ops)
        if m in ("movq", "movd") and not vector:
            src, dst = ops
            if src.startswith("%fs:"):
                value = "opaque"
            elif src.startswith("%"):
                value = self.gpr.get(src, "opaque")
            elif src.startswith("$"):
                value = ("int", int(src.lstrip("$"), 0) % 2**64)
            else:
                base, off = self.address(src)
                known = (base, off) in self.memory or base in self.inputs
                value = self.load(src, 1)[0] if known else "opaque"
            if dst.startswith("%"):
                self.gpr[dst] = value
            else:
                base, off = self.address(dst)
                self.memory[(base, off)] = value
            return
        if m == "xorq":
            src, dst = ops
            a = self.gpr[src] if src.startswith("%") else self.load(src, 1)[0]
            b = self.gpr[dst]
            mask, value = (a, b) if a == ("int", SIGN) else (b, a)
            if mask != ("int", SIGN) or not isinstance(value, int):
                raise ValueError(f"xorq {ops}")
            node = g.node("neg", value)
            self.records.append(("neg", node, m))
            self.gpr[dst] = node
            return
        if m in ("movq", "movd"):
            src, dst = ops
            if dst.startswith("%xmm"):
                if src.startswith("%xmm"):
                    lane = x[src][0]
                elif src.startswith("%"):
                    lane = self.gpr[src]
                else:
                    lane = self.load(src, 1)[0]
                x[dst] = [lane, self.zero]
            elif dst.startswith("%"):
                self.gpr[dst] = x[src][0]
            else:
                self.store(dst, [x[src][0]])
            return
        if m == "movsd":
            src, dst = ops
            if dst.startswith("%xmm"):
                if src.startswith("%xmm"):
                    x[dst] = [x[src][0], x[dst][1]]
                else:
                    x[dst] = [self.load(src, 1)[0], self.zero]
            else:
                self.store(dst, [x[src][0]])
            return
        if m in ("movapd", "movaps", "movupd", "movups", "movdqa", "movdqu"):
            src, dst = ops
            if dst.startswith("%xmm"):
                x[dst] = self.source(src)
            else:
                self.store(dst, x[src])
            return
        if m in ("movhpd", "movhps"):
            src, dst = ops
            if dst.startswith("%xmm"):
                x[dst] = [x[dst][0], self.load(src, 1)[0]]
            else:
                self.store(dst, [x[src][1]])
            return
        if m in ("movlpd", "movlps"):
            src, dst = ops
            if dst.startswith("%xmm"):
                x[dst] = [self.load(src, 1)[0], x[dst][1]]
            else:
                self.store(dst, [x[src][0]])
            return
        if m in ("unpcklpd", "movlhps", "punpcklqdq"):
            s = self.source(ops[0])
            x[ops[1]] = [x[ops[1]][0], s[0]]
            return
        if m in ("unpckhpd", "punpckhqdq"):
            s = self.source(ops[0])
            x[ops[1]] = [x[ops[1]][1], s[1]]
            return
        if m == "movhlps":
            s = self.source(ops[0])
            x[ops[1]] = [s[1], x[ops[1]][1]]
            return
        if m == "movddup":
            s = self.source(ops[0], 1)
            x[ops[1]] = [s[0], s[0]]
            return
        if m in ("shufpd", "shufps", "pshufd"):
            imm = int(ops[0].lstrip("$"), 0)
            s = self.source(ops[1])
            d = s if m == "pshufd" else x[ops[2]]
            if m == "shufpd":
                x[ops[2]] = [d[imm & 1], s[(imm >> 1) & 1]]
                return
            dwords = [(imm >> (2 * i)) & 3 for i in range(4)]
            lanes = []
            for half, source in ((0, d), (1, s)):
                lo, hi = dwords[2 * half], dwords[2 * half + 1]
                if lo % 2 != 0 or hi != lo + 1:
                    raise ValueError(f"{m} {ops} splits a double")
                lanes.append(source[lo // 2])
            x[ops[2]] = lanes
            return
        if m in ("xorpd", "xorps", "pxor"):
            src, dst = ops
            if src == dst:
                x[dst] = [self.zero, self.zero]
                return
            s = self.source(src)
            d = x[dst]
            out = []
            for lane in range(2):
                a, b = s[lane], d[lane]
                if g.nodes[a][0] == "c":
                    out.append(self.negate(m, b, a))
                elif g.nodes[b][0] == "c":
                    out.append(self.negate(m, a, b))
                else:
                    raise ValueError(f"xor of two values {ops}")
            x[dst] = out
            return
        arith = {"mul": "mul", "add": "add", "sub": "sub"}
        mm = re.match(r"^(mul|add|sub)(sd|pd)$", m)
        if mm:
            kind = arith[mm.group(1)]
            src, dst = ops
            lanes = 1 if mm.group(2) == "sd" else 2
            s = self.source(src, lanes)
            d = list(x[dst])
            for lane in range(lanes):
                d[lane] = self.arith(kind, m, d[lane], s[lane])
            x[dst] = d
            return
        raise ValueError(f"unknown instruction {m} {ops}")


def polynomial(graph, index, names, memo):
    if index in memo:
        return memo[index]
    kind, *args = graph.nodes[index]
    if kind == "c":
        value = fractions.Fraction(graph.constant(index))
        result = {(): value} if value else {}
    elif kind == "in":
        result = {(f"{names[args[0]]}[{args[1]}]",): fractions.Fraction(1)}
    elif kind == "neg":
        result = {k: -v for k, v in polynomial(graph, args[0], names,
                                               memo).items()}
    else:
        a = polynomial(graph, args[0], names, memo)
        b = polynomial(graph, args[1], names, memo)
        if kind == "mul":
            result = {}
            for ka, va in a.items():
                for kb, vb in b.items():
                    k = tuple(sorted(ka + kb))
                    result[k] = result.get(k, 0) + va * vb
        else:
            sign = 1 if kind == "add" else -1
            result = dict(a)
            for k, v in b.items():
                result[k] = result.get(k, 0) + sign * v
        result = {k: v for k, v in result.items() if v}
    memo[index] = result
    return result


class Poly:
    def __init__(self, terms):
        self.terms = {k: v for k, v in terms.items() if v}

    def __add__(self, other):
        out = dict(self.terms)
        for k, v in other.terms.items():
            out[k] = out.get(k, 0) + v
        return Poly(out)

    def __sub__(self, other):
        return self + (-other)

    def __neg__(self):
        return Poly({k: -v for k, v in self.terms.items()})

    def __mul__(self, other):
        out = {}
        for ka, va in self.terms.items():
            for kb, vb in other.terms.items():
                k = tuple(sorted(ka + kb))
                out[k] = out.get(k, 0) + va * vb
        return Poly(out)


def reference(path):
    """Each reference kernel's parameter names and expanded results."""
    text = path.read_text()
    kernels = {}
    pattern = re.compile(
        r"constexpr std::array<T, (\d+)> (\w+)\((.*?)\) \{(.*?)\n\}", re.S)
    for size, name, params, body in pattern.findall(text):
        names = re.findall(r"& (\w+)", params)
        sizes = [int(n) for n in re.findall(r"std::array<T, (\d+)>", params)]
        env = {n: [Poly({(f"{n}[{i}]",): fractions.Fraction(1)})
                   for i in range(s)] for n, s in zip(names, sizes)}
        for line in body.splitlines():
            line = line.split("//")[0].strip()
            m = re.match(r"const T (\w+) = (.*);$", line)
            if m:
                env[m.group(1)] = eval(m.group(2), {}, env)  # noqa: S307
        results = re.search(r"return \{(.*)\};", body, re.S).group(1)
        values = [eval(e.split("//")[0].strip().rstrip(","), {}, env)
                  for e in results.strip().splitlines() if e.strip()]
        assert len(values) == int(size)
        kernels[name] = (names, values)
    return kernels


def trace(path, kernels):
    functions, words = parse(path)
    print(f"### {path.name}\n")
    print("| operation | mul lanes: computed, live, padding | add/sub lanes: "
          "computed, live, padding | neg: computed, live | shared live "
          "products | results equal the reference | monomials per result |")
    print("| --- | --- | --- | --- | ---: | --- | --- |")
    notes = []
    for operation, size in OPERATIONS.items():
        root = next(n for n in functions
                    if f"{len(operation)}{operation}R" in n)
        graph = Graph()
        machine = Machine(functions, words, graph)
        names, expected = kernels[operation]
        bases = ["x", "y", "z"][:len(names)]
        machine.inputs = {b: i for i, b in enumerate(bases)}
        machine.gpr.update({"%rdi": ("out", 0), "%rsp": ("stack", 0)})
        for reg, b in zip(("%rsi", "%rdx", "%rcx"), bases):
            machine.gpr[reg] = (b, 0)
        machine.gpr["%rsp"] = ("stack", -8)
        machine.run(root)
        outputs = [machine.memory[("out", 8 * i)] for i in range(size)]
        live = set()
        stack = list(outputs)
        while stack:
            n = stack.pop()
            if n in live:
                continue
            live.add(n)
            kind, *args = graph.nodes[n]
            if kind not in ("c", "in"):
                stack.extend(args)
        summary = {}
        for kind, node, mnemonic in machine.records:
            s = summary.setdefault(kind, [0, 0, 0, []])
            s[0] += 1
            if node in live:
                s[1] += 1
                k, *args = graph.nodes[node]
                padded = graph.constant(node) is not None or (
                    k in ("add", "sub") and any(
                        graph.constant(a) == 0 for a in args))
                if padded:
                    s[2] += 1
                    places = [i for i, out in enumerate(outputs)
                              if reaches(graph, out, node)]
                    s[3].append(f"{describe(graph, node, names)}, in "
                                f"{mnemonic}, reaching result place(s) "
                                f"{places}")
        products = [n for kind, n, _ in machine.records
                    if kind == "mul" and n in live]
        shared = len(products) - len(set(products))
        memo = {}
        got = [polynomial(graph, n, names, memo) for n in outputs]
        equal = all(g == e.terms for g, e in zip(got, expected))
        mul = summary.get("mul", [0, 0, 0, []])
        addsub = [sum(summary.get(k, [0, 0, 0, []])[i] for k in ("add", "sub"))
                  for i in range(3)]
        neg = summary.get("neg", [0, 0, 0, []])
        print(f"| {operation} | {mul[0]}, {mul[1]}, {mul[2]} "
              f"| {addsub[0]}, {addsub[1]}, {addsub[2]} | {neg[0]}, {neg[1]} "
              f"| {shared} | {'yes' if equal else 'NO'} "
              f"| {[len(p) for p in got]} |")
        for kind in ("mul", "add", "sub"):
            for text in summary.get(kind, [0, 0, 0, []])[3]:
                notes.append(f"- {operation}: padding {kind} lane {text}")
        dead = {}
        for kind, node, mnemonic in machine.records:
            if node not in live:
                dead[mnemonic] = dead.get(mnemonic, 0) + 1
        if dead:
            listed = ", ".join(f"{n} {m}" for m, n in sorted(dead.items()))
            notes.append(f"- {operation}: lanes no stored result reads: "
                         f"{listed}")
    print()
    for note in notes:
        print(note)
    if notes:
        print()


def reaches(graph, root, target):
    """Whether node target is in the graph below node root."""
    seen, stack = set(), [root]
    while stack:
        n = stack.pop()
        if n == target:
            return True
        if n in seen:
            continue
        seen.add(n)
        kind, *args = graph.nodes[n]
        if kind not in ("c", "in"):
            stack.extend(args)
    return False


def describe(graph, node, names):
    kind, *args = graph.nodes[node]
    if kind == "c":
        return repr(graph.constant(node))
    if kind == "in":
        return f"{names[args[0]]}[{args[1]}]"
    if graph.constant(node) is not None and kind != "c":
        inner = ", ".join(describe(graph, a, names) for a in args)
        return f"{kind}({inner}) = {graph.constant(node)!r}"
    inner = ", ".join(
        describe(graph, a, names) if graph.constant(a) is not None
        or graph.nodes[a][0] == "in" else f"<{graph.nodes[a][0]} node>"
        for a in args)
    return f"{kind}({inner})"


def main():
    record = pathlib.Path(sys.argv[1])
    kernels = reference(pathlib.Path(sys.argv[2]))
    files = [pathlib.Path(f) for f in sys.argv[3:]] or [
        record / f"probe.{compiler}.s" for compiler in ("gcc", "clang")]
    for path in files:
        trace(path, kernels)


if __name__ == "__main__":
    main()
