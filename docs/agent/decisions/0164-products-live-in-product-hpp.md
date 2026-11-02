# 0164 The products and their operators live in `core/product.hpp`

- Status: accepted by the coordinator under plan 0018 (section 2 leaves the choice to the implementer and asks that it be recorded).
- Date: 2026-11-02
- Recorded by: Claude (agent, Sonnet 5.5), implementer of WU9.

## Decision

`geometric_product`, `wedge`, `operator*` and `operator^`, with their rejecting overloads, are in a new header `include/galoot/core/product.hpp`, which `core.hpp` includes. They are not added to `mv.hpp`.

## Reasons

- `mv.hpp` is already the largest header and `mv_test.cpp` the largest test unit (64.5 G instructions on GCC); a program that needs only multivectors keeps its compile cost.
- The unit's tests go in their own file, so a change to the products rebuilds only them (P8).
- The products depend on `mv.hpp` and `plan.hpp`, not the reverse, so the include graph stays acyclic.

## Consequences

- `core.hpp` costs every includer a little more; the smoke test's compile time did not change measurably (0.261 s before, 0.247 s after on gcc-debug).
- WU10 adds scaling, the involutions and the antiproducts; whether they join `product.hpp` or get their own headers is that plan's choice.
