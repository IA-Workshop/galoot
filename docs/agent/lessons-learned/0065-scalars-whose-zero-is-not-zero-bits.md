# 0065 Test value types over a scalar whose zero is not all zero bits

- Date: 2026-11-01
- Unit: WU8a (plan 0016), Claude (agent, Opus 5.5), cpp-expert role

## What happened

The multivector stored its coefficients as `std::array<T, K> coefficients{};`. Its zero-by-default tests passed on both compilers over all five scalars. When the named-access tests were added, which write into a zero value over the test rational inside constant evaluations, the zero tests of the same program started to fail on GCC 16.2.1 at run time, and only for the rational: `Line<rational>{}` held 0/1 in its first places and 0/0 from the place a constant evaluation had written on. Clang 23.1.4 was right throughout.

## Cause

A GCC wrong-code defect, outside Galoot: a run-time value-initialization of an array of a class type with default member initializers, written `{}`, shares one initializer with constant evaluations in the same translation unit, and a constant evaluation that writes into one element changes it (`docs/agent/records/spikes/gcc-shared-array-initializer/`, a reproducer with no Galoot code). For `double`, `float`, `std::int64_t`, and `std::complex<double>` the damage is invisible: their zero is all zero bits, which is what the damaged initializer leaves. The rational's zero, 0/1, has a nonzero denominator, so it showed.

The library writes the initializer out once per place, `coefficients{zero_at(I)...}`, which GCC compiles correctly, and the comment beside it says why. No upstream report was made (agents do not contact maintainers); the reproducer is kept for one.

## What detects it now

`core.mv`'s test "mv stays zero by default after a constant evaluation wrote into one" writes into a zero plane over the rational in a constant evaluation and checks a run-time zero plane, so the defect fails a test on GCC if the shorter initializer returns. The mutant `shared_zero_initializer` of the WU8a mutation records is killed by it on gcc-debug and is equivalent on clang-debug.

The general rule: a test of a value type covers at least one scalar whose zero is not all zero bits, and exercises it both in constant evaluation and at run time in one translation unit, because a compiler defect that zero-fills memory is invisible over the arithmetic types.

## Correction, 2026-11-01 (after the W5 review)

The description above is broader than what was observed. As measured by the blind reviewer and again on WU8a's plane (probes `review-f7:p9:*` in the run log), on GCC 16.2.1:

- The trigger was an array that is a class member with a default member initializer, `std::array<q, 6> c{}` or `= {}` (or a C array `q c[6]{}`). A local `std::array<q, 6> a{}` was not affected, nor was a second class with the same element type.
- Writes at places 0 and 1 in the constant evaluation did no damage; a write at place 2 or later damaged the later run-time zero from that place on.
- Default-initialization of the class (`box b;`) was damaged as well as value-initialization (`box b{}`).
- That GCC shares one initializer among the places is a guess at the mechanism, not something observed.

The regression test now writes into every place of the plane, so it does not depend on which places trigger the defect.

## Addendum, 2026-11-02

The defect is recorded in the knowledge base's `upstream-awareness.md` and is not reported upstream (decision 0162, item 11). Lesson 0066 records the second GCC defect found in WU8.
