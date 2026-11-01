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
