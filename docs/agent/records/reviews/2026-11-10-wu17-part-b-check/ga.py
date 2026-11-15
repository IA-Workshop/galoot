"""Independent rigid(3) algebra and kernel-term counts for the WU17 part B check.

Built from CONVENTIONS 1, 2, 3.1, 4.1, 4.2, 4.4 and the headers' kernel rules,
sharing no code with the library, test/reference, or the spike-e files.
"""
import itertools
from fractions import Fraction
import sympy as sp

N = 4
SQ = (1, 1, 1, 0)  # e1..e3 Euclidean, e4 null
FULL = (1 << N) - 1


def grade(m):
    return bin(m).count("1")


def reorder_sign(a, b):
    # sign of e_a e_b -> e_{a xor b} reordering (bit i stands for e_{i+1})
    s = 0
    for j in range(N):
        if b >> j & 1:
            s += sum(1 for i in range(j + 1, N) if a >> i & 1)
    return -1 if s % 2 else 1


def gp_blade(a, b):
    c = reorder_sign(a, b)
    for i in range(N):
        if (a >> i & 1) and (b >> i & 1):
            c *= SQ[i]
    return c, a ^ b


def wedge_blade(a, b):
    if a & b:
        return 0, a | b
    return reorder_sign(a, b), a | b


def right_c(m):
    c = FULL ^ m
    s, _ = wedge_blade(m, c)
    return s, c


def left_c(m):
    c = FULL ^ m
    s, _ = wedge_blade(c, m)
    return s, c


def anti_blade(a, b, prod):
    sa, ra = right_c(a)
    sb, rb = right_c(b)
    c, m = prod(ra, rb)
    if c == 0:
        return 0, m
    sl, lm = left_c(m)
    return sa * sb * c * sl, lm


def gap_blade(a, b):
    return anti_blade(a, b, gp_blade)


def antiwedge_blade(a, b):
    return anti_blade(a, b, wedge_blade)


def antireverse_sign(m):
    k = N - grade(m)
    return -1 if (k * (k - 1) // 2) % 2 else 1


def canon_key(m):
    idx = [i + 1 for i in range(N) if m >> i & 1]
    return (len(idx), idx)


def mask_of(name):
    if name == "1":
        return 0
    m = 0
    for ch in name[1:]:
        m |= 1 << (int(ch) - 1)
    return m


def oriented(name):
    """(mask, sign) of an oriented name like e41 = -e14."""
    if name == "1":
        return 0, 1
    idx = [int(c) for c in name[1:]]
    # sign of the permutation sorting idx
    inv = sum(1 for i in range(len(idx)) for j in range(i + 1, len(idx)) if idx[i] > idx[j])
    return mask_of(name), (-1 if inv % 2 else 1)


def layout(names):
    return [oriented(n) for n in names.split()]


POINT = layout("e1 e2 e3 e4")
LINE = layout("e41 e42 e43 e23 e31 e12")
PLANE = layout("e423 e431 e412 e321")
MOTOR = layout("e41 e42 e43 e1234 e23 e31 e12 1")


def plan(A, B, prod):
    """Terms (a, b, outmask, coeff) in order of a then b; result set canonical +1."""
    terms = []
    for ia, (ma, sa) in enumerate(A):
        for ib, (mb, sb) in enumerate(B):
            c, m = prod(ma, mb)
            if c:
                terms.append((ia, ib, m, c * sa * sb))
    result = sorted({t[2] for t in terms}, key=canon_key)
    return terms, [(m, 1) for m in result]


def kernel_counts(terms):
    """terms: list of (a, b, out, coeff) in kernel order. Form A rule of mv.hpp accumulate."""
    mult = adds = subs = negs = cmul = 0
    seen = set()
    for _, _, out, c in terms:
        mult += 1
        first = out not in seen
        seen.add(out)
        if c == 1:
            if not first:
                adds += 1
        elif c == -1:
            if first:
                negs += 1
            else:
                subs += 1
        else:
            cmul += 1
            if not first:
                adds += 1
    return dict(mul=mult, cmul=cmul, add=adds, sub=subs, neg=negs, outs=len(seen), terms=len(terms))


def product_kernel(A, B, prod, target=None):
    terms, result = plan(A, B, prod)
    if target is None:
        target = result
    pos = {m: i for i, (m, s) in enumerate(target)}
    return [(a, b, pos[m], c * target[pos[m]][1]) for a, b, m, c in terms], target


def transform_kernels(V, X, checking=False):
    first_terms, middle = plan(V, X, gap_blade)
    mpos = {m: i for i, (m, _) in enumerate(middle)}
    first = [(a, b, mpos[m], c) for a, b, m, c in first_terms]  # even V: no involution sign
    second_terms, res2 = plan(middle, V, gap_blade)
    xpos = {m: i for i, (m, _) in enumerate(X)}
    kept, dropped = [], []
    dropped_masks = [m for m, _ in res2 if m not in xpos]
    dpos = {m: i for i, m in enumerate(dropped_masks)}
    for a, b, m, c in second_terms:
        sgn = antireverse_sign(V[b][0])
        if m in xpos:
            kept.append((a, b, xpos[m], X[xpos[m]][1] * sgn * c))
        else:
            dropped.append((a, b, dpos[m], sgn * c))
    read = {a for a, _, _, _ in kept}
    first_r = [t for t in first if t[2] in read]
    return dict(first_all=first, first_r=first_r, kept=kept, dropped=dropped,
                middle=middle, dropped_masks=dropped_masks)


def run(terms, x, y, size):
    out = [0] * size
    for a, b, o, c in terms:
        out[o] += c * x[a] * y[b]
    return out


def name(m):
    return "1" if m == 0 else "e" + "".join(str(i + 1) for i in range(N) if m >> i & 1)
