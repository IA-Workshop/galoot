# Delta review of WU13a after the blind review

- Reviewer: Claude (agent), blind-reviewer role, Opus 5.5, high effort. This is the same reviewer as `2026-11-07-wu13a-review.md` (decision 0094 item 3: a delta review of fixes made after review). Date: 2026-11-07.
- Range: `ff35cc9..01835b7` on `feature/core-transform`. I reviewed it in the detached worktree `.claude/worktrees/review-wu13a-delta` at `01835b7`, and judged it against findings 1 to 9 of the first review.
- Brief: the coordinator's. It gave the implementer's account of finding 5 (the quote-character rule and the new message text) and pointed to the commits listed for review. I checked each of those claims myself. The brief carried no verdict.
- Outcome: **approve.** The fixes resolve findings 1 to 9, or record them where they cannot be resolved. No blocking finding. The two new findings below are non-blocking. The owner's confirmation in finding 1 is still needed before the merge into `develop`, and is now stated in CONVENTIONS itself.

## What was checked

1. **Builds from empty trees** at `01835b7`, on a clean tree, each step through `tools/timed`, with the job pool read at 23 of 23 free before and after (`tools/jobpool ensure` reported it already running). Results:
   - gcc-debug: 365 of 365 tests pass
   - clang-debug: 367 of 367
   - clang-measure: 359 of 359

   These counts are unchanged, since the new test is a `TEST_CASE` inside `core.transform`. No build step failed.
2. **Mutants**, run with `mut_delta.py` on gcc-debug and clang-debug. Each kills by a compile error in a test, not a warning:

   | Mutant | Result, both compilers |
   | --- | --- |
   | `first_keeps_all`, rerun: the restricted first stage keeps every term | killed by the new test's `static_assert` (`transform_test.cpp:1343` and `:1346`) |
   | `middle_read_all`, fresh: every middle blade marked as read | killed by the same `static_assert` alone |
   | `middle_read_from_dropped`, fresh: middle blades marked as read by the dropped terms instead of the kept ones | killed by the kernel-count `static_assert` (`:1299`), and by `core.transform_unchecked_rigid_int64` and `_rational` |
   | `restriction_inverted`, fresh: the restriction keeps exactly the terms it should drop | killed by the kernel-count `static_assert` (`:1267`), and by the five `core.transform_unchecked_*` programs |

   `middle_read_all` and `first_keeps_all` die only through the new degenerate case, so that case is what pins the restriction.
3. **Finding 3, the new test.** I recomputed its hand derivation (transform_test.cpp:1331-1351):
   - x = e123 is the antiscalar, the unit of ⟇, so V ⟇ x = V: 4 terms, whose middle blades are V's.
   - The complements are right(e1) = e23, right(e2) = ±e13, right(e3) = e12, and right(e123) = 1. The scalar part of (a∧b)(c∧d) is (a·d)(b·c) − (a·c)(b·d).
   - With g12 = 1 and every other entry of g zero, only e12 e12 (= 1) and 1·1 have a scalar part.
   - So the kept terms read only the middle blades e3 and e123, and the restricted first stage keeps 2 of its 4 terms.

   My probe of the first review gave the same 2 and 4 for this pair.
4. **Finding 5, the message.**
   - The quote-character rule is real: assert.hpp:58 requires that a GALOOT_ASSERT message "contains no quote characters".
   - The new text, "transform: v does not act as a versor on x", appears in exactly three places: the header at transform.hpp:395, `transform_dropped` at test/CMakeLists.txt:1847, and the mutation anchor at mutate.py:110.
   - `transform_dropped` feeds both the constant-evaluation tests (line 1852) and the abort-message tests (line 1878).
   - The old text is gone from `include/`, `test/`, and `records/diagnostics/`. Decision 0179's table keeps it, superseded by its addendum, since recorded decisions are not rewritten.
   - Its source line is 74 columns.
   - The implementer's re-run of `exact_check_truncate_message` is in its record. I did not rerun it.
5. **The commits listed for review.**
   - `f57800a` adds 22 lines of test and changes nothing else. Its expected values are derived by hand in the reference comment and agree with my computation. It has an accepting and a rejecting size (S7).
   - `9fe1d3d` changes the header, the expected text, and the mutation anchor, three single-line edits, and its message lists them (W9).
   - `34cd05d` changes one comment.
   - `2842c0b` changes records only. The reformatted `asm.cpp` passes `clang-format --dry-run --Werror`.
6. **`counts.py`.** `records/performance/transform-cost/counts.py`, run with `python3 -I`, reproduces every tuple of the kernel-count test:
   - motor or flector on a point: first stage 20 (restricted and whole), kept second-stage terms 28, dropped terms 48 − 28 = 20, dropped blades 4
   - on a line: 36, 36, 36, 12, 2
   - on a plane: 28, 28, 20, 28, 4
   - on a motor or a flector: 48, 48, 48, 0, 0

   Its rule (A or B holds e4) is the test comment's rule, and it agrees with my own `counts.py`. The test comment still says "a separate script" and does not name the file. That is right: S11 forbids citing records in tests, and the record README names the script.
7. **CONVENTIONS.**
   - The diff `ff35cc9..01835b7` changes two lines only: the status line (line 3) and the 1.10 revision row (line 151). No ratified text changed.
   - The two passages they name as awaiting the owner are the right ones. The first is 4.4's outermorphism sentence with λ^(k−1) (line 60). The second is 2.1's two clauses after the definition of the antigrade involution, with "its relations Checked" (line 38).
   - I compared the committed 4.4 with plan 0021 O2 word by word. Three other changes remain. "where x̌ is the antigrade involution (2.1)" follows decision 0192 item 2(a). The other two, the labels, are new finding B.
8. **Plan section 11.**
   - Departure 1 now names both passages.
   - Departure 5 has the new message.
   - The new departure 16 states finding 2 accurately: the marker and golden-facts failures at `dc3fdd8`, and the two failing test commits in a row.
   - The resolution paragraph agrees with the diff for each finding.
9. **Decision addenda.**
   - 0176 names the second unratified passage.
   - 0177 cites Gallier (arXiv:0805.0311, the remark after Cor. 1.12) and Porteous (Prop. 13.58), and says honestly that it read the knowledge-base note, not the paper, and that Porteous is cited only as Gallier cites it (V9). It keeps n = 5 rank ≥ 2 as the implementer's own claim beyond the published statement.
   - 0178 keeps the restriction and pins it. The reasons are recorded, and its claim that `first_keeps_all` dies is true.
   - 0179 records that the checklist's "fix after a semicolon" is not met.
   - The addenda are appended, and no recorded text is rewritten.
10. **Trace.** The new rows map to the tests:
    - 4.3, line onto a unitized point: dual_test.cpp:1737
    - 4.4, λ times an outermorphism: transform_identities_test.cpp:318
    - 4.4, mirror times λ: transform_test.cpp:968

    The trace gate exits 0.
11. **The cherry-picked record commits.** `eb54693` and `ab134e8` have trees identical to my `53d7524` and `6acb6fa` (`git diff` is empty). Each carries its `(cherry picked from commit …)` line.
12. **Other gates.** `tools/runlog check` exits 0, and clang-format passes on `transform.hpp`, `transform_test.cpp`, and `asm.cpp`.

## Findings 1 to 9 of the first review

| Finding | State |
| --- | --- |
| 1, unratified text stated as ratified | Resolved in the text: CONVENTIONS names both passages. The owner's confirmation is still needed before the merge into `develop`. |
| 2, W2 at `dc3fdd8` and two failing test commits | Recorded as departure 16, with no history rewrite. |
| 3, the restriction unpinned | Resolved. |
| 4, the kernel count's script not committed | Resolved. |
| 5, the message gives no fix | Partly resolved. The message now states the cause in the algebra's terms; the fix part is recorded as not met. Judgment and new finding A below. |
| 6, the example's double result | Resolved ("about"). |
| 7, the published source | Resolved, cited through the knowledge-base note, which the addendum says plainly. |
| 8, trace rows | Resolved. |
| 9, `asm.cpp` formatting | Resolved; the assembly is unchanged according to the implementer, and I did not recompile it. |

**Judgment on finding 5.** I accept the deviation for WU13a. The message starts with the operation and names the cause in the algebra's terms. The fix ("give a versor") is close to implied by it, and the `///` comment says when the check fires. A fix part within 48 characters and without a quote character would be cramped. The checklist item stays recorded as not met in decision 0179, which is the right place for it.

## New findings

Both are non-blocking.

**A. The message's cause is too narrow when x's layout is not a union of whole grades.**
- **Where:** transform.hpp:395 and its comment at :473-478.
- **The case:** a genuine versor makes the check fire when x's layout is not a union of whole grades. One example is the `recorded` case of `transform_assert_fail.cpp`. There, in R(4,0,0), V = 1 + e12 + 2 e13 + 3 e14 has V ⟇ V̰ = 15𝟙, so it is a versor (n ≤ 4, so (H) holds). With x on e1 alone, it drops −2 e2 − 4 e3 − 6 e4. Decision 0177 5.2 gives another: a point stored on e1 and e4 only, turned about z.
- **Failure scenario:** a user with a unitized motor and a hand-made partial layout reads "v does not act as a versor on x" and suspects the motor. The comment defines "acts as a versor on x" so that the message is consistent with it. But it states the case only as a condition ("for a versor and an x of whole grades"), not as a cause of the abort.
- **Suggested fix:** a comment-only sentence such as "A versor can also leave the blades of x when x's layout holds only part of a grade, as a point on e1 and e4 alone does under a turn; give x every blade of its grades". Being comment only, it needs no further delta review (W5).

**B. Two evidence labels differ from O2 and are not named as awaiting the owner.**
- **Where:** CONVENTIONS.md:58 and :60.
- **The labels:** 4.4's label reads "the odd sign Checked for n = 3, 4, and 5" where O2 had "n = 3 and n = 4" (plan departure 2). 4.3's label reads "Checked; Confirmed, research note 0021" where O2 had "(Checked)" (decision 0176 item 1).
- **Why non-blocking:** both are evidence labels, which decision 0192 lets the step 1 checks set, and both are recorded. They are not conventions. They are named neither in the status line nor in the request to the owner.
- **Suggested fix:** mention them in the same request to the owner. No text change is needed.

**Nit.** Commit `9fe1d3d`, decision 0179's addendum, and plan departure 5 give the message as 43 characters. It is 42. No fix needed.

## Not checked

- `lint`, gcc-release, gcc-asan, clang-release, clang-asan, and gcc-measure were not rerun at `01835b7`. I did not rerun clang-tidy on the new test. The implementer's runs after the fixes are recorded in `01835b7`'s run rows, and I did not verify them independently.
- The implementer's re-run of `exact_check_truncate_message`.
- The byte-identical assembly after the reformatting of `asm.cpp`.
- Gallier's remark itself: I made no web request. Like the addendum, I rely on the knowledge-base note.

## Reproduction

The scripts are in `2026-11-07-wu13a-review/`:

- `python3 -I mut_delta.py <worktree> first_keeps_all middle_read_all middle_read_from_dropped restriction_inverted` reruns the mutants, from a worktree with built gcc-debug and clang-debug trees and the job pool exported. The results are in `mutants-delta.tsv`.

The run rows are in `runs.tsv`, revision `01835b78e254`, branch `detached`.
