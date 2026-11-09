# Delta review of WU12 after the review rulings (plan 0030)

- Reviewer: Claude (agent, Opus 5.5, high effort), blind-reviewer role (W5), in a configuration that shares no working context with the implementer of the rulings or with the earlier blind reviewer and hci reader.
- Date: 2026-11-09.
- Reviewed: `feature/core-basis-change` from `37f1729`, the revision the blind review saw, to `82825e9`, with the weight on `30c2275`, the only commit that changes `include/`. The diff for code is `git diff 37f1729..82825e9 -- include test CMakeLists.txt`; the merge `110c02d` brought only decision 0223 and agent-cost rows from `develop`.
- Record branch: `records/wu12-delta-review`, branched from `82825e9`, worktree `.claude/worktrees/wu12-delta-review`. Supporting files are in `2026-11-09-wu12-delta-review/`: `mutants.toml` and `out/`, `probes/` (sources and compiler outputs), `scripts/` (a Python replica of the checks and its outputs), and `runs.tsv` (the probe runs).
- The brief carried no verdict. It carried the implementer's counts (180 messages, 158 pinned; 1.4 gains two facts and 1.8 four). I recounted them.

## Departures, skipped checks, and ambiguities (C20)

1. **The role file says the reviewer returns the record and the merging agent commits it.** The brief asks me to commit it on a new branch, so I did, and I changed nothing on the reviewed branch.
2. **The branch moved during the review.** `8c717f1` (decision 0224, the implementer's record of the choices in `30c2275`, and one added line in decision 0221) landed after `82825e9`. It is outside the range. I read it, since it bears on the S7 question, but I did not review it as a change.
3. **Skipped: the full P9 matrix for the merge into `develop`.** It has not run on this range. The B1 fix will change the revision anyway.
4. **Not rerun (P10): lint and `format-check`.** Both have run rows at `0fa98ac`, with the same code as `82825e9` and only records dirty (`runs.tsv`, 16:43 UTC). I infer that the dirty files were records from the README of the mutation directory. I did not configure a lint tree.
5. **Not rerun: the implementer's mutation specs** (`change.toml`, `new_cases.toml`, `review_fixes.toml`). I read their rewritten anchors and committed rows and ran my own spec of eight mutants instead.
6. **Not reproduced: the claim that `tests.messages_pinned` failed at `fcc25a7` and `9bfa204`.** Reproducing it needs build trees configured at those revisions. The claim is against the implementer's own interest, so I take it as stated (inferred).
7. **Only lightly checked: the test commits of the first resolution** (`0c5f0e0`, `cc923ac`, `a83eec3`, `9cd86f0`). They pass on both presets in my run. I read their diffs and their mutant rows and recomputed the pairing test's inner product by hand. I wrote no mutants of my own for them.
8. **Not reviewed: the content of the cherry-picked records** (the Sage cross-check and the hci reading). For those I checked only the patch-ids, the `-x` lines, and that each touches records only.
9. **Probes against mutants were compiled on scratch copies of `include/`**, in the session's scratchpad. They were never compiled in the worktree while `tools/mutate` had a mutant applied. The worktree's own builds and lint ran only on clean trees.

## What I checked and how

Every build, test, and probe went through `tools/timed` with the shared pool. The pool read "running: 23 of 23 slots free" before and after the heavy runs, and I never started or stopped it. The build and test rows are in `records/performance/runs.tsv` on this branch, and the probe rows are in `2026-11-09-wu12-delta-review/runs.tsv`. I fetched nothing from the web, sent no email, and contacted no one. The one exact computation (`scripts/exact_inverse.py`) used sympy from the math-tools Python environment.

| Check | How | Result |
| --- | --- | --- |
| Build, fresh trees | configure, then `core_basis_change_test` and `core_basis_change_identities_test`, gcc-debug and clang-debug, at `82825e9` | pass |
| Selection | `ctest -L '^core\.basis_change'` (31 tests, every compile-fail test of the component among them) and `-L '^tests'` (6), both presets | all pass |
| Trace gate | `python3 -I tools/test_references.py trace …`, from the root | exit 0; only 6.1 to 6.4 have no test; 1.4 names `basis_change_test.cpp:305`, and 1.8 names twelve places |
| `conventions_check.py` | `python3 -I test/reference/conventions_check.py` | "all convention checks passed (seed 20261031)" |
| Catalogue | `tools/catalogue report include/galoot gcc-debug=… clang-debug=… --check misuse-catalogue.md` | 180 messages, 22 unpinned in each listing, so 158 pinned; the table equals the committed `scan-output.md` row for row; one stale count (S1) |
| Run log | `tools/runlog check` | pass, with my rows |
| `30c2275`, line by line | read the whole header, with the old helpers beside the new ones; a Python replica of `eliminate`, `keeps_inner_products`, `inverts`, and `inverse_rows` with the noting arithmetic (`scripts/noting.py`, 200,000 random and structured inputs) | correct; three of its four overflow branches are untested (B1) |
| Overflow, by hand | four probes, one per overflow branch: the committed test (isometry), `probes/elim_overflow_identity.cpp`, `probes/inverts_overflow.cpp`, `probes/inverse_rows_overflow.cpp`, plus `probes/elim_overflow.cpp` | each gives exactly one error on GCC 16.2.1 and Clang 23.1.4, the overflow message on the first error line |
| Overload set of S6 | `probes/overloads.cpp`, seven cases, both compilers | valid calls compile; values get the new message; types keep the old catch-alls (below) |
| Mutants | my own spec, 8 mutants of `check_change`, the elimination, the count guard, and the catch-alls, on gcc-debug and clang-debug | 4 killed and 4 survived on each (B1) |
| Comment-only edits | `git diff -U0 37f1729 30c2275`, hunk by hunk | S4, S5, S7, N3, and N7 touch comment lines only |
| CONVENTIONS 1.17 | compared line by line with note 0026 section E (script inline in this session) | 1.4, 1.8, and the section 8 row are identical to the draft; the revision row adds the ratification clause; the file lost its final newline (S2) |
| Process | patch-ids of the cherry-picks, file sets of each commit, W2 labels, the reflog of the amend, S11 search of added lines | below |

### `check_change` and the noting arithmetic, line by line

- **`multiply_noting` and `add_noting` (lines 254 to 273)** test the negations of the old conditions. Old: `magnitude_x == 0 || magnitude_y <= max / magnitude_x`. New: `magnitude_x != 0 && magnitude_y > max / magnitude_x`. The add test is `x <= max - y` or `x >= -max - y`, negated the same way. So they decide exactly what the old checked helpers decided, and they keep every result in −max to max.
- **No negation of `INT64_MIN` can occur.** `entry_type` is `int` (`metric.hpp:29`), so a row, denominator, or Gram entry widened to `std::int64_t` is at least −2³¹. Every later value comes out of a noting operation, which keeps it within −max to max, or out of `sign * x` on such a value.
- **The same sites as before are checked.** In the tag checks, every product and sum that went through `change_multiply` or `change_add` at `37f1729` now goes through the noting forms:
  - the two products and the sum in `eliminate_entries`;
  - the product and the sum in `inverts`;
  - the product in `inverse_rows`;
  - the four in `keeps_inner_products`, the right side's two products among them.
  `change_gcd`, the division by `previous`, and `sign * …` were unchecked before and stay so, and each is safe in range. The term generation keeps `change_multiply` and `change_add` with `GALOOT_ASSERT`, as ruled.
- **No evaluation can fail inside `check_change`.** After an overflow, the step completes on the zeros the noting operations return. Its divisions are by `previous`, the pivot of a step that fit, which is not zero. Then `eliminate` returns at line 416. The other functions finish their loops on in-range values. So `static constexpr result checked = check_change(…)` always evaluates, and the five `static_assert`s read one exclusive `ChangeFault`, so at most one fails.
- **No overflowing change is accepted.** `ChangeFault::none` is reached only if `e.fits`, `isometry.holds` (which includes `fits`), `inverse_holds.holds` (likewise), and `inverse.fits` all hold. The map generation runs only on `checked.fault == ChangeFault::none` (line 938). The set of accepted changes is the same as at `37f1729`: the old code accepted a change only when all of these computations completed without an assertion, and the new code runs the same computations.
- **S1:** the target's count is asserted only inside `if constexpr (C::source::value.generator_count == n)` (line 677), and the checks only inside the target's guard. Both conditions depend on `C`, so a discarded `static_assert` is not instantiated.
- **One change of behavior** is not mentioned in the commit message. The old isometry and inverse checks returned at their first mismatch, so a non-isometry whose mismatch came before an overflowing entry got the isometry message. Now any overflow in the check wins, since `holds = fits && holds` (lines 457, 552), so that change gets the overflow message. Both reject the change, and the new order no longer depends on loop order. Note N1.

### The S6 catch-alls and overload resolution

`probes/overloads.cpp`, each case on both compilers:

| Case | Result |
| --- | --- |
| `change_basis<cga3_to_r410>(p)`, `change_basis_back<…>(q)`, `change_basis<C>(t)` in a template, and `change_basis<cga3_to_r410, P::layout, double>(p)` | compile, no diagnostic |
| `change_basis<cga3_to_r410{}>(p)`, `change_basis<3>(p)`, `change_basis<cga3::value>(p)` | one error: the new value message |
| `change_basis<int>(p)`, a dependent `generic<core::basis_change<5>>(p)` | one error: the existing "not a change of basis" message, so a type argument still picks the type catch-all |
| `change_basis<conformal_change<3>()>(5)`, a value and no multivector | the compilers' own "no matching function" |

A type argument cannot match an `auto` non-type parameter, so the new templates drop out of resolution for every call with a type. The mutant `value_catch_all_back_takes_types` shows the converse: with `class Value` the value is no longer caught, and the declared killer fails.

**STYLE S7.** The `auto Value` parameter has no named concept, which departs from "every template parameter is constrained by a named concept". On substance I find the departure sound:

- Any value given there is the same mistake.
- A trivially true concept would add a name and no meaning (C12, S16).
- A concept narrowed to `basis_change` values would lose the message for `change_basis<cga3::value>(p)`, a metric given where the tag belongs, which the catch-all now catches (case 3 above).

But STYLE is ratified governance. A departure from one of its provisions is the owner's decision (constitution, "Provisions are requirements"). Decision 0223 item 1 accepted the hci reading's prototype, which was already `auto V`, but the S7 departure was not put to the owner. Decision 0224, which states it, is the implementer's record. It is open owner item 1.

## Findings, most severe first

### B1. Blocking (test-only fix): three of the four overflow branches of `check_change` have no test, and two mutants that accept an overflowing change survive

- **Where:** `basis_change.hpp:416` to `418` (the elimination's early exit), `:638` to `:641`, `:648` to `:652`, and `:654` to `:657`. The only overflow test is `core.basis_change_overflow` (`test/CMakeLists.txt:2358`), a one-generator change whose overflow comes in the isometry check, where n = 1 makes the elimination and the inverse trivial.
- **Mutants** (`mutants.toml`, `out/mutants-<preset>.tsv`). Each survived on gcc-debug and on clang-debug:

  | Mutant | Edit | Effect |
  | --- | --- | --- |
  | `elimination_overflow_as_dependent` | line 639 always `dependent` | an overflow in the elimination gives the independence message |
  | `elimination_overflow_not_stopping` | line 416's return removed | the overflow is lost, since the later returns say `.fits = true` (lines 406, 422); `probes/elim_overflow_identity.cpp`, the identity of R(3, 0, 0) written as m e′_i over m, is then reported as **linearly dependent** on both compilers |
  | `inverts_overflow_as_self_check` | line 650 always `inverse` | the overflow gives the internal self-check message, which `misuse-catalogue.md` says no user input reaches |
  | `inverse_rows_overflow_ignored` | lines 654 to 657 removed | the change is **accepted** with inverse rows that hold 0 where a product overflowed |

- **Concrete failure for the last mutant:** `probes/inverse_rows_overflow_run.cpp` uses rows {{0, 2²⁰, 0}, {0, −2²⁴, 46341}, {2, −378, 0}} over {1, 2³⁰, 2³¹ − 1} between two copies of R(0, 0, 3), where every invertible change keeps the inner products. Under the mutant it compiles and runs. `change_basis_back` sends e′1 to 0.00018024444580078125 e1, while the exact image is 189/1048576 e1 + (2³¹ − 1)/2 e3 (`scripts/exact_inverse-out.txt`). So the e3 term of about 1.07 × 10⁹ is silently lost. Unmutated, the same source gives one error with the overflow message on both compilers.
- **The code is right today:** each of the four probes gives exactly one error, the overflow message first, on both compilers (`probes/*-g++.txt`, `*-clang++.txt`). The finding is that nothing pins it.
- **Why it blocks:**
  - The ruling's purpose (decision 0223 item 3) and the guarantee that no overflowing change is accepted rest on four separate branches.
  - Before `30c2275` one assertion covered every site. Now each stage carries its own flag, and a slip in any of them gives a wrong message or a silently wrong change.
  - This is the core, which C19 holds to the strongest evidence, and the blind review blocked on the same kind of gap (F1).
- **Fix, tests only:** three compile-fail tests through `basis_change_rejects`, each with the overflow message, `ONE_ERROR`, and the internal names forbidden on the first error line, taken from `probes/elim_overflow_identity.cpp`, `probes/inverts_overflow.cpp`, and `probes/inverse_rows_overflow.cpp`.
- **The probes kill all four mutants,** compiled against scratch copies of the header (`probes/*-under-<mutant>-*.txt`): `elim_overflow_identity` kills both elimination mutants (the independence message), `inverts_overflow` kills the third (the self-check message), and `inverse_rows_overflow` kills the fourth (it compiles).
- **What closes it:** no core code changes, so no further delta review is needed. Rerunning `mutants.toml` with the new tests closes the finding.

### S1. Should-fix (records): the committed scan output's catalogue count is stale

`records/diagnostics/catalogue/scan-output.md` says "Test names in misuse-catalogue.md: 214". The committed `misuse-catalogue.md` gives 217 when I rerun the tool. The report ran at `c634077`, before `0fa98ac` edited the catalogue's rows, and the same commit committed both. The message table is identical. Rerun `tools/catalogue report` and commit its output.

### S2. Should-fix (records, trivial): `CONVENTIONS.md` lost its final newline in `c634077`

`git diff` shows "No newline at end of file" after the 1.17 revision row, and `37f1729` ended with a newline. This is not a change of text, so I think it needs no ratification. The record should say so if it is fixed.

### S3. Should-fix (records): plan 0030 does not record the work after the review

The plan was last changed in `1a9242d`. The findings' resolutions, the rulings of decision 0223, the commits `320c0d4` to `82825e9`, this delta review, and the pending P9 matrix are recorded in the review records but not in the plan. W3 asks a session to end by writing what is done, what is in progress, and the next step into the plan.

### N1. Note: a non-isometry with an overflowing entry now gets the overflow message

This is the behavior change described in the line-by-line section. Both messages reject the change, the advice "use rows with smaller entries" is true, and once the user follows it the next build gives the isometry message. The commit message and decision 0224 call an overflow "a fault of its own, since a check that overflowed decides nothing". That is not quite true when an exact entry has already mismatched, but the simpler rule is defensible. No change is proposed.

### N2. Note: the elimination's later returns hard-code `.fits = true`

At line 406 `fits` is necessarily true, because line 416 has already returned on false. At line 422 that holds only because of the early exit. Writing `.fits = fits` at both would make each return state its own truth, and `elimination_overflow_not_stopping` would then report the overflow instead of losing it. This is a core change, so it is for a later unit, not this merge.

### N3. Note (pre-existing, outside the delta, W9): large unreduced rows can be rejected though the change's results fit

- `probes/elim_overflow.cpp` has rows {{m, m}, {−m, m}} over {m, m}, m = 2³¹ − 1, that is P = [[1, 1], [−1, 1]] from 2I to R(2, 0, 0). It is rejected with the overflow message.
- The overflow is in a left-block entry of an already-eliminated column (2m² × m). That value is discarded: the replica shows the pivot 2m² and the right block adj M exact (`scripts/not_stopping-out.txt`).
- The identity written as m/m is rejected the same way.
- `from_rows`'s comment says "A row need not be reduced … the library reduces what it generates", but the checks run on the rows as written.
- The message's advice is correct, and the behavior is the same at `37f1729`. Open owner item 2.

### N4. Note: the S6 confirmation was done in the implementer's own session

Decision 0223 item 1 asks for a cpp-expert to confirm the overload set. The implementer did it "in the cpp-expert role" (hci reading Resolution, S6). This review's probes and mutants are the independent check, and they agree.

### N5. Note: a value of the tag type gets the value message

`change_basis<cga3_to_r410{}>(p)` gets "the template argument is a value, not a change tag; put it in a tag, as in struct cga3_to_r410 { … }". The fix is to drop the `{}`, which the text does not say. This is rare, and it is left for the M5 message pass if anyone wants it.

### N6. Note (process): two labelled failing commits did not name every test they left failing

- `fcc25a7` and `9bfa204` name their failing compile-fail tests, but not `tests.messages_pinned`, which W2 asks them to name.
- The implementer found this and said so in `488a4ab` and `30c2275`, which name it and the commit that caused it.
- Rewriting the shared commits is not allowed and not needed. This record lists it.

### N7. Note (process): the amended commit `488a4ab` is acceptable

- **What the reflog shows:** `3e8fbc7` (11:18:07) was amended to `463b023` (11:18:38) and then to `488a4ab` (11:18:47), before the next commit. All three have the same tree, `697cd48`, and the same parent. No branch contains the two earlier ones.
- **What changed:** the first message claimed that the tests labelled `tests` passed except for the labelled compile-fail tests, which was false. The amends replaced the claim with the disclosure in N6.
- **Judgment:** the commit was a local, unshared one. W7 forbids rewriting shared history only. The amend made the record more accurate. I see no fault.

### N8. Note (records): the mutation README's count

The "After the review rulings" section says eight mutants of `change.toml` were re-anchored, then names nine (`columns_for_rows` to `elimination_division_dropped`) and one of `review_fixes.toml`. `82825e9`'s message says nine. "Eight" should read "nine".

## Process

- **Cherry-picks:**
  - `2d9ee04` (from `3030199`) and `23857a5` (from `c8f7395`) carry `(cherry picked from commit …)`.
  - Each has the same stable patch-id as its source (`c7a80a8…`, `93fe2aa…`).
  - Each touches records only, and each source is on its `records/` branch.
  - `23857a5` names the agent only through its `Co-Authored-By` trailer, as its source did.
- **W9:** the test commits `320c0d4`, `fcc25a7`, `9bfa204`, and `488a4ab` touch only `test/`. `30c2275` touches only `include/`. The test commits precede the header commit and are listed in its message.
- **W2:** the labelled failing commits name their tests, except as in N6.
- **W7:** every commit says what and why, and names the agent and model (`Co-Authored-By: Claude Opus 5.5`, and "Agent work: …").
- **S11:** a search of every line the range adds under `include/` and `test/` for decisions, plans, reviews, provisions, agents, or the workbench finds none. The comments state their conventions in place.
- **Comment-only edits:**
  - S4: the example, lines 21 to 33.
  - S5: Δ at lines 467, 468, 515, 516, and 577.
  - S7: lines 153 and 154.
  - N3: lines 177 to 179.
  - N7: lines 103 to 105.
  The `git diff -U0` hunks for these change comment lines only. The `///` of `inverse_change` and of `basis_change_like` also change, to describe the new routes truthfully.
- **CONVENTIONS 1.17:** 1.4 and 1.8 equal note 0026 section E's draft character for character, and so does the "Conformal duals" row. The 1.17 revision row is the draft followed by the ratification clause, as earlier rows have it. The trace table moves the blade-map row to 1.8, adds two facts to 1.4 and four to 1.8, and the gate finds each in a test's reference comment.
- **Tests that fail for the faults they claim:**
  - `core.basis_change_both_counts` kills `target_count_unguarded` (with `core.basis_change_source_count`).
  - `core.basis_change_overflow` kills `isometry_overflow_holds`, its declared killer.
  - `core.change_basis_value_as_tag` and `core.change_basis_back_value_as_tag` kill `value_catch_all_removed` and `value_catch_all_back_takes_types`.
  - The reworded literals are pinned by nine tests, which `fcc25a7` left failing until `30c2275`.
  - The header example's test (`488a4ab`) checks the hand values (−½, ½) for the origin's image and (1, 0) back on e4 e5, which follow from CONVENTIONS 1.4.

## Usability checklist, for the changed public interface

- [x] Names unchanged (decision 0222 item 1); S7's sentence says which word is the function.
- [x] Each new misuse in the catalogue has a compile-fail test on both compilers that requires the literal message.
- [x] One error, message first: both counts, the value catch-alls, and every overflow in the tag checks, the last checked by hand on all four routes. Only the isometry route is pinned (B1).
- [x] Messages start with the operation, state the mistake, and give a fix that is safe for the likely cause (S2's rewording).
- [x] Distinct causes give distinct messages; under the B1 mutants they would not.
- [x] `==`'s comment now says what it compares (N3 of the blind review; renaming stays the owner's before release).
- [x] The example compiles and is the idiom to copy.

**Before release:** the catch-alls break no user. The S7 departure, if reversed later, changes no user's code.

## Outcome

**Block, pending B1.** B1 is a test-only change: three compile-fail tests from this record's probes. With them, and with `mutants.toml` rerun to show all eight killed, the verdict becomes **approve with non-blocking findings**. No further delta review of core code is needed, since nothing in `include/` changes. S1 to S3 are records-only fixes that can ride in the same push. N1 to N8 do not block.

## Open items for the owner

1. **STYLE S7 and the unconstrained `auto Value` of the value catch-alls** (decision 0224 item 5).
   - Options: (a) accept the departure, with a sentence in S7 allowing a catch-all for a value given where a type belongs to take an unconstrained `auto`; (b) constrain `Value` by a named concept on `basis_change` values; (c) a named concept that every value satisfies.
   - Tradeoff: (a) is a governance amendment, and the code stays as it is. (b) complies, but loses the message for other values, such as `change_basis<cga3::value>(p)`. (c) complies in form only and adds a name with no meaning (C12).
   - Recommended: (a).
2. **Large unreduced rows rejected as overflow** (N3, pre-existing).
   - Options: (a) leave it, since the message's advice is right; (b) in a later unit, reduce each row by the gcd of its entries and its denominator before the checks; (c) in a later unit, skip the left-block entries of already-eliminated columns, which are discarded.
   - Tradeoff: (b) is a few lines and removes the common case, such as m e′_i over m. (c) removes spurious overflows that remain after reduction, but changes the elimination that the identity tests verify. Both are core changes with their own review. (a) costs nothing, but contradicts the spirit of `from_rows`'s "a row need not be reduced".
   - Recommended: (b) as a backlog item before release, not in this merge.
3. **Still open from before:** the meaning of `==` on `basis_change` before release (decision 0223 item 4), and N2 and N5 of the blind review. This review does not change them.
