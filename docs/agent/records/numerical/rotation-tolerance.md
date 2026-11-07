# Tolerance for a geometric test of `rotation` over double and float

- Date: 2026-11-07
- Author: Claude (agent, Opus 5.5), numerical-expert role, for WU16 (plan 0023 R11).
- Branch `records/rotation-tolerance`, from `develop` at `88c9484`. Nothing in `include/` or `test/` is changed.
- Written before any geometric test of `rotation` exists or has run (V12). The only runs below are the tracked oracle on exact inputs and the exact computation of the tolerance; neither evaluates `rotation`, `std::sin`, or `std::cos` in floating point.
- Files: `rotation-tolerance/kept.cpp` (counts K and the magnitude of each kept coefficient with `test_support::tracked`, GCC 16.2.1, `-std=c++23 -O1`, headers of `88c9484`; output `kept.out`), `rotation-tolerance/tol.py` (the numbers below, exact in `fractions`, Python 3.14.9; output `tol.out`).

## The test this covers

A point x = (x, y, z, w) on `e1 e2 e3 e4` is moved by `transform(x, rotation(l, theta))`, where l is a coordinate axis through the origin, here the z axis (direction 1 on e43, moment 0), and theta is a value of T (double or float). The test compares each coefficient of the result with a literal, such as (0, 1, 0, 1) for a quarter turn of (1, 0, 0, 1). Other axes, lines not through the origin, and tests that unitize the result are not covered (see the end).

## Sources read (V9)

1. **The GNU C Library Reference Manual, for version 2.44**, node "Errors in Math Functions" (section 19.7), read from the installed info files (`/usr/share/info/libc.info-9.gz`, package `glibc 2.44+r50+g1848099f063e-3`, the libm the tests link on this machine). This manual no longer has a per-function table of errors. It states the goal, quoted: "Each function with a floating-point result behaves as if it computes an infinite-precision result that is within a few ulp ... of the mathematically correct value of the function", and "some functions have bugs meaning they do not meet these goals in all cases", and: "The math testsuite only flags results larger than 9ulp (or 16 for IBM 'long double' format) as errors; although most of the implementations show errors smaller than the limit." Its ulp is that of the result's binade: 2^(e − p + 1) for a result in [2^e, 2^(e+1)).
2. **glibc's `sysdeps/x86_64/fpu/libm-test-ulps` at tag `glibc-2.41`**, from sourceware.org's public git (sha256 of the file as fetched `1c3dfd0bf094d05128f8935a8d01106afdd8483fcf530914548ccb6641344486`). For `sin` and for `cos`, quoted: "double: 1", "float: 1". This is the largest error the testsuite observed on its own inputs, rounded up to an integer, for whichever variant the machine that generated it selected: a sampled claim, not a bound. The file is absent at tags `glibc-2.42`, `glibc-2.43`, and `glibc-2.44` (HTTP 404); its last published form is 2.41, one version older than the library in use. The 2.42 to 2.44 entries of glibc's NEWS (tag `glibc-2.44`) list no change to x86-64 `sin`, `cos`, `sinf`, or `cosf`; their CORE-MATH imports do not include them.

So no source gives a proved bound for glibc's `sin` and `cos`. The bound below takes k ulp as a parameter and gives values for k = 9 (the only figure stated for 2.44), k = 1 (the last per-function figure, 2.41), and k = 1/2 (correct rounding, which is what GCC gives when it folds `std::sin` of a constant through MPFR).

## Model and assumptions

- T is double (p = 53) or float (p = 24); u = 2^−p; γ_k = ku/(1 − ku). Every operation rounds once to T, to nearest; no wider intermediate format; no product underflows (every nonzero input here is at least 1/2 in magnitude, and a product with an exact zero is exact).
- `rotation(l, theta)` builds the motor with ŝ = fl_lib(sin φ) on e43 and ĉ = fl_lib(cos φ) on e1234, φ = θ/2, every other coefficient 0 (plan 0023 R4; layout `e41 e42 e43 e1234 e23 e31 e12 1` of `feature/rga3`). Halving θ is exact (barring underflow), and for the z axis l_i ŝ is ŝ or 0, exact. So the motor's only errors are a = ŝ − sin φ and b = ĉ − cos φ, with |a| ≤ k ulp(sin φ) and |b| ≤ k ulp(cos φ).
- The library computes sin and cos at the double (or float) φ exactly as passed, so the rotation the motor approximates is by θ, the value of T passed, not by the real angle the test means.

## Derivation

Write the computed coefficient ŷ_j, the exact sandwich y_j of the stored motor (ŝ, ĉ) on x, the exact rotation z_j = (R_θ x)_j, and the literal r_j. Then |ŷ_j − r_j| ≤ |ŷ_j − y_j| + |y_j − z_j| + |z_j − r_j|.

**Transform's rounding.** Lemma A of decision 0180 holds for every node, kept coefficients included: |ŷ_j − y_j| ≤ γ_(K_j) P_j, and with no underflow P_j = M_j, the magnitude, which is `tracked`'s bound. The oracle (`kept.out`) gives, for this motor and point layout:

| j | y_j | M_j | K_j |
| --- | --- | --- | --- |
| 0 (e1) | (ĉ² − ŝ²) x − 2ŝĉ y | (ĉ² + ŝ²)\|x\| + 2\|ŝĉ\|\|y\| | 12 |
| 1 (e2) | 2ŝĉ x + (ĉ² − ŝ²) y | 2\|ŝĉ\|\|x\| + (ĉ² + ŝ²)\|y\| | 12 |
| 2 (e3) | (ĉ² + ŝ²) z | (ĉ² + ŝ²)\|z\| | 12 |
| 3 (e4) | (ĉ² + ŝ²) w | (ĉ² + ŝ²)\|w\| | 5 |

K is structural: it counts paths through the zero coefficients too, so γ_12 overstates the roundings of this sparse motor. It is the forward bound the core and its tests use, and it is kept.

**The motor's error, with its norm.** The entries are homogeneous quadratics in (ŝ, ĉ), equal to cos θ = c² − s², sin θ = 2sc, and 1 = c² + s² at the exact (s, c). With s = sin φ, c = cos φ:

- E1 = |a|(2|s| + |a|) + |b|(2|c| + |b|) bounds both |(ĉ² − ŝ²) − cos θ| and |(ĉ² + ŝ²) − 1|; the second is the motor's lack of unit norm, which scales the whole result, the weight included.
- E2 = 2(|s||b| + |c||a| + |a||b|) bounds |2ŝĉ − sin θ|.

So |y_0 − z_0| ≤ E1|x| + E2|y|, |y_1 − z_1| ≤ E2|x| + E1|y|, |y_2 − z_2| ≤ E1|z|, |y_3 − z_3| ≤ E1|w|. Also ĉ² + ŝ² ≤ 1 + E1 and 2|ŝĉ| ≤ ĉ² + ŝ², which bound the magnitudes.

**The angle passed.** With δ = |π/2 − θ| for the quarter turn, |cos θ| ≤ δ and 1 − sin θ ≤ δ²/2.

**The tolerance.** For the quarter turn of (1, 0, 0, 1), compared with (0, 1, 0, 1):

    tol_0 = γ_12 (1 + E1) + E1 + δ
    tol_1 = γ_12 (1 + E1) + E2 + δ²/2
    tol_2 = 0                      (M_2 = 0, so the coefficient is exactly zero)
    tol_3 = γ_5 (1 + E1) + E1

with E1, E2 at |a| = |b| = k u, since sin φ and cos φ lie in [1/2, 1), where an ulp is u. sin φ + cos φ ≤ √2, so E1, E2 ≤ 2√2 k u + 2k²u².

**The comparison.** |ŷ_0 − 0| and |ŷ_2 − 0| are exact; ŷ_1 − 1 and ŷ_3 − 1 are exact by Sterbenz's lemma, since both lie in [1/2, 2]. A tolerance written as an integer N times `std::numeric_limits<T>::epsilon() / 2` is exact. So the test adds no rounding of its own, and the values below, rounded up to integers N, are the tolerances to commit.

## Values (`tol.out`)

| T | k | tol_0 / u | tol_1 / u | tol_2 | tol_3 / u | N to commit (0, 1, 2, 3) |
| --- | --- | --- | --- | --- | --- | --- |
| double | 9 | 38.01 | 37.46 | 0 | 30.46 | 39, 38, 0, 31 |
| double | 1 | 15.38 | 14.83 | 0 | 7.83 | 16, 15, 0, 8 |
| double | 1/2 | 13.97 | 13.41 | 0 | 6.41 | 14, 14, 0, 7 |
| float | 9 | 38.19 | 37.46 | 0 | 30.46 | 39, 38, 0, 31 |
| float | 1 | 15.56 | 14.83 | 0 | 7.83 | 16, 15, 0, 8 |
| float | 1/2 | 14.15 | 13.41 | 0 | 6.41 | 15, 14, 0, 7 |

In double, u = 2^−53 and δ = 0.5515u (6.12 × 10^−17); 39u is 4.33 × 10^−15. In float, u = 2^−24 and δ = 0.7334u (4.37 × 10^−8); 39u is 2.32 × 10^−6. Of the 38u at k = 9, about 12u is transform's rounding, 25.5u the library's sine and cosine with the norm defect, and 0.55u the angle passed.

Recommended: k = 9, the only figure the documentation of the library in use states, so the tolerances 39u, 38u, 0, 31u per coefficient, in double and in float. It is not a guarantee (the manual says some functions miss its goal), but it is the documented ceiling of the release's own testsuite; k = 1 rests on an older version's sampled file. Either value rejects the mutants plan 0023 lists (θ for θ/2, sine and cosine swapped, −θ), whose errors are of order 1.

## Not covered

- Another C library (musl, a vendor libm), another architecture, or a build under `-ffast-math` (which the core rejects in checking builds).
- Other axes: K is structural and the same per output coefficient, but the magnitude polynomials were counted only for the z axis. A line not through the origin or not along an axis adds a rounding per l_i ŝ and more terms; the oracle must be rerun on that motor.
- A test that unitizes the result: dividing by the weight removes the norm factor exactly but adds a rounding and changes the bound.
- Other angles: the formulas hold for any θ, with |a| ≤ k ulp(sin φ), |b| ≤ k ulp(cos φ) in their own binades, and δ the gap between the meant and the passed angle; the values above are for the quarter turn only.
