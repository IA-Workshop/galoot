import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sympy as sp
from ga import *
P = [sp.symbols(f"{c}{i}") for i in range(3) for c in "xyzw"]
p, q, r = P[0:4], P[4:8], P[8:12]
k1, res1 = product_kernel(POINT, POINT, wedge_blade)
pq = run(k1, p, q, len(res1))
k2, res2 = product_kernel(res1, POINT, wedge_blade, PLANE)
g = [sp.expand(e) for e in run(k2, pq, r, 4)]
E = sp.symbols("ex ey ez ew")
D = sp.Matrix([list(E), p, q, r]).det()
cof = [sp.expand(D.coeff(e)) for e in E]
print([sp.expand(a - b) == 0 for a, b in zip(g, cof)], [sp.expand(a + b) == 0 for a, b in zip(g, cof)])
