# 0061 Metric interface after the review

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), applying decisions 0035 and 0036 and the W5 review of WU3 (`docs/agent/records/reviews/2026-10-31-wu3-metric.md`). Supersedes decision 0060, items 1 to 3, and item 5's description of the bound.

## Context

Decisions 0035 and 0036 replace `R<N>(p, q, r)` with `R<P, Q, R>()`, derive the entry bound from `numeric_limits`, and let `metric_like` accept any n up to the width of `mask_t`. They leave some spellings to the author.

## Decisions

1. The third count of `R` is named `Z`, for the generators that square to zero. A template parameter named `R` on a function template named `R` is ill-formed (review N5). Its `///` comment says why.
2. The width bound lives in the type. `metric<N>` has a `static_assert(N <= numeric_limits<mask_t>::digits)`, so no metric wider than a mask can exist, whether built by a factory or by hand. The factories also check `MaxN <= digits` and then 1 ≤ n ≤ MaxN; R checks each count against MaxN before the sum, and `conformal<D>` checks D before D + 2, so a count near `SIZE_MAX` cannot wrap into range. `metric_like` then needs only n ≥ 1.
3. `mask_t` is declared in `metric.hpp` as `std::uint32_t`, identical to its declaration in `bits.hpp` (WU2, not yet on `develop`). An alias may be redeclared to the same type in one scope, so the two headers can be included together. Whichever unit merges second makes `metric.hpp` include `bits.hpp` and drops this declaration; including `bits.hpp` now would make WU3 rely on WU2's unfinished work (W7).
4. `metric_like` guards the reads of `Alg::value` with `requires { typename std::bool_constant<Alg::value.is_symmetric()>; }`, so a value that is not usable in a constant expression (a non-constexpr static, a non-static member) makes the concept false on both compilers (review N1).
5. `isqrt` is a `consteval` bisection in `detail`, on `entry_type` only (no template), so it stays cheap if `entry_type` is widened.
6. `from_gram<N, MaxN>`: N first and deduced (review N9). A non-square or ragged matrix does not match the parameter and gives the compiler's own "no matching function" text; the `///` comment says the matrix must be square (review N13). A catch-all was not added: no other function in the core has one yet, and the comment states the rule.

## Consequences

- Later templates write `template <metric_like Alg>` with no bound argument.
- Review N11 (the `metric_like` cost at three sizes) is not done in this pass; it belongs with the growth measurements of WU17.
