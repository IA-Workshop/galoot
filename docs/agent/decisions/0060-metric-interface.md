# 0060 The metric interface

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), implementing WU3 under plan 0005.

## Context

Plan 0005 fixes the content of `include/galoot/core/metric.hpp` in outline: a symmetric integer Gram matrix in a structural value, the signature carried by a tag type (decision 0021, STYLE S7), the largest n as a template parameter with 16 as the default (decision 0004), `int` elements, factories `R(p, q, r)`, `rigid(d)`, `conformal(d)`, a concept `metric_like`, and generation-time checks with literal messages for an asymmetric form, p + q + r ≠ N, an entry out of range, and n outside the bound. It leaves open how each is spelled, what "an entry out of range" means, and where the bound lives.

## Options and decisions

1. Factory spelling. (a) `R<N>(p, q, r)`, N explicit, p, q, r checked; (b) `R<P, Q, R>()`, N computed, so p + q + r ≠ N cannot be written. Chosen: (a), because the plan's criterion 2 names the p + q + r ≠ N rejection and (a) is the reading under which it exists. The cost is a redundant N at the call site and two run-of-the-mill checks (negative counts, wrong sum). `rigid<D>()` and `conformal<D>()` take only D, since their n is fixed by D. Option (b) remains a simplification the owner may prefer; it would remove two checks and two compile-fail tests.
2. The bound. A named constant `max_generators = 16` is the default of a `MaxN` template parameter on every factory and on `metric_like<Alg, MaxN>`. The bound is not part of `metric<N>`'s type, so raising it changes no type. n = 0 and n > MaxN fail a `static_assert` in `detail::check_generator_count`, whose message is literal.
3. Entry range. CONVENTIONS sets no range. Chosen: |g_ij| ≤ `max_form_entry` = 32767 = 2¹⁵ − 1, so that a product of two entries and a 2 × 2 Gram minor g_ii g_jj − g_ij² fit in a 32-bit `int` (2 · 32767² < 2³¹ − 1). Larger minors can still overflow at n ≥ 3, and a constant evaluation then fails on the signed overflow, so this bound is not a guarantee for the product tables; WU6 decides the coefficient type (note 0009, open question 4). Alternatives weighed: no bound (the plan asks for one); [−1, 1] (excludes the scaled Witt and random forms of plan 0001 section 5, whose entries reach ±3); the `int8_t` range of the design study (no arithmetic reason).
4. General forms. `from_gram({{...}, ...})` builds a metric from a braced matrix and checks symmetry and range. Its parameter is a reference to a C array so that N is deduced from the braces; `std::array` cannot be deduced from a nested braced list. The C array is suppressed at its location with that reason.
5. The concept. `metric_like<Alg, MaxN>` is satisfied by a tag type whose static `value` is a `metric<N>` with 1 ≤ N ≤ MaxN, symmetric, with entries in range. It tests the data only (decision 0033). It re-checks what the factories check, so a hand-written aggregate cannot be used unchecked (V4).
6. Accessors. `gram` is public, as a structural type requires. Members: `generator_count`, `is_symmetric()`, `entries_in_range()`, `is_diagonal()`, `is_null(i)`, and a defaulted `==`. Indices count from 0, so index i is e_(i+1), the generator of mask bit i (CONVENTIONS 1.1). The null-generator "position" of plan 0005 criterion 4 is read as `is_null(i)` per generator rather than a mask, because a mask would need `mask_t` from `bits.hpp` (WU2, in flight) and conformal(3) has two null generators while being non-degenerate.
7. Catch-all overloads. No function in this unit takes a signature tag or a typed operand, so the catch-all form of decision 0033 has no place here. Every check fires inside a `consteval` factory, through `GALOOT_ASSERT` (message on the echoed source line) or `static_assert`.
8. Element type `int`, as the plan says. Note 0010 does not discuss `int8_t`; the reading followed is the plan's statement as given.

## Consequences

- Later units constrain their algebra parameter with `metric_like` and read `Alg::value`.
- The tag-and-value measurement (`docs/agent/records/performance/tag-value/`) shows the per-signature cost of `metric_like` at n = 16 is about 20M instructions on both compilers, evaluated once per tag. If that matters later, the concept could trust the factories and check only the type, at the price of letting a hand-written invalid aggregate through.
- Two clang-tidy checks, `cppcoreguidelines-pro-bounds-constant-array-index` and `cppcoreguidelines-pro-bounds-avoid-unchecked-container-access`, flag every index by generator. They are suppressed in one block per file, with the reason. Every later core header that indexes by generator will meet them; whether to exclude them in `.clang-tidy` is for the owner (STYLE S1, S4).
