# 0241 Documentation lane, plan 0040 open items, and the release surface

Date: 2026-11-10. The owner answered "accept recommendations" to the reports on plans 0040 and 0041, the WU18 drafts, and the knowledge-base notes. Where the owner gave no further information, the plans' defaults apply.

Plan 0040:
1. **O1:** replace the first `TODO` with a plain statement of the limitation and defer the one-blade catch-all past 1.0. **O3:** the two test-only `apply` overloads take the attribute and are listed for the M7 audit, to delete after 1.0. **O2:** no knob unless the W10 numbers meet the plan's fixed criterion. Coordinator items K1 to K11 as the plan recommends.

Plan 0041 (items 1 to 12 of its report):
2. The replacement W6 and CONVENTIONS text of plan section 5.4 is ratified as written; the design study moves under it. The study's number is `0000`. CONVENTIONS' "most canonical reference" sentence stays for now and is decided at the release walkthrough.
3. **Byline:** the owner did not say who wrote the study, so the default stands: "author not recorded; added by the owner on 2026-10-31". The owner may still name the author.
4. **FetchContent URL:** the owner gave none, so the README carries a marked placeholder that the release commit fills; the release record lists it as a must-fill.
5. The CMake 4.4 minimum stays and the README says so. The tested example kinds are `cpp`, `cpp rejected` and the CMake blocks. `tools/readme_examples.py` becomes `tools/doc_examples.py` with labels `<module>.docs` (settles decision 0229 item 2). The docs lane fills only the inventory's doc-page column. A third bare-compile file that prints one value covers `format.hpp`. The first page is `docs/guide/README.md`. The README's change-of-basis section moves to the core guide.

Release surface (WU18 drafts):
6. **DoD-2:** plan 0001 section 3 is amended to list the real exports, by a decision at the release record, except that pure helpers with no user outside the tests (`has_name`, `same_rows`, `basis_change_like`, `index_of`, `sum_layout`, `max_generators`) move to `detail` unless CONVENTIONS names them; the coordinator checks that before the unit is planned. `GALOOT_ASSERT` is public and documented. `GALOOT_ALWAYS_INLINE` is not part of the interface.
7. **`bulk_dual`:** a short standalone exception decision beside 0141 is written at the M7 gate.
8. **Knowledge base:** source lines in notes are generic, and the repo settles one rule.
9. **`tools/timed`:** the unconfirmed dirty-mark gap (index differs from HEAD) is confirmed and fixed by the next tools unit.
