# 0113 The limits of typed product plans, and their P2 record

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7a of plan 0013 (criteria 8 and 10) in the role of cpp-expert.

## Context

Criterion 8 asks for the default constant-evaluation limits of `product_plan` for both operations in the three configurations of decision 0103 (Galoot's flags at -O0 and -O2, and a bare `-std=c++23 -pedantic-errors -O2`), stated in its `///` comment, with any drop below decision 0103's ceilings reported with its cause. Criterion 10 asks for the P2 record of decision 0103's fixed use through `product_plan<gp>`, before and after, without the concept `static_assert` (decision 0107).

## Measurements

By `docs/agent/records/performance/plan-results/measure_results.py`, every compile through `tools/timed measure` pinned to CPU 10 (a performance core), rows in `runs.tsv` with selections `results-limits:*` and `results-growth:*`. GCC 16.2.1, Clang 23.1.4, libstdc++ 16. Instructions are those retired on the pinned core. The machine also ran an editor's language server at one core's load (other CPUs) during the growth run; in the growth rows the sibling thread of CPU 10 was busy at most 0.01 s and other processes ran on CPU 10 itself at most 0.02 s per compile (`/proc/stat`, 10 ms resolution).

### Limits (criterion 8), headers of `11ad0aa`, `out/limits.tsv`

| Unit | Clang 23, all three configurations | GCC 16, all three |
| --- | --- | --- |
| Typed rigid(3) pairs, both operations (the four of decision 0103 and line × line; for the wedge, line ∧ plane asserted empty and point ∧ line added) | builds | builds |
| conformal(3) vectors × vectors, both operations | builds | builds |
| Sparse {1, e1, e_n, e1n} × {e1, e2, e_n} in R(n−1,0,1), n = 8, 12, 16, both operations | builds | builds |
| `full<n>` × `full<n>`, geometric product, R(n,0,0), R(n−1,0,1), conformal(n−2) | n ≤ 5; n = 6 stops at the step limit in the count | n ≤ 6; n = 7 exceeds the operation limit (2^25) in the typing |
| `full<n>` × `full<n>`, geometric product, dense form (2 on the diagonal, 1 elsewhere) | n ≤ 4 | n ≤ 5; n = 6 exceeds it in `blade_product` |
| `full<n>` × `full<n>`, wedge, every form | n ≤ 5; n = 6 stops in `collect_result` | n ≤ 6; n = 7 exceeds it in `type_terms` |

The three configurations agree at every point; the bare -O2 configuration has no `_GLIBCXX_ASSERTIONS` (libstdc++ defines it only without optimization), so the comment's "with or without" is measured.

Against decision 0103 (unchanged in 0106): Clang's ceilings are unchanged (5, dense 4); GCC's dense ceiling is unchanged (5); GCC's ceiling for the product in the diagonal, rigid, and conformal forms falls from 7 to 6. Cause, from the diagnostics: at n = 7 the evaluation that stops is the typing, not the generation. For R(7,0,0) and conformal(5) GCC reports the operation limit at `plan.hpp:204`, the comparison loop of `collect_result`'s insertion into the canonical list; for R(6,0,1) at the same loop through libstdc++'s checked `span` access with the assertions on, and at `:204` with them off. The plan has T = 16,384 terms (diagonal) on R = 128 result blades; the direct typing costs O(T × R) comparisons, about 2 × 10^6 for the insertion and as many for the positions, each several operations, in one evaluation under GCC's limit of 2^25 operations. A `full<7>` product at compile time was not a supported use before either: the typed and sparse pairs, which are what programs write, are far below every ceiling. The cheaper forms (binary search for positions, or merging the per-pair runs) remain a measured follow-up (plan 0013 section 13), not built.

### P2, the fixed use as n grows (criterion 10), `out/growth-d6ea717/`

Four typed rigid(3) pairs (point ⟑ point, line ⟑ plane, motor ⟑ point, point ⟑ motor) in R(n−1,0,1), e4 as e_n; Galoot's flags at -O0, `-c`; every term read at run time; three repeats, interleaved; no unit asserts `metric_like`. "Before": headers of `0bbb968` (the claim revision) through `gp_plan`; "after": headers of `d6ea717` through `product_plan<BinaryOperation::gp>`. Plans = unit minus its baseline (same algebra and sets, no plan); spreads at most 3.8 M.

| | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| n = 4 | 182.2 M | 301.3 M | 343.8 M | 537.8 M |
| n = 8 | 185.9 M (+2.1%) | 304.4 M (+1.0%) | 348.2 M (+1.3%) | 541.1 M (+0.6%) |
| n = 12 | 192.1 M (+5.4%) | 311.3 M (+3.3%) | 355.2 M (+3.3%) | 551.6 M (+2.6%) |
| n = 16 | 200.2 M (+9.9%) | 318.6 M (+5.7%) | 364.9 M (+6.1%) | 561.4 M (+4.4%) |
| Baseline (no plan) | 803.4 → 804.4 M | 887.1 → 887.9 M | 744.5 → 745.4 M | 906.4 → 906.2 M |

- Growth with n at fixed use: about 18 M on GCC and 23 M on Clang from n = 4 to 16, before and after alike. It is the one evaluation of `metric_like` per algebra and unit that decision 0107 identified (the before figures reproduce its +9.9% on GCC and +6.1% on Clang); typing adds no growth with n. The percentages fall only because the plans cost more. The P2 question of decision 0107 item 4 stays the owner's.
- Typing at fixed use: +119 M on GCC (+59% to +65% of the four plans) and +195 M on Clang (+54% to +56%), the same at every n: the cost of the scratch result, the typed plan, the five checks of the typing, and the instantiations of the new function templates, for 86 terms on 7 or 8 result blades per plan.
- Baseline: a unit that includes `plan.hpp` and uses no plan costs +84 M on GCC (+10%) and +161 M on Clang (+22%) more after than before. A Clang `-ftime-trace` of a unit that only includes `plan.hpp` (selections `hdr-probe:clang:*` in `runs.tsv`; trace not committed, so not reproducible as recorded) attributes most of the rise to class instantiations and constraint checks of `std::span` over the new element types (`std::span<typed_term>`, `std::span<oriented_blade>`) taken by the non-template functions `collect_result` and `type_terms`: total `InstantiateClass` 25.7 ms before and 41.6 ms after, `CheckConstraintSatisfaction` 24.2 ms and 40.0 ms. Inferred, not measured: the rest is parsing the added declarations. This is the same mechanism as WU6b's header cost (plan 0011, WU6b departure 9); W10 records it per unit.

## Decision

1. The `///` comment of `product_plan` states the limits above (commit `d6ea717`), with the compiler versions measured.
2. The compile-time checks of the tests stay at `full<4>` (and `full<3>` for the degenerate form) and the typed and sparse pairs, below every ceiling; the exhaustive checks run at run time (decision 0103 item 2).
3. No preset raises a limit, and no cheaper typing is built now (C3, C12): the drop of GCC's `full<7>` product ceiling and the header cost are reported (C14), with no bound set (decision 0005).

## Consequences

- If a use of a large full set at compile time becomes a requirement, the typing's two linear scans are what to replace, and their measurement is above.
- If the header cost of `plan.hpp` to units that include `core.hpp` and use no plan matters, taking the spans by template parameter or by pointer and size are the candidates; WU6b found that making two such functions templates did not change it, so it would need its own measurement.
