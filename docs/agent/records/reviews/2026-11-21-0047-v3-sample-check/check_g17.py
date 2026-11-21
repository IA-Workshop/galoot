import sys, re
import sympy as sp

src = open(sys.argv[1]).read()
ns = src[src.index("namespace point_groups {"):src.index("} // namespace point_groups")]
epart = ns[ns.index("elements"):]
fr = [sp.Rational(int(a), int(b)) for a, b in re.findall(r"\{(-?\d+), (\d+)\}", epart)]
elements = [sp.ImmutableMatrix(3, 3, fr[9 * e:9 * e + 9]) for e in range(332)]
# offsets: from the published orders in the golden order
names = ["1", "-1", "2", "m", "2/m", "222", "mm2", "mmm", "4", "-4", "4/m", "422", "4mm", "-42m", "4/mmm",
         "3", "-3", "32", "3m", "-3m", "6", "-6", "6/m", "622", "6mm", "-6m2", "6/mmm", "23", "m-3", "432", "-43m", "m-3m"]
orders = [1, 2, 2, 2, 4, 4, 4, 8, 4, 4, 8, 8, 8, 8, 16, 3, 6, 6, 6, 12, 6, 6, 12, 12, 12, 12, 24, 12, 24, 24, 24, 48]
off = {}
s = 0
for nm, o in zip(names, orders):
    off[nm] = (s, o); s += o

def unit(v):
    v = sp.Matrix(v); return v / sp.sqrt((v.T * v)[0])
def refl(n):
    n = unit(n); return sp.eye(3) - 2 * n * n.T
def close(gens):
    found = {sp.ImmutableMatrix(sp.eye(3))}
    fr_ = [sp.eye(3)]
    while fr_:
        new = []
        for x in fr_:
            for g in gens:
                y = sp.ImmutableMatrix(sp.simplify(x * g))
                if y not in found:
                    found.add(y); new.append(y)
        fr_ = new
    return found
def angle(u, v):
    return sp.acos(sp.simplify((unit(u).T * unit(v))[0]))

# Hestenes' codes: unit vectors with angle(a,b) = pi/p, angle(b,c) = pi/q, a orthogonal to c.
# "p qbar" -> generators a, bc;  "pbar qbar" -> ab, bc.  My own realizations (different coordinates where I could).
cases = {
    "2/m": (2, 2, "a,bc", (1, 0, 0), (0, 1, 0), (0, 0, 1)),
    "-42m": (4, 2, "a,bc", (1, 0, 0), (1, 1, 0), (0, 0, 1)),
    "32": (3, 2, "ab,bc", (1, -1, 0), (0, -1, 1), (1, 1, 1)),
}
axes = {
    "2/m": (1, 0, -1),
    "-42m": (sp.cos(sp.pi / 8), sp.sin(sp.pi / 8), 0),
    "32": ((3 + sp.sqrt(3)) / 6, (sp.sqrt(3) - 3) / 6, -sp.sqrt(3) / 3),
}
for nm, (p, q, code, a, b, c) in cases.items():
    assert sp.simplify(angle(a, b) - sp.pi / p) == 0 and sp.simplify(angle(b, c) - sp.pi / q) == 0
    assert (sp.Matrix(a).T * sp.Matrix(c))[0] == 0
    A, B, C = refl(a), refl(b), refl(c)
    gens = [A, B * C] if code == "a,bc" else [A * B, B * C]
    H = close(gens)
    st, o = off[nm]
    L = set(elements[st:st + o])
    u = unit(axes[nm])
    print(nm, "|u|^2 =", sp.simplify((u.T * u)[0]))
    X = sp.simplify(2 * u * u.T - sp.eye(3))  # half-turn about u
    conj = {sp.ImmutableMatrix(sp.simplify(X * m * X.T)) for m in L}
    print(f"{nm}: |H| = {len(H)}, |library| = {len(L)}, library == H as sets: {L == H}, "
          f"half-turn about the record's axis conjugates library onto H: {conj == H}")
    # same check with my own realization in other coordinates for 2/m: H' from a=(0,0,1): 2-fold along z = library
