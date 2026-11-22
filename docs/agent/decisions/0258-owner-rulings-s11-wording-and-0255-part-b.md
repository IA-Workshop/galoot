# 0258 The owner's rulings of 2026-11-21: the S11 wording, with no drift, and part B of decision 0255

Date: 2026-11-21. Source: the owner's message of 2026-11-21, "0044 - Agree / S11 - Agree, but ensure that there is no drift / 0255 - Agree", answering the coordinator's explanation of plan 0044, of the S11 amendment, and of decision 0255.

## 1. Plan 0044

The plan stands as explained, with item 7's second half as decision 0257 ruled it. Nothing new is ruled.

## 2. The S11 amendment: the wording

Ratified by the owner, to replace the third sentence of STYLE S11's first paragraph:

> Each declaration in a public header, outside a `detail` namespace, has a `///` comment immediately before it that says in one sentence what the declaration is, in terms a user of the library can read without the project's records. Each declaration that has an entry in the guide's reference also names the guide page and heading that hold its contract: what it requires, guarantees, and preserves. A test checks that each such pointer resolves. A precondition whose violation is undefined behavior in a build without checking stays on the declaration in full.

This supersedes the prepared wording of decision 0253 item 14. It carries the rulings of items 2 and 3 of decision 0255 part B.

## 3. The S11 amendment: no drift

The owner's condition: the header and the guide must not drift apart. The coordinator reads it as follows; the owner can change the list.

The wording is applied to STYLE.md, and a contract leaves a header, only when every check below exists as a test that runs at each merge into `main`. Plan 0049's reference unit (U9) and plan 0048's phase 4 design and build them; their plans state each as an acceptance criterion before either unit starts.

1. **Pointers resolve.** Each pointer in a header names a guide page and a heading that exist.
2. **Coverage both ways.** Each public declaration in the interface manifest has a reference entry, or is a member or rejecting overload covered by its owner's entry under a stated rule. Each reference entry names a declaration that exists.
3. **One sentence, one text.** The sentence on a declaration and the opening sentence of its reference entry are the same text; a test compares them.
4. **Signatures agree.** The signature a reference entry prints is the declared one, compared by the tool that writes the manifest.
5. **Preconditions kept in full read the same.** A precondition that stays on a declaration appears in its entry in the same words.
6. **Messages agree.** Each diagnostic text a guide page quotes is compared with the literal in the header.
7. **Stated contracts are exercised.** Each requirement an entry states with a checkable violation names a compiled example or a compile-fail test that exercises it.

Two process rules go with the tests. A contract moves out of a header only after the page that receives it has passed review (decision 0253, already in force). A unit that changes what a declaration requires, guarantees or preserves changes its entry in the same unit (W12), and the blind reviewer's checklist asks for it.

Until the checks exist, S11 stands as ratified on 2026-11-01 and no unit removes a contract from a header. The reference pages are in plan 0049's third milestone (D3), which is not yet authorized.

## 4. Decision 0255 part B

Each item is ruled at the coordinator's recommendation. Part A of that decision stands.

1. **`transform` and the classical sandwich.** Documentation now (the warning in chapter 4, the bridge in chapter 6, the first-things fact, the conventions card row). A sentence in `transform`'s `///` comment when that comment is next changed. A named classical operation is the first candidate of the backlog for a later minor release.
2. **S11 and `detail`.** The contract sentence does not apply inside a `detail` namespace (section 2).
3. **Pointer placement.** One pointer on each declaration that has a reference entry (section 2).
4. **"Undefined behavior" in the exception.** The C++ standard's sense decides for `rotation`'s zero direction: `motions.hpp:48-61` stays in full. The unchecked floating division of `linear.hpp:397-406` follows the supported compilers and its text moves.
5. **Pinned constant-evaluation counts.** A unit of plan 0048 may change a pinned count in a separate commit that the review request lists, with the guide's figures and a report of the margin before and after. Definition-of-done item 4 of plan 0048 is read so.
6. **Plan 0048's targets.** Accepted as D0 measured them: about 6,120 lines and about 1,800 comment lines once every condition is met. They are restated with M1.
7. **Counting the interface.** By `inventory.py`'s method, the scope of plan 0047's manifest.
8. **The `blade_image` overload that takes a `basis_change`.** Its test changes to pass `widened(change)` in a commit of its own, then the overload is deleted, in plan 0048's unit A1.
9. **The antiproduct glyphs.** The symbols in running text; `*` and `%` inside code and text blocks.
10. **Human cold readers.** Still optional and still open: the owner has named nobody.
11. **The first-things page** gains a tenth fact, the `transform` trap of item 1.
12. **The five medium rows of the hardening register** are fixed in H6, in plan order, each with its own ruling.
13. **The units of plan 0047 past M1**, and the six scope suggestions of the plan's addendum, are ruled with M1.
14. **Rows A13, A14 and A16 of the gap register** are stated limits unless the owner supplies the sources.

Status: in force.
