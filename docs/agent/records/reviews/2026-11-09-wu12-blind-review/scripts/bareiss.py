# Replica of basis_change.hpp eliminate(): checks exactness of every division,
# that the right block is p*M^-1 with p = +-det M, across random and swap-heavy matrices.
import random, sympy as sp
def elim(M):
    n=len(M); a=[list(M[i])+[1 if j==i else 0 for j in range(n)] for i in range(n)]
    prev=1; inexact=0
    for k in range(n):
        r=k
        while r<n and a[r][k]==0: r+=1
        if r==n: return 0,None,inexact
        if r!=k: a[r],a[k]=a[k],a[r]
        pr=a[k][:]
        for i in range(n):
            if i==k: continue
            row=a[i]
            for j in range(2*n):
                if j!=k:
                    num=pr[k]*row[j]-row[k]*pr[j]
                    if num%prev!=0: inexact+=1
                    q=abs(num)//abs(prev); q = q if (num>=0)==(prev>0) else -q  # C++ truncation
                    row[j]=q
            row[k]=0
        prev=a[k][k]
    return prev,[r[n:] for r in a],inexact
random.seed(12345)
bad=0; tot=0
for trial in range(4000):
    n=random.randint(1,7)
    dens=random.random()
    M=[[random.randint(-4,4) if random.random()<dens else 0 for _ in range(n)] for _ in range(n)]
    if random.random()<0.3:  # force zero leading pivots
        M[0][0]=0
    p,R,inex=elim(M)
    d=sp.Matrix(M).det()
    tot+=1
    if d==0:
        if p!=0: bad+=1; print("singular but p",p,M)
        continue
    if inex: bad+=1; print("inexact",M); continue
    if abs(p)!=abs(d): bad+=1; print("pivot",p,d,M); continue
    if sp.Matrix(R)!=p*sp.Matrix(M).inv(): bad+=1; print("inverse",M)
print("trials",tot,"bad",bad)
