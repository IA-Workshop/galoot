# Blind review of plan 0038 part C (counting scalar, operation counts, mutants)

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10. I share no working context with the author, and I did not edit the change under review.
- **Range:** `feature/cost-evidence` at `5444671` to `feature/cost-evidence--counting` at `b1be3e9`, nine commits, with the tests branch `feature/cost-evidence--counting--tests` at `3f50c5f`, five commits. I reviewed in the worktree `.claude/worktrees/wu17-part-c-review`, on `records/wu17-part-c-review`, branched from `b1be3e9`. Its build trees are `build/gcc-debug` and `build/clang-debug`.
- **Statement of intent:** plan 0038 section 4 items 3 and 11; decisions 0185, 0233, and 0234; rule K and the counts in `records/spikes/spike-e/` (`COUNTING-RULE.md`, `counts.md`), read on `develop`; the independent check `reviews/2026-11-10-wu17-part-b-check.md`.
- **Verdict in the brief:** none. The author's departure list was not handed to me as a document. The brief paraphrased three of its points, and I judge those below:
  - intermediate commits fail checks their messages do not name;
  - `sign_multiplied` and `first_stage_all` are not gap mutants;
  - `gap.toml` keeps `sign_multiplied` declared `survived`, so `tools/mutate gap.toml` exits 1.

  Beyond that I worked from the commit messages and the branch's records.

## Outcome

**Merge after the records are corrected (M1, M2, L2) and the departures are recorded (M3, L1). No code finding blocks.**

The code is right:

- The counting scalar does what item 3 asks.
- The expected counts are independent of the library and equal spike-e's counts in every cell.
- Both programs pass on both compilers.

The weak part is the mutation evidence, in three ways:

- The "gap" the four criterion-11 mutants were meant to show is mostly not a gap. The existing `core.rounding`, which the gap selection leaves out, kills all three product mutants (M1).
- `first_stage_all` is not equivalent. A counting test of one transform kills it (M2).
- One declaration and two reasons in the specs state things the runs showed false (L2).

The counting tests do close a real gap: a mutant I wrote (double negation) survives every value suite I ran and is killed by both counting programs.

## Checks run

All runs went through `tools/timed`, and their rows are in this branch's `runs.tsv`. Pool status: 23 of 23 free before the work (load 1.07), and 23 of 23 free after. I neither started nor stopped the pool. I made no timing measurement, did not touch `master`, and changed no processor setting.

| Check | Result |
| --- | --- |
| `git diff --stat 5444671 b1be3e9 -- include/` | empty |
| Cherry-picks against tests-branch commits, by `git patch-id --stable` (a84f705/17b2a5e, 58149bd/6ee2934, 091afaf/3aaf96c, fbc2251/79c7434, 3f50c5f/61b3b81) | identical patches; each carries its `-x` line |
| Configure gcc-debug and clang-debug; build `support_counting_test`, `rga3_op_counts_checking`, `rga3_op_counts_unchecked` | exit 0 in both |
| `ctest -L '^(support\.counting\|rga3\.op_counts\|reference\.golden\|tests\.references)$'` | 7 of 7 pass in both |
| `galoot_test_support_verify_interface_header_sets` (counting.hpp on its own) | builds in both |
| `clang-tidy -p build/clang-debug --warnings-as-errors='*'` on `counting_test.cpp` and `op_counts_test.cpp` at the tip | exit 0 |
| Cheap gates at each of the nine commits, from `git archive`: `test_references.py markers`, `golden`, `golden.py --check`, the trace gate, `clang-format --dry-run -Werror` on the new files | see L1 |
| `golden.hpp`'s counts against `counts.md`, cell by cell, and the identity add + sub = terms − places written | all match (below) |
| golden.py's imports | the standard library, `clifford_reference`, `conventions_check`, `rigid_motions`; nothing under `include/` is read |
| Grep of the new test, CMake, and record text and of the messages for agent citations and British spellings | no citations; one British form (N1) |
| Mutant probes by hand, outside `tools/mutate`, on scratch copies of `include/` (gcc-debug flags, GCC only): `2026-11-10-wu17-part-c-blind/probes.sh` reruns them | see M1, M2 |

The counts, checking off / checking on, as multiplications, additions and subtractions together, negations, and comparisons:

| Operation | golden.hpp | counts.md (rule K) |
| --- | --- | --- |
| p ^ q | 12, 6, 0, 0 / same | canonical: 12, 0 + 6, 0 |
| (p ^ q) ^ r | 24, 14, 0, 0 / same | canonical: 24, 4 + 10, 0 |
| l v g | 12, 8, 1, 0 / same | 12, 3 + 5, 1 |
| transform(point) | 48, 36, 3, 0 / 68, 52, 6, 4 | 48, 36, 3 / 68, 35 + 17, 6, 4 |
| transform(line) | 72, 58, 9, 0 / 84, 68, 10, 2 | 72, 58, 9 / 84, 23 + 45, 10, 2 |
| transform(plane) | 48, 36, 5, 0 / 76, 60, 6, 4 | 48, 36, 5 / 76, 35 + 25, 6, 4 |
| compose | 48, 40, 4, 0 / same | canonical: 48, 16 + 24, 4 |

The additions follow from terms minus places written. Take `transform_test.cpp`'s kernel sizes and an 8-blade middle. For the point, unchecked: 48 − (8 + 4) = 36. Checking on: 68 − (8 + 4 + 4) = 52. The line and plane work out the same way, and so do the products (12 − 6, 24 − 10, 12 − 4, 48 − 8). The comparisons equal the dropped blades, 4, 2, and 4.

### Not checked

- The `lint`, asan, release, and measure presets, the full suite, and the `format-check` target. The brief limits me to gcc-debug and clang-debug. The author's run rows show the new targets built and tested in all eight presets, and lint failing once and then passing.
- I did not rerun `tools/mutate gap.toml` or `counting.toml`. For their verdicts I rely on the author's `out/*.tsv`. My own probes are by hand and on GCC only.
- I did not see the scratch stand-ins the tests were checked against before the code existed; they are not committed (the README says so).

## Findings, most severe first

### M1. The gap the four criterion-11 mutants were meant to show is mostly not a gap: `core.rounding` already kills the three product mutants (medium, evidence)

The plan, and `gap.toml` after it, judges the gap by `core.product`, `core.transform`, and `rga3.*`. W3 judges a gap's mutants by "the suite without the new tests". `core.rounding` is part of that suite and runs every product through `test_support::tracked`, whose rounding count is a structure check in its own right. Its case "a tracked run counts the roundings of a product" (`roundings == 2`) fails for each of the three product mutants:

| Mutant | `core.rounding` (gcc-debug flags, checking on) |
| --- | --- |
| code | 11 of 11 cases pass |
| `product_kernel_zero_term` | 1 case fails |
| `sign_multiplied` | 4 cases fail |
| `cancelling_pair` | 1 case fails |

So, against the whole existing suite:

- None of the four listed mutants counts for the gap.
- `first_stage_all` is not killed at all (M2).

The README's "the counting tests close that gap" holds only for the narrowed selection.

The counting tests do close a real gap. I checked with two mutants of my own in `accumulate`:

- **`double_negation`:** a first term of sign +1 is assigned as `-(-value)`. It is exact, and `tracked` does not count it. It survives `core.rounding`, `core.transform_rounding`, `core.product`, and `core.transform`, and both `op_counts` programs kill it (7 of 7 cases fail in each mode). I did not run the `rga3.*` value suites on it. Over an exact scalar, and over double, it changes no value.
- **`neg_as_sub`:** a first term of sign −1 is assigned as `T{} - value`. `core.rounding` kills it (3 cases), so it is not a gap mutant either.

The comparisons of the checking path are a second kind of thing only the counting tests see (`counting_without_comparisons` is killed only by `rga3.op_counts_checking` and `support.counting`).

**Resolution asked:**

- Widen `gap.toml`'s selection to include `core.rounding` (and `core.identities_rounding`).
- Rerun it and record the three product mutants as killed by the existing suite.
- Add at least one true gap mutant, such as `double_negation`, or a doubled comparison in the transform's check.
- Restate the README's gap finding to match.

The code needs no change.

### M2. `first_stage_all` is not equivalent, and a counting test of one transform kills it (medium, evidence and departure)

The README says that no test of this unit can kill `first_stage_all`. That is right for the operations item 3 lists: in rigid(3), the restricted first stage of a motor on a point, a line, or a plane is whole (20, 36, and 28 terms, as `transform_test.cpp`'s kernel sizes state).

It is not equivalent, though. Take the degenerate form of `transform_test.cpp` (e1 . e2 = 1), with V on e1, e2, e3, e123 and x on e123. Over the counting scalar with checking off:

| Build | Multiplications |
| --- | --- |
| the code | 4 |
| `first_stage_all` | 6 |

So the mutant computes 2 first-stage terms that nothing reads (`degenerate_probe.cpp`). With checking on, both make 16, as expected. A five-line test in a checking-off program would pin the restriction. No test pins it now: `transform_test.cpp` checks the plan sizes at compile time, and this mutant changes the call site, not the plan.

Criterion 11 expected this mutant killed at the tip, or already killed by `transform_test.cpp`. Neither happened, so this is a departure from criterion 11 to record (C20).

The reasons in the specs also disagree with each other and with the facts:

- `gap.toml` calls it "a gap mutant".
- `counting.toml` and the README call it "not a gap mutant".
- Both specs say "nothing changes", which is true only in rigid(3) for those layouts.

**Resolution asked:** either add a counting case for the degenerate transform (my recommendation: it is cheap, and it pins the one run-time saving `transform` makes), or record the survivor and the departure, with the reasons scoped to "in these operations".

### M3. One agent wrote both the tests and the code; plan section 9 gives them to two (medium, process)

Plan section 9 gives part C's tests to the doctest-expert and the code to the cpp-expert. Two records show one agent did both:

- the cost row on `develop`: `wu17-part-c  doctest/cpp (one agent)`;
- the mutation README: "in the test-author and implementer roles".

The README states it, which is better than part D did (that review's D1), but no record names it as a departure from the plan.

Decision 0185's mechanics were followed in form:

- The test groups were committed on the tests branch first (both at 08:36:03, before `counting.hpp` at 08:37:00).
- They were cherry-picked with `-x`, and the patches are identical.
- The failing test commits are labeled.

The separation that the mechanics protect was absent. The harm is limited: the expected counts come from `golden.py`, which shares no code with the library, and they equal spike-e's counts, which were checked independently. This is the second part in a row with this departure, so the coordinator should decide whether the plan's role split stands for the remaining parts.

**Resolution asked:** record the departure in the plan's outcome or the branch's records.

### L1. Intermediate commits fail checks their messages do not name; two of them are code commits (low, W2 departure)

The author's account is accurate. Results from `git archive` of each commit (✗ fails, · passes):

| Commit | Kind | Message names | `markers` (CTest) | format, new files | trace gate | clang-tidy (`a == a`) |
| --- | --- | --- | --- | --- | --- | --- |
| 17b2a5e | test | `support.counting` fails to build | ✗ | · | · | ✗ |
| 7de21ee | **code** | `support.counting` passes | ✗ | · | · | ✗ |
| 6ee2934 | test | `rga3.op_counts_*` fail | ✗ | ✗ | · | ✗ |
| 3aaf96c | test fix | `rga3.op_counts_*` fail | · | ✗ | · | ✗ |
| 8a6cd86 | **code** | the selection passes | · | ✗ | ✗ | ✗ |
| 79c7434 | test fix | none | · | ✗ | ✗ | · |
| 61b3b81 | test fix | none | · | · | ✗ | · |
| 286a322, b1be3e9 | | | · | · | · | · |

I did not run clang-tidy at the intermediate commits. That column comes from `79c7434`'s message and from the `a == a` line, which is present from 17b2a5e to 8a6cd86.

- **Test commits:** W2's exception covers a test commit that names the test it fails. These commits fail more than they name.
- **Code commits:** `7de21ee` leaves the CTest `tests.reference_markers` failing. The exception does not cover code commits, so that is a plain W2 departure. The trace gate, lint, and format failures at `8a6cd86` are W5 merge checks, not tests, but they are still unnamed.

Rewriting the branch's history is not an agent's to do (W7). The tip passes every check I ran.

**Resolution asked:** record the departure. Running the cheap gates (markers, format of the touched files, the trace gate) before each commit would have caught all of these.

### L2. `gap.toml` keeps a declaration and two reasons that the run showed false (low, records)

Keeping `sign_multiplied` at `expect = "survived"` preserves the prediction made before the run, which is honest. But it makes the spec a standing failure: every rerun exits 1, and a reader cannot tell a new regression from the known mismatch. Its reason, "every value is the same; only the count of operations changes", is also known to be false: `core.transform_rounding` (and, per M1, `core.rounding`) sees the extra rounding.

The prediction does not need to live in the spec. The README and git history keep it.

**Resolution asked:**

- Declare `sign_multiplied` `expect = "killed"`, with `killers = ["core.transform_rounding"]`.
- Add a TOML comment saying the plan listed it as a gap mutant and the gap run killed it.
- Do the same for the two other product mutants once M1's wider selection runs.
- Fix `gap.toml`'s header ("Each is expected to survive them").

### L3. The trace row files a structural fact under 4.4 (low, records)

The new row reads:

> 4.4 | each term is one product and each later term on a place one addition

That is a rule of evaluation, not a statement of 4.4's mathematics. The trace record defines a fact as "a short phrase of mathematics", and every one of the other 23 golden facts is a mathematical fact of its item.

4.4 does say that "in checking mode every discarded part is verified to be zero", and the checking-on counts pin that, which is the commit's reason. The fact as written does not say it.

**Resolution asked:** rephrase the namespace's fact, with its markers, to the part 4.4 states (for example, "with checking on, each blade a transform drops is compared with zero once"). Otherwise, say in the trace record that this row is a structure fact with no convention of its own.

### L4. `golden_sign_as_multiplication` is killed only by the existing staleness check (low, evidence)

`reference.golden` fails on any change to golden.py's output, and it existed before this unit. W3 counts a mutant written for a new test when that test is among those it fails. Here no new test was shown failing; the README says the regenerated header was not run against `op_counts`.

**Resolution asked:** a mutant of `golden.hpp`'s committed counts (one multiplication count + 1), killed by the `op_counts` program of that mode, would show the tests read the right namespace and mode.

### N1. `cancelling_pair` is the British spelling (nit)

The American spelling is "canceling". The word is in the mutant's name in both specs, the README, and the TSVs. Renaming it costs a rerun or a hand edit of four TSVs; renaming at the next rerun, which M1 asks for anyway, is enough.

## Points judged sound

- **The counting scalar (`test/support/counting.hpp`).**
  - It meets `ring_scalar`, and `exact_scalar` through `numeric_limits::is_exact`, with `==`. So `transform` takes its exact checking path: the checking-on program's counts (68, 84, 76, with 4, 2, 4 comparisons) pass, and `counting_without_comparisons` is killed by `rga3.op_counts_checking`.
  - It counts seven kinds separately.
  - A copy, a move, an assignment, and a read of `value` count nothing. `static_cast<T>` of a T result inside `accumulate` is a copy, so it counts nothing either.
  - It declares no `is_integer`, so the library's coefficient-range check treats it as an exact non-integer and skips the check. That is harmless for the ±1 coefficients here, and the header says inputs must stay small.
- **Two programs per preset.** The plan names one test, `rga3.op_counts`. Decision 0234 item 3 says the test asserts the set "matching the build's `GALOOT_CHECKING`". Each program's build fixes its own mode, so the reading is sound, and it is stronger: both sets are checked in every preset, the -O2 ones included. The pattern is the existing one (`core_assert_mode_*`, `core_transform_mode_*`): the programs take `include/` directly so the preset's definition does not collide, and the warning flags still arrive through `CMAKE_CXX_FLAGS`. The label `rga3.op_counts` gives the plan's single selection.
- **Independence (V1 kind 3).**
  - `golden.py` builds each term list from `conventions_check.py`'s blade products and checks it on seeded rationals against whole-multivector products.
  - It counts the lists by rule K and reads no library file.
  - It refuses a coefficient other than ±1 rather than pricing it.
  - The negation counts depend on term order and on the middle's orientation. Rule K fixes those in writing, and decision 0234 item 4 accepts the W9 consequence.
- **A structure check, not timing.** Decision 0013 and decision 0233 O10 (a) are followed: the test counts operations, says so in its header comment, and asserts no time.
- **Scope and text.**
  - Nothing under `include/` changes.
  - S11: no code or test text cites agent work or provisions; the only source cited is rigidgeometricalgebra.org.
  - American English throughout, except N1.
  - The new files pass `clang-format` and clang-tidy at the tip.
- **Merge note.** `feature/cost-evidence` has moved since `5444671`: part D merged at `3e5c4f5`, and lesson `809f505` followed. W7 asks the branch to merge its parent before its own merge. Expect overlaps in `test/CMakeLists.txt` and `runs.tsv`. `include/` is unchanged on both sides, so the mutants' anchors still hold.

## Cost

About 55 tool calls and about 45 minutes of wall time. I have no token count.
