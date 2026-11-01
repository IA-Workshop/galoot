# 0008 WU4: oriented blade sets

- Status: done, pending W5 review and the merge into `develop`; branch `feature/core-blade-set`, worktree `.claude/worktrees/core-blade-set`; decision numbers 0090 to 0099 (0090 and 0091 used).
- Goal: `include/galoot/core/blade_set.hpp`, the storage layout of a named type as an ordered list of oriented blades, with the canonical result order and the name parser (CONVENTIONS 1.1, 1.6, 1.7).
- Content, in `galoot::core`:
  - `oriented_blade`: a mask and a sign of plus or minus one, as a structural value.
  - `blade_set<K>`: a structural aggregate of K oriented blades, usable as a class-type template argument (the hybrid ruling: the algebra is a tag, blade sets are values).
  - Canonical order of a result without a declared type: grade first, then lexicographic order of the index lists (not numeric mask order), so for four generators e12, e13, e14, e23, e24, e34.
  - Constructors of blade sets: `full<N>` (every blade in canonical order), `of_grade<N, k>`, `merge` of two sets in canonical order, and `index_of` a mask in a set, which is a compile-time error when absent.
  - A name parser `blades<N>("e41 e42 e43 e23 e31 e12")` that turns oriented names into oriented blades: the mask from the indices, the sign from the sign of the permutation that sorts the indices. Compact names are parsed only for n up to 9; above that, masks only. Invalid or repeated names, indices above N, and a mask given twice are compile-time errors with messages in the algebra's terms (the catch-all form for functions, the plain static assertions in consteval code, as established).
  - Canonical names from masks (`e124` for the mask with bits 0, 1, 3), for diagnostics and the tests.
- Dependencies: `bits.hpp` (masks, grade, reorder sign) and `metric.hpp` (generator count). The core stays free of anything beyond the standard library. The generator count is `std::size_t`, grades are `int`.
- Definition of done: the header exists, is self-contained, documented (`///`, STYLE), and the tests pin the oriented-name table and the result order.
- Acceptance criteria:
  1. The oriented-name list of rigid(3) from the golden values (`golden::oriented_names`) parses to exactly the golden masks and signs, and the signs are cross-checked by a test-side permutation sign (an independent route).
  2. The result order for n = 4 and n = 5 equals `golden::result_order`, and for n up to 6 it equals the order produced by a test-side sort by (grade, index list), shown to differ from numeric mask order.
  3. `full<N>`, `of_grade<N, k>`, `merge`, and `index_of` are tested exhaustively for n up to 6 against the test-side definitions; sizes equal the binomial counts.
  4. Every rejection (unknown or repeated name, index above N, duplicate mask, n above 9 for compact names, a name with a bad leading letter or digit) has a compile-fail test on both compilers that requires the literal message, and the tests can fail (mutation shown).
  5. The set is a valid class-type template argument on both compilers, with equal values giving the same specialization.
  6. The tests carry reference markers and compare with the golden values where it covers the fact; the trace table rows 1.6 and 1.7 are set to yes only when a test states their facts; the trace gate passes.
  7. Lint, format, include scan, the citation check, and all existing tests pass on all seven presets.
- Approach: direct form; ordinary `constexpr` code over values (S7); no table is precomputed; sizes derived from the types (`numeric_limits`), no literal limits.
- Evidence: test output per preset, mutation runs, the trace gate output, and before and after build costs of the affected suites (W10).
- Agent cost: tracked for information only.
- Review: W5 blind review before the merge.
- Open questions: whether the parser reads names with a leading `e` only; whether `merge` must also accept sets with differing signs for one mask (the unit proposes and records).

## Outcome (2026-10-31, Claude, agent, Opus 5.5)

Status: done on `feature/core-blade-set`, pending the W5 review and the merge, which the coordinator makes. Code at `1de23ad`; records at `73e7994` and the commit that adds this section.

### Departures (C20)

1. Spelling of the parser: `blades<N, "e41 e42 e43 e23 e31 e12">`, not `blades<N>("...")`. A function cannot return a `blade_set<K>` whose K is the number of names in its argument (inferred from the language rules; decision 0090, item 1).
2. Every constructor is a variable template: `full<N>`, `of_grade<N, K>`, `merge<A, B>`, `index_of<S, Mask>`, `blades<N, Names>`. Each misuse is a `static_assert` in `consteval` code with a literal message and a placeholder return, so it gives one error. `canonical_name` is a `constexpr` function whose precondition is a `GALOOT_ASSERT`. No catch-all overload exists, because no public function rejects a misuse that its operand types reveal (decision 0090, items 2 and 3).
3. `metric.hpp` is not included: blade sets do not depend on the metric. N is `std::size_t` and is bounded by `mask_bits`, as for `metric<N>` (decision 0090, item 4).
4. Two rejections beyond the plan's list, each with its own test: an empty name list, and `merge` of sets that hold one mask with opposite signs (decision 0091). A name repeating a generator (e121) has its own message, separate from a blade named twice.
5. Criterion 3, read for `merge`: exhaustive over every pair of grade sets of_grade<N, J>, of_grade<N, K> for n up to 6, and over every pair of subsets of the blades for n up to 2 (16 and 256 pairs, inputs listed out of canonical order). All pairs of subsets at n = 3 would be 65,536 template instantiations and was not attempted.
6. Test time per preset rose from about 3.2 s to about 9.5 s. The cause is the 21 compile-fail tests, which run one at a time under the build-tree lock at about 0.3 s each. Tracked, not bounded.

### Done

- `include/galoot/core/blade_set.hpp`, in the header set and in `core.hpp`: `oriented_blade`, `blade_set<K>` (structural aggregate with `blades` and `size()`), `max_compact_generators`, `canonical_less`, `blade_name` and `canonical_name`, and the five variable templates above.
- Decisions 0090 (interface) and 0091 (the open questions: the parser reads "1" and e-names separated by spaces; `merge` keeps signs and rejects opposite ones).
- Trace rows 1.6 and 1.7 set to `yes`.
- Mutation script and results: `docs/agent/records/mutations/0008-blade-set/`.

### Tests, for the reviewer

`test/core/blade_set_test.cpp` (CTest `core.blade_set`): 13 test cases, 4,875 assertions, and 10 namespace-scope `static_assert`s.

| Test case | Expected values |
| --- | --- |
| oriented names of rigid(3) | the 16 golden `oriented_names::rigid_3` masks and signs, from one space-separated list; each golden sign also equals the parity of the name's inversions, counted in the test |
| line of rigid(3) | e41 e42 e43 e23 e31 e12 → {0b1001,−1}, {0b1010,−1}, {0b1100,−1}, {0b0110,+1}, {0b0101,−1}, {0b0011,+1}, written out |
| every canonical name, n = 1 to 9 | the canonical names of all 2^n masks, built in the test and listed in numeric order, parse to those masks in order with sign +1 |
| canonical_name | golden `blade_names` n4 and n5, and every mask of 9 generators against a name built in the test; evaluated at run time |
| full<4>, full<5> | golden `result_order` n4 and n5, signs +1 |
| full<N>, n = 1 to 6 | a sort by (grade, index list) in the test; that order equals grade-then-numeric order for n ≤ 3 and differs for n = 4 to 6 |
| of_grade<N, K>, n = 1 to 6 | the canonical list kept to grade K; size C(n, k) by Pascal's rule |
| merge of grade sets, n = 1 to 6 | the canonical list kept to grades J and K, every J and K |
| merge of subsets, n = 1, 2 | the union in canonical order, signs −1 for blades holding e1 and +1 otherwise, every pair |
| merge of line and plane | +e12, −e13, −e14, +e23, −e24, −e34, −e123, +e124, −e134, +e234, written out; both argument orders; line with itself keeps its signs |
| index_of, n = 1 to 6 | positions in the canonical lists, for every blade of full<N> and of every of_grade<N, K> |
| index_of in the line | positions 0 to 5 of e14, e24, e34, e23, e13, e12 |
| canonical_less | the test's comparison of index lists for every pair of masks of 6 generators, at run time |

The `static_assert`s: equal sets give one specialization (`blades<4, "e12 e13">`, a hand-written set, and the same list with extra spaces; `blades<3, "e1 e2 e3">` and `of_grade<3, 1>`; `blades<2, "1 e1 e2 e12">` and `full<2>`; `merge<A, B>` and `merge<B, A>`). Another sign (e13 against e31) or another order gives a different specialization. `blade_set<3>` and `oriented_blade` are aggregates, and `blade_set<3>` is trivially copyable.

Rejections, each a CTest labeled `core.blade_set` that runs on every preset and requires the literal message (and `static assertion failed`, or `assertion_failed` for the `GALOOT_ASSERT`):

| Test `core.blade_set_…` | Misuse | Message (and FORBID) |
| --- | --- | --- |
| no_names | `blades<4, "   ">` | blades: the list names no blade |
| not_a_name_letter, not_a_name_digit | "e1 x2", "e1 12" | blades: a name must be 1 or start with e, as in e12 |
| bad_index_zero, bad_index_letter, no_index | "e1 e10", "e1x", "e1 e" | blades: after the e, a name lists generator indices, each a digit from 1 to 9 |
| index_above_n | `blades<3, "e1 e14">` | blades: a name has an index above n, the number of generators |
| repeated_generator | "e121" | blades: a name lists one generator twice (FORBID the same-blade message) |
| repeated_name, same_blade | "e12 e3 e12", "e12 e21" | blades: two names stand for the same blade (same_blade FORBIDs the repeated-generator message) |
| names_above_9 | `blades<12, "e1 e11">` | blades: names such as e123 have one digit per index, so they are read only for n up to 9 (FORBID the repeated-generator message) |
| names_zero, full_zero, of_grade_zero | n = 0 | blade_set: an algebra has at least one generator, so n = 0 is rejected (names_zero FORBIDs the index-above-n message) |
| full_wide | `full<mask_bits + 1>` | blade_set: n is larger than the number of bits in a blade mask |
| of_grade_above_n, of_grade_negative | `of_grade<3, 4>`, `of_grade<3, -1>` | of_grade: the grade must be from 0 to n |
| merge_opposite_signs | e13 e2 with e31 | merge: the two sets hold one blade with opposite signs |
| index_of_absent | `index_of<full<2>, 0b100U>` | index_of: the set holds no blade with this mask |
| name_beyond_e9 | `canonical_name(0b10'0000'0000U)` in a `static_assert` | canonical_name: a generator beyond e9 |
| name_assert | the same mask at run time, checking build | abort with "galoot: assertion failed: canonical_name: a generator beyond e9 (" and `blade_set.hpp:` |

### Acceptance criteria

1. Met. `core.blade_set` parses the golden list and matches all 16 masks and signs; the signs are cross-checked by the test's inversion count. The mutant `parse_sign_trivectors` (signs of grade 3 and above wrong) builds and fails this case on both compilers.
2. Met. `full<4>` and `full<5>` equal `golden::result_order`; `full<N>` for n up to 6 equals the test's sort, which differs from grade-then-numeric order from n = 4 and agrees below (checked both ways).
3. Met, with the reading of departure 5 for `merge`. Sizes equal 2^n and C(n, k).
4. Met. 21 rejection tests, run in all six compiler presets and in lint. Mutations at `1de23ad`: 29 of 29 killed on gcc-debug; each rejection mutant (18) and `parse_sign_trivectors` killed on clang-debug at `ee69bd8`, and `name_assert` and `parse_sign_trivectors` again at `1de23ad`. With the run-time program rebuilt, `name_assert` kills both `name_beyond_e9` and `name_assert` on both compilers. The FORBID literals are exercised by `names_above_9_guard` and `zero_guard`. The repeated-generator and same-blade FORBIDs have no mutant of their own.
5. Met. The namespace-scope `static_assert`s above compile on GCC 16.2.1 and Clang 23.1.4.
6. Met. Every test case carries a marker (41 in `test/`). The golden check passes. The gate passes with 1.6 pinned at `blade_set_test.cpp:462, 485, 617, 670` and 1.7 at `538, 558, 683`, and 1.1 also at `520`.
7. Met at `73e7994`: `cmake --workflow --preset <p>` for gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, and lint (clang-tidy, format-check, include scan), 79 of 79 tests each; `all_verify_interface_header_sets` on gcc-debug and clang-debug. The citation check of `feature/cleanup-1` (`tools/test_references.py citations`, run from that worktree without changing it) passes on every new file and on every line this unit adds to `CMakeLists.txt`, `test/CMakeLists.txt`, and `core.hpp`. On whole files it reports only existing lines that the cleanup unit rewrites.

### Costs (W10; shared mode, track-only)

| Quantity | Before (`2976dca`) | After (`73e7994`) |
| --- | --- | --- |
| Tests per preset | 57 | 79 |
| CTest time, gcc-debug / gcc-release / gcc-asan | 3.27 / 3.06 / 3.23 s | 11.27 / 9.23 / 9.45 s (`core.blade_set` label 7.52 / 6.51 / 6.89 s) |
| CTest time, clang-debug / clang-release / clang-asan / lint | 3.29 / 3.17 / 3.05 / 1.49 s | 9.54 / 9.34 / 10.06 / 6.56 s |
| `smoke_test.cpp` (includes `core.hpp`), gcc-debug | 0.201 s, 68,992 KiB, 0.651 G instructions | 0.200 s, 78,484 KiB, 0.885 G (+36%) |
| `smoke_test.cpp`, clang-debug | 0.128 s, 113,732 KiB, 0.480 G | 0.184 s, 117,692 KiB, 0.650 G (+35%) |
| `#include <galoot/core.hpp>` alone, reviewer's measurement at f9441f7 | GCC 0.488 G, 59 MB; Clang 0.378 G, 103 MB | GCC 0.721 G, 70 MB (+48%); Clang 0.548 G, 107 MB (+45%) |
| `blade_set_test.cpp`, gcc-debug | new | 0.75 s; 226 MiB; 8.17 G |
| `blade_set_test.cpp`, clang-debug | new | 1.15 s; 175 MiB; 18.88 G |

Corrected after the W5 review (finding B1). The first version of this table took its before rows from `2976dcafb8e0-dirty` compiles, which already included the uncommitted blade set header. So its "before" was an after. It also counted only instructions retired on performance cores. Each figure is now the sum over both kinds of core, which does not depend on where the compile ran. Instruction counts for the release and asan presets were dropped for the same reason, because they were performance-core counts only.

A Clang time trace of `blade_set_test.cpp` puts most of its cost in the exhaustive sweeps of the test: the grade-pair merges at n = 6 (140 ms), the 512-name list at n = 9 (137 ms), and the subset pairs at n = 2 (78 ms). The header's own parse of that list takes 32 ms. Clang first exceeded its constant-evaluation step limit when the test re-sorted its reference list for every grade pair. The test now sorts it once per n, in a variable template. At the base revision, the gcc-debug and clang-debug compile records hold two `smoke_test.cpp` rows. The cheaper one, revision `2976dcafb8e0`, is the true before. The other, `2976dcafb8e0-dirty`, was compiled after `core.hpp` had begun to include the new header (corrected after the review; first reported here as unexplained). Most of the rise is `<string_view>`, which the header includes: the base plus `<string_view>` alone costs 0.665 G on GCC and 0.518 G on Clang, and the blade set code about 0.03 G (reviewer's measurement). The workflow wall times in `runs.tsv` are not compared: the before runs were clean builds and the after runs incremental.

### Verification and validation

Verification: performed and passed. The blade sets, order, parser, and names are checked exhaustively against definitions written in the test (n up to 6, names up to 9), and against golden values from `test/reference/`, on both compilers in every preset. Every doctest case and every rejection test was shown able to fail by mutation. The namespace-scope `static_assert`s that a set is an aggregate and trivially copyable were not mutated (corrected after the review, finding N7). Validation: performed for the oriented names of rigid(3), compared with Lengyel's names through the golden list (CONVENTIONS 1.6, Confirmed), and passed; not otherwise applicable to a storage layout.

### Open

- Agent cost is not recorded here: the coordinator logs it from the harness's figures.
- The W5 review. After `feature/cleanup-1` merges, `test/CMakeLists.txt` and the trace table need a merge; this unit's lines sit in a region the cleanup does not touch, and the cleanup's trace gate (a row for every item) must be rerun on the result.
