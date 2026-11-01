# 0151 Scalar concepts, exactness, and the exactness of plan coefficients in a scalar

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended")
- Date: 2026-11-01
- Decided by: the owner, on plan 0016 questions 6 and 8 and its readings 14 and 15; recorded by Claude (agent, Opus 5.5)

## Context

A plan coefficient is an exact `std::int64_t` (decision 0101). Converted to a scalar it may round: `float` holds integers exactly only up to 2^24 in magnitude, `double` up to 2^53, `short` up to 32767. Such coefficients exist within the metric's entry bound: e12 e12 = −(2^24 + 1) in diag(24929, 673), and e1234 e1234 = 32767^4, odd and above 2^53, in diag(32767, 32767, 32767, 32767). A rounded coefficient would be an implicit lossy conversion of a structure constant (C7, C8). Checking mode (decision 0010) needs to know whether a scalar is exact.

## Options weighed

For the coefficient check: (a) a compile-time check from `std::numeric_limits<T>` where it is specialized, and a documented requirement otherwise (chosen); (b) a Galoot trait that `std::complex` and user types could specialize, one more published interface (C5, C12); (c) no check.

For exactness: (a) `std::numeric_limits<T>::is_exact` (chosen); (b) a Galoot trait.

## Decision

1. `ring_scalar<T>` requires: `std::semiregular<T>`; `a + b`, `a - b`, `a * b`, and `-a` convertible to T; T constructible from `coefficient_type`, listed last so that a type lacking arithmetic fails on a short requirement first. `T{}` is the zero of T and the operations form a ring: documented requirements. Equality is not required.
2. `exact_scalar<T>` is `ring_scalar<T>` with `std::numeric_limits<T>::is_exact`: true for the integer types, false for floating types and for any type without a specialization, such as `std::complex<double>`. A user's exact type without a specialization is treated as rounding.
3. Applying a plan to a scalar T checks at compile time, when `std::numeric_limits<T>` is specialized, that every coefficient of the plan is exact in T: |c| at most `max()` for an integer type, |c| at most 2^digits for a floating type of radix 2. For other scalars, exactness of the plan's coefficients is a documented requirement of the scalar, not checked; the comment says that `std::complex<float>` is such a case.
4. The test-only rational of decision 0011 lives under `test/support/`, outside the core (DoD-5), and specializes `std::numeric_limits` with `is_exact` true, which the standard allows for a program-defined type. It is separate from WU5's core rational for the change of basis (M5).

## Consequences

- Checking mode in `truncate` uses `exact_scalar` (decision 0155).
- Plan 0016 criterion 14 tests each bound on both sides.
- How the class template reports a scalar that fails `ring_scalar` is decision 0152.
