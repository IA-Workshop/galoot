# 0035 Rulings on the WU3 metric questions

- Status: items 2 and 3 accepted; item 1 is the recommended default and awaits the owner's nod
- Date: 2026-10-31
- Decided by: the owner, answering the three questions raised by the WU3 report. Recorded by Claude (agent).

## Decisions

1. Spelling of `R`. The owner asked whether N can be derived from p, q, r with checks. It can. Recommended: `R<P, Q, R>()` with `std::size_t` parameters and N = P + Q + R, so a mismatch is impossible, a negative count is a compile error by narrowing, and the remaining checks stay (N at least 1 and at most MaxN). This is simpler than `R<N>(p, q, r)` and reads like the mathematical R(p, q, r). The sum check and the negative-count check of the explicit form disappear because they cannot occur. Status: recommended default; applies unless the owner objects.
2. Entry bound. The accepted range of a Gram entry is adaptive: the entry type is a template parameter (default `int`) and the bound is derived from it as the largest value whose square still fits, so a product of two entries never overflows for any entry type. Signed overflow in a constant evaluation is a hard compile error, so later arithmetic cannot wrap silently; the bound exists to give an early message in the algebra's terms. The coefficient type of product plans is decided in the plan-generator unit, with overflow checks at generation.
3. clang-tidy. The two bounds checks (constant array index and unchecked container access) are excluded in `.clang-tidy`, with the reason stated in the configuration, and the per-file suppressions in `metric.hpp` and its tests are removed. Other suppressions stay single-line with a reason.

## Consequences

The WU3 branch is adjusted once the independent review has returned, together with its findings, and the changed delta is re-reviewed.
