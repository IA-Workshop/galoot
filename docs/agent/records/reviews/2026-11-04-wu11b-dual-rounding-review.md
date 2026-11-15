# Blind review: WU11b's last group, the rounding tests of the dual operations

- Reviewer: the blind-reviewer role, general-purpose agent, Claude Opus 5.5, briefed by the coordinator (W5).
- Range: `develop` at `535c91e` to `feature/core-rounding-wu11` at `17e0b8d`.
- Outcome: **approve, with two low findings, L1 and L2**, both fixed in `f68975c`.
- Source of this text: written by the implementer from the summary the coordinator relayed on 2026-11-04. The reviewer's own report is held by the coordinator.

## What the reviewer checked (as relayed)

- Re-derived the rounding count K of each operation from a histogram of the tracked runs: the bulk, the weight, and the duals have K = 1 on rigid(3), conformal(2), conformal(3), and the degenerate form, and so are exact there; on the random form their K runs from 1 to 4; the dot's K is the number of its terms that land on the scalar; the projection's K is 5 or more.
- Ran 19 mutants of its own: 17 killed. The survivors: `antidot_left_complements`, equivalent; and `test_random_inputs_narrow`, which is L1.
- Reproduced the author's two survivors (the projection nudged by one ulp, and the exact-value check removed).
- Ran every input at its largest magnitude: every exact value and magnitude fits std::int64_t.
- Not checked: the full suites and the sanitizer presets; seeds other than 0.

## Findings

| Finding | As relayed | Resolution |
| --- | --- | --- |
| L1 (low) | Nothing checks that the random form's duals actually round: if their inputs fell back to 27 bits, every ratio would be 0 and the tests would still pass; the mutant `test_random_inputs_narrow` survives. Add a check that some coefficient has a nonzero error (largest ratio above 0) for the random form's duals and, if cheap, for the first group's sums; kill the mutant on gcc-debug and clang-debug. | `f68975c`: both families check that their largest ratio is above 0. `test_random_inputs_narrow` and a matching `test_sums_inputs_narrow` are killed on gcc-debug and clang-debug (`records/mutations/0020-rounding-wu11/`). |
| L2 (low) | The header comment of `rounding_test.cpp`, lines 1 to 27, should list every operation tested and the real input scales (p/2^27, p/2^(53+s), p/2^53 for the duals, p/2^18 for the projection) and the bound on the exact values (below 2^8, about one bit of headroom in std::int64_t). | `f68975c`. |

## Addendum, 2026-11-07: the reviewer's own report

The reviewer's own report was not preserved in the repository, so this record stays the implementer's summary of it (M3 gate finding MA1). What it says the reviewer checked or found is as relayed, not the reviewer's text. From now on a reviewer's report is committed as the reviewer wrote it (decision 0182).
