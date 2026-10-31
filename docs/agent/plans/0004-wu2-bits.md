# 0004 WU2: bits

- Status: done, pending W5 review and merge; branch `feature/core-bits`, worktree `.claude/worktrees/core-bits`
- Goal: `include/galoot/core/bits.hpp`, the constexpr functions on blade masks that every later part uses.
- Content: `mask_t` (`std::uint32_t`, decision 0004), `grade`, `antigrade(mask, n)`, `reorder_sign(a, b)`, the complement mask, the right and left complement signs (CONVENTIONS 3.1), and the reverse, antireverse, and grade-involution signs (2.1). All in `galoot::core`, usable at compile time and at run time (decision 0021 and note 0008).
- Definition of done: the functions exist with `///` comments (S11) and are self-contained (the header-set check); tests pin CONVENTIONS 1.1, 1.5, 2.1, and 3.1.
- Acceptance criteria: (1) every function is checked exhaustively against its definition for n up to 6, by a brute inversion count written in the test and sharing no code with the header (V1 kind 1); (2) the basis-rule functions (reorder sign, complement sign) are checked exhaustively up to n = 8; (3) u ∧ ū = 𝟙 and u̲ ∧ u = 𝟙 hold for every blade, and left(right(u)) = u; (4) the 16-entry table of 3.1 for n = 4 is pinned as golden values derived from the definition; (5) functions reject out-of-range input by `GALOOT_ASSERT` in checking builds; (6) clang-format, clang-tidy, and the include scan pass; (7) the unit's tests pass on both compilers in the correctness presets and the lint preset.
- Approach: direct form first (C3, W4); no table is precomputed. Tests in `test/core/bits_test.cpp`, labeled `core.bits`.
- Evidence: test output per preset, the exhaustive ranges, and the P1 before and after record of the affected suites (W10).
- Agent cost: tracked for information only (decision 0019).
- Review: W5 blind review before the merge into `develop`.
- Open questions: none.

## Outcome (2026-10-31, Claude, agent, Opus 5.5)

Status: done on `feature/core-bits`, pending the W5 review and the merge into `develop`, which the coordinator arranges. Code at `6fba7fc`.

### Done

- `include/galoot/core/bits.hpp`: `mask_t`, `mask_bits`, `grade`, `antigrade`, `reorder_sign`, `complement_mask`, `right_complement_sign`, `left_complement_sign`, `reverse_sign`, `antireverse_sign`, `grade_involution_sign`, all `constexpr`, `[[nodiscard]]`, `noexcept`, in `galoot::core`, with `detail::low_bits`, `valid_count`, `in_algebra`. Added to the header set and to `core.hpp`.
- Tests: `test/core/bits_test.cpp` (test `core.bits`, 12 doctest cases, 80,405 assertions, 5 `static_assert`s); `test/core/bits_assert_fail.cpp` (20 CTest tests `core.bits_rejects_*` and `core.bits_accepts_*`); `test/core/bits_consteval_fail.cpp` (compile-fail test `core.bits_consteval`). All labeled `core;core.bits`.
- Decision 0050: the range 1 <= n <= 32 and why assertion messages name the precondition, not the function.
- Snippet candidate `docs/agent/snippets/bits_fault_injection.py`.

### Departures (C20)

1. Range. Bits accepts 1 <= n <= 32 (the width of `mask_t`), not decision 0004's v1 limit of 16, which is left to the metric (WU3) as a template parameter (decision 0050). Ambiguity: decision 0004 says the limit is "a template parameter"; bits has no template parameter, so the reading followed is that the 16 belongs where an algebra is declared.
2. Messages. `GALOOT_ASSERT` messages are `n must be from 1 to 32` and `blade has a generator beyond e_n`, without the function name, because a message must start on the line of `GALOOT_ASSERT(` (checked: neither compiler shows a message on the next line) and a function name does not fit in 80 columns. Only `antigrade` and `complement_mask` assert; the three sign functions inherit the checks through them.
3. Comments. Mid-unit, the owner's S11 rulings (decision 0034, then `f62ec2f`) barred citing CONVENTIONS or agent records in code. Commit `248c8db` cited CONVENTIONS sections; `6fba7fc` restates each convention in place. WU1's existing citations in `CMakeLists.txt` and `test/CMakeLists.txt` were left for the cleanup unit, as instructed.
4. The P1 before and after are shared-mode records (`build/<preset>/compile.tsv`, not committed), not pinned runs, with the machine lightly loaded (load average 0.4 to 0.5 at the start). Instruction counts are the comparable figure.
5. The before workflows ran in fresh build trees and the after ones in trees cleaned with `ninja -t clean` (configure cached), so the workflow wall times are not comparable; per-TU and test times are.

### Evidence

All commands from the worktree root with `eval "$(tools/jobpool env)"`, through `tools/timed`; rows in `runs.tsv`.

| Criterion | Command | Outcome |
| --- | --- | --- |
| 1 | `ctest --preset <p> -L '^core\.bits$'`; `static_assert(all_agree(1..4))` | Every function against a brute reference (index lists, inversion counts) for every mask with n = 1 to 8; in a constant evaluation for n = 1 to 4 |
| 2 | as 1 | `reorder_sign` on all 65,536 pairs below 2^8; complement masks and signs for every blade, n = 1 to 8; a sample (scalar, antiscalar, each generator and its complement, three alternating patterns) at n = 12, 16, 32 including all pairs of the sample for `reorder_sign` |
| 3 | as 1 | u ∧ right(u) = +e_1..n and left(u) ∧ u = +e_1..n by the test's own exterior product; left(right(u)) = u; left = (-1)^(gr ag) right; all blades, n = 1 to 8 |
| 4 | as 1 | 16 rows hand-derived from the definition and checked at compile and run time. Each row was also compared, by hand, with the design study's rigid(3) table after converting its oriented names (e431 = -e134 and so on): all 16 agree. (Corrected after review N5: a scratch comparison with `test/reference/conventions_check.py` also agreed, but that script computes the complement by the same formula, so it is not an independent route, and the script was never committed; the claim is withdrawn.) |
| 5 | `core.bits_rejects_*` (15), `core.bits_accepts_*` (5), `core.bits_consteval` | Abort with the message and `bits.hpp:` for n = 0, n = 33, and e3 at n = 2 in each of the five functions that take n; the antiscalar at n = 32 is accepted; a constant evaluation fails with the message and `assertion_failed` |
| 6 | `cmake --workflow --preset lint`; `core.include_scan` | clang-tidy on every TU, `format-check`, header sets: no finding; include scan passes |
| 7 | `cmake --workflow --preset <p>` from cleaned trees, for gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint | 32 of 32 each; wall 2.59, 3.98, 6.33, 2.20, 3.25, 4.53, 1.90 s (23.8 s total) |
| DoD | `all_verify_interface_header_sets` on gcc-debug and clang-debug | `bits.hpp` compiles alone on both |

Fault injection (`docs/agent/snippets/bits_fault_injection.py`, gcc-debug): 11 mutations (reorder counting equal indices, reorder always +1, right and left signs swapped, reverse with k + 1, antireverse on the grade, involution mod 4, complement dropping the top generator, n = 32 rejected, n = 0 accepted, mask check dropped). All 11 caught. With the `static_assert`s in place, eight fail the build (checked for three that the failure is `static assertion failed`); with them removed (`--runtime-only`), each of those eight is caught by `core.bits` at run time, and the three range mutations by the rejection, acceptance, and compile-fail tests.

P1 before (`533623a`) and after (`6fba7fc`), debug presets, shared mode, per-TU retired instructions (P- plus E-core), wall time, and peak memory:

| TU | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| smoke test | 0.27 G, 0.13 s, 44 MiB | 0.43 G, 0.16 s, 54 MiB | 0.18 G, 0.14 s, 101 MiB | 0.29 G, 0.11 s, 105 MiB |
| bits test | | 2.11 G, 0.31 s, 106 MiB | | 1.82 G, 0.23 s, 127 MiB |
| bits abort program | | 0.84 G, 0.19 s, 74 MiB | | 0.68 G, 0.16 s, 115 MiB |
| bits compile-fail | | 0.22 G, 0.06 s, 39 MiB | | 0.19 G, 0.04 s, 85 MiB |
| `bits.hpp` header-set TU | | 0.23 G, 0.14 s, 38 MiB | | 0.19 G, 0.14 s, 90 MiB |
| doctest runner | 11.12 G, 1.56 s, 346 MiB | 11.13 G, 1.25 s, 347 MiB | 7.48 G, 0.99 s, 280 MiB | 7.48 G, 0.96 s, 280 MiB |

The smoke test grows by 0.16 G (GCC) and 0.11 G (Clang) because `core.hpp` now includes `bits.hpp` with `<bit>` and `<limits>` (inferred: the header itself is small). The four constant-evaluated exhaustive checks cost about 0.9 G instructions on GCC: the bits test measured 1.05 G to 1.12 G with them removed during fault injection.

Test time: CTest's total for the suite was 0.49 s (GCC) and 0.52 s (Clang) for 10 tests before, and 0.64 s and 0.59 s for 32 tests after. `core.bits` itself takes 0.21 s (gcc-debug), 0.07 s (gcc-release), 0.18 s (gcc-asan), 0.22 s, 0.06 s, 0.13 s (Clang), 0.18 s (lint).

Not checked: the measurement presets (P9 does not gate on them); pinned per-TU timings.

Verification and validation status (V10): verification performed and passed (exhaustive checks against the definitions for n <= 8 at `6fba7fc`, extended at `8ac9d93` by the diagonal-product check; sampled at n = 12, 16, 32; fault injection). Validation performed against one referent only and passed: the rigid(3) complement table of the design study, which the study reports matches Terathon's signs. Lengyel's published complement pages were not reread in this unit (research note 0005 records their reading), so validation against the published source rests on that note.

### Next step

W5 review of `feature/core-bits`. The reviewer gets plan 0004's criteria, the departures above, and the test list in the final report.

### Open questions

1. Whether bits should instead assert n <= 16 (decision 0050, option 1). WU3 depends on the answer.
2. `test/CMakeLists.txt` and `include/galoot/core.hpp` are likely also changed by `feature/core-metric`; the merge will need both header lines and both test blocks.

### Agent cost (decision 0019)

About 75 tool calls in one session; the harness reports no token counts to this agent.

## W5 fixes (2026-10-31, Claude, agent, Opus 5.5)

The review (`docs/agent/records/reviews/2026-10-31-wu2-bits.md`, of `7c3f5f0`) approved with non-blocking findings. Fixed on the branch after merging `develop` at `9586a9c` (`0129611`, records and decisions only):

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| N2 | n is `std::size_t` in every bits function and in `mask_bits`; grades stay `int` (decision 0051). Tests follow. (`4292517`) | `int n`; a `std::size_t` count from the metric needed a cast under `-Wconversion` | Both compilers build `-Wconversion -Werror` clean; lint clean after `8ac9d93` |
| N1 | The abort program uses `mask_bits + 1`, `mask_bits`, `~mask_t{0}`; the test reference reads its width from `numeric_limits<mask_t>::digits` and checks `mask_bits` against it; case `n_33` renamed `n_above` (CTest `core.bits_rejects_<f>_n_above`). Messages and their CMake expectations unchanged. (`4292517`) | Literals 33, 32, 0xFFFFFFFF | No bound literal in the tests except the message text |
| N9 | Usage errors print a usage line with `std::fputs` (no suppression); the sample capacity is 2n + 5 of the widest mask, with its derivation. (`4292517`) | Exit 2 silently; capacity 80 unexplained | Usage line on stderr; capacity 69 |
| N8 | New case "In a diagonal metric, e_A e_B is reorder_sign times the squares of the common generators times e_(A xor B) ...": all 4,096 pairs of six generators, in R(6,0,0) and in squares (1, -1, 0, -1, 1, 0), against a bubble sort with contraction. (`106069e`) | Guarantee untested | Passes (16,384 assertions). With `reorder_sign` counting equal indices and the `static_assert`s removed, it fails 3,096 of them |
| N4 | The snippet refuses a dirty header or test (exit 2), strips inside the `try`, and restores both files from the text it read. Two mutations rewritten for the unsigned count. (`d04ca79`) | `git checkout` restore; strip before `try` | With a scratch edit in the test: refused, exit 2, edit kept. Clean tree: 11 of 11 caught in both modes |
| N5 | Criterion 4 corrected above; the V and V status line added. | | |
| N6, N7 | Recorded by the coordinator; nothing changed. | | |

P9 on `8ac9d93`, each from a tree cleaned with `ninja -t clean`, through `tools/timed workflow <p> all-clean -- cmake --workflow --preset <p>`: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, and lint (clang-tidy, `format-check`, header sets) each passed 32 of 32, `core.bits` running 13 cases and 96,789 assertions in each. Wall times (s): 2.94, 3.68, 4.92, 1.33, 1.79, 3.49, 1.80; total 19.95 s. `all_verify_interface_header_sets` built under gcc-debug (0.14 s) and clang-debug (0.30 s). An earlier P9 run on `d04ca79` passed six presets and failed lint on four clang-tidy findings in the test reference, fixed in `8ac9d93`.

Departures in this pass (C20):

1. `4292517` changes the header and its tests together, because the type change does not build otherwise (W2); W9's separate commit applies to `106069e` (new test) and `8ac9d93` (reference refactor).
2. One run of `core_bits_test` was made directly, not through `tools/timed`, to read the doctest counts after adding N8.
3. Reference markers (`// Reference:`) for the 13 test cases are not added yet; the coordinator schedules them after the M0 scripts merge.

