# W5 delta review: feature/recorder, 721c287 to fbd36b0 (plan 0003, fix pass)

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed after use; its three run rows are in the session scratchpad as `recb/review-recb-runs.tsv`).
- Reviewed: `git diff 721c287 fbd36b0`, head `fbd36b04bf4d`.
- Verdict: merge after fixes. B1 (the conflict on decision 0042) is resolved at the merge by keeping the branch's 22-line version; N1 to N3 are cheap and were deferred to the cleanup unit.
- Not run: gcc-release, gcc-asan, clang-debug, clang-release, the measurement presets (the author's P9 run at d9c5059 stands under P10; only records changed since); `tools/jobpool` (unchanged in this delta); a real multiplexed shared run (a stand-in was used); a machine without hybrid perf.

## Checks

- Task-clock rule on real perf 7.3.1 (P-cores 0 to 15, E-cores 16 to 31): pinned to CPU 2 the mode is pinned and `inst_e_u` is unavailable; on CPU 20 and on E-cores the rule writes `inst_p_u` as 0; unpinned and on P-cores `inst_e_u` is 0. Three unpinned multi-process compiles sum to about 1776M instructions, matching the compile pinned to P-core 3. Raw perf for `taskset -c 20 g++` shows the E-core running time equal to task-clock's (422825304 ns).
- Stand-in perf cases: below task-clock gives unavailable; equal gives 0; an unsuffixed `task-clock` gives 0; both types not counted with task-clock above 0 gives both unavailable. The event list is correct in both modes.
- The reader refuses an older shorter header, a repeated header, a blank line, a trailing blank line, and an empty file, each with exit 1 naming file and line; `read` exits 0 on the real `runs.tsv` and `compile.tsv`.
- Attribution from outside any repository, with no git on PATH, and from a copy of the recorder outside any repository; the build-tree argument for `ctest -C Debug --test-dir`, `ctest -C Debug`, `cmake -C init.cmake`, `ninja -C`, `ninja -Cdir`, `make -C dir`, `cmake --build`, `cmake -Bdir`, `env FOO=1 ninja -C`, and an absolute ninja path.
- Fifteen mutants of the head test: killed header refusal, `--no-optional-locks`, `-MMD`, always writing 0, git without `-C`, the old tree rule, exact header compare, no empty-file check, no blank check, exact task-clock name; survived dropping task-clock from the perf command (N1), git returning "" on failure (N3), ignoring a failed `git status` (N3), and dropping the own-time subtraction in `pinned_other_s` (declared untested).
- The head test against the 1c22b70 recorder fails 11 checks plus one `FileNotFoundError`, matching the plan's correction of the commit message.
- No provision or agent citation in added or rewritten lines. Clean builds: gcc-debug 2.94 s, clang-asan 4.40 s, lint 2.88 s; 22 of 22 tests each; each clean tree's `compile.tsv` has a header and six rows of 29 fields. The trace gate and the marker check pass.

## Findings

- B1 (blocking, resolved at the merge). Develop deleted the empty `0042` decision file, so the merge reported a modify/delete conflict; keep the branch's 22-line version. The message of develop's deleting commit says "correct a cost row" but the commit only removes the file.
- N1. The test cannot see whether perf is asked for task-clock (the stand-in perf ignores `-e`); a mutant that never requests it passes and would write unavailable for every `<not counted>` core type on real perf. Have the stand-in record its `-e` list and check for `task-clock` in shared mode.
- N2. `build_tree` resets the program on every argument whose basename is ninja, make, cmake, or ctest, including label and regex values: `ctest -L make -C Debug` and `ctest -R ninja -C Debug --test-dir build/a` record unavailable. Set the program only once.
- N3. Only "git missing" is tested, not git failing: two mutants survive (returning "" makes the branch `detached`; ignoring a failed `git status`).
- N4. The recorder change and its test are again one commit (W9). A split was possible without a failing commit: the code plus the changed old expectation, then the new cases shown failing against the older recorder. It has now happened twice; the owner may rule on how W2 and W9 apply.
- N5. A single blank line reads as an empty header with exit 0; with both core types not counted and task-clock's running time 0, both are written 0 (likely unreachable).
- N6. Nits: the `cases` dict in `counters_per_core_type` unpacks by position; two docstring paragraphs and `include_scan.py:33` were left unreflowed.
- N7. Observation: shared rows written between 518823d and a232d65 keep their unchecked 0s; they can be found by revision on the branch.
