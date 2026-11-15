# 0091 What the name parser reads, and how merge treats signs

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), answering the two open questions of plan 0008.

## Question 1: which names the parser reads

Context: CONVENTIONS 1.1 names generators e1 to en and 1.6 writes oriented names such as e31 and e423. The golden list `golden::oriented_names::rigid_3` includes "1" for the scalar and "e1234" for the antiscalar, and plan 0008 criterion 1 asks that this list parse.

Options: (a) names with a leading `e` only; (b) also "1" for the scalar; (c) also other spellings, such as a leading sign ("-e13"), other letters, or multi-digit indices with separators.

Decision: (b). A name is "1", or `e` followed by one or more generator indices, each a digit from 1 to n. Names are separated by runs of spaces; leading and trailing spaces are ignored, and other characters are errors. "1" is the canonical name of the scalar in the golden names (`golden::blade_names`), and `canonical_name(0)` returns it, so parsing and naming round-trip. (c) adds spellings without a present user; a sign in a name would also duplicate what the order of the indices already says. Rejected, each with its own literal message, for the first wrong name: an empty list; a name that is not "1" and does not start with `e` (a wrong letter or a leading digit); `e` alone or followed by a character other than 1 to 9 (including 0); an index above n; a generator listed twice in one name (e121); and a blade named twice, as the same name or in another order (e12 e21). n above 9 is rejected before any name is read, because e123 could mean e1 e23 or e12 e3.

## Question 2: merging sets that hold one mask with different signs

Context: `merge<A, B>` returns the blades of A and B in canonical order. A blade can appear in both with the same sign, or with opposite signs (e13 in one, e31 = −e13 in the other).

Options: (a) the result holds every blade with sign +1, so merge is a union of masks; (b) each blade keeps its sign, and opposite signs are rejected at compile time; (c) each blade keeps its sign, and the first set's sign wins.

Decision: (b). The constructors of this unit build every set a later unit can produce, and a result with no declared type gets +1 from `full` and `of_grade` anyway. Keeping signs lets a merge of user-declared types keep their orientation, which is the reason oriented blades exist (CONVENTIONS 1.6). Of the two ways to keep signs, (c) hides a negation (constitution C8), while (b) reports the conflict with the message "merge: the two sets hold one blade with opposite signs". Moving later from (b) to (a) or (c) changes no accepted program, so (b) is the choice that every later reading permits (W8). A mask repeated within one input with one sign is merged once; the inputs need not be in canonical order.

## Consequences

- The plan generator (WU7) builds result sets in canonical order with sign +1 and needs no change. A unit that sums values of differently oriented types decides then whether to canonicalize first or to relax this rule, by a decision that supersedes question 2.
- Each rejection has a compile-fail test on both compilers (plan 0008, criterion 4).
