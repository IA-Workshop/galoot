# 0168 `part` takes what `truncate` takes and never checks; C8's check covers what an operation assumes is zero

- Status: accepted (plan 0019 section 1, question 1; ratified by the owner on 2026-11-02, ruling O1: "agreed", with CONVENTIONS version 1.8; recorded by the WU10b implementer)
- Date: 2026-11-02
- Recorded by: Claude (agent, Opus 5.5), implementer of WU10b, cpp-expert role
- Bears on: decision 0162 item 6 (the ruling that asked for a never-checking projection), decision 0161 (`truncate` keeps its check), decision 0155 (`truncate<Type>`), CONVENTIONS 4.5, constitution C8

## Context

Decision 0161 kept `truncate`'s check: on an exact scalar, a conversion that drops blades verifies that what it drops is zero. Decision 0162 item 6 asked for a second, plainly named function that takes the same components and never checks, for code that drops nonzero coefficients on purpose, as a grade part does. Two ratified texts stood in the way. CONVENTIONS 4.5 named the lossy conversion `truncate` and gave it the check. Constitution C8 says "a checking mode verifies that whatever an operation discarded was zero", which, read literally, forbids any operation that drops nonzero components by design, including the bulk, the weight, and every grade part.

## Options weighed

The name (plan 0019, question 1):
1. `part<Line>(motor)` (chosen). The field's word for a subset of components ("the scalar part", "the grade-k part"), which CONVENTIONS 3.2 and 6.3 already use; it says the rest is left behind on purpose and suggests no check; it is short and in no standard header. `part<Motor>(line)`, with a larger target, reads less well; it gives the line with zeros on the other blades, as `truncate` does.
2. `project<Line>(x)`. CONVENTIONS 4.3 gives "projection" to b ∨ (a ∧ b☆), a later unit's `project`.
3. `truncate_unchecked<Line>(x)`. States the relation, but presents the ordinary operation as the unsafe variant of the checked one.
4. `keep<Line>(x)` or `select<Line>(x)`. `keep` does not say the rest goes; `select` has other meanings.

The C8 reading: C8's checking mode covers what an operation assumes is zero, as `truncate` and `transform` do. An operation whose purpose is to discard, and whose name says so, assumes nothing, so there is nothing to check. The plan asked the owner to confirm this reading, not to amend C8.

The implementation:
1. `truncate` as `part` plus the check. One copy of the two-line kernel call, but then a mutant "part calls truncate" recurses without end, which tests nothing about the check.
2. Two short functions with the same kernel call (chosen): `part<To>(x)` applies `identity_kernel<From, To>` and returns; `truncate<To>(x)` applies it and checks. A reader sees at once that `part` has no check (S16: a short repetition that reads plainly).

## Decision

1. The owner's ruling of 2026-11-02 ratifies CONVENTIONS version 1.8: 4.5 adds "`part<Blades>(x)` and `part<Type>(x)` keep the same components and never check: what they drop is meant to be nonzero, as in a grade part." The reading of C8 above is confirmed; C8 is not amended.
2. `mv.hpp` gains `part<Blades>(x)`, `part<Type>(x)` for an mv type of x's algebra and scalar with any cv-qualification and reference, and a catch-all for any other target with the message "part: the target must be an mv type of the same algebra and scalar type, not a class derived from one; part changes only which blades are held", which names part and never truncate. The result type and value are `truncate`'s for every input whose dropped part is zero. Nothing is checked, for any scalar, in a constant evaluation or at run time, whatever `GALOOT_CHECKING` is.
3. `truncate`'s Requires paragraph ends "To drop coefficients that are not zero on purpose, use part, which takes the same components and never checks." The lossy-conversion message ruled in decision 0162 item 8 is unchanged.

## Consequences

- Two public names with the same results on the inputs where both apply. The test suite pins that they agree where the dropped part is zero, and that `part` is a constant expression where `truncate` fails its check, over `std::int64_t` and the test rational, and at run time in a checking build, including over a scalar that counts its comparisons.
- The bulk, the weight, and grade parts of later units can be built on `part` without a check and without a further amendment.
- Nothing in WU10 calls either function: the sign maps keep their layout (decision 0165).
