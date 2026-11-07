"""Polynomial identities behind note 0022, proved symbolically over the
coefficients (every identity holds for all real coefficients, not only for
sampled ones), with the independent algebra of cga.py and SymPy.

Flat-plane chain layout, e5, e125, e135, e235, e145, e245, e345, e12345,
coefficients q, a1, a2, a3, b1, b2, b3, s; x a vector x1 e1 + ... + x5 e5:
 F1  V̰ ⟇ V = V ⟇ V̰ = λ𝟙 + 2h e5, λ = b1² + b2² + b3² + s²,
     h = −a1 b3 + a2 b2 − a3 b1 + q s.
 F2  The part of V ⟇ x ⟇ V̰ off the vectors: every trivector coefficient is
     0, and the antiscalar is 2h x4.
 F3  The unpadded magnitude of that antiscalar, the sum over the terms of
     the transform's two stages (V ⟇ x, then that with V̰) of
     |c| |v| |x| |v|, is |x4| T, T = 2(|a1 b3| + |a2 b2| + |a3 b1| + |q s|).
 F4  V̰ ⟇ e5 ⟇ V = λ e5, so for m on the same layout (a flat motor of two
     planes is on it, with q = 0), W(m ⟇ V) = λ_m λ_V 𝟙 + (λ_m 2h_V +
     λ_V 2h_m) e5: τ = 2h / λ is additive under composition.
 F5  V ⟇ (𝟙 + α e5) keeps λ and has defect 2(h + α λ), and e5 ⟇ e5 = 0.
 F6  On the oriented blades of the rigid(3) motor wedged with e5, weight v on
     e415, e425, e435, e12345 and bulk m on e235, e315, e125, e5,
     h = v_x m_x + v_y m_y + v_z m_z + v_w m_w and λ = |v|², and the update
     of F5 with α = −h/λ is m ← m − (h/λ) v.
Any even V of conformal(3), all 16 blades of odd grade:
 H1  V̰ ⟇ V = λ𝟙 + w with w a vector; the trivector part of V ⟇ x ⟇ V̰ is 0,
     and its antiscalar is <x ⟇ (V̰ ⟇ V)>_𝟙, linear in w.

Usage: <python with sympy> -I identity.py   (exit status 0 when all hold)
"""
import os
import sys

import sympy as sp

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import cga  # noqa: E402

ANTI = 31
E5 = 16
failures = []


def check(cond, what):
    print(("holds  " if cond else "FAILS  ") + what)
    if not cond:
        failures.append(what)


def expand(x):
    return {m: sp.expand(c) for m, c in x.items() if sp.expand(c) != 0}


flat_masks = [16, 19, 21, 22, 25, 26, 28, 31]
q, a1, a2, a3, b1, b2, b3, s = sym = sp.symbols("q a1 a2 a3 b1 b2 b3 s", real=True)
V = dict(zip(flat_masks, sym))
X = sp.symbols("x1:6", real=True)
x = {1 << i: X[i] for i in range(5)}
lam = b1**2 + b2**2 + b3**2 + s**2
h = -a1 * b3 + a2 * b2 - a3 * b1 + q * s

# F1
W = expand(cga.antigp(cga.antireverse(V), V))
W2 = expand(cga.antigp(V, cga.antireverse(V)))
check(W == W2 == expand({ANTI: lam, E5: 2 * h}), "F1  V̰ ⟇ V = V ⟇ V̰ = λ𝟙 + 2h e5")

# F2
S = expand(cga.sandwich_even(V, x))
off = {m: c for m, c in S.items() if cga.popcount(m) != 1}
check(off == expand({ANTI: 2 * h * X[3]}), "F2  dropped part of V ⟇ x ⟇ V̰ is 2h x4 on 𝟙, 0 on every trivector")

# F3: the two stages term by term, coefficients and factors in absolute value
av = sp.symbols("Q A1 A2 A3 B1 B2 B3 S", nonnegative=True)
ax = sp.symbols("X1:6", nonnegative=True)
middle = {}
for i, m in enumerate(flat_masks):
    for j in range(5):
        for o, c in cga._ANTI[(m, 1 << j)].items():
            middle[o] = middle.get(o, 0) + abs(c) * av[i] * ax[j]
mag = 0
for o, mo in middle.items():
    for k, m in enumerate(flat_masks):
        c = cga._ANTI[(o, m)].get(ANTI, 0)
        if c:
            mag += abs(c) * mo * av[k]
Q, A1, A2, A3, B1, B2, B3, S_ = av
check(sp.expand(mag - ax[3] * 2 * (A1 * B3 + A2 * B2 + A3 * B1 + Q * S_)) == 0,
      "F3  magnitude of the antiscalar = |x4| T")

# F4
R = expand(cga.antigp(cga.antigp(cga.antireverse(V), {E5: 1}), V))
check(R == expand({E5: lam}), "F4  V̰ ⟇ e5 ⟇ V = λ e5")
msym = sp.symbols("mq ma1 ma2 ma3 mb1 mb2 mb3 ms", real=True)
M = dict(zip(flat_masks, msym))
lam_m = msym[4]**2 + msym[5]**2 + msym[6]**2 + msym[7]**2
h_m = -msym[1] * msym[6] + msym[2] * msym[5] - msym[3] * msym[4] + msym[0] * msym[7]
P = cga.antigp(M, V)
WP = expand(cga.antigp(cga.antireverse(P), P))
check(WP == expand({ANTI: lam_m * lam, E5: lam_m * 2 * h + lam * 2 * h_m}),
      "F4  W(m ⟇ V) = λ_m λ_V 𝟙 + (λ_m 2h_V + λ_V 2h_m) e5")
check(set(P) <= set(flat_masks), "F4  the layout is closed under ⟇")

# F5
alpha = sp.Symbol("alpha", real=True)
Pa = cga.antigp(V, {ANTI: 1, E5: alpha})
Wa = expand(cga.antigp(cga.antireverse(Pa), Pa))
check(Wa == expand({ANTI: lam, E5: 2 * (h + alpha * lam)}), "F5  V ⟇ (𝟙 + α e5) has λ and 2(h + αλ)")
check(cga.antigp({E5: 1}, {E5: 1}) == {}, "F5  e5 ⟇ e5 = 0")
check(expand(Pa) == expand({E5: q + alpha * s, 19: a1 - alpha * b3, 21: a2 + alpha * b2, 22: a3 - alpha * b1,
                            25: b1, 26: b2, 28: b3, 31: s}),
      "F5  V ⟇ (𝟙 + α e5): q += α s, a1 −= α b3, a2 += α b2, a3 −= α b1")

# F6: e415 = -e145, e425 = -e245, e435 = -e345, e315 = -e135 (one inversion each)
vx, vy, vz, vw, mx, my, mz, mw = sp.symbols("v_x v_y v_z v_w m_x m_y m_z m_w", real=True)
sub = {b1: -vx, b2: -vy, b3: -vz, s: vw, a3: mx, a2: -my, a1: mz, q: mw}
check(sp.expand(h.subs(sub) - (vx * mx + vy * my + vz * mz + vw * mw)) == 0 and
      sp.expand(lam.subs(sub) - (vx**2 + vy**2 + vz**2 + vw**2)) == 0,
      "F6  h = v·m and λ = |v|² on the oriented motor blades")
al6 = -(vx * mx + vy * my + vz * mz + vw * mw) / (vx**2 + vy**2 + vz**2 + vw**2)
new = {E5: (q + alpha * s), 19: (a1 - alpha * b3), 21: (a2 + alpha * b2), 22: (a3 - alpha * b1)}
new = {k: c.subs(sub).subs(alpha, al6) for k, c in new.items()}
k6 = al6  # m <- m + alpha v in oriented coordinates
check(sp.simplify(new[E5] - (mw + k6 * vw)) == 0 and sp.simplify(new[22] - (mx + k6 * vx)) == 0 and
      sp.simplify(-new[21] - (my + k6 * vy)) == 0 and sp.simplify(new[19] - (mz + k6 * vz)) == 0,
      "F6  the update is m <- m - (v·m / |v|²) v")

# H1
hyper_masks = [m for m in range(32) if cga.popcount(m) % 2 == 1]
hs = sp.symbols("h0:16", real=True)
H = dict(zip(hyper_masks, hs))
WH = expand(cga.antigp(cga.antireverse(H), H))
check(all(cga.popcount(m) in (1, 5) for m in WH), "H1  V̰ ⟇ V has only 𝟙 and vector parts")
SH = expand(cga.sandwich_even(H, x))
check(all(cga.popcount(m) != 3 for m in SH), "H1  trivector part of V ⟇ x ⟇ V̰ is 0")
check(sp.expand(SH.get(ANTI, 0) - cga.antigp(x, WH).get(ANTI, 0)) == 0, "H1  antiscalar = <x ⟇ (V̰ ⟇ V)>_𝟙")

print("sympy", sp.__version__)
sys.exit(1 if failures else 0)
