import sys
sys.path.insert(0, __import__("os").path.dirname(__import__("os").path.abspath(__file__)))
from ga import *
a = Alg([1,1,1,0])
def m(s): return mask(s)
L = {
 "point": ["1","2","3","4"], "line": ["14","24","34","23","13","12"],
 "plane": ["234","134","124","123"],
 "motor": ["14","24","34","1234","23","13","12",""],
 "flector": ["1","2","3","4","234","134","124","123"],
}
L = {k: [m(s) for s in v] for k, v in L.items()}
def terms(A, B):
    out = []
    for i in A:
        for j in B:
            r = a.agp({i: Fr(1)}, {j: Fr(1)})
            for k in r:
                out.append((i, j, k))
    return out
for vn, xn in [("motor","point"),("flector","point"),("motor","line"),("motor","plane"),("flector","motor"),("motor","flector")]:
    V, X = L[vn], L[xn]
    t1 = terms(V, X)
    mid = sorted({k for _,_,k in t1})
    t2 = terms(mid, V)
    kept = [t for t in t2 if t[2] in X]
    read = {t[0] for t in kept}
    first_kept = [t for t in t1 if t[2] in read]
    res = {k for _,_,k in t2}
    print(vn, xn, "first_kept", len(first_kept), "first_all", len(t1), "second_kept", len(kept), "second_dropped", len(t2)-len(kept), "dropped_blades", len(res - set(X)))
