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
| `squares_doubled` (informational) | survives all but the defining relation | **killed** | **killed** | the dual identities (the bulk and G𝔾 = det g read the Gram matrix themselves), the transform identities (corrected below: that suite's failure was its test rational overflowing, not a kill), and at the tip also the defining relation, the basis proof, and the floating set |

Two predictions failed, both for the same reason: the prediction counted only the identities' comparisons, and two other checks see the edits. The library's self-checks of its generated plans and maps run at compile time in every program that builds a plan. These are the internal messages the catalogue lists as unreachable by user input, and they rejected the swapped order and the wrong signs. And the dual suite computes G from the Gram matrix apart from the products, so a change of every square is not "another metric" to it; the transform suite's failure was its test rational overflowing on the mutant's values, not a kill (see "The overflow of `core_transform_identities_test`" below), and under its bounded draws that suite alone lets `squares_doubled` and `gap_squares_doubled` survive; both are now declared equivalent there, since a doubled square is another metric, in which the transform identities hold.

## The gaps of section 5.2 (criterion 12)

`identities.toml`, judged by the identity suites:

| Mutant | Gap | Branch point (gcc-debug) | Tip (both presets) | Counts |
| --- | --- | --- | --- | --- |
| `gap_squares_doubled` | criterion 2 | killed by the dual suite (and by the transform suite only through its rational's overflow, corrected below); no case of `core.identities` failed | killed; in `core.identities` by the defining relation and the basis proof | only as a gap of `core.identities` |
| `gap_offdiagonal_dropped` | criterion 2 | killed by the dual suite and by the transform suite's guard against passing vacuously, `CHECK(accepted > 0)` in `transform_identities_test.cpp`: with the off-diagonal entries dropped, the two Witt forms are totally null, so no versor has λ ≠ 0 (not an identity's failure and not overflow, as the blind review found; under the bounded draws the guard still kills it, in both cases of that suite); no case of `core.identities` failed | killed; in `core.identities` by the defining relation and the basis proof | only as a gap of `core.identities` |
| `gap_vector_contraction_doubled` | criteria 1, 2 | killed, `core.identities` among the killers | killed | no |
| `gap_null_pair_wedge_sign` | criterion 5 | killed, `core.identities` among the killers | killed | no |
| `gap_mixed_nulls_wedge_sign` | criterion 5 | survived | killed by `core.identities` (the antiproducts' and products' associativity, the product rules, the basis proof on the drawn forms) | **yes** |
| `gap_floating_products_perturbed` | criterion 8 | survived | killed by `core.identities_rounding` | **yes** |

**Criterion 12's second clause is not met as written.** It asks, for each gap, for a mutant that the branch point lets survive and the tip kills. That holds for two gaps (criteria 5 and 8), and for criterion 2 only when `core.identities` is judged alone. For the others, the argument below takes its place; the blind review found it sound, and recommended that later plans judge a gap's mutants by the suite the gap extends.

Gaps without a counting mutant, and why:

- **Criterion 1** (associativity of ⟑ and ∧) and **criterion 3** (the involutions over ∧, ⟇, and ∨): these laws follow from laws the branch point already tested. The complements turn ∧ into ∨ and ⟑ into ⟇ (the De Morgan laws, tested on every group), so ∧ is associative exactly when ∨ is, on the complemented layouts. The rules over the antiproducts follow from the grade involution over ⟑ and x̌ = (−1)^n x̂. So a mutant that breaks one of them breaks a law the old suites test. Part B's cases cover new layouts and give a direct proof on the full layouts (P); they add no kill power over the old suites.
- **Criterion 4** (the basis proof): the step from basis blades to all multivectors rests on each law being linear in each operand and on the implementation being bilinear in the coefficients, which straight-line kernels with no branch on a coefficient's value are. Then a fault on any pair of basis blades also shows in random multivectors whose coefficients on those blades are nonzero, which the draws make almost certain. The proof turns a sample into a proof (V1 kind 1); it does not add kills.
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
- **`core_transform_identities_test` aborts on seeds 8, 23, 27, 37, 38, 39, and 43** (7 of 50, logs `seeds/core_transform_identities_test-<seed>.log`). Each abort is the test helper's exact rational overflowing `std::int64_t` ("rational: a result does not fit std::int64_t"). A backtrace at seed 8 places it in the outermorphism check at grade 4, λ³ T(a₁ ∧ a₂ ∧ a₃ ∧ a₄) = T(a₁) ∧ … ∧ T(a₄), over one of the test's random dense forms of four generators. That is a limit of the test's arithmetic, not a fault of the library: the test predates WU14, and seed 0, which every preset runs, does not reach it. Reported (W9) and fixed by the test author: see "The seed failures fixed, and the test-quality items folded in" below; all 50 seeds pass under the bounded draws.

Run times on gcc-debug (median of 50): `core_identities_test` 16.3 s, `core_identities_rounding_test` 0.13 s, the other two under 0.05 s. Under gcc-asan (-O1), `core.identities` took 3.4 s, so its exhaustive loop stays in its source.

## The seed failures fixed, and the test-quality items folded in

Run on 2026-11-08 by Claude (agent, Opus 5.5, test author) on `feature/core-identities--tests2` from `f09007f`, each build and test through `tools/timed`. The test commits are `6abeef2` (the seven seeds as a case, failing on purpose), `a3aa7c7` (the bound), and `a1546f9` (the plane case); every mutant row below is at `a1546f9`, except seven rows of `transform_bound.toml` at `a3aa7c7`; the two revisions differ only in the plane case of `rga3.objects`.

### The overflow of `core_transform_identities_test`

- **Cause.** A backtrace at seed 8 places the abort in the wedge of the four images T(a₁) ∧ … ∧ T(a₄) for a versor of three hyperplanes over the random form of four generators (`[[-2, 0, -2, 2], [0, -2, -3, 3], [-2, -3, 1, 3], [2, 3, 3, 3]]`): a product of 4.6 × 10⁵ and 1.1 × 10¹⁶. Each side of that identity is a polynomial of degree 24 in the hyperplanes' entries and 4 in the vectors'.
- **Measured.** `seeds/bounded/probe/`: a copy of the test at `f09007f` with the two bounds taken from macros, over a 128-bit copy of the test rational that records the most bits any value needs (`make_probe.py` says how to build and run it). Seeds 0 to 2000, gcc at `-O1`:

  | Hyperplanes, vectors | Bits needed: least, median, most | Seeds needing more than 63 |
  | --- | --- | --- |
  | −2..2, −2..2 (the test at `f09007f`) | 47, 59, 76 | 400 of 2001; of seeds 1 to 50 exactly the campaign's seven; seed 0 needs 62 |
  | −1..1, −2..2 (the fix) | 28, 40, 56 | none; seed 0 needs 37, the seven 32 to 43 |
  | −1..1, −1..1 | 25, 37, 54 | none |

  The probe's prediction for seeds 1 to 50 matches the campaign, which checks it against the real rational.
- **Fix** (`a3aa7c7`): the hyperplanes are drawn from −1 to 1, the vectors and multivectors from −2 to 2 as before. Bounding only the hyperplanes removes 19 bits at the median, and leaves the vectors' range alone. The margin is 7 bits over 2001 seeds; it is a sample, not a bound (V9), and a seed past it would abort loudly, never pass wrongly, since the rational aborts rather than rounds.
- **Regression seeds** (`6abeef2`): a second test case runs the identities at seeds 8, 23, 27, 37, 38, 39, and 43, whatever `--rand-seed` says. Under the new draws these seeds give other values than the ones that overflowed, so the case guards that the bound keeps them in range, not the old inputs; the old inputs cannot be run on a 63-bit rational at all. `6abeef2` fails ("rational: a result does not fit std::int64_t" in the new case); `a3aa7c7` passes.
- **Campaign** (`seeds/bounded/`, `runs.tsv` a campaign log and `results.tsv`): `core_transform_identities_test --rand-seed=1` to `50` at `a3aa7c7` on gcc-debug, **all 50 pass**, two cases and about 10,700 assertions each, 0.38 s at most. The other three programs are unchanged since the first campaign and were not rerun (P10). No further failure was found.
- **Kill power** (`transform_bound.toml`, judged by `core.transform_identities` alone, gcc-debug): of the nine mutants that suite killed at run time at `d5ffafd`, seven are still killed. The other two, `squares_doubled` and `gap_squares_doubled`, now survive and are declared equivalent: every square doubled is another metric, in which the transform identities hold. Their earlier "kill" was the overflow: the test at `f09007f` built against headers with that edit aborts at seed 0 on "rational: a result does not fit std::int64_t" after 948 passing assertions and none failed (a scratch build of `f09007f`'s test with `-DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` against a copy of `include/` with the edit of `faults.toml`'s `squares_doubled`; the bounded test against the same headers passes). This is the cause the DoD-14 table above left "not traced": the transform suite's row for `squares_doubled` there, and its part in `gap_squares_doubled`'s branch-point kill, were the test's arithmetic limit, not evidence. Neither changes a verdict: the other killers listed in those rows remain.

### The bits and blade-set specs

- **`bits.toml`**: the 13 mutations of `docs/agent/snippets/bits_fault_injection.py`, anchors and replacements unchanged, judged by `core.bits` (targets `core_bits_test`, `core_bits_assert_fail`). **All 13 killed on gcc-debug and clang-debug, in default and `--runtime` mode** (`out/bits-*.tsv`). The run-time edits turn the nine `static_assert`s of `bits_test.cpp` into `static_assert(true || ...)`; that keeps `all_agree` in use, so the snippet's refusal on clang-debug in its run-time mode does not arise, and no leak was found.
- **`blade_set.toml`**: the 35 mutations of `../0008-blade-set/mutate.py`, judged by `core.blade_set` (targets `core_blade_set_test`, `core_blade_set_name_assert`, and the compile-fail tests of the label). Four were re-anchored because the header no longer holds their text, each to the same fault (the comment above each says how): `merge_conflict`, `names_above_9_guard`, `index_sign_check`, `index_repeat_check`. One mutant was added, `binomial_larger` (below). Results (`out/blade_set-*.tsv`): **gcc-debug 35 killed and 1 equivalent; clang-debug 34 killed, 1 equivalent, 1 unjudged.**
  - `zero_guard` (the guard of `full` and `of_grade` without its `N >= 1` clause) survived on both and is declared equivalent. The old driver's kill came from `core.blade_set_names_zero`; since `blades<Alg, "...">` takes an algebra tag, that test is rejected by its own message ("the algebra tag's metric has no generators") and never reaches the guard. `full<0>` and `of_grade<0, 0>` are well formed with or without the clause, so the `static_assert(N >= 1, ...)` gives the same single error. The clause has no observable effect today; that is noted, not changed (`include/` is outside this work).
  - `binomial` (a count too small) is caught before any test runs: on gcc-debug by libstdc++'s bounds assertion in `std::array` (class assert, so the tool counts it killed, though no Galoot check is the killer), on clang-debug by the compiler's constant evaluation in `blade_set.hpp` (class invalid, so unjudged). The old driver counted any failed build. Its row stays as found.
  - `binomial_larger`, added: a count one too large, so the set's size is wrong rather than overrun. Killed on both presets by the `static_assert` of `blade_set_test.cpp:584`.
  - No `--runtime` mode: the test's `static_assert`s state type properties that no run-time check replaces.

### The two warning-only blade_product mutants

`blade_product_rewrites.toml`, with the selection and the four run-time edits of `../0011-blade-product/mutants.toml`:

- `generator_scale_dropped_used`: `generator_scale_dropped` with `(static_cast<void>(scale), x.terms[k].coefficient)`, so no `-Wunused-parameter`. Killed by `core.blade_product` on both presets in both modes (the golden product tables, associativity, the antisymmetrized generators, the pseudoscalar's square).
- `range_widest_shifted_mod`: `range_widest_shifted` with the shift count taken mod `mask_bits`, so no `-Wshift-count-overflow`. In the algebra of 32 generators the range check then shifts by 0, which is what the original shift by 32 does at run time on x86, and rejects every blade but the scalar. Killed on both presets: in default mode by the `static_assert` of `blade_product_test.cpp:834` (a test file, class assert), in `--runtime` mode by "blade_product accepts the generators of the widest algebra".

Both are killed by a test; `0011-blade-product/mutants.toml` still holds the warning-only forms, recorded `unjudged`.

### The flector plane hand value and the even-sign mutant

- The case "the rga3 Flector e412 reflects the plane (0, 0, 1, -1) to (0, 0, 1, 1)" (`a1546f9`, `test/presets/rga3/objects_test.cpp`): the hand value of decision 0211 item 5, derived in its marker from CONVENTIONS 5.3 and 5.6, and the single-sign form's negation (0, 0, −1, −1). Passes on gcc-debug and clang-debug.
- `flector.toml`, `odd_versor_even_sign`: the odd factor of the transform's second stage set to +1, so a flector acts as +F ⟇ x̂ ⟇ F̰. Killed by `rga3.objects` on both presets: in default mode by the point case's `static_assert` (`objects_test.cpp:541`); in `--runtime` mode, with the two flector `static_assert`s turned off, by three cases, the new plane case among them.

### Checks

At `a1546f9`, with only records under `docs/agent/` uncommitted (rows `a1546f9-dirty`): `core.transform_identities` and `rga3.objects` pass on gcc-debug and clang-debug; lint (clang-tidy) of `core_transform_identities_test` and `rga3_objects_test` and `format-check` pass; `-L '^tests\.'` (six tests) passes on both presets; the convention trace gate passes, its only items without a test 6.1 to 6.4. The job pool had 23 of 23 slots free before and after the mutation runs. At the records' tip, `-L '^tests\.'` on gcc-debug, the trace gate, and `tools/runlog check` were run again on the clean tree.
