exec(open('factors.py').read().split('cga2=')[0])
def proj(A,a,b): return A.antiwedge(b, A.wedge(a, A.weight_dual(b)))
def aproj(A,a,b): return A.wedge(b, A.antiwedge(a, A.weight_dual(b)))
for name,g,gp_,P in [("rigid3 e4->2e4'",[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,2]]),
                     ("conformal(2)",[[1,0,0,0],[0,1,0,0],[0,0,0,-1],[0,0,-1,0]],[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,-1]],[[1,0,0,0],[0,1,0,0],[0,0,Fr(-1,2),Fr(1,2)],[0,0,1,1]])]:
    S=Alg(g);T=Alg(gp_);phi=phi_of(P);n=len(g)
    xs=[rnd(n) for _ in range(5)]; ys=[rnd(n) for _ in range(5)]
    print(name,"project",factor([phi(proj(S,x,y)) for x,y in zip(xs,ys)],[proj(T,phi(x),phi(y)) for x,y in zip(xs,ys)]),
          "antiproject",factor([phi(aproj(S,x,y)) for x,y in zip(xs,ys)],[aproj(T,phi(x),phi(y)) for x,y in zip(xs,ys)]))
