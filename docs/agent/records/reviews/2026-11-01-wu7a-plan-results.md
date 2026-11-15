# W5 review: feature/core-plan-results (plan 0013, WU7a) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01. Agent work.
- Reviewed: `git diff develop...feature/core-plan-results`, merge base and `develop` both at `0bbb968`, head `0607515` (15 commits). The code and tests at `0607515` are `d6ea717`'s (`git diff d6ea717 0607515 -- include test CMakeLists.txt cmake tools` is empty); `d6ea717` changed only the `///` comment of `product_plan` after `38f2b30`. WU7b is not under review.
- Brief: it gave the branch, its parent, criteria 1 to 11 of plan 0013 section 5, and the author's twelve departures, and asked for an adversarial review. It carried no verdict of its own. The branch's plan does carry one (its "Outcome, WU7a" marks every criterion "Met"); I read it while reading the plan for the criteria, before reading the code, so this review is not blind to it. I formed every result below from my own builds, runs, and derivations.
- Departure from the role file: the role file says the reviewer returns the record for the merging agent to commit; the brief asked me to commit this record on the feature branch, which I did, and nothing else. I changed no file of the change.
- Verdict: **accept after fixes**. No defect found in the generated plans: every term, sign, position, result set, and order I derived independently agrees, on both compilers. F1 (a record correction) and F2 (a test-only addition) should land before the merge; F3 to F6 are non-blocking. None needs a change to `include/`, so no delta review is needed unless the author also takes up F5's comment wording.
- Job pool: 23 of 23 free before my first build (17:12Z) and after every heavy run (fresh builds, mutation runs, per-commit runs, asan); never started or stopped.

## Not checked or not run (C20)

- WU7b (unary maps, conjugated plans) and the knowledge-base branch `kb/constant-evaluation-memoization`: not reviewed.
- The author's 81 mutants were not rerun; I ran 17 of my own (including the two plan section 10 names), on gcc-debug and clang-debug in normal mode only, not `--runtime`.
- Limits: boundary points only, bare `-std=c++23 -pedantic-errors -O2 -fsyntax-only`, not the two Galoot-flag configurations and not every n.
- P2 growth (criterion 10) not rerun; W10 rows in `compile.tsv` spot-checked (plan_test rows at `0bbb968` and `86226f7` exist), not recomputed.
- Presets: I ran gcc-debug, clang-debug, lint, and gcc-asan from new trees at `0607515`; not gcc-release, clang-release, or clang-asan (the author's rows at `86226f7` show all seven passing; P10).
- Outside `tools/timed` (P13): the Python reference and generator of my independent harness, the two sub-second runs of its dumper programs (their compiles were timed), the token comparisons of the migration, and git inspection.
- My run-log rows (148, selections `review-*` and the per-commit runs) are not committed, as briefed; they are in the uncommitted `runs.tsv` of my worktree `.claude/worktrees/review-wu7a` (detached at `0607515`). Six of them are failed compiles of a header experiment I mis-edited (selection `review-hdr:*:templates`, exit 1) and were rerun as `templates2`.
- Agent cost: about 60 tool calls; token counts are not visible to me. The agent-cost row of the unit is not yet written (the outcome says so).

## The four W5 checks, and where they ran

All at `0607515`, from new, empty build trees in my own worktree, through `tools/timed`, GCC 16.2.1 and Clang 23.1.4:

| Check | Result |
| --- | --- |
| Style: `format-check` (gcc-debug, clang-debug, lint) and clang-tidy (lint preset, `all`, warnings as errors) | pass; lint `all` + header sets + format-check 15.1 s, exit 0 |
| Full suite, gcc-debug | build `all` + `all_verify_interface_header_sets` exit 0; 131 of 131 |
| Full suite, clang-debug | build exit 0; 132 of 132, `core.plan_evaluated_once` passing (3 and 3 events, median 5 µs against 7,216 µs, ratio 0.0007) |
| Also: lint suite; gcc-asan (sanitizers, P9) | 132 of 132; 131 of 131 |
| Trace gate (`python3 -I tools/test_references.py trace ...`, from the root) | exit 0; 4.1 now pinned at `plan_test.cpp:1539` among others; items with no test 3.2 to 3.4, 4.2 to 4.5, 5.x, 6.x, 7.1 |
| Review | this record |

## Checks and how

### Correctness, derived independently

- Hand values re-derived: P ⟑ P (15 terms on 1 and the six bivectors, every position) and P ∧ P (12); L ⟑ G 15 on (e1, e2, e3, e4, e124, e134, e234), from 24 pairs less the 9 with e4 on both sides, e.g. e23 e321 → e1, e41 e321 → e234; L ∧ G none; P ∧ L 12 on the four trivectors; M ⟑ P 28, M ∧ P 4 + 12 = 16; L ⟑ L 27, L ∧ L 6 on e1234; conformal(3) V ⟑ V 3 + 20 + 2 = 25 with e4 e5 = −1 + e45, e5 e4 = −1 − e45, V ∧ V 20; the sparse sets (e1n e1 = −e_n, e1n e2 = −e12n, e1n ∧ e2 = −e12n, and the wedge dropping (1,0) and (3,0)); the repeated mask (e1, e2, −e1) × e1 and × e2. All agree with the tests.
- Independent harness (scratch, not committed; reproducible from the description): a Python reference computes 20 typed plans, the products from `test/reference/clifford_reference.py` (`Algebra.mul`, diagonalization and change of basis, sharing no code or method with the header), the wedge from the written index lists (zero on a shared index, else the sign of the permutation that sorts the concatenated lists), asserted equal for every pair to the top-grade part of `Algebra.mul` in the same form (a second route), result sets sorted by (grade, index list). A generated C++ unit prints `product_plan` for the same sets written by name with `core::blades<n, "...">`. Cases: rigid(3) M ∧ G, G × P, L × P, G × G, M × L, M × M; conformal(3) {e21, e54, e3, e15, e5} × {e4, e5, e1, e432, 1} and {e45, e123, e1} × {e4, e5, e12345}; a dense form of four generators [[2,1,0,−1],[1,0,3,0],[0,3,−1,2],[−1,0,2,1]] with {e21, e4, e123, 1, e43} × {e3, e41, e2341, e2}; the Witt form [[0,1],[1,0]] with {1, e1, e2, e21} × {e2, e1, e12}; each for both operations. Result: 20 plans, 330 terms, identical on GCC and Clang to the reference (diff empty but for a trailing space of my printing). No result blade has a sign other than +1.
- The wedge rule against CONVENTIONS 4.1: `pair_terms` returns nothing when `a & b` is nonzero, else `reorder_sign(a, b)` on `a | b`, reading nothing of the metric. This is 4.1's definition; the test's own `inversions` (pairs i in A, j in B, i > j) shares no code with `bits.hpp`. The tests also check 4.1's second sentence (the grade |A| + |B| part of the product) in every non-diagonal form of the set, and the golden route.
- Section 2 derivations the plan asks the reviewer to check: (1) the result set holds exactly the blades some input makes nonzero: correct for independent inputs, because distinct pairs (a, b) give distinct monomials and the order check excludes a repeated blade within a pair (see F5 for a scalar caveat). (2) Complement reverses canonical order (WU7b's fact): correct by hand, since within a grade A precedes B exactly when the lowest generator of A Δ B is in A, and complements keep A Δ B and swap membership; brute force over all pairs of masks for n = 1 to 8 found no violation.

### Migration (criterion 7, `9430423`, departure 10)

- Diffed `test/core/plan_test.cpp` and the seven fail sources at `0bbb968` against `9430423`. Every changed line is a rename (`gp_plan` → `gp_terms`, `generate_gp<C>` → `generate_plan<gp, C>`, `plan_capacity(as, bs)` → `plan_capacity(gp, as, bs)`), a reflow by clang-format, the two uses of B7 reading `.terms`, B8's type assertion now on `.terms`, and test names and comments. A token comparison of every `.a =`, `.b =`, `.out =`, `.coefficient =`, `== <number>`, and `size() ==` before and after differs only by two `== 1` and `== 2` moved to the next line by reflow and by the four new member reads inside `read_back`. Test-case and `static_assert` counts are unchanged (11 and 27). The claim "every expected value unchanged" holds.
- `read_back` reads `result.blades.at(out).mask`, so the migrated tests now also check the typing: a position out of range fails the constant evaluation.
- The rejection messages are the only changed text of the fail tests; the set rejections now forbid all ten generation messages in full, and the overflow test forbids the prefix "product plan:". `plan_not_metric_like` forbids `make_product_plan`, and my mutant `unconstrained_product_plan` (`class Alg` on `product_plan`) is killed by it on both compilers, so WU6b's review finding F2 is closed.

### Each commit from its committed tree (W2, departure 6)

Checked out each code or test commit in my worktree; built `all` and the header sets and ran the full suite on gcc-debug and clang-debug; ran `format-check`; and for five of them the lint preset's `all` (clang-tidy):

| Commit | gcc-debug | clang-debug | format-check | clang-tidy (lint `all`) |
| --- | --- | --- | --- | --- |
| `3bca0e6` | 124/124 | 124/124 | **fails** (`plan.hpp:90`, `:91`, `pair_capacity`) | passes |
| `9430423` | 124/124 | 124/124 | **fails** (same) | **fails** (`plan.hpp:502`, the cast false positive) |
| `0fce606` | 124/124 | 124/124 | **fails** (same) | **fails** (same) |
| `7818335` | 128/128 | 128/128 | **fails** (same) | **fails** (same, and trailing commas in `plan_test.cpp`) |
| `38f2b30` | 128/128 | 128/128 | passes | **fails** (trailing commas in `plan_test.cpp:1053` and others) |
| `04c12c1` | 128/128 | 128/128 | passes | not run (passes at head) |
| `11ad0aa` | 131/131 | 132/132 | passes | not run |
| `d6ea717` | 131/131 | 132/132 | passes | not run |

So the commits checked on dirty trees (`3bca0e6`, `9430423`, `11ad0aa`) do build and pass their full suites as committed, which is more than the author's selections showed. But the mechanical checks failed over a longer range than reported (F1).

### W9, W7, records

- W9: header `3bca0e6`, migration `9430423` (its own commit, listed), removal `0fce606`, tests `7818335`. After `7818335`, `include/` changed at `38f2b30` (layout of `pair_capacity` and the NOLINT, no code) and `d6ea717` (comment); `test/` changed at `04c12c1` (layout; a token comparison of expected values before and after is identical) and `11ad0aa` (the new CTest, its unit, script, and fixtures, which judge the header's existing static locals). All listed in the outcome.
- W7: every message says what and why and names the agent and model.
- Decisions 0110 to 0113 and 0117 exist, each with context, options, decision, and consequences; the owner-default answers are recorded as the brief's departure 1 says. 0117 takes the first unassigned number of the plan's range, as stated.
- W10: before rows at `0bbb968` and after rows at `86226f7` exist in `compile.tsv`, from new trees; not recomputed. The agent-cost row is open.
- STYLE S11 and decision 0034: the added lines of `include/`, `test/`, and `test/CMakeLists.txt` contain no citation of governance, decisions, plans, notes, reviews, lessons, or agents (searched for decision, plan, WU, review, lesson, agent, note, research, the document names, provision numbers, criterion, section, Claude, Opus; only the clang-tidy suppression's own reason matched), and `tests.citations` passes. The new rejection sources and `plan_evaluated_once.cpp` state their reasons in place.
- The clang-tidy suppression (departure 4): reproduced; at `9430423` to `7818335` lint fails with "C-style casts are discouraged" at the `product_plan` line, which has no cast. One line, one check, with its reason: acceptable under S4.

### Mutants (reviewer's own, `0607515`, gcc-debug and clang-debug, identical outcomes on both)

Driver in my scratch directory: each anchor asserted unique, the program deleted before each build, no CTest after a failed build (lesson 0063), `core.plan` label run otherwise (the doctest program, the compile-fail tests, and on Clang the evaluation check), header restored and rebuilt after the last.

| Mutant | Change | Result |
| --- | --- | --- |
| `wedge_shared_kept` (plan section 10) | the wedge's shared-generator test dropped | killed at build: `plan_test.cpp:1080` (P ∧ P), `:1131` to `:1141` |
| `result_by_mask` (plan section 10) | `collect_result` orders by mask value | killed at build: `:1043`, `:1080`, `:1138` and others |
| `wedge_sign_args_swapped` | `reorder_sign(b, a)` | killed at build: `:1080`, `:1182`, `:1230` to `:1232` |
| `wedge_sign_by_xor_product` | wedge sign +1 | killed at build |
| `wedge_range_and` | range check on `a & b` | killed by `core.plan_wedge_pair_beyond_n` and `_second` |
| `wedge_range_shift` | range check as `((a \| b) >> N) == 0` | **survives** (F2) |
| `wedge_capacity_zero` | `pair_capacity` gives 0 for the wedge | survives: masked by `plan_capacity`'s floor of 1 (F3) |
| `collect_insert_before_last` | insertion stops one place early | killed at build by the result-order check |
| `typed_copies_short`, `typed_size_minus_one` | one result blade dropped | killed at build |
| `result_sign_negative_only` | the result-sign check as `sign < 0` | survives: equivalent on the generator; the bound test uses only −1 (F3) |
| `type_terms_unbounded_out` | `type_terms` ignores the size of `out` | killed at run time ("collect_result and type_terms keep to their stated bounds") |
| `typed_order_b_le` | `before.b <= t.b` in the typed order | killed at run time ("The checks of a typed plan find each defect at its bound") |
| `unconstrained_product_plan` | `class Alg` on `product_plan` | killed by `core.plan_not_metric_like` |
| `locals_plain` (clang-debug) | every local of `make_product_plan` plain | killed by `core.plan_evaluated_once`: ratio 0.5543 |
| `locals_plain_thrice` (clang-debug) | plain locals, initializer generating three times | killed at build (constant-evaluation limit in `plan_test.cpp`); the evaluation check alone on it: ratio 1.6281, fails |
| `thrice_static_locals` (clang-debug) | initializer calls the generator three times, static locals | passes the check (ratio 0.0081), as criterion 9 requires |

The evaluation check against the plain-locals copy, which plan section 10 asks the reviewer to run: done above. The script's own rules are tested on fixtures that include decoy events (`plan`, `make_product_plans_of<x>`), and it fails rather than passes when it finds fewer than three events.

### Limits and cost (criteria 8, 10; departure 11)

- Boundary compiles, bare -O2, `-fsyntax-only`, against the head's headers: GCC builds `full<6>` × `full<6>` in R(6,0,0) for both operations and stops at n = 7 (product in `collect_result`, `plan.hpp:205`; wedge in `type_terms`, `:253`), and builds the dense form at 5 and stops at 6 in `blade_product`; Clang builds n = 5 for both and stops at 6 (product in the count, wedge in `collect_result`), dense 4 builds and 5 stops. Against `0bbb968`'s headers, `gp_plan` of `full<7>` in R(7,0,0) builds on GCC. So the drop of GCC's product ceiling from 7 to 6 is confirmed, and its cause (the typing's linear scans) matches the diagnostics. The `///` comment agrees with every point I measured.
- Header cost (P3, P1), `#include <galoot/core.hpp>` alone, `-std=c++23 -O0 -c`, pinned to CPU 10, three repeats each: GCC 736.4 M → 818.9 M instructions (+11.2%), Clang 734.8 M → 895.9 M (+21.9%), spreads under 1.3 M. This reproduces the outcome's W10 figures. A copy of the head's `plan.hpp` with `plan_capacity`, `collect_result`, `type_terms`, and `defects_of` made function templates gives GCC 811.7 M and Clang 896.0 M: no change on Clang, −7 M on GCC. So their being non-template functions is not the cause; decision 0113's attribution to `std::span` instantiation remains an inference from an uncommitted trace, labeled as such there.

## Findings, most severe first

| # | Where | Finding | Suggested resolution | Blocks |
| --- | --- | --- | --- | --- |
| F1 | plan 0013 outcome, departure item 4; commit messages of `3bca0e6` and `38f2b30` | The record understates when the mechanical checks failed. Format-check failed at `3bca0e6`, `9430423`, `0fce606`, and `7818335` (the layout of `pair_capacity`, `plan.hpp:90`), not only at `7818335`; clang-tidy failed at `9430423`, `0fce606`, `7818335`, and `38f2b30`. `38f2b30`'s message says lint and format-check "pass here"; lint does not pass there (it passes from `04c12c1`, as the outcome says). `3bca0e6`'s message reports lint on the header sets only, where the cast finding does not appear. W2's "tests passing" held at every commit (table above), so this is a C20 accuracy defect of the record, not a broken unit. | Correct the outcome's item 4 in a records commit, with this table's revisions; history is not rewritten. | Fix before merge (records only) |
| F2 | `test/core/plan_test.cpp`, the wedge at the widest algebra | Decision 0110 item 2 chose `in_algebra` for the wedge's range check because it handles N = `mask_bits`, but no test uses the wedge at N = 32: "product_plan accepts the generators of the widest algebra" covers the product only. The mutant `wedge_range_shift`, a shift by N that is undefined at N = 32 and not a constant expression there, survives every test on both compilers. | Add the wedge counterpart of that test case, e.g. `product_plan<wedge, widest, tops, tops>` with its terms worked by hand (e1 ∧ e32 and e32 ∧ e1 with their signs, the two squares absent). Test-only commit (W9). | Fix before merge (test-only) |
| F3 | `test/core/plan_test.cpp`, bound tests | Two contracts are pinned at one side only. `typed_defects_of`'s result-sign check is tested with −1 alone, so `sign < 0` survives (a sign of 0 or 2 would pass); `pair_capacity`'s "1 for the wedge" has no direct test, and 0 survives because `plan_capacity` floors at 1. Both are equivalent on the present generator, so no wrong plan results. | Add a sign 0 and a sign 2 case to "The checks of a typed plan find each defect at its bound", and `CHECK(pair_capacity(wedge, e1, e2) == 1)` and for a shared generator. | No |
| F4 | criterion 4; "product_plan equals its typing run at run time" | Criterion 4 asks that the typed `full<4>` plans of five forms, both operations, equal the run-time typing. At run time only rigid(3) is compared (with `point` × `line`); R(4,0,0), R(1,3,0), conformal(2), and the degenerate form are compared with direct calls inside a constant evaluation. Their run-time typing is covered by C1 against the test's own rule, so the gap is only compile time against run time for those forms. Disclosed in the outcome's table, not in its criteria list. | Extend the run-time case to the other four forms through metrics read via `volatile`, as WU6b's case does for the product, or record the reading. | No |
| F5 | `include/galoot/core/plan.hpp:25` to `:36`, `:504` (plausible) | Two comment statements say slightly more than is shown. (a) "the result set holds exactly the blades that the product can make nonzero" holds for a scalar type in which no nonzero integer coefficient vanishes (any of characteristic zero); in a dense form a coefficient such as 2 would vanish in a scalar of characteristic 2. (b) "A program reads a plan through product_plan, a variable template, so each translation unit evaluates a plan once" credits the variable template with what the `static constexpr` locals provide on Clang, and the `///` "Guarantees: evaluated once" is tested on Clang 23 only (GCC by its call cache, by design). | Optional wording, e.g. "for scalars in which no nonzero integer vanishes" and "with its generator's locals static, so ...". A header comment change needs the delta review of decision 0094 item 3. | No |
| F6 | decision 0113; owner open question 3 | For the owner, not the author: the header cost of `plan.hpp` to every includer of `core.hpp` that uses no plan is +11% (GCC) and +22% (Clang), reproduced; GCC's `full<7>` ceiling drop reproduced; typing adds +54% to +65% to the plans of the fixed P2 use. All reported with causes; no bound applies (C14). My template experiment above rules out one candidate remedy. | Owner decides whether P3's "little beyond parsing" needs a measured follow-up. | No |

## Criteria (WU7a)

| # | Status | Evidence |
| --- | --- | --- |
| 1 | Met | C1 over the 42 forms passes on gcc-debug, clang-debug, lint, gcc-asan at `0607515`; the reference order (`comes_before`) is the test's own; my 20 independent plans agree |
| 2 | Met | C3: rule, top-grade part, metric independence, Grassmann case; mutants of the rule killed |
| 3 | Met | C4, every triple of the 42 forms |
| 4 | Met in part | hand values hold (re-derived); compile-time typing equals direct calls for five forms, run time for rigid(3) only (F4) |
| 5 | Met | golden wedge case, four forms |
| 6 | Met | each typed check fires on a mutant (author's record; my `result_by_mask`, `collect_insert_before_last`, `typed_*` mutants confirm several); bound tests exist, with two one-sided bounds (F3) |
| 7 | Met | migration diffed and token-compared above |
| 8 | Met (spot-checked) | boundary points agree with the comment and decision 0113; the GCC drop reproduced with its cause |
| 9 | Met (clang-debug and lint by me; clang-release and clang-asan by the author's rows) | ratios 0.0007 to 0.0008 at head; 0.554 and 1.628 on the plain-locals mutants; 0.008 on static locals called three times |
| 10 | Not rechecked | record exists (`out/growth-d6ea717/`); my header-only measurement agrees with its baselines |
| 11 | Met (author's seven presets at `86226f7`; mine: four presets at `0607515`) | header sets on gcc-debug, clang-debug, lint built clean by me; W10 rows present |

## Revisions

- Head reviewed: `0607515`; code and tests: `d6ea717`.
- Per-commit builds: `3bca0e6`, `9430423`, `0fce606`, `7818335`, `38f2b30`, `04c12c1`, `11ad0aa`, `d6ea717`.
- Before-headers for limits and header cost: `0bbb968`.

## Delta review of 0098409 (comment-only header change after the review)

Reviewed by the coordinating agent (Claude Sonnet 5.5), 2026-11-01. The diff of `include/` from `d6ea717` to `dd875af` contains no line other than comments. The new wording of `plan.hpp` states the result-set claim only for scalar types in which no nonzero integer vanishes, and credits once-per-unit generation to the static constexpr locals, checked on Clang only. Both match what the review found (F5) and what `core.plan_evaluated_once` tests. No finding.
