# 0226 Kernel-init rulings

Date: 2026-11-09. The owner accepted all recommendations made in the report on the kernel-init unit (plan 0032).

1. **The three IEC 559 arrays in `transform.hpp`** stay as they are. Their zero is all zero bits, so the defect's zero fill is the correct value.
2. **The reference kernels in `test/core/mv_test.cpp`** take the safe form in a small test-only change after the kernel-init unit merges.
3. **Scalar times `full<10>` failing on GCC 16.2.1** (a `std::span` bounds check in `collect_result` that is not a constant expression) gets a short investigation in its own unit.
4. **Retest `minimal.cpp`** on each new GCC release before removing the workarounds (follow-up F2 of plan 0032).
