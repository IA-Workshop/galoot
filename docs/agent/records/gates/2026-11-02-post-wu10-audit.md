# Audit report: everything merged since M1, through WU10b (M2, WU9, WU10a, WU10b)

- Revision audited: `develop` at `7729c914286f` ("Record the WU10b reviews"). Scope: the 83 commits from `a75ce5e` (the M2 gate report's revision) to `7729c91`, with M2's own record (`records/gates/2026-11-02-M2.md`, accepted by decision 0163), plans 0018 and 0019, decisions 0159 to 0170, and the reviews of WU9, WU10a, and WU10b.
- Auditor: Claude Opus 5.5 (`claude-opus-5-5`), `auditor` role file, 2026-11-02, in a detached worktree of its own under the session scratchpad (removed at the end). Placed by decision 0163 item 2. Read-only toward the repository: no branch was edited.
- Status: submitted to the owner. Not accepted.
- Verification status (V10): the full suite was run at the audited revision on all seven presets from new empty trees and passed; the trace gate, conventions check, golden check, citation, marker and golden-fact tests, the format check, and `tools/runlog check` ran at the same revision and passed. Validation: not performed by this audit; the units report validation for rigid(3) (the antiscalar as identity of ⟇, point-on-plane) and none for the sums and scaling (plan 0019 Outcome), and the M2 gap for non-diagonal antiproducts stands (decision 0163 item 3).

Severity used below: **blocking** blocks a release until the owner decides; **major** is a departure from a ratified provision or owner ruling that the units did not declare, or an owner question left where the owner may not see it; **minor** changes no result; **info** is a trend or an observation with no provision behind it (C19). Counts: 1 blocking, 4 major, 14 minor, 8 info.

## Departures and open items (C20)

Of the audit itself:

1. My runs were logged to my own logs (`scratchpad/audit-runs.tsv`, `scratchpad/audit-cost.tsv`), not to the repository's `runs.tsv`, and their selections are `all`, not `gate:all`. Git, grep, and file inspection did not go through `tools/timed` (they are not build or test steps).
2. The public-name check is a text scanner over the clang-formatted headers (declarations at column 0 outside `namespace detail`). A first attempt through Clang's JSON AST was abandoned because my walker lost names. The scanner's completeness is inferred, not proven.
3. The pinned measurement of the `core.hpp` includer (CPU 4, three repeats) ran after my own heavy runs had finished. I did not check whether other workstreams were loading the machine. The figures are instruction counts, which do not depend on load.
4. The job pool reported "14 of 23 slots free ... no client holds the pool, so 9 slots are lost" before and after my runs. I did not restart it, as instructed.

Found, most severe first (full list with evidence in "Findings" below):

1. **Blocking:** the `clang-measure` preset cannot build `core_product_test` or `core_identities_test` at `develop`. No merge-check preset builds the configuration most Clang users ship (optimized, without libstdc++'s assertions). The owner was told about this only in a merge commit message (B1).
2. **Major:** WU9's fix pass on `product.hpp` merged without the delta review that decision 0094 item 3 requires and that the WU9 reviewer asked for. Nothing records the departure (M1).
3. **Major:** WU9, a unit of M3, was implemented before the M2 gate was submitted or accepted. This repeats departure 1 of the M0–M1 audit (W11), and nothing records it (M2).
4. **Major:** `agent-cost.tsv` has no row after WU8b. About ten sub-agents of WU9, WU10a, and WU10b are unrecorded, and nothing declares the gap (W10) (M3).
5. **Major:** four owner questions exist only in merge commit messages and review resolutions. No decision record, plan question, or handoff holds them, and the latest handoff still says the next unit is WU9 (M4).

## Gate facts (from the audit, at 7729c914286f)

- P9 matrix, new empty trees, all seven presets (configure, build, and test each exit 0; no `FAILED` build line): gcc-debug 301 of 301 tests (build 18.6 s, test 6.8 s); gcc-release 301 (24.8 s, 6.6 s); gcc-asan 293 (103.8 s, 3.8 s); clang-debug 303 (18.1 s, 4.8 s); clang-release 303 (26.6 s, 4.3 s); clang-asan 303 (76.7 s, 4.6 s); lint 303 (117.4 s, 4.0 s). The counts match the unit's last runs: gcc-debug 301 and clang-asan and lint 303 at `6fb7827`, plus the two compile-fail tests added after `97e94f8` in the other presets. The header sets (15 units) build on gcc-debug, clang-debug, and lint. The lint build runs `format-check`.
- Measurement presets: gcc-measure 293 of 293 pass. clang-measure fails to build `test/core/product_test.cpp` and `test/core/identities_test.cpp` (`plan.hpp:553: constexpr variable 'count' must be initialized by a constant expression`, then `no matching function for call to 'geometric_antiproduct'` and invalid-operand errors on `mv<cga3, full<5>, long>`). 293 of 295 pass and 2 are not run. clang-measure-o3 and gcc-measure-o3 were not run.
- Trace gate (W5 command, from the root): exit 0. Pinned items 1.1 to 1.7, 2.1, 3.1, 4.1, 4.2, 4.5, and 7.1; items with no test 3.2 to 3.4, 4.3, 4.4, 5.1 to 5.7, 6.1 to 6.4. The CONVENTIONS 1.8 facts "part never checks what it drops" (`mv_test.cpp:2253`) and "the negation of a multivector negates each coefficient" are pinned.
- `conventions_check.py`: "all convention checks passed (seed 20261031)". `golden.py --check test/reference/golden.hpp`: matches. `tests.citations`, `tests.reference_markers`, and `tests.golden_facts` pass in every preset. My own grep of `include/`, `test/` (outside the fixtures that hold citations on purpose), `tools/`, `cmake/`, and the build files finds no citation of a record, plan, decision, provision, review, or agent (S11). `clang-format --dry-run --Werror` over every C++ file under `include/` (except `include/ext`) and `test/`: clean. `tools/runlog check`: exit 0 ("1 eras match the history; 1727 kept rows present once; no removed row present").
- Public interface: 88 public declarations in `include/galoot/core/*.hpp` (operators counted per overload). Each has a `///` comment on the line before it. Every public name added since M2 appears in at least one test file: `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge`, the six `*_layout` names and `sum_layout`, `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`, `part`, `has_name`, and the operators `* ^ % & ~ ! + - /`. No `gp` or `antigp` is left in `include/`. The test-only names keep them, as decision 0159 item 3 allows.
- Header examples: I compiled and ran the examples in the head comments of `product.hpp`, `unary.hpp`, and `linear.hpp` on g++ 16.2.1 and clang++ 23.1.4. They compile and give the values their comments claim. The layouts that the `*_layout` and `sum_layout` comments name for rigid(3) hold as `static_assert`s on both compilers.
- Size: the core is 11 headers, 4,811 lines (2,590 of code), against 7 headers and 3,596 lines at M2 (+34%). The tooling is `tools/` 2,260 lines, the `test/reference` scripts 1,462 (plus the generated `golden.hpp`, 2,296), `test/CMakeLists.txt` 1,586 (+279 since M2), `test/tools` 2,302, and `cmake/` 155. That is about 7,800 lines, 1.6 times the core, against more than twice the core at M1. The suite has 184 compile-fail sources.
- Include graph: acyclic, standard headers and core headers only (C13). `product.hpp`, `unary.hpp`, and `linear.hpp` each include `mv.hpp`, and none includes another. `core.hpp` includes all nine public headers.
- Cost of the `core.hpp` includer since M2 (pinned on CPU 4, `-O0 -g` with the test flags, three repeats, spread under 0.3%): GCC 929.2 M to 944.2 M instructions (+15.0 M, +1.6%); Clang 937.0 M to 954.6 M (+17.6 M, +1.9%).
- Attribution: 82 of the 83 commits carry a `Co-Authored-By: Claude …` trailer (56 Opus 5.5, 26 Sonnet 5.5). The exception is the merge `a5e8c14`.
- `master` is still at `9a4e0cd` with no tag (W13: nothing reached it).

## Findings

### B1 (blocking). Clang optimized builds and the measurement presets fail at a conformal(3) full<5> pair; the merge checks never build that configuration

- Provisions: P9 (release: the full suite in every supported configuration), W13 (no open blocking defect), P1 and P6 (measurement builds), C18 (a choice that affects a reported result is recorded).
- Evidence: my clang-measure build of `7729c91` (log above). Plan 0019, WU10a review F1 and the configuration gap that follows it: "every test preset defines `_GLIBCXX_ASSERTIONS` … so no preset of the suite builds what most Clang users ship". Delta review D4 says the measurement presets cannot build the two units. The merge message of `81ffedf` says "Deferred to the owner". `product.hpp` documents the limit for users (lines 44 to 52 and 346 to 360). No decision record, plan question, or handoff item holds the question.
- Why it matters: a user of the planned cga3 model who builds `full<5> % full<5>` with Clang at `-O2` gets a step-limit failure. Every merge check passes because every test preset turns on libstdc++'s assertions, which happen to keep the plan under the limit. A release run in every configuration fails today.

### M1 (major). WU9's review-fix pass merged without a delta review

- Provisions: decision 0094 item 3 ("A delta review is required after any fix pass on core code"), W5.
- Evidence: the WU9 review (`records/reviews/2026-11-02-wu9-products.md`, Outcome) ends "A fix to the header after this review needs a delta review." Commits `f4eace5`, `7fdd8d1`, `ba85930`, and `08c70ba` change `include/galoot/core/product.hpp` after the review. They restructure `reject_product`, its return types, and the operand conditions. They merged in `19273a6` with no delta review. Plan 0018's Outcome and the merge message do not mention one.
- Mitigation (inferred): WU10a's blind and delta reviews read `product.hpp` again in full, as it stood after WU10a's own changes. So the WU9 fix code was reviewed later, in changed form.
- Smaller cases: after the delta reviews, `b6d0c60` (by the coordinator: a comment in `mv.hpp`, WU10a D1) and `48175d1` and `cd146a4` (WU10b D1 and D2: two compile-fail tests and comments in `linear.hpp` and `mv.hpp`) merged with no further review. These are comments and tests only, so they are minor on their own.

### M2 (major). WU9 ran ahead of the M2 gate

- Provision: W11 ("A module waits for the gates of the modules it depends on"). Decision 0162 item 10: "M3 formally depends on it."
- Evidence: plan 0001 places WU9 in M3. WU9 was claimed at `189418d` (03:42 −0500) and implemented in `5c5188f` to `dd8a77e` (04:14 to 04:26). The M2 gate report was submitted at `5d1347a` (05:52) and accepted at `5db4c7e` (05:54). Plan 0018 does not declare this. The M0–M1 audit reported the same pattern as its departure 1.

### M3 (major). Agent cost not recorded for any agent since WU8b

- Provisions: W10 and P13 ("Agent work records its agent cost"), W3 (a plan's cost estimate is to be compared). Plan 0019 section 9: "Recorded in `docs/agent/records/agent-cost.tsv` as each agent finishes."
- Evidence: the last commit to `agent-cost.tsv` is `dec7c6b` (01:09, WU8b). There are no rows for the WU9 blind reviewer, the WU10 planner, WU10a's implementer, test author, blind reviewer, and delta reviewer, or WU10b's implementer, test author, blind reviewer, and delta reviewer. Plan 0018 says "no sub-agent implemented or tested the unit, so no cost row", but its reviewer was a sub-agent. Nothing declares the gap.

### M4 (major). Owner questions held only in commit messages; the handoff is stale

- Provisions: C20 (a report opens with what the owner must act on), W3 ("Nothing a later session needs is left only in agent memory, the workbench, or an uncommitted file", read with its purpose), W8 (work that needs an owner decision is reported).
- Evidence: these questions appear only in merge messages or review resolutions:
  - (a) a no-assertions optimized Clang build in the merge checks (`81ffedf`; B1);
  - (b) the measurement presets cannot build two units (`81ffedf`; WU10a delta review D4);
  - (c) CONVENTIONS 4.5 reads "what they drop is meant to be nonzero" where "may be nonzero" is meant (WU10b review F6, `a5e8c14`);
  - (d) whether W2's exception covers a run of test commits before their code (`a5e8c14`).
- `AGENTS.md` names `records/handoff-2026-11-02.md` as the latest handoff. That handoff says "Next unit: WU9". Plan 0001's 2026-11-01 addendum says the next unit is M2.

### Minor findings

1. **P13 gate naming.** The ratified P13, applied to the governance in `c5f6742` (merged `ab7af27`), says "The runs of the checks of a merge or a milestone gate are named `gate:<selection>`". The three merges after it (`19273a6`, `81ffedf`, `a5e8c14`) ran their checks as selection `all`, and `runs.tsv` has no row whose selection starts with `gate:`. Those rows survive compaction anyway, because they are configure, build, and test runs of named presets at committed revisions. No result is affected.
2. **W5 for the run-log merge.** `ab7af27` (`feature/runlog-0170`) changed `tools/runlog`, `test/tools/runlog_test.py`, a test filter in `CMakePresets.json`, plan 0001, and PERFORMANCE.md. It has no review record. Lint, and with it the S3 and S4 checks, was deferred to the next merge (the message says so; P9 allows the deferral, but W5 names no exception for the review). The PERFORMANCE.md wording itself was ratified by the owner (decision 0162 item 3).
3. **W7 and C17 attribution.** The merge `a5e8c14` of WU10b into `develop` has no agent trailer.
4. **W10 and P1 cost records.** WU9 recorded no before and after costs of the affected suites. It recorded only the smoke test's wall time and an instruction count it calls "a failed measurement" (`records/mutations/0018-products/README.md`, Cost), and its outcome defers only the P2 figure. By difference from my measurement, WU9's share of the includer cost is about 2.9 M instructions on GCC and 3.0 M on Clang. That is an inferred figure: the total since M2 minus WU10a's +5.2 M and +6.7 M and WU10b's +6.9 M and +7.9 M. The committed P1 record `records/performance/compile.tsv` has had no row since WU8b (`8c67a20`). The four new test units (`product_test`, `unary_test`, `linear_test`, `identities_test`) have never been in it, because decision 0080's copy step was not run at any merge since M2. WU10a and WU10b did record pinned before and after costs in `antiproducts-cost/` and `linear-cost/`.
5. **Records out of step (W3, W6).**
   - Plan 0019's status line says "WU10b implemented on its branch, awaiting review", and its WU10b outcome opens "Not reviewed yet; not merged". WU10b merged at `a5e8c14`, and the plan has no line that records the merge.
   - The WU10b review records were committed to `develop` in `7729c91`, one second after the merge, not with the change on its branch (W5: "recorded with the change"). The first reviews of WU10a and WU10b carry no Resolution section; their resolutions are only in plan 0019.
   - PERFORMANCE.md's status line names only the P5 amendment. P7, P8, P9, and P13 were amended on 2026-11-02 (decision 0162 item 3). Decision 0170's status line still says its wording "takes effect only when the owner ratifies it", and its addenda still say "proposed", with no appended note of the ratification.
   - The M0–M1 gate file still says "Not accepted". Decision 0163 item 4 settled the question but did not update the file.
   - Decision 0162 item 11 records the merged branches as "cleaned up (done by the owner)", but the merged `feature/*`, `research/*`, and `review/*` branches are still listed (`git branch`).
6. **W2, undeclared failing commits in WU9.** At `7fdd8d1` the tests fail on every preset and the build fails on clang-debug, clang-release, clang-asan, and lint (run rows with exit 8 and 1). At `ba85930` lint fails. Plan 0018 mentions "three earlier runs that failed" but does not list these commits as W2 departures. The departures of WU10a (`3a22686`, `fdc1cb6`, `0d3d990`, `c120caa`, `a53d386`, `5cb362a`) and WU10b (`1aa4e70` to `2bcd67d`, and `b1523aa`) are declared.
7. **W6 lessons.** No lesson has been written since M2 (the last is 0066), although these surprises happened: the Clang step-limit configuration gap (B1); a run reported as clean that was `-dirty` (WU10a departure 10); fifteen commits in a row that do not build; a same-type sum that merged its layout with itself, which no test pinned (WU10b F2); and the agent-cost lapse (M3).
8. **W12 header examples.** The examples in the header comments of `product.hpp`, `unary.hpp`, and `linear.hpp` are not compiled as tests. WU10b departure 8 says its scratch probes of them were not committed. I compiled and ran them, and they are right at this revision. Whether header comments count as "user documentation" under W12 is a reading.
9. **S13 operator bodies.** `operator*` and `operator%` on two multivectors are not one-line aliases: each repeats the coefficient check so that its message names the operator. The scaling operators, `operator/`, and unary `operator-` carry their own loops. Plan 0019 R2 said each body would be "one call of a function in `detail`". Decision 0167 weighs public names for these operators but not the form of their bodies, and the owner has not ruled on either reading of S13.
10. **S7 accept and reject assertions.** The new `detail` constraints `scalar_of_another_type` and `scalar_has_division`, and the five ring clauses, have no `static_assert` of their own for an argument they accept and one they reject. This is declared as WU10b departure 3 but has no ruling.
11. **S6 and naming between units.**
    - `product.hpp` still includes `<concepts>`, though nothing in it has used `<concepts>` since `3d7907a` moved the operand checks to `mv.hpp`.
    - `linear.hpp` uses `coefficient_type`, which `blade_product.hpp` defines, only through `mv.hpp`.
    - `detail::product_operands` now also gates sums (its comment says "a product or a sum"), so its name is narrower than its use.
12. **Knowledge base.** `knowledge-base/cpp-language/consteval-limits-per-compiler.md` (`f805f68`) gives the workaround "convert one operand to the other's type first, so that the merge sees equal sets". WU10b review F2 showed that a merge of two equal `full<8>` sets also fails on Clang. Since `7e22638`, a same-type sum merges nothing at all. The note predates the fix, and its stated mechanism is wrong.
13. **C20 and P13 run evidence.** The claims that WU9's final runs at `08c70ba` and WU10a's post-review runs at `b6d0c60` came "from new empty trees" have no configure rows in `runs.tsv`; every other final run of these units has them. WU9's trace-gate and `tools/runlog check` runs have no rows. The build times fit new trees, so the claim is plausible but not backed by the log.
14. **V10 in plan 0018.** WU9's outcome lists its evidence but gives no separate validation status.

### Info

1. **Compile-cost growth (P13: noted, not judged).** In shared mode, summing retired instructions over all units:
   - gcc-debug went from 282 G instructions over 160 units (the WU8b gate, `6b40bed`, committed `compile.tsv`) to 739 G over 212 units (×2.6). clang-debug went from 295 G to 754 G.
   - `identities_test.cpp` alone takes 295 G on GCC and 300 G on Clang at `-O0`, 4.5 times `mv_test.cpp` (65.5 G, +1.6% since M2). Under gcc-asan it takes 1,309 G, 103 s, and 3.5 GB of memory, 52% of that preset's compile work.
   - Merge-level builds went from 23.4 s (gcc-asan) and 34.4 s (lint) at WU9's final run to 104 s and 117 s now.
   - The `slow` label keeps the unit out of selections before a commit, but the unit also carries the labels `core.product` and `core.unary`, so any change to either selects it.
2. **Two message styles.** The messages added since M2 follow one rule: the operation as written, then the mistake, then the fix after a semicolon, with no final period. The M1-era messages (`blades:`, `merge:`, `index_of:`, `metric:`) and the run-time assertions (`a / s: an integer scalar cannot divide by zero`, `truncate: a dropped coefficient is not zero`) have no fix clause. `mv`'s default-constructible message ("must be default constructible, copyable, and assignable") overlaps its separate copyable message and does not match the five "has no X" ring messages of WU10b. WU10b delta review D4 already defers a diagnostics review to the release.
3. **R3 result layouts** hold consistently in `product.hpp`, `unary.hpp`, and `linear.hpp`:
   - the sign maps, negation, scaling, and same-type sums return the input's type;
   - the products, the complements, and two-layout sums return the canonical set of the blades they reach.

   I checked this by reading the headers and with the layout `static_assert`s above. The seven public layout names (six from decision 0166, plus `sum_layout` from decision 0167) became public interface by implementer decisions, without an owner ruling. Under W13, removing them later must be announced.
4. **Duplicated test helpers.** `random_integers` is defined in five test units (two of them draw from −3 to 3, three from −9 to 9), and `indices_of`, `blade_name`, and `blade_count` in four each. The algebra tags are defined again in each unit: `rga3` seven times, `cga3` five times, and `cga2` three times, plus `conformal2` in `plan_test.cpp` for the same algebra. S16 tolerates short repetition, and `test/support/` exists for shared helpers.
5. **Model of the WU9 author.** WU9's core code was written by the coordinating agent at Sonnet 5.5, while plan 0018 section 9 planned an Opus implementer. `AGENTS.md` allows Sonnet for small, tightly scoped work, and WU9 is size S. The plan declares only "no sub-agent".
6. **M3 exit criteria.** M3's "V12 double tests with forward bounds" are on no plan (plan 0019 R5 leaves them "for the coordinator").
7. **C8 text.** The owner confirmed a reading of C8 (decision 0168), but the constitution's text is unchanged, so a reader of C8 alone does not find the reading (WU10b review F6).
8. **Job pool.** 9 of 23 slots are lost, with no client holding them. The owner's restart is below.

## Conformance by provision (scope since M2)

| Provision | Status | Evidence |
| --- | --- | --- |
| C3, C4 (derived, one mechanism) | Met | New operations are callers of `detail::apply` or loops over coefficients. The sum kernel is per-basis data (decision 0167, plan 0019 R4). |
| C6 (cost follows use) | Met with the recorded exception | The antiproduct growth exception of decision 0141 is unchanged. Through public `%` it is +106.8% on GCC and +356.5% on Clang, n = 4 to 16 (plan 0019 Outcome). |
| C7, C8 (exact; nothing hidden) | Met | `part` and `truncate` are documented and pinned. The C8 reading is ratified in decision 0168 (Info 7). |
| C9 (errors in the algebra's terms) | Met for the new rejections | Pinned by `expect_output` tests. The Clang step-limit failure does not name the product (B1). |
| C10, V6 (conventions under test) | Met | CONVENTIONS 1.8 rows pinned; trace gate exit 0. |
| C12, S16 (simplicity) | Met, with minor items | Minor 11, Info 4. Tooling has shrunk relative to the core. |
| C13 (layering) | Met | Include graph above. |
| C17, W7 (attribution) | Minor gap | Minor 3. |
| C18 (decisions) | Partly met | Decisions 0164 to 0169 recorded and amended by dated appended sections (0167). B1 and M4 have no record. |
| C20 (honest reports) | Partly met | Departures were well declared in WU10a and WU10b. M1, M2, Minor 6, and Minor 13 were not declared. |
| W2 | Departures declared except WU9's | Minor 6. Owner question M4(d). |
| W3, W6 (plans, records) | Partly met | Minor 5, Minor 7, M4. |
| W5 (review) | Partly met | Reviews exist for WU9, WU10a, and WU10b; no delta review of WU9's fixes (M1); no review of `ab7af27` (Minor 2). |
| W9 | Met | Tests came in separate commits before the code in WU10a and WU10b. WU9's combined commit and WU10b's `64598f3` were declared and listed. |
| W10, P1, P7 | Partly met | M3; Minor 4. No toolchain change since M2 (GNU 16.2.1, Clang 23.1.4 in every row). |
| W11 | Not met for WU9 | M2. |
| W12 | Minor gap | Minor 8. |
| P9, P10, P13 | Merge checks met for WU9, WU10a, and WU10b; naming not | Minor 1, Minor 13. |
| S3, S4, S5 | Met | Format, lint, and both sanitizer presets pass at the audited revision. |
| S6, S7, S11, S13 | S11 met; S6, S7, and S13 minor | Minor 9 to 11. |
| V2, V7, V14 | Met where read | New randomized tests seed from doctest's `rand_seed` and print it (`unary_test.cpp:498`, `product_test.cpp:349`). |

## Decisions for the owner, with recommendations

1. **B1, Clang optimized builds.** Choose what a release checks and what users are promised for full<5> in a five-generator algebra:
   - (a) add an optimized, no-assertions Clang build of `core.product` and `core.identities` to the merge checks;
   - (b) make the two tests' conformal(3) full<5> pair independent of the configuration, smaller or behind a measured `-fconstexpr-steps`, so that the measurement presets build the whole suite;
   - (c) consider whether this user-visible limit on the planned cga3 model's full multivector counts as decision 0141's "production issue" trigger, which would bring decision 0140 (the antiproduct inside the coupling closure, which reduces the plan's steps) forward.

   Recommended: (a) and (b) now, recorded in a decision; (c) at the cga3 model's plan; and the limit listed in the release record beside the C6 exception.
2. **M1, delta reviews.** Recommended: accept WU9's unreviewed fix pass as covered by WU10a's full review of `product.hpp`, recording why. Confirm that decision 0094 item 3 covers comment-only changes to `include/` after a delta review (recommended: yes; such a review is cheap).
3. **M2, W11.** Recommended: accept WU9's start before M2's acceptance, and direct that no unit of a milestone is claimed before the gate it depends on is accepted.
4. **M4(d), W2.** Recommended: amend W2 so that a contiguous run of test commits may precede the code that makes them pass, when each commit message says so and the review request lists the range. The separation is what keeps the test author independent of the implementer (W9).
5. **M4(c), CONVENTIONS 4.5.** Recommended: version 1.9 with "what they drop may be nonzero" (WU10b review F6). Separately, amend C8 to state the reading ratified in decision 0168.
6. **S13 and S7 readings (Minor 9 and 10).** Recommended: a STYLE amendment saying that an operator is an alias in meaning and may repeat a check so that its message names the operator, and that the operators of 7.1 rows without a named function may hold their own loops. Require the S7 accept and reject `static_assert`s for the new `detail` constraints in the next unit that touches `linear.hpp`.
7. **Public layout names (Info 3).** Recommended: confirm the seven `*_layout` names as public interface. Otherwise move them to `detail` before WU16 adds model aliases that would spell result types through them.
8. **M3 and Minor 4, cost records.** Recommended: the coordinator backfills `agent-cost.tsv` from the harness reports where they still exist and marks the rest "not recorded". The next merge runs decision 0080's copy step on new trees, so that `compile.tsv` holds the four new units. Merge checks are named `gate:<selection>` from the next merge on.
9. **Info 1, compile cost.** Recommended: split `identities_test.cpp` by form family or move its n = 16 sparse cases into their own unit, with before and after costs. It is now 40% of a debug build's compile work, 52% under the sanitizers, and is selected by any change to the products or the unary operations.
10. **Info 6, M3.** Recommended: plan the V12 double tests with forward bounds as part of WU11, or as a small unit beside it, so that M3 can close.
11. **Records (Minor 5, Minor 7, M4).** Recommended: one records unit that closes plan 0019, fixes the PERFORMANCE.md and decision 0170 status lines, updates the M0–M1 gate file and plan 0001, writes a current handoff with the owner questions above, adds the lessons of Minor 7, and corrects the knowledge-base note (Minor 12).

Owner actions (commands):

```bash
# Recover the 9 lost job slots, when no agent is building (it refuses while clients hold it):
cd /home/brianm/projects/utils/math-core/galoot && tools/jobpool status && tools/jobpool stop && tools/jobpool start 23

# List the merged branches that decision 0162 item 11 records as cleaned up:
git -C /home/brianm/projects/utils/math-core/galoot branch --merged develop | grep -E 'feature/|research/|review/'
```

## Not audited, and why (C20)

- The mathematics was not derived again. I relied on the suite, the golden values, and the reviewers' independent references; my checks of values were limited to the header examples and layout claims above.
- Mutants were not run again. The mutation READMEs were read for their counts only.
- No assembly read (P4) and no run-time benchmark (P6). The P2 growth benchmarks were not run again; only the `core.hpp` includer was measured.
- `clang-measure-o3` and `gcc-measure-o3` were not run. That they fail as `clang-measure` does is inferred for the Clang one.
- The review records were read for their headers, departures, outcomes, resolutions, and selected findings, not in full. The research notes and V9 were not audited, since no research note changed since M2.
- The M2 gate's own evidence at `5bcd38b` was not reproduced; the head run above supersedes it for the current code.
- The knowledge base was read only for the two commits that WU10 added (`9155e75`, `f805f68`).
- Timings are in shared mode except the pinned includer measurement. The P14 comparison rules apply.
