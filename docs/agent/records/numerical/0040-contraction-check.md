# 0040 part E: multiply-add contraction and the rounding check of `transform`

- Written by Claude (agent, Opus 5.5), numerical-expert role, read-only derivation for plan 0040 part E, 2026-11-10.
- Branch `records/contraction-check`, from `develop` at `6670026`. Nothing under `include/` or `test/` changed. The scripts, the probe, and their outputs are in [`0040-contraction-check/`](0040-contraction-check/).
- Question (plan 0040 section 1 item 10, risk 9; advisory report R9): is "the rounding check's bound holds with or without multiply-add contraction" true, and do the checked and unchecked paths still give the same bits under contraction?
- Bears on: `transform.hpp` (the comment of `transform`, lines 761-768; the derivation comment before `transform_rounding_margin`; `kernel_roundings`, `padded_magnitude`, `margin_holds`, `transform_dropped_tolerances`); decision 0180 (Lemmas A to C, its "Contraction" paragraph, its item 1); CONVENTIONS 4.4 and 4.5; VVUQ V1, V9, V12, V14; plan 0041 section 4.6.

## Departures, skipped checks, and ambiguities (C20)

1. **An unfavorable finding beyond the question.** The comment of `transform` guarantees that "the result is the same with and without the check", and decision 0180 item 1 says the kept coefficients are bitwise those of the unchecked path. Under `-ffp-contract=fast` this is false for a floating-point scalar: with GCC 16.2.1 and Clang 23.1.4 at `-O2 -march=x86-64-v3`, a build with `GALOOT_CHECKING=1` and one with `GALOOT_CHECKING=0` gave different bits in 14.3% (GCC) and 10.3% (Clang) of 204,000 kept coefficients, and a constant evaluation differed from the same transform at run time in 43% to 49% of its coefficients (section 5). Under `-ffp-contract=off` and `on` both held exactly. The second header sentence below therefore qualifies the `Guarantees:` line, which is inside the region part E may change (lines 761-768), and decision 0180 item 1 needs an addendum by its keeper (open item K-E1).
2. **The claim that holds is about the check, not about the bits.** The bound, and the check's verdict on a versor, hold under any contraction (section 3, proved). Plan 0041's sentence "the bound holds with or without multiply-add contraction" is true with the assumptions of section 2; the wording in section 7 states them.
3. **Compiled runs.** Sixteen probe builds (GCC and Clang; `-ffp-contract=off`, `on`, `fast`, and `fast` without vectorization; checking 0 and 1), each one compiler call of 1 to 3 s, timed with `tools/timed` into this record's own `runs.tsv`, not the shared run log, as spike E's bounds record did. The pool was running and was not used (single compiler calls, not pool steps). No timing measurement was made, so the timing lock was not taken. The project's presets build for baseline x86-64, where neither compiler emits a fused multiply-add at all (checked on a two-line file); the probes add `-march=x86-64-v3` so that contraction can happen.
4. **Conformal(3) is checked less strictly than rigid(3).** For rigid(3) every compiled coefficient is compared with its exact value and its bound (`verify.py`). For conformal(3) there is no exact emulation here: the evidence is that no check fired on exact versors in any checking build, and the bit comparisons.
5. **The mechanism of the bit differences is inferred.** Vectorization explains the rigid(3) differences and shared products explain the conformal(3) ones (section 5.3); the builds without vectorization support this, but no assembly was traced.
6. **Not tested:** AArch64 and other targets, `long double` (neither tested nor claimed by CONVENTIONS 4.5), compilers other than GCC 16.2.1 and Clang 23.1.4, odd versors of rigid(3) (flectors), and the rates at which drifted conformal(3) motor chains trip the check under contraction (research note 0022's rates are for uncontracted builds).
7. **Read in part:** CONVENTIONS (4.4 and 4.5 only), WORKFLOW, PERFORMANCE and STYLE (not read; no code is changed), research note 0022 (its answer section; it says nothing of contraction). Read in full: AGENTS.md, the constitution, VVUQ, plan 0040's sections named in the brief, the advisory report's R9, decision 0180 with its addenda, the bounds record of spike E, and the knowledge base's `numerical-analysis/arithmetic-and-formats/fma-and-contraction.md`, which this record agrees with ("changes the worst-case bound only through per-term rounding counts"). No web request was made; no third party was contacted.
8. **No knowledge-base note was added** (the brief limits writes to this branch); one candidate is listed at the end.

## 1. Verdict

- **The bound holds with or without contraction: true, proved** (section 3) for every straight-line program of Lemma A's kind, so for both kernels of `transform`, its padded magnitude, and its tolerance, under the assumptions of section 2. A fused multiply-add replaces two roundings by one; its count of roundings is at most that of the two operations it replaces, its absolute error below the underflow threshold is charged to the padding of the product it absorbs, and on the magnitude side it only raises the computed magnitude. The check's K, computed for the uncontracted program, is therefore an upper bound for every contraction the compiler may choose, and the check never rejects what a versor drops.
- **The check's decision stays sound when the two paths contract differently: true, proved.** The check compares the dropped coefficients and the tolerances of the checked path with each other; the theorem holds for whatever contraction that path got, whatever the unchecked path got.
- **Checked and unchecked paths give the same bits: true without contraction across statements, false under `-ffp-contract=fast`** (measured, section 5). Each result is within the bound; they are two different, equally valid roundings of the same exact value.

## 2. Model and assumptions

T is a binary IEEE 754 format with precision p, u = 2^−p, λ the smallest positive normal number, τ the padding (λ/u in the library; any τ ≥ λ in the derivation), γ_k = ku/(1 − ku). The assumptions, each needed:

1. **Round to nearest** (any tie rule), so that rounding is monotone and every rounded real r satisfies fl(r) = r(1 + δ) + η with |δ| ≤ u, |η| ≤ λu, δη = 0 (Higham, *Accuracy and Stability of Numerical Algorithms*, 2nd ed., model (2.8), pp. 56-57, which holds for the correct rounding of any real, so for a fused result too).
2. **Each operation, fused or not, rounds once, to T's own precision and range.** A fused multiply-add computes fl(±a·b ± c) from the exact a·b ± c, as IEEE 754's fusedMultiplyAdd does. Excluded: evaluation in a wider format (FLT_EVAL_METHOD ≠ 0), and a fused operation rounded first to a wider format and then to T (double rounding), whose error can exceed u. A "fused" operation that rounds the product and then the sum is the uncontracted program and is covered.
3. **Contraction only fuses a multiplication into an addition or subtraction that consumes it.** It does not reorder sums, reassociate, or otherwise rewrite the arithmetic (those are `-fassociative-math` and the rest of `-ffast-math`, which the header already excludes). A product may be fused into one, several, or none of its consumers, and an addition of two products may absorb either.
4. **Gradual underflow** (no flush to zero, no denormals-are-zero).
5. **No overflow.** An overflowing magnitude makes the tolerance infinite and the check fails, as before.
6. **v and x are exact as given**: the bound covers this operation's roundings, as in decision 0180.

## 3. Derivation

### 3.1 The programs and what contraction does to them

Lemma A's programs (decision 0180) are straight-line programs whose nodes are inputs and exact coefficients (K = 0, P = |y|), negations and copies (exact; K and P unchanged), products (K = K_a + K_b + 1, P = P_a P_b + τ), and sums or differences (K = max(K_a, K_b) + 1, P = P_a + P_b). Every node satisfies Lemma A: |ŷ − y| ≤ γ_(K_y) P_y.

Contraction takes a sum node s = m ± z (or z ± m) whose operand m = a·b is a product node and computes it as one fused node f = fl(±a·b ± z), with a and b unrounded in the product. Nothing else changes. For the fused node define

- K'_f = max(K'_a + K'_b, K'_z) + 1, the count of roundings on its longest path;
- P_f = P_s = (P_a P_b + τ) + P_z, the padded magnitude of the uncontracted sum it replaces, unchanged.

Every other node keeps its rule, applied to the contracted operands' counts K'. The P of every node is that of the uncontracted program: it is defined exactly, over the reals, from |v|, |x|, and |c|, and contraction does not change it.

### 3.2 Lemma A′: the bound under contraction

**Claim.** For every node of the contracted program, |ŷ − y| ≤ γ_(K'_y) P_y and K'_y ≤ K_y.

By induction over the nodes, as Lemma A. Every node other than a fused one is handled exactly as in Lemma A, with K' in place of K; the steps there use only the operands' bounds. For a fused node f = fl(±a·b ± z) with exact value y = ±ab ± z, write â = a + ε_a, |ε_a| ≤ γ_(K'_a) P_a, and the same for b and z, and let k = max(K'_a + K'_b, K'_z).

- The exact operand of the one rounding: |â b̂ − ab| ≤ (γ_(K'_a) + γ_(K'_b) + γ_(K'_a) γ_(K'_b)) P_a P_b ≤ γ_(K'_a+K'_b) P_a P_b (Higham Lemma 3.3), so |(±â b̂ ± ẑ) − y| ≤ γ_k (P_a P_b + P_z).
- The rounding: f̂ = (±â b̂ ± ẑ)(1 + δ) + η, so |f̂ − y| ≤ (1 + u) γ_k (P_a P_b + P_z) + u |y| + λu ≤ ((1 + u) γ_k + u)(P_a P_b + P_z) + λu, using |y| ≤ P_a P_b + P_z.
- (1 + u) γ_k + u = (k + 1) u / (1 − k u) ≤ γ_(k+1), and λu ≤ γ_1 λ ≤ γ_(k+1) τ since τ ≥ λ and k + 1 ≥ 1.
- So |f̂ − y| ≤ γ_(k+1) (P_a P_b + τ + P_z) = γ_(K'_f) P_f.

Here η is not zero, as it is for a plain sum: a fused result that lands below λ is in general inexact. It is charged to the τ of the product node the fused operation absorbed, which in the uncontracted program paid for that product's own η. A product fused into several consumers is counted in the P of each, with its τ, so each fused node has its own τ to pay for its own η.

For the counts: the replaced sum had K_s = max(K_a + K_b + 1, K_z) + 1, and by induction K'_a ≤ K_a, K'_b ≤ K_b, K'_z ≤ K_z, so K'_f = max(K'_a + K'_b, K'_z) + 1 ≤ K_s. Every other rule is monotone in its operands' counts. ∎

So for every dropped coefficient of a versor (e = 0), |d̂| ≤ γ_(K') P ≤ γ_K P with the core's K, whatever the compiler fused, and every kept coefficient is within γ_K P of its exact value.

**K' in these kernels.** In the simulation K' never fell below K (section 4). This is expected: each output is a left fold whose first term is assigned, so its product is rounded and then passes through every later addition. That path is the longest one in the first stage, where every product has count 1, and it stays the longest in the second unless a later term reads a middle coefficient with a larger count; fusing shortens other paths. Contraction never raises K, and here it does not lower it either.

### 3.3 Lemma B′: the computed magnitude under contraction

The library computes P̂ by `padded_magnitude`: per term fl(fl(|v_a| |x_b|) + τ), then fl(fl(|c| p) + τ) when |c| ≠ 1, then fl(out + p). Contraction can fuse only a product into the addition that consumes it: fl(|v_a| |x_b| + τ) and fl(|c| p + τ). (The accumulation fl(out + p) has no product operand.) Lemma B shows P̂_y ≥ (1 − u)^(K''_y) P_y with K'' the count of roundings of the magnitude program, at most two per product and one per sum, so K'' ≤ 2K. Under contraction:

- every computed value is still at least τ: a fused padded product fl(z + τ) with z ≥ 0 is at least fl(τ) = τ by monotonicity, so none is subnormal;
- a fused padded product rounds a real r = z + τ ≥ τ ≥ λ, so fl(r) ≥ (1 − u) r, where the uncontracted pair gave (1 − u)² r: the factor only improves;
- the number of roundings on every path only falls, so P̂_y ≥ (1 − u)^(2K_y) P_y still holds. ∎

If a compiler fused a product into an accumulation (the code gives it none to fuse), the same holds: r ≥ out ≥ τ.

### 3.4 Lemma C and the comparison

Lemma C does not involve the arithmetic. The tolerance is tol = fl(g P̂) with g = fl(k u / (1 − k u)), k = K + 1. Contraction can touch only 1 − k·u, which it may compute as one fused operation; k·u is exact (k times a power of two) and 1 − k u is exact in T under Lemma C's hypothesis, so the fused and unfused forms give the same value. The division and the product g·P̂ have no addition to fuse into. So g ≥ (1 − u) γ_(K+1), g P̂ ≥ (1 − u)^(2K+1) γ_(K+1) P ≥ γ_K P ≥ γ_(K') P ≥ |d̂|, and monotone rounding gives |d̂| ≤ tol, exactly as in decision 0180. The margin, `margin_holds`, and the `static_assert` are unchanged, since they use the core's K, which is at least K'.

**Theorem.** Under the assumptions of section 2, for every contraction the compiler may apply to the first kernel, the dropped kernel, the padded magnitude, and the tolerance, independently of one another and of the unchecked path, the check passes for every dropped coefficient of a versor whose magnitude does not overflow. The check's soundness never depends on the checked and unchecked paths contracting alike.

### 3.5 Same bits

Without contraction, R9 found the two paths compute each kept coefficient by the same terms, in the same order, with the same `first` marks, so the same bits; that argument uses only the order of operations. With contraction it breaks: whether a product is fused, and into which addition, is the compiler's choice at each site, and the checked and unchecked paths are different code, with different neighbors (the dropped kernel, the magnitude) and different uses of shared values. Nothing in the language ties the two choices together, so the kept coefficients of the two paths are two roundings of the same exact value, each within γ_K P, and need not be equal. A constant evaluation is a third: the compiler evaluates it operation by operation, as written (inferred; observed in section 5, where it agrees with run time under `off` and `on` and not under `fast`).

Under `-ffp-contract=on`, which fuses only within one expression, the kernels give the compiler nothing to fuse: `run_product` computes each product as the argument of `accumulate`, and `accumulate` adds it in its own statement (a coefficient other than ±1 multiplies in a separate declaration). So `on` leaves the kernels' arithmetic as written (inferred from the code and the rule; observed in section 5). Only `padded_magnitude`'s `static_cast<T>(static_cast<T>(x * y) + padding)` is one expression that `on` may fuse, which section 3.3 covers and which does not change the transform's result.

## 4. The simulation in exact arithmetic (`sim.py`)

Rigid(3) motors acting on points, lines, and planes, the library's kernels (spike E's `plans.py`), every operation rounded exactly to binary64 or binary32 with gradual underflow, a fused operation as one rounding of the exact a·b + c. Four contraction patterns: `off`; `current` (every accumulation fuses its own product); `first` (the second term of each output fuses the first term's product, then as `current`, which is what GCC did in section 5); `random` (each accumulation independently). The magnitude was computed unfused, fused, and at random, with τ = λ/u and with τ = λ. Inputs: random motors and objects, every coefficient random (in rigid(3) every dropped part is zero for any v of one parity), in three regimes: ordinary (exponents −2 to 2), wide (−24 to 24, 10% zeros), and underflow (double 2^−345 to 2^−337, float 2^−46 to 2^−40, so that second-stage products straddle λ). 400 cases per format, object, and regime, seed 20261110 (`sim.txt`).

| Check | Result |
| --- | --- |
| K' ≤ K for every coefficient and pattern | 230,400 coefficients, none above K; K − K' was 0 throughout (section 3.2) |
| \|ŷ − y\| ≤ γ_(K') P, P with τ = λ | no exception; largest ratio 0.754 (double point, underflow), otherwise at most 0.53 |
| \|d̂\| ≤ tol (every pattern of the kernels × three of the magnitude × two τ) | the check never fired; largest \|d̂\|/tol 0.288 (τ = λ, underflow) and 0.285 (τ = λ/u) |
| same pattern on both paths gives the same kept bits | no difference |
| kept bits under two different patterns | differ in 16% to 42% of coefficients, by regime |

## 5. The compiled probe (`cprobe.cpp`, `build.sh`, `compare.txt`, `verify.txt`)

GCC 16.2.1 and Clang 23.1.4, `-std=c++23 -O2 -march=x86-64-v3`, checking 0 and 1, 3,000 cases per family and scalar (seed 7), double and float. Families: rigid(3) motors with every coefficient random on points, lines, planes; conformal(3) motors that are the exact antiproduct of two flat planes with coefficients k/8 (so exact versors, in a non-diagonal metric) on vectors and on bivectors; one random conformal(3) hyperplane (an odd versor) on vectors. 34,000 coefficients per family pair, 204,000 per build.

### 5.1 The bound and the check

- No checking build aborted: the check passed on every exact versor, contracted or not, on both compilers, in double and float, conformal(3) included.
- Every rigid(3) coefficient of every build (84,000 per build, 1,344,000 in all) is within γ_K P of its exact value, with P padded by λ (`verify.txt`, 96 rows PASS; largest ratio 0.571).
- `sim.py`'s `off` pattern reproduces the `off` and `on` builds bit for bit (every rigid(3) coefficient), and its `first` pattern reproduces GCC's `fast` builds without vectorization bit for bit, so the emulation models what GCC does.

### 5.2 Bits

| Comparison (coefficients whose bits differ, of 204,000) | GCC | Clang |
| --- | ---: | ---: |
| checking 1 against checking 0, `off` | 0 | 0 |
| checking 1 against checking 0, `on` | 0 | 0 |
| checking 1 against checking 0, `fast` | 29,110 | 21,024 |
| checking 1 against checking 0, `fast`, no vectorization | 10,253 (conformal(3) only) | 10,825 (conformal(3) only) |
| `on` against `off` | 0 | 0 |
| `fast` against `off` | 101,171 | 95,741 |
| GCC against Clang, `off` | 0 | |
| GCC against Clang, `fast` | 53,655 | |

Constant evaluation against run time (32 cases per family and scalar, 2,176 coefficients per build): 0 differences under `off` and `on`, with checking 0 and 1; under `fast`, 946 to 1,066 differ (43% to 49%), with or without vectorization.

The compilers' defaults, on a two-line file: GCC 16.2.1 fuses across statements by default for C++ under both `-std=c++23` and `-std=gnu++23` (the behavior of `fast`), and Clang 23.1.4 fuses within an expression only (`on`). On baseline x86-64 neither emits a fused operation.

### 5.3 Why the paths differ (inferred)

- **Rigid(3):** with vectorization off, GCC and Clang give the same bits on both paths for every rigid(3) family, and GCC fuses by the `first` pattern everywhere; with it on, the two paths differ for points and planes. So the vectorizer, which groups different operations in the two contexts, fuses some lanes differently.
- **Conformal(3):** in a non-diagonal metric one product of two basis blades has several output blades (e4 ⟑ e5 = −1 + e45), so one multiplication feeds several additions, and the dropped kernel shares products with the kept one. Which of those additions absorbs the multiplication, or whether any does (LLVM's combiner, as I recall it and did not check here, fuses a product with several uses only on some targets), depends on how many uses it has, which differs between the paths. This remains with vectorization off.

## 6. What is proved, checked, and untested

| Claim | Status |
| --- | --- |
| Under contraction, every node of Lemma A's programs satisfies \|ŷ − y\| ≤ γ_(K') P with K' ≤ K | proved (3.2), V1 kind 1 |
| The computed magnitude stays ≥ (1 − u)^(2K) P; the tolerance's g is unchanged | proved (3.3, 3.4) |
| The check never rejects a dropped coefficient of a versor, however each path contracts | proved (3.4) |
| The same, on rigid(3) kernels with four patterns and exact arithmetic | checked (sim.py; sampled, V9) |
| The same, compiled with GCC 16.2.1 and Clang 23.1.4, `-ffp-contract=off/on/fast`, x86-64-v3, rigid(3) exact and conformal(3) by the check | checked (sampled, V9) |
| Checked and unchecked builds, and constant evaluation and run time, give the same bits without contraction across statements | proved for `off` (the order argument), inferred for `on` from the code's shape (3.5), checked for both |
| They differ under `fast` | checked (measured, both compilers) |
| Why they differ | inferred (5.3) |
| AArch64, other compilers or versions, `long double`, flectors of rigid(3), drifted conformal(3) chains under contraction | untested |

## 7. Wording

Each sentence claims no more than sections 3 and 5 show. The header sentences follow S11 (no record cited) and name no compiler version, since plan 0040 moves version-specific figures out of the headers.

### 7.1 The comment of `transform` (`transform.hpp`, lines 761-768)

**T1**, after "...and a checking build under -ffast-math or -ffinite-math-only does not compile.":

> A multiply-add that the compiler fuses under -ffp-contract counts as one operation: fusing only removes roundings that K counts, so the bound, and the check's verdict on a versor, hold however the compiler contracts, provided the fused operation rounds once to the scalar's own precision.

**T2**, replacing the `Guarantees:` line:

> Guarantees: on x's layout; with an exact scalar the result is exact; and the result is the same with and without the check, and in a constant evaluation, when the build does not fuse multiply-adds across statements (-ffp-contract=off or on). Under -ffp-contract=fast, GCC's default on a target with fused multiply-add, the compiler may fuse the two paths differently, and their results can then differ in the last bits, each within the operation's forward error bound.

If the coordinator prefers not to claim `on`, the weaker T2 is: "...and the result is the same with and without the check, and in a constant evaluation, when the build does not contract multiply-adds (-ffp-contract=off); with contraction they can differ in the last bits, each within the operation's forward error bound."

### 7.2 The limits page (`docs/guide/limits.md`, plan 0041 section 4.6)

**L1**, for "the bound holds with or without multiply-add contraction":

> The rounding check's bound holds with or without multiply-add contraction (`-ffp-contract=off`, `on`, or `fast`), provided each fused multiply-add rounds once to the scalar's own precision: fusing a multiplication into an addition removes a rounding and never adds one, so the check never rejects what a versor drops. This is derived, and was checked with GCC 16.2.1 and Clang 23.1.4 at `-O2 -march=x86-64-v3`, where no check fired on exact versors and every rigid(3) result stayed within its bound.

**L2**, beside it:

> Contraction does change the bits. Under `-ffp-contract=fast`, which GCC uses by default for C++ when the target has fused multiply-add instructions (on x86-64, with `-mfma` or `-march=x86-64-v3` and above; other targets were not tested), a transform's result can differ in its last bits between a checking and a non-checking build (10% to 14% of coefficients in our measurements), and between a constant evaluation and run time (43% to 49%). With `-ffp-contract=off` or `on` both compilers tested gave the same bits in every case. Baseline x86-64 builds contain no fused operations.

## 8. Open items

- **K-E1 (coordinator).** Decision 0180 item 1 ("the kept coefficients are bitwise those of the unchecked path") holds only without contraction across statements, and its "Contraction" paragraph is a sketch of section 3. Options: (a) an addendum to 0180 pointing here and qualifying item 1; (b) leave 0180 and rely on the header's T2. Tradeoff: (a) keeps the record a reader consults correct at the cost of a paragraph; (b) leaves a recorded guarantee that is false under the GCC default for FMA targets. Recommendation: (a).
- **K-E2 (coordinator).** Part E's sentences: T1 and T2 together, or T1 with the weaker T2. Tradeoff: T2 as written also tells users that `on` keeps the bits, which is inferred from the code's shape and checked on two compilers but is not a language guarantee; the weaker form claims only `off`. Recommendation: T2 as written, since `on` is Clang's default and users meet it.
- **O-E1 (owner).** No test runs under contraction: every preset sets `-ffp-contract=off` and builds for baseline x86-64, where nothing would be fused anyway. Options: (a) leave it, resting the contraction claim on the derivation and this record; (b) add one preset or one CTest that builds `core.transform_rounding` with `-march=x86-64-v3 -ffp-contract=fast`. Tradeoff: (b) guards the claim against a later change to the kernels or the check (for example a collected plan) at the cost of one more build in the matrix, and its bitwise assertions, if any, would have to tolerate the differences of section 5; (a) costs nothing and the proof does not depend on the compiler. Recommendation: (a) for 1.0, with (b) noted for the M6 revisit of the check.
- **For the documentation lane:** plan 0041 section 4.6's sentence should be L1, and L2 belongs beside it (V14: variation from fused operations is declared).

## 9. Reproduction

From a checkout root, with `<env>` = `/home/brianm/projects/utils/math-core/math-tools/toolset/env/py/.venv` (Python 3.14.9, python-flint 0.9.0):

    <env>/bin/python -B docs/agent/records/numerical/0040-contraction-check/sim.py --cases 400     # about 60 s; writes sim.txt
    bash docs/agent/records/numerical/0040-contraction-check/build.sh <scratch dir> <env>/bin/python  # 16 compiles, runs, about 3 min; writes compare.txt, verify.txt

Each Python script exits 1 on a failed check. The compiles and the simulation are timed in `runs.tsv` beside this file.

| File | What |
| --- | --- |
| `sim.py`, `sim.txt` | the kernels in exact arithmetic under four contraction patterns; sections 4 and 3.2 |
| `cprobe.cpp` | the probe: seeded transforms in rigid(3) and conformal(3), double and float, and constant against run-time cases |
| `build.sh` | the 16 builds, their runs, and the two comparisons |
| `compare.py`, `compare.txt` | bit comparisons between builds; constant evaluation against run time |
| `verify.py`, `verify.txt` | every rigid(3) coefficient of every build against its exact value and bound, and against the emulated patterns |
| `runs.tsv` | the timed compiles and runs |

## Candidate knowledge-base note (not written)

- Contraction and forward error bounds of straight-line sums of products: a fused node's count is max(K_a + K_b, K_z) + 1, never more than the pair it replaces, and with gradual underflow its absolute error is charged to the padding of the absorbed product; but two compilations of the same source, or a constant evaluation and a run-time one, can then round differently, so "same bits" guarantees need contraction across statements off.
