# Delta review of plan 0038 part D's review fixes

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10. I share no working context with the author or with the first reviewer. I did not edit the change under review.
- **Range:**
  - `feature/cost-evidence--bench` from `a5bb626` (the revision the first review saw) to `de5311b`: `ff13308`, `cff452b`, `302e8d2`, `fd24dea`, `de5311b`.
  - The tests branch `feature/cost-evidence--bench--tests` at `fad8b29`, one commit after `11ca72a`.
- **Where I worked:** the worktree `.claude/worktrees/wu17-part-d-delta` on `records/wu17-part-d-delta`, branched from `de5311b`, as the first review's branch was from `a5bb626`. Build trees `build/gcc-debug` and `build/clang-debug` in that worktree.
- **Measured against:** the first review, `records/reviews/2026-11-10-wu17-part-d-blind.md`: findings D1, D2, T1, M1, H1 to H3, and N1 to N3. Also W9 (no expected value or tolerance changed), S11, and the departures that `fd24dea` adds to `records/performance/wu17-bench/part-d-build-cost.md`.
- **The brief's commit id:** it names the tests-branch commit `7a3ac8f`. No such object exists in the repository. The tests branch's reflog shows one commit after `11ca72a`, `fad8b29`. `ff13308`'s `-x` line names `fad8b29`, so I reviewed `fad8b29`.

## Outcome

**The fixes resolve every finding of the first review. Nothing blocks the merge.** I found one low finding (R1) and three nits (R2 to R4), all in records or comments.

- **The small-integer draws have teeth and weaken nothing.**
  - A transform mutant that carries a sign on the sum survives the test at `a5bb626` and is killed by the test at `fad8b29`. The same holds for the two such mutants I added, on the line and the plane transforms.
  - The uniform draws, the agreement kinds, `agree`, and the case counts are unchanged.
- **Both new mutants are honest.** Expanding each edit gives the original terms and signs. I rebuilt and ran each by hand, and each is killed by the case the records name.
- **The H2 and H3 comments are accurate.** Declining `+ 0.0` is sound (see H3 below).
- **The departures record is complete on the findings it was asked to record.** Its account of the lost run rows is honest, and the loss does not matter for any gate or committed result. R1 asks for one sentence more.
- **Nothing under `include/` changes.** No changed file cites agent work (S11).

## Checks run

All runs went through `tools/timed`; their rows are in this branch's `runs.tsv`.

**Pool status.** Before: 22 of 23 slots free, with clients ninja and ctest. After: 23 of 23 free, with a ctest client. I neither started nor stopped the pool. I made no timing measurement, did not touch `master`, changed no processor setting, and fetched nothing from the web.

| Check | Result |
| --- | --- |
| `git diff --stat a5bb626..de5311b -- include/` | empty |
| `git diff fad8b29 ff13308`, test file before and after | identical; `ff13308` carries the `-x` line |
| `python3 -I test/reference/reference_kernels.py --check test/bench/reference_kernels.hpp` | exit 0, matches |
| Configure, then build the 10 part D targets, in gcc-debug and clang-debug | exit 0 in both |
| `ctest -L '^(rga3\.(reference_kernels\|benchmarks)\|reference\.kernels)$'` | 9 of 9 pass in both |
| `ctest -R '^tests\.(reference_markers\|golden_facts\|citations\|messages_pinned)$'` | 4 of 4 pass in both; the new "Reference: independent value:" markers are accepted |
| `rga3_reference_kernels_test --rand-seed=` 0, 1, 987654321, 20261110, in both presets | 14 of 14 cases and 14,700 assertions in each run (7 × 100 rational + 7 × 2 × 1,000 double; it was 7,700) |
| Mutants by hand, rebuilt and run in both presets | see the next table |
| `clang-format --dry-run -Werror` on `harness.hpp` and `reference_kernels_test.cpp` | exit 0 |
| Lines over 80 columns in `test/bench/CMakeLists.txt`, `reference_kernels.py`, `harness.hpp`, `reference_kernels_test.cpp` | none |
| Lines of `harness.hpp` that are neither blank nor comment | 150 at `de5311b` and at `a5bb626`, as departure 3 says |
| Grep of the added lines under `test/` for agent citations and British spellings | none (two false hits, "recording" and "plane") |
| `tools/runlog check` after adding this review's rows | exit 0 |

Mutants. The test at `a5bb626` is "old" and the test at `fad8b29` is "new". The results were the same in gcc-debug and clang-debug.

| Mutant (an edit of `reference_kernels.hpp`) | Old test | New test |
| --- | --- | --- |
| `transform_point_sign_on_sum`, the spec's edit | survives, 14 of 14 pass | killed: transform(point) bits over double |
| `transform_point_term_order_reversed`, the spec's edit | killed: transform(point) bits over double | (killed, per the mutation records at `302e8d2`) |
| Mine: transform_line's e23 output with its sign on the sum | survives | killed: transform(line) bits over double |
| Mine: transform_plane's e423 output with its sign on the sum | survives | killed: transform(plane) bits over double |
| Mine: wedge_point_point's e41 output with its sign on the sum | (not run) | survives, as intended: the products are compared by value |

### Not checked

- **Not run:** the `lint`, asan, release, and measure presets, the full suite, the `format-check` target, and the trace gate. The brief limits me to gcc-debug and clang-debug.
  - The author's rows at `302e8d2` show `lint`, gcc-measure and clang-measure builds of the part D targets, and `format-check`, all exiting 0.
  - The same rows show the part D tests passing in gcc-measure and clang-measure, so the new draws also pass at `-O2`.
- **Not rerun:** the mutation tool on `bench.toml`. I applied its two new edits by hand instead (above). I did not reproduce its records of the two older mutants.
- **Not verifiable:** the lost scratch rows (departure 4). Their content cannot be checked. I reproduced the claim they carried instead.

## The first review's findings

### D1. One agent wrote the tests and the code — resolved by recording

Departure 1 states:

- the role split the plan names and that it did not happen;
- that decision 0185's form was kept but not its separation;
- that the generator existed while the tests were written;
- that whether the review suffices as the independent check is the coordinator's call.

That is what the first review asked for. The fix pass repeats the pattern: the same agent wrote `fad8b29` and the code fixes. Departure 1's "did all three in turn" covers it, though it does not say so of the fix (R3). This review is an independent check of the new test's power.

### D2. The tolerance was fixed in the test commit — resolved by recording

Departure 2 states the tolerance per operation, its derivation, the 28 s gap, and that the claim rests on the author's statement (V12). The list matches the code: `Agreement::values` for p ^ q, p ^ q ^ r, l & g, and compose; `Agreement::bits` for the three transforms.

### T1. The draws could not tell bits from values — resolved

Each double case now also draws 1,000 operand sets with coefficients in {-1, 0, 1}.

**The draws are meaningful.**

- Products of up to three such coefficients are exact, and so are their sums, so exact zeros occur.
- Three sign-on-sum transform mutants, one per transform, each survive the old test and are killed by the new one. So the bits claim for transforms now meets a case that separates it from the values claim.
- The comment says why each kind of draw is there: uniform draws test term order, and small integers test the sign of zero.

**Nothing is weakened (W9).**

- `double_cases` is still 1,000, now per kind.
- The uniform draws come first from the same seed, so they are the same 1,000 operand sets as before.
- The `Agreement` argument of every case, `agree`, and `rational_cases` are unchanged.
- The rational cases skip the second kind.
- The test change is a commit of its own on the tests branch, and its message says that no expected value or tolerance changes.

The products are compared by value on the small integers as well. So a product that differs from its kernel only in a zero's sign passes, as the stated tolerance intends; my wedge mutant shows it.

### M1. The mutation spec showed only sign flips — resolved

`302e8d2` adds two mutants that change no rational value, and `fd24dea` records both killed in both presets at `302e8d2`.

**`transform_point_term_order_reversed`.** It reverses the terms of transform_point's e1 output. Those terms run over the middle blades in grade-then-index order, so their reversal is the order a reversed middle-blade order gives, as the spec's comment says. The edit keeps each term's sign. It is killed by the uniform draws, so it shows the zero tolerance live on term order.

**`transform_point_sign_on_sum`.** It turns transform_point's e3 output, -a + b + ..., into -(a - b - ...). Expanding it gives the same eight terms with the same signs. Each partial sum is the negation of the original one, except that a zero comes out as -0.

- It survives the old test and is killed by the new one, which matches the spec's comment "the test before those draws let this mutant live".
- It is the gap mutant that T1 described.

### H1. The harness is about 150 lines against about 100 — resolved by recording

Departure 3 records the size and what it buys, and says that trimming was not done. The count is right.

### H2. Whole-process counters, and a checksum inside the timed loop — resolved

The harness comment now says three things:

- The nanoseconds per operation include a move and an addition per coefficient for the checksum, the same in both forms.
- The counters include the process's start, the drawing of the operands, and the printing.
- Differencing two runs with different pass counts, divided by the difference of their operation counts, leaves those out. That is correct, since none of the three depends on the pass count.

### H3. `check` compares bits — resolved by stating the assumption; declining `+ 0.0` is sound

The comment says that the checksum adds bits, so a zero's sign counts. It says that a product or compose with its sign on the sum can differ from its kernel there, and that uniform draws make an exact zero all but impossible.

- **The comment is accurate.** The harness draws uniform doubles. Its unitization of motors scales and subtracts but sets no coefficient to an exact zero. No output of the seven operations is identically zero. I infer that from the kernels' term lists and the passing `check` tests; I did not prove it for unit motors.
- **Why not `+ 0.0`.** `bits_of` runs inside the timed loop of every run, not only in `check`. Without `-ffast-math`, a compiler may not drop `c + 0.0`, since it maps -0 to +0, so the fix would add one floating-point addition per coefficient to every timing. That adds to the bias H2 describes. A branch or a compare-and-select costs about as much.
- **The cost of declining.** `check` stays stricter than the library's guarantee, and its only failure mode is a loud false alarm, never a false pass. So declining costs nothing in assurance.
- **The decline's reason is not in any record.** This review records it.

### N1. CMake comment layout and long lines — resolved

The `bench_<operation>` comment now sits just above `set(bench_operations ...)` and the `foreach`, rewrapped. No line in the four files exceeds 80 columns. For the docstring's usage lines, see R2.

### N2. The L1 data cache figure — resolved

The comment now reads "a third of the 48 KiB first-level data cache of a performance core of the development machine and half of an efficiency core's 32 KiB". With 128 × 2 motors × 8 doubles × 8 bytes = 16 KiB, both fractions are right.

### N3. "Reference: proof" on the double cases — resolved

The seven double cases now read "Reference: independent value: the kernel ... written from the definitions", with the zero tolerance's reason after it. `tests.reference_markers` accepts the wording.

## New findings, most severe first

### R1. Departure 4 should say which claims the lost rows carried, and that they now rest on other evidence (low, records)

The lost rows were scratch compiles on an uncommitted tree, so they fall outside every class of row that P13's compaction keeps. No gate row and no row at a committed revision was lost, so the loss changes no merge evidence.

The rows were the only run evidence for two claims in `fad8b29`'s message:

- the new test passes against the stand-in at `-O0` and `-O2`;
- the sign-on-sum stand-in passes the earlier test and fails the new one.

Other evidence now carries both:

- the first is carried by the author's own rows at `302e8d2` (the part D tests pass in all four presets) and by mine;
- the second is carried by the kill record at `302e8d2` for the "fails" half, and by this review's reproduction (old test survives, new test kills, in both presets) for the "passes the earlier version" half.

**Recommendation:** add one sentence to departure 4 saying that. Also consider a lesson: restore only the files a scratch experiment changed, never `runs.tsv`.

**Resolution:** open.

### R2. The generator's usage lines no longer work if copied (nit)

To fit 80 columns, `reference_kernels.py`'s docstring now splits each command over two lines with no continuation. Copying the first line of the `--check` form runs the generator to standard output instead of checking.

**Recommendation:** end the first line of each with ` \`.

**Resolution:** open.

### R3. Departure 1 does not say that the fix pass repeats the single-agent pattern (nit, records)

Departure 1 says that one agent "did all three in turn". `fad8b29` was also written by the agent that wrote the code fixes, against a stand-in of its own. The departure's scope reads as part D's first pass.

**Recommendation:** add "including the fixes after the review".

**Resolution:** open.

### R4. Notation in departure 2 (nit, records)

Departure 2 writes "line v plane" for the meet, which the test and the plan write as l & g.

**Recommendation:** use one notation.

**Resolution:** open.

### R5. Where the departures live (information)

Plan item 13 places the outcome in the plan at the unit's close, so a record on the branch is right for now. But a list of departures in a file titled for build cost is easy to miss.

**Recommendation:** the plan's part D outcome should point to `part-d-build-cost.md`'s departures section when it is written.

## Cost

About 30 tool calls and about 15 minutes of wall time. I cannot see my token count, so the coordinator should take tokens from the harness.

Machine time:

- two configures, under 0.4 s each;
- two builds of the part D targets, 1.8 s and 1.6 s;
- 18 rebuilds of the kernel test for mutants and restores, about 1 to 2 s each;
- about 30 short test runs, each under 1.1 s;
- one generator check.
