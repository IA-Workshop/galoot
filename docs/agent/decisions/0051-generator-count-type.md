# 0051 The type of a generator count, and of a grade

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner ("the generator count is std::size_t everywhere, matching metric"), answering finding N2 of the W5 review of WU2. The grade type is the author's choice under that ruling. Recorded by Claude (agent, Opus 5.5).
- Supersedes: decision 0050 only in the type of n; its range and message rulings stand.

## Context

Decision 0050 gave the bits functions an `int n`. The metric (WU3, decision 0035) counts generators with `std::size_t`, so under `-Wconversion -Werror` every call from the metric or a later unit into bits would need a cast.

## Options for the grade type

1. `std::size_t`, like the count. Then (-1)^(k(k-1)/2) computes k - 1 in unsigned arithmetic, which wraps at k = 0 (harmless, since it is multiplied by 0, but a reader must check that), and a difference of grades can wrap.
2. `int`, the type `std::popcount` returns. Sign formulas and differences stay signed; `antigrade` converts n once, after its range check makes the conversion exact.

## Decision

Every generator count in bits is `std::size_t`: the n parameters and `mask_bits`. Masks stay `mask_t`. Grades and antigrades are `int` (option 2). The tests take their bounds from `std::numeric_limits<mask_t>::digits` rather than literals (decision 0035), and the abort program uses `mask_bits + 1`, `mask_bits`, and `~mask_t{0}`.

## Consequences

A caller that compares a grade with a count converts one of them; WU4 (blade sets) meets this first and may revisit the grade type. A negative count cannot reach bits; a count converted from a negative `int` arrives as a huge `std::size_t` and is rejected by the upper bound.
