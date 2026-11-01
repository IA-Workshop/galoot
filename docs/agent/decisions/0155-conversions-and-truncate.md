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
