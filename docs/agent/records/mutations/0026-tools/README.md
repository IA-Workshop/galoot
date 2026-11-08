# The evidence tools: their own mutants, and the two ports that validate `tools/mutate`

Plan 0026, run by its implementer (Claude, agent, Opus 5.5) in `.claude/worktrees/evidence-tools` on `feature/evidence-tools`. Every build and test went through `tools/timed`, with rows `mutant:*` in `runs.tsv`.

## Files

| File | What it is |
| --- | --- |
| `tools.toml`, `out/tools-gcc-debug.tsv` | The 30 mutants of `tools/mutate` (20) and `tools/compiles` (10), judged by `tools.mutate` and `tools.compiles` on gcc-debug; the selections build nothing |
| `compare_ports.py` | The byte comparison of criterion 8 (and the same for the blade_product port): `tools/mutate --check` against the old drivers' own mutated texts |
| `compare_runs.py` | Verdicts and failed tests of the tool's runs against the old drivers' recorded tables, mutant by mutant |
| `blade-product-three-edits.toml`, `out/blade-product-three-edits-*-runtime.tsv` | The blade_product mutants with the old driver's three run-time edits only, to show the leak of criterion 9 |

## The tools' mutants (plan section 9)

At `3dce33f` the first 29 ran: 28 killed and one survivor, `mutate_restore_skipped_after_failed_build` (a file left mutated after a failed build is hidden by the restore at the end of the run, since every fixture spec mutated one file). The test author added a spec of two mutants on different files (`499f177`). The blade_product port then led to a change of the classifier (below) with its test (`49e6110`) and a 30th mutant, `mutate_guard_not_a_kill`. **Rerun at `9322d1e`: all 30 killed, each by its tool's CTest** (`out/tools-gcc-debug.tsv`, 12 min 37 s). The driver judging its own mutants is the copy loaded when the run started; each test copies the mutated file into a scratch repository, so a fresh process judges each mutant.

Not covered by a test, so no mutant was written for them: restoring the files on SIGINT and SIGTERM, refusing unknown spec keys, the `+dirty` revision, a missing build tree, a revision git does not know, a glob that matches nothing, `diagnostics --before`, and the `--check --runtime` blocks.

## Criterion 8: the rga3 port (`../0023-rga3/mutants.toml`)

- `compare_ports.py`: for all 50 mutants the diff that `--check` prints equals the diff of the old driver's `suite()` and `mutate()` text (with one original, equal unified diffs mean equal texts); it fails when one replacement is changed.
- Runs at `1f7b971`, gcc-debug then clang-debug, 9 min 10 s: **43 killed and 7 equivalent on both build presets, the verdict of every mutant equal to the recorded `out/*.tsv`** (`compare_runs.py rga3`).
- Differences in the failed tests, the same nine on both compilers, each a test the record lists as failed and the tool lists as not run because its program did not build (the stale-binary rule; the old driver ran CTest on every selected test after a failed build): `rotation_layout` (`rga3.motions`, `rga3.rotation_checking_length_2`, `rga3.rotation_unchecked_length_2`), `translation_layout` and `translation_accepts_integers` (`rga3.motions`), `point_alias_double` (`rga3.format`, `rga3.point_groups`), `line_alias_double` and `names_wrong_blade` (`rga3.format`). No test fails now that the record lacks.

## Criterion 9: the blade_product port (`../0011-blade-product/mutants.toml`)

- `compare_ports.py`: all 42 mutants and the old driver's three run-time edits give the old driver's texts.
- **With the old three run-time edits** (`blade-product-three-edits.toml`, at `1fb1511`): a leak at `test/core/blade_product_test.cpp:856`, the `static_assert` of "blade_product reads a pairing with the top generator of the widest algebra", on the five mutants the logician's rerun named (`correction_sign`, `correction_dropped`, `scalar_scale_dropped`, `lowest_generator_highest`, `partial_sum_excursion`), on both build presets, and also on `product_magnitude_x` and `range_widest_shifted`, which that rerun did not list.
- **With that `static_assert` as a fourth edit**: no leak at line 856 on either preset. Leaks remain at lines 515 to 518 on `term_not_trivially_copyable`, `term_list_not_trivially_copyable`, `term_not_aggregate`, and `term_list_not_aggregate`: the `static_assert`s of the term types' properties (aggregate, trivially copyable), which no run-time check can replace and which the old driver's run-time mode also left on. They are reported as the tool's rule says; see the plan's outcome.
- The equivalent mutants are the three of `survivors.md`.
- **Normal mode against `results-b142837-*` (39 mutants in both): 37 verdicts equal; 2 differ.** `generator_scale_dropped` (an unused parameter) and `range_widest_shifted` (a shift count too large) fail to build only by a warning made an error. The old driver counted any failed build as a kill. Under the ruled verdict rules they are `unjudged` on both presets and in both modes: no test judged them. A rewrite that keeps the build warning-free (as 0024's `division_omitted` was rewritten) would let the tests judge them; that is outside a port.
- The failed-test lists differ in two ways, both explained: the old driver at `b142837` built with `cmake --build` without `-k 0`, so after the first failed object the abort program was not built and its tests failed as not run, where the tool builds it and its tests pass; and tests added after `b142837` (`core.blade_product_beyond_n_31`, `_beyond_n_second` and their `rejects_` forms) now fail on the range mutants.
- The classifier change: in run-time mode on gcc-debug, `product_magnitude_x` was first `unjudged`. GCC reports the library's `GALOOT_ASSERT` stopping the test's constant evaluation as a call to a function that is not constexpr, inside `assert.hpp`. Clang reports the same error in the test, and called the mutant killed. At `61f0d98` that error is class `assert` (decision 0214 item 5, as corrected); rerun at that revision, `product_magnitude_x` is killed in both run-time specs.

## Run times

The rga3 mutants took 9 min 10 s for both build presets; the blade_product mutants took about 3 min 50 s per spec and mode for both presets; the tools' mutants took 6 min 1 s (29 mutants) and 12 min 37 s (30, with the longer tests). The pool had 23 of 23 slots free before and after each run, except once, 22 of 23 after the rga3 run, while another unit's gcc-asan build held a slot.
