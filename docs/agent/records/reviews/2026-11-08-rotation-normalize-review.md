# Blind review of plan 0024: `rotation(l, theta)` normalizes its axis

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5), with the usability checklist. Date: 2026-11-08.
- Range: `git diff develop...feature/rotation-normalize`, `develop` at `bd60589`, branch tip `70e08cd`. Record branch `records/review-rotation-normalize`, worktree `.claude/worktrees/review-rotation-normalize`, from `70e08cd`.
- The brief carried no verdict. It gave the authors' W9 list and departures, which were checked, not relied on. Decision 0203 and the plan's state section, both part of the change, assert that their checks passed. Every check below was rerun here.

## Departures, skipped checks, ambiguities (C20)

- Not run here: the sanitizer presets, release presets, `clang-measure`, `lint`, `tools/runlog check`, and the full suites. P9 requires these for the merge into `develop`, and they remain the merging agent's job. The authors also ran no sanitizer preset.
- Ran the brief's selection only: `-L '^rga3'` (19 tests) and `-R '^tests\.'` (5) on gcc-debug and clang-debug, plus format-check and the trace gate.
- The brief names `records/numerical/rotation-tolerance.md`. The derivation for the new tolerances is in `rotation-normalize-tolerance.md`, which extends it. Both were read.
- About 60 run rows from this review are in this worktree's `runs.tsv` and are not committed, because the brief asks this branch to commit only this record.
- The authors' note that `norm_without_z` made doctest skip four test cases was not investigated here either.

## What was checked, and how

- **Zero core diff.** `git diff --stat develop...HEAD -- include/galoot/core include/galoot/core.hpp cmake CMakeLists.txt test/CMakeLists.txt tools` prints nothing.
- **Normalization of the whole line.** `motions.hpp:61-74` computes |v| = sqrt((v0² + v1²) + v2²) from e41, e42, e43, and multiplies all six line coefficients by s = sin(θ/2)/|v|. That is (l/|v|) sin(θ/2) + 𝟙 cos(θ/2), with the direction and the moment both divided. cos is not divided, and e1 stays 0.
- **Tolerances.** Recomputed independently in `decimal` at 80 digits, from the model of `rotation-tolerance.md` with a = (k + 2)u, b = ku, k = 9. Double gives 40.836, 40.284, 0, 33.284 (41, 41, 0, 34), and float gives 41.018, 40.284, 0, 33.284 (42, 41, 0, 34). Both match the record and the test. The 2u lemma was checked exhaustively for every float in [1/2, 1) (maximum 1u) and on 5·10⁷ random doubles (maximum 1u). The record `da55433` (05:38:57) predates the test `5165641` (05:41:31). The later edit to the record, `9b0a6b6`, changes only the prose on the safe range.
- **Unit axes are unchanged bit for bit.** Programs built against `develop`'s and the branch's headers, on g++ and clang++ at `-O0 -ffp-contract=off`, compared the motors of the six original axes over 13 angles. They also compared 2000 random axis-aligned unit lines through random points, in float and double. Every byte was identical on both compilers. The unit quarter-turn test and its tolerances are textually unchanged. The only removed check is the lookup test's `Line{}` call (`03c678f`), and its replacement expects the same motor.
- **W9.** Each tests-branch commit has the same patch-id as its cherry-pick (5165641/ed94110, a09fd1e/ff758b0, c86a68c/03c678f, f5fff61/5e38e75, 53c3464/451e03f). The header changes are in separate commits, `3ab7f85` and `4577f85`. The two corrections after the first run (signed zeros in the reference lines, and the zero lookup line) are recorded in decision 0203, item 4. Neither weakens a check: both stay bit for bit.
- **Mutants.** The 7 mutants in `mutate.py` are real semantic changes. They were rerun from a scratch copy on both presets, with 3 of my own added: `cos_divided`, `length_abs_vz` (|v_z| as the length), and `length_l1` (the 1-norm). All 10 were killed on both compilers by failed checks (test exit 8), none by a build error. The header was restored afterward (`git status` showed only `runs.tsv`).
- **CONVENTIONS 1.13 against decision 0203.** The 5.7 line is character for character the text that decision 0203 item 2 quotes. The status line and the 1.13 row say that the wording awaits the owner's confirmation. The trace row is pinned at `motions_test.cpp:335` and `:498`.
- **Agent notes in human-facing files.** No decision, plan, provision, or agent citation was added to README.md, `include/`, or `test/`.
- **Runs**, from fresh trees in this worktree at `70e08cd`, through `tools/timed` (pool 23 of 23 free before and after). gcc-debug: rga3 19/19, tests 5/5. clang-debug: rga3 19/19, tests 5/5. `rga3_motions_test` ran 15 cases and 467 assertions on each. Format-check and the trace gate passed (items with no test: 6.1 to 6.4, as on `develop`).

## Usability checklist

Three tasks, from the README alone: turn a point about the line through two points, chain that turn with a move, and turn about a coordinate axis. All three can be done without reading the implementation. The signature is unchanged, `auto r = rotation(...)` is a plain `Motor`, and no name changed. The items not met are L1, L2, and L5 below.

## Findings, most severe first

- **M1 (medium; blocks until the owner rules).** `motions.hpp:45-50`. A zero direction, or one outside the safe range, is documented but not checked. STYLE S9 asks for a `GALOOT_ASSERT` in checking builds for a precondition the types cannot express, and for a failure value on a degenerate zero-norm input. Failure scenario: `rotation(Line<double>{}, t)`, or `Line{p ^ p}`, now returns NaN on every line blade, even at t = 0, where it used to return a finite motor. It fails silently in a checking build. The authors disclosed this and left it to the owner. Recommendation: `GALOOT_ASSERT(length > T{0} && length < std::numeric_limits<T>::infinity(), ...)` in checking builds, as the core does for its own preconditions, with a delta review.
- **L1 (low).** `motions.hpp:35-36`. The comment says "the join p ^ q of two points can be passed as it is". That is wrong: the join's layout hits the "axis is not an rga3 Line" catch-all, so it must be passed as `Line{p ^ q}`.
- **L2 (low).** `motions.hpp:88`. The integer-scalar message says to build the motor from "the line times sin(theta/2)". For a non-unit line this no longer matches what `rotation` builds; it should say "the unitized line". The message is pinned by `rga3.rotation_integer_scalar`, so the test moves with it.
- **L3 (low).** `README.md:103`. "any two distinct points" is wrong for two distinct points at infinity, and for two representations of one point with different weights: both give a zero direction. The earlier text said weight 1. Suggested wording: "two distinct points of positive weight".
- **L4 (low; plausible tightening).** In float, the exhaustive check above proves |fl(5 fl(ŝ/5)) − ŝ| ≤ u. That gives a = (k + 1)u and tolerances of 40, 39, 0, 32 against the 42, 41, 0, 34 committed. V12 asks for the tightest bound. The committed bound is valid. In double, 1u is sampled only.
- **L5 (low).** The `///` comment does not state the per-call cost of one `std::sqrt` and one division (checklist, "states the cost"). Also, directions that are neither unit nor axis-aligned, such as (0, 3, 4) and (1.5, −2, 0), are checked only by the construction test, whose expected value repeats the formula. `length_abs_vz` and `length_l1` are killed only there.
- **Nit.** `motions.hpp:36` has a short comment line ("it is. The turn"), left over from a hand reflow.

## Outcome

Approve with non-blocking findings L1 to L5, once the owner rules on M1 (the assert, or a recorded departure from S9) and the P9 merge checks pass. The mathematics, the tolerances, unit-axis bit-identity, W9, and the mutants hold.

Before release (Hyrum's law): whether a zero direction asserts, returns `std::optional`, or yields NaN becomes harder to change once users rely on it.
