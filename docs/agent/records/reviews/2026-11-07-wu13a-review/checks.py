import sys, random
sys.path.insert(0, __import__("os").path.dirname(__import__("os").path.abspath(__file__)))
from ga import *

rng = random.Random(4242)

# rigid(3) hand values
r3 = Alg([1, 1, 1, 0])
I = r3.I
rotor = add(blade(r3, "43", 3), {I: Fr(4)})
p = vec(r3, [1, 0, 0, 1])
check("rigid3 rotor 3e43+4I -> (7,24,0,25)", eq(r3.transform(p, rotor), vec(r3, [7, 24, 0, 25])))
mirror = blade(r3, "412")
check("rigid3 mirror e412 (1,2,3,1)->(1,2,-3,1)", eq(r3.transform(vec(r3, [1, 2, 3, 1]), mirror), vec(r3, [1, 2, -3, 1])))
# single-sign form -F x F~ negates
single = scale(r3.agp(r3.agp(mirror, vec(r3, [1, 2, 3, 1])), r3.antireverse(mirror)), -1)
check("rigid3 single-sign form gives weight -1", single.get(8) == -1)

# header example: turn then move
turn = add(blade(r3, "43", Fr(6, 10)), {I: Fr(8, 10)})
move = add(add(add(blade(r3, "23", 1), blade(r3, "31", 2)), blade(r3, "12", -1)), {I: Fr(1)})
both = r3.agp(move, turn)
q = r3.transform(p, both)
check("header example (2.28, 4.96, -2, 1)", eq(q, vec(r3, [Fr(228, 100), Fr(496, 100), -2, 1])))
check("compose order", eq(q, r3.transform(r3.transform(p, turn), move)))
check("compose order swapped differs", not eq(q, r3.transform(r3.transform(p, move), turn)))
# translator from plan: e23 + 2 e31 - e12 + I moves (5,7,-1,1) to (7,11,-3,1)
check("translator", eq(r3.transform(vec(r3, [5, 7, -1, 1]), move), vec(r3, [7, 11, -3, 1])))

# rigid(2): mirror e23 sends (1,2,1) to (-1,2,1)
r2 = Alg([1, 1, 0])
check("rigid2 mirror e23", eq(r2.transform(vec(r2, [1, 2, 1]), blade(r2, "23")), vec(r2, [-1, 2, 1])))
# with the 1.9 sign (-V x^ V~) it would be the negation
old = scale(r2.agp(r2.agp(blade(r2, "23"), r2.ginv(vec(r2, [1, 2, 1]))), r2.antireverse(blade(r2, "23"))), -1)
check("rigid2 old sign negates", eq(old, vec(r2, [1, -2, -1])))

# R(4,0,0): V = 1 + I, x = 1 -> 2 + 2I
e4 = Alg([1, 1, 1, 1])
V = {0: Fr(1), e4.I: Fr(1)}
check("R(4,0,0) 1+I on 1 -> 2+2I", eq(e4.transform({0: Fr(1)}, V), {0: Fr(2), e4.I: Fr(2)}))
# R(6,0,0): V = 1 + I, V antiprod antirev(V) = 2 I but e1 -> grade 5
e6 = Alg([1] * 6)
V6 = {0: Fr(1), e6.I: Fr(1)}
check("R(6,0,0) V~V = 2I", eq(e6.agp(V6, e6.antireverse(V6)), {e6.I: Fr(2)}))
t = e6.transform({1: Fr(1)}, V6)
print("  R(6,0,0) T(e1) =", t)
check("R(6,0,0) T(e1) is not a vector", all(e6.gr(m) != 1 for m in t))


def lam(alg, v):
    w = alg.agp(v, alg.antireverse(v))
    if set(w) - {alg.I}:
        return None
    return w.get(alg.I, 0)


def wedge_all(alg, xs):
    r = {0: Fr(1)}
    for x in xs:
        r = alg.wedge(r, x)
    return r


forms = {
    "rigid2": [1, 1, 0], "R300": [1, 1, 1], "rigid3": [1, 1, 1, 0],
    "R220": [1, 1, -1, -1], "rigid4": [1, 1, 1, 1, 0], "R500": [1] * 5,
    "R410": [1, 1, 1, 1, -1], "R311": [1, 1, 1, -1, 0],
}
for name, sq in forms.items():
    a = Alg(sq)
    ok_om = ok_comp = ok_mirror = ok_lam = True
    ok_grade_parity_fails = False
    for trial in range(6):
        r = rng.randint(1, 3)
        hs = [rand_hyperplane(a, rng) for _ in range(r)]
        # skip null hyperplanes
        if any(lam(a, h) == 0 for h in hs):
            continue
        v = hs[0]
        for h in hs[1:]:
            v = a.agp(v, h)
        L = lam(a, v)
        if L is None or L == 0:
            ok_lam = False
            continue
        vs = [vec(a, [rng.randint(-4, 4) for _ in range(a.n)]) for _ in range(a.n)]
        for k in range(1, a.n + 1):
            lhs = scale(a.transform(wedge_all(a, vs[:k]), v), L ** (k - 1))
            rhs = wedge_all(a, [a.transform(u, v) for u in vs[:k]])
            ok_om &= eq(lhs, rhs)
        # composition with another random versor
        w = rand_hyperplane(a, rng)
        if lam(a, w):
            x = wedge_all(a, vs[:2])
            ok_comp &= eq(a.transform(x, a.agp(w, v)), a.transform(a.transform(x, v), w))
        # mirror: single hyperplane fixes vectors in it times lambda
        h = hs[0]
        lh = lam(a, h)
        # find a vector in the mirror: u with u ^ h = 0
        hv = [vec(a, [rng.randint(-5, 5) for _ in range(a.n)]) for _ in range(a.n - 1)]
        h = wedge_all(a, hv)
        lh = lam(a, h)
        if h and lh:
            u = {}
            for b in hv:
                u = add(u, scale(b, rng.randint(-3, 3)))
            assert a.wedge(u, h) == {}
            ok_mirror &= eq(a.transform(u, h), scale(u, lh))
            out = vec(a, [rng.randint(-4, 4) for _ in range(a.n)])
            if a.wedge(out, h):
                ok_mirror &= not eq(a.transform(out, h), scale(out, lh))
        # grade parity rule (wrong reading) should fail on odd n
    check(f"{name}: outermorphism lambda^(k-1)", ok_om)
    check(f"{name}: composition", ok_comp)
    check(f"{name}: mirror fixed times lambda", ok_mirror)
    check(f"{name}: V~V = lambda I", ok_lam)

print(sum(1 for _, c in results if c), "of", len(results), "passed")
