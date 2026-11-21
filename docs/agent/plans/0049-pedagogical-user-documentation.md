# 0049 Readable, pedagogical user documentation

- Status: proposed 2026-11-21 by Claude (agent, Fable 5.2), chief planner, with the math-expert and hci-expert role files read; the Fable level was authorized by the owner for this unit. From `main` at `c95fee5`, on branch `plan/0049` in the worktree `galoot-wt/plan-0049`. No unit below has started. The coordinator's merge of this plan into `main` is the claim (W3, W7).
- Source of scope: the coordinator's brief of 2026-11-21; WORKFLOW W12 and W6; STYLE S11 and S13; plan 0041 (the documentation lane) and its outcome; decision 0242 (the example mechanism); the guide as it stands (`README.md`, `docs/guide/README.md`, `core.md`, `rga3.md`, `limits.md`); CONVENTIONS 1.18; the misuse catalog (`records/diagnostics/misuse-catalogue.md`); the V10 table and release record of 1.0.0; research note 0025 (the machine-checked proofs behind the core); the owner's writing skills (`pedagogical-writing`, `technical-writing`, `writing-style`, `american-english`) and the owner's rule that proofs and mathematics are documented for a reader with no formal-methods background.
- Siblings: plan 0047 (hardening and verification) and plan 0048 (aggressive simplification of code comments), written in parallel; neither is on any branch at `c95fee5`. Sections 7 and 8 state what this plan needs from each. Plans 0043 to 0045 (release 1.0.1) share three files with this plan (section 12).
- Record locations, free at `c95fee5`: `docs/agent/records/docs/` (new: the authoring rules, the concept inventory, the claim trace, the comment-move table); `records/reviews/<date>-docs-*.md`; `records/mutations/0049-docs/`. The next free decision number is 0252; the siblings may take numbers first, so each unit takes the next free one when it writes.

## 1. Departures, ambiguities, and readings (C20)

1. **Plan only.** Nothing under `include/`, `test/`, `tools/`, `README.md`, or `docs/guide/` changed. One file was added: this plan.
2. **The integration branch is `main`.** The governance says `develop`; plan 0045 section 6 handles the wording. Every `develop` in W5, W7, and P9 is read as `main`. `plan/<name>` is not a branch kind of W7; the brief names it, and the coordinator's merge is the claim.
3. **Plans 0047 and 0048 could not be read.** They exist on no branch. Sections 7 and 8 are requirements this plan places on them, inferred from the brief's one sentence about each. If either plan reads its scope differently, the coordinator reconciles the two texts before unit U2 starts.
4. **The two researchers' notes were not committed.** At this plan's final pass (2026-11-21, 14:18 UTC) the branches `kb/doc-pedagogy` and `kb/complexity-reduction` held no commit beyond their base `80f318a`. Their worktrees held four untracked drafts, minutes old (`doc-pedagogy/documentation-forms.md`, `keeping-examples-true.md`, `learning-science-for-docs.md`, and `complexity-reduction/deep-modules-and-interface-size.md`). The brief says to read what is committed, so the drafts were read by their headings only and no choice here rests on them; the headings name nothing this plan contradicts (the four forms plus a conventions card, an accuracy contract, a cost page, and a diagnostic index; doc-tests; worked examples and fading; teaching recovery from errors). Unit U1 reads them first and records each point where they change a choice made here (section 6, U1).
5. **"Reference (generated or hand-checked against headers)"** is read as a choice this plan makes. It chooses hand-written reference pages with mechanical coverage checks and compiled facts (section 5.6), because generating from `///` comments needs the comments that plan 0048 removes, and no generator is installed (item 9).
6. **"A blind reviewer who has not seen the code"** is read as: the cold reader gets the pages and a command that compiles a file against Galoot, and is given no path to `include/`, `test/`, or `docs/agent/`. It may read compiler output. Section 9.1 says how the record shows that it stayed blind, and what a language-model reader cannot show (owner item O9).
7. **STYLE S11 conflicts with the brief's "headers carry only why-comments".** S11 requires, before each public declaration, a `///` comment that states what the function requires, guarantees, and preserves. Moving that text into the reference is a change to S11, which only the owner ratifies. This plan states what it needs from the headers under either ruling (section 7.4, owner item O3) and assumes neither.
8. **User pages may not cite the conventions specification** (S11; the citation check rejects its name). So the "conventions chapter in plain words" is a full restatement, kept in step with the specification by a trace on the agent side (section 5.8), not a commentary that links to it. Owner item O4 gives the alternative.
9. **Tools on this machine.** `amspell.py` is installed (`~/projects/utils/amspell`). `techlint.py` and `pedlint.py`, which the owner's skills name, were not found under the home directory. Pandoc, Doxygen, mdBook, MkDocs, Sphinx, Typst, Asymptote, and Inkscape are not installed; Python 3.14 and Graphviz are. The plan depends on Python's standard library only (section 5.2, owner item O11).
10. **The conformal model is not shipped.** CONVENTIONS section 6 (cga3) has no preset in 1.0.0, so the conventions chapter and the tutorial leave it out. The core's `conformal<D>` metric and its change of basis are documented where the core guide documents them today.
11. **One probe compile ran outside the job pool**, as earlier planners' probes did: a 25-line program over R(3, 0, 0), g++ 16.2.1, `-std=c++23`, 0.93 s, in the scratchpad. It informed section 4.2 (what `transform` gives for a classical rotor) and is tagged [checked-self, one case] there. `tools/jobpool status` read "not running" at the start; the pool was not started, since no build or test ran.
12. **What was read.** In full: `AGENTS.md`, the constitution, WORKFLOW, STYLE, VVUQ, CONVENTIONS, the three role files, plan 0041, the handoff, the README, the guide's first page, `test/documentation.cmake`, the citation patterns, and four knowledge-base notes (`logic-and-proof/proof-writing-style-guide.md`, `explaining-proof-artifacts.md`, `api-design/math-library-interface-lessons.md`, `geometric-algebra/sign-convention-taxonomy.md`, the last two in their first halves). By headings, tables, and samples: PERFORMANCE (P8, P9), plans 0043 to 0046, `core.md`, `rga3.md`, `limits.md`, the misuse catalog, the V10 table, research note 0025, the headers (comment counts for all sixteen; `transform.hpp` lines 560 to 680 in full), and the tests (file and test-case names only).
13. **Costs are judgments.** The token estimates of section 11 scale from plan 0041's records; they are not measurements.
14. **Checks of this commit.** Records only (P8: documentation, no tests). The plan was scanned with the writing-style patterns and `amspell.py --check`; the citation check reads code and the guide, not `docs/agent/plans/`, and was not run.

## 2. Goal and readers

Give Galoot a documentation set that two readers can use:

- **The newcomer** knows C++ (templates as a user, `constexpr`, `static_assert`) and school mathematics (vectors, the dot and cross products, trigonometry, a matrix as a table of numbers). The newcomer has not met geometric algebra. Today's guide opens "for a reader who knows geometric algebra"; this reader has nowhere to start.
- **The researcher** knows geometric algebra, often in another author's conventions, and needs to know exactly which convention Galoot uses, what is exact, what was verified and how, and where the limits are, before trusting a result.

The set follows the Diátaxis division of documentation into four kinds, because the two readers need different kinds: a *tutorial* (a lesson the reader works through), *how-to recipes* (steps for a reader who has a task), *explanation* (why things are as they are; here "concepts"), and *reference* (the facts of each name, complete and terse). Three single pages cross the kinds: the conventions chapter, the diagnostics page, and the verification page. A glossary and an exercises section serve the tutorial.

## 3. Definition of done and acceptance criteria

The plan is done when every criterion below is met at one revision of `main` and section 14 records the evidence. Each milestone of section 6 closes on the subset its units name.

| ID | Criterion | How it is shown |
| --- | --- | --- |
| A1 | A cold reader with only the declared prerequisites completes the tutorial | Per chapter: every exercise and two unseen tasks compile and pass hidden checks, each fix traced to a page passage, in at most two rounds (section 9.1) |
| A2 | Every code example compiles and runs as a test | Every `cpp`, `cpp rejected`, and `cmake` block of every page is a passing test in each configuration of the P9 `main` row; every exercise scaffold matches its tested solution (section 5.3) |
| A3 | The reference is complete and true to the headers | Each public name has an entry and each entry names a public name (`tests.docs_reference`); each entry states what the operation requires, what it guarantees, the layout of its result, its cost, and its rejections, with at least one compiled example |
| A4 | Every message a user can meet is explained | Each message that a compile-fail test pins appears on the diagnostics page or in a reference entry, and each message quoted there is a literal of the headers (`tests.docs_diagnostics`) |
| A5 | The conventions chapter is complete and pinned | Each item of CONVENTIONS sections 1 to 5, 7, and 8 that 1.0.0 ships has a heading with a compiled example whose values are derived by hand; the docs trace passes (section 5.8); five convention mutants are each killed by a page example |
| A6 | Terms are defined before use | Each italicized defining instance has a glossary row and each row links to one; along the tutorial's order no term is used before the page that defines it (`tests.docs_terms`); the symbols glossary matches the pages in both directions (review) |
| A7 | Each teaching page keeps its contract | Each objective has a worked example that demonstrates it and an exercise that tests it; each listed misconception is refuted (the author's table, checked in review) |
| A8 | Plain American English, no process records | `tests.citations_guide` over every page; `amspell.py --check` clean; the writing-style scans read by the reviewer |
| A9 | Nothing dangles | Relative links, anchors, the headers' pointers into the pages, and the figures pass their checks (`tests.docs_links`, `tests.docs_header_pointers`, `tests.docs_figures`) |
| A10 | The verification page claims only what the records show | Each claim has a row in the claim trace naming its record; the page names the release it describes and lists what is not verified; an auditor's review has no open finding |
| A11 | No knowledge is lost in the comment move | Every `move` row of plan 0048's table is marked received, and a reviewer has reread a quarter of the rows against the original comment text (section 7) |
| A12 | The mathematics is right | An independent math review re-derives every expected value of the conventions chapter and one in five elsewhere, with no open finding |
| A13 | The old pages are retired cleanly | `core.md` and `rga3.md` are gone, every example they held compiles in its new page, the README links the map, and `limits.md` keeps its path and headings |
| A14 | The cost of the examples is known | The build and test cost of the documentation tests is recorded before and after, per configuration (C14) |

## 4. The documentation set

### 4.1 Layout

Everything stays under `docs/guide/`, so W6's row for user documentation holds as written, and `docs/guide/limits.md` keeps the path and headings that sixteen header comments point to.

| Path | Kind | Pages | Reader |
| --- | --- | --- | --- |
| `README.md` (repository root) | quickstart | 1, kept as it is apart from its "Next" section | both |
| `docs/guide/README.md` | map | 1: who you are, where to start, the notation key | both |
| `docs/guide/tutorial/` | tutorial | 11: an opening page and ten chapters | newcomer |
| `docs/guide/exercises/` | practice | 12: a mixed set, an error-finding set, and one solutions page per chapter | newcomer |
| `docs/guide/howto/` | how-to | 6 pages, about 30 recipes | both |
| `docs/guide/concepts/` | explanation | 9 | both |
| `docs/guide/reference/` | reference | 12: an index and one page per header group and for the rga3 preset | both |
| `docs/guide/limits.md` | reference | 1, kept | both |
| `docs/guide/conventions.md` | conventions | 1 | researcher first |
| `docs/guide/diagnostics.md` | questions and answers on messages | 1 | both |
| `docs/guide/verification.md` | what is verified and how | 1 | researcher first |
| `docs/guide/glossary.md` | glossary of terms and of symbols | 1 | both |
| `docs/guide/figures/` | figures and their sources | about 14 SVG files | newcomer |

About 57 pages replace 5. The nine facts of today's first page become `howto/first-things.md` ("If you already know geometric algebra"), unchanged in substance; `core.md` and `rga3.md` are split between the concepts and the reference and then deleted, with no stub left behind (C12).

### 4.2 The tutorial path

The path runs from vectors to rotors and then to motors. Its order is a topological sort of the concept inventory (section 5.1): each chapter uses only the prerequisites and the chapters before it. The first five chapters stay in Euclidean space with the geometric product, where the reader's intuition holds; the rigid algebra, its null generator, and the antiproduct come only after complements have been taught on Euclidean ground.

| Ch. | Title | New ideas, in order | Algebra | Worked from |
| --- | --- | --- | --- | --- |
| 1 | A first program | include path; an algebra as a declared tag; generator; layout; multivector; coefficient; `static_assert` as the check | R(2, 0, 0), `int` | adding and scaling two plane vectors |
| 2 | Vectors and the dot product | the metric as the table of dot products of the generators; `dot`; scalar | R(3, 0, 0) | lengths and right angles with integer vectors |
| 3 | The wedge | bivector as an oriented area; antisymmetry; blade; grade; trivector; e21 = −e12; canonical and oriented names; why `^` needs parentheses; a result has its own layout | R(3, 0, 0) | the area of a parallelogram; a volume |
| 4 | The geometric product | uv = u · v + u ∧ v for vectors; squares of generators; associativity; a general multivector; taking a part; the inverse of a vector | R(2, 0, 0), R(3, 0, 0) | multiplying out by the rules, then by `*` |
| 5 | Reflections and rotors | reflection as −a v a; two reflections turn by twice the angle between the mirrors; rotor; reverse; the sandwich; half angles; composing by multiplying; an exact rotor in integers and the factor it leaves | R(3, 0, 0) | a quarter turn from the mirrors (1, 0, 0) and (1, 1, 0) |
| 6 | Complements and the meet | antiscalar; antigrade; right and left complement; `!`; the antiwedge `&` as the meet; why `!!x` is not always x | R(3, 0, 0) | the line where two planes through the origin meet |
| 7 | Points, lines, and planes | a fourth coordinate w; a generator that squares to zero; rigid(3); point and weight; join; a line's direction and moment; a plane's normal and offset; meet; incidence; the rga3 types and their oriented names; conversion with braces; bulk and weight | rigid(3), `int` | the line through two points meeting a plane |
| 8 | Motions | the antiproduct `%`; antireverse; reflection across a plane (flector); two parallel mirrors move by twice their distance, two crossing mirrors turn by twice their angle about their common line; motor; `transform`; `compose` and its order; `rotation` and `translation`; half quantities; the weight factor | rigid(3), `int` then `double` | a turn about a line that misses the origin |
| 9 | Exact and rounded | what is exact (the structure) and what rounds (the scalar); integers at compile time; `double` at run time; comparing within a tolerance; checking mode; what a product costs to compile | rigid(3) | the same motion in `int` and in `double` |
| 10 | A robot arm | nothing new: three joints composed, checked exactly with quarter turns and run with real angles | rigid(3) | forward kinematics of a three-joint arm |

Four points of design that the math author settles in unit U1 before any chapter is written:

1. **The bridge from rotors to motors.** Chapters 5 and 8 teach the same idea twice, "two reflections make a motion", first with the geometric product and then with the antiproduct, with the examples laid side by side and one question asked about what changed (the pedagogical skill's comparison rule: compare only after the reader knows one method).
2. **`transform` and the classical sandwich.** For the rotor r = b a of chapter 5, the probe gave `r * v * ~r` and `transform(v, r)` the same vector, (0, 2, 0) for v = (1, 0, 0), while the raw antiproduct sandwich `r % v % antireverse(r)` gave its negation [checked-self, one case, section 1 item 11]. U1 establishes for which algebras and versors `transform` equals the classical sandwich, from CONVENTIONS 4.4, by derivation and an exact check, before chapter 5 mentions `transform` at all. Until then chapter 5 uses `*` and `~` only.
3. **What the release lacks.** The core has no norm, inverse, `unitize`, exponential, or logarithm. The tutorial divides by a square by hand where it must and says so once. U1 lists each place the lack costs the reader something and reports the list to the owner as backlog candidates; nothing is added to the library for the tutorial's sake (owner item O13).
4. **The other convention.** Most introductions to rigid motions in geometric algebra put the null generator first, as e0, and act with the geometric product. A reader who goes on to them will be confused unless told. Chapter 8 names the difference in two sentences and points to the porting recipe; the full comparison is in the conventions chapter.

Each chapter is a teaching text under the pedagogical skill: a learner contract (level, prerequisites, two to six objectives in the reader's present vocabulary), a "Before you start" self-check, each concept introduced by motivation, a concrete case, the definition in symbols and in words, a worked example in the Problem, Plan, Solution, Check form, a contrasting example, the refutation of each misconception that applies, check questions, and exercises ordered from near to far transfer with one that mixes earlier chapters. Each chapter also shows one mistake a learner is likely to make there, the message it produces, and the fix, so that the reader learns to recover from an error before meeting one alone. The declared level is `undergraduate-intro` for mathematics; the C++ prerequisites are listed as assumed terms.

The misconceptions the path refutes, each where the reader has the words for it: the wedge is the cross product; e12 and e21 are one blade; the product of two vectors is a vector, or commutes; a rotor holds the angle (it holds half); `!x` is a truth value and `!!x` is x; `a ^ b == c` compares the wedge; a product returns a `Line` or a `Motor`; e4 is one more axis; (2, 4, 6, 2) and (1, 2, 3, 1) are different points; `compose(a, b)` applies a first; `transform` normalizes; a passing check over `double` shows exactness; the dual and the complement are the same; parallel planes do not meet.

### 4.3 How-to recipes

Each recipe states the task in its title, the steps, one whole compiled program, and what goes wrong (with the message). Six pages:

| Page | Recipes |
| --- | --- |
| `first-things.md` | the nine facts of today's first page |
| `build-and-configure.md` | use Galoot by copying, by `add_subdirectory`, by `FetchContent` (pointing to the README's blocks, which stay the tested ones); turn checking on; turn forced inlining off; build without exceptions; shorten compile time; raise the constant-evaluation limits |
| `algebras-and-types.md` | declare an algebra from its squares or its Gram matrix; name a layout and a type of your own; convert between layouts, and when to use `truncate` or `part`; read and write coefficients; print; write code generic over layouts |
| `scalars.md` | compute exactly in integers; use a rational or a type of your own as the scalar; mix `float` and `double` without surprises; compare rounded results |
| `rigid-geometry.md` | turn about any line; move; chain motions; reflect; intersect lines and planes; test incidence; project a point onto a plane; write a motor as a 4 × 4 matrix by moving the basis points |
| `port-from-other-conventions.md` | carry over a formula from Lengyel's pages; from a library with e0 first and the geometric product; from Euclidean rotors or quaternions; change basis between conformal(3) and its diagonal form |

### 4.4 Concepts

Explanation pages say why, for a reader who has the tutorial or equivalent background; their declared level is `undergraduate-advanced`. Nine pages: blades and grades; metric and signature (diagonal, degenerate, and non-diagonal forms, and what a blade is in the last); the products; complements and duals (with bulk and weight); versors and transforms; layouts and result types (why a result has its own layout, and what that buys); exactness and rounding; compile-time cost; and how generation works (the one mechanism, plans, and kernels, for the reader who wants to know what the compiler is doing, and the home for design prose that leaves the headers under plan 0048).

The exactness page is the numerical author's: what "exact structure" means, which scalars round, what the rounding check of `transform` verifies and what it cannot, the form of the error bound in plain words with one worked number, and the limits of the claim (`long double` neither tested nor claimed; coarse types not checked). Derivations there follow the knowledge base's proof layout: the claim in words, a small case with numbers and a check, a notation table, numbered steps each with its reason, "what this does not show", and exercises.

### 4.5 Reference

One page per group of headers: algebras; layouts; multivectors; products; involutions, complements, and duals; transform and compose; change of basis; printing; macros; the low-level blade helpers; the rga3 preset; and an index with every public name and the operator table. Each entry has a fixed form: the header to include; one sentence saying what it is; Requires; Guarantees; Result layout; Cost (what runs at compile time, which blades the result holds); Rejected with (each message, linked to the diagnostics page); a compiled example whose `static_assert`s state the entry's facts about types and layouts, so that a signature that drifts fails a test; See also. The entries start from `core.md`'s reference section, the headers' `///` comments, and plan 0048's move table.

### 4.6 The conventions chapter

Every convention of CONVENTIONS that 1.0.0 ships, restated in plain words in the order a reader needs them: names and numbering of generators and blades; where the null generator sits; oriented names; the order of components; the involutions; complements; bulk, weight, and the duals; the four products; projection; `transform` and `compose`; the rga3 objects and their coefficient orders; the constructors' full angles against stored halves; the operators; the printed form. Each has the rule, a compiled example that would change if the rule did, the reason for the choice where there was one, and how the evidence stands in words a user can read ("recomputed by a script that shares no code with the library", "read on Lengyel's Motor page"). Two sections serve the researcher: a table of how each choice differs in other sources (null generator first or last, wedge or ordered-product bases, right or left duals, the flector sign, the motor's coefficient letters), drawn from the knowledge base's sign-convention taxonomy with only the rows someone has read or tested; and "Where published sources disagree", the five rows of CONVENTIONS section 8 with the choice made and an example that tells the alternatives apart.

### 4.7 Diagnostics, verification, glossary, exercises

- **Diagnostics.** Questions a user asks, grouped by what the user sees: a message that starts with an operation's name (the library's own, in the form "operation: mistake; fix"); the compiler's own text for three misuses the library cannot name (an implicit conversion between layouts, a tag that is not an algebra, narrowing in braces), quoted for GCC and Clang; a very long `mv<...>` type in an error and how to read it; a constant evaluation that ran out of steps, and one that ran out of memory; a run-time assertion in checking mode, with the four causes of the transform check that today live in a comment of `transform.hpp`. Each family has a `cpp rejected` example. A table at the end lists every message with its meaning and fix.
- **Verification.** Plain words for three questions: is the library computing what its definitions say (verification), are the definitions the mathematics people mean (validation), and how far can a rounded result be from the exact one (uncertainty). Then the kinds of evidence in order of strength, a table by area (basis rules, products, complements and duals, transform, change of basis, the rga3 conventions, the rounding check, the messages, the build configurations) saying what was checked, by which kind, over which algebras, scalar types, and compilers; what a machine-checked proof does and does not show, in the knowledge base's box form; what is not verified; and the commands that rerun the checks. Its facts come from plan 0047's hand-over (section 8).
- **Glossary.** One file, two tables: terms (term, definition, first use) and symbols (symbol, meaning, C++ spelling, first use). It is built from the concept inventory before the chapters are written, so that every page uses one term for one thing.
- **Exercises.** Each chapter ends with its exercises; `exercises/` holds a cumulative mixed set, an error-finding set (programs with planted mistakes, offered only after chapter 8), and one solutions page per chapter, each solution a compiled program with its derivation.

## 5. Mechanisms

### 5.1 The learner contract and the concept inventory

Unit U1 writes, under `docs/agent/records/docs/`: the concept inventory (every term, symbol, notation, named result, and technique the set introduces, each with what it depends on and the page that defines it); the misconception list; the page templates for each kind; and the authoring rules, which gather the constraints an author must know before writing a line:

- the pedagogical, technical-writing, writing-style, and American English skills apply to every page, and the knowledge base's proof layout to every derivation;
- every expected value is derived in the prose or a comment beside it, never pasted from a run (C15);
- `^` and `&` are parenthesized beside any other operator (S13);
- no page names a governance document, a numbered rule, a record, or an agent (S11); and three habits the citation check also rejects: "section" or "item" followed by a dotted number, a capital P or W followed by digits (so points are p, q, r or `p1`, never P1), and a parenthesized list such as (C3, S4);
- sections are referred to by name, equations as "Eq. (3)";
- italics mark the defining instance of a term and nothing else.

### 5.2 Tooling: Markdown, the standard library, and no generator

Pages are GitHub-flavored Markdown, readable as plain text and rendered by any Markdown viewer; there is no site generator and no build step for reading. Every check is a Python script on the standard library, run by CTest in the existing presets, so the local gate is the test suite and nothing depends on a hosted service. The docs gate is: the tests labeled `<module>.docs` and `tests.docs` in gcc-debug, clang-debug, and lint; `tests.citations` and `tests.citations_guide`; the docs trace (section 5.8); and, by hand, `amspell.py --check` and the writing-style scans on each changed page.

### 5.3 Examples that cannot drift (doc-tests)

Decision 0242's mechanism stays: a `cpp` block is a whole program, built and run against its `// prints:` lines; a `cpp rejected` block must fail with one error that holds its literal; a `cmake` block is built as a consumer project. Unit U2 adds:

| Addition | What it does | Test |
| --- | --- | --- |
| Pages without programs | `galoot_add_doc_page(PAGE <md>)` registers a page that has no `cpp` block (the map, the glossary), so that `tests.docs_pages` still fails on an unregistered page and `_has_example` still fails on a page that should have examples | fixtures; mutant `page_kind_swapped` |
| `cpp exercise <id>` and `cpp solution <id>` | An exercise scaffold is shown but not compiled; it marks each line the reader writes with `// your turn`. Its solution is a whole program, compiled and run. The tool fails unless each exercise has exactly one solution and the two texts agree once the reader's lines are removed from both | `tests.docs_exercises`; fixtures; mutant `scaffold_drifted` |
| `links` | Every relative link and anchor in every page resolves | `tests.docs_links` |
| `pointers` | Every sentence of the form "docs/guide/X.md, under 'Heading'" in `include/` names a page and a heading that exist | `tests.docs_header_pointers` |
| `terms` | Glossary rows and italic defining instances agree in both directions; along the tutorial's declared order, no glossary term appears before the page of its defining instance | `tests.docs_terms` |
| `reference` | The public names of the headers and the entries of the reference agree in both directions | `tests.docs_reference` |
| `diagnostics` | The messages that compile-fail tests pin (as `tools/catalogue` lists them) and the messages quoted on the diagnostics and reference pages agree in both directions | `tests.docs_diagnostics` |
| `figures` | Each figure regenerated from its source equals the committed file; each image has alternative text | `tests.docs_figures` |

How the tool learns the public names is U2's first question, settled by a half-day spike and recorded as a decision: Clang's JSON syntax tree filtered to the `galoot` namespaces, or a plain list of names checked by one compiled unit that uses each. The simpler one that fails on a new public name wins (C12).

Expected cost: about 300 more translation units per configuration, against 48 today. The probe compiled a three-product program in 0.93 s on GCC without optimization; sanitizer and lint builds cost several times that. U2 measures the present cost of the documentation tests in each configuration, and U13 the final cost (criterion A14). Whether every example runs in every configuration is owner item O8.

### 5.4 Mathematics on the page

- **Symbols.** Unicode, as CONVENTIONS and today's guide write them: ⟑ for the geometric product, ∧ for the wedge, ⟇ for the antiproduct, ∨ for the antiwedge, • for the dot, 𝟙 for the antiscalar. Each is given with its C++ spelling where a page first uses it, and the map's notation key shows all of them with a note on what to do if a font lacks one.
- **Accented operations are written as words.** The combining marks of the literature (a tilde below for the antireverse, an overbar for the right complement) render badly in many fonts and terminals. Formulas write `reverse(a)`, `antireverse(a)`, `right(a)`, `left(a)`, `bulk(a)`; the literature's mark is shown once, where the operation is defined, for the reader who goes on to the sources.
- **Derivations** are calculational chains in `text` blocks: one expression per line, the relation and its reason in braces between lines, so each line is checked against its reason alone. Numbered equations are plain text, "Eq. (3)".
- **Numbers before symbols.** Each rule is first worked with small integers, which Galoot checks exactly at compile time; the general statement follows.

Owner item O5 gives the alternative of TeX in Markdown.

### 5.5 Figures

About fourteen figures, where the idea is spatial: adding vectors; a bivector as an oriented parallelogram, and e12 against e21; reflection in a line; two reflections as a rotation by twice the angle; the grades of three dimensions; the plane w = 1 and a point's weight (drawn one dimension down); a line's direction and moment; a plane's normal and offset; a line meeting a plane; two parallel mirrors and two crossing mirrors; the order of `compose`; a layout as slots of coefficients; the blades of rigid(3) with their oriented names. Each is an SVG file committed beside its source, a short scene description that one standard-library script (`tools/doc_figures.py`, about 300 lines) draws with a fixed projection. Labels use only terms already defined at that point of the text; coordinates are those of the example beside the figure; the image's alternative text and caption carry the content for a reader who cannot see it; colors are chosen to read on light and dark backgrounds. Owner item O6 gives the alternatives (hand-drawn SVG, or text diagrams only).

### 5.6 The reference: written by hand, checked by machine

Three checks stand in for generation: coverage in both directions (`tests.docs_reference`); each entry's facts stated as `static_assert`s in a compiled example; and each rejection shown by a message that `tests.docs_diagnostics` matches to the headers. What no machine checks, the prose of Requires and Guarantees, is the blind reviewer's sample (section 9.3). Owner item O7 gives the alternatives.

### 5.7 The claim trace

As in plan 0041, every factual claim that no example shows is recorded by its author in `records/docs/claim-trace.tsv`: page, heading, claim, source (a header line at a revision, a CONVENTIONS item, a record). The trace is the reviewer's sampling frame and the list to recheck when a source changes. It is an agent record and is never linked from a page.

### 5.8 Keeping the conventions chapter in step with the specification

`records/docs/conventions-trace.tsv` maps each item of CONVENTIONS to a heading of `conventions.md` and the ordinal of the example that pins it, or to "not shipped" with the reason. A script checks that every item has a row, every heading exists, and every example ordinal exists. It runs as a gate command beside W5's convention trace gate, not as a CTest test, because a test file may not name the specification (S11). A change to CONVENTIONS then fails the docs trace until the chapter is brought into line in the same unit (W12).

## 6. Units

Sizes are pages and compiled examples; tokens are in section 11. Each unit's definition of done adds to the common one: the docs gate of section 5.2 passes on the unit's tip, the author's departures are listed, and the unit's rows are in the claim trace.

| Unit | Work | Role (model) | Size | Depends on | Done when |
| --- | --- | --- | --- | --- | --- |
| U0 | Owner rulings O1 to O13 recorded; this plan merged (the claim); sequencing with plans 0043 to 0045, 0047, 0048 fixed | coordinator | 1 decision | — | the decision is on `main` |
| U1 | Design: read the two researchers' notes; learner contracts; concept inventory and its order; misconceptions; templates; authoring rules; the notation key; the four design points of section 4.2 with derivations and exact checks; the list of what the release lacks | math-expert and hci-expert (Opus; Fable under O10) | 5 records, 1 research note | U0 | an independent math-expert has checked the inventory's order for cycles and gaps, and the `transform` statement is derived and checked exactly |
| U2 | Tooling of section 5.3, its fixtures and mutants; the present cost of the documentation tests | build-expert, doctest-expert (Opus) | 8 tool modes, about 20 fixtures | U0 | every new test passes and each mutant of section 10 gives its verdict on gcc-debug and clang-debug |
| U3 | The frame: the layout of section 4.1 with today's content moved, not rewritten; `core.md` and `rga3.md` split; the map; the glossary from the inventory; the README's "Next" section | hci-expert (Opus) | about 20 pages created, 2 deleted | U1, U2 | the same 41 programs, 7 rejections, and 2 CMake projects pass in their new pages, and A13 holds |
| U4 | The conventions chapter and the docs trace | math-expert (Opus) | 1 page, about 30 examples | U3 | A5 |
| U5 | Tutorial chapters 1 to 5, their exercises and solutions, figures 1 to 5, a cold read of each chapter | math-expert author with hci-expert edit (Opus); cold reader (Sonnet) | 11 pages, about 60 examples | U3 | A1, A6, A7 for these chapters |
| U6 | Tutorial chapters 6 to 10, their exercises and solutions, figures 6 to 14, a cold read of each chapter | as U5 | 11 pages, about 70 examples | U5 | as U5 |
| U7 | Concepts | math-expert; numerical-expert for exactness and rounding; performance-engineer for compile-time cost and generation (Opus) | 9 pages, about 35 examples | U3; U6 for links | a math review has no open finding |
| U8 | How-to recipes | hci-expert (Opus) | 5 new pages, about 30 examples | U3 | each recipe was first written task-first by an agent that had not read the headers, and its stumbles are resolved |
| U9 | Reference | cpp-expert with hci-expert (Opus) | 12 pages, about 70 examples | U3; plan 0048's table (section 7) | A3 |
| U10 | Diagnostics | hci-expert (Opus) | 1 page, about 15 rejected examples | U2, U9 | A4 |
| U11 | Verification | auditor role with numerical-expert (Opus) | 1 page | plan 0047's hand-over (section 8) | A10 |
| U12 | The mixed and error-finding sets; the cold read of the whole path by one new reader; fixes | math-expert (Opus); cold reader (Opus, O9) | 2 pages, about 15 examples | U6 | A1 for the whole path |
| U13 | Reviews of section 9, fixes, the comment-move reconciliation, the final cost, the P9 matrix, merges, the outcome | reviewers (Opus; blind review of pages Sonnet), coordinator | — | all | every criterion of section 3 |

Milestones, each merged into `main` when its units pass their reviews, so that the work can stop after any of them with a coherent set:

- **D1, the frame**: U0 to U4. The new layout with today's content, the tooling, the glossary, and the conventions chapter.
- **D2, the tutorial**: U5, U6, U12.
- **D3, the rest**: U7 to U11, then U13's closing.

U5 to U11 write different files and may run at once after D1, except that U6 follows U5. The glossary is one shared file: it is complete at U3, and a later unit that needs a row asks the glossary's keeper (the U3 author) instead of editing it (W7).

Branches: `feature/docs` from `main`, with one subfeature per unit (`feature/docs--tooling`, `--frame`, `--conventions`, `--tutorial-1`, and so on), each in its own worktree; review records on `review/docs-<unit>`, cherry-picked with `-x`.

## 7. What plan 0048 hands over

Plan 0048 removes specification prose from header comments. This plan is where that prose goes. The contract:

1. **A move table**, `records/docs/comment-moves.tsv`, written by plan 0048's survey: for each comment block it will change, the header, the line range at a named revision, a one-line descriptor, and a class: *keep* (a why-comment), *summary* (kept as one sentence), *move*, or *delete* (narration or a duplicate). Each *move* row names its target page and heading from this plan's layout.
2. **What moves where.** Contracts of operations (Requires, Guarantees, what is never checked) to the reference. Definitions of products, signs, and orders to the conventions chapter, while the header keeps the one-line equation at the place it is applied, as S11 asks. Causes and fixes of a message, such as the four causes of the transform check, to the diagnostics page. Measured sizes to the limits page, as today. Derivations and design reasons, such as how the rounding bound is formed or why a plan is built once per pair of layouts, to the concepts pages on exactness and on generation. Examples written in comments to the reference, as compiled examples.
3. **Order.** The receiving text lands and passes review first; plan 0048 deletes afterward. At no revision of `main` is a fact in neither place. This plan's author marks each *move* row received; a reviewer rereads a quarter of the rows against the original text (`git show <revision>:<header>`), criterion A11.
4. **What stays in the headers**, which this plan relies on: every message literal, unchanged (the diagnostics page quotes them, and tests pin them); the pointer sentences to a page and heading, in the fixed form that `tests.docs_header_pointers` reads; and, under owner item O3, either one sentence of `///` per public declaration or S11's full contract.
5. **Stable headings.** U3 publishes the heading list of the reference, concepts, and diagnostics pages, as plan 0041 did for the limits page, so that plan 0048 can write its pointers before the pages are full. After that a heading changes only in a commit that changes its pointers too.
6. **Ownership.** Plan 0048 owns the comments under `include/`; this plan owns `docs/guide/` and the README outside the regions of section 12. Neither edits the other's files; each reports what it finds in them.

If plan 0048 runs before U9, the table is its first deliverable and its deletions wait for item 3. If it runs later, U9 writes the reference from the comments as they stand, and the table then only marks them as moved.

## 8. What plan 0047 hands over

The verification page restates evidence; it produces none. From plan 0047 it needs, in that plan's outcome or a record beside it:

1. **An evidence table** with one row per claim: the claim in plain words; the kind of evidence (a proof or an exhaustive check, an identity tested on random inputs with the count and seed, a value computed independently, a comparison with another implementation, mutation testing, a sanitizer run, a machine-checked proof with its tool and version); the scope (algebras, layouts, scalar types, compilers); the command that reruns it; the revision; and what the evidence does not show.
2. **Verification and validation kept apart**, as V10 asks, with every unverified or unvalidated item listed: at 1.0.0 these include `long double`, algebras above 16 generators, the grade and antigrade involutions' validation, and platforms other than Linux on x86-64.
3. **Each change a user can see**, such as a new check or message, reported to this plan's coordinator, so that the reference and the diagnostics page change in the same unit (W12).

If plan 0047 has not closed when U11 is ready, the page is written from the 1.0.0 V10 table, says which release it describes, and is revised when plan 0047 closes. The page is rechecked at every release; U13 adds that line to the release procedure's checklist by a note to plan 0045's keeper.

## 9. Review

### 9.1 The cold-reader test

For each tutorial chapter, when its author says it is ready:

1. A task author who is not the chapter's author (the reviewing math-expert) writes two tasks the chapter should enable but does not show, and for each task and each exercise a hidden check: a program fragment with `static_assert`s on the answer.
2. A fresh agent, not a fork, gets the learner contract, the chapters up to this one, the glossary, and a command that compiles one file against Galoot and prints the compiler's output. It gets no path to the headers, the tests, or the agent records, and is told to use no knowledge of geometric algebra beyond the prerequisites. It reads the chapter once, top to bottom, does each exercise and task, and reports the table the pedagogical skill defines: each term met before its explanation, each step it could not verify, each reliance on something later, each phrase that calls a step easy, and each example whose purpose was unclear.
3. The chapter passes when every exercise and task compiles and passes its hidden check after the reader's own fixes, each fix came from a passage the reader can quote, and every row of the table is resolved in the text. The author does not argue with the reader: a stumble is fixed, or the missing item is added to the prerequisites where the contract states it.
4. A failed chapter is changed and read by a second fresh reader. After two rounds the result goes to the owner whatever it is.

The record names the reader's model, lists the files it was given, and includes its list of every file it opened, so that a reader who found the headers is seen. At the end, one new reader does the whole path in order (U12).

What this test cannot show: a language model has read about geometric algebra, and an instruction to forget it is not a guarantee. A reader who is stopped is strong evidence of a gap; a reader who passes is weaker evidence that a person would. Owner item O9 offers the stronger test.

### 9.2 Mathematics

An independent math-expert, given the pages and CONVENTIONS but not the author's derivations, re-derives every expected value of the conventions chapter and one in five elsewhere (chosen by the reviewer), checks that each statement true only for some metrics says for which, and checks each sentence about another author's convention against a source someone has read (V9). A claim recalled but not read is cut or marked as the knowledge base marks it.

### 9.3 The user's side, the researcher's side, and the blind review

- **Task first.** For the how-to and reference pages, an hci-expert writes the program for each task from the pages alone before opening a header, and reports where the page was not enough.
- **Trust.** For the conventions, exactness, and verification pages, an auditor reads as a skeptical researcher with the claim trace open: each claim is traced, scoped, and no stronger than its record.
- **Blind review (W5).** One review per milestone before its merge into `main`, by the `blind-reviewer` role: the diff, section 3's criteria for that milestone, and the authors' departures. No file under `include/` changes in this plan, so the reviews of pages run at the Sonnet level, as decision 0240 item 8 set for the documentation lane; the tooling of U2 is reviewed at the Opus level.
- **Style.** The reviewer runs the writing-style scans and `amspell.py --check` and reads the hits.

## 10. Evidence and mutants (W3)

Every test here is new, so a mutant counts when the test written for it is among those it fails. Specs go in `records/mutations/0049-docs/`; a mutant of a header is a temporary edit in a worktree, never committed.

| Mutant | Edit | Expected killer |
| --- | --- | --- |
| `page_kind_swapped` | a page with examples registered as a page without | `<module>.<stem>_has_example` or `tests.docs_pages` |
| `scaffold_drifted` | one fixed line of an exercise scaffold changed | `tests.docs_exercises` |
| `solution_missing` | one solution block removed | `tests.docs_exercises` |
| `anchor_renamed` | one heading of a page renamed | `tests.docs_links` |
| `pointer_stale` | one header pointer's heading misspelled | `tests.docs_header_pointers` |
| `term_early` | a glossary term used one chapter before its definition | `tests.docs_terms` |
| `term_unlisted` | one glossary row removed | `tests.docs_terms` |
| `name_undocumented` | one reference entry removed | `tests.docs_reference` |
| `name_invented` | a reference entry for a name that does not exist | `tests.docs_reference` |
| `message_reworded` | one word of a header message changed | `tests.docs_diagnostics` and that message's compile-fail test |
| `figure_stale` | one coordinate of a figure's source changed | `tests.docs_figures` |
| five convention mutants | the dual taken on the left; the flector's sign dropped; `compose` applied in the other order; a null generator first; the reverse's sign rule without its division by two | an example of `conventions.md` each (criterion A5) |

## 11. Agent cost estimate

Plan 0041 estimated 0.8 to 1.4 million tokens for five pages and about 48 examples. This set is about eleven times the pages and seven times the examples, with a harder audience and more review.

| Work | Model | Tokens (thousands) |
| --- | --- | --- |
| This plan | Fable | about 400 |
| U0 coordination and rulings | Opus | 50 |
| U1 design | Opus (Fable under O10) | 250 to 350 |
| U2 tooling | Opus | 300 to 450 |
| U3 frame | Opus | 200 to 300 |
| U4 conventions chapter | Opus | 200 to 300 |
| U5 tutorial 1 to 5, with cold reads | Opus; Sonnet readers | 450 to 650 |
| U6 tutorial 6 to 10, with cold reads | Opus; Sonnet readers | 500 to 750 |
| U7 concepts | Opus | 350 to 500 |
| U8 how-to | Opus | 250 to 400 |
| U9 reference | Opus | 350 to 500 |
| U10 diagnostics | Opus | 150 to 250 |
| U11 verification | Opus | 120 to 200 |
| U12 mixed sets and the whole-path read | Opus | 150 to 250 |
| U13 reviews, fixes, matrix, merges, outcome | Opus; Sonnet blind reviews | 600 to 900 |
| Total after this plan | | about 3.9 to 5.9 million |

By milestone: D1 about 1.0 to 1.5 million, D2 about 1.1 to 1.6 million, D3 about 1.8 to 2.8 million. Plan 0042 ran 15 percent over its estimate and earlier plans up to twice theirs, so the upper figures are the ones to budget.

Machine time, inferred: about 300 more translation units per configuration (section 5.3); three P9 `main` matrices at about 24 minutes each on plan 0044's figures, plus what the new examples add; the mutants a few minutes each. No measurement takes the timing lock. Every build and test goes through `tools/timed` after `eval "$(tools/jobpool env)"`; `tools/jobpool status` is read before and after each heavy run, the pool is started with `tools/jobpool ensure` only when the status reads "not running", never twice, and never stopped; heavy runs go under `memguard.sh`. No processor setting is touched.

## 12. Files shared with other plans

| File | This plan | Other plan |
| --- | --- | --- |
| `README.md` | U3: the "Next" section only (lines 238 to 250 at `c95fee5`) | 0044 U5: lines 38 to 46; 0045: the status line, `GIT_TAG`, the repository placeholder |
| `docs/guide/core.md` | U3 splits and deletes it | 0044 U1 and U5 edit lines 1042 to 1052 and 1132 to 1134 |
| `docs/guide/limits.md` | kept; links to it change only | 0044 U5: line 409 |
| `test/documentation.cmake`, `tools/doc_examples.py` | U2 | none known |
| `test/CMakeLists.txt` | none; U2's CMake goes in `test/documentation.cmake` | 0043, 0044 |
| `include/` comments | none (pointers are read, not written) | 0048 |

Plan 0044's U5 should merge before U3 splits `core.md`; if it cannot, U3 carries its two edits into the new pages and says so. The repository placeholder in the README's `FetchContent` block is not this plan's to fill and no page of this plan repeats the block; the recipes point to the README's.

## 13. Risks

1. **The antiproduct is hard to teach to a newcomer**, and most other material teaches the other convention. Mitigation: the two-pass design of section 4.2, U1's derivations, and the porting recipe. If chapter 8 fails two cold reads, it goes to the owner with the readers' tables.
2. **The examples' build cost** may be felt at every merge. Mitigation: measured at U2 and U13; owner item O8.
3. **A language-model cold reader passes where a person would not.** Mitigation: section 9.1's limits stated in each record; owner item O9.
4. **The conventions chapter drifts from the specification.** Mitigation: the docs trace and the convention mutants.
5. **Plan 0048's timing.** If comments are deleted before their text is received, knowledge is lost. Mitigation: the order of section 7 item 3, enforced by the coordinator's sequencing and criterion A11.
6. **Symbols that a reader's font lacks.** Mitigation: the notation key, words for accented operations, C++ spellings beside each symbol.
7. **Scope.** Fifty-seven pages invite more. Mitigation: milestones that each stand alone; a page not in section 4.1 needs the owner's word (W8).
8. **Message literals the diagnostics author finds unclear.** They are reported as findings; no message changes in this plan, since no file under `include/` does.
9. **Citation-check false alarms** on ordinary tutorial words. Mitigation: the authoring rules name the three patterns; a pattern that blocks plain teaching text is reported, not worked around.

## 14. Open items for the owner

Each with its default if the owner does not rule. Only the owner can rule these, ratify a change to S11, set a bound on cost, or release.

| ID | Item | Options and tradeoffs | Recommendation (default) |
| --- | --- | --- | --- |
| O1 | How much to authorize now | (a) All three milestones, about 3.9 to 5.9 million tokens. (b) D1 and D2, about 2.1 to 3.1 million: the frame, the conventions chapter, and the whole tutorial; D3 re-estimated from the actual costs. (c) D1 only, about 1.0 to 1.5 million: no tutorial yet, so the newcomer still has nowhere to start | (b). It delivers what the newcomer lacks most and replaces a guess with measured costs before the larger half |
| O2 | The layout | (a) Section 4.1: subdirectories of `docs/guide/` by kind, `core.md` and `rga3.md` split and deleted, `limits.md` kept in place. (b) Keep the five pages and add the new ones beside them: no links break, but two places then say the same thing and drift. (c) Directories directly under `docs/`: shorter paths, but sixteen header pointers and W6's wording change | (a) |
| O3 | What `///` comments keep, with plan 0048 | (a) One sentence per public declaration, with the contract in the reference; this amends S11. Headers stay scannable and an editor's hover still says what a name is. (b) S11 as it stands: the full contract stays in the header and the reference restates it; two copies to keep in step. (c) No `///` at all: the smallest headers, and a user reading a header learns nothing without the pages | (a), ruled once for both plans. Default if not ruled: (b), since it needs no amendment |
| O4 | The conventions chapter and the specification | (a) The chapter restates the conventions in plain words and a trace keeps it in step (section 5.8). (b) Allow user pages to link the specification, by an S11 amendment and a change to the citation patterns: one source, but the specification is written as a record, with decision numbers a user cannot use | (a) |
| O5 | Notation | (a) Unicode symbols, words for accented operations, derivations in text blocks: readable in an editor, a terminal, and any viewer. (b) TeX between dollar signs: better typesetting where the viewer supports it, raw markup everywhere else, and no standard commands for ⟑ and ⟇ | (a) |
| O6 | Figures | (a) About fourteen SVG figures drawn by one standard-library script from short sources, with a staleness test. (b) SVG drawn by hand: no script, but no check that a figure matches its example. (c) Text diagrams only: nothing to maintain, weak for three dimensions | (a) |
| O7 | The reference | (a) Written by hand, with coverage checks and compiled facts. (b) Generated from `///` comments by an owned script: needs the rich comments plan 0048 removes, and parsing C++ templates without a compiler front end is fragile. (c) Doxygen: a new dependency (C13), not installed, and its output is a site, not pages in the repository | (a) |
| O8 | Where the examples run | (a) Every example in every configuration, as today. (b) Tutorial and how-to examples everywhere; solutions and reference examples in the two debug configurations and lint only, which needs a label and a preset filter. The cost difference is not known until U2 measures it | (a) until measured; U2 reports the figures and the owner may then choose (b) |
| O9 | A human cold reader | (a) Language-model readers only, as section 9.1. (b) Also one person who knows C++ and not geometric algebra works through chapters 2 to 5 (about an hour) at D2, the owner or someone the owner names; this is the only test of the tutorial on its real audience | (b) if such a reader is at hand; default (a) |
| O10 | The Fable level | (a) Opus throughout. (b) Fable for U1 only: the concept order and the route from rotors to motors decide every page after them, and an error there is the costliest to repair | (b); default (a) |
| O11 | The skills' checkers and front matter | `techlint.py` and `pedlint.py` were not found on this machine. (a) The owner says where they are; authors run them as advisory tools, and teaching pages carry the YAML front matter they read, which GitHub shows as a table. (b) In-repository checks only (`tests.docs_terms` and the rest), with the learner contract as a visible "Before you start" section and no front matter | (a) if they exist, with the repository's own tests remaining the gate; default (b) |
| O12 | The release that carries the pages | (a) Merge each milestone into `main` and tag a patch release (documentation only, no interface change) when D2 is in. (b) Hold everything for the next minor release. Plan 0045's 1.0.1 does not wait for this plan either way | (a) |
| O13 | Operations the tutorial misses | The core has no norm, inverse, `unitize`, exponential, or logarithm. (a) Teach around them and record U1's list as backlog candidates. (b) Add some first: a change to the core's interface, outside this plan and plan 0044's "no new feature" | (a) |

Still open from before this plan, and unchanged by it: the repository URL in the README's `FetchContent` block (decision 0249; plan 0045 section 4.2 fills it at the 1.0.1 release commit, on the owner's word). This plan does not write a URL anywhere.

For the coordinator, not the owner: the sequencing of section 12; reconciling sections 7 and 8 with the texts of plans 0048 and 0047 when they land; the decision numbers.

## 15. Next step

The coordinator merges this plan into `main` (the claim), puts O1 to O13 to the owner, and, once plans 0047 and 0048 are readable, confirms sections 7 and 8 against them. U1 and U2 then start in their own worktrees from `feature/docs`.

## 16. Outcome

Not started, except as recorded below.

### U1, the design (2026-11-21)

Written by Claude (agent, Fable 5.2, math-expert and hci-expert roles) on `records/docs-design`. Done: the five records under `records/docs/` (`learner-contracts.md`, `concept-inventory.md`, `misconceptions.md`, `authoring-rules.md`, `notation-key.md`), research note 0029, and the checks in `records/docs/checks/`. In progress: nothing. Not done: the independent math review that this unit's row in section 6 requires; the unit is not closed until it has run.

What later units must know, each with its place in research note 0029:

- Section 4.2 point 2 of this plan is wrong. `transform(v, r)` is not `r * v * ~r`; the probe agreed because it tried a quarter turn on a vector of the rotor's plane. The sandwich is `transform(v, !r)` in a Euclidean algebra (note sections 2 and 2.9).
- The ten chapters are cut differently from section 4.2's table: chapters 1 and 2 are one, and a new chapter 6 teaches the antiproduct in R(3, 0, 0) before the rigid algebra (note section 3.3). U5 writes chapters 1 to 5 and U6 chapters 6 to 10 of that cut.
- The tutorial may not depend on a page of D3: chapter 8's pointer to the porting recipe (section 4.2 point 4) becomes a pointer to the conventions chapter (note section 5).
- The file name `docs/guide/conventions.md` fails the citation check when a page links it (note open item 11).

Next step: the coordinator arranges the review, puts the note's open items 1 to 4 to the owner, and settles items 5 to 11. Open questions: those items.

2026-11-21, U1 closed: the independent math review ran (`records/reviews/2026-11-21-0049-u1-check.md`) and confirmed the `transform` statement and the tutorial route by its own derivation; its five findings (one gap in the inventory's order, four of wording) are corrected, the inventory check now also catches a term used above the row that defines it, and the unit is merged into `main`. The coordinator's settlements of the note's items 5 to 11 are decision 0255 part A; the note's items 1 to 4 are put to the owner there (part B). Next step: U2 starts once plans 0043 and 0044 are in `main`.
