# 0155 Conversions between blade sets, and truncate

- Status: accepted by the owner, 2026-11-01 ("proceed as recommended"); the CONVENTIONS 4.5 amendment (version 1.6) is ratified by the same ruling
- Date: 2026-11-01
- Decided by: the owner, on plan 0016 questions 3 and 7 and research note 0019 question 5; recorded by Claude (agent, Opus 5.5)

## Context

STYLE S8 makes conversions between blade sets explicit and gives a lossy conversion its own name; CONVENTIONS 4.5 names it `truncate<Blades>(x)`. Plan 0016 proposed an explicit conversion that drops nothing (reordering and re-signing, as from the canonical result of p ∧ q into a Line), and asked how checking mode applies to `truncate` on a rounding scalar, for which decision 0010 speaks of "the plan's forward error bound" while a standalone `truncate` has no plan. Note 0019 found that every conversion C++ users know takes its target as a type.

## Options weighed

For rounding scalars in `truncate`: (A) check exact scalars only (chosen); (B) exact zero for every scalar, which aborts on correct rounded inputs (V13); (C) a second overload taking a bound.

## Decision

1. A conversion that drops no blade is a constructor of the target, `explicit`, accepting a multivector of the same algebra and scalar whose blades the target all holds, in either orientation; each coefficient takes the sign s_source · s_target. `Motor{line}`, `Motor(line)`, and `static_cast<Motor>(line)` all work, because the type is no longer an aggregate (decision 0150).
2. A target that drops a blade reaches an `explicit` catch-all constructor of form P with the message "conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)". An implicit conversion between blade sets, lossy or not, is rejected by the compiler's own message, because every converting constructor is `explicit` (STYLE S8).
3. `truncate<Type>(x)` and `truncate<Blades>(x)` both exist: the target named by its type, or by its blade set. The type must be a multivector of x's algebra and scalar, otherwise a catch-all rejects it.
4. In checking mode, `truncate` checks each dropped coefficient for an exact scalar (decision 0151) with `GALOOT_ASSERT`; for a rounding scalar it checks nothing, and its comment says why. Operations that drop part of their own result, such as `transform`, check the dropped part against their own plan's forward error bound when they are built (WU13).
5. CONVENTIONS 4.5 is amended to name both forms (version 1.6).

## Consequences

- Item 4 conflicts with CONVENTIONS 4.5's remaining clause, "and within the operation's forward error bound for a rounding scalar (decision 0010)", which is ratified text and is not covered by the ruling's edit. That clause is not edited. Plan 0016 asks the owner to ratify a wording that agrees with item 4, and WU8b does not merge `truncate`'s checking until the wording is settled.
- Plan 0016 tests F9 and F10.

## Implementation, 2026-11-01 (WU8b)

Recorded by Claude (agent, Opus 5.5), cpp-expert role, implementing WU8b of plan 0016. CONVENTIONS 4.5 is version 1.7 by then (decision 0161), so the conflict in the consequences above is settled and `truncate` checks exact scalars only, as ruled.

1. The lossless conversion is `template <blade_set From> requires(holds_every_mask(Blades, From)) constexpr explicit mv(const mv<Alg, From, T>&)`, built on the identity kernel of decision 0154 (each coefficient times s_from s_to; the blades the source lacks are zero). The same-layout case is the copy constructor, which a non-template wins over the template.
2. The lossy catch-all is the same template with the negated constraint, `explicit`, whose body is `static_assert(false, "conversion drops blades of its input: it is lossy, so request it with truncate<Type>(x)")`, the wording of item 2. All three spellings reach it (`core.mv_conversion_lossy_brace`, `_paren`, `_cast`).
3. Implicit conversions stay the compiler's error, lossy (`const Point s = q;`) and lossless (`const Motor m = p;`), as item 2 rules (`core.mv_conversion_implicit_lossy` and `_lossless` record the wording). The WU8a diagnostics record and the outcome table of plan 0016 said that WU8b's catch-all would replace the compiler's message for `const Point s = q;`; it cannot, because an `explicit` constructor is not a candidate in copy-initialization. The diagnostics record of WU8b (`docs/agent/records/diagnostics/wu8/wu8b/`) measures the alternative, a lossy catch-all without `explicit`: one error with the literal, GCC 10 lines and Clang 9, against the compiler's 9 and 28. Not adopted: it is a single-argument constructor that is not explicit, against STYLE S8's wording, and `std::is_convertible_v<Motor, Point>` would answer yes. For the owner, if the literal on the assignment is wanted.
4. A source of another algebra or scalar matches neither constructor and is the compiler's "no matching constructor"; a single multivector or a class derived from one is excluded from the count catch-all and from the single-argument catch-all of decision 0150's WU8b section, so a derived class copies and converts as its base does.
5. `truncate<To>(x)` for a layout and `truncate<Target>(x)` for a type (any cv-qualification and reference; it returns the plain type), overloaded on the kind of the first template parameter. Kept blades take the conversion's sign; blades of the target that x lacks are zero. For an `exact_scalar` with `==`, each dropped coefficient is checked by `GALOOT_ASSERT(c[at] == T{}, "truncate: a dropped coefficient is not zero")`, in every constant evaluation and at run time when `GALOOT_CHECKING` is 1; with it 0, the comparison is not evaluated (tested with a scalar whose `==` counts its calls). An exact scalar without `==` is not checked; no present scalar is one. A target that is not a multivector type of x's algebra and scalar type reaches a catch-all (form P, `auto` return) with "truncate: the target must be a multivector type of the same algebra and scalar type; truncate changes only which blades are held".
6. The run-time aborts are tested, as the repository's other run-time preconditions are, by one program built in each checking mode and run under `test/expect_output.cmake` (`core.mv_truncate_nonzero_*`, `core.mv_truncate_unchecked_*`), rather than `WILL_FAIL` in the checking presets: CTest reports an abort as "Subprocess aborted", which `WILL_FAIL` does not turn into a pass, and fixing the mode in the program runs both modes in every preset.
