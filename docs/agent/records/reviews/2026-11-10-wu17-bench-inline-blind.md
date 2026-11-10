# Blind review of plan 0038 part D's inlining change (reference kernels and form lambdas always inlined)

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10. I share no working context with the author or with the part D reviewers. I did not edit the change under review.
- **Range:** `feature/cost-evidence` at `18849b5` to `feature/cost-evidence--bench--inline` at `16c88af`, five commits:
  - `5d2fb38` and `f6b8276`, the tests, cherry-picked with `-x` from `feature/cost-evidence--bench--inline--tests` (`b4b9e06`, `1ac3e83`);
  - `0c429d5`, the code;
  - `b480bd3`, the assembly record and two mutants;
  - `16c88af`, the run rows.
- **Where I worked:** the worktree `.claude/worktrees/wu17-bench-inline-review` on `records/wu17-bench-inline-review`, branched from `16c88af`. Build trees `build/{gcc,clang}-{debug,measure}` in that worktree. Scratch assembly and probes went to the session's scratch directory, not the repository.
- **Statement of intent:** decision 0237 item 2: make the straight-line reference kernels inline in the timed loops of the seven benchmarks on both compilers, so that E2 compares the library against inline code, with no change under `include/` and no other change to what is timed. Also plan 0038 section 4 items 4 and 6, and decisions 0014, 0185, 0233 and 0236.
- **Departures the author listed:** the same author wrote the tests and the code; the selection was narrower than P8 and P9 ask (release, asan and the full suite not run; build cost not recorded); the tests worktree's `runs.tsv` was left dirty. I was given no verdict.

## Outcome

**Merge after two records-level fixes (F1, F2). The code and tests are sound; no finding blocks on substance.**

- **The approach is sound and fair to the library.**
  - Marking the library form's lambda changes no instruction at `-O2` on either compiler. I compiled all seven programs with that one mark removed: the assembly matches the tip instruction for instruction. On GCC, three product programs differ only in the order of two functions in the file.
  - The library's out-of-line functions on Clang (`transform`, `run_product`) have identical instructions before and after.
  - On GCC, the library's code is inlined in the timed functions. Each such function's multiplication, addition, subtraction and negation counts after the change are exactly twice those before: the inlined reference has the library's instruction mix.
- **The inlined reference does get different code on Clang (F1).** Clang now vectorizes the reference's throughput loop across iterations. The library's form there is still one call per operation. The record does not say this, and E2 needs it.
- **The symbol-table test fails for the right reason before the change and passes after it.** The cases I could construct do not make it pass vacuously: a stripped binary, LTO on both compilers, and both `nm` flavors (F3 states the one assumption it rests on).
- **The rest holds:**
  - The regenerate-and-diff test still guards the generator.
  - The smoke tests are unchanged (W9).
  - The README's call table reproduces exactly.
  - Both mutants are honest.
  - Nothing under `include/` changes.
  - No code or comment cites agent work (S11).
  - No British spellings.

## Checks run

All builds, tests, and compiles went through `tools/timed` with the pool exported; their rows are in this branch's `runs.tsv`. Pool status: before, running with 23 of 23 slots free (load 1.2); after, 23 of 23 free (load 1.2). I neither started nor stopped the pool. I made no timing measurement, took no lock, did not touch `master`, changed no processor setting, and fetched nothing from the web.

| Check | Result |
| --- | --- |
| `git diff --stat 18849b5 16c88af -- include/` | empty |
| Cherry-picks: `git show` of `b4b9e06` against `5d2fb38`, and of `1ac3e83` against `f6b8276`, restricted to `test/` | identical diffs; both carry `-x` lines |
| `test/bench/reference_kernels_test.cpp`, and the `rga3.benchmark_<op>` commands in `test/bench/CMakeLists.txt` | unchanged; the CMake change only adds the `_inline` tests |
| Configure, then build the 7 programs and `rga3_reference_kernels_test`, in gcc-debug, clang-debug, gcc-measure, and clang-measure | exit 0 in all four |
| `ctest -L '^(rga3\.(reference_kernels\|benchmarks)\|reference\.kernels)$'` at the tip | 16 of 16 pass in each of the four presets |
| The same selection with `test/bench` and `test/reference` checked out at `f6b8276` (tests, no code), programs rebuilt | gcc-debug and clang-debug: the 7 `_inline` tests fail, each on both `reference_kernels::` and ` t main::`. gcc-measure and clang-measure: the 4 of the transforms and compose fail, on `reference_kernels::` only. The other 7 tests pass. Tree restored afterward |
| `nm -C` (GNU, from the gcc presets' `CMAKE_NM`) and `llvm-nm -C` (from the clang presets'), all 28 programs at the tip | each defines `T main` and no symbol that holds `reference_kernels::` or starts with `main::` |
| `run.sh` of the record at the tip and with the sources at `18849b5`, then `calls.py` | every line of the README's table reproduces; the flags match the gcc-measure and clang-measure compile commands in `compile_commands.json` |
| Variants of all seven programs at `-O2` with one mark removed: the library lambda's, the reference lambda's, or both | see F1 and the fairness section below |
| Kernel mutant (transform_line's kernel without its mark), compiled at `-O0` and `-O2` on both compilers | defines the weak symbol `reference_kernels::transform_line<double>` in all four, so the test would kill it |
| Vacuity probes on `transform_line` and `compose_motors` (scratch compiles): asan flags of the asan presets with `-D_GLIBCXX_ASSERTIONS` at `-O1`; `-O3`; `-O2 -flto` (lld for Clang, the default linker for GCC); `strip` | at the tip, asan, `-O3` and LTO each define `T main` and nothing forbidden. With the reference lambda unmarked, LTO on both compilers still shows the lambda. A stripped binary lacks `T main`, so the test would fail, not pass |
| `python3 -I test/reference/reference_kernels.py --check test/bench/reference_kernels.hpp` | exit 0, matches |
| `--check` against a copy of the header with transform_point's mark removed | exit 1, with a diff naming the line |
| `ctest -R '^tests\.(reference_markers\|golden_facts\|citations\|messages_pinned)$'`, gcc-debug and clang-debug | 4 of 4 pass in both |
| `clang-format --dry-run -Werror` on the 7 programs and `harness.hpp` | exit 0 |
| Grep of added lines under `test/` and in `run.sh`, `calls.py`, and `bench.toml` for agent citations and British spellings | none; the README cites plan 0038 and decision 0237, which a record may |
| Lines over 80 columns in the added lines | two, both in `calls.py` (F5) |
| The tests worktree's uncommitted `runs.tsv` rows against the rows `16c88af` copied | identical, 10 rows |
| `g++ --version`, `clang++ --version` | GCC 16.2.1, Clang 23.1.4, as the README says |
| After restoring the tip: rebuild the 7 programs, then `ctest -L '^rga3\.benchmarks$'`, in the four presets | 14 of 14 pass in each |
| `tools/runlog check` after adding this review's rows | exit 0: one era matches the history; kept rows present once |

### Not checked

- **Presets not run:** gcc-release, clang-release, gcc-asan, clang-asan, gcc-measure-o3, clang-measure-o3, and lint. The brief limits me to gcc-debug, clang-debug, and the measure presets. My scratch probes cover asan and `-O3` for two of the seven programs only (F2). The author's rows show `lint` on the bench targets exiting 0, at `f6b8276-dirty`.
- **Not run:** the full suite, `format-check`, and the trace gate.
- **Mutation tool:** I did not rerun `tools/mutate` on `bench.toml`. I compiled the two mutants' effects by hand instead (the kernel mutant directly; the lambda mutant as the "reference lambda unmarked" variant).
- **Clang's vectorized loops:** I did not trace them operation by operation. The reading in F1 rests on instruction counts per loop.
- **Build cost:** I made no timing and recorded no build cost (F6).

## Fairness of the marks to both forms

The question was whether marking changes codegen, or what Galoot's own code is measured on, beyond removing a wrapper call. I compiled four states of each program at the measure flags (`-O2 -ffp-contract=off`, checking off): before (`18849b5`), the tip, the tip with the library lambda unmarked, and the tip with the reference lambda unmarked.

| Question | Finding |
| --- | --- |
| Does the library lambda's mark change anything at `-O2`? | No. With it removed, `calls.py` gives the tip's counts for all 14 files. Every function's instructions match the tip, labels aside. In GCC's three product programs, the two `bench::run<...>.isra.0` functions trade places in the file, with the same bodies |
| What does the library lambda's mark do, then? | At `-O0` it inlines the lambda, so that `rga3.benchmark_<op>_inline` passes in gcc-debug and clang-debug. Nothing is measured there |
| Is the reference lambda's mark needed? | Yes. Without it, GCC keeps the reference lambda out of line (two calls) in the three transforms and compose, and Clang keeps it in `transform_line`. This is as the README's intermediate step says |
| Is the library's own code the same before and after? | On Clang, the out-of-line `transform` (transform_point, transform_line) and `run_product` (transform_line, compose) have identical instructions. On GCC, the library is inlined in the timed functions, so the functions themselves differ. Their operation counts after are exactly twice those before (transform_point: 80 `mulsd` and 8 `mulpd` before, 160 and 16 after; transform_line: 144 and 288; transform_plane: 96 and 192; compose: 0 before, the library being a call, and 96 after, the reference's 48 twice). That is consistent with the library part being unchanged; I inferred it from counts, since register allocation changes the text |
| Does the reference get the same code inline as out of line? | On GCC, yes in kind: the inlined reference has the library's instruction mix. On Clang, no (F1) |

## Findings, most severe first

### F1. On Clang, inlining lets the compiler vectorize the reference's throughput loop across iterations; the record calls the change only a call removed (medium, records and E2's reading)

- **What the assembly shows (Clang 23, `-O2`):** in the timed function, each program has these loops:
  - The reference throughput loop holds only packed multiplications: transform_point 48 `mulpd` (96 lanes, twice the kernel's 48 multiplications), transform_line 72 `mulpd` (144 lanes), compose 48 `mulpd` (96 lanes). Inferred from the counts: two operations per iteration, one in each lane.
  - The single-operation loops are partly packed: transform_point 40 `mulsd` and 4 `mulpd`; compose 20 `mulsd` and 14 `mulpd`, or 28 and 10.
  - Before the change, the out-of-line kernels were scalar for the transforms (48, 72, 48 `mulsd`, no `mulpd`) and packed within one call for compose (24 `mulpd`).
  - The library's loops are one call per operation, before and after.
  - The results do not change: no reassociation is allowed, and `rga3.benchmark_<op>` passes in clang-measure.
- **Why it matters:**
  - On Clang, a throughput difference in E2's rerun measures more than the cost of a call. It also includes the vectorization across iterations that the call prevents.
  - Decision 0237 item 3 has the rerun choose between recording the departure and a core inlining unit. A reader who takes the gap for call overhead would misjudge what such a unit buys. Inferred: an inlined library `transform` could get the same vectorization, so the gap is closer to what inlining buys than to the call's cost.
  - On GCC, the inlined reference matches the library's inlined form, so its comparison is like for like.
- **What the record says:** `part-d-inline/README.md` gives call counts only. It says "Every reference-kernel call is gone", which is true. This is the comparison decision 0237 item 2 asks for, so it is not a defect of the code.
- **Recommendation:**
  - Add a paragraph to the README with the per-loop counts above, and the command that gives them (`calls.py` counts calls only).
  - Have E2's brief read Clang's throughput and latency loops separately. The latency loop cannot be vectorized across iterations, so it is nearer the call's cost.

### F2. The new tests run in every preset, and six presets the change affects were not run (low, process, P9)

- **The gap:** `rga3.benchmark_<op>_inline` is a new test in every configuration. P9's merge into `feature/cost-evidence` needs P8's selection "in each configuration the change can affect". That includes gcc-release, clang-release, gcc-asan, clang-asan, gcc-measure-o3, and clang-measure-o3. The author admits release and asan; the two `-o3` presets are not mentioned.
- **Evidence so far:** my scratch probes pass for transform_line and compose on both compilers under the asan presets' flags at `-O1` and under `-O3`. Inferred: the other five programs pass too, since `always_inline` does not depend on the optimizer's heuristics.
- **Recommendation:** run the selection in those six presets before the merge, or record the deferral in the merge message.

### F3. The lambda patterns assume non-generic lambdas (low, test robustness; not vacuous today)

- **How the patterns match:** ` t main::`, ` T main::`, and ` W main::` match a lambda only when its demangled name starts with `main::`. That holds for today's lambdas: GCC writes `main::{lambda(...)#2}::operator()(...) const`, and Clang writes `main::$_1::operator()(...) const`.
- **What escapes them:** a generic lambda, or one with a template parameter list, has a templated `operator()`. Its demangled name begins with the return type, `std::array<...> main::{lambda(auto...)}::operator()<...>`, so these patterns would not see it. The kernels, which are templates, demangle the same way. The test catches them only because `reference_kernels::` is forbidden as a bare substring.
- **Other routes I tried:** stripping makes the test fail, through the required ` T main`. LTO keeps the names (GCC adds `.lto_priv` suffixes after them). GNU `nm` and `llvm-nm` print the same layout. A clone such as `[clone .isra.0]` keeps the name.
- **Recommendation:** none needed now. If a form ever becomes generic, forbid `main::{lambda` and `main::$_` as substrings instead, or say in the CMake comment that the forms must not be generic.

### F4. The README says the timed loops are inlined into `bench::main`; on GCC the products' loops are not (nit)

- **What the assembly shows:** on GCC, for the three products, the loops stay in separate `bench::run<...>.isra.0` functions. In the other programs, and on Clang, the loops live in a lambda local to `bench::main`.
- **Effect:** none on the counts, since `calls.py` counts the whole file.
- **Recommendation:** say "in the timed functions" instead.

### F5. `calls.py` has two lines over 80 columns (nit)

- **Where:** the `re.findall` line (96 columns) and the `subprocess.run` line (115 columns).
- **Recommendation:** wrap them, as the other records scripts are wrapped.

### F6. The author's admitted departures (process; resolutions are the coordinator's)

- **One author for tests and code.**
  - Decision 0236 item 5 says new code parts get a separate test author. This unit is a small fix within part D, and the same agent wrote both.
  - Decision 0185's mechanics were followed: test commits on the tests branch, cherry-picked with `-x`, identical, each labeled as failing.
  - The second test commit's message says it was written after seeing the code's intermediate state ("With the reference kernels inlined into their lambdas, GCC kept the reference lambda out of line instead").
  - So the tests were not independent of the code.
  - **Compensation:** I checked the tests' power on evidence other than the author's:
    - the pre-code state fails for the right literals in four presets;
    - the three lambda-mark variants;
    - the kernel mutant at `-O0` and `-O2`;
    - the vacuity probes.
    - They support the tests.
  - **Resolution:** open; the coordinator decides whether this review stands in for a separate test author, as it did for part D (decision 0236 item 5).
- **Build cost not recorded (P1).** The marks also force inlining in `rga3_reference_kernels_test` over the test rational at `-O0`. I did not measure the build-cost effect (no timing).
  - **Resolution:** open; record it with the next build-cost record, or state it as a departure.
- **The tests worktree's `runs.tsv` is dirty.** Its 10 rows are identical to the rows `16c88af` copied, so discarding the uncommitted copy loses nothing that version control does not hold.
  - **Resolution:** open; for its owner to restore with `git -C .claude/worktrees/cost-evidence--bench--inline--tests checkout -- docs/agent/records/performance/runs.tsv`.
- **Mutant records:** they name revision `0c429d52e66c+dirty`. The run rows show the same dirty tree for the mutation runs, before `b480bd3` committed `bench.toml`. Information only.

## What I confirmed against the intent

- **The reference kernels run inline:**
  - At `-O2`, on both compilers, in all seven programs, no reference kernel and no form lambda is called or defined.
  - At `-O0` (gcc-debug, clang-debug), none is defined.
- **What is timed is otherwise unchanged:**
  - The harness changes only by a comment.
  - The loops, the checksum, the pool, and the bench tests' commands are unchanged.
  - The library's calls are the same before and after: Clang's `transform` (and `run_product` for a line), and compose's `run_product` on both compilers.
- **The library's code is unchanged in what it is measured on (fairness section).** The one asymmetry is the one the intent asks for: the reference is inline, and the library is as the compilers build it.
- **The generator and its check:**
  - `reference_kernels.py` writes the mark.
  - The committed header matches.
  - A header without one mark fails `reference.kernels`.
- **Mutants:** `transform_line_kernel_not_always_inline` and `transform_line_reference_lambda_not_always_inline` change what they say, and the inline test would kill each in the four presets. A mutant that drops the library lambda's mark would survive at `-O2` and be killed at `-O0`. That is consistent with the fairness finding, and the spec does not claim otherwise.
- **The comments state why and cite no agent work (S11):** in `reference_kernels.hpp`, `harness.hpp`, `reference_kernels.py`, and `CMakeLists.txt`.

## Cost

About 45 tool calls and about 15 minutes of wall time. I cannot see my token count, so the coordinator should take tokens from the harness.

Machine time:
- 4 configures and 4 builds of the 8 targets;
- 4 rebuilds of the 7 programs at `f6b8276`, and 4 to restore the tip;
- 3 × 14 variant compiles at `-O2 -S`, and 28 for `run.sh` (before and after);
- 15 scratch probe compiles and links;
- about 12 short test runs.
