# 0031 WU14 follow-ups: the generator-rule family, two off-axis degenerate forms, the catalogue test's label, and the proofs note

- Status: claimed 2026-11-08 by Claude (agent, Opus 5.5), planner and implementer with doctest-expert and math-expert judgement, from `develop` at `ba12e5a`. Implemented 2026-11-09; awaiting the blind review (W5). Not merged by its author.
- Source of scope: decision 0218 items 1, 2, 3, 5, and 6 (the owner's rulings on the math-tools and knowledge-base survey and the WU14 gate items), on the coordinator's instruction. Plan 0030 is WU12's, being written on `records/plan-wu12`; this plan takes 0031.
- Branches, from `develop` after this claim (W7):
  - `feature/wu14-follow-ups`, worktree `.claude/worktrees/wu14-follow-ups`: the unit. Only this branch merges into `develop`.
  - `feature/wu14-follow-ups--tests`, worktree `.claude/worktrees/wu14-follow-ups--tests`: the test commits, never merged; cherry-picked with `-x` into the unit (decision 0185).
- Record numbers: the mutation record `records/mutations/0031-wu14-follow-ups/` and research note 0025, free on every branch at this commit. A number taken in the meantime moves to the next free one.

## 1. Departures, ambiguities, and readings (C20)

1. **The survey's wording.** The survey itself is not committed in this repository. The family is taken from the knowledge base's statement of the theorem (`knowledge-base/geometric-algebra/proofs/test-retirement.md`, "The key lever", at `e6057ce`): a table T with (1) the unit T(e_∅, x) = x, (2) the generator rule T(e_i, e_B) = e_i ⌋ e_B + e_i ∧ e_B, and (3) the recursion T(e_A, e_B) = T(e_a, T(e_A′, e_B)) − T(e_a ⌋ e_A′, e_B) for a the lowest index of A, is the Clifford product of G in the wedge basis. Its two cautions are kept: the right sides of (2) and (3) are computed in the test, with signs counted there and not by the library's reorder sign, and the triple loops stay (decision 0218 item 1).
2. **Where the family goes.** In the identity suite (`identities_checks.hpp`, `identities_test.cpp`), as the coordinator's brief places it (it reports that part B's test author noted these files; I found no such note in the repository), on the full layouts of the kernel groups (every form of up to four generators, R(4,1,0), R(4,0,1), conformal(3), R(0,3,2), R(0,2,3), and the two new forms). It is a check on every basis pair through `mv` (V1 kind 1, as the basis-blade case is). The structure-layer loops over `test/core/signatures.hpp` (n ≤ 6, the blade products directly) do not get it; that would reach n = 6 and is an open item (section 10).
3. **Condition (1) is not checked twice.** "1 ⟑ a = a" on every blade of the same layouts is already in "the laws hold on every pair and triple of basis blades over many forms". The family adds (2) and (3), and the marker of (3) names the unit check as the third condition.
4. **"Appended so that existing draws do not change."** The identity and dual suites draw every form's multivectors from one source, in list order, across groups. Appending a form to the end of `up_to_three` or `small_forms` would shift the draws of every later group. Reading followed: the two forms go into a list of their own, checked after every existing group (a group `forms_off_axis` at the end of `groups`, defined in `identities_up_to_three_test.cpp`; a list `off_axis_forms` checked last in two test cases of `dual_identities_test.cpp`), so that every existing draw stays as it was for every seed. In `signatures.hpp`, which draws nothing, they are appended to `for_each_signature`.
5. **`identities_four_test.cpp` gets no form.** Both forms have three generators; the four-generator list keeps its meaning. The coordinator's list named the file; no reading puts a 3 × 3 form in a list of 4 × 4 forms.
6. **The bulk-and-weight case of the dual suite skips non-diagonal forms**, so the new forms join only "the duals compose to det g over many forms" and "the dot and the antidot are symmetric De Morgan pairs over many forms".
7. **Criterion 12's amendment (decision 0218 item 5).** Mutants are judged by the suite the new checks extend, `core.identities`. A gap mutant counts when the suite before this unit lets it survive and the suite after kills it; a per-check mutant counts when the new case is among the failed cases. The one sentence that would carry this into the governance is not edited here (section 10, O1).
8. **The proof harness is not a Galoot build or test.** It runs in math-tools, outside `tools/timed`, and its rows go to math-tools' own ignored results file. The note copies them.

## 2. Goal

Carry out decision 0218's follow-ups to WU14: add the generator-rule characterization of the geometric product to the identity suite beside the triple loops; add the two degenerate forms whose radical lies along no generator to the structure-layer signatures and to the identity and dual suites; let header changes select `tests.messages_pinned`; and record which math-tools proofs back the core, rerun on current `develop`.

## 3. Definition of done

1. Two test cases in `identities_test.cpp`, over every full-layout group: the generator rule on every generator and blade, and the recursion on every blade of grade at least 2 and every blade, with both right sides formed in the test by dense integer arithmetic. They pass on gcc-debug and clang-debug.
2. `[[1, 1, 0], [1, 0, 1], [0, 1, −1]]` and the all-ones 3 × 3 form are in `for_each_signature` (last), in a new last identity group, and in a new last list of the dual suite's first two cases; every existing draw is unchanged.
3. `tests.messages_pinned` carries `core.headers` besides its labels.
4. Research note 0025 cites each math-tools obligation the core relies on with the commit and source hashes of a harness run on `develop` at the claim's revision.
5. A mutation spec under `records/mutations/0031-wu14-follow-ups/` has run on gcc-debug and clang-debug at the tip, and its gap mutant also at the branch point; each new test case kills at least one mutant; every survivor is explained.
6. The coverage table gains the family's row and the new forms; the trace gate, the reference-marker and citation checks pass; the blind review has run; the merge checks of P9 pass.

## 4. Acceptance criteria

1. **The generator rule.** For each form of a full-layout group, each generator e_i and each blade e_B: e_i ⟑ e_B equals Σ_p (−1)^p g_(i, b_p) e_(B∖b_p) plus, when i ∉ B, (−1)^(#{b ∈ B : b < i}) e_(B∪i), with the signs counted over the sorted index list of B in the test.
2. **The recursion.** For each blade A of grade at least 2, with a its lowest index and A′ = A∖{a}, and each blade B: e_A ⟑ e_B = e_a ⟑ (e_A′ ⟑ e_B) − (e_a ⌋ e_A′) ⟑ e_B, with e_a ⌋ e_A′ formed in the test and the two products on the right taken through `mv`. Compared on every canonical blade, in `std::int64_t`, which holds every value (the entries are at most 3 and n at most 5, so each coefficient is at most a few thousand).
3. **No new product.** The checks build only products of full layouts that the basis-blade case already builds, so the identity sources' compile cost changes only by the checking code and the two new forms (measured, criterion 7).
4. **The forms.** The two forms are appended as section 1 item 4 says, and the existing cases pass with them at seed 0 and over the seed campaign's 50 seeds for the changed programs (`core_identities_test`, `core_dual_identities_test`).
5. **The label.** `ctest -L '^core\.headers$' -N` lists `tests.messages_pinned` in a configured tree.
6. **Mutants** (`records/mutations/0031-wu14-follow-ups/theorem_r.toml`), judged by `core.identities`:
   - `resigned_e12`: the product table of the basis in which e12 is replaced by −e12, an associative algebra isomorphic to the right one. Expected to survive the suite before this unit (its laws hold in any basis) and to be killed by the generator-rule case. The gap mutant; run at the branch point and at the tip.
   - `recursion_contraction_doubled`: the contraction term of the recursion doubled, which changes only rows of grade at least 2 in non-diagonal forms. For the recursion case.
   - Each new case must be among the failed cases of at least one killed mutant on both presets.
7. **Build cost.** `tools/compiles cost` of the changed test sources at the branch point and the tip on gcc-debug and clang-debug (P1, W10 read for a test-only unit; no bound, C14).
8. **Seeds.** `core_identities_test` and `core_dual_identities_test` with `--rand-seed=1` to `50` on gcc-debug pass, through `tools/timed` into a campaign log.

## 5. Approach and order

0. Claim: commit this plan to `develop`.
1. Proof harness: run `toolset/proof-harness/harness.py run exercises/logic/proof-obligations.toml` in math-tools at its `develop` with `GALOOT_INCLUDE` set to this checkout's headers; write research note 0025 on the unit branch.
2. Tests first, on `--tests`: (a) the forms (`signatures.hpp`, the new identity group, the dual suite's list); (b) the family (`identities_checks.hpp`, `identities_test.cpp`); each commit passing on gcc-debug, then clang-debug, when committed (the code exists, W2).
3. Unit: cherry-pick (a) and (b) with `-x`; the label in `test/CMakeLists.txt`; the coverage table's rows.
4. Evidence: the mutants at the branch point and the tip, the seed campaign, the compile costs, the record README.
5. Stop and report for the blind review (the coordinator launches it). No merge by this agent.

## 6. Evidence (V1, V10)

| Claim | Evidence | Kind |
| --- | --- | --- |
| The geometric product of each full-layout form is the Clifford product of g in the wedge basis | the unit check (existing) and the two new cases, exhaustive over basis pairs, with the induction on the grade of the left factor as the argument | V1 kind 1 |
| The new forms satisfy every identity of the suites | the existing cases over them | V1 kinds 1 and 2 |
| The family adds power the triple loop lacks | `resigned_e12` at the branch point and the tip | mutation record |
| The math-tools proofs hold on current headers | the harness rows, copied into note 0025 | machine-checked, as each obligation's level states |

No floating check is added, so no tolerance is derived (V12 not engaged). Validation (V8): not applicable; this is verification of the specification's mathematics.

## 7. Checks (P8, P9, P13)

| Point | Checks | Presets |
| --- | --- | --- |
| Each test commit | `-L '^core\.(identities|dual_identities|blade_product|plan)$'`, `-L '^tests\.'` | gcc-debug, then clang-debug |
| The label commit | `-L '^tests\.'`, a listing of `core.headers` | gcc-debug, clang-debug |
| Before the review | the selection above on both presets, lint of the changed targets, `format-check`, the trace gate, `tools/runlog check` | gcc-debug, clang-debug, lint |
| Merge into `develop` (by the coordinator) | the full P9 matrix | all eight, as `gate:all` |

Every build and test through `tools/timed`, with `eval "$(tools/jobpool env)"` first; `tools/jobpool status` before and after each heavy run; `ensure` only if it reads "not running".

## 8. Review plan (W5)

A blind review at the Opus level with high effort, from a configuration that shares no context with this agent, given the diff from `ba12e5a`, section 4, and section 1. It recomputes the generator rule's signs by its own route for at least one non-diagonal form, checks that no existing draw changed (by a seed-0 run of the old cases' messages or by reading the order), runs its own `tools/mutate` spec of at least three mutants against the new cases, and reads the note's harness rows against math-tools. The usability checklist does not apply: no public interface changes.

## 9. Agent cost estimate

Planner and implementer, Opus high: 250k to 400k tokens. Blind review: 100k to 180k. Machine time: the harness about 4 minutes; the mutants about 2 minutes per mutant and preset (about 15 minutes); the seed campaign about 15 minutes; the compile costs a few minutes under the timing lock.

## 10. Open items for the owner

- O1. The sentence that carries criterion 12's amendment into the governance (decision 0218 item 5). Wording in the outcome; not edited here.
- O2. The family at the structure layer (`for_each_signature`, n ≤ 6, 44 signatures), where it would cover the sixth generator. Not in this unit's scope.

## 11. Outcome

State at the end of the implementer's session (2026-11-09 UTC): every part of section 5 is done; the blind review has not run, and the unit is not merged. Next step: the blind review of section 8, of the diff from `774ff66` to the tip.

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `45cf6cb` | cherry-picked with `-x` from `5975ca4`: the two forms in `for_each_signature`, the identity group `forms_off_axis`, the dual suite's `off_axis_forms` | test, passing when committed |
| `adfabd6` | cherry-picked with `-x` from `5388177`: the generator rule and the recursion, two cases of `core.identities` | test, passing when committed |
| `a5c2608` | cherry-picked with `-x` from `1df105b`: the tests branch's run rows | record |
| `7cc961e` | `tests.messages_pinned` carries `core.headers` | build description |
| `98fd0c2`, `6e34e67` | the mutation spec, and its recursion mutant narrowed to n ≤ 4 | record |
| `0e7c247` | the mutants' rows and README, the seed campaign, the coverage table's K9 and family row, research note 0025 | record |
| `53aa46d` | the compile cost campaign | record |

Results:

- **Criteria 1 and 2.** Both cases pass on gcc-debug and clang-debug over the 41 full-layout forms (82 assertions, 0.17 s together on gcc-debug).
- **Criterion 3.** No new product is built: the checks multiply only full layouts, whose products the basis-blade case already builds. The cost (criterion 7) grows only where forms or checking code were added.
- **Criterion 4.** The forms are appended as section 1 item 4 reads; the existing cases pass with them at seed 0 and at seeds 1 to 50 (`records/mutations/0031-wu14-follow-ups/seeds/`).
- **Criterion 5.** `ctest -L '^core\.headers$' -N` lists `tests.messages_pinned`, `core.bare_compile`, and `core.include_scan` on both presets.
- **Criterion 6.** `resigned_e12` survives `core.identities` at the branch point on both compilers and is killed at the tip by both new cases; `recursion_contraction_doubled` is killed at both points, at the tip with the recursion case among its failed cases. The first run of the latter, doubled at every n, was unjudged on clang-debug (the dense forms of five exceeded Clang's constant-evaluation limit); narrowed to n ≤ 4, both compilers judge it. Record: `records/mutations/0031-wu14-follow-ups/README.md`.
- **Criterion 7.** `core_identities_test`'s sources +3.1% (GCC) and +2.2% (Clang) in retired instructions; `records/performance/wu14-follow-ups-cost/`.
- **Criterion 8.** 100 of 100 seed runs pass.
- **Research note 0025.** The harness on math-tools `develop` `d698d34` against Galoot's headers at `ba12e5a`: 15 of 15 obligations pass. ga-P7 and ga-P11 failed only their replay in the first run, because the manifest's replay command reads the opam root from `$PWD` after a `cd` when `MT_SRC` is unset; with `MT_SRC` set both pass, replay included. The whole run takes about 13 minutes, not three: logic-E20's 32 mutants take 492 s.
- **Checks at the tip `53aa46d`:** `core.identities`, `core.dual_identities`, `core.blade_product`, `core.plan`, every `tests.*` test, and the `core.headers` selection on gcc-debug and clang-debug (12 of 12 each); the lint build of the four programs (no diagnostic); `format-check`; the trace gate (only 6.1 to 6.4 without a test); `tools/runlog check`. The full P9 matrix is the merge's.

Departures and notes beyond section 1:

- One probe run of `core_identities_test -d` (to read the new cases' durations) ran outside `tools/timed`; the harness ran outside it by design (section 1 item 8).
- The draws are unchanged by construction (section 1 item 4), not by a comparison run.

Open items for the owner, with the recommendation:

- **O1. The governance sentence for criterion 12 (decision 0218 item 5).** No plan template exists, so the sentence would go into WORKFLOW W3, after "the evidence it will produce": "Where a plan adds tests to close a gap in an existing suite, it judges the gap's mutants by that suite: a mutant counts for the gap when the suite without the new tests lets it survive and the suite with them kills it, and a mutant written for a new test counts when that test is among those it fails." Options: (a) add it to W3; (b) add it to VVUQ V2 instead, as a rule about what a test shows; (c) leave it in each plan's criteria. Recommended: (a); it is guidance for plans, and (c) relies on each planner remembering decision 0218.
- **O2. The family at the structure layer.** `for_each_signature` reaches n = 6 and the Witt and random forms the identity groups do not hold on every blade; three conditions over `blade_product` directly would cost 4^n checks per form (4,096 at n = 6). Options: (a) a small follow-up adding them to `blade_product_test.cpp`; (b) leave the family at n ≤ 5. Recommended: (a), after WU12's plan is settled, since it touches no file WU12 needs.
- **O3. The math-tools manifest's replay path.** Options: (a) a math-tools agent sets the opam root before the `cd` in the ga-P7 and ga-P11 replay commands; (b) document that `MT_SRC` must always be exported. Recommended: (a); a run from the main checkout should not need the variable.
- **O4. The readings of section 1 items 4 and 5** (the forms in lists of their own, checked last, rather than appended to `up_to_three` and `small_forms`; none in the forms of four). Recommended: confirm; appending to the existing lists would change every later group's draws, which the ruling excludes.

Agent cost: recorded by the coordinator from the harness report (decision 0215).
