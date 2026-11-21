import sys, random
sys.path.insert(0, sys.argv[1])
from mx import *

rnd = random.Random(7)
def rmv(A, lo=-3, hi=3, grades=None):
    return [Q(rnd.randint(lo, hi)) if (grades is None or A.grade(m) in grades) else Q(0) for m in range(A.N)]

results = []
def ok(name, cond):
    results.append((name, bool(cond)))
    print(("PASS " if cond else "FAIL ") + name)

def mat(rows):
    return flint.fmpq_mat([[Q(x) for x in r] for r in rows])

def change(A, Ap, P):
    """phi: A -> Ap, row i of P writes e_i in Ap's generators."""
    n = A.n
    def phi(a):
        out = Ap.zero()
        for m, c in enumerate(a):
            if c == 0: continue
            img = Ap.basis(0)
            for i in range(n):
                if m >> i & 1:
                    img = Ap.wedge(img, Ap.vec([P[i, j] for j in range(n)]))
            out = Ap.add(out, Ap.scale(c, img))
        return out
    return phi

def det(P): return P.det()

# ---------- 1.4 ----------
for d in (1, 2, 3):
    n = d + 2
    C = Alg(conformal(d)); R = Alg(diag(*([1] * (d + 1) + [-1])))
    P = flint.fmpq_mat(n, n)
    for i in range(d): P[i, i] = 1
    P[d, d] = Q(-1, 2); P[d, d + 1] = Q(1, 2)      # origin = 1/2(e'(d+2) - e'(d+1))
    P[d + 1, d] = 1; P[d + 1, d + 1] = 1          # infinity = e'(d+2) + e'(d+1)
    ok(f"1.4 d={d}: P G' P^T = G", P * mat(R.g) * P.transpose() == mat(C.g))
    ok(f"1.4 d={d}: det P = -1", det(P) == -1)
    # matrix route: build e_(d+1), e_(d+2) as matrices from R's generator matrices and wedge them
    eo = Q(-1, 2) * R.e[d] + Q(1, 2) * R.e[d + 1]
    ei = R.e[d] + R.e[d + 1]
    w = Q(1, 2) * (eo * ei - ei * eo)
    m45 = (1 << d) | (1 << (d + 1))
    ok(f"1.4 d={d}: e(d+1)(d+2) -> -e'(d+1)(d+2) (matrices)", w == -1 * R.blade[m45])
    phi = change(C, R, P)
    ok(f"1.4 d={d}: pseudoscalar -> -pseudoscalar'", phi(C.basis(C.full())) == R.scale(-1, R.basis(R.full())))
    # e(d+1).e(d+2) = -1 in conformal: from matrices
    ok(f"1.4 d={d}: e(d+1) e(d+2) + e(d+2) e(d+1) = -2", C.e[d] * C.e[d + 1] + C.e[d + 1] * C.e[d] == -2 * C.Id)

# ---------- 1.5 / 1.7 ----------
for g in (diag(1, 1, 1, 0), conformal(3), [[2, 1, 0], [1, 3, 1], [0, 1, -1]]):
    A = Alg(g)
    ok(f"1.5 right(1) = pseudoscalar (n={A.n})", A.right(A.basis(0)) == A.basis(A.full()))
order = [c for c in combinations(range(1, 5), 2)]
ok("1.7 grade-2 lex order e12,e13,e14,e23,e24,e34", ["e" + "".join(map(str, c)) for c in order] == ["e12", "e13", "e14", "e23", "e24", "e34"])

# ---------- changes of basis for 1.8 ----------
changes = []
# (a) conformal(3) -> R(4,1,0), the 1.4 change (self-inverse Gram matrices)
C3 = Alg(conformal(3)); R41 = Alg(diag(1, 1, 1, 1, -1))
P = flint.fmpq_mat(5, 5)
for i in range(3): P[i, i] = 1
P[3, 3] = Q(-1, 2); P[3, 4] = Q(1, 2); P[4, 3] = 1; P[4, 4] = 1
changes.append(("conformal(3)->R(4,1,0)", C3, R41, P, True))
# (b) generic: G' dense, random P, G = P G' P^T (condition fails)
Gp = [[2, 1, 0, 0], [1, 3, 1, 0], [0, 1, -1, 1], [0, 0, 1, 1]]
Pb = mat([[1, 2, 0, 0], [0, 1, 1, 0], [1, 0, 1, 3], [0, 0, 1, 2]])
Gb = Pb * mat(Gp) * Pb.transpose()
changes.append(("dense4 random P", Alg([[Gb[i, j] for j in range(4)] for i in range(4)]), Alg(Gp), Pb, False))
# (c) degenerate rigid(3) scaling e4 by 2: condition holds with d = 2
Pc = mat(diag(1, 1, 1, 2))
changes.append(("rigid(3) e4 -> 2e4", Alg(diag(1, 1, 1, 0)), Alg(diag(1, 1, 1, 0)), Pc, True))
# (d) Witt 2+2 -> R(2,2,0): e1=(f1+f3)/2 etc. build: pair (e1,e2) null with e1.e2=1
Pd = mat([[Q(1, 2), 0, Q(1, 2), 0], [1, 0, -1, 0], [0, Q(1, 2), 0, Q(1, 2)], [0, 1, 0, -1]])
W = [[0, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 0, 1, 0]]
R22 = diag(1, 1, -1, -1)
assert Pd * mat(R22) * Pd.transpose() == mat(W)
changes.append(("Witt -> R(2,2,0)", Alg(W), Alg(R22), Pd, True))
# (e) rigid(3) e4 -> e4 + e1 (degenerate; condition?)
Pe = mat([[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]); Pe[3, 0] = 0
# (e) rotation-like orthogonal P on R(3,0,0) with det -1 (reflection)
Pf = mat([[0, 1, 0], [1, 0, 0], [0, 0, 1]])
changes.append(("R(3,0,0) swap e1,e2 (orthogonal, d=-1)", Alg(diag(1, 1, 1)), Alg(diag(1, 1, 1)), Pf, True))

for (nm, A, Ap, P, cond) in changes:
    n = A.n
    d = det(P)
    assert P * mat(Ap.g) * P.transpose() == mat(A.g)
    phi = change(A, Ap, P)
    Pinv = P.inv()
    condition = Pinv.transpose() * mat(Ap.g) * Pinv == mat(A.g)
    ok(f"1.8 [{nm}] condition P^-T G' P^-1 = G is {cond}", condition == cond)
    a, b, x = rmv(A), rmv(A), rmv(A)
    # 1.8c antiwedge
    ok(f"1.8c [{nm}] phi(a v b) = phi(a) v phi(b) / d", phi(A.antiwedge(a, b)) == Ap.scale(1 / d, Ap.antiwedge(phi(a), phi(b))))
    # 1.8d bulk dual
    ok(f"1.8d [{nm}] phi(u*) = d phi(u)*", phi(A.bdual(a)) == Ap.scale(d, Ap.bdual(phi(a))))
    # 1.8e antiproduct, antidot, weight duals, compose (1/d), transform/project/antiproject (1/d^2)
    agp_ok = phi(A.agp(a, b)) == Ap.scale(1 / d, Ap.agp(phi(a), phi(b)))
    ok(f"1.8e [{nm}] antiproduct preserved with 1/d iff condition", agp_ok == cond)
    ad = A.antidot(a, b); adp = Ap.antidot(phi(a), phi(b))
    # phi(a o b) = (a o b) d 1'; the reading "phi(a o b) = (1/d) phi(a) o phi(b)" means ad*d == adp/d
    ok(f"1.8e [{nm}] antidot: phi(a o b) = (1/d) phi(a) o phi(b) iff condition", (ad * d == adp / d) == cond)
    print(f"   antidot a o b = {ad}, phi(a) o phi(b) = {adp}")
    wd = phi(A.wdual(a)) == Ap.scale(1 / d, Ap.wdual(phi(a)))
    ok(f"1.8e [{nm}] weight dual with 1/d iff condition", wd == cond)
    # a versor: even, a product of two hyperplanes in the antiproduct; and an odd one
    h1 = rmv(A, grades={n - 1}); h2 = rmv(A, grades={n - 1})
    Veven = A.agp(h1, h2); Vodd = h1
    for V, lab in ((Veven, "even"), (Vodd, "odd")):
        tr = phi(A.transform(x, V)) == Ap.scale(1 / d ** 2, Ap.transform(phi(x), phi(V)))
        ok(f"1.8e [{nm}] transform ({lab}) with 1/d^2 iff condition", tr == cond)
    cp = phi(A.compose(Veven, Vodd)) == Ap.scale(1 / d, Ap.compose(phi(Veven), phi(Vodd)))
    ok(f"1.8e [{nm}] compose with 1/d iff condition", cp == cond)
    pr = phi(A.project(x, h1)) == Ap.scale(1 / d ** 2, Ap.project(phi(x), phi(h1)))
    ap = phi(A.antiproject(x, h1)) == Ap.scale(1 / d ** 2, Ap.antiproject(phi(x), phi(h1)))
    ok(f"1.8e [{nm}] project with 1/d^2 iff condition", pr == cond)
    ok(f"1.8e [{nm}] antiproject with 1/d^2 iff condition", ap == cond)
    # 1.8f
    if cond and mat(A.g).det() != 0:
        ok(f"1.8f [{nm}] nondegenerate + condition => d = +-1", d in (1, -1))
    print("   d =", d)

# 1.8f by hand: det(P^-T G' P^-1) = det G'/d^2 and det G = d^2 det G' => d^4 = 1
# 1.8i
e4 = C3.basis(8)
ok("1.8i conformal(3): bulk(e4) = -e5", C3.bulk(e4) == C3.scale(-1, C3.basis(16)))
phi = change(C3, R41, changes[0][3])
Pinv = changes[0][3].inv()
phiinv = change(R41, C3, Pinv)
ok("1.8i via R(4,1,0): bulk(e4) = -1/2 e5", phiinv(R41.bulk(phi(e4))) == C3.scale(Q(-1, 2), C3.basis(16)))

# ---------- section 2 ----------
for g in (diag(1, 1, 1, 0), diag(1, -1, 1, 1, -1), conformal(3)):
    A = Alg(g)
    # 2.1a: reverse via matrices: reversed product of generators for diagonal forms; general: rev is
    # the anti-automorphism fixing vectors, check rev(ab) = rev(b) rev(a) and rev(v) = v.
    a, b = rmv(A), rmv(A)
    ok(f"2.1a n={A.n} rev is anti-automorphism", A.rev(A.gp(a, b)) == A.gp(A.rev(b), A.rev(a)))
    # direct: for an orthogonal basis the blade equals the product; reverse the product of the matrices
    if all(A.g[i][j] == 0 for i in range(A.n) for j in range(A.n) if i != j):
        for mask in range(A.N):
            idx = [i for i in range(A.n) if mask >> i & 1]
            pr = A.Id
            for i in reversed(idx): pr = pr * A.e[i]
            k = len(idx)
            assert pr == (-1) ** (k * (k - 1) // 2) * A.blade[mask]
        ok(f"2.1a n={A.n} reversed generator product = (-1)^(k(k-1)/2) blade", True)
    # 2.1b antireverse = left o rev o right
    ok(f"2.1b n={A.n} antireverse = left(rev(right x))", A.antirev(a) == A.left(A.rev(A.right(a))))
    # 2.1c grade involution = automorphism with v -> -v: check via matrices on products
    ok(f"2.1c n={A.n} grade involution automorphism", A.ginv(A.gp(a, b)) == A.gp(A.ginv(a), A.ginv(b)))
    # 2.1e
    ok(f"2.1e n={A.n} aginv = (-1)^n ginv", A.aginv(a) == A.scale((-1) ** A.n, A.ginv(a)))
    ok(f"2.1e n={A.n} right(aginv x) = ginv(right x)", A.right(A.aginv(a)) == A.ginv(A.right(a)))

# ---------- section 3 ----------
R3 = Alg(diag(1, 1, 1, 0))
names = {}
for m in range(16):
    names[R3.name(m)] = R3.show(R3.right(R3.basis(m)))
print("3.1 rigid(3) right complements:", names)
for g in (diag(1, 1, 1, 0), conformal(3), diag(1, 1, 1), [[2, 1, 0], [1, 3, 1], [0, 1, -1]]):
    A = Alg(g)
    allok = True
    for m in range(A.N):
        u = A.basis(m)
        k = A.grade(m)
        allok &= A.wedge(u, A.right(u)) == A.basis(A.full())
        allok &= A.wedge(A.left(u), u) == A.basis(A.full())
        allok &= A.left(u) == A.scale((-1) ** (k * (A.n - k)), A.right(u))
        allok &= A.left(A.right(u)) == u
    ok(f"3.1a/3.1b n={A.n}", allok)

forms = {
    "rigid3": diag(1, 1, 1, 0), "conformal3": conformal(3), "R(2,0,2)": diag(1, 1, 0, 0),
    "dense3": [[2, 1, 0], [1, 3, 1], [0, 1, -1]],
    "degen-nondiag4": [[1, 1, 0, 0], [1, 1, 0, 0], [0, 0, 2, 1], [0, 0, 1, 1]],
    "witt": W,
}
for nm, g in forms.items():
    A = Alg(g)
    # 3.2a diagonal factors
    if all(g[i][j] == 0 for i in range(A.n) for j in range(A.n) if i != j):
        okk = True
        for m in range(A.N):
            fb = 1; fw = 1
            for i in range(A.n):
                if m >> i & 1: fb *= g[i][i]
                else: fw *= g[i][i]
            okk &= A.bulk(A.basis(m)) == A.basis(m, fb) and A.weight(A.basis(m)) == A.basis(m, fw)
        ok(f"3.2a {nm} diagonal bulk/weight factors", okk)
    # 3.2b: bulk via minors det g[A,B] equals outermorphism (my G); weight via cofactor form
    okk = True
    for m in range(A.N):
        rows = [i for i in range(A.n) if m >> i & 1]
        for m2 in range(A.N):
            cols = [i for i in range(A.n) if m2 >> i & 1]
            if len(cols) != len(rows): continue
            if rows:
                sub = flint.fmpq_mat([[A.g[i][j] for j in cols] for i in rows]).det()
            else:
                sub = Q(1)
            okk &= A.G(A.basis(m))[m2] == sub
    ok(f"3.2b {nm} bulk = minors of g", okk)
    # 3.3
    okk = True
    for k in range(A.n + 1):
        a, b = rmv(A, grades={k}), rmv(A, grades={k})
        okk &= A.wedge(a, A.bdual(b)) == A.basis(A.full(), A.dot(a, b))
    ok(f"3.3 {nm} a ^ b* = (a.b) 1", okk)
    # 3.4
    detg = flint.fmpq_mat([[Q(x) for x in r] for r in g]).det()
    okk = True
    for m in range(A.N):
        k = A.grade(m)
        dd = A.bdual(A.bdual(A.basis(m)))
        if detg == 0: okk &= dd == A.zero()
        else: okk &= dd == A.basis(m, (-1) ** (k * (A.n - k)) * detg)
    ok(f"3.4 {nm} u** = {'0' if detg == 0 else '(-1)^(k(n-k)) det g u'}", okk)
    # 4.1c wedge equals grade part of the matrix product (non-diagonal forms included)
    okk = True
    for ka in range(A.n + 1):
        for kb in range(A.n + 1 - ka):
            a, b = rmv(A, grades={ka}), rmv(A, grades={kb})
            okk &= A.wedge(a, b) == A.gradepart(A.gp(a, b), ka + kb)
    ok(f"4.1c {nm} wedge = grade (ka+kb) part of the matrix product", okk)
    # 4.2b De Morgan, associativity of antiproducts
    a, b, c = rmv(A), rmv(A), rmv(A)
    ok(f"4.2b {nm} right(ab) = right a (anti) right b", A.right(A.gp(a, b)) == A.agp(A.right(a), A.right(b)))
    ok(f"4.2b {nm} left(ab) = left a (anti) left b", A.left(A.gp(a, b)) == A.agp(A.left(a), A.left(b)))
    ok(f"4.2b {nm} right(a^b) = right a v right b", A.right(A.wedge(a, b)) == A.antiwedge(A.right(a), A.right(b)))
    ok(f"4.2b {nm} left(a^b) = left a v left b", A.left(A.wedge(a, b)) == A.antiwedge(A.left(a), A.left(b)))
    ok(f"4.2b {nm} antiproduct associative", A.agp(A.agp(a, b), c) == A.agp(a, A.agp(b, c)))
    ok(f"4.2b {nm} antiwedge associative", A.antiwedge(A.antiwedge(a, b), c) == A.antiwedge(a, A.antiwedge(b, c)))
    # 4.2c
    ok(f"4.2c {nm} ginv(a anti b) = (-1)^n ginv a anti ginv b", A.ginv(A.agp(a, b)) == A.scale((-1) ** A.n, A.agp(A.ginv(a), A.ginv(b))))
    # 4.4a outermorphism with lambda^(k-1), and 4.4b mirror vectors times lambda, 4.4c compose order
    n = A.n
    h1, h2, h3 = (rmv(A, grades={n - 1}) for _ in range(3))
    for V, lab in ((h1, "odd"), (A.agp(h1, h2), "even"), (A.agp(A.agp(h1, h2), h3), "odd3")):
        lam_mv = A.agp(V, A.antirev(V))
        lam = lam_mv[A.full()]
        if lam == 0 or any(c != 0 for m, c in enumerate(lam_mv) if m != A.full()):
            print(f"   skip {nm} {lab}: V anti rev V = {A.show(lam_mv)}")
            continue
        vs = [rmv(A, grades={1}) for _ in range(n)]
        okk = True
        for k in range(1, n + 1):
            blade = A.basis(0)
            img = A.basis(0)
            for v in vs[:k]:
                blade = A.wedge(blade, v)
                img = A.wedge(img, A.transform(v, V))
            okk &= A.scale(lam ** (k - 1), A.transform(blade, V)) == img
        ok(f"4.4a {nm} {lab}: lambda^(k-1) T(a1^..^ak) = T(a1)^..^T(ak), lambda={lam}", okk)
        # single-sign odd form for contrast
        if lab == "odd":
            v = vs[0]
            wrong = A.agp(A.agp(V, A.aginv(v)), A.antirev(V))
            print(f"   odd: +V x^ V~ on a vector gives {'the same' if wrong == A.transform(v, V) else 'the negation' if wrong == A.scale(-1, A.transform(v, V)) else 'other'}")
            # 4.4b: vectors of the mirror: v with v ^ V = 0
            # find mirror vectors: solve v ^ V = 0 over a basis
            M = flint.fmpq_mat([[A.wedge(A.basis(1 << i), V)[A.full()] for i in range(n)]])
            # null space of 1 x n row
            row = [M[0, i] for i in range(n)]
            piv = next(i for i in range(n) if row[i] != 0)
            okk = True
            for j in range(n):
                if j == piv: continue
                v = A.zero(); v[1 << j] = Q(1); v[1 << piv] = -row[j] / row[piv]
                assert A.wedge(v, V) == A.zero()
                okk &= A.transform(v, V) == A.scale(lam, v)
            ok(f"4.4b {nm}: mirror vectors map to lambda v (lambda={lam})", okk)
    # 4.4c compose order for each parity pair
    x = rmv(A)
    vers = {"odd": h1, "even": A.agp(h2, h3)}
    okk = True
    for p1, Q1 in vers.items():
        for p2, Q2 in vers.items():
            okk &= A.transform(x, A.compose(Q2, Q1)) == A.transform(A.transform(x, Q1), Q2)
    ok(f"4.4c {nm} transform(x, compose(Q2,Q1)) = transform(transform(x,Q1),Q2), all parities", okk)

print("SUMMARY", sum(r for _, r in results), "/", len(results))
