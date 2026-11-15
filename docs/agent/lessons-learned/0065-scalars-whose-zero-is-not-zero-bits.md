# 0065 Test value types over a scalar whose zero is not all zero bits

- Date: 2026-11-01
- Unit: WU8a (plan 0016), Claude (agent, Opus 5.5), cpp-expert role

## What happened

The multivector stored its coefficients as `std::array<T, K> coefficients{};`. Its zero-by-default tests passed on both compilers over all five scalars. When the named-access tests were added, which write into a zero value over the test rational inside constant evaluations, the zero tests of the same program started to fail on GCC 16.2.1 at run time, and only for the rational: `Line<rational>{}` held 0/1 in its first places and 0/0 from the place a constant evaluation had written on. Clang 23.1.4 was right throughout.

## Cause

A GCC wrong-code defect, outside Galoot, present on GCC 16.2.1 (the only GCC release tried): a value-initialized array (`std::array`, a C array, nested, a local, a member, a returned object) of an element type whose zero is not all zero bits is zero-filled from index 2 or later at run time after a constant evaluation of the same declaration in the same translation unit. The constant evaluation must write first at index 2 or later; a first write at index 0 or 1 does no damage, and an array of two elements is never damaged. Clang 23.1.4 is right throughout. The first reproducer and the spike (`docs/agent/records/spikes/gcc-shared-array-initializer/`) used the class-member form; the later investigation (`docs/agent/records/research/2026-11-09-gcc-constexpr-init-bug.md`, evidence beside it, with a 23-line `minimal.cpp` and no Galoot code) shows the member is not required.

Forms that are wrong: `T t{}`, `T t = {}`, `T t = T{}`, a helper that returns `T{}`, and a class member `c{}`. Forms that are safe: `T t = T()`, `T t;` (with every element written before it is read), and one initializer per element (`{zero_at(I)...}`). No compiler option avoids it: none of 18 tried, at any optimization level, in C++17 to C++26. GCC also starts the constant evaluations itself: a namespace-scope `const` or `static` initialized by a call with constant arguments, an automatic variable so initialized, and at -O1 and above a folded call in an expression all count, so a unit with no `constexpr` variable can still be damaged.

For `double`, `float`, `std::int64_t`, and `std::complex<double>` the damage is invisible: their zero is all zero bits, which is what the damage leaves. The rational's zero, 0/1, has a nonzero denominator, so it showed. A damaged place that is assigned again before it is read hides it too, which is why the library's own tests did not see it in the run-time kernels `run_map`, `run_product` and `run_sum` (which value-initialized `out{}`); a conversion to a wider layout over the test rational returned a wrong motor.

The multivector's coefficient storage writes the initializer out once per place, `coefficients{Ts{}...}`, which GCC compiles correctly, and the comment beside it says why. Since plan 0032 the kernels take the same form (decision 0225): `zero_places` gives `T{}` once per place, and two canaries in `core.mv`, a conversion and a product onto a wider layout over the test rational, each with a constant evaluation of the same kernel, fail on GCC if `out{}` returns. No upstream report was made (agents do not contact maintainers); the reproducer is kept for one.

## What detects it now

`core.mv`'s test "mv stays zero by default after a constant evaluation wrote into one" writes into a zero plane over the rational in a constant evaluation and checks a run-time zero plane, so the defect fails a test on GCC if the shorter initializer returns. The mutant `shared_zero_initializer` of the WU8a mutation records is killed by it on gcc-debug and is equivalent on clang-debug.

The general rule: a test of a value type covers at least one scalar whose zero is not all zero bits, and exercises it both in constant evaluation and at run time in one translation unit, because a compiler defect that zero-fills memory is invisible over the arithmetic types. For each run-time function that value-initializes a result array, add a test over such a scalar in which the same instantiation is evaluated at compile time with its first write at index 2 or later, and then at run time with a result place that no term writes, checked place by place against the expected value. A test that writes only at indices 0 and 1, or whose run-time call assigns every place, cannot see the defect. Retest `minimal.cpp` on each new GCC release (exit 0 means fixed) before any workaround is removed.

## Correction, 2026-11-01 (after the W5 review)

On GCC 16.2.1: the trigger then appeared to be an array that is a class member with a default member initializer; writes at places 0 and 1 did no damage and a write at place 2 or later damaged the later run-time zero from that place on; default-initialization of the class was damaged as well as value-initialization. The regression test now writes into every place of the plane.

## Correction, 2026-11-09 (decision 0225, item 2)

The 2026-11-01 correction still said a local `std::array<q, 6> a{}` was not affected and named the class-member form as the trigger. That was wrong: the earlier probes used two different functions for the constant evaluation and the run-time use, so they saw no damage. A local array is damaged when both are the same declaration, at any depth and for a C array as for `std::array`. The Cause and the general rule above state the defect as the later investigation measured it. That GCC shares one initializer among the places and splits it at the first write is a reading of the pattern (the lowered code shows the run-time loop bounds shortened), not a reading of GCC's source.

## Addendum, 2026-11-02

The defect is recorded in the knowledge base's `upstream-awareness.md` and is not reported upstream (decision 0162, item 11). Lesson 0066 records the second GCC defect found in WU8.
- 2026-11-09: the knowledge-base note and the upstream-awareness row carry the corrected trigger; still not reported upstream.
