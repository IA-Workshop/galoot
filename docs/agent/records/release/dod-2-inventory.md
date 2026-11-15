# DoD-2 inventory: the public interface of core 1.0.0

- Status: re-grepped at `develop` `a73cd4e` (2026-11-11), for the release record. Records only. U9 of plan 0042 re-reads it at the release revision R, where only the placeholders of the release commit should have changed (README, `CMakeLists.txt`), none of them an entry here. Checked at R `7580e88` by U9 on 2026-11-12: `git diff --stat a73cd4e 7580e88 -- include/` prints nothing, and the release commit changed only `CMakeLists.txt` and the README, so every row holds at R.
- Written first: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`; doc-page column filled the same day by the documentation lane (plan 0041). Re-grepped and rewritten: 2026-11-11 by Claude (agent, Opus 5.5), planner, plan 0042 unit U6, on branch `records/release-prep`.
- Source of the requirement: plan 0001, DoD-2. Rulings applied: decision 0187 (the `*_layout` names are public), decision 0241 item 6 (`GALOOT_ASSERT` public and documented, `GALOOT_ALWAYS_INLINE` not interface), decision 0244 item 16 (`GALOOT_FORCE_INLINE` public), decision 0248 item 9 (O1 (a): the six helpers stay public, 0241 item 6's move to `detail` superseded; O2: `basis_change.hpp` stays in `core.hpp`).

## What DoD-2 says

> DoD-2 (C12, C5). The public inventory is exactly the list in section 3. Each entry names its CONVENTIONS section, tests, documentation page, and present user. An entry with no user is removed.

Plan 0001 section 3 lists, under "In": assertions and checking mode; bits; metric (symmetric form, factories `R`, `rigid`, `conformal`); oriented blade sets with the 1.6 name parser and 1.7 order; the one term-list mechanism; `mv` and the scalar concepts; gp, antigp, wedge, antiwedge, dot, antidot and the operators of 7.1; reverse, antireverse, grade involution, left and right complements; bulk, weight and their duals; exact change of basis; projection, antiprojection, `transform`, `compose`; a lossy conversion under its own name (`truncate<Blades>`); `format.hpp` outside the umbrella header.

Decision 0248 item 9 adopts plan 0042's O1 (a): "Public in 1.0, section 3 amended to the real exports". The tables below are those real exports, so they are the list section 3 now means. Plan 0001's text itself is not edited here (see departure 1).

## Departures, readings, and unknowns, first (C20)

1. **Section 3's amendment is this list, not an edit of plan 0001.** Decision 0241 item 6 said section 3 is amended "by a decision at the release record"; decision 0248 item 9 is that decision. I did not edit plan 0001, which plan 0042 leaves to the coordinator (U4, U11). A one-line addendum in plan 0001 pointing here would make the amendment findable from section 3; that is an owner or coordinator item.
2. **"Present user" is read more widely than in the first draft.** A user is a use in code by something other than the entry's own header and tests: another core header, the rga3 preset, or a `cpp` example of the README or of the guide. The guide's examples count because each is compiled and run as a test on both compilers (DoD-27) and is written as a user's program. Comments do not count: the first draft's word-grep counted comment hits, and several of its "verified: header" cells were comments only (for example `part` in `basis_change.hpp`, `of_grade` in `mv.hpp`, `max_form_entry` in `blade_product.hpp`). Operators are counted by their spelling in the examples (`^`, `&`, `*`), by reading.
3. **Entries that still have no user under that reading**, kept by ruling and listed for the owner: `sum_layout` and `basis_change_like` (two of the six helpers, kept by decision 0248 item 9, which departs from DoD-2's last sentence for them); the layout names `compose_layout`, `change_basis_layout`, `change_basis_back_layout` and the ten dual layouts (public by decision 0187, whose reason is that a user names the result type of an operation); and `GALOOT_FORCE_INLINE`, whose user is a build (the cache option of `CMakeLists.txt:105` passes it, and four presets set it to 0). No entry was removed.
4. **One public name was missing from the first draft: `from_gram`** (`metric.hpp:247`), the factory of a metric from a symmetric integer matrix. It has a row now. No other public name of `galoot::core` is missing, by the scan below.
5. **How the scan was made.** A script stripped comments and string literals from each header, blanked every `namespace detail` block, kept only declarations at namespace scope of `galoot::core`, and listed concepts, aliases, structs, variables, functions, operators and macros. Its output was read against the tables; its false hits (`defined` in a `#if`, the `std::formatter` specialization's `format` and `write` members) were dropped by reading. Tests: a word-grep over `test/**/*.cpp` and `*.hpp`, files named, not judged for adequacy (V10's job). Doc pages: every heading cited was checked to exist in `docs/guide/core.md` at `a73cd4e`, and every relative link and anchor of the README and the guide pages resolves. The script is not committed (it is a one-off read; its method is stated here).
6. **Not run**: no build and no test. Everything is a read at `a73cd4e`.
7. **Members are not listed separately.** `mv`'s `==` (declared for a scalar with `==`, through its storage base in `detail`), `metric`'s members, and `basis_change`'s deleted `==` (`basis_change.hpp:177`, whose comment names `same_rows`) belong to their types' rows.

## Macros: `assert.hpp`

| Entry | Public | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- | --- |
| `GALOOT_CHECKING` | yes | 4.5 (checking mode) | `core/smoke_test.cpp` and three more `_test.cpp`; `core/assert_fail.cpp`, `core/assert_checking_value_fail.cpp`, and four more `_fail.cpp`; set by every preset | core.md, "`GALOOT_CHECKING` and `GALOOT_ASSERT`" and "Checking mode"; README, "Using Galoot" | `transform.hpp` (the `-ffast-math` guard); every preset |
| `GALOOT_ASSERT` | yes (decision 0241 item 6) | 4.5 | `core/smoke_test.cpp`; `core/assert_fail.cpp`, `core/assert_consteval_fail.cpp` | core.md, "`GALOOT_CHECKING` and `GALOOT_ASSERT`" | nine core headers and `presets/rga3/motions.hpp`; a guide example (core.md) |
| `GALOOT_FORCE_INLINE` | yes (decision 0244 item 16) | none (build mode, not mathematics) | `core.force_inline_off`, `_default`, `_build` (`core/force_inline_test.cpp`); `core.force_inline_value`, `_on`, `_empty` (`core/force_inline_{value,on,empty}_fail.cpp`); `rga3.reference_kernels_unforced` and `_forced` | core.md, "`GALOOT_FORCE_INLINE`"; README, "Using Galoot" | the build: the cache option `GALOOT_FORCE_INLINE` (`CMakeLists.txt:105`); gcc-asan, clang-asan and clang-release set it OFF |
| `GALOOT_ALWAYS_INLINE` | **no**: defined in every unit, not interface (decision 0241 item 6; its comment says "a program neither uses nor defines it") | none | `core.force_inline_*` read its expansion | none, by design | six core headers |
| `GALOOT_DETAIL_PASTE_`, `GALOOT_DETAIL_PASTE`, `GALOOT_DETAIL_FORCE_INLINE_IS_0`, `GALOOT_DETAIL_FORCE_INLINE_IS_1` | **no**: helpers of the mode's spelling check, defined in every unit (`assert.hpp:44-47`), never undefined | none | `core.force_inline_value`, `_on`, `_empty` | none | `assert.hpp` only. Known friction: they stay defined in a user's unit |

## `bits.hpp`: bits

Section 3 names "bits"; the entries are its names. CONVENTIONS names each sign rule (1.1, 2.1, 3.1, 4.1).

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `mask_t`, `mask_bits` | 1.1 | `core/bits_test.cpp` and about 20 more | core.md, "`mask_t` and `mask_bits`" | `basis_change.hpp`, `blade_product.hpp`, `blade_set.hpp`, `plan.hpp` (`mask_bits` also `metric.hpp`) |
| `grade`, `antigrade` | 1.5, 2.1 | `core/bits_test.cpp`, `core/identities_test.cpp`, `core/transform_test.cpp` and others | core.md, "`grade` and `antigrade`" | `grade`: `basis_change.hpp`, `blade_product.hpp`, `blade_set.hpp`, `plan.hpp`; `antigrade`: `transform.hpp`; both in a guide example |
| `reorder_sign` | 4.1 | `core/bits_test.cpp` | core.md, "`reorder_sign`" | `blade_product.hpp`, `blade_set.hpp`, `plan.hpp` |
| `complement_mask`, `right_complement_sign`, `left_complement_sign` | 3.1 | `core/bits_test.cpp` | core.md, "`complement_mask`, `right_complement_sign`, and `left_complement_sign`" | `plan.hpp` |
| `reverse_sign`, `antireverse_sign`, `grade_involution_sign` | 2.1 | `core/bits_test.cpp` | core.md, "`reverse_sign`, `antireverse_sign`, and `grade_involution_sign`" | `plan.hpp`; `antireverse_sign` also `transform.hpp` |

## `metric.hpp`: metric

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `metric<N>` (with `is_symmetric`, `entries_in_range`, `is_diagonal`, `is_null`), `metric_like` | 1.2 | `core/metric_test.cpp`, `core/plan_test.cpp`, `core/basis_change_test.cpp`; 12 `core/metric_*_fail.cpp` | core.md, "`metric` and `metric_like`" and "Declaring an algebra" | every operation's header constrains on `metric_like`; guide examples |
| `R<P, Q, Z>()` | 1.2 | `core/metric_test.cpp` and most suites | core.md, "`R`" and "Declaring an algebra" | seven core headers; guide examples |
| `rigid<D>()` | 1.3 | most suites | core.md, "`rigid`" | `presets/rga3/objects.hpp`; guide examples |
| `conformal<D>()` | 1.4 | `core/basis_change_conformal_test.cpp` and others | core.md, "`conformal`" | guide examples (core.md) |
| `from_gram<N, MaxN>({...})` (**new row**) | 1.2 | about 20 `_test.cpp` (for example `core/basis_change_random_five_test.cpp`, `core/basis_change_forms.hpp`) and about 30 `_fail.cpp` | core.md, "`from_gram`" and "Declaring an algebra"; limits.md, "Number of generators" | guide examples (core.md) |
| `max_generators` (one of the six helpers; public by decision 0248 item 9) | 1.2 | `core/metric_test.cpp`; one `_fail.cpp` | core.md, "`max_generators`, `max_form_entry`, and `entry_type`"; limits.md, "Number of generators" | the default `MaxN` of the factories in its own header; a guide example (core.md) |
| `max_form_entry`, `entry_type` | 1.2 | `core/metric_test.cpp` and others | as above; limits.md, "Coefficient range" | `entry_type`: `basis_change.hpp`; both in a limits.md example |

## `blade_set.hpp`: oriented blade sets, the 1.6 name parser, the 1.7 order

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `blade_set`, `oriented_blade` | 1.6 | `core/blade_set_test.cpp`, `core/mv_test.cpp` and others; 37 `core/blade_set_*_fail.cpp` | core.md, "`blade_set` and `oriented_blade`" and "Layouts" | most core headers; guide examples |
| `blades<Alg, "names">` (the name parser) | 1.6 | `core/blade_set_test.cpp` and most suites | core.md, "`blades`" and "Layouts" | seven core headers and `presets/rga3/objects.hpp`; guide examples |
| `full<N>`, `of_grade<N, K>` | 1.7 | `core/blade_set_test.cpp` and others | core.md, "`full` and `of_grade`"; limits.md, "Compile-time limits: blade sets" | guide examples |
| `merge<A, B>` | 1.7 | `core/blade_set_test.cpp`, `core/blade_product_test.cpp`, `core/mv_test.cpp` | core.md, "`merge`" | `linear.hpp`; a guide example |
| `index_of<S, Mask>` (one of the six helpers; public by decision 0248 item 9) | 1.6 | `core/blade_set_test.cpp`; 4 `core/blade_set_index_of_*_fail.cpp` | core.md, "`index_of`" | guide examples (core.md, "Layouts") |
| `canonical_name`, `blade_name`, `canonical_less`, `max_compact_generators` | 1.1, 1.7 | `core/blade_set_test.cpp` and others | core.md, "`canonical_name`, `blade_name`, `canonical_less`, and `max_compact_generators`"; limits.md, "Blade names" | `canonical_name`: `format.hpp`; `blade_name`: the return type of `canonical_name`; `canonical_less`: `blade_product.hpp`, `mv.hpp`, `plan.hpp`; `max_compact_generators`: `format.hpp`, `mv.hpp` |

## `blade_product.hpp` and `plan.hpp`: the one term-list mechanism

Everything is in `detail` except `coefficient_type` (`blade_product.hpp:58`). `plan.hpp` declares no public name.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `coefficient_type` | 4.1 | `core/blade_product_test.cpp`, `core/plan_test.cpp`; overflow `_fail.cpp` | core.md, "`coefficient_type`"; limits.md, "Coefficient range" | six core headers; a limits.md example |
| the term-list generator, plans, unary maps, complement conjugation, memoization (`detail`) | 4.1, 4.2 | `core/blade_product_test.cpp`, `core/plan_test.cpp` | internal; limits.md, "Compile-time limits: products and plans" | every operation |

## `mv.hpp`: `mv`, the scalar concepts, the lossy conversion

`apply` and its two overloads with no target layout are in `detail` (the block at `mv.hpp:720` to `1022`), so they are not public; their deletion after 1.0 removes no public name.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `mv<Alg, Blades, T>`, its constructors and its `==` | 1.6, 4.5 | `core/mv_test.cpp` and others; 86 `core/mv_*_fail.cpp` | core.md, "`mv`", "Multivectors" and "Conversions" | most core headers and `presets/rga3/objects.hpp`; README and guide examples (`==` in the README's static assertions) |
| `multivector` (concept) | 4.5 | `core/mv_test.cpp` and about 12 other suites | core.md, "`multivector`" | `basis_change.hpp`, `format.hpp`, `linear.hpp`, `presets/rga3/motions.hpp`; guide examples |
| `ring_scalar`, `exact_scalar` | 4.5 | `core/mv_test.cpp`, `core/linear_test.cpp`, `support/counting.hpp` | core.md, "`ring_scalar` and `exact_scalar`" and "Scalars" | `ring_scalar`: six core headers and `presets/rga3/motions.hpp`; `exact_scalar`: `transform.hpp`; guide examples |
| `truncate<Blades>`, `truncate<Type>` | 4.5 | `core/mv_test.cpp` and others; 8 `core/mv_truncate_*_fail.cpp` | core.md, "`truncate`" and "Conversions" | `presets/rga3/motions.hpp`; guide examples |
| `part<Blades>`, `part<Type>` | 4.5 | `core/mv_test.cpp`, `core/plan_test.cpp` and others | core.md, "`part`" and "Conversions" | guide examples (core.md) |
| `get<"name">` | 1.6 | `core/mv_test.cpp` and others | core.md, "`get`" and "Reading and writing coefficients"; guide README, "5. Names are oriented" | README example; guide examples |
| `has_name` (one of the six helpers; public by decision 0248 item 9) | 1.6 | `core/mv_test.cpp` | core.md, "`has_name`" and "Writing generic code" | a guide example (core.md, "Writing generic code") |

## `product.hpp`: gp, antigp, wedge, antiwedge, operators

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `geometric_product`, `*` | 4.1, 7.1 | `core/product_test.cpp`, `core/plan_test.cpp` and others; 22 `core/product_*_fail.cpp` | core.md, "`geometric_product` and `*`" and "Products" | `dual.hpp`, `plan.hpp`; guide examples (`*`) |
| `wedge`, `^` | 4.1, 7.1 | `core/product_test.cpp` and others | core.md, "`wedge` and `^`"; rga3.md, "Join and meet" | `basis_change.hpp`, `dual.hpp`, `plan.hpp`; README example (`p ^ q`) |
| `geometric_antiproduct`, `%` | 4.2, 7.1 | `core/product_test.cpp`, `core/transform_test.cpp` and others | core.md, "`geometric_antiproduct` and `%`"; guide README, "3. Two operators to learn: `%` and `!`" | `plan.hpp`, `transform.hpp` |
| `antiwedge`, `&` | 4.2, 7.1 | `core/product_test.cpp` and others | core.md, "`antiwedge` and `&`"; rga3.md, "Join and meet" | `dual.hpp`, `plan.hpp`; README example (`line & plane`) |
| `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout` | 4.1, 4.2 | `core/product_test.cpp` | core.md, "`geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, and `antiwedge_layout`" and "Result layouts" | `dual.hpp` (three), `transform.hpp` (`geometric_antiproduct_layout`); `wedge_layout` in guide examples; public by decision 0187 |

## `unary.hpp`: involutions and complements

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `reverse`, `~` | 2.1, 7.1 | `core/unary_test.cpp` and others | core.md, "`reverse` and `~`" and "Involutions and complements" | `dual.hpp`, `plan.hpp`; guide examples |
| `antireverse` | 2.1 | `core/unary_test.cpp`, `core/transform_test.cpp` and others | core.md, "`antireverse`" | `plan.hpp`; guide examples |
| `grade_involution` | 2.1 | `core/unary_test.cpp`, `core/identities_test.cpp` | core.md, "`grade_involution`" | `plan.hpp`; a guide example |
| `right_complement`, `!` | 3.1, 7.1 | `core/unary_test.cpp`, `core/dual_test.cpp` | core.md, "`right_complement` and `!`"; guide README, "3. Two operators to learn: `%` and `!`" | `dual.hpp`, `plan.hpp` |
| `left_complement` | 3.1 | `core/unary_test.cpp` and others | core.md, "`left_complement`" | `dual.hpp`, `plan.hpp`; a guide example |
| `right_complement_layout`, `left_complement_layout` | 3.1 | `core/unary_test.cpp` | core.md, "`right_complement_layout` and `left_complement_layout`" | `dual.hpp`; public by decision 0187 |

## `linear.hpp`: sums and scaling (the operators of 7.1)

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `+`, `-` (binary and unary), scalar `*` and `/` | 7.1 | `core/linear_test.cpp`; 26 `core/linear_*_fail.cpp` | core.md, "`+`, `-`, and scaling by `*` and `/`" and "Sums and scaling"; guide README, "2. Scalars must match exactly" | guide examples; the rga3 tests |
| `sum_layout` (one of the six helpers; public by decision 0248 item 9 and decision 0187) | 7.1 | `core/linear_test.cpp` only | core.md, "`sum_layout`" | **none** beyond its header and test (departure 3) |

## `dual.hpp`: bulk, weight, duals, dot, antidot, projections

None of these is used by another header or the preset in code; their users are the guide's examples.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `bulk`, `weight` | 3.2 | `core/dual_test.cpp`, `core/dual_identities_test.cpp` and others | core.md, "`bulk` and `weight`" and "Bulk, weight, and their duals" | guide examples |
| `bulk_dual`, `weight_dual` | 3.3 | `core/dual_test.cpp`, `core/dual_identities_test.cpp`, `core/rounding_test.cpp` | core.md, "`bulk_dual` and `weight_dual`" | guide examples |
| `left_bulk_dual`, `left_weight_dual` | 3.3 | `core/dual_test.cpp`, `core/rounding_test.cpp` | core.md, "`left_bulk_dual` and `left_weight_dual`" | guide examples |
| `dot`, `antidot` | 4.1 | `core/dual_test.cpp` and about 8 other suites | core.md, "`dot` and `antidot`" and "Dot and antidot" | guide examples |
| `project`, `antiproject` | 4.3 | `core/dual_test.cpp` and others | core.md, "`project` and `antiproject`" and "Projection and antiprojection" | guide examples |
| `bulk_layout`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`, `left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout`, `antidot_layout`, `project_layout`, `antiproject_layout` | 3.2 to 4.3 | `core/dual_test.cpp`; the `core/dual_*_fail.cpp` messages name them | core.md, "`bulk_layout` and the other layouts of the duals" | **none** beyond the header and tests; public by decision 0187 (departure 3) |

## `transform.hpp`: `transform`, `compose`

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `transform` | 4.4, 5.4, 5.6 | `core/transform_test.cpp`, `core/transform_identities_test.cpp`, `core/transform_rounding_test.cpp`, `presets/rga3/chains_test.cpp` and others | core.md, "`transform`" and "Transform and compose"; guide README, "7. `transform` does not unitize" and "9. Qualify `transform` near `std::ranges`" | README example; guide examples |
| `compose` | 4.4, 5.5 | `core/transform_test.cpp` and others; 6 `core/compose_*_fail.cpp` | core.md, "`compose` and `compose_layout`"; rga3.md, "Composing motions" | README example; guide examples |
| `compose_layout` | 4.4 | `core/transform_test.cpp` | as above | **none** beyond the header and tests; public by decision 0187 (departure 3) |

## `basis_change.hpp`: exact change of basis

`core.hpp` includes this header, and it stays there for 1.0 (decision 0248 item 9, O2).

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `basis_change<N>` (its `==` deleted, naming `same_rows`), `from_rows` | 1.8 | `core/basis_change_test.cpp`, `core/basis_change_scalars_test.cpp`, `core/basis_change_validation_test.cpp`, and the compile-fail files | core.md, "`basis_change` and `from_rows`" and "Change of basis" | guide examples (core.md, "Change of basis"); no preset uses it |
| `conformal_change<D>()` | 1.4 | `core/basis_change_conformal_test.cpp`, `core/basis_change_conformal3_test.cpp` and others | core.md, "`conformal_change`" | guide examples |
| `change_basis<C>`, `change_basis_back<C>` | 1.8 | 8 `core/basis_change_*_test.cpp`; 18 `core/change_basis_*_fail.cpp` | core.md, "`change_basis` and `change_basis_back`" | guide examples |
| `same_rows` (one of the six helpers; public by decision 0248 item 9) | 1.8 | `core/basis_change_test.cpp` (and the test that the type is not `std::equality_comparable`) | core.md, "`same_rows`" and "Change of basis" | a guide example; named by the deleted `==`'s comment and its diagnostic |
| `basis_change_like` (one of the six helpers; public by decision 0248 item 9) | 1.8 | `core/basis_change_test.cpp` | core.md, "`basis_change_like`" | **none** beyond its header and test (departure 3) |
| `change_basis_layout`, `change_basis_back_layout` | 1.8 | `core/basis_change_test.cpp`; the `core/change_basis_*_fail.cpp` messages | core.md, "`change_basis_layout` and `change_basis_back_layout`" | **none** beyond the header and tests; public by decision 0187's reasoning (departure 3) |

## `format.hpp`: text output (outside the umbrella)

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `std::formatter` for `mv` | 7.2 | `core/format_test.cpp`, `core/format_outside_core_test.cpp`, `presets/rga3/format_test.cpp`; `core.bare_compile_format_no_exceptions` | core.md, "`std::formatter` for `mv`" and "Printing"; guide README, "8. Printing needs its own header" | README example (the third); guide examples |

## The six helpers of decision 0241 item 6, in one place

| Helper | Named by CONVENTIONS | User | State at 1.0 |
| --- | --- | --- | --- |
| `has_name` | no | a guide example | public (0248 item 9) |
| `same_rows` | no | a guide example; the deleted `==` names it | public |
| `basis_change_like` | no | none | public, with no user (a ruled departure from DoD-2's last sentence) |
| `index_of` | no | guide examples | public |
| `sum_layout` | no | none | public, with no user (as above; also 0187) |
| `max_generators` | no | the factories' default bound; a guide example | public |

Moving any of them to `detail` after 1.0 removes a public name, which semantic versioning makes a 2.0.0 change (W13).

## Appendix: the `rga3` preset (outside DoD-2, which covers the core)

Included because the preset is a present user of several core entries. Read in `include/galoot/presets/rga3/` at `a73cd4e`.

| Entry | Section | Tests | Doc page | Present user |
| --- | --- | --- | --- | --- |
| `algebra`, `Point`, `Line`, `Plane`, `Motor`, `Flector`, their layouts | 1.6, 5.1 to 5.3, 5.8 | `presets/rga3/objects_test.cpp`, `incidence_test.cpp`, `chains_test.cpp`, `point_groups_test.cpp`, `op_counts_test.cpp` | rga3.md, "Points", "Lines", "Planes", "Motors" and "Flectors" | README examples |
| `rotation`, `translation` | 5.5, 5.7 | `presets/rga3/motions_test.cpp`; 7 `rotation_*_fail.cpp`, 3 `translation_*_fail.cpp` | rga3.md, "Rotation and translation" | README example |

## At the release revision (U9)

- `git diff --stat a73cd4e R -- include` should be empty; if it is not, re-run the scan of departure 5 on the changed headers.
- Re-read the README's and the guide's cited headings only if `git diff a73cd4e R -- README.md docs/guide` touches them (the release commit changes the README's status line and `FetchContent` block, neither a heading cited here).
