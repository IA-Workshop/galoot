# Independent route: Clifford algebra from the defining relation e_j e_i = -e_i e_j + 2 g_ij,
# products in the basis of increasing words; wedge blades as antisymmetrized products.
import itertools, sys
from sympy import Rational, Matrix
from functools import lru_cache

def make(G):
    n = len(G)
    @lru_cache(None)
    def reduce_word(word):
        # returns dict: increasing tuple -> coefficient
        w = list(word)
        for k in range(len(w)-1):
            if w[k] == w[k+1]:
                rest = reduce_word(tuple(w[:k]+w[k+2:]))
                return {m: G[w[k]][w[k]]*c for m, c in rest.items() if G[w[k]][w[k]]*c != 0}
            if w[k] > w[k+1]:
                out = {}
                a = reduce_word(tuple(w[:k]+[w[k+1], w[k]]+w[k+2:]))
                for m, c in a.items(): out[m] = out.get(m, 0) - c
                b = reduce_word(tuple(w[:k]+w[k+2:]))
                for m, c in b.items(): out[m] = out.get(m, 0) + 2*G[w[k]][w[k+1]]*c
                return {m: c for m, c in out.items() if c != 0}
        return {tuple(w): 1}
    def mul(x, y):
        out = {}
        for a, c in x.items():
            for b, d in y.items():
                for m, e in reduce_word(a+b).items():
                    out[m] = out.get(m, 0) + c*d*e
        return {m: c for m, c in out.items() if c != 0}
    def perm_sign(p):
        s = 1
        for i in range(len(p)):
            for j in range(i+1, len(p)):
                if p[i] > p[j]: s = -s
        return s
    from math import factorial
    @lru_cache(None)
    def wedge_blade(idx):  # idx increasing tuple -> element in word basis
        out = {}
        for p in itertools.permutations(range(len(idx))):
            word = tuple(idx[q] for q in p)
            for m, c in reduce_word(word).items():
                out[m] = out.get(m, 0) + Rational(perm_sign(p), factorial(len(idx)))*c
        return tuple(sorted((m, c) for m, c in out.items() if c != 0))
    blades = [tuple(i for i in range(n) if (mask >> i) & 1) for mask in range(1 << n)]
    # matrix: columns = wedge blades in word basis; word basis = increasing tuples = same index set
    idx_of = {b: k for k, b in enumerate(blades)}
    M = Matrix.zeros(1 << n, 1 << n)
    for k, b in enumerate(blades):
        for m, c in wedge_blade(b):
            M[idx_of[m], k] = c
    Minv = M.inv()
    def to_wedge(x):  # word-basis dict -> wedge coefficients by mask
        v = Matrix([x.get(b, 0) for b in blades])
        return list(Minv * v)
    def blade_elt(mask):
        return dict(wedge_blade(blades[mask]))
    def gp_wedge(ma, mb):
        return to_wedge(mul(blade_elt(ma), blade_elt(mb)))
    return n, blades, gp_wedge

def rule_rhs(G, i, b):
    # my own count: contraction (-1)^p g(i,b_p), wedge (-1)^{#b<i}
    n = len(G); x = [0]*(1 << n)
    idx = [j for j in range(n) if (b >> j) & 1]
    for p, j in enumerate(idx):
        x[b & ~(1 << j)] += (-1)**p * G[i][j]
    if i not in idx:
        x[b | (1 << i)] += (-1)**sum(1 for j in idx if j < i)
    return x

forms = {
  'off-axis': [[1,1,0],[1,0,1],[0,1,-1]],
  'all-ones': [[1,1,1],[1,1,1],[1,1,1]],
  'conformal3-like': [[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,0,-1],[0,0,0,-1,0]],
  'dense4': [[2,-1,3,0],[-1,0,1,-2],[3,1,-3,1],[0,-2,1,1]],
}
for name, G in forms.items():
    n, blades, gp = make(G)
    bad = 0
    for i in range(n):
        for b in range(1 << n):
            if [x for x in gp(1 << i, b)] != rule_rhs(G, i, b): bad += 1
    print(name, 'generator-rule mismatches:', bad, 'of', n*(1 << n))
    # sample printout for off-axis
    if name == 'off-axis':
        for (a, b) in [(1, 6), (4, 3), (2, 5), (6, 6), (3, 7)]:
            print('  mask', a, '*', b, '=', gp(a, b))
    # recursion & unit check
    badr = 0
    for a in range(1 << n):
        idx = [j for j in range(n) if (a >> j) & 1]
        if len(idx) < 2: continue
        lo = 1 << idx[0]; rest = a & ~lo
        for b in range(1 << n):
            lhs = gp(a, b)
            # e_lo * (e_rest e_b)
            t = gp(rest, b); r = [0]*(1 << n)
            for m, c in enumerate(t):
                if c:
                    for k, d in enumerate(gp(lo, m)): r[k] += c*d
            inner = [0]*(1 << n)
            ip = [j for j in range(n) if (rest >> j) & 1]
            for p, j in enumerate(ip): inner[rest & ~(1 << j)] += (-1)**p*G[idx[0]][j]
            for m, c in enumerate(inner):
                if c:
                    for k, d in enumerate(gp(m, b)): r[k] -= c*d
            if r != lhs: badr += 1
    print(name, 'recursion mismatches:', badr)
    print(name, 'rank', Matrix(G).rank(), 'nullspace', [list(v) for v in Matrix(G).nullspace()], 'det', Matrix(G).det())
