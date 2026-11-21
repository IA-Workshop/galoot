"""Exact check of how Galoot's transform relates to the classical sandwich.

A small Clifford algebra over the integers for diagonal forms R(p, q, r),
written from the definitions of the conventions specification (sections 1 to
4) and sharing no code with the library or with
test/reference/conventions_check.py. Blades are bitmasks, bit i-1 for e_i;
a multivector is a dict from mask to integer.

Each side of every identity below is linear in x and a quadratic form in the
versor V, restricted to V of one parity. Two such forms agree for every V and
x exactly when their symmetrized coefficients agree: for each pair of basis
blades a and c of one parity and each basis blade b, the (a, c) term plus the
(c, a) term of one side equals that of the other. So a check that passes on
every such triple proves the identity for every multivector of that algebra,
and a check that fails on one triple disproves it. The script prints one line
per identity and algebra and exits nonzero if an identity claimed to hold
fails, or if one claimed to fail holds.

Run: python3 -I transform_sandwich_check.py
"""

import sys


class Algebra:
    def __init__(self, squares):
        self.squares = tuple(squares)
        self.n = len(squares)
        self.full = (1 << self.n) - 1
        self.blades = range(1 << self.n)

    # -- blades ------------------------------------------------------------
    def grade(self, a):
        return bin(a).count("1")

    def antigrade(self, a):
        return self.n - self.grade(a)

    def reorder_sign(self, a, b):
        """Sign of sorting the generators of e_a followed by those of e_b."""
        swaps = 0
        for i in range(self.n):
            if a >> i & 1:
                swaps += bin(b & ((1 << i) - 1)).count("1")
        return -1 if swaps & 1 else 1

    def blade_product(self, a, b):
        """e_a times e_b in the geometric product: (coefficient, mask)."""
        c = self.reorder_sign(a, b)
        shared = a & b
        for i in range(self.n):
            if shared >> i & 1:
                c *= self.squares[i]
        return c, a ^ b

    # -- multivectors ------------------------------------------------------
    def e(self, mask, c=1):
        return {mask: c}

    @staticmethod
    def clean(x):
        return {m: c for m, c in x.items() if c != 0}

    def add(self, x, y):
        out = dict(x)
        for m, c in y.items():
            out[m] = out.get(m, 0) + c
        return self.clean(out)

    def scale(self, s, x):
        return self.clean({m: s * c for m, c in x.items()})

    def gp(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                c, m = self.blade_product(a, b)
                if c:
                    out[m] = out.get(m, 0) + ca * cb * c
        return self.clean(out)

    def wedge(self, x, y):
        out = {}
        for a, ca in x.items():
            for b, cb in y.items():
                if a & b:
                    continue
                m = a | b
                out[m] = out.get(m, 0) + ca * cb * self.reorder_sign(a, b)
        return self.clean(out)

    def by_sign(self, x, sign):
        return self.clean({m: sign(m) * c for m, c in x.items()})

    def reverse(self, x):
        return self.by_sign(
            x, lambda m: -1 if (self.grade(m) * (self.grade(m) - 1) // 2) & 1 else 1
        )

    def antireverse(self, x):
        def sign(m):
            k = self.antigrade(m)
            return -1 if (k * (k - 1) // 2) & 1 else 1

        return self.by_sign(x, sign)

    def grade_involution(self, x):
        return self.by_sign(x, lambda m: -1 if self.grade(m) & 1 else 1)

    def antigrade_involution(self, x):
        return self.by_sign(x, lambda m: -1 if self.antigrade(m) & 1 else 1)

    def right(self, x):
        """u ^ right(u) = antiscalar on each basis blade."""
        return self.clean(
            {
                self.full ^ m: c * self.reorder_sign(m, self.full ^ m)
                for m, c in x.items()
            }
        )

    def left(self, x):
        """left(u) ^ u = antiscalar on each basis blade."""
        return self.clean(
            {
                self.full ^ m: c * self.reorder_sign(self.full ^ m, m)
                for m, c in x.items()
            }
        )

    def metric(self, x):
        """The metric exomorphism of a diagonal form."""

        def factor(m):
            f = 1
            for i in range(self.n):
                if m >> i & 1:
                    f *= self.squares[i]
            return f

        return self.clean({m: factor(m) * c for m, c in x.items()})

    def bulk_dual(self, x):
        return self.right(self.metric(x))

    def anti(self, x, y):
        """The geometric antiproduct, by its definition."""
        return self.left(self.gp(self.right(x), self.right(y)))

    # -- the two actions ---------------------------------------------------
    def galoot_term(self, a, x, c):
        """The (a, c) term of transform(x, V), V = sum of V_a e_a.

        a and c are basis blades of one antigrade parity; the transform of
        section 4.4 is the sum over a and c of V_a V_c times this.
        """
        ea, ec = self.e(a), self.e(c)
        if self.antigrade(a) % 2 == 0:
            return self.anti(self.anti(ea, x), self.antireverse(ec))
        inner = self.anti(
            self.anti(ea, self.antigrade_involution(x)), self.antireverse(ec)
        )
        return self.scale(-1, inner)

    def classical_term(self, a, x, c):
        """The (a, c) term of the classical action of V on x.

        V x reverse(V) for V of even grade, V grade_involution(x) reverse(V)
        for V of odd grade: on vectors, the twisted adjoint action times
        V reverse(V).
        """
        ea, ec = self.e(a), self.e(c)
        if self.grade(a) % 2 == 1:
            x = self.grade_involution(x)
        return self.gp(self.gp(ea, x), self.reverse(ec))


def name(squares):
    p = sum(1 for s in squares if s == 1)
    q = sum(1 for s in squares if s == -1)
    r = sum(1 for s in squares if s == 0)
    return f"R({p},{q},{r})"


failures = []


def report(label, squares, holds, expected=True, detail=""):
    status = "holds" if holds else "FAILS"
    note = "" if holds == expected else "   <-- UNEXPECTED"
    print(f"{label:<58} {name(squares):<10} {status}{note} {detail}")
    if holds != expected:
        failures.append((label, squares))


def same_parity_pairs(alg, parity_of):
    for a in alg.blades:
        for c in alg.blades:
            if parity_of(a) % 2 == parity_of(c) % 2:
                yield a, c


def sym(alg, term, a, x, c):
    """The symmetrized (a, c) coefficient of a quadratic form in V."""
    if a == c:
        return term(a, x, c)
    return alg.add(term(a, x, c), term(c, x, a))


def dual_term(alg, dual, a, x, c):
    """The (a, c) term of transform(x, dual(V)) for V = sum of V_a e_a."""
    got = {}
    for ma, ca in dual(alg, alg.e(a)).items():
        for mc, cc in dual(alg, alg.e(c)).items():
            got = alg.add(got, alg.scale(ca * cc, alg.galoot_term(ma, x, mc)))
    return got


def check_A(squares):
    """transform(x, V) = reverse(V) x' V, x' = grade involution of x when V
    has even grade and x when V has odd grade (Euclidean forms)."""
    alg = Algebra(squares)
    ok = True
    for a, c in same_parity_pairs(alg, alg.antigrade):
        for b in alg.blades:
            x = alg.e(b)

            def want(a2, x2, c2):
                xp = alg.grade_involution(x2) if alg.grade(a2) % 2 == 0 else x2
                return alg.gp(alg.gp(alg.reverse(alg.e(a2)), xp), alg.e(c2))

            ok &= sym(alg, alg.galoot_term, a, x, c) == sym(alg, want, a, x, c)
    return ok


def check_B(squares, dual):
    """transform(x, dual(V)) = the classical action of V on x."""
    alg = Algebra(squares)
    ok = True
    for a, c in same_parity_pairs(alg, alg.grade):
        for b in alg.blades:
            x = alg.e(b)
            got = sym(alg, lambda p, y, q: dual_term(alg, dual, p, y, q), a, x, c)
            ok &= got == sym(alg, alg.classical_term, a, x, c)
    return ok


def check_naive(squares):
    """transform(x, V) = the classical action of V on x (claimed false)."""
    alg = Algebra(squares)
    for a, c in same_parity_pairs(alg, alg.grade):
        for b in alg.blades:
            x = alg.e(b)
            if sym(alg, alg.galoot_term, a, x, c) != sym(
                alg, alg.classical_term, a, x, c
            ):
                return False
    return True


def check_lemmas(squares):
    """a anti b = b I^-1 a and antireverse(V) = (-1)^q I reverse(V) I."""
    alg = Algebra(squares)
    q = sum(1 for s in squares if s == -1)
    one = alg.e(alg.full)
    ii = alg.gp(one, one)
    assert set(ii) == {0} and ii[0] in (1, -1)
    inv = alg.scale(ii[0], one)  # I^-1 = I / (I I)
    ok = True
    for a in alg.blades:
        ea = alg.e(a)
        want = alg.scale((-1) ** q, alg.gp(alg.gp(one, alg.reverse(ea)), one))
        ok &= alg.antireverse(ea) == want
        for b in alg.blades:
            eb = alg.e(b)
            ok &= alg.anti(ea, eb) == alg.gp(alg.gp(eb, inv), ea)
    return ok


def check_C(squares):
    """right(transform(x, V)) = W X reverse(W) for even V and
    -W grade_involution(X) reverse(W) for odd V, with W = right(V) and
    X = right(x), in the geometric product (any diagonal form)."""
    alg = Algebra(squares)
    ok = True
    for a, c in same_parity_pairs(alg, alg.antigrade):
        for b in alg.blades:
            x = alg.e(b)

            def want(a2, x2, c2):
                wa, wc = alg.right(alg.e(a2)), alg.right(alg.e(c2))
                X = alg.right(x2)
                if alg.antigrade(a2) % 2 == 0:
                    return alg.gp(alg.gp(wa, X), alg.reverse(wc))
                return alg.scale(
                    -1,
                    alg.gp(alg.gp(wa, alg.grade_involution(X)), alg.reverse(wc)),
                )

            got = alg.right(sym(alg, alg.galoot_term, a, x, c))
            ok &= got == sym(alg, want, a, x, c)
    return ok


def check_D(d):
    """In rigid(d), for V on the blades of e1..ed only and any x of
    rigid(d): transform(x, right(V)) = the classical action of V on x."""
    squares = (1,) * d + (0,)
    alg = Algebra(squares)
    euclid = [m for m in alg.blades if not m >> d & 1]
    ok = True
    for a in euclid:
        for c in euclid:
            if alg.grade(a) % 2 != alg.grade(c) % 2:
                continue
            for b in alg.blades:
                x = alg.e(b)
                got = sym(
                    alg,
                    lambda p, y, q: dual_term(alg, Algebra.right, p, y, q),
                    a, x, c,
                )
                ok &= got == sym(alg, alg.classical_term, a, x, c)
    return ok


def full_transform(alg, x, v):
    """transform(x, V) for a multivector V of one antigrade parity."""
    parities = {alg.antigrade(m) % 2 for m in v}
    assert len(parities) == 1, "V holds blades of both parities"
    out = {}
    for a, ca in v.items():
        for c, cc in v.items():
            out = alg.add(out, alg.scale(ca * cc, alg.galoot_term(a, x, c)))
    return out


def classical(alg, x, v):
    parities = {alg.grade(m) % 2 for m in v}
    assert len(parities) == 1
    out = {}
    for a, ca in v.items():
        for c, cc in v.items():
            out = alg.add(out, alg.scale(ca * cc, alg.classical_term(a, x, c)))
    return out


def worked_values():
    """The numbers the design note quotes, each by both routes."""
    ok = True
    e3 = Algebra((1, 1, 1))
    E1, E2, E3, E12 = 0b001, 0b010, 0b100, 0b011

    def vec(alg, x, y, z):
        return alg.clean({E1: x, E2: y, E3: z})

    # r = b a for the mirrors a = (1, 0, 0) and b = (1, 1, 0): 1 - e12.
    r = e3.gp(vec(e3, 1, 1, 0), vec(e3, 1, 0, 0))
    ok &= r == {0: 1, E12: -1}
    ok &= classical(e3, vec(e3, 1, 0, 0), r) == vec(e3, 0, 2, 0)
    ok &= classical(e3, vec(e3, 0, 0, 1), r) == vec(e3, 0, 0, 2)
    ok &= full_transform(e3, vec(e3, 1, 0, 0), r) == vec(e3, 0, 2, 0)
    ok &= full_transform(e3, vec(e3, 0, 0, 1), r) == vec(e3, 0, 0, -2)
    ok &= e3.right(r) == {0b111: 1, E3: -1}
    for v in (vec(e3, 1, 0, 0), vec(e3, 0, 0, 1), vec(e3, 2, -3, 5)):
        ok &= full_transform(e3, v, e3.right(r)) == classical(e3, v, r)

    # s = b a for the mirrors a = (1, 0, 0) and b = (2, 1, 0): 2 - e12,
    # a turn with cosine 3/5 and sine 4/5, scaled by s reverse(s) = 5.
    s = e3.gp(vec(e3, 2, 1, 0), vec(e3, 1, 0, 0))
    ok &= s == {0: 2, E12: -1}
    ok &= classical(e3, vec(e3, 1, 0, 0), s) == vec(e3, 3, 4, 0)
    ok &= classical(e3, vec(e3, 0, 1, 0), s) == vec(e3, -4, 3, 0)
    ok &= classical(e3, vec(e3, 0, 0, 1), s) == vec(e3, 0, 0, 5)
    ok &= full_transform(e3, vec(e3, 1, 0, 0), s) == vec(e3, -3, 4, 0)
    ok &= full_transform(e3, vec(e3, 0, 0, 1), s) == vec(e3, 0, 0, -5)
    ok &= full_transform(e3, vec(e3, 1, 0, 0), e3.right(s)) == vec(e3, 3, 4, 0)

    # One mirror: the classical reflection -a v a against transform(v, a)
    # and transform(v, right(a)), for a = (1, 0, 0).
    a = vec(e3, 1, 0, 0)
    v = vec(e3, 1, 2, 3)
    ok &= classical(e3, v, a) == vec(e3, -1, 2, 3)
    ok &= full_transform(e3, v, a) == vec(e3, 1, -2, -3)
    ok &= full_transform(e3, v, e3.right(a)) == vec(e3, -1, 2, 3)

    # The same rotor carried into rigid(3) by its right complement there:
    # 1 -> e1234 and e12 -> e34 = -e43, so right(1 - e12) = e1234 + e43,
    # the motor (0, 0, 1, 1, 0, 0, 0, 0) on e41 e42 e43 e1234 e23 e31 e12 1.
    pga = Algebra((1, 1, 1, 0))
    E4 = 0b1000
    motor = pga.right(r)
    ok &= motor == {0b1111: 1, 0b1100: -1}
    p = {E1: 1, E4: 1}
    ok &= full_transform(pga, p, motor) == {E2: 2, E4: 2}
    ok &= full_transform(pga, {E1: 1, E2: 2, E3: 3, E4: 1}, motor) == pga.clean(
        {E1: -4, E2: 2, E3: 6, E4: 2}
    )
    # The mirror a = e1 becomes the plane right(e1) = e234 = e423.
    mirror = pga.right({E1: 1})
    ok &= mirror == {0b1110: 1}
    ok &= full_transform(pga, {E1: 1, E2: 2, E3: 3, E4: 1}, mirror) == pga.clean(
        {E1: -1, E2: 2, E3: 3, E4: 1}
    )
    # Two mirrors compose to the motor: right(b) anti right(a) = right(b a).
    mb = pga.right({E1: 1, E2: 1})
    ok &= pga.anti(mb, mirror) == motor
    # In R(2,0,0) a quarter-turn rotor is the one coincidence for vectors.
    e2 = Algebra((1, 1))
    q = {0: 1, 0b11: -1}
    t = {0: 2, 0b11: -1}
    for b in (0b01, 0b10):
        ok &= full_transform(e2, {b: 1}, q) == classical(e2, {b: 1}, q)
    ok &= full_transform(e2, {0b01: 1}, t) != classical(e2, {0b01: 1}, t)
    return ok


def main():
    euclidean = [(1,) * n for n in range(1, 6)]
    negative_definite = [(-1, -1), (-1, -1, -1), (-1, -1, -1, -1)]
    indefinite = [(1, -1), (1, 1, -1), (1, -1, -1), (1, 1, 1, -1), (1, -1, -1, -1),
                  (1, 1, 1, 1, -1)]
    degenerate = [(1, 1, 0), (1, 1, 1, 0), (1, 0, 0), (1, 1, 1, -1, 0)]

    for sq in euclidean:
        report("A  transform(x,V) = rev(V) x' V", sq, check_A(sq))
    for sq in euclidean + negative_definite:
        report("B  transform(x,right(V)) = classical V on x", sq,
               check_B(sq, Algebra.right))
    for sq in indefinite:
        report("B  the same with the complement, indefinite form", sq,
               check_B(sq, Algebra.right), expected=False)
    for sq in euclidean + negative_definite + indefinite:
        report("B' transform(x,bulk_dual(V)) = classical V on x", sq,
               check_B(sq, Algebra.bulk_dual))
    for sq in euclidean + negative_definite + indefinite:
        report("N  transform(x,V) = classical V on x (naive)", sq,
               check_naive(sq), expected=False)
    for sq in euclidean + negative_definite + indefinite:
        report("L  a anti b = b I^-1 a; antireverse formula", sq, check_lemmas(sq))
    for sq in euclidean[:4] + indefinite[:4] + degenerate:
        report("C  right(transform) = sandwich of complements", sq, check_C(sq))
    for d in (1, 2, 3, 4):
        report("D  rigid(d): transform(x,right(V)) = classical V on x",
               (1,) * d + (0,), check_D(d))
    ok = worked_values()
    print(f"{'W  worked values of the design note':<58} {'':<10} "
          f"{'hold' if ok else 'FAIL'}")
    if not ok:
        failures.append(("worked values", ()))
    print("all checks as expected" if not failures else f"FAILURES: {failures}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
