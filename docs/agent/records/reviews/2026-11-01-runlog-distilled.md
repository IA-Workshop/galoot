# W5 review: feature/runlog-distilled (plan 0017) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01, using the performance-engineer and build-expert role files. Agent work.
- Reviewed: `git diff ebdd06d...feature/runlog-distilled`, head `69f814f` (11 commits; the last commit with content other than run rows is `b3a0258`). `tools/` and `test/` are identical at `1a10aeb` and `69f814f`. `develop` is still at `ebdd06d`. `feature/core-mv-a` moved from `1a5af96` to `14bff60` during the review; I did the merge trials against both (section 3).
- Brief: it gave the branch, the claim commit, the owner's ruling, the plan's sections to read, the author's five departures, and a list of checks to make. It carried no verdict on the change as a whole. It did carry the author's facts as statements (row counts, "corrected by a rule change", "a rule selects 1,666 rows (a superset)"). I recomputed each one and did not take any of them as given.
- Blindness: **not fully blind.** The plan's Outcome section is in the same file as the definition of done, the acceptance criteria, and the test list, and I read the whole file in one pass, before I had formed my own view. So I saw the author's verdict per criterion early. To offset this, every number below comes from my own scripts, written from `git show ebdd06d:docs/agent/records/performance/runs.tsv` and not from `tools/runlog`. My mutants are distinct from the author's 26.
- Verdict: **accept after fixes**. Nothing that matters is lost from git. The history is exact: an independent recomputation of all 1,206 groups agrees in every cell. Union merges with `feature/core-mv-a` are safe in every order I tried. `check` fails for the right reasons. One claim is false, though, and the change rests on it. The plan, the history README, the commit message of `544ba01`, and the proposed wording of P13 and DoD-25 (decision 0170) all say that every gate run stays in `runs.tsv`. The compaction removed the whole recorded P9 gate of one develop merge, plus parts of others (F1). Fix the rule or fix the wording before the merge. F2 to F11 do not block.
- Job pool: never started or stopped. It showed 23 of 23 slots free before my builds.

## Not checked or not run (C20)

- Presets run from new, empty trees at `69f814f`: gcc-debug, clang-debug, lint, and gcc-asan. I did not run gcc-release, clang-release, or clang-asan. The author's runs at `b3a0258` cover those on the same build inputs (P10).
- The cheap gates were not rerun at every commit. Only `tests.citations` was rerun, at `d7aa523` and `c16e6cc`, to confirm departure 2.
- The P1, P2, and P6 measurement records and `compile.tsv` were not reviewed (not compacted, unchanged).
- The 233,921 removed rows were checked as aggregates (section 2). I did not decide row by row which removed rows are "gate runs". F1 lists the counterexamples I found by searching the merge messages and the step names. There may be more.
- `toolchain.tsv`: I checked the versions with `--version` and `/var/log/pacman.log`. libc++ 23.1.4 was installed during the era (2026-11-01T10:24Z). It is not a column of the table, and no preset links it. I did not treat this as a defect.
- Outside `tools/timed`: git inspection, my recomputation, merge, and mutation scripts, the timing runs of `tools/runlog`, and `ruff` on default rules. The four workflows, the isolated `ruff` check, the author's mutation driver, and the trace gate ran through `tools/timed`.
- My run rows (selections `review-*`) are uncommitted in `.claude/worktrees/review-runlog`. My scripts (`recompute.py`, `merges.sh`, `mutants.py`) are scratch and are not committed, so their results are labeled not reproducible as committed (V10). Each step is described below so that it can be redone.
- Agent cost: about 70 tool calls. Token counts are not visible to me.

## 1. The four W5 checks

All at `69f814f`, in `.claude/worktrees/review-runlog`, from new, empty build trees, through `tools/timed` (`cmake --workflow --preset <p>`):

| Check | Result |
| --- | --- |
| Style: lint preset (clang-format `--dry-run -Werror`, clang-tidy on every target) | 160 of 160, exit 0, 26.6 s |
| Style: `ruff check --isolated --select E,F,W` on `tools/runlog`, `tools/measure`, both tests, `mutate.py` | passed. With ruff's default rules: 10 findings in new code (F8) |
| Full suite, gcc-debug | 158 of 158, 9.0 s (`tools.runlog` 1.89 s) |
| Full suite, clang-debug | 160 of 160, 6.2 s |
| Full suite, gcc-asan (sanitizers, P9) | 150 of 150, 14.2 s |
| Trace gate (the W5 command) | exit 0, 0.16 s |
| Author's mutation driver (`mutate.py`, 26 mutants) | 26 of 26 killed at `69f814f`, which reproduces the author's record |

Governance is not edited. `git diff --stat ebdd06d 69f814f -- docs/agent/governance docs/agent/decisions .claude .gitattributes tools/citation_patterns.txt` is empty, and decision 0170 is still `proposed`.

## 2. Nothing lost that matters: an independent recomputation

I wrote my own script, `recompute.py`, using `fractions.Fraction`, from the column definitions in the history README and plan section 4.1. I ran it on `git show ebdd06d:docs/agent/records/performance/runs.tsv`:

- Retrieval: the command prints 235,591 lines and 76,111,132 bytes. The log is byte-identical at `2d90f41` and `ebdd06d`.
- Column layouts: 231,658 rows have 28 cells, 1,808 have 26, and 2,125 have 9. From the history of `tools/measure` (`6ad07b2`, `518823d`), the 26-cell rows are the first 26 columns of today's header, so it is correct to read the missing cells as `--` (departure 5).
- `groups.tsv`: I found the same 1,206 group keys and the same 235,590 rows in total. **0 differing cells** across `rows`, `dirty`, `revisions`, first and latest time and revision, `hosts`, and all 18 statistics of all 1,206 groups, compared exactly. Every group's `toolchains` is the one recorded state. A seeded sample of 30 groups was printed for the record. Examples: `test|clang-debug|^core\.blade_set$|shared|pass` has 10 rows, first 7.032, latest 11.718, min 6.801, low median 8.811, range 4.917. `test|lint|all|shared|fail` has 7 rows, 7.718, 2.266, 2.266, 8.777, 7.373. `measure|-|probe-0012|pinned|pass` has 432 rows, 0.184, 0.145, 0.098, 0.122, 0.086.
- Kept rows: the 1,707 rows at `69f814f` are the 1,669 era rows, a subsequence of the old log in order and byte for byte, followed by 38 rows that are not in the old log. My own implementation of the rule keeps 1,666 rows. Lines 235,538, 235,539, and 235,591 are the 3 anchors, and there are 0 mismatches. The old log has no duplicate lines.
- Commits: `544ba01` changes `runs.tsv` by 0 added and 233,923 removed lines. `a3f0a60` adds 2 lines (the two anchors) and changes 1 line of `eras.tsv`.
- Reproduction: I checked out `6a02ad1` in a scratch clone and ran the head's `tools/runlog compact ebdd06d` with `feature/core-mv-a` as a local branch. It gives `groups.tsv` and `eras.tsv` byte-identical to the branch, with the same 3 anchors, in 3.8 s. The `tools/runlog` at `6a02ad1` finds only the boundary anchor. That is the old rule of departure 1, as stated.

## 3. Union merges

All of these ran in a scratch clone with commits, except where a throwaway worktree is named. After each step: `tools/runlog check`, conflict markers, and a count of `core-mv-a`'s 984 rows that are not in the boundary log and of this branch's 38 later rows, each of which must appear exactly once.

| Scenario (core-mv-a at `1a5af96`) | Rows | check | 984 and 38 rows once |
| --- | --- | --- | --- |
| runlog merges core-mv-a | 2,691 | 0 | yes |
| core-mv-a merges runlog | 2,691 | 0 | yes |
| core-mv-a into develop, develop into runlog, runlog into develop | 2,691 | 0 at each step | yes |
| runlog into develop, develop into core-mv-a, core-mv-a into develop | 2,691 | 0 at each step | yes |
| runlog then core-mv-a straight into develop | 2,691 | 0 | yes |
| Throwaway worktree (`--no-commit`, aborted): `69f814f` with develop | (up to date) | 0 | |
| Throwaway worktrees: `69f814f` merges core-mv-a at `14bff60`, and `14bff60` merges `69f814f` | 2,710 | 0 | |

Each merge took 0.06 to 0.52 s. Hostile cases, each merged into develop after runlog and core-mv-a:

| Case | Result |
| --- | --- |
| Branch at the boundary appends a row | check 0 |
| The stale review branch `c4191e4` with its 128 uncommitted rows committed | check 0, all 128 rows once |
| A new row appended on branches at `d89806d`, `b199bf4`, `0607515`, `26cc06e`, `6cc0545`, `807369f`, `39d87a8`, `890e72c` | check 0 for each |
| The same on a branch at **`3bd9904`** (a develop merge commit) | **88,931 removed rows come back** without a conflict; check exit 1 and names each one (F2) |
| Branch from runlog's head deletes a kept row | check exit 1: "a row an era kept is there 0 times" |
| Branch at the boundary deletes two old rows | 580 removed rows come back; check exit 1 |
| Branch at the boundary edits one old row (line 100,000) | 88,930 removed rows come back; check exit 1; `prune` removes 88,930, then check is 0. The edited row stays as a stray row that check does not report |

## 4. Mutants: does check fail for the right reasons?

`mutants.py` ran 23 data mutants on the clone at `69f814f`, with the unmodified tool's `check`, and 21 tool mutants of `tools/runlog`. The tool mutants are distinct from the author's 26. Each was run both by the self-check (`runlog_test.py`) and by `check` on the real history. "Killed" means a nonzero exit, not a warning.

Data mutants. 21 of 23 are killed by `check` (exit 1, or exit 2 by refusal):

- Killed: a group's `rows` + 1; a median changed; `groups.tsv` truncated by 100 rows; two group rows swapped; a group row duplicated; a group row of an unknown era; a group's `hosts` changed; a kept row dropped; a kept row duplicated; one cell of a kept row edited; a removed row appended; an anchor row dropped; `eras.tsv` `kept`, `rows`, `latest_utc`, or `anchors` changed; an anchor dropped from `eras.tsv` with `kept` adjusted to match (the dropped anchor's row is then reported as "removed ... back"); the era moved to `2d90f41`; `toolchain.tsv` `since_utc` changed; `eras.tsv` deleted; the log's last newline dropped (exit 2).
- **Survived:** two adjacent kept rows swapped (F5); `eras.tsv` and `groups.tsv` both deleted (exit 0, "records no era") (F5).

Tool mutants: 14 of 21 are killed by the self-check, and 9 of 21 by the real `check`. Survivors of the self-check:

| Mutant | Edit | Self-check | Real check |
| --- | --- | --- | --- |
| `toolchain_end_inclusive` | `ends > first` to `>=` | survived | survived |
| `mode_default_shared` | missing `mode` to `shared` | survived (equivalent: the padded cell is `--` already, so the branch is dead) | survived |
| `earlier_kept_not_required` | `era_lines` never refuses a missing earlier-kept row | survived | survived |
| `unknown_era_groups_ok` | no report of group rows of an unknown era | survived | survived |
| `citations_not_cheap` | `citations` dropped from the cheap gates | survived | killed |
| `no_second_header_check` | `split_log` accepts a second header | survived | survived |
| `check_ignores_log_option` | `check --log` ignored | survived | survived |

Killed by the self-check: `toolchain_no_unrecorded`, `latest_is_max`, `revisions_keep_dirty`, `family_last_colon`, `inst_p_only`, `duplicates_allowed`, `prune_keeps_copies`, `insertion_after_dropped`, `merged_branches`, `anchors_ignored_in_split`, `first_utc_first_line`, `gate_needs_preset`, `range_abs_first_last`, `era_rows_include_kept_before`.

## 5. Findings

**F1. Major; blocks until fixed or reworded. "Every gate run stays in `runs.tsv`" is false for this compaction.** The rule (`tools/runlog`, `stays`, lines 159 to 171) keeps rows of steps `configure`, `build`, `test`, and `workflow` with a named preset, `gate`, and `check` or `trace` with six family names, all at a revision of 12 hex digits with no `-dirty`. The old log has gate runs outside that rule. All of these are removed from the working tree:

- The whole recorded P9 gate of the develop merge `eb9a8fa` (WU1, plan 0002). Its message says "Checks (P9) ran on the merged tree by the implementer, revision recorded as 0863bad-dirty, standing for this tree under P10". Those are old lines 748 to 754 (seven `workflow ... all-clean` rows at `0863badb9c5f-dirty`) and the two header builds and the `fresh-tree` configure and build at that revision. All 11 rows are removed.
- The cheap gates at `d9c5059`, named in the message of `c2e0163` as the revision of its P9 checks. `convention-trace` is kept, but `reference-markers` and `golden-facts` (old lines 3,796 and 3,797) are removed, because their families are not in `CHEAP_GATES`.
- Style checks run with the step `lint`. Examples: `lint - ruff` and `lint - ruff-measure` at `ed3783e` (old lines 4,932 and 4,933), the delta review's style check of that revision, and `lint lint clang-tidy plan_cost units` at `db9b9ad` (old line 232,789). W5's first check is the mechanical style check.
- Reviewers' trial-merge gate runs on uncommitted merged trees. Examples: `all (merge of develop 547fe97, uncommitted)` at `f9441f77d27c-dirty` (old lines 5,282 to 5,284), and `all-merge` with its cheap gates at `721c28783890-dirty` (old lines 3,775 to 3,778).

So the plan's Goal and D1 ("keeps DoD-25's promise ... literally true"), D2 ("a superset of the gate runs"), the history README ("Every gate run is among them"), and the message of `544ba01` overstate what was kept. So does the proposed wording of decision 0170: P13's "which include every gate run", and DoD-25's "`runs.tsv` keeps a row for every gate run through every compaction of the log". For later compactions the promise also rests on naming habits that nothing enforces, because gate runs are not marked. Nothing is lost from git, and the history counts these rows. What is broken is the promise.

Resolution, either of:

- (a) Widen the rule and correct the compaction, as `a3f0a60` did. Keep `workflow` rows of a named preset at any revision, `lint` rows at committed revisions, and add `golden-facts`, `reference-markers`, `golden-check`, and `conventions_check` to the cheap gates. In the old log this adds 45 + 7 + 8 = 60 rows, about 19 KB. This still keeps no record of dirty `test`/`build` gate steps, so the wording still needs "recorded as" care.
- (b) Reword the plan, the README, the commit's claim in the outcome, and decision 0170 (a new superseding record or an addendum, since a recorded decision is not rewritten): "keeps every build, test, workflow, and gate-step row of a committed revision; gate runs recorded on uncommitted trees or under other step names are distilled into the history and retrievable from git". DoD-25 should then say what is actually kept.

Either way, a lasting promise needs a marker. For example, a `gate` step or a selection prefix that `tools/timed` callers use for gate runs. Otherwise DoD-25 is only as true as the next agent's choice of names.

**F2. Medium; does not block. A resurrection of 88,931 rows is caught only if someone runs `check`.** A branch based at `3bd9904`, a develop merge commit that is an ancestor of the boundary, appends one row. Merged into develop after the compaction, it brings back 88,931 removed rows (about 28 MB) with no conflict. None of the W5 or P9 checks reads the real log: `tools.runlog` uses only fixtures. The README asks for `tools/runlog check` after every merge that touches a log, but nothing enforces it, and the open review worktrees (`review-metric-like*`, `review-plan-limits*`, `review-wu7b*`, `review-mv-a-mut`) hold up to 176 uncommitted rows each on old revisions. The same happens with a branch at or after the boundary that edits or deletes an old row (section 3). The README describes only "a branch based before a boundary whose base was not an anchor". Resolution: make `tools/runlog check` (2.7 to 5.1 s) one of the cheap gates of a develop merge, or a CTest test with its own label that is not run per preset. Extend the README's warning to edits and deletions of old rows on any branch.

**F3. Low; does not block. The citation workaround (departure 2) passes the check by its form.** `HISTORY = f"{RECORDS}/history"` (`tools/runlog:65`, `test/tools/runlog_test.py:19`) does not match `docs/agent(?!/records/performance(?:/runs\.tsv)?\b(?!/))`. The comment above that pattern says that any other path there is a citation. The history path is not a citation. It is the new tool's interface, just as the run log is the recorder's, so the rule's intent is not broken. But the code now passes only because of how the string is built. That is a workaround, and the next reader of `citation_patterns.txt` will not know the exception exists. The fixture selection `spike-a` was also renamed `sketch-a` for the same reason; that one is harmless test data. The author's reason for not widening the pattern ("a change to a check that judges this change (W9)") misreads W9: W9 allows such a change if it is committed separately and listed in the review request. Resolution: add `history` (or `history/[a-z]+\.tsv`) to the pattern's exception with a comment naming `tools/runlog` as its writer, in its own commit listed for review, and write the path plainly.

**F4. Low; does not block. The self-check has untested branches** (section 4, 6 survivors of both checks):

- the refusal when a later era's log lacks a row an earlier era kept;
- group rows of an era that `eras.tsv` does not record;
- a second header line;
- `check --log`;
- the toolchain interval's boundary;
- the cheap-gate families other than `golden` and `trace` (only the real check kills `citations_not_cheap`).

`fields`' `row["mode"] = MISSING` is dead for short rows, because the padding already writes `--`. Resolution: add one fixture case for each, and remove or explain the dead branch.

**F5. Low; does not block. `check` does not check order, and passes with no history.** Acceptance criterion 1 promises the kept rows in their original order. `check` compares counts only, so swapped kept rows pass (section 4). With both history files deleted, `check` prints "records no era" and exits 0, so a merge or edit that drops the history passes. Resolution: in `log_problems`, check that the kept rows appear as a subsequence in era order. Fail when the log has rows but `eras.tsv` is missing, or have the README say what that output means.

**F6. Low; does not block. W9: the tests that show the anchor fix is needed share the fix's commit.** `1a10aeb` adds cases 9 and 10 to `runlog_test.py` together with the new rule. They are not in plan section 6, which was the review request. No existing expected value changed, which I checked. Committing the cases first, failing against `544ba01`'s rule, would have been the stronger evidence. The author did show this separately, with the `base_end_only` mutant. Resolution: note it in the outcome. Nothing to redo.

**F7. Low; does not block. README and code wording.**

- The history README says that every file is append-only. That holds from the merge on (the `a3f0a60` edit was before any merge, and is disclosed).
- `tools/runlog`'s docstring wraps oddly at lines 46 to 48.
- The `Refused` docstring says "nothing has been written". That is untrue for `prune` over several logs, where a later log's refusal follows an earlier log's rewrite. Only one log exists today.
- In `compact`, `anchors = {n for n in ... if n in {number for number, _ in rows}}` rebuilds the inner set for each anchor. That is correct, but it reads as a mistake.

**F8. Low; does not block. ruff.** With default rules, the new code has 10 findings: 6 `RUF059` (unused unpacked variables in `runlog_test.py` at lines 354, 372, and 413), 3 `SIM905`, and 1 `ISC004` at `runlog_test.py:155`. The `ISC004` is intended (a 9-cell row as one literal); wrapping it in parentheses would say so. Departure 5 names `SIM905` on `tools/measure` only. Resolution: fix the six `RUF059`, or record that ruff's default rules are not used.

**F9. Nit. Records.**

- The plan's status line still reads "claimed ... Not started."
- The outcome gives `runs.tsv` at `a3f0a60` as 622 KB; it is 620,714 bytes.
- The plan's 612,647 bytes for the 1,666 rule-kept rows is right (recomputed).

**F10. Plausible, low. `TIMED_LOG` relative to each run's directory.** A relative `TIMED_LOG` is resolved again by every `tools/timed` call, from that call's current directory. The one campaign script today, `harness.py`, runs `tools/timed` with `cwd=ROOT`, so it works. A script that changes directory would scatter its rows, possibly into a build tree. Resolution: in the README's example, use an absolute path (`"$PWD/docs/..."`), or have `tools/timed` say that the path is resolved per run.

**F11. Observation. `compact` refuses while a pre-era branch is open.** At a second compaction, a branch based before the first era has a base log whose rows are not in the new boundary's log. `line_number` then refuses the whole compaction, by design and loudly. The history README's compaction steps could say this.

## 6. Acceptance criteria (plan section 3)

| # | Verdict | Evidence |
| --- | --- | --- |
| 1 | met | Section 2: 1,669 era rows as an ordered, byte-identical subsequence, plus 38 later rows |
| 2 | met | Section 2: all 1,206 groups recomputed independently, 0 differences; `check` exit 0 at `69f814f` |
| 3 | met | 235,591 lines |
| 4 | met for the listed damage | section 4; not for order (F5) |
| 5 | met | fixture case 5; author's mutants `no_containment_check`, `no_repeat_check`, `no_header_check` killed at `69f814f` |
| 6 | met | case 6; hostile cases in section 3 |
| 7 | met | fixture cases 7, 9, 10; real merges in section 3 |
| 8 | met | `tools.measure_record` passes on four presets; my own `tools/timed` rows went to `runs.tsv` without `--log` |
| 9 | met | 26 of 26 killed at `69f814f`; my further mutants show gaps (F4) |
| 10 | partly met | S11: no provision, plan, or agent citations in `tools/runlog`, the tests, or the recorder diff. ruff: E,F,W pass, defaults do not (F8); `ruff format` not applied (departure 5) |
| 11 | met with P10 | four presets at `69f814f` by me, seven at `b3a0258` by the author on the same inputs; trace gate exit 0; `tests.citations` fails at `d7aa523` and passes at `c16e6cc` (departure 2 confirmed) |

## 7. On the author's departures

1. The first compaction and its correction: confirmed and reproduced (section 2). The correction is sound.
2. Citations: confirmed. I prefer an explicit exception (F3).
3. Gate rows left in place, selected by a rule: keeping them in place is right. The claim that the rule gives a superset is false (F1).
4. "Regenerate from retained rows" read as rows retained in git: a reasonable reading, and `check` implements it exactly.
5. Accepted. Campaign rows distilled rather than moved; one hand-written toolchain state, which I verified; `compile.tsv` not compacted; 28 columns (verified); ruff (F8); the `-dirty` rerun at `b3a0258` (its rows are in `69f814f`).

## 8. Cost

- `tools/runlog check`: 2.7 to 5.1 s, mostly `git show` of the 76 MB era.
- `compact`: 3.5 to 3.8 s.
- `summary` of the compacted log: 0.05 s.
- CTest `tools.runlog`: 1.75 to 1.89 s in each preset. That is one of the longer tests, but it runs in parallel inside suites of 2.4 to 5.8 s of test time, so it is reasonable. Like the other `tools.*` tests it does not depend on the preset, and it runs in all seven presets.
- Trial merges: 0.06 to 0.52 s each.
