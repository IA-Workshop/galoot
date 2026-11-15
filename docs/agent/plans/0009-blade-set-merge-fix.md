# 0009 Blade set merge and index_of fixes

- Status: closed; the unit merged into `develop` by `5393f05` on 2026-11-01 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: done, pending W5 review and the merge into `develop`; branch `feature/blade-set-merge-fix`, worktree `.claude/worktrees/blade-set-merge-fix`; decision numbers 0093 to 0099 (0093 used).
- Goal: fix the defects the review of the unreviewed fix commits found in `merge` and `index_of` (record `docs/agent/records/reviews/2026-10-31-unreviewed-fix-passes.md`, findings A1 to A6, and the D1 half that concerns the ONLY mode is left for a later tooling unit).
- Definition of done: A1 to A6 fixed or recorded with a reason; the public contract in the header and decision 0092 true of the code; new tests each shown failing against the merged header and passing after; the full suite passes on all seven presets.
- Acceptance criteria: (1) `merge<{e1:+1}, {e1:+1, e1:+1}>` and `merge<{e1:+1}, {e1:+1, e1:-1}>` are rejected with "a set holds one mask twice" and without "opposite signs"; (2) `opposite_signs` is set only when both signs are plus or minus one and differ, with a test that forbids "opposite signs" for a sign-0 input; (3) a bad sign in the second input is rejected, tested; (4) a valid merge whose second input is out of canonical order and shares a mask with the first is accepted, tested, and the mutant that drops the flag shift is killed; (5) the cost of `index_of` is measured at n = 9 and 10 against the merged header and the measured alternative (position search, then a scan from the next position for a repeat); the cheaper form is adopted if it keeps every check the contract promises, or the contract is narrowed to what the code checks, recorded in a decision; (6) the `merge` bound at default compiler limits is stated in the header and decision 0090's note; (7) the random-merge reference of the reviewer (300 random valid merges and lookups) is committed as a test or its absence is recorded; (8) mutations for the survivors of the review (`merge_sign_first_only`, `merge_no_flag_shift`, `merge_repeat_first_only`, `index_count_exactly_one`) are killed.
- Approach: direct form; keep the checks inside the insertion already done; measure before deciding.
- Evidence: test output per preset, mutation records in `docs/agent/records/mutations/`, the cost measurements.
- Review: W5 blind review of the diff (core code) before the merge.
- Agent cost: tracked for information only.

## Outcome (2026-10-31, Claude, agent, Opus 5.5)

Status: done on `feature/blade-set-merge-fix`, pending the W5 review of the diff. Code at `0a8b8f3`.

### Wrong claims acknowledged

- A1. Decision 0092 and the `merge` contract said that `merge` checks every mask of both inputs. It did not: a repeat in the second input of a mask the first input held was missed.
- A2. Decision 0092 and the message of `e191731` said that the removed guard hid only a true second message. It hid a false one, "opposite signs" for a sign-0 input. The author had read a surviving mutant as proof of redundancy without building the input that tells the two apart.

Both are corrected by a dated section appended to decision 0092.

### Departures (C20)

1. The definition of done asks that each new test fail against the merged header. Only the A1 and A2 tests can: the others judge behavior the merged header already has right. Those were committed before the fix (`9670158`, `c24bfe4`). Their evidence is that each kills a mutant: the review's survivors on the merged header, or survivors of the fixed header.
2. W2 broken: from `9670158` to `3cda375`, `tests.reference_markers` failed. A `;` inside the new merge test's reason split the marker. The author did not run the marker check before those commits, and P9 found it. Fixed in `0a8b8f3`; history is not rewritten.
3. Mutants of the fixed header found three more gaps. Two were closed by tests (`c24bfe4`), and a guard with no effect was removed (`3cda375`). The guard's removal rests on the loop bounds: for an absent mask the scan starts past the end. It does not rest on the survivor alone.
4. The 300 random cases more than double the test unit's compile cost (below).

### Criteria

1. Met. `merge<{+e1}, {+e1, +e1}>` and `merge<{+e1}, {+e1, −e1}>` give "merge: a set holds one mask twice", the second with "opposite signs" forbidden (`core.blade_set_merge_repeated_in_second`, `…_signs`). Against the merged header the first compiled (result 0, message absent) and the second printed only "opposite signs". The fix is in `644939e`.
2. Met. Opposite signs need both signs ±1. `core.blade_set_merge_bad_sign_no_conflict` (`merge<{e1 sign 0}, {+e1}>`) forbids "opposite signs"; the merged header printed it.
3. Met. `core.blade_set_merge_bad_sign_second` (`merge<blades<2, "e1">, {e2 sign 0}>`) requires "merge: a set holds a sign other than +1 or -1".
4. Met. The test case "merge of e2 with e3 e1 e2 is e1 e2 e3" expects {e1, e2, e3}, all +1. The flag-shift mutant fails the build at `blade_set.hpp:300` ("holds one mask twice") on the merged header, and at `314` for the second-input shift on the fixed one. The first-input shift is killed by `core.blade_set_merge_repeated_after_shift`.
5. Met. The measurement and choice are in decision 0093: index_of costs 39.1 G (GCC) and 55.5 G (Clang) at n = 10, against 62.1 G and 101.7 G merged, and 21.0 G and 32.3 G unchecked. The sign check is kept, once per set, because on Clang it costs nothing measurable and on GCC 16% over the review's alternative, which drops it. The contract is unchanged: signs, and the looked-up mask exactly once.
6. Met. The `merge` documentation and the 0090 note record the limits: Clang 23 merges `full<7>` with itself and fails `full<8>`; GCC 16 merges `full<9>` and fails `full<10>`. These agree with the review.
7. Met. The test case "merge and index_of agree with the union of 300 random pairs of sets" (seed `0x5eedb1ade5e70001`, SplitMix64; n among 3, 4, 5, 6, 9, 12, 32; 1 to 8 blades per set, in the order drawn; signs shared through a hash of the mask) compares each merge with a union sorted in the test, and checks `index_of` for every blade of each result. The reviewer's own 300 cases live in its scratchpad and are not committed; these are new.
8. Met. All four survivors are killed on both compilers: `merge_sign_first_only` by `merge_bad_sign_second`, `merge_no_flag_shift` by the out-of-order test and `merge_repeated_after_shift`, `merge_repeat_first_only` by the two `repeated_in_second` tests, and `index_count_exactly_one` by FORBID "twice" on `index_of_absent`.

### Mutations (`docs/agent/records/mutations/0009-blade-set-merge-fix/`)

- `results-merged-9670158.tsv`: the review's four survivors on the merged header with the first tests. Three are killed on both compilers; `merge_repeat_first_only` survives until its tests, which fail against that header and came with the fix.
- `results-80a0405.tsv`: 13 mutants of the fixed header; 10 are killed on both compilers. The survivors are `merge_no_flag_shift` (first input), `index_repeat_from_next`, and `index_absent_says_twice`.
- `results-c24bfe4.tsv`: the first two survivors are killed by the new tests on both compilers.
- `results-3cda375.tsv`: all 12 mutants of the final list are killed on gcc-debug and clang-debug, 24 of 24. The first build error is recorded for each failed build.

### Tests added

| Test | Input | Expected |
| --- | --- | --- |
| `core.blade_set_merge_bad_sign_second` | `merge<blades<2, "e1">, {e2 sign 0}>` | "merge: a set holds a sign other than +1 or -1" |
| `core.blade_set_merge_repeated_in_second` | `merge<blades<2, "e1">, {+e1, +e1}>` | "merge: a set holds one mask twice" |
| `core.blade_set_merge_repeated_in_second_signs` | `merge<blades<2, "e1">, {+e1, −e1}>` | same; FORBID "opposite signs" |
| `core.blade_set_merge_bad_sign_no_conflict` | `merge<{e1 sign 0}, blades<2, "e1">>` | "merge: a set holds a sign other than +1 or -1"; FORBID "opposite signs" |
| `core.blade_set_merge_repeated_after_shift` | `merge<{+e2, +e1, +e2}, blades<2, "e12">>` | "merge: a set holds one mask twice" |
| `core.blade_set_index_of_repeated_adjacent` | `index_of<{+e2, +e1, +e1}, e1>` | "index_of: the set holds this mask twice" |
| `core.blade_set_index_of_absent` (changed) | as before | adds FORBID "twice" |
| "merge of e2 with e3 e1 e2 is e1 e2 e3" | as named | {e1, e2, e3}, signs +1, written out |
| "merge and index_of agree with the union of 300 random pairs of sets" | as above | 0 mismatches |

No expected value of an existing test changed.

### P9 at `0a8b8f3`

- `tools/timed workflow <p> all`, 94 of 94 tests in each preset:

  | Preset | Wall time |
  | --- | --- |
  | gcc-debug | 18.6 s |
  | gcc-release | 15.7 s |
  | gcc-asan | 16.3 s |
  | clang-debug | 16.5 s |
  | clang-release | 16.3 s |
  | clang-asan | 16.7 s |
  | lint | 22.4 s |
  | Total | 122.6 s |

- `all_verify_interface_header_sets` builds on gcc-debug and clang-debug.
- `tests.citations`, `tests.reference_markers` (43 test cases), and `tests.golden_facts` pass in every preset.
- The trace gate passes: 1.6 at `blade_set_test.cpp:598, 621, 753, 833`; 1.7 at `674, 694, 846`; 1.1 also at `656`.
- An earlier P9 run at `3cda375` failed `tests.reference_markers` in all seven presets (departure 2).

### Costs (shared mode, track-only)

- `blade_set_test.cpp`: 21.30 G, 421 MiB, and 1.64 s on gcc-debug; 45.05 G, 211 MiB, and 2.60 s on clang-debug. On `develop`, the same file cost 8.83 G and 19.82 G.
- Without the random test case (scratch copy, `-fsyntax-only`), it costs 7.25 G (GCC) and 19.42 G (Clang), against 18.83 G and 44.14 G with it. So the 300 random merges and their lookups are the rise.
- The `core.blade_set` label takes 12.2 s on gcc-debug and 10.1 s on clang-debug, for 32 tests; the compile-fail tests still run one at a time under the build-tree lock.

### Verification and validation

Verification: performed and passed. Each defect A1 to A3 has a test that fails against the merged header or kills its surviving mutant. Both compilers agree on every mutant. Validation: not applicable; no convention changed.

### Open

- The ONLY-mode half of D1 is left for a later tooling unit, as the plan says.
- Agent cost is not recorded here.

## W5 review fixes (2026-11-01, Claude, agent, Opus 5.5)

The review (`docs/agent/records/reviews/2026-11-01-blade-set-merge-fix.md`, of `e6f6601`) said merge after F1. The fixes are at `0885bf5` (header) and `28ef08e` (tests), with records in `be02b98`. `develop` at `15ca655`, which contains `caae9aa`, merged as `1d55950` with no conflict.

| Finding | Fix | Before | After |
| --- | --- | --- | --- |
| F1 | The `merge` `///` comment and a dated note on decision 0090 state what was measured, by shape of input | "merges two sets of 128 blades (full<7>) but not of 256; GCC two sets of 512 (full<9>) but not of 1,024", true only for a canonical set with itself | Clang 23: `full<7>` with itself and 64 + 64 disjoint pass, 128 + 128 disjoint fail. GCC 16: `full<9>` with itself and 256 + 256 disjoint pass; 512 + 512 disjoint and `full<9>` reversed with itself fail. Repeated by the author (probes in `merge_bounds/`), matching the review |
| F2 | `merge_repeated_in_second_only`, `merge_repeated_in_second_empty_first`, `merge_bad_sign_second_shared`, each with FORBID "opposite signs" | `second_flag_never` and `opp_blade_unchecked` survived on both compilers | both killed on gcc-debug and clang-debug (`results-28ef08e.tsv`) |
| F3 | The `signs_valid` comment states the measured cost | "adds almost nothing to a lookup" | "costs little on Clang and about a sixth of the rest of a lookup on GCC" (38.9 G against 33.5 G at n = 10) |
| F4 | Each random case is checked on its own with `CAPTURE(c)`; the seed prints in hex | a total count and `random_seed = 6840318769490231297` in decimal | a failure names its case; `random_seed = 0x5eedb1ade5e70001`, as written in the source |
| F5 | 30 cases, with n cycling through 3, 4, 5, 6, 9, 12, 32 by case, so every width appears | 300 cases | `blade_set_test.cpp`: gcc-debug 21.30 → 10.30 G, 421 → 259 MiB, 1.64 → 0.86 s; clang-debug 45.05 → 22.44 G, 1.36 s. The owner may reverse it |
| F6 | The SplitMix64 citation is dropped; the comment says it is a fixed-seed integer hash whose quality does not matter | a citation no one had read | no citation |
| F7 | The random test reuses `mask_list`, `push`, `mismatches`, and `index_mismatches` | `random_list` and two duplicate loops | removed |
| F8 | `[[nodiscard]]` on `position_of`, `unite`, and `binomial` | absent | present |

Tests added (each passes on the header both before and after these fixes, since the code did not change):

| Test | Input | Expected |
| --- | --- | --- |
| `core.blade_set_merge_repeated_in_second_only` | `merge<blades<2, "e1">, {+e2, +e2}>` | "merge: a set holds one mask twice"; FORBID "opposite signs" |
| `core.blade_set_merge_repeated_in_second_empty_first` | `merge<{}, {+e2, −e2}>` | same |
| `core.blade_set_merge_bad_sign_second_shared` | `merge<blades<2, "e1">, {e1 sign 0}>` | "merge: a set holds a sign other than +1 or -1"; FORBID "opposite signs" |

Changed: the random test case now runs 30 cases, named "merge and index_of agree with the union of 30 random pairs of sets", each expected to have 0 mismatches.

Mutations at `28ef08e`: 14 mutants, all killed on gcc-debug and on clang-debug, 28 of 28. The first build error of each failed build is recorded.

P9 at `1d55950`:

- `tools/timed workflow <p> all`, 97 of 97 tests in each preset:

  | Preset | Wall time |
  | --- | --- |
  | gcc-debug | 18.8 s |
  | gcc-release | 15.8 s |
  | gcc-asan | 17.3 s |
  | clang-debug | 16.1 s |
  | clang-release | 16.9 s |
  | clang-asan | 16.2 s |
  | lint | 15.9 s |
  | Total | 117.1 s |

- Header sets build on gcc-debug and clang-debug.
- The trace gate passes: 1.6 at `blade_set_test.cpp:602, 625, 757, 843`; 1.7 at `678, 698, 856`; 1.1 also at `660`.
- Markers (43 test cases), golden facts, and citations pass in every preset.
- The `core.blade_set` label takes 12.4 s on gcc-debug and 10.6 s on clang-debug, for 35 tests.
