"""Straight-line forms as shared-node programs, and two counters.

A form is written with the operators of `S` (a signed reference to a node of
a shared program). The program records only multiplications, additions and
subtractions of two nodes, doublings, and variables; a sign is carried with
the reference at no cost, as the counting rule (COUNTING-RULE.md, rule S)
says. `count_dag` tallies a program from its nodes; `count_by_execution`
generates Python text from the same nodes and runs it on a counting number
that tallies every operator it meets. The two share the nodes and nothing
else; their agreement checks the tally of one against the execution of the
other.
"""

import re
from fractions import Fraction

import sympy as sp


class Dag:
    def __init__(self):
        self.nodes = []      # tuples: ('var', name) ('mul', i, j) ...
        self.index = {}
        self.vars = {}       # name -> sympy symbol

    def _node(self, key):
        if key not in self.index:
            self.index[key] = len(self.nodes)
            self.nodes.append(key)
        return self.index[key]

    def var(self, name):
        self.vars[name] = sp.Symbol(name)
        return S(self, self._node(("var", name)), 1, self.vars[name])

    def const_free(self, expr):
        """A sympy-only value with no node (a literal zero)."""
        return S(self, None, 1, expr)


class S:
    """sign * (value of node), with the sympy value of the whole."""

    def __init__(self, dag, node, sign, expr):
        self.dag, self.node, self.sign, self.expr = dag, node, sign, expr

    def __neg__(self):
        return S(self.dag, self.node, -self.sign, -self.expr)

    def __mul__(self, o):
        if isinstance(o, int):  # only +-1 is free
            assert o in (1, -1)
            return self if o == 1 else -self
        a, b = sorted((self.node, o.node))
        return S(self.dag, self.dag._node(("mul", a, b)), self.sign * o.sign,
                 self.expr * o.expr)

    __rmul__ = __mul__

    def __add__(self, o):
        d = self.dag
        if self.sign == o.sign:
            a, b = sorted((self.node, o.node))
            return S(d, d._node(("add", a, b)), self.sign, self.expr + o.expr)
        if self.sign > 0:  # x - y
            return S(d, d._node(("sub", self.node, o.node)), 1,
                     self.expr + o.expr)
        return S(d, d._node(("sub", o.node, self.node)), 1,
                 self.expr + o.expr)  # y - x

    def __sub__(self, o):
        return self + (-o)

    def dbl(self):
        return S(self.dag, self.dag._node(("dbl", self.node)), self.sign,
                 2 * self.expr)


def lin(terms):
    """Left fold of signed terms; len(terms) - 1 additions or subtractions."""
    acc = terms[0]
    for t in terms[1:]:
        acc = acc + t
    return acc


def dot3(a, b):
    return lin([a[i] * b[i] for i in range(3)])


def cross(a, b):
    return [a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]]


# --- Counter 1: tally of the reachable nodes ---------------------------------

def reachable(dag, outs):
    seen, stack = set(), [o.node for o in outs]
    while stack:
        i = stack.pop()
        if i in seen:
            continue
        seen.add(i)
        for j in dag.nodes[i][1:]:
            if isinstance(j, int):
                stack.append(j)
    return seen


def depends(dag, inputs):
    """Nodes that read any variable whose name satisfies `inputs`."""
    dep = {}
    for i, n in enumerate(dag.nodes):
        if n[0] == "var":
            dep[i] = bool(inputs(n[1]))
        else:
            dep[i] = any(dep[j] for j in n[1:] if isinstance(j, int))
    return dep


def count_dag(dag, outs, only=None):
    """(mul, add, neg, dbl, fused) of the nodes reachable from `outs`,
    restricted to the nodes in `only` when given. add counts additions and
    subtractions; neg is one per output whose polarity is negative, except
    that a negative subtraction is swapped for free. fused is the number of
    additions that absorb a multiplication: a sum is the tree of additions
    and subtractions that only each other read, its leaves are the nodes the
    tree reads, p of them multiplications that nothing else reads, and the
    tree can be run as a chain that absorbs min(additions, p) of them."""
    seen = reachable(dag, outs)
    if only is not None:
        seen = {i for i in seen if i in only}
    uses = {i: 0 for i in seen}
    for i in seen:
        for j in dag.nodes[i][1:]:
            if isinstance(j, int) and j in uses:
                uses[j] += 1
    for o in outs:
        if o.node in uses:
            uses[o.node] += 1  # an output is a use
    kinds = {i: dag.nodes[i][0] for i in seen}
    mul = sum(1 for k in kinds.values() if k == "mul")
    add = sum(1 for k in kinds.values() if k in ("add", "sub"))
    dbl = sum(1 for k in kinds.values() if k == "dbl")

    def is_sum(i):
        return kinds[i] in ("add", "sub")

    fused = 0
    for r in seen:
        if not is_sum(r):
            continue
        # a root: read by something that is not a sum, or by more than one
        parents = [i for i in seen if r in dag.nodes[i][1:]]
        if len(parents) == 1 and is_sum(parents[0]) and uses[r] == 1:
            continue
        leaves, adds, stack = [], 0, [r]
        while stack:
            i = stack.pop()
            adds += 1
            for j in dag.nodes[i][1:]:
                if is_sum(j) and uses[j] == 1:
                    stack.append(j)
                else:
                    leaves.append(j)
        p = sum(1 for j in leaves if kinds[j] == "mul" and uses[j] == 1)
        fused += min(adds, p)
    neg = 0
    for o in outs:
        if o.node in seen and o.sign < 0:
            if dag.nodes[o.node][0] == "sub" and uses[o.node] == 1:
                continue
            neg += 1
    return mul, add, neg, dbl, fused


# --- Counter 2: generated text run on a counting number ----------------------

class Cn:
    """A number that counts the operators applied to it."""
    tally = {"mul": 0, "add": 0, "neg": 0, "dbl": 0}

    def __init__(self, v):
        self.v = Fraction(v) if not isinstance(v, sp.Basic) else v

    def __mul__(self, o):
        Cn.tally["mul"] += 1
        return Cn(self.v * o.v)

    def __add__(self, o):
        Cn.tally["add"] += 1
        return Cn(self.v + o.v)

    def __sub__(self, o):
        Cn.tally["add"] += 1
        return Cn(self.v - o.v)

    def __neg__(self):
        Cn.tally["neg"] += 1
        return Cn(-self.v)

    def dbl(self):
        Cn.tally["dbl"] += 1
        return Cn(2 * self.v)


def generate_text(dag, outs, only=None):
    """Python statements for the nodes reachable from `outs` (in node order,
    which is a topological order), then the list of outputs with a negation
    where an output's polarity is negative and no free swap applies."""
    seen = reachable(dag, outs)
    uses = {i: 0 for i in seen}
    for i in seen:
        for j in dag.nodes[i][1:]:
            if isinstance(j, int):
                uses[j] += 1
    for o in outs:
        uses[o.node] += 1
    lines = []
    for i in sorted(seen):
        n = dag.nodes[i]
        if only is not None and i not in only and n[0] != "var":
            # a value computed elsewhere: read it from the environment
            lines.append(f"# t{i} given")
            continue
        if n[0] == "var":
            lines.append(f"t{i} = env['{n[1]}']")
        elif n[0] == "mul":
            lines.append(f"t{i} = t{n[1]} * t{n[2]}")
        elif n[0] == "add":
            lines.append(f"t{i} = t{n[1]} + t{n[2]}")
        elif n[0] == "sub":
            lines.append(f"t{i} = t{n[1]} - t{n[2]}")
        elif n[0] == "dbl":
            lines.append(f"t{i} = t{n[1]}.dbl()")
    res = []
    for k, o in enumerate(outs):
        n = dag.nodes[o.node]
        if o.sign > 0:
            res.append(f"t{o.node}")
        elif n[0] == "sub" and uses[o.node] == 1:
            # swap the operands in the text of the node
            lines = [re.sub(rf"^t{o.node} = t(\d+) - t(\d+)$",
                            rf"t{o.node} = t\2 - t\1", ln) for ln in lines]
            res.append(f"t{o.node}")
        else:
            res.append(f"(-t{o.node})")
    return lines, res


def count_by_execution(dag, outs, env_values):
    """Run the generated text on counting numbers; returns (tally, values)."""
    lines, res = generate_text(dag, outs)
    src = "\n".join(lines) + "\nresult = [" + ", ".join(res) + "]\n"
    env = {k: Cn(v) for k, v in env_values.items()}
    ns = {"env": env}
    Cn.tally = {"mul": 0, "add": 0, "neg": 0, "dbl": 0}
    exec(src, ns)  # the text is built above from the program's own nodes
    return dict(Cn.tally), [r.v for r in ns["result"]], src
