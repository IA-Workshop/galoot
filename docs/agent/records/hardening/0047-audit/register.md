# Audit register of the core headers (plan 0047, unit H0)

- Unit: H0 of [plan 0047](../../../plans/0047-core-completion-hardening-verification.md) (sections 5.1 and 7): the one audit register, made from two independent reads.
- Written by Claude (agent, Opus 5.5) in the cpp-expert role, 2026-11-21, on branch `records/hardening-audit` in the worktree `galoot-wt/hardening-audit`, from `main` at `01dce7f`. Records only: nothing outside `docs/agent/` changed.
- Inputs: the cpp read, [cpp/register.md](cpp/register.md) with its reproducers under `cpp/repro/` (branch `records/hardening-audit-cpp` at `4df8976e`), and the numerical read, [numerical/register.md](numerical/register.md) with its reproducers beside it (branch `records/hardening-audit-numerical` at `ea2847a2`). Both are merged into this branch unchanged.
- Revision audited: `main` at `01dce7f39d1cae6b65ab57349fe870aa187f5663`, whose headers are byte for byte those of `c95fee5` and of the tag 1.0.0. Every line number below is at that revision.
- Compilers: GCC 16.2.1 (20260810) and Clang 23.1.5, libstdc++ 16, x86-64. Python 3.14.10 for the cpp read's transcription; sympy 1.14.0 and mpmath in the math tools' environment for m08.

## How this register was made

1. **Both reads were rerun.** `cpp/repro/run.sh` and the numerical read's `run.sh` were run again at this revision, the second from a copy in the session scratch directory whose root pointed at this worktree, so that the recorded outputs stay as their authors left them. Every outcome reproduced. The cpp read's output differed from `cpp/repro/outputs.txt` only in where long lines are cut (the include path has another length) and in process numbers. Of the numerical read's 76 case outputs, the 37 that are not wide-algebra cases were identical once the build paths were normalized. The 39 wide-algebra cases (`p09`) differed only in timings, once rerun from inside the worktree, since `tools/timed` needs a git checkout as its working directory.
2. **Rows were matched by site and condition.** A row is one site: a place in a header and the condition that can go wrong there (plan 0047 section 5.1). Where the two reads name the same lines for different conditions, the row is split; where one read groups several functions under one ID (the numerical read's N02), each function keeps its own row and both rows cite N02.
3. **Where the reads agree**, the row carries the shared verdict, and the rerun of step 1 is its evidence.
4. **Where they disagree, where only one read reported the site, or where a site was open**, this read decided: it reran the reproducer, or wrote one under [`merge/`](merge/), or read the code again, and the row's merged verdict is this read's, with the evidence named in the notes. A disagreement on severity alone counts as a disagreement.
5. **No row is dropped and no verdict is softened.** A site both reads dismissed stays here as dismissed. Where the reads give different severities, the higher stands unless the evidence shows it rests on a wrong fact; no such case arose.

How this read's own reproducers ran: [`merge/run.sh`](merge/run.sh) calls `g++` and `clang++` directly on one to four small files each, outside the test suite and the job pool, and [`merge/outputs.txt`](merge/outputs.txt) is one run of it at this revision (about one minute of wall time on a loaded machine). No test suite was built. The job pool read "running: 0 of 23 slots free" with four clients from other sessions; it was neither started nor stopped. No timing measurement was made, so the timing lock was not taken.

### Verdicts and severity

- **Confirmed**: a reproducer shows it, or for a statement in a comment, the code read shows the statement false.
- **Dismissed**: the suspicion does not hold; the reason and what was checked are given.
- **Open**: neither shown nor ruled out; the row says what would settle it and which unit it falls to.

Severity, one scale for both reads (they differed in wording, not in substance):

- **High**: undefined behavior or a wrong result from ordinary, documented use, with no diagnostic in any mode. None was found.
- **Medium**: undefined behavior, a silent loss or a wrong result from legal but unusual input; or a guarantee or documented bound that fails for an accepted input.
- **Low**: a missing or unhelpful diagnostic; undefined behavior or a wrong value only when a stated precondition is broken in an unchecked build; a valid input refused; a documented limitation shown concretely.
- **Nit**: wording, a comment, or the count of errors only.

## Summary

48 sites: **29 confirmed, 17 dismissed, 2 open.** Of the confirmed, none is high, 5 are medium, 20 low and 4 nits.

| | Rows | Both reads | One read only | Decided by this read |
| --- | --- | --- | --- | --- |
| Confirmed | 29 | 11 | 18 | 21 |
| Dismissed | 17 | 6 | 11 | 11 |
| Open | 2 | 0 | 2 | 2 |
| All | 48 | 17 | 31 | 34 |

"Decided by this read" counts the 31 one-read rows, the 2 rows the reads split on (A03, A16: confirmed by one, open in the other) and the 1 row where they differed on severity alone (A22). Every one-read row was rerun or checked again here. The cpp read has 35 rows and the numerical read 28. 17 sites are common to both (the numerical read's N02 covers three of them), 18 are the cpp read's alone, and 13 are the numerical read's alone. Plan 0047's candidates of section 5.1 each have a row: the one-blade catch-all (A01), a mask listed twice (A12), `collect_result`'s capacity check (A13), the least `coefficient_type` (A14), shifts at 32 generators (A15), 17 to 32 generators (A21, A24), the rounding edges of `transform` (A22, A32, A33, A43 to A46) and `rotation` (A18, A37, A48), formatting above nine generators (A27), `get` of an absent blade (A28), and the configuration of one program (A03, A29, A30, A31).

## The register

Reader IDs: H0-nn is the cpp read's, Nnn, Dnn and Onn the numerical read's. "—" means the read has no row for the site.

| ID | Site | Condition | cpp read | numerical read | Merged verdict | Severity | Evidence |
| --- | --- | --- | --- | --- | --- | --- | --- |
| A01 | `mv.hpp:390-401`, `669-700` | one-blade layout over a scalar that converts from `std::int64_t` only explicitly: the compiler's error, not the library's (release exception 8) | H0-01: confirmed, low | N10: confirmed (known), low | confirmed | low | `cpp/repro/r01`, `numerical/p07` case 3; both rerun |
| A02 | `mv.hpp:399-401`, `669-679` | the same for a layout of any size given the right number of such values | H0-02: confirmed, low | — | confirmed | low | `r02` rerun: GCC 2 errors, Clang 1 with notes |
| A03 | `assert.hpp:10-18`, `115-141` | `GALOOT_CHECKING` 0 in one unit and 1 in another: link order picks the definition; a checked unit runs unchecked | H0-03: confirmed, medium | O02: open (H5) | confirmed | medium | `r03` rerun: aborts or returns 254 by link order, both compilers |
| A04 | `assert.hpp:16-18`, `119` | spellings of `GALOOT_CHECKING` the numeric `#if` lets through (`ON` gives the compiler's error; `true`, `01`, `0x1`, `1-0` build as 1) | H0-04: confirmed, nit | — | confirmed | nit | `cpp/repro/run.sh` spelling loop rerun |
| A05 | `bits.hpp:48-51`, `83-151`, `170-173` | n outside 1 to 32 at run time, unchecked: shift by 32 in `low_bits`; wrong values from `complement_mask`, `antigrade` | H0-05: confirmed, low | N02: confirmed, low | confirmed | low | `r04`, `p02` rerun; UBSan at `bits.hpp:50` |
| A06 | `blade_set.hpp:131-150` | `canonical_name` of a blade beyond e9, unchecked: the name "e" | H0-06: confirmed, low | N02: confirmed, low | confirmed | low | `r05`, `p02` rerun |
| A07 | `metric.hpp:129-132` | `is_null` past the last generator, unchecked: out-of-bounds read | H0-07: confirmed, low | N02: confirmed, low | confirmed | low | `r06`, `p02` rerun (`_GLIBCXX_ASSERTIONS` catches it) |
| A08 | `mv.hpp:102-107`, `format.hpp:245-264` | a `char` scalar prints as characters, a zero as a NUL byte | H0-08: confirmed, low | — | confirmed | low | `r07` rerun: bytes `41` and `00` |
| A09 | `blade_set.hpp:386-403` | a name list given as a `char` array without a final null: its last character is dropped, so "e12" reads as e1 | H0-09: confirmed, medium | — | confirmed | medium | `r08` rerun: holds mask 1, both compilers |
| A10 | `mv.hpp:122-133`, `1259-1338` | `get` and `has_name` on a class derived from an `mv`: no matching function, no library message | H0-10: confirmed, low | — | confirmed | low | `r09` rerun |
| A11 | `linear.hpp:340-343`, `443-478` | a scalar of another type times a derived class: the library's message is lost | H0-11: confirmed, low | — | confirmed | low | `r10` rerun |
| A12 | `plan.hpp:260-262`, `605-607`, `926-927` | a mask listed twice in one set | H0-12: dismissed | D05: dismissed (nit, what happens recorded) | dismissed | none | `r11`, `p07` case 2 rerun |
| A13 | `plan.hpp:308-327` (`:319`) | `collect_result`'s capacity check | H0-13: dismissed | D04: dismissed | dismissed | none | reading: its only caller gives it enough room |
| A14 | `blade_product.hpp:91-92`, `basis_change.hpp:266-267`, `315-316` | negating the least `coefficient_type` | H0-14: dismissed | D01: dismissed | dismissed | none | reading of every caller; `p06` rerun |
| A15 | every shift of `mask_t` at 32 generators | shift by the width | H0-15: dismissed | D02: dismissed | dismissed | none | `r17` at 32 on GCC, `p09` n = 32 rerun |
| A16 | `basis_change.hpp:403-469`, reported at `:719-721` | a change refused because the elimination alone overflows (release exception 9) | H0-16: confirmed, low | O03: open (H2) | confirmed | low | `r12` rerun: (78, 2, 1, 1) accepted, (79, 2, 1, 1) refused; `r12_elimination_sim.py` rerun, output identical |
| A17 | `mv.hpp:722-781`, `basis_change.hpp:1063-1072`, `linear.hpp:416`, `mv.hpp:113-114` | a scalar without a `numeric_limits` specialization (`_BitInt(128)`): the change of basis truncates silently | H0-17: confirmed (documented class), low | — | confirmed | low | `r13` rerun on Clang: image (0, 0) for (-1/2, 1/2) |
| A18 | `presets/rga3/motions.hpp:80-97` | `rotation` with a NaN or infinite angle: a NaN motor, no check | H0-18: confirmed, low | N05: confirmed, low | confirmed | low | `r14`, `p04` rerun |
| A19 | `presets/rga3/motions.hpp:159-176` | `translation` over `bool`: two errors where one is meant | H0-19: confirmed, nit | — | confirmed | nit | `r15` rerun |
| A20 | `dual.hpp:179-185`, `336` | empty layouts through every operation | H0-20: dismissed | — | dismissed | none | `r16` rerun with `-Werror`, both compilers |
| A21 | `blade_product.hpp:186-192`, `plan.hpp:147-153`; claim at `metric.hpp:51-55`, `limits.md:24-26` | term-list capacity 2^(\|A ∪ B\| - 1): Clang refuses antiproducts, duals and products by the antiscalar from 22 generators, though "every later part of the library accepts" up to 32 | H0-21: confirmed, medium | N06: confirmed, medium | confirmed | medium | `r17`, `p09` rerun: 21 builds, 22 refused on Clang; GCC builds to 32 |
| A22 | `transform.hpp:408-414`, `486-513`, `720-728` | the rounding check over `std::float16_t`: its floor τ = 1/8 lets a gross non-versor through at ordinary magnitudes | H0-22: confirmed (GCC), medium | N03: confirmed, low to medium | confirmed | medium | `r18`, `p03` rerun (GCC only) |
| A23 | `mv.hpp:298-369`, `linear.hpp:359-436`; claims at `mv.hpp:892-895`, `transform.hpp:764`, `basis_change.hpp:1136-1137`, `core.md:375-378` | an integer scalar's own overflow: undefined over `int`, wrapping over `std::int16_t`, while the headers call the result "exact" | H0-23: confirmed, low | N13: confirmed, low | confirmed | low | `r19`, `p01` rerun; UBSan at `mv.hpp:366` |
| A24 | `plan.hpp:308-327`; `limits.md:17-32` | placement cost above 16 generators: two vector layouts refused at 20 on Clang, 31 on GCC | H0-24: confirmed, low | — | confirmed | low | `r20` rerun: Clang 19 builds, 20 refused; GCC 30 builds, 31 refused |
| A25 | `blade_set.hpp:460-469`, `513-515` | a tab between names is reported as a bad index | H0-25: confirmed, nit | N09: confirmed, nit | confirmed | nit | `r21`, `p07` case 1 rerun |
| A26 | `blade_product.hpp:141-168` (`:162`) | a `detail` term list over its capacity at run time | H0-26: dismissed for public input | — | dismissed | none | reading: every caller is `detail`, reached from `consteval` builders |
| A27 | `format.hpp:78-129`, `220-264` | the formatter's buffers and wide algebras | H0-27: dismissed | D11: dismissed | dismissed | none | reading; `format_wide_algebra_*_fail` tests |
| A28 | `mv.hpp:1299-1338` | `get` of an absent blade | H0-28: dismissed | — | dismissed | none | ten `mv_get_*_fail` tests in `test/core` |
| A29 | `assert.hpp:20-51`, `86-90` | `GALOOT_FORCE_INLINE` 0 in one unit and 1 in another | H0-29: open (H5) | — | confirmed (documented) | low | `merge/m02`: formal ODR difference; no merged definition differs; no bit differs |
| A30 | every include guard; namespace `galoot::core` | two versions of Galoot in one program | H0-30: open (H5) | — | confirmed | low | `merge/m03`: second version skipped in one unit; link order picks across units; no diagnostic, even with `-flto -Wodr` |
| A31 | `assert.hpp:53-83` (release friction 8) | a forced-inline operation called from a function built for fewer instruction-set features | H0-31: open (H1) | — | confirmed (documented) | low | `merge/m04`: both compilers stop with forced inlining, build without |
| A32 | `transform.hpp:91-104` | floating-point options the check cannot see: `-fassociative-math`, `-freciprocal-math`, `-fno-signed-zeros`, flush-to-zero | H0-32: open (H3) | — | open (H3) | unknown | `merge/m05`: no false rejection in 200,000 versors per setting; no bound |
| A33 | `transform.hpp:587-620`; contract at `751-763` | NaN, infinity, subnormal values and overflow in the rounding check | H0-33: open (H3) | — | dismissed | none | `merge/m06`; stated on `limits.md:383-384`; pinned by `core.transform_rounding_checking_nan`, `_infinite`, `_overflow` |
| A34 | `linear.hpp:383-424` | integer division by zero, `min() / -1` | H0-34: dismissed | D12: dismissed | dismissed | none | stated on the declaration; `linear_divide_*` tests |
| A35 | `metric.hpp:244-314` (`from_gram`, `is_valid_form`, `metric_like`) | the form checks themselves: symmetry, entry range, counts | H0-35: dismissed | — | dismissed | none | 14 `metric_*_fail` tests; `p07` case 5 shows the checks hold |
| A36 | `mv.hpp:313`, `:366`; `linear.hpp:364`, `:378`; claims at `mv.hpp:745-748`, `876-878` | a narrow unsigned scalar (`std::uint16_t`) promotes to `int` in the kernels and scalings, so its "modular" arithmetic overflows `int` | — | N01: confirmed, medium | confirmed | medium | `p01` rerun; `merge/m01` shows the coefficient path at `mv.hpp:313` that N01 inferred |
| A37 | `presets/rga3/motions.hpp:81-86`; claim at `:36-37` | `rotation` loses unitization where the squares of the direction are subnormal; "any positive length" contradicts the Requires clause | — | N04: confirmed (documented in part), low | confirmed | low | `p04` rerun: 7.5e13 u at length 1e-161 |
| A38 | `blade_product.hpp:75`, `:94`, through `plan.hpp:553-556` | an overflowing product coefficient cascades into 4 errors (Clang) or 14 (GCC) | — | N07: confirmed, low | confirmed | low | `p05` case 4 rerun |
| A39 | `metric.hpp:300-314` (`metric_like`) | a hand-written metric beyond `max_form_entry` gets "constraints not satisfied", not the library's message | — | N08: confirmed, low | confirmed | low | `p07` case 5 rerun; no test covers a hand-written metric's message |
| A40 | `presets/rga3/motions.hpp:158-164` | `translation` over a scalar without division: the compiler's error inside the body | — | N11: confirmed, low | confirmed | low | `p07` case 4 rerun |
| A41 | `metric.hpp:57-62`; `limits.md:110-113` | the rationale of `max_form_entry` names a minor the code never forms in `entry_type` | — | N12: confirmed by reading, nit | confirmed | nit | reading: every use of `entry_type` in the core |
| A42 | `mv.hpp:755-781` (`holds_exactly`) | converting exact coefficients to the scalar | — | D03: dismissed | dismissed | none | `p05` cases 1, 2, 3, 5 rerun |
| A43 | `transform.hpp:457-481` | the margin inequality of the rounding check | — | D06: dismissed | dismissed | none | `merge/m08`: the expansion is exact; the inequality holds at the largest admitted K |
| A44 | `transform.hpp:422-439`, `496-513` | the counts of roundings and the padding | — | D07: dismissed | dismissed | none | reading against `accumulate` (`mv.hpp:298-321`) |
| A45 | `transform.hpp:758-770` under `-ffp-contract=fast` | contraction | — | D08: dismissed (record 0040) | dismissed | none | record 0040; `merge/m05` contraction rows |
| A46 | `transform.hpp:753-757` under a directed rounding mode | the bound assumes rounding to nearest | — | D09: dismissed as documented | dismissed | none | `merge/m05`: largest ratio 0.489 of the tolerance, upward and downward |
| A47 | `basis_change.hpp:403-469`, `507-530`, `883-927` | the exact divisions of the change of basis | — | D10: dismissed | dismissed | none | reading; `p06` case 1 rerun |
| A48 | `rga3::rotation`; `transform`'s check | no derived bound for `rotation`, no recorded worst ratio for the check | — | O01: open (H3) | open (H3) | unknown | `merge/m07`: 6.13 u in 10^6 motors; `merge/m05`: 0.241 of the tolerance |

## Notes on the rows this read decided

Rows not listed here are the ones both reads agree on; their evidence is the rerun of step 1.

- **A02, A04, A08, A09, A10, A11, A17, A19, A24 (cpp read only, confirmed).** Each reproducer was rerun and gave the recorded outcome on both compilers, except `r13` (A17), which runs on Clang only because GCC 16 has no `_BitInt` in C++. For A24, the GCC limit at 31 generators reproduced too (39 errors, the first "'constexpr' evaluation operation count exceeds limit of 33554432"). The numerical read has no row for these sites. A02, A04, A08, A10, A11 and A19 fall outside its field. A09 and A17 are silent wrong results it did not look for. A24 is a different limit from its N06: its sparse layouts of two blades never reached the placement cost. That one read missed them is no evidence against them.
- **A03 (confirmed against open).** `r03` was rerun: on both compilers, with the checked unit linked first the program aborts with "galoot: assertion failed: n must be from 1 to 32", and with the unchecked unit first the checked unit's call with n = 40 returns 254 unchecked. The numerical read left it to H5 because whether two definitions differ "is a question about the language". They differ: `checking_now` reads the macro in its body (`assert.hpp:119`), so its definition is a different sequence of tokens in the two units, which the one-definition rule forbids for an inline function ([basic.def.odr], ill-formed, no diagnostic required). The consequence is a silent loss of checks, so the cpp read's medium stands. H5 still owns the remedy and its decision record.
- **A16 (confirmed against open).** `r12` was rerun with `-DPROBE` on both compilers ("elimination fits 0, inner products fit 1 hold 1" at 79, "fits 1" at 78), and the Python transcription's output was identical to the recorded one. The numerical read did not build an input; this is one, and it is the characterization H2 asks for.
- **A20, A26, A28 (cpp read only, dismissed).** `r16` was rerun under `-Wall -Wextra -Werror` on both compilers. For A26, every use of `term_list`, `blade_product` and `generate_plan` in the headers is in `detail`, and the change of basis reaches `blade_image` only through `generate_change` from the `consteval` `make_change_map`, so no public call runs them at run time. For A28, the ten `mv_get_*_fail` sources in `test/core` cover the causes the cpp read lists.
- **A22 (severity).** Both reads confirm the site; they differ only on severity, medium against "low to medium". `r18` and `p03` were rerun on GCC: at a = 0.05 the check passes a non-versor with padding 0.125, and over `float` it aborts. A checking mode that passes a gross non-versor at ordinary magnitudes, for a type the concept admits, is a guarantee failing for accepted input, which is medium on this scale.
- **A29 (open, now confirmed as documented).** `merge/m02` builds two units that each apply the same rotor with `transform`, one with `GALOOT_FORCE_INLINE` 1 and one with 0, at `-O0`, `-O2`, and `-O2 -march=x86-64-v3` with `-ffp-contract=off` and `fast`, links them in both orders on both compilers, and compares the two units' results bit for bit on 100,000 random inputs. No bit differed in any case. At `-O0` the two objects both define 23 (GCC) or 7 (Clang) weak `galoot` functions, but none of them is a forced-inline function: those (`run_product`, `transform_middle`, `transform_outer`, `transform`) are emitted only by the free unit, since the forced unit inlines every call. So the linker never chooses between two different definitions here. The program still breaks the one-definition rule in form, because the forced-inline functions' definitions differ by the attribute's tokens. The header states the rule ("Every translation unit of a program must use the same mode"). It is low: a documented limitation, shown concretely, with no effect found.
- **A30 (open, now confirmed).** `merge/m03` uses two real versions: the headers of the tag 1.0.0 (equal to `main`'s) and of the 1.0.1 candidate, branch `feature/header-hygiene-1-0-1` at `62374f1a`, whose inline functions differ from 1.0.0's only in the names of locals. In one unit, including both versions' `assert.hpp` keeps whichever came first, without a diagnostic, on both compilers. Across units, the objects share 9 (GCC) or 8 (Clang) inline `galoot` functions at `-O0` and link without a diagnostic, also under `-O2 -flto -Wodr`. A third copy, 1.0.0 with only the message of `complement_mask`'s check edited, stands for a later version whose inline functions behave differently. With it, the message a call prints depends on link order (GCC at `-O0` and with LTO; Clang at `-O0`). Neither the headers nor the guide mention two versions in one program. It is low today, because the two real versions behave alike; a version that changes an inline function's behavior would make the mix silent and order-dependent.
- **A31 (open, now confirmed as documented).** `merge/m04` is the header's own example (`assert.hpp:72-76`): in a file built with `-march=x86-64-v3`, a function declared `[[gnu::target("arch=x86-64")]]` that calls `transform`. With forced inlining GCC stops with one error ("inlining failed in call to 'always_inline' ..."), and Clang with 18, one per missing target feature ("always_inline function 'transform' requires target feature 'movbe' ..."), at `-O0` and `-O2`. With `GALOOT_FORCE_INLINE` 0 it builds. The comment's statement holds. The pin plan 0047 assigns to H1 has nothing in `test/` yet.
- **A32 (open, still open).** `merge/m05` computes, for each of 200,000 versors (random even elements of R(3,0,0), and products a b of random vectors of R(4,0,0) rounded once), the largest ratio of a dropped coefficient to its tolerance, through the same functions `transform` calls. It does this under `-fassociative-math -fno-signed-zeros -fno-trapping-math`, `-freciprocal-math`, `-fno-signed-zeros`, and `-march=x86-64-v3 -ffp-contract=fast`, each with and without the processor's flush-to-zero and denormals-are-zero bits set, at ordinary scale and at a scale where the second product underflows. No ratio exceeded 1; the largest was 0.292 (GCC with reassociation, R(3)), against 0.241 for the baseline. One trap: with `-frounding-math` also given, Clang's results under reassociation equal its baseline exactly, so that flag suppressed the reassociation; the rows run without it show 0.265 on Clang. Why it stays open: the check's per-coefficient counts of roundings (`kernel_roundings`) follow the kernel's order of terms, and a reassociated sum can give a term with a high count more additions than that order gives it, so the derivation does not cover reassociation where terms carry different counts. That needs a metric with non-unit coefficients, which these families lack [inferred, not run]. Flush-to-zero appears covered by the padding, since τ = λ/u is exactly the padding the lower bound needs when a product's absolute error is λ rather than λu [inferred from the derivation at `transform.hpp:445-470`, not proven here]. H3 settles both with adversarial inputs and the bound.
- **A33 (open, now dismissed).** `merge/m06` in checking mode over double, both compilers: a NaN in x and an infinity in v each abort with "transform: v is not a versor on x within rounding". The limits page states that a NaN or infinite dropped coefficient, or one whose magnitude overflows, fails the check (`limits.md:383-384`), and tests pin the three cases (`core.transform_rounding_checking_nan`, `_infinite`, `_overflow`, each over double and float). x = 1.2e308 (e1 + e2), of norm 1.7e308, within 6% of the largest double, is rotated correctly with no abort. The least subnormal x is rotated with no abort, to twice the least subnormal, an absolute error of one least subnormal. The header's non-versor at scale 1e-160 passes unseen, below the floor the header states for double. All of this is as documented. One remark, not counted as a finding: for a NaN in x the message blames v; the guide quotes the message and the limits page lists its causes.
- **A35 and A39 (one site, two conditions).** The cpp read dismissed the form checks; the numerical read confirmed that a hand-written metric gets no message. Both hold. Rerunning `p07` case 5 gives Clang's "constraints not satisfied for class template 'mv'" with the note "because 'detail::is_valid_form_v<big>' evaluated to false", and no message of the library's. The `from_gram` path has its message (`metric_entry_range_fail.cpp`: "from_gram: an entry is beyond max_form_entry"). The two conditions keep two rows: the checks are right (A35, dismissed), and the hand-written path's diagnostic is not (A39, low).
- **A36 (numerical read only, confirmed and extended).** `p01` was rerun. The numerical read inferred, without running it, that `accumulate`'s path for a coefficient other than ±1 overflows the same way. `merge/m01` shows it: in diag(181, 181) over `std::uint16_t`, (65535 e12)(e12) has the coefficient -32761, which converts to 32775, and `static_cast<T>(C) * value` at `mv.hpp:313` overflows `int`. UBSan reports "signed integer overflow: 32775 * 65535" on both compilers; a constant evaluation fails on both ("value 2147909625 is outside the range of representable values of type 'int'" on Clang). The unoptimized result was the modular 32761. So the site covers four lines: `mv.hpp:313` and `:366` and `linear.hpp:364` and `:378`.
- **A37, A38, A40 (numerical read only, confirmed).** `p04`, `p05` case 4 and `p07` case 4 were rerun with identical outputs. The cpp read noted A37's subnormal case as the numerical reader's (H0-18 note), so its silence there is by design.
- **A41 (numerical read only, confirmed).** Every use of `entry_type` in the core was read: `isqrt`, comparisons, and the divisions of `reduced` (`basis_change.hpp:369-386`); no product of entries is formed in `entry_type`. The sentence at `metric.hpp:57-59` gives a reason the code does not use.
- **A42, A44, A47 (numerical read only, dismissed).** `p05` and `p06` were rerun. For A44, `kernel_roundings` (`transform.hpp:422-439`) was read against `accumulate` (`mv.hpp:298-321`): one rounding for the product, one more for a coefficient other than ±1, none for an assigned first term, one per later addition after the larger count, as the numerical read says. For A47, the elimination divides only by the previous pivot, which starts at 1, and returns at a zero pivot (`basis_change.hpp:439-447`).
- **A43 (numerical read only, dismissed).** `merge/m08` expands the polynomial with sympy: f minus the numerical read's form is 0. It then checks the inequality itself, without the Bernoulli step: exactly in rationals for K = 1, 2 and 10, and with mpmath at 60 digits at the largest K that `margin_holds` admits (67,108,863 for double, 2,895 for float, 31 for binary16). It holds in every case. `margin_holds` (`transform.hpp:528-535`) tests (K + 1)(2K + 1) < 2^digits, which is the stated condition with u = 2^-digits.
- **A45, A46 (numerical read only, dismissed).** Record `records/numerical/0040-contraction-check.md` exists and is the evidence for A45. `merge/m05` adds rows under `-march=x86-64-v3 -ffp-contract=fast` (largest ratio 0.24 on GCC, 0.241 on Clang). For A46, `merge/m05` built with `-frounding-math` and run with `fesetround` gave largest ratios of 0.467 and 0.482 upward, 0.417 and 0.489 downward, and 0.347 and 0.304 toward zero (R(3) and R(4)), about twice the 0.24 of rounding to nearest, and below 1. Random inputs only; the comment states the assumption, so the row stays dismissed as documented.
- **A48 (open, still open).** `merge/m07` ran 10^6 calls of `rotation` with direction lengths log-uniform from 1e-100 to 1e100 and angles from -100 to 100: the largest defect of the squared weight norm was 6.13 u, the same on both compilers. `merge/m05` gives the check's largest observed ratio on random versors, 0.241 (R(3)) and 0.206 (R(4)). These are measurements on random inputs, not bounds, and the adversarial campaign against high-precision ground truth is H3's.

## Confirmed defects ranked by severity

Within a severity, a silent wrong result or undefined behavior ranks above a silent loss of a check, which ranks above a refused valid input, which ranks above a diagnostic, which ranks above a documented limitation shown concretely.

| Rank | ID | Severity | What goes wrong | Kind |
| --- | --- | --- | --- | --- |
| 1 | A36 | medium | narrow unsigned scalars overflow `int` in the kernels and scalings: undefined at run time, against a stated "modular" | undefined behavior, no diagnostic at run time |
| 2 | A09 | medium | a `char` array without a final null loses its last character: e12 reads as e1 | silent wrong result |
| 3 | A03 | medium | `GALOOT_CHECKING` mixed across units: checks vanish by link order | silent loss of checks |
| 4 | A22 | medium | the float16 rounding check passes gross non-versors at ordinary magnitudes | silent loss of a check |
| 5 | A21 | medium | Clang refuses antiproducts and duals from 22 generators, where the docs say up to 32 is accepted | valid input refused, claim false |
| 6 | A08 | low | a `char` scalar prints characters and NUL bytes | silent wrong output |
| 7 | A17 | low | a scalar without `numeric_limits` truncates in the change of basis (documented class) | silent wrong result |
| 8 | A23 | low | an integer scalar's own overflow; the results are called "exact" without the condition | undefined behavior of the scalar, undocumented |
| 9 | A37 | low | `rotation` loses unitization where the squares are subnormal | silent loss of accuracy |
| 10 | A30 | low | two versions in one program mix silently, by link order | silent mixing |
| 11 | A05 | low | bit functions with n outside 1 to 32, unchecked: a shift by 32 | undefined behavior, broken precondition |
| 12 | A07 | low | `is_null` past the last generator, unchecked: out-of-bounds read | undefined behavior, broken precondition |
| 13 | A06 | low | `canonical_name` beyond e9, unchecked: "e" | wrong value, broken precondition |
| 14 | A16 | low | a change refused for the elimination's overflow alone (exception 9) | valid input refused |
| 15 | A24 | low | placement cost refuses two vector layouts at 20 generators on Clang | valid input refused |
| 16 | A18 | low | `rotation` does not check its angle | missing diagnostic |
| 17 | A01 | low | one-blade layout from an explicit-only scalar (exception 8) | compiler's diagnostic |
| 18 | A02 | low | the same for any layout | compiler's diagnostic |
| 19 | A10 | low | `get` on a derived class | compiler's diagnostic |
| 20 | A11 | low | a scalar of another type times a derived class | compiler's diagnostic |
| 21 | A39 | low | a hand-written metric out of range | compiler's diagnostic |
| 22 | A40 | low | `translation` over a scalar without division | compiler's diagnostic |
| 23 | A38 | low | an overflowing coefficient cascades into 4 to 14 errors | diagnostic count |
| 24 | A29 | low | `GALOOT_FORCE_INLINE` mixed: a formal ODR breach, no effect found | documented limitation |
| 25 | A31 | low | forced inlining into a narrower target stops the build | documented limitation |
| 26 | A04 | nit | spellings of `GALOOT_CHECKING` | diagnostic |
| 27 | A19 | nit | `translation` over `bool` gives two errors | error count |
| 28 | A25 | nit | a tab between names reported as a bad index | wording |
| 29 | A41 | nit | `max_form_entry`'s rationale | comment |

## Defects whose fix would change behavior a user may rely on (plan 0047 risk 6)

Behavior a user may rely on is an accepted input, a computed result, which programs abort in checking mode, a message the guide quotes, the answer of a `requires`-expression or a type trait, or a mangled name. A fix that changes such behavior goes to the owner with its options before H6 makes it (plan 0047 section 5.2, H6; risk 6). The options are the reads' own.

Every fix the reads name changes such behavior:

| ID | What would change |
| --- | --- |
| A04 | checking the spelling as `GALOOT_FORCE_INLINE` already is rejects `true`, `01`, `0x1` and `1-0`, which build today |
| A08 | rejecting `char` removes accepted input; printing it as a number changes output |
| A18 | a check of the angle in checking mode aborts programs that return a NaN motor today |

Only some of the named fixes change it; others do not:

| ID | Option that changes relied-on behavior | Option that does not |
| --- | --- | --- |
| A01, A02 | a library message from a constrained catch-all makes `std::is_constructible` answer true where it answers false today | a sentence on the limits page |
| A03, A29, A30 | the settings or the version made part of the entities' names (an inline namespace or an ABI tag) changes every core function's mangled name | stating the rule |
| A10 | `get` and `has_name` accepting derived classes change `requires`-expressions about them | a sentence on `get`'s comment and a pin of today's error |
| A11 | a rejecting overload for derived classes makes `requires { 2 * p; }` true for them | a pin of today's error |
| A17 | rejecting scalars without a `numeric_limits` specialization removes accepted input | documentation that names `_BitInt` |
| A21 | lowering the bound of generators to 21 removes accepted input (a major release) | a tighter capacity, or the limit on the limits page |
| A22 | excluding narrow types from the check, or a smaller τ for them, changes which programs abort in checking mode over `std::float16_t` | stating the floor for `std::float16_t` |
| A36 | rejecting narrow unsigned scalars removes accepted input | converting the operands to `unsigned int` first, which only makes defined what is undefined today |
| A37 | scaling the direction before the sum changes computed values where a square is subnormal (inferred: the same bits elsewhere) | narrowing the sentence at `motions.hpp:36-37` |
| A41 | changing `max_form_entry` changes accepted input | correcting the sentence |

A fix relies on nothing but a defect:

| ID | Note |
| --- | --- |
| A09 | a `consteval` check that the last character is a null rejects an unterminated array, which only a program relying on the dropped character would notice |

No other confirmed row's fix, as the reads name it, changes such behavior. A38's fix changes the expected error counts of its own future test (W9), and plan 0048's E1 and E2 change about 50 message literals under decision 0253 item 15, which is that plan's ruling, not a row here.

## Registered sites in code plan 0048 intends to remove or rewrite

From [plan 0048](../../../plans/0048-aggressive-simplification.md) section 5, with the function-level rows of its unit D0's candidate list (`records/simplification/candidates.tsv` on the branch `records/simplification-survey` at `a4c18f1`, not yet in `main`), read as a cross-reference only. For every site here, a test that H1 to H4 add attaches at the public boundary only (plan 0047 section 9.2 items 2 and 3).

Sites whose own lines a candidate removes or rewrites:

| ID | Verdict | Site lines | Plan 0048 candidate |
| --- | --- | --- | --- |
| A36 | confirmed, medium | `mv.hpp:366` (`run_product`); `linear.hpp:364`, `:378` (`s * x`, `x * s`) | B8 merges the runners (`mv.hpp:346-369`); B7 rewrites the scalings (`linear.hpp:359-381`). `mv.hpp:313` (`accumulate`) is kept by section 5.5 item 1 |
| A23 | confirmed, low | `mv.hpp:298-369` (`run_map`, `run_product`); `linear.hpp:359-436` | B8; B7 (`linear.hpp:359-367`, `373-381`, `412-424`, `429-436`). `accumulate` and `zero_places` are kept |
| A17 | confirmed, low | `linear.hpp:416` (`operator/`) | B7 rewrites `operator/` (`linear.hpp:412-424`; its division-by-zero assertion stays). E2 moves the callers of `divides_exactly` and `holds_exactly` (`change_basis`, `change_basis_back`, `apply`) |
| A38 | confirmed, low | `blade_product.hpp:75`, `:94` (`add_checked`, `multiply_checked`) | B5 makes them wrappers of one recording form; D0 notes that at run time without checking the wrapper would return 0 where an overflow is undefined today |
| A14 | dismissed | `blade_product.hpp:91-92` (`multiply_checked`); `basis_change.hpp:266-267` (`multiply_noting`) | B5 (wrap; move). `change_gcd` (`:315-316`) is not a candidate |
| A34 | dismissed | `linear.hpp:383-424` | B7 rewrites `operator/` (`:412-424`) |
| A20 | dismissed | `dual.hpp:179-185`, `336` (`make_scalar_part_layout`, `dot_layout`) | B3 rewrites `make_scalar_part_layout` (`dual.hpp:178-185`); C1b rewrites `dual.hpp:109-212` if its spike passes and the owner agrees |
| A47 | dismissed | `basis_change.hpp:883-927` | B2 merges `typed_change`'s kernel loop (`:904-913`) into the one builder |
| A12 | dismissed | `plan.hpp:926-927` (the comment of `unary_map`) | C1c rewrites the unary path (`plan.hpp:644-945`) if C0 passes; B1 keeps `unary_map` with new term types |

Sites whose own lines stay but which are reached only through public operations that a candidate rewrites, so their tests attach at the same public boundary:

| ID | Verdict | Reached through |
| --- | --- | --- |
| A21 | confirmed, medium | `bulk`, `weight`, the duals and the antiproducts: E2 (`dual.hpp:377-509`, `product.hpp:347-424`), C1b if it proceeds. The capacity itself (`blade_product.hpp:186-192`, `plan.hpp:147-153`) is no candidate; section 5.5 item 4 keeps a capacity per pair |
| A22 | confirmed, medium | `transform`: E2 rewrites it (`transform.hpp:771-819`); B3 and C1c rewrite its two kernel builders (`:271-362`). `padded_magnitude` is kept by section 5.5 item 10 |
| A11 | confirmed, low | the scalings, whose bodies B7 rewrites beside the rejections at `linear.hpp:443-478`, which are not candidates |
| A16 | confirmed, low | `change_basis`: B4 and E2 merge its bodies; B5 moves the checked arithmetic the elimination calls. The elimination is kept by section 5.5 item 9 |
| A42 | dismissed | `apply` and the products, where E2 makes the exactness check once |
| A44 | dismissed | the first and second kernels of `transform`, rewritten by B3 and C1c, whose terms `kernel_roundings` counts |

The comment units of plan 0048 (section 5.4, D1 to D4) rewrite the comments on the declarations of A05, A06, A07, A23 and A41, whose remedies are sentences. Decision 0253 item 14 keeps a precondition whose breach is undefined behavior in an unchecked build on its declaration in full (A05, A07).

## What the register implies for the size of H6

Plan 0047 section 5.2 makes H6 one subunit per confirmed defect, each starting from its failing test, and budgets 0.40M to 0.90M tokens for H6 with its reviews (section 11), "unknown until H0 reports". The counts:

- **29 confirmed rows**: 5 medium, 20 low, 4 nits.
- By the remedy the reads name, the 29 fall into six classes, each row in one:

  | Class | Rows | Count |
  | --- | --- | --- |
  | Only a change under `include/` is named | A04, A08, A09, A18, A19, A36, A38, A40 | 8 |
  | A change under `include/` or a sentence or pin instead | A01, A02, A10, A11, A17, A21, A22, A37, A39 | 9 |
  | A sentence in a header comment or the guide only | A05, A06, A07, A23, A41 | 5 |
  | Decided by H5's decision record, whose own unit changes no header | A03, A29, A30 | 3 |
  | A fix already deferred by a ruling: decision 0236 item 2 (A16), owner item O3 (A24) | A16, A24 | 2 |
  | A test only, no fix named | A25, A31 | 2 |

- So **at most 17 rows** (the first two classes) can each become an H6 subunit that changes a header, and **8** of them (the first class) have no alternative named. A01 is among the 17: plan 0047 section 4 already assigns its fix to H6 and its compile-fail test to H1.
- Of those 17, **3** need the owner's ruling whatever the fix (A04, A08, A18). **9** need it if the fix chosen is the one that changes relied-on behavior (A01, A02, A10, A11, A17, A21, A22, A36, A37). **5** need none (A09, whose fix relies only on a defect, and A19, A38, A39, A40). Outside the 17, A03, A29, A30 and A41 can also reach the owner through an option that changes relied-on behavior.
- Of the 17, **6** are in or reached through code that plan 0048 removes or rewrites (A17, A36 and A38 by their own lines; A11, A21 and A22 through the public operations they reach). Over all 29, **8** confirmed rows are in that code: 4 by their own lines (A17, A23, A36, A38, all in B5, B7 or B8) and 4 through the public operations they reach (A11, A16, A21, A22).
- **2 sites are open** (A32, A48), both H3's, and either can still add a confirmed row.
- **17 sites are dismissed** and need no fix. Plan 0047 section 3 still asks for a boundary test at each integer and rounding site in this register (criterion 6; among them A14, A15 and A42 to A47) and for compile-fail tests (criterion 2), so a dismissed row is not a row without tests.

## Not verified here

- Neither read's reproducers were run on any other compiler, library or target. Clang has no `std::float16_t` here and GCC has no `_BitInt` in C++, so A22 is shown on GCC only and A17 on Clang only.
- The open rows A32 and A48 have measurements on random inputs, not bounds. No adversarial input was searched for.
- `long double`, `std::bfloat16_t` (beyond what the headers say), and scalar types from outside the standard library were not probed.
- The 32-generator compiles of `r17` and `r20` and the per-operation checks at 22 generators that the cpp read ran by hand outside `run.sh` were not rerun. The `run.sh` cases at 21, 22, 19, 20, 30 and 31 were.
- Plan 0048's D0 list is on an unmerged branch and was read as a cross-reference; a later revision of it can move rows between the two overlap tables.
- The lines that plan 0044 changes in `assert.hpp`, `basis_change.hpp`, `bits.hpp`, `metric.hpp` and `motions.hpp` (the 1.0.1 candidate) were read only for A30. As the cpp read notes, plan 0047 section 8 asks for those rows to be read again once 1.0.1 is tagged, because their line numbers will move.

## This read's reproducers

Under [`merge/`](merge/), run by [`merge/run.sh`](merge/run.sh) `<galoot repository> <scratch dir>`, output in [`merge/outputs.txt`](merge/outputs.txt):

| File | Row | What it does |
| --- | --- | --- |
| `m01_uint16_coefficient.cpp` | A36 | the coefficient path of `accumulate` over `std::uint16_t`, at run time under UBSan and in a constant evaluation |
| `m02_force_inline_mixed/` | A29 | two units with different `GALOOT_FORCE_INLINE`, four optimization settings, both link orders, bitwise comparison and shared weak symbols |
| `m03_two_versions/` | A30 | the 1.0.0 headers, the 1.0.1 candidate's and an edited copy, in one unit and across units, with and without LTO |
| `m04_target_mismatch_fail.cpp` | A31 | the header's example of forced inlining into a narrower target |
| `m05_check_environment.cpp`, `m05_run.sh` | A32, A45, A46, A48 | the check's worst ratio on random versors under reassociation, reciprocal math, flush-to-zero, contraction and directed rounding |
| `m06_check_special_values.cpp` | A33 | NaN, infinity, near-overflow, subnormal and a tiny non-versor through `transform` in checking mode |
| `m07_rotation_campaign.cpp` | A48 | 10^6 calls of `rga3::rotation` and the defect of their weight norm |
| `m08_margin_identity.py` | A43 | the margin inequality, symbolically and at the largest admitted K |
