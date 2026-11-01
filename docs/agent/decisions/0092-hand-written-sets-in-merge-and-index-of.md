# 0092 merge and index_of check sets written by hand

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), answering finding N1 of the W5 review of WU4 (`docs/agent/records/reviews/2026-10-31-wu4-blade-sets.md`).
- Supersedes: decision 0091, question 2, only its sentence "A mask repeated within one input with one sign is merged once". The rest of 0091 stands.

## Context

`blade_set<K>` is an aggregate, so a user can write one by hand. The review found that `merge` and `index_of` accepted a hand-written set with a sign of 0 or 2, or with one mask listed twice. `merge` silently collapsed a repeated mask, and for a mask listed twice in one input with both signs it reported the false message "merge: the two sets hold one blade with opposite signs". The constructors (`full`, `of_grade`, `blades`, `merge`) never produce such a set.

## Options

1. Check in `merge` and `index_of` now, each check with its own message.
2. Defer to WU8, where `mv<A, S>` could check each set once per type through its concept.
3. Check whole sets in both, for every rule.

## Decision

Option 1, with a cost limit on `index_of`:

- `merge` checks every sign and every mask of both inputs while it inserts them. The insertion already compares each blade with the merged list, and recording which input a list entry came from makes a repeat within one input distinguishable from the same blade in both. The cost is no worse than the merge's own. Messages: "merge: a set holds a sign other than +1 or -1", "merge: a set holds one mask twice". A mask found again in the same input counts as a repeat, never as opposite signs, so the opposite-signs message appears only for a blade that the two inputs both hold. An input that is also wrong in another way can add that message alongside, and both messages are then true. A guard that suppressed the opposite-signs check for invalid inputs was tried and removed. It hid only that second, true message, and its mutant survived on both compilers, so it added code without changing any false message.
- `index_of` checks every sign, and that the mask it looks up appears once: "index_of: the set holds a sign other than +1 or -1", "index_of: the set holds this mask twice". It does not check that no other mask repeats. That would cost O(K²) steps per set, or a sort. Measured at default limits, Clang 23 already fails to build `full<14>` and `of_grade<32, 4>` for want of steps. A whole-set check in every lookup would lower the size at which `index_of` works, while a repeat of another mask does not change this lookup's answer.
- Option 3 is left for the type that stores a set (WU8), which can check a set once rather than per lookup.

## Consequences

- Four compile-fail tests: `core.blade_set_merge_bad_sign`, `core.blade_set_merge_repeated_mask` (which forbids "opposite signs"), `core.blade_set_index_of_bad_sign`, and `core.blade_set_index_of_repeated_mask`.
- A hand-written set with a repeated mask that is never merged and whose repeated mask is never looked up is still accepted. WU8 decides whether `mv` rejects it.

## Correction, 2026-10-31 (plan 0009)

The review of the fix commits (`docs/agent/records/reviews/2026-10-31-unreviewed-fix-passes.md`) showed two statements of this record wrong as the code stood. The record above is left as written; this section corrects it.

1. A1. "merge checks every sign and every mask of both inputs" was false. The merged list recorded only the input that first listed a blade. So a mask repeated in the second input, when the first input also held it, compared against the first input's entry and was taken for the same blade in both. `merge<{+e1}, {+e1, +e1}>` was accepted, and `merge<{+e1}, {+e1, −e1}>` reported only "opposite signs". The author's tests had no repeat in the second input of a mask the first also held, and no mutant of that branch. Fixed in `644939e`: each listed blade records both inputs, and a repeat is a mask met again in an input that already gave it. Tests `core.blade_set_merge_repeated_in_second` and `core.blade_set_merge_repeated_in_second_signs`, the second with FORBID "opposite signs", fail against the header as merged and pass after.
2. A2. The claim that the removed guard "hid only that second, true message", in this record and in the message of `e191731`, was wrong. For `merge<{e1 with sign 0}, {+e1}>`, the two signs differ, so the opposite-signs flag was set, and the message "the two sets hold one blade with opposite signs" is false: a sign of 0 is neither orientation. The guard had suppressed it. The author's mutant of the guard survived because no test had a bad sign on a mask shared by both inputs, and the author read the survivor as proof that the guard was redundant, without constructing that input. Fixed in `644939e` at the cause: opposite signs are reported only when both signs are +1 or −1 and differ. Test `core.blade_set_merge_bad_sign_no_conflict` forbids "opposite signs" for that input, and fails against the header as merged.
3. A3. Further tests close the gaps the surviving mutants showed, in `9670158`, each passing against the header as merged:
   - `core.blade_set_merge_bad_sign_second`, a bad sign in the second input.
   - A valid merge, e2 with e3 e1 e2, whose second input is out of canonical order and shares e2 with the first. It kills the mutant that drops the flag shift, which wrongly rejects valid merges.
   - FORBID "twice" on `core.blade_set_index_of_absent`.
   - 300 seeded random merges and lookups.
4. `index_of` now checks the signs once per set and scans only after the position found for a repeat (decision 0093). The rules it checks are unchanged: the signs, and that the looked-up mask appears once.
5. Mutants of the fixed header found three more gaps on both compilers, each closed in this unit:
   - Dropping the shift of the first input's flags survived, because no test repeated a mask in the first input with an insertion between the two listings. `core.blade_set_merge_repeated_after_shift` (e2 e1 e2) now kills it.
   - Starting the repeat scan two places on survived, because the existing test's repeat was two places away. `core.blade_set_index_of_repeated_adjacent` (e2 e1 e1) now kills it.
   - Removing the guard that ran the repeat check only for a present mask survived. For an absent mask the position found is the size of the set, and the scan starts after it, so the guard had no effect. Unlike `e191731`, this conclusion rests on reading the loop bounds, not on the survivor alone. The guard is removed in `3cda375`, and `core.blade_set_index_of_absent`, which forbids "twice", still gives its one message on both compilers.
