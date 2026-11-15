# 0005 WU3: metric

- Status: closed; the unit merged into `develop` by `47efa71` on 2026-10-31 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: closed 2026-10-31 with the outcome below (implementation done on `feature/core-metric`; the W5 review and the merge into `develop` are the coordinator's); branch `feature/core-metric`, worktree `.claude/worktrees/core-metric`
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
8. Lesson (moved to `docs/agent/lessons-learned/0060-mutation-runs-check-the-build.md` after the review, N8): the first mutation script hid the build output, so two mutations that broke the build ran the previous binary and "passed". The rerun showed both caught at compile time by the test's `static_assert`s. A mutation run must check the build's exit status before reading the tests.

### Verification and validation status (V10)

- Verification: performed and passed at `1dc00cb`, and again after the review follow-up at `ed2a3e2` (section below). Kind 1 (exhaustive over every Gram matrix for n ≤ 6 against definitions restated in the test) for criteria 1, 4, 5; compile-fail and abort tests for the checks.
- Validation: not performed. The metric is data; the referent for the signatures (Clifford algebra relations, the conformal model) is checked when products exist (WU6, WU12). Status: unvalidated.

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
| core.metric, R splits | every R<N>(p, q, r), n ≤ 6 (83 splits) | diagonal e_(i+1)² = +1 for i < p, −1 for p ≤ i < p + q, 0 after; off-diagonal 0 (definition of R(p, q, r), null generators last) |
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

## Review follow-up (2026-10-31, Claude, agent, Opus 5.5)

The W5 review of `b12f07d` (`docs/agent/records/reviews/2026-10-31-wu3-metric.md`) returned "merge after named fixes". The owner's rulings are decisions 0035 and 0036; the author's choices within them are decision 0061, which supersedes decision 0060 items 1 to 3.

### Departures (C20)

1. `mask_t` is redeclared in `metric.hpp` as `std::uint32_t`, identical to `bits.hpp` (WU2, not on `develop`), so that WU3 does not rely on unfinished work (decision 0061, item 3). The second unit to merge replaces it with `#include <galoot/core/bits.hpp>`.
2. Commit `708c5dc` holds the changed tests before the code they judge (W9, review N6), so at that commit the unit test does not build and five tests fail; this is against W2's "building at every step". The test-only fix `c8dfa51` is likewise separate. In the first pass, the tests were committed with the code (`6f458b0`), which the review records as N6.
3. N11 (the `metric_like` cost at three sizes) is not done; it belongs with WU17's growth grid.
4. N13 is met by the comment ("must be square"), not by a catch-all (decision 0061, item 6).
5. The remaining `//` lines between a `///` block and its declaration are the two `NOLINTNEXTLINE` markers that clang-tidy needs on the line above the flagged name (`R`) and statement; each carries its reason on the same line. `from_gram`'s marker moved to the end of its line (N10).

### Fixes, with before and after

Before: the tests of `708c5dc`/`c8dfa51` against the header of `b12f07d`. After: `d6d9b10`. Mutations: each applied to the header of `d6d9b10`, built and run on the presets named, then restored from git (lesson 0060: a build failure is a kill).

| Finding | Fix | Before | After | Mutation |
| --- | --- | --- | --- | --- |
| 0035.1, N4, N5 | `R<P, Q, Z, MaxN>()`, n = P + Q + Z; each count checked against MaxN before the sum | `R<P, Q, Z>` does not exist; `core.metric_count_wrap` fails | `R<SIZE_MAX, 0, 2>()` rejected: "metric: n is larger than MaxN" | count check removed: `core.metric_count_wrap` fails |
| 0035.2, 0036.2, N3 | `entry_type = int`; `max_form_entry = isqrt(max / 2)` | literal 32767 and a comment assuming 32-bit `int` | derived; the test checks 2k² ≤ `INT_MAX` < 2(k+1)² | |
| 0036.1, N2 | `metric<N>` and `metric_like` accept N ≤ `digits(mask_t)`; 16 is only the factories' default MaxN | `metric_like<r17>` false | `metric_like<above_default>` and `metric_like<widest>` (n = 32) true; `core.metric_max_n_width` and `core.metric_type_width` reject 33 | |
| 0035.3 | Two bounds checks excluded in `.clang-tidy` with the reason; block suppressions removed from `metric.hpp` and `metric_test.cpp` | `NOLINTBEGIN/END` in both files | none; lint clean | |
| B1 | `GALOOT_ASSERT(i < N, ...)` in `is_null` | `core.metric_is_null` fails (no abort) | aborts with "metric::is_null: the index must be below n" | assertion removed: `core.metric_is_null` fails |
| N1 | constant-expression guard in `metric_like` | hard error: GCC "not usable in a constant expression"; Clang "substitution into constraint expression resulted in a non-constant expression" | `metric_like<runtime_value>` and `metric_like<member_value>` false on both compilers | guard removed: build fails on both compilers |
| N7 | (test) lookalike type | no test | `!metric_like<lookalike>` | `is_metric_v` dropped: build fails on both compilers |
| N9 | `from_gram<N, MaxN>` | `from_gram<MaxN, N>` | `from_gram<1, 32>({{1}})` in the test | |
| N10, N13 | `///` for `gram`; "must be square" in `from_gram`'s comment; NOLINT moved | | | |
| N8 | V&V status line above; lesson file 0060 | | | |
| N12 | `develop` (`319be69`) merged as `ed2a3e2`; records only, no conflict; `runs.tsv` has 3009 rows, all distinct | | | |

### Tests changed or added (W9)

| Test | Change | Expected |
| --- | --- | --- |
| core.metric | `R<P, Q, Z>()` throughout; the R-split sweep expands over every (p, q, z) with n ≤ 6 at compile time; bound checks against `numeric_limits`; new concept cases | as before for the matrices; `metric_like` true for rga3, cga3, n = 17 built with MaxN 17, n = 32 built with MaxN 32; false for no value, int value, `int`, `metric<4>`, lookalike, non-constexpr static value, non-static member value, asymmetric, entry `max_form_entry + 1`, n = 0; 2k² ≤ `INT_MAX` < 2(k+1)² for k = `max_form_entry`; ±k in range, ±(k+1) not; n = `max_generators` by default and n = 32 with MaxN = 32 accepted |
| core.metric_split, core.metric_negative | removed: neither misuse can be written | |
| core.metric_zero | `R<0, 0, 0>()` | "metric: an algebra has at least one generator, so n = 0 is rejected" |
| core.metric_entry_range | `from_gram({{max_form_entry + 1}})` | "from_gram: an entry is beyond max_form_entry" |
| core.metric_count_wrap (new) | `R<SIZE_MAX, 0, 2>()` | "metric: n is larger than MaxN, the largest number of generators accepted" |
| core.metric_max_n_width (new) | `R<1, 0, 0, 33>()` | "metric: MaxN is larger than the number of bits in a blade mask" |
| core.metric_type_width (new) | `metric<33>` by hand | "metric: n is larger than the number of bits in a blade mask" |
| core.metric_is_null (new) | `rigid<3>().is_null(argc + 3)` at run time, checking build | abort with "galoot: assertion failed: metric::is_null: the index must be below n (" |
| core.metric_asymmetric, core.metric_bound | unchanged | as before |

### P9 on the merged revision

At `ed2a3e2`, `tools/timed workflow <p> all -- cmake --workflow --preset <p>`: gcc-debug 3.60 s, gcc-release 3.52 s, gcc-asan 3.80 s, clang-debug 3.15 s, clang-release 3.56 s, clang-asan 3.33 s, lint 3.06 s (clang-tidy, header sets, format-check); 19 of 19 tests in each; total 24.0 s. `all_verify_interface_header_sets` built under gcc-debug and clang-debug. Nothing deferred.

Cost (track-only, shared mode): `metric_test` 0.20 s, 93 MiB, 2.03 G instructions on GCC (was 0.29 s, 84 MiB, 1.73 G); on Clang 0.17 s, 121 MiB, at least 1.79 G (its efficiency-core count is `unavailable`; was 1.16 G). The rise is the compile-time expansion of the R sweep over 83 splits as template instantiations, which the derived `R<P, Q, Z>` requires. The header-check TU of `metric.hpp` costs 0.45 G (GCC) and 0.38 G (Clang).

### Next step

Re-review of the delta `b12f07d..ed2a3e2`, then the merge into `develop` (the coordinator's). If WU2 merges first, replace the `mask_t` declaration in `metric.hpp` with an include of `bits.hpp`, and resolve the one-line conflicts in `CMakeLists.txt` and `core.hpp` by keeping both lines. Reference markers wait for the M0 scripts, as directed.

## Golden values and reference markers (2026-10-31, Claude, agent, Opus 5.5)

After the M0 scripts merged (`develop` at `8327147`, merged here as `911b4e8`; one conflict in `test/CMakeLists.txt`, both units' blocks kept), the metric tests were changed without touching the header, while the delta review ran:

- Every test case carries a `// Reference:` marker (decisions 0071, 0072); 11 test cases in `test/` pass the marker check.
- The comparisons use `test/reference/golden.hpp` where it reaches: `golden::diagonal_signatures` (every R(p, q, r), n = 1 to 6), `golden::rigid` (squares for d = 1 to 5), and `golden::conformal` (Gram matrices, the squares of R(d+1, 1, 0), and the change of basis, for d = 1 to 4). The golden values do not cover rigid(0), conformal(0), or the change of basis at d = 0; those stay written out from the definition, and the markers say so. The test-side `expected_*` definition sweeps and the doubled-basis literal for d = 3 are replaced by the golden values; off-diagonal zeros of diagonal signatures are still checked from the definition.
- `docs/agent/records/convention-trace.md`: 1.2, 1.3, and 1.4 set to Implemented `yes`. 1.1 (mask bits) stays `no`: the metric does not implement masks; WU2 does.
- Mutations (restored): the last negative generator made positive fails 3 test cases; the conformal pair made +1 on both sides fails 2.
- At `d9de465`: the seven correctness workflows pass 30 of 30 each (gcc-debug 3.15 s, gcc-release 3.08 s, gcc-asan 3.17 s, clang-debug 2.92 s, clang-release 3.21 s, clang-asan 3.36 s, lint 2.81 s; 21.7 s in total). The gate command passes: 1.2 at `metric_test.cpp:194, 207`; 1.3 at `233, 268`; 1.4 at `255, 268, 299`.
- Still open: replacing the `mask_t` redeclaration with `#include <galoot/core/bits.hpp>` once WU2 is on `develop`; the result of the delta review.

## Delta review fixes (2026-10-31, Claude, agent, Opus 5.5)

The delta review (`docs/agent/records/reviews/2026-10-31-wu3-metric-delta.md`, of `b12f07d..0e2d3ef`) returned "merge after fixes". `develop` (`c7bff1e`) merged as `389630f` without conflict.

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| D1 | markers and golden values (previous section, `22983f6`, `d9de465`) | 9 unmarked test cases | 11 of 11 marked; P9 below |
| D2 | `.clang-tidy` states the reason for both bounds exclusions | no reason (the earlier edit silently matched nothing; lesson 0061) | reason in the comment block |
| D3 | `metric_like` guards on `entries_in_range()`, which reads every entry | declared-only, non-constexpr, and non-static `metric<1>` values: hard errors on both compilers | false on both; the old guard as a mutant breaks the build on both |
| D4 | `core.metric_conformal_wrap` requires the literal message for `conformal<SIZE_MAX>()` | the check had no test | passes; fails when `static_assert(D <= MaxN)` is removed |
| D5 | tests use `core::entry_type`; `from_gram`'s NOLINT has its reason on the line; the `///` says raising MaxN means writing N; generator-count checks run only when every count is within MaxN | `R<SIZE_MAX, 0, 1>()` printed "n is larger than MaxN" and "n = 0 is rejected" | only "n is larger than MaxN", on both compilers |

Departures: the declared-only `metric<1>` tag sits outside the anonymous namespace, because Clang rejects a declared-only member with internal linkage, with one `misc-use-internal-linkage` suppression carrying its reason. The tests (`36f4d9f`, `64dc0ac`) and the code (`10aeff3`) are separate commits, so `36f4d9f` does not build (W9 over W2, as before). The `mask_t` redeclaration stays until WU2 is on `develop`.

P9 at `10aeff3`: gcc-debug 3.51 s, gcc-release 3.57 s, gcc-asan 3.89 s, clang-debug 3.39 s, clang-release 3.42 s, clang-asan 3.60 s, lint 3.17 s; 31 of 31 in each; 24.5 s in total; header sets built on both debug presets. Marker check: 11 test cases marked; golden check passes; the trace gate passes (1.2 at `metric_test.cpp:213, 226`; 1.3 at `252, 288`; 1.4 at `275, 288, 319`).

## Second delta review and the bits merge (2026-10-31, Claude, agent, Opus 5.5)

The second delta review (`docs/agent/records/reviews/2026-10-31-wu3-metric-delta2.md`) says merge, with non-blocking findings N1 to N5. `develop` (`507a747` and a cost row; it brings `bits.hpp`, the recorder, the markers and trace gate, and the W5 changes) merged as `02e7093`. Conflicts, each resolved by keeping both sides: `include/galoot/core.hpp` (bits, then metric), the header set in `CMakeLists.txt` (assert, bits, metric), `test/CMakeLists.txt` (both units' blocks), and `docs/agent/records/convention-trace.md` (every `yes` kept: 1.1, 1.5, 2.1, 3.1 from bits; 1.2 to 1.4 from metric). The snippets README merged without conflict. Six old-format `build/*/compile.tsv` files of this worktree (gcc and clang debug, release, and asan), whose header lacked `pinned_other_s`, were listed and deleted; their figures are in the sections above.

Item 1.1 (bit i - 1 of a blade mask stands for e_i) is pinned by the bits tests; the metric adds nothing to it.

| Finding | Fix | Evidence |
| --- | --- | --- |
| mask_t | `metric.hpp` includes `bits.hpp` and bounds n by `mask_bits`; the redeclaration is gone (decision 0062) | P9 below |
| N1 | `static_assert(golden::diagonal_signatures::nK.size() == splits_of(K))`, with (n + 1)(n + 2) / 2 splits, for K = 1 to 6 | a golden row dropped with its size adjusted: the build fails at the static_assert |
| N2 | `FORBID <literal>...` in `test/expect_output.cmake` and the compile-fail helper; `core.metric_count_wrap_zero` forbids "at least one generator" for `R<SIZE_MAX, 0, 1>()`; `tools.expect_output_forbid` (WILL_FAIL) and `tools.expect_output_forbid_absent` test the option | R's `if constexpr` removed: the test fails with "output contains forbidden 'at least one generator'" |
| N3 | accepted: one true message for a call that is wrong twice (decision 0062, item 2) | |
| N4 | recorded here (history is not rewritten) | |
| N5 | `.clang-tidy` says the correctness presets check std::array accesses; written-out values are marked "independent value" throughout; the from_gram and max_form_entry markers cover what their cases check; the change-of-basis test name says the signatures differ only at d = 3 | markers check: 28 test cases marked |

N4, the state of earlier commits: `22983f6` fails the lint build (the conformal(0) basis initializers; fixed in `d9de465`). `708c5dc`, `36f4d9f`, and `64dc0ac` hold tests ahead of the header they judge and do not build. The message of `36f4d9f` says "the next commit makes it false"; the commit that does is `10aeff3`, after `64dc0ac`. The message of `64dc0ac` says Clang "rejects" the declared-only member; it is a warning (`-Wundefined-internal`) that `-Werror` turns into an error.

P9 at `a01add5`: gcc-debug 4.47 s, gcc-release 4.40 s, gcc-asan 4.96 s, clang-debug 3.61 s, clang-release 4.39 s, clang-asan 4.46 s, lint 2.41 s; 57 of 57 in each; 28.7 s in total; header sets built on both debug presets. Marker check: 28 test cases marked; golden check passes; the trace gate passes (1.2 at `metric_test.cpp:218, 239`; 1.3 at `265, 301`; 1.4 at `288, 301, 332`; bits' items 1.1, 1.5, 2.1, 3.1 pinned by `bits_test.cpp`).

Still open: the three-size cost of `metric_like` (review N11), for WU17.
