# 0005 WU3: metric

- Status: closed 2026-10-31 with the outcome below (implementation done on `feature/core-metric`; the W5 review and the merge into `develop` are the coordinator's); branch `feature/core-metric`, worktree `.claude/worktrees/core-metric`
- Goal: `include/galoot/core/metric.hpp`, the signature as data (C2, C5).
- Content: a symmetric integer Gram matrix over N generators held in a structural value, with the signature carried by a tag type that holds its value (decision 0021, STYLE S7); the largest N is a template parameter, not a constant (decision 0004), with 1 ≤ n ≤ 16 as the v1 default and n = 0 rejected; elements are `int`, not `int8_t` (note 0010); factories `R(p, q, r)`, `rigid(d)`, and `conformal(d)` (CONVENTIONS 1.2 to 1.4); a concept `metric_like` and accessors; no basis names inside the metric (architecture finding, plan 0001).
- Definition of done: the header exists, is self-contained, documented (S11), and the tests pin 1.2, 1.3, and 1.4.
- Acceptance criteria: (1) each factory's matrix equals the matrix in CONVENTIONS for n up to 6, written out in the test from the definition; (2) generation-time checks (V4) reject an asymmetric form, p+q+r ≠ N, an entry out of range, and n outside the bound, each with a compile-fail test on both compilers that requires the literal message (C9; the catch-all form of decision 0033 where a function is called); (3) one accept and one reject `static_assert` per concept (S7); (4) `is_diagonal` and the null-generator position are tested on rigid(3) and conformal(3); (5) conformal(3) differs from R(4,1,0) as a signature but the test pins the exact change of basis of 1.4 as data only (the conversion itself is WU12); (6) lint, format, and include scan pass; tests pass on both compilers.
- Approach: direct form; `consteval` factories returning the structural value; no dependence on `bits.hpp` unless needed.
- Evidence: test output per preset and the P1 before and after record.
- Agent cost: tracked for information only (decision 0019).
- Review: W5 blind review before the merge into `develop`.
- Open questions: whether the tag-and-value split costs more than Spike A measured for the real types; the unit reports it.

## Outcome (2026-10-31, Claude, agent, Opus 5.5)

### Departures and ambiguities (C20)

1. The interface choices the plan left open are recorded in decision 0060: `R<N>(p, q, r)` with N explicit (the reading under which "p + q + r ≠ N" exists), a `MaxN` template parameter defaulting to `max_generators = 16`, `from_gram({...})` for general forms, entry range |g| ≤ `max_form_entry` = 32767, 0-based `is_null(i)` as the "null-generator position", and no catch-all (no function in this unit takes a tag).
2. Two checks beyond criterion 2: negative p, q, or r is rejected (`core.metric_negative`), because the split check alone would accept R<3>(4, 0, −1).
3. "Entry out of range" had no defined range. 32767 is my choice (decision 0060, item 3) and is not a guarantee against overflow in later product tables.
4. "elements are `int`, not `int8_t` (note 0010)": note 0010 does not mention `int8_t`. I followed the plan's statement as given.
5. Style rulings arrived mid-unit (decision 0034, then the tightened STYLE S11 at `f62ec2f`). Every file this unit wrote restates its conventions in place and cites no record, provision, or CONVENTIONS section. Old files were not cleaned, as directed.
6. Two clang-tidy checks (`cppcoreguidelines-pro-bounds-constant-array-index`, `cppcoreguidelines-pro-bounds-avoid-unchecked-container-access`) are suppressed in one block per file, with the reason, in `metric.hpp` and `metric_test.cpp`; two more are suppressed at single lines (`readability-identifier-naming` for `R`, `avoid-c-arrays` for the deduced braced parameters, `readability-use-anyofallof` for one loop). Every later core header that indexes by generator will meet the first two; a project-wide exclusion is the owner's decision.
7. `test/CMakeLists.txt`, `CMakeLists.txt` (the header set), and `include/galoot/core.hpp` (the umbrella include) are shared with WU2. This unit added only: one `galoot_add_test(MODULE core COMPONENT metric)`, six `galoot_add_compile_fail_test(NAME metric_* ...)` calls and their comment, the line `include/galoot/core/metric.hpp` in `target_sources`, and `#include <galoot/core/metric.hpp>` in `core.hpp` (whose comment was reworded because it said the umbrella was empty).
8. Lesson, kept here to avoid a number clash with lessons written by parallel units: the first mutation script hid the build output, so two mutations that broke the build ran the previous binary and "passed". The rerun showed both caught at compile time by the test's `static_assert`s. A mutation run must check the build's exit status before reading the tests.

### Acceptance criteria

All runs on `feature/core-metric` at `1dc00cb` (code identical to `6f458b0`; `1dc00cb` adds only records), from the worktree root with `eval "$(tools/jobpool env)"`, through `tools/timed` (rows in `runs.tsv`).

| # | Criterion | Evidence | Command | Met |
| --- | --- | --- | --- | --- |
| 1 | Each factory's matrix equals the definition for n ≤ 6 | `core.metric` cases 1 to 4: every split of R(p, q, r) for n = 1 to 6, rigid(d) for d = 0 to 5, conformal(d) for d = 0 to 4, each entry against an expected_* function in the test that restates the definition; literal matrices for R(1,1,1), R(2,2,2), R(4,1,0), rigid(3), conformal(3) | `ctest --preset <p> -L '^core\.metric$'` | Yes |
| 2 | Compile-time rejection with literal messages, both compilers | `core.metric_asymmetric`, `_split`, `_negative`, `_entry_range`, `_zero`, `_bound`, each requiring its literal plus `assertion_failed` or `static assertion failed`, pass under gcc-* and clang-* presets | as above | Yes; the catch-all form does not apply (departure 1) |
| 3 | One accept and one reject `static_assert` per concept | `metric_like`: 3 accepts (rga3, cga3, r17 with MaxN 32), 8 rejects (no value, int value, `int`, `metric<4>` itself, asymmetric, entry 32768, n = 0, n = 17 at default MaxN) in `metric_test.cpp` | compiles in every preset | Yes |
| 4 | `is_diagonal` and the null generators on rigid(3), conformal(3) | `core.metric` case 5, at run time on copies and by `static_assert` | as above | Yes |
| 5 | conformal(3) differs from R(4,1,0); change of basis pinned as data | `core.metric` case 6: `conformal<3>() != R<5>(4,1,0)`; the doubled basis matrix for d = 3 written out; (2B)ᵀ diag (2B) = 4G for d = 0 to 4 | as above | Yes |
| 6 | Lint, format, include scan; tests on both compilers | `cmake --workflow --preset` for gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint: 17 of 17 each; lint runs clang-tidy, `all_verify_interface_header_sets`, `format-check`; `core.include_scan` and `core.bare_compile` (which includes `core.hpp`, so `metric.hpp`) in the suite; header sets also built under gcc-debug and clang-debug | `tools/timed workflow <p> all -- cmake --workflow --preset <p>` | Yes |

Wall times of the full-suite workflows (s): gcc-debug 3.41, gcc-release 5.89, gcc-asan 7.76, clang-debug 3.22, clang-release 4.92, clang-asan 6.59, lint 2.47; total 34.3 s for 7 configurations, no failure.

### Tests and expected values (W9)

| Test | Fixes | Expected value and its source |
| --- | --- | --- |
| core.metric, R splits | every R<N>(p, q, r), n ≤ 6 (84 splits) | diagonal e_(i+1)² = +1 for i < p, −1 for p ≤ i < p + q, 0 after; off-diagonal 0 (definition of R(p, q, r), null generators last) |
| core.metric, R literals | R(1,1,1), R(2,2,2), R(4,1,0) | diag(1, −1, 0); diag(1, 1, −1, −1, 0, 0); diag(1, 1, 1, 1, −1), written out |
| core.metric, rigid | rigid(d), d ≤ 5; rigid(3) | diag(1 × d, 0); diag(1, 1, 1, 0) written out; rigid(3) == R(3, 0, 1) |
| core.metric, conformal | conformal(d), d ≤ 4; conformal(3) | e1..ed square to 1, e(d+1), e(d+2) to 0, e(d+1)·e(d+2) = −1, else 0; the 5 × 5 matrix written out |
| core.metric, accessors | rigid(3), conformal(3) | rigid(3) diagonal, only e4 null; conformal(3) not diagonal, e4 and e5 null |
| core.metric, change of basis | conformal(d) against R(d+1, 1, 0), d ≤ 4 | origin = ½(e(d+2) − e(d+1)), infinity = e(d+2) + e(d+1); (2B)ᵀ diag(2B) = 4G entry by entry |
| core.metric, from_gram | [[0,1,0],[1,0,0],[0,0,0]] | kept entry by entry; not diagonal; all three null |
| core.metric, predicates | hand-written metrics | [[0,1],[2,0]] not symmetric; ±32767 in range; 32768 and −32768 not |
| core.metric, bound | max_generators, MaxN | 16; R<16> accepted; n = 17 accepted with MaxN = 32 |
| core.metric_asymmetric | from_gram({{1,2},{0,1}}) | "from_gram: the form must be symmetric" |
| core.metric_split | R<4>(3, 0, 2) | "R<N>(p, q, r): p + q + r must equal N" |
| core.metric_negative | R<3>(4, 0, −1) | "R<N>(p, q, r): p, q, and r cannot be negative" |
| core.metric_entry_range | from_gram({{32768}}) | "from_gram: an entry is beyond max_form_entry" |
| core.metric_zero | R<0>(0, 0, 0) | "metric: an algebra has at least one generator, so n = 0 is rejected" |
| core.metric_bound | rigid<16>() (n = 17) | "metric: n is larger than MaxN, the largest number of generators accepted" |

Regression guards, not evidence: `max_form_entry == 32767` and `max_generators == 16` pin the chosen constants.

Mutations of `metric.hpp` on gcc-debug, each restored from git afterward: R's +1 and −1 swapped (build fails on the test's `first_square` static_assert); conformal's pair entry +1 on one side only (build fails on `metric_like<cga3>`); the last negative generator turned positive (`core.metric` fails at least 9 checks in at least 3 cases; the output read was cut at 12 lines); both pair entries +1 (`core.metric` fails at least 10 checks in 2 cases, same cut); the symmetry assertion removed (`core.metric_asymmetric` fails); the range test `>` made `>=` (`core.metric` fails); `N >= 1` made `N >= 0` (`core.metric_zero` fails).

### Build and test cost, before and after (W10, P1)

Clean `cmake --workflow --preset <p> --fresh`, shared mode (not pinned; other agents' work on the machine, load about 1.4). Before: `533623a` (develop). After: `1dc00cb`. Instructions are user-space, both core types summed.

| TU | Compiler | Before: wall s, peak MiB, instructions | After |
| --- | --- | --- | --- |
| smoke_test (includes core.hpp) | GCC 16.2.1 | 0.143, 44, 0.272 G | 0.175, 62, 0.601 G |
| smoke_test | Clang 23.1.4 | 0.133, 101, 0.177 G | 0.163, 110, 0.444 G |
| metric_test (new) | GCC | | 0.290, 84, 1.733 G |
| metric_test (new) | Clang | | 0.235, 117, 1.161 G |
| each metric_*_fail (new, 6) | GCC | | 0.05 to 0.20, 54, 0.40 G |
| each metric_*_fail (new, 6) | Clang | | 0.10 to 0.20, 90, 0.34 G |
| doctest runner, assert tests | both | unchanged | unchanged |

Cause of the smoke_test rise, measured pinned to CPU 6 with `-fsyntax-only -O0 -D_GLIBCXX_ASSERTIONS`: `<array>` alone costs 316 M (GCC) and 281 M (Clang) instructions against 22 M and 37 M for an empty unit; `<utility>` 176 M and 157 M; `<type_traits>` 83 M and 84 M. All four standard headers with assert.hpp's: 376 M and 328 M; `metric.hpp` itself adds 3.3 M (GCC) and 4.9 M (Clang). So the rise is `<array>`, which later headers (blade sets, `mv`) will include in any case. Command, from the worktree root, for each header h: `taskset -c 6 perf stat -x, -e instructions:u -- g++ -std=c++23 -O0 -D_GLIBCXX_ASSERTIONS -I include -fsyntax-only <unit including h>` (and `clang++`).

Test time (CTest "Total Test time (real)"), clean debug workflows: before 0.48 s (GCC) and 0.51 s (Clang) for 10 tests; after 2.31 s and 2.27 s for 17 tests. The six compile-fail tests take about 0.36 s each and run one at a time under the `galoot_build_tree` resource lock, so they account for nearly all of the increase.

### Open question: does the tag-and-value split cost what Spike A measured?

`docs/agent/records/performance/tag-value/measure_tag_value.py` (snippet candidate), run as `tools/timed measure - tag-value -- python3 -I docs/agent/records/performance/tag-value/measure_tag_value.py . <out> 6`, pinned to CPU 6, results in `out/`:

- Names, as predicted. A function template instantiated on a tag has a mangled name of 20 to 28 characters (GCC; 43 to 54 with GCC's second alias and on Clang), constant in n. On the metric value it is 136 (rigid(3)), 170 (conformal(3)), and 349 (R(16,0,0)) on GCC, with aliases 152, 214, 833; Clang 152, 214, 833. Value names grow with n², as note 0008 inferred.
- Diagnostics, as predicted. With values, GCC 16 still misprints conformal(3) as `{1}, {1}, {1}, {-1}, {-1}`, the rows of a diagonal form, through `std::__array_traits` internals (`out/diag-value-gcc.txt`); Clang prints both matrices correctly. With tags, both compilers say "deduced conflicting types for parameter 'Alg' ('cga3' and 'r410')".
- Compile cost, which Spike A did not measure. Per added signature of n = 16, from 1 to 128 signatures: an unconstrained tag costs 2.6 M instructions (GCC) and 1.45 M (Clang), less than the value form's 3.4 M and 1.76 M. A tag checked by `metric_like` costs 22.1 M and 18.9 M, so the concept's validity loops (symmetry and range over 256 entries) are 88% (GCC) and 92% (Clang) of the tag's cost. The check is evaluated once per tag: 64 constrained templates on one tag cost 25.6 M (GCC) and 24.7 M (Clang) more than unconstrained, against 20.6 M and 19.6 M for one. In wall time, 128 checked signatures take 0.20 s against 0.07 s for values; a program uses a handful.
- Conclusion: the split costs about what Spike A implied for names and diagnostics; for compile cost the tag itself is cheaper than the value, and the extra cost comes from re-validating in `metric_like`, about 1 ms per signature at n = 16, once per translation unit (decision 0060, consequences).

### Agent cost (decision 0019)

About 75 tool calls in one session; the harness does not report tokens to this agent, so tokens are unavailable.

### Next step

W5 blind review of `feature/core-metric` against this plan and the departures above, then the merge into `develop` (the coordinator's). Before the merge, `develop` is merged into the branch; expect conflicts with WU2 in `CMakeLists.txt` (header set), `include/galoot/core.hpp`, and `test/CMakeLists.txt`, each resolved by keeping both units' lines, and a possible `runs.tsv` union doubling (snippet `runs_tsv_merge.py`).
