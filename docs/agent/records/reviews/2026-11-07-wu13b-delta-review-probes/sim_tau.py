"""Independent simulation of transform's rounding check.

Reads the kernels the core dumps, runs them over IEEE doubles (Python floats,
correctly rounded, gradual underflow) and over exact Fractions, recomputes the
core's tolerance, fl(fl(gamma_(K+1)) P) with P the magnitude padded by the
smallest normal number after each rounded product, and checks the forward
error bound in its general form |d - e| <= tol (any input, not only versors),
which for a versor (e = 0) is the check. Also evaluates the additive bound
gamma_K M + K * denorm_min, which ignores how later factors scale an
underflowed product's error.
"""
import json, math, random, sys
from fractions import Fraction as Fr

U = 2.0 ** -53
LAM = 2.0 ** -1022
TAU = LAM / U
DMIN = 2.0 ** -1074
MAX = sys.float_info.max

def run(kernel, n, x, y, mul, neg):
    out = [None] * n
    for a, b, o, c, first in kernel:
        p = mul(x[a], y[b])
        if c == 1:
            out[o] = p if first else out[o] + p
        elif c == -1:
            out[o] = neg(p) if first else out[o] - p
        else:
            t = mul(c, p)
            out[o] = t if first else out[o] + t
    return [0 if v is None else v for v in out]

fmul = lambda a, b: a * b
fneg = lambda a: -a

def tolerance(kd, v, x, margin=1, pad=TAU):
    av = [abs(t) for t in v]; ax = [abs(t) for t in x]
    def padded(kernel, n, X, Y):
        out = [0.0] * n
        for a, b, o, c, first in kernel:
            term = (X[a] * Y[b]) + pad
            if c not in (1, -1):
                term = (abs(c) * term) + pad
            out[o] = term if first else out[o] + term
        return out
    mid = padded(kd['first'], kd['nm'], av, ax)
    mag = padded(kd['second'], kd['nd'], mid, av)
    tol = []
    for j, K in enumerate(kd['K']):
        k = float(K + margin)
        g = (k * U) / (1.0 - k * U)
        tol.append(g * mag[j])
    return tol

def exact_magnitude(kd, v, x):
    av = [abs(Fr(t)) for t in v]; ax = [abs(Fr(t)) for t in x]
    mid = run([[a, b, o, abs(c), f] for a, b, o, c, f in kd['first']], kd['nm'], av, ax, fmul, fneg)
    return run([[a, b, o, abs(c), f] for a, b, o, c, f in kd['second']], kd['nd'], mid, av, fmul, fneg)

def evaluate(kd, v, x):
    mid = run(kd['first'], kd['nm'], v, x, fmul, fneg)
    d = run(kd['second'], kd['nd'], mid, v, fmul, fneg)
    fv = [Fr(t) for t in v]; fx = [Fr(t) for t in x]
    emid = run(kd['first'], kd['nm'], fv, fx, fmul, fneg)
    e = run(kd['second'], kd['nd'], emid, fv, fmul, fneg)
    return d, e

def gamma(k):
    return Fr(k) * Fr(U) / (1 - Fr(k) * Fr(U))

kernels = [json.loads(l) for l in open(sys.argv[1])]
# 1. the simulation equals the core bit for bit on the probe input
for kd in kernels:
    v = [(-1.0 if k % 2 else 1.0) * (k + 1) / 7.0 for k in range(kd['nv'])]
    x = [(k + 2) / 3.0 for k in range(kd['nx'])]
    mid = run(kd['first'], kd['nm'], v, x, fmul, fneg)
    d = run(kd['second'], kd['nd'], mid, v, fmul, fneg)
    tol = tolerance(kd, v, x)
    assert [float.hex(t) for t in d] == [float.fromhex(s).hex() for s in kd['probe_d']], kd['name']
    assert [float.hex(t) for t in tol] == [float.fromhex(s).hex() for s in kd['probe_tol']], kd['name']
print("simulation equals the core on the probe inputs:", len(kernels), "transforms")

def draw(rng, lo, hi, zero_share=0.0):
    if rng.random() < zero_share:
        return 0.0
    m = rng.getrandbits(53) | (1 << 52)
    s = -1 if rng.random() < 0.5 else 1
    return s * math.ldexp(m, rng.randint(lo, hi) - 52)

rng = random.Random(int(sys.argv[2]) if len(sys.argv) > 2 else 1)
trials = int(sys.argv[3]) if len(sys.argv) > 3 else 2000
regimes = {
    'ordinary': (-3, 3, 0.0),
    'mixed scales': (-400, 300, 0.0),
    'deep underflow': (-560, -330, 0.0),
    'underflow and zeros': (-560, -300, 0.3),
    'one huge': None,
    'near overflow': (95, 102, 0.0),
}
worst = {}
viol = 0
old_viol = 0
fired_versor = 0
for kd in kernels:
    for name, reg in regimes.items():
        for t in range(trials // len(regimes)):
            if reg is None:
                v = [draw(rng, -540, -520) for _ in range(kd['nv'])]
                v[rng.randrange(kd['nv'])] = draw(rng, 480, 520)
                x = [draw(rng, -560, -530) for _ in range(kd['nx'])]
            else:
                lo, hi, z = reg
                v = [draw(rng, lo, hi, z) for _ in range(kd['nv'])]
                x = [draw(rng, lo, hi, z) for _ in range(kd['nx'])]
            d, e = evaluate(kd, v, x)
            if any(math.isinf(t) or math.isnan(t) for t in d):
                continue
            tol = tolerance(kd, v, x)
            M = exact_magnitude(kd, v, x)
            for j in range(kd['nd']):
                err = abs(Fr(d[j]) - e[j])
                if math.isinf(tol[j]):
                    continue
                if err > Fr(tol[j]):
                    viol += 1
                    print("VIOLATION", kd['name'], name, j, float(err), tol[j])
                old = gamma(kd['K'][j]) * M[j] + kd['K'][j] * Fr(DMIN)
                if err > old:
                    old_viol += 1
                    r = float(err / old)
                    key = (kd['name'], name)
                    worst[key] = max(worst.get(key, 0), r)
                if tol[j] > 0 and err > 0:
                    key = ('ratio', kd['name'], name)
                    worst[key] = max(worst.get(key, 0), float(err / Fr(tol[j])))
print("violations of |d - e| <= tol:", viol)
print("violations of gamma_K M + K denorm_min:", old_viol)
for k in sorted(worst, key=str):
    print(k, "%.3g" % worst[k])
