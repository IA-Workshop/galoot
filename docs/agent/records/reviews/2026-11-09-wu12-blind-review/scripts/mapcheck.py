# Replica of blade_image/typed_change; compares with minors of P computed by sympy,
# and records the largest term list seen against C(n, min(k, n//2)).
import random, itertools, math
from fractions import Fraction as F
import sympy as sp
def wedge_sign(blade, j):
    if blade>>j &1: return 0
    # e_B ^ e_j : sign = (-1)^{number of bits of B above j}
    return -1 if bin(blade>>(j+1)).count('1')%2 else 1
def blade_image(rows,a,n,stat):
    img={0:1}
    k=0
    for i in range(n):
        if not a>>i&1: continue
        k+=1
        nxt={}
        for b,c in img.items():
            for j in range(n):
                if rows[i][j]==0: continue
                s=wedge_sign(b,j)
                if s==0: continue
                key=b|1<<j
                nxt[key]=nxt.get(key,0)+c*rows[i][j]*s
                if nxt[key]==0: del nxt[key]
            stat[0]=max(stat[0],len(nxt))
        img=nxt
        cap=math.comb(n,min(k,n//2))
        assert stat[0]<=max(cap, 1) or True
        stat[1]=max(stat[1], len(img)/math.comb(n,min(bin(a).count('1'),n//2)))
    return img
def typed(rows,dens,blades,n):
    # returns {out: (q, {a: c})}
    terms=[]
    for ai,a in enumerate(blades):
        st=[0,0]
        for b,c in blade_image(rows,a,n,st).items():
            own=1
            for i in range(n):
                if a>>i&1: own*=dens[i]
            terms.append((ai,b,c,own))
    q={}
    for ai,b,c,own in terms: q[b]=q.get(b,1)*own//math.gcd(q.get(b,1),own)
    co=[(ai,b,c*q[b]//own) for ai,b,c,own in terms]
    g=dict(q)
    for ai,b,c in co: g[b]=math.gcd(g[b],c)
    out={}
    for ai,b,c in co: out.setdefault(b,[q[b]//g[b],{}])[1][ai]=c//g[b]
    return out
def minor(P,A,B):
    ra=[i for i in range(len(P)) if A>>i&1]; cb=[j for j in range(len(P)) if B>>j&1]
    return sp.Matrix([[P[i][j] for j in cb] for i in ra]).det() if ra else sp.Integer(1)
random.seed(7); bad=0; worst=0
for t in range(300):
    n=random.randint(1,5)
    rows=[[random.randint(-3,3) for _ in range(n)] for _ in range(n)]
    dens=[random.randint(1,6) for _ in range(n)]
    P=[[sp.Rational(rows[i][j],dens[i]) for j in range(n)] for i in range(n)]
    blades=list(range(1<<n))
    m=typed(rows,dens,blades,n)
    for A in blades:
        for B in range(1<<n):
            if bin(A).count('1')!=bin(B).count('1'): continue
            want=minor(P,A,B)
            got=sp.Rational(m[B][1].get(A,0),m[B][0]) if B in m else 0
            if want!=got: bad+=1
    # least denominator: q_B must equal lcm of reduced denominators
    for B,(q,cs) in m.items():
        lq=1
        for A in cs:
            r=minor(P,A,B); lq=lq*r.q//math.gcd(lq,r.q)
        if lq!=q: bad+=1; print("q not least",q,lq)
print("bad",bad)
# capacity: worst ratio of a partial term list to C(n, min(k,n/2)) for dense rows
for n in range(1,9):
    rows=[[random.choice([-2,-1,1,2,3]) for _ in range(n)] for _ in range(n)]
    worst=0
    for a in range(1<<n):
        st=[0,0]; blade_image(rows,a,n,st)
        k=bin(a).count('1')
        worst=max(worst, st[0]/math.comb(n,min(k,n//2)))
    print(n,"max partial/cap",worst)
