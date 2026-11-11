# 0041 Documentation lane: README, the guide, the limits page, DoD-21's bare compiles, and the design study's move

- Status: proposed 2026-11-10 by Claude (agent, Opus 5.5), planner in the hci-expert role, reading for the human user. From `develop` at `77cd947`. Committed on `plan/documentation-lane`; the coordinator merges it into `develop`, which is the claim (W7). No implementation has started.
- Source of scope: decision 0240 items 3 to 7 (the documentation lane now, the design study moved to the research notes, the README's status line; DoD-21 by bare `-fno-exceptions` compiles in the compile-fail lane; no `install()` in 1.0, the README names `add_subdirectory` and `FetchContent`, the version set at release; the umbrella question for `basis_change.hpp` open until part H; `%`, `!` and the strict scalar rule kept and taught on the guide's first page) and item 8 (review proportion); item 9 (the compiler-version limit catalogues move out of the headers in the core pass, plan 0040); plan 0001 section 4 (DoD-1, DoD-2, DoD-21, DoD-27), section 2 (what the design study recommends that was found wrong), and section 6 (M8 row); the Fable advisory report `records/advice/2026-11-10-fable-path-to-1-0.md` sections 3, 4, 5 and 8 (R3, R4, R7 to R10); decisions 0199 and 0229 item 2 (the README mechanism and its labels, left to this unit); WORKFLOW W6, W7 and W12; STYLE S11 and S13.
- Branches (section 6): `feature/documentation-lane` from `develop` after this plan merges, worktree `.claude/worktrees/documentation-lane`, with two parts, `feature/documentation-lane--tooling` and `feature/documentation-lane--pages`, each in its own worktree. Review records on `review/documentation-lane` and `review/documentation-first-page`, cherry-picked with `-x`.
- Record locations, free on every branch at this commit: `records/mutations/0041-documentation-lane/`; `records/reviews/<date>-documentation-lane-review.md` and `<date>-documentation-first-page-cold-read.md`; research note `0000-design-study.md` (the moved study, section 1 item 6). Decision numbers: one, for the example mechanism (section 5.1), allotted by the coordinator at the claim, since plan 0040 may hold the next free number.

## 1. Departures, ambiguities, and readings (C20)

1. **Plan 0040 is not on `develop`.** The brief says plan 0040, the core pass, owns `include/` and moves the compiler-version limit text to `docs/guide/limits.md`. I could not read it. Reading followed: this lane owns `docs/guide/limits.md` and writes it from the header comments as they stand at `77cd947`; plan 0040 deletes the catalogues from the headers and leaves one sentence each that names the page and the heading (section 5.3 fixes the headings now so plan 0040 can cite them). Neither lane writes the other's files. If plan 0040 instead writes `limits.md` itself, section 5.3 moves to it whole and this lane only links to the page. The coordinator should confirm one reading in both plans.
2. **One shared file, `test/CMakeLists.txt`.** W7 asks that concurrent units change different files. Plan 0040 will add tests there (its inlining structure test, the deleted-`==` hint), and this lane must change it too. This lane touches only three regions, each named by the comment that heads it: the `rga3.readme_example_<n>` and `tools.readme_examples_*` blocks (replaced by one `include()` of a new file, `test/documentation.cmake`, which then holds all of this lane's CMake), and the `core.bare_compile` and `rga3.bare_compile` blocks (one new test beside each). Plan 0040 should not edit those regions; git merges disjoint regions cleanly. This is a departure from W7's letter, stated here so that the coordinator can veto it.
3. **The DoD-2 inventory already exists as a draft.** `records/release/dod-2-inventory.md` merged at `263bee3` (the WU18 records drafts, decision 0240 item 11). This lane does not rewrite it: it fills the "Doc page" column, which the draft marks "none yet" until the guide exists, and every public entry the table lists gets a heading in the guide. The other columns and the owner questions the draft raises (public status of the bit helpers, `GALOOT_ASSERT`, entries with no non-test user) stay with WU18.
4. **"Every code example" (W12) against "every fenced C++ example" (DoD-27).** W12 is wider. Reading followed: every fenced block whose info string is `cpp` is a whole translation unit, compiled and run on every preset; a block marked `cpp rejected` is a whole translation unit that must fail to compile with one error carrying Galoot's own literal message; each `cmake` block in the README is a whole consumer project, configured and built offline by a test. Command lines, compiler output and include lists are `text` and are not run. A fragment of C++ is never fenced; it is written inline. Section 5.1 gives the mechanism and open item O7 the alternative.
5. **"In the compile-fail lane" (decision 0240 item 4)** is read as the tests that run one direct compiler command (plan 0010's mechanism), beside `core.bare_compile` and `rga3.bare_compile`. The new tests pass when the compile succeeds; they are bare compiles, not rejections. "`bare_user.cpp`" is read as both bare users, the core's and the rga3 preset's.
6. **The design study's move conflicts with ratified text.** W6's table says `docs/` holds "Documentation for users, including the design study", and CONVENTIONS links the study at `../../galoot-design-study.md` (its "Study" evidence label) and ranks it first among sources. Decision 0240 item 3 orders the move, but both documents change only by owner ratification. This plan gives the exact replacement text (section 5.4) for the owner to ratify before the move merges (O1), so no link is broken on `develop` at any revision. The new name `docs/agent/research-notes/0000-design-study.md` takes the prefix 0000 because the study predates every note and no branch uses that number (O3).
7. **The study's byline is an account handle**, not a name, and git does not say who wrote the text (it was added by the owner's commit `9a4e0cd`). C17 asks that agent work be identified as such, and I cannot tell whether an agent drafted it. Asked as O2; the default if the owner does not rule is in O2.
8. **What DoD-1 means, checked against the code.** DoD-1 is a claim about which signatures the core accepts, not about blade names. The factories `R`, `rigid`, `conformal` and `from_gram` take a bound `MaxN` that defaults to `max_generators = 16` (`metric.hpp:51-55`), and reject n = 0 and n > MaxN at compile time with a message (`check_generator_count`). A user may raise `MaxN` up to `mask_bits` (32); that range is accepted and stated as untested (the owner's amendment in decision 0004). The identities run on R(8,0,0), R(6,6,0), R(15,0,1) and R(8,8,0) over sparse layouts written as masks (`test/core/identities_sparse_test.cpp`), and a plan-cost probe at n = 16 (`plan_cost_antigp_sparse16.cpp`). Blade names are a separate convenience with one decimal digit per index, so `blades<Tag, "...">`, `get<"...">`, `has_name` and printing work only for n ≤ 9 (`max_compact_generators`, `blade_set.hpp:81-88`; `mv.hpp`'s `look_up_name`; `format.hpp`'s wide-algebra rejection); above 9 a layout is a `blade_set` literal of masks and a coefficient is read by position. So the two do not conflict, but a reader who sees "n ≤ 16" and then fails to name e10 has been misled, and dense layouts reach the compilers' constant-evaluation limits far below 16 (the limit catalogues: full<8> already fails some operations on Clang at the default steps). The guide's first page and the limits page say all three facts. One consequence for WU18, not edited here: the release-record draft's "compile-time rejection outside it" holds only at the default `MaxN`.
9. **Branch prefix.** `plan/<name>` is not a kind in W7's table; the nearest is `records/<name>`. I used the brief's name.
10. **No repository URL exists.** The checkout has no git remote, so a `FetchContent_Declare` block has no `GIT_REPOSITORY` to name. The test of section 5.1 runs offline and does not depend on it, but a 1.0 README must not tell users to fetch a placeholder (O5).
11. **Probe compiles in this planning run.** To choose the DoD-21 command I compiled, outside any build tree, a two-line `throw` probe and both bare users with `-std=c++23 -pedantic-errors -fno-exceptions` on g++ 16.2.1 and clang++ 23.1.4, through `tools/timed` with a log in my scratch directory (not the run log). Both bare users compile on both compilers; under `-fsyntax-only -fno-exceptions` GCC reports a `throw` when the template that holds it is instantiated, and Clang when it is defined. No timing claim.

## 2. Goal

Give a person who knows geometric algebra, but not Galoot, the documentation that DoD-27 asks for, with every example compiled and run so that the text cannot drift from the library:

1. a README that is a quickstart, current in its status line, and says how to use Galoot from CMake without an install rule;
2. a guide under `docs/guide/`: a first page that teaches, before anything else, the facts most likely to cost a new user an hour; a core guide; an rga3 guide; and a limits page;
3. DoD-21's evidence: the two bare users compile with `-fno-exceptions`;
4. the design study out of the user documentation and into the research notes, with every inbound link kept working;
5. the DoD-2 inventory's documentation column filled.

## 3. Definition of done

1. The pages of section 4 exist, written by the hci-expert author, and meet section 4's criteria.
2. Every fenced example of every page under `docs/` outside `docs/agent/` and of `README.md` is extracted and tested as section 5.1 says, on every preset; a page that is not registered fails a test.
3. `core.bare_compile_no_exceptions` and `rga3.bare_compile_no_exceptions` exist and pass on both compilers.
4. The design study is at `docs/agent/research-notes/0000-design-study.md` with the header of section 5.4; `AGENTS.md` and the two governance lines point at it, the latter with the owner's ratified text; no Markdown link in the repository is broken by the move.
5. The inventory's "Doc page" column names a guide heading for every public entry.
6. The cold read of the first page (section 10) is done and its findings resolved.
7. The mutants of section 8 give their declared verdicts on gcc-debug and clang-debug.
8. One blind review (section 10); the P9 matrix from empty trees at the final tip; merge into `develop` with a P13 line; worktrees and branches removed after their content is verified on `develop`.
9. The outcome is recorded in section 13, with the evidence for the V10 rows of DoD-21 and DoD-27 stated there for the WU18 keeper to copy (this lane does not edit the V10 skeleton or the release-record draft).

## 4. Acceptance criteria

### 4.1 Rules for every page

- Plain American English, for a reader who knows geometric algebra and C++ but not Galoot; taught in order of need, each idea with a worked example (the owner's pedagogical style). No agent records, governance numbers, unit names, or agent names (S11, W12): `tests.citations_guide` (section 5.1) enforces it on `docs/guide/`, and `tests.citations` already does on `README.md`. The citation patterns reject, among others, "section 4.4", "P2" and "W1"; points are named with lower-case letters, and a convention is restated in words where it is used, never cited by its number.
- The only citations are published sources: Lengyel's wiki pages and posters, cited by page name.
- Every expected value in an example (`static_assert`, `// prints:`) is derived from the mathematics in the prose or a comment beside it, as the README does today, and never taken by running the code and pasting its output (C15). The reviewer re-derives a sample (section 10).
- Every factual claim not shown by an example is taken from a header's `///` comment or the conventions, and the author keeps a trace (claim, page, source line) in the mutation record directory, not in the page.
- `^` and `&` are parenthesized in every mixed expression (S13).
- Change-of-basis examples include `<galoot/core/basis_change.hpp>` explicitly, so they compile whichever way the umbrella question (decision 0240 item 6) is decided.

### 4.2 `README.md`

- The status line says where the project stands today: the core is close to its first stable release and its interface may change until then; the rga3 preset is versioned separately and below 1.0. No version number is written; the version is set at release (decision 0240 item 5).
- "Using Galoot" names three supported routes, each as a whole example: copy or vendor `include/` (the bare compile, which needs no build system); `add_subdirectory`; `FetchContent`. It says plainly that there is no `install()` rule and no package for `find_package` in this release, and that CMake 4.4 or later is needed for the two CMake routes, because Galoot's build requires it and no older version is tested.
- It stays a quickstart: the first example (the join, a motor, a transform, a printed point), the exact-integer example, and printing. The change-of-basis section moves to the core guide unless the owner rules otherwise (O12); every moved block keeps compiling in its new page.
- It links to `docs/guide/README.md` as the next thing to read.

### 4.3 The first page, `docs/guide/README.md`

The landing page of the guide and the page the cold reader tests. Each fact below gets a short heading, two to five sentences, and a compiled example; a rejection that Galoot reports with its own message is shown as a `cpp rejected` block, and a rejection the compiler reports in its own words is described in prose with the code inline.

1. **Results live on their own layouts.** A product's result is stored on the canonical layout of the blades it can reach, not on a named type. Convert with braces: `Line{p ^ q}`, `Motor{compose(b, a)}`. `const Motor m = compose(b, a);` does not compile, since the conversion is explicit. With `auto`, the value keeps the canonical layout: it prints and is read by canonical names (`e24`, where a `Line` says `e42` with the opposite sign).
2. **Scalars must match exactly.** Over `double`, `2 * line` is rejected with a message that says to write `2.0`; `translation(1.0, 0, 2)` and `rotation(l, 1)` likewise (a `cpp rejected` block for `2 * line`).
3. **Two operators to learn: `%` and `!`.** `a % b` is the geometric antiproduct and `!a` the right complement. `!x` is a multivector, not a `bool`: `if (!x)` does not compile, and `!!x` is x with each blade times (−1)^(gr·ag), not x. Write `(!x) == y`.
4. **`^` and `&` bind more loosely than `==`.** Write `(p ^ q) == l`; `p ^ q == l` means `p ^ (q == l)` and does not compile.
5. **Names are oriented.** `get<"e41">(line)` reads the coefficient the layout stores on e41 = −e14; `get<"e14">(line)` is rejected with a message that says the layout holds the other orientation (`cpp rejected`).
6. **How many generators.** An algebra may have up to 16 generators (more with an explicit bound, untested); blade names, `get<"...">` and printing work up to 9, one digit per index; above that, layouts are written with masks and coefficients read by position (shown on the limits page). Dense layouts reach the compilers' limits well before 16; the limits page gives the measured sizes.
7. **`transform` does not unitize.** A versor V with V ⟇ V̰ = λ𝟙, where V̰ is the antireverse, multiplies the weight of what it moves by λ; a motor built by `rotation` or `translation` has λ = 1, a motor written by its coefficients may not. There is no `unitize` in this release.
8. **Printing needs its own header,** `<galoot/core/format.hpp>`, which the umbrella headers leave out.
9. **Qualify `transform`** in a scope with `using namespace std::ranges`, where an unqualified call finds `std::ranges::transform`; the same holds for `reverse`, and `translation` must always be qualified.

Each fact links to the guide section that explains it. The page is short enough to read in ten minutes.

### 4.4 The core guide, `docs/guide/core.md`

- Declaring an algebra (`R`, `rigid`, `conformal`, `from_gram`; the tag); layouts (`blades<...>`, `full`, `of_grade`, masks); `mv` and its construction rules (one value per component, explicit conversions, `truncate` against `part`); scalars (the concepts, exact and rounding scalars, the strict type rule); every product, involution, complement, bulk and weight and their duals, dot and antidot, projection and antiprojection, `transform` and `compose`, each with the convention restated in words; checking mode (`GALOOT_CHECKING`, what it checks for exact and for rounding scalars, and that `truncate` never checks a `double`); printing; change of basis (moved from the README, with `same_rows` and the absence of `==`); a note for authors of generic code (a rejecting overload makes a requires-expression answer yes; ask `has_name` or compare types instead); the result-layout names.
- A reference section with one heading per public entry of the DoD-2 inventory, so that every row's "Doc page" cell can name a heading. An entry whose public status the owner has not settled is documented as it stands, and a later move to `detail` removes its text in the same change (W12).
- Examples link `galoot::core` only, so the core guide cannot depend on the preset.

### 4.5 The rga3 guide, `docs/guide/rga3.md`

The objects and their coefficient orders (point, line with direction and moment, plane with normal and offset, motor, flector) restated from the conventions with their signs; join and meet with orientation; `rotation` (full angle, axis of any positive length, run time only) and `translation`; `compose` order; the flector's action on points, lines and planes; incidence; what the preset does not have in this release (`unitize`, norms, `exp` and `log`).

### 4.6 The limits page, `docs/guide/limits.md`

The headings of section 5.3, in that order, each with the measured figures, the compiler versions they were measured with (GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16), the flags that raise the limit, and where a failure is reported. Every number that plan 0040 deletes from a header comment appears here or is listed in the outcome as dropped with its reason; the check is a diff read at the later of the two merges (section 9). It also holds: the generator bound and `MaxN`; blade names to 9 and the mask form above (one compiled example at n = 12); the range of Gram entries (`max_form_entry`) and what an overflow of `coefficient_type` reports; the scope of the rounding check (`float` and `double` checked; `long double` neither tested nor claimed; a type too coarse for the bound, such as `std::bfloat16_t`, not checked; the bound holds with or without multiply-add contraction; the platform it assumes: x86-64 with SSE2 arithmetic, round to nearest, gradual underflow); the toolchains tested.

## 5. Approach and file ownership

### 5.1 Example mechanism (part `--tooling`, build-expert role)

- `tools/readme_examples.py` becomes `tools/doc_examples.py` (O8), with its fixtures moved to `test/tools/doc_examples/`. It keeps `extract`, `check` and `run`, and gains:
  - `extract --info cmake`, which writes each `cmake` block as `<out>/example_<n>.cmake`;
  - blocks marked `cpp rejected`, written as `rejected_<n>.cpp`, each of which must hold one line `// rejected: <literal>`; `check` fails on a `cpp rejected` block without it;
  - `pages <docs-dir> <registered>...`, which exits 1 naming each Markdown file under `<docs-dir>` outside `agent/` that is not registered.
- `test/documentation.cmake`, included once from `test/CMakeLists.txt` where the README block stands today, defines `galoot_add_doc_examples(PAGE <md> MODULE <core|rga3> NAME <stem> LINK <target>)`, and registers `README.md` (module rga3, stem `readme`, linking the preset), `docs/guide/README.md` (rga3, `start`), `docs/guide/rga3.md` (rga3, `rga3_guide`), `docs/guide/core.md` (core, `core_guide`, linking `galoot::core` only) and `docs/guide/limits.md` (core, `limits`). For each page:
  - `<module>.<stem>_has_example` (the old `rga3.readme_has_example`);
  - `<module>.<stem>_example_<n>`: built and run as today, with the README's lint settings;
  - `<module>.<stem>_rejected_<n>`: `galoot_add_compile_fail_test` with the block's literal as `MESSAGE` and `ONE_ERROR`;
  - labels `<module>;<module>.docs` (O8; decision 0229 item 2 left this to the unit).
- `rga3.readme_cmake_<n>`: the n-th `cmake` block of the README is written as the `CMakeLists.txt` of a consumer project in the build tree, beside a `main.cpp` (`test/docs/consumer/main.cpp`, a minimal program on the preset) and a `galoot` link to the source tree; the test configures it with the outer build's generator and compiler, `-DFETCHCONTENT_SOURCE_DIR_GALOOT=<source>` and `-DFETCHCONTENT_FULLY_DISCONNECTED=ON` (so nothing is fetched), builds it and runs the program. It checks that the block is a working project and names real targets; it cannot check the URL or the tag (O5).
- `tests.docs_pages`: `doc_examples.py pages docs/ <the registered pages>`, so a new page that nobody registered fails rather than going untested; `tools.doc_examples_*` show each new rejection on fixtures.
- `tests.citations_guide`: the citation check over `docs/guide/`.
- A decision record (number from the coordinator) supersedes decision 0199 items 1, 2 and 4 in the mechanism's name, the labels and the reading of section 1 item 4; item 3 stands.

### 5.2 DoD-21 (part `--tooling`)

Beside each bare compile, the same command with `-fno-exceptions`: `core.bare_compile_no_exceptions` (label `core;core.headers`) and `rga3.bare_compile_no_exceptions` (`rga3;rga3.headers`), `-std=c++23 -pedantic-errors -fno-exceptions -I include -fsyntax-only <bare_user.cpp>`. What it covers: on GCC every function the bare user instantiates, on Clang every non-dependent `throw` in every header the bare user includes. Neither bare user includes `format.hpp` (O10). The outcome states the sentence for the release record: the core and the preset throw nothing and compile without exceptions; Galoot's own test runner uses exceptions.

### 5.3 Headings of the limits page (fixed now, for plan 0040's pointers)

1. Number of generators
2. Blade names
3. Coefficient range
4. Compile-time limits: products and plans (from `plan.hpp`, `product.hpp`, `mv.hpp`)
5. Compile-time limits: blade sets (`blade_set.hpp`)
6. Compile-time limits: duals, dot products and projections (`dual.hpp`)
7. Compile-time limits: transform and compose (`transform.hpp`)
8. Compile-time limits: change of basis (`basis_change.hpp`)
9. Raising the limits
10. The rounding check
11. Toolchains
12. The rga3 preset

A pointer in a header names the page and one heading, as in "The measured sizes are in docs/guide/limits.md, under 'Compile-time limits: transform and compose'." The headings are written without numbers on the page.

### 5.4 The design study (part `--pages`, a records commit)

- `git mv docs/galoot-design-study.md docs/agent/research-notes/0000-design-study.md`; the body is unchanged.
- A header in the research-note form: the date of the study (2026-10-30); its author per O2; status: the project's first design record, kept as history; the roadmap of record is plan 0001 (decision 0001), which superseded its phases 1 to 4; the items plan 0001 section 2 records as wrong or ungoverned (dyadic coefficients; `static_assert` messages that print blade names; `basis_names` inside `metric` and a fixed 16 × 16 matrix; the `algebra<M, T>` layer; the fourteen-repository split and graded signatures in the first phases; the "GCC 15+ / Clang 19+" floor; "within 10% of Terathon" and failing CI on compile-time regressions; masks up to 32 against a 16 × 16 matrix); and that a convention taken from it is confirmed only where CONVENTIONS labels it Confirmed or Checked. The study's own outbound links are all absolute (`https://`), so the move breaks none.
- Inbound references, every one found by a grep of the repository outside `build/` and the worktrees: `AGENTS.md` line 3 (a link; edited by this lane to call the study the project's first design record and plan 0001 the roadmap); CONVENTIONS line 9 (a link) and W6's `docs/` row (text), both governance, edited only with the owner's ratified text (O1); plan 0007, decision 0199, the advisory report and CONSTITUTION's revision history mention it in text and are records of their time, left as they are. Proposed text for O1:
  - W6, `docs/` row: "Documentation for users: the README's sequel, the guide under `docs/guide/`." W6, research-notes row, appended: "The design study of 2026-10-30, the project's first design record, is kept here as `0000-design-study.md`."
  - CONVENTIONS, the Study label: "taken from the design study ([0000-design-study.md](../research-notes/0000-design-study.md)); ...", the rest unchanged.
- After the move, a one-off scan of every Markdown file in the repository for relative links whose target is missing, run before and after; the outcome lists both results, and the after list has no entry the before list lacks.

### 5.5 Who owns which file

| Part | Author role | Files it may change |
| --- | --- | --- |
| `--tooling` | build-expert (Opus) | `tools/readme_examples.py` → `tools/doc_examples.py`; `test/tools/readme_examples/` → `test/tools/doc_examples/` and new fixtures; `test/documentation.cmake` (new); `test/docs/consumer/main.cpp` (new); in `test/CMakeLists.txt`, the three regions of section 1 item 2 only; the decision record |
| `--pages` | hci-expert (Opus, high effort) | `README.md`; `docs/guide/README.md`, `core.md`, `rga3.md`, `limits.md` (new); the study's move and header; `AGENTS.md` line 3; the "Doc page" column of `records/release/dod-2-inventory.md`; after ratification, the two governance lines of O1 as one commit quoting the ruling |
| Cold reader | a fresh agent with only the first page and the README (Sonnet) | its review record only |
| Reviewer | `blind-reviewer` (Sonnet, decision 0240 item 8) | its review record only |
| Coordinator | | this plan's merge, the cost rows |

Not touched by this lane: anything under `include/` (plan 0040), the root `CMakeLists.txt` (its version is set at release), `CMakePresets.json`, `cmake/`, the test sources other than `test/docs/`, the V10 skeleton and the release-record draft, and the governance except through O1.

## 6. Order

0. This plan merges into `develop` (the claim); the coordinator allots the decision number and confirms section 1 items 1 and 2 with plan 0040's planner.
1. `feature/documentation-lane` from `develop`; `--tooling` and `--pages` from it, each in a worktree.
2. `--tooling`: fixtures and the tool's new modes with their `tools.doc_examples_*` tests; `test/documentation.cmake` with the README registered (the README's tests pass unchanged under their new labels); the two bare compiles; the consumer test; `tests.docs_pages`, `tests.citations_guide`; mutants; merge into the feature branch.
3. `--pages`, from step 2 merged in: the first page first, then the cold read on `review/documentation-first-page` from that revision (section 10) and its fixes, then the core guide, the rga3 guide, the limits page, the README; the study's move with `AGENTS.md`; the inventory column; merge into the feature branch. The pages author may draft before step 2 merges, compiling examples by hand, but registers each page only after it.
4. If O1 is ratified, the governance lines; otherwise the move waits on the branch and the rest merges without it.
5. Merge `develop` into the feature branch; read the diff of `include/` since `77cd947` and recheck every guide claim it touches (plan 0040's changes); the limits diff of section 9.
6. Blind review on `review/documentation-lane`; fixes.
7. P9 matrix from empty trees at the final tip; merge into `develop`; clean up.

## 7. Evidence (V1, V10)

| Claim | Evidence | Kind |
| --- | --- | --- |
| Every example compiles and runs, and prints what its text says | `<module>.<stem>_example_<n>` on every preset | V1 kind 3, values derived by hand |
| Every rejection a page shows is the library's message | `<module>.<stem>_rejected_<n>` | compile-fail, as the misuse catalogue |
| The CMake routes work | `rga3.readme_cmake_<n>`, offline | integration, not the URL |
| No page escapes the tests | `tests.docs_pages`, its fixtures, mutant `page_unregistered` | V2 |
| The core and preset compile without exceptions (DoD-21) | `*.bare_compile_no_exceptions`, mutants of section 8 | compile check |
| The first page teaches its facts | the cold read | validation by a reader (V8 in spirit, recorded as a review) |
| No process records in user pages | `tests.citations_guide`, `tests.citations` | mechanical |
| The move breaks no link | the before and after link scans | mechanical, one-off |

## 8. Mutants (W3)

Specs in `records/mutations/0041-documentation-lane/`, run with `tools/mutate` on gcc-debug and clang-debug; a mutant of a header is a temporary edit in the worktree, never committed. Every test here is new, so each mutant counts when the test written for it is among those it fails.

| Mutant | Edit | Written for | Expected |
| --- | --- | --- | --- |
| `throw_in_assert` | a `throw` in `detail::assertion_failed`, the failure path of `GALOOT_ASSERT`, which both bare users include and the core's calls | both `_no_exceptions` tests | killed on both compilers |
| `throw_in_transform` | a `throw` on a branch of `transform` | `rga3.bare_compile_no_exceptions` | killed on both |
| `throw_in_format` | a `throw` in `format.hpp` | none | survives, as section 5.2 states (O10) |
| `page_unregistered` | `docs/guide/rga3.md` removed from the registration | `tests.docs_pages` | killed |
| `prints_wrong` | one `// prints:` value of the first page changed | its example test | killed |
| `rejected_ignored` | the tool treats `cpp rejected` as `cpp` | the rejected tests | killed (the block fails to build as a program) |
| `rejected_message_changed` | one word of a block's `// rejected:` literal changed | that rejected test | killed |
| `cmake_block_broken` | the README's `add_subdirectory` block links a target that does not exist | `rga3.readme_cmake_1` | killed |

## 9. Checks (P8, P9, P13)

| Point | Checks | Presets |
| --- | --- | --- |
| Each `--tooling` commit | `^tools\.doc_examples`, `\.docs$` labels, `bare_compile`, `tests.docs_pages` | gcc-debug, clang-debug |
| Each `--pages` commit | the page's own tests (`-L <module>.docs`), `tests.citations`, `tests.citations_guide` | gcc-debug, clang-debug |
| Step 5 | every test labelled `docs`, the limits diff (each number removed from `include/` since `77cd947` found on the limits page or listed as dropped), the link scan | gcc-debug |
| Before the merge | the P9 matrix from empty trees, `format-check`, the trace gate, `tools/runlog check` | all eight, as `gate:<selection>` |

Every build and test through `tools/timed`, after `eval "$(tools/jobpool env)"`; `tools/jobpool status` before and after each heavy run; the pool is started only when it reads "not running" and is never stopped. No measurement in this unit takes the timing lock. The examples add about forty translation units to each preset; their build and test rows in `runs.tsv` are the record of that cost (no W10 campaign: the unit does not change the core).

## 10. Review plan (W5, decision 0240 item 8)

- **Cold read of the first page.** A fresh agent at the Sonnet level, given only `README.md`, `docs/guide/README.md`, the headers, and five tasks written without the facts: join two points and compare the result with a given line; turn a point a quarter turn about the z axis and then move it; double a line over `double`; read a line's direction by name; check whether the right complement of a point equals a given plane. It writes each program, records each compile error and whether the page let it fix the error, and states in its own words what it learned. It does not see section 4.3 or this plan. Pass: every task compiles after the reader's own fixes, and each fix came from the page. A failed task changes the page, and a second fresh reader repeats the read; after two rounds the result goes to the owner whatever it is.
- **Blind review.** One review of the whole feature branch at the Sonnet level by the `blind-reviewer` role, since no file under `include/` changes: given the diff from the branch point, sections 3 and 4, and section 1, not the author's view. It applies the usability checklist's documentation items, re-derives five expected values chosen by itself across the guide and the limits page, and checks the claim trace of section 4.1 on a sample. No delta review unless a fix touches `include/`, which this lane cannot.
- No separate test author: the tests here judge documentation and tooling, not core arithmetic.

## 11. Agent cost estimate

| Work | Model | Tokens |
| --- | --- | --- |
| This plan | Opus | about 200k (not visible to me; estimated) |
| `--tooling` author | Opus | 150k to 250k |
| `--pages` author (first page, guides, limits, README, the move, the inventory column) | Opus, high | 350k to 550k |
| Cold read, one or two rounds | Sonnet | 40k to 80k each |
| Blind review | Sonnet | 120k to 200k |
| Fixes, mutants, matrix, merge | Opus | 100k to 200k |
| Total | | about 0.8M to 1.4M; the history of plan estimates suggests up to twice that |

Machine time: the mutants about 2 minutes per mutant and preset (about 30 minutes); the matrix from empty trees about 21 minutes of pool time; no timing lock.

## 12. Open items for the owner

Each with its default if the owner does not rule.

- **O1. Ratify the text of the two governance lines** (W6's `docs/` and research-notes rows; CONVENTIONS's Study link) in section 5.4. Tradeoff: ratifying now lets the move merge with no broken link; deferring keeps the study in `docs/` at the merge, where a 1.0 user takes it for the design. Recommend: ratify as written. Default: the move waits on the branch; everything else merges.
- **O2. The study's byline.** Options: (a) "Author: not recorded; added by the owner on 2026-10-31", dropping the handle; (b) the owner named as author; (c) the owner as author with an agent named as the drafter, if one drafted it (C17); (d) keep the handle. Tradeoff: (b) and (c) need a fact only the owner has; (d) leaves an anonymous byline on a record. Recommend: tell us who wrote it, and use (b) or (c); default (a).
- **O3. The study's number.** (a) `0000`, before every note, as it predates them; (b) `0028`, the next free number. Recommend (a); no record uses either.
- **O4. CONVENTIONS's ranking sentence** ("The design study is the most canonical reference for Galoot"). Moving the study does not change it, but the new header calls the study history. Options: keep; or the owner rewords it at M8. Recommend: keep now and decide at the release walkthrough; this lane changes only the link.
- **O5. The repository URL for `FetchContent`.** There is no remote. Options: (a) the owner gives the public URL and the tag form now; (b) the README's block carries a marked placeholder that the release commit fills, listed in the release record as a must-fill; (c) leave `FetchContent` out of the README until release. Tradeoff: (b) and (c) leave the README incomplete until the tag; (c) departs from decision 0240 item 5. Recommend (a); default (b).
- **O6. The CMake minimum for users.** Galoot's `cmake_minimum_required(VERSION 4.4)` applies to anyone who adds it with `add_subdirectory` or `FetchContent`, and CMake 4.4 is recent. A lower minimum would reach more users but would be a claim no installed CMake can test (decision 0006 claims no untested minimum). Recommend: keep 4.4 and say so in the README; this lane does not change it.
- **O7. The example kinds of section 1 item 4.** (a) `cpp`, `cpp rejected` and the README's `cmake` blocks all tested; (b) `cpp` only, rejections in prose and CMake as `text`, DoD-27's letter. Tradeoff: (a) costs a tool mode and one compile-fail test per rejected block, and catches drift in the messages and the install text that a user meets first; (b) is smaller and leaves W12's "every code example" unmet for CMake. Recommend (a).
- **O8. The tool's name and the labels.** (a) `tools/doc_examples.py`, labels `<module>.docs`, the README's test names unchanged; (b) keep `readme_examples.py` and `rga3.readme`. Tradeoff: (a) renames a tool and its fixtures once (only run-log rows name the old label); (b) leaves a name that misleads once five pages use it. Recommend (a).
- **O9. The inventory's reading.** This lane fills only the "Doc page" column and documents every entry the table calls public at its merge; the draft's owner questions (public status of the bit helpers and `GALOOT_ASSERT`, entries with no non-test user) stay with WU18. Recommend: accept.
- **O10. `format.hpp` under `-fno-exceptions`.** Neither bare user includes it, so a `throw` there would pass (mutant `throw_in_format`). Options: (a) as ruled, the two bare users; (b) also compile, the same way, a third small file that prints one value, `test/docs/bare_format.cpp`. Tradeoff: (b) is one more file and test for full coverage of the headers users include. Recommend (b); default (a).
- **O11. Where the first page lives.** (a) `docs/guide/README.md`, a landing page of its own that the cold reader tests alone; (b) the opening section of the core guide. Recommend (a): one short page that every user meets, linked from the README.
- **O12. The README's change-of-basis section.** (a) Move it to the core guide, leaving the README a quickstart of about 180 lines; (b) keep it in the README. Recommend (a).
- **O13. The coordinator's choices, not the owner's:** section 1 items 1 and 2 (the boundary with plan 0040 and the three regions of `test/CMakeLists.txt`), and the decision number.

## 13. Outcome

Merged into `develop` on 2026-11-11 by fast-forward to the merge that closes `feature/documentation-lane`, after the P9 matrix below. Recorded by Claude (agent, Opus 5.5), merge-gate runner.

### Departures from this plan

1. **The first page's fourth fact is taught differently from section 4.3 item 4.** "Write `(p ^ q) == l`" holds only for an `l` of the join's own canonical layout: values of different layouts have no `==`, so against a `Line` the parenthesized form does not compile either. The page says that `p ^ q == l` means `p ^ (q == l)` and compares a join with a `Line` after converting it, `Line{p ^ q} == l`. The claim trace (`records/mutations/0041-documentation-lane/claim-trace.md`) records the reading, and the blind review confirmed that both forms are rejected on both compilers.
2. **The example mechanism is decision 0242**, which supersedes decision 0199 items 1, 2 and 4 (item 3 stands under the new names) and records section 5.1 as built. The counts in its consequences are those of the pages part's tip. At the merge, the five pages give 55 tests on each preset (41 programs, 7 rejected blocks, 2 CMake projects, 5 page checks), plus `tests.docs_pages` and `tests.citations_guide`.
3. **The cold read** took one round: `records/reviews/2026-11-10-first-page-cold-read-1.md`. Reader: Sonnet 5.5, given only the two READMEs. All five tasks compiled after the reader's own fixes, each fix taken from the page, and the page's wording was fixed where the reader stumbled. No second reader was needed.
4. **The blind review** is `records/reviews/2026-11-11-documentation-lane-review.md` (blind-reviewer role, Sonnet 5.5). Verdict: approve with six minor findings, no blocker or major. Findings 1 to 5 are fixed in the pages and the claim trace (the merge of `--pages` at `4483b6d`). Finding 6 is the coordinator's. The release record draft on `develop` now lists the `FetchContent` placeholders as a must-fill. The page's figures that the core pass removes from the headers are traced to `limits-removed.md`, which is on the core pass's branch, so the limits diff of section 9 is read at the later of the two merges (item 7).
5. **The lint round.** The first matrix at `6ca81ad` failed only under `lint`: clang-tidy rejected twelve programs of the guide, which the pages part had checked on gcc-debug and clang-debug only. The findings were designated initializers for `oriented_blade`, static members read through an instance, two redundant expressions, a variable `l` confusable with a header's pack `I`, external linkage of a helper, and an unused include. The pages author fixed each in its page at `00f10f0`, without switching any check off, and the matrix below was rerun from empty trees. The `l` against `I` finding is reported inside `include/galoot/core/linear.hpp`, so any user who runs that check and names a variable `l` meets it; a rename of the pack is left to the core, which this lane does not touch.
6. **Mutants.** `throw_in_format` is killed, not surviving as section 8 expected, because decision 0241 item 5 added the third bare compile (`test/docs/bare_format.cpp`). Six mutants of the tool's new modes were added. `rejected_ignored` was first judged on the tool's fixtures only; at `6ca81ad` it also ran against the pages, where each of the seven rejected blocks, read as a program, stops its own build at the static assertion its literal quotes. It names no killer test, since the mutant unregisters the rejected tests. The record is `records/mutations/0041-documentation-lane/README.md`.
7. **Step 5 had nothing to recheck.** `git diff 77cd947..develop -- include` was empty at both merges of `develop`, since the core pass had not merged. The limits diff of section 9 and the recheck of guide claims against changed headers move to the core pass's merge, or to this lane's successor if that merge comes first.
8. **The matrix ran nine configurations**, the plan's eight and gcc-measure.
9. **Not yet done:** the removal of the worktrees and branches of this lane after their content is verified on `develop` (definition of done item 8), which the coordinator does.

### The P9 matrix

`gate:all` from empty build trees at `ed9288f`, all through `tools/timed`. The shared pool showed 23 of 23 slots free before; at the end another workstream held 11. No timing lock.

| Check | Result | Wall time |
| --- | --- | --- |
| gcc-debug | 626 of 626 pass | 101.7 s |
| gcc-release | 626 of 626 pass | 149.6 s |
| gcc-asan | 618 of 618 pass | 248.4 s |
| clang-debug | 626 of 626 pass | 98.3 s |
| clang-release | 626 of 626 pass | 118.5 s |
| clang-asan | 626 of 626 pass | 207.2 s |
| lint | 626 of 626 pass | 260.7 s |
| gcc-measure | 618 of 618 pass | 110.5 s |
| clang-measure | 618 of 618 pass | 116.1 s |
| format-check | pass | 3.4 s |
| trace gate | pass | 0.5 s |
| `tools/runlog check` | pass | 4.6 s |

The workflows took 1411 s together. Mutants at `6ca81ad`, on gcc-debug and clang-debug: `tooling.toml` 10 killed and `pages.toml` 5 killed on each, both exit 0. The lint fixes left the pages' anchors and block counts as they were, and `tools/mutate --check` accepts `pages.toml` at `ed9288f`.

### Evidence for the release record (V10)

- **DoD-21.** `core.bare_compile_no_exceptions`, `rga3.bare_compile_no_exceptions`, and `core.bare_compile_format_no_exceptions` pass on both compilers. They run `-std=c++23 -pedantic-errors -fno-exceptions -I include -fsyntax-only` on the core's and the preset's bare users and on `test/docs/bare_format.cpp`. The mutants `throw_in_assert`, `throw_in_transform` and `throw_in_format` are killed. Sentence for the record: the core and the preset throw nothing and compile without exceptions; Galoot's own test runner uses exceptions.
- **DoD-27.** Every fenced example of `README.md` and of the four pages under `docs/guide/` is a test on every preset. A `cpp` block is a program built and run against its `// prints:` lines. A `cpp rejected` block must fail with one error holding its literal. A `cmake` block of the README is a consumer project configured and built offline. `tests.docs_pages` fails on a page nobody registered, and `tests.citations` and `tests.citations_guide` keep process records out of the pages. The mechanism is decision 0242.
- **The design study's move** (decision 0241 item 2): a link scan of every tracked Markdown file found 1 broken relative link before the move (this plan's proposed CONVENTIONS link, to the new path) and 0 after, and the blind review's own scan found 0 at its revision.
- **DoD-2.** Every row of `records/release/dod-2-inventory.md` names a heading of the guide in its "Doc page" column, checked by script against the pages.
