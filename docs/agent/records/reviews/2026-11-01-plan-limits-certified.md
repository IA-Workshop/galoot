# W5 review: feature/plan-limits-certified (plan 0015) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01, using the performance-engineer and numerical-expert role files for the cost model. Agent work.
- Reviewed: `git diff 4895a40...feature/plan-limits-certified`, head `26cc06e` (9 commits). The build inputs are final at `57f9d1b`: `git diff --stat 57f9d1b 26cc06e -- . ':!docs'` is empty, so `1f7cf28` and `26cc06e` build exactly what `57f9d1b` builds. This confirms the author's departure 3. `develop` has moved since the claim (now `37a562d`); those commits were not reviewed.
- Brief: it gave the branch, the claim commit, the acceptance criteria (by reference to plan 0015 sections 2, 3, 5, and 6), the owner's ruling, the author's seven departures, and a list of checks to make. It carried no verdict. It did pass on one author claim (that a script checks the comments' figures against the figures files) and named lessons from earlier reviews. I checked that claim myself and did not take it as given.
- Blindness: mostly blind. I read the plan's Outcome only after my own builds, figure re-checks, and mutants had run. Before that, though, I read decision 0131 (required reading) and the author's mutation record `survivors.md` and its result tables. Those carry the author's view of which phases the canary reaches. My mutants were chosen to probe the paths that record does not name. Research note 0017 was read only through decision 0125's summary, lesson 0064, and the snippets' docstrings, not in full.
- Verdict: **accept after fixes**. The figures in the two comments are right. I recompiled 41 stated cases by checked search on the two compilers: 39 reproduce exactly, and the other two are one case, the sparse sets at n = 16 on both compilers, which is explained (F6). The comment diff is comments only. The main problem is the canary. Three things should be fixed before the merge: its GCC acceptance rule can pass a unit whose count has risen (F1, demonstrated); it is registered in four presets where it fails by design (F2, demonstrated); and two generator paths that decide stated figures are pinned by no reference unit, so the CMake comment's promise that a generator change fails the test is not kept (F3, demonstrated). F4 to F11 do not block.
- Job pool: never started or stopped. It showed 23 of 23 slots free at my first build and at the end. A few of my runs held up to 20 slots for under a minute, as load for the canary's timing (section 4).

## Not checked or not run (C20)

- Presets I ran from new, empty trees at `26cc06e`: gcc-debug, clang-debug, lint, gcc-asan, and clang-measure (canary only). I did not run gcc-release, clang-release, or clang-asan as suites. Instead, I configured release-flag variants of gcc-release and clang-release, with the presets' flags and without `-D_GLIBCXX_ASSERTIONS`, as mutants (section 3). The author's full runs at `1f7cf28` cover those presets on the same build inputs (P10).
- The author's 23 canary mutants and 14 self-check mutants were not rerun as such. I ran 12 header mutants, 6 threshold mutants, and 7 flag or library mutants of my own on the canary, and 6 self-check mutants of my own. Two of my mutants (`gen_write_step` and `type_terms_term_step`) are close to the author's `product_term_step` and `typed_scan_step`.
- The refit itself (the probe searches, `verify`, `fit`) was not repeated. I checked its output three ways. I recompiled 41 figure cases by checked search, reran `cost.py selfcheck` on the committed data, and spot-checked the case and threshold files.
- Research note 0017's GCC-source argument that the cold count bounds the warm count from above was not re-derived. I only sampled it: on 20 GCC cases, with the cache on, every unit compiled one below its cold threshold and at the default limit wherever the cold count was under it.
- The knowledge-base branch `kb/constant-evaluation-canary` was not reviewed.
- Outside `tools/timed` (P13): git inspection, `ruff`, reads of logs, one `python3` run of the canary script against the stand-in compiler (a fraction of a second), one `cost.py predict` loop and one mirror event count (they compile nothing), and `ctest -N`. Every build, test, compile, self-check, and gate went through `tools/timed`.
- My run-log rows (selections `review-*`) are not committed. They are in the uncommitted `runs.tsv` of my worktrees `.claude/worktrees/review-plan-limits`, `review-plan-limits-mut`, and `review-plan-limits-trialmerge`. My scripts (`recheck.py`, the mutation drivers, a CPU-load script) are scratch and not committed, so their results are labeled not reproducible as committed (V10). Each step is described below so that it can be redone.
- Agent cost: about 85 tool calls. Token counts are not visible to me.

## 1. The four W5 checks, and where they ran

All at `26cc06e` (build inputs of `57f9d1b`), in my worktree `.claude/worktrees/review-plan-limits`, from new, empty build trees, through `tools/timed`, with GCC 16.2.1 and Clang 23.1.4:

| Check | Result |
| --- | --- |
| Style (lint preset: `all`, `all_verify_interface_header_sets`, `format-check`; clang-tidy with warnings as errors) | build exit 0, lint suite 150 of 150. The six `plan_cost_*.cpp` units are compiled by no target, so lint does not see them (F8). Run by hand at `26cc06e`, `clang-tidy --warnings-as-errors='*'` on them found 0 problems and `clang-format --dry-run -Werror` passed |
| Full suite, gcc-debug | 148 of 148 (canary 4.86 s) |
| Full suite, clang-debug | 150 of 150 (canary 4.74 s) |
| Full suite, gcc-asan | 147 of 147 (no canary, as decision 0131 item 5 says) |
| Trace gate (`python3 -I tools/test_references.py trace ...`, from the root) | exit 0 |
| This review | below |

Trial merge: in a throwaway worktree at `26cc06e` (`.claude/worktrees/review-plan-limits-trialmerge`), `git merge --no-commit --no-ff develop` (`37a562d`) merged with no conflict. `runs.tsv` merged by its union attribute. `develop` changes only `docs/` since the claim (decisions 0140 and 0141, plan 0016, spike records), and its numbers do not collide with 0131 or lesson 0064. The merge was aborted and not committed. Plan 0016 (WU8) will add `test/CMakeLists.txt` blocks and `mv.hpp` to `core.hpp`. The reference units do not include `core.hpp`, so WU8 cannot change their counts unless it edits `plan.hpp`, `blade_set.hpp`, `metric.hpp`, or `bits.hpp`.

## 2. The comments

- Comments only (g): checked mechanically. `git diff -U0 4895a40 26cc06e -- include/` changes only lines that begin `///`, and the non-`///` lines of `plan.hpp` are identical at both revisions (`diff` of the file with `///` lines removed is empty). No other file under `include/` changed.
- Citations (h): no added line in `include/` or `test/` names a decision, plan, note, review, provision, agent, or record. `tests.citations` passes. The snippets cite note 0017 and plan 0015, but they live under `docs/agent/`, where STYLE S11 places such notes.
- Figures against the figures files: every count in both comments is the `threshold` (largest phase) of its row in `out/figures-clang.txt` or `out/figures-gcc.txt`. I checked this by hand, row against claim, all 41 counts, and also every "(over ... limit)" annotation against 1,048,576 and 33,554,432. The script the outcome mentions for this check (criterion 4, "checked by script at 57f9d1b") is not committed (F10).
- Figures against fresh compiles (a): I wrote my own units for the stated cases (not `probe.py`'s), with different names and layout. Each was compiled with the debug presets' flags at the stated count T, at T − 1, and at T+1, +2, +3, +7, +31, +127, +T/1000, +T/100, and +T/10. That is the checked rule, because GCC's pass/fail is not monotone. On GCC it was cold (`-fconstexpr-cache-depth=1`), and with the cache on at T − 1 and at the default limit.
  - Clang, 19 cases: `full<5>` and `full<6>` gp in R(n,0,0), dense `full<4>` gp, wedge `full<5>` in R(5,0,0) and conformal(3), wedge `full<6>` dense, antigp `full<5>` in R(5,0,0) and conformal(3), antigp `full<6>`, antiwedge `full<5>` and `full<6>` (rigid), the sparse sets at n = 4 (gp and antigp) and n = 16 (gp), the reverse of `full<8>`, the grade involution and antireverse of `full<7>`, the left complement of `full<9>`, and the right complement of `full<8>`. Eighteen pass at T, fail at T − 1 with the step-limit message, and pass at every point above. The one exception is sparse gp at n = 16 (F6).
  - GCC, 22 cases: the same kinds, plus `full<7>` gp (203,069,770 operations, 2.7 GB peak), dense `full<5>` gp, wedge and antiwedge `full<7>`, the reverse of `full<9>`, and the right complement of `full<10>`. Twenty-one reproduce exactly by the checked rule; the exception is again sparse gp at n = 16. With the cache on, every unit compiled at T − 1, and every unit whose cold count is under 33,554,432 compiled at the default limit. Wedge `full<7>` (39,732,810, stated as over the limit) failed at the default limit, with `__builtin_expect(...) is not a constant expression`, not with the operation-count message.
- Honesty about limits (d): the comment says the counts are exact for Clang 23.1 classic and GCC 16.2 cold with libstdc++ 16 and `_GLIBCXX_ASSERTIONS`. It says that GCC's warm count is never higher, so a count over the limit does not mean failure, and that other versions, evaluators, and libraries count differently. That covers the one-sided GCC certificate and the calibrated configuration. Three gaps are in F7: the effect of the assertions setting is stated for Clang only, GCC's sanitizers are not mentioned, and the comment does not say that a unit has evaluations besides the plan's.

## 3. Mutants of the canary (b)

Driver: my own, in a separate worktree (`.claude/worktrees/review-plan-limits-mut`, detached at `26cc06e`). Each header mutant inserts `[[maybe_unused]] const bool reviewer_step = true;`. For each mutant the driver builds the preset (a `-Werror` diagnostic would show as a build failure, recorded as "warning", not a kill), runs only `core.plan_cost_model`, and restores the file. Every header mutant built without a warning on both presets.

| Mutant | clang-debug | gcc-debug | Note |
| --- | --- | --- | --- |
| `gen_pair_step` (per pair in `generate_plan`) | killed | killed | dense4, antigp16 (and full5 on GCC) |
| `gen_write_step` (inside `if (count < out.size())`) | killed | killed | |
| `collect_repeat_step` (the repeated-blade branch of `collect_result`) | killed (full5, +992) | killed (reverse7, +128) | |
| `typed_copy_step` (result copy in `typed`) | killed (grades) | killed (grades) | |
| `type_terms_term_step` (per term in `type_terms`) | killed | killed | grades, rc8 |
| `factor_anti_step`, `run_term_anti_step` | killed | killed | antigp16 |
| `wedge_branch_step` (the wedge branch of `pair_terms`) | **survived** | **survived** | not equivalent: F3 |
| `collect_shift_step` (the shift loop of `collect_result`) | **survived** | **survived** | not equivalent: F3 |
| `capacity_pair_step`, `defects_term_step` | survived | survived | equivalent for the stated figures: neither phase is the largest of any figure row on either compiler (checked against both figures files) |
| `typed_scan_flip` (`a != b` written `b != a` in `type_terms`) | survived | survived | equivalent: the same count |
| thresholds: dense4 Clang +10 %, full5 Clang −10 %, antigp16 Clang +1; reverse7 GCC −10 %, grades GCC +10 %, rc8 GCC −1 | 3 of 3 killed | 3 of 3 killed | each reports the true threshold |
| no `-D_GLIBCXX_ASSERTIONS`, release flags (`-O2`) | killed, all 6 moved (full5: 770,687) | killed, all 6 moved (full5: 3,704,610) | |
| no `-D_GLIBCXX_ASSERTIONS`, debug flags (`-O0`) | survived | survived | equivalent: libstdc++ 16 defines `_GLIBCXX_ASSERTIONS` by itself when `__OPTIMIZE__` is not defined (`bits/c++config.h`, lines 603 to 608) |
| `-D_GLIBCXX_NO_ASSERTIONS` at `-O0` | killed, all 6 moved | not run | |
| `-D_GLIBCXX_DEBUG` added | survived | survived | equivalent for these units (inferred: debug mode does not change the cost of the subscripts they evaluate) |
| `-fexperimental-new-constant-interpreter` | killed, all 6 (full5: 133,186) | n/a | |

Every kill came from a moved count ("does not compile at its threshold" or "compiles one below"), never from a warning. The two non-equivalent survivors change stated figures, as F3 shows.

Self-check mutants (f), Clang side, run in the throwaway worktree on the committed data. The unmutated `cost.py selfcheck` passes on both compilers (1,228 and 1,235 cases, 9 metric-check units skipped each, 0 differences; 82 s and 87 s).

| Mutant | Verdict | First difference |
| --- | --- | --- |
| `fill`'s `add.call` +1 and `add.cl_equal` −1 (the pair the README says moved in the fit's null space) | killed (329 differences) | refit gives `fill add.call 64` |
| `full-gp-r-n6`'s measured threshold +1 in `plans-clang-*.tsv` | killed | predicted 5110503, measured 5110504 |
| `mirror.py` counts `collect.shift` twice | killed | a training row of `collect` differs |
| a figures row's `covered` set to `NO` | killed | that figures line |
| `full-gp-r-n6`'s row deleted from `plans-clang-*.tsv` | **survived** (1,227 cases checked, 0 differences) | F9 |
| `full-gp-r-n6`'s threshold set to `fails` | **survived** (1,227 cases checked) | F9 |

The self-check is strong against changed data, weights, mirror events, and figures, but not against measurements that go missing. Both survivors are flagged in F9.

## 4. The canary's cost (c)

Twenty-four runs of `ctest -R '^core.plan_cost_model$'`, each through `tools/timed`, plus the canary inside three full suites:

| Condition | clang-debug | gcc-debug |
| --- | --- | --- |
| Full suite, idle machine, parallel CTest | 4.74 s | 4.86 s |
| Lint suite | 4.67 s | |
| Light load (one GCC search of 8 compiles) | 5.41, 5.42, 4.41 s | 4.59, 5.08, 4.13 s |
| Idle (a load script that failed to start) | 4.20, 4.22, 4.23 s | 4.03, 4.01, 3.98 s |
| Moderate load (3 of 23 slots free) | 4.53, 4.31, 4.31 s | 4.16, 3.99, 3.98 s |
| 20 of 23 slots spinning | 6.83, 5.21, 6.53 s | 4.74, 6.67, 6.11 s |

- Peak memory: 124 MB (Clang) and 200 MB (GCC). User CPU equals wall time within 5 %, because the twelve compiles run one after another.
- Determinism: all 27 runs passed. The counts do not depend on load, so load moves only the wall time. I saw no flake. A failing canary is slow: 66 s to 81 s, because it searches each unit's threshold.
- Short memory was not tested. With these peaks, it matters only if the machine is close to running out.
- The 5 s criterion: the run exceeded 5 s in 8 of these 27 runs (F5).

## 5. Findings, most severe first

### F1 (medium, blocks): the GCC canary can pass a unit whose count has risen

`test/tools/plan_cost_model.py`, `check()`, lines 89 to 94. The canary passes when a unit compiles at T and fails at T − 1 with the limit message. On GCC, a unit can compile under a limit below its count, when the count runs out inside a built-in function's argument. So a unit whose count rose can still compile at exactly T, and the canary passes it. Owner ruling item 4 asks that every GCC limit measurement be confirmed above its result. The canary does this only on its failure path (`search`), not on the path that decides a pass.

Demonstrated twice:

- With the stand-in: `plan_cost_model.py gcc unit.cpp=1000 -- fake_compiler.py 1005 evades=1000` prints "compiles at 1000 and fails at 999, as recorded" and exits 0, although the threshold is 1005.
- With a real generator change: under my `wedge_branch_step` mutant (one statement in `pair_terms`' wedge branch), antiwedge `full<5>` in R(5,0,0) on GCC compiles at its stated 1,393,084 and fails at 1,393,083 with the limit message. It fails at +1 (as not a constant expression), at +2 to +31 (limit), and at +T/1000, and passes from +T/100. The canary script, run on that unit, reports it as passing.

For the six present references none of the recorded mutants hit this, but any future change can.

The docstring's "So it passes only if the compiler's count of the unit's largest evaluation is exactly the one recorded" is false on GCC.

Resolution: on GCC, accept a pass only if the unit also compiles at points above T. The harness's set (+1, +2, +3, +7, +31, +127, +T/10000, +T/1000, +T/100, +T/10) or a stated subset would do. Add a test `tools.plan_cost_model_evades_at_threshold` (`fake_compiler.py 1005 evades=1000`, expected to fail), and correct the docstring. This adds compiles, which is a further reason for F5's split.

### F2 (medium, blocks): the canary is registered, and fails, in the four measurement presets

`test/CMakeLists.txt`, lines 626 to 655. Registration tests only the compiler ID and `-fsanitize`. The measurement presets (`clang-measure`, `clang-measure-o3`, `gcc-measure`, `gcc-measure-o3`) build the test suite at `-O2` or `-O3` without `_GLIBCXX_ASSERTIONS`. There the counts differ by design, which is the comment's own caveat.

Demonstrated: `ctest --preset clang-measure -R '^core.plan_cost_model$'` fails on all six units (66 s; gp full5 770,687 against 655,239). My gcc-release-without-assertions variant, which has the flags of `gcc-measure`, fails on all six as well (81 s).

The outcome's criterion 5 ("passes in all six presets that register it") is therefore inaccurate: ten presets register it. Decision 0131 item 5 says "registered on Clang in every preset" without noting this.

Resolution: register only where the calibrated library configuration holds. For example, register when `CMAKE_CXX_FLAGS` defines `_GLIBCXX_ASSERTIONS` (and, on GCC, has no `-fsanitize`), and say so in the CMake comment in plain words. Then correct criterion 5 and decision 0131 item 5 by a dated note.

### F3 (medium, should fix before the merge): two paths that decide stated figures are pinned by no reference

The comments state figures for paths that no reference exercises, so the canary can miss a generator change that moves them. Two such paths are demonstrated:

- The wedge branch of `pair_terms` (`plan.hpp`, lines 196 to 199). It is on the largest evaluation (`fill`) of every antiwedge figure on both compilers. No reference uses the wedge or the antiwedge. Under `wedge_branch_step` the canary passes on both presets. Meanwhile, antiwedge `full<5>` and `full<6>` no longer compile at their stated 271,653 and 1,108,607 Clang steps (they pass only from about +T/1000), and on GCC `full<6>` and `full<7>` move above 5,695,232 and 23,415,628.
- The shift loop of `collect_result` (`plan.hpp`, lines 317 to 319). The two `collect` references (gp `full<5>` and the reverse of `full<7>`) insert every result blade at the end, so no reference shifts. The mirror counts 0 shifts for them and 2,016 for antigp `full<6>`, whose figures are `collect`-largest on both compilers. Under `collect_shift_step` the canary passes. Antigp `full<6>` then fails at its stated 5,076,962 (Clang) and 25,775,777 (GCC).

The CMake comment says that "a change to the generators ... can change every such count, and then this test fails". As written, this overstates what the canary guards. The plan's reading of "a mutant that changes a weight" (departure 4) narrowed the mutant class to the references' own largest paths, and so did not test these.

Resolution: give each path that is the largest evaluation of some stated figure a reference unit, on each compiler. With the model, that means a case whose `fill` takes the wedge branch (an antiwedge set) and a `collect`-largest case with shifts (an antiproduct plan, or sets whose first factor is not the scalar). `cost.py predict` and the mirror's event counts can pick small ones. The alternative is to narrow the CMake comment and the comments' claim to what is pinned. Either way, add both survivors to the mutation record.

### F4 (low): the canary's GCC failure message can misname the cause

`plan_cost_model.py`, lines 108 to 113. When GCC's count runs out inside a built-in function's argument at T − 1, the canary prints "fails one below its threshold for another reason than the limit". I observed GCC's not-a-constant-expression diagnostic at points around a moved count four times (antiwedge `full<5>` at +1, `full<6>` at +T/1000, and `full<7>` at T − 1 under the wedge mutant; the sparse n = 16 unit unmutated at +1 to +7). If it happens at T − 1 of a reference, the message points away from the real cause. The function already treats this case at T, by compiling under the largest limit. Resolution: treat T − 1 the same way. If the unit compiles under the largest limit, report "count changed" and search.

### F5 (low): the 5 s criterion is not met on an ordinary shared machine; split the canary

Criterion 7. In 8 of my 27 runs the canary took over 5 s (up to 6.83 s), and every run took 3.98 s or more. The outcome's 5.00 s maximum came from two runs on an idler machine. By the criterion as written, it should carry `slow`.

A better resolution than the label is the one the outcome itself lists as open item 3: one CTest per reference unit (`core.plan_cost_model.gp_full5`, and so on). Each would take about 0.7 s, they would run in parallel from the pool, each failure would name its unit, and F1's extra GCC compiles would not lengthen the critical path. This is also the plainer structure. Whichever form is chosen, record the timings above in decision 0131.

### F6 (low): the sparse-set figure at n = 16 for the geometric product cannot be reproduced by a compile

`plan.hpp`, lines 639 to 644: "{1, e1, e_n, e1 e_n} with {e1, e2, e_n} in R(n-1,0,1) cost 3,136 steps and 18,252 operations at every n from 4 to 16 through the geometric product". At n = 16 a unit holding that plan does not compile at 3,136 or 18,252. Its largest evaluation is the metric's check of R(15,0,1): 4,404 and 29,291, confirmed by my checked compiles, and the same as the author's `wide` rows. The figure is the model's count of the plan's own evaluations, which no whole-unit compile at n ≥ 12 can confirm. The outcome says so (open item 4), but the comment does not.

Resolution: say in the comment that these are the plan's own evaluations and that, for sets this small at n = 16, the algebra's own check (4,404 steps and 29,291 operations) is the larger evaluation. Or state n = 4 and 8, where the plan's figure is the unit's threshold.

### F7 (low): the comment understates which settings change the counts

`plan.hpp`, lines 614 to 623.

- "Without _GLIBCXX_ASSERTIONS ... costs Clang more steps": GCC counts more too (all six references rose, full5 from 3,611,426 to 3,704,610 at `-O2`).
- libstdc++ 16 defines `_GLIBCXX_ASSERTIONS` by itself in every unoptimized build unless `_GLIBCXX_NO_ASSERTIONS` is given (shown by my `-O0` mutant). So the figures also hold for a plain `-O0` build, and the setting matters only for optimized builds. A user reading "with _GLIBCXX_ASSERTIONS defined" would not know that.
- GCC with sanitizers counts 2.2 % to 4.4 % more (decision 0131), and the comment does not say so.

Resolution: one or two plain sentences covering all three. That is a comment change, so it needs the short delta review of a header comment.

### F8 (low): no preset compiles or lints the reference units

The six `test/core/plan_cost_*.cpp` files are compiled only by the canary's script. So no `-Werror` build, no clang-tidy, and no compile-cost launcher sees them. This is how `22db6bc` came to fail lint unnoticed (departure 2). Every one of them compiles under the default limits. Resolution: an OBJECT library (or a `galoot_add_compile_test`-style target) holding them. Then lint and every preset compile them at the default limits as ordinary units, and the canary keeps its lowered-limit compiles.

### F9 (low): records state the self-check's reach and the measurement status loosely

- Decision 0131, item 4, last sentence: "the weights' self-check (item 7) still sees it at the next refit". The self-check compiles nothing and cannot see a generator change. A refit's new measurements can (`survivors.md` words this correctly). Resolution: a dated correction.
- `cost.py selfcheck` silently skips a measured case whose row was deleted or set to `fails` (see section 3), and its counts of checked cases and of metric-check exclusions are printed but not compared with expected values. Resolution: compare the number of checked cases (1,228 and 1,235) and of excluded units (9) with expected values, or fail on a case in the case file with no threshold row.

### F10 (low): the comment-to-figures check is not committed

Criterion 4's "checked by script at 57f9d1b" refers to a script that is not in the repository, so that check is not reproducible as committed (V10). My hand check (section 2) agrees with its result. Resolution: commit the check (for example a `cost.py` subcommand that reads `plan.hpp`'s two comments and compares each number with its figures row), or reword the outcome.

### F11 (note): small code nits

- `cost.py` and `probe.py` start with `#!/usr/bin/env -S python3 -I` but are mode 100644 (`ruff` EXE001). The README runs them as `python3 -I ...`. Drop the shebang or set the mode.
- `ruff`'s other reports (`B023`, `ISC004`, `PLW1510`, `FURB161`) are not defects: the lambdas run at once, and the concatenation is a deliberate header string.
- The scripts and the CMake wiring are otherwise plain and readable. The canary script is short, its search is easy to follow, and the CMake comment explains the thresholds in plain words. The stand-in compiler's tests cover both sides of each result except the one in F1.

## 6. Records (i)

- Lesson 0064: accurate, and states the safe direction correctly.
- Decision 0131: items 3 and 5 are accurate for what they cover. Item 4's last sentence is in F9, item 5's preset list in F2, and item 6 in F5. The dated correction of item 6's times is honest.
- W10 rows: `compile.tsv` gains 360 rows (before at `4895a40`, after at `1f7cf28`). The outcome's reading (no measurable change from the comment; the header-set compiles explain the 8 extra compiles) is consistent with the numbers it gives. Not recomputed.
- Outcome: departures 1 to 10 are stated plainly. Inaccuracies: criterion 5 (F2), criterion 6 (true for the author's mutants; incomplete as a claim about the canary, F3), and criterion 7 (F5). The "three limits" exception in criterion 4 is fine.
- Commits: each says what and why and carries the agent trailer. The tests and expected values (`22db6bc`, with its message listing them) came before the comment (`57f9d1b`), as W9 asks.
- Run log: `runs.tsv` at `26cc06e` is 76 MB and 235,048 rows. The branch adds 124,498 rows and about 40 MB; 210,812 of all rows are `ceiling-cert:*` probe compiles from note 0017 and this unit. The union merge attribute keeps merges clean, but the file is past GitHub's 50 MB warning size. One more refit of this size would put it near the 100 MB limit, and every reader of the log now parses mostly probe rows.

  Recommendation, not an edit: give measurement campaigns their own log. For example, `tools/timed` could honor a run-log path, so that `harness.py` writes `records/performance/plan-cost-model/runs.tsv` beside the data it produced (compressed once the campaign closes). `runs.tsv` would then keep the build and test runs that P13 is about. Moving the existing probe rows is a records change for the owner to approve.

## 7. Criteria (plan 0015 section 3), as I find them

| # | Status | Evidence |
| --- | --- | --- |
| 1 Mirror fidelity | met (records; not rerun) | `verify-*` files, 0 differ |
| 2 Exact fit | met | self-check refits to the committed weights, 0 differences, both compilers |
| 3 Prediction | met as recorded | self-check, 0 differences; the 9 metric-check units excepted |
| 4 Comments | met for the figures; the check is not committed (F10); the configuration caveats are incomplete (F6, F7) | section 2 |
| 5 Canary positive | met in the six debug, release, asan, and lint presets I ran or that the author ran; fails in the four measure presets that also register it (F2) | section 1, F2 |
| 6 Canary negative | met for the mutant class the plan defined; two further generator mutants that change stated figures survive (F3); GCC acceptance unsound (F1) | section 3 |
| 7 Canary cost | not met as measured here (over 5 s in 8 of 27 runs) | section 4, F5 |
| 8 Snippet self-check | met; it does not notice deleted or `fails` measurement rows (F9) | section 3 |
| 9 Checks | met with the author's departures 2 and 3; lint does not reach the reference units (F8) | section 1 |
| 10 W10 | met (not recomputed) | outcome table |

## 8. Revisions

- Reviewed head: `26cc06e`; build inputs `57f9d1b`; claim `4895a40`.
- My checks: all at `26cc06e`. Mutants were applied to a detached worktree at `26cc06e` and restored after each one.
- Trial merge: `26cc06e` with `develop` at `37a562d`, clean, aborted.

## Delta review of 0aeb46b (comment-only header change after the review)

Reviewed by the coordinating agent (Claude Sonnet 5.5), 2026-11-01. The diff of `include/` from `26cc06e` to the head contains only `///` comment lines. The reworded comment of `product_plan` now states that libstdc++ 16 defines `_GLIBCXX_ASSERTIONS` itself in an unoptimized build, that GCC's sanitizers add about 2% to 4%, and that the counts are those of the plan's own evaluations, and the comment of `unary_map` says the check of an algebra's metric costs more than the product's plan at n = 16. These are the points of findings F6 and F7. No finding.

## Open item carried to the merge

The slowest GCC canary test, `gp_full5`, took 5.46 s on gcc-debug and 5.43 s on gcc-release in the final runs, over the 5 s threshold of the plan's `slow` rule, because it now compiles eleven times for the check above the threshold. The coordinator accepts this: the canary is split into one test per unit and runs in parallel, no single test dominates the suite, and the slow label stays off. Decision 0132 item 4 says no single test exceeded 5 s; that statement was true of the earlier runs only.
