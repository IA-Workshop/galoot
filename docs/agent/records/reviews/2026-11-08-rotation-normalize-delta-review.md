# Delta review of plan 0024: the fixes for M1 and L1 to L5

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5). Date: 2026-11-08.
- Range: `git diff 70e08cd..f97c68f` on `feature/rotation-normalize`, the fixes for the findings of `2026-11-08-rotation-normalize-review.md`. Record branch `records/review-rotation-delta`, worktree `.claude/worktrees/review-rotation-delta`, from `f97c68f`.
- The brief carried no verdict. It gave the authors' commit list and departures, which were checked, not relied on.

## Departures, skipped checks, ambiguities (C20)

- Authors' departures: STYLE S9's failure-value sentence not applied (finding D1 below); two early filtered runs did not go through `tools/timed`; five unpushed commits were replayed, so `runs.tsv` names pre-replay hashes (`0afa69c`, `8219d78`) that are not on the branch. Because of the last two, V12's order (tolerance before first run) is checkable only from commit times and the logged runs, not proved.
- Not run here: sanitizer presets, `lint`, `clang-measure`, the full suites, `tools/runlog check`. They remain P9 merge checks.
- Not rerun: the "fails alone" claims for `8557c3d` and `6047355`. `runs.tsv` shows exit 8 for both at their pre-replay hashes.
- The double case of the 1u lemma was checked by proof and sampling, not exhaustively.
- `96153ff` and `d881ec7` are outside the range. Only decision 0203 item 6 was read from them.
- The run rows from this review are left uncommitted in this worktree's `runs.tsv`, since the brief allows only this record to be committed.

## What was checked, and how

- **No core diff.** `git diff --stat 70e08cd..f97c68f -- include/galoot/core include/galoot/core.hpp cmake CMakeLists.txt tools` prints nothing.
- **The 1u lemma** (`rotation-normalize-tolerance.md`, "A tighter bound"). I checked the proof by hand. The quotient q lies in [1/16, 1/4), so |q − ŝ/5| ≤ u/8 and |5q − ŝ| ≤ 5u/8 < 1 − ŝ + 5u/8. If 5q ≥ 1/2, the result is within 9u/8 on a grid of spacing u, so the error is at most u. If 5q < 1/2, it is within 7u/8 on a grid of spacing u/2, so the error is at most u/2. The proof holds for any binary format with round to nearest. My own program found a maximum of exactly 1u over every float with |s| in [1/2, 1), both signs. For double it found 1u over 10⁸ random values plus the 2²⁴ values at each end of [1/2, 1).
- **Tolerances.** I recomputed them in `decimal` at 80 digits from the model in `rotation-tolerance.md`. With a = 10u, double gives 39.42, 38.87, 0, 31.87 and float gives 39.60, 38.87, 0, 31.87, so 40, 39, 0, 32 for both. The rows a = 9u and a = 11u reproduce the old values. The fifths bounds come out at 2.33, 4.65, 7.10, 9.30, which give 3, 5, 8, 10. I checked by hand that both lines are lines (v · m = 0) and that their fifths are right. The expected values are exact fifths, independent of the formula. In the diff, only the length-5 tuple and the same-motor bound (from 2u to u) change; `quarter_turn_tolerance_in_u` and every other tolerance are textually unchanged. No check is weakened. The same-motor test's angles are guarded by `REQUIRE(|ŝ| in [1/2, 1))`, the lemma's hypothesis.
- **V12.** The record `9631f38` was committed at 11:07:02Z. The test commit `0ae3144` followed at 11:07:38Z, and its first logged run at 11:08:11Z. `bfbc131` was authored at 11:08:25Z, after the record.
- **The assert.** It uses `GALOOT_ASSERT` from `core/assert.hpp`, as `linear.hpp`, `transform.hpp`, and `metric.hpp` do, and the message starts on the macro's line. `std::isnormal(length)` is correct: a square root of a non-negative double or float is never subnormal, so the check rejects exactly 0, ∞, and NaN. Assembly from `-DGALOOT_CHECKING=0 -O2` is identical to the pre-assert header (`6047355`) on g++ and clang++, and also at clang++ `-O0`.
- **The mode tests are not tautological.** The aborting tests require `Subprocess aborted`, the full message, and `motions.hpp:`. The unchecked tests require the documented NaN, ∞, or 0. I ran five mutants on both debug presets, and each was killed by the expected tests: `isfinite`, `length > 0`, the assert removed, the message changed, and the moment left undivided (killed by `rga3.motions`). The header was restored afterward.
- **L2.** The message and the `rotation_no_sin` literal match character for character. `rga3.rotation_integer_scalar` and `rotation_rational_scalar` pass on both compilers.
- **README.** I verified the claim algebraically. With v = p_w q⃗ − q_w p⃗, v = 0 exactly when both weights are 0 or one point is a multiple of the other. The checking and unchecked statements match the tests.
- **Runs**, from fresh trees at `f97c68f`, through `tools/timed`, with the pool at 23 of 23 free before and after:
  - gcc-debug and clang-debug: `-L '^rga3'` 29/29 and `tests.*` 5/5.
  - gcc-release and clang-release (`GALOOT_CHECKING` OFF): a full build, then `-L '^rga3'` 29/29.
  - format-check passes. The trace gate passes, with 6.1 to 6.4 untested, as on `develop`.

## Findings, most severe first

- **D1 (medium; needs an owner ruling, blocks the merge only until then).** STYLE S9: "An operation whose input can be degenerate, such as a zero norm, … states the degenerate set and returns a failure value on it." In my reading this sentence applies: the degenerate input here is the zero norm, S9's own example. `rotation` returns a plain `Motor`, and in an unchecked build `rotation(Line{p ^ p}, t)` silently gives NaN. Decision 0203 item 6 records that the sentence was not applied and leaves it to the owner. The library has no `std::optional` or `std::expected` result yet, so applying the sentence would set a precedent and change the signature.
- **L1 (low).** `motions.hpp:56-57` says the unchecked check "costs nothing". That holds at `-O2` on both compilers. At g++ `-O0`, a call to `checking_now()` and a branch remain, as with every `GALOOT_ASSERT`. The `Cost:` line also omits the `isnormal` in checking builds. Suggested fix: "costs nothing once optimized".
- **L2 (low).** The comment's claim that "a length whose squares fall among the subnormal numbers is not caught" has no test. An example is (1e−160, 0, 0) in a checking build.

## Outcome

Approve, with L1 and L2 non-blocking, once the owner rules on D1. All of M1's assert part, L1 to L5, and the nit are resolved correctly.

## Resolution (added by the implementer after the review)

- **D1.** Ruled by the owner on 2026-11-08 ("confirmed the recommendations"): the checking-build assert stays and the result stays a plain `Motor`; the unchecked NaN is a ratified departure from S9's failure-value sentence, recorded in decision 0210 and stated in the `rotation` comment.
- **L1.** The Cost line names the `std::isnormal` test and its branch in checking builds, and the unchecked sentence says an optimizing build drops the check while without optimization a call and a branch remain. Comment only.
- **L2.** Reworded: such a length passes the check, and no test covers the case. Comment only; no test added.
- These are changes to comments only, so they get no delta review (W5); decision 0210 lists them.
