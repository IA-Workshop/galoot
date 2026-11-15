# Reviewer's own route: Clifford algebra of a symmetric Gram matrix in the wedge
# basis (Chevalley recursion), complements, exomorphism, duals, written from
# CONVENTIONS 3.x/4.x and dual.hpp's table. Determines for each operation the
# single factor f with phi(op(x,y)) = f op'(phi x, phi y), or "none".
import random, itertools
from fractions import Fraction as Fr
import sympy as sp
random.seed(2026)
def bits(m): return [i for i in range(32) if m>>i&1]
def gr(m): return bin(m).count('1')
def wsign(A,B):  # e_A ^ e_B sign, 0 if overlap
    if A&B: return 0
    s=0
    for b in bits(B): s+=gr(A>>(b+1))
    return -1 if s%2 else 1
def add(d,k,v):
    if v==0: return
    d[k]=d.get(k,0)+v
    if d[k]==0: del d[k]
def lin(f,x):
    out={}
    for k,v in x.items():
        for k2,v2 in f(k).items(): add(out,k2,v*v2)
    return out
def scale(x,s): return {k:v*s for k,v in x.items() if v*s!=0}
class Alg:
    def __init__(s,g):
        s.g=[[Fr(v) for v in r] for r in g]; s.n=len(g); s.I=(1<<s.n)-1
        s._m={}
    def vlc(s,i,B):
        out={}; bs=bits(B)
        for r,b in enumerate(bs): add(out,B&~(1<<b),(-1)**r*s.g[i][b])
        return out
    def vmul(s,i,Y):
        out={}
        for B,c in Y.items():
            for k,v in s.vlc(i,B).items(): add(out,k,c*v)
            w=wsign(1<<i,B)
            if w: add(out,B|1<<i,c*w)
        return out
    def bmul(s,A,Y):
        if A==0: return dict(Y)
        a=bits(A)[0]; Ap=A&~(1<<a)
        t1=s.vmul(a,s.bmul(Ap,Y))
        out=dict(t1)
        for K,v in s.vlc(a,Ap).items():
            for k2,v2 in s.bmul(K,Y).items(): add(out,k2,-v*v2)
        return out
    def gp(s,x,y):
        out={}
        for A,c in x.items():
            for k,v in s.bmul(A,y).items(): add(out,k,c*v)
        return out
    def wedge(s,x,y):
        out={}
        for A,a in x.items():
            for B,b in y.items():
                w=wsign(A,B)
                if w: add(out,A|B,a*b*w)
        return out
    def right(s,x): return lin(lambda A:{s.I&~A:Fr(1,wsign(A,s.I&~A))},x)
    def left(s,x): return lin(lambda A:{s.I&~A:Fr(1,wsign(s.I&~A,A))},x)
    def G(s,A):
        out={}
        for B in range(1<<s.n):
            if gr(B)!=gr(A): continue
            if A==0: out[0]=Fr(1); continue
            m=sp.Matrix([[s.g[i][j] for j in bits(B)] for i in bits(A)]).det()
            add(out,B,Fr(int(sp.fraction(m)[0]),int(sp.fraction(m)[1])))
        return out
    def bulk(s,x): return lin(s.G,x)
    def weight(s,x): return s.left(s.bulk(s.right(x)))
    def bulk_dual(s,x): return s.right(s.bulk(x))
    def lbulk_dual(s,x): return s.left(s.bulk(x))
    def weight_dual(s,x): return s.right(s.weight(x))
    def lweight_dual(s,x): return s.left(s.weight(x))
    def dot(s,a,b):
        Gb=s.bulk(b); return {0:sum(a.get(k,0)*v for k,v in Gb.items())} if sum(a.get(k,0)*v for k,v in Gb.items()) else {}
    def antidot(s,a,b):
        Wb=s.weight(b); v=sum(a.get(k,0)*w for k,w in Wb.items()); return {s.I:v} if v else {}
    def antiwedge(s,a,b): return s.left(s.wedge(s.right(a),s.right(b)))
    def antiprod(s,a,b): return s.left(s.gp(s.right(a),s.right(b)))
    def rev(s,x): return {k:v*(-1)**(gr(k)*(gr(k)-1)//2) for k,v in x.items()}
    def antirev(s,x): return {k:v*(-1)**((s.n-gr(k))*(s.n-gr(k)-1)//2) for k,v in x.items()}
    def ginv(s,x): return {k:v*(-1)**gr(k) for k,v in x.items()}
def phi_of(P):
    n=len(P)
    def blade(A):
        out={}
        for B in range(1<<n):
            if gr(B)!=gr(A): continue
            if A==0: out[0]=Fr(1); continue
            m=sp.Matrix([[P[i][j] for j in bits(B)] for i in bits(A)]).det()
            add(out,B,Fr(str(m)))
        return out
    return lambda x: lin(blade,x)
def rnd(n,even=None):
    x={}
    for A in range(1<<n):
        if even is not None and gr(A)%2!=(0 if even else 1): continue
        add(x,A,Fr(random.randint(-5,5),random.choice([1,1,2,3])))
    return x
def factor(lhs,rhs):
    # single f with lhs = f*rhs over all samples, else None
    f=None
    for L,R in zip(lhs,rhs):
        keys=set(L)|set(R)
        for k in keys:
            l=L.get(k,0); r=R.get(k,0)
            if r==0:
                if l!=0: return None
                continue
            q=Fr(l)/Fr(r)
            if f is None: f=q
            elif q!=f: return None
    return f
def run(name,g,gp_,P,samples=6):
    S=Alg(g); T=Alg(gp_); n=len(g)
    Pm=sp.Matrix(P); Gm=sp.Matrix(g); Gp=sp.Matrix(gp_)
    assert Pm*Gp*Pm.T==Gm, name
    d=Pm.det(); phi=phi_of(P)
    C=(Pm.inv().T*Gp*Pm.inv()==Gm)
    orth=(Pm*Pm.T==sp.eye(n)); bulkc=(Pm*Pm.T*Gm==Gm); wc=(Gp*Pm.T*Pm==Gp)
    xs=[rnd(n) for _ in range(samples)]; ys=[rnd(n) for _ in range(samples)]
    res={}
    bin_ops={'gp':'gp','wedge':'wedge','dot':'dot','antiwedge':'antiwedge','antiprod':'antiprod','antidot':'antidot'}
    for op in bin_ops:
        res[op]=factor([phi(getattr(S,op)(x,y)) for x,y in zip(xs,ys)],[getattr(T,op)(phi(x),phi(y)) for x,y in zip(xs,ys)])
    for op in ['rev','antirev','ginv','bulk_dual','lbulk_dual','weight_dual','lweight_dual','right','left','bulk','weight']:
        res[op]=factor([phi(getattr(S,op)(x)) for x in xs],[getattr(T,op)(phi(x)) for x in xs])
    # transform by even and odd versor built from random vectors (antiproduct of hyperplanes)
    for par in (0,1):
        lhs=[];rhs=[]
        for x in xs[:3]:
            hs=[{1<<i:Fr(random.randint(-3,3)) for i in range(n)} for _ in range(2 if par==0 else 1)]
            hs=[S.right(h) for h in hs]  # hyperplanes: complements of vectors (antigrade 1)
            V=hs[0]
            for h in hs[1:]: V=S.antiprod(V,h)
            def tr(A,V,x):
                if par==0: return A.antiprod(A.antiprod(V,x),A.antirev(V))
                xc={k:v*(-1)**(A.n-gr(k)) for k,v in x.items()}
                return scale(A.antiprod(A.antiprod(V,xc),A.antirev(V)),-1)
            lhs.append(phi(tr(S,V,x))); rhs.append(tr(T,phi(V),phi(x)))
        res['transform_'+('even' if par==0 else 'odd')]=factor(lhs,rhs)
    print(f"{name}: d={d} (C)={C} orth={orth} PPtG=G:{bulkc} G'PtP=G':{wc}")
    print("   ", {k:(str(v) if v is not None else 'none') for k,v in res.items()})
cga2=[[1,0,0,0],[0,1,0,0],[0,0,0,-1],[0,0,-1,0]]
r310=[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,-1]]
P_cga2=[[1,0,0,0],[0,1,0,0],[0,0,Fr(-1,2),Fr(1,2)],[0,0,1,1]]
run("conformal(2)",cga2,r310,P_cga2)
r301=[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,0]]
run("rigid3 e4->2e4'",r301,r301,[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,2]])
M=sp.Matrix([[1,1,0],[0,1,1],[1,0,2]]); D=sp.diag(1,1,-1); G=M*D*M.T
run("dense d=3",G.tolist(),D.tolist(),M.tolist())
run("rotation R200",[[1,0],[0,1]],[[1,0],[0,1]],[[Fr(3,5),Fr(4,5)],[Fr(-4,5),Fr(3,5)]])
run("R101 e2->2e2'",[[1,0],[0,0]],[[1,0],[0,0]],[[1,0],[0,2]])
run("null shear R101",[[1,0],[0,0]],[[1,0],[0,0]],[[1,1],[0,1]])
run("witt1",[[0,1],[1,0]],[[1,0],[0,-1]],[[1,1],[Fr(1,2),Fr(-1,2)]])
# an LDLT-like change of a dense nondegenerate form with d != +-1: G=[[2,1],[1,3]] -> diag(2, 5/2*?) -> integer diag via rescale
# e1'=e1, e2' = e2 - e1/2 has square 3-1+1/2 = 5/2; scale e2'' = 2 e2' squares 10: P rows write e_i in e'': e1=e''1, e2 = e''1/2 + e''2/2
run("ldlt [[2,1],[1,3]] -> diag(2,10)",[[2,1],[1,3]],[[2,0],[0,10]],[[1,0],[Fr(1,2),Fr(1,2)]])
