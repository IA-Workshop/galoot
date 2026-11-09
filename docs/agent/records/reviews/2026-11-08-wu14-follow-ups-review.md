# Blind review of plan 0031: WU14 follow-ups (the generator rule and the recursion, two off-axis degenerate forms, the catalogue test's label, research note 0025)

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5, `claude-opus-5-5`, high effort). Date: 2026-11-08 local, 2026-11-09 UTC.
- Range: `774ff66` (the plan's claim) to `7d9e22f`, the tip of `feature/wu14-follow-ups`, 10 commits. Worktree read: `.claude/worktrees/wu14-follow-ups`. Record branch `records/wu14-follow-ups-review`, worktree `.claude/worktrees/wu14-follow-ups-review`, branched from `7d9e22f`. The unit's branch was not modified.
- Did the brief carry a verdict? No. It listed what the unit adds and the claims to check. The plan's section 11 holds the author's own outcome, and I treated each statement there as a claim to test, not as evidence.
- Job pool: running, 23 of 23 slots free before the first run and after the last. I neither started nor stopped it.
- Every build and test went through `tools/timed`. The rows are in this record's campaign log, `2026-11-08-wu14-follow-ups-review/runs.tsv` (55 rows, 848 s of wall time in all).

## Departures, skipped checks, ambiguities (C20)

- **The first lint run was invalid, because of my own error.** I ran the lint build in the review worktree while `tools/mutate` was applying a mutant to `blade_product.hpp` in the same worktree. The lint build compiled the mutant, and `plan_test.cpp` failed its static assertions. I reran it after the mutants had finished, on a clean tree (`git status` showed nothing under `include/` or `test/`): no warning, exit 0. The failed run's row stays in the log (step `build`, preset `lint`, exit 1).
- **Merge-level checks not run here:** gcc-release, clang-release, gcc-asan, clang-asan, `clang-measure`, and the full `lint` preset. They belong to the coordinator's `gate:all` (plan section 7). I ran the full suite on gcc-debug and clang-debug, plus lint of the four changed programs and `format-check`.
- **The math-tools harness was not rerun.** It takes about 13 minutes and runs outside Galoot. I read the result rows, the manifest, and its hash, and checked them against note 0025.
- **Not checkable from the records:** note 0025's "30.7 s" for the second harness run. The rows give only the check walls, 4.18 s and 7.34 s.
- **`adfabd6` was not built on its own.** Its `test/` tree equals the tip's, apart from the `test/CMakeLists.txt` label line, so the runs at the tip cover it. I did build and test `45cf6cb` on its own.
- **Ambiguity in the ruling.** Decision 0218 item 2 says to append the forms to the existing lists "so existing draws do not change". Taken literally, appending to `up_to_three` or `small_forms` would shift every later group's draws, so the ruling contradicts itself. The plan's reading (section 1 item 4) puts the forms in lists of their own, checked last. That is the only reading that meets the stated purpose, and I reviewed against it.
- **Ambiguity in the ruling.** Decision 0218 item 1 says "a Theorem R structure-layer family is added to the identity suites". The unit adds the family at the kernel layer (`mv`, n ≤ 5, 41 forms) and leaves the structure layer (`for_each_signature`, n ≤ 6) as the plan's O2. The plan states this reading (section 1 item 2). See owner item O2 below.
- The plan's review section names `ba12e5a` as the base and the brief names `774ff66`. The only commit between them is the plan's claim, so the two diffs are the same apart from the plan file.

## What was checked, and how

### Theorem R against the tests, with the signs recomputed by an independent route

- **The statement.** `knowledge-base/geometric-algebra/proofs/test-retirement.md` (at `e6057ce`, lines 31 to 35) states three conditions: the unit; the generator rule T(e_i, e_B) = e_i ⌋ e_B + e_i ∧ e_B; and the recursion T(e_A, e_B) = T(e_a, T(e_A′, e_B)) − T(e_a ⌋ e_A′, e_B), for a the lowest index of A and |A| ≥ 2. Together they make T the Clifford product in the wedge basis. The tests check exactly conditions 2 and 3:
  - `check_generator_rule`, `test/core/identities_checks.hpp:729`;
  - `check_recursion`, `:754`;
  - condition 1 is the existing "1 ⟑ a = a" on every blade, in `check_basis_triples`, `:532`, which runs on the same full layouts.

  The wedge basis is CONVENTIONS 1.1. I read the paper proof's induction step by step (3.1 to 3.5) and found no gap. Theorem R is a paper proof only, and the unit's text says so (note 0025, "What is not machine-checked").
- **The signs, by a route that shares no formula with the test.** My script builds the Clifford algebra from the defining relation alone: words are normal-ordered with e_j e_i = −e_i e_j + 2 g_ij and e_i e_i = g_ii. The script then converts to the wedge basis by antisymmetrizing over all orderings, (1/k!) Σ sgn σ, and compares every e_i ⟑ e_B with the generator rule. It also checks the recursion on every pair. The script is committed beside this record as `generator_rule.py` and ran under the math-tools venv's Python with sympy.

  | Form | Generator-rule pairs that mismatch | Recursion pairs that mismatch |
  | --- | --- | --- |
  | [[1,1,0],[1,0,1],[0,1,−1]] | 0 of 24 | 0 |
  | all-ones 3 × 3 | 0 of 24 | 0 |
  | a conformal-type 5 × 5 form | 0 of 160 | 0 |
  | a dense 4 × 4 form, det 36 | 0 of 64 | 0 |

  For example, in the off-axis form e2 ⟑ e13 = −e1 + e3 − e123. The test's sign counting in `contraction` (`:663`: (−1)^p over the sorted index list) and `add_wedge` (`:678`: (−1) to the count of generators below i) is the same rule. Since the test passes on both compilers, Galoot's products of these forms agree with this independent route.
- **The library's algorithm.** `include/galoot/core/blade_product.hpp:235–262` computes every product by exactly the recursion of condition 3, with the generator step (`:195–225`) as its base. So the recursion case can catch only faults in the correction term, or in the plan and `mv` layers. Faults inside the generator step leave it true. My mutants confirm both halves (below). This is inherent in checking Theorem R's conditions against a library built on them, and the generator-rule case covers the other half. I record it as a note, not a defect.
- **Counts.** The two new cases give 82 assertions at the tip: one `CHECK` per form per case, so 41 full-layout forms, matching the plan's outcome. They take 0.25 s together on gcc-debug.

### The new forms: rank and radical

I computed these with sympy:
- **[[1,1,0],[1,0,1],[0,1,−1]]:** rank 2, det 0, null space spanned by (−1, 1, 1), that is, by e1 − e2 − e3. g_22 = 0, but e2 is not in the radical. This matches the comments in `signatures.hpp:14–19`, `identities_up_to_three_test.cpp:62–66`, and `dual_identities_test.cpp:165–170`.
- **The all-ones form:** rank 1, with the radical spanned by e2 − e1 and e3 − e1. No generator lies in it, and no generator squares to zero. This matches the comments.
- **Is anything in the core sensitive to where a radical lies?** `is_diagonal` and `is_null` have no caller under `include/`. This supports the mutation README's reason for giving K9 no mutant (inferred: I found nothing that branches on the form's shape).

### "Existing draws unchanged": shown by a run, beyond the reasoning

- **By reading.** Each test case seeds a fresh `std::mt19937_64` (`identities_test.cpp:23–25`; `dual_identities_test.cpp:484, 527, 568`). The groups are visited in the order of `groups`, and `forms_off_axis` is last (`identities_checks.hpp:868–873`). The dual suite's `off_axis_forms` is the last call in its two cases. Draws happen only in `random_integers`, and the new cases draw nothing. `blade_product_test.cpp` and `plan_test.cpp` draw nothing from a seed.
- **By a run.** I made two scratch worktrees, at `774ff66` and `7d9e22f`, and instrumented `random_integers` in both suites to print every drawn coefficient (`instrument.py`). I built `core_identities_test` and `core_dual_identities_test` on gcc-debug and ran each old test case alone at seeds 0 and 12345 (`draws.sh`).

  In all 20 comparisons, the old program's draw sequence is an exact prefix of the new one's. The extra draws (6 to 18 lines per case) come only at the end. The bulk-and-weight case and the basis-blade case are unchanged. Results: `draws.txt`.
- The mutation README says the property holds "by construction, not by a run". That is now backed by a run as well.

### Mutants of my own (`review.toml`, judged by `core.identities` at the tip on gcc-debug and clang-debug)

Predictions were written into the spec before the run.

| Mutant | Edit | Tip gcc / clang | New cases among the failed | Branch point |
| --- | --- | --- | --- | --- |
| `wedge_sign_on_grade_two` | e_i ∧ e_B with its sign flipped for \|B\| ≥ 2, n ≤ 4, in the generator step | killed / killed | generator rule only; the recursion case passes, as predicted | not run (older cases kill it at the tip) |
| `recursion_sign_flipped` | the recursion's correction term added instead of subtracted, n ≤ 4 | killed / killed | recursion only; the generator rule passes, as predicted | not run |
| `resigned_e123` | the table on the basis in which e123 stands for −e123 | killed / killed | both, and **only** the two new cases | **survived / survived** |

- **`resigned_e123` is a second, independent gap mutant.** The suite before the unit lets it survive on both compilers, and at the tip only the two new cases kill it. So the gap is a general one: the old suite does not pin the basis. It is not special to e12.
- **The first run of `wedge_sign_on_grade_two`** flipped the sign at every n. gcc-debug killed it, with the same failed cases. clang-debug left it unjudged: `identities_five_dense_test.cpp` hit `plan.hpp:553`, "constexpr variable 'count' must be initialized by a constant expression". This is the same limit the author met. Narrowed to n ≤ 4, both compilers judged it. The first run's rows are in `out/first-run/`.
- **The author's mutants were not rerun.** I read their rows instead (`out/*.tsv`, `out/branch-point/`, `out/first-run/`). They agree with the README's table:
  - `resigned_e12` survived at `774ff66` on both compilers, and both new cases kill it at the tip;
  - `recursion_contraction_doubled` is killed, with the recursion case among the failed cases.

### Label

- `ctest -N -L '^core\.headers$'` lists `tests.messages_pinned`, `core.bare_compile`, and `core.include_scan`, on both gcc-debug and clang-debug.
- **No other selection broke.** I compared, for each of the 59 labels, the count of tests that `-L '^<label>$'` selects at `774ff66` and at the tip (gcc-debug trees). The set of labels is the same, and only `core.headers` changed, from 2 tests to 3. The unanchored `-L '^core'` now selects 407 tests instead of 406. No committed spec uses that selection; the only unanchored selection on record is `'^rga3'` in `records/mutations/0023-rga3/mutants.toml`.

### Suites and gates at `7d9e22f`, in this worktree

- `cmake --workflow` gcc-debug: 499 of 499 passed. clang-debug: 499 of 499 passed. `ctest -N` lists 500; the `runlog` test is left out by the presets.
- Lint of `core_identities_test`, `core_dual_identities_test`, `core_blade_product_test`, `core_plan_test`, and `format-check`: exit 0, no warnings (second run; see Departures).
- Convention trace gate: exit 0. The only items without a test are 6.1 to 6.4, as before.
- `tools/runlog check`: passes.
- `45cf6cb` on its own (scratch worktree, gcc-debug): built, and `-R '^core\.(identities|dual_identities|blade_product|plan)$'` passed. So the first test commit meets W2.

### Cost record (`records/performance/wu14-follow-ups-cost/`)

- I recomputed the means from `out/rows.tsv` (156 rows, three repeats per unit, tree, and compiler). Every instruction mean and percentage in the README matches, as do the totals for the ten sources: GCC 461.0 → 475.4 (+3.1%), Clang 450.2 → 460.2 (+2.2%). The largest range is 1.3% of a mean, as stated.
- Three of the README's wall times differ from my means by 0.01 s, a rounding difference: `identities_five_dense` GCC 4.04 against 4.03, `identities_sparse` Clang 2.51 against 2.50, `identities_up_to_three` Clang 3.54 against 3.53.

### Research note 0025 against the harness rows

These are in `math-tools/exercises/logic/build/proof-results.tsv`, with math-tools at `d698d34`, clean:

- **The rows.** The 15 obligations are those in the manifest: ga-P7 to ga-P15 and logic-E14, E17, E18, E19, E11, E20. Every hash in the note equals its row. The manifest's SHA-256 equals `3ba3450f…4d14159`. The first run's rows end at 04:01:59Z, and logic-E20's wall is 491.76 s. A start at 03:49:10Z gives 12 min 49 s. The rows' own walls sum to 741 s, which is consistent.
- **The failures and the replay defect.** ga-P7 and ga-P11 have FAIL rows with "replay failed (exit 1)" and PASS rows at 04:03:01Z and 04:03:16Z with replay PASS. The manifest's replay commands for these two (lines 31 and 86) do `cd exercises/ga/proofs/rocq && … OPAMROOT=${MT_SRC:-$PWD}/installed/opam-root`. `$PWD` is expanded after the `cd`, so the note's description of the defect is right.
- **Trust reports.** ga-P9 trusts `propext`. logic-E19 trusts `Classical.choice`, `Quot.sound`, `propext`, and six `native_decide` axioms on sample products. logic-E17's exceptions are `audit=none, src:ML`. All agree with the note.
- **Galoot's headers.** `ba12e5a:include` is tree `4fd84ca5…`, and `include/` is unchanged from `ba12e5a` to `7d9769b`.
- **Theorem R's status.** ga-P6 is not in the manifest. The math-tools backlog (line 179) lists Theorem R as not formalized. The note calls Theorem R a paper proof and says its conditions were checked on oracle tables outside the harness. It does not overclaim.

### Process

- **Cherry-picks.** `45cf6cb`, `adfabd6`, and `a5c2608` have patches identical to `5975ca4`, `5388177`, and `1df105b`. Their messages are identical apart from the `-x` line, and the sources' parents form the chain from `774ff66`.
- **Commits.** Each commit message says what changed and why, and names the agent and the model ("Agent work: Claude (Opus 5.5) …" plus the `Co-Authored-By` trailer). Tests are in their own commits, apart from the label commit (W9). No commit needed W2's failing-test label.
- **No agent citations in human-facing files.** The diff adds no decision, plan, or provision citation under `test/`, `include/`, `tools/`, the build files, or `README.md`.

## Findings, most severe first

No finding blocks the merge.

- **R1 (should-fix, records, confirmed): the seed campaign's rows are tagged `-dirty`, and the README does not say so.** In `records/mutations/0031-wu14-follow-ups/seeds/runs.tsv`, 64 of the 100 rows (seed 19 onward, from 04:11:49Z) carry revision `6e34e678783a-dirty`. Meanwhile, `README.md:23` says the campaign ran "on gcc-debug at `6e34e67`". The programs were built at 04:06:49Z from the clean tree (`records/performance/runs.tsv`), and no build ran in the unit's worktree until the campaign ended at 04:20:23Z. So the runs did test `6e34e67`'s programs. The dirty files were most likely records being written meanwhile (inferred; not recoverable from the rows). Scenario: a reader auditing V10 finds dirty rows behind a result that is reported as clean, with no explanation. Fix: one sentence in the README.
- **R2 (note, confirmed): note 0025's provenance has two small imprecisions.**
  - Its tools row says "as each row records them" but lists g++ 16.2.1 and clang++ 23.1.4, which the rows do not record. I confirmed both versions on the machine with `--version`.
  - The "30.7 s" for the second run cannot be recovered from the rows.

  Fix, optional: say where each figure comes from.
- **R3 (note, confirmed): rga3 headers do not select `tests.messages_pinned`.** The catalogue scans the whole of `include/galoot`, rga3's `motions.hpp` included, since it has `static_assert` and `GALOOT_ASSERT` messages. But the new label is only `core.headers` (`test/CMakeLists.txt:182`). A change to an rga3 header selects `rga3.headers`, so a new rga3 message is caught only at a merge's full suite. That is the gap the ruling closed for the core. Decision 0218 item 6 names only `core.headers`, so this is within the ruling; see owner item O5.
- **R4 (note, confirmed): the generator rule's right side reads the Gram matrix from the algebra's metric** (`identities_checks.hpp:670`, `Alg::value.gram`). A fault in `from_gram` that stored a wrong but symmetric matrix would move both sides of this check together. The metric's own tests cover that path. This is no defect of the unit; it bounds what the case shows.
- **R5 (note, confirmed): the recursion case cannot see faults in the generator step**, because the library is built on the same recursion (see above). `wedge_sign_on_grade_two` passes the recursion case and fails the generator rule. The two cases are complementary, as Theorem R requires, so neither may later be dropped as redundant.
- **R6 (note, plausible): two failing runs on the tests branch are not explained in the records.** At 03:52:18Z, a `-L '^core\.(identities|dual_identities|blade_product|plan)$'` test run exited 8 and was followed by a narrower `-R` run that passed. The `-L` selection includes about 40 death and compile-fail tests (`core.plan_beyond_n` and others), whose programs a four-target build does not make. That is the plausible cause. The plan's checks table names the `-L` selection, but the passes it reports used `-R`. The full suite at the tip passes, so nothing is hidden; only the plan's description is imprecise. The exit-8 runs of `-L '^tests\.'` at 03:55:46Z and 03:56:12Z are explained in `adfabd6`'s message (a marker wording change, then a pass).
- **R7 (note, confirmed): rounding.** Three wall times in the cost README differ from the rows' means by 0.01 s (listed above).

## Outcome

**Approve, with non-blocking findings.** R1 is a one-sentence records fix, which can come before or at the merge. The merge still needs the `gate:all` matrix of P9 (release, sanitizers, `clang-measure`, full lint), which this review did not run.

## Open items for the owner

- **O1. The governance sentence for criterion 12 (plan 0031, O1).** Options:
  - (a) put it in WORKFLOW W3;
  - (b) put it in VVUQ V2;
  - (c) leave it in each plan.

  Tradeoffs: (a) puts the rule where plans are defined, at the cost of one more W3 clause. (b) frames it as evidence, which is closer to what it means, but V2 is about single tests. (c) needs no ratification, but depends on each planner remembering decision 0218. Recommendation: (a), as the author recommends. `resigned_e123` shows the rule does work: it found a second gap mutant that the old reading would not have credited.
- **O2. Theorem R at the structure layer (plan 0031, O2), and the reading of decision 0218 item 1.** The ruling says "structure-layer family". The unit's family runs at the kernel layer, n ≤ 5. Options:
  - (a) a small follow-up that adds the three conditions over `blade_product` on `for_each_signature`, which reaches n = 6 and the Witt and random forms, at 4^n pair checks per form;
  - (b) accept the kernel-layer family as meeting the ruling.

  Tradeoffs: (a) covers the sixth generator and is the step toward the test retirement the knowledge base proposes, at the cost of a little compile or run time. (b) costs nothing, but leaves n = 6 under the triple loops only. Recommendation: (a), after WU12's plan is settled, as the author recommends. It is also the literal reading of the ruling.
- **O3. math-tools' replay path (plan 0031, O3).** Options:
  - (a) set the opam root before the `cd` in ga-P7 and ga-P11;
  - (b) document that `MT_SRC` must be exported.

  Tradeoffs: (a) is a two-line manifest change in another project, made by a math-tools agent. (b) leaves a trap for every run from a plain checkout. Recommendation: (a).
- **O4. The readings of the form lists (plan 0031, O4).** Recommendation: confirm. The literal ruling cannot keep the draws, and my run shows that the reading does keep them.
- **O5. `rga3.headers` on `tests.messages_pinned` (R3).** Options:
  - (a) add the label;
  - (b) leave it to the full suite.

  Tradeoffs: (a) costs about 0.1 s on each rga3 header change and closes the same gap the ruling closed for the core. (b) keeps the label set as ruled. Recommendation: (a), in any later unit that touches `test/CMakeLists.txt`.
- **O6. R1's sentence.** Recommendation: the merging agent adds it to `records/mutations/0031-wu14-follow-ups/README.md` before the merge, as a records-only commit on the unit.

## Files beside this record

`2026-11-08-wu14-follow-ups-review/`:

- `review.toml`: my mutant spec;
- `out/`: its rows at the tip, at the branch point (`branch-point/`), and the first, unnarrowed run (`first-run/`);
- `generator_rule.py`: the independent recomputation;
- `instrument.py`, `draws.sh`, `draws.txt`: the draw comparison;
- `runs.tsv`: this review's run log.
