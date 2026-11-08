# 0214 Mutation specs, verdict rules, and where the evidence tools take their flags

- Status: recorded. The spec format (TOML), the two tools and their names, the two ports, the verdict rules, and the flag sources were ruled by the owner on 2026-11-08 (decision 0215 item 10: plan 0026's O1 to O5 as recommended). The details below that the plan left open are the implementer's choices, open to the blind review and to the owner (C18).
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of plan 0026
- Number: 0214 is the number plan 0026 reserved; it was free on every branch when recorded (0215 was already on `develop`).
- Bears on: `tools/mutate`, `tools/compiles`; plan 0026 sections 6.1, 6.2, O1 to O6; decision 0215 item 10; lessons 0060, 0061, 0063; the snippet index

## Context

Every unit since WU8 copied a mutation driver into its record (nine of one family, with the same five functions), every cost record a pinned before-and-after harness (seven), and every diagnostics record a sizing script (six). The copies differ in how they judge a build: the newer drivers run CTest after any build, so a program that failed to build passes from its previous build, and the run-time mode of the blade product driver left one `static_assert` on. The M4 snippets review recommended one general driver and one harness before WU14; the owner accepted it.

## Decision

1. **Spec format (O1): TOML, read with `tomllib`.** A mutation spec has `presets`, `[selection.<name>]` (`targets`, `labels`), optional `[[runtime]]` edits (`file`, `anchor`, `replacement`, `count`), and `[[mutant]]` tables (`name`, `selection`, `file`, `edits`, `expect`, `reason` for an equivalent one, optional `killers`). An edit replaces every occurrence of its anchor, which must occur exactly `count` times (default 1); a mutant's edits apply in order, each to the text the previous one left. Anchors that the old drivers computed with helpers are written out in full; a removed declaration is its whole text replaced by nothing. Unknown keys are refused. A cost spec has `compilers`, `flags_from`, and `[[unit]]` tables (`name`, `source` or `text`, `trees`, `kind`).
2. **Two tools (O2).** `tools/mutate <spec> [--runtime] [--check] [--preset <p>]... [<mutant>...]`; `tools/compiles cost ...` and `tools/compiles diagnostics ...`. `tools/measure` is unchanged.
3. **Refusals (exit 2), all before the first build:** an anchor whose count is not the declared one, a mutant that changes nothing, a file to edit that is untracked or differs from `HEAD`, a missing build tree, a selection that selects no test, a selected test that runs a program of the build tree that no target of its selection builds, and a baseline that does not build and pass. The program of a target is read from Ninja (`-t query`, executable link rules from `-t targets all`), and a test's command from CTest's JSON listing or, while the program does not exist yet (CTest then leaves the command out), from the `add_test` lines of the tree's `CTestTestfile.cmake`.
4. **The stale-binary rule.** Before each mutant's build the selection's programs are deleted; CTest gets only the selected tests whose programs exist after the build, by `--tests-from-file`; the rest are listed in the row as not run. Tests that run no program of the tree (compile-fail tests run by the compiler or by `cmake -P`) always run, so a compile-fail test now runs after a failed build, which the old blade product driver skipped.
5. **Verdict rules (O4).** Each failed object is classified by its first line holding `error:`: `warning` if it carries `[-Werror`; else `assert` if the message is a failed static assertion (the text holds `static assertion` or `static_assert`, which covers GCC's and Clang's spellings and the non-constant condition of one), wherever it lies, so a library check reached from a test counts; else `test` if it lies in a file under `test/`; else `invalid`. `assert` and `test` objects and failed CTest tests are kills; `warning` and `invalid` are not. A mutant is `killed` if anything killed it; else `unjudged` if an object failed; else `equivalent` if declared so; else `survived`. A declared-equivalent mutant that is killed, a declared killer that is not among the killers, a survivor, an unjudged mutant, and a run-time leak are each a problem, which makes the exit status 1.
6. **Run-time mode.** The spec's run-time edits apply to their files for the whole run of each build preset, the baseline included, and are undone at its end. A failed static assertion located in one of those files is a leak, reported by file and line.
7. **Revision.** Each row records `git rev-parse --short=12 HEAD`, with `+dirty` when tracked files other than the edited ones differ from `HEAD`; the spec's `out/` directory and the run log, which every timed run appends to, are left out of that test.
8. **Outputs.** `out/<spec stem>-<preset>[-runtime].tsv` beside the spec, one row per mutant in spec order, a rerun of some mutants replacing their rows only; a summary on stdout. No file an old driver wrote is written.
9. **Flag sources (O5).** Both subcommands of `tools/compiles` read flags from the build trees `build/<compiler>-<kind>`: a source in the tree's `compile_commands.json` with its own command; any other source with the compile command of a compile-fail test from CTest's listing (the source's own test, else the test named by `flags_from` or `--flags-from`), whose compile command follows its second `--`. The launcher prefix `tools/measure tu ... --`, `-o <object>`, `-c`, and the source are dropped; in a before compile, each `-I` or `-isystem` path under the checkout is moved into the extracted tree. Both write the command each compile ran, with paths relative to the root.
10. **Cost rows** go to `<out-dir>/runs.tsv`, a campaign log with one writer, never to `docs/agent/records/performance/runs.tsv`; each row is read back and must be a row of the expected selection in the expected mode. Unavailable counters stay `unavailable` in the summary. Diagnostics compiles go to the default run log, as the old scripts' did.
11. **`diagnostics/catalogue/scan.py` (O6)** stays where it is until WU14.

## Options weighed

- For item 1: a Python data module (helpers stay, but a spec becomes code), JSON (no multi-line strings). TOML was the owner's choice.
- For item 3's program lookup: reading `test/CMakeLists.txt` (would need to evaluate CMake), building the baseline first and listing after (a refusal would then come after a build). The test files are what CTest itself reads.
- For item 4: running CTest after a failed build on every test (the newer drivers: a stale program passes), or skipping CTest after a failed build (the blade product driver: the compile-fail tests and the other programs' tests never run). Passing only the tests whose programs exist keeps both.
- For item 7: no revision in the row (the old drivers), or `+dirty` on any change, which every real run would show because of the run log.

## Consequences

- A later unit writes a spec of its mutants and runs `tools/mutate`; a reviewer writes its own spec. The old drivers stay as records of their runs, at the revisions their records name.
- The tools' sizes are larger than the plan's estimate of about 300 lines each (plan 0026's outcome gives them).
- Choosing some occurrences of an anchor, running build presets in parallel, and configuring build trees are not built; the unit that needs one adds it.
