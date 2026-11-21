# The comment table (plan 0048, unit D0, second part)

Records of the second part of unit D0 of [plan 0048](../../plans/0048-aggressive-simplification.md), made on 2026-11-21 by an agent (Claude Opus 5.5, hci-expert role). It is the hand-over that [plan 0049](../../plans/0049-pedagogical-user-documentation.md) section 7 asks for, and that [decision 0253](../../decisions/0253-post-1-0-rulings-and-sequencing.md) ("Hand-overs") assigns to D0.

The table is a plan. No header was edited.

## Files

| File | Holds |
| --- | --- |
| [comment-moves.tsv](comment-moves.tsv) | One row for each comment block, or part of a block, of every header under `include/galoot/`: 887 rows |
| [check_comment_moves.py](check_comment_moves.py) | Checks that the rows cover every comment line exactly once at the named revision, then prints the sums below |

## The revision

Every line range is at `01dce7f` on `main`. Its `include/` tree is identical to `c95fee5`, the revision plan 0048 surveyed, and to this branch's tip before this work (`a4c18f1`). So the ranges here, in the plan, and in [the first part's records](../simplification/README.md) agree.

## How the table was made

- Every comment block of all 18 headers was read in place, with the code it sits on.
  - A block is a run of `//` lines, or of `///` lines, as the line census counts them.
  - Every header was read this way, including the eleven the plan's survey had measured only by script.
- Each verdict was checked against the text that already exists elsewhere:
  - the other headers;
  - the guide at that revision: `docs/guide/core.md`, `rga3.md`, `limits.md`, and the first page, `docs/guide/README.md`.
  - A delete row is marked as a repeat only where that text states the same facts.
- A block that holds several kinds of text is split into several rows, at line boundaries.
  - Where a sentence starts or ends in the middle of a line, the line goes with the row that holds most of it, and the note says so.
  - So a row's range is exact, while its descriptor says which sentences it means.
  - A shared line is rewritten, never cut: see [Sentences that cross a row boundary](#sentences-that-cross-a-row-boundary).
- The 73 trailing comments on code lines are covered as well.
  - Most are namespace closers, include-guard ends, and IWYU pragmas.
  - Three are kept for the reader of a compiler error: `mv.hpp:619` and `:636`, and `basis_change.hpp:177`.
  - The 70 `/*name*/` markers on unnamed parameters are code, not comments, and have no row.

## Columns

| Column | Holds |
| --- | --- |
| `header` | The header's path |
| `lines` | A range `a-b`, a single line, or a comma list (used only for trailing comments) |
| `position` | `prologue` (before the first `#include`), `detail` (inside a `detail` namespace), `public` (elsewhere), or `trailing`. These are the line census's rules, so the sums compare with plan 0048 section 3.1 |
| `kind` | What the text is (below) |
| `class` | `keep`, `summary`, `move`, or `delete` (below) |
| `descriptor` | One line on what the text says |
| `target` | For a move: the page that receives it. For a summary: the page that receives everything but the kept sentence, or the comment that already says the rest. For a delete of a repeat: the place that already says it, a page at the revision or a header range |
| `heading` | The heading on the target page |
| `waits_for` | What has to happen before a comment unit may act on the row (below) |
| `note` | Why, where the verdict needs a reason. "core 'X'" means `docs/guide/core.md` under the heading X, and likewise "rga3", "limits", and "the guide's first page" |

### Kinds

| Kind | Meaning |
| --- | --- |
| file prologue | Text before the first `#include` about the header as a whole |
| public contract | What a public declaration requires, guarantees, or preserves |
| internal contract | The same for a `detail` entity. This kind is not in the brief's list; it is added because a `detail` contract is neither public nor narration |
| reason | Why: intent, a workaround, the reason for a constant or a design |
| equation or convention | The formula or convention the code applies at that place |
| narration | Text that restates the code beside it, or the process that produced it |
| example | Code written in a comment |
| marker | A `NOLINT`, `clang-format`, or `IWYU` directive and the reason the static-check rule requires beside it, a namespace closer, or an include-guard end |
| pointer | A sentence that sends the reader to a guide page and heading. Also not in the brief's list; it is added because plan 0049's pointer test reads these sentences |

### Classes

| Class | Meaning |
| --- | --- |
| keep | Stays as it is: a reason, an invariant, the equation applied, a pointer, a marker, a public declaration's comment that is already one sentence, or a precondition that is undefined behavior when broken in an unchecked build |
| summary | Stays as one sentence, and the rest moves to `target` or already lives there |
| move | Its content belongs in user documentation, at `target` under `heading` |
| delete | Narration, or a repeat of text that `target` already holds |

The owner's ruling, decision 0253 item 14, is applied as follows:
- A public declaration keeps one sentence, as a summary row or a keep row, and the rest moves to its reference entry.
- A precondition that is undefined behavior when broken in an unchecked build stays on the declaration in full, as a keep row whose note says so.
- A reason stays: S11's first paragraph asks for it, and the prepared wording of the amendment moves only the contract.

### What a row waits for

| Value | Meaning |
| --- | --- |
| none | Nothing. A keep row, or a delete that moves nothing |
| B9 | A cast marker and its reason. They go when plan 0048's unit B9 has confirmed every report is false and added the exclusion to `.clang-tidy`, as decision 0253 item 19 rules |
| the detail ruling | Removes or shortens a `///` comment on a `detail` entity. That waits for the owner's answer to open item 2 of [restated-targets.md](../simplification/restated-targets.md): whether S11's contract sentence covers `detail` |
| the S11 amendment | Deletes public contract text that the guide already holds. Decision 0253 says no unit removes a contract from a header until the amendment is applied |
| D1, D3 | A move whose receiving page is in that milestone of plan 0049. Decision 0253 lets phase 4 move text only after its receiving page has passed review |
| D3 and the S11 amendment | Both of the above: public contract moving to a reference page |
| D3 and the detail ruling | A `detail` `///` comment moving to a concepts page |

### Target pages

Plan 0049 section 4.1 names the directories, the six how-to pages, and the single pages. For the twelve reference pages and the nine concepts pages it names only the subjects, so their file names here are proposals:

- **Reference:** `reference/algebras.md`, `layouts.md`, `multivectors.md`, `products.md`, `involutions-complements-duals.md`, `transform-and-compose.md`, `change-of-basis.md`, `printing.md`, `macros.md`, `blade-helpers.md`, `rga3.md`, and `index.md`.
- **Concepts:** `concepts/blades-and-grades.md`, `metric-and-signature.md`, `products.md`, `complements-and-duals.md`, `versors-and-transforms.md`, `layouts-and-result-types.md`, `exactness-and-rounding.md`, `compile-time-cost.md`, and `how-generation-works.md`.

The headings are proposals too:
- A reference heading is the entry's name, such as `geometric_product`.
- A concepts, conventions, or diagnostics heading names the topic or quotes the message.

U3 publishes the real heading list (plan 0049 section 7 item 5). The table's targets are then renamed in one commit. The checker's list of layout pages changes with them.

## How to check it

From the repository root:

```sh
python3 -I docs/agent/records/docs/check_comment_moves.py
```

It reads the headers through `git show 01dce7f:<path>`, not from the working tree, so it means the same thing after the headers change. `--revision` names another revision.

It checks:
- that each full-line comment line and each trailing comment is covered by exactly one row, and no row covers anything else;
- that each position matches the census rule;
- that kinds, classes, and the waiting conditions take known values;
- that each move or summary names a page of the layout and a heading;
- that each delete of anything but narration or a marker names where the text already is. A page named that way must exist at the revision with that heading. A header range named that way must hold comment lines whose own rows are not deleted.

It ran on this branch with Python 3.14, in under a second, and printed no problem, both when the table was first written and after the corrections below. No build or test ran for this work. For the corrections, one probe of a few lines was compiled with GCC 16.2.1 and `-fsanitize=undefined` in the session's scratch directory, outside the job pool, to confirm two cases of undefined behavior. Nothing was timed, so the timing lock was not taken.

## Sums by header and class

Full-line comment lines, from the checker:

| Header | Keep | Summary | Move | Delete | All |
| --- | --- | --- | --- | --- | --- |
| `core.hpp` | 3 | 0 | 0 | 0 | 3 |
| `core/assert.hpp` | 37 | 16 | 27 | 6 | 86 |
| `core/basis_change.hpp` | 212 | 18 | 112 | 96 | 438 |
| `core/bits.hpp` | 50 | 31 | 16 | 1 | 98 |
| `core/blade_product.hpp` | 97 | 0 | 16 | 19 | 132 |
| `core/blade_set.hpp` | 106 | 49 | 23 | 7 | 185 |
| `core/dual.hpp` | 88 | 17 | 88 | 72 | 265 |
| `core/format.hpp` | 31 | 11 | 30 | 18 | 90 |
| `core/linear.hpp` | 79 | 25 | 20 | 70 | 194 |
| `core/metric.hpp` | 61 | 8 | 41 | 8 | 118 |
| `core/mv.hpp` | 246 | 97 | 60 | 59 | 462 |
| `core/plan.hpp` | 270 | 0 | 31 | 65 | 366 |
| `core/product.hpp` | 76 | 5 | 25 | 45 | 151 |
| `core/transform.hpp` | 127 | 10 | 102 | 106 | 345 |
| `core/unary.hpp` | 21 | 8 | 7 | 32 | 68 |
| `presets/rga3.hpp` | 5 | 0 | 0 | 0 | 5 |
| `presets/rga3/motions.hpp` | 32 | 0 | 58 | 15 | 105 |
| `presets/rga3/objects.hpp` | 40 | 0 | 7 | 50 | 97 |
| **All** | **1,581** | **295** | **663** | **669** | **3,208** |

The 73 trailing comments are all kept.

By kind:

| Kind | Keep | Summary | Move | Delete | All |
| --- | --- | --- | --- | --- | --- |
| public contract | 250 | 266 | 485 | 97 | 1,098 |
| internal contract | 495 | 0 | 12 | 37 | 544 |
| equation or convention | 364 | 0 | 54 | 73 | 491 |
| file prologue | 86 | 0 | 80 | 195 | 361 |
| reason | 298 | 29 | 21 | 10 | 358 |
| example | 0 | 0 | 2 | 167 | 169 |
| marker | 51 | 0 | 0 | 45 | 96 |
| narration | 0 | 0 | 9 | 45 | 54 |
| pointer | 37 | 0 | 0 | 0 | 37 |

Narration is rare: 54 lines in all. Contracts, reasons, and equations together are 2,491 of the 3,208 lines, 78%. That agrees with the knowledge-base census, which found about three quarters of the comment lines to be contracts or reasons by a keyword heuristic.

By what the rows wait for:

| Waits for | Lines | Of which delete |
| --- | --- | --- |
| none (kept) | 1,581 | |
| none (delete or summary now) | 453 | 447 |
| B9 | 45 | 45 |
| the detail ruling | 97 | 80 |
| D1 | 26 | |
| D3 | 146 | |
| D3 and the detail ruling | 15 | |
| the S11 amendment | 97 | 97 |
| D3 and the S11 amendment | 748 | |

## Against plan 0048 and the restated targets

Plan 0048 section 5.4 estimated its four comment candidates from position alone. Here are the same four classes, by the verdicts read block by block. "Kept" counts keep rows, plus two lines for each summary row (one sentence), plus every row whose condition has not yet been met. It counts no pointer lines.

| Candidate | Now | Plan, S11 as ratified (a) | Plan, one sentence and a pointer (b) | Table, S11 as ratified, after D3 | Table, after the amendment |
| --- | --- | --- | --- | --- | --- |
| D1 prologues | 702 | 90 | 90 | 176 | 165 |
| D2 `///` on `detail` | 443 | 110 | 110 | 344 | 344 |
| D3 `//` elsewhere | 725 | 330 | 330 | 634 | 634 |
| D4 `///` on public declarations | 1,338 | 1,090 | 340 | 1,282 | 538 |
| **Total** | **3,208** | **about 1,620** | **about 870** | **about 2,440** | **about 1,680** |

The amended S11 also asks each public declaration's comment to name its guide page and heading. Each pointer on a line of its own adds lines:
- **A pointer on every declaration:** about 190 lines. There are 157 public `///` blocks at namespace scope, 37 of them on rejecting overloads, and 36 on members.
- **A pointer per reference entry:** about 120 lines, with members and rejecting overloads covered by the entry of the declaration they belong to.

So the realistic end state is about **1,800 to 1,870 comment lines**, a fall of 42% to 44%. The plan estimated about 870, a fall of 73%.

Stage by stage, as each condition is met (kept lines, no pointers):

| Stage | Comment lines | Change |
| --- | --- | --- |
| Now | 3,208 | |
| Deletions that wait for nothing, and B9 | 2,712 | -15% |
| The detail ruling, if it is (a) | 2,621 | -18% |
| Plan 0049's D1 and D3 pages received, S11 as ratified | 2,436 | -24% |
| The S11 amendment applied | 1,681 | -48% |
| The same with pointers | about 1,800 to 1,870 | -42% to -44% |

The first part's [restated targets](../simplification/restated-targets.md) gave the comment column as follows:
- **Before D3:** "at least about 1,620, and more by whatever the comment table marks move". The table says about 2,620: the deletions that wait for nothing, B9, and the detail ruling.
- **After D3 and the amendment:** about 870. The table says about 1,800.

So the restated totals' comment column should read about 2,620 and about 1,800. With the code figures unchanged and blank lines scaled as there, that makes the restated "all lines" about 6,940 and about 6,120, not 5,940 and 5,190.

### Why the estimate and the reading differ

| Class | Gap | Where it comes from |
| --- | --- | --- |
| D3, `//` | about 300 lines | The plan expected to drop narration and repeats and to move derivations. Read block by block, the `//` lines inside functions are mostly reasons (a GCC 16 miscompilation, why a first term is assigned, why a check is a variable template), equations applied where they are applied, and internal contracts of the generator. Only 54 delete and 33 move. The 42 marker lines that stay are suppressions and formatting directives, each suppression with the reason the static-check rule requires beside it |
| D2, `///` on `detail` | 234 lines | Most `detail` comments describe the generator: `generate_plan`'s count-then-fill protocol, the capacity proofs, the self-checks and the conditions they test, the rounding counts, the elimination. Plan 0048's own rule 4 keeps a `detail` comment whose behavior, bound, or cost is not obvious, and these are the least obvious code in the library. What goes is "Requires: nothing", "an aggregate, equal when every member is", and member comments that repeat their class |
| D4, public `///` | about 200 lines, before pointers | Four things stay that the estimate of 340 lines did not allow for: the one sentence of each of 42 rejecting overloads; one-line comments on 36 members; summary sentences that are themselves the formula, such as the sign rules of `bits.hpp` and the coefficient orders of the rga3 layouts; and eleven preconditions on public declarations kept in full under the ruling |
| D1, prologues | 75 lines | Prologues keep the conventions they apply and the reasons for them: the mask convention in `bits.hpp`; why a blade is a wedge and not a product, with the integer-polynomial invariant, in `blade_product.hpp`; why the complement reverses canonical order, in `plan.hpp`; why the odd form of `transform` has its sign. The examples, the restated guide text, and the cost paragraphs go: 430 of 702 lines are deleted |

A tighter pass is possible:
- **Cut every kept `detail` `///` block to two lines:** about 177 lines more.
- **Exclude `performance-enum-size` in `.clang-tidy`:** 14 more. That needs a ruling like O6.

It would bring the end state to about 1,600 lines. It would remove the generator's internal contracts, the text that explains the items plan 0048 section 5.5 protects. This record does not recommend it.

## What a reader of a header would lose

These are the losses after every row is applied, with the amendment in force:

1. **The contract at the declaration.**
   - Requires, guarantees, result layout, cost, and the rejected misuses leave the header for the reference page. An editor's hover shows one sentence and a pointer.
   - Whoever reads only the headers, with the headers copied into a project or the guide out of reach, can no longer learn from them alone that `truncate` never checks a `double`, or which scalar types `x / s` checks for a zero divisor.
   - That is the trade the owner ruled. The undefined-behavior preconditions stay, so nothing that can corrupt a program is hidden from the header.
2. **The tutorials.** Every prologue example goes. Each was checked against a compiled example in `core.md` or `rga3.md` that covers the same calls. The header no longer shows a first-time reader how to declare a tag, but the guide does, and it is compiled.
3. **The one-glance tables.**
   - `dual.hpp`'s table of what each operation computes goes, since each function states its own formula.
   - `basis_change.hpp`'s list of which operations a change preserves goes to a concepts page. `change_basis` keeps one sentence that names the pitfall (the bulk and the complements are not preserved) and points there.
4. **The causes of a rounding-check abort.**
   - The four causes of "transform: v is not a versor on x within rounding" and their remedies move to the diagnostics page. Plan 0049 names this block for that page.
   - A maintainer debugging an abort at the check reads a pointer instead of the causes.
5. **The proofs that join the equations.**
   - The derivation of the blade-product recursion moves to the generation concepts page, though each equation stays where it is applied.
   - The derivation of the rounding bound moves to the exactness page. Its statement, its published source, and the reason the margin is 1 stay.
6. **The generic-code caveat on each rejecting overload.** "A requires-expression that asks is told yes" is said once, at the products' operand rule, and moves to the reference. A reader of one rejecting overload learns it from there.
7. **Lookup caveats at the declaration.** The `std::ranges` clash of `reverse` and `transform`, and `part` against a variable named `part`, leave the header. Both are in the guide already. The `part` caveat moves with its entry.

A maintainer keeps the following:
- every reason;
- every invariant the code relies on;
- every equation at the place it is applied;
- every published source;
- every pointer to the limits page, in the fixed form plan 0049's pointer test reads;
- the comments that keep a compiler's error readable: the trailing comments on the conversion constructors and on the deleted `==`, and the rule that a message must start on the line of its `GALOOT_ASSERT`.

## Preconditions kept in full

Undefined behavior when broken in an unchecked build, kept under the ruling. Plan 0048 named two (the first two rows). Reading found fourteen more, five of them on `detail` helpers. Three of these were added after the sample check (marked "added"), and two more were already kept with that reason in their notes but missing from this list (marked "listed"):

| Place | Precondition |
| --- | --- |
| `blade_product.hpp:49-57` | A coefficient overflow at run time with checking off |
| `linear.hpp:392-396` | An integer division by zero |
| `linear.hpp:407-411` | `min() / -1` for a signed type at least as wide as `int` (not checked in any build) |
| `bits.hpp:115-116`, `:130-131`, `:143-144` | `complement_mask`, `right_complement_sign`, `left_complement_sign` with n of 0 or above 32: `low_bits` then shifts by the width of the mask |
| `bits.hpp:80-81` (added) | `antigrade` with n of 2^31 or more: `static_cast<int>(n) - grade(a)` overflows `int` |
| `bits.hpp:167-168` (added) | `antireverse_sign` with an antigrade above 46341: m(m − 1) overflows `int` |
| `bits.hpp:45-47`, `:58-60` | The same requirement on the `detail` helpers that the shift sits in |
| `plan.hpp:303-304` (added) | `collect_result`'s `out` must have a place for every distinct blade, or it is written past its end |
| `plan.hpp:664-665` (listed) | `unary_term` with an n out of range reaches `complement_mask`'s shifts |
| `blade_product.hpp:82-83` (listed) | `multiply_checked`'s operands within range, since negating the most negative value is undefined |
| `metric.hpp:126` | `is_null(i)` with i at or beyond N, which reads past the array |
| `mv.hpp:1281-1283` | The reference that `get` returns from a temporary must not outlive it (in every build) |
| `assert.hpp:30-32` | Every translation unit must use one `GALOOT_FORCE_INLINE` mode, since mixing them breaks the one-definition rule |

The two cases of `bits.hpp` were confirmed by a probe built with GCC 16.2.1, `-fsanitize=undefined` and `GALOOT_CHECKING` 0, which reports a signed overflow for `antigrade(1, 2^31)` and for `antireverse_sign(0, 50000)`. The `detail` generator contracts of `plan.hpp`, `blade_product.hpp` and `basis_change.hpp` that hold other `Requires:` lines are kept in full as well, so none of them is at risk; this list names only the ones whose violation is undefined behavior.

Two more were weighed and are not undefined behavior: `canonical_name` stops at e9 and does not overrun, and `rotation`'s zero direction gives infinities and NaNs, which floating arithmetic defines.

## Hard calls

1. **`blade_product.hpp:14-29`, the recursion's derivation: move.** Each equation it derives is restated at its function, so the header keeps what it applies. What leaves is the proof that e_A = e_a ∧ e_A' = e_a e_A' − e_a ⌋ e_A' gives the recursion. A maintainer changing the recursion would want it in place. A concepts page that the header points to was judged enough.
2. **`transform.hpp:464-475`, the rounding bound's derivation: move.** It is the rounding-bound text the plan names, minus what stays: the statement, Higham's lemmas, and the margin's reason (`:457-463`, `:476-480`).
3. **`transform.hpp:590-610` and `:730-749`, the causes of a rounding abort: both move to one diagnostics heading.** They say partly the same thing in two places. The receiving author merges them. The alternative was to keep the internal copy at the check, for the maintainer, and move only the public one. That keeps a fact in two places, which plan 0048's rule 3 forbids.
4. **`plan.hpp:27-33`, why the complement reverses canonical order: keep.** Plan 0049 sends design reasons to the concepts pages. This one is the invariant that `run_term` relies on and that the order check tests, and no other comment proves it.
5. **`plan.hpp`, `mv.hpp`, `basis_change.hpp`, the generator's `detail` contracts: mostly keep.** They are where plan 0048 expected the largest `detail` saving (D2), and where reading found the least narration. The verdict rests on S11's first paragraph and the plan's own rule 4. It is the largest single cause of the gap above.
6. **The seven "An int, so that a compiler prints its values and not a byte": keep.** The plan lists them as a repeat. Each one sits beside a `performance-enum-size` suppression, and the static-check rule asks for the reason beside the suppression. They go only if the check is excluded in `.clang-tidy` (open item 3 below).
7. **The "message on the line of its GALOOT_ASSERT" rule: one home, at `assert.hpp:126-129`, kept as a reason.** Its five repeats point there. It is also part of `GALOOT_ASSERT`'s contract, but a maintainer adding an assertion needs it at the macro.
8. **`blade_set.hpp:21-25`, canonical order in the prologue: keep.** `canonical_less` states the order again in its one sentence. The prologue alone warns that canonical order is not mask order, and so says twice what plan 0048's rule 3 would say once.
9. **`linear.hpp:396-406`, which integer types the zero-divisor check reads: move.** The undefined behavior stays (`:392-396`). Which types are checked goes to the reference, so a reader with a big-integer type learns there, not here, that its division by zero is its own affair.
10. **`dual.hpp:25-42`, the table of definitions: delete as a repeat of core 'Bulk, weight, and their duals'.** Every function keeps its own formula. The table is the one place a reader sees the eight side by side. It survives in the guide, and plan 0049's conventions chapter receives it from there.
11. **`rotation`'s reason for returning a plain `Motor`: keep (`motions.hpp:62-66`).** It explains a departure from the rule that a degenerate input returns a failure value. That is the kind of reason a reviewer asks for, in the place the reviewer reads.

## For the units that use the table

- **Repeats cited in `core.md`, `rga3.md`, and the first page.** 45 delete rows name `docs/guide/core.md`, `docs/guide/rga3.md`, or `docs/guide/README.md` as the place that already says the text. Plan 0049's U3 splits and deletes the first two and moves the first page's nine facts to `howto/first-things.md`. By the time phase 4 runs, each such row must name the page and heading the text went to. U3's list of where each old heading went is the input to that rename.
- **Cross-references inside kept text.**
  - `dual.hpp:368` sends the reader to the header's own prologue, which moves. Its row moves with it.
  - `change_basis`'s pointer to the prologue becomes a pointer to the concepts heading (`basis_change.hpp:1113-1115`).
  - Several pointer rows say "reword to stand alone" where the sentence they finish goes.
- **Plan 0049's U9 and U10.** The reference pages receive 482 lines of public contract, by page in the table's `target` column. The diagnostics page receives 54 lines, almost all about the two `transform` messages.
- **Illustrations for the reference.** Two prologue deletes carry a concrete case that no guide page has: `transform.hpp:24-29` (a motor of rigid(2) or conformal(3) has blades of odd grade) and `dual.hpp:60-70` (the bulk of a rigid(3) line holds e12, e13, e23, with the moment on e31 negated on e13). Both follow from rules the guide states, so neither is a lost fact. Their notes name the reference entries for `transform` and `bulk` as the place for them; U9's author takes them from the header at `01dce7f`.
- **The pointer test.** The kept pointer sentences to `limits.md` use the form `docs/guide/limits.md, under 'Heading'`. One, `mv.hpp:889-891`, names two headings in another wording and needs rewriting into that form. Several others finish a sentence that goes, and their notes say to reword them to stand alone.

### Sentences that cross a row boundary

A row's range is whole lines, and a sentence may start on the last line of one row and end on the first line of the next. Where the two rows wait for different conditions, or are classed differently, the unit that acts on one of them first must not delete or move the shared line alone. It rewrites that line, and the neighbor's part of the sentence, so that what stays is whole sentences; the neighbor row's own verdict then applies to the rewritten text. Some notes name such lines (`unary.hpp:24`, `transform.hpp:589`, `blade_set.hpp:577`, `plan.hpp:627`), but the rule holds at every boundary, named or not. Boundaries of this kind are common, since most comments wrap mid-sentence, and they are found by reading the last line of the row before.

## Corrections after the sample check

An independent sample check of this table (the review record `docs/agent/records/reviews/2026-11-21-0048-d0-sample-check.md`, on branch `review/simplification-survey` at `cfad408`) found six errors. Each was checked against the header and corrected:

| Finding | Correction |
| --- | --- |
| 1. `plan.hpp:302-307`, `collect_result`: a precondition that is undefined behavior when unchecked was classed delete | Split: `302-304` keep, `305-307` delete as before. The first split was one line early and deleted the end of the sentence; see [Corrections after the precondition check](#corrections-after-the-precondition-check) |
| 2. `transform.hpp:492-495`: the floor of the padded tolerance, about K λ W, was deleted as a repeat of limits.md, which lacks the formula | Keep, beside the reason for the padding |
| 3. `bits.hpp:132`: a public contract deleted with nothing to wait for, and "returns -1 or +1" said nowhere else | Move to the `right_complement_sign` reference entry, after D3 and the S11 amendment |
| 4. `plan.hpp:63-66` and `:624-627` cited a home with only the Clang half of the reason | Notes name the second home, the comment above `core.plan_evaluated_once` in `test/CMakeLists.txt` |
| 5. Row boundaries inside sentences | The rule above, and notes on the three rows |
| 6. Two illustrations only in deleted prologues | Notes name the reference entries that receive them |

Re-reading every row of the same header and kind as each confirmed error found two more preconditions of the same class as finding 1, on `antigrade` and `antireverse_sign` in `bits.hpp`, and two notes in `transform.hpp` that named the wrong heading. All are corrected, and the sums above are the checker's output after the corrections. Rows after the split rows are numbered five higher than in the record of the check, at most.

## Corrections after the precondition check

The checker's precondition check (see [How to check it](#how-to-check-it)) found one row of the table wrong:

| Header | Finding | Correction |
| --- | --- | --- |
| `plan.hpp` | `collect_result`'s precondition is lines 303-304, but the split after the sample check put the keep row at `302-303`, the blank line before it and the first half of the sentence, and deleted `304`, "GALOOT_ASSERT; plan.size() places always suffice", with the guarantee | Keep `302-304`, delete `305-307`. One line moves from delete to keep, under the detail ruling's column: the sums, the stages and the list above include it |

## Open items

Each item has its options, their tradeoffs, and a recommendation.

1. **Where the pointers go, under the prepared S11 wording.**
   - **The problem.** The prepared wording (decision 0253 item 14) says each declaration in a public header names its guide page and heading. Read strictly, that covers 36 members and 37 rejecting overloads as well as the 120 or so declarations a reference entry describes.
   - **The options.**
     - (a) A pointer on every declaration: about 190 lines. Uniform and easy to check, but a member's or a rejecting overload's pointer repeats its owner's.
     - (b) A pointer on each declaration that has a reference entry, with members and rejecting overloads covered by it: about 120 lines. Plan 0049's pointer test then needs a rule for what counts as covered.
   - **Recommendation:** (b). Put it to the owner with the confirmation of the wording, which decision 0253 already reserves.
2. **The detail ruling** (open item 2 of [restated-targets.md](../simplification/restated-targets.md)). The table marks 97 lines as waiting for it alone, and 15 more as waiting for it and D3. The recommendation there stands: S11's contract sentence applies outside `detail`.
3. **The seven `performance-enum-size` suppressions.**
   - **The options.**
     - (a) Keep each with its reason: 14 lines. Nothing changes.
     - (b) Exclude the check in `.clang-tidy` with the reason once, as decision 0253 item 19 does for the cast check. The check then no longer reports a real oversized enumeration in a header. The core has seven enumerations, all reviewed.
   - **Recommendation:** (a). The saving is small, and the reason is local to each enumeration.
4. **The headings.**
   - **The problem.** Every target heading here is a proposal, and so are the file names of the reference and concepts pages. 45 repeats cite pages that U3 removes or splits.
   - **The options.**
     - (a) When U3 publishes its heading list and its map of where each old heading went, one commit renames the table's targets. The checker's page list changes in the same commit.
     - (b) Leave the targets as they are and let each comment unit translate them.
   - **Recommendation:** (a). It gives the pointer test and the received-rows check one set of names.
5. **The comment targets of plan 0048.**
   - **The problem.** Section 5.4 and the restated targets expect about 870 comment lines at the end. The verdicts here give about 1,800.
   - **The options.**
     - (a) Accept the table and restate the targets with M1, as decision 0253 asks for the other figures.
     - (b) Ask for the tighter pass described above, about 1,600 lines. It cuts the generator's internal contracts.
     - (c) Have a second reader, the cpp-expert as plan 0048 section 7 pairs it with D0, reread the keep verdicts on `plan.hpp`, `mv.hpp`, and `basis_change.hpp` before phase 4.
   - **Recommendation:** (a), with (c) folded into the review of the first comment unit that opens those headers. One independent sample check has read the table (see [Corrections after the sample check](#corrections-after-the-sample-check)); it read 106 rows at random and every delete row, not the keep verdicts this item is about.
