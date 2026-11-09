"""Recomputes the formulas read for research note 0026 section B, exactly,
with galgebra (symbolic where a source has a free parameter or a square root).

  B1  Lengyel, conformal wiki, Main Page: e4 = (e- - e+)/2, e5 = e- + e+, with
      e-^2 = -1, e+^2 = +1; the point formula; "ab = 1/2 makes e- ^ e+ and
      e4 ^ e5 equal"; the Metrics page's Gram matrix.
  B2  Colapinto, eqs. 2.39 to 2.42 and the caption of Table 2.6.
  B3  Hestenes, "Old Wine in New Bottles", eqs. 1 to 7, with e+ ^ e- against
      e ^ e* for every lambda, and the translation to the conformal(d) basis.
  B4  Doran, Hestenes, Sommen, Van Acker, eqs. 2.5, 2.7, 3.16, 3.34: the
      Witt basis and the anticommutation relations, and the scaled Witt form
      of the test list against them.

Each line printed is one check; the script exits 1 if any fails.

Run with the math-tools environment (galgebra 0.6.0, SymPy 1.14.0):
  .venv/bin/python -I docs/agent/records/spikes/wu12-part-a/readings_check.py
"""

import contextlib
import io
import sys

import sympy
from galgebra.ga import Ga

failures = []


def check(ok, what):
    print(("ok    " if ok else "FAIL  ") + what)
    if not ok:
        failures.append(what)


def build(names, g):
    with contextlib.redirect_stdout(io.StringIO()):
        ga = Ga(names, g=sympy.Matrix(g))
    return ga, list(ga.mv())


def zero(x):
    return all(sympy.simplify(c) == 0 for c in x.blade_coefs())


def same(x, y):
    return zero(x - y)


def scalar(x):
    return sympy.simplify(x.scalar())


def b1_lengyel():
    # R(4,1,0) written with e+ squaring to +1 and e- to -1, as the Main Page
    # names them; e1, e2, e3 Euclidean.
    ga, (e1, e2, e3, ep, em) = build("e1 e2 e3 ep em", sympy.diag(1, 1, 1, 1, -1))
    e4 = sympy.Rational(1, 2) * (em - ep)
    e5 = em + ep
    check(scalar(em * em) == -1 and scalar(ep * ep) == 1, "B1 e-.e- = -1, e+.e+ = +1")
    check(scalar(e4 * e4) == 0 and scalar(e5 * e5) == 0, "B1 e4 and e5 are null")
    check(sympy.simplify((e4 | e5).scalar()) == -1, "B1 e4 . e5 = -1, the Metrics page's -1 at (4,5)")
    check(same(e4 ^ e5, em ^ ep), "B1 e4 ^ e5 = e- ^ e+ (a = 1/2, b = 1)")
    check(same(e4 ^ e5, -(ep ^ em)), "B1 so e4 ^ e5 = -(e+ ^ e-): with e'4 = e+, e'5 = e-, e45 -> -e'45")
    a, b = sympy.symbols("a b")
    check(same((a * (em - ep)) ^ (b * (em + ep)), 2 * a * b * (em ^ ep)),
          "B1 (a(e- - e+)) ^ (b(e- + e+)) = 2ab e- ^ e+, equal to e- ^ e+ when ab = 1/2")
    x, y, z = sympy.symbols("x y z")
    r2 = x**2 + y**2 + z**2
    p1 = x * e1 + y * e2 + z * e3 + sympy.Rational(1, 2) * (r2 + 1) * em \
        + sympy.Rational(1, 2) * (r2 - 1) * ep
    p2 = x * e1 + y * e2 + z * e3 + e4 + sympy.Rational(1, 2) * r2 * e5
    check(same(p1, p2), "B1 the Main Page's point formula in e-, e+ equals x e1 + y e2 + z e3 + e4 + x^2/2 e5")
    check(scalar(p2 * p2) == 0, "B1 that point is null")


def b2_colapinto():
    ga, (ep, em) = build("ep em", sympy.diag(1, -1))
    h = sympy.Rational(1, 2)
    ninf, no = em - ep, h * (em + ep)  # eq. 2.39
    check(same(ep, no - h * ninf) and same(em, no + h * ninf), "B2 eq. 2.40 inverts eq. 2.39")
    check(scalar(ninf * ninf) == 0 and scalar(no * no) == 0, "B2 n_inf and n_o of eq. 2.39 are null")
    check(sympy.simplify((ninf | no).scalar()) == -1, "B2 n_inf . n_o = -1")
    check(same(no * ninf, -1 + (no ^ ninf)), "B2 eq. 2.41: n_o n_inf = -1 + E")
    check(same(no ^ ninf, ep * em), "B2 eq. 2.42: E = n_o ^ n_inf = e+ e- for eq. 2.39")
    check(scalar((no ^ ninf) * (no ^ ninf)) == 1, "B2 eq. 2.43: E^2 = 1")
    no6, ninf6 = h * (em - ep), em + ep  # caption of Table 2.6
    check(scalar(no6 * no6) == 0 and scalar(ninf6 * ninf6) == 0
          and sympy.simplify((no6 | ninf6).scalar()) == -1,
          "B2 Table 2.6's n_o = (e- - e+)/2, n_inf = e- + e+ are null with n_o . n_inf = -1")
    check(same(no6 ^ ninf6, -(ep * em)), "B2 for Table 2.6's pair, n_o ^ n_inf = -e+ e-: the sign of eq. 2.42 flips")
    p39 = sympy.Matrix([[h, h], [-1, 1]])   # rows: n_o, n_inf in (e+, e-)
    p26 = sympy.Matrix([[-h, h], [1, 1]])
    check(p39.det() == 1 and p26.det() == -1, "B2 det = +1 for eq. 2.39, -1 for Table 2.6 (CONVENTIONS 1.4)")


def b3_hestenes():
    ga, (e, es) = build("e es", [[0, 1], [1, 0]])  # eq. 1: e.e* = 1, both null
    E = e ^ es
    check(same(e * es, 1 + E), "B3 eq. 3: e e* = 1 + E")
    check(same(es * e, 1 - E), "B3 eq. 4: e* e = 1 - E")
    check(scalar(E * E) == 1, "B3 eq. 2: E^2 = 1")
    check(same(E * e, e) and same(-e * E, e) and same(es * E, es) and same(-E * es, es),
          "B3 eq. 5: Ee = -eE = e, e*E = -Ee* = e*")
    lam = sympy.symbols("lambda", nonzero=True)
    s2 = sympy.sqrt(2)
    epl = (lam * e + es / lam) / s2
    emi = (lam * e - es / lam) / s2
    check(scalar(epl * epl) == 1 and scalar(emi * emi) == -1 and sympy.simplify((epl | emi).scalar()) == 0,
          "B3 eq. 7: e+^2 = 1, e-^2 = -1, e+ . e- = 0, for every lambda")
    check(same(epl ^ emi, -E), "B3 e+ ^ e- = -(e ^ e*) for every lambda")
    check(not same(epl ^ emi, E), "B3 eq. 7's 'E = e ^ e* = e+ ^ e-' fails: an erratum; E = e- ^ e+")
    check(same(epl * emi, epl ^ emi), "B3 e+ e- = e+ ^ e- (orthogonal), so eq. 7's e+ e- is -E as well")
    # The conformal(d) basis: origin o = e*, infinity = -e, so that o . inf = -1
    # and x + o + x^2/2 inf = x - x^2/2 e + e*, eq. 20's point in additive form.
    o, inf = es, -e
    check(sympy.simplify((o | inf).scalar()) == -1, "B3 with o = e*, inf = -e: o . inf = -1")
    l0 = 1 / s2
    epl0 = (l0 * e + es / l0) / s2
    emi0 = (l0 * e - es / l0) / s2
    check(same(epl0, sympy.Rational(1, 2) * e + es) and same(emi0, sympy.Rational(1, 2) * e - es),
          "B3 lambda = 1/sqrt(2): e+ = e/2 + e*, e- = e/2 - e*, no square roots left")
    pp, pm = -epl0, -emi0  # e'(d+1) = -e+, e'(d+2) = -e-
    check(same(o, sympy.Rational(1, 2) * (pm - pp)) and same(inf, pm + pp),
          "B3 then o = (e'(d+2) - e'(d+1))/2 and inf = e'(d+2) + e'(d+1), CONVENTIONS 1.4")
    check(same(o ^ inf, -(pp ^ pm)), "B3 and o ^ inf = -e'(d+1) ^ e'(d+2), det -1")


def b4_witt():
    # Two modes: e1, e2 square to +1 and f1, f2 (DHSV's e-bar) to -1.
    ga, (e1, e2, f1, f2) = build("e1 e2 f1 f2", sympy.diag(1, 1, -1, -1))
    h = sympy.Rational(1, 2)
    w = [h * (e1 + f1), h * (e2 + f2)]      # from eq. 3.16: e = w + w*, e-bar = w - w*
    ws = [h * (e1 - f1), h * (e2 - f2)]
    check(same(e1, w[0] + ws[0]) and same(f1, w[0] - ws[0]), "B4 eq. 3.16 a, b")
    ok25 = all(sympy.simplify((ws[i] | w[j]).scalar()) == (h if i == j else 0)
               for i in range(2) for j in range(2))
    check(ok25, "B4 eq. 2.5: w*_i . w_j = delta_ij / 2")
    ok27 = all(scalar(w[i] * w[j] + w[j] * w[i]) == 0 and scalar(ws[i] * ws[j] + ws[j] * ws[i]) == 0
               and zero(w[i] * w[j] + w[j] * w[i]) and zero(ws[i] * ws[j] + ws[j] * ws[i])
               for i in range(2) for j in range(2))
    check(ok27, "B4 eqs. 2.7 a, b: {w_i, w_j} = 0 and {w*_i, w*_j} = 0")
    car = all(same(w[i] * ws[j] + ws[j] * w[i], ga.mv(1 if i == j else 0, "scalar"))
              for i in range(2) for j in range(2))
    check(car, "B4 from eqs. 2.5 and 3.7: w_i w*_j + w*_j w_i = delta_ij (the CAR)")
    printed = all(same(w[i] * ws[j] + w[j] * ws[i], ga.mv(1 if i == j else 0, "scalar"))
                  for i in range(2) for j in range(2))
    check(not printed, "B4 eq. 3.34 as printed, w_i w*_j + w_j w*_i = delta_ij, fails: an erratum")
    # The scaled Witt form of the test list: e1 . e2 = 1, e3 . e4 = 1, the
    # rest 0; c_j = e(2j-1) and c_j^dagger = e(2j)/2.
    gw = [[0, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 0, 1, 0]]
    gw_, (a1, a2, a3, a4) = build("a1 a2 a3 a4", gw)
    c, cd = [a1, a3], [h * a2, h * a4]
    one = gw_.mv(1, "scalar")
    check(all(same(c[i] * cd[j] + cd[j] * c[i], one if i == j else 0 * one)
              and zero(c[i] * c[j] + c[j] * c[i]) and zero(cd[i] * cd[j] + cd[j] * cd[i])
              for i in range(2) for j in range(2)),
          "B4 the scaled Witt form: {c_j, c_k^dagger} = delta_jk, {c_j, c_k} = {c_j^dagger, c_k^dagger} = 0")
    # The Witt change of note 0026: e1 -> (e'1 + e'3)/2, e2 -> e'1 - e'3,
    # e3 -> (e'2 + e'4)/2, e4 -> e'2 - e'4 into R(2,2,0) = (e1, e2, f1, f2),
    # so c_j -> w_j and c_j^dagger -> w*_j.
    img = [h * (e1 + f1), e1 - f1, h * (e2 + f2), e2 - f2]
    check(same(img[0], w[0]) and same(h * img[1], ws[0]) and same(img[2], w[1]) and same(h * img[3], ws[1]),
          "B4 the Witt change sends c_j to w_j and c_j^dagger to w*_j")
    gram = sympy.Matrix(4, 4, lambda i, j: sympy.simplify((img[i] | img[j]).scalar()))
    check(gram == sympy.Matrix(gw), "B4 the Witt change is an isometry onto R(2,2,0)")


def main():
    b1_lengyel()
    b2_colapinto()
    b3_hestenes()
    b4_witt()
    print("FAILED: %d" % len(failures) if failures else "all reading checks passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
