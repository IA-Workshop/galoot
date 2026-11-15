# W5 delta review of WU13b: the fixes after the blind review

- Reviewer: Claude (agent, Opus 5.5), blind-reviewer role, on 2026-11-07. I am the reviewer of `2026-11-07-wu13b-review.md`, and I share no working context with the implementer or the test author.
- Range reviewed: `43355bb..c98eee2` on `feature/core-transform--rounding`. The review ran in a new detached worktree, `.claude/worktrees/review-wu13b-delta`, at `c98eee2`, with its own empty build trees. I did not edit the change.
- Brief: the coordinator named the range and the findings to check, B1 and N1 to N9. It carried no verdict on the change, and its brief quoted the implementer's cost figures, which I measured myself.
- Outcome: **approve with non-blocking findings.** B1 is fixed. N2, N5, N6, N7, and N9 are resolved. N1, N3, N4, and N5's reading go to the owner. The delta raises three new non-blocking points, D1 to D3, plus a note:

- D1: the float figure of the raised tolerance floor is undocumented.
- D2: the plan's job-pool statement is stale.
- D3: a test gap that the mutant `gate_removed` exposes.
- The note: no test pins that unchecked builds still compile under `-ffast-math` or `-ffinite-math-only`.

The header itself is correct in every case I checked.

## Findings checked

### B1: fixed

`transform.hpp:108-113` adds an `#elif` on `__FINITE_MATH_ONLY__ == 1 && GALOOT_CHECKING == 1`, with its own message. My reproduction of the first review's cases (`transform_rounding_assert_fail.cpp`, `-O2 -ffinite-math-only`):

| Compiler | `GALOOT_CHECKING=1` | `GALOOT_CHECKING=0` |
| --- | --- | --- |
| g++ 16 | stops with "transform: checking mode needs NaN and infinity, not -ffinite-math-only" | compiles |
| clang++ 23 | stops with the same message | compiles |

Under `-ffast-math` the `-ffast-math` message comes first, as intended. `core.transform_finite_math_only` requires the new message and forbids the other one. `core.transform_fast_math` now forbids the finite-math text, and `core.transform_without_fast_math` forbids both. All three pass in my runs on both compilers.

### N2: the padding λ/u is sound; its cost to sensitivity is real but small, and only half-documented (D1)

- **Rederived by hand.** For any padding τ ≥ λ:
  - Lemma A: a product's |η| ≤ λu ≤ γ_1 λ ≤ γ_K τ, which its padding covers.
  - Lemma B: fl(fl(z) + τ) ≥ (1 − u)((1 − u) z − λu + τ) ≥ (1 − u)²(z + τ), because uτ ≥ uλ. The padded values are at least τ(1 − u)², which is normal.
  - Lemma C does not involve τ.
  - τ = min() / (epsilon() / 2) is 2^-969 for double and 2^-102 for float, exact.
- **My own simulation.** I used my `sim.py` with only the padding changed to λ/u (`2026-11-07-wu13b-delta-review-probes/sim_tau.py`), against kernels I dumped from `c98eee2`'s header.
  - It reproduces the core's d and tolerance bit for bit on all 11 transforms.
  - Over fresh seeds 101, 202, 303, and 404, 6,600 inputs each in the six regimes (26,400 in all), it found **0** violations of |d − e| ≤ tol.
  - The additive bound γ_K M + K·denorm_min still fails, 5,222 to 5,240 times per seed.
- **The floor.** The tolerance's absolute floor rises from about γ_K λ W to about K λ W. I measured how far down a gross non-versor is still detected, against the headers of `43355bb` (λ) and `c98eee2` (λ/u), with every input scaled by s (`2026-11-07-wu13b-delta-review-probes/floor.cpp`):

| Case | float, λ | float, λ/u | double, λ | double, λ/u |
| --- | --- | --- | --- | --- |
| R(4,0,0), V = s(1 + e1234), x = s, dropping 2s³ | s ≥ 2^-49 | s ≥ 2^-41 (4.5 × 10^-13) | s ≥ 2^-357 | s ≥ 2^-339 (10^-102) |
| conformal(3) flat-plane motor with one component 1% off, on a vector | s ≥ 2^-43 | s ≥ 2^-35 (2.9 × 10^-11) | s ≥ 2^-352 | s ≥ 2^-334 |

  The blind region grows by u^(-1/3) in input scale, which is 2^8 in float and 2^18 in double.
  - **Double:** negligible.
  - **Float:** a program whose versor and object are both near 10^-11, or with |v|² |x| below about 10^-33, loses the check of what it drops. That scale is unusual but not absurd in float.
  - **Before as after:** no tolerance can see a dropped part below rounding noise at that scale, so the change moves an existing limit rather than creating one.
  - **Judgement:** acceptable, given the four-fold wall-time gain.
  - **But (D1):** decision 0180's addendum item 3 and the plan give the floor for double only ("near 10^-306"). Neither the `///` comment of `transform` (`transform.hpp:715-723`) nor the comment beside `padded_magnitude` (`:491-499`) says that dropped parts below that floor are not seen. Float's figure is the one users could meet.
- **The cost claim checks out.** It is measured below.

### N5: resolved by gating; owner item confirmed

`detail::transform_rounding_checked<Alg, V, X, T>` (`transform.hpp:529-558`) is `rounding_checked_scalar<T>` plus `margin_holds<T>(K)` for every dropped coefficient. `margin_holds` computes (K + 1)(2K + 1) < 2^digits in unsigned 64-bit integers, short-circuited for digits ≥ 62. That is exact. Since (K + 1)(2K + 1) is odd, `<` and `≤` agree.

My probe (`2026-11-07-wu13b-delta-review-probes/gate.cpp`, `GALOOT_CHECKING=1`, both compilers) covered rigid(3) motor on point and on line, flector on line, the conformal(3) motor and flector on a vector, and R(4,0,0) `full<4>` on vectors:

| Scalar | Checked |
| --- | --- |
| float, double, long double | every case, on both compilers |
| std::float16_t (GCC) | every case |
| std::bfloat16_t (GCC) | no case |

- `bf16_use.cpp` compiles again in both modes on GCC. Clang 23 here has no `std::bfloat16_t`; the test's `coarse` scalar pins the gate on both compilers.
- A bfloat16 transform whose margin does hold is still checked. My first probe's R(4,0,0) non-versor with K = 3 failed its constant evaluation on GCC, as it should.
- **Nothing that was checked is now unchecked.** The gate is the old concept plus the margin, and a concept-true type whose margin failed did not compile at `43355bb`. So every transform that compiled and was checked before is still checked, and only the newly compiling ones are unchecked.
- **Owner item, confirmed.** CONVENTIONS 4.5 says the IEEE binary types that round to nearest are checked. For bfloat16 and narrower types this is narrower, and the addendum and plan put it to the owner.

### N6 and N1: resolved in the comments; N1 stays with the owner

- **The comment at the check** (`transform.hpp:591-610`) names the four causes and a remedy for each, with my chain rates.
- **The `///` comment of `transform`** gives the rates and the remedy ("rebuild or renormalize v so that it is a versor again (scaling alone does not), or turn checking off"). It says that coarse types are not checked and that `-ffinite-math-only` does not compile.
- **The record:** the message text is now in decision 0180's addendum item 4.
- **The rates are quoted correctly**, from my record: 0.2%, 2.2%, 11.4%, and 22.7%.
- **The owner's question stands:** whether O6 stands in the light of these rates.
- **Note:** library comments that quote a seeded sampling result will age. That is acceptable as a documented example.

### N9: both survivors now die; fresh mutants

Mutants of `c98eee2`'s `transform.hpp`, each alone, on gcc-debug and clang-debug. Each run builds the four judging targets, runs `ctest -L core.transform`, restores the header, and rebuilds before the next mutant (`2026-11-07-wu13b-delta-review-probes/mutants.py`).

A build failure counts as a kill only when it is a test's own `static_assert` or a constant evaluation that a test asks for.

| Mutant | Killed by (gcc-debug and clang-debug) |
| --- | --- |
| `size_signed`: \|d\| replaced by d (first review's survivor) | `core.transform_rounding_consteval_negative_double` and `_float`, `core.transform_rounding_checking_negative_double` and `_float` |
| `middle_magnitude_restricted`: the magnitude's first stage uses the restricted kernel (first review's survivor) | `core.transform_rounding`, the degenerate-form family |
| `margin_always_holds`: `margin_holds` returns true | the coarse scalar's `static_assert` (`transform_rounding_test.cpp:1175`); the mutant also leaves two unused variables, which `-Werror` rejects in the header, so the compile-fail tests failed on Clang too, a warning and not a kill |
| `margin_product_squared`: (K + 1)² instead of (K + 1)(2K + 1) | the same `static_assert` (`:1175`; on GCC also `:1197`) |
| `finite_math_guard_removed` | `core.transform_finite_math_only` |
| **`gate_removed`: the outer `if constexpr` of `transform` (`transform.hpp:771-772`) back to `rounding_checked_scalar`, with the inner one still gated** | **survives (D3)** |
| `finite_math_guard_ignores_checking`: the `#elif` without `GALOOT_CHECKING == 1` | survives (the note under the new findings) |
| `padding_back_to_lambda` | survives; equivalent, since both paddings are sound, as the plan records |

**D3, `gate_removed`.**

- **What the mutant does.** With only the outer gate reverted, a coarse scalar in checking mode takes the checking path, and its inner gate then sends it to the exact check, a comparison with zero.
- **Why the tests miss it.** The coarse and bfloat16 tests (`transform_rounding_test.cpp`, the test case "transform checks a rounding scalar only where its margin suffices") use small integer inputs whose dropped parts are exactly zero even in coarse arithmetic, so they cannot tell "not checked" from "checked exactly".
- **The consequence, confirmed.** I built a scratch copy of `c98eee2`'s header with the mutant and ran 1,000 random rigid(3) motors on points over `std::bfloat16_t` with `GALOOT_CHECKING=1` (`2026-11-07-wu13b-delta-review-probes/bf_rand.cpp`). The correct header runs them all. The mutant aborts with "transform: v does not act as a versor on x".
- **Suggested fixes.** Give the coarse tests inputs whose dropped parts round to nonzero, and require no abort in a checking build. Or make the two `if constexpr` conditions one named constant, so that they cannot disagree.

### N7: resolved

- The float abort cases are added: `ten_times_float`, `nan_float`, `infinite_float`, `overflow_float`, and `negative_float`, which pass in my runs.
- Criterion 3's scaling is now stated as a reading.
- `tenth` over float is not added, and the reason is given (test author's departure).

### N3 and N4: unchanged, owner items

These are as in my first record. The `-ffinite-math-only` error widens N4's breadth: all of `core.hpp` is unusable in a checking build under that option too.

## Process

- **fbd8a11: acceptable.** It combines the test author's `bc0ff87` and its fix `61ba798`.
  - **Content:** the test diff of `fbd8a11` equals `bc0ff87~1..61ba798` exactly. Both `-x` trailers are kept, and the message says that it fails to build until the code exists and explains the combination.
  - **The reset:** the replaced cherry-pick, `be53d19`, is contained in no ref (`git for-each-ref --contains` gives 0). It lived about three minutes, from 12:09:28 to 12:12:27 -05:00 by the worktree's reflog, in the subfeature's own worktree, and nothing had merged it. So the reset rewrote nothing shared (W7), and it avoided two failing commits in a row (W2), the pattern the WU13a review flagged.
  - The plan's departure 3 records it. I judge it acceptable. The project may want the combination recorded as standing practice next to decision 0185, rather than case by case.
- **e188522 and bc06e9e: the implementer's code.**
  - `bc06e9e` changes only the signedness of `margin_holds`'s arithmetic, with the same values for every reachable K.
  - Neither commit changes a test.
- **The test changes are the test author's (fbd8a11):**
  - the negative non-versor (program and constant evaluation, double and float);
  - the degenerate layout of `f57800a` in `tolerance_family`, which requires the core's K to equal `tracked`'s and the tolerance to lie in [γ_K M, γ_(K+2) M) for any input;
  - the coarse scalar and the bfloat16 tests;
  - float versions of the abort cases;
  - the finite-math tests.

  None changes an existing expected value. `core.transform_fast_math` gains a FORBID of the new text, which the plan lists as departure 4. Fine.
- **Decision 0180's addendum: its claims match what I ran or read.**
  - Item 1 matches my reproduction.
  - Item 2 describes the gate as implemented.
  - Item 3's derivation agrees with mine. Its simulation claim (seeds 7 and 11, largest ratio 0.318) I did not rerun; I ran fresh seeds instead.
  - Item 4 gives the message, and item 5 quotes my figures correctly.
  - Gap (D1): float's floor.
- **The cost README's correction is honest.** It names the earlier inference as wrong and gives the assist counts. Its figures match my measurement below.
- **Plan outcome: accurate against the diff, with one stale claim (D2).** It says "The job pool read 23 of 23 slots free before and after each heavy run". When I started, the pool read 21 of 23 free with no client: 2 slots lost, by `tools/jobpool status`. I cannot tell when they were lost, so the claim may have held at the implementer's runs. It now needs an owner restart, which I did not do, as instructed.

## Counts from empty trees

At `c98eee2`, clean tree, each step through `tools/timed`. Jobpool ensure only. The pool read 21 of 23 free with no client before and after; I did not restart it.

| Preset | Tests | Build wall | Test wall |
| --- | --- | --- | --- |
| gcc-debug | 403 of 403 passed | 46.1 s | 9.3 s |
| clang-debug | 405 of 405 passed | 43.5 s | 5.8 s |
| clang-measure | 397 of 397 passed | 66.6 s | 3.9 s |

These are the expected counts, with no failed build step.

## Cost measured

Setup:

- The headers of `c98eee2`, `-O2 -ffp-contract=off`, 10^7 iterations.
- Pinned to CPU 4, a performance core, with its sibling idle (`sibling_busy_s` 0.00).
- Under the timing lock, with no build running. The pool read 21 of 23 free with no client.
- Each program ran once through `tools/timed measure`, then once under `perf stat` with `assists.fp`, cycles, and instructions, unmultiplexed at 100%.

| Program | Instructions | Cycles | FP assists | Wall |
| --- | --- | --- | --- | --- |
| `cost.cpp`, GCC, unchecked | 2,292.6 M | 0.538 G | 0 | 0.195 s |
| `cost.cpp`, GCC, checking | 24,472.6 M (10.7×) | 5.211 G (9.7×) | 0 | 0.930 s |
| `cost.cpp`, Clang, unchecked | 2,322.6 M | 0.515 G | 0 | 0.129 s |
| `cost.cpp`, Clang, checking | 21,882.6 M (9.4×) | 4.843 G (9.4×) | 0 | 0.881 s |
| my `cost2.cpp`, GCC, checking, inputs with zeros | 24,212.6 M | 4.856 G | 0 (was 110,000,000) | 0.865 s (was 3.41 s) |
| my `cost2.cpp`, GCC, checking, every input nonzero | 24,212.6 M | 4.609 G | 0 | 0.831 s |

- **Confirmed:** the instruction ratios, 10.7× and 9.4×, unchanged; 0 assists; and the cycle counts the README gives.
- **The wall-time ratio.** The README's "about 6.7 times as slow" is a ratio of measured wall times. That ratio includes each run's fixed start-up, which weighs most on the 0.13 s unchecked run. I measured 4.8× (GCC) and 6.8× (Clang) wall. By cycles, the per-transform slowdown is 9.7× and 9.4×, which the README's cycle figures also imply.
- **Judgement:** the claim is honest as stated. A reader comparing per-call cost should take the cycle ratio, about 9.5×.
- **Assists gone:** the zero-rich and dense inputs now cost the same within 5%.

## New non-blocking findings

- **D1.** The floor's float figure, and a user-facing sentence on it, are missing (`transform.hpp:491-499` and `:715-723`, decision 0180 addendum item 3).
  - Scenario: a float program whose inputs are near 10^-11 has a non-versor's dropped part pass the check unseen, and the documentation gives no hint of the scale.
  - Fix: one sentence in the comment, and float's figure in the addendum: a dropped part below about (K + 1) λ W, near 10^-36 in float (my R(4,0,0) case lost detection below 2^-122, about 2 × 10^-37) and 10^-306 in double, is not seen.
- **D2.** The plan's pool statement is stale for the present state of the pool. See Process.
- **D3.** The coarse-type tests cannot tell "not checked" from "checked exactly" (`transform_rounding_test.cpp`, the test case "transform checks a rounding scalar only where its margin suffices"; `transform.hpp:772` and `:780`). See the mutant table above.
- **Note.** Both `-ffast-math` guards also tie themselves to `GALOOT_CHECKING == 1`. No test pins that an unchecked build under `-ffinite-math-only` or `-ffast-math` still compiles, so a guard that drops that condition survives (mutant `finite_math_guard_ignores_checking` below). I compiled `transform_rounding_assert_fail.cpp` unchecked under each option on both compilers, and all four build. A compile test would pin it.

## Not checked

- gcc-release, gcc-asan, clang-release, clang-asan, and lint at `c98eee2`.
- The build-cost rows (+0.2% to +0.5%).
- The implementer's 22 mutants.
- Decision 0180 addendum's seeds 7 and 11.
- `std::bfloat16_t` on Clang, which is unavailable here.
- `long double`'s rounding behavior (x87, out of scope); I checked only its gating.
- The cause of the flat versus general difference in N1.

## Reviewer's cost

About 45 tool calls and 12 minutes of wall time for the delta, from 18:16 to 18:28 UTC by the run log, plus the time to write this record. The harness reports the tokens; I cannot measure them. Machine time: the three builds and tests from empty trees took about 2.5 minutes, and the 16 mutant runs with their restore builds about 6 minutes. I sent no web requests and made no upstream contact.
