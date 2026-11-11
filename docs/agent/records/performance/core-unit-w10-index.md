# Index of core units to their W10 cost records

For DoD-22's second half: "before and after for each core unit (W10)". Each row names a unit that changed `include/` (or, where marked, a unit beside it), the plan that holds its outcome, and where its before-and-after build cost is recorded. Every record is track-only (C14): "recorded" means measured and kept, and nothing here bounds anything. Built on 2026-11-11 from the plans' outcome sections, the directories of this folder, and `git diff --stat` of each merge against its first parent for `include/` (the units that changed no file under `include/galoot/core/` are marked).

Where a plan says "the W10 rows", the rows are in `compile.tsv` in this folder, copied by the step of decision 0080 from fresh before and after trees; the plan's outcome section then holds the summary table. Those rows are shared mode (the pool, not pinned) unless a directory below says pinned. Shared-mode and pinned figures are not comparable with each other (P14).

## The index

| Unit | Plan | W10 record | Form | Gap or note |
| --- | --- | --- | --- | --- |
| WU1, build skeleton (`assert.hpp`) | 0002 | `compile.tsv` rows of the acceptance runs (recorded in plan 0003's outcome) | Pre-core baseline, pinned | No "before": nothing existed. The baseline for all later rows |
| WU1b, recorder follow-ups | 0003 | Plan 0003's outcome, acceptance rows at `6d64975a3b7d`, against the old baseline `5134bb5-dirty` | Before and after, pinned | The change is to the recorder and build tools; the plan 0004 outcome mentions `assert.hpp` comments among its changes (not checked further) |
| WU2, `bits.hpp` | 0004 | Plan 0004's outcome, "P1 before (`533623a`) and after (`6fba7fc`)"; the review's cost split of `bits_test.cpp` | Before and after, shared | None |
| WU3, `metric.hpp` | 0005 | Plan 0005's outcome, "Build and test cost, before and after" (`533623a` and `1dc00cb`); `tag-value/` | Before and after, shared; one pinned probe | Note N11 (the `metric_like` cost at three sizes) was left to the growth grid and is answered by `plan-results/` and `metric-like-single-pass/` |
| Cleanup 1 (comments, citations, gates) | 0007 | None found | | Gap. No cost table in plan 0007's outcome. The change to `include/` is comments |
| WU4, `blade_set.hpp` | 0008 | Plan 0008's outcome, "Costs (W10; shared mode)", corrected after review finding B1 | Before and after, shared | None |
| Blade set merge and `index_of` fixes | 0009 | Plan 0009's outcome, "Costs (shared mode)" and decision 0093 | After, with one before figure for `blade_set_test.cpp` (8.83 G GCC, 19.82 G Clang on `develop`) | Partial. No before and after of `core.hpp` or `smoke_test.cpp` |
| WU6a, WU6b, `blade_product.hpp`, `plan.hpp` | 0011 | Plan 0011's outcome, two "Costs (W10; shared mode)" sections (before `0b8e663`, after `620fbd3`); `overflow-guard/`, `plan-growth/`, `stack-probe/` | Before and after, shared; guard and growth pinned | The after rows predate later test growth in `blade_product_test.cpp`, which the plan says was not remeasured |
| Set-bit loops (growth of a fixed use) | 0012 | Plan 0012's outcome, "Costs (W10; fresh trees, shared mode)"; `product-cost-growth/` | Before (`0f1039a`) and after (`c97fac5`), shared; growth pinned | None |
| WU7a, WU7b, typed results and unary maps | 0013 | Plan 0013's outcome, two "Costs (W10; new trees, shared mode)" sections (before `0bbb968`, after `86226f7` for part a); `plan-results/`, `plan-maps/` | Before and after, shared; growth pinned | None |
| `metric_like` in one pass | 0014 | Plan 0014's outcome (the W10 trees before `c4b2e70`, after `a3cd8c5`); `metric-like-single-pass/` (`w10_summary.py`, `out/p2-c4b2e70-a3cd8c5/`); `metric-like-check/` | Before and after, shared; P2 pinned | None |
| Certified plan limits | 0015 | Plan 0015's outcome table (85 compiles, 124.66 G to 93 compiles, 129.62 G on gcc-debug; 135.93 G to 140.48 G on clang-debug); `plan-cost-model/`, `constexpr-certification/` | Before (`4895a40`) and after (`1f7cf28`), shared | The change to `include/` is comments; the rise comes from the canary and tools tests |
| WU8a, WU8b, `mv.hpp` | 0016 | `mv/README.md`, `mv/runs.tsv`, `mv/out-0acef07`, `mv/out-wu8b-7ee7d32`; plan 0016's outcomes (W10 before rows at `965059e` and `11d1ec9`) | Before and after, shared; limits and uses pinned | None |
| WU9, `product.hpp` | 0018 | None found | | Gap. Plan 0018's outcome records no W10 and no cost row, and says the P2 growth of the new includer is not done. Plan 0019 notes that WU9's P2 measurement failed. `compile.tsv` may hold its tree rows, which no document summarizes |
| WU10a, WU10b, antiproducts and linear operations | 0019 | `antiproducts-cost/`, `linear-cost/` (`out/`, `out-full8/`); plan 0019's outcome (`product_test.cpp` 21.6 G to 43.0 G on GCC, 19.2 G to 42.7 G on Clang; an includer of `core.hpp` +0.6% and +0.7%) | Before and after, pinned, three repeats | The 0019 W10 before is WU9's tip, so it is also WU9's only after |
| WU11a, `dual.hpp` | 0020 | `duals-cost/README.md`, `duals-cost/out/`; plan 0020's outcome | Before and after, pinned | None |
| WU13a, WU13b, `transform.hpp` and the rounding check | 0021 | `transform-cost/`, `transform-rounding-cost/`; plan 0021's outcome | Before and after, pinned, three repeats | None |
| Comment-only explanation on `transform` | none (merge `a5eb7f5`) | None | | Comments only; no record owed beyond the token-stream check, which this index did not look for |
| WU15, `format.hpp` and catch-alls | 0022 | `format-cost/` | Before and after, pinned | None |
| WU16, the `rga3` model | 0023 | `rga3-cost/` (`README.md`, `rerun-2026-11-08.md`) | Before and after of one includer of `core.hpp`, pinned; new units after | `include/galoot/core/` unchanged, so no core W10 was owed |
| Rotation normalizes its axis (`rga3/motions.hpp`) | 0024 | None found | | Gap. No file under `core/` changed, so no W10 was owed; the preset's own cost was not recorded |
| `presets::rga3` layout | 0025 | None found | | Gap, as 0024: preset only, core unchanged |
| WU12 (with WU5), `basis_change.hpp` | 0030 | `wu12-basis-change-cost/` (`out/`, `out-rulings/`) | Before (`1818efe`) and after, pinned, three repeats | None |
| WU14 and follow-ups (test-only) | 0029, 0031 | `wu14-identities-cost/` (`out/`, `out-later/`), `wu14-follow-ups-cost/` | Before and after of the changed test units, pinned | Test units, not core units; listed because they carry the largest sources |
| Kernel initializers | 0032 | `kernel-init-cost/` (`out/`, `out-gcc/`, `probes/`) | Before (`6b8e2b6`) and after, pinned | None |
| Safe form in `mv_test.cpp`'s reference kernels | 0033 | None | | Test-only unit; no W10 owed |
| Placement cost in the comments | 0034 | None | | Comment-only; the plan asks for one timed core test, not a W10 |
| M5 messages (`static_assert` split in headers) | 0035 | None, deliberately | | The plan states no W10 is taken because generation does not change |
| `basis_change`: `same_rows`, reduced rows | 0039 | `basis-change-rows-cost/` | Before and after, pinned, under the lock | None |
| Pre-release core pass, forced inlining | 0040 | `pre-release-core-pass/cost/` (parts A to E merged, `7a27f76`), `cost-public-ops/` (`a6f3196`), `cost-d1/` (`d2568e3`), `object-sizes/`; the opt-out's `opt-out/` (`2d292ef`) | Before and after, on debug and measure trees of both compilers | The opt-out's comparison rows come from two other trees' records, not a rerun; clang-measure was not run under the opt-out |

## Gaps in one place

1. WU9 (plan 0018): no W10 record found, and none summarized.
2. Plan 0009 (after-only, one before figure) and cleanup 1 (plan 0007, none).
3. Rotation normalization and the rga3 preset's relocation (plans 0024, 0025): no cost record; the core was unchanged.
4. Comment-only changes (`a5eb7f5`, plans 0034 and 0035) and the test-only unit (plan 0033): none owed.
5. WU17 (plan 0038) changed nothing under `include/`; its records are the E1, E2 and E3 directories (`wu17-asm/`, `wu17-bench/`, `wu17-growth/`), not W10 records. `wu17-baseline/`, which plan 0038 part H names, was not made (decision 0246 item 2).
6. DoD-22's per-translation-unit rows at the release revision are not here. The newest committed rows in `compile.tsv` before the release matrix are from `c799821` (2026-11-07) and the later gates' trees; they are copied by the release matrix, in shared mode (plan 0042 section 1, item 5).

## Draft: the row for CONVENTIONS section 8 (errata naming)

Not applied. CONVENTIONS is governance and only the owner edits it (decision 0240 item 12). The row below follows the table's columns (Topic, Disagreement, Choice, Test) and would go after the "Conformal duals" row. It records the naming disagreement of decision 0234 item 7 and research note 0027, section 1 item 6. The recommendation, accepted in decision 0240 item 12, is to add this row and leave 5.8 as it is.

```
| Motor coefficient names | The rigid wiki's Errata page, in its remark on the transform of a line, says Q_vw is the coefficient of the scalar and Q_mw that of the antiscalar. The Motor page, which 5.8 follows, puts Q_vw on 𝟙 and Q_mw on the scalar, and the rga3 preset's Motor does the same. The errata page's remark is read as a slip of naming: the constraint it states is symmetric in the two coefficients, so no count or polynomial of the transform depends on which is which. The book has not been read (V9). | 5.8 | "an rga3 Motor prints on e41, e42, e43, e1234, e23, e31, e12, 1" in `test/presets/rga3/format_test.cpp`, which pins the fourth coefficient (Q_vw) to e1234 and the eighth (Q_mw) to the scalar; the layout assertions of `test/presets/rga3/objects_test.cpp`, which reject the two orders that would swap them |
```

Two points for the owner. First, "the constraint it states is symmetric in the two coefficients" is research note 0027's reading (section 1 item 6: "the constraint h is symmetric in the two"); the note was checked independently, but the errata page is a secondary statement of a book nobody on the project has read. Second, the row cites a test by name where the table's other rows do; both tests exist at the tip this index was written from (`3a2aa32`).
