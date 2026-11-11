# 0032 Kernel initializers: one initializer per place in run_map, run_product, and run_sum

- Status: closed; the unit merged into `develop` by `5edf616` on 2026-11-09, after a blind review (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: claimed 2026-11-10 by Claude (agent, Opus 5.5, cpp-expert role), planner and implementer, from `develop` at `7cc0825`. Not merged by its author; it stops for a blind review (W5).
- Source of scope: decision 0225 item 2 (the owner chose form A), on the evidence of `records/research/2026-11-09-gcc-constexpr-init-bug.md` and its directory.
- Branches, from `develop` after this claim (W7):
  - `feature/core-kernel-init`, worktree `.claude/worktrees/core-kernel-init`: the unit. Only this branch merges into `develop`.
  - `feature/core-kernel-init--tests`, worktree `.claude/worktrees/core-kernel-init--tests`: the canary commits, never merged; cherry-picked with `-x` into the unit (decision 0185).
- Record locations: `records/mutations/0032-kernel-init/`, `records/performance/kernel-init-cost/`, free on every branch at this commit.

## 1. Departures, ambiguities, and readings (C20)

1. **"Grep for all sites with the same pattern."** Read as: every run-time value-initialization `{}` of an array of the user's scalar type in `include/`. The grep finds six: the three kernels, and `padded_magnitude`, `absolute_values`, and the tolerances of `transform_tolerances` in `transform.hpp` (`:510`, `:526`, `:586` at `7cc0825`). The three in `transform.hpp` are left as they are: their T satisfies `rounding_checked_scalar`, an IEC 559 type, whose value-initialized +0 is all zero bits, so the defect's zero fill is the right value; and `absolute_values` and the tolerance loop assign every place before returning. The decision names the three kernels only. Every other value-initialized array in `include/` holds a type whose zero is all zero bits (the scan's section 4.3). Open item O1.
2. **`run_product` has no public path to a wrong result.** A public product (`geometric_product`, `wedge`, the antiproducts, `compose`) is on its plan's result layout, where a term reaches every place. `transform`'s second kernel, probed over 70 pairs of rigid(3) layouts (`records/performance/kernel-init-cost/probes/reach.cpp`), reaches either every place of the target or none, and a kernel with no term writes nothing for a constant evaluation to split. The one path with an unwritten place is `detail::apply` onto a wider target, which `core.mv` already tests as the kernel's interface ("apply onto a target layout equals the converted result"). The product canary goes through it.
3. **`run_sum` has no path to a wrong result at all.** A sum's target is `sum_layout`, the union of its operands' layouts, and a term reaches each place, so the zero fill is always overwritten. The fix is applied there because the decision names it and the kernel's shape is the same; its mutant is declared equivalent on both compilers, with that reason, and no canary is written for it.
4. **"Notably more build time."** Read, before any measurement, as: the mean retired instructions of a largest-place unit grow by more than 5% on either compiler, or of an existing test source by more than 1%, both beyond the range of the repeats. If so, the figures are reported, form B is checked against the canaries, and the fallback rule of the brief applies. No bound is set (C14); this is only the trigger of a reported comparison.
5. **The tests' own reference kernels** (`test/core/mv_test.cpp`, the loops of the apply cases) value-initialize arrays of the rational the same way. They are latent, not exposed (the scan's section 4.3), and not in the decision; they are left as they are. Open item O2.

## 2. Goal

GCC 16.2.1 compiles a run-time value-initialization `std::array<T, R> out{};` wrongly when a constant evaluation of the same declaration in the same translation unit wrote first at place 2 or later: from that place on, the run-time zero holds zero bits, which for a scalar whose zero is not all zero bits (a rational's 0/1) is not zero. The three kernels that build a multivector's coefficients from straight-line terms value-initialize their result this way and promise that the places no term reaches are zero. Give each one initializer per place, as `coefficient_storage` already does, and pin the behavior with canaries that fail on GCC against the old code.

## 3. Definition of done

1. `run_map`, `run_product` (`mv.hpp`), and `run_sum` (`linear.hpp`) initialize their result with one `T{}` per place through one helper; each keeps a comment that says why the shorter form is not used.
2. Two canary cases in `core.mv` (`test/core/mv_test.cpp`), committed first on the tests branch and failing on gcc-debug against the old kernels, passing on clang-debug; passing on both after the fix.
3. A `tools/mutate` spec restoring `out{}` in each kernel, run at the tip on GCC and Clang, with its gap judged at the suite without the canaries; a mutation record.
4. Before and after build cost (P1, W10): retired instructions, wall time, and peak memory of representative sources and of the largest-place cases (1,024 places), on GCC and Clang.
5. The P9 matrix passes from empty build trees at the final tip.
6. The unit stops for the blind review; not merged by its author.

## 4. Acceptance criteria

1. **Conversion canary** (`run_map`): a value on the rigid(3) layout `e42 e43` over the test rational, converted to the motor layout `1 e41 e42 e43 e23 e31 e12 e1234`, at namespace scope as a `constexpr` variable (the constant evaluation's first write lands on place 2) and at run time from a value read through `volatile`. The run-time motor equals `(0, 0, 1, 2, 0, 0, 0, 0)` and every denominator is 1. The expected value comes from the conversion's contract (each coefficient on its blade, times the product of its signs in the two layouts, which are +1 for e42 and e43 in both; every other blade zero), not from the implementation.
2. **Product canary** (`run_product`): the wedge of `3 e1` and `5 e2`, written by `detail::apply` onto the motor layout, as a namespace-scope `constexpr` and at run time from `volatile`. e1 ∧ e2 = e12, the motor's place 6, so the result is `(0, 0, 0, 0, 0, 0, 15, 0)`; place 7 (e1234) is the one no term writes. Every denominator is 1.
3. Both canaries fail on gcc-debug and gcc-release against `7cc0825`'s kernels (the run-time places 4 to 7, and 7, read 0/0) and pass on clang-debug, as the prototype in `records/performance/kernel-init-cost/probes/` showed before the plan was written.
4. **The helper** is a `constexpr` function template in `detail` of `mv.hpp`, returning `{((void)P, T{})...}` for an index pack of R places; R = 0 gives an empty array.
5. **Mutants** (`records/mutations/0032-kernel-init/`), each restoring `std::array<T, R> out{};` in one kernel:
   - `run_map_shared_initializer`: killed on gcc-debug and gcc-release by the conversion canary; equivalent on clang-debug (Clang compiles the form correctly).
   - `run_product_shared_initializer`: the same with the product canary.
   - `run_sum_shared_initializer`: equivalent on all three, for the reason of section 1 item 3.
   - The gap: the canaries close a gap in `core.mv`. A mutant counts for the gap when the suite without the new tests lets it survive and the suite with them kills it. The first two are run on gcc-debug at the tip both with `test/core/mv_test.cpp` as at `7cc0825` (expected: survived) and as at the tip (expected: killed).
6. **Build cost**, `tools/compiles cost` on the debug trees, before `7cc0825`'s include and test against the tip, three repeats pinned to one performance core under the timing lock:
   - existing sources that instantiate the kernels: `core/mv`, `core/linear`, `core/product`, `core/unary`, `core/transform`, `core/basis_change`, `presets/rga3/objects`, `presets/rga3/motions`;
   - text units at 1,024 places in R(10,0,0), over double and over the test rational: a conversion from the scalar layout to `full<10>` (`run_map`), the sum of two `full<10>` values (`run_sum`), and the geometric product of a scalar and a `full<10>` value (`run_product`, 1,024 terms).
   The before units of the canaries' source differ by the canaries themselves; the record says so.
7. **Checks** of section 7 pass at the tip.

## 5. Approach and order

0. Claim: commit this plan to `develop`.
1. Tests branch: the two canaries in one commit, labelled failing on GCC (W2, decision 0216), naming the two cases.
2. Unit: cherry-pick the canary commit with `-x`; then the fix in its own commit.
3. Evidence: the mutants (at the tip, and the gap run), the cost campaign, the records.
4. P9 matrix from empty trees at the final tip; plan outcome; stop and report.

## 6. Evidence (V1, V10)

| Claim | Evidence | Kind |
| --- | --- | --- |
| A conversion to a wider layout is zero on the blades the source lacks, after a constant evaluation of the same conversion | the conversion canary, on GCC and Clang | V1 kind 3 (a value from the contract) |
| A product written onto a wider target is zero on the places no term reaches, likewise | the product canary | V1 kind 3 |
| The canaries see the defect | they fail on GCC against the old kernels, and the first two mutants are killed on GCC | mutation record |
| The fix adds the stated build cost | the cost campaign | V15 |

Validation (V8): not applicable; a compiler workaround with no new mathematics. Verification is on GCC 16.2.1 and Clang 23.1.4 only, the installed compilers.

## 7. Checks (P8, P9, P13)

| Point | Checks | Presets |
| --- | --- | --- |
| The canary commit (tests branch) | `core.mv` | gcc-debug (expected to fail the two canaries), clang-debug (pass) |
| The fix (unit) | every test that includes `mv.hpp` or `linear.hpp`: the full suite | gcc-debug, clang-debug |
| Before the review | the P9 matrix from empty trees: gcc and clang debug and release, gcc-asan and clang-asan, lint, clang-measure, format-check, the trace gate, `tools/runlog check` | all, as `gate:all` |

Every build and test through `tools/timed`, with `eval "$(tools/jobpool env)"` first; `tools/jobpool status` before and after each heavy run; `ensure` only if it reads "not running", and the pool is never stopped.

## 8. Review plan (W5)

A blind review at the Opus level with high effort, from a configuration that shares no context with this agent, given the diff from `7cc0825`, section 4, and section 1. It checks that the canaries fail on GCC against the old kernels (by its own run of the canary commit or its own mutant), that the helper is a per-place initializer on every path that reaches a kernel, and the cost figures against the record's rows. The usability checklist does not apply: no public interface changes.

## 9. Agent cost estimate

Planner and implementer, Opus high: 200k to 350k tokens. Blind review: 80k to 150k. Machine time: the matrix from empty trees about 15 minutes; mutants about 2 minutes per mutant and preset (about 20 minutes with the gap run); the cost campaign about 10 minutes under the timing lock.

## 10. Follow-ups and open items

- F1. **Lesson 0065 and the knowledge base** (decision 0225 item 2): correct the lesson, the knowledge-base note on shared array initializers, and its upstream-awareness row to the trigger the scan measured. Owned by another agent, not this unit. (The lesson's correction of 2026-11-09 is on `develop` at `af9c5df`; the knowledge-base edits are in that repository.)
- F2. **Retest `minimal.cpp`** on each new GCC (decision 0225 item 2), before this workaround or `coefficient_storage`'s is removed.
- O1. The three IEC 559 arrays of `transform.hpp` (section 1 item 1): leave them, or take the helper too for one form in the core.
- O2. The tests' own reference kernels in `mv_test.cpp` (section 1 item 5).

## 11. Outcome

Done, 2026-11-10: the blind review approved the unit with no must-fix (`records/reviews/2026-11-09-kernel-init-blind-review.md`, cherry-picked with `-x` from `d9ca6c9`), and it is merged into `develop`. The owner's rulings on its open items are decision 0226. The history of the unit follows; the changes after the review are at the end.

| Commit | What | Kind (W9) |
| --- | --- | --- |
| `6b8e2b6` | cherry-picked with `-x` from `cc6565c`: the two canaries in `core.mv` | test, failing on GCC when committed, as its message says |
| `6d1624e` | `zero_places` and the three kernels | core |
| `19205b8`, `20ae7ae` | the mutation specs, and the target they lacked | record |
| `d5fafe5` | the mutants at the tip, the gap run, the probes | record |
| `e8c1fa1`, `b121014` | the cost campaign's units and results | record |

Results:

- **Criteria 1 to 3.** Both canaries fail on gcc-debug and gcc-release at `cc6565c` (the conversion's places 4 to 7 and the product's place 7 read 0/0; no other case of `core_mv_test` fails) and pass on clang-debug; with the fix both pass on all three. The full suite passes on gcc-debug and clang-debug at `6d1624e` (540 of 540 each).
- **Criterion 4.** `zero_places<T>(std::index_sequence<P...>)` returns `{(static_cast<void>(P), T{})...}`.
- **Criterion 5.** On GCC (both presets) the `run_map` and `run_product` mutants are killed, each by its own canary alone; on Clang they are equivalent; the `run_sum` mutant is equivalent everywhere. Against the suite without the canaries both killable mutants survive on gcc-debug, so the canaries close the gap. Record: `records/mutations/0032-kernel-init/README.md`.
- **Criterion 6.** No unit grows by more than 0.82% in retired instructions (GCC, 256 places), at 1,024 places by at most 0.64%, and the existing sources by at most 0.64%: below the reading of section 1 item 4, so form A stays and form B was not measured. Record: `records/performance/kernel-init-cost/README.md`. Three departures from the criterion's wording: "before" is `6b8e2b6` (the old kernels with the canaries), so that `mv_test.cpp` is the same on both sides; Clang builds no `mv` on `full<9>` or `full<10>` at its default limits, so the units on both compilers are at 256 places and the 1,024-place units are GCC's alone; and the product of a scalar and `full<10>` does not build on GCC 16 before or after (the plan's result collection, `plan.hpp:313`, fails in constant evaluation), so `run_product` is measured at 1,024 places through `detail::apply` onto `full<10>` (one term) and with many terms at `full<9>` (512).
- **Criterion 7.** The P9 matrix at `1c5a9bf`, from empty build trees (the worktree's `build/` removed first), through `tools/timed` and the shared pool, each row named `gate:<selection>`, all passing: gcc-debug 540 tests (93.7 s), clang-debug 540 (90.3 s), gcc-release 540 (103.0 s), clang-release 540 (110.2 s), gcc-asan 532 (219.2 s), clang-asan 540 (185.8 s), clang-measure 532 (105.7 s), lint 540 (246.3 s, no diagnostic); `format-check` (1.7 s), the trace gate (0.3 s; items 6.1 to 6.4 without a test, as on `develop`), and `tools/runlog check` (2.5 s). The commit after `1c5a9bf` adds only these rows and this paragraph.

Departures and notes beyond section 1:

- **A defect found outside the unit** (W9): `geometric_product` of a scalar and a `full<10>` value of R(10,0,0) fails to build on GCC 16.2.1 inside `collect_result` (a `std::span` bounds check in constant evaluation, `plan.hpp:313`); scalar times `full<9>` builds. Whether it is a defect of the plan builder, a limit, or the compiler was not investigated. The documented limits name products of two full layouts, not this case.
- One configure in the gap worktree went to that worktree's run log and was lost with it; one direct run of `core_mv_test` with a test-case filter, on the tests branch, ran outside `tools/timed`.
- The probes of `records/performance/kernel-init-cost/probes/` were compiled in scratch space through `tools/timed` with a campaign log, committed as `probes/runs.tsv`.

After the review (W5; comment and test edits only, so no delta review):

- `72b4ed7`: finding S1, comment only. The conversion canary's reference said e42 and e43 have sign +1 in both layouts; they stand for −e24 and −e34 in both, and the conversion uses the product of the signs, +1, so the expected value was right.
- `fe8a0f2`: note N3, test only. Each canary checks the denominators by index with the place captured, so a failure names the wrong places; the assertions and expected values are unchanged. `core.mv` on gcc-debug and clang-debug, 100 of 100 each.
- `4f2a3af`: note N9, record only. The unused zero-length array in `probes/reach.cpp` removed; the probe's output is unchanged, and it builds with `-Wpedantic -Werror`.
- The review record cherry-picked with `-x`; its 56 run rows apply once each, and `tools/runlog check` passes on the result; `format-check` passes. These edits reach only `core.mv` (P8), so the P9 matrix of `1c5a9bf` was not rerun.
- Lesson 0065 now says the kernels take one initializer per place and names the canaries.
- Decision 0226: the three IEC 559 arrays of `transform.hpp` stay as they are (O1); the tests' reference kernels in `mv_test.cpp` take the safe form in a small test-only change after this merge (O2); the scalar times `full<10>` failure gets its own short investigation (O3); `minimal.cpp` is retested on each new GCC (F2). The reviewer's residual risk on O1 stands as a note: `rounding_checked_scalar` checks only `numeric_limits`, so a class type that declares itself IEC 559 while holding another member whose zero is not all zero bits would pass the concept.
