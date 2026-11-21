# 0260 Readings made while applying decision 0258, and what later units inherit

Date: 2026-11-21. Source: the units that applied decision 0258 to the comment table (plan 0048 D0) and to the documentation design records (plan 0049 U1), each checked independently three times, and the fix round and delta review of plan 0044. These are the coordinator's readings; the owner can reverse any of them.

## 1. Readings

1. **The `detail` ruling is in force now.** Item 2 of decision 0258 section 4 reads the present S11 as well as the amended one: its contract sentence does not apply inside a `detail` namespace. The 36 comment rows that waited only for that ruling wait for nothing. The no-drift condition of section 3 governs text that moves to the guide, and these rows do not move.
2. **Contract text waits wherever it sits.** "No unit removes a contract from a header" (section 3) covers any comment that is the only header statement of what a public declaration requires, guarantees or preserves, in a prologue or a `detail` comment as much as on the declaration. Text that a kept comment points to waits at least as long as the pointer. The comment table applies both; fourteen rows wait longer for it.
3. **Figure labels and alt text** write the two antiproducts in words. Item 9 of section 4 does not cover figures.
4. **The bulk-dual form of the sandwich** is stated in the guide only for R(p, q, 0), the diagonal forms whose generators square to 1 or −1, where research note 0029 proves it. A metric with squares (2, 1, 1) is a counterexample. It also held in conformal(3) on every versor tried, and a proof for forms whose Gram matrix is its own inverse looks short; until one is written and checked the guide does not claim it.
5. **Plan 0044's finding p1 is not fixed in 1.0.1.** A user who includes Galoot with `-I`, builds with `-Wundef -Werror`, and misspells `GALOOT_FORCE_INLINE` gets two errors, the first naming an internal macro. Three ways to fix it inside `assert.hpp` were probed and each changes something for valid builds (decision 0254, correction of this date). It becomes a row of the hardening register for H6.

## 2. What later units inherit

1. **Plan 0049's reference unit (U9)** states in its plan, before it starts: how checks 1, 3 and 4 treat a member or rejecting overload covered by its owner's entry; where the signature of a macro or of a member with its own entry comes from, since the manifest holds only declarations at namespace scope; the exact list of public macros that need entries, without the include guards; that pointers to concepts, limits and diagnostics headings each resolve to one heading; that check 3 removes only Markdown escapes; and that check 7 asks, for a requirement rejected at compile time or checked in constant evaluation, for a test that breaks it.
2. **The blind reviewer's checklist** gains the line of decision 0258 section 3 (a unit that changes a declaration's contract changes its entry in the same unit) in the unit that lands the first reference entries.
3. **The comment table's checker** gets a guarded list of the rows held as contract text, as it has for preconditions, when the table next changes; today a row set back to an earlier wait passes it.
4. **`tools/mutate`** classes a mutant that only a lint build or an `#error` catches as unjudged; two cases were seen in plan 0044. For the first unit after 1.0.1 that changes the tool.
5. **Plan 0048's phase 4** asks whether the two conventions that move off public declarations (`format.hpp:158-169`, `blade_set.hpp:624-629`) meet S11's "a convention is restated where it is applied" while they stay on the `detail` helpers only.

Status: in force.
