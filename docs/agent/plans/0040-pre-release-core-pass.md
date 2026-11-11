# 0040 Pre-release core pass: forced inlining of the kernel chain, `inverse_change` deleted, the deleted-`==` hint, the two `TODO`s, and the limit catalogues out of the header comments

- Status: proposed 2026-11-10 by Claude (agent, Opus 5.5), planner in the cpp-expert and performance-engineer roles. From `develop` at `b400fac` (no file under `include/`, `test/`, `tools/` or `cmake/` has changed since `77cd947`, checked with `git diff`, so every line number below holds at both). Committed on `plan/pre-release-core-pass`; the coordinator merges it into `develop`, which is the claim (W7). No implementation has started.
- Source of scope: decision 0240 items 1, 2 and 9 (the pass before the M7 gate; one macro on the whole kernel chain, a structural CTest on E1's probe, W10 records on debug and release trees, no knob unless the debug-build cost is notable; the compiler-version limit catalogues out of the header comments, one sentence left behind) and item 8 (review proportion); decision 0238 items 1 and 3 (the inlining unit, amended in its order by 0240 item 1; the vendor attribute); decision 0236 items 1 and 3 (`detail::inverse_change` deleted, the test reading the checked inverse; the deleted `==` with a comment naming `same_rows`, with its own core review); decision 0162 item 1 (the two `TODO`s, deferred then); decision 0183 (comment-only changes need no delta review); the advisory report `records/advice/2026-11-10-fable-path-to-1-0.md` sections 2 (R1, R2, R8, R9), 4, 6 and 10; plan 0038 (E1, E2, G and H, which rerun after this unit); plan 0041 (the documentation lane, which owns `docs/guide/limits.md` and fixed its headings in its section 5.3).
- Branches (W7), each in its own worktree under `.claude/worktrees/`:
  - `feature/pre-release-core-pass`, worktree `pre-release-core-pass`, from `develop` after this plan merges. Only this branch merges into `develop`.
  - `feature/pre-release-core-pass--code`, worktree `pre-release-core-pass--code`: parts A and B (implementer 1).
  - `feature/pre-release-core-pass--comments`, worktree `pre-release-core-pass--comments`: parts C, D and E (implementer 2).
  - No `--tests` branch (section 7).
  - The review record on `review/pre-release-core-pass`, from the revision it reviews, cherry-picked with `-x`.
- Record locations, free on every branch at this commit: `records/mutations/0040-pre-release-core-pass/`; `records/performance/pre-release-core-pass/` (the W10 campaign in `cost/`, the removed limit text in `limits-removed.md`, the probe's object sizes); `records/diagnostics/pre-release-core-pass-equality.md`; `records/numerical/0040-contraction-check.md` (part E); `records/reviews/<date>-pre-release-core-pass-review.md`. Decision numbers: one, for the implementers' choices (section 15), allotted by the coordinator.
- Shared resources: the job pool is read only (`tools/jobpool status` before and after every heavy run; `eval "$(tools/jobpool env)"` before builds; started only when it reads "not running", never stopped). The W10 campaign takes the timing lock with `bash /home/brianm/projects/utils/math-core/math-tools/exercises/performance/pe-common/lock.sh take` and releases it with `... release` (read `free`, queue empty, at planning time); no hand-written lock loop. No processor configuration is touched or proposed.

## 1. Departures, ambiguities, and readings (C20)

1. **Thirteen functions, not twelve.** The advisory report's section 6 lists "twelve functions" and then names thirteen: `run_map`, `run_product`, `run_sum`, `run_change`, four `apply` overloads, `detail::sum`, `transform_middle`, `transform_outer`, `transform`, `compose`. All thirteen are checked against the headers in section 3.1; the plan attributes all thirteen.
2. **Two of the four `apply` overloads have no caller in the library.** The overloads that take no target layout (`mv.hpp:940-945`, binary; `mv.hpp:975-979`, unary) are called only by tests (`mv_test.cpp` and four compile-fail sources); every library caller (`product.hpp`, `unary.hpp`, `transform.hpp`) names the target. They are test-only code in a header, the kind decision 0236 item 1 removes from `basis_change.hpp`. Decision 0240 item 2 names all four, so all four get the attribute (it costs nothing); deleting the two is owner item O3, not done here.
3. **Callers of the runners outside the list.** `dot` (`dual.hpp:549`) calls `run_product` directly; `truncate` and `part` (`mv.hpp:1055`, `1116`) and the conversion constructor (`mv.hpp:618-624`) call `run_map`; `change_basis` and `change_basis_back` (`basis_change.hpp:1182`, `1219`) call `run_change`. The report says the one-line wrappers stayed inline in every variant, but its probe reached only the conversion constructor and the product wrappers. Reading: the structure test's probe reaches each of them (section 7.1); any the test shows out of line gets the attribute, recorded as a choice under "the whole kernel chain" (0240 item 2). None is attributed in advance (C12).
4. **The probe is copied, extended, into `test/`.** The brief points at E1's probe in `records/performance/wu17-asm/`; a test may not depend on a record (S11; records are not code). The test's probe is E1's seven functions verbatim plus four that reach the chain members E1's do not reach (section 7.1). E1's own probe stays in the record, unchanged, so plan 0038's E1 rerun compares like with like.
5. **The structure test's flags are fixed, and checking is off.** It compiles with the flags E1 used (those of an optimized, unchecked build: `-DGALOOT_CHECKING=0 -O2 -ffp-contract=off` and the S4 warnings), whatever the tree's preset, and runs with the tree's compiler, so the matrix covers both compilers. With checking on, `transform` calls its check helpers out of line by design; DoD-24 is about the code users build for speed. The checking path is not tested for calls.
6. **"Release trees" read as the measure trees.** `tools/compiles cost` takes the kinds `debug` and `measure` only. The measure presets are the release presets without `-D_GLIBCXX_ASSERTIONS` (both `-O2`, checking off). The W10 campaign uses `debug` and `measure`; the alternative, a `release` kind in `tools/compiles`, is a tools change outside this unit (coordinator item K7).
7. **Who writes `docs/guide/limits.md`.** The brief says this unit moves the removed text to the page. Plan 0041, already on `develop`, says the documentation lane owns the page and writes it from the headers at `77cd947`, while plan 0040 deletes the catalogues and leaves one sentence naming the page and one of its headings (0041 section 1 item 1, section 5.3). Reading followed: plan 0041's, so the two lanes share no file. This unit records every passage it removes, verbatim with its header and lines, in `records/performance/pre-release-core-pass/limits-removed.md`, which the documentation lane's diff check (0041 section 9, step 5) reads. If the coordinator rules the other way, part D also writes the page under 0041's headings (K1).
8. **`linear.hpp` has a catalogue too.** The brief names seven headers; `linear.hpp:58-63` and `104-111` hold the same kind of text (sums measured on Clang 23 and GCC 16 at the default limits). Decision 0240 item 9 names no header list, so part D trims eight. Plan 0041's headings have none for sums; the pointer names "Compile-time limits: blade sets", since a sum's limit is that of the merge of its layouts (K2).
9. **`cost.py comments` would go vacuous.** `docs/agent/snippets/plan_cost_model/cost.py comments` checks every count in `plan.hpp`'s `///` lines against the cost model's figures, and plan 0038's part G runs it. With the counts gone from `plan.hpp` it would pass on nothing. An author whose change breaks a check restores it (W9), so part D teaches it to read one section of a Markdown page, with a test first, and points it at the limits page (K3).
10. **The rounding sentences (R9).** The owner accepted all of the report's recommendations (0240); R9 asks for two sentences in the `transform` comment "in the pre-release pass", and its section 3 puts them in this lane, but 0240 item 1 does not name them. Reading: in scope, as part E, on its own commit, so the coordinator can strike it (K4). Plan 0041 section 4.6 will state on the limits page that "the bound holds with or without multiply-add contraction"; that claim has not been derived by anyone on the project, so part E has a numerical-expert derive it before either lane states it.
11. **Not moved: the conformal(3) chain discussion** in `transform`'s comment (`transform.hpp:745-757`, part of an 85-line comment). The report's section 4 table suggests moving it, but it states measured rates of a numerical effect, not compiler-version limits, so decision 0240 item 9 does not reach it. Left as it is (K11).
12. **The records' cross-check program** `records/cross-checks/0030-sage-change-of-basis/dump_changes.cpp` calls `detail::inverse_change`. Records are evidence for the revision they name (`37f1729`) and are not rewritten; it is left as it is.
13. **The first `TODO` asks a question an owner ruling answered once.** Decision 0162 item 1 deferred the catch-all "if either becomes a problem for a user"; the WU8b delta review (its D2) called it "a trade to decide before release". The plan resolves the `TODO` as a plain statement (part C) and puts the catch-all to the owner as O1. The second `TODO` concerns non-conforming code only and needs no ruling (section 3.4).
14. **Branch prefix.** `plan/<name>` is not a kind in W7's table; the nearest is `records/<name>`. The brief's name is used, as for plan 0041.
15. **`test/CMakeLists.txt` is shared with the documentation lane** (W7's "different files"). This unit edits only two regions of it: the `core.basis_change` compile-fail rows (lines 2346-2390) and the comment above `core.plan_cost_model` (lines 728-742). Plan 0041 edits three others (the README blocks and the two `bare_compile` blocks). The structure test is registered in `test/bench/CMakeLists.txt`, which no active unit touches (K5).
16. **Nothing was compiled or run in planning.** Every fact here is from reading the code and the records at `b400fac`; the report's experiment (its section 6) and E1 (`records/performance/wu17-asm/README.md`) are the only evidence about the compilers' inlining, and part A's first test commit is the first check of the extended probe.

## 2. Goal

Before E1, E2, G and H rerun and the M7 gate, five small changes to the core, in one unit:

1. Every function on the chain from a public operation to its generated kernel is forced inline, through one macro, so that the code of an operation is the straight-line arithmetic of its terms at its call site on both compilers (P4, DoD-24), and a test fails if a call into the library comes back.
2. `detail::inverse_change`, used only by a test, is deleted, and the test reads the inverse the change's checks compute.
3. `basis_change` gets a deleted `operator==` whose declaration line names `same_rows`, so `a == b` tells the user what to write.
4. The two `TODO` comments in `mv.hpp` become statements of what the code does and why.
5. The compiler-version limit catalogues leave the public header comments, one sentence each staying behind with the page and heading where the figures are.

## 3. What the code holds now (verified at `b400fac`)

### 3.1 The kernel chain

| # | Function | Place | Called by |
| --- | --- | --- | --- |
| 1 | `detail::run_map` | `mv.hpp:346-356` | unary `apply`, the conversion constructor, `truncate`, `part`, `run_change` |
| 2 | `detail::run_product` | `mv.hpp:358-369` | binary `apply`, `transform_middle`, `transform_outer`, `dot` |
| 3 | `detail::apply`, binary, target given | `mv.hpp:910-936` | `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge` (`product.hpp:241`, `269`, `372`, `403`), `compose` (`transform.hpp:677`) |
| 4 | `detail::apply`, binary, result layout | `mv.hpp:940-945` | tests only (section 1 item 2) |
| 5 | `detail::apply`, unary, target given | `mv.hpp:956-972` | `reverse`, `antireverse`, `grade_involution`, the complements (`unary.hpp:72-110`) |
| 6 | `detail::apply`, unary, result layout | `mv.hpp:975-979` | tests only |
| 7 | `detail::run_sum` | `linear.hpp:156-168` | `detail::sum` |
| 8 | `detail::sum` | `linear.hpp:176-187` | `+` and `-` (`linear.hpp:250`, `271`) |
| 9 | `detail::run_change` | `basis_change.hpp:1044-1051` | `change_basis`, `change_basis_back` |
| 10 | `detail::transform_middle` | `transform.hpp:384-391` | `transform` |
| 11 | `detail::transform_outer` | `transform.hpp:395-404` | `transform` |
| 12 | `compose` | `transform.hpp:659-683` | users |
| 13 | `transform` | `transform.hpp:769-818` | users |

`accumulate` (`mv.hpp:298-320`), `zero_places` (`331-335`) and `divide_by` (`basis_change.hpp:1031-1036`) are the leaves inside the runners' folds; at `-O2` E1 and the report found them inline. They are not attributed (C12); the test catches them if that changes. No function of the chain recurses (`apply`'s two-argument form calls the other overload), so GCC's refusal to inline a recursive `always_inline` function cannot arise. Every header on the chain already includes `assert.hpp`, where the macro goes. E1 found, on the committed headers, no call on GCC and on Clang a tail `jmp` to `transform` (point, line, plane), a call to `run_product<first_kernel>` inside the line's `transform`, and a call to `run_product<product_kernel>` for `compose`.

### 3.2 `detail::inverse_change`

`basis_change.hpp:532-553`: a `///` comment and the function, which eliminates the rows as written, asserts that the values fit and that the pivot is nonzero, and returns `inverse_rows`. Its one user in `include/` and `test/` is `basis_change_identities_test.cpp:223`, inside the test's own `check_change`, which runs at run time on values (a source and target metric and a change), not on tags. `detail::check_change(change, source, target)` (`basis_change.hpp:670-707`) takes the same values and returns `.fault` and `.inverse`, the inverse of the reduced rows, which plan 0039 section 5.3 shows is the same P⁻¹ in lowest terms with positive denominators: the same `change_rows<N>` value. `check_fits`, `eliminate` and `inverse_rows` keep other callers, so nothing else becomes dead. The `GALOOT_ASSERT` message in `inverse_change` repeats the `static_assert` message of `make_checked_change` word for word, so `tests.messages_pinned` loses a place, not a message.

### 3.3 Equality on `basis_change`

`basis_change<N>` (`basis_change.hpp:144-178`) declares no `operator==`; its `///` says "a basis_change has no ==, because two values for one matrix can differ", and `basis_change_test.cpp:187` asserts `!std::equality_comparable<core::basis_change<2>>`. A user's `a == b` gets the compilers' generic "no match" error, which does not name `same_rows` (plan 0039 O3). The value is used as a template argument (`takes_a_change<witt1_to_r110::value>`, `basis_change_test.cpp:163`); a deleted member function does not change whether a class is structural, an aggregate, or trivially copyable, and template-argument equivalence does not use `==`, so the suite confirms this rather than the change risking it. `mv.hpp:610-618` is the precedent for a comment at the end of a declaration's line, which both compilers echo in their notes (WU8b delta review, its S6).

### 3.4 The two `TODO`s

- **`mv.hpp:390-396`** (decision 0162 item 1, from the WU8b delta review's D2). A one-blade value built from a source that T is made from only explicitly (`Scalar<rational>{1}`, `Scalar<std::complex<float>>{std::complex<double>{}}`) reaches no constructor and gets the compiler's own error, 114 lines on GCC 16. A catch-all with a message would fix it and make `std::is_constructible` true for those sources, against three answers the tests pin (one is `mv_test.cpp:2535`). Every other catch-all of `mv` already makes that trade and says so ("Because this constructor exists, std::is_constructible_v<...> is true"). Needs: either a plain comment of the limitation, or the catch-all; the choice is O1.
- **`mv.hpp:725-728`** (the same ruling, the review's D3). A `std::numeric_limits` specialization that defines only some members is treated as no specialization, so its coefficients are not checked. The comment above it (`721-724`) already says so; the `TODO` adds only "revisit if a user reports it". A conforming specialization defines every member, so only non-conforming code is affected. Needs: the `TODO` deleted, and one sentence in `apply`'s public comment (`mv.hpp:880-883`, "A scalar without a specialization ... must hold the coefficients exactly without a check") saying that a specialization lacking a member it reads counts as none. No owner decision: the ruling it carries deferred a fix, and the statement changes no behavior.

### 3.5 The limit catalogues

| Header | Lines | What | Heading of `docs/guide/limits.md` (plan 0041 section 5.3) |
| --- | --- | --- | --- |
| `plan.hpp` | 618-667 (in `product_plan`'s `///`, 602-674) | the scalar times full<10> on GCC 16.2.1, the 89 million figure and its flag, Clang 23's steps, the counts for full<n> with full<n> per operation, the sparse sets from n = 4 to 16 | Compile-time limits: products and plans |
| `plan.hpp` | 974-981 (`unary_map`'s `///`) | the counts of the maps for full<n> | the same |
| `mv.hpp` | 488-493 (`mv`'s `///`) | the largest layout and product per compiler | the same |
| `mv.hpp` | 883-905 (binary `apply`'s `///`) | Clang's fold limit, the sizes per compiler, the flags, the conformal(3) exception | the same |
| `mv.hpp` | 951-954 (unary `apply`'s `///`) | the maps' sizes per compiler | the same |
| `product.hpp` | 47-52 (header comment), 340-344 and 346-360 (`geometric_antiproduct`), 418-419 (`%`) | the antiproduct's limits on Clang 23, the growth figures per compiler, the flag | the same |
| `linear.hpp` | 61-62 (header comment), 105-111 (`sum_layout`) | the largest sums per compiler and the failure texts | Compile-time limits: blade sets (section 1 item 8) |
| `blade_set.hpp` | 575-577, 586-588, 600-605 | `full`, `of_grade`, `merge` sizes per compiler | Compile-time limits: blade sets |
| `dual.hpp` | 78-91 (header comment) | the sizes per operation and compiler, the failure texts | Compile-time limits: duals, dot products and projections |
| `transform.hpp` | 70-82 (header comment) | the sizes per compiler, the failure texts | Compile-time limits: transform and compose |
| `basis_change.hpp` | 111-124 (header comment) | the sizes per change and compiler, the failure texts | Compile-time limits: change of basis |

Not catalogues, and kept: the comments that explain a workaround of a compiler defect or a design choice (`mv.hpp:322-330`, `380-386`, `404-405`; `plan.hpp:63-66`, `537`; `blade_set.hpp:366-368`; the "GCC can compile wrongly" lines in the runners), and every sentence that holds on any compiler (what the cost grows with, that the compilers' limits bound the sizes, the requirements and guarantees). The `core.plan_cost_model` comment in `test/CMakeLists.txt:728-742` says the tests pin "the costs that plan.hpp states"; it changes to name the limits page.

## 4. Definition of done

1. `GALOOT_ALWAYS_INLINE` is defined in `assert.hpp` (`[[gnu::always_inline]]` when `__GNUC__` or `__clang__` is defined, empty otherwise), with a `///` comment that says what it does, why the kernels use it, and that it is not part of the interface; it is on the thirteen functions of section 3.1 and on any other the structure test showed out of line (section 1 item 3).
2. `rga3.kernels_inline` passes on every preset; `tools.kernels_inline_*` pass; the mutants of section 8 have their declared verdicts.
3. `detail::inverse_change` is gone; `basis_change_identities_test.cpp` reads `detail::check_change(...).inverse` and requires `.fault` to be `ChangeFault::none`; `git grep inverse_change -- include test` is empty.
4. `basis_change` has `bool operator==(const basis_change&) const = delete;` with a `///` comment and an end-of-line comment that names `same_rows(a, b)`; its type comment says so; `core.basis_change_equality` and `core.basis_change_inequality` pass on both compilers; `!std::equality_comparable` still holds; the diagnostics record shows the output before and after.
5. `git grep -n TODO -- include/galoot` is empty; the two sites state what the code does (and, for the second, `apply`'s comment says what a partial specialization gets).
6. Each catalogue of section 3.5 is replaced by one sentence (what bounds the size, which flag raises the limit, and the page and heading where the measured sizes are); every removed passage is in `limits-removed.md` with its header and lines at the branch point; no comment cites a record, plan, decision or provision (S11, `tests.citations`).
7. `cost.py comments` reads a named section of a Markdown page, refuses a page without it, and its tests pass; the snippets index row and the `core.plan_cost_model` comment name the page.
8. If part E stands: the two sentences in `transform`'s comment, each supported by `records/numerical/0040-contraction-check.md` or worded to claim no more than it shows.
9. W10: before and after build cost (retired instructions, wall time, peak memory) on debug and measure trees of both compilers; the probe's object sizes; test times from the matrix rows; the knob criterion of section 9 read and reported.
10. One blind review; a delta review of any non-comment fix under `include/`; the P9 matrix from empty trees at the final tip; merge into `develop` with a P13 line; worktrees and branches removed once their content is on `develop`; the misuse catalogue's scan regenerated (its places move).

## 5. Acceptance criteria

1. **No call into the library.** `rga3.kernels_inline` compiles `test/bench/kernels_inline.cpp` at `-O2 -S` with the flags of section 1 item 5 and passes only if (a) the output defines every probe function, and (b) no `call` or `jmp` (in any suffixed form, such as `callq`, and through the PLT) targets a symbol whose mangled name is a name in namespace `galoot` (`_ZN`, then any of the qualifiers `K`, `V`, `r`, `R`, `O`, then `6galoot`; and the local-entity form `_ZZN...6galoot`). On failure it prints each offending line. It passes on gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint and clang-measure.
2. **The check can fail.** `tools.kernels_inline_rejects_call`, `_rejects_jmp`, `_rejects_plt`, `_rejects_const_member` and `_rejects_missing_function` feed the script fixture assembly and expect exit 1 with the offending line or the missing name; `tools.kernels_inline_accepts` feeds a clean fixture (with local `.L` jumps and a call to a non-galoot symbol) and expects exit 0.
3. **The test closes a gap.** With the macro defined empty, the suite without `rga3.kernels_inline` passes and the suite with it fails on Clang (section 8).
4. **Values unchanged.** Every existing test passes with no change to an expected value: the attribute changes where code is placed, not the arithmetic (`-ffp-contract=off`, no reassociation). A changed bit is a defect (W9), not an expected change.
5. **The inverse.** `core.basis_change_identities` passes reading the checked inverse, and the mutant `inverse_rows_sign` is killed with `core.basis_change_identities` among its killers.
6. **The hint.** `core.basis_change_equality` (`a == b`) and `core.basis_change_inequality` (`a != b`) fail to compile with one error, their output holding the end-of-line comment's literal (for example `compare with same_rows(a, b)`; the exact literal and each compiler's error text are fixed when the test is written and checked on both compilers), and no `detail::` on the first error line, as the `basis_change_rejects` rows require.
7. **The trims.** For each row of section 3.5, the header's diff removes only the measured, version-specific sentences and adds one sentence; the reviewer checks `limits-removed.md` against the diff line for line. `cost.py comments include/galoot/core/plan.hpp` with both figures files exits 0; `cost.py comments` on `limits-removed.md`'s plan section exits 0 (the counts removed are the figures); after the documentation lane's page is on `develop`, plan 0038's part G runs it on the page.
8. **Checks** of section 10 pass at the tip.

## 6. Parts, file ownership, and order

Two implementers, each in its own subfeature worktree. A file is owned by one part at a time; where two parts touch one file, the later starts only after the earlier is merged into the feature branch and the feature branch is merged into the later part's branch.

| Part | Implementer | Files (only these) | Starts |
| --- | --- | --- | --- |
| A, inlining | 1: cpp-expert and performance-engineer (Opus, high) | `assert.hpp`; the thirteen declarations of section 3.1 (one attribute line each) in `mv.hpp`, `linear.hpp`, `transform.hpp`, `basis_change.hpp`, and any the probe shows out of line; `test/bench/kernels_inline.cpp` (new); `test/tools/kernels_inline.py` and `test/tools/kernels_inline/` fixtures (new); `test/bench/CMakeLists.txt` (one new block) | after the claim |
| B, `basis_change` | 1, after A | `basis_change.hpp:144-178` and `532-553`; `test/core/basis_change_identities_test.cpp:219-225`; `test/core/basis_change_equality_fail.cpp`, `basis_change_inequality_fail.cpp` (new); `test/CMakeLists.txt` lines 2346-2390 (the `basis_change_rejects` rows) | after A |
| D1, trims in headers part A does not touch | 2: hci-expert, reading as a user, with the cpp-expert's care for what is version-specific (Opus, high) | `plan.hpp`, `product.hpp`, `dual.hpp`, `blade_set.hpp` (comments only); `docs/agent/snippets/plan_cost_model/cost.py`, `comments_test.py`; `records/performance/pre-release-core-pass/limits-removed.md` | after the claim, beside A |
| C, the `TODO`s | 2, after `--code` is merged into the feature branch and the feature branch into `--comments` | `mv.hpp:390-396`, `721-728`, `880-883` (comments only, under O1's default) | after A and B merge |
| D2, the other trims | 2, after C | `mv.hpp`, `linear.hpp`, `transform.hpp`, `basis_change.hpp` (comments only); `test/CMakeLists.txt` lines 728-742 (comment only); `docs/agent/snippets/README.md` (the cost-model row's description, not its "Last checked") | after C |
| E, rounding sentences | 2, after D2; the derivation by a numerical-expert (Opus), read-only, any time | `transform.hpp`, `transform`'s `///` (about lines 755-768) only; `records/numerical/0040-contraction-check.md` | after D2 |
| Evidence | 1 | `records/mutations/0040-pre-release-core-pass/`; `records/performance/pre-release-core-pass/cost/` and the object sizes; `records/diagnostics/pre-release-core-pass-equality.md`; the catalogue scan `records/diagnostics/catalogue/scan-output.md` | mutants with each part; W10 and the scan at the feature tip |

If the owner chooses O1 (b), the catch-all is code and its tests: part C moves to implementer 1 on `--code`, after B, tests first, and D2 waits for it.

Order:

0. Claim: this plan on `develop`.
1. `feature/pre-release-core-pass` from `develop`; `--code` and `--comments` from it.
2. In parallel: A then B on `--code`; D1 on `--comments`.
3. `--code` merges into the feature branch (P9 selection, section 10). `--comments` merges the feature branch in, then C, D2, E; then merges into the feature branch.
4. At the feature tip: the W10 campaign, the catalogue scan, the diagnostics record, the decision record.
5. Blind review; fixes; a delta review of any non-comment fix under `include/` (the W10 campaign reruns only if such a fix touches the chain).
6. Merge `develop` into the feature branch if it moved; the P9 matrix from empty trees; merge; clean up.
7. Hand-off: plan 0038's E1, E2, G and H at the closing revision (decision 0240 item 1), then the M7 gate.

## 7. Test plan (tests first, W2)

No separate test author (decision 0240 item 8): the structure test judges where code is placed, the hint tests judge a diagnostic, and the rerouted identities test adds no expected value and no new test (it reads the library's inverse from another function and keeps its own reference: the round trips and P G′ Pᵀ). The mutants of section 8 stand in for an independent author's evidence. If the coordinator reads the identities test as judging core arithmetic, its commit moves to `feature/pre-release-core-pass--code--tests` by a doctest-expert and is cherry-picked with `-x` (K8).

### 7.1 Part A

- **A1, test (labelled failing).** `test/bench/kernels_inline.cpp`: E1's seven functions, verbatim (`wedge_point_point`, `wedge_point_point_point`, `antiwedge_line_plane`, `transform_point`, `transform_line`, `transform_plane`, `compose_motors`), and four more, each one operation on the rga3 preset's `double` types: `sum_lines` (`l + m`: `detail::sum`, `run_sum`), `reverse_line` (`core::reverse(l)`: unary `apply`, `run_map`), `dot_points` (`core::dot(p, q)`: `run_product` from outside `apply`), and `change_point` (`core::change_basis<C>(p)` for a tag C on the preset's algebra written like `basis_change_test.cpp`'s `rga3_doubled`: `run_change`). Its comment says what it is for, in a user's terms, with no record cited. `test/tools/kernels_inline.py` (standard library, `python3 -I`, in the form of `test/tools/plan_cost_model.py`): runs the compile command it is given with `-S -o -`, applies criterion 1, and prints each finding. In `test/bench/CMakeLists.txt`: `rga3.kernels_inline` (labels `rga3;rga3.kernels_inline`), compiled with the tree's `CMAKE_CXX_COMPILER`, galoot::core's include directories and standard flag (as `galoot_compile_command` reads them), and the fixed flags of section 1 item 5, outside the compile-cost launcher (its output is assembly, as `core.plan_cost_model` compiles outside it); the probe also compiled once as an OBJECT library with the tree's own flags, so that the static checks and every preset's warnings see it, as the plan-cost units are; `tools.kernels_inline_*` of criterion 2 (labels `tools;tools.kernels_inline`). The commit message names the tests that fail and on which compiler, as observed on gcc-debug and clang-debug when the commit is made (expected from E1: `rga3.kernels_inline` on Clang; GCC not known for the four new functions).
- **A2, core.** The macro and the attributes. Every test passes. If the probe still shows a call, the function called gets the attribute in the same commit and the commit message names it (section 1 item 3); a call that the attribute cannot remove is a halt under W8, reported with the assembly.

### 7.2 Part B

- **B1, test (passes before and after).** In `basis_change_identities_test.cpp`'s `check_change`, `core::detail::check_change(change, source, target)` in place of `core::detail::inverse_change(change)`, with `REQUIRE(checked.fault == core::detail::ChangeFault::none)` before `.inverse` is read. Commit message lists it as a change to a test that judges this unit's change (W9).
- **B2, test (labelled failing).** `basis_change_equality_fail.cpp` and `basis_change_inequality_fail.cpp`, each one comparison of two `from_rows` values, and their `basis_change_rejects` rows with the literal of criterion 6. They fail before B3: the compile fails, but without the literal.
- **B3, core.** `inverse_change` and its comment deleted; the deleted `==` with its `///` (what it is and why, what to call instead) and the end-of-line comment; the type's `///` sentence "a basis_change has no ==" reworded to say its `==` is deleted and why. Every test passes.

### 7.3 Parts C, D, E

- **C, D2, E** change comments only in `include/`; they add no test (a comment has no behavior). D1's and D2's comment changes also need `tests.citations` and `format-check`.
- **D1, `cost.py` (test first).** `comments_test.py` gains fixtures, committed first and failing against the current `cost.py` (labelled): a Markdown page whose named section holds a figures count (passes); the same section with an unlisted count (fails, naming it); an unlisted count outside the section (passes); a page without the section (refused, so a renamed heading cannot make the check pass on nothing). Then `cost.py comments <page.md> --section <heading> <figures>...`, its docstring, and the `FIGURES` comment updated. The section name is agreed with the documentation lane (K3).

## 8. Mutants (W3)

Specs in `records/mutations/0040-pre-release-core-pass/`, run with `tools/mutate` on gcc-debug and clang-debug; header mutants are temporary edits in the worktree, never committed.

**The gap.** No test of the existing suite looks at where the library's code is placed (`rga3.benchmark_*_inline` checks only the reference kernels and the benchmarks' lambdas). The gap mutant `macro_empty` (`GALOOT_ALWAYS_INLINE` defined empty) runs twice at the tip: with the selection `^(core|rga3)` less `rga3.kernels_inline`, `expect = "survived"`; with `rga3.kernels_inline`, `expect = "killed"` on clang-debug (E1's calls come back). On gcc-debug the second run is a spec of its own, `expect = "survived"`, reason: GCC made no call in E1's seven functions without the attribute; a kill there (from one of the four added functions) is recorded as a finding, not a failure of the plan.

**Written for the new tests**, each counting when the test written for it is among those it fails:

| Mutant | Edit | Written for | Expected |
| --- | --- | --- | --- |
| `apply_not_forced` | no attribute on the binary `apply` with a target (`mv.hpp:912`) | `rga3.kernels_inline` | killed on both (the report's experiment, row 2: both compilers then call it from `compose_motors`) |
| `run_product_not_forced` | no attribute on `run_product` | `rga3.kernels_inline` | killed on Clang (E1) |
| `transform_not_forced` | no attribute on `transform` | `rga3.kernels_inline` | killed on Clang (E1) |
| `<function>_not_forced`, one for each of the other eight attributed functions that the probe reaches | its attribute removed | `rga3.kernels_inline` | survived, reason: the compilers inline it at `-O2` without the attribute, which then guards against a later inliner (decision 0240 item 2); a kill is recorded as evidence that it is needed now |
| `apply_result_layout_not_forced`, `unary_apply_result_layout_not_forced` | the attribute removed from overloads 4 and 6 | none | equivalent: no library function calls them, so no probe can reach them (section 1 item 2) |
| `check_ignores_jmp`, `check_ignores_plt`, `check_skips_missing` | the script's pattern loses `jmp`, loses the PLT form, or its missing-function check | `tools.kernels_inline_rejects_*` | killed |
| `inverse_rows_sign` | one entry of `inverse_rows` negated | the rerouted `core.basis_change_identities` | killed, `killers = ["core.basis_change_identities"]` (other tests may kill it too) |
| `equality_hint_removed` | the deleted `operator==` removed | `core.basis_change_equality`, `_inequality` | killed (the literal is missing) |
| `equality_defaulted` | `= default` in place of `= delete` | `basis_change_test.cpp:187`'s `static_assert`, the two compile-fail tests | killed |
| `section_ignored` (by hand, `python3 -I`) | `cost.py comments` reads the whole page | `comments_test.py`'s outside-the-section fixture | killed |

## 9. Resource records (W10, P1)

- **Campaign.** `tools/compiles cost records/performance/pre-release-core-pass/cost/units.toml records/performance/pre-release-core-pass/cost/out --before <feature branch point> --cpu 4 --repeats 3`, under the timing lock, at the feature tip after both subfeatures merge, with `tools/jobpool status` and the load average read before and after. Compilers gcc and clang; kinds `debug` and `measure` (section 1 item 6). Units, both trees: a text unit `#include <galoot/core.hpp>` (parse cost: the comments, the deletion and the macro, no instantiation); E1's probe as a text unit (the inlining alone); `test/core/transform_test.cpp`, `product_test.cpp`, `identities_test.cpp` (many call sites, the `-O0` risk), `basis_change_conformal_test.cpp` (`run_change`); `test/presets/rga3/chains_test.cpp`. Seven units, 168 compiles.
- **Code size.** The text size of E1's probe compiled at `-O0` and at `-O2` on each compiler, before and after (`size` on four objects per tree), recorded beside the campaign.
- **Test time.** The suites' times from the `gate:all` rows of the last matrix on `develop` before the branch and of this unit's final matrix; shared pool, recorded, not compared.
- **The knob criterion, fixed here before any run.** The debug-build cost is notable when, for any debug unit, the mean of retired instructions after exceeds the mean before by more than twice the larger of the two ranges of the repeats (decision 0005's form) and by more than 5%. If it is, the implementer adds no knob: the numbers go to the owner as O2. Otherwise no knob (0240 item 2).

## 10. Checks (P8, P9, P13)

| Point | Checks | Presets |
| --- | --- | --- |
| A1, A2 | `rga3.kernels_inline`, `^tools\.kernels_inline`; at A2 every test, since `assert.hpp` reaches every component | gcc-debug, clang-debug |
| B1 to B3 | `^core\.basis_change`, `core.headers`, `tests.messages_pinned` | gcc-debug, clang-debug |
| D1 | `comments_test.py`; `cost.py comments` on `plan.hpp` and on `limits-removed.md`; `tests.citations`; `format-check`; the tests of the four headers' components (comments only, so one configuration) | gcc-debug |
| C, D2, E | `tests.citations`, `format-check`, the tests of the touched headers' components | gcc-debug |
| Each subfeature into the feature branch | P8's selection for everything the part changed, in each configuration it can affect: for `--code` every test on all eight presets (a header every component includes); for `--comments` the components of the touched headers on gcc-debug and clang-debug | as stated |
| Before the merge into `develop` | the P9 matrix from empty trees (the eight presets), `format-check`, the trace gate, `tools/runlog check` | all, as `gate:<selection>` |

Every build and test through `tools/timed`, after `eval "$(tools/jobpool env)"`; a passing run is not repeated on an unchanged revision and configuration (P10).

## 11. Review plan (W5, decision 0240 item 8)

- **One blind review of the feature branch,** at the Opus level with high effort, by the `blind-reviewer` role, sharing no context with the implementers: given the diff from the branch point, sections 4 and 5, section 1 of this plan and the implementers' departures, `limits-removed.md`, and the records of sections 8 and 9, but not the authors' view that the change is correct. It applies the usability checklist (the deleted `==`'s diagnostic is a public interface, and the macro is a new public name), reads the attribute commit on its own, checks the probe and the script against criterion 1 by mutating them itself, and checks the trims against `limits-removed.md`. Each part is under about 200 lines of non-comment change, so there is no review per part.
- **A delta review** (Opus) of any fix to non-comment code under `include/` made after the blind review. A fix only to comments, tests, tools or records is listed in the review record with its resolution, with no delta review (decisions 0183, 0240 item 8).
- **Part E's derivation** is read by the blind reviewer; it is evidence for a header sentence, not a separate review.
- No auditor run here; the M7 gate's auditor reads this unit.

## 12. Evidence (V1, V10)

| Claim | Evidence | Kind |
| --- | --- | --- |
| The operations compile without a call into the library at `-O2` | `rga3.kernels_inline` on both compilers; plan 0038's E1 rerun | structure check (decision 0233 item 10's reading), not a cost assertion |
| The check can fail | the fixtures, the mutants of section 8 | V2 |
| The arithmetic did not change | the whole suite unchanged, its exact and bitwise tests included | V1 kinds 1 and 3, as they stand |
| The inverse is still judged | `core.basis_change_identities`, `inverse_rows_sign` | V1 kind 1 (round trips) |
| `a == b` names `same_rows` | the compile-fail tests, the diagnostics record | compile-fail |
| No limit figure is lost | `limits-removed.md`, the reviewer's check, plan 0041's diff check | records |
| Build cost before and after | section 9 | P1, track only |
| The contraction sentence (part E) | `records/numerical/0040-contraction-check.md` | V1 kind 1 (derivation), or the sentence claims less |

Validation (V8): not applicable; nothing here changes what the library computes. Verification on g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, the installed compilers.

## 13. Agent cost estimate (W3)

| Work | Model | Tokens |
| --- | --- | --- |
| This plan | Opus | not visible to me; about 250k, estimated |
| Implementer 1: parts A and B, mutants, W10 campaign, diagnostics and catalogue records | Opus, high | 300k to 450k |
| Implementer 2: parts C, D1, D2, E, `cost.py`, `limits-removed.md` | Opus, high | 150k to 250k |
| Part E's derivation | Opus, numerical-expert | 40k to 80k |
| Blind review | Opus, high | 200k to 300k |
| Subtotal: authors and one review | | 0.7M to 1.1M |
| With the history's multiplier of about 2 (fix passes, a delta review, reruns, merges, the decision record) | | about 1.4M to 2.2M |

Machine time, inferred: the W10 campaign about 45 to 70 minutes under the lock (168 pinned compiles, the identities unit the longest); mutants about 30 minutes of pool time (the probe compiles take seconds; part B's rebuild `core_basis_change_test`); the `--code` merge's eight-preset selection about 20 minutes; the final matrix from empty trees about 21 minutes. Plan 0038's E2 (about 22 minutes) and H (about 20 minutes) under the lock follow this unit and are not counted here.

## 14. Risks

1. **Debug-build cost and code size.** Every call site of a forced function gets its body, at `-O0` too; the identities tests make thousands of products. Mitigation: section 9's campaign and criterion; O2 if notable.
2. **The structure test is tied to the compilers' output format.** A future compiler may write calls differently, or split a probe function into a `.cold` part. Mitigation: the fixtures name each form the script reads; a toolchain change runs every test (P8), and a failure then is read as a toolchain change, not loosened (W9).
3. **GCC refusing an `always_inline` function** (an error, not a warning). None of the thirteen recurses or is called through a pointer in the library; the matrix shows any case on the sanitizer presets.
4. **A call the attribute does not remove,** for example one Clang keeps for a reason other than its inliner's budget. Then W8: halt and report with the assembly, rather than widen the list blindly.
5. **The deleted `==` and structural types:** confirmed by the tags already used as template arguments in the suite (section 3.3).
6. **A comment pointer to a page that does not exist yet.** If this unit merges before the documentation lane's page, `develop` has, for a while, header sentences naming a heading of a page not yet written. Mitigation: plan 0041's diff check at its later merge; or the coordinator orders the two merges (K10).
7. **A figure lost in the move.** Mitigation: `limits-removed.md`, the reviewer's line-for-line check, plan 0041's diff check.
8. **`cost.py comments` checking nothing.** Mitigation: the refusal of a page without the section; plan 0038's G runs it on the page.
9. **The contraction claim.** Stated on the limits page by plan 0041 and asked for in R9 without a derivation. Mitigation: part E derives it first; if the derivation fails or is not done, both the header and the page claim only what is shown, and the coordinator tells the documentation lane.
10. **Other work in flight:** the documentation lane (shared regions of `test/CMakeLists.txt`, section 1 item 15). No other core unit is active; plan 0038's remaining parts wait for this one.

## 15. Open items

### For the owner

- **O1. The one-blade catch-all of the first `TODO`** (section 3.4). Explanation: `Scalar<rational>{1}`, where the rational's constructor from an integer is explicit, gives the compiler's own error (114 lines on GCC 16, about 44 on Clang 23, as the WU8b delta review counted them). Options: (a) the `TODO` becomes a plain statement of the limitation and the catch-all waits until after 1.0, listed as known friction in the release record; (b) add the catch-all now (one constructor in `mv`, its compile-fail tests, three `std::is_constructible` answers changed by a recorded decision under W9). Tradeoff: (b) gives one short message, matches every other catch-all of `mv`, and is free of W13's notice before 1.0, but it is a late change to the most-used type's overload set, its tests judge the constructor set, and it moves part C to implementer 1 (about 60k to 100k tokens more, and one more delta-reviewable change); (a) keeps the pass small and the owner's 2026-11-02 ruling, and adding the catch-all after 1.0 changes only what a requires-expression is told about a use that does not compile. Recommendation: (a). Default if no ruling before part C: (a).
- **O2. A knob to turn the forced inlining off** (only if section 9's criterion finds the debug-build cost notable). Options then: (a) no knob, the cost recorded; (b) a documented macro that defines `GALOOT_ALWAYS_INLINE` empty. Tradeoff: (b) gives debug builds their old cost and step-into frames, and adds a public option to test in some preset; (a) keeps one configuration. Recommendation: on the numbers; default (a), as decision 0240 item 2 says.
- **O3. The two `apply` overloads with no library caller** (section 1 item 2). Options: (a) attribute them as ruled and list them for the M7 gate's audit, to delete after 1.0; (b) delete them now and rewrite their tests to name the result layout (`mv_test.cpp` and four compile-fail sources). Tradeoff: (b) applies decision 0236 item 1's reasoning (no test-only code in a header) consistently, and widens this unit by a rewrite of tests that judge core arithmetic, which would need a separate test author; (a) leaves two small `detail` overloads users never meet. Recommendation: (a).

### For the coordinator (reversible; not the owner's)

- **K1.** Who writes `limits.md`: plan 0041's reading (the documentation lane writes it; this unit trims and records), followed here, or the brief's (this unit writes it under 0041's headings). Recommendation: 0041's, so the lanes share no file.
- **K2.** `linear.hpp`'s pointer: "Compile-time limits: blade sets", or ask the documentation lane for a heading for sums. Recommendation: the former.
- **K3.** The section `cost.py comments` reads: ask the documentation lane for a level-3 heading under "Compile-time limits: products and plans" holding exactly the two count lists of `plan.hpp` (suggested: "Steps and operations of each plan"); otherwise the whole heading, with `NON_COST` gaining the sizes it also holds (2,048, about 950,000, 1200000), each with its source. Recommendation: the subsection.
- **K4.** Part E in or out (section 1 item 10). Recommendation: in, with the derivation; and tell the documentation lane that its contraction sentence waits for it.
- **K5.** The two shared regions of `test/CMakeLists.txt` (section 1 item 15): accept, as for plan 0041. Recommendation: accept.
- **K6.** The probe's four added functions (section 1 item 4). Recommendation: keep; they are what makes each attribute reachable by the test.
- **K7.** `measure` for "release" (section 1 item 6), or a `release` kind added to `tools/compiles` first. Recommendation: `measure`.
- **K8.** No separate test author for the rerouted identities test (section 7). Recommendation: accept.
- **K9.** The `plan/` branch prefix (section 1 item 14).
- **K10.** The order of this unit's merge and the documentation lane's page (risk 6). Recommendation: do not hold either; plan 0041's diff check closes the gap.
- **K11.** The conformal(3) chain discussion stays in `transform.hpp` (section 1 item 11). Recommendation: leave it; the documentation lane may restate it in the guide.
- **Forward to the keepers of the WU18 records:** `GALOOT_ALWAYS_INLINE` is a new macro to list in the DoD-2 inventory as not part of the interface; the release record's `same_rows` item now also says that `a == b` names `same_rows`; under O1 (a), the one-blade message as known friction; the DoD-24 item is rewritten from plan 0038's reruns, not from this unit.

## 16. Next step

The coordinator merges this plan into `develop` and rules K1 to K11 (the defaults above apply if not), allots the decision number, puts O1 and O3 to the owner (O2 only if section 9 triggers it), and launches implementer 1 (parts A, B) and implementer 2 (part D1) with briefs that carry the goal, the definition of done, the scope of section 6, and the governance (W14); that say that no web request carries the owner's email, that no third party is contacted, that the pool is read only, that the timing lock is taken through `lock.sh`, that comments and tests cite no record, plan, decision or provision (S11), and that MSVC is out of scope.

## 17. Outcome

Not yet recorded: the merge gate writes it when this unit merges into `develop`.

### Deferred from the review records

- **m5 of the blind review** (the hint of the deleted `==` reaches the user only in the source line the compiler echoes, and is lost where the excerpt is not shown). Not a defect of this unit: it is the form decision 0236 item 3 chose, and C++26's `= delete("reason")` would put the hint on the error line, which STYLE S2 allows only in an optional header. Deferred to the owner, for after 1.0 or for a move to C++26.

## 18. Addendum: an opt-out of the forced inlining (after gate 2)

- Status: added 2026-11-11 by Claude (agent, Opus 5.5), implementer of this addendum, on `feature/pre-release-core-pass--inline-opt-out`, worktree `.claude/worktrees/pre-release-core-pass--inline-opt-out`, from `merge/pre-release-core-pass-gate2` at `54f2487`. This addendum is the claim of the unit (W3, W7); it merges into `merge/pre-release-core-pass-gate2`, never directly into `develop`.
- Source of scope: the owner's ruling on the gate 2 result (decision 0245 item 1, reopened), accepting recommendation (a): a documented macro that turns the forced-inline attribute off, used in the sanitizer presets and in Clang's optimized presets.

### 18.1 Why

Gate 2 ran the P9 matrix at `a310a2f`. With the public operations forced (decision 0244 item 1), the identity test units cost far more in the sanitizer and the optimized Clang builds than at `ed9288f`, the last `develop` before this unit: gcc-asan 5,367.6 s against 248.4 s, one compile (`identities_four_test.cpp`) at 22.0 GB peak; clang-asan stopped at 216 of 237 build steps with 8 to 16 GB per compile; clang-release 665.1 s against 118.5 s (`identities_rounding_test.cpp` 600.0 s); clang-measure 666.0 s against 116.1 s. gcc-debug, gcc-release, clang-debug, lint and gcc-measure stayed within about 1.6 times their `ed9288f` times, so they keep the attribute.

### 18.2 Goal and definition of done

1. A public macro, documented where `GALOOT_CHECKING` is (the README and the core guide), turns `GALOOT_ALWAYS_INLINE` into nothing. It follows `GALOOT_CHECKING`'s form: a value of 0 or 1, 1 by default, any other value an `#error`.
2. The build passes the opt-out to every target that links `galoot::core` when a cache option says so, and the presets gcc-asan, clang-asan, clang-release and clang-measure say so. No other preset changes. The structure tests `rga3.kernels_inline` and `rga3.kernels_inline_alone` keep their fixed flags and do not see the opt-out, in any preset.
3. Tests first (W2): the opt-out makes the attribute expand to nothing and the default keeps it (a program built in each mode); the code gives the same values with the opt-out (the reference-kernel test built again with it); a value other than 0 or 1 stops the compile with one error; and `core.smoke` checks that the mode the build declares reaches the code.
4. W10: the six identity units of the gate's evidence (`identities_rounding_test`, `identities_five_diagonal`, `identities_four`, `identities_conformal3`, `identities_five_drawn`, `identities_five_dense`) built under gcc-asan, clang-release and clang-asan with the opt-out, through `tools/timed` and the compile-cost launcher, with time and peak memory recorded under `records/performance/pre-release-core-pass/opt-out/` against the gate's figures and those of `ed9288f`. A compile above 20 GiB, or free memory below 20 GiB, stops the run, which is then reported.
5. Records: decision 0244 gains an item for the opt-out and one for the bare `git stash` of part D1; the Resolution sections of the four review records are filled in.

### 18.3 Acceptance criteria

- The new tests and the selection P8 gives the change (the core and rga3 suites, the docs tests of the README and the core guide, the format check, `tools/runlog check`) pass on gcc-debug and clang-debug, and the lint preset passes on the changed files.
- The new tests fail on the mutants of 18.4 (`records/mutations/0040-pre-release-core-pass/opt-out.toml`).
- The six units' records exist for the three presets, or the run is reported as stopped under item 4's rule.
- The full P9 matrix is not run here; the next gate runner runs it.

### 18.4 Mutants

`opt_out_ignored` (the macro's value is not read, so the attribute stays on) must be killed by the program built with the opt-out; `opt_out_always` (the attribute is never on) by the program built with the default; `value_unchecked` (the `#error` removed) by the compile-fail test. Run with `tools/mutate` on gcc-debug and clang-debug.

### 18.5 Agent cost estimate

About 300k to 500k tokens of Opus 5.5, most of it in reading the records and in the W10 runs.
