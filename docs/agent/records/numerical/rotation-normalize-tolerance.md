# Tolerances for `rotation` once it normalizes its axis

- Date: 2026-11-08
- Author: Claude (agent, Opus 5.5), implementer of plan 0024 with numerical-expert judgement.
- Branch `feature/rotation-normalize`, from `develop` at `bd60589`. Written before the tests it covers exist or have run (V12), and before `rotation` changes. The only run is `rotation-normalize-tolerance/tol.py` (exact, in `fractions`, Python 3.14.9; output `tol.out`), which evaluates no floating-point `sin`, `cos`, or `rotation`.
- Extends `rotation-tolerance.md` (the numerical expert's derivation for the unit z axis, decision 0201): same model of arithmetic, same k = 9 for the C library's `sin` and `cos`, same oracle counts K for the z axis, which do not depend on the length of the stored direction.

## The change

`rotation(l, theta)` will compute n̂ = fl(√(fl(fl(v0·v0 + v1·v1) + v2·v2))), the length of l's direction (v0, v1, v2) on e41, e42, e43, then q = fl(ŝ / n̂) with ŝ = fl_lib(sin(θ/2)), and store fl(l_i · q) on each of the six line blades and ĉ = fl_lib(cos(θ/2)) on e1234. Mathematically this is the rotor of the normalized line l / |v|. One square root and one division per call; the six products were there before. The build uses `-ffp-contract=off`, so no product is fused.

## When the motor is unchanged bit for bit

- Direction of length exactly 1 in T (every axis-aligned unit line, such as the z axis of the existing quarter-turn test): the squares and sums are exact, n̂ = 1, q = ŝ, and every coefficient is the one the unnormalized formula gave. **The existing quarter-turn test and its tolerances (39u, 38u, 0, 31u) are unchanged**, and so is the bit-for-bit construction test for those axes.
- Direction (0, 0, 2^j) with no underflow or overflow: n̂ = 2^j exactly, q = ŝ / 2^j exactly, and 2^j · q = ŝ, so the motor equals that of (0, 0, 1) bit for bit, the moment scaled by 2^j included (m_i · 2^−j is exact). So a quarter turn about the z axis of length 2 meets the same tolerances as about the unit axis.
- A unit direction that is not along an axis, such as (0.6, 0.8, 0) in double, need not give n̂ = 1, so its motor can differ in the last bits from the unnormalized formula. The bit-for-bit construction test therefore computes its expected motor by the same formula as the code (that test verifies the construction, not its accuracy).

## Direction of length 5 along z

Direction (0, 0, 5): 25 is exact, n̂ = 5 exactly (IEEE square root is correctly rounded), q = ŝ/5 · (1 + ε1), and the stored sine is s' = fl(5q) = ŝ (1 + ε1)(1 + ε2), |ε1|, |ε2| ≤ u.

**Claim.** If |ŝ| lies in [1/2, 1), |s' − ŝ| ≤ 2u. Proof, for ŝ > 0 (the negative case is its mirror): |s' − ŝ| ≤ (2u + u²) ŝ < 2u + u², so s' lies in (1/4, 1], where every value of T is a multiple of u/2 (values in [1/4, 1/2) are multiples of u/2, those in [1/2, 1] multiples of u). ŝ is a multiple of u, so the difference is a multiple of u/2 below 2u + u², hence at most 2u.

**Same motor as the unit line.** For the line through (1, 0, 0) along z given as (0, 0, 5, 0, −5, 0), the stored coefficients on e43 and e31 are s' and −s'; those on e41, e42, e23, e12 are 0 · q, a zero of the sign of ŝ, as 0 · ŝ is for the unit line (0, 0, 1, 0, −1, 0); e1234 holds ĉ in both, and 1 holds 0. So the two motors agree bit for bit except on e43 and e31, where they differ by at most 2u for every θ with |sin(θ/2)| in [1/2, 1). The comparison |x − y| ≤ 2u is exact: the difference of two values within a factor 2 of each other is exact (Sterbenz), and 2u is a power of 2.

**Quarter turn about it.** With s = sin φ, the stored sine's error is a' = |s' − s| ≤ |s' − ŝ| + |ŝ − s| ≤ 2u + k u; the cosine's is b ≤ k u as before. The derivation of `rotation-tolerance.md` then holds unchanged with a = (k + 2) u: E1 = a(2|s| + a) + b(2|c| + b), E2 = 2(|s| b + |c| a + ab), and

    tol_0 = γ_12 (1 + E1) + E1 + δ
    tol_1 = γ_12 (1 + E1) + E2 + δ²/2
    tol_2 = 0
    tol_3 = γ_5 (1 + E1) + E1

K = 12, 12, 12, 5 are the counts of `kept.out` for this motor and point layout, which depend on the layouts, not on the values. ŝ for θ = fl(π/2) is about 0.7071, in [1/2, 1).

## Values (`tol.out`, k = 9)

| T | axis length | tol / u (e1, e2, e3, e4) | N to commit |
| --- | --- | --- | --- |
| double | 1 or 2 | 38.01, 37.46, 0, 30.46 | 39, 38, 0, 31 (as before) |
| double | 5 | 40.84, 40.28, 0, 33.28 | 41, 41, 0, 34 |
| float | 1 or 2 | 38.19, 37.46, 0, 30.46 | 39, 38, 0, 31 (as before) |
| float | 5 | 41.02, 40.28, 0, 33.28 | 42, 41, 0, 34 |

The first and third rows reproduce `rotation-tolerance.md`'s values, a check of the script.

## A direction of length zero

n̂ = 0, so q = ŝ/0 is ±∞ (or NaN when ŝ = 0, at θ = 0), and each line coefficient is 0 · q = NaN or m_i · q = ±∞ or NaN; e1234 still holds ĉ. The motor is not finite, and `transform` propagates the NaN. This is a precondition violation, documented in the comment, not checked. A direction whose squared length underflows (length below about 1.5e−154 in double, 1.1e−19 in float) behaves the same way, or loses accuracy when the sum is subnormal; one whose squared length overflows (length above about 1.3e154 in double, 1.8e19 in float) gives n̂ = ∞, q = 0, and the motor holds only ĉ on e1234, a wrong motor. The comment states a safe range for the length, 1e−150 to 1e150 in double and 1e−18 to 1e18 in float.

## Not covered

As in `rotation-tolerance.md`: other C libraries and architectures, other axes and angles for the geometric test, lines off the origin in the geometric test. A direction length L that is not a power of 2 and not 5 adds the error of n̂ (for an inexact square root up to about 2.5u relative) to that of q; it is not derived here and no test relies on it.
