# 0201 The tolerance of the geometric test of `rotation`: k = 9 ulps for sin and cos

- Status: recorded. The derivation and the recommendation of k = 9 are the numerical expert's; the choice to commit the test with it is the test author's and the implementer's, open to the blind review and the owner (C18).
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU16
- Bears on: plan 0023 R11 ("if no such source can be read, the test is not written"), criterion 5; VVUQ V9, V12; decision 0180 (transform's forward bound, Lemma A)

## Context

R11 asks for a geometric test that `rotation` turns by θ at double, with a tolerance that includes the C library's error for sin and cos, derived from `transform`'s forward bound and the library's published accuracy read under V9, before the test runs (V12). The derivation is `docs/agent/records/numerical/rotation-tolerance.md` on `records/rotation-tolerance` (`3acd5d0`, branched from `develop` at `88c9484`, not yet merged), written before any geometric test of `rotation` existed.

## Decision

1. **The source and k.** The GNU C Library Reference Manual for 2.44 (the libm the tests link, read from the installed info files) states no per-function bound; it says its testsuite "only flags results larger than 9ulp ... as errors". The last per-function figure, `libm-test-ulps` at `glibc-2.41`, gives 1 ulp for `sin` and `cos` in double and float, a sampled figure of an older release. Neither is a proved bound. The test uses k = 9, the only figure stated for the library in use.
2. **The test.** A quarter turn of (1, 0, 0, 1) about the z axis by `rotation(l, π/2)` over float and double is compared with (0, 1, 0, 1), coefficient by coefficient, within 39u, 38u, 0, and 31u on e1, e2, e3, e4, u the unit roundoff of T: γ_K times each coefficient's magnitude (K = 12, 12, 12, 5, from `tracked`), plus the error of the motor's sine and cosine at k ulps with the norm defect it causes, plus the gap between π/2 and the T that holds it, rounded up to integers. The comparisons add no rounding. The values and the formula are in the test's comment, as derived, and were fixed before the test first ran (`b711850`, cherry-picked as `79abffd`).
3. **What it rejects.** The mutants of plan 0023 section 6 (θ for θ/2, sine and cosine swapped, −θ) err by order 1 and fail it; the bit-for-bit test of `rotation` (R11) kills them as well.

## Options weighed

- k = 1 (tolerances 16u, 15u, 0, 8u): tighter, but resting on the testsuite file of an older release, which states the largest error it observed, not a bound.
- k = 1/2, correct rounding: holds only where GCC folds `std::sin` of a constant through MPFR, not for a call at run time.
- No geometric test (R11's fallback): leaves "turns by θ" at double pinned only through rational half angles, which never call `std::sin`.

## Consequences

- The test checks the construction's accuracy at double and float for one case only, on glibc 2.44 on x86-64. Another C library, another architecture, other axes, lines off the origin, and other angles are not covered (the derivation's "Not covered"); a library worse than 9 ulps would fail the test, which is then a finding about the library, not a reason to widen the tolerance silently (W9).
- The derivation's record is on `records/rotation-tolerance`, which is to be merged into `develop` so that this decision's source is there.
