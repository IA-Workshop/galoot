# WU14's mutants, seeds, and coverage

Plan 0029 criteria 9 to 13. Run on 2026-11-08 by Claude (agent, Opus 5.5, implementer of parts C1 to C3) with `tools/mutate`, each build and test through `tools/timed`; the choices are decision 0217. The coverage table is `coverage.md`.

## Where each run ran

- **Tip:** the tests of `d5ffafd` (part B's commits cherry-picked; later commits of the unit change only records). `faults.toml` and `faults_identities.toml` ran in the unit's worktree; `identities.toml`, `new_cases.toml`, and `catalogue.toml` ran in a second worktree detached at `d5ffafd`, so that two runs did not share build trees. Rows marked `+dirty` differ from their revision only in records under `docs/agent/` (a spec file moved aside or extended); `include/`, `test/`, and `tools/` equal `d5ffafd` in every row.
- **Branch point:** `e076a8b`, the unit before part B (the suites as they were), in a worktree detached there, on gcc-debug only, with the specs in `out/branch-point/` (the tip's specs less the program `core_identities_rounding_test`, which did not exist).
- The job pool showed 23 of 23 slots free before the runs; other agents' builds may have shared it.

## DoD-14's faults (criterion 11)

`faults.toml`, judged by every test labeled `core`: all six killed on gcc-debug and clang-debug at the tip, and on gcc-debug at the branch point. The killers include correctness tests in every row, not only the cost canaries `core.plan_cost_model.*`, which fail on any change to a plan's evaluation steps.

`faults_identities.toml`, the same edits judged by the identity suites alone (informational), against research note 0024 E's prediction:

| Fault | Predicted | Tip, both presets | Branch point | What killed it |
| --- | --- | --- | --- | --- |
| `reorder_sign_class` | killed, by associativity and the defining relation | killed | killed | the library's own check that each complement's sign equals its closed form (`plan.hpp`, "unary map: a complement's sign differs from its closed form"), at compile time, and `core.transform_identities` |
| `right_complement_sign_flipped` | killed, by the De Morgan laws | killed | killed | the same compile-time check, and `core.transform_identities` |
| `last_term_dropped` | killed on the non-diagonal groups | killed | killed | the antiproducts' associativity, the dual and transform identities, and at the tip the products' associativity and the floating set |
| `one_coefficient_doubled` (g_12 in one loop) | killed | killed | killed | as above |
| `order_swapped` (e13 before e12) | survives | **killed** | **killed** | the library's own check that a product plan's generated terms are in canonical order (`plan.hpp`, "product plan: generated terms are out of order or repeated"), at compile time |
| `squares_doubled` (informational) | survives all but the defining relation | **killed** | **killed** | the dual identities (the bulk and G𝔾 = det g read the Gram matrix themselves), the transform identities, and at the tip also the defining relation, the basis proof, and the floating set |

Two predictions failed, both for the same reason: the prediction counted only the identities' comparisons, and two other checks see the edits. The library's self-checks of its generated plans and maps run at compile time in every program that builds a plan. These are the internal messages the catalogue lists as unreachable by user input, and they rejected the swapped order and the wrong signs. And the dual suite computes G from the Gram matrix apart from the products, so a change of every square is not "another metric" to it; the transform suite failed too, for a reason not traced here.

## The gaps of section 5.2 (criterion 12)

`identities.toml`, judged by the identity suites:

| Mutant | Gap | Branch point (gcc-debug) | Tip (both presets) | Counts |
| --- | --- | --- | --- | --- |
| `gap_squares_doubled` | criterion 2 | killed by the dual and transform suites; no case of `core.identities` failed | killed; in `core.identities` by the defining relation and the basis proof | only as a gap of `core.identities` |
| `gap_offdiagonal_dropped` | criterion 2 | as above | as above | only as a gap of `core.identities` |
| `gap_vector_contraction_doubled` | criteria 1, 2 | killed, `core.identities` among the killers | killed | no |
| `gap_null_pair_wedge_sign` | criterion 5 | killed, `core.identities` among the killers | killed | no |
| `gap_mixed_nulls_wedge_sign` | criterion 5 | survived | killed by `core.identities` (the antiproducts' and products' associativity, the product rules, the basis proof on the drawn forms) | **yes** |
| `gap_floating_products_perturbed` | criterion 8 | survived | killed by `core.identities_rounding` | **yes** |

Gaps without a counting mutant, and why:

- **Criterion 1** (associativity of ⟑ and ∧) and **criterion 3** (the involutions over ∧, ⟇, and ∨): these laws follow from laws the branch point already tested. The complements turn ∧ into ∨ and ⟑ into ⟇ (the De Morgan laws, tested on every group), so ∧ is associative exactly when ∨ is, on the complemented layouts. The rules over the antiproducts follow from the grade involution over ⟑ and x̌ = (−1)^n x̂. So a mutant that breaks one of them breaks a law the old suites test. Part B's cases cover new layouts and give a direct proof on the full layouts (P); they add no kill power over the old suites.
- **Criterion 4** (the basis proof): a fault on any pair of basis blades also shows in random multivectors whose coefficients on those blades are nonzero, which the draws make almost certain. The proof turns a sample into a proof (V1 kind 1); it does not add kills.
- **Criterion 2** counts only if `core.identities` is judged alone: at the branch point, the dual and transform suites already killed both of its mutants.

Each test case part B added kills at least one mutant on both presets. The products' associativity, the defining relation, the basis proof, and the floating set kill mutants of `identities.toml`. The mirror images kill `squares_doubled` in `faults.toml`. `new_cases.toml` covers the cases that no other mutant reached, all killed on both presets:

- `grade_involution_grade_three`, killed by "the involutions follow their antiproduct rules over many forms" among others;
- `bulk_dual_reverse_dropped`, killed by "the four duals of every blade are the reference's images" among others;
- `identity_margin_zero` and `identity_equality_rule_dropped`, killed by the identity check's two `support.tracked` cases.

## The catalogue tool's mutants

`catalogue.toml`: the nine mutants of the scan's old record (`records/mutations/0022-format/`), ported, and seven for the tool's new parts. All 16 killed on gcc-debug by `tools.catalogue` and `tests.messages_pinned`.

## Seeds (criterion 13)

`seeds/`: `core_identities_test`, `core_dual_identities_test`, `core_transform_identities_test`, and `core_identities_rounding_test`, each with `--rand-seed=1` to `50`, on gcc-debug at `d5ffafd`, rows in `seeds/runs.tsv` (a campaign log) and `seeds/results.tsv`.

- Three programs pass all 50 seeds.
- **`core_transform_identities_test` aborts on seeds 8, 23, 27, 37, 38, 39, and 43** (7 of 50, logs `seeds/core_transform_identities_test-<seed>.log`). Each abort is the test helper's exact rational overflowing `std::int64_t` ("rational: a result does not fit std::int64_t"). A backtrace at seed 8 places it in the outermorphism check at grade 4, λ³ T(a₁ ∧ a₂ ∧ a₃ ∧ a₄) = T(a₁) ∧ … ∧ T(a₄), over one of the test's random dense forms of four generators. That is a limit of the test's arithmetic, not a fault of the library: the test predates WU14, and seed 0, which every preset runs, does not reach it. Reported (W9); the fix belongs to the test's author.

Run times on gcc-debug (median of 50): `core_identities_test` 16.3 s, `core_identities_rounding_test` 0.13 s, the other two under 0.05 s. Under gcc-asan (-O1), `core.identities` took 3.4 s, so its exhaustive loop stays in its source.
