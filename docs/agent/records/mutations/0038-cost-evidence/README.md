# Plan 0038 part C's mutants: the counting scalar and the operation counts

Run on 2026-11-10 (UTC) by Claude (agent, Opus 5.5, in both the test-author and implementer roles of plan 0038 part C; see Departures) with `tools/mutate`, every build and test through `tools/timed` with the shared job pool. Compilers: g++ 16.2.1 (gcc-debug), clang++ 23.1.4 (clang-debug). Both presets build with checking on; the counting tests are built in both modes in every preset (`rga3.op_counts_checking` and `_unchecked`, `core.transform_counts_checking` and `_unchecked`), so both sets of counts judge every mutant here.

- **Revision:** `feature/cost-evidence--counting` at `38a711f` with `gap.toml` and `counting.toml` as committed in the next commit (the rows say `38a711f07ff4+dirty`: those two files), after the merge of `feature/cost-evidence` (part D and lesson 0072).
- **Gap:** `tools/mutate gap.toml`, from 14:46 to 15:37, pool 23 of 23 free before and after, load average 1.8 at the start. Rows: `out/gap-<preset>.tsv`.
- **Tip:** `tools/mutate counting.toml`, from 15:37 to 16:36, pool 23 of 23 free before and after, load average 3.8 at the start and 5.2 at the end (other work on the machine). Rows: `out/counting-<preset>.tsv`.
- Builds and tests of both runs are the rows `mutant:<name>` of `records/performance/runs.tsv` on this branch.

These runs replace a first pair made at `286a322`, before the blind review, whose gap selection held only `core.product`, `core.transform`, and the rga3 components; the review found that `core.rounding`, outside that selection, already kills the three product mutants, and that `first_stage_all` is killable by a counting case in a form whose first stage is restricted. Git history keeps the first specs and rows. `bench.toml` and `out/bench-*.tsv` in this directory are part D's and are not described here. `tools/mutate` runs CTest one test at a time, so each mutant of the whole suite took about five minutes per preset.

## How the gap is judged

The gap's suite is the whole suite as the build presets run it, without this unit's tests: every test of `ctest --preset gcc-debug` (553 tests at `38a711f`, the tools' tests and part D's smoke tests among them) except `core.transform_counts_*`, `rga3.op_counts_*`, and `support.counting`. The selection names every component label of those tests, and lists every program they run. The tip's selection is the same with the three components of the counting tests added (558 tests). This unit changes no existing test source and nothing under `include/`, and `golden.hpp` only gains a namespace, so the suite without the new tests is the suite at the branch point; the run uses the tip's tree with the new tests left out of the selection rather than a second tree.

Before each implementation step, the test author built the counting programs by hand with GCC against scratch stand-ins of the golden counts, in both modes, and applied the gap mutants to the headers: for the rga3 tests, three of the first four failed them (not `first_stage_all`); for the degenerate case, `first_stage_all` failed the checking-off build (6 multiplications against 4). Those builds used scratch scripts and logs, so that step is not reproducible from the repository; the runs below are.

## Verdicts

| Mutant | Written for | Gap: whole suite without the counting tests (both presets) | Tip: with them (both presets) | Counting tests among the killers at the tip |
| --- | --- | --- | --- | --- |
| `product_kernel_zero_term` | listed in the plan as a gap mutant: a product kernel keeps one extra term with coefficient zero | killed by `core.rounding` | killed | both `rga3.op_counts` |
| `sign_multiplied` | listed in the plan as a gap mutant: `accumulate` multiplies by +1 and -1 instead of folding the sign | killed by `core.rounding`, `core.transform_rounding` (and on Clang a static assertion in `test/core/mv_test.cpp`) | killed | both `rga3.op_counts`, both `core.transform_counts` |
| `canceling_pair` | listed in the plan as a gap mutant: a product kernel adds a term t and its negation -t to one output | killed by `core.rounding`, `rga3.reference_kernels`, and four `rga3.benchmark_*` smoke tests | killed | both `rga3.op_counts` |
| `first_stage_all` | the gap: transform's unchecked path computes the whole first stage | survived | killed | `core.transform_counts_unchecked` |
| `double_negation` | the gap: a first term of sign +1 assigned as -(-value) | survived | killed | both `rga3.op_counts`, both `core.transform_counts` |
| `counting_without_<kind>`, seven mutants | the new tests: the scalar not counting multiplications, additions, subtractions, negations, comparisons, integer constructions, or default constructions | (not run) | each killed | `support.counting` kills all seven; the counting programs kill the first five (comparisons only with checking on) |
| `golden_sign_as_multiplication` | the new reference: `golden.py` counts a term of coefficient +1 or -1 as two multiplications | (not run) | killed | `reference.golden` (the committed header differs from what the script writes) |
| `golden_header_unchecked_count` | the new tests: the committed checking-off multiplications of the point transform one higher | (not run) | killed | `rga3.op_counts_unchecked` |
| `golden_header_checking_count` | the same for checking on | (not run) | killed | `rga3.op_counts_checking` |

Both runs exited 0: every verdict as declared, and every declared killer among the killers. The rows list every killer.

## Findings

- **The gap the counting tests close.** Two mutants survive the whole existing suite on both compilers and are killed by the counting tests: `first_stage_all`, which only `core.transform_counts_unchecked` sees, and `double_negation`, which both counting programs see. Neither changes a value, over an exact scalar or over double, and the tracking oracle counts no rounding for either: no test before this unit counts the first stage's terms at run time (the kernel-size tests of `transform_test.cpp` read the plan, which `first_stage_all` leaves alone), and none counts negations. The comparisons of the checking path are a third thing only the counting tests see (`counting_without_comparisons`).
- **The plan's three product mutants are not gap mutants.** The existing suite kills each of them: `core.rounding`'s tracking oracle counts the rounding of every product and addition the kernels make, so an extra term with coefficient zero, a sign multiplied in, or a canceling pair each change its count ("a tracked run counts the roundings of a product" among the failing cases). `canceling_pair` also moves double results by rounding, which part D's bit-exact smoke tests see. The specs now declare them killed, with the existing tests as killers and a comment that gives their history; the counting tests kill them too.
- **`first_stage_all` needed a case outside rga3.** In rigid(3) the restricted first stage of a motor on a point, a line, or a plane is whole (20, 36, and 28 terms; `golden.py` computes both lists, and they are equal), so the rga3 counts cannot see it. In the degenerate form with e1 . e2 = 1, for V on e1, e2, e3, e123 and x on e123, the first stage keeps 2 of its 4 terms, and with checking off the mutant makes 6 multiplications where `golden.py` gives 4.
- **The tests read the right counts.** Raising one committed count in `golden.hpp` fails the counting program of that mode only, so each program reads its own mode's set from the namespace.
- **Each kind the scalar counts has a test that sees it.** The integer and default constructions are seen only by `support.counting`: the operations counted form no integer constant, and the default constructions are not pinned.

## Departures

- **One agent wrote the tests and the code.** Plan 0038 section 9 gives part C's tests to the doctest-expert and its code to the cpp-expert, two agents. One agent did both, in turn, under the coordinator's brief. Decision 0185's mechanics were kept in form (each test group committed on `feature/cost-evidence--counting--tests` first, compiled against a scratch stand-in, and cherry-picked with `-x` before the code it judges), but the separation they protect was absent. What limits the harm: the expected counts come from `golden.py`, which shares no code with the library, and they equal Spike E's counts, which were checked independently; the degenerate case's counts were computed by `golden.py` the same way, and the blind review recomputed the rga3 ones.
- **Intermediate commits fail checks their messages do not name.** On this branch:
  - `17b2a5e` (test), `7de21ee` (code), and `6ee2934` (test) fail `tests.reference_markers` (a `;` inside the markers), and every commit from `17b2a5e` to `8a6cd86` fails clang-tidy in the lint preset (`a == a` in `support.counting`);
  - `6ee2934` to `79c7434` fail clang-format on the new tests;
  - `8a6cd86` (code) and the two after it fail the convention trace gate (the new golden fact's row came in `286a322`).
  `7de21ee` leaving a CTest failing is a departure from W2, which allows a failing commit only for a test that says so; the others fail merge checks (W5) their messages do not name. The tip passes all of them. Since then the cheap gates (reference markers, golden facts, `golden.py --check`, the trace gate, clang-format of the staged C++ files, and the citation check of staged code) have run before every commit of this branch and of the tests branch, and will continue to.
- **`first_stage_all` against plan 0038 item 3.** Killing it needs a transform outside the operations item 3 lists (the five DoD-24 operations, transform of a plane, and compose): `core.transform_counts` counts a transform in the degenerate form of `transform_test.cpp`. The coordinator asked for the case after the blind review.
