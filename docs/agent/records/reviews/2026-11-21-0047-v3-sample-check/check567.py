import sys, random, math
sys.path.insert(0, sys.argv[1])
from mx import *

rnd = random.Random(11)
results = []
def ok(name, cond):
    results.append((name, bool(cond)))
    print(("PASS " if cond else "FAIL ") + name)

R = Alg(diag(1, 1, 1, 0))
n = 4
def m(*idx):  # mask of a canonical index list (1-based)
    s = 0
    for i in idx: s |= 1 << (i - 1)
    return s
def oriented(*idx):
    """the oriented blade e_{idx} as written (wedge in the written order)"""
    out = R.basis(0)
    for i in idx: out = R.wedge(out, R.basis(1 << (i - 1)))
    return out
E41, E42, E43 = oriented(4, 1), oriented(4, 2), oriented(4, 3)
E23, E31, E12 = oriented(2, 3), oriented(3, 1), oriented(1, 2)
E423, E431, E412, E321 = oriented(4, 2, 3), oriented(4, 3, 1), oriented(4, 1, 2), oriented(3, 2, 1)
ANTI = R.basis(15); ONE = R.basis(0)
def coef(x, ob):  # coefficient of x on oriented blade ob (a +-1 multiple of a canonical blade)
    mk = next(i for i, c in enumerate(ob) if c != 0)
    return x[mk] / ob[mk]
def lin(*pairs):
    return R.add(*[R.scale(c, b) for c, b in pairs])
def point(x, y, z, w=1): return R.vec([x, y, z, w])
def line(v, mm): return lin((v[0], E41), (v[1], E42), (v[2], E43), (mm[0], E23), (mm[1], E31), (mm[2], E12))
def plane(gx, gy, gz, gw): return lin((gx, E423), (gy, E431), (gz, E412), (gw, E321))
def cross(a, b): return [a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]]
def dot3(a, b): return sum(Q(x) * Q(y) for x, y in zip(a, b))
def ri(): return rnd.randint(-5, 5)

# 5.1: weight of a point is w e4 (3.2: the part with e4)
p = point(1, 2, 3, 5)
ok("5.1 weight(point) = w e4", R.weight(p) == R.basis(8, 5))

# 5.2
okk = True
for _ in range(20):
    P = [ri(), ri(), ri(), ri()]; Qp = [ri(), ri(), ri(), ri()]
    L = R.wedge(point(*P), point(*Qp))
    dirn = [coef(L, E) for E in (E41, E42, E43)]
    mom = [coef(L, E) for E in (E23, E31, E12)]
    okk &= dirn == [P[3] * Qp[i] - Qp[3] * P[i] for i in range(3)]
    okk &= mom == cross(P[:3], Qp[:3])
    okk &= L == line(dirn, mom)
ok("5.2 p^q: direction w_p q - w_q p on e41,e42,e43; moment p x q on e23,e31,e12", okk)

# 5.3a / 5.3b
okk = True
for _ in range(20):
    g = [ri() for _ in range(4)]; P = [ri() for _ in range(4)]
    val = R.antiwedge(point(*P), plane(*g))
    okk &= val == R.basis(0, sum(Q(a) * Q(b) for a, b in zip(g, P)))
ok("5.3a p v g = gx x + gy y + gz z + gw w (a scalar)", okk)
# counterclockwise seen from the normal's tip: points in z=0 plane, ccw seen from +z
pl = R.wedge(R.wedge(point(0, 0, 0), point(1, 0, 0)), point(0, 1, 0))
ok("5.3b p^q^r ccw from +z has normal +z: " + R.show(pl), coef(pl, E412) > 0 and coef(pl, E423) == 0 and coef(pl, E431) == 0)
# generic: normal = (q-p) x (r-p)
okk = True
for _ in range(20):
    A_, B_, C_ = ([ri(), ri(), ri()] for _ in range(3))
    pl = R.wedge(R.wedge(point(*A_), point(*B_)), point(*C_))
    nrm = cross([B_[i] - A_[i] for i in range(3)], [C_[i] - A_[i] for i in range(3)])
    okk &= [coef(pl, E) for E in (E423, E431, E412)] == nrm
ok("5.3b generic: normal of p^q^r = (q-p) x (r-p)", okk)

# 5.5a translator; 5.5b rotor about the z axis and a random line
t = [Q(1), Q(-2), Q(3)]
T = lin((t[0], E23), (t[1], E31), (t[2], E12), (1, ANTI))
pt = R.transform(point(4, 5, 6), T)
ok("5.5a translator moves by 2t: " + R.show(pt), pt == point(4 + 2, 5 - 4, 6 + 6))
# rotor about z axis by 2phi with exact cos/sin: phi with cos=3/5, sin=4/5 -> 2phi: cos = -7/25, sin = 24/25
lz = line([0, 0, 1], [0, 0, 0])
c, s = Q(3, 5), Q(4, 5)
Rz = R.add(R.scale(s, lz), R.scale(c, ANTI))
pr = R.transform(point(1, 0, 0), Rz)
ok("5.5b z-axis rotor takes (1,0,0) to (cos2phi, sin2phi, 0) = (-7/25, 24/25, 0): " + R.show(pr), pr == point(Q(-7, 25), Q(24, 25), 0))
# 5.7 rotation(l, theta): (l/|v|) sin(theta/2) + 1 cos(theta/2), with |v| rational; line through (1,2,0) along (0,0,2)
v = [0, 0, 2]; through = [1, 2, 0]
l = R.wedge(point(*through), point(through[0] + v[0], through[1] + v[1], through[2] + v[2]))
lv = [coef(l, E) for E in (E41, E42, E43)]
norm = Q(2)
ch, sh = Q(3, 5), Q(4, 5)  # cos(theta/2), sin(theta/2); theta with cos theta = -7/25, sin theta = 24/25
Rot = R.add(R.scale(sh / norm, l), R.scale(ch, ANTI))
pr = R.transform(point(2, 2, 7), Rot)   # offset (1,0) from the axis -> rotated by theta about the axis
ok("5.7 rotation by theta about l (|v| = 2) turns (offset 1,0) to (cos theta, sin theta): " + R.show(pr), pr == point(1 + Q(-7, 25), 2 + Q(24, 25), 7))
Tr = lin((Q(1, 2), E23), (Q(-1), E31), (Q(2), E12), (1, ANTI))  # translation(1, -2, 4)
ok("5.7 translation(1,-2,4) moves by (1,-2,4)", R.transform(point(0, 0, 0), Tr) == point(1, -2, 4))

# 5.6 flector: -F anti x^ anti F~; the plane z = 0 reflects (1,2,3,1) -> (1,2,-3,1), weight +1
F = E412
img = R.scale(-1, R.agp(R.agp(F, R.ginv(point(1, 2, 3))), R.antirev(F)))
ok("5.6 reflection in e412 (z=0): (1,2,3,1) -> " + R.show(img), img == point(1, 2, -3, 1))
single = R.scale(-1, R.agp(R.agp(F, point(1, 2, 3)), R.antirev(F)))
ok("5.6 single-sign form gives weight -1: " + R.show(single), single == R.scale(-1, point(1, 2, -3, 1)))
# lines and planes spanned by reflected points in order, for a general flector (plane then a motor)
Fl = R.agp(T, plane(1, 2, -1, 3))  # odd versor
def refl(x): return R.scale(-1, R.agp(R.agp(Fl, R.ginv(x)), R.antirev(Fl)))
lam = R.agp(Fl, R.antirev(Fl))[15]
p1, p2, p3 = point(1, 0, 2), point(0, 3, 1), point(2, 2, -1)
ok("5.6 reflected line = span of reflected points (lambda^1)", R.scale(lam, refl(R.wedge(p1, p2))) == R.wedge(refl(p1), refl(p2)))
ok("5.6 reflected plane = span of reflected points (lambda^2)", R.scale(lam * lam, refl(R.wedge(R.wedge(p1, p2), p3))) == R.wedge(R.wedge(refl(p1), refl(p2)), refl(p3)))
ok("5.6 reflected point keeps positive weight (lambda>0)", lam > 0 and refl(p1)[8] > 0)
ok("5.6 agrees with 4.4 transform for this odd V", refl(p1) == R.transform(p1, Fl))

# 5.8 closure
mot = [E41, E42, E43, ANTI, E23, E31, E12, ONE]
flc = [R.basis(1), R.basis(2), R.basis(4), R.basis(8), E423, E431, E412, E321]
masks_even = {m for m in range(16) if bin(m).count("1") % 2 == 0}
masks_odd = set(range(16)) - masks_even
ok("5.8 motor blades = the 8 even-grade blades", {next(i for i, c in enumerate(b) if c) for b in mot} == masks_even)
ok("5.8 flector blades = the 8 odd-grade blades", {next(i for i, c in enumerate(b) if c) for b in flc} == masks_odd)
def rnd_of(bs): return R.add(*[R.scale(ri(), b) for b in bs])
def support(x): return {i for i, c in enumerate(x) if c != 0}
okk = True
for _ in range(10):
    M1, M2, F1, F2 = rnd_of(mot), rnd_of(mot), rnd_of(flc), rnd_of(flc)
    okk &= support(R.agp(M1, M2)) <= masks_even and support(R.agp(F1, F2)) <= masks_even
    okk &= support(R.agp(M1, F1)) <= masks_odd and support(R.agp(F1, M1)) <= masks_odd
ok("5.8 closure under the antiproduct", okk)

# G5 meet signs
okk_a = okk_b = okk_c = okk_d = okk_e = True
for _ in range(15):
    g = [ri() for _ in range(4)]; h = [ri() for _ in range(4)]; k = [ri() for _ in range(4)]
    G_, H_, K_ = plane(*g), plane(*h), plane(*k)
    L = R.antiwedge(G_, H_)
    okk_a &= L == line(cross(h[:3], g[:3]), [h[3] * g[i] - g[3] * h[i] for i in range(3)])
    Pt = R.antiwedge(L, K_)
    dt = flint.fmpq_mat([[Q(x) for x in g[:3]], [Q(x) for x in h[:3]], [Q(x) for x in k[:3]]]).det()
    okk_b &= Pt[8] == dt
    if dt != 0:
        xyz = [Pt[1] / dt, Pt[2] / dt, Pt[4] / dt]
        for pl_ in (g, h, k):
            okk_b &= dot3(pl_[:3], xyz) + pl_[3] == 0
    v = [ri(), ri(), ri()]; pp = [ri(), ri(), ri()]
    mm = cross(pp, [pp[i] + v[i] for i in range(3)])  # line through pp with direction v: moment pp x (pp+v) = pp x v
    l1 = line(v, mm)
    okk_c &= R.antiwedge(l1, G_)[8] == -dot3(v, g[:3])
    P4 = [[ri() for _ in range(4)] for _ in range(4)]
    vol = R.wedge(R.wedge(R.wedge(point(*P4[0]), point(*P4[1])), point(*P4[2])), point(*P4[3]))
    okk_d &= vol[15] == flint.fmpq_mat([[Q(x) for x in r] for r in P4]).det()
    v2 = [ri(), ri(), ri()]; q2 = [ri(), ri(), ri()]
    m2 = cross(q2, v2)
    l2 = line(v2, m2)
    s = R.antiwedge(l1, l2)
    okk_e &= s == R.basis(0, -(dot3(v, m2) + dot3(v2, mm)))
ok("G5a g v h: direction n_h x n_g, moment h_w n_g - g_w n_h", okk_a)
ok("G5b (g v h) v k: weight det[n_g; n_h; n_k], point solves the three planes", okk_b)
ok("G5c l v g weight = -(v . n_g)", okk_c)
ok("G5d p^q^r^s antiscalar = det of rows (x,y,z,w)", okk_d)
ok("G5e l1 v l2 = -(v1.m2 + v2.m1), a scalar", okk_e)

# ---------- section 6 ----------
C = Alg(conformal(3))
def cvec(x, y, z, a4, a5): return C.vec([x, y, z, a4, a5])
okk = True
for _ in range(10):
    p = [Q(ri()), Q(ri()), Q(ri())]; r2 = Q(ri())
    p2 = dot3(p, p)
    P = cvec(*p, 1, Q(1, 2) * (p2 + r2))
    okk &= C.dot(P, P) == -r2
    okk &= C.gp(P, P) == C.basis(0, -r2)  # matrix route: P P = P.P for a vector
ok("6.1a round point P.P = -r^2 (dot and matrix square)", okk)
# 6.1b: the sphere as the wedge of four points on it; its bulk dual is a multiple of sigma = p + e4 + 1/2(p^2 - r^2) e5
p = [Q(1), Q(2), Q(-1)]; r = Q(3)
pts = [[p[0] + r, p[1], p[2]], [p[0], p[1] + r, p[2]], [p[0], p[1], p[2] + r], [p[0] - r, p[1], p[2]]]
def rp(x): return cvec(*x, 1, Q(1, 2) * dot3(x, x))
S = C.wedge(C.wedge(C.wedge(rp(pts[0]), rp(pts[1])), rp(pts[2])), rp(pts[3]))
D = C.bdual(S)
sigma = cvec(*p, 1, Q(1, 2) * (dot3(p, p) - r * r))
ratio = D[8] / sigma[8]
ok(f"6.1b bulk dual of the sphere through 4 points is proportional to sigma (ratio {ratio})", D == C.scale(ratio, sigma))
print("   sphere S =", C.show(S))
# Is the round point's own bulk dual a sphere? round point of radius r at p:
RP = cvec(*p, 1, Q(1, 2) * (dot3(p, p) + r * r))
print("   bulk dual of the round point =", C.show(C.bdual(RP)))
# 6.2 check: flat point = round point ^ e5 equals rigid point (p + e4) ^ e5 with coefficients unchanged
e5 = C.basis(16)
okk = True
for _ in range(5):
    a = [Q(ri()) for _ in range(3)]; b = [Q(ri()) for _ in range(3)]; c3 = [Q(ri()) for _ in range(3)]
    okk &= C.wedge(rp(a), e5) == C.wedge(cvec(*a, 1, 0), e5)
    okk &= C.wedge(C.wedge(rp(a), rp(b)), e5) == C.wedge(C.wedge(cvec(*a, 1, 0), cvec(*b, 1, 0)), e5)
    okk &= C.wedge(C.wedge(C.wedge(rp(a), rp(b)), rp(c3)), e5) == C.wedge(C.wedge(C.wedge(cvec(*a, 1, 0), cvec(*b, 1, 0)), cvec(*c3, 1, 0)), e5)
ok("6.2 (probe) flats through round points = rigid objects (e4 for the projective generator) wedged with e5", okk)

print("SUMMARY", sum(r for _, r in results), "/", len(results))
