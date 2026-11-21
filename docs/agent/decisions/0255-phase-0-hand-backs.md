# 0255 The phase 0 hand-backs of plans 0047, 0048 and 0049: what the coordinator settled and what goes to the owner

Date: 2026-11-21. Source: the hand-backs of units V1, V3 and H0 (plan 0047), D0 (plan 0048) and U1 (plan 0049), each with its independent check, merged into `main` on this date.

The units returned more open items than their plans foresaw. This record sorts them. Part A holds what the coordinating agent settled, because the item is a reading of a plan or a brief detail and no ruling of the owner's changes. Part B holds what only the owner can rule; none of it is acted on. Part C holds the items that change nothing now.

## A. Settled by the coordinator

Each can be reversed by the owner; the reports of this date list them as readings.

1. **V3's criterion 5.** Read per numbered item of sections 1 to 5 and 7 of CONVENTIONS (28 items), each split where its status label splits. The plan's "54 labeled items" counted label words over the whole file. Recorded in plan 0047's addendum of this date.
2. **The model level after the sample checks (decision 0253 item 12).** H0's first sample check found errors, so H0's correction ran at the Fable level, as item 12 provides. V3's check found none, so V3 stays at Opus. Item 12 belongs to plan 0047; D0's errors (plan 0048) do not trigger it. D0's answer is mechanical: the comment table's checker now holds its own list of the 27 preconditions kept in full and fails when one is demoted.
3. **H0's rule for documented behavior** stands as the correction wrote it: open when the effect was neither shown nor ruled out; dismissed only when the documentation states the effect where the contract lives and a test pins it; otherwise confirmed, low. Row A49 stays confirmed, and A46 stays open.
4. **The tutorial's chapter cut.** The cut of U1's design stands: the plan's chapters 1 and 2 are one, and a new chapter 6 teaches the antiproduct in R(3, 0, 0) before the rigid algebra. The page count of plan 0049 section 4.1 does not change.
5. **Tutorial examples** are whole programs with one fixed preamble per chapter. The mechanism of decision 0242 does not change.
6. **D2 without D3.** The tutorial links only pages that exist at D2; chapter 8 points to the conventions chapter, not to the porting recipe.
7. **The conventions page's name.** U3 names the page so that the citation check passes unchanged. No citation pattern is weakened for it (W9).
8. **A second language-model cold reader**, told that it knows rotors and quaternions, reads the conventions chapter and the first-things page at U4. It is the only test of item B1's documentation.
9. **Research note numbers.** V3's note is 0028 and U1's is 0029; both units had taken 0028.
10. **The comment table.** No comment unit deletes the two prologue illustrations (`transform.hpp:24-29`, `dual.hpp:60-70`) before D3's reference entries for `transform` and `bulk` hold them; their rows change to that wait in the commit that renames the table's targets, which follows U3's published headings. The seven enum-size suppressions keep their reasons.
11. **Briefs of later units.** H1 counts diagnostics by the compiler's own count, not by lines that contain "error", and pins both paths of rows A39 and A50. Plan 0048's object-code comparison passes `-fno-ipa-icf` on GCC; E2's review reads where `detail::` names appear in its errors; C0's pass rule includes constant-evaluation step counts; B5's author keeps the overflow statement at `coefficient_type`; E1's author keeps the reason that three summary rows cite.
12. **Knowledge-base candidates** from these units go to the math-tools backlog, not into these records.

## B. Put to the owner

Each has its options and tradeoffs in the record named. The recommendation is the coordinator's.

1. **`transform` and the classical sandwich** (research note 0029, open item 1). `transform(v, r)` is not `r * v * ~r`; in a Euclidean algebra the classical result is `transform(v, !r)`. A user who knows rotors gets a wrong vector with no message, and a quarter-turn test passes. Options: (a) documentation only; (b) also a named classical operation in a later minor release; (c) also a sentence in `transform`'s `///` comment. Recommendation: (a) now, (c) when that comment is next changed, (b) as the first backlog candidate. Units U3 to U6 proceed on (a), which every option includes.
2. **Does S11's contract sentence cover `detail` declarations?** (`records/simplification/restated-targets.md` item 2.) Options: (a) no, and the prepared wording of decision 0253 item 14 gains "outside a `detail` namespace"; (b) yes. 97 comment lines wait on this alone. Recommendation: (a).
3. **Where the guide pointers go** (`records/docs/README.md` open item 1). Options: (a) on every public declaration, about 190 lines; (b) on each declaration with a reference entry, about 120 lines. Recommendation: (b).
4. **"Undefined behavior" in decision 0253 item 14** (`records/docs/README.md` open item 6). Floating division by zero is undefined in the C++ standard and defined by GCC and Clang for IEEE 754 types. Options: (a) the supported compilers' behavior decides, and both texts move; (b) the standard's wording decides for `rotation`'s zero direction only, about 14 lines kept; (c) for both places, about 24 lines. Recommendation: (b).
5. **Plan 0048's definition-of-done item 4 against the pinned step counts** (`restated-targets.md` item 1). The `core.plan_cost_model` tests pin exact constant-evaluation counts and fail when one moves in either direction; B1, B5, B6 and C1c are on that path. Options: (a) a unit may change a pinned count in a separate, listed commit with a margin report; (b) item 4 holds literally, which probably closes B1 and B6. Recommendation: (a).
6. **Plan 0048's targets.** D0 measured the end state at about 6,120 lines of 8,410 (about 27% fewer) once every condition is met, where the plan said 5,050; comments end at about 1,800 lines where the plan said 870. Options: (a) accept and restate with M1; (b) a tighter pass to about 1,600 comment lines that cuts the generator's internal contracts. Recommendation: (a).
7. **The counting method for plan 0048's definition-of-done item 8.** Options: (a) `inventory.py`'s method, which is the scope of plan 0047's manifest (110 names, 153 declarations at namespace scope); (b) the matcher that counts each template twice (293). Recommendation: (a).
8. **The `blade_image` overload that takes a `basis_change`** has one test caller. Options: (a) change the test to pass `widened(change)` in its own commit, then delete the overload; (b) keep it. Recommendation: (a).
9. **The two antiproduct glyphs** (research note 0029, open item 4). No monospaced font on the development machine has them. Options: (a) as ruled, with a fallback note; (b) as ruled in running text, and `*` and `%` inside code and text blocks; (c) TeX. Recommendation: (b). The notation key is written for (a) until this is ruled.
10. **The human cold reader** (decision 0253 item 31, still open). The design now suggests two: one new to geometric algebra for chapters 1 to 4, and one who knows rotors for chapters 4 and 6 and the conventions card.
11. **The first-things page gains a tenth fact** (item B1's trap), though plan 0049 section 4.3 has it "unchanged in substance". Recommendation: accept with B1.
12. **The five medium rows of the hardening register** (A36, A09, A03, A22, A21) are in 1.0.0. Options: (a) H6 fixes them in plan order, each with its own ruling; (b) a fix is pulled into a patch release sooner. Recommendation: (a). None is high, H3 and H5 may still change the rows, and decision 0252 holds additions out of 1.0.1.
13. **The units of plan 0047 past M1** await the ruling on the remainder of decision 0253 item 1. With it come the six scope suggestions of the plan's addendum (the Sage rerun, the old mutation specs, the growth grid E3, `rotation` off this C library, DoD-5's diff, and the rows with no closing unit). The first hand-back recommended adding E3 to V7b; the addendum recommends waiting for the review of 2027-11-01. Recommendation: rule these with M1.
14. **Sources.** Rows A13, A14 and A16 of the gap register close only with Lengyel's book, Denavit and Hartenberg's paper, or the International Tables. Recommendation: stated limits unless the owner supplies them.

## C. For a later revision or plan; nothing changes now

1. **CONVENTIONS 3.2** defines the antiexomorphism only for diagonal metrics. V3 read it as left∘G∘right, which reproduces every pinned value. A sentence saying so would make every weight row derivable from the specification alone. For the owner at the next revision.
2. **CONVENTIONS 1.8.** "For a nondegenerate form this condition makes d = ±1" is right, and on a degenerate form the condition also holds with other d. A clause could say so at the next revision.
3. **A rigid(2) rotor about a point turns clockwise** in the value a test pins. CONVENTIONS covers only rigid(3). Input for the rga2 plan.
4. **Rows A39 and A50** of the hardening register need no ruling for H1's pins; their fixes are H6's.
5. **Plan 0047 section 12** still lists O1 to O13 as open; decision 0253 is the record of their rulings.

Status: in force. Part B is open.
