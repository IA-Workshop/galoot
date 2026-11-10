# Blind review of plan 0039 (basis_change: `same_rows` for `==`, rows reduced before the checks, `.fits = fits`)

- Reviewer: blind-reviewer role, Claude (agent, Opus 5.5), high effort; 2026-11-10. No shared context with the author.
- Change: `feature/basis-change-rows`, parent `develop`, range `3ddc371..fa86535` (four test commits cherry-picked with `-x` from `feature/basis-change-rows--tests`, the header commit `38925ad`, and records). Files: `include/galoot/core/basis_change.hpp`, `test/core/basis_change_test.cpp`, `test/core/basis_change_overflow_fail.cpp`, `test/core/basis_change_overflow_elimination_fail.cpp`, mutation specs, run rows.
- Reviewed in the worktree `.claude/worktrees/basis-change-rows-blind-review` (branch `records/basis-change-rows-blind-review`, at `fa86535`), with build trees of its own. Probes and their outputs are in `2026-11-10-basis-change-rows-blind-review/` beside this file.
- Outcome: **block**, on one finding (B1, a layout failure that `format-check` reports in a test file). With B1 fixed and `format-check` passing, approve with the non-blocking findings below.

## Whether the brief carried a verdict

No. The brief gave the range, plan 0039 sections 3 and 4 as the criteria, and section 1 as the departures, and named section 5 (the author's proof) as part of the change under review. The commit messages carry the author's own checks ("Checked: the full suite on gcc-debug and clang-debug, 544 of 544 each") and claims ("no accepted change changes and none is newly rejected"); they were treated as claims to test, not as evidence.

## Ambiguities in the brief and how I read them

1. **Which checks at which level.** The role asks for the P9 checks of this merge level (a merge into `develop`); plan 0039 schedules its own P9 matrix from empty trees after the review. I ran the matrix in my own new build trees (fresh, so equivalent to empty ones) at `fa86535`; the author's later run at the final tip is still needed (P10 applies only to an unchanged revision).
2. **Evidence the plan schedules before the review but that is not in the range.** Plan section 6 step 3 (mutant outputs, the gap run, the 0030 reruns, the 50 seeds, the cost campaign, the catalogue rows) comes before step 4, the review, but none of its outputs is committed at `fa86535` (a mutation run is in progress in the unit's worktree). I read criteria 7 and 8 and definition-of-done item 6 as not checkable at this revision, judged the specs that are present, and ran mutants of my own.
3. **Independence from the author's replica.** I read `witnesses-out.txt` (the output of the author's replica) but not `witnesses.py`, and wrote my own transcription of the header (`scaling_probe.py`) before comparing. Its stage verdicts agree with the author's output on every input.

## What I checked, and how

- **Diff and files.** `git diff 3ddc371..fa86535` and the log; `basis_change.hpp` read in full at the tip; the changed test files read in full in the diff and around it; decision 0233; plan 0039; the other two overflow compile-fail tests; the README's text on changes; the governance (CONSTITUTION, WORKFLOW, VVUQ, PERFORMANCE, STYLE).
- **The mathematics of plan section 5, independently.**
  - 5.2 (the reduction keeps P, keeps denominators positive, keeps entries in range): by hand; holds. g_i divides d_i > 0, so 1 ≤ g_i ≤ d_i; |r/g| ≤ |r|; an entry of `INT_MIN` is safe, since its magnitude 2³¹ fits `coefficient_type` in the gcd and its quotient by a g_i > 1 fits `entry_type`.
  - 5.3 (every value shrinks): re-derived the Bareiss minor argument (rows {0..k, i} of [M | I], the identity's column removing one g_l, the two products of a step carrying one factor or one being zero), the inner products (factor g_i g_j), the inverse check (factor g_i det Γ / g_j), the inverse rows (factor det Γ, removed by the gcd), and the generation (minors times Π g, lcm' | lcm). Then tested it with `scaling_probe.py`: a second transcription of `eliminate`, `keeps_inner_products`, `inverts`, `inverse_rows`, `blade_image`, `image_denominator`, and `typed_change`, run with exact integers on 9000 random changes (n from 1 to 4, seeds 1 to 3, row factors g_i up to 10⁶), comparing about 1.0 million products and sums position by position: every value as written was the reduced one times a positive integer, or both zero, with the same row swaps, and the forward and back maps of the full layout were identical. A mutated reduction (entries divided, denominators kept) is caught on 91 of 300 cases, so the probe can fail. The claim as stated is false for two quotients (N1), without consequence for overflow.
  - 5.4 (the map is a function of P): the canonical (c, q_B) with gcd(q_B, c…) = 1 is unique for a list of rationals (any other q is t times the least one and t divides every c and q); holds, and the probe's map comparison agrees.
  - The overflow inputs: `scaling_probe.py` gives, as written and reduced, the stage that overflows first. Old `_overflow` and `_overflow_elimination` inputs and the 2I probe: overflow as written, accepted reduced. New `_overflow` (diag(k, 0), k = 32767, e1 = e'1 + e'2/m): overflow in the inner products both ways (m k fits, m k m ≈ 2⁷⁷ does not; P G′ Pᵀ = G holds since e'2 is null). New `_overflow_elimination` (m I on R(0, 0, 3)): overflow in the elimination both ways (m² · m² at step 1). `_inverse_check` (third row reduces by 2) and `_inverse_rows`: overflow at their own stages both ways, so the reduction does not move either guard to a later stage.
- **Each acceptance criterion** (table below).
- **Mutants of my own** (`reviewer-mutants.toml`, run by `tools/mutate` on gcc-debug and clang-debug at `fa86535`): each later stage reading the rows as written while the others read the reduced rows (`elimination_as_written`, `isometry_as_written`, `inverse_check_as_written`, `inverse_rows_as_written`), the last row never reduced, a divisor of 2 kept: all killed on both compilers, by the header's assertions (`basis_change.hpp:740`, `:752`, `assert.hpp:71`) or the reduction's hand values (`basis_change_test.cpp:292`). `reduced_before_denominator_check` is equivalent on both, as declared (division by a positive gcd keeps each sign, and a zero denominator stays zero). The run exits 1 only because I copied the author's `killers` declaration (N2).
- **Two of the author's mutants** from a copy of `rows.toml` (`rows_not_reduced`, `same_rows_ignores_rows`, gcc-debug): both killed, both reported with a problem (N2).
- **Usability probes.** `equality_misuse_probe.cpp`: what `a == b` gives on both compilers now. `deleted_equality_probe.cpp`: what a plain deleted `operator==` would give (N4).
- **Checks** (`checks-summary.txt`; rows in `runs.tsv`), all at `fa86535` through `tools/timed`, with the pool read before and after: gcc-debug and clang-debug full suite, 544 of 544 each; gcc-asan 536 of 536; clang-asan 544 of 544; clang-measure 536 of 536; gcc-release and clang-release 544 of 544 each; lint (clang-tidy on every target, `basis_change_test.cpp` among them): clean; `format-check`: **fails** (B1); the convention trace gate: passes (exit 0; it lists items 6.1 to 6.4 as having no test, which this change does not touch); `tools/runlog check`: passes.
- **Process.** Test commits separate from the header commit and listed (W9); the two labelled failing commits name the test they leave failing (W2); every commit identifies agent work (W7); the choices are decision 0233 (C18); the plan exists with goal, criteria, evidence, and cost estimate (W3); the change of the guarded result of the two overflow tests is stated in the plan before the change (W9). No governance or agent citation in the added code or comments (S11): checked by grep of the added lines.

## What I did not check

- The author's mutation outputs, the gap run at the branch point, the 0030 specs' reruns, the 50-seed campaign, and the build-cost campaign: none is committed at `fa86535`. I did not run the seeds.
- The exact first error line and the one-error rule of the compile-fail tests beyond their passing on all seven configurations (their CMake rows are unchanged).
- The parts of `basis_change.hpp` the change does not touch, beyond reading them for how the reduced rows flow through them.
- The probe is a transcription of the header; agreement with it is evidence for the claim, not for the transcription (two transcriptions could share a misreading).

## Acceptance criteria (plan 0039 section 4) and definition of done (section 3)

| Criterion | Verdict | Evidence |
| --- | --- | --- |
| 1 `same_rows` | Met | The `static_assert`s at `basis_change_test.cpp:114`, `:163`, `:180` to `:187`, `:191` to `:207` cover every case named, with values from `from_rows`'s arguments; `!std::equality_comparable<core::basis_change<2>>` at `:187`. `same_rows_ignores_rows` killed at `:183`. `[[nodiscard]] constexpr`, and its `///` says it does not compare matrices. |
| 2 the reduction | Met | `basis_change_test.cpp:292` against hand values, rechecked by hand (divisors 2, 1, 7). `last_row_kept` and `divisor_two_kept` killed there. |
| 3 large rows accepted, same results | Met | Three pairs, `same_map` field by field (all five fields of `kernel_term`) on the full layout both ways, and the test case at `basis_change_test.cpp:619` both ways; `r300_scaled` also against x itself. `rows_not_reduced` and my four stage mutants killed. |
| 4 verdicts unchanged otherwise | Met | Full suite passes in seven configurations; the only changed expected inputs are those of criterion 5; the probe shows no previously fitting value grows. |
| 5 overflow still fails, pinned messages | Met | Both tests pass on all configurations with unchanged CMake rows; the probe confirms the stage of each, as written and reduced. |
| 6 N2 | Met | `.fits = fits` at `basis_change.hpp:444` and `:460`; I agree the mutant is equivalent (the loop returns on `!fits` at `:454` before either is reached). |
| 7 mutants | Not checkable | Outputs not committed; the specs will report problems on every killed mutant (N2). |
| 8 seeds | Not checkable | No `seeds/runs.tsv` at `fa86535`. |
| 9 checks of section 9 | **Not met** | `format-check` fails (B1); every other check passes. |
| DoD 4 tests first, labelled | Met | `be3f045`, `70bbcf3`, `dd67ead`, cherry-picked with `-x`. |
| DoD 6 build cost | Not checkable | No `records/performance/basis-change-rows-cost/` at `fa86535`. |

## Findings, most severe first

### B1. `format-check` fails on a changed test file (blocking, confirmed)

`test/core/basis_change_overflow_elimination_fail.cpp:22-24`: clang-format 23.1.4 with the repository's `.clang-format` joins `{1, 1, 1});` onto the line before it, so `--dry-run -Werror` reports `code should be clang-formatted` at 23:68. The `lint` workflow and the `format-check` target both exit 1 at `fa86535`, and S3 and W5 require the check to pass before the merge. The line was shortened in `be3f045` (the denominators went from three copies of 2147483647 to `{1, 1, 1}`) without reformatting.

Fix: run clang-format on the file (one line joined). It changes the layout of a test only, not core code, so in my reading it needs no delta review; the author's P9 run at the final tip shows it.

### N1. Plan 5.3 and the comment on `detail::reduced` claim more than holds (non-blocking, confirmed)

Plan section 5.3 ("each value that a check forms from M′ and Δ′ … is the value it forms at the same place from M and Δ divided by a positive integer") and the comment at `basis_change.hpp:360-365` ("Every value that the checks and the term generation form from the reduced rows is the value they form from the rows as written divided by a positive integer") are false for the two quotients that `typed_change` forms: `q / change_gcd(q, own[k])` at `:922` and `out.denominators[...] / own[k]` at `:931`. Counterexample (`quotient_probe.py`): e1 = (3 e'1 + 3 e'2) / 6 and e2 = (2 e'1 − 2 e'2) / 6, which reduce to (1, 1) / 2 and (1, −1) / 3. For the result blade e'1, `q / gcd(q, own)` is 1 then 1 as written and 1 then 2 reduced; `q_B / own` is 1 and 1 as written and 3 and 2 reduced. The reduced value is larger.

The conclusion still holds. No overflow guard applies to a division. Each quotient is at most its dividend, which shrinks (q′ divides q). The products that use the quotients scale by the integer L / L′, and my probe checks those products. So the safety claim ("a check that fits on the rows as written fits on the reduced rows") and the map claim stand.

Fix: restrict the statement to the products and sums that the overflow checks guard, and add that each quotient is at most a dividend that shrinks. This touches the plan and one comment.

### N2. The author's mutant specs declare killers that `tools/mutate` never reports (non-blocking, confirmed)

`records/mutations/0039-basis-change-rows/rows.toml` (every mutant expected killed) and `gaps.toml` declare `killers = ["core.basis_change"]`. These mutants kill by making `core_basis_change_test` fail to compile, and `tools/mutate` then names the killer by the failing assertion's location (`include/galoot/core/basis_change.hpp:740`, `test/core/basis_change_test.cpp:183`), not by the CTest name. So each run reports `declared killer core.basis_change did not kill it` and exits 1, though the verdict is "killed" as expected. I reproduced this for `rows_not_reduced` and `same_rows_ignores_rows` on gcc-debug (`out/author-rows-copy-gcc-debug.tsv`). The earlier 0030 specs declare a killer only once, for a compile-fail test, where the CTest name is what kills.

Fix: drop `killers` from these mutants, or name the test-source locations. This touches records only. Criterion 7's "each verdict as declared" holds only after this fix.

### N3. Evidence and records the plan requires are not yet in the branch (non-blocking at review, required before merge, confirmed)

At `fa86535` the branch lacks the mutation outputs, the branch-point gap run, the 0030 reruns, `seeds/runs.tsv` (criterion 8), the cost records (DoD 6, W10), the plan's outcome (section 13), and the regenerated rows of the misuse catalogue (section 1 item 6). The catalogue is stale for this header:
- `records/diagnostics/misuse-catalogue.md:88` names `basis_change.hpp:707` and `:965`, which are now `:752` and `:1010`.
- Line 24 names `:1050` and `:1051` for `divides_exactly`, which are now `:1095` and `:1096`.

All of these are records. Under decision 0215 they need no further independent review, but they must land before the merge.

### N4. `a == b` on two changes gives the compiler's generic error, which does not mention `same_rows` (non-blocking, usability, confirmed)

`equality_misuse_probe.cpp` gives GCC `no match for 'operator==' (operand types are 'const galoot::core::basis_change<2>' …)` and Clang `invalid operands to binary expression`. Neither names `same_rows`, and neither says the omission is deliberate. The checklist asks that a likely misuse give a message in the algebra's terms with the fix.

Decision 0233 item 1 considered only C++26's `= delete("…")`. A plain C++11 `bool operator==(const basis_change&) const = delete; // compare with same_rows(a, b)` is available in C++23 (`deleted_equality_probe.cpp`). It keeps `std::equality_comparable` false. Both compilers report a deleted function and quote that declaration line, comment included, in their note: GCC with "declared here", Clang with "candidate function has been explicitly deleted".

Tradeoff: one more public declaration, and the hint sits on a note line, not the first error line. Against that, a reader is pointed at the fix. This is a suggestion for the author or the owner. The current state matches the recorded decision.

### N5. A sentence the change did not touch now contradicts the text beside it (non-blocking, pre-existing, confirmed)

`basis_change.hpp:172`: "denominators[i] divides every entry of row i." In the mathematical sense this is false: `conformal_change`'s origin row is (−1, 1) over 2. The from_rows text that this change added (`:195-198`, "divide each row and its denominator by their greatest common divisor") makes the misreading costly, because if d_i divided every entry, every reduced denominator would be 1. The line is outside the unit's hunks, so it is reported, not fixed here (W9).

Suggested wording: "denominators[i] is the denominator of every entry of row i: P_ij = rows[i][j] / denominators[i]."

## Usability checklist (public interface: `same_rows` added, `operator==` removed)

Tasks from the examples only: (1) declare a change tag and map a multivector there and back; (2) write a change with `from_rows`, rows not reduced, and have it accepted; (3) check in a test that a change value holds the rows given (for example, a factory's output). For (2) and (3) the `///` comments of `from_rows` and `same_rows` were enough; I read the implementation only to review it.

- Names: `same_rows` uses `from_rows`'s word and reads as a predicate (`same_` as in `std::same_as`). The name says "rows" but the function also compares denominators; its `///` says so, and decision 0233 weighs the longer names. No clash with a standard meaning. Met.
- Construction and conversion: unchanged by this unit.
- Misuse and diagnostics: `==` is rejected and pinned by `!std::equality_comparable`, but with the compiler's generic message (N4). Distinct causes still give distinct messages; the overflow message is one message for every stage, as before.
- Semantics and cost: the reduction is stated in the `///` of `from_rows` and `basis_change_like` (C8). Its cost is about N² gcds per tag, negligible beside the checks' n³. `same_rows` is exact and named. The checklist item "exact comparisons are `==`" yields here to the owner's ruling (decision 0225 item 3).
- Examples and readability: the `same_rows` and `reduced` comments place the behavior where a maintainer looks; the reduction lives in one function and one call.
- Before release: removing `==` now is the point of the unit. Adding a matrix-equality `==` later would be an addition, not a break, so the option stays open. Users who wrote `==` before release are none (decision 0233).

## Files

- `2026-11-10-basis-change-rows-blind-review/scaling_probe.py` and `scaling_probe-out.txt`: the second transcription; claims (a) and (b) on random changes and the stage verdicts of the named inputs.
- `quotient_probe.py` and `quotient_probe-out.txt`: N1's counterexample.
- `reviewer-mutants.toml`, `reviewer-mutants-run.log`, `out/reviewer-mutants-*.tsv`: my mutants.
- `author-rows-copy.toml`, `author-rows-copy-run.log`, `out/author-rows-copy-gcc-debug.tsv`: N2.
- `equality_misuse_probe.cpp`, `deleted_equality_probe.cpp`, and their `-g++.txt` and `-clang++.txt` outputs: N4.
- `checks-summary.txt`: the check results; the run rows are in `docs/agent/records/performance/runs.tsv`.
