# 0048 Aggressive simplification of the Galoot core

- Status: proposed 2026-11-21 by Claude (agent, Fable 5.2), chief planner, with the cpp-expert, performance-engineer and hci-expert role files read. The owner authorized the Fable level for this planning unit (stated in the coordinator's brief). From `main` at `c95fee5`, on branch `plan/0048` in the worktree `galoot-wt/plan-0048`. No unit below has started. The coordinator's merge of this plan into `main` is the claim (W3, W7).
- Source of scope: the coordinator's brief of 2026-11-21 (simplify the code, the code comments and the in-code documentation aggressively; removal, consolidation and compositional abstraction are permitted); CONSTITUTION C3, C4, C9, C12, C14; STYLE S7, S11, S12, S16; PERFORMANCE P1 to P4, P8, P9; the release record's known friction; the handoff of 2026-11-15; the owner's code style guide.
- Siblings: plan 0047 (completion, hardening, verification; `plan/0047` at `e41f173`) and plan 0049 (pedagogical documentation; `plan/0049` at `0f0b6e5`), both read in their sections on this plan (0047 sections 5.3, 7, 8, 9.2; 0049 sections 6, 7). Plans 0043 to 0045 (release 1.0.1) and 0046 (Garnet) were read for what they change under `include/` (section 1, item 3).
- Evidence gathered for this plan: `records/performance/simplification-survey/` (scripts and outputs, committed with the plan). Everything was read-only on the tree; compile probes used scratch copies.
- Shared resources: the job pool read "not running" and was not started, since no build ran through it. The compile probes held the timing lock through `pe-common/lock.sh take` and `release`. No processor configuration was touched and none is proposed. One web request was made: `pip install lizard` into a scratch environment, with no personal identifier.
- Numbers: the next free decision is 0252 at `c95fee5`; siblings may take numbers first, so each unit takes the next free one when it writes.

## 1. Departures, ambiguities, and readings (C20)

1. **How deep the survey went.** Every header under `include/galoot/` was read as code. Five were read with their comments (`assert.hpp`, `bits.hpp`, `unary.hpp`, `blade_product.hpp`, `metric.hpp`); the other eleven were read as comment-stripped copies that mark each comment run with its length, and their comments were measured by script (kind, placement, repeats, pattern scans) but not read block by block. So this plan gives comment savings by class and by header, not a verdict per block. The per-block table is unit D0's deliverable, which plan 0049 section 7 item 1 also asks for.
2. **Savings are estimates.** Only two effects were measured: what the generator's self-checks cost to compile, and what comments cost to compile (nothing). Every line saving in sections 4 and 5 is read from the code and marked as an estimate; none was prototyped. Each code unit states its measured saving when it closes.
3. **Reading depth of the other plans.** Plan 0044 was read through its unit U2; plan 0043 through its status block; plans 0045 and 0046 by their headings and status. What matters here was checked directly: 0044 changes `assert.hpp`, two locals in `bits.hpp` and `basis_change.hpp`, and deletes the two untargeted `apply` overloads of `mv.hpp`. This plan does not count those 17 lines as its own saving.
4. **The knowledge-base notes on complexity reduction were not committed when this plan was written.** The brief names branches `kb/complexity-reduction-*`; one branch exists, `kb/complexity-reduction`, with no commit beyond its base. Its worktree held three uncommitted drafts when last read on 2026-11-21: `complexity-reduction/deep-modules-and-interface-size.md` (read), and `deletion-first-simplification.md` and `simple-versus-easy.md` (read in their sections on where to look and on a compile-time generator). Sections 3.6 and 5.2 cite them as drafts; nothing in them contradicts this plan. The notes it points to (`safe-refactoring-evidence.md`, `compositional-design-in-cpp.md`, `comments-and-self-documentation.md`, `compile-time-cost-measurement.md`) did not exist yet. Unit D0 reads whatever is committed by then and records any conflict with this plan.
5. **The comment reduction the owner asks for conflicts with STYLE S11 as ratified.** S11 says each declaration in a public header has a `///` comment that states what the function requires, guarantees and preserves. Moving the specification to the human documentation departs from that sentence. Only the owner amends STYLE (owner item O1; plan 0049 lists the same question as its O3). Until ruled, the comment units do only what both readings permit (W8): prologues, narration, duplicates, and comments on `detail` entities.
6. **One proposal conflicts with plan 0049 as written.** Plan 0049 section 7 item 4 relies on every message literal staying unchanged. Section 5.2 of this plan offers the owner an option that changes about 50 literals. If the owner takes it, the ruling must come before plan 0049's unit U10 writes the diagnostics page (owner item O2).
7. **Tests of this plan's own.** The brief says behavior is proven by the existing tests plus the tests plan 0047 adds first. Plan 0047's safety net has no test that fixes the arithmetic an operation performs, term by term and in order. This plan adds one (unit R0, section 6.3) before any header changes. Reading followed: a plan may add tests that judge its own later changes if they are committed first, apart from the code, and pass on the unchanged tree (W9).
8. **Baselines.** The brief says about 8,400 lines in `include/galoot/core/`. Measured: 8,410 lines in `include/galoot/` (the core directory 8,036, `core.hpp` 21, the rga3 preset 353). The plan covers all of it, since the preset's comments have the same shape.
9. **How the probes ran.** `/usr/bin/time` is not installed, so the probes read peak memory from `getrusage` and instructions from `perf stat`. Unpinned instruction counts varied by up to 25% between identical compiles (the process moved between core kinds), so only the rows pinned to one performance core are used for instruction counts. The probes ran outside `tools/timed`, so they have no row in `runs.tsv`. Shared mode: other sessions were working (load average 1.3 to 4.0).
10. **The integration branch.** The governance says `develop`; the branch is `main` (plan 0045 section 6 treats the rename). This plan says `main`.
11. **Checks of this commit.** Records only (P8: documentation, no tests). No build, test or lint run was made for the plan itself.

## 2. Goal and definition of done

Make the core smaller to read, verify and change (C12), without changing what any public operation computes, any public name, or the code a compiler generates for an operation, and without raising what a user's build costs. Done when:

1. Every candidate of section 5 is either merged with its evidence, or closed with the measurement or ruling that rejected it.
2. The public interface manifest of plan 0047 (unit V2) is unchanged from its state at merge M1 of plan 0047, or each difference is listed with a decision.
3. The expression snapshot (section 6.3) is byte-identical before and after every code unit on every instantiation in its catalogue, on both compilers.
4. Every test of the suite at M1 passes unmodified, except tests labelled `internal` that a recorded decision retires together with the entity they named, each with the public-level test that covers the behavior.
5. On the five heaviest translation units of gcc-debug and clang-debug and on clang-measure's `identities_rounding_test.cpp`, compile instructions and peak memory after the last code unit are not above their values at M1 by more than the criterion of section 6.6, or the rise is reported with its cause and accepted by the owner.
6. `rga3.kernels_inline`, the counting-scalar operation counts and the benchmark protocol of plan 0047 unit V7b show no change beyond the stated variation.
7. Comments state why (S11's first paragraph); no comment restates the code beside it; every sentence removed from a header because it was specification is present in the guide at the heading the header points to (plan 0049 criterion A11), or was a duplicate.
8. The before-and-after table of section 3's measures is recorded, with the public-name and declaration counts.
9. The full P9 matrix passes at the final merge; an auditor has checked the branch history against this plan; the outcome is recorded here.

## 3. Survey of the tree at `c95fee5`

Scripts and raw outputs: `records/performance/simplification-survey/`. Toolchain: GCC 16.2.1, Clang 23.1.4, lizard 1.26.1.

### 3.1 Size and comments

| Header | Lines | Code | Comment | Comment per code line | Prologue | `///` public | `///` detail | `//` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `assert.hpp` | 143 | 45 | 86 | 1.91 | 29 | 38 | 11 | 8 |
| `bits.hpp` | 186 | 65 | 98 | 1.51 | 18 | 67 | 7 | 6 |
| `metric.hpp` | 318 | 163 | 118 | 0.72 | 13 | 77 | 0 | 28 |
| `blade_set.hpp` | 645 | 405 | 185 | 0.46 | 22 | 101 | 0 | 62 |
| `blade_product.hpp` | 301 | 141 | 132 | 0.94 | 30 | 10 | 72 | 20 |
| `plan.hpp` | 949 | 529 | 366 | 0.69 | 63 | 0 | 236 | 67 |
| `mv.hpp` | 1,342 | 778 | 462 | 0.59 | 42 | 209 | 54 | 157 |
| `unary.hpp` | 132 | 48 | 68 | 1.42 | 27 | 35 | 0 | 6 |
| `product.hpp` | 457 | 274 | 151 | 0.55 | 47 | 88 | 7 | 9 |
| `linear.hpp` | 482 | 247 | 194 | 0.79 | 62 | 96 | 8 | 28 |
| `dual.hpp` | 679 | 367 | 265 | 0.72 | 88 | 141 | 7 | 29 |
| `transform.hpp` | 845 | 449 | 345 | 0.77 | 70 | 120 | 26 | 129 |
| `basis_change.hpp` | 1,285 | 764 | 438 | 0.57 | 112 | 162 | 15 | 149 |
| `format.hpp` | 272 | 160 | 90 | 0.56 | 24 | 43 | 0 | 23 |
| `core.hpp`, rga3 (4 files) | 374 | 126 | 210 | 1.67 | 55 | 151 | 0 | 4 |
| **Total** | **8,410** | **4,561** | **3,208** | **0.70** | **702** | **1,338** | **443** | **725** |

Findings on the comments:

- **No comment cites an agent record or a governance provision.** The pattern scan (decision, plan, WU, research note, lesson, spike, audit, review, provision numbers) hit 35 lines, all false: `NOLINT` markers with "cast", and `-march=x86-64-v3`. S11's citation rule already holds; `tests.citations` guards it.
- **702 lines are file prologues** (up to 112 lines in `basis_change.hpp`). They are tutorials with examples: the same `struct rga3 { ... rigid<3>() ... }` block is written out 9 times, the `Point` and `Line` aliases 3 times each. The guide teaches the same material from compiled examples; these copies are not compiled.
- **41 distinct sentences are repeated, 122 times in all** (about 11,300 characters). The most repeated after the example: the clang-tidy note on the C-style cast (8), "An int, so that a compiler prints its values and not a byte" (7), the paragraph that sends the reader to the limits page with both compilers' flags (9 in two wordings).
- **67 lines are `NOLINT` markers**, most of them `modernize-avoid-c-style-cast` on variable templates, each with a sentence explaining the same false report. One entry in `.clang-tidy` with the reason would replace them (owner item O6).
- **22 lines narrate process**: what "the structure test found", what "is tested up to", where sizes "measured on each compiler" are. These belong to the guide's limits page, which exists.
- **443 `///` lines document `detail` entities** with full Requires and Guarantees blocks (236 in `plan.hpp`). The owner's code style guide asks for that form on public interfaces only, and for private helpers only where the behavior is not obvious.
- **Comments cost nothing to compile.** Stripping all 3,100 non-`NOLINT` comment lines changed no wall time on three test sources on either compiler (`compile-probes.tsv`). The comment work is for the reader only.

### 3.2 Complexity per function

267 functions (lizard; function templates are counted once).

| Header | Functions | Highest cyclomatic complexity | Mean | Above 10 | Longer than 40 lines |
| --- | --- | --- | --- | --- | --- |
| `plan.hpp` | 23 | 17 (`map_defects_of`) | 4.78 | 3 | 2 |
| `product.hpp` | 17 | 16 (`reject_product`, 92 lines) | 2.12 | 1 | 1 |
| `blade_set.hpp` | 24 | 14 (`unite`) | 3.67 | 1 | 1 |
| `basis_change.hpp` | 41 | 13 (`eliminate`) | 3.51 | 1 | 4 |
| `mv.hpp` | 43 | 11 (`look_up_name`) | 2.63 | 1 | 0 |
| all others | 119 | 9 | 2.3 | 0 | 2 |

Seven functions exceed 10. Three are the generator's self-checks (`map_defects_of` 17, `typed_defects_of` 16, `defects_of` 13), one is a chain of sixteen literal messages (`reject_product`), and three are real algorithms (`unite`, `eliminate`, `look_up_name`). The core is not complex function by function. Its weight is in repetition.

### 3.3 Duplication

Token windows over the code with comments removed (`duplication.txt`):

| Measure | Whole tree | Worst headers |
| --- | --- | --- |
| Exact repeats, string literals masked, 40 tokens | 12.6% of 40,192 tokens | `product.hpp` 43.7%, `linear.hpp` 38.1% |
| Repeats with identifiers normalized, 60 tokens | 25.6% | `product.hpp` 97.0%, `unary.hpp` 63.0%, `dual.hpp` 62.0%, `linear.hpp` 25.3%, `basis_change.hpp` 23.8%, `mv.hpp` 23.2% |

Where it comes from, by reading:

1. **Rejecting overloads, one literal per spelling.** A `static_assert` message must be a string literal in C++23, and the messages name the operation as the user wrote it ("a * b: ..."). So the two sentences "the operands are in different algebras; give both types the same algebra tag" and "the operands have different scalar types; give both types the same scalar type" are each written 16 times, in four functions (`reject_product`, `reject_dual_operands`, `reject_transform_operands`, `reject_sum`) with four spelling enumerations, reached from 16 catch-all overloads. About 330 code lines.
2. **The exactness check, repeated at every layer.** "the scalar type cannot hold one of the X's exact coefficients" has 19 variants. `operator*` checks and then calls `geometric_product`, which checks and calls `detail::apply`, which checks: three checks so that the one error carries the outermost name. Each of the ten functions of `dual.hpp` is twelve lines of which one is the operation. About 190 code lines.
3. **Four term types and three table types for one idea.** `plan_term`, `typed_term`, `map_term`, `kernel_term`; `typed_plan`, `typed_map`, `change_map`. A map term is a typed term without its second index. Each has its own builder: `make_product_kernel`, `make_map_kernel`, `make_identity_kernel`, `make_second_operand_kernel`, `make_scalar_part_kernel`, `make_first_kernel`, `make_second_kernel`, `typed_change`; and its own runner: `run_product`, `run_map`, `run_sum`, `run_change`.
4. **Count, then fill.** Six kernels are built by one function that counts and a second that repeats the same loop to fill (`kept_count` and `make_identity_kernel`; `scalar_term_count`; `second_term_count`; `first_term_count`; `dropped_blade_count` and `make_dropped_places`). `plan.hpp` already has the single-function form: `generate_plan` writes while there is room and returns the count.
5. **Twins.** `truncate` and `part` (three overloads each, bodies equal but for one check and the word in the message); `change_basis` and `change_basis_back` (two bodies and six rejecting overloads, equal but for the direction); `multiply_checked` and `add_checked` in `blade_product.hpp` against `multiply_noting` and `add_noting` in `basis_change.hpp` (the same overflow tests, one asserting and one recording); the three self-check functions of `plan.hpp`, which share four of their fields and both of their loops.

### 3.4 Diagnostics, overloads, macros, template depth

- **193 messages** (`static_assert` and `GALOOT_ASSERT`), 179 distinct, **133 distinct once the prefix before the first colon is dropped**. `tests.messages_pinned` requires a compile-fail test for each, or an entry in `test/internal_messages.txt` (20 entries, 17 of them the generator's self-checks). The compile-fail suite is 320 sources in `test/core/` and 10 in the preset's tests.
- **About 42 public overloads exist only to reject**: 16 for mismatched operands, 7 in `linear.hpp` for numbers, 6 in `basis_change.hpp`, 5 constructors of `mv`, 2 for `truncate` and `part`, 1 for `get`, 5 in the preset. They are the mechanism STYLE S7 prescribes, and the reason the public declaration count (263 by the draft note of section 3.6) is 2.7 times the name count (97).
- **Macros: 9 beside the include guards**, all in `assert.hpp`: three public (`GALOOT_CHECKING`, `GALOOT_FORCE_INLINE`, `GALOOT_ASSERT`), `GALOOT_ALWAYS_INLINE`, and four helpers that plan 0044 undefines. Nothing to gain here.
- **Template depth: 16 suffices** for `transform_test.cpp` on GCC (12 does not). There is no recursive instantiation to remove; S7 is followed.
- **Tests name internals.** 53 test files name `detail::` entities, about 215 times in the six files that do it most (`mv_test.cpp` 50, `transform_rounding_test.cpp` 45, `plan_test.cpp` 34, `blade_product_test.cpp` 30). The names used most: `BinaryOperation` 27, `blade_product` 21, `product_plan` 18, `transform_outer` 16, `transform_middle` 15, `UnaryOperation` 11, `term_list` 10, `unary_map` 8, `apply` 7. Any consolidation that renames these touches tests, which W9 makes a separate, decided change. Section 6.2 fixes which internal names stay.

### 3.5 Compile cost

From `compile.tsv` at the release revision (rows of `7276f82`):

| Tree | Units | Sum of wall time | Heaviest five (wall, peak memory) |
| --- | --- | --- | --- |
| gcc-debug | 140 | 786 s | `transform_identities_test.cpp` 92.3 s, 5.93 GiB; `identities_four_test.cpp` 88.6 s, 4.83 GiB; `identities_five_diagonal_test.cpp` 59.1 s, 3.00 GiB; `identities_rounding_test.cpp` 47.5 s, 2.68 GiB; `identities_five_drawn_test.cpp` 43.9 s, 2.17 GiB |
| clang-debug | 142 | 381 s | `transform_identities_test.cpp` 32.8 s, 1.51 GiB; `identities_four_test.cpp` 29.0 s, 1.49 GiB; `identities_five_diagonal_test.cpp` 21.5 s, 1.02 GiB; `dual_identities_test.cpp` 20.4 s, 0.52 GiB; `identities_five_drawn_test.cpp` 17.5 s, 0.70 GiB |
| clang-measure | (release record) | 667 s | `identities_rounding_test.cpp` 602.5 s, 1.81 GiB |

Probes made for this plan (debug flags of the presets, `-O0 -g`, checking on):

| Question | Result |
| --- | --- |
| What do the generator's self-checks cost? | `product_test.cpp`, pinned, two runs each: GCC 57.60 to 56.71 G instructions (1.5%), Clang 35.56 to 34.72 G (2.4%). Wall time within 0.3 s on all three sources. One GCC unit (`dual_identities_test.cpp`) dropped from 1.01 to 0.87 GiB. |
| What do comments cost? | Nothing measurable. |
| What does forced inlining cost? | `product_test.cpp` on GCC: 4.6 s and 0.80 GiB with it, 2.2 s and 0.40 GiB without (31.5 G against 57.6 G instructions). It is the largest single compile cost, and it is ruled (decisions 0244, 0245). |
| Where does Clang spend `transform_test.cpp` (8.6 s)? | Front end 6.9 s, of which instantiating `transform` 4.5 s (90 instantiations) and `make_product_plan` 3.2 s (129). Inside the plan: the counting pass 1.04 s and the filling pass 1.07 s (119 and 118 evaluations). Back end 1.7 s. |

Reading: the generator's cost is the plans, each generated twice (once to count, once to fill), and the nesting of forced-inline layers above them. A simplification that halves the layers between a public operation and its kernel can lower compile cost; one that adds instantiations per operation can raise it. Every code unit measures.

### 3.6 Interface size

The draft knowledge-base note `complexity-reduction/deep-modules-and-interface-size.md` (uncommitted when read; section 1, item 4) measured the same tree: 97 distinct public names and 263 public declarations, 23 of the names being `_layout` companions. Its point bears on this plan: collapsing implementation code leaves the interface the same size, so the module becomes cheaper to maintain but no easier to learn. This plan is an implementation simplification by that measure. It records both counts before and after (definition of done, item 8) and treats a rise in either as a defect.

## 4. Classes of simplification

| Class | Meaning here | Proof it needs |
| --- | --- | --- |
| (a) Deletion | Code no caller reaches | The build and the suite; the manifest unchanged |
| (b) Consolidation | Two or more forms of one thing become one form, with the same observable behavior | (a), the expression snapshot identical, compile cost not above the criterion, diagnostics identical unless the unit says otherwise |
| (c) Compositional abstraction | Operations are built from a few typed primitives at compile time, instead of being nested at run time | (b), plus object-code comparison and the benchmarks, plus a spike first |
| (d) Comment and in-code documentation reduction | Why-only comments; the specification lives in the guide | The token stream of the code unchanged; plan 0049's received-rows check |
| (e) Not to be simplified | Carries numerical, compile-cost or diagnostic value | Listed in section 5.5, each with its reason, so that no unit removes it by accident |

## 5. Ranked candidates

Code-line savings are estimates from reading (section 1, item 2). "Risk" is the chance that the unit changes behavior or cost in a way its proofs must catch. Rank is saving against risk and against how many later units the candidate makes easier.

### 5.1 Consolidation and deletion that change no diagnostic text

| Rank | ID | Class | Candidate | Headers | Code lines saved | Risk | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | B1 | (b) | One table type and one term type: a unary map is a plan whose terms leave the second index unused. Removes `map_term`, `typed_map`, `typed_map_of`, `filled_map`, and makes `make_map_kernel` the product builder's twin no longer | `plan.hpp`, `mv.hpp` | 60 | Medium: tests name `unary_map` and `map_term` (10 uses); constant-evaluation memory per map rises by one index per term | Opens B2 and C1 |
| 2 | B2 | (b) | One kernel builder: reindex a table onto a target layout with its orientation signs, then mark first terms. Replaces the position-and-sign loop written in five builders | `mv.hpp`, `linear.hpp`, `dual.hpp`, `basis_change.hpp` | 60 | Low: pure compile-time data; the snapshot sees any change | |
| 3 | B3 | (b) | One count-then-fill helper in the form `generate_plan` already has (write while there is room, return the count). Removes five counting functions | `mv.hpp`, `dual.hpp`, `transform.hpp` | 45 | Medium for compile cost: the helper must not add an instantiation per kernel | Measure on `transform_test.cpp` |
| 4 | B4 | (b) | `truncate` and `part` share one body and one rejecting overload; `change_basis` and `change_basis_back` share one body parameterized by direction, as `change_map_of<C, Back, S>` already is | `mv.hpp`, `basis_change.hpp` | 75 | Low. The six rejecting overloads of the change stay as declarations; only their bodies merge where the literals allow | |
| 5 | B5 | (b) | One overflow-checked integer arithmetic: the recording form, with the asserting form as a two-line wrapper | `blade_product.hpp`, `basis_change.hpp` | 25 | Low; integer edges are plan 0047's H2 | |
| 6 | B6 | (b) | One self-check structure for the three in `plan.hpp` (shared fields and loops written once) | `plan.hpp` | 40 | Low; the 17 internal messages stay | See (e) item 7 and O4 |
| 7 | B7 | (b) | The nine named booleans of `mv`'s scalar check become nine assertions on the fault value; the four coefficient-wise loops of `linear.hpp` become one helper | `mv.hpp`, `linear.hpp` | 30 | Low to medium: GCC prints the asserted expression, so the named boolean is part of what a user reads. Unit compares both compilers' full output before and after and keeps the names where the output gets worse | |
| 8 | B8 | (b) | One runner for a kernel of one or two operands (`run_map`, `run_product`, `run_sum`, `run_change`) | `mv.hpp`, `linear.hpp`, `basis_change.hpp` | 25 | High for its size: this is the code P4 is about. Only if object code is identical on both compilers; otherwise closed as rejected | Last of the consolidations |
| 9 | A1 | (a) | Dead code: `change_add` (`basis_change.hpp:304`, never called) and the `blade_image` overload taking a `basis_change` (`:794`, no caller in headers or tests) | `basis_change.hpp` | 13 | None | The two untargeted `apply` overloads are plan 0044's |
| 10 | B9 | (b) | `.clang-tidy` excludes `modernize-avoid-c-style-cast` once, with the reason; 67 marker lines and their repeated explanation go | 9 headers, `.clang-tidy` | 0 code, about 90 comment lines | Low; needs O6 | |

Subtotal: about 370 code lines (8%), no message changed, no public name changed.

### 5.2 Consolidation of the diagnostic families (owner item O2)

The two largest repeats of section 3.3 exist because each message names the operation as written, and the guide says so ("a message that names the product as written, such as `a * b`", `core.md:506`). Three ways forward:

| Option | What changes | Code lines saved | What it costs |
| --- | --- | --- | --- |
| A. Keep | Nothing | 0 | 520 lines stay that say two things 35 times |
| B. Same literals, stamped by a macro | Each rejecting overload and each exactness check becomes one line of a function-like macro that concatenates the spelling with the shared sentence; the macros are undefined at the end of the header | about 330 | Three or four function-like macros in public headers (Core Guidelines ES.31; the owner's style guide); every error gains "in expansion of macro" notes; `tools/catalogue` must expand macros to find the literals |
| C. One literal per fault, checked once | "the operands are in different algebras; ..." and "the operands have different scalar types; ..." written once; the exactness check made once, in the kernel, with one message; public functions return the kernel's result directly | about 470 | About 50 literals change, so 50 compile-fail expectations change (a W9 decision listing them); the message no longer names the spelling, which the compiler's own "required from here" line shows; the guide's sentence changes; plan 0049 section 7 item 4 must allow it; the release is 1.1.0, not a patch |

A C++26 computed `static_assert` message would give C's code with A's messages, but STYLE S2 keeps the baseline at C++23, and both compilers reject the feature under `-Wpedantic -Werror` in C++23 mode (knowledge base, `cpp-language/lang-cxx26-diagnostic-channels.md` and `api-design/library-diagnostics-in-cpp.md`).

Under B and C every compile-fail test stays; under C its expected literal changes. The draft note `complexity-reduction/deletion-first-simplification.md` reports a mutation sample in which ten mutants were killed only by tests of a rejection path and its message, tests that look redundant beside the unit tests; so no rejection test is retired here without that evidence.

Recommendation: C. From the user's side the lost prefix is the weakest part of the message: the fix ("give both types the same algebra tag") is the same for every spelling, and the user's own line is in the error. From the maintainer's side C removes the only functions in the core whose length is pure repetition, and it removes two of the three forced-inline layers above each kernel, which section 3.5 suggests is where compile cost sits. If not ruled, the default is A, the only option every reading permits.

### 5.3 Compositional abstraction (owner item O3)

**C1. A small algebra of kernels.** Today a derived operation composes run-time functions and relies on forced inlining to flatten them: `bulk(x)` is `left_complement(geometric_product(reverse(x), antiscalar))`, three instantiated operations, three kernels, three exactness checks. The antiproducts are already derived differently, inside `generate_plan`, by mapping blades through the complement before and after the base product (`factor_of`, `run_term`): composition at compile time, written by hand for that one case.

The proposal is to make that the general rule. Three primitives over one kernel type (a result layout, terms `out += c * x[a] * y[b]` with a unary kernel leaving `y` unused, and one sign per output applied after its sum):

- *before*: compose a kernel with a signed permutation of an operand's blades (a reverse, a complement). Folds into the coefficients.
- *after*: compose a signed permutation or a selection of blades with a kernel's result. A permutation moves outputs and sets the output sign; a selection drops terms.
- *bind*: fix one operand of a two-operand kernel to a constant, giving a one-operand kernel (the antiscalar in `bulk`).

Every operation of `dual.hpp` except the two projections is then one kernel: `dot = select(scalar) ∘ gp ∘ (id, reverse)`; `bulk = left_complement ∘ bind(gp, 𝟙) ∘ reverse`; `weight = left_complement ∘ bulk ∘ right_complement`; and so on. The projections and `transform` stay two kernels run in sequence, because they are cubic in one operand. `sum` is two one-operand kernels on one result; a change of basis is a one-operand kernel with denominators. C4 already says this: one mechanism, composed with exact per-basis data.

Why the output sign is kept apart instead of folded into the coefficients: `-(t1 + t2)` and `(-t1) + (-t2)` are the same number but can differ in the sign of a zero result (`-(1 + -1)` is `-0`, `(-1) + 1` is `+0`). Folding an operand's sign into a coefficient is exact in every bit under round to nearest, since `x * (-y)` is `-(x * y)` and `a + (-p)` is `a - p`; folding a result's sign is not. With the output sign kept, the composed kernel performs the same arithmetic as today's nested calls, in the same order. [derived, not yet checked: the spike checks it with the expression snapshot and with bit patterns over signed zeros]

Estimated saving: about 150 code lines beyond option C of section 5.2 (`dual.hpp` to about 150 from 367, the unary machinery of `plan.hpp` merged, `transform.hpp`'s two kernel builders written as selections), after adding about 80 for the primitives. Expected effect on cost: fewer instantiations per derived operation and less dependence on forced inlining; unknown until measured.

Risk: high. It rewrites how every derived operation reaches its arithmetic. It is admissible only as C3 allows a replacement: verified against the direct form, which stays in the tests as the reference, and justified by measurement. So it starts with a spike (unit C0) on `bulk`, `weight` and `dot` in rigid(3) and conformal(3), on a scratch branch, with a stated pass rule: expression snapshot identical; object code of the reference kernels identical or operation counts equal and benchmarks within variation; the five heaviest units not above the criterion. If the spike fails any of them, C1 closes as rejected and the record says why.

**C2. Derive the wedge from the geometric product's generator.** Rejected at planning: `pair_terms` computes a wedge in one step where the product's recursion would cost its full capacity, and the wedge is used to build change-of-basis images without a metric. The one-step form is the cheaper one by reading; no measurement was made for this plan.

### 5.4 Comments and in-code documentation (class d)

| ID | Candidate | Comment lines now | After, if S11 keeps the full contract (O1 a) | After, with one sentence and a pointer (O1 b) |
| --- | --- | --- | --- | --- |
| D1 | Prologues: two to five lines on what the header holds, and one pointer to the guide page. The examples go; the guide's are compiled | 702 | 90 | 90 |
| D2 | `///` on `detail` entities: a why-comment where the behavior is not obvious, nothing elsewhere | 443 | 110 | 110 |
| D3 | `//` inside functions: keep the equation applied, the invariant, the reason for a workaround; drop narration, repeats, and derivations that the concepts pages receive (the rounding bound, 117 lines in `transform.hpp`, keeps its statement and source) | 725 | 330 | 330 |
| D4 | `///` on public declarations | 1,338 | 1,090 (repeats and process narration removed, contract kept) | 340 (one sentence each, with the page and heading where the contract is) |
| | **Total** | **3,208** | **about 1,620 (-49%)** | **about 870 (-73%)** |

Rules for the comment units, taken from S11's first paragraph and the owner's code style guide:

1. A comment says why: intent, an invariant the types do not express, the equation or convention applied where it is applied, a unit, a published source, the reason for a workaround. If deleting it loses nothing a maintainer could not read from the next five lines, it goes.
2. No tutorial in a header. An example lives where it is compiled.
3. Nothing is said twice in one header, and nothing is said in two headers. A shared fact has one home and the others point to it or say nothing.
4. A `detail` entity gets a comment only when its behavior, its bound or its cost is not obvious.
5. A message literal is never touched by a comment unit.
6. A sentence that is specification moves only after plan 0049 has received it (plan 0049 section 7 item 3); at no revision of `main` is a fact in neither place.
7. Wording follows the code style guide: no narration, no step labels, no notes to the reader, no history.

### 5.5 What must not be simplified (class e)

Each item below looks like a candidate and is not. A unit that would touch one stops and reports.

1. **The first term of each output is assigned, and coefficients of +1 and -1 are folded** (`accumulate`, `mark_first_terms`). Adding to a zero changes the sign of a zero result and costs an operation; multiplying by ±1 costs one. Knowledge base, `cpp-language/lang-straight-line-kernels-from-term-tables.md`.
2. **The order of terms in every kernel.** It fixes the bits of a rounded result and the rounding counts the transform's check is built on (`kernel_roundings`). No unit reorders terms, whatever it saves.
3. **The two-stage sandwich with a shared middle, and the separate kernel for dropped blades.** It is how `transform` shares its first product between the result and the check, and the exact form of that check.
4. **Count, then fill, with a capacity per pair of blades.** A single pass into a buffer sized for the worst case would be simpler and would hold, for a full nine-generator layout, tens of millions of unused places during constant evaluation (inferred from the capacities; research notes 0008, 0011 and 0017 hold the measured limits). B3 changes how the two passes are written, not that there are two.
5. **One evaluation per table**: the `static constexpr` locals of the `make_` functions and the variable templates that hold them (P3, STYLE S7), `is_valid_form_v` and its single pass (plan 0014).
6. **Pack-expansion runners and `GALOOT_ALWAYS_INLINE`.** They are what makes an operation straight-line code (P4); the structure test guards them. Their compile cost is ruled.
7. **The generator's self-checks.** They cost 1.5% to 2.4% of compile instructions and no user reaches their messages, so they look removable. They are what certifies a plan for an algebra no test of the project has seen: a user's own signature is checked at the user's compile. C12 asks for the strongest evidence on the generative mechanism. B6 merges their structure and keeps every check (O4 offers the alternative).
8. **Literal messages and rejecting overloads as a mechanism** (C9, S7). Their number may fall only by the owner's ruling on O2.
9. **Checked integer arithmetic, the fraction-free elimination and the reduced rows** of `basis_change.hpp`. Exactness (C7) rests on them.
10. **The rounding check's arithmetic** (`padded_magnitude`, the margin, `margin_holds`). Its comments shrink; its code does not.
11. **The storage base of `mv` with one constructor parameter per component.** It is how a wrong count gives one error that prints both counts (S8).
12. **`metric::is_symmetric`, `entries_in_range` and the `_sign` functions of `bits.hpp`**, which duplicate internal checks but are public names with users.

### 5.6 Public names, and what 2.0.0 would mean

No candidate above removes, renames or changes the type of a public name, so all of it is 1.x by W13 and by the stability policy plan 0047 proposes (remove only in a major release, after a deprecation in a minor one). Options B and C of section 5.2 change message text, and plan 0047 counts a message as interface "only as far as the guide quotes" it; the guide quotes the idea of the prefix, so option C ships as 1.1.0 with the change in the release notes.

A public-name change would force: the manifest's change with a decision; `[[deprecated]]` for one minor release; a 2.0.0 that removes it; the guide, the conventions trace and every dependent (Garnet, per plan 0046 section 8) updated; and the release record's list of removed interfaces (W13).

Candidates a 2.0.0 could take, for the record: `sum_layout` and `basis_change_like` (public, no user; decision 0248 item 9 kept them); the thirteen layout names with no user outside their own header (decision 0187); `left_bulk_dual` and `left_weight_dual` with their layouts. Together they are about 25 declarations and 120 lines, mostly comments.

Recommendation: no 2.0 batch. The saving is small, the names cost a reader little, and a major version one release after 1.0.0 costs users more than these names do (owner item O5).

### 5.7 Expected totals

| Scenario | Code lines | Comment lines | All lines (blank lines scaled) | Change |
| --- | --- | --- | --- | --- |
| Now | 4,561 | 3,208 | 8,410 | |
| Minimum: section 5.1 only, O1 (a), O2 A, no C1 | about 4,190 | about 1,620 | about 6,400 | -24% |
| Middle: 5.1, O2 C, O1 (b) | about 3,720 | about 870 | about 5,050 | -40% |
| Recommended: 5.1, O2 C, O1 (b), C1 if its spike passes | about 3,570 | about 870 | about 4,900 | -42% |

Public names and declarations: unchanged in every scenario (97 and 263 by the draft note's count; unit D0 re-counts with plan 0047's manifest). Lines are a proxy. The measures that say whether a reader's load fell are in the definition of done: the duplicated-token share (25.6% now), the number of term and table types (7 now), the number of builders and runners (12 now), and the functions above complexity 10 (7 now).

## 6. How behavior is shown preserved

### 6.1 What plan 0047 provides, and what this plan asks of it

Plan 0047 section 9.2 sets the terms, and this plan accepts all seven: no file under `include/` changes before its merge M1; this plan gives its candidate list to unit H0 early (section 5 is that list; D0 hands it over with line ranges); tests attach at the public boundary where they can; each simplification keeps the manifest, the phase 1 tests, `rga3.kernels_inline` with V7a, the counting-scalar counts, and the mutation baseline; the direct form of anything rewritten stays in a test as reference; a public-name removal is this plan's owner item (O5); one plan's unit at a time per header.

### 6.2 Internal names that stay

So that consolidation does not turn into a rewrite of the tests, these `detail` names keep their meaning and their shape through every unit: `BinaryOperation`, `UnaryOperation`, `blade_product`, `term_list`, `term`, `product_plan<Op, Alg, A, B>` with `.result` and `.terms`, `unary_map<Op, Alg, S>` with `.result` and `.terms` (its term type may gain an unused second index), `kernel_term`, `transform_middle`, `transform_outer`, `first_kernel`, `second_kernel`. A unit that must change one of them says so in its plan entry, commits the test change apart (W9), and names the public-level test that covers the behavior. Everything else in `detail` may go.

### 6.3 The expression snapshot (unit R0)

A scalar type for tests that records, instead of computing: each value is the expression that produced it, built from named inputs by the operations the core applies to scalars (`+`, `-`, `*`, `/`, unary minus, construction from a coefficient, default construction, assignment). `test/support/counting.hpp` already counts these; the recording scalar keeps the tree. Printed in a canonical form, the result of `geometric_product(a, b)` over it is the exact sequence of arithmetic for each component, in order.

The snapshot is that text for a catalogue of instantiations, committed as a golden file and compared by a test on both compilers:

- every public operation on every pair of the rga3 preset's types and on its full 16-blade layout;
- the same over conformal(3) for the products, duals, `transform` and both directions of the change of basis;
- one non-diagonal, degenerate metric from the random-form tests, on mixed layouts with negative orientations;
- `truncate`, `part`, the explicit conversion, the sums, and the scalar operations.

Why this is the right instrument: the snapshot is taken at the public boundary, so it survives any change of internals; and two revisions with the same snapshot perform the same arithmetic in the same order on every input, so their results agree in every bit for every scalar type, rounding mode and contraction setting that the compiler applies alike to both. It turns "no behavior change" from a sample of inputs into an identity of programs. What it does not show: the code the compiler emits (section 6.4), constant-evaluation cost, and diagnostics.

R0 is committed before any header change and passes on the unchanged tree. Its mutants (W3): a reordered pair of terms in `generate_plan`; a first term added to zero instead of assigned; a coefficient of -1 applied as a multiplication; a result sign folded into coefficients. Each must fail the snapshot test and is recorded as killed or not by the suite without R0, so the record shows what R0 adds.

### 6.4 Object code

For the units that touch the runners or the layers above them (B8, the O2 units, C1): the disassembly of `test/bench/kernels_inline.cpp` at `-O2`, both compilers, forced inlining on, with addresses and symbol hashes normalized, is compared before and after. Identical text closes the question. A difference is read by the performance-engineer and accepted only with equal operation counts, the structure test passing, and the benchmarks of section 6.7.

### 6.5 Diagnostics

`tests.messages_pinned` and the compile-fail suite pass unmodified on every unit that claims no diagnostic change. In addition each such unit saves the complete compiler output of ten representative compile-fail sources before and after, on both compilers, and the reviewer reads the differences: the literal can be unchanged while the lines around it get worse (B7). A unit under O2 lists every changed literal, old and new, in its review request, and its review applies the usability checklist (W5).

### 6.6 Compile cost

Before the first code unit and after each: `compile.tsv` rows from fresh gcc-debug and clang-debug trees, and pinned single compiles (one performance core, timing lock held, three runs) of the five heaviest units of each tree and of `identities_rounding_test.cpp` under clang-measure's flags. Criterion, stated now (P7): pinned instruction counts repeated within 0.03% in this plan's probes, so a rise above 1% in instructions or 2% in peak memory on any of those units is a change beyond variation. It is reported with its cause, and the unit is merged only if the owner accepts the rise or a later unit of the same phase removes it. This is a reporting rule, not a bound; only the owner sets bounds (C14, O8). Builds that may pass 10 minutes or 10 GiB run under `pe-common/memguard.sh`.

### 6.7 Run cost

At the end of each code phase: the benchmark protocol plan 0047's V7b uses (E2, under the lock, pinned), compared with the run made at M1 by the criterion that protocol states. The counting-scalar operation counts (`op_counts_test.cpp`) are checked at every unit, since they are a test.

### 6.8 The public interface

Plan 0047's manifest test at every unit. At the end of each phase, the name and declaration counts, and a diff of the manifest against M1 attached to the phase's gate record even when empty.

### 6.9 Comment units

A comment unit changes no code token. Proof, mechanical: for each changed header, the token stream with comments removed is identical before and after (a script in the review record), and the preprocessed output of a unit that includes every header is identical apart from line markers. `tests.citations`, `tests.messages_pinned` and plan 0049's `tests.docs_header_pointers` pass. No build of the suite is needed beyond P8's selection in one configuration, because the compiled program is the same.

## 7. Units

Sizes: S under 100k tokens, M 100k to 250k, L 250k to 500k, without review. Each code unit is one header or one mechanism, small enough for one reviewer in one sitting (W2): the largest diffs expected are B1 and the O2 units, a few hundred lines each, mostly deletions.

| # | Unit | Class | Role, model | Size | Changes | Phase |
| --- | --- | --- | --- | --- | --- | --- |
| D0 | The comment table for plan 0049 (`records/docs/comment-moves.tsv`: header, lines at a named revision, descriptor, keep, summary, move or delete, target heading), the candidate list with line ranges for plan 0047's H0, the committed knowledge-base notes read, the baseline counts | (d) | hci-expert with cpp-expert, Opus high | L | `records/` | 0 |
| O | Owner rulings O1 to O9 recorded as one decision | | coordinator | S | a decision | 0 |
| R0 | The recording scalar, the snapshot catalogue, its test and mutants | | doctest-expert, Opus high | M | `test/support/`, `test/core/`, a golden file | 1, first |
| M0 | Baselines at M1: compile rows, pinned probes, object code, benchmark run | | performance-engineer, Opus high | M | `records/performance/` | 1 |
| A1 | Dead code | (a) | cpp-expert, Sonnet high | S | `basis_change.hpp` | 1 |
| B5 | One checked arithmetic | (b) | cpp-expert, Opus high | S | `blade_product.hpp`, `basis_change.hpp` | 1 |
| B1 | One table type | (b) | cpp-expert, Opus high | M | `plan.hpp`, `mv.hpp`, tests under 6.2 | 1 |
| B6 | One self-check structure | (b) | cpp-expert, Opus high | S | `plan.hpp` | 1 |
| B2 | One kernel builder | (b) | cpp-expert, Opus high | M | `mv.hpp`, then `linear.hpp`, `dual.hpp`, `basis_change.hpp` one commit each | 1 |
| B3 | Count-then-fill helper | (b) | performance-engineer, Opus high | M | `mv.hpp`, `dual.hpp`, `transform.hpp` | 1 |
| B4 | The twins | (b) | cpp-expert, Opus high | M | `mv.hpp`; `basis_change.hpp` | 1 |
| B7 | Scalar-check booleans, coefficient loops | (b) | hci-expert reads the output, cpp-expert writes, Opus high | S | `mv.hpp`, `linear.hpp` | 1 |
| B9 | The clang-tidy exclusion (needs O6) | (b) | build-expert, Sonnet high | S | `.clang-tidy`, 9 headers | 1 |
| B8 | One runner | (b) | performance-engineer, Opus high | M | `mv.hpp`, `linear.hpp`, `basis_change.hpp` | 1, last |
| G1 | Phase 1 gate: sections 6.3 to 6.8 on the branch tip, the matrix, the merge | | performance-engineer and auditor, Opus high | M | `records/gates/` | 1 |
| E1 | O2 as ruled, mismatch family: `product.hpp`, `dual.hpp`, `transform.hpp`, `linear.hpp`, one commit each, expectations in separate commits | (b) | cpp-expert with hci-expert, Opus high | L | 4 headers, compile-fail expectations, a decision | 2 |
| E2 | O2 as ruled, exactness family: the check made once | (b) | cpp-expert, Opus high | L | `mv.hpp`, `product.hpp`, `dual.hpp`, `transform.hpp`, `basis_change.hpp`, expectations, a decision | 2 |
| C0 | Spike of C1 on a scratch branch, with the pass rule of 5.3 | (c) | cpp-expert and performance-engineer, Opus high (Fable for the design under O9) | L | a spike record | 2, beside E1 |
| C1a to C1c | If C0 passes: the primitives with their tests; `dual.hpp` on them; `plan.hpp`'s unary path and `transform.hpp`'s builders on them | (c) | cpp-expert, Opus high | L, M, M | as named | 3 |
| G2 | Gate after phases 2 and 3 | | as G1 | M | `records/gates/` | 3 |
| K1 to K8 | Comment units, one per group: `assert` + `bits` + `metric`; `blade_set`; `blade_product` + `plan`; `mv`; `unary` + `product` + `linear`; `dual`; `transform`; `basis_change` + `format` + rga3 | (d) | hci-expert, Opus high; Sonnet high for the mechanical deletions (prologue examples, repeats) | M each | comments only | 4, each after its header's last code unit and after plan 0049 marks its rows received |
| G3 | Final gate: the full matrix from empty trees, the before-and-after table, the audit, the outcome, the text for the release record | | performance-engineer, auditor, coordinator | M | `records/`, this plan | 4 |

## 8. Order, branches, and merges

**Phase 0, now.** D0 and O change only `docs/agent/`. They run beside plan 0047's phases 0 and 1 and plan 0049's U1 to U3. Branch `records/simplification-survey`.

**Phase 1, after plan 0047's merge M1 is in `main`.** Feature branch `feature/core-simplification`, each unit a subfeature in its own worktree (`feature/core-simplification--one-table`, and so on). R0 and M0 first. Then bottom-up along the include order, so that a header is settled before the headers above it are opened: A1 and B5, B1, B6, B2, B3, B4, B7, B9, B8. Units on different headers may run at once (A1 with B1); two units never hold one header at the same time, and none holds a header that a fix unit H6 of plan 0047 has open (plan 0047 section 9.2 item 7). G1 merges the phase into `main`.

**Phase 2.** E1 and E2 as the owner ruled O2 (under option A both close at once). C0 runs beside them on a scratch branch that is never merged.

**Phase 3.** C1a to C1c if C0 passed and the owner said go (O3), then G2. Plan 0047's V6b (the mutation sweep on changed files) and V7b follow this phase, as that plan orders them.

**Phase 4.** The comment units, per header group, each when (1) no code unit remains for its headers and (2) plan 0049's U9 and U10 have received the rows that move. Deletions that move nothing (repeats, narration, prologue examples the guide already has, comments on `detail` entities) do not wait for plan 0049 and may run from the end of phase 1. G3 closes.

Why code before comments: a comment unit on code that a later unit deletes is wasted, and a code diff is easier to review when the comments around it have not moved. Why comments at all after the code: the code units delete about 15% of the comments with the code they describe.

Checks at each level (P8, P9):

| Point | Runs |
| --- | --- |
| Before each commit of a code unit | The header's tests and the tests of every component that includes it, in gcc-debug; R0's snapshot test; `tests.messages_pinned`; the manifest test |
| Merge of a subfeature into the feature branch | The same selection in gcc-debug, clang-debug and one sanitizer tree; clang-format and clang-tidy on the changed files; the unit's cost rows (section 6.6) |
| G1, G2: merge into `main` | The full suite on gcc-debug, clang-debug, gcc-asan; the full suite under clang-measure; the lint preset; the convention trace gate; `tools/runlog check`; sections 6.4, 6.6, 6.7, 6.8 |
| G3 | All eleven presets from empty trees; the benchmarks; the seed campaign once; the auditor's read |
| A comment unit | P8's selection in gcc-debug; section 6.9's two comparisons; the three text tests |

Every run goes through `tools/timed`. A unit reads `tools/jobpool status` before and after a heavy run, starts the pool with `tools/jobpool ensure` only when the status says "not running", never twice, and never stops it.

## 9. Review

- **Every code unit gets a blind review (W5)** by the `blind-reviewer` agent in a configuration different from the author's, given the diff, the unit's entry in this plan, the criteria of section 6 that apply, and the author's list of departures. Not given: the author's reasoning that the change is correct, or the snapshot's verdict before the reviewer has stated what the diff should and should not change. A fix after review gets a delta review.
- **Units that change what a user reads** (B7, E1, E2) add the usability checklist and an hci-expert read of the full compiler output on both compilers.
- **Units that can change generated code** (B8, E2, C1) add a performance-engineer read of the object-code comparison.
- **Comment units** are reviewed for the opposite failure: the reviewer gets the before and after of each header and is asked what a maintainer can no longer learn from the header that they could before, and whether the guide heading the pointer names says it. A quarter of the moved rows are reread against the original text, sampled by the reviewer (plan 0049's criterion A11).
- **C0's design** is reviewed before C1a starts by someone who did not write the spike, with the question "what does a maintainer hold in mind to change `bulk` now, against before", since C12 measures simplicity that way and a clever composition can lose by it (S16).
- **The auditor** checks at G3 that each merged unit had its tests-first commits, its separate expectation commits, its review record and its cost rows.

## 10. Risks

| Risk | What catches it | What then |
| --- | --- | --- |
| A consolidation changes the order of arithmetic | The expression snapshot | The unit is fixed or closed; class (e) items 1 and 2 are not negotiable |
| A generic helper adds instantiations and compile cost rises | Section 6.6 at each unit | Reported; the unit is reworked with fewer distinct types or closed |
| Clang or GCC stops inlining through a merged runner | Object-code comparison, `rga3.kernels_inline` | B8 closes as rejected; the four runners stay |
| A test that named an internal is "adapted" in a way that weakens it | W9: separate commit, listed for the reviewer; section 6.2's short list of names that stay | The reviewer rejects; the mutation baseline (plan 0047 V6b) shows any mutant that now survives |
| A fact disappears from header and guide both | Plan 0049's order rule and received-rows check; this plan's rule 6 | The comment unit waits |
| The composition of C1 is harder to hold in mind than the nesting it replaces | C0's design review | C1 closes; option C of section 5.2 already takes most of the lines |
| Three plans edit the same headers | One unit per header at a time; plan 0044 lands with 1.0.1 before phase 1; plan 0047's H6 and this plan's units take turns | The second merges `main` first |
| The abstraction outlives its users (C12: every layer has a present user) | Each helper introduced must have two callers in the unit that adds it | A helper with one caller is inlined back before review |

## 11. Agent cost estimate (W3)

Tokens, all agents, reviews included. Plan 0042 ran 15% over its estimate; the range allows for more.

| Part | Estimate | Range |
| --- | --- | --- |
| This planning unit (Fable) | reported by the planner at hand-back | |
| Phase 0: D0, O | 0.45 M | 0.3 to 0.6 M |
| R0, M0 | 0.55 M | 0.4 to 0.8 M |
| Phase 1 code units (ten), each with blind and delta review | 2.4 M | 1.8 to 3.2 M |
| G1 | 0.25 M | |
| E1, E2 with reviews (zero under option A) | 1.0 M | 0.7 to 1.4 M |
| C0 | 0.45 M | 0.3 to 0.6 M |
| C1a to C1c with reviews (zero if C0 fails) | 1.3 M | 0.9 to 1.8 M |
| G2 | 0.25 M | |
| Comment units K1 to K8 with reviews | 1.5 M | 1.1 to 2.0 M |
| G3 and the audit | 0.4 M | |
| **Total, recommended scenario** | **8.5 M** | **6 to 11 M** |
| Total, minimum scenario (5.1 and comments only) | 5.5 M | 4 to 7 M |

Machine cost: the matrix at each of three gates (the 1.0.0 matrix record gives its time; 667 s of each is clang-measure), the pinned probes (about 15 minutes per unit under the lock), and three benchmark runs. Recorded, not bounded.

## 12. Open items for the owner

Each has a default that applies if it is not ruled.

**O1. How much contract stays in a public `///` comment (STYLE S11; the same question as plan 0049's O3).**
(a) Keep S11 as it is: the full requires, guarantees and preserves text stays on each public declaration; only repeats and narration go. Comments fall by about half. A reader of a header needs nothing else; the text exists twice once plan 0049's reference is written, and the two can drift.
(b) Amend S11: one sentence per public declaration saying what it is, plus a pointer to the guide heading that holds the contract, checked by plan 0049's pointer test. Comments fall by about three quarters. A user reading only the header (in an editor's hover, or with the headers copied into a project) gets less; the contract has one home and is compiled where it has examples.
Recommendation: (b), with one exception: a precondition that is undefined behavior when broken in an unchecked build (the overflow note on `coefficient_type`, the division by zero of `operator/`) stays on the declaration in full. Default if not ruled: (a).

**O2. The diagnostic families (section 5.2).** A keep; B same literals stamped by macros; C one literal per fault, checked once. Tradeoffs are in the table there. Recommendation: C, ruled before plan 0049's U10. Default: A.

**O3. The kernel algebra (section 5.3).** (a) Spike, then decide on its record; (b) skip; (c) commit now. Recommendation: (a). The spike costs about 0.45 M tokens and answers the two unknowns (bit identity and compile cost) that reading cannot. Default: (b).

**O4. The generator's self-checks.** (a) Keep every check and merge their structure (B6, 40 lines); (b) move them to the tests, removing about 130 lines, 17 internal messages and 1.5% to 2.4% of compile instructions, at the price that a plan for a user's own algebra is no longer checked at the user's compile. Recommendation: (a); the saving is small and the check is the only evidence the core gives outside the project's tested algebras. Default: (a).

**O5. A 2.0 batch (section 5.6).** (a) None; (b) deprecate the listed names in 1.1.0 and remove them in 2.0.0. Recommendation: (a). Default: (a).

**O6. `.clang-tidy`: exclude `modernize-avoid-c-style-cast`** with its reason in the file, removing 67 marker lines. The check then no longer reports a real C-style cast in a header; clang-format and review remain, and the core has none. Alternative: keep the markers and delete only their repeated explanations (about 20 lines). Recommendation: exclude, after B9 confirms every present report is the false one. Default: keep the markers.

**O7. The version that carries this work.** 1.1.0 if any message text changes (O2 B or C) or S11 is amended; otherwise it can ride a patch release. Recommendation: one 1.1.0 after G3, not a release per phase, so that users see one change of diagnostics. Default: no release is planned here; plan 0045's successor decides.

**O8. The compile-cost criterion (section 6.6).** (a) Report-and-accept at 1% instructions and 2% peak memory on the named units; (b) track only. Recommendation: (a); "simpler" must not quietly cost every user build time (C14), and the rule sets no bound. Default: (b), as PERFORMANCE stands.

**O9. Fable for C0's design.** The composition rules and the zero-sign argument are the one place in this plan where a wrong design costs every later unit. Alternative: Opus high with a second specialist's read. Recommendation: Fable for the design note only (about 0.15 M tokens), Opus for the spike's code. Default: Opus.

## 13. Next step

The coordinator merges this plan into `main` (the claim), puts O1 to O9 to the owner together with plan 0047's and plan 0049's items, since O1 and O2 touch all three, and starts D0, whose first hand-overs are the candidate list to plan 0047's H0 and the comment table to plan 0049.

## 14. Outcome

Not started.

- 2026-11-21: D0, first part, recorded in [records/simplification/](../records/simplification/README.md): the candidate sites by unit (`candidates.tsv`), the baselines with the commands that rerun them, the knowledge-base notes read against this plan, and this section 5.7 restated for the owner (`restated-targets.md`); the comment table for plan 0049 is D0's second part.
