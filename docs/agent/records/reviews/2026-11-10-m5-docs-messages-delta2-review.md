# Second delta review: the README examples' anonymous namespaces

- Reviewer: blind-reviewer role (a general-purpose agent briefed with `.claude/agents/blind-reviewer.md`), Claude Sonnet 5.5, delta reviewer, 2026-11-10 UTC. The author of the fix was Claude Opus 5.5 (hci-expert and implementer); this reviewer did not see the author's view of the fix.
- Range: `59eeb01^..59eeb01` on `feature/m5-docs-messages` (one commit, `README.md` only). The commit after it, `fe4fb3b`, is run rows only and was read, not reviewed.
- The brief carried no verdict. It gave the cause (clang-tidy `misc-use-internal-linkage` in the lint preset) and the fix's intent.
- Reviewer's tree: worktree `.claude/worktrees/review-m5-docs-messages-delta2` at `fe4fb3b`, each step through `tools/timed`; the run rows are in `docs/agent/records/performance/runs.tsv` on this branch.

## Outcome

Approve with one non-blocking finding. The fix does only what it says: two `namespace {` / `} // namespace` pairs in two README blocks, eight added lines, nothing else changed in `README.md`. The five extracted examples build, run, and print what their `// prints:` lines say on gcc-debug, clang-debug, and lint (clang-tidy), and the lint build fails without the fix and passes with it.

## What was checked and how

- **Diff.** `git show 59eeb01` and `git diff 59eeb01~1 fe4fb3b` read in full: `README.md` gains eight lines (`namespace {` and a blank line after the `namespace core = ...;` alias in each of the two blocks; a blank line and `} // namespace` before `int main`). No line is removed or reworded. The other file in the range is `runs.tsv`. The alias stays outside the anonymous namespace in both blocks, `main` is outside it, and no `static_assert` moved out of it.
- **Build and run.** `cmake --preset` for gcc-debug, clang-debug, and lint; `cmake --build --preset <p> --target rga3_readme_example_1 ... rga3_readme_example_5`; `ctest --preset <p> -L rga3.readme --output-on-failure`. All three presets: 6 of 6 tests passed (`rga3.readme_has_example` and the five examples). The lint build compiles each example under clang-tidy (the compile command is `cmake -E __run_co_compile --tidy=clang-tidy;--checks=-bugprone-exception-escape ...`, read from `ninja -t commands`) with `-Werror`, and it succeeded.
- **The prints lines.** The runner compares the program's output with every `// prints:` line exactly, so passing the test means the three expected lines of the second new example match. I also ran `build/lint/test/rga3_readme_example_5` by hand and read `{e1: 1, e2: 2, e3: 3, e4: 6.5, e5: 7.5}`, `{e1: 1, e2: 2, e3: 3, e4: 1, e5: 7}`, and `-12.5 -12.5`, the three `// prints:` texts. The `static_assert`s of the first new example are compile-time, and compiling it on both compilers and under clang-tidy shows they hold.
- **Negative control (the fix does cure the failure).** I extracted the examples from the README at `59eeb01~1` with `tools/readme_examples.py` into a scratch directory and ran the same clang-tidy options on them, from a path under `build/lint` so that the repository's `.clang-tidy` applies. The old example 4 gives three `misc-use-internal-linkage` errors (`witt`, `r110`, `witt_to_r110`) and the old example 5 gives four (`cga3`, `r410`, `cga3_to_r410`, and the function `point`); the new ones give none. So the lint failure of the P9 matrix is reproduced and the fix removes it, for each name the check named. The scratch copy under `build/lint` was removed.
- **Format.** `clang-format --dry-run --Werror` on the two extracted new examples: no output. The README text is not otherwise formatted by a tool.
- **Idiom (usability checklist, "every example compiles and is the idiom you want copied").** The examples compile and are the idiom for a program of one file: a tag is a struct with a `value`, and an anonymous namespace is the standard way to give such a struct internal linkage, which is what the lint preset demands of the library's own tests and what a user who runs clang-tidy would be told. Operators mixed with `^` or `&` are fully parenthesized (the only `^` and `&` in these blocks are in comments, and `(f * g) + (g * f)` and `((x * x) + ...)` carry their parentheses). The first three blocks of the README already have only `constexpr` variables at namespace scope, which have internal linkage, so they are unaffected and consistent.
- **Whether the prose needs a word.** For a reader who copies the example into one file, no: the namespace is a normal C++ construct and the examples' purpose is the change of basis. For a reader who copies the tag into a header, one sentence would help; see finding 1.
- **Nothing else changed.** `git diff fe4fb3b~2 fe4fb3b --stat` lists only `README.md` and `runs.tsv`; the `README.md` diff has exactly the eight added lines above.
- **Process.** The commit message says what and why, names the failing matrix revision (`fd9e324`) and the preset, says what was run, and names the agent and model (W7). A change of README text only needs no decision record. The commit has one change, so there is no test/implementation ordering to check (W9).

## What was not checked

- The full P9 matrix, release, asan, and every lint target other than the five examples; the convention trace gate (the range touches no `test/`, `include/`, or convention text).
- Compile-time cost of the examples (no probe; two namespace braces do no template work). No build-cost record exists for the fix.
- The first three README blocks under clang-tidy beyond what the lint test run above covers (they built and passed, so they are unaffected by this change).
- Rendering of the README on a Markdown host; the fences are unchanged.
- Whether a header-shared tag in an anonymous namespace actually warns on a given compiler (finding 1 states the behavior from the language rules; not built here).

## Findings, most severe first

1. **`README.md` lines 225 and 291 (low, plausible, usability; does not block).** The prose does not say that the anonymous namespace is there for a program of one file. A user who copies the tags (`witt`, `cga3`) into a header shared by several translation units gets, from the anonymous namespace, a different algebra type in each unit, so a function declared in that header with an `mv<witt, ...>` parameter has internal linkage per unit (GCC warns with `-Wsubobject-linkage` when a class in a header has a member of such a type, and values cannot cross units). In that setting the tag wants external linkage, which is what the lint rule would then flag for a program of one file only. Failure scenario: a two-file program whose shared header opens `namespace {`. Suggested fix, at the user's discretion: one sentence before either block, such as "The tags and values are in an anonymous namespace because the program is one file; in a header shared by several files, declare a tag in a named namespace instead." The check that forced the wrapper is the author's lint preset, which a user may not run; the plain idiom without it is still correct for one file. Not a defect of the fix, which is minimal.
2. **Observation (not a defect).** The brief's claim that the examples "print what their `// prints:` lines say" is checked by the runner and by hand; the first new example prints nothing and its checks are all `static_assert`s, which the compilation shows.

## Interface changes the review suggests that would break later users

None. The fix changes no public interface.
