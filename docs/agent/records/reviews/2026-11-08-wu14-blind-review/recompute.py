import sys
sys.path.insert(0, sys.argv[1])
from ga import *
# The reviewer's recomputation of golden and hand values. Run:
#   python3 -I recompute.py <this directory>   (needs sympy)
# 1. rigid(2): squares (1,1,0); mirror V = 5 e12 - 4 e13 + 3 e23 ; image of e3
sq = [1, 1, 0]; n = 3
V = {0b011: 5, 0b101: -4, 0b110: 3}
for m in range(8):
    print('rigid2', bin(m), transform_mirror({m: 1}, V, sq, n))
# own route for the mirror: line 3x+4y+5w=0 ; incidence x v V = f.x
f = [3, 4, 5]
for i in range(3):
    print('  x v V on e%d:' % (i+1), antiprod_w := left(wedge(right({1 << i: 1}, n), right(V, n)), n))
# 2. conformal(3): bulk dual via the pairing property, G conformal: e1..e3 =1, e4.e5=-1
G = [[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,0,-1],[0,0,0,-1,0]]
for b in [0b00001, 0b01000, 0b11000, 0b00111, 0b10011]:
    print('cga3 bulk_dual', bin(b), {bin(k): v for k, v in bulk_dual_by_pairing(b, G, 5).items()})
# rigid(3) G = diag(1,1,1,0)
Gr = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,0]]
for b in [0b0001, 0b0010, 0b0101, 0b0111]:
    bd = bulk_dual_by_pairing(b, Gr, 4)
    # left dual: left(G u) ; right dual right(G u): compare
    print('rga3 bulk_dual', bin(b), {bin(k): v for k, v in bd.items()})
# 3. flector plane: rigid(3), sq (1,1,1,0), F = e412 (mask e1 e2 e4 = 0b1011 with orientation e412 = e4 e1 e2)
sq3 = [1, 1, 1, 0]; n = 4
e412 = {0b1011: reorder(0b1000, 0b0011)}  # e4 e1 e2 = sign * e124
def oriented(names_signs):
    out = {}
    for idx, c in names_signs:
        m = 0; s = 1
        for i in idx:
            s *= reorder(m, 1 << (i - 1)); m |= 1 << (i - 1)
        out[m] = out.get(m, 0) + c * s
    return out
F = oriented([((4, 1, 2), 1)])
g = oriented([((4, 2, 3), 0), ((4, 3, 1), 0), ((4, 1, 2), 1), ((3, 2, 1), -1)])
img = transform_mirror(g, F, sq3, n)
back = {}
for name in [(4, 2, 3), (4, 3, 1), (4, 1, 2), (3, 2, 1)]:
    o = oriented([(name, 1)]); (m, s), = o.items(); back[name] = img.get(m, 0) * s
print('flector plane image', back)
pt = oriented([((1,), 1), ((2,), 2), ((3,), 3), ((4,), 1)])
img = transform_mirror(pt, F, sq3, n); print('flector point image', {bin(k): v for k, v in img.items()})
