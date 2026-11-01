# 0106 The constant-evaluation limits after the set-bit loops

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing plan 0012 (criterion 4).

## Context

Plan 0012 asks that the `full<n>` ceilings (the `///` comment of `full` in `blade_set.hpp`, decision 0090) and the plan limits of decision 0103 (the `///` comment of `gp_plan` in `plan.hpp`) be rechecked after the loops of decision 0104 change, and that the header statements be updated if they moved.

## Measurement

`docs/agent/records/performance/product-cost-growth/limits_recheck.py`, headers at `9ff402c` (code of `77ed567`), output `out/limits-77ed567/limits.tsv`. Each case starts one below the last n recorded to build and rises until the first failure, `-fsyntax-only`, in four configurations: Galoot's test flags at -O0 with checking and at -O2 without (both with `_GLIBCXX_ASSERTIONS`), bare `-std=c++23 -pedantic-errors` (libstdc++ enables its assertions without `-O`), and bare with `-O2` (assertions off).

| Unit | Clang 23.1.4 | GCC 16.2.1 |
| --- | --- | --- |
| `full<n>` | builds 13, stops at 14 in three configurations; bare -O2 stops at 13 | builds 15, stops at 16 in all four |
| `full<n>` × `full<n>` in R(n,0,0), R(n−1,0,1), conformal(n−2) | builds 5, stops at 6 in all four | builds 7, stops at 8 in all four |
| `full<n>` × `full<n>`, every entry nonzero | builds 4, stops at 5 in all four | builds 5, stops at 6 in all four |

Every plan boundary is where decision 0103 and the review's bare -O2 compiles put it, so the loops did not move a limit; the step and operation counts they save are too few at these n to admit the next n. Two observations:

- GCC's conformal n = 8 failure is now reported at libstdc++'s subscript check (`array:219`, "`__builtin_expect(...)` is not a constant expression") inside `add_generator_product`, where before it was "operation count exceeds limit" at `blade_product.hpp:241`. The index read there is below the list's size by the loop condition, and with `-fconstexpr-ops-limit=2^30` the unit builds (`out/limits-77ed567/probes.txt`), so this is the operation limit running out at a different point and reported in other words; the run-time comparison of decision 0104 also covered every pair of conformal(6) blades with the assertions on.
- Clang at bare -O2 stops at `full<13>`, with the headers of `0f1039a` as well as the new ones (`probes.txt`). `make_full` uses nothing that this unit changed. The comment of `full`, "Clang 23 builds full<13> but not full<14>", is right in the three configurations with libstdc++'s assertions and not in an optimized build without them. Reported to the owner as a defect found outside this unit (W9); not fixed here, because `blade_set.hpp` is outside plan 0012's scope.

## Decision

No header statement of a limit changes: the `gp_plan` comment of `plan.hpp` and the `full` comment of `blade_set.hpp` are left as they are, the second with the defect above for the owner.
