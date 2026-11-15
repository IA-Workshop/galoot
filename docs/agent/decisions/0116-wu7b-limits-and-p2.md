# 0116 The limits of the antiproduct plans and the unary maps, and their P2 record

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7b of plan 0013 (criterion 19) in the role of cpp-expert.

## Context

Criterion 19 asks for criteria 8 to 11 again for `unary_map` and the conjugated plans: the default limits in the three configurations of decision 0103 on both compilers, stated in the `///` comments; the P2 record of a fixed use over n = 4, 8, 12, 16; and before-and-after costs.

## Measurements

By `docs/agent/records/performance/plan-maps/measure_maps.py`, every compile through `tools/timed measure` pinned to CPU 10, rows in `runs.tsv` with selections `maps-limits:*` and `maps-growth:*`. GCC 16.2.1, Clang 23.1.4, libstdc++ 16. Instructions are those retired on the pinned core. The first limits run (script at `bc510e3`) was stopped when its sparse antiwedge units failed their own assertion (the plan of the low-grade sparse sets is empty; fixed at `f83aa3c`) and is not used. The `math-tools` timing lock was free during the limits and growth runs of this script; it was held by another workstream for three minutes during the mutation runs, which are not timing runs, and during the rerun of the product limits below.

### Limits (headers of `f83aa3c`, `out/limits.tsv`)

| Unit | Clang 23 | GCC 16 |
| --- | --- | --- |
| Typed rigid(3) pairs (point ⟇ point, plane ⟇ plane, motor ⟇ point, motor ⟇ motor; plane ∨ plane, line ∨ plane, plane ∨ line), the conformal(3) vectors, the sparse sets {1, e1, e_n, e1n} × {e1, e2, e_n} of R(n−1,0,1) and their right complements at n = 8, 12, 16, both antiproducts | builds in all three configurations | builds in all three |
| `full<n>` × `full<n>`, geometric antiproduct, R(n,0,0) and R(n−1,0,1) | n ≤ 5 | n ≤ 6 |
| the same, conformal(n−2) | n ≤ 5 with Galoot's flags, n ≤ 4 at bare -O2 | n ≤ 6 |
| the same, dense form (2 on the diagonal, 1 elsewhere) | n ≤ 4 | n ≤ 5 |
| `full<n>` × `full<n>`, antiwedge, every form | n ≤ 5 | n ≤ 7 |
| Maps of the four rigid(3) sets, every operation | builds | builds |
| `unary_map` of `full<n>`, R(n,0,0), reverse, antireverse, grade involution | n ≤ 7 | n ≤ 9 |
| the same, right and left complement | n ≤ 8 with Galoot's flags, n ≤ 7 at bare -O2 | n ≤ 9 |

Where each stops: on Clang, the step limit in the count of the plan (`plan.hpp`, the `count` local) or in the typing of the map (`found`, `mapped`); on GCC, the operation limit of 2^25 in the typing's scans, in `blade_product` for the dense form, and in libstdc++'s `span` access.

The configurations disagree on Clang in two places, unlike the typed products of decision 0113. The unit that fails at bare -O2 (`unary_map` of the right complement of `full<8>`) builds with the same flags plus `-D_GLIBCXX_ASSERTIONS`, and fails with Galoot's flags minus it: so the difference is libstdc++'s configuration, not the optimization level. Clang's note puts the step limit inside `std::span::operator[]`'s call of `size()`. Inferred, not measured: without `_GLIBCXX_ASSERTIONS`, libstdc++ 16 still checks a subscript in a constant evaluation, through a path that costs Clang more steps per access than the asserting form. The comments state the ceilings with this condition.

Product ceilings after the generator's restructuring: the typed-plan script of decision 0113 (`plan-results/measure_results.py limits`), rerun on the headers of `f83aa3c` (`out/products-limits-f83aa3c/limits.tsv`), gives every ceiling of the geometric product and the wedge exactly as decision 0113 states them, in every form and configuration, and every fixed unit builds. That run started while the `math-tools` timing lock was held by another workstream, whose holder had released it before it could be read; its results are exit statuses and retired instructions, which load does not change, but it may have disturbed that workstream's timing.

### P2 (fixed uses in R(n−1,0,1), `out/growth-f83aa3c/`)

Galoot's flags at -O0, `-c`, three repeats interleaved, every term read at run time; each use is its unit minus the baseline of its headers at the same n; spreads at most 20 M. "Before" is the headers of `7704146` (the claim revision), "after" those of `f83aa3c`.

| Use | Compiler | n = 4 | n = 8 | n = 12 | n = 16 |
| --- | --- | --- | --- | --- | --- |
| Four typed pairs, geometric product, before | GCC | 300.9 M | 304.4 M | 310.8 M | 318.9 M |
| the same, after | GCC | 306.2 M | 308.9 M | 315.2 M | 323.6 M |
| the same, before | Clang | 537.9 M | 542.9 M | 549.2 M | 561.1 M |
| the same, after | Clang | 547.6 M | 553.5 M | 560.6 M | 571.7 M |
| Four typed pairs, geometric antiproduct | GCC | 308.8 M | 477.5 M | 754.3 M | 1137.2 M |
| the same | Clang | 702.5 M | 1380.4 M | 2463.0 M | 3955.9 M |
| Right complement and reverse of the four rigid(3) sets | GCC | 154.7 M | 159.0 M | 164.8 M | 172.6 M |
| the same | Clang | 263.3 M | 268.3 M | 274.9 M | 285.4 M |
| Baseline (no plan), before → after | GCC | 888.3 → 898.0 M | | | 889.2 → 898.7 M |
| the same | Clang | 906.3 → 918.5 M | | | 906.9 → 918.5 M |

- The geometric product: the same growth with n before and after (about 18 M on GCC and 23 M on Clang from n = 4 to 16, the per-algebra `metric_like` check of decision 0107), and +5 M (GCC) and +10 M (Clang) at every n after, about 1.7%, from the generator's helpers `factor_of` and `run_term`, instantiated for every plan. The baseline rises by +10 M and +12 M: the declarations added to `plan.hpp`.
- The maps: their growth with n is the same 18 M and 22 M, the `metric_like` check; the maps themselves add none.
- The geometric antiproduct: +268% on GCC and +463% on Clang from n = 4 to 16 at fixed use. Cause: the right complement of a blade of grade k has grade n − k, so the four rigid(3) pairs, of grades 0 to 4, are generated as products of blades of grade n − 4 to n, whose `blade_product` recursion runs over the grade and whose term lists have the capacity of a pair holding up to n generators. The result of each pair is as small as before (the antiproduct of a point and a point has seven terms at every n). So the cost follows the complements' grades, not the use. P2 records growth with n at fixed use as a defect against C6 unless it follows from the algebra's own structure; whether the antiproduct's definition through complements counts as that structure is for the owner. It is reported here and in the plan's outcome, and blocks nothing (decision 0005).

## Decision

1. The `///` comments of `product_plan` and `unary_map` state the ceilings above, with the libstdc++ condition, and that the antiproducts of the sparse sets and of their complements build at n = 16.
2. No cheaper form is built (C3, C12). The candidates, each a measured follow-up: an antiproduct pair rule that multiplies the original blades with the antimetric, which would make the cost follow the inputs' grades but adds per-basis data that C4 would need to admit; and a term-list capacity tighter than product_capacity for a diagonal form.
3. The compile-time tests stay at `full<4>` and `full<5>` and the typed pairs; the exhaustive checks run at run time (decision 0103 item 2).

## Consequences

- A program that uses antiproducts of low-grade objects in a large algebra pays for products of their complements. At n = 4, the size of rigid(3), the four typed pairs cost 308.8 M through the antiproduct against 306.2 M through the product on GCC, and 702.5 M against 547.6 M (+28%) on Clang.
- If the owner rules the growth a defect to fix, the antimetric pair rule is the first candidate, with its own decision under C4.

## Correction, 2026-11-01 (W5 review of WU7b, finding F1)

The review (`records/reviews/2026-11-01-wu7b-plan-maps.md`, head `50c7c70`) corrects two statements above; the text above is left as recorded.

1. The cause. "The cause" in the P2 section names two parts: `blade_product`'s recursion over the grade of the complements, and the term-list capacity of a pair that holds up to n generators. The reviewer compiled a copy of the headers in which the antiproduct's capacity is 1, which is valid in a diagonal form, where every partial product is one term, so the copy generates the same plans. At n = 4, 8, 16 the cost of the four pairs changed by about 1% on both compilers (GCC 1121.1 M against 1129.7 M at n = 16, Clang 3925.4 M against 3941.7 M). So the capacity is not a cause. The growth comes from the route the generator takes: the product of the complements, of grade n − k, through a recursion whose work grows with that grade.
2. The classification. The open question "whether the antiproduct's definition through complements counts as that structure" has an answer under P2's own wording. The form is diagonal, as sparse as signature data can be, and each pair's result stays one term, with fixed result sets, at every n. In such a form e_A ⟇ e_B is one term fixed by the two masks and the antimetric. So the growth follows the route, not the algebra's structure: it is growth with the dimension at fixed use, contrary to C6, and it is recorded as a defect found (P2; decision 0005: reported, and it does not block progress).
3. The remedy. The remedy is a pair rule whose cost follows the inputs' grades, such as one built from the antimetric. Its first candidate in decision item 2 stands; it is new per-basis data that C4 must admit, so it waits for the owner's decision under C4 and is not built here. The second candidate of item 2, a tighter term-list capacity, is withdrawn by item 1. The mathematics of the remedy is being worked out on another branch, `research/antiproduct-growth`.
