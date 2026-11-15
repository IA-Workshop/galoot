# 0042 The M7 gate and the release of core 1.0.0 (WU18, M8)

- Status: proposed 2026-11-11 by Claude (agent, Opus 5.5), planner. From `develop` at `6de1beb`. Committed straight to `develop`, which is the claim (W3, W7). Units U1 to U10 are done, and U11's records are on `feature/release-1.0.0` (2026-11-12; section 11). Open: the coordinator's merge of U11 into `develop`, the owner's CONVENTIONS 1.18 edit, the walkthrough, and the merge to `master` and the tag (section 8). Status corrected 2026-11-12 by U11, from the release audit's m7; it read "No unit below has started" until then.
- Source of scope: plan 0001 (sections 4 and 6: the M7 and M8 rows, DoD-2, DoD-4, DoD-18, DoD-22 to DoD-29; section 8: WU17, WU18); plan 0038 (WU17) and its rulings (decisions 0233, 0234, 0236 to 0239); plans 0040 and 0041 and their outcomes; decisions 0240, 0241, 0243 to 0246. **Decision 0246 binds this plan**: accelerate; the E1 and E2 reruns and WU17 parts G and H are deferred past 1.0; the release record states the date of every baseline it cites; a green matrix, the V10 table, the release audit, the owner's walkthrough, and the owner's merge and tag are not waived.
- Drafts this plan finishes: `records/release/release-record-draft.md`, `v10-table-skeleton.md`, `dod-2-inventory.md`, `plan-closures.md` (decision 0240 item 11). U9 finished the first two as `records/release/release-record.md` and `v10-table.md`. The next free decision number is 0247.
- Shared resources: the job pool reads "not running" at `6de1beb`. A unit that builds reads `tools/jobpool status` before and after; it starts the pool with `tools/jobpool ensure` only when the status says it is stopped, never twice, and never stops it. Timed runs take the lock through `math-tools/exercises/performance/pe-common/lock.sh take` and `release`. No processor configuration is touched or proposed. No web request, no third-party contact, no Terathon source.

## 1. Departures, ambiguities, and readings (C20)

1. **DoD-2 helper split, listed by the brief as post-1.0.** Decision 0241 item 6 ruled that `has_name`, `same_rows`, `basis_change_like`, `index_of`, `sum_layout` and `max_generators` move to `detail` (none is named by CONVENTIONS: grep at `6de1beb`). After 1.0.0 such a move removes public names, which semantic versioning (W13) makes a 2.0.0 change. `same_rows` cannot move at all while the deleted `==` points users to it (`basis_change.hpp:177`). The plan does not schedule the move and asks the owner to supersede 0241 item 6 (O1).
2. **`GALOOT_FORCE_INLINE` in the DoD-2 inventory, listed by the brief as post-1.0.** It is a public macro, documented in the README and the core guide, and DoD-2 requires the inventory to be exactly the public interface. Adding its row is one line of records, so it is in U6, not in the backlog.
3. **Four backlog items have no record in this repository**: placement search, the CTest label anomaly, `tools/mutate` running CTest in parallel, and the knowledge-base source-line rule (the last is decision 0241 item 8, which is about math-core's knowledge base). They are listed in section 9 as the brief names them.
4. **The released revision and the matrix revision.** The V10 table and the release record are filled from the matrix, so they are committed after it. The reading followed: the matrix runs at the release commit R; records-only commits follow; the owner tags their tip T, and the release record states that `git diff --stat R T -- . ':!docs/agent'` is empty and that the cheap gates whose inputs are records reran at T (O4).
5. **DoD-22 in shared mode.** Decision 0233 item 5 chose pinned serial builds; decision 0246 item 2 defers them. DoD-22's rows at the release revision come from the release matrix's debug trees, shared mode, by decision 0080's copy step, and the release record says they are not comparable with pinned rows (P14).
6. **"Every supported configuration" (P9, release row)** is read as all eleven named presets, the two `-O3` measure presets included, although no gate has run them. O5 offers the narrower reading.
7. **Checks of this commit.** The plan is records only (P8: documentation, no tests). The citation check reads code paths, not `docs/agent/`, so it cannot be affected; it was not run.

## 2. Goal and definition of done

Close M7 on the evidence that exists, then release core 1.0.0 with every check run on the released code. Done when: (1) the owner's M7 acceptance is recorded in plan 0001 and a decision; (2) the release matrix is green on all configurations at R; (3) the V10 table has a filled row for every requirement, gaps stated; (4) the release audit has no blocking finding; (5) the owner has walked through the release and merged and tagged it; (6) this plan's outcome is recorded.

## 3. M7: what is open

### 3.1 WU17 (plan 0038), parts A to F, at `6de1beb`

| Part | State | Where |
| --- | --- | --- |
| A, `cost.py comments` list (0230 item 1) | Done | merged in `43cc6da`; review `f8440ba` |
| B, derivations and counts | Done, checked independently | `fd2ccd9`; decision 0234 |
| C, counting scalar, `rga3.op_counts_<mode>` | Done | `43cc6da`; reviews `26db481`, `7ca2be2` |
| D, harness, reference kernels (inline) | Done | `43cc6da`, `2de0d36`; reviews `4e1804e`, `2cba608`, `e32cda3` |
| E1, E2 (first run and inline rerun), E3 | Done at `18849b5` and `2de0d36`, before the forced inlining | `1e573ad`; decisions 0237, 0238 |
| F, Spike E, note 0027 | Done, checked independently; D8 ruled | `c31415d`; decision 0239 item 1 |
| E1 and E2 reruns, G, H | Deferred past 1.0 | decision 0246 item 2 |

Plan 0038 has no outcome and its status line says no work started (U1 closes it).

### 3.2 Spike E and D8: the disposition

D8 is decided: decision 0239 item 1 takes note 0027's option (a), under which `transform` keeps form A, DoD-4 stands on the declared-output check, and DoD-24's counts are form A's (decision 0233 item 2). No new decision is needed. The gate report cites 0239 and note 0027, section "Answer" item 7, and states DoD-4's branch.

### 3.3 Items owed at the gate by earlier rulings

- The `bulk_dual` exception, a short decision beside 0141 (0241 item 7; growth from 0237 item 5). U1.
- The errata-naming row of CONVENTIONS section 8 (0234 item 7, 0240 item 12): the coordinator gives the exact text in the gate report; only the owner edits CONVENTIONS. U3, then the owner.
- The two `apply` overloads with no target layout, listed for the audit (0241 item 1). U3.
- The evaluation counter deleted by the M7 snippets review (0233 item 7). U2.
- `basis_change.hpp` in the umbrella: decision 0240 item 6 tied it to part H, which is deferred. O2.

### 3.4 V10 table for M7, from the evidence at `6de1beb`

Corrected 2026-11-11 by unit U6, from the M7 gate audit's m1: the benchmark row's Gap cell, the Spike E row's Validation cell and the DoD-4 row's Evidence and Gap cells; the U1 gaps of DoD-22 and DoD-23, since closed, now name what closed them.

Performance rows are track-only (C14): "performed" means recorded; validation does not apply to cost. Seeds are given where a run has one.

| Requirement | Verification | Validation | Evidence | Gap |
| --- | --- | --- | --- | --- |
| DoD-22: per-TU time and peak memory, both compilers, release revision | Not performed at a 1.0 candidate; newest committed rows at `c799821` (2026-11-07), shared | n/a | `records/performance/compile.tsv` | Copy from the release matrix's debug trees (U8), shared mode |
| DoD-22: before and after for each core unit (W10) | Performed unit by unit | n/a | `records/performance/*-cost/`; `pre-release-core-pass/{cost,cost-public-ops,cost-d1,opt-out}/` | Index: `core-unit-w10-index.md` (U1). Gaps: WU9 (plan 0018), plan 0009 partial, accepted as past gaps (decision 0248 item 4) |
| DoD-23: fixed use against n = 4, 8, 16 | Performed at `18849b5`, `--cpu 4`, three repeats | n/a | `wu17-growth/README.md`, "Fixed use against n" | Before the forced inlining; antiwedge's use is empty for n ≥ 6 (its caveat 2) |
| DoD-23: n = 4 against 1, 4, 16 operations | Performed at `18849b5` | n/a | same, "Fixed n = 4 against distinct operations" | Before the forced inlining |
| DoD-23: growth exceptions | Antiproduct entered against 0141; `bulk_dual` found | n/a | `wu17-growth`; decisions 0141, 0237 item 5 | `bulk_dual`: decision 0247, beside 0141 |
| DoD-24: counts equal the reference's | Performed, passed in every preset at `2438604` (gate 3), checking on and off | n/a | `rga3.op_counts_<mode>`; `golden.py`; `records/spikes/spike-e/COUNTING-RULE.md` | Form A's counts, by 0233 item 2 |
| DoD-24: no call at `-O2` | Performed, passed at `2438604`: 52 functions, both compilers, in one unit and alone | n/a | `rga3.kernels_inline`, `rga3.kernels_inline_alone` | x86-64 and `-O2` only; conformal(3) calls remain (0244, not claimed) |
| DoD-24: no blade branch, no allocation | Performed at `18849b5` (E1), passed | n/a | `wu17-asm/README.md`, "Findings" | Not re-read after the forced inlining; at release, inferred |
| DoD-24: pinned benchmark against the straight-line form | Performed at `2de0d36`, CPU 4, lock, seed 20261110 | n/a | `wu17-bench/README.md` | Measures code before the forced inlining. Galoot slower on Clang in the three transforms and `compose` (both loops), and on GCC in p ∧ q throughput, `compose` (both loops) and transform(l, Q) latency at the smallest size; 10 of 22 loops slower and 6 faster under the strict reading. The release claims no benchmark of its own code |
| DoD-25: history baseline committed | Partly: era `ebdd06d09289` (rows 2026-10-31 to 2026-11-01); no WU17 baseline | n/a | `history/eras.tsv` | Release record states the dates (0246 item 2) |
| DoD-25: `runs.tsv` keeps P13's rows | Performed, passed at `2438604` (`tools/runlog check`) | n/a | gate 3 rows | Rerun at R (U8) |
| Spike E, D8 | Performed: forms A, B, C derived and counted; benchmarks, bounds, prototype; both halves checked | Performed; not reproduced as a pair (54 in one arrangement for the line, 35 in none for the plane), recorded as a counting-rule difference; secondary (the errata page; book unread, V9) | note 0027; `records/spikes/spike-e/`; decision 0234 | None for 1.0 |
| DoD-4, repeated inputs | Declared-output check, by 0239 | n/a | `core.transform_nonzero_int64` and `core.transform_nonzero_rational` (abort with "transform: v does not act as a versor on x" when checking is on); `core.transform_consteval_nonzero` and `core.transform_consteval_nonzero_rational` (the constant evaluation ends); `core.transform_checking_recorded` (compares exactly the dropped coefficients); the `core.transform_unchecked_*` twins; `core.transform_rounding` over double and float (`test/CMakeLists.txt`, the transform section) | Passed in gate 3's workflows at `2438604`, inferred from their exit 0 (rows are per preset, not per test) |
| 0230 item 2, cost model refresh (G) | Not performed | n/a | `snippets/plan_cost_model/` | Deferred (0246 item 2) |

## 4. WU18 (M8): what the release needs

- **Matrix.** The full suite in each of the eleven presets from empty trees at R, the benchmarks once, the trace gate, `format-check`, `tools/runlog check`, and the 50-seed identity campaign on gcc-debug (as at M5; the binaries are built, so it costs run time only).
- **DoD-27.** Written and tested by plan 0041; its tests run in every preset. U6 re-reads the README's status line and the guide's pointers; the owner's walkthrough is the reading for humans. No new cold read.
- **Placeholders, filled only in the release commit** (decision 0245 item 3): the README's `FetchContent` URL and tag; `project(galoot VERSION 0.1.0 ...)` to 1.0.0; the README's status line ("close to its first stable release"); the study's byline if the owner names an author (default stands, decision 0241 item 3); the known friction below.
- **Known friction in the release record**, each with its record:
  1. gcc-debug's largest compile, `transform_identities_test.cpp`, peaks at 5.93 GiB (gate 3); a user's large debug unit can need several GiB.
  2. With the default forced inlining, Clang at `-O2` compiles operation-heavy units slowly: the clang-measure tree takes about 664 s per `develop` merge, 599 s of it on `identities_rounding_test.cpp`, against 120 × 10⁹ instructions unforced (decision 0244 item 16).
  3. Debug builds compile slower with the forced inlining (GCC `product_test.cpp` +80.5%, 2.18 s to 4.32 s, 418 to 830 MiB; decision 0245 item 1).
  4. The four `GALOOT_DETAIL_*` helper macros of the mode check stay defined in every user unit (`assert.hpp:44-47`).
  5. An empty `GALOOT_FORCE_INLINE` gives two errors, the literal first (decision 0244 item 17).
  6. From the draft: `-Os` builds still call into the library; the one-blade conversion gets the compiler's own error; `-ffp-contract=fast` can change the last bits between checked and unchecked paths (decision 0243).
- **Baseline dates** stated in the release record: history era 2026-10-31 to 2026-11-01; E1 and E3 2026-11-10 at `18849b5`; E2 2026-11-10 at `2de0d36`; the core pass's W10 records at `7a27f76`, `a6f3196`, `d2568e3`, `2d292ef`; DoD-22's rows at R.

## 5. Units

Each unit has a role (agent roles via briefs, W14), a branch in its own worktree (W7), and its merge checks (P8, P9). Records-only merges need the P9 checks their files affect and no independent review (decision 0215); a cost row goes to `agent-cost.tsv` at each hand-back.

| # | Unit | Role, model | Branch | Merge checks |
| --- | --- | --- | --- | --- |
| U1 | M7 records: plan 0038 outcome and status (V10 per criterion; G, H, reruns deferred); decision 0247, the `bulk_dual` exception beside 0141, with 0141's end conditions; an index of core units to W10 records, gaps named; the errata-naming row's exact text, drafted | implementer, Sonnet | `records/m7-gate` | records only: trace gate, `tools/runlog check` if a log changed |
| U2 | M7 snippets review `snippets/reviews/M7.md` (window since `aa644c2`); delete `records/spikes/spike-cdf/count_evals.py` and its index row; list the rest, apply nothing else | reviewer, Sonnet | `records/m7-snippets` | records only |
| U3 | M7 gate audit at the `develop` tip after U1 and U2: exit table, the V10 table of section 3.4 checked against the files, findings, the `apply` overloads, the errata row's text, acceptance wording; no matrix (gate 3 at `2438604` plus `4b87d8c`'s rerun in nine trees; the only later change outside records) | auditor, Opus | `gate/m7` | records only; cheap gates rerun |
| U4 | Owner rulings and acceptance: decision 0248; plan 0001 addendum | coordinator | direct to `develop` | records only |
| U5 | Plan closures (DoD-28): status lines and missing outcomes of `plan-closures.md`, except plans 0001, 0038, 0042 | implementer, Sonnet | `records/plan-closures` | records only |
| U6 | Release records: DoD-2 inventory re-grepped at the tip, rows for `GALOOT_FORCE_INLINE` and `GALOOT_ASSERT` (public, 0241 item 6), O1's ruling applied; release record rewritten (exception 4 from the structure test and E2's date, friction, dates); DoD-27 re-read; known exceptions searched in decisions 0141 to 0248 | planner, Opus | `records/release-prep` | records only |
| U7 | Release commit R: VERSION 1.0.0, README status line, `FetchContent` URL and tag, the byline if named | build-expert, Sonnet | `feature/release-1.0.0` | one Sonnet blind review (decision 0240 item 8); the checks are U8 |
| U8 | Release matrix at R, section 4; debug trees configured and built first, 0080's copy step, then `ctest`; benchmarks once under the lock on gcc-measure and clang-measure; a memory watchdog as at gate 3 | performance-engineer, Opus | same branch | P9 release row, all `gate:all@R` |
| U9 | V10 table and release record filled from U8's rows: every cell, its revision, command and seed; failures as found | planner, Opus | same branch | records only; cheap gates at T |
| U10 | Release audit of `1aa1e40..T`, weighted to what U3 did not see (U4 to U9, DoD-28, DoD-29, W13) | auditor, Opus | `gate/release-1.0.0` | records only |
| U11 | `develop` fast-forwarded to T; this plan's outcome | coordinator | `develop` | none beyond U8 to U10 |

## 6. Order

1. U1, U2 and U5 at once (they change different files; U5 leaves plans 0001 and 0038 to U1 and U4).
2. U3 once U1 and U2 are on `develop`. Coordinator verifies the hand-back (W14), then reports to the owner with O1 to O6.
3. Owner rules and accepts M7; U4 records it.
4. U6, then U7 once the owner gives the URL and tag.
5. U8 on U7's tip; U9; U10.
6. A blocking finding in U3 or U10, or a failure in U8, gets a fix unit of its own. A fix under `include/`, `test/`, `tools/`, `cmake/` or the build files reruns U8 whole; W8's two-attempt rule applies.
7. U11; the owner's walkthrough (section 8); the owner merges and tags.

## 7. Owner decisions (each with a default if not ruled)

- **O1. The six helpers of decision 0241 item 6.** (a) Public in 1.0, section 3 amended to the real exports, 0241 item 6 superseded; (b) a core unit moves five to `detail` before 1.0 (`same_rows` stays, the deleted `==` names it), with tests, the core guide, a blind review and a `develop` matrix before U8; (c) move them after 1.0, which makes a 2.0.0. Tradeoff: (a) costs nothing and keeps five names that only generic code and the guide use; (b) costs a core unit and about an hour of matrix; (c) breaks users. Recommendation: (a). Default: (a).
- **O2. `basis_change.hpp` in `core.hpp` without part H's measurement.** (a) Keep it, as the guide teaches (`#include <galoot/core.hpp>` for the whole core); (b) measure parse cost first. Removing a header from the umbrella after 1.0 breaks users; adding it later does not. Recommendation: (a); the cost is in the debug-build friction anyway. Default: (a).
- **O3. Separate M7 and release audits.** (a) Separate, as planned: M7 findings are fixed before the costly matrix; (b) one audit after U9. (b) saves one auditor run (about 400k tokens), and a code finding then reruns the matrix. Recommendation: (a). Default: (a).
- **O4. Which revision the tag names.** (a) T, the records tip after the matrix, with the empty code diff from R stated; (b) rerun the whole matrix at T, about an hour. Recommendation: (a). Default: (a).
- **O5. The `-O3` measure presets in the release matrix.** (a) Run them, about 15 to 25 minutes (inferred from gate 3's clang-measure); (b) only the nine gate presets. (b) leaves untested a level users choose. Recommendation: (a). Default: (a).
- **O6. Names.** The tag's spelling, which the README's `GIT_TAG` repeats: `1.0.0` or `v1.0.0`; and the rga3 preset's version for the record (no file carries one). Recommendation: `1.0.0`, and rga3 0.1.0. Default: the same.

**Only the owner can:** rule O1 to O6; edit CONVENTIONS for the errata-naming row (or decline it), and decide CONVENTIONS' "most canonical reference" sentence at the walkthrough (0241 item 2); give the repository URL; name the study's author, if anyone; accept M7; walk through the release; merge to `master` and tag; approve a `runs.tsv` compaction (none recommended).

## 8. Owner walkthrough and release commands

On `develop` at T, from `/home/brianm/projects/utils/math-core/galoot`:

1. Read the release record (`docs/agent/records/release/`), its V10 table and known friction, and the release audit.
2. Read `README.md` and `docs/guide/README.md` as a new user would.
3. Build and run the README's examples outside any tree:
   ```bash
   python3 -I tools/doc_examples.py extract README.md /tmp/galoot-walk
   g++ -std=c++23 -pedantic-errors -I include /tmp/galoot-walk/example_1.cpp -o /tmp/galoot-walk/ex1 && /tmp/galoot-walk/ex1
   clang++ -std=c++23 -pedantic-errors -I include /tmp/galoot-walk/example_1.cpp -o /tmp/galoot-walk/ex1c && /tmp/galoot-walk/ex1c
   ```
4. Compile a rejected example of the guide's first page (the README has none) and read the message:
   ```bash
   python3 -I tools/doc_examples.py extract docs/guide/README.md /tmp/galoot-walk/start
   g++ -std=c++23 -I include -c /tmp/galoot-walk/start/rejected_1.cpp -o /dev/null
   ```
5. See the build-mode message: add `-DGALOOT_FORCE_INLINE=ON` to step 3's command.
6. Decide the CONVENTIONS sentence, then say whether the release goes ahead.

To release, keeping the main checkout on `develop`. Corrected 2026-11-12 by U11 from the release audit's m1 and the owner's rulings (decision 0250 items 4 to 6); the first form had no check that `develop` holds the audited T and named CONVENTIONS 1.17 in the tag.

Before these commands, the owner's CONVENTIONS 1.18 edit lands on `develop` as a commit of that file alone, and the trace gate and `tools/runlog check` run on that commit and their rows are committed after it (release record, "The tip T", rule 4; the text and the commands are in `docs/agent/records/release/conventions-1.18-owner-text.md`). `develop` cannot equal T itself at the merge: that commit, its gate rows and the coordinator's records after U11 all come after T. So the first checks show that `develop` holds T and that nothing but records changed after it, and the next ones that the trace gate passed on the 1.18 commit.

```bash
cd /home/brianm/projects/utils/math-core/galoot
T=________                                                      # the hash of T from section 11; as printed, every check below fails
git merge-base --is-ancestor "$T" develop && echo ok             # must print ok: develop holds T
git diff --stat "$T" develop -- . ':!docs/agent'                 # must print nothing: no code, test, tool, build or user page after T
git diff --name-only "$T" develop -- docs/agent/governance       # must print only docs/agent/governance/CONVENTIONS.md
git log --oneline "$T"..develop                                  # read: each commit is a records commit
git show develop:docs/agent/governance/CONVENTIONS.md | sed -n 's/^Status: version \([0-9.]*\),.*/\1/p'   # must print 1.18
C=$(git rev-parse --short=12 "$(git log -1 --format=%H develop -- docs/agent/governance/CONVENTIONS.md)")
git show develop:docs/agent/records/performance/runs.tsv \
  | awk -F'\t' -v c="$C" '$2 == c && $4 == "trace" && $8 == 0' | wc -l   # must print 1 or more: the trace gate passed on the 1.18 commit
git merge-base --is-ancestor master develop && echo ok           # must print ok
git worktree add /home/brianm/projects/utils/math-core/galoot-master master
git -C /home/brianm/projects/utils/math-core/galoot-master merge --no-ff develop -m "Release galoot core 1.0.0"
git diff develop master --stat                                   # must print nothing
git tag -a 1.0.0 master -m "galoot core 1.0.0, CONVENTIONS 1.18"
git worktree remove /home/brianm/projects/utils/math-core/galoot-master
git show -s --format='%h %p %s' '1.0.0^{commit}'                 # the merge commit M; its second parent is develop's tip
```

Nothing is pushed. The tag names the `--no-ff` merge commit M, whose tree is `develop`'s (decision 0250 item 4). If the owner declines 1.18, the `governance` line must print nothing, the version line prints `1.17`, the trace-row check is skipped, and the tag message names CONVENTIONS 1.17. To publish, push `master`, `develop` and the tag to the repository the README names, once it exists (decision 0249 item 1).

## 9. Post-1.0 backlog (not scheduled)

Validation of the `GALOOT_FORCE_INLINE` cache option; a CMake-setting mutant on an OFF tree; the `convention-trace` exit anomaly at `81c923136055`; the `==` hint note (`= delete("…")` with C++26); guide wording n2 (edge spellings such as `1+0`); `#undef` of the four helper macros; the `tools/timed` dirty-mark gap (0241 item 9); the CTest label anomaly; the DoD-2 helper split, only if O1 is not (a) and only with 2.0.0; header variables that clash with users' names under `misc-confusable-identifiers` (`l` in `basis_change.hpp:542`, `m` in `bits.hpp:171`); deleting the two `apply` overloads without a target layout; placement search; Theorem R at the structure layer (0219 item 11); `witt_change` (0225 item 5); M6, the cga3 preset (0211); `tools/mutate` running CTest in parallel; "model" to "preset" wording (0215 item 11); the knowledge-base source-line rule (0241 item 8). Also deferred by record: E1 and E2 reruns, G and H (0246 item 2); the batch-under-one-motor interface (0239 item 1); a contraction preset (0243 item 3); spurious overflows left after the row reduction (0236 item 2); the one-blade catch-all (0241 item 1); conformal(3) and `-Os` calls (0244); option B of `collect_result` (0227 item 2).

## 10. Evidence, risks, cost

- Evidence (V10): U8's rows, at R, each `gate:all@R` with its command; seeds as the skeleton lists (doctest 0, campaign 1 to 50, `conventions_check.py` 20261031, the reference scripts' fixed seeds).
- Risks: (1) the `-O3` presets or the long-idle benchmarks fail for the first time at U8, which is late; O5 (b) trades that risk for coverage. (2) gcc-asan or clang-asan memory: the watchdog of gate 3 stops a build below 15 GiB free or above 20 GiB in one process. (3) An audit finding under `include/` costs a fix unit, a delta review and a rerun of U8.
- Agent cost estimate (W3): about 2.0 M tokens: U1 120k, U2 80k, U3 400k, U5 120k, U6 250k, U7 80k with review, U8 250k, U9 200k, U10 400k, coordinator 100k. Machine time, inferred from gate 3: U8 about 60 to 75 minutes of matrix, one at a time, plus the campaign and the benchmarks.

## 11. Outcome

Draft written 2026-11-12 by Claude (agent, Opus 5.5), U9, for U11; finished 2026-11-12 by Claude (agent, Opus 5.5), U11, on `feature/release-1.0.0`. The cells still marked **[coordinator]** hold the facts of U11's merge into `develop`, which exist only after this hand-back; the owner's are marked so.

### Departures and readings (C20)

1. **The README changed after R.** Decision 0249 item 2 put the rga3 sentence's rewording in the records tip T (`1cd7ac8`), so section 1 item 4's rule, "`git diff --stat R T -- . ':!docs/agent'` is empty", does not hold as written. The release record's "The tip T" replaces it: from R to T nothing outside `docs/agent/` changes except that sentence, and the README's tests passed after it on both compilers.
2. **The repository URL is a placeholder at release** (decision 0249 item 1), against decision 0245 item 3 and section 4's "Placeholders"; stated as known friction.
3. **U8's rows name `7276f82`**, R plus one run row, with an empty code diff from R (the matrix record, departure 1).
4. **Untimed runs in U9.** The README's tests after `1cd7ac8` ran directly, since the pool had stopped itself and `tools/timed` refuses a build or test step without it (release record, departure 3). They were rerun through `tools/timed` at `8118033` (rows 21958, 21959, committed by the coordinator in `18eb677`) and by U10 at `18eb677`.
5. **U11 ran as an agent on the release branch, not as the coordinator on `develop`** (section 5's row). Its brief gave it the records of the release audit's rulings (decision 0250) and the final T; the merge into `develop` stays the coordinator's. `develop` was merged into the branch first (a fast-forward to `300f6b4`: the audit, its rows and the U9 and U10 cost rows).
6. **`develop` cannot be T at the owner's merge.** The owner's CONVENTIONS 1.18 commit and its gate rows, and the coordinator's records after U11, land after T. Section 8's commands check that `develop` holds T and that only records changed after it (decision 0250 item 6).
7. **No build or test ran in U11.** Its gates are `tools/runlog check`, the trace gate and the citation checks, none a build or test step, so the job pool (stopped when U11 began: `tools/jobpool status` printed "not running") was neither needed nor started.

### Per unit

| Unit | Result | Where |
| --- | --- | --- |
| U1 | Plan 0038 outcome and status; decision 0247; the W10 index; the errata row drafted | `records/m7-gate` `c43fb63`, merged `655ad63` |
| U2 | M7 snippets review; `count_evals.py` deleted | `records/m7-snippets` `53bbb28`, merged `316af44`, with `e4bd07d` |
| U5 | Plan closures (DoD-28) | `records/plan-closures` `e730ed3`, merged `985c9db` |
| U3 | M7 audit at `f15632b`: 0 blocking, 2 major (MA1 the errata row's draft, MA2 decision 0245's edit), 10 minor, 9 info | `gate/m7` `6a0f027`, merged `d9c48dc`; `records/gates/2026-11-11-M7-audit.md` |
| U4 | M7 accepted; O1 to O6 ruled as recommended | decision 0248 (`a73cd4e`); plan 0001 addendum (`c41ed75`) |
| U6 | Audit minors m1 to m3, m5, m6, m9 fixed; DoD-2 inventory re-grepped; release record rewritten | `records/release-prep` `421d572`, merged `036dd40` |
| U7 | Release commit R `7580e88`: VERSION 1.0.0, README status, `GIT_TAG "1.0.0"`, URL placeholder kept; blind review approve, 3 minors | `feature/release-1.0.0`; review `b64e1c4`, merged `a35eec8`; decision 0249 (`36b270a`) |
| U8 | Release matrix: all eleven presets passed from empty trees on R's code (657 of 657 on six, 649 of 649 on five), 2,883.7 s; trace gate, `format-check`, `conventions_check.py`, `tools/runlog check`, `plan_cost_model` checks and `jobserver_check.py` passed; campaign 300 of 300; benchmarks once, 22 of 22 per compiler, checksums agree; 944 DoD-22 rows, shared mode | `7276f82..ab6836a` (`4e7d4aa` the record, `ab6836a` the final checks and the correction of `4e7d4aa`'s preset count); `records/gates/2026-11-12-release-matrix.md` |
| U9 | `develop` merged in (`eea9132`); README rga3 sentence (`1cd7ac8`); snippets index and `jobserver_check.py` header refreshed (`7bad7b2`); V10 table and release record filled and renamed (`records/release/v10-table.md`, `release-record.md`); this draft | `feature/release-1.0.0`: records `2e9d7ed`, its closing gates' rows `8118033` (the U9 tip); the coordinator's README rows `18eb677` |
| U10 | Release audit of `1aa1e40..18eb677`: 0 blocking, 3 major (MA1 DoD coverage, MA2 P9's history, MA3 the quoting of 0248 and 0249), 9 minor, 10 info; owner items O-A to O-G, all ruled as recommended (decision 0250) | `gate/release-1.0.0` `5bfb3a3`, merged into `develop` at `9075620`; cost row `300f6b4`; `records/gates/2026-11-12-release-audit.md` |
| U11 | `develop` merged in; decision 0250 with its addendum; the DoD table and DoD-3's map; the CONVENTIONS 1.18 text for the owner; release record (departures 3, 8 to 10, rule 4, exception 12, friction 10 to 13, the U10 and coordinator cells) and V10 table (departure 6, the header) completed; section 8's commands corrected; plan 0001's outcome; the status lines of plans 0019 and 0042; the cost rows of U4 and `18eb677`; the closing gates at T | `feature/release-1.0.0`, T as below. Merge into `develop`: **[coordinator]** `________` |

### Definition of done (section 2)

1. M7 accepted: met (decision 0248 item 1; plan 0001 addendum).
2. Matrix green on all configurations at R: met (U8).
3. V10 table with a filled row for every requirement, gaps stated: met at U11 (`records/release/v10-table.md`, with every DoD in `records/release/dod-1-to-29.md`). U9 wrote "met" for the V10 table alone; the release audit's MA1 found that DoD-3, DoD-8 and DoD-13 had no row, which the DoD table closes (decision 0250 item 1).
4. Release audit with no blocking finding: met (U10, `records/gates/2026-11-12-release-audit.md`; its owner items ruled, decision 0250).
5. Owner walkthrough, merge and tag: pending, the owner's (section 8), after the owner's CONVENTIONS 1.18 edit.
6. This outcome: this section, finished at U11 except the merge facts of U11 and the owner's.

### The tip T

T is the audited records tip that the owner's commands check `develop` against (section 8). It qualifies when the five rules of the release record's "The tip T" hold; U11 checked rules 1 and 3 with:

```bash
git diff --stat 1cd7ac8 T -- . ':!docs/agent'          # must print nothing
git diff --stat 7580e88 1cd7ac8 -- . ':!docs/agent'     # README.md only
tools/timed check - gate:runlog@T -- tools/runlog check
tools/timed trace - gate:trace@T -- python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test
```

T is the commit that adds, to U11's last records commit, only the run rows of the gates above, run on that records commit's tree (rule 3, its second form). A commit cannot hold its own hash, so both hashes are in T's commit message and U11's hand-back, and the coordinator writes them here:

- U11's last records commit: `8882890`; T: `ff1d49f`.
- At U11's records commit (run by U11, outputs as printed): `git diff --stat 1cd7ac8 <records commit> -- . ':!docs/agent'` printed nothing; `git diff --stat 7580e88 1cd7ac8 -- . ':!docs/agent'` printed ` README.md | 4 ++--` and ` 1 file changed, 2 insertions(+), 2 deletions(-)`; rules 2, 4 and 5 hold (no README change after `1cd7ac8`; no CONVENTIONS edit yet; nothing under `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/` or the build files after R).
- Closing gates (rows in T): `gate:runlog@U11`, `gate:trace@U11`, `gate:citations@U11`, `gate:citations-guide@U11`, and `check:trace-1.18-draft@U11`, the trace gate over a scratch copy of CONVENTIONS with the 1.18 text applied (`records/release/conventions-1.18-owner-text.md`).
- Merge of U11 into `develop`: fast-forward to `ff1d49f` (no merge commit). The tag and its commit: the owner's (`1.0.0` on the `--no-ff` merge commit M, decision 0250 item 4).

### Tag

The owner ran `records/release/close-out-v1.sh` on 2026-11-15: `develop` at `a4cd99d` (CONVENTIONS 1.18 at `68f77fc`, its gate rows at `a4cd99d`) was merged `--no-ff` into `master` as `2383e6d`, and the annotated tag `1.0.0` names that commit. `git diff develop master` is empty. Nothing was pushed (decision 0249). Lessons 0073 and 0074 close DoD-28's gap (decision 0251 item 3).

### Cost

Agent tokens recorded in `agent-cost.tsv` for this plan before U9: about 1.48 M (planner 216k, U1 170k, U2 72k, U5 88k, U3 307k, U6 286k, U7 53k and its review 52k, U8 234k), against the estimate of 2.0 M for all units. U9 228k and U10 282k, so about 1.99 M recorded for the plan (1,988,134 tokens over eleven rows). Not counted: the coordinator's U4 (not tracked) and `18eb677` (not visible), both rows added at U11, and U11 itself, whose cost is not visible to it: **[coordinator]** `________`. Machine time: U8's matrix 48 minutes, the campaign 19 minutes, the benchmarks under a second, within section 10's estimate.
