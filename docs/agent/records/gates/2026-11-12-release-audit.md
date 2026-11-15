# Audit report: the release candidate of core 1.0.0, `feature/release-1.0.0` at `18eb677`

- Scope: unit U10 of plan 0042. The candidate T is `feature/release-1.0.0` at `18eb677`; the release commit R is `7580e88`. The window is `1aa1e40..18eb677`, weighted to what the M7 audit (`records/gates/2026-11-11-M7-audit.md`, at `f15632b`) did not see: units U4 to U9, DoD-28, DoD-29 and W13. Checked:
  - every DoD of plan 0001 (DoD-1 to DoD-29) against the V10 table and the release record, opening each cited path, hash, `runs.tsv` row and `compile.tsv` line;
  - the release matrix record against `runs.tsv`, every row of all eleven presets;
  - the rule for T in the release record ("The tip T") and in plan 0042 section 11, at `18eb677`, against decision 0249 item 2 and decision 0248 item 9 (O4);
  - the release commands of plan 0042 section 8;
  - `git diff --stat 2438604..18eb677 -- . ':!docs'`;
  - the README against the guide, and placeholders in the README and the release records;
  - the DoD-2 inventory against the headers, by a scan;
  - the known friction against decisions 0244 to 0249;
  - `agent-cost.tsv` for plan 0042's units and plan 0040's gates;
  - W13 readiness, `master`, and `develop..T`.

  The governance applied is the text at `18eb677`, with the owner's rulings to decision 0249.
- Auditor: Claude Opus 5.5 (`claude-opus-5-5`), in the `auditor` role, 2026-11-12 UTC (2026-11-11 local). Worked on branch `gate/release-1.0.0` in the worktree `galoot-wt/gate-release`, made from `feature/release-1.0.0` at `18eb677`. Only this report and its run rows in `runs.tsv` are committed. No fork or subagent was used.
- Status: submitted to the owner. Not accepted.
- Verification status (V10): no build or full suite ran in this audit. The code evidence is U8's matrix on R's code (rows 21905 to 21937, all exit 0, checked below). At `18eb677` this audit ran, each through `tools/timed`, each exit 0: `tools/runlog check`, the citation check over the code paths (574 files) and over `docs/guide` (4 files), the convention trace gate (items with no test: 6.1 to 6.4 only), `conventions_check.py` (seed 20261031), and the README, documentation-page and citation tests in U8's gcc-debug and clang-debug trees (11 of 11 on each). The walkthrough's compiles of plan 0042 section 8 also ran (rows in the appendix).
- Validation status (V10): nothing new is validated at release. The V10 table restates earlier validation, which this audit did not redo.

Severity follows the earlier audits:
- **blocking** blocks a release until the owner decides;
- **major** departs from a ratified provision or an owner ruling without a declaration, or rests a record on a wrong fact;
- **minor** changes no result;
- **info** is an observation (C19).

Counts: 0 blocking, 3 major, 9 minor, 10 info.

## Departures and ambiguities (C20)

Of this audit:

1. **The role file and the brief.** The `auditor` role file says the auditor never edits files. The brief asks for this report and its run rows to be committed on `gate/release-1.0.0`. I followed the brief for those files only.
2. **I changed untracked files in U8's worktree.** To count each preset's tests I ran `ctest --preset P -N` in `galoot-wt/release-1.0.0` for all eleven presets. CTest rewrites `Testing/Temporary/LastTest.log` even when it only lists tests, so that file now holds my listing in all eleven trees. The file is untracked and no record cites it. The logs of gcc-debug and clang-debug had already been replaced by U9's later runs. No tracked file in that worktree changed (`git status` is clean). The later test runs of this audit used `--test-dir` from my own worktree, so their rows are on this branch.
3. **Untimed steps.** Git, grep, file reads, two scan scripts in my scratch directory, `ctest -N`, `ninja -n`, and the two README example binaries I built through `tools/timed` (`ex1` and `ex1c`, each printing "(1.000, 3.000, 3.000) at weight 1.000") ran outside `tools/timed`. None of them is a build or a test run.
4. **The walkthrough's files** went to my scratch directory, not `/tmp/galoot-walk`. The commands are otherwise those of plan 0042 section 8, steps 3 to 5.
5. **`format-check` was not run.** It reads no file that changed after R (R's two files are `CMakeLists.txt` and the README, and later changes are under `docs/agent/` or the README), and it passed at R (row 21940).
6. **Reading the window.** For W5, I checked that each merge into `develop` in the window that carries code has a review record (the plans 0035 to 0041 reviews in `records/reviews/` from 2026-11-10 on), and I read U4 to U9's commits one by one. I did not re-audit each review's content; the M7 audit did that for its part of the window.
7. **Which commit is T.** The brief names `18eb677`. That commit cannot be the commit `develop` is fast-forwarded to (m2), and this report itself is a records commit after it. I audited the rules for T at `18eb677` and say what U11 must recheck at the final T.

## Found, most severe first

1. **No blocking finding.** The matrix, the seed campaign, the benchmarks run, the DoD-22 rows and every cheap gate are as the records say. The code diff from R to T is empty, and the one README edit after R is the ruled one and its tests pass at T.
2. **Major (MA1), owner decision:** the V10 table and the release record do not cover every DoD. DoD-3 has no record at all, and DoD-8 and DoD-13 have no evidence statement. Plan 0042's outcome reports its definition of done item 3 ("a filled row for every requirement, gaps stated") as met.
3. **Major (MA2), owner decision:** P9's release row asks for "an update of the performance history". It was not done, and no record declares this.
4. **Major (MA3), records:** decisions 0248 and 0249 do not quote the options and recommendations they accept, the pattern the M7 audit's MA2 found and decision 0248 item 3 reaffirmed against.
5. **Owner items at the walkthrough:** the tag's commit (the merge commit or T), the CONVENTIONS 1.18 row, the "most canonical reference" sentence, and two safety lines for the release commands (m1).

## DoD coverage (plan 0001 section 4)

"V10" is `records/release/v10-table.md`; "RR" is `records/release/release-record.md`.

| DoD | Where evidenced | Status at T |
| --- | --- | --- |
| 1 class covered | RR, "Class covered (DoD-1)"; the rejections are in the misuse catalogue (V10 section 3, DoD-19 row) | Met; stated from the limits page, not a new test (RR says so) |
| 2 inventory | V10 section 3; `dod-2-inventory.md` | Met with two ruled departures (RR, "DoD-2"); my scan agrees (info i5) |
| 3 operation map, C3 record | **None.** Plan 0011 (line 238) handed "the C3 decision record that maps every operation to this mechanism (DoD-3)" to WU14 and WU17; neither plan names it | **Not met, not declared** (MA1) |
| 4 blade sets, repeated inputs, `sizeof` | V10 section 3 (repeated inputs only) | Repeated inputs met. The `sizeof` asserts exist (`test/presets/rga3/objects_test.cpp:255-259`, `test/core/mv_test.cpp:223-225`), and the blade-set half rests on the plan tests, but neither is cited (MA1) |
| 5 to 7 | V10 section 3 | Met (5 recorded at M4) |
| 8 `==` only in exact tests | **None** | Not stated. A grep finds no `Approx` in `test/` (inferred support) (MA1) |
| 9, 10 | V10 sections 3 and 2 | Met |
| 11 golden values, section 8 rows, trace | V10 section 1 and its section 8 table; the trace gate | Met, not labeled DoD-11 |
| 12 | V10 section 3 | Met |
| 13 one test file per component | **None** | Not stated; departed from by rulings (for example decision 0189's split of `identities_test.cpp`; `test/core/` has 9 `basis_change_*` and 10 `identities_*` files) (MA1) |
| 14 | V10 section 3 | Met (fault injection recorded at `d5ffafd`, not rerun; declared) |
| 15 seeds | V10 section 2, the campaign | Met for the campaign; "printed on failure" and "varying signatures" not stated |
| 16, 17 | V10 section 4 | Met, with the stated secondary referents |
| 18 V10 table | the table itself | Met for its four families |
| 19 | V10 section 3 | Met |
| 20 conversions, checking mode | V10 4.5; the `mv_truncate_nonzero_*` and `transform_nonzero_*` tests | Met in substance, not labeled DoD-20 |
| 21 | V10 section 3 | `-fno-exceptions` met; the `[[nodiscard]]` half not checked (declared) |
| 22 to 26 | V10 sections 3 and 5 | Met as recorded; performance rows track-only |
| 27 | V10 section 3; RR "Evidence" | Met; README tests rerun at T by this audit |
| 28 | RR "Evidence" (plan closures) | Plans: 0001 and 0042 at U11. A review record per merge and "lessons recorded" are not stated in RR. This audit: no blocking finding |
| 29 | RR, "Release" and "Evidence" | Version, CONVENTIONS 1.17, rga3 0.1.0, the V10 table, every check on R's code: stated. No open blocking defect and the merge and tag: the owner's |

## The release matrix against `runs.tsv`

Every one of rows 21905 to 21937 was read: 33 rows, three per preset for all eleven presets, each `gate:all@7580e88` at `7276f826533f`, each exit 0. For each preset, the sum of its configure, build and test rows equals the table's wall time to 0.1 s, and the build row's `maxrss_kib` equals the table's largest compile (for example gcc-asan 0.605 + 164.481 + 63.672 = 228.8 s, 4,961,752 KiB = 4.73 GiB; clang-measure-o3 0.546 + 602.079 + 63.986 = 666.6 s, 1,885,556 KiB = 1.80 GiB). The test counts (657 on six presets, 649 on five) equal `ctest -N` in U8's trees, and the 56 tests with a `docs` label and the named DoD-4 and README tests exist in the gcc-debug tree's CTest files. The check rows 21938 to 21953 and the closing rows 21954 to 21959 all exit 0.

- `compile.tsv` lines 4673 to 5616: 944 rows, all `7276f826533f`, `feature/release-1.0.0`, `shared`; gcc-debug 471 (137 before its suite started, 334 after), clang-debug 473 (137 and 336); 662 nonzero exits; no source and object pair twice; largest units as stated (6,214,900 KiB, 92.3 s; 1,585,484 KiB, 32.8 s).
- `records/gates/2026-11-12-release-seeds.tsv`: 300 rows, 6 programs × 50 seeds, all exit 0, all at `7276f826533f`; `core_identities_test` low median 21.825 s, range 21.767 to 21.937 s.
- Benchmarks: rows 21947 and 21948, `pinned`, CPU 4, exit 0.
- The total, 2,883.7 s, is the sum of the rounded per-preset figures; the rows sum to 2,883.56 s (info i1).

## The tip T

At `18eb677`:

- `git diff --stat 1cd7ac8 18eb677 -- . ':!docs/agent'` prints nothing; `git diff --stat 7580e88 1cd7ac8 -- . ':!docs/agent'` prints only `README.md`, 2 lines changed. Rule 1 holds.
- Rule 2: the README tests passed at `8118033` (rows 21958, 21959) and again in this audit at `18eb677` (rows 21969 to 21971). The README examples extracted in U8's gcc-debug tree are the same, line for line after the `#line` directive, as those extracted from T's README.
- Rule 3: `tools/runlog check` and the trace gate pass at `18eb677` (rows 21960 and 21963 on this branch).
- Rules 4 and 5: no CONVENTIONS edit yet, and nothing under `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/` or the build files changed after R.
- `git diff --stat 2438604..18eb677 -- . ':!docs'` lists exactly `test/CMakeLists.txt` and `test/core/force_inline_on_fail.cpp` (`4b87d8c`, rerun in nine trees after gate 3 and covered by the matrix at R), `CMakeLists.txt` (the version, `0.1.0` to `1.0.0`), and `README.md` (R's status line and `FetchContent` block, and `1cd7ac8`'s rga3 sentence).
- `develop..18eb677` holds R, `7276f82`, `4e7d4aa`, `ab6836a`, `eea9132` (a merge of `develop`), `1cd7ac8` (the README), `7bad7b2`, `2e9d7ed`, `8118033` and `18eb677`. Apart from R and `1cd7ac8`, each touches only `docs/agent/`.
- `master` is `9a4e0cd`, the initial commit, with one reflog entry. It is an ancestor of `18eb677` and of `develop`. No tag exists.

**Is the rule a faithful reading?** Decision 0248 item 9 (O4) says the tag names "the records-only tip T, and the release record states the empty code diff from the release commit R to T". Decision 0249 item 2 then puts a README edit in T, so T cannot be records-only. The release record's rule allows exactly that README sentence, reruns its tests, and keeps the empty code diff (nothing under `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/` or the build files). It is stricter than O4 and it declares the conflict (RR, departure 2). I read it as faithful.

**The tag's commit.** Plan 0042 section 8 merges `develop` into `master` with `--no-ff` and tags `master`, which is the merge commit M, not T. M's tree equals T's, which `git diff develop master --stat` checks. The release record declares this (departure 8) and leaves the choice to the owner. O4's question was which tree is tagged (T as is, or T after a second matrix), not which of two commits with the same tree, so tagging M meets its purpose. It does not meet its letter (owner item O-D).

**The release commands (plan 0042 section 8)** are correct and safe as far as they go. The `master` worktree is temporary and beside the main checkout, which stays on `develop`. No other worktree has `master` checked out. The ancestor check guards the merge. `git diff develop master --stat` is the right empty check, and it passes trivially when `master` is an ancestor. The tag is spelled `1.0.0`, as decision 0248 item 9 (O6) and the README's `GIT_TAG` require. Nothing is pushed. Two gaps are in m1.

## Findings

### MA1 (major, owner decision). DoD-3 has no record; DoD-8 and DoD-13 have no evidence; several DoDs are covered only implicitly

- Provisions: DoD-3, DoD-8, DoD-13, DoD-18, DoD-28, DoD-29 (plan 0001 section 4); C20 (departures first); W8 (halted or dropped work is never silently dropped); plan 0042 section 2, definition of done item 3.
- Evidence: the DoD coverage table above. `grep -rn "DoD-3\b"` over `docs/agent` finds only plan 0001, plan 0011 (line 238, which hands the record to WU14 and WU17), decision 0220 (section 4, the change of basis only) and plan 0030 (likewise). No decision maps every operation to the term-list generator. The pieces exist across decisions 0110 (products), 0114 (unary maps), 0115 (conjugated plans), 0167 (sums), 0172 (duals as compositions), 0178 (transform) and 0220 section 4 (change of basis). `DoD-8` and `DoD-13` appear only in plan 0001 and plan 0011. Plan 0042's outcome says definition of done item 3 is "met", and `plan-closures.md` says plan 0001 still needs "a closing outcome with the DoD-1 to DoD-29 result, written at release".
- Why it matters: DoD-29 asks the release record to state "that every check ran on the released revision". A reader of the release records cannot see whether DoD-3, DoD-8 or DoD-13 holds, and DoD-13 is in fact departed from by ruling.
- Not blocking: it changes no result, and the code and tests are as the matrix ran them (auditor role file; C19). The owner may judge it blocking, since DoD-3 is a release criterion and its record does not exist.
- Fix, records only: at U11, a DoD-1 to DoD-29 table in plan 0001's outcome (or the release record), each row citing its evidence or stating its gap; and either a short DoD-3 record that maps each operation to the decision covering it, or an owner decision that accepts DoD-3 as not met for 1.0.

### MA2 (major, owner decision). P9's release row: no update of the performance history

- Provisions: P9, "Release" row ("The full suite in every supported configuration, the benchmarks, and an update of the performance history (W13)"); P7; P13 (compaction needs the owner's approval); C20.
- Evidence: `records/performance/history/eras.tsv` holds one era, `ebdd06d09289`, rows 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z. `git log -- docs/agent/records/performance/history/` ends at `973fa35` (the M3 acceptance). `tools/runlog` updates the history only through `compact`, which P13 leaves to the owner. Plan 0042 section 7 lists "approve a `runs.tsv` compaction (none recommended)" among the owner's items but does not tie it to P9. Neither the release record, the V10 table, nor plan 0042 section 1 declares that the update was not made. The release record states the history's dates (decision 0246 item 2), which is a different requirement.
- Fix: (a) the release record declares the departure, with the reason (the log is complete and every result is track-only, so nothing is compared with the history); or (b) the owner approves `tools/runlog compact` at T, a change to `runs.tsv` that reruns `tools/runlog check`.

### MA3 (major, records). Decisions 0248 and 0249 do not quote what they accept

- Provisions: C18 (alternatives weighed, the reason); decision 0228 item 6(a) ("an owner-ruling record quotes the options and the recommendation it accepts"), reaffirmed by decision 0248 item 3.
- Evidence: decision 0248 item 9 states the rulings on O1 to O6 and points to plan 0042 section 7, but does not quote the options. Items 4 to 8 state outcomes without alternatives. Decision 0249 ("the recommendations proceed") names no alternatives for deferring the URL, such as holding the release or dropping the `FetchContent` block, and none for where the rga3 rewording goes (in T, or in a new R).
- Why it matters: this is the defect of the M7 audit's MA2, repeated in the decision that reaffirmed the rule against it. The options are findable through plan 0042 section 7 and the release-commit review, so no result depends on it.
- Fix: a new decision that quotes the options and recommendations accepted by 0248 and 0249, with no edit to either (0228 item 6(a)).

### Minor

- **m1. Two safety lines for the release commands** (plan 0042 section 8; W13 "evidence current for the released revision"). (i) Nothing checks that `develop` is the audited T before the merge. `develop` has already moved past one candidate: `dff6590` (U9's cost row) is on `develop` and not in `18eb677`. A cost row landing after U11 would make the owner merge and tag a commit nobody checked, though only records would differ. Add, before the merge: `test "$(git rev-parse develop)" = "<T>" && echo ok`, or `git diff --stat <T> develop` printing nothing. (ii) The tag message hard-codes "CONVENTIONS 1.17", while decision 0248 item 2 rules that the owner adds the row as 1.18. The text under the commands says to change it, but the command as printed would be wrong after the ruled edit. Suggest `CONVENTIONS 1.18` in the command, or a line that prints the version from CONVENTIONS' status line first.
- **m2. `develop` cannot be fast-forwarded to `18eb677`** (plan 0042 section 5, U11; release record "The tip T"). `git merge-base --is-ancestor develop 18eb677` fails, because of `dff6590`. U11 must merge `develop` into the release branch (records only), add this audit and its own outcome, and rerun rule 3's two gates on that commit. That commit, not `18eb677`, is T.
- **m3. Known friction is missing three user-visible items** (decision 0246 item 1 and plan 0040 section 17; plan 0042 section 9). (i) Header locals `l` (`basis_change.hpp:542`) and `m` (`bits.hpp:171`) are confusable under clang-tidy's `misc-confusable-identifiers` with a user's `I` and `rn`, for a user who lints with Galoot's headers not marked as system headers (`records/reviews/2026-11-11-pre-release-core-pass-delta2-review/lint/scan-26f1b62.tsv`, rows 35 and 53). It is on plan 0042's post-1.0 backlog but not in the release record. (ii) The CMake cache option `GALOOT_FORCE_INLINE` is not validated: a configure value such as `2` or `foo` reads as ON with no message (plan 0040 section 17, m2). (iii) The deleted `==` names `same_rows` only in the source line that the compiler echoes (plan 0040 section 17, m5); the release record's interface list names the hint but not this limit.
- **m4. The README's "Copy the headers" route also copies doctest.** "Copy or vendor the `include/` directory" brings `include/ext/doctest/` (the test framework, its `.cpp` and its MIT license) into a user's project, beside "Galoot is headers alone". Nothing breaks, and the core does not include it (decision 0026). Changing the README to `include/galoot` would be a README change after R (release record rule 5: the README tests rerun).
- **m5. The README rows at `8118033` are cited nowhere** (P13; V10 "command"). Rows 21958 and 21959 (`gate:docs@8118033`, committed by `18eb677`) are timed reruns of the README tests. The release record's departure 3 and the V10 table's departure 6 still say those checks "have no row in `runs.tsv`". Neither the record nor `18eb677`'s message gives the command, and the message gives no reason (W7). `18eb677` names Claude Sonnet 5.5, and no cost row names a Sonnet agent for it; the U9 row (`dff6590`) names Opus. That this agent's cost is missing is inferred, not shown.
- **m6. Six merge commits in the window do not identify agent work** (W7): `6d7d7b4`, `4483b6d`, `db81e97`, `f580f2a`, `6c278df` (documentation lane and core pass branches) and `7284061` (rescued run rows). Every non-merge commit in `1aa1e40..18eb677` and `1aa1e40..develop` does.
- **m7. Stale plan status lines** (W3). Plan 0019's status still says "Closed except the open owner items in the post-WU10 audit", although decisions 0182 to 0189 cite that audit (inferred, from their citations, to be its rulings; not traced item by item). U5 left it for checking (`plan-closures.md`, section C). Plan 0042's status line still says "No unit below has started"; U11 closes it.
- **m8. The rerun after CONVENTIONS 1.18 is stated two ways.** The V10 table's header says the trace gate and `conventions_check.py` rerun; the release record's rule 4 names the trace gate only. `conventions_check.py` does not read CONVENTIONS.md (by grep), so rule 4 is enough; the V10 sentence asks for a harmless extra run.
- **m9. U4 has no `agent-cost.tsv` row.** It is the coordinator's unit (decision 0248, the plan 0001 addendum). Plan 0042 section 11 leaves "the coordinator" to U11. Every other plan 0042 unit has a row: U1, U2, U3, U5, U6, U7 and its review, U8, and U9 (`dff6590`, on `develop` only). Plan 0040's gate runs (gate-run-1, gate2, gate3) and plan 0041's two gate runs have rows. U10 and U11 are to be added.

### Info

- **i1.** The matrix total, 2,883.7 s, is the sum of the per-preset figures after rounding; the rows sum to 2,883.56 s.
- **i2.** The matrix record's "23 of 23 slots free before and after every step" and the watchdog's minimum of 92.4 GiB are not in `runs.tsv`. They rest on U8's report and are not verified here.
- **i3.** The README matches the guide: the guide's first page has nine numbered facts, as the README says, and the toolchain, the CMake 4.4 minimum and the checking and inlining modes are stated the same way. The citation check finds no project citation in the README or the four guide pages. No British spelling was found by grep. The only placeholder is the ruled `<galoot-repository-url>`, stated in the text and the comment. "Each `cmake` block is built as a project" holds with the URL overridden offline, as the release record says.
- **i4.** No `TODO`, `XXX`, `<release-tag>` or `____` is in the README, the guide, `include/galoot`, `CMakeLists.txt` or `CMakePresets.json` (doctest's own `TODO`s are under `include/ext`). The release records keep their designed fill cells, marked **[U10]**, **[owner]** or **[coordinator]**: release record lines 21, 93, 212, 216 to 218 and 229, and plan 0042 section 11. They are to be filled at U11 and by the owner.
- **i5.** DoD-2: a scan of `include/galoot` that skips comments, strings and every `detail` namespace (both `namespace detail {` and `namespace galoot::core::detail {`) lists each concept, struct, alias, variable, function and macro of `galoot::core`. Every name is in the inventory, the rga3 layouts under its appendix row "their layouts". The six helpers, `from_gram`, `conformal_change` and the `*_layout` names exist in the headers as the inventory says.
- **i6.** The known friction covers decisions 0244 to 0249 apart from m3: the forced inlining's debug and Clang `-O2` cost (0244 items 16 and 17, 0245 item 1), the helper macros, the empty-value errors, `-Os`, the instruction-set mismatch, contraction (0243), the URL (0249 item 1), the W10 gaps (0248 item 4) and the tools limits (0248 items 6 and 8).
- **i7.** The walkthrough commands of plan 0042 section 8 work as written. README example 1 builds and prints the expected line on both compilers. The guide's `rejected_1.cpp` stops with "s * a: the scalar is not of the multivector's scalar type ...". `-DGALOOT_FORCE_INLINE=ON` stops with "GALOOT_FORCE_INLINE must be 0 or 1".
- **i8.** `4b87d8c` and the U9 commits carry the owner's git identity as author with an agent trailer, as elsewhere in the history; W7 is met by the trailer.
- **i9.** The release-commit review was branched from `develop` and merged (`a35eec8`), which W7's `review/<name>` row allows.
- **i10.** The release-commit review cites "Decision 0248 item 6" for the rga3 version. The ruling is item 9 (O6). The review is a record and is not rewritten.

## W13 readiness: what only the owner does

1. The CONVENTIONS 1.18 row (decision 0248 item 2; the text is MA1 of the M7 audit), with a revision-history row and a status-line sentence. Then the trace gate reruns (release record rule 4) and the tag message names 1.18.
2. The "most canonical reference" sentence (CONVENTIONS line 12, "The design study is the most canonical reference for Galoot"; decision 0241 item 2). AGENTS.md calls the study "kept as history" and plan 0001 "the roadmap of record".
3. The tag's commit (O-D below), then the merge to `master` and the tag `1.0.0`, after U11 has set T.
4. The repository URL, whenever a repository exists (decision 0249 item 1). The push is the owner's.
5. Rulings on MA1 and MA2.

## Decisions for the owner, with recommendations

- **O-A (MA1). DoD coverage.** (a) U11 adds a DoD-1 to DoD-29 table to plan 0001's outcome, with DoD-3 collated from decisions 0110, 0114, 0115, 0167, 0172, 0178 and 0220 section 4 into one short record, and DoD-8 and DoD-13 stated (DoD-13 as a ruled departure). Records only, roughly 100k to 150k agent tokens; no matrix. (b) The owner accepts DoD-3, DoD-8 and DoD-13 as not shown for 1.0, by a decision, and U11 adds the table with those gaps. (c) Release as is. Tradeoff: (a) costs one records unit and closes the gap. (b) is cheaper and honest, but ships 1.0 with a core criterion unshown. (c) leaves the release record silent on three DoDs, against C20. **Recommendation: (a).**
- **O-B (MA2). Performance history.** (a) Declare in the release record that P9's history update was not made, and why. (b) Approve `tools/runlog compact` at T now. Tradeoff: (a) costs one paragraph and loses nothing, since the log keeps every row. (b) updates the history to 2026-11-12, but rewrites `runs.tsv` at release and needs `tools/runlog check` and a review of the new era. **Recommendation: (a).**
- **O-C (MA3).** (a) A new decision quoting the options and recommendations accepted by 0248 and 0249, written at U11. (b) Accept the omission as a departure. **Recommendation: (a)**; it is a short record.
- **O-D. What the tag names.** (a) Keep `--no-ff` and tag the merge commit M, recording that "the tag names T" is read as "names a commit whose tree is T's, checked by `git diff develop master`". (b) `git merge --ff-only develop`, so that `master` is T and the tag names T literally; `master` then has no release commit of its own, and its first-parent history is `develop`'s. (c) Tag T and leave M untagged; `FetchContent` of `1.0.0` then fetches T, and `git describe master` reads `1.0.0-1-g...`. **Recommendation: (a).** It keeps W7's "releases only" readable on `master`'s first-parent line, and the tree is the one the evidence covers.
- **O-E. The CONVENTIONS sentence.** (a) Keep it. (b) Reword it in the same 1.18 edit, for example to name CONVENTIONS itself and plan 0001 as the references, with the design study as history. Tradeoff: (b) avoids a later 1.19 for a sentence that the repository's own AGENTS.md already contradicts. It is governance text, so its wording is the owner's. **Recommendation: (b)**, folded into the 1.18 edit.
- **O-F (m1). The release commands.** Add the `develop` = T check and put the 1.18 tag message in the command. **Recommendation: yes**, at U11.
- **O-G (m3, m4).** Add m3's three items to the release record's known friction. For m4: (a) leave the README and add a friction line, or (b) say `include/galoot` in the README and rerun its tests. **Recommendation:** m3's lines in U11; m4 (a), since the copy does no harm and a further README change reopens rule 2.

## Whether the release is ready for the owner's walkthrough

Yes, once U11 has done the records work above. No finding touches `include/`, `test/`, `tools/`, `cmake/`, `docs/guide/` or the build files, so the matrix at R stands and no fix unit reruns it. U11's records are: the DoD table or the owner's ruling (MA1), the P9 declaration (MA2), the quoting decision (MA3), the command lines (m1), the merge of `develop` and the final T with its two gates (m2), and the friction lines (m3). If the owner prefers, MA1 to MA3 can be ruled at the walkthrough itself. They change no result.

## Not audited, and why (C20)

- The content of the W5 reviews of the window's code merges. I checked that they exist; the M7 audit read the ones in its part of the window.
- Validation referents. They were not re-read; the V10 table restates them.
- The matrix's pool and memory observations (i2), and the benchmark figures, which support no claim.
- `format-check` (departure 5) and the `plan_cost_model` checks. Their inputs did not change after R.
- Whether each plan's outcome is complete against its criteria. `plan-closures.md` did not judge that either.
- My own agent cost. It is not visible to me; the coordinator records it at hand-back.

## Appendix: this audit's run rows (in `runs.tsv` on this branch)

| Row | utc_start | revision | step | preset | selection | wall_s | exit | maxrss_kib |
| ---: | --- | --- | --- | --- | --- | ---: | ---: | ---: |
| 21960 | 2026-11-12T04:01:38Z | 18eb677d6361 | check | - | gate:runlog@18eb677 | 2.583 | 0 | 744364 |
| 21961 | 2026-11-12T04:01:41Z | 18eb677d6361 | check | - | gate:citations@18eb677 | 0.599 | 0 | 15988 |
| 21962 | 2026-11-12T04:01:41Z | 18eb677d6361 | check | - | gate:citations-guide@18eb677 | 0.074 | 0 | 15264 |
| 21963 | 2026-11-12T04:01:41Z | 18eb677d6361 | trace | - | gate:trace@18eb677 | 0.291 | 0 | 26168 |
| 21964 | 2026-11-12T04:01:42Z | 18eb677d6361 | check | - | conventions_check@18eb677 | 2.654 | 0 | 18112 |
| 21965 | 2026-11-12T04:02:08Z | 18eb677d6361 | build | - | walk:readme-gcc@18eb677 | 0.776 | 0 | 256724 |
| 21966 | 2026-11-12T04:02:09Z | 18eb677d6361 | build | - | walk:readme-clang@18eb677 | 0.756 | 0 | 227660 |
| 21967 | 2026-11-12T04:02:10Z | 18eb677d6361 | build | - | walk:rejected@18eb677 | 0.168 | 1 | 98040 |
| 21968 | 2026-11-12T04:02:10Z | 18eb677d6361 | build | - | walk:force-on@18eb677 | 0.468 | 1 | 213004 |
| 21969 | 2026-11-12T04:10:32Z | 18eb677d6361 | test | gcc-debug | gate:docs@18eb677 | 1.329 | 0 | 107728 |
| 21970 | 2026-11-12T04:10:33Z | 18eb677d6361 | test | clang-debug | gate:docs@18eb677 | 1.430 | 0 | 143132 |
| 21971 | 2026-11-12T04:10:41Z | 18eb677d6361 | test | gcc-debug | gate:docs-list@18eb677 | 1.336 | 0 | 107608 |
| 21972 | 2026-11-12T04:12:17Z | 18eb677d6361 | check | - | gate:runlog-final@18eb677 | 2.616 | 0 | 744028 |

Rows 21967 and 21968 exit 1 by design: they are the walkthrough's rejected example and the `ON` spelling, which must not compile. The test rows ran `ctest --test-dir <U8's tree> -R "readme|docs_pages|citations"` (11 of 11). The job pool read "running: 23 of 23 slots free" before and after; it was neither started nor stopped. Host `ds-001`; branch `gate/release-1.0.0`.
