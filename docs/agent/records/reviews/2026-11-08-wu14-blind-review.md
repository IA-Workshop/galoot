# Blind review of WU14 (plan 0029): the identity and conventions suites, the catalogue tool

- Reviewer: blind-reviewer role, Claude (agent, Opus 5.5, high effort), 2026-11-08. No part in writing the change and no shared working context with its authors.
- Reviewed: `feature/core-identities` at `e248bf1` against `develop` at `c0ff494` (`git diff develop...e248bf1`, 109 files), in a worktree of its own at `.claude/worktrees/wu14-blind-review`, branched at `e248bf1` as `records/wu14-blind-review` (W7: a review record is branched from the revision it reviews and cherry-picked, never merged).
- Did the brief carry a verdict? No. It listed what to check and described the change's contents (the seed fix, the corrected `squares_doubled` reading, decision 0217's criterion 12 reading); I treated those descriptions as claims to check.
- Outcome: **approve with non-blocking findings.** No blocking finding. Five should-fix findings, all in records; none touches `include/`, `test/`, `tools/`, or the build, so fixing them needs no delta review.

## Departures, skipped checks, ambiguities (C20)

- **Branch name.** Plan 0029 section 12 names the review branch `records/review-wu14`. The brief names `records/wu14-blind-review`. I followed the brief.
- **P9 develop-level matrix not run by me.** I ran the full suite on gcc-debug and clang-debug, format-check, the trace gate, `tools/runlog check`, and `tools/catalogue pinned` against eight presets' listings. I did not run the suites of gcc-release, clang-release, gcc-asan, clang-asan, clang-measure, or a full lint build. Plan section 11 assigns them to the merge's `gate:all` run. I configured those trees only to read their CTest listings.
- **Mutants on gcc-debug only.** My nine mutants ran on gcc-debug, not clang-debug.
- **Not rerun by me:** the authors' mutation specs (`faults*.toml`, `identities.toml`, `new_cases.toml`, `bits.toml`, `blade_set.toml`, `blade_product_rewrites.toml`, `flector.toml`, `catalogue.toml`, `transform_bound.toml`) and the seed campaigns. I read their result rows and checked them against the README's statements. I did not rebuild `bd7a3d4` to confirm that it fails as its message says, and I did not rerun the compile-cost campaign (only its arithmetic).
- **One scratch step.** I built `transform_identities_test.cpp` against a copy of `include/` carrying `gap_offdiagonal_dropped`'s edit, outside `tools/mutate`, to see how that mutant fails (finding S4). Its run row is in this branch's `runs.tsv` (`scratch-offdiag-transform-identities`), and the commands are given under S4.
- The agent's token cost was not visible to me.

## What I checked and how

| Check | How | Result |
| --- | --- | --- |
| Full suite, gcc-debug and clang-debug, at `e248bf1` | `tools/timed` build, then `ctest --preset`, in my own trees | 499 of 499 pass on each. `core.identities` 17.05 s, `core.identities_rounding` 0.16 s |
| format-check | `cmake --build --preset gcc-debug --target format-check` | pass |
| Convention trace gate (W5 command) | from the worktree root | pass; items with no test: 6.1 to 6.4 only |
| `tools/runlog check` | on this branch's log | pass |
| `tests.messages_pinned` in every preset | `tools/catalogue pinned include/galoot build/<preset> test/internal_messages.txt` for gcc-release, clang-release, gcc-asan, clang-asan, lint, clang-measure (configured only), and as a CTest on gcc-debug and clang-debug | all pass: 159 messages, 139 pinned, 20 internal |
| Catalogue regenerated at the tip | `tools/catalogue report include/galoot gcc-debug=… clang-debug=… --check misuse-catalogue.md` | byte-identical to `catalogue/scan-output.md`; 187 test names, none absent |
| Cherry-picks with `-x` | `git patch-id --stable` of each of the 23 picked commits against its named source | all 23 identical; each source is on its `--tests`, `--tests2`, or `--catalogue--tests` branch |
| Criterion 3's sign | my own derivation: an antiproduct of grades j and k has grades j + k − n + 2i, so ĝ(a ⟇ b) = (−1)^(j+k+n) a ⟇ b = (−1)^n ĝa ⟇ ĝb, and likewise for ∨ | agrees with `identities_checks.hpp:377` and research note 0024 A1 |
| Golden values, by my own route | `ga.py`/`recompute.py` beside this record, which share no code with `golden.py`: a diagonal Clifford product on masks, complements from u ∧ right(u) = 𝟙, the antiproduct as left(right(a) ⟑ right(b)), the mirror as −V ⟇ x̌ ⟇ V̰, and bulk duals solved from a ∧ bulk_dual(b) = det g[A, B] 𝟙 | all 8 `mirrors::rigid_2_images`, 5 `duals::conformal_3_bulk_dual` entries (non-diagonal G), and 4 `duals::rigid_3_bulk_dual` entries agree; output in `out/recompute.txt` |
| The plane hand value | the same script | the Flector e412 sends the plane (0, 0, 1, −1) to (0, 0, 1, 1) and the point (1, 2, 3, 1) to (1, 2, −3, 1), as the tests require |
| Seeded draw R(p, q, r) | reran the procedure the source comments state with `random.Random(20261108)` | R(0,3,2), R(0,2,3), R(1,3,2), R(1,1,4), as committed |
| Tolerance derivation (V12) | sympy expansion of (1 − u)³ (K+1)(1 − Ku) − K(1 − (K+1)u) | equals the comment's polynomial in `tracked.hpp`; the bound 1 − 3x + 3x² ≥ 1/4 and the cubic term ≤ 3x²u hold; monotone rounding gives the comparison; the K ≤ 1 equality rule follows from the counting rules (K = 1 means one operation on exact inputs) |
| CONVENTIONS 1.16 against the tests | read the three named cases | the texts match; "the duals of a point …" (`dual_test.cpp:1033`) checks both left duals' negations; the sphere case (`dual_test.cpp:1194`) is unchanged; the plane case (`objects_test.cpp:579`) checks the single-sign negation |
| Agent citations in human-facing files | grep of the diff outside `docs/agent/` for decision, plan, WU, DoD, criterion, part, agent, review, record numbers, and provision numbers | none (S11) |
| W2 labelled failing commits | read each message | `ad8bf7d`, `c0f4195`, `f9e5ae9`, `8a40039`, `bd7a3d4` say they fail and name the test; `70564f5`, `47fdf97`, `d500d0f` make them pass (see N6) |
| Trailers (W7) | every commit's message | all agent commits carry the trailer except merge `878e190` (N5) |
| Compile-cost arithmetic | summed `out/summary.txt` | `core_identities_test` 314.3 → 460.8 (GCC) and the per-unit percentages are right |
| Seed records | tallied `seeds/results.tsv` and `seeds/bounded/results.tsv` | seven failures at seeds 8, 23, 27, 37, 38, 39, 43 before the fix; 50 of 50 pass after |

### My own mutants (plan 0029 section 12)

`2026-11-08-wu14-blind-review/reviewer.toml`, written before any of them ran, judged at `e248bf1` on gcc-debug. Rows: `out/reviewer-gcc-debug.tsv`. All nine were killed.

| Mutant | What it breaks | Killed by |
| --- | --- | --- |
| `first_stage_grade_not_antigrade` | transform folds the grade involution's sign, not the antigrade's, into an odd versor | "a mirror maps every blade to the reference's image" and five more cases in `core.transform` and `core.transform_identities` |
| `left_bulk_dual_right_complement` | `left_bulk_dual` uses the right complement | "the four duals of every blade are the reference's images" and six more |
| `test_antiproduct_sign_dropped` | the test's own expected (−1)^n set to 1 (a wrong expectation) | IA and BP: the correct library fails the wrong rule, so the sign is load-bearing |
| `floating_products_2e40` | every floating product times 1 + 2⁻⁴⁰ | `core.identities_rounding` |
| `floating_products_2e48` | every floating product times 1 + 2⁻⁴⁸, 32 units of roundoff | `core.identities_rounding`; the bound is tight enough to see a systematic error of 32u |
| `identity_tolerance_left_only` | `identity_tolerance` drops the right side's bound | `support.tracked` only; `core.identities_rounding` still passes (N3) |
| `catalogue_whole_pins_shortest` | a literal holding two messages whole pins the shorter one | `tools.catalogue`, `tests.messages_pinned` |
| `catalogue_twin_forbid_shared` | the twin rule accepts a FORBID that the short message also holds | `tools.catalogue` |
| `catalogue_define_not_skipped` | the `#define` line's assertion is no longer skipped | `tools.catalogue` |

### Acceptance criteria

| Criterion | Verdict | Evidence |
| --- | --- | --- |
| 1 Kernel associativity | met | `check_product_laws` over all nine groups (`identities_checks.hpp:252`); the dense forms of five on vectors, as their source says |
| 2 Defining relation | met | `anticommutator_difference` sums 2 aᵀ g b from `Alg::value.gram` (`identities_checks.hpp:277`), with no use of `dual.hpp` |
| 3 Remaining product rules | met | sign recomputed above; my test mutant shows it is load-bearing |
| 4 Exhaustive basis checks | met | `check_basis_pairs` and `check_basis_triples` over full layouts up to n = 5. The proof assumes that the products are bilinear in the coefficients, which their straight-line kernels are (N4) |
| 5 Seeded R(p, q, r) | met | draw reproduced; every family runs on it, with the dual identities on full<6> |
| 6 Golden values | met | two routes in `golden.py`; my third route agrees on 17 values |
| 7 Section 8 | met | text matches the tests |
| 8 Floating identities | met | derivation verified; see N3 |
| 9 Catalogue | met | CTest in every preset; listings of eight presets pass; report regenerated identically |
| 10 Twin messages | met | six FORBIDs; my twin-rule mutant killed |
| 11 DoD-14 | met, as recorded | rows read; not rerun |
| 12 Mutants of the new tests | first clause met; second clause **not met as written** for the gaps of criteria 1, 3, and 4, and for criterion 2 only when `core.identities` is judged alone | README "Gaps without a counting mutant"; decision 0217 item 6; owner item O1 |
| 13 Seeds | met | tallies above |
| 14 Build cost | **partly met** | two changed sources were never measured (S1) |
| 15 Trace | met | gate output |

## Findings

No blocking findings.

### Should-fix (records; before or at the merge)

**S1. Criterion 14 does not cover the sources the test author changed after the cost campaign.** `records/performance/wu14-identities-cost/README.md:3` says the campaign's worktree at `d5ffafd` has "`include/` and `test/` … those of the unit's tip". Later commits changed `test/core/transform_identities_test.cpp` (`bd7a3d4`, `d500d0f`) and `test/presets/rga3/objects_test.cpp` (`25d5119`), and neither is in `units.toml`. So the claim is no longer true at `e248bf1`, and two changed test sources have no before-and-after cost. Failure scenario: a reader of the release record takes the table as the unit's whole compile-cost change. Fix: add the two units and rerun `tools/compiles cost` under the timing lock (a few minutes), or state the omission and amend the sentence.

**S2. "Nine" trace rows, where the diff adds eight.** `decisions/0217-wu14-integration-choices.md:17` ("Nine facts … three of 2.1, two of 3.2, three of 4.2"), `records/mutations/0029-identities/coverage.md:53`, and the plan's outcome (`plans/0029-…:250`, "nine trace rows") all count nine, but 3 + 2 + 3 = 8, and `git diff develop...e248bf1 -- docs/agent/records/convention-trace.md` adds eight rows. Fix: correct the count. Decision 0217 is a recorded decision, so see N1 on how to correct it.

**S3. The coverage table says that section 8's flector row is unchanged.** `coverage.md:55` says "The flector row is unchanged", but `c7fdccf` (CONVENTIONS 1.16) added the plane case to that row. `e025d33` updated the table for the seed fix but not here. Fix: name the plane case.

**S4. The README carries the overflow explanation over to `gap_offdiagonal_dropped`, where it is wrong, and calls the mutant "consistent in itself".** `records/mutations/0029-identities/README.md:35`: the row reads "as above", which inherits `gap_squares_doubled`'s note that the transform suite killed it "only through its rational's overflow". `identities.toml` says of the mutant that "on the non-diagonal groups only the defining relation can see it". But `out/transform_bound-gcc-debug.tsv` shows that, under the bounded draws, `core.transform_identities` still kills it in both cases, the fixed-seed case included. I built the bounded test against the mutated headers. Every failure is `CHECK(accepted > 0)` (`transform_identities_test.cpp:315`), 16 times, for the Witt forms `[[0,1],[1,0]]` and `[[0,1,0,0],[1,0,0,0],[0,0,0,1],[0,0,1,0]]`: with their off-diagonal entries dropped, every generator squares to zero, no versor has λ ≠ 0, and the case's guard against passing vacuously fires. That is a legitimate kill, but not by an identity and not by overflow, and it contradicts "only the defining relation can see it". No verdict changes. Fix: correct the row and the spec comment. Commands, from this worktree:

```bash
cp -r include "$S/include"   # S a scratch directory
# apply identities.toml's two gap_offdiagonal_dropped edits to $S/include/galoot/core/blade_product.hpp
g++ <gcc-debug flags> -I"$S/include" -Itest -isystem "$S/include/ext" -o "$S/t" \
  test/core/transform_identities_test.cpp \
  build/gcc-debug/test/CMakeFiles/galoot_doctest_main.dir/__/include/ext/doctest/doctest.cpp.o
"$S/t"   # 16 failures of CHECK( accepted > 0 ), none of an identity
```

**S5. The run rows of the three test branches never reach `develop`.** The unit's `runs.tsv` holds no row of `feature/core-identities--tests` (143 rows, committed there in `eb1db0d`), `feature/core-identities--tests2` (379, `4df954d` and `e032121`), or `feature/core-identities--catalogue--tests`. Those branches are never merged (decision 0185), and every earlier `--tests` branch's rows are on `develop` (for example 305 of `core-transform--rounding--tests`), so the P13 log would lose this unit's test-author runs, the lint runs of part B among them. Fix: cherry-pick the records commits with `-x`, or append the rows. Also confirm whether `--catalogue--tests` ran anything through `tools/timed`: its log has no row of its own.

### Notes

**N1. Decision 0217 was edited in place.** `decisions/0217-…:6` adds a "Corrected:" line and rewrites item 11 (`e025d33`). W6 says that a recorded decision is not rewritten and that a later decision supersedes it. The decision has not reached `develop`, and the edit is dated and visible, so I read this as a draft amended before integration rather than a rewrite of shared history. The owner may prefer a superseding decision (O2).

**N2. A stale sentence in the catalogue record.** `records/diagnostics/misuse-catalogue.md:9` still says the scan output "at `7c07cc4`, from the scan before the move, is `catalogue/scan-output.md`". The file is now the tool's output at `f1ed6f1` (and matches `e248bf1`).

**N3. In the floating test, the comparison of the two sides adds little power beyond the per-side checks.** `identities_rounding_test.cpp:186` requires `within_identity_bound` in addition to `within_rounding_bound` on each side. A correct double result is within γ_K M of e by the theorem, so the two-sided check can fail while both per-side checks pass only in a window of about one rounding of margin. My `identity_tolerance_left_only`, which halves the tolerance, survives `core.identities_rounding` (largest observed ratio 0.23 to 0.33) and is killed only by `support.tracked`. That meets criterion 8 as written (the sum of the two bounds, fixed beforehand); the evidence of the tolerance's own correctness is the derivation and `tracked_test.cpp`. The margin of 1 is justified: it is the least integer margin for which the derivation holds, and I verified the derivation.

**N4. The basis-blade proof's premise.** Its marker (`identities_test.cpp:126`) says each law is linear in each operand. The step from basis blades to all multivectors also needs the implementation to be bilinear in the coefficients, which straight-line kernels without coefficient-dependent branches are. Saying so in one clause would make the proof self-contained. Optional.

**N5.** Merge `878e190` ("Merge branch 'feature/core-identities' into feature/core-identities--math") carries no agent identification (W7). Every other commit does.

**N6.** `c77fb9c` sits between labelled failing commits and leaves `tools.catalogue` and `tests.messages_pinned` failing, which it inherits from `ad8bf7d` and `c0f4195`. Its message says only that its own six tests pass. W2's labelling is per commit, so the message would ideally say so too. Cosmetic: the run of labelled commits ends at `70564f5` as W2 requires.

**N7.** README "Kill power": the claim that `f09007f`'s test aborts at seed 0 under `squares_doubled` after 948 passing assertions rests on a scratch build whose steps are described but not committed. V10 would label it not reproducible. The reasoning is sound (a doubled square is another metric, in which λ times the outermorphism holds), so the equivalence verdict stands on the bounded run's rows.

**N8.** `tests.messages_pinned` is labelled `tests;tests.catalogue`. So the P8 selection for a header change does not run it, and an unpinned new message is caught only by a develop-level full suite (or by a feature merge that selects it). That matches option C ("at every merge" into `develop`), but the catalogue's sentence "at every merge" may read as broader than it is.

**N9.** The plan's line references `dual_test.cpp:961` and `:1122` are now at lines 1033 and 1194. CONVENTIONS rightly names the cases by text.

## Open items for the owner

- **O1. Criterion 12's second clause (decision 0217 item 6 and the README).** As written, it asks for a mutant per gap that the branch point lets survive and the tip kills. For the gaps of criteria 1, 3, and 4 there is none, and for criterion 2 there is one only when `core.identities` is judged alone. The authors argue that criteria 1 and 3 follow from laws the branch point already tested (through the De Morgan laws and x̌ = (−1)^n x̂), and that criterion 4 turns a sample into a proof without adding kills. I find the argument sound: any edit that breaks ∧-associativity also breaks ∨-associativity or a De Morgan law.
  - (a) Accept the reading and record it as "not met as written, replaced by the stated argument". This costs nothing.
  - (b) Require layout-specific mutants that only the new layouts reach. These would be contrived and add little evidence.
  - (c) Amend the criterion for later units, to judge gap mutants by the suite they extend.
  - Recommendation: (a), with (c) for later plans.
- **O2. Decision 0217's in-place correction (N1).**
  - (a) Accept the dated "Corrected:" line, since the decision was never on `develop`.
  - (b) Restore the original text and record the correction as a new decision.
  - Recommendation: (a). It is simpler, and git keeps the original.
- **O3. The missing cost rows (S1).**
  - (a) Measure the two units now, a few minutes under the timing lock.
  - (b) Record the omission only.
  - Recommendation: (a). Criterion 14 then holds as stated.
- **O4. The test branches' run rows (S5).**
  - (a) Cherry-pick their records commits into the unit before the merge.
  - (b) Leave them on the unmerged branches.
  - Recommendation: (a). It keeps the P13 log whole, as for every earlier `--tests` branch.

## Resolution

Filled in by the merging agent (Claude, Opus 5.5, implementer of plan 0029) on `feature/core-identities` before the merge, as the coordinator directed. All are records only; no source, test, or build file changed after the review.

- **S1.** Fixed (O3 (a)). `transform_identities_test.cpp` and `presets/rga3/objects_test.cpp` measured before and after under the timing lock (`records/performance/wu14-identities-cost/units-later.toml`, `out-later/`, the README's second section); the README's sentence on the first campaign's tree now says which sources changed after it. `transform_identities_test.cpp` is unchanged within its range and is the largest changed unit, so the README no longer calls `dual_identities_test.cpp` the longest.
- **S2.** Fixed. The diff adds eight rows (counted again); decision 0217 item 2, the coverage table, and the plan say eight, and 0217's "Corrected:" line names the change.
- **S3.** Fixed. The coverage table names the flector row's plane case.
- **S4.** Fixed. The README row of `gap_offdiagonal_dropped` and the comment of `identities.toml` say that the transform suite's kill is the guard `CHECK(accepted > 0)` on the Witt forms made totally null; no verdict changes. The spec copy in `out/branch-point/` stays as it ran.
- **S5.** Fixed (O4 (a)). `eb1db0d` (`--tests`), `4df954d` and `e032121` (`--tests2`) cherry-picked with `-x`. `feature/core-identities--catalogue--tests` committed no run row and its worktree log holds none: its tests were run in the subfeature's worktree, whose rows are in the unit.
- **N1.** Kept as recorded (O2 (a)), as the coordinator directed.
- **N2.** Fixed. The catalogue record says the scan output is the tool's at `f1ed6f1`.
- **N3.** No change; noted.
- **N4.** Fixed in the records: the mutation README states the bilinearity premise of the basis proof. The test's marker is unchanged.
- **N5.** Not fixable without rewriting history; noted.
- **N6.** Noted here: `c77fb9c` passes its own six tests and inherits the failures of `tools.catalogue` and `tests.messages_pinned` from `ad8bf7d` and `c0f4195`, which `70564f5` ends.
- **N7, N8, N9.** No change; noted. Criterion 12 (O1) is recorded as not met as written and replaced by the argument, with the note that later plans should judge a gap's mutants by the suite the gap extends.
