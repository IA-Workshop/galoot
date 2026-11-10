# The mutants of plan 0036

Run by the plan's implementer (Claude, agent, Opus 5.5) with `tools/mutate` on gcc-debug, each mutant judged by its tool's own CTest (`tools.jobpool`, `tools.measure_record` through the label `tools.measure`, `tools.mutate`). The selections build nothing. Every build and test went through `tools/timed`; the driver's rows are `mutant:*` in `records/performance/runs.tsv`.

| File | What it is |
| --- | --- |
| `tools.toml`, `out/tools-gcc-debug.tsv` | The 16 mutants of the unit's changes to `tools/jobpool`, `tools/timed`, and `tools/mutate`, against the tip's suite |
| `gap.toml`, `out/gap-gcc-debug.tsv` | The two gap mutants, with `expect = "survived"`, against the suite before the unit's tests |

## Results

- **`tools.toml` at `6f373e7`: 16 killed, exit 0** (463 s). Each mutant written for a new test is killed by that test's CTest.
- **An earlier run at `963ca6a`**, before the blind review's fixes, of the 14 mutants then in the spec: 13 killed, and `mutate_notes_past_next_error` survived. The review found the same (its finding B1). The fixture then gained `overflow_then_guard`, which kills it on clang-debug. The output of that run was replaced by the run at `6f373e7`; its rows are in the run log at revision `963ca6a`.
- **`gap.toml`: 2 survived, as declared, exit 0.** It ran in a detached worktree at commit `46e6c55`, on no branch: `6f373e7` with `test/tools/jobpool_test.py` and `test/tools/measure_record_test.py` put back to their state at the branch point `7715d5a`. Its rows carry `46e6c55a6e9a`; the mutants' rows say `-dirty` because the mutated tool differs from that commit, as every mutant run does. The commit is not kept; `git diff 6f373e7 46e6c55` was exactly those two files.

So both gap mutants count for the gap (W3): `jobpool_gate_removed` (the repair's `free != n` gate dropped) survives the old `tools.jobpool` and is killed by `idle_full`; `timed_missing_fifo_allowed` (a missing FIFO no longer refused) survives the old `tools.measure_record`, which had no case of the pool check, and is killed by `timed_pool`. The blind review confirmed both independently with its own copies.

## Mutants that survive and are not in the spec

Found by the delta review with its own copies, at `6f373e7`, in code that predates or is shared with this unit; no spec mutant was written for them:

- the end-of-job cut of an object's output removed (`tools/mutate`, the loop that collects a failed object's lines): no fixture job fails without an `error:` line, so no object takes a later object's error as its own;
- the notes bounded by the object's last error in place of its next one: no fixture object has three errors.
