# 0217 WU14's integration and evidence: the implementer's choices

- Status: recorded. The implementer's choices in plan 0029 parts C2 and C3, open to the blind review and to the owner (C18). The owner's rulings they rest on are decision 0216.
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of plan 0029 parts C1 to C3
- Number: 0217, the next free number on every branch when recorded.
- Bears on: `test/CMakeLists.txt`, `records/convention-trace.md`, CONVENTIONS 1.16, `records/diagnostics/misuse-catalogue.md`, `records/mutations/0029-identities/`, `records/performance/wu14-identities-cost/`

## Context

Part B's eleven test commits were cherry-picked into `feature/core-identities` in order, with `-x`, each passing when its author committed it. Part C2 integrates them (labels, trace rows, the catalogue, the coverage table, section 8 of CONVENTIONS); part C3 produces the evidence of plan 0029 sections 8 and 10. The plan leaves several details to the implementer.

## Decision

1. **Labels.** The test author's registration of `core.identities_rounding` stays: the module and component labels, and `core.product`, `core.unary`, `core.transform`, `core.rounding`, since it tests products, complements, and transforms over a rounding scalar. It is not labeled `slow`: it runs in 0.13 s (median of the seed campaign's 50 runs on gcc-debug) and compiles in about 5.2 s on GCC and 4.2 s on Clang (`records/performance/wu14-identities-cost/`), less than several unlabeled sources. `core.identities` takes about 16 s on gcc-debug and 3.4 s on gcc-asan (-O1), well under the two minutes at which its exhaustive loop would move to a source of its own, so it stays in one program. `core.identities` keeps `slow`; its comment now names what part B added. No other test changed labels.
2. **Trace rows.** Nine facts that part B's markers state join the table: three of 2.1 (the antigrade involution), two of 3.2 (the diagonal bulk, the blades in neither part), three of 4.2 (the De Morgan law of ⟑, the associativity of all four products, the sign (−1)^n of the grade involution over the antiproducts). The rule of criterion 3 goes under 4.2, the item of the antiproducts, since CONVENTIONS gives the product rules no item of their own. Plan section 6's 4.1 clause (the wedge as the top grade of ⟑) gets no row: the marker that states it writes the grade as |A| + |B|, and a fact may not contain `|`; 4.1 already has three facts with tests.
3. **CONVENTIONS 1.16** names the two section 8 tests by case name and file, as the 1.14 flector row does, in research note 0024 D3's wording, with no line numbers.
4. **The catalogue** is regenerated with `tools/catalogue report` over `include/galoot/`; the rga3 preset's six messages get a section after the core's rejections, outside DoD-19's nine items.
5. **The coverage table** is `records/mutations/0029-identities/coverage.md`, with the record of this unit's mutants, and names test cases by text, not line.
6. **Gap mutants (criterion 12) are judged by the identity suites alone**, `core.identities`, `core.identities_rounding`, `core.dual_identities`, and `core.transform_identities`, at the branch point (`e076a8b`, the unit before part B) and at the tip. The gaps of section 5.2 are gaps of those suites; judged by the whole core suite, the golden product tables at the branch point kill any change to a product, so no gap mutant could count. The branch point runs on gcc-debug only; a mutant the branch point lets survive on one compiler is a survivor there, and the tip runs on both.
7. **DoD-14's faults** run on both presets at the tip and on gcc-debug at the branch point, judged by every test labeled `core`. The second run, judged by the identity suites alone, runs at both points and is informational (criterion 11). A sixth fault, every square doubled, is added beside the five as informational: it is the consistent change of research note 0024 E's fourth row, which only a comparison with the algebra's own Gram matrix can see.
8. **The seed campaign** runs the four identity programs (`core_identities_test`, `core_dual_identities_test`, `core_transform_identities_test`, `core_identities_rounding_test`) with `--rand-seed=1` to `50` on gcc-debug, each through `tools/timed` into the campaign's own log.
9. **The scan's old mutants** (`records/mutations/0022-format/scan_mutants.py`, judged by the removed `--self-test`) are ported to `catalogue.toml` for `tools/mutate`; the script is removed and 0022's README says where they went.
10. **Mutants added after a run, each confirmed before it counts.** The branch point killed `gap_null_pair_wedge_sign`, so `gap_mixed_nulls_wedge_sign`, a fault only the drawn forms can show, was added to `identities.toml` and run at the branch point (it survived) before the tip. `new_cases.toml` holds four mutants written so that every case part B added kills one (criterion 12's first clause); it runs at the tip only. The second worktree that ran `identities.toml` and `new_cases.toml` at the tip shares no build tree with the unit's.
11. **Not done in C3: the parts of the test-quality unit that O4 folded in** (the bits and blade-set mutation specs, the two warning-only blade_product mutants rewritten, the flector plane hand value and the even-sign mutant). The coordinator's brief for C3 named the DoD-14 faults, the gap mutants, and the scan's mutants; these stay open for the unit before its review (owner item).

## Options weighed

- For item 6, judging gap mutants by the whole core suite: the golden tables pin every product, so no product mutant survives the branch point and criterion 12 could not be met for any gap; judging by the identity suites shows what part B's tests add to those suites, which is the gap the plan named.
- For item 2, a fact for 4.1 reworded without `|`: it would need a marker change in a test the unit did not write (W9), for a clause already pinned.

## Consequences

- The results are in `records/mutations/0029-identities/README.md` and the plan's outcome.
