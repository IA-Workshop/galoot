# 0265 The documentation tooling merges into its feature branch, and unit U3 starts, while the release waits for the owner

Date: 2026-11-21. Source: the hand-backs of plan 0049 unit U2 (two implementation steps, a blind review, three fix rounds and three delta reviews; the last, `records/reviews/2026-11-21-plan-0049-u2-delta-review-3.md` on `review/docs-tooling` at `ef48b1d`, approves `feature/docs--tooling` at `243ed65`), and the state of release 1.0.1: unit R10 is done, `main` holds the records tip T `75b9a03` with records only after it, and what remains is the owner's (the walkthrough, the close-out script, the push). Each ruling is a reading within the plans and the owner's rulings (decisions 0253 and 0258); the owner can reverse any.

1. **Decision 0261 item 3 is narrowed.** It held `feature/docs--tooling` out of `main` and out of its feature branch until 1.0.1 is tagged, so that the run-log compaction would not meet an open branch. The compaction is made (decision 0263; the release record's trial merge brought back no removed row). What still holds is the reason that matters for the tag: nothing outside `docs/agent/` reaches `main` before the owner has tagged, since the close-out script refuses such a commit. So the tooling branch now merges into `feature/docs`, not into `main`: `main` is merged into the tooling branch first, `tools/runlog check` runs (with `tools/runlog prune` and its printed count committed if a removed row is present), the review records of `review/docs-tooling` are cherry-picked with their run-log rows counted (lesson 0077), the checks PERFORMANCE P9 asks of a subfeature merge run on the result, and the check passes again on the merged tree. `feature/docs` reaches `main` only at the end of plan 0049 (its unit U13), long after the tag.

2. **Unit U3 of plan 0049 starts** on a subfeature of `feature/docs` once item 1's merge is made. Decision 0253's order lets U3 start after plan 0044's U5 is in `main`, which it is (`c6e3b18`).

3. **The coordinator commits nothing to `main` from this decision until the owner has tagged 1.0.1 or says otherwise**, so that the commit the owner's dry run names is the commit the script merges. Cost rows and records of the documentation units are committed on the documentation branches meanwhile.

4. **The fixer's edits to plan 0049's "Done when" cells of U3, U9 and U10 are confirmed** (blind review of U2, findings m2, M1 and n2; delta review 1, note n-c): the three fixed paths fail, not note, once their pages are due; the reference check reads each entry's Name field and its `name_undocumented` mutant is killed on the real reference before the manifest is passed; U10 widens the diagnostics check to every guide page or records which it leaves out.

5. **What U3 inherits from U2's open items**, each settled at the recommendation of the hand-back that raised it:
   - The glossary has a small table of words with an everyday meaning ("point", "line", "plane"), which the terms check exempts until their defining chapter; a glossary row is added when its defining page lands, never ahead of it.
   - A missing page kind fails, not notes, through an option the build file passes once a page of that kind is due (`--require`), added in the unit that writes the first page of each kind; U3 adds it for the kinds whose pages it creates.
   - The `pages` mode refuses a page registered twice with examples under two names.
   - The notation key's check of characters outside ASCII on user pages is built in U3, since U3 creates the pages it reads.
   - The two test gaps the last delta review left (the column of a paragraph's first line; the `+` and `1)` list markers) are closed in U3, tests first, with their mutants.
   - Left as stated limits, with their reasons in `records/mutations/0049-docs/README.md`: a line holding only `>` does not end a code span; alternative text that holds brackets is refused as an image by reference; a raw string literal that spans lines is not skipped by the pointer check.

6. **What later units inherit**, unchanged from the hand-backs: U5 decides the figure labels' color before the first figure is committed (labels in the ink color, the palette for strokes and fills, and one figure looked at in the viewers that matter); U9 marks an entry's name with a fixed label line, set in template T4 with the change to the check; U10 revisits the similarity cutoff of the diagnostics check on real pages.

Status: in force.
