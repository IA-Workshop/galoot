# Audit register of the core headers: the numerical read (plan 0047, unit H0)

- Written by Claude (agent, Opus 5.5) in the numerical-expert role, 2026-11-21, as one of the two independent reads of plan 0047 section 5.1. The other read was not looked at.
- Revision read: `main` at `01dce7f39d1cae6b65ab57349fe870aa187f5663`. `git diff --stat c95fee5 main -- include/` is empty, so the headers are those plan 0047 names (`c95fee5`). Line numbers below are at that revision.
- Branch `records/hardening-audit-numerical`, worktree `galoot-wt/hardening-audit-numerical`. Nothing outside this directory changed.
- Scope: every header under `include/galoot/` (the fourteen core headers, the umbrella, and the rga3 preset), read in full. The sites start from the candidates of plan 0047 section 5.1 and add those this read found.
- Reproducers: the `p*.cpp` files beside this register, built and run by [`run.sh`](run.sh), which writes each case's command, exit status, first 40 lines of output, and first error lines to `<case>.out` here. Compilers: GCC 16.2.1 (20260810) and Clang 23.1.5, `-std=c++23 -Wall -Wextra`, x86-64. Each case is one compiler call of under a second; the p09 cases ran under `memguard.sh` (16 GiB cap) and `tools/timed`, whose rows are in [`p09.timed.tsv`](p09.timed.tsv). No test suite was built and the job pool was not used (it read "running", 22 of 23 slots free, before and after the full run). Every `.out` file comes from one run of `run.sh` with no arguments, at the revision above.

## How to read a row

Each row gives: the site (file and lines); the class of fault; the smallest input I found that shows it; what a user sees today; the verdict; the severity; whether a test pins the behavior today; whether a fix would change behavior a user may rely on; and the test I propose.

- **Verdicts.** *Confirmed*: a reproducer here shows it, and its output is cited. *Dismissed*: the reason, and what I checked. *Open*: what would settle it.
- **Severity.** *High*: a wrong result or undefined behavior from an input the library accepts and documents, with no diagnostic in any mode. *Medium*: the same with a diagnostic in some mode, or a stated guarantee that fails for an accepted input. *Low*: a documented limit, a precondition violation, or a weak diagnostic. *Nit*: a comment or rationale that does not match the code, with no effect on results.
- **Classes**, as the brief names them: undefined behavior (UB), wrong result, silent loss, unhelpful or missing diagnostic, integer edge, rounding edge, contract stated and not kept.

## Summary

| ID | Site | Class | Verdict | Severity |
| --- | --- | --- | --- | --- |
| N01 | `mv.hpp:366`, `:313`; `linear.hpp:364`, `:378`; the claim at `mv.hpp:745-748`, `:876-878` | UB at run time; integer edge; contract not kept | confirmed | medium |
| N02 | `bits.hpp:48-51` and its callers; `blade_set.hpp:131-150`; `metric.hpp:129-132` | UB and silently wrong values when a stated precondition is broken in an unchecked build | confirmed | low |
| N03 | `transform.hpp:486-513`, `:720-728` | rounding edge; check weaker than stated for `std::float16_t` | confirmed | low to medium |
| N04 | `presets/rga3/motions.hpp:81-86`, the claim at `:36-37` | rounding edge; silent loss of unitization | confirmed (documented in part) | low |
| N05 | `presets/rga3/motions.hpp:90-95` | missing diagnostic: a NaN or infinite angle | confirmed | low |
| N06 | `blade_product.hpp:122-192`, `plan.hpp:147-153`, the claims at `metric.hpp:51-55` and `docs/guide/limits.md:25-26` | contract not kept: above 21 generators Clang rejects every antiproduct and dual | confirmed | medium |
| N07 | `blade_product.hpp:75`, `:94`, reached through `plan.hpp:553-556` | unhelpful diagnostic: an overflowing coefficient cascades | confirmed | low |
| N08 | `metric.hpp:300-314` | unhelpful diagnostic: a hand-written metric beyond `max_form_entry` | confirmed | low |
| N09 | `blade_set.hpp:426-469` | unhelpful diagnostic: a tab between names | confirmed | nit |
| N10 | `mv.hpp:390-401` (release exception 8) | unhelpful diagnostic: one-blade value from an explicit-only source | confirmed (known) | low |
| N11 | `presets/rga3/motions.hpp:158-164` | missing diagnostic: `translation` over a scalar without division | confirmed | low |
| N12 | `metric.hpp:57-62`, `docs/guide/limits.md:110-113` | rationale that does not match the code | confirmed by reading | nit |
| N13 | `mv.hpp:892-895`, `transform.hpp:764`, `basis_change.hpp:1136-1137` | contract stated more widely than kept: "exact" over a narrow or overflowing integer scalar | confirmed | low |
| D01 | `basis_change.hpp:266-267`, `blade_product.hpp:91-92`, `basis_change.hpp:315-316` | integer edge: magnitude of the least `std::int64_t` | dismissed | — |
| D02 | shifts of `mask_t` at `MaxN` 32 | integer edge | dismissed | — |
| D03 | `mv.hpp:755-781` (`holds_exactly`) | integer and rounding edge of coefficient conversion | dismissed | — |
| D04 | `plan.hpp:319` (`collect_result`'s capacity check) | error path | dismissed | — |
| D05 | `plan.hpp:605-607`, `:926-927` (a mask listed twice) | error path | dismissed, with what happens recorded | nit |
| D06 | `transform.hpp:457-481` (the margin of the rounding check) | rounding edge | dismissed | — |
| D07 | `transform.hpp:422-439`, `:496-513` (the counts and the padding) | rounding edge | dismissed | — |
| D08 | `transform.hpp:758-770` under `-ffp-contract=fast` | rounding edge | dismissed (record 0040) | — |
| D09 | `transform.hpp:753-757` under a directed rounding mode | rounding edge | dismissed as documented | — |
| D10 | `basis_change.hpp:403-469`, `:507-530`, `:883-927` (exact divisions) | integer edge | dismissed | — |
| D11 | `format.hpp:78-129`, `:245-264` | UB at run time | dismissed | — |
| D12 | `linear.hpp:412-424` (integer division) | integer edge | dismissed as documented | — |
| O01 | `rga3::rotation`'s rounding, `transform`'s worst observed ratio | rounding edge | open, for H3 | — |
| O02 | `GALOOT_CHECKING` differing between translation units | configuration | open, for H5 | — |
| O03 | `basis_change.hpp:427-469`: Bareiss intermediates (release exception 9) | integer edge | open, for H2 | — |

## Which sites constant evaluation covers

Undefined behavior of the core language in a constant expression makes it ill-formed, so on the inputs a program uses, every site that runs only in a constant evaluation is already checked by the compiler. That covers the whole generator: `blade_product.hpp`, `plan.hpp`, the set constructors of `blade_set.hpp`, the checks and term generation of `basis_change.hpp`, `metric.hpp`'s factories, and the kernel construction in `mv.hpp`, `linear.hpp`, `dual.hpp`, and `transform.hpp`. p01 (constant case) and p05 case 4 show the compilers stopping on such faults.

Sites that run at run time, where nothing but the code itself guards them:

- the kernels: `run_map`, `run_product`, `accumulate`, `zero_places` (`mv.hpp:298-369`), `run_sum` (`linear.hpp:154-167`), `run_change` and `divide_by` (`basis_change.hpp:1008-1028`);
- the scalings and negation of `linear.hpp:359-436`;
- the rounding check of `transform` (`transform.hpp:496-620`);
- `rga3::rotation` and `rga3::translation`;
- `format.hpp`'s `write`;
- the public `constexpr` functions a user may call at run time: `grade`, `antigrade`, `reorder_sign`, `complement_mask`, the sign functions (`bits.hpp`), `canonical_less` and `canonical_name` (`blade_set.hpp`), and the members of `metric` and `basis_change`. The generator's `detail` functions are `constexpr`, not `consteval`, so they too can run at run time, but only a caller of `detail` reaches them that way.

## Confirmed sites

### N01 Narrow unsigned scalars overflow `int` in the kernels

- **Site.** `mv.hpp:366`: `static_cast<T>(x[a] * y[b])` in `run_product`; `mv.hpp:313`: `static_cast<T>(C) * value` in `accumulate` (inferred, not run, see below); `linear.hpp:364` and `:378`: `s * x` and `x * s`. The header states, for `holds_exactly` (`mv.hpp:745-748`) and in `apply`'s contract (`mv.hpp:876-878`), that an unsigned scalar is one "whose arithmetic is modular".
- **Class.** Undefined behavior at run time; integer edge; a contract stated and not kept.
- **Input.** `mv<R<2,0,0>, blades<"e1 e2">, std::uint16_t>{65535, 0}` squared with `geometric_product`, or multiplied by `std::uint16_t{65535}`. Both operands promote to `int`, and 65535 × 65535 = 4,294,836,225 exceeds `INT_MAX`.
- **What a user sees.** In a constant evaluation, a compile error: GCC "overflow in constant expression [-fpermissive]" at `mv.hpp:366`, Clang "value 4294836225 is outside the range of representable values of type 'int'" (`p01.constant.*.out`). At run time, undefined behavior with no diagnostic; under `-fsanitize=undefined` both compilers report "signed integer overflow: 65535 * 65535 cannot be represented in type 'int'" at `mv.hpp:366:58` and `linear.hpp:364:44` (`p01.run.*.out`). In that unoptimized build the value printed was the modular one, 1, which an optimizer is free not to give.
- **Contrast in the same run.** Over `std::int16_t` the product is formed in `int` without overflow (32767² fits) and the conversion back is defined (C++20, modular), so it gives 1 with no report. Over `int` and wider signed types an overflowing product is the scalar's own undefined behavior, as for any `int` arithmetic (N13).
- **Scope, inferred.** Any unsigned type narrower than `int` whose maximum squared exceeds `INT_MAX`: `unsigned short`, `std::uint16_t`, `char16_t`. `unsigned char` and `char8_t` cannot overflow (255² fits). The `accumulate` path with a coefficient other than ±1 has the same promotion; p01 runs in R(2,0,0), whose coefficients are all ±1, so that path was not run. Sums cannot overflow `int` for these types (2 × 65535 fits).
- **Severity.** Medium: an exact scalar the concepts accept, undefined behavior at run time without a diagnostic in any mode, against a stated property.
- **Pinned today.** No: no test under `test/` names `uint16_t`, `unsigned short`, or `char16_t` as a scalar (grep).
- **Would a fix change relied-on behavior.** Converting each operand to `unsigned int` before `*` for an unsigned `T` narrower than `int` makes the stated modular result defined; no defined behavior changes. Rejecting such scalars would remove accepted input and is the owner's.
- **Proposed test.** A constant-evaluated `geometric_product` over `std::uint16_t` of 65535 e1 with itself, expecting 1 on the scalar, and the same for `s * x` (each fails to compile today, so it is committed labeled as failing, W2); plus a run-time case under the sanitizer preset. The mutant it kills: removing the conversion to unsigned.

### N02 Run-time calls of the bit functions with a broken precondition

- **Site.** `bits.hpp:48-51` (`low_bits`, a shift by `mask_bits - n`), reached from `complement_mask` (`:119-124`), `right_complement_sign` and `left_complement_sign` (`:133-151`); `antigrade` (`:83-88`); `canonical_name` (`blade_set.hpp:131-150`); `metric::is_null` (`metric.hpp:129-132`).
- **Class.** Undefined behavior, and silently wrong values, when a stated precondition is broken in an unchecked build.
- **Input.** With `GALOOT_CHECKING` 0, at run time: `complement_mask(1, 0)`, `complement_mask(1, 33)`, `antigrade(1, 0)`, `canonical_name(1 << 9)`, `R<3,0,0>().is_null(3)`.
- **What a user sees** (`p02.unchecked.*.out`): UBSan reports "shift exponent 32 is too large for 32-bit type" at `bits.hpp:50`, in `low_bits`; `complement_mask(1, 0)` printed `0xfffffffe`, `complement_mask(1, 33)` printed 0, `antigrade(1, 0)` printed -1 against its guarantee 0 <= antigrade <= n, `canonical_name(1 << 9)` printed `"e"`, a name with no index, and `is_null(3)` read past the array (caught only by `_GLIBCXX_ASSERTIONS`). With `GALOOT_CHECKING` 1 the first call aborts with "galoot: assertion failed: n must be from 1 to 32 (include/galoot/core/bits.hpp:121)" (`p02.checked.*.out`).
- **Severity.** Low. Each precondition is stated and checked in checking mode and in every constant evaluation. What is missing is the statement, on the declaration, that breaking it in an unchecked build is undefined behavior (for `complement_mask`, the sign functions, and `is_null`) or gives an unspecified value (for `antigrade` and `canonical_name`). Decision 0253 item 14 keeps exactly such preconditions on the declaration in full.
- **Pinned today.** The checking-mode aborts are pinned (`test/core/bits_assert_fail.cpp`, `test/core/blade_set_name_assert_fail.cpp`); the unchecked behavior is not, and should not be.
- **Would a fix change relied-on behavior.** No: the fix is a sentence on each declaration. Masking the shift count would make a broken call defined, which no one can rely on today.
- **Proposed test.** None beyond the existing aborts: an unchecked violation has no specified result to test. The register records it so that the comment change has a reason.

### N03 The rounding check of `transform` over `std::float16_t` misses gross non-versors at ordinary magnitudes

- **Site.** `transform.hpp:496-513` (`padded_magnitude`, the padding τ = λ / u), with the floor stated at `:486-495` and `:720-728` for double and float only. `rounding_checked_scalar` (`:411-414`) admits any IEEE 754 binary type that rounds to nearest, and `std::float16_t` is one (GCC 16: `is_iec559` 1, `round_style` 1, digits 11).
- **Class.** Rounding edge; a check weaker than the header leads a reader to expect.
- **Derivation.** For binary16, λ = 2^-14 and u = 2^-11, so τ = 2^-3 = 0.125. The padding enters every rounded product of the magnitude P, so the tolerance has a floor of about γ_(K+1) τ W for W padded products on the path, which for unit-size factors is comparable to the rounding error of the computation itself, and for factors of size 10^-4 is above the whole value. In double the same floor is near 10^-306 and in float near 10^-36, as the header says.
- **Input.** The header's own non-versor: in R(4,0,0), v = 1 + e12 + 2 e13 + 3 e14 on x = s e1. For each scale s, p03 prints the largest |d| / tolerance, the comparison the check makes (`p03.table.g++.out`):

  | s | float16 | float |
  | --- | --- | --- |
  | 1 | 454 | 4.19e6 |
  | 1e-2 | 37.8 | 4.19e6 |
  | 1e-3 | 4.06 | 4.19e6 |
  | 3e-4 | 1.22 | 4.19e6 |
  | 2e-4 | 0.816 | 4.19e6 |
  | 1e-4 | 0.408 | 4.19e6 |

  Below about s = 2.5e-4 the float16 check passes it. At s = 1e-4, a normal float16 (the least normal is about 6.1e-5), `transform` in checking mode returns 0.0013 on e1 with no abort, while the parts it dropped, computed again in double from the same inputs, are -2e-4, -4e-4, and -6e-4, larger than x itself (`p03.checked-half.g++.out`). Over float the same call aborts with "transform: v is not a versor on x within rounding" (`p03.checked-float.g++.out`).
- **What a user sees.** In checking mode over `std::float16_t`, no abort for a non-versor whose inputs are a few times 10^-4 or smaller; the dropped part is silently lost.
- **Severity.** Low to medium: a checking mode that passes what it exists to catch, for a type the header calls checked; limited to `std::float16_t`, which GCC provides and Clang's library here does not.
- **Pinned today.** `transform_rounding_test.cpp` (around line 1131) pins that `std::float16_t` is checked; nothing pins the floor.
- **Would a fix change relied-on behavior.** Stating the floor for float16 changes nothing. A smaller τ for binary16 (τ = λ suffices for the bound, `transform.hpp:466-469`) would let the check fire on inputs it passes today, so a checking build could abort where it now returns; that changes checking-mode behavior only, and never a computed value. The reason given for λ / u, avoiding subnormal products, matters little for a type that x86-64 computes through float, where 2^-14 times u is far from float's subnormal range (inferred, not measured).
- **Proposed test.** For whichever the owner rules: the table above as a test that the float16 check fires for s = 1e-3 and, with τ = λ, also for s = 1e-4. The mutant: τ multiplied by 2^k.

### N04 `rga3::rotation` loses unitization where the squares of the direction are subnormal

- **Site.** `presets/rga3/motions.hpp:82` (`std::sqrt` of the sum of squares) and `:85` (the check is `std::isnormal(length)` only). The comment at `:36-37` says the result "is a unitized motor, up to rounding, for a direction of any positive length"; the Requires clause at `:52-55` asks for a length between about 1e-150 and 1e150, and `:66-68` says a length whose squares are subnormal "passes the check and loses accuracy; no test covers that case".
- **Class.** Rounding edge; silent loss of accuracy; the "any positive length" sentence contradicts the Requires clause.
- **Input.** A direction along (1, 1, 1) of length L, theta = 1, double, `GALOOT_CHECKING` 1. p04 prints the defect |w - 1| of the squared weight norm w = Q_vx² + Q_vy² + Q_vz² + Q_vw², in units of u = 2^-53, summed in binary128 (`p04.*.out`, both compilers equal):

  | L | 1 | 1e-150 | 1e-154 | 1e-156 | 1e-158 | 1e-160 | 1e-161 | 1e150 | 1e153 |
  | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
  | \|w - 1\| / u | 0.99 | 0.99 | 1.82 | 3.2e3 | 6.9e7 | 1.0e12 | 7.5e13 | 0.99 | 0.99 |

  At L = 1e-161 the motor's weight is off by about 8e-3 relative, and no check fires. Below about 2e-162 each square underflows to zero and the check aborts. Above the documented range the result stays unitized up to about 1.3e154, where the squares overflow and the check aborts.
- **Severity.** Low: documented in the Requires clause, and the input is extreme. The sentence at `:36-37` should carry the same range.
- **Pinned today.** No (the comment says so). `rotation_assert_fail.cpp` pins the zero and infinite lengths.
- **Would a fix change relied-on behavior.** Scaling the direction by a power of two before the sum, as `hypot` does, gives the same bits wherever no square is subnormal, since the scaling is exact (inferred from the arithmetic, not run); it adds a branch or a `std::frexp` to the cost the comment lists. Narrowing the sentence changes nothing.
- **Proposed test.** For H3: the table's L = 1e-158 case, with the bound H3 derives, asserting the defect within it after a fix, or the documented loss without one.

### N05 `rga3::rotation` checks the length but not the angle

- **Site.** `presets/rga3/motions.hpp:90-95`.
- **Class.** Missing diagnostic.
- **Input.** `rotation(Line{0, 0, 1, 0, 0, 0}, NaN)` or with `inf`, in a checking build.
- **What a user sees.** A motor whose coefficients on the line's blades and on e1234 are NaN, with no abort (`p04.*.out`: "theta = nan: e43 nan, e1234 nan (no check fired)"; the same for inf). The comment's Requires clause says nothing about theta being finite.
- **Severity.** Low: the result is plainly NaN, but the function already checks one non-finite input and not the other, and a NaN motor that reaches a `transform` in checking mode would then abort there, with a message about versors (inferred, not run).
- **Pinned today.** No.
- **Would a fix change relied-on behavior.** A `GALOOT_ASSERT(std::isfinite(theta), ...)` changes only checking builds given a non-finite angle, from a NaN motor to an abort.
- **Proposed test.** A run-time abort test beside `rotation_assert_fail.cpp`, with the message.

### N06 Above 21 generators, Clang rejects every antiproduct and every dual, even of sparse layouts

- **Site.** `blade_product.hpp:122-172` (`term_list<Capacity>`, a `std::array` of Capacity terms) sized by `product_capacity` (`:186-192`), 2^(|A ∪ B| - 1), and `plan.hpp:147-153` (`pair_capacity`, which for an antiproduct takes the complements). An antiproduct, and the bulk dual, which is the product of x~ with the antiscalar, have a factor of grade near n, so the capacity is about 2^(n-1) whatever the metric and the layouts. The claims it contradicts: `metric.hpp:51-55` ("a factory given a larger MaxN, up to mask_bits, builds an algebra that every later part of the library accepts but that is not tested") and `docs/guide/limits.md:25-26`.
- **Class.** Contract not kept; a compile-time limit the limits page does not list.
- **Input.** `R<n, 0, 0, 32>()` with the layout {e1, en} written as masks, one operation per build, each with its expected value derived by hand and checked by `static_assert` (`p09_wide_algebras.cpp`). n = 17, 24, 32 on both compilers:

  | Operation | GCC 17, 24, 32 | Clang 17 | Clang 24, 32 |
  | --- | --- | --- | --- |
  | geometric product | builds, value right | builds | builds |
  | wedge | builds | builds | builds |
  | right complement | builds | builds | builds |
  | bulk dual | builds | builds | rejected |
  | antiwedge of complements | builds | builds | builds |
  | geometric antiproduct | builds | builds | rejected |

  Clang's reason (`p09.n24.op4.clang++.out`): "constexpr variable 'count' must be initialized by a constant expression", with the note "cannot allocate array; evaluated array bound 8388608 exceeds the limit (1048576); use '-fconstexpr-steps' to increase this limit", at `blade_product.hpp:166`, four errors in all. The threshold is n = 22 (2^21 elements; n = 21 builds, `p09.n21.op4.clang++.out`, `p09.n22.op4.clang++.out`). With `-fconstexpr-steps=100000000` the n = 24 bulk dual builds in 0.17 s (`p09.n24.op4.clang++.steps.out`). Every case took under 0.4 s and 130 MiB on either compiler (`p09.timed.tsv`): GCC does not materialize the zero-filled array.
- **Scope, inferred and not run.** Everything built on an antiproduct or on the bulk product: `bulk`, `weight`, the weight duals, the left duals, `antidot`, `project`, `antiproject`, `transform`, and `compose`.
- **What a user sees.** On Clang, above 21 generators, one of these operations fails to compile with Clang's own wording about constant-evaluation limits, not a message of the library's, though its layouts hold two blades.
- **Severity.** Medium, mostly for owner item O9: H2's planned tests of sparse use at 24 and 32 generators will fail on Clang for these operations unless they raise the step limit, and the guide's sentence is false on Clang for them today.
- **Pinned today.** No: nothing above 16 generators is tested.
- **Would a fix change relied-on behavior.** (a) Stating the limit and the flag on the limits page, beside the other Clang limits, changes nothing. (b) Sizing the term list by a bound that depends on the metric (a diagonal metric gives each pair one term) changes no result, but it is a change to the generative mechanism, which plan 0048's ordering rules govern. (c) Lowering the bound to 21 removes accepted input and is the owner's, for a major release.
- **Proposed test.** For H2: the six operations at n = 17, 24, and 32 with the values above, on both compilers, with the step limit raised on Clang where option (a) is chosen; the mutant is a capacity one too small (`std::size_t{1} << (generators - 2)`), which the existing capacity assertion catches in a dense case.

### N07 An overflowing product coefficient cascades into several errors

- **Site.** `blade_product.hpp:75` and `:94` (the coefficient checks), reached through `plan.hpp:553-556`, where `count` and `generated` are evaluated after the failure.
- **Class.** Unhelpful diagnostic.
- **Input.** `from_gram` of diag(32767, 32767, 32767, 32767, 32767), every entry within `max_form_entry`, and the product e12345 e12345 = 32767^5 ≈ 3.8e22 over `std::int64_t` (`p05.case4.*.out`). For comparison, case 3, 32767^4 = 1,152,780,773,560,811,521 at n = 4, is generated and right.
- **What a user sees.** The message "a product coefficient does not fit coefficient_type" is in the output, but Clang reports four errors in 100 lines and GCC fourteen "error" lines in 181, the later ones "no matching function for call to 'filled_plan'" and "invalid operands to binary expression". The comment of `make_product_plan` (`plan.hpp:530-534`) promises one message per misuse for the set checks it makes first; an overflow is not among them.
- **Severity.** Low.
- **Pinned today.** Not by me; H1 counts errors per message.
- **Would a fix change relied-on behavior.** No.
- **Proposed test.** A compile-fail test of case 4 with its message and an error count, which records the count today and fails when the cascade is removed (expected values change then, by W9).

### N08 A hand-written metric beyond `max_form_entry` gets no message of the library's

- **Site.** `metric.hpp:300-314` (`metric_like`).
- **Input.** `struct big { static constexpr core::metric<2> value{{{{40000, 0}, {0, 1}}}}; }` used as `mv<big, full<2>, double>` (`p07.case5.*.out`).
- **What a user sees.** "constraints not satisfied for class template 'mv'", with the note "because 'detail::is_valid_form_v<big>' evaluated to false" on Clang and "template constraint failure" on GCC, then a second error for the unusable alias. Nothing says whether the form is unsymmetric or out of range; `from_gram` says which.
- **Class and severity.** Unhelpful diagnostic (C9), low. The contract (false, not an error) is kept.
- **Would a fix change relied-on behavior.** A message would have to come from a use site, as `blades` does for tags; `metric_like` itself must stay a concept that answers false.
- **Proposed test.** For H1, a compile-fail test pinning today's wording, if H1 judges the case worth one.

### N09 A tab between blade names is reported as a bad index

- **Site.** `blade_set.hpp:460-469` (`next_name` separates by spaces only) and the message at `:513-515`.
- **Input.** `blades<r300, "e1\te2">` (`p07.case1.*.out`).
- **What a user sees.** "blades: after the e, a name lists generator indices, each a digit from 1 to 9". The guide and the comment say names are separated by spaces, so the behavior is as documented; the message does not point at the separator.
- **Severity.** Nit.
- **Would a fix change relied-on behavior.** Accepting tabs would accept new input; a message for a non-digit that is whitespace would not change what is accepted.
- **Proposed test.** None unless the message changes.

### N10 A one-blade value from a source that converts only explicitly (release exception 8)

- **Site.** `mv.hpp:390-401`, documented there and in release exception 8.
- **Input.** A ring scalar constructible from `std::int64_t` only explicitly, `mv<r300, blades<r300, "e1">, ring> x{1}` (`p07.case3.*.out`).
- **What a user sees.** "no matching constructor" (Clang, 46 lines) or "no matching function for call to ...mv(<brace-enclosed initializer list>)" (GCC, 125 lines), listing the candidates. Known and accepted by decision 0241 item 1; plan 0047 puts its compile-fail test in H1 and its fix in H6.
- **Severity.** Low. **Pinned today.** Not found by a search of test file names for "one_blade" and "one-blade"; H1 confirms.

### N11 `rga3::translation` over a scalar without division gets the compiler's error

- **Site.** `presets/rga3/motions.hpp:158-164`. The comment requires "T has division"; the constraint asks only `ring_scalar` and not `std::integral`.
- **Input.** The same `ring` type, `translation(ring{1}, ring{2}, ring{3})` (`p07.case4.*.out`).
- **What a user sees.** "invalid operands to binary expression ('ring' and 'ring')" (Clang, 9 lines) or three "no match for 'operator/'" (GCC, 91 lines), inside the function's body.
- **Severity.** Low. **Would a fix change relied-on behavior.** A rejecting overload with a message, like the integer one beside it, changes only an error's text.
- **Proposed test.** For H1, a compile-fail test with the message, if a message is added.

### N12 The rationale of `max_form_entry` names a computation the code does not make

- **Site.** `metric.hpp:57-62` and `docs/guide/limits.md:110-113`: the bound is "the largest k for which the 2 x 2 minor g_ii g_jj - g_ij^2 of entries of magnitude k, at most 2 k^2, fits in entry_type".
- **Finding.** No minor, and no product of entries, is formed in `entry_type`: a search of the core for arithmetic on `entry_type` finds only `isqrt`, comparisons, and the divisions of `reduced` (`basis_change.hpp:369-386`), whose results are no larger than their operands. `blade_product.hpp` multiplies entries as `coefficient_type` (`:219`, `:254`), and `basis_change.hpp` widens its rows first (`:333-344`). So 32,767 is not the bound the code needs: a 2 × 2 minor would fit `std::int64_t` for entries up to about 2^31, while a minor of order 5 already overflows at 32,767 (N07).
- **Severity.** Nit: no result depends on it. Changing the bound would change accepted input, which is the owner's; correcting the sentence changes nothing.

### N13 "Exact" over an integer scalar holds only while the scalar's own operations do not overflow

- **Site.** `mv.hpp:892-895` ("with an exact scalar the result is exact"), `transform.hpp:764`, and `basis_change.hpp:1136-1137`.
- **Input.** p01's `std::int16_t` case: (32767 e1)² gives 1 on the scalar, the product reduced modulo 2^16, with no report, since the conversion back to `int16_t` is defined. Over `int` or `std::int64_t` an overflowing product is undefined behavior of the scalar's own arithmetic.
- **Finding.** `exact_scalar` is read from `numeric_limits<T>::is_exact`, which every built-in integer type sets, so these guarantees read as exact over any integer type. They hold for the arithmetic of the scalar: exact in Z modulo 2^k for the narrow types and the unsigned ones (once N01 is fixed), and exact over the integers only while nothing overflows. The guide's "Coefficient range" section speaks of the generated coefficients' range, not the scalar's.
- **Severity.** Low; one clause in each guarantee, or a sentence in the guide, settles it.
- **Would a fix change relied-on behavior.** No.

## Dismissed sites

### D01 The magnitude of the least `std::int64_t` (plan 0047 section 5.1)

`basis_change.hpp:266-267` (`multiply_noting`), `blade_product.hpp:91-92` (`multiply_checked`), and `basis_change.hpp:315-316` (`change_gcd`) negate a negative argument, which is undefined for -2^63. No input reaches them with it:

- A change's rows and denominators, and a metric's entries, are `int`, widened to `std::int64_t`.
- Every value the checks and the generation form comes from `add_noting` and `multiply_noting`, or `add_checked` and `multiply_checked`, which return a value within -max to max or, on overflow, 0 with the flag cleared or a failed assertion; from a negation of such a value; from a quotient of such a value (`eliminate_entries`, `inverse_rows`, `typed_change`), whose magnitude is no larger; or from a sign of ±1 times one.
- `change_gcd` receives values of the same kinds; its `x % y` has y nonzero and both magnitudes nonnegative.

What I checked: the reading above, for every call of the three functions; p06 case 1, the most extreme rows a user can write (`INT_MIN` and `INT_MAX` entries, between two R(3,0,0) tags), which gives one error with the change's own message "basis_change: an exact value the change computes does not fit coefficient_type; use rows with smaller entries" on both compilers (`p06.rows.*.out`); and p06 case 2, which calls the guard directly with -2^63 at run time to show that the guard itself would be undefined if reached ("negation of -9223372036854775808 cannot be represented", `p06.guard.*.out`). Only a caller of `detail` can do that. The functions' Requires clauses (`blade_product.hpp:65`, `:82-83`) already state the range.

### D02 Shifts of `mask_t` at `MaxN` 32

`low_bits(32)` is `~0 >> 0`; `blade_product` skips its range shift when N is `mask_bits` (`:283-289`); `append_grade` shifts by at most 31; `reorder_sign` by at most 31; `canonical_name` by 9. p09 built six operations at n = 32 on GCC, and four of them on Clang (N06 is the other two), each with its value checked by `static_assert`; no shift fault appeared in any constant evaluation. The unchecked run-time shift by 32 for n = 0 is N02, not this.

### D03 Converting coefficients to the scalar (`holds_exactly`, `mv.hpp:755-781`)

Its rule for a binary floating type (the odd part of |c| within `digits` bits, |c| below 2^max_exponent) is the exact representability condition, and its magnitude of a negative c is formed in `std::uint64_t`, defined for -2^63. p05: diag(4096, 4096), whose e12 e12 = -2^24, is accepted over float with the exact value (case 1); diag(4097, 4097), whose -4097² = -16,785,409 is odd with 25 bits, is rejected with "a * b: the scalar type cannot hold one of the product's exact coefficients" (case 2); 32767^4, odd with 60 bits, is held by `std::int64_t` (case 3) and rejected for double (case 5). Each case gave one error. The unsigned rule (|c| at most max) is stricter than modular arithmetic needs, which is safe.

### D04 `collect_result`'s capacity check (`plan.hpp:319`)

Plan 0047 asks what a non-checking constant evaluation shows here. `checking_now()` is true in every constant evaluation (`assert.hpp:115-120`), so the assertion's own message would show; and it cannot fire from user input, since its only caller, `collected` (`plan.hpp:510-516`), gives it as many places as the plan has terms, and a plan has no more distinct blades than terms. Dismissed by reading; no reproducer is possible without a `detail` call.

### D05 A mask listed twice in one set (`plan.hpp:605-607`, `:926-927`)

No multivector can hold such a set: `mv` rejects it with "mv: the layout holds one blade twice, perhaps in both orientations" (`mv.hpp:577-579`), and `merge` rejects it too. The public layout templates do take it: p07 case 2 builds `geometric_product_layout<r300, twice, twice>` and `right_complement_layout<r300, twice>` for {e1, e1}, each a layout of one blade (`p07.case2.*.out`). That is consistent with the plan's statement that each place gets its own terms on one position. Nothing wrong results, since nothing can be computed on the set. A test that pins it, as plan 0047 suggests, is p07 case 2 with its sizes.

### D06 The margin of the rounding check (`transform.hpp:457-481`)

I derived the margin's inequality again. With γ_k = ku / (1 - ku), (1 - u)^(2K+1) ≥ 1 - (2K + 1)u, and γ_K / γ_(K+1) = K(1 - (K + 1)u) / ((K + 1)(1 - Ku)), the claim (1 - u)^(2K+1) γ_(K+1) ≥ γ_K follows from (K + 1)(1 - Ku)(1 - (2K + 1)u) - K(1 - (K + 1)u) = 1 - (K + 1)(2K + 1)u + K(K + 1)(2K + 1)u², which is positive when (K + 1)(2K + 1)u < 1. So the stated condition suffices, as the comment says. The rest of the chain holds: fl(γ_(K+1)) ≥ (1 - u)γ_(K+1) because ku and 1 - ku are exact for these k, and rounding to nearest is monotone, so the computed tolerance is at least |d|, which is itself a floating-point number. `margin_holds` (`:528-535`) compares in integers and avoids shifting by the width.

### D07 The counts of roundings and the padding (`transform.hpp:422-439`, `:496-513`)

By reading, `kernel_roundings` counts the operations `accumulate` performs: one rounding for the product x y; one more for a coefficient c other than ±1, which multiplies; none for the first term of a result, which is assigned or negated; one for each later addition or subtraction, after the larger of the two counts. The standard model then gives |d - e| ≤ γ_K P for the padded magnitude P, by Higham's Lemma 3.3 for products (γ_i + γ_j + γ_i γ_j ≤ γ_(i+j)) and the usual bound for sums, with the absolute error η ≤ λu of an underflowing product covered by any padding τ ≥ λ(1 - u). The lower bound of the computed P, fl(fl(z) + τ) ≥ (1 - u)²(z + τ) when τ ≥ λ, holds as the comment derives. The coefficient c converts to T exactly (D03), so |c| in `padded_magnitude` adds no rounding. The bound is right; its floor is N03.

### D08 The bound under multiply-add contraction (`transform.hpp:758-770`)

Record `records/numerical/0040-contraction-check.md` proved that the bound and the check's verdict on a versor hold under any contraction, and measured that the bits of the checked and unchecked paths can differ under `-ffp-contract=fast`. The header's guarantee was corrected after that record and now says so. Dismissed with that record; plan 0047 has H3 measure it again at the tip.

### D09 A directed rounding mode set at run time (`transform.hpp:753-757`)

`numeric_limits<T>::round_style` reports round to nearest whatever `fesetround` has set, so a program that rounds upward runs a check whose bound assumes nearest rounding, and with up to twice the error per operation a versor could fail it. The comment states the assumption ("rounds once to the scalar's own precision, to nearest"). Dismissed as documented; not run.

### D10 The exact divisions of the change of basis (`basis_change.hpp:403-469`, `:507-530`, `:883-927`)

Every `/` there divides by a pivot that is nonzero (the elimination returns at a zero pivot), by a gcd that is at least 1 (the denominators are positive, so each gcd taken with one is positive), or by a denominator `own[k]` that is a product of positive denominators. After an overflow, the elimination continues to the end of its step with zeros, so its division may be inexact, but `fits` is false and the result is discarded at `:456-457`. No division by zero and no -2^63 / -1 is reachable. Dismissed by reading, with p06 case 1 as the extreme input.

### D11 Run-time formatting (`format.hpp`)

`printed_name` holds 11 characters; a declared name, checked by `first_error` before it is used, has at most 10 (e and nine distinct digits), and a canonical name with its sign at most 11. An algebra above nine generators, or a scalar without a formatter, is rejected at compile time before `write` is instantiated (`:216-240`). `write` indexes `names` and `x.coefficients` by the same bound, `Blades.size()`. Dismissed by reading; not run.

### D12 Integer division by a multivector's scalar (`linear.hpp:412-424`)

A zero divisor is checked for an integer type in checking mode and in every constant evaluation; with `GALOOT_CHECKING` 0 it is undefined behavior, and min() / -1 overflows, both as the comment states. Dismissed as documented; not run.

## Open sites

### O01 The rounding of `rga3::rotation` and the transform check's worst observed ratio

Neither has a derived bound with its constants in the headers: the comment of `rotation` describes the rounding qualitatively (`motions.hpp:70-74`), and the transform check has a bound but no recorded worst ratio of error to bound on adversarial inputs. Plan 0047 assigns both to H3. p04's first rows give one data point: in the documented range the squared weight norm of the rotor was within 0.99 u of 1 for theta = 1 and the (1, 1, 1) direction. What would settle it: H3's derivation and its campaign against high-precision ground truth.

### O02 Translation units built with different `GALOOT_CHECKING`

`assert.hpp:30-32` requires one `GALOOT_FORCE_INLINE` mode per program and says why; it states no such rule for `GALOOT_CHECKING`, yet `transform`'s body differs by that setting at run time (the call of `checking_now()` takes a different branch, and in checking mode computes the dropped part), and so does every function that contains a `GALOOT_ASSERT`. Whether that makes two definitions of one inline function differ, in the sense of the one-definition rule, is a question about the language, and the numerical consequence follows from record 0040: the kept coefficients agree bit for bit between the two paths under `-ffp-contract=off` and `on`, and may differ under `fast`. What would settle it: H5.

### O03 Overflow of the elimination's intermediates (release exception 9)

The fraction-free elimination (`basis_change.hpp:427-469`) forms a_kk a_ij - a_ik a_kj before dividing by the previous pivot, so an intermediate can overflow `std::int64_t` while the minor it divides to fits, and the change is then rejected with the overflow message. I did not build the smallest such input. What would settle it: H2's characterization, as plan 0047 section 4 assigns.

## Notes for the units that follow

- **H1.** N07, N08, N10, and N11 are diagnostics; the reproducers give the inputs and today's counts of errors and lines.
- **H2.** N01 and N13 for integer scalars; N06 before writing the tests at 24 and 32 generators; O03.
- **H3.** N03, N04, N05, O01.
- **H5.** O02.
- **H6.** N01 is a fix to a kernel's arithmetic for narrow unsigned scalars. N06 (b) and N03's smaller τ touch the generator or the check, and come under the ordering rules of plan 0047 section 9.2 and decision 0253.
- **Knowledge base.** The general fact behind N06, Clang's limit on the elements of an array in a constant evaluation, is already in `knowledge-base/cpp-language/consteval-limits-per-compiler.md`. I added no note.
