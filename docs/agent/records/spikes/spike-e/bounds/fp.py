"""Shared pieces of the bounds scripts: the forms as programs, their counts
of roundings and magnitudes, and their evaluation in exact, double, and
float arithmetic.

A form is a program of `dag.Dag` nodes (forms.py, one directory up). Each
node is evaluated once; a sign is carried with a reference to a node, and a
negation is exact, so it costs no rounding.

Float arithmetic is emulated by rounding each double result to binary32.
For +, -, and * of two binary32 values the exact result fits in, or rounds
once to, binary64, and a binary64 result rounded to binary32 is the binary32
result correctly rounded, since 53 >= 2 * 24 + 2 (double rounding is
innocuous for these operations under that condition; a result due to
Figueroa, quoted from memory, and checked here against the library's float
results bit for bit by replay.py).
"""

import os
import struct
import sys

import sympy as sp
from flint import fmpq

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))

import dag as D          # noqa: E402
import forms as F        # noqa: E402
import plans as P        # noqa: E402
import rigid3 as R       # noqa: E402

OBJECTS = [("point", R.POINT, "p"), ("line", R.LINE, "l"),
           ("plane", R.PLANE, "g")]
MOTOR_NAMES = [n for n, _, _ in R.MOTOR]
U = {"double": sp.Rational(1, 2**53), "float": sp.Rational(1, 2**24)}


def form_programs(name, layout, suffix):
    """{form label: (dag, outputs)} for one object."""
    out = {}
    out["A"] = F.form_A(layout, suffix)
    g, o, _ = F.form_B(layout, suffix)
    out["B"] = (g, o)
    maker = {"point": F.form_C_point, "line": F.form_C_line,
             "plane": F.form_C_plane}[name]
    out["C-pre"] = maker(True)
    out["C-post"] = maker(False)
    return out


# --- Counts of roundings and magnitudes ---------------------------------------

def track(g, outs):
    """Per output: (K by the core's rule, K with exact doubling, magnitude).

    The core's rule (kernel_roundings in transform.hpp, and tracked.hpp)
    counts a product by a constant that is not +1 or -1 as a rounding; a
    doubling is exact in binary floating point unless it overflows, so the
    second count leaves it out. The magnitude is the program run on the
    absolute values of the inputs with every subtraction an addition, as a
    polynomial in the symbols A<name> = |<name>|."""
    k_core, k_exact, mag = {}, {}, {}
    for i, n in enumerate(g.nodes):
        kind = n[0]
        if kind == "var":
            k_core[i] = k_exact[i] = 0
            mag[i] = sp.Symbol("A" + n[1], nonnegative=True)
        elif kind == "mul":
            a, b = n[1], n[2]
            k_core[i] = k_core[a] + k_core[b] + 1
            k_exact[i] = k_exact[a] + k_exact[b] + 1
            mag[i] = sp.expand(mag[a] * mag[b])
        elif kind in ("add", "sub"):
            a, b = n[1], n[2]
            k_core[i] = max(k_core[a], k_core[b]) + 1
            k_exact[i] = max(k_exact[a], k_exact[b]) + 1
            mag[i] = sp.expand(mag[a] + mag[b])
        elif kind == "dbl":
            a = n[1]
            k_core[i] = k_core[a] + 1
            k_exact[i] = k_exact[a]
            mag[i] = sp.expand(2 * mag[a])
        else:
            raise ValueError(kind)
    return [(k_core[o.node], k_exact[o.node], mag[o.node]) for o in outs]


def kernel_roundings(terms, nout, k_x, k_y):
    """The core's count of roundings for one kernel (rule of
    kernel_roundings in transform.hpp): every coefficient is +1 or -1 here,
    so a term rounds once, after both factors' counts."""
    out = [None] * nout
    for a, b, o, c in terms:
        term = k_x[a] + k_y[b] + 1
        out[o] = term if out[o] is None else max(out[o], term) + 1
    return out


def dropped_roundings(layout):
    """K of each coefficient transform drops in checking mode, from the
    whole first stage and the dropped terms of the second, in the order of
    the dropped blades (canonical order of the second stage's result)."""
    first, second, mid, dropped = P.sandwich_terms(R.MOTOR, layout)
    first_all = P.sandwich_terms.first_all
    k_mid = kernel_roundings(first_all, len(mid), [0] * 8, [0] * len(layout))
    masks = sorted({t[2] for t in dropped}, key=P.canonical_key)
    pos = {m: i for i, m in enumerate(masks)}
    terms = [(a, b, pos[o], c) for a, b, o, c in dropped]
    return kernel_roundings(terms, len(masks), k_mid, [0] * 8), masks


# --- Evaluation ------------------------------------------------------------

def to_float32(x):
    return struct.unpack("<f", struct.pack("<f", x))[0]


def compile_program(g, outs, mode):
    """A function of a dict {input name: value} returning the outputs.

    mode: 'double' (Python floats), 'float' (each result rounded to
    binary32), 'exact' (fmpq), 'abs' (fmpq on absolute values, every
    subtraction an addition: the magnitude)."""
    lines = ["def run(env):"]
    rnd = "f32" if mode == "float" else ""
    for i, n in enumerate(g.nodes):
        kind = n[0]
        if kind == "var":
            v = f"env['{n[1]}']"
            lines.append(f"    t{i} = abs({v})" if mode == "abs"
                         else f"    t{i} = {v}")
        elif kind == "mul":
            lines.append(f"    t{i} = {rnd}(t{n[1]} * t{n[2]})")
        elif kind == "add" or (kind == "sub" and mode == "abs"):
            lines.append(f"    t{i} = {rnd}(t{n[1]} + t{n[2]})")
        elif kind == "sub":
            lines.append(f"    t{i} = {rnd}(t{n[1]} - t{n[2]})")
        elif kind == "dbl":
            lines.append(f"    t{i} = {rnd}(t{n[1]} + t{n[1]})")
    res = []
    for o in outs:
        res.append(f"t{o.node}" if (o.sign > 0 or mode == "abs")
                   else f"(-t{o.node})")
    lines.append("    return [" + ", ".join(res) + "]")
    ns = {"f32": to_float32}
    exec("\n".join(lines), ns)  # text built above from the program's nodes
    return ns["run"]


def exact(x):
    """The exact value of a float or an int as fmpq."""
    n, d = float(x).as_integer_ratio()
    return fmpq(n, d)


# --- compose, as the library's kernel accumulates it ---------------------------

def compose_terms():
    """Terms of a ⟇ b for two motors, onto motor-layout positions, in the
    order of a's position and then b's, with the orientation of the motor
    layout folded in."""
    pos = {m: (i, s) for i, (_, m, s) in enumerate(R.MOTOR)}
    terms = []
    for a, b, mask, c in P.product_terms(R.MOTOR, R.MOTOR):
        i, s = pos[mask]
        terms.append((a, b, i, c * s))
    return terms


COMPOSE = compose_terms()


def compose(qa, qb, rnd=None):
    """qa ⟇ qb (apply qb first), accumulated as the library's kernels do:
    each output's first term assigned or negated, the rest added or
    subtracted in order. rnd rounds each result (None for exact or
    double arithmetic)."""
    r = rnd or (lambda v: v)
    out = [None] * 8
    for a, b, o, c in COMPOSE:
        p = r(qa[a] * qb[b])
        if out[o] is None:
            out[o] = p if c == 1 else -p
        else:
            out[o] = r(out[o] + p) if c == 1 else r(out[o] - p)
    return [v if v is not None else 0 for v in out]
