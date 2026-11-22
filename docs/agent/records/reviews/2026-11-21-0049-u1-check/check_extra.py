import sys, os
import sympy as sp
sys.path.insert(0, os.path.dirname(__file__))
from ga import Alg, vec, mvb, neg, sub, scale, popcount
ok = True
def check(n, c):
    global ok; print(("PASS " if c else "FAIL ") + n); ok &= bool(c)
def sym(alg, p, pred=lambda m: True):
    return {m: sp.Symbol(f"{p}{m}") for m in alg.blades() if pred(m)}
for sq, lab in [([1,1,1,0], "rigid(3)"), ([1,1,-1], "R(2,1,0)"), ([1,0,0], "R(1,0,2)"), ([1,1,1], "R(3,0,0)")]:
    A = Alg(sq); x = sym(A, "x"); V = sym(A, "v")
    check(f"{lab}: antirev(V) = left(rev(right(V)))", sub(A.antirev(V), A.left(A.rev(A.right(V)))) == {})
    for par in (0, 1):
        Vp = sym(A, "v", lambda m: (A.n - popcount(m)) % 2 == par)
        W, X = A.right(Vp), A.right(x)
        rhs = A.gp(A.gp(W, X), A.rev(W)) if par == 0 else neg(A.gp(A.gp(W, A.ginv(X)), A.rev(W)))
        check(f"{lab} antigrade parity {par}: Theorem C", sub(A.right(A.transform_full(x, Vp)), rhs) == {})
R4 = Alg([1,1,1,0])
check("M6: right(right(p)) = -p in rigid(3)", R4.right(R4.right(vec(1,2,3,1))) == neg(vec(1,2,3,1)))
check("M27: right(e4) = e321 (= -e123)", R4.right(vec(0,0,0,1)) == mvb((1,(3,2,1))))
# M29: same-sign copy turns the other way
same = mvb((1,(4,3)), (1,(1,2,3,4)))  # (0,0,-1? ) copying b3=-1 onto vz with same sign -> vz=-1
same = mvb((-1,(4,3)), (1,(1,2,3,4)))
check("M29: motor (0,0,-1,1,...) sends (1,0,0,1) to 2(0,-1,0,1)", R4.transform_full(vec(1,0,0,1), same) == scale(vec(0,-1,0,1), 2))
print("ALL PASS" if ok else "SOME FAILED")
