# 0035 Rulings on the WU3 metric questions

- Status: items 2 and 3 accepted; item 1 is the recommended default and awaits the owner's nod
- Date: 2026-10-31
- Decided by: the owner, answering the three questions raised by the WU3 report. Recorded by Claude (agent).

## Decisions

1. Spelling of `R`. The owner asked whether N can be derived from p, q, r with checks. It can. Recommended: `R<P, Q, R>()` with `std::size_t` parameters and N = P + Q + R, so a mismatch is impossible, a negative count is a compile error by narrowing, and the remaining checks stay (N at least 1 and at most MaxN). This is simpler than `R<N>(p, q, r)` and reads like the mathematical R(p, q, r). The sum check and the negative-count check of the explicit form disappear because they cannot occur. Status: recommended default; applies unless the owner objects.
2. Entry bound. No limit is written as a literal. The entry type is a template parameter (a signed integral type, default `int`), and the accepted range is computed from `std::numeric_limits<Entry>::max()`: the largest k whose 2x2 Gram minor, 2k^2, still fits, which is `isqrt(max / 2)` by a small `constexpr` integer square root. For `int` this gives 32767, the value the author had hardwired, now derived; a wider entry type widens the range with no code change. Signed overflow in a constant evaluation is a hard compile error, so later arithmetic cannot wrap silently; the bound exists to give an early message in the algebra's terms. The coefficient type of product plans is decided in the plan-generator unit, with overflow checks at generation. The largest number of generators is likewise bounded by the width of the mask type (`std::numeric_limits<mask_t>::digits`) with the v1 default of 16 kept as a named default, which is a policy and not derivable. Tests check the bounds against `numeric_limits`, not against literals.
3. clang-tidy. The two bounds checks (constant array index and unchecked container access) are excluded in `.clang-tidy`, with the reason stated in the configuration, and the per-file suppressions in `metric.hpp` and its tests are removed. Other suppressions stay single-line with a reason.

## Consequences

The WU3 branch is adjusted once the independent review has returned, together with its findings, and the changed delta is re-reviewed.
