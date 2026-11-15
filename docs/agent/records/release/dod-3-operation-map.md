# DoD-3: every operation of core 1.0.0 mapped to the one mechanism

- Status: written 2026-11-12 by Claude (agent, Opus 5.5), plan 0042 unit U11, records only, on `feature/release-1.0.0` (from `develop` at `300f6b4`), by the owner's ruling on the release audit's O-A (decision 0250 item 1). It collates decisions that already exist; it decides nothing new.
- DoD-3 (plan 0001 section 4; C3, C4): "A decision record maps every operation to 'the term-list generator composed with per-basis data'. No hand-written or cached table, or each has a C3 decision with measurement."
- Sources: decisions 0100, 0102, 0110, 0114, 0115, 0118, 0150, 0153, 0154, 0155, 0167, 0168, 0172, 0178, 0220 section 4 and 0222 item 5, each read for this record; the headers under `include/galoot/core/` and `include/galoot/presets/rga3/` at `300f6b4`, read where a row says so. The list of operations is the DoD-2 inventory (`dod-2-inventory.md`).

## Departures, readings, and unknowns, first (C20)

1. **Not a new decision.** DoD-3 asks for "a decision record". The mapping is spread over the decisions below, each accepted when its unit merged; this record joins them, by the owner's ruling (0250 item 1, the audit's option (a): "collated ... into one short record"). Where a row's composition is stated in a header comment rather than a decision, the row says so.
2. **"Per-basis data" is read as C4 lists it:** signs, permutations, masks and coefficients that depend on single blades or on one tag, computed at compile time. Positions of blades in a layout and orientation signs (the kernels of decision 0154) are read as such data. So is the inverse of a change of basis, which decision 0222 item 5 ruled per-basis data (the owner's answer to decision 0220's OI5).
3. **The headers were read, not tested, for this record.** For each row the header's call into the mechanism was found by grep and read at the line cited; no build or test ran for this record. The tests that pin each composition are named in the decisions and ran in the release matrix at R.
4. **No hand-written, precompiled or cached table was found** in `include/galoot/` by reading the operations below. Plans are generated once per translation unit and kept as `static constexpr` variable templates (decisions 0102 item 7 and 0118); that is the memoization plan 0001 section 3 lists under the mechanism, not a cached table in C3's sense, since nothing is stored between translation units or written by hand. So the second sentence of DoD-3 needs no C3 decision with measurement.
5. **The rga3 preset's `rotation` and `translation`** build a motor from the coefficient formulas of CONVENTIONS 5.5 and 5.7 (`presets/rga3/motions.hpp:80` and `:160`). They are value constructors on a named type, not operations of the algebra, and use no table. They are listed for completeness; the preset is outside DoD-3's "core" reading, as DoD-2's inventory treats it.

## The mechanism

- **The term-list generator.** `detail::blade_product` gives the exact terms of e_A e_B from the metric by the recursion on the defining relations (Spike A, note 0008), in integer arithmetic with overflow checks (decisions 0100, 0101). `detail::generate_plan` applies a pair rule to every pair of input blades and writes the plan's terms (decisions 0102, 0110 item 4); `product_plan<Op, Alg, A, B>` types it on its result set (`plan.hpp:635`).
- **Unary maps** are the same typing over one term per input blade (`unary_map`, `plan.hpp:945`; decision 0114 items 2 and 3).
- **Kernels** move each term of a plan or map to its place in a target layout and fold in the orientation signs: `product_kernel`, `map_kernel`, `identity_kernel` and `run_product`, `run_map` (decision 0154 items 1 and 2; `mv.hpp:250` to `:361`). They add no term.

## Operations

| Operation (public name) | Composition | Decision | Where in the header |
| --- | --- | --- | --- |
| `geometric_product`, `*` | `product_plan` with the pair rule `blade_product` | 0102; 0110 item 2 | `product.hpp:195` |
| `wedge`, `^` | `product_plan` with the pair rule: `reorder_sign` on disjoint masks, nothing otherwise; reads no metric | 0110 items 1 and 2 | `product.hpp:201` |
| `geometric_antiproduct`, `%`; `antiwedge`, `&` | `product_plan` of the base product with each factor entering as its right complement and each term replaced by its left complement, pair by pair (complement masks and signs from `bits.hpp`) | 0115 items 1 and 2 | `product.hpp:208`, `:215` |
| `reverse`, `~`; `antireverse`; `grade_involution`; `right_complement`, `!`; `left_complement` | `unary_map`: one term per blade, the sign of `bits.hpp` and, for a complement, `complement_mask` | 0114 items 1 to 5 | `unary.hpp:51`, `:57`; `mv.hpp:841` |
| `a + b`, `a - b` | `identity_kernel` of each operand onto `sum_layout`, the second with the sign of the operation; no new plan | 0167 ("The sum of two layouts", option 2; Decision item 2) | `linear.hpp:182` |
| `s * a`, `a * s`, `a / s`, `-a` | a loop over the coefficients with the scalar's own operation; every blade maps to itself, so no kernel | 0167 ("Scaling and negation are loops") | `linear.hpp` |
| conversion between layouts (`Type{x}`), `truncate<To>`, `part<To>` | `identity_kernel<From, To>`: positions and orientation signs; `truncate` also checks the dropped positions | 0155; 0168 item 2 | `mv.hpp:621`, `:1050`, `:1111` |
| `get<"name">` | the constant position of the named blade in the layout, with the value category of the argument | 0153 item 2 | `mv.hpp` |
| `bulk`, `weight`, `bulk_dual`, `weight_dual`, `left_bulk_dual`, `left_weight_dual` | compositions of `reverse`, `geometric_product` with the unit antiscalar 𝟙, and the complements (0172's table) | 0172 item 1 | `dual.hpp` |
| `dot` | the terms of the plan of a ⟑ b whose output is the scalar, applied to a and `reverse(b)`: a mask on the existing plan's outputs | 0172 item 2 | `dual.hpp:149` to `:174`, `:529` |
| `antidot` | `left_complement(dot(right_complement(a), right_complement(b)))` | 0172 item 1 | `dual.hpp:577` |
| `project` | `antiwedge(b, wedge(a, weight_dual(b)))` | 0172 item 1 | `dual.hpp:624` |
| `antiproject` | `wedge(b, antiwedge(a, weight_dual(b)))` | 0178 item 2 | `dual.hpp:662` |
| `compose` | one geometric antiproduct plan, Q2 ⟇ Q1; `compose_layout` is that plan's layout | 0178 item 2 | `transform.hpp:654` |
| `transform` | the two geometric antiproduct plans of the sandwich, V ⟇ x and then with V, with the kept and dropped terms selected by mask and the antireverse's and the antigrade involution's signs folded into the coefficients | 0178 item 4 | `transform.hpp:198`, `:206`, `:380`, `:394` |
| `change_basis`, `change_basis_back` | the wedge's pair rule folded over the integer rows of the change (per-basis coefficients and one denominator per row), typed as a unary map is, applied by `run_map` with one division per output | 0220 section 4.1 | `basis_change.hpp:953` to `:1024` |
| the checks and the inverse of a `basis_change` | per-tag data: the isometry check, and the determinant and inverse by fraction-free elimination, which generate no term | 0220 sections 4.3 and 4.4; 0222 item 5 (OI5 (a)) | `basis_change.hpp` |
| `std::formatter` for `mv` (`format.hpp`) | printing, not an operation of the algebra; reads the layout's names | none needed | `format.hpp` |

The declarations that are data rather than operations (the metric factories, `blade_set`, `blades`, `full`, `of_grade`, `merge`, the bit functions of `bits.hpp`, the `*_layout` names) generate nothing; the bit functions are the per-basis signs and masks that the rows above compose.

## Result

DoD-3 is met at R, with the readings of departures 2 and 4: every public operation of the core is a plan or map of the one generator, a composition of other operations, or a kernel of per-basis positions and signs over them; no table is hand-written, precompiled or cached. The mapping existed before this record, spread over the decisions above; it was not in one record until now.
