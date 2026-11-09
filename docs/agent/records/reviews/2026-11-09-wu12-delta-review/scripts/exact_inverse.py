"""The exact inverse of the probe's change P = Δ^-1 M, by sympy: row j of
P^-1 writes e'_j in the source's generators."""
import sympy as sp
M = sp.Matrix([[0, 1048576, 0], [0, -16777216, 46341], [2, -378, 0]])
D = sp.diag(1, 1073741824, 2147483647)
P = D.inv() * M
Pi = P.inv()
for j in range(3):
    print(f"e'{j+1} ->", [str(x) for x in Pi.row(j)], [float(x) for x in Pi.row(j)])
