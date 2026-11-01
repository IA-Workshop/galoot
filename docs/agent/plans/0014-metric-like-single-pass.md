# 0014 metric_like in one pass, checked once per algebra

- Status: claimed 2026-11-01 (planner and implementer: Claude, agent, Opus 5.5, cpp-expert role, with the doctest-expert and build-expert role files for the tests and their registration). Implemented on the branch, awaiting the W5 review (Outcome below).
- Branch `feature/metric-like-single-pass`, worktree `.claude/worktrees/metric-like-single-pass`, from `develop` at the claim commit. Decision numbers 0122 and 0123 (0114 to 0116 belong to WU7b, 0130 to the job-pool work).
- Owner ruling behind the unit: decision 0121, accepted as option A′ on 2026-11-01. Evidence and proofs: research note 0015 (Theorems 1, 2, 4) and `docs/agent/records/performance/metric-like-check/`.
- Goal: `metric_like` reads each pair of Gram entries once, in one pass that checks symmetry and range together, and each algebra's verdict is computed once per translation unit and held in an `inline constexpr bool` variable template, reached only after a guard that reads `gram[0][0]`. No flat copy and no `std::bit_cast` (decision 0121, owner ruling). The concept accepts and rejects exactly the tags it does today, false and never an error.

## 1. Departures, ambiguities, and owner questions (C20)

Each item states the reading this plan follows.

1. "The old verdict" is read as today's three conditions: n ≥ 1, `is_symmetric()`, and `entries_in_range()`. The two members stay public and unchanged, and `from_gram` keeps its two checks with their separate messages, so the old verdict stays computable beside the new one in every test.
2. The exhaustive comparison is made at two levels, because the concept is a property of a tag type and one tag is one instantiation. For n = 1 and 2 it compares the concept itself, through one tag per matrix, at compile time (6 + 1,296 tags). For n = 3 it compares the pass the concept stores, `detail::is_valid_form`, at run time over all 6⁹ = 10,077,696 matrices; 6⁹ tags cannot be instantiated in a test build. What links the n = 3 run to the concept is the concept's definition (the stored value is `is_valid_form(Alg::value)`) and the n ≤ 2 compile-time comparison, which runs through that definition.
3. The value set has six values, two from each class an entry can fall in: below the range (`INT_MIN`, −M − 1), within it (−M, M), and above it (M + 1, `INT_MAX`), with M = `max_form_entry`. Note 0015 used seven (also 0). Two per class is what the covering argument needs (section 5, T1), and the run at n = 3 is then 10 M matrices rather than 40 M. Every value in range is a bound, so an off-by-one in either bound changes a verdict.
4. The test n ≥ 1 cannot change the verdict with libstdc++: without it, the guard reads `gram[0][0]` of a `metric<0>`, and libstdc++'s zero-size `std::array` subscript is not a constant expression, so the concept is still false (probe while planning, both compilers). The test stays: the standard does not require a library's precondition violation to end a constant evaluation, so without it the verdict for n = 0 would rest on one library's implementation, and it is what makes the diagnostic name the generator count. Its mutant is therefore killed by a new compile-fail test that pins that diagnostic (section 5, T5), a check of C9 that the library does not have today. The diagnostic text was read in the planning probe: GCC "`...generator_count) >= 1 [with Alg = none]' evaluated to 'false'", Clang "`...generator_count >= 1' (0 >= 1) evaluated to false"; with the test removed, both name the guard instead.
5. A mutant that removes the cache (the concept calls `is_valid_form` directly after the guard) is equivalent in verdict; only its cost differs. It is run and recorded, and is expected to survive every test; the P2 measurement (section 7) is what tells the two apart. A CTest that fails when the verdict is computed twice, like `core.plan_evaluated_once` for plans, is not part of this unit; it is listed as an open item.
6. "All seven presets at the final revision" is read as the final revision of the build inputs (headers, tests, build files, scripts the tests run). The outcome section, and any record commit after the seven-preset run, change records only; the outcome names the revision the run used and shows that the diff to the last commit touches only `docs/agent/`.
7. The owner's ruling on decision 0107 item 4 is recorded in decision 0121's owner-ruling section (claim commit), and a dated pointer to it is appended to decision 0107 on the branch. Decision 0118's last consequence ("stays open") is not edited; 0121's ruling closes that question.
8. User documentation gets no statement about programs that specialize Galoot's templates (owner ruling); it is an open item for the owner.
9. The P2 measurement runs note 0015's `measure.py` unchanged, through a small wrapper in a new records directory that points it at two trees, `before` and `after`, instead of the seven variants. `measure.py` itself is not edited.

## 2. Definition of done

- `include/galoot/core/metric.hpp`: `metric_like` is the type test, then n ≥ 1 from the type, then the one-entry guard, then the stored verdict `detail::is_valid_form_v<Alg>`; `detail::is_valid_form` is the one pass of note 0015 option (a), reading `m.gram[i][j]` through `std::array`. Every non-obvious line has a short plain comment, in particular why the guard reads one entry. The `///` comment of the concept says what it requires and that it is false, not an error, for a value that cannot be read at compile time, without the sentence "the guard reads every entry". No other header changes.
- The tests of section 5 exist and pass; no existing test or expected value changes.
- The mutants of section 6 are run on gcc-debug and clang-debug; each correctness mutant is killed by a test (a `static_assert`, a doctest case, or a compile-fail test), never by a warning; the cache mutant is recorded as an equivalent survivor.
- The P2 growth and the W10 costs are measured before and after and recorded.
- Decisions 0122 (and 0123 if needed) record the choices this plan leaves to the implementer; the plan's outcome is written.
- W5 blind review requested by the coordinator; merge by the coordinator.

## 3. Acceptance criteria

1. Same verdict. The concept's verdict equals the requirement on every 1 × 1 and 2 × 2 Gram matrix over the six values (T1), the pass equals both the requirement and the old verdict on every 3 × 3 one (T2), and every existing `static_assert` of `metric_test.cpp` holds unchanged.
2. False, not an error. Every tag whose value cannot be read at compile time gives `false` on both compilers: the five existing ones and the four new reference and template-parameter cases (T3).
3. Diagnostics. `core.plan_not_metric_like` passes unchanged; the new `core.metric_like_no_generators` passes (T5).
4. Mutants. Each mutant of section 6 except the cache mutant is killed on gcc-debug and clang-debug, and the record names the test that kills it (file and line of the failing `static_assert`, the failing CTest, or the instantiating test line of a hard error); a mutant killed only by a warning counts as a survivor.
5. P2. The growth of a fixed use from n = 4 to 16 is measured before and after, pinned, three repeats, both compilers, with and without the concept `static_assert` in the unit. Criterion stated before the run (P7): the after tree's figures equal note 0015's `cached` row (GCC +11.4 M and +11.5 M, Clang +11.5 M and +14.5 M; one check at n = 16 14.5 / 16.4 M) within that table's spread (1.3 M on GCC, 4.8 M on Clang), and the before tree's equal its `base` row within the same spread. A figure outside is traced to a cause or reported as unexplained.
6. W10. Build time, build memory, and test time of the whole suite (every core test includes `metric.hpp`) on fresh gcc-debug and clang-debug trees, before at the claim commit and after at the final code revision, with the rows copied by `copy_compile_rows` (decision 0080, second amendment).
7. Checks. Every commit passes, at that commit with a clean tree, the tests it can affect, clang-format and clang-tidy of each touched C++ file, and the cheap gates (`tests.reference_markers`, `tests.golden_facts`, `tests.citations`, `reference.golden`, `core.include_scan`, and the trace gate). The full gcc-debug suite runs at the end of the test-and-code pair; all seven presets build and pass from new, empty build trees at the final revision of the build inputs (departure 6), with the header sets and `format-check` on gcc-debug, clang-debug, and lint.
8. No citation of project records in code, comments, tests, or build files (STYLE S11; `tests.citations` and reading).

## 4. Approach

### The header

In `metric.hpp`, after the factories, in `namespace detail`:

- `template <std::size_t N> constexpr bool is_valid_form(const metric<N>& m) noexcept`: for i from 0 to N − 1 and j from 0 to i, read `a = m.gram[i][j]` and `b = m.gram[j][i]`, and return false when `a != b` or `a` is outside ±M. Comment: one pass over the pairs; when the two entries of a pair are equal, the range of one is the range of both, so every entry is covered (Theorem 1).
- `template <class Alg> inline constexpr bool is_valid_form_v = is_valid_form(Alg::value);` Comment: computed once per algebra in each translation unit, and every later check reads the stored value.

The concept:

```cpp
template <class Alg>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    (std::remove_cvref_t<decltype(Alg::value)>::generator_count >= 1) &&
    requires {
      typename std::integral_constant<entry_type, Alg::value.gram[0][0]>;
    } && detail::is_valid_form_v<Alg>;
```

with a comment above the guard saying, in plain words: the stored verdict is an error rather than false when the value cannot be read at compile time, so the guard first reads one entry; any entry can be read at compile time exactly when the whole value can, so one entry decides it for all (Theorem 2). The n ≥ 1 test reads only the type, so that `gram[0][0]` exists when the guard reads it. The order of the four conjuncts is what makes the stored verdict safe to reach (Theorem 4): a conjunction stops at its first false operand.

Unchanged: `metric<N>` and its members, the factories, `from_gram`, every other header.

### Commits

1. Tests that judge the change and pass on today's header: T1, T3, T4 in `metric_test.cpp`; T5's source and its registration in `test/CMakeLists.txt`. Listed for the review (W9). That they pass before the change shows they are correct for the concept as it is.
2. The header change, with T2, which uses the new `detail::is_valid_form` and so cannot build before it (one commit, W9; listed with its expected values).
3. Records: the mutation driver and its results; the P2 wrapper and its outputs; the W10 rows; decision 0122; the 0107 pointer; the outcome.

## 5. Tests

M = `max_form_entry` = 32767. S6 = (`INT_MIN`, −M − 1, −M, M, M + 1, `INT_MAX`).

- T1 (`static_assert`, `metric_test.cpp`). For every 1 × 1 and 2 × 2 matrix with entries from S6, `metric_like<tag>` equals `required(m)`, a predicate written in the test from the concept's stated requirement: every entry within ±M, and g_ij = g_ji for every i and j, read over all n² positions (not one triangle, so that it shares no shape with the pass). Expected: 0 disagreements; accepted 2 of 6 at n = 1 and 8 of 1,296 at n = 2. Reference: proof. A matrix is valid exactly when each diagonal entry is one of the two values in range and each mirrored pair holds one value in range twice, which gives 2^(n(n+1)/2) valid matrices: 2, 8, and 64 at n = 3. Coverage: both the requirement and the pass look at an entry only through its class and whether it equals its mirror; S6 has two values in each class, so every pattern of classes and equalities that an `int` matrix can show occurs over S6, and agreement on S6 is agreement on every `int` matrix of that size (note 0015, Theorem 1's exhaustive check, restated in the test's comment without the citation).
- T2 (doctest case, run time). Every 3 × 3 matrix over S6, 10,077,696 of them: `detail::is_valid_form(m)`, `required(m)`, and `m.is_symmetric() && m.entries_in_range()` agree. Expected: 0 disagreements; each accepts 64. Reference: the same proof. Time at -O0 while planning, with seven values (40 M matrices): 1.3 s on GCC; six values should take about a quarter of that.
- T3 (`static_assert`). False, not an error, for every value that cannot be read at compile time, and true for values that can, reached in ways the tests do not use today: a `static constexpr` reference to a constexpr metric (true); a `static constexpr` reference to a non-constexpr global metric at n = 2 and at n = 1 (false; at n = 1 the only entry is a square, so only the guard's read rejects it); a template parameter object, named by a static reference member (true). Reference: contract (the concept's comment), and the standard's rule that an entry can be read in a constant expression exactly when its complete object is usable in one ([expr.const]).
- T4 (`static_assert`). The widest metric a blade mask allows (n = 32), with one entry broken in the far corner (g_31,0 = 1, its mirror 0): false. Reference: proof, the form is not symmetric; it fixes that the pass reaches the last pair at the largest n.
- T5 (compile-fail, `core.metric_like_no_generators`, component `metric`). A class template constrained by `metric_like`, given a tag whose value is `metric<0>`, fails to compile, and the diagnostic contains `generator_count` and `metric_like`. Reference: contract, the concept requires n ≥ 1, and a rejection is reported in the terms of the algebra.
- Unchanged and rerun: every `static_assert` of `metric_test.cpp`, `core.plan_not_metric_like` (the diagnostic names `metric_like` and not `make_product_plan`), and every test that uses an algebra.

All new tests and expected values are listed in the review request (W9).

## 6. Mutations

A driver under `docs/agent/records/mutations/0014-metric-like-single-pass/`, modeled on the plan 0012 driver: apply one mutation to `metric.hpp`, build `all`, run the whole suite only if the build succeeded (lesson 0063), restore the header, and record the build exit, the CTest exit, the failing tests, the failing `static_assert`s as file:line, and the first error with the test line that instantiated it. A build that fails only on a `-Werror` diagnostic is recorded as "warning", not as a kill. Each anchor must occur once. Runs on gcc-debug and clang-debug; a second mode turns the `static_assert`s of `metric_test.cpp` into `static_assert(true ? true : ...)` so that the run-time case T2 is shown to kill the pass mutants by itself.

| Mutant | Change | Expected killer |
| --- | --- | --- |
| `no_symmetry` | drop `a != b` | T1, `asymmetric`; T2 |
| `no_range` | drop both bounds | T1, `entry_too_large`; T2 |
| `no_lower_bound` | drop `a < -M` | T1; T2 |
| `no_upper_bound` | drop `a > M` | T1, `entry_too_large`; T2 |
| `lower_bound_inclusive` | `a <= -M` | T1 (−M rejected); T2 |
| `upper_bound_inclusive` | `a >= M` | T1 (M rejected); T2 |
| `diagonal_skipped` | `j < i` | `entry_too_large` (n = 1); T1; T2 |
| `last_row_skipped` | `i + 1 < N` | `asymmetric`; T1; T4; T2 |
| `no_count_test` | drop n ≥ 1 | T5 |
| `no_guard` | drop the guard | the five existing unreadable-value `static_assert`s (a hard error in place of false) |
| `guard_reads_count` | guard on `generator_count` instead of an entry | the same |
| `no_type_test` | drop `is_metric_v` | `lookalike`, `int_value` |
| `no_cache` | call `is_valid_form(Alg::value)` in the concept | none expected (equivalent; departure 5) |

## 7. P2 measurement

`docs/agent/records/performance/metric-like-single-pass/measure_before_after.py` loads note 0015's `measure.py` by path, sets its trees to `before` (the claim commit's `include/`, from `git archive`) and `after` (the final code revision's), and runs it unchanged: concept units at n = 4, 8, 12, 16, 32 and the four typed rigid(3) plans at n = 4 to 16, with and without `static_assert(metric_like<alg>)`, GCC and Clang, Galoot's test flags at -O0, pinned to one performance core whose sibling is idle, three repeats interleaved, every compile through `tools/timed`. The machine is shared with WU7b; its load is read before the run and recorded, the rows record other work on the pinned CPU and its sibling, and wall times are not compared (P6, P11, P14). Outputs in `out/` beside the wrapper, with the revision in the directory name.

## 8. W10 costs

Fresh gcc-debug and clang-debug trees (`cmake --preset <p> -B build/w10-<when>-<p> --fresh`), built once at a committed revision with a clean tree; `ctest --preset`-equivalent run in each; rows copied by `copy_compile_rows`. Before: the claim commit (the header of `develop`). After: the final code revision. Reported: the build wall time, the sum of compile instructions, the peak memory of `metric_test.cpp`, `plan_test.cpp`, and `core.hpp` alone, and the CTest wall time. All shared-mode and track-only (C14).

## 9. Review

W5 blind review of the header change, the tests (T1 to T5), and the CMake registration, with this plan's criteria and the departures of the outcome; the reviewer gets note 0015 and decision 0121, not the author's view that the change is correct. The coordinator requests it and merges.

## 10. Cost estimate

Agent: about 150 to 250 tool calls and 0.6 to 1.2 M tokens (planning, three code and test commits, the mutation runs, two measurements, records). Machine, tracked only: the P2 run is about 500 pinned compiles on one core (about 10 to 15 minutes); W10 is four fresh builds; the mutation runs are 13 mutants × 2 presets × 2 modes, each an incremental build of the core tests and one suite run; the seven presets once from empty trees.

## Outcome (2026-11-01, Claude, agent, Opus 5.5, cpp-expert, implementer)

Status: done on `feature/metric-like-single-pass`, awaiting the W5 blind review. Code and tests are final at `a3cd8c5`; all seven presets ran from new, empty trees at `d959657`, whose build inputs are `a3cd8c5`'s (`git diff --name-only a3cd8c5 d959657` lists only files under `docs/agent/`). The commits after `a3cd8c5` change records only. Decision 0122 is used; 0123 is not.

### Departures, skipped checks, and ambiguities (C20)

1. Two commits fail a check and their messages say so; history is not rewritten. `93543c4` (the tests) fails the lint preset on two clang-tidy findings in the new test code (`bugprone-throwing-static-initialization` on the two non-constant metrics, `readability-trailing-comma`), fixed in `30d8391`. `6cc0545` (the code) fails `tests.reference_markers`, because the n = 3 test's reference listed a second source without its kind; fixed in `a3cd8c5`, a comment change. Every other test passed at `6cc0545` on both compilers, and lint built `all`, the header sets, and `format-check` clean there.
2. The `no_symmetry` mutant (drop `a != b`) cannot be shown killed by a test in the run-time mode: it leaves `b` unused, and `-Wunused-variable` under `-Werror` fails the build first, so the driver records it as "warning" there. In the normal mode the failing `static_assert`s are recorded beside the warning (lines 312, 326, 335 to 338, 341 to 342 of `metric_test.cpp`), so it is killed by tests. A second mutant, `mirror_read_twice` (read g_ij twice), removes the same check with no warning, and is killed in both modes on both presets. It was added after the first runs, and its results are from the driver at `99b8423`; the other results are from `1c07695`. The two driver revisions differ only in that mutant.
3. Checks not run at every commit. `30d8391` changed `test/CMakeLists.txt` (one registration). P8 asks for every test built under a changed build description; that commit ran `core.metric` on both compilers and the gates, and the full suite ran on both compilers at `6cc0545` and `a3cd8c5`. The record commits (`5a15540`, `1c07695` and after) change no build input and no file the gates read, so no tests ran at them except the seven presets at `d959657`. clang-tidy did not run on `metric_like_no_generators_fail.cpp`: it is a source that must not compile, and no lint target compiles it.
4. Runs outside `tools/timed` (P13): the planning probes in the scratch directory (the A′ prototype compiled against `metric_test.cpp`, the n ≥ 1 mutant and the n = 0 diagnostic, the seven-value cost probe, and the compile-fail source against the old, new, and mutant headers), `clang-format --dry-run` on touched files before each commit (the timed `format-check` ran at the commits), and a probe compile of WU7b's `plan_test.cpp` and `plan_map_not_metric_like_fail.cpp` at `feature/core-plan-maps` `50c7c70` against the new `metric.hpp` (item 8 of Open). None of them is cited as evidence below except as stated.
5. The machine was shared during the P2 run (load about 24, no performance core with an idle sibling when it started; WU7b was building). The script records per compile: the sibling thread was busy at most 0.3 s and other work on CPU 2 at most 0.02 s. Retired instructions of the compile do not count other processes' work; wall times are not used. The shared timing lock (`math-tools/workbench/timing.lock.d`) was held for the run.
6. The W10 trees are run with `ctest --test-dir`, not `ctest --preset`, because the test presets point at the default build directory; the sanitizer environment of the test presets was set by hand for the asan trees. The before trees were tested the same way.

### Done

| Commit | What |
| --- | --- |
| `c4b2e70` (develop) | Claim: this plan; decision 0121 accepted as A′ with the owner-ruling section |
| `5a15540` | W10 before rows, fresh gcc-debug and clang-debug trees at `c4b2e70` |
| `93543c4`, `30d8391` | Tests before the code (T1, T3, T4, T5), passing on the old header; the lint fix |
| `6cc0545`, `a3cd8c5` | The header change with T2; the reference-marker fix |
| `1c07695`, `99b8423`, `25e8371` | Mutation driver and results; `mirror_read_twice` |
| `53d34ca` | P2 before and after |
| `d959657` | Decision 0122; dated note on decision 0107 |
| `928a419` | Seven presets and W10 after rows at `d959657` |
| this commit | This outcome |

Tests and expected values for the review (W9): T1 (`metric_test.cpp:333` to `:342`, 0 disagreements, 2 and 8 accepted), T2 (the n = 3 test case, 0 disagreements, 64 accepted by each), T3 (`:320` to `:323`), T4 (`:326`), T5 (`core.metric_like_no_generators`, requires `generator_count` and `metric_like`). No existing test or expected value changed (`git diff c4b2e70 a3cd8c5 -- test` shows only additions).

### Acceptance criteria

1. Met at `a3cd8c5`, both compilers: T1 and T2 pass (0 disagreements; 2, 8, and 64 accepted), and every existing `static_assert` of `metric_test.cpp` holds. T1, T3, T4, and T5 also pass on the old header (`30d8391`).
2. Met: the five existing unreadable-value tags and `by_reference_to_runtime` (n = 2 and n = 1) give false, `by_reference` and `from_parameter` true, on both compilers.
3. Met: `core.plan_not_metric_like` and `core.metric_like_no_generators` pass in all seven presets at `d959657`.
4. Met. 14 mutants, `records/mutations/0014-metric-like-single-pass/`. In the normal mode on gcc-debug and clang-debug, all 13 correctness mutants are killed by tests, and `no_cache` survives as the equivalent mutant it is (decision 0122 item 5):

   | Mutant | Killed by (`metric_test.cpp` lines, or CTest) |
   | --- | --- |
   | `no_symmetry`, `mirror_read_twice` | `asymmetric` 312, `widest_broken` 326, T1 335 to 342 |
   | `no_range`, `no_upper_bound`, `diagonal_skipped` | `entry_too_large` 313, T1 |
   | `no_lower_bound`, `lower_bound_inclusive`, `upper_bound_inclusive` | T1 only (335 to 342) |
   | `last_row_skipped` | 312, 313, 326, T1 |
   | `no_count_test` | `core.metric_like_no_generators` only |
   | `no_guard`, `guard_reads_count` | hard errors reached from the unreadable-value tags, 307 to 310 and 322 to 323 |
   | `no_type_test` | hard error reached from `lookalike`, 306 |
   | `no_cache` | none (survives) |

   In the run-time mode (the `static_assert`s of `metric_test.cpp` neutralized), every mutant of the pass is killed by the n = 3 test case on both presets, except `no_symmetry`, which the warning stops first (departure 2); `upper_bound_inclusive` also fails `core.plan_overflow`. Three mutants are killed only by the tests this unit adds (the two inclusive bounds and the lower bound by T1, the count test by T5).
5. Met. Pinned on CPU 2, three repeats, before `c4b2e70` and after `a3cd8c5` (`records/performance/metric-like-single-pass/out/p2-c4b2e70-a3cd8c5/`), against note 0015's rows and the spreads stated in section 3:

   | Measure (M instructions) | GCC before | GCC after | Clang before | Clang after |
   | --- | --- | --- | --- | --- |
   | Four plans, growth n = 4 to 16, no `static_assert` | +17.6 | +12.1 | +23.3 | +14.3 |
   | The same, `static_assert` in the plan unit only | +18.3 | +11.6 | +46.3 | +17.0 |
   | One check at n = 16 (`c-use1`) | 21.4 | 14.5 | 24.6 | 15.1 |
   | One check at n = 32 | 77.2 | 50.2 | 96.4 | 56.7 |
   | Note 0015, `base` / `cached` rows, first two lines | +17.7, +18.5 | +11.4, +11.5 | +20.8, +42.8 | +11.5, +14.5 |

   Every before figure is within the spread of the `base` row and every after figure within the spread of the `cached` row (largest difference 2.8 M on Clang, spread 4.8 M). With the `static_assert` in both units, GCC's growth stays at 0.0 to 0.4 M (its constant cache), and Clang's falls from +24.8 M to +2.0 M: Clang no longer evaluates the concept twice. The header's own cost does not move: `c-base` is within 1 M before and after at every n.
6. Met; shared mode, track-only (C14). Fresh trees, built once:

   | | gcc-debug before `c4b2e70` | after `d959657` | clang-debug before | after |
   | --- | --- | --- | --- | --- |
   | Compiles; instructions, all compiles | 76; 105.0 G | 77; 107.1 G | 77; 106.7 G | 78; 112.0 G |
   | `metric_test.cpp` | 4.24 G, 134 MiB | 5.84 G, 166 MiB | 2.28 G, 132 MiB | 7.53 G, 153 MiB |
   | `plan_test.cpp` | 21.37 G, 332 MiB | 21.36 G, 331 MiB | 23.26 G, 290 MiB | 23.03 G, 291 MiB |
   | Build of `all`, wall | 3.7 s | 3.7 s | 3.3 s | 5.2 s |
   | CTest, whole suite, wall | 131 tests, 16.3 s | 132 tests, 18.8 s | 132 tests, 20.9 s | 133 tests, 21.5 s |

   The rise is the new tests: T1's 1,302 tags cost `metric_test.cpp` +1.6 G instructions on GCC and +5.2 G on Clang, and the one new compile-fail test is the extra compile. The library's own saving is per algebra and unit (about 7 M on GCC and 9 M on Clang at n = 16, criterion 5), too small to see in these totals. The test time of `core.metric` is 1.0 s on clang-debug and 0.7 s on gcc-asan (T2's 10 M matrices).
7. Met with the departures of items 1 and 3: each commit's checks ran at that commit with a clean tree (the run log is the only uncommitted file each time). Seven presets from new, empty trees at `d959657`: gcc-debug, gcc-release, gcc-asan 132 of 132; clang-debug, clang-release, clang-asan, lint 133 of 133; header sets and `format-check` clean on gcc-debug, clang-debug, and lint; lint's clang-tidy clean; trace gate exit 0.
8. Met: `tests.citations` passes at `a3cd8c5` and `d959657`; the header and test comments were read for citations.

### Verification and validation

Verification: performed and passed at `a3cd8c5` (the exhaustive comparisons T1 and T2, the reference cases, the mutants, the suite in seven presets). Validation: not applicable; the concept's meaning did not change. Reproduction from the worktree root with the pool exported: `python3 -I docs/agent/records/mutations/0014-metric-like-single-pass/mutate.py <preset> [--runtime] [mutation...]`; `python3 -I docs/agent/records/performance/metric-like-single-pass/measure_before_after.py $PWD c4b2e70 a3cd8c5 <new-out-dir> <cpu> 3`; `python3 -I docs/agent/records/performance/metric-like-single-pass/w10_summary.py docs/agent/records/performance/compile.tsv docs/agent/records/performance/runs.tsv c4b2e70 d959657 w10-before final:all`.

### Open

1. For the owner: the statement in the user documentation about programs that specialize Galoot's templates (decision 0121, question 4) is deferred, as ruled.
2. A CTest that fails when one algebra's form is read twice in a unit would be the only test that kills `no_cache`; it is not built (decision 0122 item 5).
3. `metric_test.cpp` now costs about three times as much to compile on Clang (T1). If that is unwanted, the n = 2 part of T1 can run at run time on the pass, as T2 does, at the cost of comparing the concept itself only at n = 1.
4. The n ≥ 1 test and T5 should be rechecked on a second standard library when one is added (decision 0122, consequences).
5. Merge with WU7b: both branches add registrations to `test/CMakeLists.txt` (in different places) and rows to `compile.tsv` and `runs.tsv`. WU7b adds `core.plan_map_not_metric_like`, whose tag is `int` and so fails at the type test as before; the probe of departure 4 compiled WU7b's `plan_test.cpp` and gave that source's expected diagnostic (names `metric_like`, not `make_unary_map`) on both compilers with this branch's header. Not run as WU7b's CTest.
6. Knowledge base: the topic is covered by the unmerged branch `kb/validated-compile-time-values` (another agent's); no note was added. A possible addendum for its owner: the size test before a one-element guard is invisible in the verdict with libstdc++ (a zero-size `std::array` subscript is not a constant expression) but still needed for portability, and its mutant can be killed by testing the diagnostic.

Agent cost: about 125 tool calls; token counts are not visible to me.
