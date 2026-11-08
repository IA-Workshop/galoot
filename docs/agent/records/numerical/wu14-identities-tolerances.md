# Tolerances for the floating identities of WU14 over double

- Date: 2026-11-08
- Author: Claude (agent, Opus 5.5), numerical-expert role, part N of plan 0029 (criterion 8; decision 0216 item 7).
- Branch `records/wu14-numerical`, from `develop` at `da5d91a`. Nothing in `include/` or `test/` is changed.
- Written before any floating identity test exists or has run (V12). The only runs are of the `tracked` oracle on exact inputs: no double result of an identity was computed, so no tolerance below was seen against an error.

## The identities

Criterion 8 asks for one test case over `double` on rigid(3) and conformal(3) that compares the two sides L and R of each identity, coefficient by coefficient:

1. Associativity of ⟑: (a ⟑ b) ⟑ c against a ⟑ (b ⟑ c).
2. Associativity of ⟇: (a ⟇ b) ⟇ c against a ⟇ (b ⟇ c).
3. The De Morgan laws: right(a ⟑ b) against right(a) ⟇ right(b), right(a ∧ b) against right(a) ∨ right(b), the same with left, and the four converses with ⟇ and ∨ on the left (the eight of `identities_checks.hpp`'s `check_de_morgan`).
4. Transform composition: transform(x, compose(Q2, Q1)) against transform(transform(x, Q1), Q2).

## Sources (V9)

- `test/support/tracked.hpp` at `da5d91a`: the counting rules (an input or integer constant K = 0; negation unchanged; a sum or difference K = max(K_a, K_b) + 1 with magnitudes added; a product K = K_a + K_b + 1 with magnitudes multiplied), the bound |d − e| ≤ γ_K M, and the computed check `within_rounding_bound` with its margin of two.
- Decision 0180 (Lemma A and its use of Higham's model and Lemmas 3.1 and 3.3) and `rotation-tolerance.md`, as models of method.
- Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., is cited here only through those two records' quotations of equation (2.4), Lemma 3.1, Lemma 3.3, and the monotonicity of IEEE rounding. It was not re-read for this record; the derivation below uses only those four facts.

## Model and assumptions

- T is double: p = 53, u = 2^−53, γ_k = k u / (1 − k u). Every operation rounds once to double, to nearest (`FLT_EVAL_METHOD == 0`, which `tracked.hpp` asserts). A fused multiply-add rounds once where the count has two, so the bound also holds for a contracted build; the test build sets `-ffp-contract=off`.
- No underflow and no overflow. The inputs below are dyadic rationals of ordinary size, so every nonzero intermediate is far above 2^−1022.
- Every input is exact in both runs: the double input is the tracked input's value. Error in forming the inputs is not covered.
- The double run and the tracked run perform the same operation tree. Both run the same templates; no kernel of `mv.hpp`, `product.hpp`, `unary.hpp`, or `transform.hpp` branches on the scalar type except for checking. In checking mode `transform` keeps the unchecked path's kept coefficients bit for bit (decision 0180 item 1).
- The two sides agree exactly on the exact inputs: e_L = e_R = e on every blade. The test checks this on the tracked values before it compares the doubles. In the scratch run every coefficient of every identity agreed (see "Counts").

## The rule

For each blade B in the union of the two sides' layouts, let L̂, R̂ be the double coefficients and (e, K_L, M_L), (e, K_R, M_R) the tracked ones. A blade absent from one side counts there as 0 with K = 0 and M = 0, and its tracked value on the other side must be 0.

**Bound.** By `tracked`'s bound on each side and the triangle inequality through the common e,

    |L̂ − R̂| ≤ γ_(K_L) M_L + γ_(K_R) M_R.

K and M are those of the whole composed computation, as the oracle counts it through the inner product, the inner transform, or the composition, so the bound needs no separate composition argument.

**Both sides with at most one rounding.** If K_L ≤ 1 and K_R ≤ 1, each side is the double nearest e: a K = 0 result is exact and e is then a double; a K = 1 result is one correctly rounded operation on exact operands. So the test requires L̂ == R̂. This is the tightest rule (V12), and it occurs: in each wedge and antiwedge law one coefficient is a single product on both sides (the image of a₀b₀ or of its complement), so K = 1.

**Otherwise, the computed comparison.** The test computes, in double,

    g_X = fl(k u / (1 − k u)) with k = K_X + 1,  M'_X = nearest_double(M_X),
    t_X = fl(g_X M'_X)  for X = L, R,
    tol = fl(t_L + t_R),

and requires fl(|L̂ − R̂|) ≤ tol. Proof that a result within the bound always passes:

- k u is an integer times 2^−53 and 1 − k u a multiple of u in [1/2, 1), so both are exact and only the division rounds: g ≥ (1 − u) γ_k.
- `nearest_double` rounds the numerator once and divides by a power of two exactly: M' ≥ (1 − u) M.
- So t ≥ (1 − u)^3 γ_(K+1) M, and (1 − u)^3 γ_(K+1) ≥ γ_K whenever 3(K + 1) u < 1. Multiplying out by the positive denominators, the difference of the two sides is at least 1 − 3(K + 1) u + 3K(K + 1) u² (expanded with sympy 1.14.0, and checked in exact rationals for K = 0 to 200 with Python 3.14.9's `fractions`, both in the math-tools interpreter).
- Hence t_L + t_R ≥ γ_(K_L) M_L + γ_(K_R) M_R ≥ |L̂ − R̂| as exact reals. fl(L̂ − R̂) is the correctly rounded difference and rounding is monotone, so fl(|L̂ − R̂|) ≤ fl(t_L + t_R) = tol.

A margin of one rounding per side is the smallest integer margin this argument admits: with k = K the factor (1 − u)^3 < 1 makes it fail (checked for K = 1 to 200). It is one less than `tracked`'s `rounding_margin` of two, which pays for comparing against fl(e) rather than another computed value. Calling `rounding_tolerance(K_X, M_X)` for each side instead is also sound and looser by the factor γ_(K+2)/γ_(K+1), about 1 + 1/K. Recommended: margin one, as V12 asks for the tightest tolerance, written as a helper beside `rounding_tolerance` with this derivation.

The tolerance is computed per coefficient at run time from the tracked run. There is no literal to commit. The numbers below are what the rule gives on the scratch inputs, as orientation for the test author.

**Also recommended:** the per-side check `within_rounding_bound(L̂, e, K_L, M_L)` and the same for R. It implies the identity bound and also catches an error common to both sides, which the identity comparison cannot see. Criterion 8 requires only the identity comparison.

## Counts (scratch run, tracked only)

Program `counts.cpp` in the session scratchpad (not committed, so not reproducible as committed, V10). GCC 16.2.1, `-std=c++23 -O1 -ffp-contract=off`, headers of `da5d91a`, `std::mt19937_64` seed 20261108. Each run was under `tools/timed` with a log in the scratchpad: build about 3.2 s, run about 2.2 s. K is structural: it depends on the layouts, not the values. "tol / u" is the largest γ_(K_L) M_L + γ_(K_R) M_R in units of u. "Relative" divides it by the largest |e| of the same sample. "Rounds" is the share of exact results that need more than 53 bits, so that double must round. This is a lower bound on how often the test exercises rounding.

| Identity, algebra, layouts | inputs p / 2^b | K_L, K_R (blades) | tol / u, max | relative, max | rounds |
| --- | --- | --- | --- | --- | --- |
| ⟑ assoc., rigid(3), full<4> | b = 18 | 16, 16 (8); 29, 32 (8) | 2900 | 1068 u = 1.19e−13 | 58% |
| ⟇ assoc., rigid(3), full<4> | b = 18 | 16, 16 (8); 32, 29 (8) | 2977 | 735 u = 8.2e−14 | 58% |
| ⟑ assoc., conformal(3), full<5> | b = 17 | 64, 64 (32) | 2.87e4 | 4402 u = 4.9e−13 | 28% |
| ⟇ assoc., conformal(3), full<5> | b = 17 | 64, 64 (32) | 2.87e4 | 2345 u = 2.6e−13 | 30% |
| De Morgan, products, rigid(3), full<4> | b = 27 | 8, 8 (8); 16, 16 (8) | 227 | 195 u = 2.2e−14 | 43% to 44% |
| De Morgan, wedges, rigid(3), full<4> | b = 27 | 1, 2, 4, 8, 16, both sides, on the parts of grade 0 to 4 of a ∧ b (or their complements) | 227 | 226 u = 2.5e−14 | 25% |
| De Morgan, products, conformal(3), full<5> | b = 27 | 32, 32 (32) | 768 | 231 u = 2.6e−14 | 55% to 57% |
| De Morgan, wedges, conformal(3), full<5> | b = 27 | 1, 2, 4, 8, 16, 32 by grade, both sides | 672 | 397 u = 4.4e−14 | 29% |
| compose, rigid(3) motor ∘ motor on point | Q: b = 7; x: b = 26 | 13, 10 (1); 24, 24 (3) | 3066 | 993 u = 1.10e−13 | 24% |
| compose, rigid(3) motor ∘ motor on line | as above | 15, 14 (3); 26, 28 (3) | 5719 | 1853 u = 2.06e−13 | 29% |
| compose, rigid(3) motor ∘ motor on plane | as above | 15, 14 (3); 24, 24 (1) | 4251 | 1371 u = 1.52e−13 | 24% |
| compose, rigid(3) flector ∘ flector, motor ∘ flector on point | as above | as motor ∘ motor on point | 3529 | 1166 u = 1.29e−13 | 22% to 26% |
| compose, rigid(3) flector ∘ motor on line | as above | 15, 14 (3); 26, 27 (3) | 5661 | 2451 u = 2.72e−13 | 25% |
| compose, conformal(3), Q = h ⟇ h, on vectors | h integers in [−2, 2]; x: b = 42 | 37 to 40, 32 (5) | 4.0e7 | 2.95e4 u = 3.3e−12 | 6% |
| compose, conformal(3), Q = h ⟇ h, on bivectors | as above | 42 or 43, 42 (10) | 7.9e7 | 1.84e4 u = 2.0e−12 | 9% |

## Input sizes and the oracle's range

`tracked`'s rationals stop the program when a value leaves `std::int64_t`. For double to round, some exact intermediate must need more than 53 bits. So inputs must leave the window between 54 and about 62 significant bits. The scratch run's largest magnitude numerators were 60 bits (rigid(3) associativity, b = 18), 59 (conformal(3), b = 17; at b = 18 they reached 62, one bit from the limit), 57 and 58 (De Morgan, b = 27), 60 and 61 (rigid(3) composition), and 61 (conformal(3) composition). The sizes in the table therefore work but leave one to three bits of headroom. Another seed can overflow; the oracle then aborts loudly and does not pass silently. Two choices that do not round enough: conformal(3) composition with x at b = 30 gave no exact result over 53 bits in 1,500 coefficients. In the composition, compose's own rounding cannot be exercised together with the sandwich within this range. Its rounding is covered per operation by `rounding_test.cpp`'s antiproduct test.

## A precondition: transform's own check in conformal(3)

In a checking build `transform` over double aborts when a dropped coefficient exceeds its bound. In rigid(3) every motor and flector layout drops a part that is zero as a polynomial in V (decision 0177), so any coefficients pass and the identity holds exactly. In conformal(3) the dropped part is zero only for a true versor, and a versor composed in double can trip the check (decision 0180, first addendum: 11 in 10,000 versors of two flat planes). The test must therefore pass transform versors that are exact in double: Q1 and Q2 made exact as inputs, and compose(Q2, Q1) exact in double. Integer hyperplanes give that. In the scratch run compose's magnitudes stayed below 2^8, so every partial sum is an integer far below 2^53. The forward bound itself does not need V to be a versor, so this condition concerns only the abort.

## Not covered

- float and long double. The rule holds for any IEEE binary type with u replaced, but `tracked`'s helpers are written for double, and plan 0029 claims double only (section 10).
- Underflow, overflow, and subnormal inputs. Decision 0180's padded magnitude would be needed.
- `-ffast-math`, reassociation, or flush-to-zero; a platform whose double arithmetic is not IEEE round-to-nearest.
- Errors in forming the inputs, and versors carrying rounding error into conformal(3)'s transform (see the precondition).
- Tightness. The bound is a worst case over every rounding sign. How close the doubles come to it was deliberately not measured before the tests exist; the test's report of the largest ratio, as `rounding_test.cpp` does, will show it.
- Other algebras and layouts. K and M come from the oracle, so the rule carries over unchanged, but the numbers above are for the layouts listed only.
