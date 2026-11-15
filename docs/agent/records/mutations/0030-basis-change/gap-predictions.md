# WU12 gap mutants: predictions written before any run

Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-09 UTC, for plan 0030 part A item (g) and criterion 12, on `feature/core-basis-change--math` at the revision that adds this file, before `gaps.toml` exists or runs. The argument is in research note 0026 (sections A and G). Part E records the outcomes beside these predictions; a prediction is not changed after a run.

## What counts

Criterion 12 (as decision 0218 item 5 amended it, and as W3 now words it): a gap mutant counts when the suite the gap extends lets it survive at the branch point and kills it at the tip. The suite at the branch point is the identity suites, CTest labels `^core\.(identities|identities_rounding|dual_identities|transform_identities)$`; at the tip, the same plus `^core\.basis_change_identities`. Every mutant edits `include/galoot/core/metric.hpp`; the anchors below each occur exactly once at `a6307ba`.

Why the narrowed suite: the whole core suite kills each of these at the branch point through `metric_test.cpp`'s hand values and the golden product tables (decision 0217 item 6), so only the identity suites can show the gap.

## The mutants and the predictions

### 1. `conformal_pairing_positive`

Edits: `m.gram[D][D + 1] = -1;` → `m.gram[D][D + 1] = 1;` and `m.gram[D + 1][D] = -1;` → `m.gram[D + 1][D] = 1;`.

Branch point: **survives**, on both presets. Every identity program reads g from `Alg::value.gram` (R3), and the generator rule ties each product to the Clifford product of that g, so conformal(d) with pairing +1 is a consistent algebra. The one conformal-specific identity, "the weight is the negated bulk" in `dual_identities_test.cpp`, holds for pairing +1 too: its premises, G = G⁻¹ and det g = −1, hold for [[0, 1], [1, 0]] as for [[0, −1], [−1, 0]], and the weight is det g times the exomorphism of G⁻¹. `identities_rounding`'s conformal(3) cases take their bounds from the tracking scalar, so they adapt.

Tip: **killed**, on both presets, in `core.basis_change_identities`: `conformal_change<D>()`'s rows are fixed by the convention, so P G′ Pᵀ has −1 at the null pair while the mutated G has +1, and the core's isometry check rejects the change when the program instantiates it. The first error is the check's `static_assert` or `GALOOT_ASSERT`, which `tools/mutate` classifies as `assert`, a kill. Were the isometry check absent (a `change.toml` mutant), the homomorphism of criterion 4 would fail at run time instead, because P is then not an isometry (note 0026, Theorem 1 needs it).

### 2. `diagonal_negative_first`

Edit: `m.gram[i][i] = i < P ? 1 : -1;` → `m.gram[i][i] = i < Q ? -1 : 1;`, which puts R's −1 squares before its +1 squares. Only forms with p > 0 and q > 0 change; `rigid()` (q = 0) and `conformal()` (which does not call `R()`) do not.

Branch point: **survives**, for the reason of mutant 1: every identity reads its squares from the stored Gram matrix, including the diagonal-parts identity of `dual_identities_test.cpp`.

Tip: **killed** in `core.basis_change_identities`, by the isometry checks of the conformal changes (target R(d+1, 1, 0), whose e′1 would square to −1, so e1 · e1 = −1 against 1) and of the Witt changes (target R(1, 1, 0) or R(2, 2, 0): with note 0026's Witt rows, e1 · e2 becomes −1 against 1). Classified `assert`.

### 3. `witt_pairing_doubled`

No factory builds a Witt form; the Witt forms are `from_gram` literals in the tests, and editing a test's list is not a mutant of existing library code. The nearest library mutant: `from_gram` doubles each off-diagonal entry.

Edit: `m.gram[i][j] = gram[i][j];` → `m.gram[i][j] = i == j ? gram[i][j] : 2 * gram[i][j];`.

Branch point: **survives, with less confidence.** The doubled forms stay symmetric and within `max_form_entry`, and the identity suites read the stored matrix, so no identity fails. But the doubling raises the product-table coefficients of the dense random forms (random 7's largest from 875 to 21,836, random 2's from 236 to 1,532; computed with `clifford_reference.py`), and a test-rational overflow in a dense identity group, which aborts the program, would kill it at the branch point, where it then does not count. I have not bounded the identity suites' draws against the doubled forms.

Tip: **killed** in `core.basis_change_identities`, by the isometry check of the Witt changes (G with pairing 2 against P G′ Pᵀ with pairing 1), classified `assert`; also by the named change of the degenerate form, if part B writes one (note 0026, H).

If the branch-point run kills it by an overflow, the record says so, and the Witt row of the coverage table rests on mutants 1 and 2 and on criterion 9 (b)'s CAR values.

### 4. The test-side LDLᵀ row: no counting mutant

Prediction: **no mutant of existing library code can count for this gap.** Argument (R3, note 0026 Theorem 1): a mutant survives the branch-point identity suites only if each algebra stays self-consistent, every product being the Clifford product of the algebra's stored Gram matrix. An LDLᵀ test computes its diagonal counterpart D and its change P from that same stored matrix, so P D Pᵀ = G holds by construction and the homomorphism holds by Theorem 1; every LDLᵀ assertion passes. A mutant that broke self-consistency would already die at the branch point by the generator rule. Mutants of `basis_change.hpp` or of `test/support/diagonalize.hpp` are new code, judged by their own specs (`change.toml`, `ldlt.toml`), not gap mutants. This is the expected result recorded under criterion 12, not a failure.

Condition: the argument assumes every diagonal counterpart is built with `from_gram`, as R5's integer diagonal suggests. Two forms have a scaled diagonal in R's order with squares 1, −1, 0 only: "radical off the axes", (1, −1, 0), and "all ones", (1, 0, 0). If part B builds the first as `R<1, 1, 1>()`, then mutant 2 would also be killed by that LDLᵀ test at the tip, and would count for this row too.

## What a run must show for each prediction to hold

| Mutant | gcc-debug, branch point | gcc-debug, tip | clang-debug, branch point | clang-debug, tip |
| --- | --- | --- | --- | --- |
| `conformal_pairing_positive` | survived | killed (assert) | survived | killed (assert) |
| `diagonal_negative_first` | survived | killed (assert) | survived | killed (assert) |
| `witt_pairing_doubled` | survived (uncertain: an overflow would kill it) | killed (assert) | the same | killed (assert) |

Each tip kill also assumes criterion 4's conformal and Witt groups are in `core.basis_change_identities`, as plan 0030 places them; a group moved to `core.basis_change` would leave its kill outside the counted suite.
