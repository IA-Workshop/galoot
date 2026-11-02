# W5 delta review: the review fixes of feature/runlog-distilled (plan 0017)

- Reviewer: blind-reviewer, Claude Sonnet 5.5 (`claude-sonnet-5-5`), 2026-11-01. Agent work.
- Reviewed: `git diff 69f814f..98fa35b` on `feature/runlog-distilled` (15 commits; the last content commit is `ba52a8e`, the head `98fa35b` adds run rows). Reference: the first review, `bbe13f2` (`docs/agent/records/reviews/2026-11-01-runlog-distilled.md`, on `review/runlog-distilled`), whose findings F1 to F5 and F10 are the subject. `develop` is at `2d90f41`. `feature/core-mv-a` is at `f066f02`.
- Brief: no verdict on the change. The brief gave the commits, the checks to make, and my own tools to use. I did not use the author's scripts; the one exception is the author's mutation driver, which I reran only to see whether its 41 of 41 reproduces (it does). Blindness is partial: I read the plan's "W5 review fixes" section and decision 0170's addendum when I judged the wording in check 1, after I had made the recomputations.
- Worktree and trees: `.claude/worktrees/delta-runlog`, branch `review/runlog-delta` from `98fa35b`; build trees `build/gcc-debug`, `build/clang-debug`, `build/lint` in it. Every build and test ran through `tools/timed`. My run rows (selections `delta-*`) are uncommitted in `runs.tsv` and are not part of this commit. The job pool was never started or stopped (23 of 23 slots free when I looked).
- Verdict: **accept after fixes**, with no fix that blocks anything but plan and wording consistency (D2). Every one of the first review's findings F1 to F5 and F10 is fixed or honestly reworded, I found no way for the rule to lose a row it claims to keep, and both merge directions with `feature/core-mv-a` at its current head pass. What remains is a short list of non-blocking items (D1 to D8).

## Not checked or not run (C20)

- gcc-release, clang-release, clang-asan, gcc-asan were not run. The change adds no C++ outside test fixtures, so P9 at this level is the two debug presets and lint. I did not rerun the first review's asan run (P10: `tools/` and `test/` code is new since then, but asan does not change a Python tool's behavior).
- Not every removed row was judged. D1 lists families by step and name; there may be gate runs under names I did not think of.
- Mutants: I ran 14 of my own. I did not re-derive the author's mutants beyond rerunning the driver (41 of 41 killed).
- Memory: I report `maxrss` of one `check` run only, not a distribution.
- A source tarball nested in another git repository: `tools/runlog check` would then look at the outer repository. Not tried.
- The wording of P8 and P9 that decision 0170 proposes was read, not applied; nothing in governance changed, so there is nothing to run.
- Agent cost: about 60 tool calls. Token counts are not visible to me.

## 1. F1: gate rows in the log, and the wording

**Pass, with residual gaps that the new wording states.**

Recomputed from `git show ebdd06d:docs/agent/records/performance/runs.tsv` (235,591 lines, 0 duplicate lines):

- The first 1,727 rows of the head's `runs.tsv` are an ordered, byte-identical subsequence of the old log (greedy match; first 1,727 rows matched in order, contiguous; the remaining 56 rows are not in the old log). `eras.tsv` records `kept` 1,727 and the same three anchors (235538, 235539, 235591).
- `git diff --numstat 227d2c8~1 227d2c8`: `runs.tsv` +58 −0, `eras.tsv` 1/1. Between `a3f0a60` and `227d2c8` the total is +76, of which 18 are the earlier run-row commits; no line is removed.
- `groups.tsv`: 1,207 lines (1,206 groups); my own `Fraction` script computes the same 1,206 group keys and a total of 235,590 rows; 31,356 cells compared (rows, dirty, revisions, first and latest time and revision, hosts, and all 18 statistics of each group), 0 differences. Sample of 20 groups (seed 94), as wall time first/latest/min/median/range: `test|clang-debug|review|shared|pass` 6 rows 0.430 / 2.194 / 0.430 / 2.194 / 17.831; `measure|-|antiproduct-growth|pinned|pass` 240 rows 0.094 / 0.242 / 0.090 / 0.167 / 0.649; `measure|-|limits-0012|shared|pass` 158 rows 0.338 / 0.835 / 0.174 / 0.464 / 1.853; `build|clang-asan|fresh|shared|pass` 5 rows 25.113 / 6.685 / 6.669 / 6.885 / 18.444; the other 16 agree in the same way.
- Each counterexample of the first review:

| Counterexample | Now |
| --- | --- |
| Seven `all-clean` workflows of the develop merge `eb9a8fa` at `0863badb9c5f-dirty` (old lines 748 to 754) | kept (workflow at any revision) |
| The two header builds and the `fresh-tree` configure and build at `0863badb9c5f-dirty` (old lines 755 to 758) | **still removed**; the README and plan D1 name exactly these four |
| `reference-markers`, `golden-facts` at `d9c5059` (old 3,796, 3,797) | kept |
| `lint - ruff`, `ruff-measure` at `ed3783e`, `lint clang-tidy` at `db9b9ad` | kept |
| Dirty trial-merge workflows (`f9441f77d27c-dirty`, `721c28783890-dirty all-merge`) | kept |
| The three cheap gates beside `all-merge` at `721c28783890-dirty` (`convention-trace`, `reference-markers`, `golden-facts`) | **still removed** (dirty revision) |

Remaining gate-run families the rule misses, in the old log (my script over the 233,863 removed rows): 17 cheap-gate rows at `-dirty` revisions (`trace-gate` 5, `markers` 4, `convention-trace` 3, `gate trace` 2, `golden` 1, `reference-markers` 1, `golden-facts` 1); 18 dirty header-set builds (`all_verify_interface_header_sets`) and 2 `fresh-tree` rows; 176 of the 8,518 dirty preset `build` and `test` rows whose selection looks like a full suite (`all`, `p9`, `final`, `fresh`, and so on); `check` steps that are gate-like but not named in the cheap list (`jobserver_check:all`, `measure_record-before/after`). I count roughly 215 rows. These are rows of uncommitted trees or of other names, which the README, plan D1/D2/R2, and decision 0170's addendum now say are counted in the history and read back from git. I found no row that the new text says is kept and is not.

Wording, judged:

- `history/README.md` ("The rows that stay"), `records/performance/README.md`, the plan's Goal, D1, D2, and decision 0170's addendum are honest. They describe the rule by step, preset, revision, and name, say that gate runs are not marked, say "at least" the four WU1 rows, and say that the promise needs a marker. The proposed P13 and DoD-25 sentences in the addendum promise only what the rule keeps.
- The `gate:` selection prefix proposed in the addendum is a sound next step; nothing enforces it yet, and the addendum says so.
- Not honest or not finished: see D2 (the plan's section 5 keeps the old false sentences with no pointer) and D3 (one phrase of the proposed P13 text).

## 2. F2: `tools.runlog_check`

**Pass, with an open owner question that the author stated correctly.**

- Runs in a build tree: gcc-debug 159 of 159 passed at `98fa35b` (test `tools.runlog_check` 3.40 s of a 5.53 s suite, the longest test); clang-debug 161 of 161 (2.68 s of 3.52 s); lint 161 of 161. It is registered once and runs in every preset with identical content (it does not depend on the preset).
- Cost of one run by hand, through `tools/timed`: 2.657 s, peak 743,804 KiB (about 726 MiB). The memory is the 76 MB era log read from git, held as lines and counters.
- Depth-1 clone (`git clone --depth 1`, checked out at `98fa35b`): `tools/runlog check` exits 2 with "era ebdd06d09289 is not in this clone's history, so it cannot be recomputed", which `SKIP_REGULAR_EXPRESSION` matches, so CTest reports it skipped. Outside a git checkout (a copy of `tools/` and `docs/` in a plain directory): exit 2 with "not inside a git checkout", which also matches.
- Decision 0080: the author's concern is right and the result is worse than a skip. With `docs/agent/records/performance` deleted, or all of `docs/agent`, in a checkout, `check` exits 1 ("`eras.tsv` is missing, though commit 227d2c8 has changed it"), so a contributor who clears the agent records gets a failing build-time test, not a skipped one. That is deliberate (F5: a merge or edit that drops the history must fail), and it is the same act that 0080 wants not to change what a build does. Both goals cannot hold in one default test. Options for the owner: keep it as is and accept that deleting agent records fails one test; make the missing-records case skip only when `.git` has no such history (it has); or take the test out of the default set by its label (`tools.runlog_check` is a label of its own) and put the check in the merge procedure of P9, as the addendum proposes. I judge the second-to-last option unsound and prefer the label, because the check costs 2.7 s and 0.7 GiB and gives the same answer in all seven presets.

## 3. F3, F4, F5, F10

**F3 (citation exception): pass.** Three hostile paths, run through `tools/test_references.py citations` on a scratch file: `docs/agent/records/performance/history/README.md`, `.../history/../notes.md`, and `.../history/eras.tsv/x` are each rejected ("cites a project document or record"); `.../history/notes.tsv` is rejected too. The three tables and the directory itself are accepted. Two shapes slip through and are low: `.../history.md` and `.../history/eras.tsv.bak` (the same tail `\b(?!/)` already let `runs.tsv.bak` through before the change, so this is old behavior extended to the new names; see D5).

**F4 (self-check cases): pass.** The author's 41 mutants: all killed (rerun, 41 killed, 0 survived, 162 s). Of the first review's seven survivors, my reruns of `toolchain_end_inclusive` and `groups_unknown_era_ok` are now killed. See the table below for new mutants.

**F5 (order, missing history): pass.** By hand, in a scratch clone at `98fa35b`: swapping two kept rows (lines 101 and 102) gives exit 1, "the rows era ebdd06d09289 kept are out of their order from line 101"; deleting `eras.tsv` and `groups.tsv` gives exit 1 ("`eras.tsv` is missing, though commit 227d2c8 has changed it"); deleting `groups.tsv` alone gives exit 1 ("0 group rows, but recomputes to 1206"); deleting `toolchain.tsv` alone gives exit 1 (the `toolchains` cell differs).

**F10 (relative `TIMED_LOG`): pass.** `TIMED_LOG=rel.tsv tools/timed other - rel -- /bin/echo ran` exits 2 with "TIMED_LOG must be an absolute path", the command does not run, and no `rel.tsv` is created. `--log x.tsv` with the same `TIMED_LOG` set still works and wins, as documented. The README's example now uses `"$PWD/..."`.

**Mutants of `tools/runlog` (mine, 14, none from the first review or the author's list).** Each was applied once to a copy and run against the head's `runlog_test.py`. "Killed" means the test exited 1, not that it warned.

| # | Mutant | Result | First failing line |
| --- | --- | --- | --- |
| 1 | `order_check_off`: `order_problems` never called | killed | check exit, out of their order: got 0, expected 1 |
| 2 | `has_commit_true`: every era present in the clone | killed | check stderr, era not in the clone |
| 3 | `golden_check_not_cheap` | killed | rows kept by the rule |
| 4 | `workflow_needs_commit`: dirty workflows not kept | killed | rows kept by the rule |
| 5 | `lint_not_kept` | killed | rows kept by the rule |
| 6 | `missing_eras_ignored` | killed | check exit, history gone: got 0, expected 1 |
| 7 | `range_is_max`: range = largest | killed | summary row 1 |
| 8 | `toolchain_end_inclusive`: `ends >= first` | killed | summary row 3 |
| 9 | `anchor_before_off`: the row before a hole is not an anchor | killed | anchors of a mid-log insertion |
| 10 | `groups_unknown_era_ok` | killed | check exit, has a row of era ffffffffffff |
| 11 | `order_first_only_len`: order loop compares nothing | killed | check exit, out of their order |
| 12 | `sort_ties_by_line_desc` | killed | summary row 1 |
| 13 | `blank_line_allowed`: `split_log` accepts a blank row | **survived** | |
| 14 | `relative_keeps_abs`: an absolute `--log` is not made relative | **survived** | |

12 of 14 killed, which is at least the 8 asked for. The two survivors are real gaps of the self-check (D6). The real tool refuses a blank line in the real log (exit 2, "runs.tsv:1785 is blank") and takes an absolute `--log` (exit 0), so both are tested by hand only.

## 4. Trial merges with `feature/core-mv-a` at `f066f02`

Two throwaway worktrees (`--no-commit --no-ff`, aborted, worktrees removed):

| Direction | Log lines | Conflict markers | `tools/runlog check` | Every row once |
| --- | --- | --- | --- | --- |
| `98fa35b` merges `feature/core-mv-a` | 3,157 (3,156 rows) | 0 | exit 0; 1 era; 1,727 kept rows present once; no removed row | the 1,373 rows of core-mv-a beyond the boundary and the 1,783 rows of the branch are each present once |
| `feature/core-mv-a` merges `98fa35b` | 3,157 | 0 | exit 0; same figures | same; the two logs agree as multisets |

The sum is 1,783 + 1,373 = 3,156 rows plus the header. I also reproduced the hostile case of the first review: a branch from `3bd9904` that appends one row, merged into `98fa35b`, brings back 88,931 removed rows with no conflict. `check` exits 1 and names them; `prune` removes 88,931 rows; `check` then exits 0. So F2 is not prevented, only caught, and only by running `check` or the full suite after the merge.

## 5. Mechanical checks (revision `98fa35b`)

| Check | Result |
| --- | --- |
| `ruff check --isolated --select E,F,W` on `tools/runlog`, `tools/measure`, both tests, `mutate.py` | passed |
| `ruff check --isolated` (default rules) on `tools/runlog`, `runlog_test.py`, `mutate.py` | passed (the first review's ten findings are gone) |
| `ruff check --isolated` on `tools/measure` (not touched) | 3 findings (the known `SIM905` of departure 5 in the first review) |
| `ruff format --check` | 2 files would be reformatted (departure 5; not applied, known) |
| lint workflow (clang-format `--dry-run -Werror`, clang-tidy, tests) | 161 of 161, exit 0, 25.8 s |
| gcc-debug workflow | 159 of 159 |
| clang-debug workflow | 161 of 161 |
| Trace gate (W5 command) | exit 0, 0.19 s |
| Author's mutation driver | 41 of 41 killed (it reproduces) |
| `git diff 69f814f 98fa35b -- docs/agent/governance` | empty. Against `ebdd06d` the only file outside records, plans, tools, and test is decision 0170 (+41 lines, an addendum; it is `proposed`) |
| Process citations in code (STYLE S11) | none in `tools/runlog`, `tools/timed`, `tools/citation_patterns.txt`, `test/CMakeLists.txt`, or the test files; the records READMEs under `docs/agent` cite decisions, which is allowed there |

W9 and W7: the tests precede the fixes in `6368822` before `5bd42ce` and in `5ffde56` before `156db03`, each listed as failing alone. Exception: `0fe7a18` puts the citation pattern and the fixture cases that judge it in one commit (D5). All 15 commits carry the trailer naming the model that did the work.

## 6. Findings, most severe first

None blocks the merge on its own; D2 should be done before it.

**D2. Low to medium. The plan's section 5 still carries the false promise, with no pointer to the correction.** `docs/agent/plans/0017-runlog-distilled.md:130` ("which include every gate run and stay in the log unchanged") and `:134` ("`runs.tsv` keeps a row for every gate run through every compaction") are the proposed P13 and DoD-25 texts that the first review found false. Decision 0170 keeps its original text and adds an addendum (right for a decision record), and the plan's own R2 and D1/D2 corrections say so. But section 5 itself, the part an owner reads to ratify, says "Nothing below takes effect until ratified" and gives the old sentences. An owner who ratifies from the plan ratifies the false sentence. Fix: replace those two blockquotes in section 5 by the addendum's wording, or add one line that they are superseded. Likewise line 46 and 77 of the plan (1,666 / 1,669 expected) are stale against 1,727; O6 and the table at 344 to 347 explain, so this is only a pointer.

**D1. Low. Gate runs the rule still misses** (about 215 rows, section 1). The wording says it. Two of them contradict an expectation a reader may form from the README: `convention-trace`, `reference-markers`, and `golden-facts` at `721c28783890-dirty` are three of the four rows of one trial merge, and only the workflow row of that merge is kept. Resolution: the `gate:` prefix, or accept; no change is required for honesty.

**D3. Low. One phrase of the proposed P13 over-reaches.** The addendum's P13 wording says a compaction keeps "the build, test, style, and gate steps of committed revisions". The rule keeps the `gate` step, `lint`, and `check`/`trace` steps of ten named cheap gates, but not other `check` steps (`jobserver_check`, the `measure_record_*` checks) that an agent may call a gate. "gate steps" can be read as all of them. Say "the `gate` step and the cheap gates named in `tools/runlog`".

**D4. Low, owner question. `tools.runlog_check` fails when agent records are cleared, costs 0.7 GiB and 2.7 to 3.4 s, and runs identically in all seven presets** (section 2). The author's addendum asks the owner; my preference is a label of its own run once per merge (P9) rather than in every preset's full suite. Not a defect of the implementation.

**D5. Low. The citation exception.** `docs/agent/records/performance/history.md` and `.../history/eras.tsv.bak` pass the citation check (an extension of the old `runs.tsv.bak` hole). No fixture asserts either. `0fe7a18` changes the pattern and the fixtures that judge it in one commit; the first review's F3 asked for its own commit, and it is its own commit, but the expected values are not separate from the change (W9), which is weak here only because both sides are one pattern.

**D6. Low. Two survivors of the self-check:** a blank line in a log (`split_log`'s refusal) and an absolute path given to `--log` (`relative`). Add a case each. The tool behaves correctly in both (by hand).

**D7. Nit. Contradiction of "append-only" is explained and fine** (the `eras.tsv` row was corrected twice before any merge; the README says so). The `tools.runlog_check` skip depends on two exact message strings (`not inside a git checkout`, `is not in this clone's history`) in the tool's stderr; changing the wording of either silently turns a skip into a failure. The self-check checks the second string but not that CMake's regex matches it.

**D8. Observation. Resurrection is caught, not prevented** (section 4: 88,931 rows from a branch at `3bd9904`). It is caught by `tools.runlog_check` in a full suite, which runs on every merge at P9's higher levels but not on a merge whose selection is narrower. P8 and P9 wording that would require it is only proposed. Until the owner ratifies it, a small-selection merge into `develop` can bring rows back without a check.

## 7. Acceptance of the first review's findings

| Finding | Status |
| --- | --- |
| F1 | fixed: rule widened, 58 rows put back, wording corrected; residual gaps stated (D1, D2, D3) |
| F2 | fixed as far as a test can: `tools.runlog_check`; open owner question (D4); not prevented (D8) |
| F3 | fixed (D5 for two odd shapes) |
| F4 | fixed; two new survivors of my own (D6) |
| F5 | fixed |
| F6 to F9 | noted by the author (R4 for F6); not re-judged |
| F10 | fixed |
| F11 | documented in the README (step 2 of the compaction) |
