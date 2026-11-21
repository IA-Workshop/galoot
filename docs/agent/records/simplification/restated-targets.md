# Plan 0048's expected totals, restated from the measurements

For the owner, before phase 1 of plan 0048 starts. Decision 0253 asks for this to go with merge M1 of plan 0047. Written 2026-11-21 by an agent (Claude Opus 5.5) as part of unit D0. It restates plan 0048 section 5.7. The plan's body is not edited; its outcome section points here.

The sources are the measurements of this directory, at `01dce7f`, whose `include/` equals the plan's `c95fee5` ([README.md](README.md)), and the knowledge-base reading ([knowledge-base-reading.md](knowledge-base-reading.md)). The owner's rulings applied are those of decision 0253 items 14 to 22: O1 (b), O2 C, O3 (a), O4 (a), O5 (a), O6 exclude, O7 one 1.1.0, O8 (a), O9 Fable for C0's design.

## What changed from the plan's figures

| Figure | Plan 0048 | Measured | Why it differs |
| --- | --- | --- | --- |
| Lines, code, comment, blank | 8,410; 4,561; 3,208; 641 | the same | |
| Comment lines by position (prologue, public `///`, `detail` `///`, `//`) | 702; 1,338; 443; 725 | the same | |
| Public names | 97 | 110 | The plan quotes a draft whose query missed the preset's namespace |
| Public declarations | 263 | 153 at namespace scope, plus 34 public members and 1 `std::formatter` specialization: 188 a user reads | The plan's and the research cycle's 263 and 293 list each template twice, along with instantiations (README, "Why the earlier figures differ") |
| Rejecting overloads | about 42 | 42 (37 at namespace scope, 5 constructors) | |
| `_layout` names | 23 | 25 (20 core, 5 preset) | The plan quotes the draft |
| Duplicated tokens | 25.6% | 25.6% by the plan's measure; 12.6% exact; 2.78% by lizard | Three measures of different things; each depends on its threshold (README, "Duplication") |
| Messages | 193 (32 in the mismatch family, 19 in the exactness family) | the same | |
| A1 | 13 lines | 7 code lines (`change_add`), or 12 if a test changes | The `blade_image` overload the plan calls dead has a test caller |
| B9 | 67 marker lines, about 90 comment lines | 23 marker lines, about 45 comment lines | The other 44 markers are for other checks |
| Option C of section 5.2 (E1, E2) | about 470 code lines | about 410: E1 about 235, E2 about 175 | Under C the 16 mismatch overloads stay, as declarations that now call one shared rejection, so their 96 lines mostly stay; the exactness checks measure 182 lines, of which about 22 stay as the one check (in `apply`, `transform`, and the change of basis) |
| Clang's step margin | not in the plan | 1.49 on six sampled programs (knowledge base); 1.29 and 1.11 on pinned plans of the suite | Point 4 of the knowledge-base reading |

The section 5.1 estimates (B1 to B8) were made by reading and were not prototyped. They stay as the plan gives them. Each is well inside the lines its unit opens (`baseline/candidates-check.txt`):

| Unit | Plan's estimate | Code lines the unit opens |
| --- | --- | --- |
| B5 | 25 | 43 |
| B1 | 60 | 96 |
| B6 | 40 | 172, of which 49 are the kept messages |
| B2 | 60 | 104 |
| B3 | 45 | 135 |
| B4 | 75 | 160 |
| B7 | 30 | 89 |
| B8 | 25 | 42 |

Nothing measured contradicts them. Nothing measured confirms them either: each unit states its measured saving when it closes, as the plan says.

## The totals, restated

Blank lines are scaled with code lines, as the plan's minimum and recommended rows imply. Its middle row, scaled that way, would read about 5,110, not 5,050. All figures are estimates, rounded.

| Scenario (as ruled) | Code lines | Comment lines | All lines | Change | Plan 0048 said |
| --- | --- | --- | --- | --- | --- |
| Now | 4,561 | 3,208 | 8,410 | | 8,410 |
| Phase 1 and 2 code (5.1 and option C), comments limited to deletions that move nothing | about 3,790 | at least about 1,620, and more by whatever the comment table marks "move" | about 5,940 | about -29% | (no such row) |
| The same with phase 4's moves, after the D3 ruling and the S11 amendment | about 3,790 | about 870 | about 5,190 | about -38% | 5,050, -40% (middle) |
| The same with C1, if its spike passes and the owner says go | about 3,640 | about 870 | about 5,020 | about -40% | 4,900, -42% (recommended) |

How each row is built:
- **Code.** 4,561, less about 365 for section 5.1 (the plan's 373 with A1 at 7), less about 410 for option C. C1 takes about 150 more, the plan's figure, which nothing here can check before the spike.
- **Comments before D3.** Decision 0253 lets phase 4 move a contract out of a header only after its receiving page in plan 0049 has passed review. Those pages are in D3, which is not authorized. S11 stands as ratified until its amendment, so no unit removes a contract from a header before then. What can go now is the plan's column (a): prologue examples, narration, repeats, process notes, the cast markers, and possibly the `detail` comments (open item 2). That leaves about 1,620 lines at best. The comment table (`records/docs/comment-moves.tsv`, D0's second part) will say how much of that column is deletion and how much is a move.
- **Comments after D3.** The plan's column (b), about 870, unchanged. B9 removes about 45 lines where the plan counted about 90. That is within the precision of these columns.

## The other measures, restated

Plan 0048's definition of done (item 8) and section 5.7 name these as the measures of a reader's load. The table restates each with its method fixed, so that the before-and-after table compares like with like.

| Measure | Now | Expected after the ruled scenario | Method |
| --- | --- | --- | --- |
| Public names, namespace scope | 110 | 110 | `inventory.py`, tier `ns`, distinct names |
| Public declarations, namespace scope | 153 | 153 | `inventory.py`, tier `ns`: templates once, redeclarations once, the partial specialization included |
| Public members, `std` specialization | 34, 1 | 34, 1 | `inventory.py`, tiers `member` and `std` |
| Overloads that only reject a call | 42 | 42 | `inventory.py`, column `role`. Option C keeps them; their bodies call one shared rejection |
| Message literals (`static_assert` and `GALOOT_ASSERT`) | 193 | about 145 (the 32 mismatch literals become 2, the 19 exactness literals become 1) | `records/performance/simplification-survey/msgs.py` |
| Term and table types | 7 | 5 (B1 removes `map_term` and `typed_map`) | By reading |
| Builders and runners | 12 (8 and 4) | about 4 (one builder, the two `transform` builders until C1, one runner if B8 passes) | By reading |
| Functions above cyclomatic complexity 10 (lizard) | 7 | about 4 (B6 merges three self-checks, E1 deletes `reject_product`) | `lizard -l cpp`, as the plan's survey |
| Highest cognitive complexity (clang-tidy, threshold 0) | 22 (`map_defects_of`), 21, 21, 20 (`reject_product`), 17 (`typed_defects_of`) | the two 21s (`unite`, `eliminate`) remain highest | knowledge-base exercise cpp-cr01's command |
| Duplicated tokens, the plan's type 2 (window 60) | 25.6% | lower, but not below about 9%: 35.6% of today's duplicated tokens lie outside every candidate site | `duplication_by_site.py`; a trend on one tree, not a measure of load |
| Duplicated tokens, exact (window 40) and lizard (70 tokens) | 12.6%, 2.78% | both lower | the same script; `clones_lizard.py` |
| Clang's step margin on the pinned plans (`core.plan_cost_model`) | 1.29 at the tightest of eight (`right_complement_full8`) | reported before and after by each unit marked `pinned` | the thresholds in `test/CMakeLists.txt` and the counts in `docs/guide/limits.md` |
| Clang's step margin on the six programs of the knowledge base | 1.49 (`product_test.cpp`) | reported before and after by each generator unit | exercise cpp-cr02's bisection |

The interface does not shrink in any scenario. The owner ruled no 2.0 batch (decision 0253 item 18), so by the knowledge base's own definition this plan simplifies the implementation and leaves the interface as it is. Its savings are in what a maintainer must read, verify and change, not in what a user must learn.

## Open items for the owner

Each item has options, their tradeoffs, and a recommendation. None blocks phase 0, and items 1 and 2 matter only from phase 1 on.

1. **Definition-of-done item 4 and the pinned plan costs.**
   - **The problem.** Item 4 says every test at M1 passes unmodified, except retired `internal` tests. `core.plan_cost_model` pins the exact step and operation count of eight plans and fails if a count moves either way. The guide publishes the same counts. A unit marked `pinned` in `candidates.tsv` (B5, B1, B6, and C1c) may move them.
   - **The options.**
     - (a) The thresholds are expected values. A unit that moves a count changes them in a separate commit listed for review (W9), with the guide's counts in the same unit (W12), and reports the margin before and after. Generator units stay possible. Every count change is visible and reviewed. The guide's page is rewritten a few times.
     - (b) Item 4 holds literally: a unit that moves a count is reworked or closed. That is the strictest guard on compile-time behavior. It probably closes B1 and B6, and perhaps B5, since a change of structure on that path rarely leaves the count exact.
     - (c) Exclude the plan-cost tests from item 4 without review of the new counts. That is not recommended: it hides what decision 0253 asks to be reported.
   - **Recommendation:** (a). It is what the test's own comment in `test/CMakeLists.txt` anticipates. It also turns decision 0253's margin report into a check that already runs.

2. **Whether S11 covers `detail` declarations.**
   - **The problem.** S11 says "each declaration in a public header has a `///` comment" stating its contract. Every header under `include/galoot/` is public. On that reading, the 443 `///` lines on `detail` entities are required, and the plan's D2 (443 down to about 110) departs from S11 as ratified. The plan treats `detail` comments as outside S11 (its section 1 item 5). The wording prepared under decision 0253 item 14 keeps "each declaration in a public header".
   - **The options.**
     - (a) Rule that S11's contract sentence applies to declarations outside `detail`, and add "outside a `detail` namespace" to the prepared wording. D2 can then run as a deletion. A maintainer keeps a why-comment where a `detail` entity's behavior is not obvious.
     - (b) S11 covers every declaration. D2 waits for an amendment, or drops, and about 330 comment lines stay.
   - **Recommendation:** (a). S11 frames the contract "in terms a user of the library can read", and the prepared wording points each contract to a guide page, which `detail` entities do not have. Until it is ruled, the comment units should leave D2 alone, the one choice every reading permits (W8).

3. **The `blade_image` overload of A1.**
   - **The problem.** It has one user, a test (`test/core/basis_change_identities_test.cpp:222` through `:75`).
   - **The options.**
     - (a) Change the test to pass `widened(change)`, in a commit apart from the deletion (W9), and delete the overload. That removes 5 code lines and one overload of a `detail` function. The test then exercises the same images through the call the header itself uses.
     - (b) Keep the overload. Nothing changes.
   - **Recommendation:** (a). A function whose only user is a test is the case C12 names. The test's check, minors over the rows' denominators, is unchanged by the call it makes.

4. **The counting method for definition-of-done item 8.**
   - **The options.**
     - (a) Adopt the method of `inventory.py` for the plan's before-and-after counts: namespace scope, each template once, members and the `std` specialization as separate tiers. Plan 0047's V2 manifest should agree with it on scope, so the two can be compared.
     - (b) Keep the research cycle's matcher, which is comparable with its notes but counts each template twice.
   - **Recommendation:** (a). It counts what a user declares, and it agrees with the manifest's scope.

These need no ruling, only the coordinator's attention when briefing the units:
- The object-code comparison of plan section 6.4 should pass `-fno-ipa-icf` on GCC.
- E2's diagnostic sample should include the exactness rejections, read for `detail::` names.
- C0's pass rule should include step counts.

All three are in the knowledge-base reading, points 5, 8 and 9.
