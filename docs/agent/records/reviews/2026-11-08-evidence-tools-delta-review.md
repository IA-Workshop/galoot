# Delta review: evidence tools, review fixes (plan 0026)

- Reviewer: blind-reviewer (Claude, agent, Opus 5.5), 2026-11-08.
- Range: `d72f74a..ece4bf4` on `feature/evidence-tools`, the fixes for m1 to m4 of `2026-11-08-evidence-tools-review.md`. Worktree `.claude/worktrees/review-tools-delta`, branch `records/review-tools-delta` from `ece4bf4`.
- The brief carried the authors' own summary (each test fails until the fix, all 33 mutants killed). I treated those as claims and checked them myself.

## Departures, skipped checks, ambiguities

- **Departure (W2): three failing commits in a row.** `65632d0`, `2ca060b` and `c1e5451` each fail `tools.mutate` until `76c9e97`. Each message says that its own test fails. But W2 allows "that single commit" to fail, and the second and third also fail because of the earlier tests, which their messages do not say. Decision 0195 item 6 does not apply here, since nothing was combined. In substance the departure is harmless. The failures are Python test failures, not build failures; each one is labelled; and the tests come from a different author than the fix (W9). See the open owner items.
- Skipped: the authors' untested list (unknown spec keys, `+dirty`, an unknown revision, `diagnostics --before`), the run-log and agent-cost rows, and F1, which belongs to another unit. I reran no gcc-release, asan or lint presets. I did not send SIGINT to the whole process group as a terminal does; I sent it to the driver alone.
- Ambiguity: when CTest names a test that hit its own timeout (`N - name (Timeout)`), the mutant counts as killed, which is the usual mutation-testing convention. When CTest itself crashes or is killed (no test named), the mutant is unjudged. The usage text states neither case.

## What I checked, and how

- **New tests against the old tool.** I ran the tip's `mutate_test.py` and `compiles_test.py` against copies of `tools/` at `d72f74a` and at `ece4bf4`, from scratch directories. With the old tool, exactly the expected checks fail: the killer order (`check catch_all_fail`, on both presets), the silent CTest failure (verdict `survived`, problem missing), and the SIGTERM stop (`check_test` and `uses_test` not rebuilt). The m1 cases pass with the old tool, as the authors said. Everything passes at the tip.
- **No test was weakened.** The only lines the delta removes from the tests are the case lists and the `env=`/`cpu=` parameter changes.
- **m2, by hand,** in the fixture repository, with a mutant that compiles (`x << 2`, so a stale program would fail `check`):
  - SIGTERM and SIGINT, at delays of 0 to 0.3 s after the program was deleted: exit 143 or 130, tracked files clean, `ninja -n` reports no work, and CTest passes all three tests. So no program is left deleted or built from the mutated source, even though the killed `tools/timed` leaves its ninja orphaned.
  - An exception in `record()` (the output path made a directory): the run ends with a traceback, the files are restored, and the programs are rebuilt.
- **m4, by hand.** A `ctest` stand-in that kills itself with SIGSEGV gives `unjudged` with "CTest exited 139 without naming a failed test". A mutant that a `static_assert` kills, with CTest silent, stays `killed` but carries the same problem. The summary line counts 1 killed and 1 unjudged, and the run exits 1. `status` and `failed` are reset for each mutant, so no earlier mutant's kill can carry over into a later row.
- **m3.** The killers are now `sorted(set(...))` of test names, followed by sorted locations, so their order no longer depends on timing. My reruns of `rotation_layout` give the recorded set in sorted order.
- **Tool mutants.** `tools.toml` holds 33 mutants, and the record shows 33 killed at `1775c22`. I reran six on gcc-debug and all six were killed by `tools.mutate`, matching the record: `mutate_no_rebuild_after_early_stop`, `mutate_killers_in_report_order`, `mutate_ctest_status_ignored`, `mutate_warning_kills`, `mutate_test_class_ignored`, and `mutate_restore_skipped_after_failed_build`. The run took 1 min 44 s.
- **Ported verdicts.** I reran four mutants on gcc-debug and on clang-debug. On rga3, `rotation_layout` was killed and `angle_type_catch_all_unconstrained` was equivalent. On blade_product, `correction_sign` was killed and `generator_scale_dropped` was unjudged. The verdicts, not-run lists and killer sets equal the recorded rows; only the revision and the killers' order differ. I reverted the rewritten output files afterwards.
- **Fresh tree, gcc-debug and clang-debug.**
  - `-L '^(tools|tests\.)'`: 48 of 48 tests pass on each preset.
  - The trace gate exits 0.
  - `format-check` passes.
  - The pool read 23 of 23 slots free before and after each run.

## Findings

No blocking findings.

- **n1 (nit) `tools/mutate:578-583`: a failed rebuild after an early stop is never reported.** The rebuild in the `finally` block appends its failure to `session.problems`, but the exception that is propagating skips the `print`. A second signal during that rebuild aborts it, and any later presets are then not rebuilt. A rebuild after a baseline refusal repeats a build that is known to fail.
- **n2 (nit) `tools/mutate:59`: the usage text and the code disagree on the exit status.** The usage text says the run exits 0 when every mutant is killed. The code exits 1 when a killed mutant carries the problem that CTest exited without naming a failed test (line 490).
- **n3 (nit) `tools/mutate:512-514`: other columns still depend on timing.** `build_failures`, `failed_cases` and `leaks` are still written in the order the jobs finish. The m3 fix covered only the killers.
- **n4 (nit) `README.md:18`: SIGINT is no longer listed as untested, but only SIGTERM is tested.** SIGINT works, by hand, but nothing tests it or its exit status 130.

## Outcome

Approve, with non-blocking nits n1 to n4.

## Open owner items

1. **The W2 departure.** Options:
   - Accept it, and widen W2 to "a run of test commits before the code they judge, each labelled". This costs nothing now.
   - Require one combined test commit per fix. This is stricter for later units.

   History is not rewritten either way. Recommendation: accept, and widen W2's wording.
2. **The timeout rule.** Options:
   - State in the usage text that a test CTest names as timed out kills the mutant. This is the common practice.
   - Make such a mutant unjudged. This is stricter, but an infinite loop then never counts as a kill.

   Recommendation: state the first.
