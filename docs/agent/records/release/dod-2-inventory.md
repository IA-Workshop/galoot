# DoD-2 inventory: the public interface of core 1.0.0 (draft)

- Status: draft. Records only (decision 0240 item 11). Nothing here is evidence until the cells marked unknown are filled at the release revision.
- Written: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`, from a read of `include/galoot/`, `test/`, `README.md`, `docs/agent/governance/CONVENTIONS.md`, and `docs/agent/records/convention-trace.md`.
- Source of the requirement: plan 0001, DoD-2.

## What DoD-2 says

> DoD-2 (C12, C5). The public inventory is exactly the list in section 3. Each entry names its CONVENTIONS section, tests, documentation page, and present user. An entry with no user is removed.

Section 3 of plan 0001 lists, under "In" (headers under `include/galoot/core/`): assertions and checking mode; bits; metric (symmetric form, factories `R`, `rigid`, `conformal`); oriented blade sets with the 1.6 name parser and 1.7 order; the one term-list mechanism; `mv` and the scalar concepts; gp, antigp, wedge, antiwedge, dot, antidot and the operators of 7.1; reverse, antireverse, grade involution, left and right complements; bulk, weight and their duals; exact change of basis; projection, antiprojection, `transform`, `compose`; a lossy conversion under its own name (`truncate<Blades>`); `format.hpp` outside the umbrella header.

So DoD-2 asks for four cells per entry (CONVENTIONS section, tests, documentation page, present user) and for the inventory to match section 3 exactly, no more and no less.

## How the cells were made, and what the marks mean

- **Public** means declared in `namespace galoot::core` and not in a `detail` namespace, found by reading each header and skipping `detail` blocks (for example `apply` in `mv.hpp` is inside `detail` at lines 719 to 1027, so it is not public). The term-list mechanism (`blade_product`'s generator, `plan.hpp`) is entirely `detail`, except `coefficient_type`.
- **verified**: read in the code, the test files, the README, or the governance in this run. For tests, "verified" means the test file names the entry (`grep -w` over `test/**/*_test.cpp`) and that I opened the file list; it does **not** mean the tests cover the entry adequately. Adequacy is the V10 table's job.
- **unknown**: not established in this run. Every unknown has the thing that would settle it.
- Test paths are relative to `test/`. Each header has a `*_test.cpp` (V5), plus compile-fail `*_fail.cpp` files that exercise misuse; those are named once per header, not per entry.
- **Doc page**: filled on 2026-11-10 by the documentation lane (plan 0041, `--pages` part, Claude, agent, Opus 5.5, hci-expert role). Each cell names the page under `docs/guide/` and the heading, quoted, that documents the entry: `core.md`'s reference section has one heading per entry, and the cell adds the teaching section where there is one. "README.md of the guide" is `docs/guide/README.md`, the first page; "README" alone is the root `README.md`. The headings were checked against the pages at the lane's tip; the other columns are unchanged.
- **Present user**: a use by something other than the entry's own test: another header, the `rga3` preset, or the README examples. Found by `grep -w` over `include/galoot/**` and `README.md`, then read where the hit was not obviously code. A word-grep also hits comments, so each user cell says "header" only where I saw a call or a constraint.

## Departures and unknowns, first (C20)

1. **The inventory is not exactly section 3's list.** The code exports more than section 3 names: `mask_t`, `mask_bits`, `max_generators`, `max_form_entry`, `entry_type`, `coefficient_type`, `canonical_less`, `canonical_name`, `blade_name`, `max_compact_generators`, `has_name`, `part`, the left duals, the eighteen `*_layout` aliases, `same_rows`, `basis_change_like`, `GALOOT_ASSERT`. Decision 0187 ratified the `*_layout` names as public (and `compose_layout`, `antiproject_layout`) and decision 0191 ratified `dot` and `antidot` returning multivectors. The rest has no ruling that I found. The release needs either a section 3 amendment by the owner or a move of the excess to `detail`; I did not choose. Rows marked "beyond section 3" below.
2. **Entries with no non-test user** (DoD-2 says remove them): `weight_dual`, `left_bulk_dual`, `left_weight_dual` (no use outside the dual header and tests, README silent), `index_of`, `has_name`, `same_rows`, `basis_change_like`, `max_generators`, `sum_layout`, `canonical_less` in no public use. CONVENTIONS 3.3 names the left duals and the weight dual, so the convention itself is a reason to keep some. Whether a convention counts as a present user is the owner's reading of C12; I mark these "unknown: owner".
3. **Documentation pages: mostly missing.** Only the README names a minority of the entries. The core guide and the model guide do not exist on `develop`. All "none yet" cells wait for the documentation lane. Superseded on 2026-11-10: the documentation lane wrote the guide under `docs/guide/` and filled every "Doc page" cell with a heading of it.
4. **Test adequacy** is not judged here (see V10).
5. **Not run**: no build, no test, no script. Everything is a read.
6. **Checking-mode macros.** `GALOOT_CHECKING` is defined in `assert.hpp` and the README names it. Whether `GALOOT_ASSERT` is public interface or an implementation macro is unknown (the owner's call). `GALOOT_ALWAYS_INLINE` is to be added by the pre-release core pass (decision 0240 item 2) and is not in this table.
7. **`format.hpp`** is outside the umbrella and its `std::formatter<mv<...>>` is its whole interface; the only public names in it are in `detail`. Listed as one entry.
8. **`basis_change.hpp` in the umbrella.** `core.hpp` includes `basis_change.hpp` today, though `format.hpp` is excluded; decision 0240 item 6 measures this in part H and decides later. The row notes the current state.

## The table: `include/galoot/core/`

Columns: entry; CONVENTIONS section; tests (primary files); documentation page; present user.

### assert.hpp: assertions and checking mode

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `GALOOT_CHECKING`, `GALOOT_ASSERT` | 4.5 (checking mode of `truncate`) | verified: `core/smoke_test.cpp`; `core/assert_fail.cpp`, `core/assert_consteval_fail.cpp`, `core/assert_checking_value_fail.cpp` | core.md, "`GALOOT_CHECKING` and `GALOOT_ASSERT`" and "Checking mode"; README (lines 88 to 90) | verified: 11 headers use `GALOOT_ASSERT`; every preset sets `GALOOT_CHECKING`. Public status of `GALOOT_ASSERT` unknown: owner |

### bits.hpp: bits (beyond section 3 as individual names; section 3 says "bits")

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `mask_t`, `mask_bits` | 1.1 | verified: `core/bits_test.cpp` and many others | core.md, "`mask_t` and `mask_bits`" | verified: `blade_set.hpp`, `blade_product.hpp`, `mv.hpp`, `plan.hpp` |
| `grade`, `antigrade` | 1.5, 2.1 | verified: `core/bits_test.cpp`, `core/identities_test.cpp`, `core/transform_test.cpp` | core.md, "`grade` and `antigrade`" | verified: `dual.hpp`, `plan.hpp`, `transform.hpp`, `unary.hpp` |
| `reorder_sign` | 4.1 | verified: `core/bits_test.cpp` | core.md, "`reorder_sign`" | verified: `blade_set.hpp`, `blade_product.hpp`, `plan.hpp` |
| `complement_mask`, `right_complement_sign`, `left_complement_sign` | 3.1 | verified: `core/bits_test.cpp` | core.md, "`complement_mask`, `right_complement_sign`, and `left_complement_sign`" | verified: `plan.hpp` |
| `reverse_sign`, `antireverse_sign`, `grade_involution_sign` | 2.1 | verified: `core/bits_test.cpp` | core.md, "`reverse_sign`, `antireverse_sign`, and `grade_involution_sign`" | verified: `plan.hpp`, `transform.hpp` (antireverse) |

Whether the bit helpers are meant to be public to users is unknown: owner (they are named by "bits" in section 3, but a user has no present use for them).

### metric.hpp: metric

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `metric<N>` (struct, with `is_symmetric`, `entries_in_range`, `is_diagonal`, `is_null`), `metric_like` | 1.2 | verified: `core/metric_test.cpp`, `core/plan_test.cpp`, `core/basis_change_test.cpp`; `core/metric_*_fail.cpp` | core.md, "`metric` and `metric_like`" and "Declaring an algebra" | verified: every header constrains on `metric_like` |
| `R<P, Q, Z>()` | 1.2 | verified: `core/metric_test.cpp` and most suites | core.md, "`R`" and "Declaring an algebra" | verified |
| `rigid<D>()` | 1.3 | verified | core.md, "`rigid`" | verified |
| `conformal<D>()` | 1.4 | verified | core.md, "`conformal`" | verified |
| `max_generators`, `max_form_entry`, `entry_type` | 1.2 | verified: `core/metric_test.cpp` (`max_generators` has two test files, one compile-fail) | core.md, "`max_generators`, `max_form_entry`, and `entry_type`"; limits.md, "Number of generators" and "Coefficient range" | `max_form_entry`: `blade_product.hpp`; `max_generators`: no user outside its header and tests, unknown: owner |

### blade_set.hpp: oriented blade sets, the 1.6 name parser, the 1.7 order

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `blade_set`, `oriented_blade` | 1.6 | verified: `core/blade_set_test.cpp`, `core/mv_test.cpp`; about 40 `blade_set_*_fail.cpp` | core.md, "`blade_set` and `oriented_blade`" and "Layouts" | verified: `mv.hpp`, `plan.hpp`, `linear.hpp`, `transform.hpp`, `basis_change.hpp`, `format.hpp` |
| `blades<Alg, "names">` (the name parser) | 1.6 | verified: `core/blade_set_test.cpp` and most suites | core.md, "`blades`" and "Layouts" | verified: the `rga3` preset |
| `full<N>`, `of_grade<N, K>` | 1.7 | verified: `core/blade_set_test.cpp` | core.md, "`full` and `of_grade`"; limits.md, "Compile-time limits: blade sets" | verified: `mv.hpp` (`of_grade`), many (`full`); a user has no README example |
| `merge<A, B>` | 1.7 | verified: `core/blade_set_test.cpp`, `core/blade_product_test.cpp`, `core/mv_test.cpp` | core.md, "`merge`" | verified: `linear.hpp` (sum layout) |
| `index_of<S, Mask>` | 1.6 | verified: `core/blade_set_test.cpp`, `core/blade_set_index_of_*_fail.cpp` | core.md, "`index_of`" | none found outside tests: unknown, owner |
| `canonical_name`, `blade_name`, `canonical_less`, `max_compact_generators` (beyond section 3) | 1.1, 1.7 | verified: `core/blade_set_test.cpp` | core.md, "`canonical_name`, `blade_name`, `canonical_less`, and `max_compact_generators`"; limits.md, "Blade names" | `canonical_name`, `max_compact_generators`: `format.hpp`, `mv.hpp`; `canonical_less`: `blade_product.hpp`, `plan.hpp`, `mv.hpp` |

### blade_product.hpp and plan.hpp: the one term-list mechanism

Everything is in `detail` except `coefficient_type` (`std::int64_t`), at `blade_product.hpp:58`.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `coefficient_type` (beyond section 3) | 4.1 | verified: `core/blade_product_test.cpp`, `core/plan_test.cpp`, overflow compile-fail tests | core.md, "`coefficient_type`"; limits.md, "Coefficient range" | verified: `plan.hpp`, `basis_change.hpp`, and others (7 headers by word-grep; most are the type) |
| the term-list generator, plans, unary maps, complement conjugation, memoization | 4.1, 4.2 | verified: `core/blade_product_test.cpp`, `core/plan_test.cpp` | internal; n/a (its compile-time limits: limits.md, "Compile-time limits: products and plans") | verified: every operation |

### mv.hpp: `mv`, the scalar concepts, the lossy conversion

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `mv<Alg, Blades, T>` and its constructors | 1.6, 4.5 | verified: `core/mv_test.cpp`; about 30 `mv_*_fail.cpp` | core.md, "`mv`", "Multivectors" and "Conversions" | verified |
| `multivector` (concept) | 4.5 | verified: `core/mv_test.cpp` and 10 other suites | core.md, "`multivector`" | verified: `dual.hpp`, `linear.hpp`, `plan.hpp`, `transform.hpp`, `unary.hpp` |
| `ring_scalar`, `exact_scalar` | 4.5 | verified: `core/mv_test.cpp`, `core/linear_test.cpp`, `support/counting` | core.md, "`ring_scalar` and `exact_scalar`" and "Scalars" | verified: `ring_scalar` in `basis_change.hpp`, `dual.hpp`, `linear.hpp`, `product.hpp`, `transform.hpp`, `unary.hpp`, `rga3/motions.hpp`; `exact_scalar` in `transform.hpp` |
| `truncate<Blades>`, `truncate<Type>` | 4.5 | verified: `core/mv_test.cpp`; `core/mv_truncate_*_fail.cpp` | core.md, "`truncate`" and "Conversions" | verified: `rga3/motions.hpp`, README |
| `part<Blades>`, `part<Type>` (beyond section 3, though named in CONVENTIONS 4.5) | 4.5 | verified: `core/mv_test.cpp`, `core/plan_test.cpp` | core.md, "`part`" and "Conversions" | verified: `basis_change.hpp`, `dual.hpp`, `transform.hpp` |
| `get<"name">` | 1.6 | verified: `core/mv_test.cpp` and others | core.md, "`get`" and "Reading and writing coefficients"; README.md of the guide, "5. Names are oriented" | verified: README example |
| `has_name` (beyond section 3) | 1.6 | verified: `core/mv_test.cpp` | core.md, "`has_name`" and "Writing generic code" | none found: unknown, owner |

### product.hpp: gp, antigp, wedge, antiwedge, operators

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `geometric_product`, `*` | 4.1, 7.1 | verified: `core/product_test.cpp`, `core/plan_test.cpp`; `core/product_*_fail.cpp` | core.md, "`geometric_product` and `*`" and "Products" | verified: `dual.hpp`, `plan.hpp`; README |
| `wedge`, `^` | 4.1, 7.1 | verified: `core/product_test.cpp` | core.md, "`wedge` and `^`"; rga3.md, "Join and meet" | verified: `rga3` preset, README |
| `geometric_antiproduct`, `%` | 4.2, 7.1 | verified: `core/product_test.cpp`, `core/transform_test.cpp` | core.md, "`geometric_antiproduct` and `%`"; README.md of the guide, "3. Two operators to learn" | verified: `transform.hpp` |
| `antiwedge`, `&` | 4.2, 7.1 | verified: `core/product_test.cpp` | core.md, "`antiwedge` and `&`"; rga3.md, "Join and meet" | verified: `dual.hpp`, `plan.hpp`, `rga3/objects.hpp` |
| `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout` | 4.1, 4.2 | verified: `core/product_test.cpp` | core.md, "`geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, and `antiwedge_layout`" and "Result layouts" | verified: ratified public by decision 0187 |

### unary.hpp: involutions and complements

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `reverse`, `~` | 2.1, 7.1 | verified: `core/unary_test.cpp` | core.md, "`reverse` and `~`" and "Involutions and complements" | verified: `dual.hpp`, `plan.hpp` |
| `antireverse` | 2.1 | verified: `core/unary_test.cpp`, `core/transform_test.cpp` | core.md, "`antireverse`" | verified: `transform.hpp`, `rga3/objects.hpp` |
| `grade_involution` | 2.1 | verified: `core/unary_test.cpp`, `core/identities_test.cpp` | core.md, "`grade_involution`" | `plan.hpp` only; no non-test public use found: unknown, owner |
| `right_complement`, `!` | 3.1, 7.1 | verified: `core/unary_test.cpp`, `core/dual_test.cpp` | core.md, "`right_complement` and `!`"; README.md of the guide, "3. Two operators to learn" | verified: `dual.hpp`, `plan.hpp` |
| `left_complement` | 3.1 | verified: `core/unary_test.cpp` | core.md, "`left_complement`" | verified: `dual.hpp`, `plan.hpp` |
| `right_complement_layout`, `left_complement_layout` | 3.1 | verified | core.md, "`right_complement_layout` and `left_complement_layout`" | verified: ratified public (decision 0187) |

### linear.hpp: sums and scaling (the operators of 7.1)

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `+`, `-` (binary and unary), scalar `*` and `/` | 7.1 | verified: `core/linear_test.cpp`; `core/linear_*_fail.cpp` | core.md, "`+`, `-`, and scaling by `*` and `/`" and "Sums and scaling"; README.md of the guide, "2. Scalars must match exactly" | verified: README, `rga3` tests |
| `sum_layout` | 7.1 | verified: `core/linear_test.cpp` only | core.md, "`sum_layout`" | none outside its header and its test: unknown, owner (ratified public by decision 0187) |

### dual.hpp: bulk, weight, duals, dot, antidot, projections

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `bulk`, `weight` | 3.2 | verified: `core/dual_test.cpp`, `core/dual_identities_test.cpp` | core.md, "`bulk` and `weight`" and "Bulk, weight, and their duals" | verified: `transform.hpp`, `basis_change.hpp` use the same names in comments and constraints; read to confirm: unknown |
| `bulk_dual`, `weight_dual` | 3.3 | verified: `core/dual_test.cpp`, `core/dual_identities_test.cpp`, `core/rounding_test.cpp` | core.md, "`bulk_dual` and `weight_dual`" | `bulk_dual` appears in `basis_change.hpp` (comment or call: unknown); `weight_dual`: none outside tests, unknown, owner |
| `left_bulk_dual`, `left_weight_dual` (beyond section 3) | 3.3 (the "own names") | verified: `core/dual_test.cpp`, `core/rounding_test.cpp` | core.md, "`left_bulk_dual` and `left_weight_dual`" | none found outside tests: unknown, owner |
| `dot`, `antidot` | 4.1 | verified: `core/dual_test.cpp` and 9 other suites | core.md, "`dot` and `antidot`" and "Dot and antidot" | `basis_change.hpp` states the preserved law; no other call: unknown |
| `project`, `antiproject` | 4.3 | verified: `core/dual_test.cpp` | core.md, "`project` and `antiproject`" and "Projection and antiprojection" | none found in the preset or README: unknown, owner |
| the dual layouts (`bulk_layout`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`, `left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout`, `antidot_layout`, `project_layout`, `antiproject_layout`) | 3.2 to 4.3 | verified: `core/dual_*_fail.cpp` messages name them | core.md, "`bulk_layout` and the other layouts of the duals" | ratified public by decision 0187 |

### transform.hpp: `transform`, `compose`

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `transform` | 4.4, 5.4, 5.6 | verified: `core/transform_test.cpp`, `core/transform_identities_test.cpp`, `core/transform_rounding_test.cpp`, `presets/rga3/chains_test.cpp` | core.md, "`transform`" and "Transform and compose"; README.md of the guide, "7. `transform` does not unitize" and "9. Qualify `transform` near `std::ranges`" | verified: `rga3/objects.hpp`, README |
| `compose`, `compose_layout` | 4.4, 5.5 | verified: `core/transform_test.cpp`, `core/compose_*_fail.cpp` | core.md, "`compose` and `compose_layout`"; rga3.md, "Composing motions" | verified: `rga3/motions.hpp`, README |

### basis_change.hpp: exact change of basis

`core.hpp` includes this header today (decision 0240 item 6 revisits it after part H).

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `basis_change<N>`, `from_rows` | 1.8 | verified: `core/basis_change_test.cpp`, `core/basis_change_scalars_test.cpp`, `core/basis_change_validation_test.cpp`, the compile-fail files | core.md, "`basis_change` and `from_rows`" and "Change of basis" | verified: README example; no preset uses it |
| `conformal_change<D>()` | 1.4 | verified: `core/basis_change_conformal_test.cpp`, `core/basis_change_conformal3_test.cpp` | core.md, "`conformal_change`" | verified: README example |
| `change_basis<C>`, `change_basis_back<C>` | 1.8 | verified: `core/basis_change_*_test.cpp`, `core/change_basis_*_fail.cpp` | core.md, "`change_basis` and `change_basis_back`" | verified: README example |
| `same_rows` | 1.8 | verified: `core/basis_change_test.cpp` (and the test that the type is not `std::equality_comparable`) | core.md, "`same_rows`" and "Change of basis" | no non-test user: unknown, owner (decision 0235 names the user as the person who compared changes with `==`, of whom there is none before release) |
| `basis_change_like` (concept) | 1.8 | verified: `core/basis_change_test.cpp` | core.md, "`basis_change_like`" | no use outside its own header and test: unknown, owner |
| `change_basis_layout`, `change_basis_back_layout` | 1.8 | verified: the `change_basis_*_fail.cpp` messages | core.md, "`change_basis_layout` and `change_basis_back_layout`" | public by decision 0187's reasoning, not named in it: unknown |

### format.hpp: text output (outside the umbrella)

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `std::formatter` for `mv` | 7.2 | verified: `core/format_test.cpp`, `core/format_outside_core_test.cpp`, `presets/rga3/format_test.cpp` | core.md, "`std::formatter` for `mv`" and "Printing"; README.md of the guide, "8. Printing needs its own header" | verified: README examples; the `rga3` format test |

## Appendix: the `rga3` preset (outside DoD-2, which covers the core)

Included because the "present user" of several core entries is the preset. All verified by reading `include/galoot/presets/rga3/`.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `algebra`, `Point`, `Line`, `Plane`, `Motor`, `Flector`, their layouts | 1.6, 5.1 to 5.3, 5.8 | verified: `presets/rga3/objects_test.cpp`, `incidence_test.cpp`, `chains_test.cpp`, `point_groups_test.cpp` | rga3.md, "Points", "Lines", "Planes", "Motors" and "Flectors" | verified: README examples |
| `rotation`, `translation` | 5.5, 5.7 | verified: `presets/rga3/motions_test.cpp`; `rotation_*_fail.cpp`, `translation_*_fail.cpp` | rga3.md, "Rotation and translation" | verified: README examples |

## To fill at the release revision

- Re-run the greps against the release revision; the entries and test names move with the pre-release core pass (decision 0240 item 1, which deletes `detail::inverse_change` and adds a deleted-`==` hint) and the documentation lane.
- Doc-page cells: filled with guide headings on 2026-11-10; re-read each cited heading and README line at the release revision, and remove the text of any entry moved to `detail` (decision 0241 item 6).
- Owner decisions: the excess over section 3 (departure 1), the no-user entries (departure 2), the macros (departure 6).
- Test cells: replace "names the entry" with the test case that tells it from its alternatives, for the entries the V10 table lists.
