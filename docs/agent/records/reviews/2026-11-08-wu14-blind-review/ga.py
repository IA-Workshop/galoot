# Independent reviewer route: a diagonal-metric Clifford algebra on bit masks,
# complements from u ^ right(u) = antiscalar, Gram minors for the duals.
from fractions import Fraction
from itertools import combinations
import sympy as sp

def reorder(a, b):
    # sign of e_A e_B reordering for diagonal product (count swaps)
    s = 0
    a >>= 1
    while a:
        s += bin(a & b).count('1'); a >>= 1
    return -1 if s & 1 else 1

def gp(x, y, sq):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            c = ca * cb * reorder(a, b)
            for i in range(len(sq)):
                if (a >> i) & 1 and (b >> i) & 1: c *= sq[i]
            if c: out[a ^ b] = out.get(a ^ b, 0) + c
    return {k: v for k, v in out.items() if v}

def wedge(x, y):
    out = {}
    for a, ca in x.items():
        for b, cb in y.items():
            if a & b: continue
            out[a | b] = out.get(a | b, 0) + ca * cb * reorder(a, b)
    return {k: v for k, v in out.items() if v}

def right(x, n):
    full = (1 << n) - 1; out = {}
    for a, c in x.items():
        s = reorder(a, full ^ a)  # e_A ^ e_comp = s * antiscalar; right(e_A)= s e_comp
        out[full ^ a] = out.get(full ^ a, 0) + c * s
    return out

def left(x, n):
    full = (1 << n) - 1; out = {}
    for a, c in x.items():
        s = reorder(full ^ a, a)
        out[full ^ a] = out.get(full ^ a, 0) + c * s
    return out

def grade(a): return bin(a).count('1')
def scale_by(x, f): return {k: v * f(k) for k, v in x.items()}
def antiprod(x, y, sq, n): return left(gp(right(x, n), right(y, n), sq), n)
def antirev(x, n):
    return scale_by(x, lambda k: (-1) ** (((n - grade(k)) * (n - grade(k) - 1)) // 2))
def antigrade_inv(x, n): return scale_by(x, lambda k: (-1) ** (n - grade(k)))

def transform_mirror(x, V, sq, n):
    r = antiprod(antiprod(V, antigrade_inv(x, n), sq, n), antirev(V, n), sq, n)
    return {k: -v for k, v in r.items() if v}

def bits(a, n): return [i for i in range(n) if (a >> i) & 1]

def bulk_dual_by_pairing(b, G, n):
    # find d of grade n-k with e_A ^ d = det G[A,B] antiscalar for all A of grade k
    k = grade(b); full = (1 << n) - 1; out = {}
    for A in [sum(1 << i for i in c) for c in combinations(range(n), k)]:
        m = sp.Matrix([[G[i][j] for j in bits(b, n)] for i in bits(A, n)]).det() if k else 1
        if m:
            C = full ^ A
            out[C] = out.get(C, 0) + m * reorder(A, C)  # e_A ^ (s e_C) = antiscalar needs s = reorder(A,C)
    return {k_: v for k_, v in out.items() if v}
