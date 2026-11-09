exec(open('factors.py').read().split('cga2=')[0])
run("swap R210 (orthogonal, d=-1)",[[1,0,0],[0,1,0],[0,0,-1]],[[1,0,0],[0,1,0],[0,0,-1]],[[0,1,0],[1,0,0],[0,0,1]])
# conformal(3) bulk of e4 directly and by way of R(4,1,0)
c3=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,0,-1],[0,0,0,-1,0]]
r41=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,1,0],[0,0,0,0,-1]]
P=[[1,0,0,0,0],[0,1,0,0,0],[0,0,1,0,0],[0,0,0,Fr(-1,2),Fr(1,2)],[0,0,0,1,1]]
S=Alg(c3);T=Alg(r41);phi=phi_of(P);psi=phi_of([[Fr(str(v)) for v in r] for r in sp.Matrix(P).inv().tolist()])
print("bulk e4 direct",S.bulk({8:1})," via R410",psi(T.bulk(phi({8:1}))))
print("images e4",phi({8:1}),"e5",phi({16:1}),"e45",phi({24:1}),"I",phi({31:1}))
