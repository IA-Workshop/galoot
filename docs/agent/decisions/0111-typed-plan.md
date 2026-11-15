# 0111 The typed plan: result set, positions, generation-time checks, and the removal of gp_plan

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7a of plan 0013 in the role of cpp-expert. The owner said to proceed with the plan's defaults; this record holds owner question 3 (departure 5, replace `gp_plan`), answered with the plan's default.

## Context

WU6b's `gp_plan<Alg, A, B>` was a `std::array<plan_term, T>` whose terms name their output blade by mask. To store a product, a multivector needs the blade set of the result and each term's position in it (decision 0091: generated results carry sign +1; CONVENTIONS 1.7: results without a declared type are ordered by grade, then lexicographically by index list). Plan 0013 section 3 asks for the typing as `constexpr` functions over spans, in its direct form, and for cheap checks of the typed result at generation (VVUQ V4).

## Options weighed

1. Replace `gp_plan` by `product_plan<Op, Alg, A, B>`, whose value holds the result set and terms with a position. Chosen (plan default; owner question 3).
2. Keep `gp_plan` as the mask-level variable template beside the typed plan, or as an alias. A second layer with no user once the typed plan exists (C12).

For the typing functions:

1. Insertion into a list kept in canonical order (`collect_result`), then a linear search for each term's position (`type_terms`). Chosen: the direct form, O(T × R) for T terms and R result blades.
2. A presence table indexed by mask. O(T + 2^n): the cost follows the dimension, not the use (C6), and cannot reach n = 32.
3. Binary search for positions, or merging the per-pair runs. Cheaper at large T, not built before a measured need (risk 3 of the plan; decision 0113 records the ceiling the direct form gives).

## Decision

1. `typed_term {std::size_t a, b, out; coefficient_type coefficient;}` with defaulted `==`, `out` a position in the result set; `typed_plan<R, T> {blade_set<R> result; std::array<typed_term, T> terms;}` with defaulted `==`, a structural, trivially copyable aggregate (a test passes one as a template argument).
2. `collect_result(plan, out)` writes the distinct output blades with sign +1 in canonical order (`canonical_less`) and returns their number. It requires a place for each distinct blade, checked by `GALOOT_ASSERT` ("more result blades than places for them"); `plan.size()` places always suffice, and `make_product_plan` gives that many. A precondition rather than a "write what fits" contract, because a list kept in order cannot be built without its storage.
3. `type_terms(plan, result, out)` writes as many typed terms as fit, each with the position of the first result blade with its mask, or `result.size()` when there is none, so it has no precondition; a position at `result.size()` is what the typed check below rejects.
4. `make_product_plan<Op, Alg, A, B>()`, `consteval`, every local `static constexpr` (decision 0102 item 7): the set checks of `make_gp_plan` before generation, then count, fill, the five mask-level checks of a generated plan, `collected` (the scratch result of T places), `typed<R>` (the typed plan), and five checks of the typing, each a `static_assert` with a literal message: the result set out of strictly increasing canonical order ("... out of canonical order or repeats a blade"), a result sign other than +1, a position at or past R, a result blade no term uses, and typed terms out of the order a, then b, then strictly increasing position. `typed_defects_of` computes the five flags in one pass over the result and one over the terms; it is tested directly on hand-written typed plans at each bound.
5. Messages: a `static_assert` message is a literal, so the prefix names no one operation: "product plan:" for both, in place of "gp plan:". The set rejections forbid every message that generation can add, listed in full in `test/CMakeLists.txt`; the old prefix "gp plan: a generated" had not covered "gp plan: generated terms are out of order or repeated".
6. `gp_plan`, `make_gp_plan`, `generate_gp`, the two-set `plan_capacity`, and `filled_gp` are removed (commit `0fce606`), after the tests were migrated (commit `9430423`). The migration reads a typed plan back as mask-level terms (`read_back`, `result.blades[out].mask`) so that every expected value of WU6b's tests is unchanged; the one assertion that changed form is the plan's type (its terms are `std::array<typed_term, 15>` where the plan was `std::array<plan_term, 15>`).
7. A mask listed twice in one input set is still not rejected (decisions 0092, 0102 item 10): each listing gets its own terms, which may land on one position of the result. The result set never repeats a blade.

## Why the result set holds exactly the blades that can be nonzero

Stated in the header's opening comment and in plan 0013 section 2: for independent inputs x and y, the coefficient on a result blade C is Σ c_(a,b,C) x_a y_b over the terms on C, a sum of distinct monomials with nonzero coefficients (the generator writes no zero coefficient and one pair gives one blade at most once), which is not the zero polynomial. So C is in the result exactly when some input makes it nonzero (C6). For x times x the monomials can coincide; that is the collected plan of D8, left to M7.

## Evidence

At `7818335` and later (`test/core/plan_test.cpp`): every typed plan of both operations over `full<n>` × `full<n>` for the 42 forms reads back as its generated terms on a result set equal to the distinct output blades ordered by the test's own rule; the typed rigid(3), conformal(3), and sparse plans equal hand-written values; the result sets of `full<4>` and `full<5>` in R(n,0,0) equal the golden canonical lists; `product_plan` equals direct calls of the typing functions in a constant evaluation and at run time; `typed_defects_of`, `collect_result`, and `type_terms` are tested at their bounds; `core.plan_result_room` requires the room message. Mutants: `records/mutations/0013-plan-results/`.

## Consequences

- WU8's multivector stores a product on `plan.result` and applies `plan.terms` by position.
- WU7b types unary maps and conjugated plans with the same functions.
- The direct typing lowers GCC's `full<n>` × `full<n>` ceiling of the product from 7 to 6 (decision 0113); a cheaper form is a measured follow-up.
