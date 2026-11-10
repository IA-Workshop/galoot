# 0026 Evidence tools: `tools/mutate`, `tools/compiles`, and the rga3 record scripts repathed

- Status: planned 2026-11-08 by Claude (agent, Opus 5.5), planner role with cpp-expert, doctest-expert, and build-expert judgement, on `records/plan-tools` (worktree `.claude/worktrees/plan-tools`) from `develop` at `5190c58`. Merged into `develop` by `c829b02` on 2026-11-08 (corrected 2026-11-10; see the outcome). The unit is claimed when the coordinator merges this plan into `develop` (W7). Nothing in section 11 after step 0 starts until the owner has answered O1 to O6 (section 15), or has said to proceed as recommended.
- Branches, from `develop` after the claim, named by W7:
  - `feature/evidence-tools`, worktree `.claude/worktrees/evidence-tools`: the unit. Only this branch merges into `develop`.
  - `feature/evidence-tools--tests`, worktree `.claude/worktrees/evidence-tools--tests`: the test author's fixtures and test scripts, never merged; each group is cherry-picked with `-x` into the unit (decision 0185).
- Record numbers: decision 0214 and the mutation record `records/mutations/0026-tools/`, the next free at this commit on every branch (no branch holds a plan numbered 0026 or a decision above 0213). A number taken in the meantime moves to the next free one.
- Source of scope: the owner's rulings of 2026-11-08 on the M4 snippets review ("accept all recommendations"): write the general mutation driver and the cost and diagnostics harness before WU14, as a small unit; keep the evaluation-counter snippet for WU17; repath the three rga3 scripts in this unit. The review is `docs/agent/snippets/reviews/M4.md` on `records/snippets-m4` at `6f1e90a` (items 5 to 8, "Owner decisions needed" 1 to 3). Also: handoff `records/handoff-2026-11-07-second.md` item 10 (the `mutate.py` gaps), lesson 0063 (CTest keeps a pool slot for a missing program), lessons 0060 and 0061 (mutation runs check the build; edit scripts assert their anchor), plan 0025's outcome ("the record scripts keep their old paths until a rerun").
- Words: a *driver* runs mutants; a *spec* is the data file that lists one record's mutants; a *selection* is the build targets and the CTest label regex that judge a mutant; a *build preset* is a CMake configuration of `CMakePresets.json`.

## 1. Departures, ambiguities, and readings (C20)

1. **"Decision 0202 item 10" does not exist.** Decision 0202 has items 1 to 7; its item 6 cites "handoff items 5, 6 and 10". The two `mutate.py` gaps are in item 10 of `records/handoff-2026-11-07-second.md`. Traced to their sources:
   - *`static_assert` at line 856 in `--runtime` mode* is a gap of galoot's `docs/agent/records/mutations/0011-blade-product/mutate.py`, not of anything in math-tools. It was found by the math-tools logician's rerun of that script on galoot `c86cb58`, recorded outside galoot in `math-tools/docs/agent/logician/section1-mutation-rerun.md` ("Caveats", first item): the script's `RUNTIME_EDITS` turn off three product `static_assert`s of `test/core/blade_product_test.cpp`, but the one at line 856 (test case "blade_product reads a pairing with the top generator of the widest algebra", added after the list) stays on and still stops the build in `--runtime` mode on five mutants (`correction_sign`, `correction_dropped`, `scalar_scale_dropped`, `lowest_generator_highest`, `partial_sum_excursion`), so the run-time record does not show which doctest cases would kill them. The only `mutate.py` in math-tools is `math-tools/exercises/logic/E20-mutation/mutate.py`, a proof-assistant driver unrelated to galoot.
   - *Stale-binary behavior* is a property of the nine drivers of the newer family (section 5), all in galoot's records: each builds with `ninja -k 0` and then runs CTest whatever the build did, so a program that failed to build is run from its previous build and passes. `records/mutations/0019-antiproducts/README.md` (line 31) and `0193-test-split/README.md` state it as a caution on reading the records. The older `0011-blade-product/mutate.py` does not have it: it deletes its programs before each build and skips CTest after a failed build, the fix of lesson 0063, at the price of not running the compile-fail tests after a failed build. Neither gap is in `tools/`; galoot has no general driver yet.
2. **Four verdicts, not three.** The brief asks for killed, equivalent, or survived per mutant. A mutant whose only build failures come from a warning made an error, or from an error that is not a check (a malformed mutant), and whose built programs all pass, is none of these: the tests of the programs that did not build never ran. The tool reports it as `unjudged` (section 6.1), as `0024-rotation-normalize/README.md` treated `division_omitted` ("a warning made an error, which does not count") before the mutant was rewritten.
3. **"Selection" read as the judging selection.** The spec names, for each group of mutants, the targets to build and the CTest label regex to run; which mutants run is chosen on the command line, as in every existing driver.
4. **`tools/measure` does not already do the cost harness's work.** It records the cost of one command. The seven harnesses add a "before" tree from `git archive`, compiles pinned to one CPU and interleaved by repeat, reading back the row each compile wrote, and a summary. So a wrapper is needed (section 6.2); `tools/measure` is not changed.
5. **The old cost harnesses depart from P13.** Each writes its pinned compiles into the run log `runs.tsv` and reads back its last row, where P13 sends a measurement campaign's runs to the campaign's own log. The new tool uses a campaign log (section 6.2). The old scripts are not changed beyond the repath, so the repathed `rga3-cost/measure.py` still writes `runs.tsv`.
6. **Ruling on the repath, read literally.** The three scripts are repathed in place (paths, and "model" to "preset" in their docstrings), not ported to the new tools; each is rerun once to show it runs (criterion 12). The review's recommended one-line note goes into each record's README, or for `diagnostics/wu16/`, which has no README, into `diagnostics/wu16-messages.md`.
7. **Which scripts this unit ports.** The review recommends that "the existing drivers stay where their records cite them". This plan ports two mutant sets to specs, as the tool's acceptance cases and because each has a present user (section 5), and leaves every other script as a historical record. O3 asks.
8. **The survey counts agree with the review**: nine drivers with the five functions `mutate`, `timed`, `failed_builds`, `run`, `main` (`0019-antiproducts`, `0019-linear`, `0020-duals`, `0020-rounding`, `0020-rounding-wu11`, `0021-transform`, `0021-transform-rounding`, `0022-format`, `0023-rga3`), with `0024-rotation-normalize` (only `timed` and `main`) a tenth; seven `measure.py` harnesses with `extract`, `read_last_row`, `compile_pinned`; six diagnostics scripts; eight reviewer mutant scripts under `records/reviews/` (`2026-11-01-wu6a-blade-product/mutate_review.py`, `2026-11-01-wu6b-gp-plan/mutate_review.py`, `2026-11-01-plan-0012-cost-growth/mutate_review.py`, `2026-11-02-wu8b-delta/mutants.py`, `2026-11-07-wu13a-review/mut.py` and `mut_delta.py`, `2026-11-07-wu13b-review-probes/mutants.py`, `2026-11-07-wu13b-delta-review-probes/mutants.py`). Two details the review does not give: the two diagnostics scripts of WU15 and WU16 write their columns in different orders, and one reviewer script (`wu13b-review-probes/mutants.py`) restores the header with `git`, which would also discard any uncommitted edit of it.

## 2. Goal

Two small tools that replace the copied drivers for every later unit, starting with WU14, with the knowledge the copies carry built in once and tested: `tools/mutate`, which runs a record's mutants from a declarative spec on gcc-debug and clang-debug and reports each as killed, equivalent, survived, or unjudged; and `tools/compiles`, which measures the compile cost of units before and after a change (`cost`) and the size of the diagnostics of compile-fail sources (`diagnostics`). And the three rga3 record scripts that would fail on a rerun, repathed. Nothing under `include/` changes, and no recorded result is rewritten.

## 3. Definition of done

1. `tools/mutate` and `tools/compiles` exist, each with a usage docstring a reader can run from, and CTest tests with fixtures (section 8) under the labels `tools` and `tools.mutate` or `tools.compiles`, passing on gcc-debug and clang-debug.
2. Two mutant sets are ported to specs and run with `tools/mutate`, with outcomes that agree with their old drivers' records under the criteria of section 4; the old drivers and their outputs are untouched.
3. `tools/compiles` reproduces the recorded WU16 diagnostics and the rga3 compile-cost figures under the criteria of section 4.
4. The three rga3 scripts name the preset's paths and run.
5. The tools' own mutants are run and recorded in `records/mutations/0026-tools/`, each killed by the tools' tests or shown equivalent.
6. Decision 0214 records the spec format, the verdict rules, and the flag sources; the snippet index rows that name the general driver and harness point at the tools.
7. W5 review passed, with a delta review of any fix to the tools; P9's merge checks pass; this plan's outcome records the commits, runs, costs, and open items.

## 4. Acceptance criteria

Each is checked and stated in the outcome with its revision and command (V10).

`tools/mutate`

1. Refuses with exit 2, before any build and with the file unchanged, when an anchor of any selected mutant occurs a number of times other than its declared count, when a mutant's edits change nothing, when a file it would edit has uncommitted changes, when a build tree is missing, when the selection matches no test, when a selected test runs a program that none of the selection's targets builds, or when the unmutated baseline does not build and pass.
2. Never runs a program left from an earlier build: before each mutant's build it deletes the programs of the selection's targets, and it passes CTest only the tests whose programs exist after the build; the rest are listed in the row as not run. No run leaves CTest a test whose program is missing (lesson 0063).
3. Classifies each failed object by its first error (section 6.1) and gives each mutant one verdict; exit 0 only when every mutant run is killed or declared equivalent, every declared killer is among the actual killers, and no run-time-mode leak is found; otherwise exit 1 with a summary that names each case.
4. In `--runtime` mode, applies the spec's run-time edits to the test sources for the whole run, baseline included, and reports any `static_assert` of those sources that still fails as a leak, by file and line.
5. Restores every edited file byte for byte after each mutant, after a failed build, and on SIGINT or SIGTERM, and rebuilds the selection's targets at the end.
6. Writes `out/<spec stem>-<build preset>[-runtime].tsv` beside the spec, keyed by mutant, so a partial rerun replaces its own rows and keeps the others; never writes a file an old driver wrote.
7. `--check` validates the spec and every selected anchor and prints each mutant's unified diff, without building.
8. The port of `records/mutations/0023-rga3/` (50 mutants): `--check` produces, for every mutant, the same mutated text as the old driver's `suite()` and `mutate()` (byte comparison by a committed check in `records/mutations/0026-tools/`); on both build presets the tool reports 43 killed and 7 equivalent, the same verdict per mutant as the recorded `out/*.tsv`; each difference between the recorded failed-test lists and the tool's is explained in the record, mutant by mutant, by the stale-binary rule or by the compile-fail tests that now run after a failed build.
9. The port of `records/mutations/0011-blade-product/` (42 mutants, normal and `--runtime`): with the old three run-time edits, the tool reports a leak at the `static_assert` of the "top generator of the widest algebra" case on the five mutants named in item 1 of section 1, on both build presets; with that `static_assert` added as a fourth edit, it reports no leak; the mutants declared equivalent are the author's three of `survivors.md` (`contraction_transposed`, `correction_transposed`, `sum_guard_branch`); the verdicts in normal mode equal the recorded `results-b142837-*` tables for the 39 mutants they share (the logician's rerun found the same).

`tools/compiles`

10. `diagnostics` over `test/presets/rga3/*_fail.cpp` on both compilers gives, per source and compiler, one error line, as `diagnostics/wu16/sizes.tsv` records, and the same first error text as the repathed `wu16/diagnostics.py` run at the same revision, apart from differences the record explains (the two take their flags from different places, section 6.2); a source that compiles is reported and makes the exit status 1.
11. `cost` over the units of `records/performance/rga3-cost/` (the two includers and the five test units, at the recorded before revision), three repeats pinned to one performance-core CPU, under the timing lock: for each unit and compiler the mean retired instructions differ from those of the repathed `measure.py`, run in the same session, by less than the larger of the two runs' ranges; or the flag difference that explains the gap is listed. This criterion is stated before either run (P7). Rows go to a campaign log in the output directory, never to `runs.tsv`.

The repath

12. `0024-rotation-normalize/mutate.py` names `include/galoot/presets/rga3/motions.hpp`; `rga3-cost/measure.py` includes `<galoot/presets/rga3.hpp>` and compiles `test/presets/rga3/<unit>_test.cpp`; `diagnostics/wu16/diagnostics.py` globs `test/presets/rga3/*_fail.cpp`; their docstrings say "preset". Each runs to completion once on this branch: the mutants on both build presets with all 11 killed by the tests the README lists, its tables saved as new files beside the recorded ones and the recorded `out/*.tsv` restored; the cost harness with the runs of criterion 11; the diagnostics with the run of criterion 10, written to a new file.

## 5. The survey and what happens to each script

| Scripts | What they share | This unit | Why |
| --- | --- | --- | --- |
| `records/mutations/0019-antiproducts` to `0022-format` (seven drivers) and `0021-transform` | `mutate`, `timed`, `failed_builds`, `run`, `main`; a table of (header, edits); `ninja -k 0`, CTest after any build | Left as records | No present user; their records cite them at their revisions. `0021-transform` is ported by the core unit of decision 0202 items 2 and 3 (S2, S3), which changes `transform.hpp` and is its next user |
| `0023-rga3` | as above, plus helpers that build anchors for layouts, aliases, and names | Ported: `0023-rga3/mutants.toml` | Its anchors are current (plan 0025 updated them); 50 mutants over two headers and two selections, 7 declared equivalent: the widest test of the spec format. Next user: any change to the preset |
| `0024-rotation-normalize` | `timed`, `main`; skips CTest after a failed build | Repathed, not ported | The owner's ruling; 11 mutants, a port adds nothing the 0023 port does not test |
| `0011-blade-product` (older family; deletes programs, skips CTest after a failed build, `--runtime`) | run-time edits of the test file | Ported: `0011-blade-product/mutants.toml`, with the fourth run-time edit | The only set that exercises run-time mode and the line-856 gap; its user is the retirement procedure the logician's note repeats for the product tests and, per that note, for the bits and blade-set tests once they have mutation sets |
| `0008` to `0018` (other older drivers, `0017-runlog-distilled` mutating a Python tool) | various | Left as records | No present user |
| Eight reviewer scripts under `records/reviews/` | the newer family's shape | Left as records | A later reviewer writes a spec of its own mutants and runs it with the tool |
| Seven `records/performance/*-cost/measure.py` (`antiproducts`, `linear`, `duals`, `format`, `rga3`, `transform`, `transform-rounding`) | `extract`, `read_last_row`, `compile_pinned`; two add `oriented` and `growth_unit`, one adds run-time timing | `rga3-cost` repathed; all left as records | Their growth units and run-time timing are WU17's P2 grid and benchmark harness, not built here (C12); `tools/compiles cost` takes a unit's source as a file, so a later unit's generated growth units are files it writes |
| Six diagnostics scripts (`mutations/0019-antiproducts/diagnostics.py`, `0019-linear`, `diagnostics/wu11a`, `wu13a`, `wu15`, `wu16`) | compile each fail source with `-fsyntax-only` on both compilers, count lines and errors | `wu16` repathed; all left as records | `tools/compiles diagnostics` replaces them for later units |
| `diagnostics/catalogue/scan.py` | maps each core message to its tests | Left; not in this unit | A different tool; its user is WU14's compile-fail catalogue (O6) |

## 6. Design

The tools are Python 3.14 scripts run as `python3 -I`, standard library only, in the style of `tools/runlog` and `tools/include_scan.py`. Their comments describe what the code does and cite no plan, decision, or provision (STYLE S11). Each is meant to stay near 300 lines; the outcome records the sizes.

### 6.1 `tools/mutate`

Usage, from the root of a worktree whose build trees no other workstream uses, with the job pool exported:

```
tools/mutate <spec.toml> [--runtime] [--check] [--preset <build preset>]... [<mutant>...]
```

The spec is TOML, read with `tomllib` (O1). Multi-line literal strings (`'''`) hold anchors with their newlines and backslashes exactly. A sketch:

```toml
presets = ["gcc-debug", "clang-debug"]

[selection.motions]
targets = ["rga3_motions_test", "rga3_rotation_mode_0", "rga3_rotation_mode_1"]
labels = '^rga3\.motions$'

[[runtime]]                      # applied for the whole run under --runtime
file = "test/core/blade_product_test.cpp"
anchor = "static_assert(added("
replacement = "static_assert(true || added("
count = 3                        # the anchor must occur exactly this often; all are replaced

[[mutant]]
name = "division_omitted"
selection = "motions"
file = "include/galoot/presets/rga3/motions.hpp"
edits = [{ anchor = '''const T s = std::sin(half) / length;''', replacement = '''const T s = std::sin(half);''' }]
expect = "killed"                # or "equivalent", which needs a reason
killers = ["rga3.motions"]       # optional: tests expected among the killers
```

`count` defaults to 1. Choosing some occurrences of an anchor, which `0021-transform` and `0022-format` do, is added by the unit that ports one of them, not now (C12). A run-time edit replaces a check with `true || ...` rather than deleting it, so that no function a check used becomes unused and fails a `-Werror` build, as deleting lines did in the bits snippet's `--runtime-only` mode on Clang.

For each build preset in turn:

1. Validate before touching anything: the spec, every selected mutant's anchors and counts, that its edits change the text, that each file to edit equals `HEAD`, that `build/<preset>` exists, that `ctest --show-only=json-v1 -L <labels>` selects at least one test (a filter matching nothing must not pass), and that each selected test whose command runs a program of the build tree runs one that a target of the selection builds (the program of a target is resolved through Ninja). This last check is the stale-binary guard at the level of the spec: it rejects a selection that would run a program the driver never rebuilds, the mistake plan 0019 records for a `core.mv` run.
2. Baseline: build the targets and run the selection on the unmutated files (with the run-time edits under `--runtime`); it must pass, so that a later failure is the mutant's doing.
3. For each mutant: apply its edits; delete the selection's programs; build the targets with `ninja -C build/<preset> -k 0`; run CTest on the selected tests whose programs now exist (compile-fail tests always run, since they compile the mutated header themselves); restore the files. Every build and test goes through `tools/timed` with the selection `mutant:<name>`, the baseline as `mutant:baseline`, the final rebuild as `mutant:restore`; `TIMED_LOG` sends them to a campaign log when the caller sets it.
4. Rebuild the targets on the restored files.

A selection with no targets has no build step and no programs to delete; its tests are scripts that CTest runs (section 9 is its user).

Each failed object is classified by its first error line, which the row records with paths made relative to the root:

| Class | First error | Counts as |
| --- | --- | --- |
| `assert` | a failed `static_assert` (GCC's and Clang's spellings), in a test source or in the library's own checks | a kill |
| `test` | any other error located in a test source, such as a call with no viable overload | a kill |
| `warning` | carries `[-Werror` | not a kill |
| `invalid` | any other error, such as a mutant that is not valid C++ | not a kill; the mutant needs rewriting |

A failed CTest test is a kill. The verdict of a mutant on a build preset is `killed` if anything killed it; else `unjudged` if some object failed (its tests did not run); else `equivalent` if the spec declares it so, or `survived`. A mutant declared equivalent that is killed is reported as unexpected, as is a declared killer that is not among the actual ones. O4 asks the owner to confirm these rules, which decision 0214 records.

The row of a mutant on a build preset holds: mutant, preset, revision (with `+dirty` when files other than the edited ones differ from `HEAD`), verdict, expected, the killers (failed tests, and `<source>:<line>` for `assert` and `test` objects), the build failures (`<object>: <class>: <first error>`), the tests not run because their program did not build, the failed doctest cases (from `TEST CASE:` lines), and under `--runtime` the leaks: `static_assert`s of the run-time edit files that failed, by line.

Not built (C12): running build presets in parallel, configuring build trees, choosing occurrences, any output format other than the TSV and the summary.

### 6.2 `tools/compiles`

```
tools/compiles cost <spec.toml> <out-dir> --before <revision> --cpu <cpu|shared> [--repeats <n>]
tools/compiles diagnostics <out.tsv> --label <label> [--before <revision>] [--flags-from <test>] <source glob>...
```

Shared by both: the "before" tree is `git archive <revision> include test`, extracted into the output directory; the "after" tree is the checkout, whose `include/` and `test/` must be clean. Each compile goes through `tools/timed` with a selection of the form `cost:<compiler>:<unit>/<tree>` or `diagnostics:<label>:<compiler>:<source>`.

Flags come from the build trees, not from lists in the script (O5), so that a change of the presets' flags cannot leave a measurement using the old ones:

- a test source is compiled with its own command from `build/<compiler>-<kind>/compile_commands.json`;
- any other unit, and every compile-fail source, with the compile command of the build tree's compile-fail tests, read from `ctest --show-only=json-v1`: the source's own compile-fail test when it has one, else the test named by `--flags-from`. That command comes from `galoot_compile_command` in `cmake/galoot_testing.cmake`, which `tests.compile_fail_flags` keeps equal to what CMake compiles the core's header checks with.

The launcher prefix (`tools/measure tu ... --`), the object, and the source are dropped from the command, and in a "before" compile each `-I` or `-isystem` path under the checkout is moved to the extracted tree. Both subcommands write the flags they used into their output, so a later reader can compare like with like.

`cost`: the spec lists units, each a source path (or a short inline text written to `<out-dir>/units/`), the trees it is compiled in (`before`, `after`, or both), and the kind of build tree its flags come from (`debug` or `measure`). Compiles run as `taskset -c <cpu> tools/timed --log <out-dir>/runs.tsv measure ...`, interleaved repeat by repeat and unit by unit as the old harnesses do; each row is read back from that campaign log, which has one writer, and must be a pinned row of the expected selection. `--cpu shared` runs without `taskset` and marks the summary "not a measurement"; its user is the tool's own test. A counter written `unavailable` stays `unavailable` in the summary, never 0. Output: `rows.tsv`, `summary.txt` (per unit and compiler: mean and range of instructions, mean wall time, largest peak memory, the flags), the campaign log, and the extracted tree, which the record does not commit. The tool does not take the timing lock; the caller takes it for a measurement (P11).

`diagnostics`: each source is compiled with `-fsyntax-only` in place of `-c` and the object, on GCC and Clang. A row holds label, source, compiler, exit, lines, error lines (lines that hold `error:`), longest line, length of the first error line, and the first error line, with paths relative to the tree. A source that compiles is a row with exit 0 and makes the tool's exit status 1.

Not built (C12): growth units, run-time timing, statistics beyond mean and range, any attribution by `-ftime-trace`. Those are WU17's.

### 6.3 The repath

One commit changes the three scripts' paths and their docstrings' "model" to "preset", and adds one dated line to each record's README (`0024-rotation-normalize/README.md`, `rga3-cost/README.md`, `diagnostics/wu16-messages.md`): the script now names the preset's paths after plan 0025's move (`26c8a82`); the recorded outputs were made at the old paths and are kept as they are. `rga3-cost/out/units/rga3_only.cpp`, an output of the recorded run, is not changed; a rerun writes its own copy into its own output directory. If an anchor of `0024`'s mutants no longer matches after the move, the script stops (it asserts each anchor), and the anchor is updated in the same commit, listed in the record.

## 7. How recorded runs stay valid

- No recorded output, README sentence, or result table is edited. The only additions to old records are the one-line notes of section 6.3 and a line in the READMEs of `0023-rga3` and `0011-blade-product` (in `survivors.md` for the latter) that names the new spec and its output files.
- Old drivers stay where their records cite them and run at the revisions their records name (`git worktree add` at that revision). Only the three repathed scripts change, and only in paths and docstrings.
- The new tool writes files whose names no old driver used (`out/mutants-<preset>.tsv` beside `mutants.toml`); a rerun of a repathed script that writes over its recorded table has the new table saved as a new file and the recorded one restored from version control, its absolute path listed first (W7).
- A result of the tools is compared with an old record only under a criterion written before the run (criteria 8 to 11), and a difference is explained, not absorbed. The flags differ in source between old and new cost runs, which is why criterion 11 runs both routes in one session rather than comparing with the recorded figures.

## 8. Tests of the tools

Written by the test author on `feature/evidence-tools--tests` from this section, before the tools; registered by the build-expert in `test/CMakeLists.txt` under `tools;tools.mutate` and `tools;tools.compiles`. Each test script builds a fresh git repository in a scratch directory under the build tree, as `test/tools/runlog_test.py` does, copies `tools/timed`, `tools/measure`, and the tool under test into it, so that every row goes to the scratch repository's run log and the real one is never read or written, and prints one line per failed check. The fixtures use plain C++ and CMake, not doctest; the runtime program prints a `TEST CASE:` line in doctest's form when its check fails. Each expected outcome is derived by hand from the fixture (V2), and each check is one that a mutant of section 9 makes fail.

Fixture for `tools/mutate`: a header `lib.hpp` with one `constexpr` function, a catch-all whose `static_assert` names a message, and a run-time path; `check_test.cpp` with one `static_assert` on the function and a run-time check; `uses_test.cpp`, a second program; a compile-fail test on the catch-all's message; built with `-Wall -Werror` in two build trees, one per compiler.

| Check | Fixture mutant or spec | Expected |
| --- | --- | --- |
| Bad anchor | an anchor that occurs 0 times; one that occurs twice with count 1 | exit 2 before any build; files equal `HEAD`; no `mutant:` row in the scratch run log |
| No change | an edit whose replacement equals its anchor | exit 2 |
| Dirty file | `lib.hpp` edited before the run | exit 2, the edit kept |
| Empty selection | a label regex that matches nothing | exit 2 |
| Unbuilt program | a selected test whose program is not in the targets | exit 2 |
| Failing baseline | a fixture with a failing check | exit 2 |
| Run-time kill | a wrong value on the run-time path only | `killed` by the run-time test, its `TEST CASE:` line in the row |
| `static_assert` kill and stale binary | a wrong value the test's `static_assert` sees | `killed`, class `assert` at the test line; the program's test listed as not run; the program file absent after the build; CTest's output has no "Not Run" |
| Compile-fail kill | the catch-all's message changed | `killed` by the compile-fail test |
| Survivor | `2 * x` written `x + x` with `expect = "killed"` | `survived`, exit 1 |
| Equivalent | the same mutant declared equivalent | `equivalent`, exit 0 |
| Declared equivalent killed | an equivalent declaration on a killing mutant | `killed`, reported unexpected, exit 1 |
| Wrong killer | `killers` naming a test that does not fail | reported, exit 1 |
| Warning only | an unused variable in `uses_test.cpp`'s path only | `unjudged`, class `warning`, exit 1 |
| Invalid mutant | a syntax error in the header | `unjudged`, class `invalid` |
| Run-time leak | `--runtime` with an edit that turns off one of two `static_assert`s | the other reported as a leak by line; exit 1 |
| Run-time mode | `--runtime` with both turned off | the mutant killed at run time, no leak |
| Restore | after every case | files byte-identical to `HEAD`; targets rebuilt |
| Partial rerun | one mutant rerun | its row replaced, the others kept, rows in spec order |
| `--check` | a two-edit mutant | its unified diff printed, no build |
| Two presets | the default `presets` | one output file per build tree |

Fixture for `tools/compiles`: a scratch repository with `include/` and `test/` at two commits, the second adding a function to the header, and a hand-written `compile_commands.json` and CTest JSON per compiler whose commands carry the launcher prefix and `-I` paths under the root.

| Check | Expected |
| --- | --- |
| Before tree | built from the named commit; a before compile reads the first commit's header (a unit that `static_assert`s on the added function fails before and passes after) |
| Include paths | every `-I` and `-isystem` under the root points into the before tree in a before compile, and nowhere else |
| Launcher | the `tools/measure tu ... --` prefix is not in the command run |
| Dirty after tree | exit 2 |
| Failing unit | exit 1 with the compiler's message |
| Interleaving | rows in order repeat, compiler, unit |
| Campaign log | rows in `<out-dir>/runs.tsv`; the scratch repository's `runs.tsv` has no `cost:` row |
| Shared mode | summary marked "not a measurement"; `unavailable` counters kept as `unavailable` |
| Diagnostics | a source with one error: error lines 1, first-line length equal to the first error's length; a source with two errors: 2; a source that compiles: exit 0 in its row and tool exit 1; paths relative to the tree; flags from the source's own compile-fail test, or from `--flags-from` |

The tests take a few seconds of compiling each; if either takes long enough to slow the edit and test cycle, it carries `slow` (P9), measured, not guessed. They need the shared pool, which `tools/timed` requires; under CTest it is inherited from the caller.

## 9. Mutants of the tools

Run with `tools/mutate` from a spec in `records/mutations/0026-tools/`, whose selection has no targets and the labels `^tools\.(mutate|compiles)$`; a selection without targets skips the build step, which is this record's present use of it. For mutants of `tools/mutate` itself the driver is the copy loaded when the run started; the test copies the mutated file into its scratch repository, so each mutant is judged by a fresh process. Because the driver judges its own mutants, the blind reviewer also runs an independent set with a script of its own (section 13).

`tools/mutate`: the anchor count check removed; the no-change check removed; the dirty-file check removed; the empty-selection check removed; the unbuilt-program check removed; the baseline skipped; programs not deleted before a build; tests of unbuilt programs passed to CTest; `warning` counted as a kill; `invalid` counted as a kill; `test`-class errors not counted; equivalent declarations ignored; the unexpected-kill report removed; the killer comparison removed; leak detection removed; run-time edits not applied to the baseline; restore skipped after a failed build; rows of earlier runs dropped; exit status always 0.

`tools/compiles`: the before tree taken from `HEAD`; include paths not moved; the launcher kept; the dirty check removed; interleaving by unit instead of by repeat; rows written to `runs.tsv`; `unavailable` read as 0; a compiling source not reported; notes counted as errors; flags taken from the wrong compile-fail test.

Each must be killed by the tests of section 8, or shown equivalent with its reason in the record.

## 10. Evidence (V10)

- Verification of the tools: the fixture tests of section 8, whose expected outcomes are derived by hand from the fixtures (V1 kind 3, an independent route: the outcome of a small program is known without the tool), and the tools' mutants of section 9 (each test able to fail, V2).
- Validation, that the tools give the answers the project means: agreement with independent earlier routes on real records, under criteria stated here before the runs: the old `0023-rga3` driver's verdicts and its mutated texts (criterion 8); the leak on the five mutants that the logician's rerun found by reading builds (criterion 9); the repathed `wu16` diagnostics (criterion 10); the repathed rga3 cost harness, run in the same session (criterion 11). These are comparisons with other implementations (V1 kind 4), made after the fixture tests pass.
- The outcome states, separately, whether verification and validation ran and passed, with the revision and command of each run; a check run only from scratch steps is labeled not reproducible until committed.

## 11. Parts and order

0. This plan merged into `develop` (the claim); the owner's answers to section 15 recorded in decision 0214.
1. Part A, the repath (section 6.3), first, since it is independent and small: one commit on `feature/evidence-tools`, with the three reruns done later where criteria 10 to 12 need them.
2. Part B, `tools/mutate`: the test author's fixture and test script on the tests branch, cherry-picked with `-x` (fails until the tool lands, and says so); the tool; its CTest registration.
3. Part C, the two ports: `0023-rga3/mutants.toml` and its byte comparison, then its runs on both build presets; `0011-blade-product/mutants.toml`, its runs in both modes with three run-time edits, then with four. The record READMEs gain their one line.
4. Part D, `tools/compiles`: tests first as in part B, then the tool and its registration.
5. Part E, the validation runs of criteria 10 and 11 and the reruns of criterion 12 (the cost runs under the timing lock, with the pool read before and after).
6. Part F, the tools' mutants (section 9) and their record.
7. Part G, decision 0214, the snippet index rows (the general driver and harness rows point at `tools/mutate` and `tools/compiles`; "bits fault injection" says the driver now exists and the snippet stays until the bits tests have a spec), and the plan's outcome.
8. Review (section 13), fixes and delta review, merge checks, merge.

Parts B and D are independent once part A is in, and may run in two sessions; C needs B, E needs A and D, F needs B and D.

## 12. Checks (P8, P9, P13)

- During work: `ctest -L '^tools\.(mutate|compiles)$'` on gcc-debug; the ports' runs on gcc-debug and clang-debug, as they need both. The rest of the suite is not affected by `tools/` (P8), except `test/CMakeLists.txt`, whose change runs the tests it registers.
- Merge into `develop`: P9's checks from new, empty build trees on each supported build preset, `clang-measure` among them, with the style checks, the trace gate, and `tools/runlog check`, every run as `gate:<selection>` through `tools/timed`.
- Every build, test, compile, and mutant run goes through `tools/timed` (P13); the cost runs to their campaign logs. The job pool is read before and after each heavy run, and never started or stopped by the unit.

## 13. Review plan (W5)

A blind reviewer (Opus, high effort, a configuration that shares no context with the authors) gets the branch, section 4, and the authors' departures, and checks: the tools against sections 6 to 8; that no recorded output changed (`git diff develop -- docs/agent/records` shows only additions and the three scripts' paths and docstrings); the ports' comparisons; that the tests can fail, by running at least five mutants of each tool of its own choosing with a script of its own, recorded with the review, since `tools/mutate` judges its own mutants in section 9; that nothing under `include/`, `cmake/`, or the presets changed; and STYLE S11 in the tools' comments. A fix to a tool after the review gets a delta review. The usability checklist applies only in so far as the tools' usage text and messages are an interface for agents and the owner; the reviewer reads them as a first-time user would.

## 14. Risks

1. **Pool slots lost** if a regression lets CTest run a test whose program is missing (lesson 0063). The design passes CTest only existing programs, the fixture checks it, and the implementer reads `tools/jobpool status` before and after each run; a loss is reported, and the owner restarts the pool.
2. **A driver that judges itself.** A defect of `tools/mutate` can hide its own mutants. Mitigated by the fresh process per mutant, the hand-derived fixtures, and the reviewer's independent mutants.
3. **One shared driver for authors and reviewers** removes the independence the copied scripts gave by accident. The independence W5 asks for is in the mutants and their reading, which stay the reviewer's own; the driver is checked once, by its tests.
4. **Error classification by text.** GCC and Clang word a failed `static_assert` differently and may change it with a version. The patterns are those the records already use; a version change shows up as `invalid` rows, which the summary names.
5. **Flags from the build trees** need configured trees for `gcc-measure` and `clang-measure` in the worktree, and differ in source from the old harnesses' lists, so new and old cost records are compared only as criterion 11 does.
6. **A crash between edit and restore** could leave a mutated header. Mitigated by restoring on SIGINT and SIGTERM, by refusing to edit a file that differs from `HEAD`, so that `git diff` shows any leftover, and by never restoring with `git`. A SIGKILL is not covered; the tool's usage text says how to check.
7. **Scope growth.** The ports invite porting the rest; the plan ports two, and each later unit ports what it needs.

## 15. Open questions for the owner

| | Question | Options and tradeoffs | Recommendation |
| --- | --- | --- | --- |
| O1 | The spec format | TOML with `tomllib`: declarative, standard library, exact multi-line anchors, but anchors the old drivers computed with helpers are written out in full, which makes the 0023 spec long. A Python data module: helpers stay, but a spec becomes code to review and can do anything. JSON: no multi-line strings | TOML |
| O2 | Names and shape | `tools/mutate` and one `tools/compiles` with two subcommands, sharing the before tree and the flag sources (about 60 lines). Three tools would repeat that part; a subcommand of `tools/measure` would mix orchestration into the recorder every build uses | `tools/mutate`, `tools/compiles` |
| O3 | What to port now | Only `0023-rga3` and `0011-blade-product` (about 92 mutants, a few minutes of builds per preset), as the tools' validation and for their present users. All nine of the newer family: more validation, but work no present unit needs, against C12 | The two |
| O4 | The verdict rules (section 6.1) | Counting a `static_assert` in the library's own checks as a kill credits the V4 checks at generation, which is what they are for; counting only test failures would call such mutants unjudged. `warning` and `invalid` not counted follows the existing records | As in section 6.1 |
| O5 | Where the flags come from | From the build trees (`compile_commands.json` and the compile-fail tests' command): cannot drift from the presets, but needs configured trees and makes old and new cost records comparable only by a same-session run. From lists in each spec: as the old harnesses, but drifts silently | From the build trees |
| O6 | `diagnostics/catalogue/scan.py` | Promote it to `tools/` here, with its fixtures under CTest; or leave it for WU14, whose compile-fail catalogue is its user | WU14, as its first step |

## 16. Agent cost estimate

| Role | Sessions | Tool calls (about) |
| --- | --- | --- |
| Planner (this plan) | 1 | 50 |
| Test author (doctest-expert judgement), fixtures and test scripts for both tools | 1 | 70 |
| Implementer (build-expert and cpp-expert judgement), parts A to G | 1 or 2 | 200 |
| Blind reviewer, with its own mutants | 1 | 80 |
| Delta review, if needed | 1 | 30 |
| Coordinator: claim, cherry-picks, merge | — | 30 |

Machine time, from the run log of the recorded runs: the rga3 mutants took about 3 minutes per build preset on `feature/presets-rga3`, the product mutants about 1 minute per build preset and mode in the logician's rerun, the rga3 cost harness about 25 seconds of compiling, the WU16 diagnostics about 14 seconds; with baselines, the tools' tests and mutants, and P9's merge checks, about 1.5 hours in all. Token counts are not visible to the agents; the outcome records tool calls in `agent-cost.tsv`.

## 17. Out of scope, and notes for later units

- The evaluation-counter snippet stays for WU17 (the owner's ruling); this unit does not touch it.
- Mutation sets for the bits and blade-set tests (handoff item 10), the lost tripwire on debug Clang presets, and the CMake comment's wording: a later test-quality unit, which writes specs for `tools/mutate`.
- The `0021-transform` spec: by the core unit of decision 0202 items 2 and 3, with the choice of occurrences it needs.
- WU17's P2 grid, run-time benchmarks, and growth units: `tools/compiles cost` takes their sources as files, and WU17 decides whether to extend it.
- The bits snippet stays in `docs/agent/snippets/` until the bits tests have a spec; then its index row says superseded.

## 18. Outcome

Written 2026-11-08 by the implementer and coordinator (Claude, agent, Opus 5.5), on `feature/evidence-tools`. **State: parts A to G done; P9's checks from new empty trees passed at `f6392cf`; the blind review (section 13) not yet started; the branch is not merged.** Next step: the blind review, launched by the session's coordinator, then fixes with a delta review, then the merge. *Corrected 2026-11-10: that was the state when this paragraph was written. The blind review and its delta review ran (`records/reviews/2026-11-08-evidence-tools-review.md`, `records/reviews/2026-11-08-evidence-tools-delta-review.md`), and the branch merged into `develop` as `c829b02` on 2026-11-08. The sections below record the review's resolutions.*

### Departures, skipped checks, and ambiguities (C20)

1. **Criterion 9 is not met as worded, in two ways, each traced to its cause.** (a) In normal mode, 37 of the 39 shared verdicts equal `results-b142837-*`; `generator_scale_dropped` and `range_widest_shifted` are `unjudged`, because their only build failure is a warning made an error, which the old driver counted as a kill and the ruled verdict rules do not. (b) With the fourth run-time edit there is no leak at line 856, but four mutants still leak at the type-property `static_assert`s (lines 515 to 518), which no run-time check can replace; the criterion assumed line 856 was the only one left on. Owner items 1 and 2.
2. **Criterion 11 is not met for two of 16 pairs**: GCC's two includers, where the tool is 0.11% and 0.20% lower. The listed flag difference (the old harness's extra `-I test -isystem include/ext`) accounts for 1.35 M of the 2.81 M gap of `rga3_only` by a probe, the rest within the probe's range; for `core_only` the same cause is inferred, not probed (`records/performance/rga3-cost/rerun-2026-11-08.md`).
3. **A verdict rule read beyond its words** (decision 0214 item 5, as corrected): a call to a function that is not constexpr, how `GALOOT_ASSERT` stops constant evaluation, counts as class `assert` like a failed static assertion. Without it GCC called `product_magnitude_x` unjudged in run-time mode where Clang called it killed. Owner item 3.
4. **Criterion 12, the 0024 tables**: the rerun wrote tables byte for byte equal to the recorded ones, so no new files were saved beside them and nothing needed restoring.
5. **The tools are larger than the plan's "near 300 lines"**: `tools/mutate` 583 lines (a 64-line usage docstring), `tools/compiles` 389.
6. **Not tested** (so no mutant of them): restoring files on SIGINT and SIGTERM, unknown spec keys, the `+dirty` revision, a missing build tree, an unknown revision, a glob matching nothing, `diagnostics --before`, and `--check --runtime`.
7. The revision rule leaves the run log out of `+dirty` (decision 0214 item 7), since every timed run appends to it. The diagnostics commit's message `2ed4d0d` names its revision as "1e3e..."; the revision was `3dce33f`.
8. Run-log rows of the test author's hand runs came in with the cherry-picked test commits.

### Commits (W9)

Records and scripts of records: `e7a04df` (part A, the repath), `bef6652`, `e843c75` (the two specs and `compare_ports.py`), `6a34947` (criterion 11 stated before the runs), `672b6fe` (decision 0214, `tools.toml`, the snippet index), `b5e4d31`, `1fb1511`, `3dce33f`, `2ed4d0d`, `ba98d0c`, `9322d1e`, `3b2869c`, and this outcome. Tests, by the test author, cherry-picked with `-x` from `feature/evidence-tools--tests` (decision 0185): `ce3b16f` (tools.mutate, fails until `0f53ed6`), `fefcd32` (tools.compiles, fails until `6514dad`), `499f177` (restore between files, passes), `49e6110` (a library guard, fails on gcc-debug until `61f0d98`). Tools: `0f53ed6` (`tools/mutate`), `6514dad` (`tools/compiles`), `1f7b971` (the run log not dirty), `61f0d98` (the guard classed as a kill). Merge of `develop` (plan 0027's unit, which shares `test/CMakeLists.txt` and `runs.tsv`): `f6392cf`. Nothing under `include/`, `cmake/`, or the build presets changed; the root `CMakeLists.txt` did not change.

### Criteria (section 4)

1. Refusals: each by a check of `tools.mutate` and a killed mutant (`records/mutations/0026-tools/`). Pass.
2. Stale binaries: programs deleted before each build, CTest given only existing programs (`--tests-from-file`); tested; in the ports, every test whose program did not build is listed as not run. Pass.
3. Classification, verdicts, exit: as decision 0214 item 5; tested. Pass.
4. Run-time mode and leaks: tested; the leak at line 856 found on the five named mutants. Pass.
5. Restore: tested, after every case and between mutants of two files; SIGINT and SIGTERM not tested (departure 6).
6. Outputs: `out/<stem>-<preset>[-runtime].tsv`, partial reruns tested. Pass.
7. `--check`: tested. Pass.
8. The rga3 port: byte comparison passes for all 50 (`python3 -I docs/agent/records/mutations/0026-tools/compare_ports.py`, at `e843c75` and after); at `1f7b971` on gcc-debug and clang-debug, 43 killed and 7 equivalent, every verdict as recorded; the nine differences in failed tests explained mutant by mutant (`0026-tools/README.md`). Pass.
9. The blade_product port: see departure 1; the leak, the equivalents, and the 37 other verdicts as the criterion says. Not met as worded.
10. Diagnostics, at `3dce33f`: one error line per compile-fail source and compiler, and the same first error line, line count, and longest line as the repathed script in all 18 rows (`records/diagnostics/wu16/sizes-tools-2026-11-08.tsv`, `sizes-rerun-2026-11-08.tsv`); `rotation_assert_fail.cpp`, added after the recorded run, compiles and is reported. Pass.
11. Cost: departure 2.
12. The repath: the three scripts name the preset's paths and ran to completion: 0024's mutants (11 of 11 killed on both presets, by the tests its README lists, 19 s and 16 s), the cost harness (criterion 11), the diagnostics (criterion 10). Pass.

### Verification and validation (V10)

- Verification: `tools.mutate` and `tools.compiles` pass on gcc-debug and clang-debug (about 12 s and 2 s); the tools' 30 mutants are all killed at `9322d1e`. Expected values of the fixtures were derived by hand by the test author. The driver judging its own mutants is mitigated as section 14 says; the reviewer's independent mutants are still to come.
- Validation: criteria 8 and 10 pass; 9 and 11 are partly met, with each gap traced (departures 1 and 2).

### Merge checks

At `f6392cf` (the merge of `develop`), from new empty trees, each as `tools/timed workflow <preset> gate:all -- cmake --workflow --preset <preset>`: gcc-debug 496 of 496 (68 s), gcc-release 496 of 496 (76 s), gcc-asan 488 of 488 (174 s), clang-debug 496 of 496 (64 s), clang-release 496 of 496 (83 s), clang-asan 496 of 496 (158 s), lint 496 of 496 (194 s), clang-measure 488 of 488 (80 s); `format-check` on the lint tree, the trace gate, and `tools/runlog check` (one era, 1,727 kept rows) pass. About 17 minutes in all. The pool read 23 of 23 free before, and 32 of 23 after: a surplus of nine slots, which the pool's own check brings back to 23. Its cause is not traced (unexplained). No check was deferred.

### Run times and the pool

The rga3 mutants 9 min 10 s (both presets); the blade_product mutants about 3 min 50 s per spec and mode; the tools' mutants 6 min 1 s and 12 min 37 s; both cost routes 53 s. The pool had 23 of 23 slots free before and after each run, except 22 of 23 after the rga3 run, a slot held by another unit's gcc-asan build at that moment.

### Agent cost

In `records/agent-cost.tsv`: the test author, 242,234 tokens and 61 tool calls (1,161 s), then 262,830 tokens as reported for the resumed agent and 15 tool calls (231 s); the implementer about 190 tool calls, tokens not visible.

### Open items for the owner

1. **The two warning-only blade_product mutants.** Options: rewrite them so that the build stays warning-free (for example `static_cast<void>(scale)`, as 0024's `division_omitted` was rewritten), so the tests judge them; or keep them as recorded, unjudged. Rewriting costs two lines in the spec and changes the port away from the old driver's text. Recommendation: rewrite them in the test-quality unit that writes the bits and blade-set specs, and record their verdicts there.
2. **Leaks at type-property `static_assert`s.** Options: leave the rule as it is (any failed static assertion in a run-time file is a leak, so these four always show); or let a mutant declare that only a compile-time check can kill it, which the tool then does not report as a leak. The first keeps the tool simple but makes the blade_product run-time mode exit 1 on every run; the second adds a key to the spec format. Recommendation: the second, a per-mutant `compile_time_only = true`, but only when a second spec needs it (C12); until then, the record lists the four.
3. **The guard rule of departure 3.** Options: keep it (GCC and Clang agree; the library's compile-time checks count as the owner ruled for `static_assert`); or revert to the plan's words, under which GCC calls such mutants unjudged and Clang killed. Recommendation: keep it.
4. **The diagnostics script `catalogue/scan.py`** stays for WU14, as ruled. No action.

### After the blind review (2026-11-08)

The review (`records/reviews/2026-11-08-evidence-tools-review.md`, Opus 5.5, cherry-picked as `af9bd41`) approved with non-blocking findings m1 to m4 and recorded F1, the job pool's surplus-token race.

- **Owner rulings on the open items**, relayed by the coordinator: (1) the two warning-only blade_product mutants stay `unjudged` now and are rewritten in the later test-quality unit (the unit that writes the bits and blade-set specs), which lists them; (2) the type-property leaks at lines 515 to 518 stay under the current rule, and `compile_time_only` is added only when a second spec needs it; (3) the guard rule of decision 0214 item 5 stays.
- **F1** is outside this unit: a separate unit is fixing `tools/jobpool`. The "32 of 23" of the first P9 was that race (the review traced it to the start of the clang-release workflow). It is not this unit's defect.
- **m1**, five test gaps, by the test author: `70bdd01` (rows in spec order when a later run adds an earlier mutant; a dirty run-time edit file refused) and `7199d2d` (the read-back mode, the summary's mean and range from a counting `perf`, and the efficiency-core count in shared mode). Both pass against the tools as they were.
- **m2 to m4**, tests first, each failing until `76c9e97`: `65632d0` (SIGTERM: files restored and the programs rebuilt), `2ca060b` (killers sorted), `c1e5451` (a CTest run failing without naming a test leaves the mutant unjudged, with the problem `CTest exited <n> without naming a failed test`). The fix `76c9e97`: a run that stops early rebuilds the targets of each preset it started; killers are sorted sets; CTest's status is read. These three test commits fail in a row before the one code commit they judge.
- **The tools' mutants** follow the fix (`1775c22`), with three more for m2 to m4: see the rerun below.
- The note on `2ed4d0d`'s message (departure 7) stands; history is not rewritten.
- `tools.mutate` now takes 16 to 19 s here, and up to 55 s on a loaded machine by the test author's runs; it does not carry `slow`.

- **The tools' mutants rerun** at `1775c22` on gcc-debug: all 33 killed (the 30 earlier and the three of m2 to m4), 6 min 56 s, the pool at 23 of 23 free before and after (`records/mutations/0026-tools/out/tools-gcc-debug.tsv`).

- **Final merge checks** at `b78c38d` (the merge of `develop` with plan 0028's claim), from new empty trees, clean tree, 11 `gate:*` rows at that revision with no dirty mark: gcc-debug, gcc-release, clang-debug, clang-release, clang-asan, lint 496 of 496; gcc-asan, clang-measure 488 of 488; `format-check`, the trace gate, and `tools/runlog check` pass; about 17 minutes (72, 100, 221, 67, 87, 168, 203, 84 s). The pool read 22 of 23 free before (another unit's slot) and 23 of 23 after. The commit after `b78c38d` adds only these rows and this line.

### After the delta review (2026-11-08)

The delta review (`records/reviews/2026-11-08-evidence-tools-delta-review.md`, cherry-picked as `0048fd3`) approved with four nits:

- **W2, a past departure of this unit (labelled for the owner's ruling):** `65632d0`, `2ca060b`, and `c1e5451`, three test commits that each failed, came in a row before the one fix `76c9e97` they judge, where W2 allows a single failing commit before the code. The owner will rule on whether to widen the rule; WORKFLOW is not edited. The fixes below use one failing commit per fix.
- **n1:** a failed rebuild after an early stop is now named on stderr. Test `7243c04` (written by the implementer; it also tests SIGINT, n4), which failed before the fix `6598d9d`.
- **n3:** build failures, failed cases, and leaks are sorted like the killers. Test `42ba98a` (implementer), which failed only when Ninja finished out of order, then the fix `a482524`.
- **n2:** the usage text's exit-0 rule names every problem the code counts (`a482524`).
- **n4:** SIGINT is tested (`7243c04`); the record's README says which signals are tested.
- **The owner's ruling on timeouts:** a test that CTest names as timed out counts as killed, and a CTest run that fails without naming a test leaves the mutant unjudged; the usage text and the record's README say so (`a482524`).
- The tools' mutants: no anchor's text changed; the two whose anchors lie in the rewritten block, `mutate_no_rebuild_after_early_stop` and `mutate_exit_always_0`, were rerun and are killed.

- **Checks** at `8ff2042`, the merge of `develop` with plan 0028's unit, whose `test/CMakeLists.txt` change called for the full P9 rather than the reduced set: from new empty trees, clean tree, 11 `gate:*` rows at that revision with no dirty mark: gcc-debug, gcc-release, clang-debug, clang-release, clang-asan, lint 496 of 496; gcc-asan, clang-measure 488 of 488; `format-check`, the trace gate, and `tools/runlog check` pass; about 18 minutes (90, 98, 195, 88, 104, 180, 229, 105 s); the pool at 23 of 23 free before and after. The commit after `8ff2042` adds only these rows and this line.
