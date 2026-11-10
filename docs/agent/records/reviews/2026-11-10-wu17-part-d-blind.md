# Blind review of plan 0038 part D (benchmark harness, reference kernels, smoke tests)

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10. I share no working context with the author. I did not edit the change under review.
- **Range:** `feature/cost-evidence` at `5444671` to `feature/cost-evidence--bench` at `a5bb626`, seven commits, with the tests branch `feature/cost-evidence--bench--tests` at `11ca72a`, two commits. Reviewed in the worktree `.claude/worktrees/wu17-part-d-review` on `records/wu17-part-d-review`, branched from `a5bb626`. Build trees `build/gcc-debug` and `build/clang-debug` in that worktree.
- **Statement of intent:** plan 0038 section 4, items 4, 6 (harness support only), and 11; decisions 0014, 0185, 0233, and 0234; rule K in `records/spikes/spike-e/COUNTING-RULE.md`. Decision 0234 and the spike-e records are on `develop`, not on the branch's base, so I read `develop`'s copies.
- **Verdict in the brief:** none. **The author's departure list (C20) was not given to me**, although W5 says the reviewer gets it. I worked from the commit messages and the branch's records.

## Outcome

**Merge after the author records two departures (D1, D2). No finding blocks on substance.**

- The reference kernels are right. I derived their term lists by a second route, spike-e's sympy algebra (`plans.py`, `rigid3.py`), and both routes give identical lists for the three transforms and for compose.
- The smoke tests' zero tolerances are right, and they can fail. A term-order mutant is killed by the double cases alone. A probe with exact zeros shows the products differ only in the sign of zero, and the transforms not even there.
- The CMake and preset change does what plan item 4 and decision 0233 O8 ask.
- Nothing under `include/` changes, and no file cites agent work (S11).
- Two process points need recording rather than code changes: one agent wrote both the tests and the code (D1), and the double tolerance was fixed in the test commit rather than in the plan (D2).

## Checks run

All runs went through `tools/timed`; their rows are in this branch's `runs.tsv`. Pool status: 10 of 23 slots free before the work (load 18.0), 21 of 23 before the build, and 0 of 23 free after (other clients; load 13.4). I neither started nor stopped the pool. I made no timing measurement, did not touch `master`, and changed no processor setting.

| Check | Result |
| --- | --- |
| `git diff --stat 5444671...a5bb626 -- include/` | empty |
| Cherry-picks: `git diff 78a4458 3b60b57`, `git diff 11ca72a 92d86c2` | identical trees; both carry `-x` lines |
| `python3 -I test/reference/reference_kernels.py --check test/bench/reference_kernels.hpp`, and regenerate + `diff` | exit 0, byte-identical |
| `--check` against an altered header (term order reversed), against CRLF line endings, and with a bad argument | exit 1, exit 1, exit 1 with usage |
| Configure gcc-debug and clang-debug; build the 11 part D targets (test, 7 programs, 2 header checks, plus deps) | exit 0 in both |
| `ctest -L '^(rga3\.(reference_kernels|benchmarks)|reference\.kernels)$'` | 9 of 9 pass in both |
| `rga3_reference_kernels_test --rand-seed=0`, `1`, `987654321`, in both presets | 14 of 14 cases, 7,700 assertions, in each run |
| `ctest --preset gcc-debug -N` against `ctest --test-dir build/gcc-debug -N` | 553 against 576: the 22 `bench.*` tests and `tools.runlog_check` are left out, and the 7 `rga3.benchmark_*` and both kernel tests stay in |
| `--show-only=json-v1` of the bench tests | each `bench.*` has `LABELS bench;bench.<op>` and `RUN_SERIAL ON`; `rga3.benchmark_*` has `rga3;rga3.benchmarks` and no `RUN_SERIAL` |
| Bench programs by hand (gcc-debug): bad loop, zero passes, no arguments, `check 1 5`, `check 1 99` | exit 2 with usage, 2, 2; the checksums agree, exit 0 |
| `clang-format --dry-run -Werror` on the 10 new C++ files | exit 0 |
| Grep for agent citations (plan, decision, spike, provision numbers, agent names) and British spellings in the new files and messages | none |
| Build-cost record: recomputed from `part-d-compile.tsv` | sums, largest TUs, and peak memory match the Markdown table |
| Cross-check of term lists against spike-e (math-tools venv, sympy 1.14) | identical: point 20 + 28 terms, line 36 + 36, plane 28 + 20, compose 48 |
| Generator mutants (scratch copy): antireverse sign forced to +1; one operand orientation dropped | both refused, exit 1, nothing written |
| Generator mutant: middle blades in reverse order | written (as expected: order does not change rational values); see M1 |
| Motor drawing: harness unitization, then `Q ⟇ antireverse(Q)` in `conventions_check.py` | `e1234` = 1, every other coefficient 0 to rounding |
| Signed-zero probe: library against kernel on coefficients from {-1, 0, 1}, 20,000 draws, g++ and clang++ at `-O0 -ffp-contract=off` | see T1 |

### Not checked

- `lint` (clang-tidy), the asan, release, and measure presets, the full suite, the `format-check` target, and the trace gate. The brief limits me to gcc-debug and clang-debug. The author's run rows show lint failing once and then passing, and the smoke tests passing in gcc-measure and clang-measure.
- I did not read the benchmark loops' assembly at `-O2`. So I have not confirmed that the optimizer keeps all the work; that read belongs to part E1.
- I ran no benchmark under the lock or pinned, and made no timing.
- I could not see the scratch stand-in that the test commit was checked against: it is not committed (see D1).

## Findings, most severe first

### D1. One agent wrote the tests and the code; the role split the plan names did not happen, and the branch does not say so (medium, process)

- **Plan:** section 5 item 4 and section 9 give part D's tests to the doctest-expert. The code goes to the performance-engineer, with the build-expert for CMake.
- **Record:** the cost row on `develop` reads `wu17-part-d  doctest/build/performance (one agent)`. No commit or record on the branch mentions it.
- **Mechanics:** decision 0185's form was followed:
  - Tests were committed on the tests branch and cherry-picked with `-x`.
  - The failing commit `3b60b57` is labeled and names `rga3.reference_kernels`. At `3b60b57`, `test/bench/` holds only the test and its CMake, so the build does fail as the message says.
- **Timeline:** the run log shows the separation that 0185 protects was not there:
  - The stand-in was compiled and run at 13:37:58 to 13:38:29 UTC.
  - The test was committed at 13:38:57 and cherry-picked at 13:39:26.
  - At 13:39:43, the code worktree, dirty at `3b60b57`, configured. At 13:39:51 it built `rga3_reference_kernels_test` with a working `reference_kernels.hpp`.
  - The 387-line generator was committed at 13:40:37.
  - So the generator existed while the test was written.
- **Later test commit:** the second test commit (`11ca72a`, cherry-picked as `92d86c2`) followed the lint failure at 13:42:54. It changes the seed source and two clang-tidy points, and no expected value or tolerance. W9 is met by its being a separate commit.
- **Compensation:** I checked the tests' power independently (M1, T1) and found them sound, so the result stands.
- **Resolution:** open. Record the single-agent departure (C20) in the plan's part D outcome; whether this review suffices as the independent check is the coordinator's call.

### D2. The double tolerance was fixed in the test commit, not in the plan, and the commit follows the stand-in runs (medium-low, process; substance confirmed)

- **Plan:** item 4 allows bitwise equality "where the term order is the same and the plan says so before the run". The plan was not amended.
- **Where it is stated:** the tolerance (bits for transforms, values for products and compose) is in the header comment of `reference_kernels_test.cpp` and in the message of `78a4458`. That commit came 28 s after the last stand-in run. So the record cannot show that the tolerance was fixed before a result was seen (V12). The claim "derived before any run" rests on the author's word.
- **Substance:**
  - The argument is correct: rounding to nearest is symmetric, so moving a sign from a sum into its terms changes only the sign of a zero, and `-ffp-contract=off` is in every preset's flags.
  - T1's probe confirms it on evidence other than the test.
  - M1 confirms that zero tolerance has teeth.
- **Resolution:** open. Note the tolerance and its reason in the plan's part D outcome.

### T1. The smoke test's draws cannot tell "bits" from "values", so its signed-zero claims go untested (low)

- **Probe:** the library against the kernels on coefficients drawn from {-1, 0, 1}, 20,000 draws. The counts are differences in bits / differences in value; g++ and clang++ gave the same:

  | Operation | Bits / value |
  | --- | --- |
  | p ^ q | 13,788 / 0 |
  | p ^ q ^ r | 10,430 / 0 |
  | l & g | 0 / 0 |
  | compose | 13,818 / 0 |
  | transform of a point, line, plane | 0 / 0 each |

  This is exactly what the test's comment predicts.
- **The gap:** the test draws doubles uniformly from [-1, 1) with 53 random bits. Such operands essentially never give an exact zero, so on its inputs `Agreement::bits` and `Agreement::values` accept the same results.
- **What can fail today:** a change that moved a transform's sign onto the sum would pass the "bits" test. The value cases still catch any change of order or terms.
- **Recommendation:** add one small-integer draw (coefficients in {-1, 0, 1}) to the double cases. Then the bit claim for transforms, and the value claim for products, each meet a case that separates them.

### M1. The mutation spec shows only sign flips, which the rational cases already kill (low)

- **Spec:** `bench.toml` holds the plan's item 11 mutant for part D, a reference kernel with one sign flipped, twice. Both are expected `killed` and are killed in both presets. No gap mutant is claimed, which is correct for part D: it closes no gap in an existing suite, and the gap mutants belong to part C. The spec says honestly that `reference.kernels` would also kill both and is left out of the selection.
- **What the sign flips do not show:** in both runs the rational cases fail as well, so neither mutant shows that the double comparison's zero tolerance can fail on its own.
- **My mutant:** generator mutant "middle blades in reverse order", built into `rga3_reference_kernels_test` in gcc-debug. It is killed by exactly four cases, all double: p ^ q ^ r values, and the bits of the three transforms. The ten others pass, all seven rational cases among them. So the zero tolerance is live.
- **Recommendation:** add this mutant to `bench.toml` (edit: the stage-1 lines of one transform in reverse order), expected `killed`, so the record shows it.

### H1. The harness is about twice decision 0014's "about 100 lines", with no departure recorded (low)

- **Count:** `harness.hpp` has 201 lines, 150 of them neither blank nor comment. Each of the seven programs adds 19 to 22 such lines.
- **Where the size goes:** argument parsing and usage (about 25 lines), the `check` mode, the motor unitization, and the tuple-and-apply plumbing. All of it has a present use.
- **Recommendation:** record the size as a departure from 0014's estimate, or trim the parsing (for example, positional arguments with a fixed form list).

### H2. Instructions and cycles per operation from a whole-process counter include setup; ns per operation includes the checksum (low, for part E2)

- **Counters:** the harness says so: "Instructions and cycles come from outside, from a counter over the whole process". So the figure per operation that item 6 reads from `tools/timed measure` includes process start, `std::print`, and drawing the pool.
- **Checksum:** the timed region includes the checksum, one move from a floating-point register and one integer add per coefficient. For p ^ q that is 6 moves and 6 adds beside 12 multiplications and 6 additions.
- **Effect:** both overheads are the same for the two forms, so differences are unaffected, but ratios and absolute figures are biased.
- **Recommendation:** E2 can difference two pass counts (for example 4,096 and 8,192) for the counters. It should state that ns per operation includes the checksum.
- **Not a dead-code risk:** every coefficient reaches `sink` through the checksum, and `sink` is volatile.

### H3. `check` compares bit checksums for the products and compose, which is stronger than the value equality the library guarantees (low)

- **Where:** `rga3.benchmark_<op>` runs `check`, which requires equal checksums of bits. T1 shows that compose and both joins differ in the sign of zero on inputs with exact zeros.
- **Why it passes now:** with the harness's uniform draws, exact zeros do not occur, so the tests pass and will keep passing at seed 20261110.
- **Recommendation:** state the assumption in the harness comment, or add `+ 0.0` to each coefficient before `bit_cast` in `bits_of`. That maps -0 to +0 under rounding to nearest, at one addition per coefficient, in both forms alike.

### N1. CMake comment layout (nit)

- **Long line:** `test/bench/CMakeLists.txt:37` is 104 columns.
- **Placement:** the `bench_<operation>` comment block (lines 33 to 41) runs straight into "The harness, which needs the rga3 preset" and sits above `galoot_bench_harness`, not above the `foreach` it describes.
- **Recommendation:** move it to just above `set(bench_operations ...)` and rewrap it.
- **Python:** in `reference_kernels.py`, lines 41, 43, 254 and 295 are 81 to 89 columns (two docstring usage lines and two rule comments).

### N2. The L1 data cache figure in the harness is the efficiency core's (nit)

- **Where:** `harness.hpp:42-44` says the pool is "half of a first-level data cache of 32 KiB".
- **The machine:** on the development machine, CPU 4 (a performance core, where E2 pins) has 48 KiB. 32 KiB is an efficiency core's L1d (`/sys/devices/system/cpu/cpu{4,20}/cache/index0/size`).
- **Impact:** 16 KiB fits either, so nothing changes.
- **Recommendation:** say "at most a third of a performance core's 48 KiB", or drop the cache size.

### N3. "Reference: proof" on the double cases (nit)

- **The labels:** the double cases' comments name their reference "proof".
- **What is proved:** the tolerance, given the premise that the library adds the same products in the same order. The test itself checks that premise. The expected value is the generated kernel, which is an independent value (V1 kind 3).
- **Recommendation:** say "independent value, with a tolerance of zero by the following argument". Only if `tests.reference_markers` accepts that wording.

### I1. The generator's self-check of the transforms and compose uses the same algebra that produced their terms (information, no defect)

- **The asymmetry:** `check_transform` and compose's check compare against `conventions_check.py`'s `sandwich` and `antigp`, the module that generated the terms. Only the joins and the meet are also checked against `rigid_motions.py`'s closed forms.
- **Why it is no defect:** my cross-check against spike-e's separate sympy algebra (`rigid3.py`, `plans.py`) gives identical term lists, orders, and signs for all three transforms and for compose.
- **Ordering:** the kernels encode the library's term order (rule K: by the first operand's position, then the second's; the middle in grade-then-index order). That is a specification the kernels follow, not shared code, so V1 kind 3 holds.

### I2. Interleaving and environment sizes are left to part E2's driver (information)

- **What the harness gives a driver:** one form per process, the seed and pass count on the command line, and a printed line with the operation, form, loop, operation count, seed, ns, and checksum. A driver can interleave forms, check that the two forms' checksums agree, and vary the environment size.
- **What is missing:** no driver is committed. Item 6 asks only for harness support here.
- **Recommendation:** E2 commits its driver with its records, or its results are not reproducible (V10).

## What I confirmed against the plan

- **Item 4:**
  - Smoke tests exist for the seven operations: exact over the test rational and zero tolerance over double, with the reason stated in the file.
  - The harness lives under `test/bench/`.
  - Kernels are generated by `test/reference/reference_kernels.py` from the definitions. The header includes only `<array>`, and its target does not link the library.
  - The CTest `reference.kernels` regenerates and diffs.
  - There is one program per operation.
  - The benchmarks build in every preset, run only under `bench`, and are left out by the presets. The smoke tests run everywhere.
- **Item 6 (harness support):**
  - Operands are drawn at run time from a command-line seed (`std::mt19937_64`, the same in every library), the same for both forms.
  - Every result is folded into a printed checksum and a volatile sink.
  - There is a throughput loop with shifted, independent operands, and a latency loop for the transforms and compose.
  - Unit motors are drawn as described.
  - Each process runs one form.
- **Item 11:**
  - The part D mutant is present and killed in both presets.
  - No gap mutant is wrongly claimed.
- **Rule K:**
  - Counts per kernel are 48, 72, 48 (transforms), 12, 24, 12 (products), and 48 (compose).
  - Negations are 3, 9, 5 (transforms) and 4 (compose).
  - All agree with `COUNTING-RULE.md` and the spike-e README.
- **Build-cost record (P1):**
  - It covers ten units in gcc-debug, clang-debug, gcc-measure, and clang-measure.
  - Every row is marked `shared`, and the record says so.
  - The 5 % share is labeled as inferred.
- **S11 and American English:** no agent citations, no British spellings.

## Cost

About 50 tool calls and about 20 minutes of wall time. I cannot see my token count, so the coordinator should take tokens from the harness. Machine time: two configures (under 1 s), two builds of the part D targets (5.0 s and 1.7 s), one mutant rebuild and one restore (gcc-debug), about 12 short test runs, and two scratch compiles of the zero probe.
