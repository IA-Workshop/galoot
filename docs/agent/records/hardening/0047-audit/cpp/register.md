# Audit register of the core headers: the C++ reader

- Unit: H0 of plan 0047 (sections 5.1 and 7), one of two independent reads. This is the cpp-expert's; the numerical-expert's register is separate, and the coordinator merges the two. This read did not look at the other one.
- Author: Claude (agent, Opus 5.5), in the cpp-expert role, 2026-11-21.
- Revision read: `main` at `01dce7f39d1cae6b65ab57349fe870aa187f5663`. The headers there are byte for byte those of `c95fee5`, the revision plan 0047 names (`git diff --stat c95fee5 01dce7f -- include/ docs/guide/` is empty). Every line number below is at that revision.
- Scope: every header under `include/galoot/`: the fourteen core headers, `core.hpp`, and the rga3 preset (`rga3.hpp`, `rga3/objects.hpp`, `rga3/motions.hpp`), all 8,410 lines read in full. The guide (`docs/guide/core.md`, `limits.md`) and the release record were read only where a site's stated contract lives.
- Branch `records/hardening-audit-cpp`, worktree `galoot-wt/hardening-audit-cpp`. Records only: nothing under `include/`, `test/` or `tools/` changed.

## How to read this register

Each row is one site: a place where input a user can supply (a signature, a blade set, a name, a scalar type and its values, a change of basis, a macro setting, a size at a documented bound) can reach undefined behavior, a wrong result, a silent loss, an unhelpful or missing diagnostic, an integer or rounding edge, or a stated contract that the code does not keep.

Verdicts:

- **confirmed**: a reproducer under `repro/` was compiled or run on GCC 16.2.1 and Clang 23.1.5 (libstdc++ 16), and `repro/outputs.txt` holds what it gave.
- **dismissed**: the suspicion does not hold; the reason and what was checked are given.
- **open**: neither shown nor ruled out here; the row says what would settle it and which unit of plan 0047 it falls to.

Severity, a scale of this register's own:

- **high**: undefined behavior or a wrong result from ordinary, documented use, with no diagnostic in any mode. None was found.
- **medium**: a silent loss or a wrong result from legal but unusual input, or a contract the header states that fails at a documented bound.
- **low**: a missing or unhelpful diagnostic; undefined behavior or a wrong result only when a stated precondition is broken in an unchecked build; a documented limitation shown concretely.
- **nit**: wording or the count of errors only.

"Fix changes relied-on behavior" says whether a fix would change something a user may rely on today: an accepted input, a result, a message the guide quotes, or the answer of a `requires`-expression or a type trait.

How the reproducers ran: `repro/run.sh <include dir> <scratch dir>` compiles each file with `-std=c++23` and the flags its line names, by calling `g++` and `clang++` directly (small probes, outside the test suite and the job pool), and runs the programs; `repro/outputs.txt` is its output at this revision. `repro/r03_checking_mixed/run.sh` builds three units and links them in both orders. The wide-algebra compiles of `r17` were run once more through `tools/timed --log repro/r17_runs.tsv` under `memguard.sh` with an 8 GiB cap; their rows are in `repro/r17_runs.tsv`. Each reproducer is a single small compile on one processor; the 30- and 31-generator compiles of `r20` on GCC are the longest, and were not timed. The 32-generator compiles of `r17` and `r20`, and the per-operation checks at 22 generators in H0-21, were run by hand with the same flags and are not in `run.sh`.

## What constant evaluation already checks

Undefined behavior of the core language in a constant evaluation makes the evaluation fail, so for every plan, map, layout and change check that a program instantiates, the compilers have already ruled out out-of-bounds access, signed overflow and bad shifts on the inputs that program uses. Through the public names, all of these run only in constant evaluation: the plan generator (`plan.hpp`), the blade product (`blade_product.hpp`), every blade-set builder (`blade_set.hpp`), the metric checks (`metric.hpp`), the change-of-basis checks and term generation (`basis_change.hpp`), the kernels' construction (`make_*_kernel`), and the names of `format.hpp`. Their defects therefore show as compile errors, never as run-time behavior; what remains is whether the error is the library's message or the compiler's, and whether a valid input is refused.

What runs at run time, and is not covered by that argument:

- the kernels that apply a plan (`run_map`, `run_product`, `run_sum`, `run_change`, `mv.hpp:346-369`, `linear.hpp:154-167`, `basis_change.hpp:1021-1028`): every index is a template argument checked when the kernel was generated, so the only run-time faults are those of the scalar's own arithmetic (H0-23);
- the scalings and the negation (`linear.hpp:359-436`), the checks of `truncate` and `transform`, and `format.hpp`'s `write`;
- the public functions of `bits.hpp`, `canonical_name`, and `metric::is_null`, when a program calls them with values known only at run time (H0-05 to H0-07);
- the rga3 preset's `rotation`, which is not `constexpr` (H0-18).

A program can also call the `detail` generators at run time; nothing public does. Those calls are plan 0047's unit H4 and are outside this register except where noted (H0-26).

## Summary

35 sites: 20 confirmed, 10 dismissed, 5 open. Of the confirmed, 4 are medium (H0-03, H0-09, H0-21, H0-22), 13 low and 3 nits; none is high. Three were known before this read (H0-01, H0-16, H0-17, each stated in the release record or a header), and the rest are new to the records as far as this read found.

| ID | Site (at `01dce7f`) | Class | Verdict | Severity | Fix changes relied-on behavior |
| --- | --- | --- | --- | --- | --- |
| H0-01 | `mv.hpp:390-401`, `669-700` | diagnostic | confirmed (known, release exception 8) | low | possibly: `std::is_constructible` answers |
| H0-02 | `mv.hpp:399-401`, `669-679` | diagnostic | confirmed (new: exception 8 for every layout) | low | possibly, as H0-01 |
| H0-03 | `assert.hpp:10-18`, `115-141` | configuration, silent loss | confirmed | medium | no, for the rule; yes, mangled names, for an inline namespace |
| H0-04 | `assert.hpp:16-18`, `119` | configuration, diagnostic | confirmed | nit | yes: `true`, `01`, `0x1`, `1-0` are accepted today |
| H0-05 | `bits.hpp:48-51`, `83-151`, `170-173` | undefined behavior (unchecked build) | confirmed | low | no |
| H0-06 | `blade_set.hpp:131-150` | wrong result (unchecked build) | confirmed | low | no |
| H0-07 | `metric.hpp:129-132` | undefined behavior (unchecked build) | confirmed | low | no |
| H0-08 | `mv.hpp:102-107`, `format.hpp:245-264` | silent wrong output | confirmed | low | yes, if char is rejected or printed as a number |
| H0-09 | `blade_set.hpp:386-403` | silent wrong result | confirmed | medium | only for programs that rely on the dropped character |
| H0-10 | `mv.hpp:1259-1338` | diagnostic, missing capability | confirmed | low | yes, if get accepts derived classes: `requires` answers change |
| H0-11 | `linear.hpp:340-343`, `443-478` | diagnostic | confirmed | low | `requires` answers change |
| H0-12 | `plan.hpp:260-262`, `605-607`, `926-927` | repeated mask | dismissed | none | n/a |
| H0-13 | `plan.hpp:308-327` (`:319`) | capacity check wording | dismissed | none | n/a |
| H0-14 | `blade_product.hpp:91-92`, `basis_change.hpp:266-267`, `315-316` | integer edge (least value) | dismissed | none | n/a |
| H0-15 | shifts of `mask_t` at 32 generators | integer edge | dismissed | none | n/a |
| H0-16 | `basis_change.hpp:403-469` | integer edge, valid input refused | confirmed (known, release exception 9) | low | no: accepts more input |
| H0-17 | `mv.hpp:722-781`, `basis_change.hpp:1069-1072`, `linear.hpp:416` | scalar without `numeric_limits` | confirmed (documented class) | low | yes, if such scalars are rejected |
| H0-18 | `motions.hpp:80-97` | rounding edge, unchecked input | confirmed | low | yes, if a check is added in checking mode |
| H0-19 | `motions.hpp:159-176` | diagnostic | confirmed | nit | no |
| H0-20 | empty layouts (`dual.hpp:179-185`, `336`) | edge | dismissed | none | n/a |
| H0-21 | `blade_product.hpp:186-192`, `plan.hpp:147-153`; claim at `metric.hpp:51-55` | contract not kept at a documented bound | confirmed | medium | no: accepts more input |
| H0-22 | `transform.hpp:408-414`, `496-513` | rounding edge, silent loss of a check | confirmed (GCC) | medium | yes: which programs abort in checking mode |
| H0-23 | `mv.hpp:298-369`, `linear.hpp:359-436`; guide `core.md:375-378` | undefined behavior (scalar overflow), undocumented | confirmed | low | no, for a sentence in the guide |
| H0-24 | `plan.hpp:308-327`; `limits.md:17-32` | compile-time limit above 16 generators | confirmed | low | no |
| H0-25 | `blade_set.hpp:460-469`, `513-515` | diagnostic | confirmed | nit | no |
| H0-26 | `blade_product.hpp:141-168` (`:162`) | run-time capacity of a `detail` list | dismissed for public input | none | n/a |
| H0-27 | `format.hpp:88-93`, `220-224` | bounds, wide algebra | dismissed | none | n/a |
| H0-28 | `mv.hpp:1299-1338` | `get` of an absent blade | dismissed | none | n/a |
| H0-29 | `assert.hpp:20-51`, `86-90` | configuration: `GALOOT_FORCE_INLINE` across units | open (H5) | low | depends on H5's option |
| H0-30 | every header's include guard and `galoot::core` | configuration: two versions in one program | open (H5) | unknown | depends on H5's option |
| H0-31 | `assert.hpp:77-83` (release friction 8) | forced inlining into a narrower target | open (H1) | low | no |
| H0-32 | `transform.hpp:91-104` | floating-point options the check does not see | open (H3) | unknown | depends |
| H0-33 | `transform.hpp:587-620`, `751-763` | NaN, infinity, subnormal, overflow in the rounding check | open (H3) | unknown | depends |
| H0-34 | `linear.hpp:383-424` | integer division by zero, `min() / -1` | dismissed (stated and tested) | none | n/a |
| H0-35 | `metric.hpp:244-262`, `264-314` | form entries and symmetry | dismissed (checked and tested) | none | n/a |

## The sites

### H0-01 One-blade layout over a scalar with an explicit constructor (release exception 8)

- Site: `mv.hpp:390-401`, the storage constructor and its comment; the catch-alls at `mv.hpp:669-700`.
- Class: error path, diagnostic not in the terms of the algebra (C9).
- Smallest input: a scalar `Exact` constructible from `std::int64_t` only explicitly, and `mv<rga3, blades<rga3, "1">, Exact> s{1};` (`repro/r01_one_blade_explicit_fail.cpp`).
- What a user sees: GCC, two errors, "no matching function for call to `mv<rga3, blade_set<1>{...}, Exact>::mv(<brace-enclosed initializer list>)`" and "could not convert `{1}`"; Clang, one error, "no matching constructor", with eight candidate notes, among them "no known conversion from 'int' to 'Exact'".
- Verdict: confirmed, as the release record's exception 8 and the comment at `mv.hpp:390-398` state.
- Pinned by: none. The nearest test, `core.mv_rejects_one_explicit_value` (`test/core/mv_one_explicit_value_fail.cpp`), is a six-blade line given one value, which the count catch-all reports ("1 == 6").
- Proposed test (H1): a compile-fail test of this input on both compilers, pinning the error count and the line, so that a later library message is a visible change.
- Fix changes relied-on behavior: possibly. The header's own reason for having no catch-all is that one would make `std::is_constructible_v<Scalar, int>` true; a user's generic code may test it.

### H0-02 The same for a layout of any size

- Site: `mv.hpp:399-401` with the count catch-all `mv.hpp:669-679`, which steps aside when the count is right.
- Class: diagnostic.
- Smallest input: the same `Exact`, `mv<rga3, blades<rga3, "e1 e2 e3 e4">, Exact> p{1, 2, 3, 4};` (`repro/r02_many_blades_explicit_fail.cpp`).
- What a user sees: the same two GCC errors ("could not convert `{1, 2, 3, 4}`") and Clang's "no matching constructor" with notes.
- Verdict: confirmed. Exception 8 and the comment at `mv.hpp:390-398` speak of one-blade layouts only; the case with the right number of values of an explicitly convertible type is the same gap for every layout. Clang's note names the conversion; GCC's text does not.
- Pinned by: none. Proposed test (H1): as H0-01, for a four-blade layout.
- Fix changes relied-on behavior: as H0-01. A message here could come from a constrained overload that takes `Values&&...` when the count matches and some value is not implicitly convertible to `T`; that overload would also change `std::is_constructible` answers.

### H0-03 `GALOOT_CHECKING` set differently in two units of one program

- Site: `assert.hpp:10-18` (the default and the check of the value), `assert.hpp:115-120` (`checking_now`, whose body reads the macro), `assert.hpp:137-141` (`GALOOT_ASSERT`). Every inline function with a `GALOOT_ASSERT` therefore has a definition that depends on the macro.
- Class: configuration; one-definition rule; silent loss of checks.
- Smallest input: `repro/r03_checking_mixed/`: `checked.cpp` built with `-DGALOOT_CHECKING=1` and `unchecked.cpp` with `0`, each calling `complement_mask`, linked with `main.cpp`, at `-O0` so that the inline function stays out of line.
- What a user sees: with the checked unit linked first, the program aborts with "galoot: assertion failed: n must be from 1 to 32"; with the unchecked unit first, the checked unit's call with n = 40 returns 254 with no check, on both compilers. Which definition survives depends on the link order. At `-O2` the calls may be inlined and each unit keep its own behavior, so the outcome also depends on optimization.
- Verdict: confirmed. The program violates the one-definition rule (ill-formed, no diagnostic required). The guide states the rule for `GALOOT_FORCE_INLINE` (`core.md:1051-1053`, README) and for nothing else; for `GALOOT_CHECKING` neither the header nor the guide says that every unit must use one value, though the consequence is the same.
- Pinned by: none. This is the question of unit H5; the reproducer is offered to it.
- Fix changes relied-on behavior: stating the rule changes nothing. Making the mode part of the entities' names (an inline namespace or an ABI tag) changes the mangled names of every core function, which matters only to objects built against an earlier version; source code is unaffected.

### H0-04 Spellings of `GALOOT_CHECKING` that the check lets through

- Site: `assert.hpp:16-18` (a numeric `#if`) and `assert.hpp:119` (`return GALOOT_CHECKING == 1;`).
- Class: configuration; diagnostic.
- Smallest input: `-DGALOOT_CHECKING=ON`, and separately `-DGALOOT_CHECKING=true` (outputs at the end of `repro/outputs.txt`).
- What a user sees: `ON` passes the `#if` (an unknown name reads as 0) and then fails in C++ with "'ON' was not declared in this scope" (GCC) or "use of undeclared identifier 'ON'" (Clang), not "GALOOT_CHECKING must be 0 or 1". `true`, `01`, `0x1` and `1-0` build and check, as 1 would; `false` builds unchecked. An empty value gives the preprocessor's own error.
- Verdict: confirmed. The comment beside `GALOOT_FORCE_INLINE` (`assert.hpp:39-41`) names exactly this weakness of a numeric `#if` and avoids it by pasting; `GALOOT_CHECKING` keeps the weak form. Harmless in effect (every spelling accepted means what it says), but the message promised by `core.assert_checking_value` comes only for some wrong values.
- Pinned by: `core.assert_checking_value` (the value 2 only).
- Proposed test (H1): compile-fail tests for `ON` and an empty value, pinning what they give today.
- Fix changes relied-on behavior: yes. Checking the spelling as `GALOOT_FORCE_INLINE` does would reject `true`, `01` and `0x1`, which build today.

### H0-05 The public functions of `bits.hpp` with n outside 1 to 32, unchecked

- Site: `bits.hpp:48-51` (`low_bits`, a right shift by `mask_bits - n`), reached from `antigrade` (`:83-88`), `complement_mask` (`:119-124`), `right_complement_sign` and `left_complement_sign` (`:133-151`), and `antireverse_sign` (`:170-173`).
- Class: undefined behavior when a stated precondition is broken in an unchecked build.
- Smallest input: `complement_mask(1, 0)` with n known only at run time and `GALOOT_CHECKING` 0, the default (`repro/r04_bits_unchecked_ub.cpp`).
- What a user sees: nothing, without a sanitizer; `complement_mask(1, 0)` returns 4294967294 and `complement_mask(1, 40)` 254, and `antigrade(1, 40)` returns 39. Under `-fsanitize=undefined`: "shift exponent 32 is too large for 32-bit type" at `bits.hpp:50`. In a checking build each aborts with its message (tested by `core.bits_rejects_*`).
- Verdict: confirmed. Each declaration says its precondition is "checked by GALOOT_ASSERT"; none says that in an unchecked build a violation is undefined. The guide says so in general ("a violated precondition is undefined behavior or a wrong result, as each function's comment says", `core.md:980-982`), but these comments do not say which. Decision 0253 item 14 keeps such a precondition "on the declaration in full".
- Pinned by: `core.bits_rejects_*` (checking build). Proposed: none beyond a sentence on each declaration; a test of undefined behavior cannot be written portably.
- Fix changes relied-on behavior: no, for the sentence. Checking always would cost a branch in the unchecked build.

### H0-06 `canonical_name` of a blade beyond e9, unchecked

- Site: `blade_set.hpp:131-150`.
- Class: wrong result when a stated precondition is broken in an unchecked build.
- Smallest input: `canonical_name(0x200)` (e10) at run time, unchecked (`repro/r05_canonical_name_unchecked.cpp`).
- What a user sees: the name "e", which is not a name; e1 e10 gives "e1". No undefined behavior: the loop reads only nine bits.
- Verdict: confirmed. Stated precondition, checked by `GALOOT_ASSERT`; the consequence when unchecked is not stated.
- Pinned by: `blade_set_name_assert_fail` (checking build). Fix changes relied-on behavior: no.

### H0-07 `metric::is_null` with an index past the last generator, unchecked

- Site: `metric.hpp:129-132`.
- Class: undefined behavior (out-of-bounds read) when a stated precondition is broken in an unchecked build.
- Smallest input: `rigid<3>().is_null(4)` with the index known at run time (`repro/r06_is_null_unchecked.cpp`).
- What a user sees: an arbitrary answer; with `-D_GLIBCXX_ASSERTIONS`, libstdc++'s "Assertion '__n < this->size()' failed" and an abort. AddressSanitizer did not report this read in the probe (the object was not instrumented as a separate stack object), so that sanitizer is not a reliable witness here.
- Verdict: confirmed. As H0-05: the declaration states the check, not the consequence.
- Pinned by: `core.metric_is_null` (checking build). Fix changes relied-on behavior: no.

### H0-08 A multivector over `char` prints characters

- Site: `mv.hpp:102-107` (`ring_scalar` admits `char`: it is semiregular, its arithmetic converts back, and it is constructible from `std::int64_t`); `format.hpp:245-264`, which prints each coefficient with `std::formatter<char, char>`.
- Class: silent wrong output.
- Smallest input: `mv<r2, blades<r2, "e1 e2">, char>{65, 0}` formatted with `"{}"` (`repro/r07_char_scalar_format.cpp`).
- What a user sees: `{e1: A, e2: ` followed by a NUL byte and `}` (bytes in `outputs.txt`). `signed char` and `unsigned char` print as numbers, since the standard formats them as integers.
- Verdict: confirmed. Nothing in the header or the guide says that a `char` scalar prints as text, and a zero coefficient writes a NUL into the output.
- Pinned by: none. Proposed test (H1 or the format tests): pin today's output, or the rejection if one is added.
- Fix changes relied-on behavior: yes, either way: rejecting `char` as a scalar removes accepted input, and printing it as a number changes output.

### H0-09 A name list given as a char array without a terminating null

- Site: `blade_set.hpp:386-403`, `name_list`, whose `text()` is the first `L - 1` characters, on the assumption that the last is a string literal's null.
- Class: silent wrong result.
- Smallest input: `constexpr char names[] = {'e', '1', '2'};` and `blades<rga3, names>` (`repro/r08_unterminated_names.cpp`).
- What a user sees: nothing. The set holds e1 (mask 1), not e12. `has_name<Point, names>` is true and `get<names>(p)` reads e1, since they parse the name the same way. An algebra tag whose `names` member is such an array is different: it converts to `std::string_view` by a search for the null, which reads past the array, and the constant evaluation fails on both compilers.
- Verdict: confirmed. The comment says the argument is "a string literal"; the constructor accepts any `const char[L]` constant.
- Pinned by: none. Proposed test (H1): a compile-fail test that a list whose last character is not a null is rejected, once the rejection exists; until then, a test pinning today's reading would pin a defect, so none is proposed before a ruling.
- Fix changes relied-on behavior: only for a program that relies on the dropped character, which would be relying on a defect. A `consteval` check that `text[L - 1]` is the null, with a message, is the direct form.

### H0-10 `get` and `has_name` on a class derived from a multivector

- Site: `mv.hpp:122-133` (`multivector` is true for `mv` itself only), `mv.hpp:1259-1262` (`has_name` requires it), `mv.hpp:1285-1338` (both overloads of `get` require it).
- Class: diagnostic; a capability the other operations have.
- Smallest input: `struct NamedPoint : Point { using Point::Point; };` and `get<"e1">(p)` for a `const NamedPoint& p` (`repro/r09_get_derived_fail.cpp`).
- What a user sees: "no matching function for call to `get<"e1">(const NamedPoint&)`" (GCC) or "no matching function for call to 'get'" (Clang), with no library message. `get<"e1">(static_cast<const Point&>(p))` works.
- Verdict: confirmed. The products, sums, scalings, the unary operations and `transform` accept such a class as its mv base (their headers say so, and `repro` checked `2.0 * p` and `p ^ q`); `truncate`, `part` and the formatter document that they do not. `get` says neither.
- Pinned by: none. Proposed test (H1): a compile-fail test pinning today's error, plus a sentence in `get`'s comment.
- Fix changes relied-on behavior: if `get` is made to accept derived classes, `has_name` and `requires`-expressions about `get` answer differently for them.

### H0-11 A scalar of another type times a class derived from a multivector

- Site: `linear.hpp:340-343` (`scalar_of_another_type` requires `multivector<X>`), the rejections at `linear.hpp:443-478`.
- Class: diagnostic.
- Smallest input: `2 * p` for `p` of a class derived from an mv over `double` (`repro/r10_scale_derived_fail.cpp`).
- What a user sees: "no match for 'operator*' (operand types are 'int' and 'const NamedPoint')" (GCC), "invalid operands to binary expression" (Clang). For an mv itself the library says "s * a: the scalar is not of the multivector's scalar type ...; convert it to that type first, as 2.0 for 2 over double" (tested by `linear_scale_left_int_fail`).
- Verdict: confirmed. `linear.hpp:51-52` says a derived class "is accepted as its mv base"; it is, with the right scalar (`2.0 * p` builds), but the misuse message is lost. The sums do not have this gap: their rejection of a number takes the multivector as `const mv<Alg, B, T>&`, which a derived class binds to (tested by `linear_plus_number_derived_fail` and `linear_minus_number_derived_fail`). Only the scalings require `multivector<X>` of the operand itself.
- Proposed test (H1): pin today's error. Fix changes relied-on behavior: a rejecting overload for derived classes would make `requires { 2 * p; }` true for them.

### H0-12 A mask listed twice in one blade set (plan 0047's candidate)

- Site: `plan.hpp:260-262`, `605-607`, `926-927`: the generator does not reject a repeated mask.
- Verdict: dismissed. Through the public interface a repeated mask reaches the generator only through the layout variables, which take a raw set (`geometric_product_layout`, `antiwedge_layout`, `right_complement_layout`, `bulk_layout`, and the others). `repro/r11_repeated_mask_layouts.cpp` checks, on both compilers, that each of four of them gives the same layout for a set with e1 listed twice (with both signs) as for e1 once, as it must, since a result layout is the set of blades the terms reach. A multivector on such a set is rejected with "mv: the layout holds one blade twice" (tested by `mv_repeated_*_fail`), so no value on a repeated layout exists to apply a plan to. `merge` and `sum_layout` reject or merge repeats themselves; `index_of` documents that it checks only the mask it looks up.
- Proposed test: none needed for the interface; if H1 wants the documented sentence pinned, `r11` is the test.

### H0-13 `collect_result`'s capacity check (plan 0047's candidate)

- Site: `plan.hpp:308-327`, the `GALOOT_ASSERT` at `:319`.
- Verdict: dismissed. Its only caller in the library, `collected` (`plan.hpp:510-516`), gives it as many places as the plan has terms, which is always enough, so no public input reaches the check. In a constant evaluation `checking_now()` is true whatever `GALOOT_CHECKING` is (`assert.hpp:115-120`), so if it were reached the message would show in either mode; `core.plan_result_room` shows it by calling the function directly. The misleading GCC wording that decision 0227 item 3 recorded belongs to a plan over GCC's operation limit, where GCC may report libstdc++'s `std::span` check as "not a constant expression" though every index is in range; the limits page states that (`limits.md:204-209`), and it is a limit, not this check.

### H0-14 Magnitudes of the least `coefficient_type` (plan 0047's candidate)

- Site: `basis_change.hpp:266-267` (`multiply_noting`), `blade_product.hpp:91-92` (`multiply_checked`), `basis_change.hpp:315-316` (`change_gcd`), and `transform.hpp:507`.
- Verdict: dismissed: `x < 0 ? -x : x` would overflow for -2^63, but no caller can pass it. Every value these functions see is one of: an `entry_type` (`int`) widened; a sign of +1 or -1; a result of `add_checked`, `add_noting`, `multiply_checked` or `multiply_noting`, each kept within -max to max (they return 0, or fail, rather than leave it); the negation of such a result; a quotient of such a result by a nonzero integer; or a product of a sign with such a result. Each of these lies within -max to max. Checked by reading every call site in `blade_product.hpp`, `plan.hpp`, `basis_change.hpp`, `mv.hpp` and `transform.hpp`. `multiply_checked` states the precondition; `multiply_noting` and `change_gcd` do not, which matters only to a future caller.

### H0-15 Shifts of a mask at 32 generators (plan 0047's candidate)

- Verdict: dismissed. Every shift was read: `low_bits` shifts right by `mask_bits - n`, defined for 1 to 32 (`bits.hpp:48-51`); `blade_product` guards its range test with `if constexpr (N < mask_bits)` (`blade_product.hpp:283-289`); every left shift (`bits.hpp:107`, `blade_product.hpp:200`, `221`, `256`, `blade_set.hpp:194`, `446`, `basis_change.hpp:779`) shifts 1 by a bit position below 32; `std::size_t{1} << N` in `make_full` and `product_capacity` stays below 2^32 in a 64-bit `size_t`. At 32 generators GCC builds the bulk dual of a vector (`r17 -DGEN=32`, a row of `r17_runs.tsv`), so no shift fails a constant evaluation on that path. What does fail at high counts is a capacity and a limit, H0-21 and H0-24.

### H0-16 A change of basis refused for an overflow of the elimination alone (release exception 9)

- Site: `basis_change.hpp:403-469` (`eliminate_entries`, `eliminate`), reported at `basis_change.hpp:719-721`.
- Class: integer edge; valid input refused.
- Smallest input found: the rotation of R(3, 0, 0) from the quaternion (79, 2, 1, 1), P = M / 6247 (`repro/r12_orthogonal_change_overflow_fail.cpp`). The integer rows are at most 6,243 in magnitude; M M^T = 6247^2 I fits; the inverse is P^T; every blade image is a ratio of integers below 6247^3, about 2.4e11. The quaternion (78, 2, 1, 1), N = 6090, is accepted.
- What a user sees: "basis_change: an exact value the change computes does not fit coefficient_type; use rows with smaller entries". The `-DPROBE` build prints "elimination fits 0, inner products fit 1 hold 1".
- Verdict: confirmed, and the same class as exception 9 and decision 0236 item 2. A transcription of the reduction and the elimination in exact integers (`repro/r12_elimination_sim.py`, output in `r12_elimination_sim.out`) shows that at N = 6247 only the left block's diagonal entries of the last step overflow, at 9.50e18; these are the entries option (a) of decision 0236 item 2 would skip. The transcription and the compiled `-DPROBE` build agree on 78 and 79. Rational rotations are a natural input, and the threshold for three generators is a denominator near 6,200, so this is smaller than the record's wording suggests. "Smallest" is within the families scanned ((w, 2, 1, 1), (w, w-1, w-2, w-3), (w, w/2, w/3, 1) for w up to 5,000), not proven over all inputs.
- Pinned by: none at this input. Proposed test (H2): this pair, 78 accepted and 79 refused, as the characterization plan 0047 section 4 asks for.
- Fix changes relied-on behavior: no; it accepts more input. The message would no longer appear for such changes.

### H0-17 A scalar whose `std::numeric_limits` is not specialized

- Site: `mv.hpp:722-781` (`holds_exactly` checks nothing for such a type), `basis_change.hpp:1063-1072` (`divides_exactly` is true for any type with `/` that `integer_limits` does not describe), `linear.hpp:416` (the zero-divisor check needs `integer_limits`), `mv.hpp:113-114` (`exact_scalar` is false, so `truncate` and `transform` do not check).
- Class: scalar edge; silent wrong result.
- Smallest input: Clang's `_BitInt(128)`, an integer type with no specialization in libstdc++ 16, and `change_basis<to_r410>(Origin{1})` for the conformal origin (`repro/r13_bitint_change_truncates.cpp`; Clang only, since GCC 16 has no `_BitInt` in C++).
- What a user sees: the image (0, 0) where the exact one is (-1/2, 1/2): the halving truncates, and nothing is reported. `__int128` is specialized in both `-std=c++23` and `-std=gnu++23` on both compilers (checked), so it is handled correctly.
- Verdict: confirmed, within a documented class: `change_basis`'s comment says "A scalar whose division truncates and whose numeric_limits do not say it is an integer, such as some big-integer classes, is not detected", and `operator/` and `exact_scalar` say the same for their checks.
- Fix changes relied-on behavior: yes, if such scalars were rejected; no, for documentation of `_BitInt` by name.

### H0-18 `rotation` with an angle that is not finite

- Site: `motions.hpp:80-97`.
- Class: rounding edge; input not checked.
- Smallest input: `rotation(z_axis, inf)` in a checking build (`repro/r14_rotation_angle_not_finite.cpp`).
- What a user sees: a motor with NaN on the line's blades and on e1234, no message. The axis is checked (`GALOOT_ASSERT(std::isnormal(length))`); the angle is not, and the comment does not say so.
- Verdict: confirmed. The rounding side of `rotation` (accuracy, subnormal squares, which the comment says no test covers) is the numerical reader's and H3's.
- Fix changes relied-on behavior: a check in checking mode would abort programs that pass today.

### H0-19 `translation` over `bool`

- Site: `motions.hpp:159-176`.
- Class: diagnostic.
- Input: `translation(true, false, true)` (`repro/r15_translation_bool_fail.cpp`).
- What a user sees: two errors where the library aims for one: "mv: bool cannot be the scalar type", then "translation: halving d would truncate in an integer scalar type", since `std::integral<bool>` picks the integer rejection. The first is the right message.
- Verdict: confirmed. Nit.

### H0-20 Empty layouts

- Site: `dual.hpp:179-185` and `336` (a dot of a point and a line is a value of the empty layout), and every operation that receives one.
- Verdict: dismissed. `repro/r16_empty_layout.cpp` takes the empty dot through `+`, `2.0 *`, unary `-`, a geometric product, `!`, `bulk`, `transform`, a conversion to a point, `==`, and `std::format`; all build with `-Wall -Wextra -Werror` on both compilers and give empty values (`{}`), and the point is zero.

### H0-21 Term-list capacity far above the product's size, refused on Clang from 22 generators

- Site: `blade_product.hpp:186-192` (`product_capacity`: 2^(|A ∪ B| - 1)), used by `plan.hpp:147-153` (`pair_capacity`) for every term list of a pair's product. The contract at stake is `metric.hpp:51-55` and `limits.md:24-26`: an algebra built with `MaxN` up to 32 "every later part of the library accepts", though nothing above 16 is tested.
- Class: contract not kept at a documented bound; valid input refused.
- Smallest input: the bulk dual of a vector of R(22, 0, 0) built with `MaxN` 32 (`repro/r17_wide_algebra_capacity.cpp -DGEN=22`). A vector times the antiscalar has at most 22 terms; its lists get 2^21 places.
- What a user sees on Clang: four errors, the first "constexpr variable 'count' must be initialized by a constant expression" at `plan.hpp:553`, with the note "cannot allocate array; evaluated array bound 2097152 exceeds the limit (1048576); use '-fconstexpr-steps' to increase this limit" in `term_list::add`. None names the operation. GCC builds every size to 32, in about 0.2 s and 85 MiB (`r17_runs.tsv`), so its constant evaluator holds the unused places cheaply. Clang builds up to 21.
- Also refused on Clang at 22 generators, for vector operands (each compiled once, not kept as reproducers): with the note "cannot allocate array", the geometric antiproduct, `bulk`, `weight`, `bulk_dual` and `antidot`, all of which multiply by the antiscalar or through the complements; with "constexpr evaluation hit maximum step limit", the geometric product, the wedge and `dot`, which is the placement cost of H0-24. The antiwedge and the right complement build. GCC builds all ten.
- Verdict: confirmed. The capacity is a correct upper bound, so no result is wrong; it is loose by a factor of 2^(n-1)/n for a vector times the antiscalar, and Clang's array limit turns that into a refusal.
- Pinned by: none (nothing above 16 is tested). Proposed test (H2, under O9): the bulk dual of a vector at 21 and 22 generators on Clang, so that the limit is pinned until it is lifted or stated.
- Fix changes relied-on behavior: no. A tighter bound (the number of blades of A ∪ B whose grade lies in the window the product can reach, taken over the partial products too) accepts more input and changes no result; it is a change to the generative mechanism, so it needs the strongest evidence and the coordination with plan 0048 that plan 0047 section 9.2 sets out. The alternative is a sentence on the limits page.

### H0-22 The rounding check over `std::float16_t` misses a non-versor at ordinary magnitudes

- Site: `transform.hpp:408-414` (`rounding_checked_scalar` admits any `is_iec559` type that rounds to nearest), `transform.hpp:496-513` (the padding τ = λ/u, the smallest normal number over the unit roundoff), with the margin check at `:525-554`.
- Class: rounding edge; silent loss of a check.
- Smallest input: in R(4, 0, 0) over `std::float16_t`, x = a on the scalar and v = a + a e1234, which is not a versor (the header's own example of one that leaves x's blades), in a checking build (`repro/r18_float16_rounding_floor.cpp`).
- What a user sees: for a = 0.1 the check aborts with "transform: v is not a versor on x within rounding"; for a = 0.05 it passes, and the result is returned with the dropped part ignored. The program prints "checked: 1, padding: 0.125": the type passes the margin, and τ is 2^-3.
- Verdict: confirmed, on GCC; Clang 23 with libstdc++ 16 has no `std::float16_t` here. For `float` the same input is caught down to a = 1e-12 and missed at 1e-13, matching the header's "below about 2^-41". The header states the floor for `double` and `float` only (`transform.hpp:488-495`, `722-728`); for `float16_t` it lies near the size of ordinary inputs, so the check is active and blind at once. The trade the header makes (τ above λ so that padded products stay normal) is reasonable for `float` and `double` and not for a type of 11 digits.
- Pinned by: none. This row is handed to the numerical reader and H3 for the bound's analysis; the C++ side is only which types the concept admits.
- Fix changes relied-on behavior: yes. Excluding narrow types from the check, or lowering τ for them, changes which programs abort in checking mode.

### H0-23 Overflow of an integer scalar's own arithmetic

- Site: the kernels, `mv.hpp:298-369`, and the scalings, `linear.hpp:359-436`; the guide's statement that the integer types are exact (`core.md:375-378`).
- Class: undefined behavior at run time, in every mode; undocumented.
- Smallest input: `wedge(V{65536, 0, 0}, V{0, 65536, 0})` over `int` (`repro/r19_int_scalar_overflow.cpp`).
- What a user sees: 0 on e12; under `-fsanitize=undefined`, "signed integer overflow: 65536 * 65536 cannot be represented in type 'int'" at `mv.hpp:366`.
- Verdict: confirmed. This is the scalar's own arithmetic, not the library's, and constitution C7 puts rounding (and by extension range) in the scalar's hands. But the guide and the headers call an integer scalar "exact" and promise "with an exact scalar the result is exact" (`mv.hpp:894`) without saying that this holds only while the values fit, while they do state the range of the generated coefficients and the `min() / -1` case of division (`linear.hpp:407-411`).
- Fix changes relied-on behavior: no; a sentence in the guide.

### H0-24 Placement cost above 16 generators

- Site: `plan.hpp:308-327` (`collect_result`, T terms on R blades); `limits.md:17-32`.
- Class: compile-time limit at a documented bound.
- Smallest input: the wedge of two vector layouts of R(20, 0, 0), `MaxN` 32 (`repro/r20_wide_vectors_wedge.cpp`), 380 terms on 190 blades.
- What a user sees on Clang: four errors, "constexpr variable 'found' must be initialized by a constant expression" with the note "constexpr evaluation hit maximum step limit" in `collect_result`; 19 builds. GCC builds to 30 and at 31 exceeds its operation limit with 39 errors, the first "'constexpr' evaluation operation count exceeds limit of 33554432" at `plan.hpp:362`.
- Verdict: confirmed, within the class the limits page describes (placement grows as T times R), but the page gives figures only for full layouts up to 10 generators, and "accepted by every later part of the library" up to 32 does not hold for two vectors at 20 on Clang. This is plan 0047's O9 (H2 tests sparse use at 17, 24 and 32) and the deferred placement search (O3), whose trigger is "a preset or a user report that reaches the limit".
- Fix changes relied-on behavior: no.

### H0-25 Names separated by a tab

- Site: `blade_set.hpp:460-469` (only spaces separate names) and the message at `:513-515`.
- Input: `blades<rga3, "e1\te2">` (`repro/r21_tab_separator_fail.cpp`).
- What a user sees: "blades: after the e, a name lists generator indices, each a digit from 1 to 9", one error. Correct rejection; the message does not mention that names are separated by spaces only. Nit.

### H0-26 A `detail` term list over its capacity at run time

- Site: `blade_product.hpp:141-168`; the check at `:162` precedes the shift that would write one place past the array.
- Verdict: dismissed for public input. No public name calls `term_list::add`, `blade_product` or `generate_plan` at run time; they run in constant evaluation, where the check always holds. A program that calls them at run time in an unchecked build with too small a `Capacity` writes out of bounds; that is unit H4's harness, which calls the generators at run time, and its briefs should say so.

### H0-27 The formatter's buffers and wide algebras

- Site: `format.hpp:88-93` (`append` has no bound), `format.hpp:220-224`.
- Verdict: dismissed. `append` runs only in `consteval` code after the tag's names were validated, so a name has at most ten characters and `printed_name` has eleven places; an overrun would fail the constant evaluation in any case. An algebra above nine generators is rejected with "format: names such as e123 have one digit per index ..." (tested by `format_wide_algebra_fail` and `format_wide_algebra_auto_fail`).

### H0-28 `get` of an absent blade (plan 0047's candidate)

- Site: `mv.hpp:1299-1338`.
- Verdict: dismissed. Every cause gives one error with its own message, tested by `mv_get_*_fail` (absent, absent in an ordered layout, the other orientation in both kinds of layout, an index above n, an index of zero, not a name, a repeated generator, two names, a wide algebra). The rejecting overload's `return std::forward_like<X>(x.coefficients[0]);` follows a `static_assert(false, ...)` in every branch, so it is never compiled into a program that builds.

### H0-29 `GALOOT_FORCE_INLINE` set differently in two units

- Site: `assert.hpp:20-51` and `86-90`.
- Verdict: open, for H5. The rule is stated ("Every translation unit of a program must use the same mode"), and the mechanism is the same as H0-03's (a function's attributes differ between definitions), but no reproducer was written here, because what a user would observe (a call where an inline expansion was expected, or the reverse) depends on the linker's choice and the optimizer and is H5's measurement.

### H0-30 Two versions of Galoot in one program

- Site: every header's include guard (`GALOOT_CORE_*_HPP`, unversioned) and the namespace `galoot::core` (no inline namespace for the version).
- Verdict: open, for H5. Read only: within one unit, the second version's headers are skipped by their guards without a diagnostic; across units, two versions' inline functions of one name are one entity with two definitions. What settles it: H5's reproducers with two copies of the headers.

### H0-31 A forced-inline operation called from a function built for fewer instruction-set features

- Site: `assert.hpp:77-83`, release friction 8.
- Verdict: open, for H1, which plan 0047 section 5.2 already assigns: a pin that both compilers stop and name the attribute. Not reproduced here.

### H0-32 Floating-point options that the rounding check cannot see

- Site: `transform.hpp:91-104`: a checking build stops under `-ffast-math` (`__FAST_MATH__`) and `-ffinite-math-only` (`__FINITE_MATH_ONLY__`); `-fassociative-math`, `-freciprocal-math`, `-fno-signed-zeros` alone, and flushing subnormals to zero without `-ffast-math` (GCC's `-mdaz-ftz`, Clang's `-fdenormal-fp-math=preserve-sign`) set no macro the header can test, as its comment says.
- Verdict: open, for the numerical reader and H3: whether any of these makes the check reject a versor or pass a non-versor needs the bound's analysis, not a C++ reading.

### H0-33 NaN, infinity, subnormal values and overflow in the rounding check

- Site: `transform.hpp:587-620` and the contract at `751-763`.
- Verdict: open, for the numerical reader and H3, as plan 0047 section 5.1 assigns. Read only: the check rejects a NaN or infinite dropped coefficient and an overflowing magnitude, as the comment says (`size <= tolerance && tolerance <= max` is false for NaN and infinity).

### H0-34 Integer division by zero and `min() / -1`

- Site: `linear.hpp:383-424`.
- Verdict: dismissed. Both are stated on the declaration; division by zero is checked by `GALOOT_ASSERT` for a scalar that `integer_limits` describes and tested (`linear_divide_assert_fail`, the `linear_divide_consteval_zero*` tests); `min() / -1` is stated as undefined at run time, as the scalar's own arithmetic.

### H0-35 The checks of a form

- Site: `metric.hpp:244-262` (`from_gram`), `264-314` (`is_valid_form`, `metric_like`).
- Verdict: dismissed. Symmetry, the entry range (`max_form_entry`, 32,767 with a 32-bit `int`), a count of zero, a count above `MaxN`, and a `MaxN` above 32 each fail at compile time with a message, tested by `metric_*_fail`; a hand-written metric with an entry of `INT_MIN` is caught by the range test, which compares with `-max_form_entry` and never negates the entry.

## Not verified here

- The other reader's register, and plan 0048's candidate list for H0 (section 9.2 item 2 of plan 0047): neither was read. The candidate list is not in `main`.
- Rounding: the bounds of `transform` and `rotation` were not analyzed; only H0-22 was probed, because the C++ question there is which types the concept admits. `long double` was not examined.
- `-ffp-contract=fast` (release friction 7) was not re-measured.
- Libraries other than libstdc++ 16, and compilers other than GCC 16.2.1 and Clang 23.1.5.
- The lines that plan 0044 changes in `assert.hpp`, `basis_change.hpp`, `bits.hpp`, `metric.hpp` and `motions.hpp`: plan 0047 section 8 asks H0 to re-read them once 1.0.1 is tagged. None of this register's findings is fixed by that branch as it stands (its `assert.hpp` change hardens only `GALOOT_FORCE_INLINE`'s spelling, and H0-04 remains), but line numbers in those five files will move.
