# Delta review of plan 0038 part C's review fixes (degenerate counts, whole-suite gap, records)

- **Reviewer:** blind-reviewer role (a general-purpose agent), Claude Opus 5.5, 2026-11-10. I share no working context with the author or with the first reviewer, and I did not edit the change under review.
- **Range:** `feature/cost-evidence--counting` from `b1be3e9` to `0dfc35a`, minus what the merge `38a711f` brought in from `feature/cost-evidence` (part D and lesson 0072). The author's own commits are `4bb9195` (cherry-pick of `45618d1` on `feature/cost-evidence--counting--tests`), `5295358`, `bc8a7e2`, `5354189`, and `0dfc35a`, and I checked the merge for what it kept. I reviewed in the worktree `.claude/worktrees/wu17-part-c-delta`, on `records/wu17-part-c-delta`, branched from `0dfc35a`. Its build trees are `build/gcc-debug` and `build/clang-debug`.
- **Statement of intent:** plan 0038 section 4 items 3 and 11; decisions 0234 and 0236 (items 5 to 7); the first blind review, `reviews/2026-11-10-wu17-part-c-blind.md` on `develop` (M1 to M3, L1 to L4, N1).
- **Departures handed to me:** through the brief only. The author says it briefly deleted part D's mutation rows (`out/bench-*.tsv`) and restored them before committing. The README's Departures section lists the rest: one agent wrote the tests and the code, intermediate commits failed unnamed checks, and `first_stage_all` needs a transform outside the operations item 3 lists.

## Outcome

**Merge. Every finding of the first review is resolved or recorded. The new findings are wording only (R1 low, R2 and R3 nits), and none blocks.**

- The new test `core.transform_counts_*` is independent of the library. Its expected counts, 4, 1, 1, 0 with checking off and 16, 8, 2, 3 with checking on, equal my hand derivation from the definitions (below).
- `first_stage_all` and `double_negation` are real gap mutants. My probes and the author's whole-suite rows show both survive the existing tests and are killed at the tip, on both compilers.
- The records are honest. Part D's rows are intact, the merge kept every row of `runs.tsv`, and nothing under `include/` changes.

## Checks run

All runs went through `tools/timed`, and their rows are in this branch's `runs.tsv`. Pool status: running, 23 of 23 free before the work (load 0.97), and 23 of 23 free after (load 1.81). I neither started nor stopped the pool. I made no timing measurement, did not touch `master`, changed no processor setting, and made no web request.

| Check | Result |
| --- | --- |
| `git diff --stat b1be3e9 0dfc35a -- include/` and `5444671 0dfc35a -- include/` | empty |
| `git diff --stat 38a711f 0dfc35a` | records only: the mutation specs, README, TSVs, and `runs.tsv` |
| Cherry-pick `4bb9195` against `45618d1`, by `git patch-id --stable` | identical patch, with its `-x` line |
| Configure gcc-debug and clang-debug; build the five counting targets | exit 0 in both |
| `ctest -L '^(core\.transform_counts\|support\.counting\|rga3\.op_counts\|reference\.golden\|tests\.references\|tests\.citations)$'` | 10 of 10 pass in both |
| `ctest -N` with `gap.toml`'s and `counting.toml`'s label regexes | 553 and 558 tests in both presets; 558 is the whole preset run, and the 5 left out of the gap are exactly the counting programs |
| Cheap gates at each of `4bb9195`, `5295358`, `bc8a7e2`, `5354189`, `38a711f`, `0dfc35a`, in a detached scratch worktree (`2026-11-10-wu17-part-c-delta/gates.sh`): `markers`, `golden` facts, `golden.py --check`, the trace gate, `citations` (as `tests.citations` runs it), `clang-format --dry-run -Werror` on the new test | all pass at every commit |
| `tools/mutate` on a narrow probe spec of my own (`2026-11-10-wu17-part-c-delta/probe.toml`, rows in its `out/`): the two gap mutants against the existing tests most likely to see them, and against the counting programs | see "The two gap mutants" |
| `runs.tsv` across the merge, by sorted line comparison | see "The merge and the records" |
| `tools/runlog check` at this branch's tip, with this review's rows | exit 0 |
| `clang-tidy -p build/clang-debug --warnings-as-errors='*'` on `test/core/transform_counts_test.cpp` (both programs' entries) | exit 0 |
| Grep of the author's added text (the delta's diffs and messages) for British spellings and for citations of agent work or provisions in `test/` | no citation, and no British form the delta introduces (see "Points judged sound") |

### Not checked

- The full suite and whole-suite mutation runs. Each `tools/mutate` run of `gap.toml` or `counting.toml` takes about an hour, and the brief rules them out. For the verdicts of the three product mutants and of the counting-scalar and header mutants I rely on the author's `out/*.tsv` rows. I read all four files: every verdict equals its declaration, no row lists a problem, and every declared killer appears among the row's killers.
- The lint, asan, release, and measure presets. The author's rows at `38a711f` show the counting targets built and tested in all of them, lint included, with exit 0.
- `golden.py`'s degenerate algebra was not rerun by a second implementation. The hand derivation below covers its output for this one case.

## The degenerate counts, derived by hand

Take the Gram form with e1 . e2 = 1 and every other product of generators 0, and the wedge basis e12 = e1 ^ e2 = e1 e2 - 1, e13 = e1 e3, e23 = e2 e3. Since e2 e1 = 2 - e1 e2 and e3 anticommutes with e1 and e2, the products of bivectors are:

| | e12 | e13 | e23 |
| --- | --- | --- | --- |
| **e12** | 1 | e13 | -e23 |
| **e13** | -e13 | 0 | 0 |
| **e23** | e23 | 0 | 0 |

For example, e12 e12 = e1 e2 e1 e2 - 2 e1 e2 + 1, and e1 e2 e1 e2 = e1 (2 - e1 e2) e2 = 2 e1 e2, so e12 e12 = 1. For three generators the left and right complements agree: e1 → e23, e2 → -e13, e3 → e12, e123 → 1, and back. The antireverse negates grade 1 and keeps e123, with sign (-1)^(a(a-1)/2) for antigrade a.

**First stage, V ⟇ x.** right(x e123) = x, so V ⟇ x = V x: four terms, one per place of V, each a single blade with sign +1. The middle is e1, e2, e3, e123.

**Second stage, M ⟇ antireverse(V).** The right complements of antireverse(V)'s oriented blades are -e23, e13, -e12, and 1. Of the 16 pairs, 12 give a term:

- **On e123,** the blade x is on: (e3, e3) with sign -1 and (e123, e123) with sign +1. These read only the middle places e3 and e123.
- **Dropped, on e1:** (e1, e3) -1, (e1, e123) +1, (e3, e1) +1, (e123, e1) -1.
- **Dropped, on e2:** (e2, e3) +1, (e2, e123) +1, (e3, e2) -1, (e123, e2) -1.
- **Dropped, on e3:** (e3, e123) +1, (e123, e3) -1.

Count by rule K: each term is one multiplication, each later term on a place is one addition or subtraction, and a first term of sign -1 is one negation.

- **Checking off.** The first stage keeps the 2 middle terms the kept terms read, and the second stage computes the 2 terms on e123: 4 multiplications, 1 addition, 1 negation (from (e3, e3) coming first), 0 comparisons.
- **Checking on.** The first stage keeps all 4 terms and the second computes all 12: 16 multiplications and 0 + 1 + 3 + 3 + 1 = 8 additions and subtractions. The negations are 2, from the first terms on e123 and on e1. The 3 comparisons are one per dropped blade.

Both sets equal `golden::operation_counts::degenerate_transform`, `{{4, 1, 1, 0, 0}, {16, 8, 2, 3, 0}}`. They also satisfy the identity the first review used: additions = terms − places written, 4 − (2 + 1) = 1 and 16 − (4 + 1 + 3) = 8. With V the antiscalar, as the test takes it, every dropped coefficient is 0, so the checking program's check passes.

`golden.py` gets these from its own `complements(3)` and `grade_signs(3, ...)`, which build the complements and antireverse signs from their definitions in R(3, 0, 0), and from `clifford_reference.py`'s product for the `degenerate_3` form of `GP_TABLE_FORMS`. It reads no library file. Its term lists are checked against its own whole-multivector antiproducts on seeded rationals. That check shares `golden.py`'s `antigp` with the lists, so it validates the restriction and the placement, not the product; the hand derivation covers the product for this case.

**The `blade_terms` change leaves the rga3 counts unchanged.** The function now lists every blade of a pair's product, in the result order, where it used to raise on a product of more than one blade. In rigid(3), a diagonal form, every product of two blades is one blade, so each list is unchanged. The order of terms on any one place is still the order of the pairs, so the negation counts cannot move either. `golden.hpp`'s diff confirms it: only the comment and the new `degenerate_transform` line change, and `golden.py --check` passes at every commit of the delta.

## The two gap mutants

My probe used `tools/mutate` with each mutant judged twice, once by each of two selections:

- **existing:** `core.rounding`, `core.identities_rounding`, `core.transform_rounding`, `core.transform`, `core.transform_identities`, `core.product`, `core.identities`, `rga3.motions`, `rga3.reference_kernels`, and `rga3.benchmarks`, 123 tests, declared `survived`;
- **counting:** `core.transform_counts` and `rga3.op_counts`, declared `killed`.

| Mutant | Existing (gcc-debug / clang-debug) | Counting (gcc-debug / clang-debug) |
| --- | --- | --- |
| `first_stage_all` | survived / survived | killed by `core.transform_counts_unchecked` / the same |
| `double_negation` | survived / survived | killed by all four counting programs / the same |

The existing selection holds every test that killed one of the five gap-spec mutants in the author's rows (`core.rounding`, `core.transform_rounding`, `rga3.reference_kernels`, the benchmarks; not `core.mv`, whose Clang-only static assertion catches `sign_multiplied`), plus the value suites of products and transforms. The probe exited 0 on both presets, every verdict as declared. It is a sample, not the whole suite. For the rest of the 553 tests I rely on the author's gap rows, where both mutants survive on both compilers with nothing unjudged or not run.

At the tip, the author's rows give the same killers on both compilers:

- `first_stage_all` is killed by `core.transform_counts_unchecked` alone. That is right: with checking on, the first stage is whole anyway, 16 multiplications either way.
- `double_negation` is killed by both `core.transform_counts` and both `rga3.op_counts` programs.

So both meet plan item 11's test for a gap mutant (survives without the new tests, killed with them), judged against the whole suite as decision 0236 item 6 asks.

`tools/mutate` refuses to run a selection whose tests run a program no target of the selection builds, so a gap selection cannot judge a mutant with a stale program. My first probe was refused exactly that way (`rga3.rotation_checking_zero` needs `rga3_rotation_mode_1`). The author's specs passed that check, and their `not_run` columns are empty except for the two Clang objects that `sign_multiplied` breaks (`core.linear`, `core.mv`), which its README row states.

## The first review's findings

| Finding | Resolution asked | What the delta does | Judged |
| --- | --- | --- | --- |
| M1, the gap judged by a narrow selection | widen the gap selection, rerun, declare the product mutants killed, add a true gap mutant, restate the README | `gap.toml` and `counting.toml` select the whole preset suite (553, 558); the three product mutants are `killed` with existing killers; `double_negation` added; README restated | resolved |
| M2, `first_stage_all` not equivalent | a degenerate counting case, or record the survivor | `core.transform_counts_checking` and `_unchecked`; the mutant is killed at the tip; the reason in `gap.toml` now matches | resolved; counts verified by hand |
| M3, one agent in both roles | record the departure | README Departures; decision 0236 item 5 accepts it for part C, with the reviews' probes standing in | recorded. The degenerate case repeats it: `45618d1` (test) and `5295358` (counts) were committed 83 seconds apart by the same agent. The commit message of `45618d1` says what its scratch stand-in could and could not show. My hand derivation is the independent check of the expected values. |
| L1, intermediate commits fail unnamed checks | record the departure | README Departures, with the commits and checks; the author now runs the cheap gates before each commit | recorded. Every commit of this delta passes all six cheap gates. `4bb9195` fails to build and names the two tests it fails, as W2 allows. |
| L2, a standing failure in `gap.toml` | declare `sign_multiplied` killed with killers, comment, fix the header | done for all three product mutants; the header now says which mutants survive | resolved; R2 is a nit on one comment |
| L3, a structure fact under 4.4 | rephrase, or say in the trace record that the row is structural | a rule in `convention-trace.md` names the row as a fact of structure and says why it sits under 4.4 | resolved by the second option; decision 0236 item 7 accepts the row |
| L4, `golden_sign_as_multiplication` only killed by the staleness check | a header mutant killed by the program of its mode | `golden_header_unchecked_count` and `golden_header_checking_count`, each killed by that mode's `rga3.op_counts` program only, on both compilers | resolved |
| N1, `cancelling_pair` | rename at the next rerun | renamed `canceling_pair` in both specs, the README, and all four TSVs | resolved |

## The merge and the records

- **Part D's rows.** `bench.toml`, `out/bench-gcc-debug.tsv`, and `out/bench-clang-debug.tsv` at `0dfc35a` are byte-identical to part D's tip `60962e9` and to `feature/cost-evidence` at `809f505`. No commit in `b1be3e9..0dfc35a` touches them. The deletion the author admits left no trace in the tree.
- **`runs.tsv` across the merge.** First parent `5354189`: 19,642 rows. Second parent `809f505`: 19,638. Their union: 19,766. The merge `38a711f` has 19,767 rows: every row of both parents, plus the merge's own `tools/runlog check` row (on the dirty tree before the merge commit). The tip adds 126 rows and removes none, and it has no duplicate row.
- **README revision note.** The rows say `38a711f07ff4+dirty`, and the README says the dirt is the two specs as committed next. `git diff --stat 38a711f 0dfc35a` shows only records, so the code the mutants ran against is the tip's.

## Findings, most severe first

### R1. The README says the gap's suite is the suite at the branch point; after the merge it is larger (low, records)

> This unit changes no existing test source and nothing under `include/`, and `golden.hpp` only gains a namespace, so the suite without the new tests is the suite at the branch point

After `38a711f`, the 553 tests include part D's `rga3.reference_kernels` and the seven `rga3.benchmark_*` smoke tests, which `5444671` did not have. The same paragraph says part D's smoke tests are among the 553, so the two sentences disagree. "Changes no existing test source" is also loose: `golden.py`'s `blade_terms` changed, though not its output for any existing namespace.

The conclusions stand. A mutant that survives the larger suite survives any part of it. The kills of the three product mutants rest on `core.rounding`, which was there at the branch point, so they hold at the branch point too.

**Resolution asked:** say the gap is judged by the whole suite at `38a711f`, without the counting tests, which includes part D's tests. Decision 0236 item 6 asks for exactly that reading. A records-only edit, no rerun.

### R2. `canceling_pair`'s spec comment names one way it is killed; the run shows two (nit, records)

`gap.toml` and `counting.toml` say the whole suite kills it "through the tracking oracle's count of roundings". The gap rows on both compilers also name `rga3.reference_kernels` and four `rga3.benchmark_*` tests, and the README and the commit message say so. The comment is true but incomplete, and the declared killers (`core.rounding` only) are a valid subset.

**Resolution asked:** optional. Add "and part D's bit-exact smoke tests" to the comment.

### R3. The new test case's name is true only with checking off (nit, S11)

"transform(x, V) in a degenerate form computes only the first-stage terms its result reads" is also the case's name in `core.transform_counts_checking`. There the transform computes all four first-stage terms, as 4.4's check requires, and the case asserts 16 multiplications. The file's header comment explains the two modes, but a failure message from the checking program states something that program does not test.

**Resolution asked:** optional. Use a name true in both builds, in the pattern of the rga3 cases, such as "transform(x, V) in a degenerate form makes the reference's operations".

## Points judged sound

- **The test.**
  - It counts one call over the counting scalar, in the degenerate form of `transform_test.cpp`, and compares all five pinned kinds with the reference's set for its build's mode.
  - Its CMake block follows the `rga3.op_counts` pattern: two programs per preset that take `include/` directly, with `GALOOT_CHECKING` fixed per program, and the zero-cases guard.
  - Its `Reference:` marker states the counting rule and the case in words, cites nothing, and passes `markers` and `golden` facts.
  - It passes clang-format at every commit.
- **Independence (V1 kind 3).** The expected values come from `golden.py`, which reads no library file (its imports are the standard library, `clifford_reference`, `conventions_check`, and `rigid_motions`), and they equal a derivation by hand from the definitions.
- **The specs.**
  - The gap and tip selections differ by exactly the five counting programs and their three labels.
  - Both list every component label of the preset run, and `tools/mutate`'s check confirms each test's program is a target.
  - Declarations match the rows, and the reasons of the two surviving gap mutants state the mechanism (no value changes; nothing before the counting tests counts first-stage terms or negations).
- **Scope and text.**
  - Nothing under `include/` changes.
  - The author's new code, test, and script text cites no agent work or provision.
  - American English throughout the author's text. The only British forms in the diff are the existing labels `tests.catalogue` and `tools.catalogue`, inside the selection regexes, which this delta does not introduce.

## Cost

About 60 tool calls and about 20 minutes of wall time, of which the probe took 7 minutes. I have no token count.
