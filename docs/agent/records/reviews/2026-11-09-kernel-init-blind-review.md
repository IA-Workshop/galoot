# Blind review of plan 0032, one initializer per place in the kernels

- Reviewer: Claude (agent, Opus 5.5, high effort), blind-reviewer role (W5). My configuration shares no working context with the implementer of plan 0032.
- Date: 2026-11-09 local (2026-11-10 UTC, 02:05 to 02:25).
- Reviewed: `feature/core-kernel-init` at `6d00878` against the claim `e2f317a` (`git diff e2f317a 6d00878`). The code changes are `6b8e2b6`, the canaries cherry-picked with `-x` from `cc6565c` (the two trees are identical), and `6d1624e`, the fix. Every later commit holds records only.
- Record branch: `records/kernel-init-blind-review`, branched from `6d00878`, worktree `.claude/worktrees/review-kernel-init`. The supporting files are in `2026-11-09-kernel-init-blind-review/` beside this file.
- The brief carried no verdict. It did state the implementer's claims as questions (the three IEC 559 arrays, the equivalence of the `run_sum` mutant and the Clang mutants), and I checked each one independently.

## Departures, skipped checks, and ambiguities (C20)

1. **I read the plan's outcome section.** The brief asked for sections 1, 3, and 4. Section 11, the implementer's own outcome, is in the same file, and I read it before I knew it was there. I read the mutation record's README after my own mutant spec was written and running, and the cost README before I recomputed its figures from the raw rows. My mutants and probes come from the header, the tests, and the research record, not from the implementer's specs.
2. **The role file says the reviewer returns the record and the merging agent commits it.** The brief asks me to commit it on `records/kernel-init-blind-review`. I followed the brief. I did not touch the reviewed branch, and I did not merge.
3. **Not rerun (P10): the P9 matrix and lint.** The rows `gate:all` at `1c5a9bf` show all eight presets passing, plus `format-check`, the trace gate, and `tools/runlog check`. `6d00878` differs from `1c5a9bf` only in records. Per the brief, I ran no full matrix. I did run `clang-format --dry-run --Werror` on the three changed sources and the trace gate at `6d00878`. Both pass.
4. **Not rerun: the cost campaign.** It needs the timing lock and a pinned core. I recomputed every figure in its README from `out*/rows.tsv` instead (below).
5. **Files read in part.** For `mv.hpp` and `linear.hpp` I read the changed regions, `coefficient_storage`, `identity_kernel`, `sum_layout`, every caller of the three kernels, and every `{}` in `include/`. For `mv_test.cpp` I read the canaries and the earlier storage canary. I did not read every unchanged line.
6. **My spec declares `expect = "killed"` on all three presets.** On Clang I expected three of the four mutants to survive, because the forms they restore are correct C++ and Clang compiles them correctly. `tools/mutate` therefore reports "problems" on clang-debug. `helper_gives_one` also reports "declared killer core.mv did not kill it", because a `static_assert` in `mv_test.cpp` killed it at compile time before any test ran. Both are expected outcomes, not defects of the change. The table below gives each verdict.
7. **Probes outside the build trees.** Four small programs were compiled directly with `g++` and `clang++` through `tools/timed`, with the campaign log `2026-11-09-kernel-init-blind-review/probes/runs.tsv`.

## Findings

No finding blocks the merge.

### Must-fix

None.

### Should-fix

**S1. The conversion canary's reference states a wrong orientation.** At `test/core/mv_test.cpp:2078` the comment says "e42 and e43 have sign +1 in both layouts". CONVENTIONS 1.6 gives e42 = −e24 and e43 = −e34. `probes/sign.cpp` prints sign −1 for both blades in `blades<rga3, "e42 e43">` and in the motor layout. The expected value is still right, because the contract multiplies by the product of the two signs, (−1)(−1) = +1.

- Failure scenario: a maintainer checks the reference against CONVENTIONS and finds a contradiction. Or someone reuses the reasoning for a source layout that names a blade in the other orientation (`e24`), and gets the sign of the expected value wrong.
- Fix: replace the phrase with "e42 and e43 have the same orientation, −1, in both layouts, so each sign product is +1". Plan criterion 1 has the same ambiguity ("which are +1 for e42 and e43 in both").
- Merge: not blocking. This is a change to a comment only, and W5 lets the review record list it with its resolution, with no delta review.

### Notes

**N1. The mutant specs are well formed, and both equivalence claims are sound.**

- Each anchor includes the following line, so it is unique, and the comment line above each kernel stays unchanged.
- The selections cover every program their labels run, which `tools/mutate` enforces.
- `gap/gap.toml` is a byte-for-byte copy of `gcc.toml` that still expects `killed`. The gap verdict "survived" is therefore read by the record, not checked by the tool, and that run exits 1 by design. The README says so.
- The Clang mutants are equivalent because the restored form and the helper give the same value in the C++ abstract machine. That is a stronger reason than the one in the spec, "Clang compiles it correctly", which is also true. The GCC kills are therefore guards against a compiler defect, not tests of the library's semantics. That is the right framing for a workaround.

**N2. The `run_sum` equivalence holds, and it rests only on the target being the union.**

- `sum_layout` is `merge` of the two layouts. Both callers (`linear.hpp:250`, `:271`) pass it.
- `identity_kernel` marks the first term on each place through `mark_first_terms`, and `second_operand_kernel` marks a blade first exactly when A lacks it. So every place is assigned before it is read.
- `probes/sum_reach.cpp` builds on the old kernels and checks three cases.
  - A public sum, with its first constant-evaluation write at place 2: right on both compilers.
  - A direct `detail::run_sum` onto the union: right on both compilers.
  - A direct `detail::run_sum` onto a target one place wider: wrong on GCC at -O0 and -O2 (place 3 reads 0/0), right on Clang. With the tip's kernels it is right on GCC.
- So the equivalence holds by an argument, and the probe has a positive control. Keeping the fix in `run_sum` costs nothing measurable and protects a future caller with a wider target. No test can see it there, as the record says.

**N3. The canaries' failure output does not say which place is wrong.** `CHECK(m == expected)` prints `{?} == {?}`, because the motor has no doctest printer. The denominator loop prints `0 == 1` once per bad place, without the place's index. Without a debugger, a failure report shows how many places are bad but not which ones. A `CAPTURE` of the index, or an indexed loop, would fix this. Optional.

**N4. No site was missed, and the transform claim is sound for every type the concept documents.** My grep of `include/` agrees with the research record's section 4.3.

- Every other value-initialized array holds `kernel_term`, `plan_term`, `typed_term`, `bool`, `char`, `int`, `std::size_t`, `coefficient_type`, or `printed_name` (chars and a size, in a `consteval` function). Each of these is zero when all its bits are zero.
- Multivector returns (`R{}`, `return {}`, `Motor<T>{}`) go through `coefficient_storage`'s per-place initializer.
- `basis_change.hpp:985` and `dual.hpp:549` reach the fixed kernels. `dual.hpp:549` has one place, so no first write can come at place 2.
- `transform.hpp:510`, `:526`, `:586` take `T` with `rounding_checked_scalar`, which `transform.hpp:416-422` documents as an IEEE 754 binary type.
  - For float, double, and the test's `coarse` (a `float` member defaulting to `0.0F`), the GCC zero fill gives +0, which is the right value.
  - `absolute_values` (`:526`) and the tolerance loop (`:586`) write every place, by reading.
  - `padded_magnitude` (`:510`) may leave places unwritten, and +0 is right there too.
- One residual case remains. The concept checks only `numeric_limits`, so a user class that declares `is_iec559` while carrying a member whose default is not zero bits passes it. On GCC, such a type could get a wrong value in an unwritten place of `padded_magnitude`. I judge this unlikely, and it can only affect a tolerance.
- On open item O1, I recommend taking the helper in `padded_magnitude` alone, the one array of the three that may hold unwritten places. That is one line for one form in the core. Leaving all three is also defensible, if the concept's comment is read as a requirement.

**N5. The helper is robust beyond what the canaries exercise.** `probes/helper_stress.cpp` shares one `zero_places` instantiation among four kernels. Its constant evaluations write first at places 2, 5, and 15 of 16, and at every place. It finds no wrong place at -O0 to -O3 on either compiler. Its control, the same program with `out{}`, finds 29 wrong places on GCC. The canaries test R = 8 with first writes at places 2 and 6.

**N6. The cost record supports its numbers.**

- Every mean, range, maximum, percentage, asterisk, and MiB figure in `kernel-init-cost/README.md` reproduces from `out*/rows.tsv`. The MiB figures are truncated: 679 for 679.8.
- Every row is pinned, and the sibling thread was busy for at most 0.04 s.
- One wording point. Commit `b121014` and plan section 11 say "No unit grows by more than 0.82%". Clang's `places256/map_rational` grows by +1.55%, inside its 2.07% range. The README's wording, "the largest growth beyond the variation", is the accurate one.
- The campaign measured -O0 (the debug trees) only, as the criterion asks. The -O2 build cost of a 1,024-element pack was not measured.
- The peak-memory variation over repeats is unknown, as the README declares. That leaves GCC's `core/linear` +17.2 MiB unexplained.

**N7. Process and style conform.**

- The canary commit precedes the fix, is labelled failing, and names both cases (W2, W9). Every commit names the agent and its role (W7).
- The `include/` and `test/` diff cites no provision, decision, plan, or agent record. The comments name GCC 16.2.1 and the form only (S11).
- `zero_places` has three present callers (S16). It has no `static_assert` of its own. I read S7's clause as aimed at metaprogramming facilities rather than a value helper, and my `helper_gives_one` mutant shows that existing tests see its value.

**N8. The run log is complete, with one ambiguous row (plausible, not confirmed).** The canary commit's `gcc-release` row `test ^core\.mv$` ran 0.058 s with exit 8. Only `core_mv_test` was built in that tree, so CTest probably failed on the label's other, unbuilt programs, not on the canaries. The evidence for gcc-release is the following `test-program` row (exit 1), and my own run, which shows the canaries failing there. The record's departures already list one lost configure row and one untimed direct run.

**N9. A small defect in a committed probe.** `probes/reach.cpp` contains `const char* xs[] = {};`, a zero-length array. ISO C++ does not allow it (it is a GNU extension), and the code is dead. It is a record, not a source. Harmless.

## Outcome

Approve, with one should-fix finding that changes only a comment (S1) and notes N1 to N9.

The fix is correct, and on GCC 16.2.1 it avoids the defect for every form and use I tried: the three kernels, the shared helper, and R up to 16. The canaries fail on the old code on GCC and pass on Clang, as claimed. They test what their names say. They also guard the helper and the other two wrong spellings. The `run_sum` and Clang equivalences are sound.

## Acceptance criteria (plan section 4) and definition of done (section 3)

| Item | Verdict | Evidence |
| --- | --- | --- |
| AC1 conversion canary | met, apart from S1 (a wrong comment) | `mv_test.cpp:2051-2093`; the expected `(0, 0, 1, 2, 0, 0, 0, 0)` follows from the contract at `mv.hpp:602-608`; namespace-scope `constexpr` plus a run-time value through `volatile` |
| AC2 product canary | met | e1 ∧ e2 = +e12 (CONVENTIONS 1.6), place 6 of the motor layout with orientation +1 (probe); place 7 is the only place no term writes |
| AC3 fail on GCC, pass on Clang, against the old kernels | met | my own run at `6b8e2b6` on three presets |
| AC4 helper | met | `mv.hpp:331-335`, `{(static_cast<void>(P), T{})...}` in `detail`. R = 0 expands to `return {};` of `std::array<T, 0>`, which has no element (by reading; I did not check whether the suite instantiates it) |
| AC5 mutants and gap | met | specs well formed (N1); my gap rerun agrees |
| AC6 build cost | met, with three declared departures (Clang stops at 256 places; the product is measured at 512 terms; "before" is `6b8e2b6`) | recomputed (N6) |
| AC7 / DoD5 P9 matrix | met per the record, not rerun | `gate:*` rows at `1c5a9bf` |
| DoD1 to DoD4, DoD6 | met | as above; not merged |

The plan's section 1 readings, as I judge them:

- Item 1 (transform's arrays): the claim is sound (N4).
- Item 2: I agree with it. The only public exposure is `run_map` through a conversion to a wider layout.
- Item 3: confirmed by the probe (N2).
- Item 4: the trigger of a reported comparison was fixed before the measurement.
- Item 5: outside this unit.

## What I checked and how

All builds and tests went through `tools/timed` with the shared pool, which read "23 of 23 slots free" before and after. I never started or stopped it. My run rows (53) are appended to `records/performance/runs.tsv` on this branch, and `tools/runlog check` passes afterwards. I fetched nothing from the web and contacted nobody.

| Check | How | Result |
| --- | --- | --- |
| Canaries against the old kernels | worktree detached at `6b8e2b6` (old kernels with canaries); built `core_mv_test` on gcc-debug, gcc-release, clang-debug; ran the program | GCC, both presets: 84 cases, exactly the two canaries fail (conversion: four denominators 0, the run-time places 4 to 7; product: one denominator 0, place 7). Clang: 84 of 84 pass |
| Canaries at the tip | the baselines of my mutant runs at `6d00878` | pass on all three presets |
| Implementer's gap claim | worktree at `6d00878` with `test/core/mv_test.cpp` from `e2f317a`; `tools/mutate` with a copy of `gcc.toml`, gcc-debug, the two killable mutants | both survive, as the record says (`gap/out/gap-gcc-debug.tsv`) |
| My own mutants | `reviewer.toml`, four mutants, gcc-debug, gcc-release, clang-debug | table below |
| `run_sum` equivalence | read `sum_layout`, `identity_kernel`, `second_operand_kernel`, and both callers; probe `probes/sum_reach.cpp` | sound (finding N2) |
| The helper's form under stress | `probes/helper_stress.cpp`: four kernels sharing one `zero_places` instantiation, constant evaluations first writing at places 2, 5, 15 and at every place, R = 16, -O0 to -O3 on both compilers; a control with `out{}` | helper: 0 wrong places everywhere; control on GCC: 29 wrong places at -O0 and -O2 |
| Other sites in `include/` | grep for every `{}`, `= {}`, `return {}`, `R{}`, every `std::array<T…>` and alias; read each hit's element type | no missed site (finding N4) |
| Orientations in the conversion canary | `probes/sign.cpp`, printing the layouts' oriented blades | finding S1 |
| Cost record | `costcheck.py` (scratch) recomputed means, ranges, maxima, percentages, asterisks, and MiB from `out*/rows.tsv` | every README figure reproduces (finding N6) |
| Style and governance | S11 grep of the `include/` and `test/` diff for citations of agent work or provisions; commit messages; W2, W9 ordering; P13 rows | conforming (notes N7, N8) |

### Mutants

Every mutant in `reviewer.toml` changes the helper or the form a kernel starts its result from. The selection is `core.mv`.

| Mutant | Change | gcc-debug | gcc-release | clang-debug |
| --- | --- | --- | --- | --- |
| `helper_returns_braces` | `zero_places` returns `{}` | killed (both canaries, plus "A conversion between layouts keeps every coefficient<rational>") | killed (both canaries) | survived (correct C++, compiled correctly) |
| `helper_gives_one` | `zero_places` gives `static_cast<T>(1)` per place | killed (a `static_assert` at `mv_test.cpp:1971`) | killed (same) | killed (same) |
| `run_map_braced_temporary` | `out = std::array<T, R>{};` in `run_map` | killed (conversion canary) | killed | survived |
| `run_product_copy_list` | `out = {};` in `run_product` | killed (product canary) | killed | survived |

These results show that:

- The canaries guard the helper itself as well as the three kernel lines. A helper that returns a value-initialized array is miscompiled on GCC in the same way, as the research record's `T t = zero()` row predicts, and the canaries catch it.
- The helper's value reaches results on every compiler. A wrong zero is caught at compile time by an existing `static_assert`.
- The canaries catch all three spellings that the research record found wrong (`out{}`, `out = {}`, `out = T{}`), not only the one the implementer's mutants restore.

## Open items for the owner

1. **S1, the comment in the conversion canary.** Options: the implementer fixes it before the merge, or after it as a follow-up. Tradeoff: the change touches a comment only, needs no delta review, and costs one commit. Recommendation: fix it before the merge.
2. **O1, transform's three IEC 559 arrays (N4).** Options: leave all three; take the helper in `padded_magnitude` alone; take it in all three. Tradeoffs: leaving them relies on the concept's documented meaning, which its definition does not enforce. The helper in `padded_magnitude` closes the one residual case at no measured cost. All three gives a single form everywhere, but `absolute_values` and the tolerance loop gain nothing from it. Recommendation: take the helper in `padded_magnitude` alone, in a later small unit.

## Cost of this review

- Tool calls: about 70.
- Tokens: not visible to me. I estimate 150k to 200k in context. The coordinator records the actual figure.
- Wall time: about 20 minutes (02:05 to 02:25 UTC).
- Machine time: about 6 minutes of mutant builds and tests across two concurrent runs, under a minute of old-code builds, and under 3 s of probe compiles. All of it is in the run rows.
