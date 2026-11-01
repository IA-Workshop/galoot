# 0101 The coefficient type, and the overflow guard with its measured cost

- Status: accepted (recorded by the author; the owner may reverse it, and plan 0011 section 13 question 3 asks the owner to confirm or reverse it now that the cost is measured)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU6a under plan 0011, on the plan's recommended answer (departure 3), which the owner's standing instruction adopts.

## Context

Decision 0035 item 2 left the coefficient type of product plans to this unit, with overflow checks at generation. Every coefficient of e_A e_B in the wedge-monomial basis is an integer polynomial in the Gram entries, of degree (|A| + |B| − |C|)/2 on e_C (note 0009, B3 and B5), so no bound on the entries makes `int` safe: with entries near `max_form_entry` = 2^15 − 1, a coefficient of degree 2 can already pass 2^31. Signed overflow in a constant evaluation is a compile error, but with the compiler's own message, and at run time it is undefined.

## Options weighed

1. `int`, as `entry_type`. Overflows at n = 4 for dense forms near the entry bound. Rejected.
2. `std::int64_t` with no guard. Overflow in constant evaluation still fails to compile, in the compiler's terms; at run time it is undefined, and a checking build would not report it.
3. `std::int64_t` with a guard on every multiplication and addition of coefficients, whose bound is derived from `std::numeric_limits<coefficient_type>::max()`. Chosen.
4. A template parameter for the coefficient type. No second type has a present user (decision 0036, STYLE S16). Rejected for now.

## Decision

- `coefficient_type` is one public alias for `std::int64_t`.
- `detail::multiply_checked(x, y)` and `detail::add_checked(x, y)` return x y and x + y when the result lies within −max to max, max the largest `coefficient_type`, and otherwise fail `GALOOT_ASSERT` with "a product coefficient does not fit coefficient_type". The product check is |y| <= max / |x| (for x ≠ 0); the sum check is x <= max − y for y > 0 and x >= −max − y otherwise, neither of which can overflow. No literal bound appears.
- The range is symmetric: the most negative `int64_t` is excluded, so a magnitude is always defined. A product equal to it is rejected (`core.blade_product_rejects_product_below` and `_below_second`).
- Every multiplication and addition of coefficients in `blade_product.hpp` goes through a guard: the scale of each recursion step, the wedge sign, each Gram entry times its contraction sign, and every sum in `term_list::add`. Gram entries are widened to `coefficient_type` before they are multiplied.
- The guard holds in constant evaluation in every build and at run time when `GALOOT_CHECKING` is 1, as `GALOOT_ASSERT` does everywhere. With checking off at run time, an overflow is undefined behavior, as for any unchecked precondition.

## Measured cost (criterion 8)

Measured by the author at `b142837` with `docs/agent/records/performance/overflow-guard/measure_guard.py` (three repeats, each compile through `tools/timed`, rows in `runs.tsv` with selections `overflow-guard:*`, summary in `out/cost.tsv`). Each translation unit constant-evaluates the full table of an n = 5 form, all 1,024 products with a term list of capacity 16, under Galoot's test flags (`-O0`, `_GLIBCXX_ASSERTIONS`, `GALOOT_CHECKING=1`) with `-fsyntax-only` and the limits raised (`-fconstexpr-ops-limit=2^40`, `-fconstexpr-steps=0`). The unguarded copy returns x + y and x y with no check. Retired instructions, both kinds of core added, mean of three; the spread of the three was under 1% on GCC and about 2% on Clang. Shared mode (the machine had a load average near 1 from other work), so wall times are indicative only.

| Compiler | Form | Guarded | Unguarded | Change | Wall, guarded / unguarded |
| --- | --- | --- | --- | --- | --- |
| GCC 16.2.1 | conformal(3), 1,024 terms | 1.673 G | 1.669 G | +0.3% | 0.23 / 0.22 s |
| GCC 16.2.1 | seeded random form 3, dense, 5,326 terms | 9.117 G | 9.113 G | +0.05% (within the spread) | 0.56 / 0.57 s |
| Clang 23.1.4 | conformal(3) | 4.025 G | 3.629 G | +10.9% | 0.34 / 0.32 s |
| Clang 23.1.4 | seeded random form 3 | 31.88 G | 28.49 G | +11.9% | 1.70 / 1.57 s |

Peak memory is the same with and without the guard (75 and 165 MiB on GCC, 97 MiB on Clang). These counts include parsing the headers, about 0.7 G on GCC and 0.55 G on Clang for `core.hpp`'s headers (decision 0090's note), so the share of the constant evaluation itself is somewhat larger on Clang, about 13% (inferred from that subtraction).

At the compilers' default limits, with the guard: the conformal(3) table compiles on both compilers at -O0 and -O2; the dense table compiles on GCC and fails Clang's step limit at -O0 and -O2, and it fails there without the guard too (rows `overflow-guard-default-limits:*`). So the guard does not move that limit for these two tables.

## Consequences

- The guard costs nothing measurable on GCC and about 11 to 12% of the instructions of a constant-evaluated n = 5 table on Clang. The owner may drop it by a later decision; without it, overflow at compile time still fails to compile, in the compiler's words, and at run time it is undefined.
- WU6b's plan and WU8's scalar conversion read `coefficient_type`. The conversion into the user's scalar is WU8's (DoD-7).
- Widening the type later is a one-line change of the alias; the guards follow from `numeric_limits`.
