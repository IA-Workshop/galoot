# Blind review: the evidence tools (plan 0026)

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5), 2026-11-08. It shares no context with the plan's test author or implementer.
- Reviewed: `git diff develop...feature/evidence-tools`, `develop` at `d0b84bc`, branch tip `d72f74a` (merge of `develop` at `f6392cf`). Recorded on `records/review-evidence-tools` from `d72f74a`, in `.claude/worktrees/review-evidence-tools`.
- The brief carried no verdict. It carried the authors' departures, which this record checks rather than adopts. It also asked for a look at the shared job pool's surplus, which lies outside this unit. That finding is F1 below.

## Departures, skipped checks, ambiguities (C20)

- **Skipped:** P9's full suite on every preset. It passed at `f6392cf`, and `d72f74a` changes only the plan, `agent-cost.tsv` and `runs.tsv` (P10). Also skipped: the cost reruns of criterion 11 and of `rga3-cost/measure.py`, which need the timing lock and an idle machine. Instead, `new/summary.txt` was recomputed from `new/rows.tsv`, and all 16 means, ranges and repeat counts agree.
- **Ambiguity:** the brief expects the rows of `compiles diagnostics` to go to a campaign log. Decision 0214 item 10 sends them to `runs.tsv` on purpose, as the old scripts did, and the tool follows the decision. Only `cost` uses a campaign log: no `cost:` row was added to `runs.tsv`.
- **Uncommitted:** this review's `tools/timed` rows sit in the worktree's `runs.tsv`, not committed. The brief allows only this file to be committed.
- **Shared pool:** never started, stopped or ensured. Its status and its journal were read only.

## What was checked, and how

- Read in full: both tools, both test scripts, the fixtures, the `test/CMakeLists.txt` hunk, decision 0214, plan 0026 sections 1 to 18, the `0026-tools` README, the three repath diffs, and `compare_ports.py`.
- **Protected paths:** nothing changed under `include/`, `cmake/`, `CMakePresets.json` or the root `CMakeLists.txt` (`git diff --stat` is empty).
- **Python:** stdlib only, `#!/usr/bin/env -S python3 -I`, mode 755, Python 3.14.9. No comment cites a plan or decision (S11).
- **W9:** the four test commits carry `-x` trailers, and the tools are in separate commits. All 22 commits carry the agent trailer.
- **Fresh trees** in this worktree, gcc-debug and clang-debug, all through `tools/timed`:
  - `ctest -L '^(tools\.(mutate|compiles)|tests\..*)$'`: 7 of 7 pass on each preset.
  - Trace gate, `format-check` and `tools/runlog check`: pass.
  - Pool: 23 of 23 free before and after.
- **Hand tests of `tools/mutate`** in a scratch fixture repository:
  - SIGINT and SIGTERM sent while `lib.hpp` was mutated give exit 130 and 143, with the file restored. A script sent each signal only once the file differed from `HEAD`, at several delays.
  - An exception inside `record()` leaves the files restored, with exit 1.
  - A SIGKILL leaves the file mutated, as the usage text warns, and the next run refuses it as dirty.
  - Each of these refuses with exit 2: an unknown spec key, a dirty run-time file, a missing build tree.
  - A dirty README gives the revision `+dirty`, and `--check --runtime` prints the run-time diff.
- **The rga3 port** (criterion 8), in a scratch clone at `d72f74a`:
  - `compare_ports.py` finds 0 differences in 92 mutants.
  - Eight mutants were rerun on both presets: `rotation_layout`, `rotation_any_scalar`, `translation_accepts_integers`, `non_floating_catch_all_unconstrained`, `names_swapped`, `point_alias_double`, `motor_alias_double`, `names_wrong_blade`. All 16 verdicts, not-run lists and failed tests equal the recorded rows, and the killer sets are equal (m3). `compare_runs.py rga3` reports only the nine stale-binary differences already explained.
- **The blade_product port** (criterion 9), rerun with `--runtime` on both presets for five mutants: `correction_sign`, `correction_dropped`, `scalar_scale_dropped`, `lowest_generator_highest`, `partial_sum_excursion`. A sixth, `generator_scale_dropped`, was added as an unjudged control.
  - With the three old run-time edits: a leak at `blade_product_test.cpp:856` on all five, on both presets.
  - With the fourth edit: no leak.
  - `generator_scale_dropped` is `unjudged` on both presets (class `warning`). It is counted apart from the kills, and the run exits 1.
  - The rows equal the recorded ones.
- **Repathed scripts** (criterion 12):
  - `wu16/diagnostics.py` reproduces `sizes-rerun-2026-11-08.tsv` byte for byte.
  - `0024-rotation-normalize/mutate.py` on both presets leaves the recorded `out/*.tsv` byte for byte unchanged.
- **Mutants of my own**, 16 of them, written and run by my own script (not `tools/mutate`): apply each edit to the tool, run its CTest test, restore the tool.
  - Killed, each by a check that names the broken behavior:
    - in `tools/mutate`: failed tests left unparsed, the final rebuild skipped, any located error taken as `test`, only the first occurrence replaced, `TEST CASE` lines dropped;
    - in `tools/compiles`: the first-error length off by one, paths left absolute, a source's own compile-fail flags ignored, the dirty check of `diagnostics` removed.
  - Survived, untested as the authors declared: `+dirty` never marked, and no SIGTERM handler.
  - Survived, not declared: see m1.

## Findings

No finding blocks the merge.

**F1 (major, outside this unit; plan 0027's `tools/jobpool`): the automatic top-up makes surplus tokens. Confirmed.**

- *Cause.* The service counts clients by scanning `/proc` (`count_clients`), and only then reads and fixes the FIFO, in a second Python process (`balance`). A client that opens the FIFO and takes tokens in between is counted as lost slots, and those tokens are written back. When the client returns its own tokens, they become a surplus, up to the pool's total. The surplus lasts until the next poll that finds no client.
- *Reproduced* on a private pool (unit `review-evt-pool`, FIFO under the scratchpad, 4 slots, 1 s poll):
  - Synthetic clients that never lose a token: "wrote back 1 of 4" twice in 90 s.
  - Real `ninja` and `ctest` clients through `tools/timed`: "wrote back 4 of 4" and "wrote back 3 of 4" in 240 s. That doubles the pool. Each was drained 1 to 2 s later, at a poll with no client.
- *The shared pool's journal* has "wrote back 9 of 23" at 11:38:59 EST. That is the same second as the start of P9's `clang-release` workflow (`16:38:59Z` in `runs.tsv`), which began just after `clang-debug` ended. The surplus was drained at 11:48:00, after P9's last row. So `clang-asan`, `lint` and `clang-measure` ran with up to 32 slots on 24 cores (P11). This explains the outcome's "32 of 23".
- The race is stated in a comment in `tools/jobpool`, but its size, up to the whole pool, held for as long as the machine stays busy, is not.
- *Recommendation.* Fix it on a branch of its own, for example by writing back only when two consecutive polls with no client see the same deficit. Until then, read a surplus as this race, not as a fault of this unit.

**m1 (minor): five test gaps that my surviving mutants show.**
- Rows out of spec order when a later run adds mutants missing from the file.
- The clean check of run-time files.
- The `mode` check of a row read back.
- `inst_range` taken as `max` instead of `max − min`. Criterion 11 rests on this range; the recorded figures are right (recomputed above).
- The E-core count in shared mode.

**m2 (minor): a run that ends early leaves the selection's programs deleted.** SIGINT, SIGTERM, an exception, or a baseline refused on the second preset all skip the final rebuild. Seen by hand: after a SIGINT, `check_test` was missing. CTest then shows those tests as Not Run. The usage text speaks only of files.

**m3 (minor): the order of the killers column depends on the order in which Ninja jobs finish.** Two of my reruns gave the same set in another order. A comparison of rows has to compare sets, as `compare_runs.py` does.

**m4 (plausible, not observed): `tools/mutate` ignores CTest's exit status for mutants.** A CTest failure without a line of the form `N - name (` would be read as no failure.

**Confirmed as declared:** criterion 9 (a, b), criterion 11 (two of 16 pairs), the reading of the guard rule, the tool sizes, the untested paths, the wrong revision in `2ed4d0d`'s message, and the fixture rows in `runs.tsv`.

## Outcome

Approve, with non-blocking findings m1 to m4. F1 belongs to the job pool, not to this unit.

## Open items for the owner

1. **F1.** Fix the race now, or accept surplus readings until a pool unit fixes it. Fixing it is a small change, but it touches a tool every agent uses. Recommendation: fix it, with a delta review.
2. **m1.** Add the five tests in a follow-up, or accept the gaps. Recommendation: add the range test at least, since a measurement criterion rests on it.
3. The authors' items 1 to 3: the two warning-only mutants, the leaks at lines 515 to 518, and the guard rule. Recommendation: as the authors recommend.
