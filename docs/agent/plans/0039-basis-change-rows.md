# 0039 basis_change: `same_rows` for `==`, rows reduced before the checks, and `.fits = fits`

- Status: done 2026-11-10; merged into `develop` by its author after the blind review, as the brief asks. Claimed 2026-11-10 by Claude (agent, Opus 5.5, cpp-expert and numerical-expert roles), planner and implementer, from `develop` at `ca6939d`.
- Source of scope: decision 0225 items 3 and 4 (the rename of equality on `basis_change` before release; each row reduced by its gcd before the checks, with N2 of the WU12 delta review); decision 0223 item 4 (what `==` compares, documented until then); the WU12 blind review's N3 (`records/reviews/2026-11-09-wu12-blind-review.md`) and the WU12 delta review's N2 and N3 (`records/reviews/2026-11-09-wu12-delta-review.md`); plan 0030's open items; research note 0026 (what a change preserves; nothing here changes it).
- Branches, from `develop` after this claim (W7):
  - `feature/basis-change-rows`, worktree `.claude/worktrees/basis-change-rows`: the unit. Only this branch merges into `develop`.
  - `feature/basis-change-rows--tests`, worktree `.claude/worktrees/basis-change-rows--tests`: the test commits, never merged; cherry-picked with `-x` into the unit (decision 0185).
- Record locations, free on every branch at this commit: `records/mutations/0039-basis-change-rows/`, `records/performance/basis-change-rows-cost/`, decision 0235 (the implementer's choices, committed with this claim). Research note 0027 is plan 0038's, so the proof of section 5 lives here rather than in a note.

## 1. Departures, ambiguities, and readings (C20)

1. **"Genuinely overflowing rows."** Read as: rows whose reduced form still makes a check overflow. Two of the four overflow compile-fail tests use rows that the reduction makes small (`core.basis_change_overflow`, k e1 = m e'1 / m; `core.basis_change_overflow_elimination`, m e'_i over m in R(3, 0, 0)); after this unit both changes are accepted. Their inputs move to the new acceptance tests, and each test gets a reduced input that overflows in the same check (section 4, criterion 5). The change of the guarded result is stated here before the change, as W9 asks of a regression guard. The other two (`_inverse_check`, `_inverse_rows`) overflow in the same check after the reduction (section 5.4) and keep their inputs.
2. **What remains spurious.** An overflow that the reduced rows still cause is reported as before, though the change's results may fit: the new input of `core.basis_change_overflow` is e1 = e'1 + e'2 / m, whose P and P⁻¹ are small, but whose integer check M G' Mᵀ = Δ G Δ forms m² k. The delta review's option (c) (skip the discarded left-block entries of the elimination) and an isometry check in reduced fractions would remove more; neither is in scope. Open item O2.
3. **The name.** The brief offers `same_rows` and asks for the best name with the hci view. Chosen: `same_rows(a, b)`, a free function (decision 0235 item 1 weighs `same_rows_and_denominators`, `same_entries`, and a member). No `==` remains on `basis_change`; a test pins its absence.
4. **N2 has no killable mutant.** `.fits = fits` at the elimination's two later returns states the same value as `true` there, because the loop has returned on `!fits` before either is reached. Its mutant (`true` restored) is declared equivalent with that reason. N2 is a change of form for the reader and for a later edit that drops the early exit; no test can tell it apart, and none is added.
5. **`detail::inverse_change`** (used by `basis_change_identities_test.cpp` only) is left as it is: it inverts the rows as written, and its comment says that every intermediate must fit. The tag checks, which every public path uses, reduce. Not a departure from the brief, which names the checks.
6. **The misuse catalogue record** names places in `basis_change.hpp` by line; the header's lines move, so its rows of that header are regenerated at the end (a record step, as plan 0030 did).

## 2. Goal

Three small changes to `include/galoot/core/basis_change.hpp`, with their tests:

1. Equality on `basis_change` compares the stored rows and denominators, not the matrix P they stand for. Replace `operator==` by a named function whose name says what it compares, so no user reads `==` as matrix equality.
2. The checks of a change tag run on the rows as written, so a row with a large common factor, such as m e'_i over m, overflows `coefficient_type` though the change is the identity. Divide each row and its denominator by their gcd before the checks and the term generation.
3. `eliminate` returns `.fits = fits` at its later returns instead of `true`.

## 3. Definition of done

1. `basis_change` has no `operator==`; `same_rows(a, b)` is `[[nodiscard]] constexpr`, compares rows and denominators entry by entry, and its `///` says it does not compare matrices. Every former use in tests compiles with it; the README needs no change (it compares no changes).
2. `check_change` reduces each row by the gcd of its entries and its denominator after the denominator check and before every other check; the reduced rows are the forward rows the term generation reads.
3. `eliminate`'s two later returns state `.fits = fits`.
4. Tests first on the tests branch, cherry-picked with `-x`; the labelled failing commits name every test they leave failing (W2).
5. Mutants of section 8, run with `tools/mutate` on gcc-debug and clang-debug at the tip, with the gap run at the branch point; the 0030 specs' mutants whose code this unit edits run again; the 50-seed campaign of both programs.
6. Before and after build cost of the affected suites (W10).
7. Blind review, and a delta review after any non-comment fix; the P9 matrix from empty trees at the final tip; merge into `develop` with a P13 line; worktrees and branches removed after their content is verified on `develop`.

## 4. Acceptance criteria

1. **`same_rows`.** Pinned by `static_assert`s in `basis_change_test.cpp`, with expected values from the definition (the arguments of `from_rows`): true for a value and itself, for `conformal_change<1>()` and `<3>()` against their rows written out, and for `random_1`'s change against its rows; false for rows that differ with equal denominators, for equal rows with denominators that differ, and for a row scaled with its denominator (the same P). `std::equality_comparable<core::basis_change<2>>` is false.
2. **The reduction itself.** `detail::reduced(from_rows({{6, -4, 0}, {2, 4, 0}, {0, 0, 7}}, {2, 3, 7}))` has rows `{{3, -2, 0}, {2, 4, 0}, {0, 0, 1}}` over `{1, 3, 1}`: the first row's divisor is 2, the second's 1 (its entries share 2, its denominator does not), the third's 7. Expected values by hand from the definition.
3. **Large unreduced rows are accepted and give the results of the reduced rows.** Three tags, each beside the tag of its reduced rows:
   - the identity of R(3, 0, 0) written as m e'_i over m, m = 2³¹ − 1 (the old input of `core.basis_change_overflow_elimination`), against the identity;
   - k e1 = m e'1 / m on the form of one generator of square k = `max_form_entry` (the old input of `core.basis_change_overflow`), against e1 = e'1;
   - 2I to R(2, 0, 0) with rows m(1, 1) and m(−1, 1) over m (the delta review's probe), against rows (1, 1) and (−1, 1) over 1.
   For each pair, on the full layout both ways: the maps' result layouts, terms, and denominators are equal (a field-by-field comparison in the test), and `change_basis` and `change_basis_back` of one rational value give equal results. The full layout of the first pair holds e123, whose image as written is m³ (more than 2⁶³), so a generation from the rows as written cannot pass.
4. **The verdicts do not change otherwise.** Every existing test passes unchanged in its expected values, apart from the inputs of criterion 5.
5. **Overflow still fails with the pinned messages.** `core.basis_change_overflow` gets source = target = the form diag(k, 0) and e1 = e'1 + e'2 / m, e2 = e'2: an isometry (e'2 is null), invertible, reduced (gcd(m, 1, m) = 1), whose elimination fits and whose check of the inner products forms m² k > 2⁶³. `core.basis_change_overflow_elimination` gets R(0, 0, 3) to itself with e_i = m e'_i (every invertible change keeps the null form), reduced, whose elimination forms m⁴. Both keep their CMake rows (the overflow message, one error, its FORBID). The replica of section 5.4 shows each is rejected at the stage named, both as written and reduced.
6. **N2** as section 1 item 4.
7. **Mutants** as section 8, each verdict as declared.
8. **Seeds**: `core_basis_change_test` and `core_basis_change_identities_test` with `--rand-seed=1` to `50` on gcc-debug at the tip, through `tools/timed --log` into the mutation record's `seeds/runs.tsv`, all passing.
9. **Checks** of section 9 pass at the tip.

## 5. The mathematics

This section proves what the change relies on. It assumes linear algebra and nothing from formal methods.

### 5.1 Notation

A change holds, for each row i, integers r_i1, …, r_in and a denominator d_i, and stands for the matrix P with P_ij = r_ij / d_i. Write M for the integer matrix of the r_ij and Δ for the diagonal matrix of the d_i, so P = Δ⁻¹ M. The reduction takes, for each row, g_i = gcd(d_i, r_i1, …, r_in) and replaces r_ij by r_ij / g_i and d_i by d_i / g_i. Write M′, Δ′ for the result and Γ for the diagonal of the g_i, so M = Γ M′ and Δ = Γ Δ′.

### 5.2 The reduction is the same change

The checks reduce only after the denominator check has passed, so every d_i > 0. Then g_i divides d_i, so 1 ≤ g_i ≤ d_i; d_i / g_i is a positive integer; and each r_ij / g_i is an integer. P′ = Δ′⁻¹ M′ = (Γ⁻¹ Δ)⁻¹ (Γ⁻¹ M) = Δ⁻¹ Γ Γ⁻¹ M = P. So the reduced rows name the same P, their denominators are still positive, and |r_ij / g_i| ≤ |r_ij|, so every entry still fits `entry_type`. A row with g_i = 1 is left as it is.

### 5.3 Every value the checks form shrinks or stays

Claim: each product and sum that a check or the term generation forms from M′ and Δ′, computed exactly, is the value it forms at the same place from M and Δ divided by a positive integer. These are the values whose range the noting arithmetic and `change_multiply` check. So no checked value grows, and a check that fits on the rows as written fits on the reduced rows. (Corrected after the blind review, its N1: the first wording said every value, but two quotients of `typed_change`, `q / gcd(q, own)` and `q_B / own`, can be larger reduced, as for rows (3, 3) / 6 and (2, −2) / 6. A quotient is not range-checked, it is at most its dividend, and the products that use it scale as the claim says.)

- **The elimination.** Bareiss's method keeps, after step k, entries that are minors of [M | I] on the rows {0, …, k, i} and the columns {0, …, k, j}, and forms each from a difference of two products of such minors, divided by the previous pivot. A minor is linear in each of its rows. In [M | I], row i is (g_i M′_i | e_i); a column of I meets only one row, so a minor that uses the identity's column for row l either is 0 or expands along that column to a minor of M without row l. So each minor of [M | I] is the matching minor of [M′ | I] times the product of g_l over the rows of M it uses: a positive integer t. In the difference a_kk a_ij − a_ik a_kj both products use the same multiset of rows of M (rows 0 to k − 1 twice, k and i once each, less the identity's row of column j in both), so both carry the same factor and so does their difference; where the identity's row is k or i, one of the two products is 0 and the other carries the factor alone. The quotient by the previous pivot is again a minor of [M | I], so its factor is given by the first sentence. Zero minors stay zero, so the pivot search finds the same rows and makes the same swaps. The last pivot is p′ = p / det Γ, with the sign of p.
- **The inner products.** The check compares M G′ Mᵀ with Δ G Δ entry by entry. (M G′)_il = g_i (M′ G′)_il; each partial sum of (M G′ Mᵀ)_ij is g_i g_j times its reduced counterpart; d_i G_ij = g_i d′_i G_ij and d_i G_ij d_j = g_i g_j d′_i G_ij d′_j. Since Γ is invertible, M G′ Mᵀ = Δ G Δ holds exactly when M′ G′ M′ᵀ = Δ′ G Δ′ does, and both say P G′ Pᵀ = G.
- **The check of the inverse.** The right block is p M⁻¹, which up to the sign of the swaps is the adjugate of M. Its entry (k, j) is a cofactor of M without row j, so it is the reduced one times det Γ / g_j. A term of (M · p M⁻¹)_ij is then r_ik times that entry, the reduced term times g_i det Γ / g_j, and g_j divides det Γ. The identity M (p M⁻¹) = p I holds for both or neither.
- **The inverse's rows.** Row i of P⁻¹ = M⁻¹ Δ is formed as (p M⁻¹)_ij d_j over p. As written that is the reduced value times (det Γ / g_j) g_j = det Γ. Both are then divided by the gcd of the row and |p|, which gives row i of P⁻¹ in lowest terms with a positive denominator: the same output from both.
- **The term generation.** The image of e_A is formed by wedging the rows of A one at a time; each partial coefficient is a minor of M on the rows used so far, so it carries the product of their g_l. The denominator of the image is the product of the d_l of A, which carries the same product. The running least common multiple of the terms' denominators, formed as the product `(q / gcd(q, own)) · own`, divides the one formed as written, because each reduced denominator divides the one as written; and each coefficient brought over q_B, `c · (q_B / own)`, is c · q_B / own, the written one times q′_B / q_B. So every product is the written one divided by a positive integer, as before.

### 5.4 The results do not depend on the reduction

Each result coefficient of `change_basis` is y_B = Σ_A s_A det P[A, B] x_A, a fixed list of rationals per result blade B. The generator writes it as integers c over one positive q_B that shares no factor with all of them, which is unique for the list: q_B is the least positive integer that makes every q_B · s_A det P[A, B] an integer, and c is that product. The terms are the pairs (A, B) with det P[A, B] ≠ 0, which does not depend on M's scaling (det M[A, B] = det P[A, B] · Π_{l∈A} d_l), in the order of the layout and of the canonical blades. So the map of terms, result blades, and denominators is a function of P alone, and the same holds for `change_basis_back` through the inverse's rows of 5.3. The generated code is identical, and so is every result, for every scalar, rounding ones included. Criterion 3 tests this on three pairs.

The replica `records/mutations/0039-basis-change-rows/witnesses.py` (a transcription of `check_change` written from the header, with the reduction as an option) gives, for the inputs of the tests: the old inputs of `_overflow` and `_overflow_elimination` and the delta review's probe overflow as written and pass reduced; the new inputs of criterion 5 overflow at the stage named both ways; `_inverse_check` (whose third row reduces by 2) and `_inverse_rows` (already reduced) overflow at their stages both ways.

## 6. Approach and order

0. Claim: this plan and decision 0235 on `develop`.
1. Tests branch, from the unit's branch:
   - T1: the new inputs of `core.basis_change_overflow` and `_overflow_elimination` (criterion 5). Passes before and after the change: the inputs overflow at the same stage either way.
   - T2: `same_rows` in place of `==` and `!=` in the tests, the new `static_assert`s of criterion 1. Labelled failing: `core_basis_change_test` does not build until the header has `same_rows`.
   - T3: criteria 2 and 3. Labelled failing: `core_basis_change_test` does not build (no `detail::reduced`, and the large rows overflow).
2. Unit: cherry-pick T1 to T3 with `-x`; then the header in one commit (the three changes and their comments).
3. Evidence: mutants, the gap run, the 0030 mutants rerun, the seeds, the cost campaign, the catalogue record's rows; records committed.
4. Blind review; fixes; delta review if a fix is not comments only.
5. P9 matrix from empty trees at the final tip; merge `develop` in first if it moved; merge; clean up.

## 7. Evidence (V1, V10)

| Claim | Evidence | Kind |
| --- | --- | --- |
| The reduction keeps P, keeps denominators positive, and keeps entries in range | section 5.2 | V1 kind 1 |
| No check can overflow on the reduced rows where it fit on the rows as written; the verdicts agree | section 5.3 | V1 kind 1 |
| The generated maps, so every result, do not depend on the reduction | section 5.4; criterion 3 on three pairs | V1 kinds 1 and 3 |
| `same_rows` compares what its comment says; no `==` exists | criterion 1 | V1 kind 3 (values from `from_rows`'s arguments) |
| The overflow tests still see an overflow in each check | criterion 5; the replica | V1 kind 3; the replica is `[checked-self]`, a second transcription |
| The tests see the changes | section 8 | mutation record |

Validation (V8): not applicable; no new mathematics reaches users beyond the same changes accepted over a wider range of inputs. Verification is on GCC 16.2.1 and Clang 23.1.4, the installed compilers.

## 8. Mutants (W3)

Specs in `records/mutations/0039-basis-change-rows/`, run by `tools/mutate` on gcc-debug and clang-debug. The selection is `^core\.basis_change` (both programs and the compile-fail tests).

The rename's denominator test closes a gap in an existing suite: `core.basis_change` pinned `==` only on values whose rows or whose rows and denominators both differ, so an equality that ignored the denominators passed it. That gap is judged by the W3 rule: a mutant counts for the gap when the suite without the new tests lets it survive and the suite with them kills it. `gaps.toml` runs at the branch point (`ca6939d`'s header and tests, in a worktree detached there), with `operator==` comparing the rows only, `expect = "survived"`; and at the tip, with `same_rows` comparing the rows only, `expect = "killed"`. Every other mutant is written for a new test, and counts when that test is among those it fails (for a test that is a `static_assert`, the failed object is the test's source):

| Mutant (`rows.toml`) | Edit | Written for | Expected |
| --- | --- | --- | --- |
| `rows_not_reduced` | `check_change` checks and generates from the rows as written | criterion 3 | killed |
| `reduction_keeps_denominator` | entries divided, the denominator not | criteria 2 and 3 | killed |
| `reduction_gcd_without_denominator` | the divisor from the entries alone | criterion 2 | killed |
| `forward_rows_as_written` | checks on the reduced rows, terms from the rows as written | criterion 3 (e123's m³) | killed |
| `same_rows_ignores_rows` | `same_rows` compares the denominators only | criterion 1 | killed |
| `equality_restored` | `operator==` defaulted again in `basis_change` | criterion 1 (`equality_comparable`) | killed |
| `elimination_fits_hard_coded` | `true` at the two later returns | N2 | equivalent: the loop has already returned on `!fits` |

The 0030 specs edit code that this unit changes (`check_change`, `eliminate`): `change.toml`, `review_fixes.toml`, `new_cases.toml`, and the delta review's `mutants.toml`. Each is run with `--check` at the tip; the mutants whose anchors lie in the changed functions run again, with their rows in this unit's record, and a mutant whose anchor the change removed is rewritten in a copy here, keeping its fault.

## 9. Checks (P8, P9, P13)

| Point | Checks | Presets |
| --- | --- | --- |
| T1 (tests branch) | `core.basis_change_overflow`, `_overflow_elimination` | gcc-debug, clang-debug |
| T2, T3 | `^core\.basis_change` | gcc-debug (expected to fail to build `core_basis_change_test`) |
| The header commit | every test of a component that includes `basis_change.hpp` (`^core\.basis_change`, `core.headers`, `tests.messages_pinned`, and the `includer`/header-set checks) | gcc-debug, clang-debug |
| Before the merge | the P9 matrix from empty trees: gcc and clang debug and release, gcc-asan and clang-asan, lint, clang-measure, `format-check`, the trace gate, `tools/runlog check` | all, as `gate:all` |

Every build and test through `tools/timed`, with `eval "$(tools/jobpool env)"` first; `tools/jobpool status` before and after each heavy run; the pool is started only when it reads "not running" and is never stopped.

## 10. Review plan (W5)

A blind review at the Opus level with high effort by the `blind-reviewer` role, sharing no context with this agent, given the diff from `ca6939d`, sections 3 and 4, and section 1, but not this agent's view that the change is correct. It applies the usability checklist (a public name changes). A delta review of any non-comment fix.

## 11. Agent cost estimate

Planner and implementer, Opus high: 250k to 400k tokens. Blind review: 100k to 200k; delta review, if needed, 50k to 100k. Machine time: mutants about 2 minutes per mutant and preset (about 40 minutes with the gap run and the 0030 reruns); seeds 1 minute; cost campaign about 10 minutes under the timing lock; matrix from empty trees about 20 minutes.

## 12. Open items

- O1. Whether `detail::inverse_change`, used by one test only, should reduce too, or be removed in favor of the checked rows (section 1 item 5).
- O2. Remaining spurious overflows of the reduced rows (section 1 item 2): the delta review's option (c), and an isometry check that reduces P G′ Pᵀ entry by entry in fractions.
- O3. The blind review's N4: `a == b` on two changes gets the compilers' generic error, which does not name `same_rows`. A C++11 `bool operator==(const basis_change&) const = delete;` with a comment naming `same_rows` keeps `std::equality_comparable` false and puts the hint on a note line that both compilers quote. Not applied: it is a non-comment interface change after the review.

## 13. Outcome

Done, 2026-11-10. The blind review (`records/reviews/2026-11-10-basis-change-rows-blind-review.md`, cherry-picked with `-x` from `1f2dbdd`) blocked on one layout failure (B1) and approved otherwise; its resolutions are in the record. The fixes after it change comments, one test's layout, and records only, so no delta review was made.

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `be3f045` (from `3a6734b`) | the new inputs of `core.basis_change_overflow` and `_overflow_elimination` | test, passing before and after |
| `70bbcf3` (from `92a0046`) | `same_rows` in the tests, and its `static_assert`s | test, failing when committed, as its message says |
| `dd67ead` (from `f4d6bf5`) | the reduction's hand values; the three scaled changes against their reduced forms | test, failing when committed, as its message says |
| `38925ad` | the header: `same_rows`, the reduction in `check_change`, `.fits = fits` | core |
| `fa86535`, `c7c0f1f` | mutation specs and runs | record |
| `0d4088d` | the blind review record | record |
| `b2d5dd7` | B1: the elimination overflow test formatted | test layout |
| `42b2c22` | N1 and N5: comments of `detail::reduced` and of `denominators`, and section 5.3 | comments, record |
| `cb345ec`, `d5465b1` | seeds, build cost, the catalogue's places, the review's resolutions | record |

Results against section 4:

- **Criteria 1 to 5.** The `static_assert`s and the test case "rows with a large common factor are the change without it" pass on every preset of the matrix. Every former use of `==` on a change compiles with `same_rows`; the README compares no changes and is unchanged. The replica (`records/mutations/0039-basis-change-rows/witnesses-out.txt`) and the reviewer's own transcription (`scaling_probe.py`, 9,000 random changes, about a million products and sums compared exactly) agree on every stage verdict and on the scaling of section 5.3, corrected for the quotients (N1).
- **Criteria 6 and 7.** `records/mutations/0039-basis-change-rows/README.md`: the six mutants written for the new tests are killed on both presets, each by the test written for it; `elimination_fits_hard_coded` is equivalent as declared; the gap mutant survives at the branch point and dies at the tip on both presets. Of the 13 mutants of plan 0030 run again, 12 are killed and `elimination_overflow_not_stopping` is now equivalent, as the N2 change makes it.
- **Criterion 8.** 100 runs, all passing (medians 0.031 s and 0.191 s).
- **Build cost (definition of done item 6).** `records/performance/basis-change-rows-cost/README.md`: retired instructions within ±0.09% on both compilers, about one range of the repeats; measured pinned to CPU 4 under the timing lock on a loaded machine (load average 4).
- **Criterion 9.** The P9 matrix at `d5465b1`, from empty build trees (the worktree's `build/` removed first), through `tools/timed` and the shared pool, each row named `gate:<selection>`, all passing: gcc-debug 544 tests (105.2 s), gcc-release 544 (110.0 s), gcc-asan 536 (234.4 s), clang-debug 544 (96.6 s), clang-release 544 (115.7 s), clang-asan 544 (214.3 s), lint 544 (267.0 s, no diagnostic), clang-measure 536 (119.4 s); `format-check` (1.7 s), the trace gate (0.3 s; items 6.1 to 6.4 without a test, as on `develop`), and `tools/runlog check` (2.6 s).

Departures and notes beyond section 1:

- The first mutation run declared a CTest test as each kill's killer, which `tools/mutate` cannot match for a kill at compile time; the rows list that problem and the verdicts stand (mutation record).
- The cost campaign ran while other workstreams loaded the machine; the instruction counts are the comparison, the wall times are only recorded.
- Two configure rows of the branch-point worktree went to its own run log before the campaign log was named; they are copied into `branch-point/runs.tsv`.
- After the matrix, `develop` gained records only, merged in before the merge into `develop`, with `tools/runlog check` on the result.

Open items: O1 to O3 of section 12.
