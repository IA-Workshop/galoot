# Review record: feature/snippets-cleanup (plan 0037, decision 0229 items 1 and 3)

- Reviewer: blind-reviewer, Sonnet 5.5 (claude-sonnet-5-5), 2026-11-10.
- Range: `f5a2698..753a334`, three commits: 6a3cf13 (path and name repairs), 142f700 (README), 753a334 (index, reruns, deletion). Worktree `.claude/worktrees/review-snippets-cleanup`.
- Brief carried a verdict: no. It carried the plan's criteria and the author's departures (C20), which I treated as claims to check.
- Committed by the implementer of plan 0037 (Claude, agent, Opus 5.5) as returned, with this line added and the worktree path shortened; the reviewer's run rows are committed with it.

## Outcome: approve with non-blocking findings

All six criteria are met. Every reproducible claim I re-ran reproduced. The findings are small provenance and wording points; none blocks the merge.

## What I checked, and how

**Read in full:** the diff, with every changed file read (the 751 `runs.tsv` lines classified by selection). Also AGENTS.md, decision 0229, plan 0037, the whole of M5.md, and the index before and after. I did not read the full governance set (CONSTITUTION, WORKFLOW, VVUQ, PERFORMANCE); the change touches only docs, snippets, and spike scripts, so I checked the points that apply (P13, C12, C20, W7, V2) against the diff directly.

**Criterion 1, README: met.**
- One sentence is added after the first change-of-basis example, in a paragraph of its own. It says the tag goes in a named namespace, and gives the reason: an anonymous one gives each file its own tag and so its own algebra type. That is true: an anonymous-namespace type is distinct in each translation unit.
- Its first clause ("the examples declare their tags in an anonymous namespace because each is a program of one file") holds: the README's `namespace {` blocks are at lines 225 and 296.
- No process citation appears, and the diff changes no code block.
- I built and ran `rga3_readme_example_1..5` and `tests.citations` on gcc-debug and clang-debug in my own `build/`: 7 of 7 passed on each.

**Criterion 2, index: met.** Row by row against the M5.md table:
- The mutation driver, cost harness, and catalogue rows are gone.
- The bits row is gone.
- The table diff is "Superseded by `test/reference/golden.py`".
- The constexpr generator is "Historical, kept for note 0008".
- The Spike B row says V2 for `validate` and DoD-14 met by `faults.toml`.
- The evaluation counter is "Kept for WU17 (decision 0215 item 10)".
- The jobserver and cost-model rows record the reruns.
- The last sentence about the M0 review is dropped.
- The peak-memory row is kept.

**Criterion 3, repairs: met.**
- By path arithmetic: `parents[5]` from `spike-a/` and `spike-b/` is the repository root; `HERE.parents[4]` in run_grid.py is the root and `tools/timed` exists there. The old values gave `docs/` and `docs/tools/timed`.
- Docstrings in the three scripts name `docs/agent/records/spikes/`.
- probe.py's `ENUMERATOR` maps `gp` and `antigp` to `geometric_product` and `geometric_antiproduct`. That is the only place C++ operation names are built (`op_expr`, line 129).

**Criterion 6, failing before and working after: met.** I reproduced each case from `f5a2698` copies placed in scratchpad trees that mimic the layout:
- `fault_injection.py` and `diff_reference.py` before: `ModuleNotFoundError: clifford_reference`.
- `fault_injection.py` after, in the worktree: "all faults caught", exit 0.
- `diff_reference.py` after: it needs `out/runtime_check_<compiler>`, so I built those in a scratch copy of the layout, since writing into the worktree's `out/` would leave untracked files. All 28 tables agree on gcc and clang. The real script also imports and runs with no arguments.
- run_grid.py: I ran one grid point (g++, -O0, euclidean, n=4) in a git-initialised scratch mirror of the layout, and it completes. The real script's own set run was not made.
- probe.py before: with the `f5a2698` probe.py, a 10-case gp/antigp sample on clang gave 6 failures ("no member named 'antigp' in ...BinaryOperation").
- probe.py after: `verify` on all seven sets (test, maps, train, train-gen, train-map, plans, wide), both compilers, gives 0 differences. The author ran only test and maps.
- Outputs for test, maps, train, train-map, plans, and wide equal the committed `verify-*.tsv`.

**Criterion 4, reruns: met.**
- `cost.py selfcheck`: Clang 1228 cases, GCC 1235 cases, 0 differences, matching the recorded figures (my runs took 84.7 s and 103.4 s).
- `cost.py comments`: exit 1 on the same four counts: lines 619 (twice), 620, and 624. The sources the README gives are consistent with the `plan.hpp` comment text. The checker and `plan.hpp` are untouched, as the plan allows ("remedy left for the owner").
- `jobserver_check.py`: reproduced exactly. SIGKILL returns 0 of 3, SIGINT and SIGTERM return 3 of 3, a restart under a build lets 8 jobs run, and a missing FIFO lets 34 jobs run on 32 CPUs. Ninja is 1.13.2, and `tools/jobpool` last changed at 6bc4674.
- The author's runs are in `runs.tsv` (the rows are listed under "Process" below).

**Criterion 5, deletion: met.** `git grep bits_fault_injection` hits only decisions, plans, the survivors and README records, `bits.toml`'s header comment, and the review files. None is under `include/`, `test/`, `tools/`, `cmake/`, or the build files. Those mentions are history.

**Process:**
- No file under `include/`, `test/`, `tools/`, `cmake/`, or the build files changed, so I did not rerun the P9 matrix or the convention trace gate (P10).
- `tools/runlog check` passes on this tip. The check also counted my uncommitted rows.
- Commit trailers name the model (Claude Opus 5.5), and the messages say what and why.
- Plan 0037 exists.
- Both repaired scripts have a before and an after row in `runs.tsv`.
- Nothing in the diff judges the author's own change by an expected value taken from the implementation.

## What I did not check

- `format-check` (the diff has no C++ changes).
- The convention trace gate (no `test/` or `include/` change).
- The full P9 matrix.
- A full `run_grid.py` set.
- `probe.py run` and `cost.py fit` (a refit is out of scope).
- The blind-review (plan 0037) process requirements beyond this record.
- Whether the prose in the README paragraph reads well to a user beyond its correctness (a human read is advisable).
- I made no web requests and sent no messages.

## Findings (most severe first; none blocks)

1. **Provenance rows are loose** (low, non-blocking).
   - `docs/agent/snippets/README.md` index rows (the table diff, Spike B, and cost-model rows) say "Last checked at 6a3cf13". The `runs.tsv` rows for the `diff_reference`, `fault_injection`, and `verify-*` runs carry revision `f5a269814000-dirty`: they ran on the uncommitted edits before 6a3cf13 existed. The selfcheck, comments, and jobserver rows are at 6a3cf131b849.
   - Failure scenario: someone who checks the row's revision against `runs.tsv` finds a dirty `f5a2698`, not 6a3cf13. I re-ran all of these on this tip and they pass, so the claim is true; only the recorded revision is imprecise.
   - Action if wanted: reword to "checked on the working tree that became 6a3cf13", or accept the row as is.

2. **The `comments` check still fails** (informational, owner item already listed by the author).
   - `docs/agent/records/performance/plan-cost-model/README.md`, "Rerun at 6a3cf13"; `include/galoot/core/plan.hpp:619-624`.
   - Failure scenario: anyone who runs `cost.py comments` as a gate gets exit 1 until the checker learns these four counts (two sizes and a flag value) or the comment is reworded. The index row says "In use" and names the failure, so it is not hidden.
   - The same README says "about 89 million operations" is not matched because the checker's pattern takes only digits. So that figure is outside the checker's reach, which is a gap in the checker. The README states the cause and the source.
   - Options: (a) leave as is (the author's choice, within the plan); (b) teach `cost.py comments` a list of known non-cost counts, as it already has for limits (`cost.py:472`); (c) reword `plan.hpp`. Recommendation: (b) when `cost.py` is next touched, with no hurry.

3. **The committed `verify-train-gen-{gcc,clang}.tsv` are stale** (informational, pre-existing, not from this change).
   - Each holds 118 data rows. Re-running `verify` on the committed `train-gen-cases.tsv` gives 116, because `traingen47-count` and `traingen47-fill` are not in the case file.
   - The README claim "verify of test and maps equals the committed files" is accurate. The train-gen files are not claimed to match.
   - The other eight files (train, train-map, plans, wide, on both compilers) matched `cmp` exactly.

4. **Wording nit** (non-blocking). In the index row "table diff", "kept so that note 0008's commands stay valid" is loose. Note 0008 line 157 runs the script from `$S/diff_reference.py` (a scratch copy), not from a workbench path. The script is kept so the note's `diff_reference.py` claim stays checkable.

## Runs I made

Rows are in this worktree's `runs.tsv`. All selections are `review:snippets:*` or `review:rga3.readme` or `review:runlog-check`:

- `fault_injection`
- `diff_reference` (also a no-argument import check)
- the `runtime_check` builds, gcc and clang
- `verify-{test,maps,train,train-gen,train-map,plans,wide}` on gcc and clang
- `probe-before` (the old probe.py against the old enumerator names, from scratch trees)
- `comments`
- `selfcheck-clang` and `selfcheck-gcc`
- `jobserver_check`
- `rga3.readme` configure, build, and test on gcc-debug and clang-debug (build trees under `build/` in the worktree, git-ignored)
- `runlog check`
- one `run_grid` point in the scratch mirror

I built nothing in tracked paths. The job pool was already running; I did not start or stop it.

Resource use: about 150k tokens, about 60 tool calls, wall time about 25 minutes (of which about 4 minutes were selfcheck runs).
