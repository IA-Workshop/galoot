# The comment table (plan 0048, unit D0, second part)

Records of the second part of unit D0 of [plan 0048](../../plans/0048-aggressive-simplification.md), made on 2026-11-21 by an agent (Claude Opus 5.5, hci-expert role). It is the hand-over that [plan 0049](../../plans/0049-pedagogical-user-documentation.md) section 7 asks for, and that [decision 0253](../../decisions/0253-post-1-0-rulings-and-sequencing.md) ("Hand-overs") assigns to D0.

The table is a plan. No header was edited.

## Files

| File | Holds |
| --- | --- |
| [comment-moves.tsv](comment-moves.tsv) | One row for each comment block, or part of a block, of every header under `include/galoot/`: 887 rows |
| [check_comment_moves.py](check_comment_moves.py) | Checks that the rows cover every comment line exactly once at the named revision and that every checked precondition is accounted for, then prints the sums below and the boundary report. It holds its own copy of the list of preconditions kept in full |
| [boundary-report.tsv](boundary-report.tsv) | The checker's boundary report: each place where a sentence runs from one row into a neighbor with another class or another condition |
| [readme_figures.py](readme_figures.py) | Prints every other figure this README states from the table: the stages, the candidates against plan 0048, the pointer estimates, the restated totals, and the counts the prose quotes. `--compare REV` lists the rows whose class or condition differ from the table at a revision |

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
| `position` | `prologue` (before the first `#include`), `detail` (inside a `detail` namespace), `public` (elsewhere), or `trailing`. These are the line census's rules, so the sums compare with plan 0048 section 3.1. The rule takes a namespace alias for a namespace, so `format.hpp:215`, in a public member function after the alias `namespace detail = galoot::core::detail;` on line 211, is labeled `detail`; it is the only comment line affected, and its row's note says so |
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

The owner's ruling, decision 0253 item 14, with the S11 wording the owner ratified in decision 0258 section 2, is applied as follows:
- A public declaration keeps one sentence, as a summary row or a keep row, and the rest moves to its reference entry.
- A precondition that is undefined behavior when broken in an unchecked build stays on the declaration in full, as a keep row whose note says so. "Undefined behavior" is the C++ standard's, except for the one case the owner ruled otherwise (decision 0258 item 4.4; see [The precondition check](#the-precondition-check)).
- A reason stays: S11's first paragraph asks for it, and the ratified wording moves only the contract.
- The contract sentence does not apply inside a `detail` namespace (decision 0258 item 4.2), so a `///` comment on a `detail` entity may be deleted or shortened now; plan 0048's rule 4 still keeps one whose behavior, bound, or cost is not obvious.

### What a row waits for

| Value | Meaning |
| --- | --- |
| none | Nothing. A keep row, or a delete that moves nothing. Since the owner ruled that S11's contract sentence does not apply inside a `detail` namespace (decision 0258 item 4.2), this includes the deletions and summaries of `///` comments on `detail` entities that waited for that ruling |
| B9 | A cast marker and its reason. They go when plan 0048's unit B9 has confirmed every report is false and added the exclusion to `.clang-tidy`, as decision 0253 item 19 rules |
| the S11 amendment | Deletes contract text, in the sense below, that the guide already holds. No unit removes a contract from a header until the amendment is applied, and decision 0258 section 3 applies it only once its no-drift checks run as tests (see [Before a contract leaves a header](#before-a-contract-leaves-a-header)) |
| D1, D3 | A move whose receiving page is in that milestone of plan 0049. Decision 0253 lets phase 4 move text only after its receiving page has passed review. Two deletes wait for D3 as well, the prologue illustrations that a reference entry must hold first (decision 0255 part A item 10). None of these rows holds contract text in the sense below |
| D3 and the S11 amendment | Both of the above: contract text, in the sense below, moving to a guide page, almost always a reference page |

The checker accepts only these values. Two values are gone: "the detail ruling", whose 36 rows now wait for nothing, and "D3 and the detail ruling", whose three rows now wait for D3 and the S11 amendment (`assert.hpp:77-83`, on a macro in no `detail` namespace, and `mv.hpp:880-882` and `:895-899`, a requirement and a guarantee that every product inherits from `detail::apply`).

Contract text, for these conditions, is text that states what a public declaration requires, guarantees, or preserves (the sense of decision 0258 section 2), wherever it sits, when the headers would hold no other statement of it once the row is acted on. Decision 0258 section 3 says that no unit removes a contract from a header until the no-drift checks exist, and does not limit that to the `///` comment on a declaration. So such text waits for the amendment in three places besides a declaration's own comment:
- **A prologue or `detail` comment that is the only statement of a contract in the headers**, such as the rule for classes derived from `mv` (`linear.hpp:51-53`, `unary.hpp:25-27`, `product.hpp:35-43`), or what every product requires of and guarantees for its scalar (`mv.hpp:880-882`, `:895-899`).
- **Text that a declaration's comment sends the reader to.** `change_basis` (`basis_change.hpp:1113-1115`), `bulk` (`dual.hpp:368`), and `transform` (`transform.hpp:752-754`) each say that another comment of the header lists part of what they do. That text waits for the same condition as the comment that points to it, so a reader never follows a pointer to text that is gone.
- **The lookup caveats of a prologue**, since the same caveat on `reverse`'s declaration (`unary.hpp:63-69`) is public contract.

A prologue sentence that a declaration's own comment also states is a repeat, not the only statement: the sums' operand rule (`linear.hpp:36-43`) is said again on each rejecting overload, so the contract stays in the header on the declarations until the amendment, and the prologue's copy goes with its own condition. The headers are read together: `presets/rga3/objects.hpp:30-33` repeats `transform.hpp:59-65`, which waits for the amendment, so it may go first. Two kinds of text are not contracts in this sense: worked cases of a definition that the declaration's comment keeps (the bulk's in other metrics, `dual.hpp:44-59`, is held back only because `bulk`'s comment points to it) and explanations of why a stated guarantee holds (`plan.hpp:47-56`, the proof that a product's result layout holds no blade it cannot reach).

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
- that each delete of anything but narration or a marker names where the text already is. A page named that way must exist at the revision with that heading. A header range named that way must hold comment lines whose own rows are not deleted;
- that every checked precondition is accounted for, by the method below.

It then prints the boundary report, which never fails; `--boundaries FILE` also writes it to a file, and [boundary-report.tsv](boundary-report.tsv) is that file.

The figures outside the checker's sums come from

```sh
python3 -I docs/agent/records/docs/readme_figures.py
```

which reads the same table and the headers at the same revision, and the inventory of the first part's records for the declarations under the `///` blocks.

### The precondition check

The check finds the text that states a checked precondition, then asks the three tables under [Preconditions kept in full](#preconditions-kept-in-full) where each line of it stands.

1. **Assertions.** Every line whose code (comments and literals removed) calls `GALOOT_ASSERT(`, except the macro's own `#define`, is an assertion.
2. **The enclosing declaration.** A brace scanner reads each header's code and labels each `{` by the text since the last `;`, `{`, or `}`: a namespace, a class (the last `struct`, `class`, `union`, or `enum` with no `(` after it), or other. The assertion belongs to the outermost "other" brace around it, which is the body of a function or the initializer of a variable, whatever lambdas or blocks lie between.
3. **Its comment.** From that brace's line the check walks up over the declaration's code lines (template heads, `requires` clauses, attributes, a `NOLINT` line between two of them) and stops at a blank line, a preprocessor line, a comment line, or a code line that ends with `;`, `{`, or `}`. The run of full-line comment lines right above is the declaration's comment.
4. **Its Requires clause.** In that comment, a clause runs from a line whose text starts with `Requires` to the line before the next labeled clause (`Guarantees:`, `Cost:`, and the like), an empty comment line, or the end of the comment.
5. **Clauses checked elsewhere.** Every Requires clause in any comment that names `GALOOT_ASSERT` is taken too, which brings in the callers whose precondition a callee checks, such as `right_complement_sign` or `generate_map`.
6. **Assertions with no clause.** An assertion whose declaration has no Requires clause must be in the third table, which names where its condition is stated; those places are then taken as if they were clauses.

Every line taken must be in the first table (undefined behavior when broken in an unchecked build), whose rows must all be keep, or in the second (checked, but not undefined behavior), which leaves the row to the ordinary classes. Each place in the first table must be a comment line with a keep row, whether an assertion leads to it or not.

The checker holds the first table's places a second time, in its list `KEPT_IN_FULL`. Each place there must be a comment line with a keep row, and the first table must name exactly the same places. So a comment unit that demotes a kept precondition and deletes its line from this README, or moves the line to the second table, fails the check until it also edits the checker's list, and the change to the check is what its reviewer sees. Many of the places have no assertion behind them, or their clause leads only to the second table, so without that list this README's line would be their only guard. A comment unit does not edit the list; a change to it is a change to the check, made with the table, as the corrections below were. A line in both tables, a place in the second that no assertion leads to, or a place in the third that is not such an assertion, is an error.

The rule behind the first table: a precondition is undefined behavior when broken if a call that breaks it, at run time with `GALOOT_CHECKING` 0, reaches an operation the standard leaves undefined. The owner ruled one exception (decision 0258 item 4.4): the unchecked floating division of `x / s` (`linear.hpp:397-406`) follows the supported compilers, which define it for IEEE 754 types, so its text moves; `rotation`'s zero direction follows the standard and stays. A `constexpr` function in `detail` is judged as if it were called at run time, since nothing stops a call, though the generator calls them in constant evaluation, where every assertion checks. A `consteval` function is never unchecked, so its preconditions are not undefined behavior in any build.

What the method can miss:
- **A precondition with no assertion behind it** that is not named in the first table and whose clause does not name `GALOOT_ASSERT`, such as one marked "not checked here". Only reading finds those; the first table, and the checker's copy of it, name the ones found that way.
- **A precondition stated outside a Requires clause**: in a `Guarantees:` sentence ("a list that would outgrow Capacity fails GALOOT_ASSERT"), in a prologue (`bits.hpp:14-15`, now named in the first table), or in a paragraph of its own (`transform`'s "Checks, ..."). The clause is found; such a sentence is not, unless the third table names it.
- **A clause that runs on past the precondition**, into a remark in the same paragraph: the check then asks about lines that state no precondition (as at `metric.hpp:127-128`), which the second table names. The opposite, a precondition continued after an empty comment line or a label, is cut off.
- **An assertion in a lambda or a local class** is charged to the function around it.
- **A layout the walk does not expect**: a blank line between a comment and its declaration, a declaration line that ends with `;` or `}` inside its own head (a `requires requires { ... }` on a line of its own), braces that differ between the branches of an `#if`, a comment that sits between a declaration's lines and is not a `NOLINT`. Each of these either gives an assertion no clause, which fails, or charges it to the wrong comment, which fails unless that comment's clause is listed; the scanner also fails on braces that do not balance. None occurs at `01dce7f`: every assertion's declaration and clause were read against the header.
- **Whether a listed verdict is right.** The check makes every line pass through a table; whether a condition is undefined behavior is judged by reading, and the tables say why.

### The boundary report

Two rows are neighbors when the second starts on the line after the first ends. The report lists each pair of neighbors whose class or `waits_for` differ and where the first row's last line does not end a sentence (its text, without closing brackets, quotes, or backticks, does not end in `.`, `?`, or `!`), so the sentence open there ends in the second row. Marker and example rows are left out: they hold directives and code, not sentences. The test reads only the last line's end, so it misses a sentence that runs on past a line ending in an abbreviation such as "e.g.", and it reports a line that ends a sentence without a period, such as one that ends with a code span or introduces a list with a colon. It reports a line that holds the end of one sentence and the start of the next, which is the case the row ranges cannot split.

It ran on this branch with Python 3.14, in under a second, and printed no problem, both when the table was first written and after the corrections below. With the precondition check, it fails on a table in which one kept precondition row (`metric.hpp:126`) is demoted to delete, on the table before the correction of `collect_result` (line 304 deleted), and on a README whose first table lacks the `generate_map` entry, each with one problem that names the row or the lines, and passes again once the change is undone. After the delta check it was run on 19 altered copies of the table and this README, the delta check's 14 demotions and five more for the places listed since: each demotion of a place kept in full is reported, with this README unchanged, with its line deleted, or (as tried for `antigrade`) with its line moved to the second table, and so is deleting a line from the first table alone. No build or test ran for this work. For the corrections, one probe of a few lines was compiled with GCC 16.2.1 and `-fsanitize=undefined` in the session's scratch directory, outside the job pool, to confirm two cases of undefined behavior. Nothing was timed, so the timing lock was not taken.

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
| `core/linear.hpp` | 79 | 25 | 23 | 67 | 194 |
| `core/metric.hpp` | 61 | 8 | 41 | 8 | 118 |
| `core/mv.hpp` | 246 | 97 | 60 | 59 | 462 |
| `core/plan.hpp` | 270 | 0 | 31 | 65 | 366 |
| `core/product.hpp` | 76 | 5 | 25 | 45 | 151 |
| `core/transform.hpp` | 127 | 10 | 102 | 106 | 345 |
| `core/unary.hpp` | 21 | 8 | 7 | 32 | 68 |
| `presets/rga3.hpp` | 5 | 0 | 0 | 0 | 5 |
| `presets/rga3/motions.hpp` | 46 | 0 | 44 | 15 | 105 |
| `presets/rga3/objects.hpp` | 40 | 0 | 7 | 50 | 97 |
| **All** | **1,595** | **295** | **652** | **666** | **3,208** |

The 73 trailing comments are all kept.

By kind:

| Kind | Keep | Summary | Move | Delete | All |
| --- | --- | --- | --- | --- | --- |
| public contract | 264 | 266 | 483 | 100 | 1,113 |
| internal contract | 495 | 0 | 0 | 37 | 532 |
| equation or convention | 364 | 0 | 54 | 70 | 488 |
| file prologue | 86 | 0 | 83 | 192 | 361 |
| reason | 298 | 29 | 21 | 10 | 358 |
| example | 0 | 0 | 2 | 167 | 169 |
| marker | 51 | 0 | 0 | 45 | 96 |
| narration | 0 | 0 | 9 | 45 | 54 |
| pointer | 37 | 0 | 0 | 0 | 37 |

Narration is rare: 54 lines in all. Contracts, reasons, and equations together are 2,491 of the 3,208 lines, 78%. That agrees with the knowledge-base census, which found about three quarters of the comment lines to be contracts or reasons by a keyword heuristic.

By what the rows wait for:

| Waits for | Lines | Of which delete |
| --- | --- | --- |
| none (kept) | 1,595 | |
| none (delete or summary now) | 499 | 476 |
| B9 | 45 | 45 |
| D1 | 26 | |
| D3 | 97 | 17 |
| the S11 amendment | 128 | 128 |
| D3 and the S11 amendment | 818 | |

## Against plan 0048 and the restated targets

Plan 0048 section 5.4 estimated its four comment candidates from position alone. Here are the same four classes, by the verdicts read block by block. "Kept" counts keep rows, plus two lines for each summary row (one sentence), plus every row whose condition has not yet been met. It counts no pointer lines.

| Candidate | Now | Plan, S11 as ratified (a) | Plan, one sentence and a pointer (b) | Table, S11 as ratified, after D3 | Table, after the amendment |
| --- | --- | --- | --- | --- | --- |
| D1 prologues | 702 | 90 | 90 | 252 | 165 |
| D2 `///` on `detail` | 443 | 110 | 110 | 352 | 344 |
| D3 `//` elsewhere | 725 | 330 | 330 | 655 | 634 |
| D4 `///` on public declarations | 1,338 | 1,090 | 340 | 1,292 | 552 |
| **Total** | **3,208** | **about 1,620** | **about 870** | **2,551** | **1,695** |

The ratified S11 wording also asks each declaration that has a reference entry to name its guide page and heading, and the owner ruled one pointer on each such declaration, with members and rejecting overloads covered by the entry of the declaration they belong to (decision 0258 item 4.3). There are 157 public `///` blocks at namespace scope, 37 of them on rejecting overloads, and 36 on members. With each pointer on a line of its own:
- **A pointer per reference entry, as ruled:** about 120 lines, one for each of the 120 blocks at namespace scope that are not on a rejecting overload. One of them, on `GALOOT_ALWAYS_INLINE`, documents a macro that is not part of the interface, so the figure is an upper bound by one.
- **A pointer on every declaration, the option not taken:** about 190 lines (157 and 36).

So the realistic end state is about **1,815 comment lines**: 1,695 kept lines and about 120 pointers, a fall of 43%. The plan estimated about 870, a fall of 73%.

Stage by stage, as each condition is met (kept lines, no pointers):

| Stage | Comment lines | Change |
| --- | --- | --- |
| Now | 3,208 | |
| Deletions that wait for nothing, with those of `detail` comments, and B9 | 2,672 | -17% |
| Plan 0049's D1 and D3 pages received, S11 as ratified | 2,551 | -20% |
| The S11 amendment applied, once its no-drift checks run | 1,695 | -47% |
| The same with one pointer per reference entry | about 1,815 | -43% |

The first part's [restated targets](../simplification/restated-targets.md) gave the comment column as follows:
- **Before D3:** "at least about 1,620, and more by whatever the comment table marks move". The table says 2,672: the deletions that wait for nothing, including those of `detail` comments, and B9.
- **After D3 and the amendment:** about 870. The table says about 1,815, pointers included.

So the restated totals' comment column should read 2,672 and about 1,815. With the code figures unchanged and blank lines scaled as there, that makes the restated "all lines" about 6,992 and about 6,135, not 5,940 and 5,190. The owner accepted the targets as D0 measured them, about 6,120 lines and about 1,800 comment lines (decision 0258 item 4.6); the 14 lines of `rotation`'s precondition, kept in full by item 4.4 of the same decision, are most of the difference, which is within the precision of those figures.

### Why the estimate and the reading differ

| Class | Gap | Where it comes from |
| --- | --- | --- |
| D3, `//` | about 300 lines (304) | The plan expected to drop narration and repeats and to move derivations. Read block by block, the `//` lines inside functions are mostly reasons (a GCC 16 miscompilation, why a first term is assigned, why a check is a variable template), equations applied where they are applied, and internal contracts of the generator. Only 54 delete and 33 move. The 46 marker lines that stay are suppressions and formatting directives, each suppression with the reason the static-check rule requires beside it |
| D2, `///` on `detail` | 234 lines | Most `detail` comments describe the generator: `generate_plan`'s count-then-fill protocol, the capacity proofs, the self-checks and the conditions they test, the rounding counts, the elimination. Plan 0048's own rule 4 keeps a `detail` comment whose behavior, bound, or cost is not obvious, and these are the least obvious code in the library. What goes is "Requires: nothing", "an aggregate, equal when every member is", and member comments that repeat their class |
| D4, public `///` | about 210 lines (212), before pointers | Four things stay that the estimate of 340 lines did not allow for: the one sentence of each of 42 rejecting overloads; one-line comments on 36 members; summary sentences that are themselves the formula, such as the sign rules of `bits.hpp` and the coefficient orders of the rga3 layouts; and eleven preconditions on public declarations kept in full under the ruling |
| D1, prologues | 75 lines | Prologues keep the conventions they apply and the reasons for them: the mask convention in `bits.hpp`; why a blade is a wedge and not a product, with the integer-polynomial invariant, in `blade_product.hpp`; why the complement reverses canonical order, in `plan.hpp`; why the odd form of `transform` has its sign. The precondition of `GALOOT_FORCE_INLINE` (`assert.hpp:30-32`) stays in full. The examples, the restated guide text, and the cost paragraphs go: 427 of 702 lines are deleted. Of these, 17, the two illustrations, go only once D3's reference entries hold them, and 28 only under the S11 amendment, since they are contract text no other header line states: what a change of basis preserves, which `change_basis`'s comment points to, and two lookup caveats |

A tighter pass is possible:
- **Cut every kept `detail` `///` row to two lines:** 178 lines more.
- **Exclude `performance-enum-size` in `.clang-tidy`:** 14 more. That needs a ruling like O6, and decision 0255 part A item 10 keeps the suppressions' reasons.

It would bring the end state to about 1,620 lines. It would remove the generator's internal contracts, the text that explains the items plan 0048 section 5.5 protects. This record does not recommend it, and the owner did not take it (decision 0258 item 4.6).

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
6. **The generic-code caveat on each rejecting overload.** "A requires-expression that asks is told yes" is said once in the guide, in core 'Writing generic code', and at the products' operand rule, which moves to the reference. A reader of one rejecting overload learns it from there.
7. **Lookup caveats at the declaration.** The `std::ranges` clash of `reverse` and `transform`, and `part` against a variable named `part`, leave the header. Both are in the guide already. The `part` caveat moves with its entry.

A maintainer keeps the following:
- every reason;
- every invariant the code relies on;
- every equation at the place it is applied;
- every published source;
- every pointer to the limits page, in the fixed form plan 0049's pointer test reads;
- the comments that keep a compiler's error readable: the trailing comments on the conversion constructors and on the deleted `==`, and the rule that a message must start on the line of its `GALOOT_ASSERT`.

## Preconditions kept in full

Undefined behavior when broken in an unchecked build, kept under the ruling. Plan 0048 named two (the first two rows). Reading found fourteen more, five of them on `detail` helpers. Three of these were added after the sample check (marked "added"), and two more were already kept with that reason in their notes but missing from this list (marked "listed"). The precondition check found six more, and reading where they lead found a seventh, all generator contracts in `detail` whose rows were already keep (marked "checked"). After the delta check, four more that were already keep rows with this reason are listed, so that the checker guards them too (marked "guarded"). The owner's ruling on "undefined behavior" (decision 0258 item 4.4) added `rotation`'s, which was a move row (marked "ruled"). The checker reads this table, and the two below it, and holds its own copy of this one:

| Place | Precondition |
| --- | --- |
| `blade_product.hpp:49-57` | A coefficient overflow at run time with checking off |
| `linear.hpp:392-396` | An integer division by zero |
| `linear.hpp:407-411` | `min() / -1` for a signed type at least as wide as `int` (not checked in any build) |
| `bits.hpp:115-116`, `:130-131`, `:143-144` | `complement_mask`, `right_complement_sign`, `left_complement_sign` with n of 0 or above 32: `low_bits` then shifts by the width of the mask |
| `bits.hpp:80-81` (added) | `antigrade` with some n of 2^31 or more: `static_cast<int>(n)` is n modulo 2^32, so `static_cast<int>(n) - grade(a)` overflows `int` when n modulo 2^32 is from 2^31 to 2^31 + grade(a) − 1 |
| `bits.hpp:167-168` (added) | `antireverse_sign` with an antigrade above 46341: m(m − 1) overflows `int` |
| `bits.hpp:45-47`, `:58-60` | The same requirement on the `detail` helpers that the shift sits in |
| `plan.hpp:303-304` (added) | `collect_result`'s `out` must have a place for every distinct blade, or it is written past its end |
| `plan.hpp:664-665` (listed) | `unary_term` with an n out of range reaches `complement_mask`'s shifts |
| `blade_product.hpp:82-83` (listed) | `multiply_checked`'s operands within range, since negating the most negative value is undefined |
| `metric.hpp:126` | `is_null(i)` with i at or beyond N, which reads past the array |
| `blade_product.hpp:139-140` (checked) | `term_list::add` on a full list writes past the end of its array; stated as a guarantee, so the check does not find it, but the precondition clauses below lead to it |
| `blade_product.hpp:268-275` (checked) | `blade_product`: a mask beyond e_N reads `g.gram` past its end; too small a Capacity writes past the term list; an overflowing coefficient is a signed overflow |
| `basis_change.hpp:753-757` (checked) | `blade_image`: a mask beyond e_N reads `rows.rows` past its end, and the same capacity and overflow conditions |
| `plan.hpp:142-143` (checked) | `pair_capacity` for an antiproduct with n out of range reaches `complement_mask`'s shifts |
| `plan.hpp:185-187` (checked) | `pair_terms`: too small a Capacity writes past the term list, and the product's case is `blade_product`'s |
| `plan.hpp:255-259` (checked) | `generate_plan`: the masks, the capacity and the coefficients, as for `pair_terms` and `blade_product` |
| `plan.hpp:698-699` (checked) | `generate_map`: what `unary_term` requires, whose n out of range reaches the shifts |
| `mv.hpp:1281-1283` | The reference that `get` returns from a temporary must not outlive it (in every build) |
| `assert.hpp:30-32` | Every translation unit must use one `GALOOT_FORCE_INLINE` mode, since mixing them breaks the one-definition rule |
| `bits.hpp:14-15` (guarded) | The precondition every function of `bits.hpp` shares, n from 1 to 32 and a mask within it, stated once in the prologue |
| `blade_product.hpp:117-118` (guarded) | A `term_list` starts empty: one made at run time without `{}` has an indeterminate `length`, which `add` reads and writes past |
| `basis_change.hpp:258-259` (guarded) | `multiply_noting`'s operands lie within -max to max, since negating the most negative value is undefined |
| `plan.hpp:160` (guarded) | `plan_capacity` requires what `pair_capacity` requires of each pair, whose n out of range reaches `complement_mask`'s shifts |
| `presets/rga3/motions.hpp:48-61` (ruled) | `rotation`'s floating T, its line, and a direction whose length is finite and nonzero: with checking off, a zero direction divides by zero, which [expr.mul] leaves undefined, and squares that overflow give a result outside the type's range, which [expr.pre] leaves undefined, whatever IEEE 754 says |

The two cases of `bits.hpp` were confirmed by a probe built with GCC 16.2.1, `-fsanitize=undefined` and `GALOOT_CHECKING` 0, which reports a signed overflow for `antigrade(1, 2^31)` and for `antireverse_sign(0, 50000)`.

### Checked conditions that are not undefined behavior

The precondition check leads to these lines too. Breaking what they state is diagnosed at compile time, gives a defined result, or is not a precondition at all, so the ruling does not keep them and each row keeps its own class.

| Place | What it states | Why it is not undefined behavior when broken |
| --- | --- | --- |
| `assert.hpp:126-129` | `GALOOT_ASSERT`'s message is a literal on the macro's line | Only the diagnostic changes; a message that is not a string is a compile error |
| `blade_product.hpp:65` | `add_checked`'s operands lie within -max to max | With an operand of min, the check still compares correctly and its bounds cannot overflow; what is undefined unchecked is an overflowing sum, which `blade_product.hpp:49-57` states and keeps |
| `blade_product.hpp:134-135` | `term_list::add`'s coefficient lies within -max to max | The same: the sum goes through `add_checked`. A full list is the case listed above, at `:139-140` |
| `blade_set.hpp:129-130` | `canonical_name`'s mask names only e1 to e9 | The loop stops at e9 and does not overrun |
| `linear.hpp:389-391` | `x / s`: s has x's scalar type, and T has a `/` | A call that breaks it does not compile |
| `linear.hpp:397-406` | Which integer types the zero-divisor check reads, and that a floating T is not checked | Not a precondition but the scope of the check; the undefined case is kept at `:392-396`. An unchecked floating division by zero is undefined in the standard, but the owner ruled that here the supported compilers decide, and GCC and Clang define it for IEEE 754 types (decision 0258 item 4.4), so the text moves |
| `metric.hpp:127-128` | A null generator need not make the form degenerate | A remark that ends the Requires paragraph, not a precondition |
| `metric.hpp:239-242` | `from_gram`'s form is square, of a valid size, symmetric, with entries in range | `from_gram` is `consteval`, so every check runs at compile time in every build |
| `mv.hpp:1035-1044` | `truncate` drops only zero coefficients of an exact scalar | With checking off they are dropped unchecked: a defined result |
| `transform.hpp:697-711` | `transform` checks that an exact scalar's dropped coefficients are zero | With checking off the dropped terms are not computed; the result is defined |
| `transform.hpp:713-728` | `transform` checks a rounding scalar's dropped coefficients against a bound | The same |
| `basis_change.hpp:285-287` | `check_fits`: a value the change computes fits `coefficient_type` | The arithmetic that sets the flag returns 0 instead of overflowing, so a broken condition gives a wrong value, not undefined behavior; the change tables are built in constant evaluation, where it always checks |
| `presets/rga3/motions.hpp:62-68` | Why `rotation` returns a plain `Motor`, and that a subnormal length passes the check and loses accuracy | Not a precondition: a reason, kept, and a remark that ends the Requires paragraph, as at `metric.hpp:127-128`. The precondition before them, `:48-61`, is in the first table |

### Assertions with no Requires clause above them

| Assertion | Its condition is stated at | Why there |
| --- | --- | --- |
| `basis_change.hpp:290` | `basis_change.hpp:285-287` | `check_fits` is a helper that reports a flag its callers compute; its own comment says what the flag means |
| `mv.hpp:983` | `mv.hpp:1035-1044` | `check_dropped` checks the precondition of `truncate`, its only caller |
| `transform.hpp:404` | `transform.hpp:697-711` | `check_transform_dropped` checks what `transform`'s comment says it checks for an exact scalar |
| `transform.hpp:618` | `transform.hpp:713-728` | `check_transform_rounded` checks the bound `transform`'s comment states for a rounding scalar |

## Hard calls

1. **`blade_product.hpp:14-29`, the recursion's derivation: move.** Each equation it derives is restated at its function, so the header keeps what it applies. What leaves is the proof that e_A = e_a ∧ e_A' = e_a e_A' − e_a ⌋ e_A' gives the recursion. A maintainer changing the recursion would want it in place. A concepts page that the header points to was judged enough.
2. **`transform.hpp:464-475`, the rounding bound's derivation: move.** It is the rounding-bound text the plan names, minus what stays: the statement, Higham's lemmas, and the margin's reason (`:457-463`, `:476-480`).
3. **`transform.hpp:590-610` and `:730-749`, the causes of a rounding abort: both move to one diagnostics heading.** They say partly the same thing in two places. The receiving author merges them. The alternative was to keep the internal copy at the check, for the maintainer, and move only the public one. That keeps a fact in two places, which plan 0048's rule 3 forbids.
4. **`plan.hpp:27-33`, why the complement reverses canonical order: keep.** Plan 0049 sends design reasons to the concepts pages. This one is the invariant that `run_term` relies on and that the order check tests, and no other comment proves it.
5. **`plan.hpp`, `mv.hpp`, `basis_change.hpp`, the generator's `detail` contracts: mostly keep.** They are where plan 0048 expected the largest `detail` saving (D2), and where reading found the least narration. The verdict rests on S11's first paragraph and the plan's own rule 4. It is the largest single cause of the gap above.
6. **The seven "An int, so that a compiler prints its values and not a byte": keep.** The plan lists them as a repeat. Each one sits beside a `performance-enum-size` suppression, and the static-check rule asks for the reason beside the suppression. They go only if the check is excluded in `.clang-tidy` (open item 3 below).
7. **The "message on the line of its GALOOT_ASSERT" rule: one home, at `assert.hpp:126-129`, kept as a reason.** Its five repeats point there. It is also part of `GALOOT_ASSERT`'s contract, but a maintainer adding an assertion needs it at the macro.
8. **`blade_set.hpp:21-25`, canonical order in the prologue: keep.** `canonical_less` states the order again in its one sentence. The prologue alone warns that canonical order is not mask order, and so says twice what plan 0048's rule 3 would say once.
9. **`linear.hpp:396-406`, which integer types the zero-divisor check reads: move.** The undefined behavior stays (`:392-396`). Which types are checked goes to the reference, so a reader with a big-integer type learns there, not here, that its division by zero is its own affair. The owner confirmed that the floating case moves too (decision 0258 item 4.4).
10. **`dual.hpp:25-42`, the table of definitions: delete as a repeat of core 'Bulk, weight, and their duals'.** Every function keeps its own formula. The table is the one place a reader sees the eight side by side. It survives in the guide, and plan 0049's conventions chapter receives it from there.
11. **`rotation`'s reason for returning a plain `Motor`: keep (`motions.hpp:62-66`).** It explains a departure from the rule that a degenerate input returns a failure value. That is the kind of reason a reviewer asks for, in the place the reviewer reads.

## For the units that use the table

- **Repeats cited in `core.md`, `rga3.md`, and the first page.** 53 delete rows name `docs/guide/core.md`, `docs/guide/rga3.md`, or `docs/guide/README.md` as the place that already says the text. Plan 0049's U3 splits and deletes the first two and moves the first page's nine facts to `howto/first-things.md`. By the time phase 4 runs, each such row must name the page and heading the text went to. U3's list of where each old heading went is the input to that rename.
- **Cross-references inside kept text.**
  - `dual.hpp:368` sends the reader to the header's own prologue (`:44-59`), which moves. Its row moves with it: both wait for D3 and the S11 amendment.
  - `change_basis`'s pointer to the prologue becomes a pointer to the concepts heading (`basis_change.hpp:1113-1115`). The pointer and the prologue rows it points to (`:51-65`, `:66-73`, `:74-82`) wait for the S11 amendment together.
  - `transform`'s pointer to the comment at the rounding check (`transform.hpp:752-754`) and that comment (`:590-610`) wait for D3 and the S11 amendment together, so the pointer becomes one to the diagnostics heading when both go.
  - Several pointer rows say "reword to stand alone" where the sentence they finish goes.
- **Plan 0049's U9 and U10.** The reference pages receive 450 lines, 433 of them public contract, by page in the table's `target` column. The diagnostics page receives 54 lines, almost all about the two `transform` messages. (Earlier versions of this record gave 482, the lines of every move that waits for D3 and the S11 amendment, some of which go to concepts and diagnostics pages.)
- **Illustrations for the reference.** Two prologue deletes carry a concrete case that no guide page has: `transform.hpp:24-29` (a motor of rigid(2) or conformal(3) has blades of odd grade) and `dual.hpp:60-70` (the bulk of a rigid(3) line holds e12, e13, e23, with the moment on e31 negated on e13). Both follow from rules the guide states, so neither is a lost fact. Their notes name the reference entries for `transform` and `bulk` as the place for them; U9's author takes them from the header at `01dce7f`. Both rows wait for D3: no comment unit deletes them before those entries hold the illustrations (decision 0255 part A item 10).
- **The pointer test.** The kept pointer sentences to `limits.md` use the form `docs/guide/limits.md, under 'Heading'`. One, `mv.hpp:889-891`, names two headings in another wording and needs rewriting into that form. Several others finish a sentence that goes, and their notes say to reword them to stand alone.

### Before a contract leaves a header

The owner ratified the S11 wording on the condition that the header and the guide do not drift apart (decision 0258 sections 2 and 3). Phase 4 must have the following before it acts on any row that waits for the S11 amendment, alone or with D3, and so before any contract leaves a header. Each check exists as a test that runs at each merge into `main`; plan 0049's U9 and plan 0048's phase 4 design and build them, and their plans state each as an acceptance criterion before either unit starts:

1. **Pointers resolve.** Each pointer in a header names a guide page and a heading that exist.
2. **Coverage both ways.** Each public declaration in the interface manifest has a reference entry, or is a member or rejecting overload covered by its owner's entry under a stated rule. Each reference entry names a declaration that exists.
3. **One sentence, one text.** The sentence on a declaration and the opening sentence of its reference entry are the same text, compared by a test.
4. **Signatures agree.** The signature a reference entry prints is the declared one, compared by the tool that writes the manifest.
5. **Preconditions kept in full read the same.** A precondition that stays on a declaration, as each place in the first table of [Preconditions kept in full](#preconditions-kept-in-full) does, appears in its entry in the same words.
6. **Messages agree.** Each diagnostic text a guide page quotes is compared with the literal in the header.
7. **Stated contracts are exercised.** Each requirement an entry states with a checkable violation names a compiled example or a compile-fail test that exercises it.

Two process rules go with them: a contract moves out of a header only after the page that receives it has passed review, and a unit that changes what a declaration requires, guarantees or preserves changes its entry in the same unit. Until the checks exist, S11 stands as ratified on 2026-11-01, and the rows that wait for the amendment stay where they are. The decision's list is the coordinator's reading of the owner's condition and the owner can change it; if it changes, this list follows it.

### Sentences that cross a row boundary

A row's range is whole lines, and a sentence may start on the last line of one row and end on the first line of the next. Where the two rows wait for different conditions, or are classed differently, the unit that acts on one of them first must not delete or move the shared line alone. It rewrites that line, and the neighbor's part of the sentence, so that what stays is whole sentences; the neighbor row's own verdict then applies to the rewritten text. Some notes name such lines (`unary.hpp:24`, `transform.hpp:589`, `blade_set.hpp:577`, `plan.hpp:627`), but the rule holds at every boundary, named or not. Boundaries of this kind are common, since most comments wrap mid-sentence: [boundary-report.tsv](boundary-report.tsv) lists the 103 the checker finds, with both rows, their verdicts, and the two lines on either side. A unit acting on a row reads that list for the row's number first.

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

Every other failure of the first run was the record's, not the table's: six Requires clauses of generator contracts whose violation is undefined behavior were keep rows but missing from the list of preconditions kept in full, and are now in it, with the capacity guarantee of `term_list::add` that they lead to; the other clauses the check reached, and the four assertions with no clause of their own, are now named in the two tables after it, each with its reason. No other row changed class.

## Corrections after the delta check, 2026-11-21

An independent delta check of the corrections above (the review record `docs/agent/records/reviews/2026-11-21-0048-d0-delta-check.md`, on branch `review/simplification-survey-fix` at `0a1505e`) found three minor errors and four nits. Each was judged against the headers at `01dce7f` and decision 0253 item 14. The table's corrections:

| Finding | Verdict | Correction |
| --- | --- | --- |
| 2. `objects.hpp:97-99`, the flector's transform rule, was an equation that deleted with nothing to wait for, while the motor's rule at `:84-86` is a public contract that waits for the S11 amendment | Confirmed. Both sentences say what `transform` does with a value of the layout they sit on, so they are the same kind of text; read as the motor's is, the flector's is part of `flector_layout`'s contract, and item 14 lets no unit remove it before the amendment | Kind public contract, waits for the S11 amendment, and the home is rga3 'Reflections', which states it in full. Its boundary with `:100-103` is gone, since both rows now wait for the same thing |
| 3. `linear.hpp:51-53` deleted "a class derived from mv is accepted as its mv base" for the sums and scaling, citing `unary.hpp:25-26`, which says it only for the involutions and complements | Confirmed. No other header line and no guide page states it for the sums; `product.hpp:41-42` says something else for the products | Move to `reference/index.md`, under a proposed heading 'Classes derived from mv', after D3. `unary.hpp:25-27`, whose note already proposed that the index state the rule once, moves to the same heading |
| 4. `linear.hpp:36-43` cited `product.hpp:35-42`, which covers products only | Confirmed | The home is core 'Writing generic code', which has the rejection by an overload, the requires-expression caveat, and the question to ask for a sum. The note names core 'Sums and scaling' for the number case and the kept lines on the rejecting overloads |
| 5. The overflow range of `antigrade` was overstated | Confirmed. `n` is a `std::size_t`, and the conversion to `int` is modular since C++20, so only n modulo 2^32 from 2^31 to 2^31 + grade(a) − 1 overflows; n = 2^32 converts to 0 | The first table's entry and the row's note give that range. The verdict, keep, is unchanged |

Rows of the same kinds were looked for:
- **Public deletes that wait for nothing.** Three remain: `metric.hpp:66` and `objects.hpp:87-90` restate a convention that is stated where it is applied (`bits.hpp:9-13` and `rotation` and `translation` in `motions.hpp`, both kept), and `metric.hpp:256-257` is a `//` reason inside a function, with its home at `assert.hpp:126-129`. None of them says what a declaration requires, guarantees, or preserves. `objects.hpp:87-90`, the half angles of a motor, is the one the delta check asked about; it is left as a delete that waits for nothing, since STYLE S11 asks for a convention where it is applied, and it is applied in `rotation`'s and `translation`'s formulas (`motions.hpp:32-34` and `:144-145`), both kept.
- **Deletes whose cited header range covers another case.** Every delete or summary row that cited a header range once the corrections in the table above were made (38 rows) was reread beside that range. Six more had the error of finding 4: the requires-expression caveat on the rejecting overloads of the sums and scaling (`linear.hpp:255-256`, `:295`, `:440-441`), of the dot (`dual.hpp:550-551`), and of `transform` and `compose` (`transform.hpp:823-824`, `:834-835`) cited `product.hpp:35-42`, which states it for the products only. Each now cites core 'Writing generic code', which states it for every rejecting overload; they still wait for the S11 amendment. The other header homes hold what their rows delete.

The other findings:

| Finding | Verdict | Correction |
| --- | --- | --- |
| 1. Six kept places were guarded only by this README's own list: a unit that demoted one and deleted its line here passed the check, and so did one that moved a line to the second table | Confirmed, by rerunning the delta check's altered copies | The checker holds the list itself, as `KEPT_IN_FULL`, and requires that each place is a keep row and that the first table names exactly those places (see [The precondition check](#the-precondition-check)). The delta check's 14 cases and five more all fail now. Rereading the keep rows found four more places of the same kind that the list left out, `bits.hpp:14-15`, `blade_product.hpp:117-118`, `basis_change.hpp:258-259` and `plan.hpp:160`; they are listed and guarded. Their rows were already keep, so no sum changes |
| 6. "Eleven preconditions on public declarations" in the D4 row counted `assert.hpp:30-32`, a `//` prologue line | Confirmed | The D4 row says ten, and the D1 row names the prologue's precondition. Its count of deleted prologue lines is now 427, the checker's figure after finding 3 |
| 7. Eight of the eleven commits named the agent only in the `Co-Authored-By` trailer | Not a defect. WORKFLOW W7 asks that a change made by an agent be "identified as such in its message", and the trailer, which names the model, is part of the message. History is not rewritten for it | None to those commits. Each commit of this correction also says in a sentence that an agent made it |

The sums, the stage figures, and the counts were then the checker's output after these changes (the owner's rulings changed them since; see the next section): 666 lines move and 666 delete; the stages are 2,718, 2,627, 2,439 and 1,681; 53 delete rows cite `core.md`, `rga3.md` or the first page (43, 8 and 2); the boundary report lists 104 places.

## Changes after the owner's rulings, 2026-11-21

The owner ruled the open items below that went to them (decision 0258 section 4, which rules part B of decision 0255), ratified the S11 wording with a condition that header and guide not drift apart (sections 2 and 3), and left the coordinator's readings of decision 0255 part A standing. The table, the checker, and this README were brought in step, by an agent (Claude Opus 5.5, cpp-expert role):

| Ruling | Rows | Change |
| --- | --- | --- |
| S11's contract sentence does not apply inside `detail` (item 4.2) | The 36 rows that waited for "the detail ruling" alone: 33 deletes (80 lines) and three summaries (17 lines) | Wait for nothing |
| The same | `mv.hpp:895-899`, an internal contract of the product kernel moving to a concepts page | Waits for D3 alone |
| The same | `assert.hpp:77-83`, on `GALOOT_ALWAYS_INLINE`, and `mv.hpp:880-882`, the requirement every product puts on a scalar the exactness check cannot read | Wait for D3 and the S11 amendment. The first is on a macro, in no `detail` namespace, so the ruling does not reach it; the second sits in `detail` but is public contract, stated in no public comment. Both are the only statement in the headers of something a user meets, so both leave a header only under the no-drift condition |
| "Undefined behavior" in the standard's sense for `rotation` (item 4.4) | `motions.hpp:48-61` | Keep, in the first table of [Preconditions kept in full](#preconditions-kept-in-full) and in the checker's `KEPT_IN_FULL`, now 28 places. The rest of the clause, `:62-68`, stays in the second table |
| The supported compilers' sense for the floating division (item 4.4) | `linear.hpp:397-406` | Still moves; its note and the second table name the ruling |
| One pointer per reference entry (item 4.3) | None | The end state counts about 120 pointer lines, not a range |
| Decision 0255 part A item 10, the two prologue illustrations | `transform.hpp:24-29`, `dual.hpp:60-70` | Still deletes of a repeat, with the same homes; they wait for D3. The item placed this change in the commit that renames the targets after U3; it is made now, so the wait holds from today |

Two changes to the check go with them. The checker now accepts only the waiting conditions in [What a row waits for](#what-a-row-waits-for); this README already said it checked them, but it checked only that a value was given. And [readme_figures.py](readme_figures.py) computes the figures above that the checker does not print, so every figure here is a script's. Writing it corrected three figures of earlier versions, by stating how each is counted: the marker lines kept among the `//` comments are 46, not 42; cutting every kept `detail` `///` row to two lines saves 178, not about 177; and the reference pages receive 450 lines, where 482 counted every move that waits for D3 and the S11 amendment.

After these changes (the corrections in the next section changed some since): 652 lines move and 666 delete; 1,595 are kept; the stages are 2,644, 2,449 and 1,695, and about 1,815 with the pointers; 53 delete rows cite `core.md`, `rga3.md` or the first page (43, 8 and 2); the boundary report lists 103 places. The checker passes. A copy of the table with `motions.hpp:48-61` demoted to move fails it, and so does one in which a row waits for "the detail ruling" again. No build or test ran, and no header changed.

## Corrections after the rulings check, 2026-11-21

An independent check of the changes above (the review record `docs/agent/records/reviews/2026-11-21-0048-d0-rulings-check.md`, on branch `review/comment-table-rulings` at `196b2a8`) found three minor errors and four nits. Each was judged against the headers at `01dce7f` and decision 0258, by an agent (Claude Opus 5.5, cpp-expert role). The table's corrections:

| Finding | Verdict | Correction |
| --- | --- | --- |
| 3. The criterion that kept `assert.hpp:77-83` and `mv.hpp:880-882` under the S11 amendment, "contract text a user needs", was not applied to four rows of the same kind that waited for D3 alone: `linear.hpp:51-53`, `unary.hpp:25-27`, `product.hpp:35-43`, `mv.hpp:895-899` | Confirmed. No other header line states what the sums, the scaling, the involutions and the complements do with a derived class, that a product takes no scalar operand, or that a product agrees between constant evaluation and run time unless multiply-adds are fused (`transform.hpp:764-770` says it for `transform` only). Decision 0258 section 3 says no unit removes a contract from a header, not only from a declaration's comment | The four rows wait for D3 and the S11 amendment. `mv.hpp:895-899` is public contract, as `:880-882` in the same comment is. [What a row waits for](#what-a-row-waits-for) now defines contract text for these conditions, and the definition was applied to every row that left before the amendment (below) |
| 4. `assert.hpp:77-83` was "internal contract", text on a `detail` entity, though the macro is in no `detail` namespace | Confirmed. It states what a caller of each operation the macro marks must arrange | Kind public contract: 7 lines move between the two rows of the by-kind table, with the 5 of `mv.hpp:895-899` |
| 6. `format.hpp:215` has position `detail` because the census rule takes the namespace alias on line 211 for a namespace | Confirmed, and the only comment line it affects | Left as the census labels it, so that the sums still compare with plan 0048 section 3.1. The row's note and the `position` column above say so |

The definition was then applied to every row that a unit may act on before the amendment: the 81 rows that wait for nothing, B9, D1 or D3 and are not keep rows, examples, markers or narration. The 52 in prologues and the 9 public ones were read beside the declarations of their header and searched for in the other headers. The 20 in `detail` were judged from their text and the rulings check's reading of the rows the detail ruling freed. That found seven more rows of finding 3's kind:
- **Text a declaration's comment sends the reader to.** `change_basis`'s comment says that "the header's comment lists the operations it preserves, with their factors, and those it does not" (`basis_change.hpp:1113-1115`, which waits for the amendment). That list, `:51-65`, was deleted with nothing to wait for, and `:66-73` and `:74-82`, the conditions under which the complements, the bulk and the weight are preserved, waited for D3; no other header line states any of it. Likewise `bulk`'s comment points to `dual.hpp:44-59` (D3), and `transform`'s to the causes and remedies at `transform.hpp:590-610` (D3). All five now wait for the amendment, with D3 where they move. The README already said that `dual.hpp:368`'s row "moves with" the prologue; it did not, and now does.
- **Lookup caveats.** `transform.hpp:59-65` (the clash with `std::ranges`) and `dual.hpp:86-91` (a variable named `bulk`, `weight` or `dot`, and `std::execution::bulk`) were deleted with nothing to wait for. The same caveat on `reverse`'s declaration, `unary.hpp:63-69`, is public contract and waits for the amendment, and no other header line states these two, so both now wait for it. `presets/rga3/objects.hpp:30-33` repeats `transform.hpp:59-65` and still waits for nothing; its note says why.

Rows read and left as they were, with the reason:
- the other prologue deletes, each of whose contract text a declaration's comment in the same header also states (the result layouts, the operand rules, the scalar rules, `truncate` and `part`, the forms of `transform` and `compose`, the checks of a change of basis), or which is an example, a cost paragraph with its pointer kept, or a convention stated where it is applied;
- `unary.hpp:28-30`, that the involutions and complements read only n: each operation's own comment defines it without the metric;
- `transform.hpp:41-46`, the outermorphism property: a consequence of the formula `transform`'s comment keeps;
- `plan.hpp:47-56`, why a result layout holds no blade a product cannot reach, and the D3 moves of derivations, design reasons and costs;
- the five D1 conventions: `format.hpp:158-169` and `blade_set.hpp:624-629` are restated in kept comments on the helpers that apply them (`format.hpp:95-98`, `blade_set.hpp:421-425`), and the rest are consequences of formulas their declarations keep.

28 lines (three rows) move from the deletions that wait for nothing to those that wait for the amendment, and 74 lines (eight rows) from D3 to D3 and the amendment. So the stages are 2,672 (-17%) and 2,551 (-20%), and the end state is unchanged at 1,695, about 1,815 with the pointers. The restated "all lines" before D3 is about 6,992. The boundary report still lists 103 places; three of them now name the new conditions. No build or test ran, and no header changed.

## Open items

Each item has its options, their tradeoffs, and a recommendation. The owner has ruled items 1, 2, 5 and 6 (decision 0258); each says how, and the table follows the ruling. Items 3, 4 and 7 remain open.

1. **Where the pointers go, under the prepared S11 wording.** Ruled: option (b), one pointer on each declaration that has a reference entry (decision 0258 item 4.3, deciding decision 0255 part B item 3). The ratified wording says so (decision 0258 section 2), and the end state above counts about 120 pointer lines. Plan 0049's pointer test still needs its rule for which members and rejecting overloads an entry covers; check 2 of [Before a contract leaves a header](#before-a-contract-leaves-a-header) asks for that rule to be stated.
   - **The problem.** The prepared wording (decision 0253 item 14) says each declaration in a public header names its guide page and heading. Read strictly, that covers 36 members and 37 rejecting overloads as well as the 120 or so declarations a reference entry describes.
   - **The options.**
     - (a) A pointer on every declaration: about 190 lines. Uniform and easy to check, but a member's or a rejecting overload's pointer repeats its owner's.
     - (b) A pointer on each declaration that has a reference entry, with members and rejecting overloads covered by it: about 120 lines. Plan 0049's pointer test then needs a rule for what counts as covered.
   - **Recommendation:** (b). Put it to the owner with the confirmation of the wording, which decision 0253 already reserves.
2. **The detail ruling** (open item 2 of [restated-targets.md](../simplification/restated-targets.md)). Ruled: S11's contract sentence does not apply inside a `detail` namespace, the recommendation made there (decision 0258 item 4.2, deciding decision 0255 part B item 2). The table had 97 lines waiting for it alone and 15 waiting for it and D3; [Changes after the owner's rulings](#changes-after-the-owners-rulings-2026-11-21) says what became of each row.
3. **The seven `performance-enum-size` suppressions.** Open: the owner was not asked. Decision 0255 part A item 10 settled it for now at the recommendation, so the suppressions keep their reasons; the owner can reverse it.
   - **The options.**
     - (a) Keep each with its reason: 14 lines. Nothing changes.
     - (b) Exclude the check in `.clang-tidy` with the reason once, as decision 0253 item 19 does for the cast check. The check then no longer reports a real oversized enumeration in a header. The core has seven enumerations, all reviewed.
   - **Recommendation:** (a). The saving is small, and the reason is local to each enumeration.
4. **The headings.**
   - **The problem.** Every target heading here is a proposal, and so are the file names of the reference and concepts pages. 53 repeats cite pages that U3 removes or splits.
   - **The options.**
     - (a) When U3 publishes its heading list and its map of where each old heading went, one commit renames the table's targets. The checker's page list changes in the same commit.
     - (b) Leave the targets as they are and let each comment unit translate them.
   - **Recommendation:** (a). It gives the pointer test and the received-rows check one set of names. Decision 0255 part A item 10 already assumes such a commit, after U3's published headings.
5. **The comment targets of plan 0048.** Ruled: option (a), the targets as D0 measured them, about 6,120 lines and about 1,800 comment lines, restated with M1 (decision 0258 item 4.6, deciding decision 0255 part B item 6). The tighter pass, option (b), was not taken. The ruling does not mention (c); the recommendation's second reader, folded into the review of the first comment unit that opens `plan.hpp`, `mv.hpp` or `basis_change.hpp`, is left to that unit's brief. With the later rulings the table gives about 1,815 comment lines and about 6,135 in all, within that precision.
   - **The problem.** Section 5.4 and the restated targets expect about 870 comment lines at the end. The verdicts here give about 1,800.
   - **The options.**
     - (a) Accept the table and restate the targets with M1, as decision 0253 asks for the other figures.
     - (b) Ask for the tighter pass described above, about 1,600 lines. It cuts the generator's internal contracts.
     - (c) Have a second reader, the cpp-expert as plan 0048 section 7 pairs it with D0, reread the keep verdicts on `plan.hpp`, `mv.hpp`, and `basis_change.hpp` before phase 4.
   - **Recommendation:** (a), with (c) folded into the review of the first comment unit that opens those headers. One independent sample check has read the table (see [Corrections after the sample check](#corrections-after-the-sample-check)); it read 106 rows at random and every delete row, not the keep verdicts this item is about.
6. **Floating division by zero and overflow, read by the standard.** Ruled, by neither of the two options below but by a third that decision 0255 part B item 4 offered: the standard's sense decides for `rotation`'s zero direction, so `motions.hpp:48-61` stays in full, and the supported compilers' sense decides for the floating division of `linear.hpp:397-406`, whose text moves (decision 0258 item 4.4). Both places were changed, one commit per header.
   - **The problem.** The table treats `rotation`'s zero direction (`motions.hpp:48-68`) and an unchecked floating `x / 0.0` (`linear.hpp:397-406`) as defined, giving infinities and NaNs, and moves both texts. The C++ standard does not define them: [expr.mul] makes a division by zero undefined, and [expr.pre] does the same for a result outside the type's range, with no exception for IEEE 754 types. GCC documents `-fsanitize=float-divide-by-zero` as left out of `-fsanitize=undefined` because such a division "can be a legitimate way of obtaining infinities and NaNs" (read in the GCC 16.2.1 manual); Clang's documentation is recalled to say the same, and was not read for this record. The header's own text at `linear.hpp:403-405` relies on the standard's reading for a constant evaluation, where the division is not a constant expression.
   - **The options.**
     - (a) Keep the verdicts: both compilers Galoot supports define the result for IEEE 754 types, and the ruling's purpose, that nothing that can corrupt a program is hidden from the header, is met.
     - (b) Read "undefined behavior" in decision 0253 item 14 as the standard's term, and keep `motions.hpp:48-61` and `linear.hpp:397-406` in full: about 24 lines more in the end state.
   - **Recommendation:** (a), with the owner's confirmation that the ruling means behavior the supported compilers leave undefined. If the answer is (b), the two places move from the second table to the first and their rows become keep, one commit per header.
7. **Undefined behavior stated as a guarantee.** Open: the owner was not asked. Decision 0255 part A item 11 asks B5's author to keep the statement at `coefficient_type`, the second half of the recommendation. Since the detail ruling, both rows wait for nothing, so a comment unit may delete them before B5 runs.
   - **The problem.** `add_checked` and `multiply_checked` (`blade_product.hpp:66-68`, `:84-86`) say in their `Guarantees:` that a result out of range fails `GALOOT_ASSERT`. With checking off at run time that result is a signed overflow. Both rows are deleted as narration, and the precondition check does not see them, since they are not Requires clauses. The undefined behavior is stated once, in full, at `coefficient_type` (`blade_product.hpp:49-57`), which is kept.
   - **The options.**
     - (a) Leave the rows: the one kept statement covers every coefficient the generator forms, and plan 0048's B5 rewrites both functions.
     - (b) Keep both rows, and widen the check to `Guarantees:` sentences that name `GALOOT_ASSERT`. That brings in every guarantee of that form, most of which state checked behavior, not a precondition, and each would need a line in the second table.
   - **Recommendation:** (a), and ask B5's author to keep the statement at `coefficient_type` when the functions change.
