exec(open('factors.py').read().split('cga2=')[0])
G=[[2,0,-3],[0,1,-3],[-3,-3,-2]]; D=[[2,0,0],[0,1,0],[0,0,-62]]
P=[[1,0,0],[0,1,0],[Fr(-3,2),-3,Fr(1,2)]]
Pm=sp.Matrix(P); print("isometry",Pm*sp.Matrix(D)*Pm.T==sp.Matrix(G), "det", Pm.det())
S=Alg(G);T=Alg(D);phi=phi_of(P);psi=phi_of([[Fr(str(v)) for v in r] for r in Pm.inv().tolist()])
e12={3:Fr(1)}; e3={4:Fr(1)}
print("antiprod e12 e12",S.antiprod(e12,e12),"via",psi(T.antiprod(phi(e12),phi(e12))))
print("antidot",S.antidot(e12,e12),"via",psi(T.antidot(phi(e12),phi(e12))))
print("wdual e3",S.weight_dual(e3),"via",psi(T.weight_dual(phi(e3))))
c3=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,0,-1],[0,0,0,-1,0]]
r41=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,1,0],[0,0,0,0,-1]]
P=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,Fr(-1,2),Fr(1,2)],[0,0,0,1,1]]
S=Alg(c3);T=Alg(r41);phi=phi_of(P);psi=phi_of([[Fr(str(v)) for v in r] for r in sp.Matrix(P).inv().tolist()])
o={8:Fr(1)}
for op in ['weight','right','left']:
    print(op, getattr(S,op)(o), "via", psi(getattr(T,op)(phi(o))))
