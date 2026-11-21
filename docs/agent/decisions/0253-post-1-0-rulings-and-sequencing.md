# 0253 Rulings for plans 0047, 0048 and 0049, and how the three run together

Date: 2026-11-21. Source: the owner's message of 2026-11-21, "Accept all recommendation and proceed", answering the coordinator's reports of that day on the three plans and the two research cycles.

Each item is ruled at the option the report or the plan recommended. Options are quoted in short; the full text is in each plan's owner section (0047 section 12, 0048 section 12, 0049 section 14). Where the coordinator's report and a plan gave different recommendations, the report's is the one the owner answered, and the item says so.

## Plan 0047 (completion, hardening, verification)

1. **O1, extent.** Options: (a) all of it, 7M to 11M tokens; (b) phase 0 and the safety net to M1, the rest ruled when the registers exist; (c) only the safety net. Ruled: (b).
2. **O2, order against plan 0048.** Options: (a) no change under `include/` by plan 0048 before M1; (b) run together. Ruled: (a).
3. **O3, placement search.** The reading (decision 0227 item 2, option B) was put to the owner for confirmation and accepted with the rest; it stands unless the owner says otherwise. Options: (a) deferred with triggers; (b) implemented now; (c) dropped. Ruled: (a).
4. **O4, `witt_change`.** Options: (a) a test helper and a guide example; (b) a public factory in 1.1.0; (c) dropped. Ruled: (a).
5. **O5, M6 (cga3).** Options: (a) its own later plan, this plan doing only K1; (b) in this plan; (c) no probe. Ruled: (a).
6. **O6, the mutation sweep.** Options: (a) every mutant of `bits.hpp`, `blade_product.hpp` and `plan.hpp`, about 40 sampled per other header; (b) every mutant everywhere; (c) a sample everywhere. Ruled: (a). The sweep is past M1, so it starts only on the ruling of item 1's remainder.
7. **O7, fuzzing and coverage tools.** Options: (a) seeded doctest programs and the compilers' own coverage tools, no new package or preset; (b) also a coverage-guided fuzz target; (c) a twelfth preset. Ruled: (a).
8. **O8, stability policy.** Options: (a) a decision: the manifest test is the gate, removal only in a major release after a deprecation; (b) also in W13; (c) the manifest only. Ruled: (a).
9. **O9, generators 17 to 32.** Options: (a) H2 tests sparse use at 17, 24 and 32; (b) the guide says untested; (c) lower the bound to 16. Ruled: (a), falling back to (b) as the plan says.
10. **O10, the deferred cost evidence.** Options: (a) in phase 3, after plan 0048's kernel-affecting units; (b) now and again after; (c) its own plan. Ruled: (a). Plan 0048's unit M0 takes the baseline its own gates compare against.
11. **O11, the release that carries the work.** Options: (a) none planned here, a wrong-result fix ships as a patch; (b) a 1.0.2 after M3. Ruled: (a).
12. **O12, model level.** Options: (a) Opus for all, with the second-specialist sample check; (b) Fable for H0 and V3. Ruled: (a), moving to (b) if a sample check finds an error.
13. **O13, the readings of section 1.** Ruled: as read.

## Plan 0048 (simplification)

14. **O1, the `///` contract (the same item as plan 0049's O3).** Options: (a) STYLE S11 as it is; (b) one sentence per public declaration with a pointer to the guide heading that holds the contract, checked by a test; (c, plan 0049 only) no `///` at all. Ruled: (b), with the exception the plan states: a precondition that is undefined behavior when broken in an unchecked build stays on the declaration in full. This is the owner's ruling on the substance of an amendment to S11. Prepared wording, to replace S11's third sentence when the reference pages and the pointer test exist (plan 0049 U9, plan 0048 phase 4), and put to the owner for confirmation before it is applied, as decisions 0207 and 0213 did: "Each declaration in a public header has a `///` comment immediately before it that says in one sentence what the declaration is, in terms a user of the library can read without the project's records, and names the guide page and heading that hold its contract: what it requires, guarantees, and preserves. A test checks that each such pointer resolves. A precondition whose violation is undefined behavior in a build without checking stays on the declaration in full." Until then S11 stands as ratified and no unit removes a contract from a header.
15. **O2, the diagnostic families.** Options: A keep; B the same literals stamped by macros; C one literal per fault, checked once. Ruled: C. About 50 literals change, in units E1 and E2, after M1 and G1.
16. **O3, the kernel algebra.** Options: (a) a spike, then decide on its record; (b) skip; (c) commit now. Ruled: (a).
17. **O4, the generator's self-checks.** Options: (a) keep every check and merge their structure; (b) move them to the tests. Ruled: (a).
18. **O5, a 2.0 batch.** Options: (a) none; (b) deprecate in 1.1.0, remove in 2.0.0. Ruled: (a).
19. **O6, `modernize-avoid-c-style-cast`.** Options: exclude it in `.clang-tidy` with its reason, or keep the markers. Ruled: exclude, after B9 confirms that every present report is the false one.
20. **O7, the version.** Ruled: one 1.1.0 after G3, not a release per phase.
21. **O8, the compile-cost criterion.** Options: (a) report-and-accept at 1% instructions and 2% peak memory on the named units; (b) track only. Ruled: (a). It sets no bound.
22. **O9, Fable for C0's design.** Options: Fable for the design note only, or Opus with a second specialist's read. Ruled: Fable for the design note only (the plan's recommendation; the coordinator's report offered it "if you want it" and the owner accepted without excluding it).

## Plan 0049 (user documentation)

23. **O1, extent.** Options: (a) all three milestones; (b) D1 and D2, with D3 estimated again from actual costs; (c) D1 only. Ruled: (b).
24. **O2, layout.** Options: (a) subdirectories of `docs/guide/` by kind; (b) the five pages kept with new ones beside them; (c) directories under `docs/`. Ruled: (a).
25. **O3.** Item 14.
26. **O4, the conventions chapter.** Options: (a) a restatement kept in step by a trace; (b) user pages may link the specification. Ruled: (a).
27. **O5, notation.** Options: (a) Unicode symbols and words; (b) TeX. Ruled: (a).
28. **O6, figures.** Options: (a) about fourteen SVG figures drawn by one standard-library script, with a staleness test; (b) SVG by hand; (c) text diagrams. Ruled: (a).
29. **O7, the reference.** Options: (a) by hand with coverage checks; (b) generated by an owned script; (c) Doxygen. Ruled: (a).
30. **O8, where the examples run.** Ruled: (a) every example in every configuration until U2 reports the measured cost.
31. **O9, a human cold reader.** Options: (a) language-model readers only; (b) also one person at D2. Ruled: (b) if the owner names a reader by D2; otherwise (a). Open: the name.
32. **O10, the Fable level.** Options: (a) Opus throughout; (b) Fable for U1 only. Ruled: (b), on the same reading as item 22.
33. **O11, `techlint.py` and `pedlint.py`.** Not found on the machine and the owner has not said where they are. Ruled: (b), in-repository checks only, until the owner does.
34. **O12, the release that carries the pages.** Options: (a) a documentation-only patch release when D2 is in; (b) hold for the next minor release. Ruled: (a).
35. **O13, operations the tutorial misses.** Options: (a) teach around them and record the list; (b) add some first. Ruled: (a).

## The research cycles and the tools

36. `kb/doc-pedagogy`, `kb/complexity-reduction` and `tools/complexity-reduction` each get one independent review, then merge.
37. Saving open-access PDFs through the fetch tool, after the `curl` batch was refused, is accepted; paywalled primaries are not sought.
38. The wrapper-collapse experiment and the investigation of Clang's constant-evaluation step margin run on a copy of Galoot before plan 0048 relies on either.
39. Of the proposed packages, only `include-what-you-use` is asked for, since header work is planned. The install is the owner's action.

## How the three plans run together

The plans were written without sight of each other. These points bind where their texts differ.

- **Order.** 1.0.1 (plans 0043 to 0045) first. Beside it, records only: plan 0047 phase 0 (V1, V3, H0), plan 0048 phase 0 (D0), plan 0049 U1. After the 1.0.1 tag: plan 0047 phase 1 to M1. After M1: plan 0048 phase 1. Plan 0049's U2 may start once plans 0043 and 0044 are in `main`; U3 starts after plan 0044's U5 is in `main`.
- **Messages.** Plan 0049 section 7 item 4 ("every message literal, unchanged") is read as: unchanged except by plan 0048's E1 and E2 under item 15. Plan 0049's U10 (in D3) is written after E1 and E2 merge, or revised with them. Plan 0047's H1 pins the literals as they stand; E1 and E2 change those expectations in their own commits (W9).
- **Comments.** Plan 0048's phase 4 moves a contract out of a header only after the receiving page of plan 0049 has passed review. D3 holds the reference (U9) and is not yet authorized, so phase 4's moves wait for a ruling on D3; deletions that move nothing do not.
- **Hand-overs, each with a named owner (lesson 0074).** D0 (plan 0048) writes `records/docs/comment-moves.tsv` for plan 0049 and the candidate list for plan 0047's H0. V3, H3, H5 and K1 (plan 0047) hand plan 0049 the records named in plan 0047 section 9.3. Plan 0047's C1 writes the evidence table plan 0049 section 8 asks for.
- **Figures to check.** The complexity cycle measured 110 public names and 293 declarations where plan 0048 says 97 and 263, 2.78% duplication by a clone detector where plan 0048 says 25.6% of code tokens in repeated windows, and, by a keyword heuristic, about three quarters of comment lines as contracts or reasons. D0 measures these again and restates plan 0048's targets before phase 1; the restated targets go to the owner with M1.
- **The step margin.** Clang's constant-evaluation step margin on the product tests was measured at 1.49. A unit of plan 0048 that touches the generator reports the margin before and after.

## Not ruled here

Plan 0046 (Garnet) is merged as a record; its owner items stay open and belong to the Garnet repository's own sessions.

Status: in force.
