# 0045 The 1.0.1 release procedure and records housekeeping

- Status: proposed 2026-11-16 by Claude (agent, Opus 5.5), planner and release process owner, with the auditor and build-expert role files read. From `main` at `c95fee5`, on branch `plan/0045` in the worktree `galoot-wt/plan-0045`. No unit below has started. The coordinator's merge of this plan into `main` is the claim (W7; section 1 item 1).
- Source of scope: the coordinator's brief of 2026-11-16 (items (a) to (g)); the handoff of 2026-11-15; plan 0042 and its outcome; the release record, the V10 table, the DoD table and `close-out-v1.sh` in `records/release/`; the release matrix and the release audit of 2026-11-12 in `records/gates/`; decisions 0245 to 0251; lessons 0073 and 0074. Plans 0043 (test, build and tool cost) and 0044 (header and interface hygiene) are written in parallel by other planners and were not read; this plan refers to them by number and states what each of their possible changes does to the release (section 3.4).
- Facts at `c95fee5`, each from a command run for this plan: the integration branch is `main`, tracking `origin/main`, and the governance still calls it `develop` (section 6); `master` is `2383e6d`, the `--no-ff` merge M of 1.0.0, and the tag `1.0.0` names it; `origin` (`https://github.com/IA-Workshop/galoot.git`) answers an unauthenticated `git ls-remote` with `HEAD` and `refs/heads/main` only, so neither `master` nor any tag is on the remote; the README's `FetchContent` block still holds `<galoot-repository-url>` with `GIT_TAG "1.0.0"`; toolchain g++ 16.2.1, clang++ 23.1.4, CMake 4.4.5, Ninja 1.13.2, the packages of `history/toolchain.tsv`'s last row; the job pool reads "not running".
- Shared resources: a unit that builds reads `tools/jobpool status` before and after, starts the pool with `tools/jobpool ensure` only when the status says "not running", never twice, and never stops it. Heavy steps run under `math-tools/exercises/performance/pe-common/memguard.sh`. Timed runs take the lock through `lock.sh take` and `release`. No processor configuration is touched or proposed. Web access is limited to plain unauthenticated `git ls-remote` of the public repository, with no personal identifier in any request. No third-party contact. GCC and Clang only.
- Numbers: the next free decision is 0252 and the next free lesson 0075 at `c95fee5`; plans 0043 and 0044 may take numbers first, so each unit takes the next free number when it writes.

## 1. Departures, ambiguities, and readings (C20)

1. **The plan is on `plan/0045`, not on `main`.** W7 says a unit is claimed by committing its plan to `develop` (now `main`), and W7's branch table has no `plan/<name>` row. The brief asks for this branch and a commit on it only. Reading followed: the coordinator merges it into `main` as a records-only merge (W5, decision 0215), and that merge is the claim.
2. **"Patch release" is not a term of the governance.** P9's release row and W13 make no difference between a major and a patch release. Reading followed: 1.0.1 takes P9's full release row; section 3 states the minimum that row allows and what an owner ruling could narrow (O6).
3. **"The owner already added the repository URL"** is read as the git remote. The README still holds the placeholder at `c95fee5` (verified), so filling it is part of the 1.0.1 release commit (section 4).
4. **No CI.** Every check below runs on the development machine; nothing proposes a hosted service.
5. **Not done here:** no build, test or matrix ran; nothing was implemented. The draft close-out script (section 9) was checked only by `bash -n`, by running it unfilled (it refuses: "T, R or MATRIX_ROWS is not filled in"), and by running its sections 3 and 4 checks by hand against the 1.0.0 facts (`gate:all@7580e88`: 33 rows, 0 failed; the passing `check` and `trace` rows on `ff1d49f`'s parent; `master` equal to `1.0.0^{commit}`).
6. **Checks of this commit.** The plan is records only (P8: documentation, no tests). The citation check reads code paths and the guide, not `docs/agent/plans/`, so it cannot be affected; it was not run.

## 2. Goal and definition of done

Release core 1.0.1, carrying the merged work of plans 0043 and 0044, the filled `FetchContent` block, and the records housekeeping below, with every check of P9's release row run on the released code and the governance's branch names brought in line with the repository.

Done when: (1) the owner has ruled O1 to O10 (section 10) and the rulings are recorded; (2) the branch-name text matches the repository (section 6); (3) 0043's and 0044's units are merged into `main` under W5 and P9; (4) the release matrix is green on all eleven presets at the release commit R; (5) the 1.0.1 release record states every interface change (none incompatible), the checks at R, and the performance history's state; (6) the release audit has no blocking finding; (7) the owner has merged `main` into `master`, tagged 1.0.1 and pushed, and an unauthenticated `ls-remote` shows `master` and both tags; (8) this plan's outcome, the lessons and the handoff are written.

Acceptance criteria: the matrix rows `gate:all@R` number three per preset and all exit 0; `tools/runlog check` and the trace gate pass at T; `git diff 1.0.0 R -- include/` changes no entry of `records/release/dod-2-inventory.md` except as 0044 states and the release record lists; `grep -w develop` over the governance, `AGENTS.md` and `.claude/agents/` prints nothing (under O1 (a)); the close-out script's dry run passes before the owner runs it.

## 3. Minimum local verification (brief item (a))

### 3.1 What P9's release row asks

"The full suite in every supported configuration, the benchmarks, and an update of the performance history (W13)." Plan 0042 read "every supported configuration" as all eleven named presets (its section 1 item 6), and the owner ruled for it (decision 0248 item 9, O5). Decision 0216 item 6 adds a recorded 50-seed campaign "at each gate and the release". W5's trace gate and `tools/runlog check` (P9, P13) close the list. So the minimum, without a new ruling, is:

| Check | Where it runs | 1.0.0 cost (record) |
| --- | --- | --- |
| Full suite, all eleven presets, at R | one tree per preset, serially (O6) | 2,883.7 s (48 min) |
| Trace gate, `conventions_check.py` (seed 20261031), `format-check` | repository root, lint tree | 0.3 s, 2.7 s, 1.7 s |
| `tools/runlog check` | repository root | 2.6 s |
| `plan_cost_model`: `comments_test.py`, both `cost.py comments`, both `selfcheck`s; `jobserver_check.py` | snippets, private pool | 0.2 + 0.2 + 161.2 + 12.7 s |
| Seed campaign, six programs × 50 seeds | the matrix's gcc-debug tree, one run at a time | 19 min (03:20:21 to 03:39:22 UTC) |
| Benchmarks once, gcc-measure and clang-measure | pinned to CPU 4 under the lock | under 1 s each |
| History update (P9, P13) | `tools/runlog compact` under O4 | not done at 1.0.0 (release record departure 9) |

The `plan_cost_model` checks were added by decision 0248 item 7 for 1.0.0, not by the governance. They cost 3 minutes and no tokens to speak of, and 0043 may change the tools they read, so they stay.

### 3.2 From empty trees, or incremental

All eleven presets run from empty trees at R. P12 asks for a clean build when the build description or the toolchain changes, or a tree is suspected stale; plan 0043 is likely to change the build description (inferred from its title), and every test unit includes core headers, so after 0044's header changes an incremental build recompiles nearly every unit anyway (inferred from the header layout: every `*_test.cpp` includes `galoot/core` headers). The difference is the configure step, 0.5 to 0.6 s per tree. An empty tree also settles the staleness question the auditor would otherwise ask. The main checkout's own `build/` trees date from 2026-11-01 to 2026-11-07 (`stat`) and are not used.

Incremental is right for everything else: the P9 merge gates of 0043 and 0044 into `main` (persistent trees, P12), the seed campaign (it reuses the matrix's gcc-debug binaries), the benchmarks (the matrix's measure trees), and any README-only rerun after R (its tests in both debug trees, as rule 2 of 1.0.0's "The tip T").

### 3.3 Expected wall time and memory, from the 1.0.0 matrix

Measured at `7276f82` (R's code), 2026-11-12, shared pool, 23 of 23 slots free, one preset at a time (`records/gates/2026-11-12-release-matrix.md`):

| Preset | Wall (s) | Largest compile |
| --- | ---: | --- |
| gcc-debug | 159.5 | `transform_identities_test.cpp` 5.93 GiB, 92.3 s |
| clang-debug | 100.0 | same unit, 1.51 GiB |
| gcc-release | 157.1 | same unit, 1.96 GiB |
| gcc-asan | 228.8 | same unit, 4.73 GiB |
| clang-release | 113.7 | same unit, 1.03 GiB |
| clang-asan | 204.5 | same unit, 2.68 GiB |
| lint | 264.9 | clang-tidy over everything |
| gcc-measure | 153.5 | 1.85 GiB |
| gcc-measure-o3 | 167.7 | 2.03 GiB |
| clang-measure | 667.4 | `identities_rounding_test.cpp` 602.5 s, 1.81 GiB |
| clang-measure-o3 | 666.6 | same unit, 601.5 s, 1.80 GiB |

Every suite takes about 63 s, bounded by `tools.jobpool` (61.9 s). The largest process the watchdog sampled was 5.91 GiB, and the least memory available was 92.4 GiB of 125 GiB, during the gcc-asan build. Today an unrelated session uses about two cores and `free` shows 106 GiB available, so the 1.0.0 margins hold (inferred). Expected for 1.0.1 without 0043's changes: 48 min of matrix, 19 min of seeds, 3 min of other checks, about 70 min of machine time. The memguard limit: `MEMGUARD_MIN_GB=15`, as gate 3's and the matrix's watchdog.

### 3.4 What plans 0043 and 0044 change in the list

Each line is conditional on what those plans contain, which this plan has not read.

| If the merged work... | Then the release... |
| --- | --- |
| changes `CMakeLists.txt`, `CMakePresets.json` or `cmake/` (0043, likely) | needs nothing beyond the empty trees above. If it changes test labels or a test preset's filter, the expected counts (657 on six presets, 649 on five) change, and 0043's merge record states the new counts, which become the matrix's acceptance figures |
| lowers clang-measure's build (0043, possible) | saves up to about 18 min: at `ed9288f`, before the forced inlining, clang-measure took 116.1 s against 667.4 s (release record, friction item 2). A forced-inlining change is a core interface question (decision 0244), not a test-cost one |
| removes or shortens `tools.jobpool` (0043, possible) | shortens each suite from about 63 s toward the next longest test, `core.identities` at 24.1 s: up to about 7 min over eleven suites. The test is the only guard of the pool's race invariant (decision 0228 item 9; 0248 item 8), so this is a ruling of 0043's, not of this plan |
| changes `tools/timed`, `tools/runlog` or `tools/jobpool` (0043, possible) | reruns `jobserver_check.py` and `tools/runlog check` at R (already listed) and puts the compaction (O4) after 0043's merge |
| changes headers under `include/` (0044, certain) | requires 0044's W10 before and after records (P1), its blind review and a delta review of any fix (W5), the DoD-2 inventory re-grepped against `dod-2-inventory.md`, and the semantic-version check of section 4.3. A header change cannot shrink the matrix: every preset compiles the core |
| touches `bulk_dual` or the generation it composes (0044, possible) | requires the fixed-use cost at n = 4 and at the largest supported n (decision 0247 item 4), recorded by 0044 |
| renames `l` (`basis_change.hpp:542`) or `m` (`bits.hpp:171`), or `#undef`s the four `GALOOT_DETAIL_*` macros (0044, likely, from the backlog) | closes release record friction items 10 and 4; the 1.0.1 release record says so. Neither is interface (release record, "Interfaces removed or changed incompatibly") |
| deletes the two `apply` overloads without a target layout (0044, possible) | changes no public name: they are in `detail` (`dod-2-inventory.md` line 81) |
| validates the `GALOOT_FORCE_INLINE` cache option (0043 or 0044, possible) | turns a configure value such as `2`, which 1.0.0 accepts as ON, into an error. Read as a bug fix (friction item 11), listed under user-visible changes, not as an incompatible change |
| changes CONVENTIONS (owner only) | reruns the trace gate and `conventions_check.py` at R, and the tag message names the new version |
| changes `docs/guide/` | needs nothing extra: its tests are in every suite |

### 3.5 Not repeated from 1.0.0

The M7 gate audit (there is no milestone), the DoD-1 to DoD-29 table and the DoD-3 map (plan 0001's DoD is the definition of 1.0; 1.0.1 restates it by reference), the V10 table in full (the 1.0.1 record lists only the rows whose evidence changed), and the deferred items of decision 0246 item 2 (WU17 parts G and H, the E1 and E2 reruns), which stay deferred and stay stated.

DoD-22's copy of the debug trees' compile rows (decision 0080) is not required by any release-level rule, since DoD-22 is a 1.0 criterion; P1 asks that the build record them, which the trees do. O9 asks whether to copy them again.

## 4. Version, changelog, and the README (brief item (b))

### 4.1 Where the version lives

By `git grep -F 1.0.0 -- . ':!docs/agent'` at `c95fee5`, three places, and no header carries a version (no `GALOOT_VERSION` macro, so a user who copies `include/` cannot read the version from the headers; adding one would add a public name, which is a 1.1.0 change, so it goes to the backlog as a note):

| File | Line | 1.0.0 | 1.0.1 |
| --- | --- | --- | --- |
| `CMakeLists.txt` | 6 | `project(galoot VERSION 1.0.0 LANGUAGES CXX)` | `VERSION 1.0.1`. Nothing reads `PROJECT_VERSION`: no install rule, no version file (decision 0240 item 5) |
| `README.md` | 23 | "This is the 1.0.0 release of the core." | "This is the 1.0.1 release of the core." |
| `README.md` | 72 | `GIT_TAG "1.0.0")` | `GIT_TAG "1.0.1")` |

Outside the tree: the annotated tag and its message (`galoot core 1.0.1, CONVENTIONS 1.18`, or the CONVENTIONS version in force), and the 1.0.1 release record.

### 4.2 The README's `FetchContent` block

At `c95fee5`, lines 60 to 77. The block is otherwise right: `rga3.readme_cmake_1` and `_2` build each `cmake` block as a project with `FETCHCONTENT_SOURCE_DIR_GALOOT` pointed at the tree and `FETCHCONTENT_FULLY_DISCONNECTED=ON` (`test/documentation.cmake:115-116`), and passed in every preset at R. Those tests never read the URL or the tag, by design. What the release commit changes:

1. Line 71: `GIT_REPOSITORY "https://github.com/IA-Workshop/galoot.git"`, the remote's URL. An unauthenticated `git ls-remote` of it answered on 2026-11-16, so the repository is public and the URL is right.
2. Line 72: `GIT_TAG "1.0.1"`. **The tag must also exist on the remote.** No tag is on `origin` today, so the block with the URL filled and `1.0.0` would fail for any user; the close-out prints the push, and R12 checks it (section 8).
3. Lines 61 and 62: drop "The repository below is a placeholder to replace with Galoot's repository;", keeping "the tag names this release:"; line 69: delete the comment "# Replace the placeholder URL with Galoot's repository."
4. Optional, O5 (c): `GIT_SHALLOW TRUE`. The repository's pack is 38 MiB over 1,836 commits on `main` (`git count-objects -vH`), mostly the history of the run log (inferred), and a shallow fetch of a tag takes only that tag's tree. The offline tests would not exercise it.

These change prose and the block only; the README's tests rerun in the matrix, since R is the commit that carries them.

### 4.3 Semantic version

1.0.1 is a patch: no public name is added, removed or changed incompatibly (W13; C12 asks a removal be announced in the release that removes it). The check, by the coordinator before the matrix (gate G2) and again by the auditor: the DoD-2 inventory re-grepped at R against `dod-2-inventory.md`, and `git diff 1.0.0 R -- include/ docs/guide/` read for any changed declaration. If 0043 or 0044 adds a public name or a CMake option a user sets, the release is 1.1.0, not 1.0.1, and section 4.1's strings change with it (O10).

### 4.4 Changelog or release notes

None exists (`git ls-files` holds no `CHANGELOG`, `NEWS` or release notes outside `docs/agent/`). The release record is a process record, and W12 keeps process records out of user documentation; DoD-27's reading keeps decisions, plans and records out of user pages. Now that the repository is public, a user upgrading from 1.0.0 has no user-facing place to read what changed, and C12's announcement of a removal needs one the first time anything is removed. O5 weighs a root `CHANGELOG.md` against the tag message alone.

## 5. Audits (brief item (c))

1.0.0 had two: the M7 gate audit (U3, 306,512 tokens, 1,021 s, at `f15632b`), which accepted the milestone and found records gaps before the costly matrix, and the release audit (U10, 282,191 tokens, 1,055 s, at `18eb677`). Decision 0248 item 9 (O3) chose two so that "M7 findings are fixed before the costly matrix".

For 1.0.1 there is no milestone, so the M7 audit has no counterpart. One release audit is proposed, after the matrix and the release records, at T, by one Opus auditor (O7). Proportion (C19): the code changes are 0043's and 0044's, each blind-reviewed at its merge (W5); the auditor checks governance conformance across the window, not the correctness of each change. What the M7 audit bought at 1.0.0, findings before the matrix, is bought here more cheaply by gate G2, a mechanical checklist the coordinator runs before the matrix (section 8).

Scope, the window `a4cd99d..T` (`master^2`, the tree 1.0.0 released, to the 1.0.1 records tip), weighted:

1. W13 and section 4.3: the inventory, the version strings, the README block and URL, the CONVENTIONS version in the tag message.
2. P9's release row: the matrix record against `runs.tsv`, row by row; the seed campaign log; the benchmarks run; the compaction's new era by `tools/runlog check` and a read of `eras.tsv` and `groups.tsv` (O4).
3. W5 and W10 for each code merge in the window: a review record per merge naming its reviewer and model, delta reviews of fixes, 0044's before and after records, decision 0247 item 4 if it applies.
4. Lesson 0073: each decision file in the window compared with its first commit (`git log --follow -p`), and each owner ruling quoting the options it accepts (decision 0228 item 6(a)). Lesson 0074: each record one unit hands to another named in that unit's deliverables.
5. The branch-name edit against the owner's ratified text (section 6).
6. The 1.0.0 known exceptions and friction: which ones 0043 and 0044 close, and that the rest are restated, with decisions 0141 and 0247's review date 2027-11-01.

Not audited: the 1.0.0 window (audited twice), the DoD table, the research notes' V9 marks unless the window adds a note. Estimate 150k to 220k tokens, about 20 minutes. The risk of one audit after the matrix: a finding under `include/`, `test/`, `tools/`, `cmake/` or the build files voids R and reruns the matrix, about 70 minutes of machine and about 230k tokens (1.0.0's U8 cost). G2 and the merge-time blind reviews are the mitigation.

## 6. The branch name (brief item (d))

Every occurrence of the word `develop` (`grep -o -w`) in the files that bind or brief current work, at `c95fee5`:

| File | Occurrences | Lines | Who edits |
| --- | ---: | --- | --- |
| `docs/agent/governance/WORKFLOW.md` | 13 | 28, 33, 58, 59, 62, 63, 67, 91 | owner ratifies (Amendment) |
| `docs/agent/governance/PERFORMANCE.md` | 1 | 73 (P9's table) | owner ratifies |
| `docs/agent/governance/STYLE.md` | 2 | 26, 30 | owner ratifies |
| `docs/agent/governance/VVUQ.md` | 1 | 42 | owner ratifies |
| `AGENTS.md` | 2 | 23, 24 | any contributor |
| `.claude/agents/auditor.md` | 3 | 3, 20 | any contributor |
| `.claude/agents/blind-reviewer.md` | 1 | 3 | any contributor |
| `.claude/agents/{cpp-expert,math-expert,numerical-expert,performance-engineer}.md` | 1 each | 30 | any contributor |
| `.claude/agents/hci-expert.md` | 1 | 31 | any contributor |
| `.claude/agents/doctest-expert.md` | 1 | 37 | any contributor |

Seventeen occurrences in twelve lines of four governance files, and twelve in eleven lines of ten agent files. Nothing under `tools/`, `cmake/`, `test/`, `include/`, the build files, the README or `docs/guide/` names the branch (`git grep -w develop`; `tools/timed` records whatever branch is checked out). CONVENTIONS does not name it.

Left as they are, because they are history and a recorded text is not rewritten (W6): about 240 other files under `docs/agent/` (plans, decisions, records, lessons, research notes, snippet reviews), the handoff of 2026-11-15, and `snippets/plan_cost_model/mirror.py:20` ("the headers of develop at 4895a40", which names a commit and stays true). The next handoff says `main`.

The minimal edit set, under O1 (a): replace `develop` by `main` in those twenty-nine places, word for word, and nothing else. `AGENTS.md` line 23 then reads "The main checkout stays on `main`", W7's table row reads `main`, and W13 reads "a merge of `main` into `master`". A decision records the rename (C18: it changes a governance document) and quotes the options. The governance part takes effect when the owner ratifies it; the agent files change in the same records commit, so that the brief text and the governance agree on one day. Check: `git diff --word-diff` shows only `develop` to `main`, and `grep -w develop` over those files prints nothing.

**`master` stays the release branch** under O2 (a). W7 and W13 then need no change beyond the word above, and decision 0250 item 4's reason for a `--no-ff` merge ("it keeps W7's 'releases only' readable on `master`'s first-parent line") still holds. Since `master` is not on the remote, the owner pushes it with the tags (O2).

One consequence for the release commands: the 1.0.0 check `git merge-base --is-ancestor master develop` now fails (verified: `master` is the merge M, which `main` does not hold). The draft script checks instead that `master` is still `1.0.0^{commit}`, that `main` holds `master^2`, and that `master`'s tree is `master^2`'s; all three hold at `c95fee5` (verified).

## 7. Records (brief item (e))

### 7.1 P9's history update and `tools/runlog compact`

The history's one era, `ebdd06d09289`, holds the rows of 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z; `tools/runlog` updates the history only by `compact`, which needs the owner's approval (P13), and 1.0.0 declared the departure instead (decision 0250 item 2). The log now holds 21,978 rows over 2026-10-31T21:44:05Z to 2026-11-15T16:40:16Z, 7.9 MB (`ls -l`, `awk`).

What a compaction would do, estimated from the row counts and the stay rules of `history/README.md` ("The rows that stay"), not by a dry run: 11,858 rows (54%) are at `-dirty` revisions, and these leave the log unless they are `workflow` rows of a named preset (580 `workflow` rows in all); 6,069 are `measure`, `probe`, `diagnostics`, `asm`, `spike`, `run` or `compile` steps, which leave unless anchored. So roughly half to two thirds of the rows would leave, leaving about 7,000 to 11,000 (inferred). Every `gate:` row, every committed `configure`, `build` and `test` row of a named preset, and every cheap-gate row stays.

O4 weighs three choices; the recommended one compacts at the 1.0.1 matrix's records tip, after 0043 and 0044 are merged, in the release records unit (R7), so that the history covers the 1.0.1 matrix and the auditor checks the new era. Preconditions: no other branch holding new `runs.tsv` rows is open (the union merge rule would bring removed rows back, which `tools/runlog prune` then removes, P9), and any 0043 change to `tools/runlog` is merged first. Command, in R7's worktree: `tools/timed check - gate:runlog-compact@<tip> -- tools/runlog compact <tip>`, then `tools/runlog check`, both rows committed with the history files.

### 7.2 Lessons

Two candidates for lesson 0075 and the next, written in R10 if they still stand at T:

1. **A branch named by the governance was renamed without the governance.** The integration branch became `main`, and twenty-nine places in the governance and the agents' briefs still said `develop`; every brief since has had to say so. What detects it: a rename is a decision under C18 whose unit edits the governing text and the briefs in one commit, and G2 greps for the old name.
2. **The released README fetched a tag no remote held.** 1.0.0's `FetchContent` block named `1.0.0` while neither the URL nor the tag was published, and nothing checks a fetch block against a remote, because the tests are offline by design. What detects it: after the owner's push, an unauthenticated `git ls-remote` of the README's URL for the README's tag (R12).

### 7.3 Handoff

R12 writes `records/handoff-<date>.md` after the tag and points `AGENTS.md` line 18 at it (one line, as `8f34d8d` did). It states: `main` is the integration branch; `master` and the tags on the remote or not; the open owner items; the backlog of plan 0042 section 9 less what 0043 and 0044 closed.

## 8. Units, order, and gates

| # | Unit | Role, model | Branch | Depends on | Merge checks |
| --- | --- | --- | --- | --- | --- |
| R0 | Merge plans 0043, 0044 and 0045 into `main` (the claims) | coordinator | `plan/0043`, `plan/0044`, `plan/0045` | none | records only |
| R1 | Owner rulings on O1 to O10 here and on 0043's and 0044's items; one decision quoting the options (lesson 0073) | coordinator, Sonnet | `records/rulings-1.0.1` | R0 | records only |
| R2 | Branch name: the twenty-nine replacements of section 6, after the owner ratifies the governance text; the rename's decision | implementer, Sonnet | `records/branch-main` | R1 (O1). Must not run while a unit of 0043 or 0044 edits a governance file or a role file; the coordinator orders them | records only; `git diff --word-diff` and the grep of section 6 |
| R3 | Plan 0043's units | per plan 0043 | per plan 0043 | R0; R2 if 0043 edits PERFORMANCE.md | P9 `main` merge row, W5 |
| R4 | Plan 0044's units, with W10 records | per plan 0044 | per plan 0044 | R0; one merge into `main` at a time with R3 (W7) | P9 `main` merge row, W5, delta reviews |
| G1 | Gate: R3 and R4 on `main`, each merge's message naming its selection, configurations, outcome and wall time (P13) | coordinator | none | R3, R4 | none |
| R5 | Release commit R: section 4.1's strings, section 4.2's README edits, `CHANGELOG.md` under O5 (b) | build-expert, Sonnet | `feature/release-1.0.1` from `main` | G1, R1 | one Sonnet blind review of R (decision 0240 item 8); the checks are R7 |
| G2 | Readiness checklist on R, mechanical: section 4.3's inventory check; `grep -w develop` (under O1 (a)); the URL by unauthenticated `ls-remote`; no open branch with `runs.tsv` rows; decision files in the window unchanged since their first commit; every record handed between units named in a deliverable (lesson 0074); `tools/jobpool status` | coordinator | none | R5 and its review | none. A finding is fixed before R6 |
| R6 | Release matrix at R: section 3.1's list, eleven presets from empty trees, one at a time under memguard; the seed campaign into `records/gates/<date>-release-1.0.1-seeds.tsv`; the benchmarks once under the lock; the matrix record `records/gates/<date>-release-1.0.1-matrix.md`; DoD-22's copy under O9 | performance-engineer, Opus | same branch | G2 | P9 release row, every row `gate:all@R` |
| R7 | Release records: `records/release/release-record-1.0.1.md` (a delta on the 1.0.0 record: R, T's rule, the matrix, the checks, interface changes, friction and exceptions closed or kept, baselines with dates, CONVENTIONS version); the compaction under O4; the closing gates | planner, Opus | same branch | R6 | records only; `tools/runlog check`, trace gate, citations over code paths and `docs/guide` |
| R8 | Release audit, section 5 | auditor, Opus | `gate/release-1.0.1` from R7's tip | R7 | records only |
| R9 | Owner rulings on the audit; fixes. A fix outside `docs/agent/` voids R and reruns R6 whole (W8 applies) | coordinator; implementer as needed | as needed | R8 | as the fix needs |
| R10 | T: the close-out script filled with T, R and the row count, reviewed by a Sonnet reviewer against section 9 and its dry run, committed as `records/release/close-out-1.0.1.sh`; plan 0045's outcome; the lessons; the closing gates' rows; `main` fast-forwarded to T | coordinator, Sonnet | same branch, then `main` | R9 | `tools/runlog check`, trace gate |
| R11 | Owner: walkthrough (O8), `close-out-1.0.1.sh --dry-run`, then the script, then the printed push | owner | `main`, `master` | R10 | the script's checks |
| R12 | After the push: `git -c credential.helper= ls-remote https://github.com/IA-Workshop/galoot.git refs/heads/master refs/tags/1.0.0 refs/tags/1.0.1` shows all three; handoff and `AGENTS.md` pointer (section 7.3); cost rows | coordinator, Sonnet | `records/handoff-1.0.1` | R11 | records only |

Order: R0, R1; then R2 beside R3 and R4 (different files); G1; R5; G2; R6; R7; R8; R9; R10; R11; R12. The seed campaign and the benchmarks run after the eleven presets, in R6, as at 1.0.0. Plans 0043 and 0044 decide their own internal order; this plan needs only that both are merged before R5, or that the coordinator records which one is held back to a later release.

## 9. Close-out script for the owner (brief item (f)): DRAFT

**Draft.** Not reviewed and never run; R10 fills it, has it reviewed, and commits it under `records/release/`. Modeled on `close-out-v1.sh`, without the CONVENTIONS step (no CONVENTIONS change is planned) and with the `master` checks of section 6. It refuses to run until `T`, `R` and `MATRIX_ROWS` are filled.

```bash
#!/usr/bin/env bash
# DRAFT (plan 0045, section 9). Not reviewed and never run. Unit R10 fills
# T, R and MATRIX_ROWS, has it reviewed, and commits it as
# docs/agent/records/release/close-out-1.0.1.sh; until then it refuses to run.
#
# Closes out Galoot core 1.0.1 from the main checkout on `main`: checks that
# `main` holds the audited records tip T with only records after it, that the
# version strings and the README's FetchContent block name 1.0.1 and the
# repository, that the matrix and the closing gates passed, and that `master`
# has not moved since 1.0.0; then merges `main` into `master` with --no-ff in
# a temporary worktree and tags 1.0.1 on that merge commit.
# Nothing is pushed: the push and the check after it are printed at the end.
# `close-out-1.0.1.sh --dry-run` runs every check and stops before the merge.
set -euo pipefail
cd /home/brianm/projects/utils/math-core/galoot
T=________            # the records tip T (plan 0045, outcome)
R=________            # the release commit R, as the matrix selection names it
MATRIX_ROWS=________  # rows of gate:all@R: 3 per preset, 33 for eleven presets
V=1.0.1
PREV=1.0.0
CONV_VERSION=1.18     # the CONVENTIONS version this release implements
URL=https://github.com/IA-Workshop/galoot.git
RUNS=docs/agent/records/performance/runs.tsv
W=/home/brianm/projects/utils/math-core/galoot-master

fail() { echo "close-out: $*" >&2; exit 1; }
case "$T$R$MATRIX_ROWS" in *_*) fail "T, R or MATRIX_ROWS is not filled in";; esac

[ "$(git branch --show-current)" = main ] || fail "not on main"
[ -z "$(git status --short)" ] || fail "working tree not clean"
if git rev-parse -q --verify "refs/tags/$V" >/dev/null; then fail "tag $V exists"; fi

# 1. main holds T, and nothing but records changed after it.
git merge-base --is-ancestor "$T" main || fail "main does not hold T"
[ -z "$(git diff --stat "$T" main -- . ':!docs/agent')" ] || fail "non-records change after T"
[ -z "$(git diff --name-only "$T" main -- docs/agent/governance)" ] || fail "governance changed after T"

# 2. The version strings and the README's FetchContent block at main.
grep -qF "project(galoot VERSION $V LANGUAGES CXX)" CMakeLists.txt || fail "CMake version"
grep -qF "This is the $V release of the core." README.md || fail "README status line"
grep -qF "GIT_REPOSITORY \"$URL\"" README.md || fail "README repository URL"
grep -qF "GIT_TAG \"$V\")" README.md || fail "README tag"
if grep -qF '<galoot-repository-url>' README.md; then fail "README placeholder left"; fi
[ "$(git remote get-url origin)" = "$URL" ] || fail "origin is not $URL"
c=$(sed -n 's/^Status: version \([0-9.]*\),.*/\1/p' docs/agent/governance/CONVENTIONS.md)
[ "$c" = "$CONV_VERSION" ] || fail "CONVENTIONS is $c, not $CONV_VERSION"

# 3. The matrix at R passed, and the closing gates ran on T's parent's tree.
#    runs.tsv columns: 2 revision, 4 step, 6 selection, 8 exit.
git show "main:$RUNS" | awk -F'\t' -v s="gate:all@$R" -v n="$MATRIX_ROWS" '
  $6 == s { rows++; if ($8 != 0) bad++ }
  END { if (rows != n || bad) { print "matrix rows " rows+0 ", failed " bad+0; exit 1 } }' \
  || fail "matrix rows"
P=$(git rev-parse --short=12 "$T^")
for step in check trace; do
  n=$(git show "$T:$RUNS" | awk -F'\t' -v c="$P" -v s="$step" '$2 == c && $4 == s && $8 == 0' | wc -l)
  [ "$n" -ge 1 ] || fail "no passing $step row at $P"
done

# 4. master has not moved since the last release: it is the --no-ff merge
#    commit that the previous tag names, and its tree is its second parent's,
#    which main holds.
[ "$(git rev-parse master)" = "$(git rev-parse "$PREV^{commit}")" ] || fail "master moved since $PREV"
git merge-base --is-ancestor master^2 main || fail "main does not hold master^2"
[ -z "$(git diff master master^2)" ] || fail "master's tree is not its second parent's"
[ ! -e "$W" ] || fail "$W exists"

if [ "${1:-}" = "--dry-run" ]; then echo "dry run: every check passed; nothing merged or tagged"; exit 0; fi

# 5. Merge and tag.
git worktree add "$W" master
git -C "$W" merge --no-ff main -m "Release galoot core $V"
git worktree remove "$W"
[ -z "$(git diff main master --stat)" ] || fail "master's tree differs from main's"
git tag -a "$V" master -m "galoot core $V, CONVENTIONS $CONV_VERSION"
echo "done: master is at $(git rev-parse --short master), tag $V -> $(git rev-parse --short "$V^{commit}"). Nothing was pushed."
echo "To publish:   git push origin main master $PREV $V"
echo "Then check:   git -c credential.helper= ls-remote $URL refs/heads/master refs/tags/$PREV refs/tags/$V"
```

Notes for R10's reviewer: `! grep` is avoided on purpose, since `set -e` ignores a negated command; the step-3 row check reads `main`'s log for the matrix and T's log for the closing gates, as 1.0.0's "The tip T" rule 3 puts T's gate rows on T's parent's tree; if O5 (b) adds `CHANGELOG.md`, add a check that it names `$V`; if the owner changes CONVENTIONS before the tag, add 1.0.0's CONVENTIONS step and its trace-row check back.

## 10. Owner decisions (each with a default if not ruled)

- **O1. The branch name in the governance.** (a) Replace `develop` by `main` in the seventeen governance occurrences, ratified as one text, with the twelve agent-file occurrences in the same commit; (b) add one ratified sentence to W7, "The branch this document calls `develop` is named `main`", and leave the rest; (c) rename `main` back to `develop` locally and on the remote. Tradeoff: (a) is a mechanical edit that a word diff shows whole, and agents stop being told to translate; (b) changes one line but leaves twenty-eight wrong ones, which every brief must keep correcting; (c) undoes the owner's choice and GitHub's default. Recommendation: (a). Default: (a) for the agent files, which need no ratification, and nothing in the governance until ratified.
- **O2. `master` and the remote.** (a) `master` stays the release branch; the owner pushes `master` and the tag `1.0.0` now (`git -C /home/brianm/projects/utils/math-core/galoot push origin master 1.0.0`), and `master` and `1.0.1` at the close-out; (b) retire `master`: tags go on `main`, W7 and W13 are amended, and decision 0250 item 4's reading is superseded; (c) keep `master` local only. Tradeoff: (a) keeps the governance and the 1.0.0 procedure, and makes `GIT_TAG` work for users; the 1.0.0 tag's README holds the placeholder, which the tag's own history explains; (b) is simpler on GitHub but changes ratified text and loses the first-parent release line; (c) leaves `FetchContent` of any tag broken. Recommendation: (a), with the 1.0.0 push now. Default: (a), pushes at the close-out.
- **O3. The repository URL in the README.** Fill `https://github.com/IA-Workshop/galoot.git` in the release commit (decision 0249 item 1 said "when a repository exists"). Recommendation: yes. Default: yes.
- **O4. `tools/runlog compact`.** (a) Approve a compaction at the 1.0.1 matrix's records tip, in R7, so that P9's release row is met and the auditor checks the new era; (b) approve one at `a4cd99d`, the tree 1.0.0 released, before the matrix, so that no log is rewritten at release time, and the history then stops at 1.0.0; (c) decline, and the 1.0.1 record declares the departure again. Tradeoff: (a) meets P9 and shrinks the log by about half to two thirds (inferred), but rewrites `runs.tsv` inside the release and adds about 60k to 100k tokens to R7 and R8 together; (b) has the same cost early, with a history one release behind; (c) costs nothing and loses nothing, since the log keeps every row, but it is the second release in a row to miss P9's row. Recommendation: (a). Default: (c).
- **O5. Release notes and the README block.** (a) The annotated tag message only; (b) a root `CHANGELOG.md` with a 1.0.0 entry ("first release") and a 1.0.1 entry listing the user-visible changes in plain words, no record citations, no code blocks, kept by each release commit; (c) as (a) or (b), and add `GIT_SHALLOW TRUE` to the `FetchContent` block. Tradeoff: (a) adds no file, but a tag message is hard for a user to find; (b) adds one short file with a present user, the public repository's readers, and the place C12's announcement of a removal will need; `GIT_SHALLOW` saves users most of a 38 MiB fetch but is untested by the offline example tests. Recommendation: (b), with `GIT_SHALLOW TRUE`. Default: (a), no `GIT_SHALLOW`.
- **O6. The matrix's shape.** (a) All eleven presets, one at a time, as at 1.0.0 (about 48 min before 0043); (b) all eleven, with the two clang-measure builds started first and run beside the others on the shared pool, suites still one at a time (about 30 min, inferred: their 600 s units hold one slot each while the other trees build on the remaining slots); (c) the nine gate presets only, without the two `-O3` presets (saves about 14 min). Tradeoff: (b) saves about 18 min of machine time and no tokens, but its per-preset wall times are not comparable with 1.0.0's, which 0043 may want as its comparison; (c) leaves a level users choose untested, against decision 0248's O5. Recommendation: (a). Default: (a).
- **O7. One audit.** (a) One release audit after the matrix, section 5, with gate G2 before the matrix; (b) two, as at 1.0.0, a pre-matrix audit of the window to R and the release audit. Tradeoff: (b) costs about 200k to 300k more tokens and finds code findings before the matrix; with G2 and the merge-time blind reviews, what it would find early is mostly records. Recommendation: (a). Default: (a).
- **O8. The walkthrough.** (a) Light: read the 1.0.1 release record, the changelog (if O5 (b)) and the README's install section; build the README's first example with both compilers (plan 0042 section 8, step 3); run the script's dry run. About 15 minutes. (b) Full, as plan 0042 section 8. Decision 0246 item 3's "not waived" was a ruling for 1.0. Recommendation: (a). Default: (a).
- **O9. DoD-22's compile rows at R.** (a) Copy the debug trees' rows into `compile.tsv` by decision 0080's step, as at 1.0.0 (about 944 rows, about 0.5 MB, minutes of agent time), so that peak memory is comparable release to release; (b) skip, and rely on 0044's W10 records. Recommendation: (a). Default: (a).
- **O10. The version number.** If 0043 or 0044 adds a public name or a user-set CMake option, (a) hold that addition to a 1.1.0, or (b) release 1.1.0 instead of 1.0.1. Recommendation: (a), so that the patch stays a patch; only an issue if either plan proposes an addition. Default: (a).

**Only the owner can:** rule O1 to O10; ratify the governance text of O1; push; walk through, merge into `master` and tag; approve a compaction (O4).

## 11. Evidence, risks, and cost

- Evidence (V10): R6's rows at R, each `gate:all@R`; the seed log; the benchmark rows; the closing gates at T; the audit. Seeds: doctest 0 in the suites, 1 to 50 in the campaign, 20261031 for `conventions_check.py`, 20261110 for the benchmarks.
- Risks: (1) 0043's or 0044's merge slips, and the release waits or ships one of them; the coordinator records which. (2) A finding after the matrix reruns it (section 5). (3) The compaction under O4 (a) fails `tools/runlog check`; then it is reverted by a new commit, (c) applies, and the record says why. (4) The owner's push is forgotten and the README names an unpublished tag again; R12's `ls-remote` is the check. (5) The pool stops itself after 30 idle minutes and `tools/timed` refuses a build step (1.0.0's departure 3); each unit that builds reads the status first.
- Machine time (inferred from the 1.0.0 records): two P9 `main` merge gates, each about 24 min (gcc-debug 160 s, clang-debug 100 s, one sanitizer tree about 230 s, clang-measure 667 s, lint 265 s, from the 1.0.0 matrix), plus 0044's pinned W10 measurements, about 10 to 20 min; the release matrix 48 min (less under 0043), the seeds 19 min, other checks 3 min. About 2 to 2.5 hours of machine in all, under 1.5 hours of it heavy.
- Agent cost (W3), tokens, by the 1.0.0 rows of `agent-cost.tsv` (plan 0042's units and plan 0040's reviews and gates). The 0043 and 0044 lines are placeholders until those plans give their own estimates.

| Item | Model | Tokens | Wall |
| --- | --- | ---: | --- |
| Planning: 0043, 0044, 0045 | Opus | 450k to 650k | about 20 min, in parallel |
| R1 rulings, R2 branch name | Sonnet | 60k to 100k | 10 min |
| 0043 and 0044 implementation | Opus | 300k to 600k | 30 to 60 min |
| Their blind reviews and delta reviews | Opus or Sonnet | 250k to 500k | 20 to 40 min |
| Two P9 `main` merge gates | Opus | 300k to 450k | 2 × 45 min |
| R5 release commit and its review | Sonnet | 100k | 5 min |
| R6 matrix, seeds, benchmarks | Opus | 200k to 250k | about 80 min |
| R7 release records and compaction | Opus | 200k to 300k | 20 min |
| R8 audit | Opus | 150k to 220k | 20 min |
| R9, R10, R12 coordinator and records | Sonnet | 100k to 200k | 20 min |
| Total | | about 2.1 M to 3.4 M | about 6 to 8 hours elapsed, plus the owner's replies |

For comparison, plan 0042 (records, matrix and audits, no code unit) recorded about 2.3 M tokens; this train adds two code plans, so its upper half is the likelier figure (inferred).

## 12. Outcome

Not started.
