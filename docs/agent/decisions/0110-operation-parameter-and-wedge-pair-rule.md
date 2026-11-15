# 0110 The operation parameter of the plan generator, and the wedge as its definition

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7a of plan 0013 in the role of cpp-expert. The owner said to proceed with the plan's defaults for its owner questions; this record holds owner question 4 (departure 6), answered with the plan's default.

## Context

Plan 0013 grows `include/galoot/core/plan.hpp` from the geometric-product plan into a generator for every binary product of the core. STYLE S16 admits a template parameter only where two present uses differ in it, so the operation parameter needs a second operation now: the wedge of CONVENTIONS 4.1. The constitution's C4 describes the one generative mechanism as taking "a signature, an operation, and the basis elements of its inputs". CONVENTIONS 4.1 defines e_A ∧ e_B as the reorder sign times e_(A ∪ B) for disjoint A and B and 0 otherwise, independent of the metric, and states that it equals the part of e_A ⟑ e_B of grade |A| + |B|.

## Options weighed

For the form of the operation:

1. An enumeration value as a non-type template parameter, with the pair rule selected by `if constexpr`. Chosen: the most direct form of a value that selects a rule, structural, and it names the algebra's operations in diagnostics.
2. A tag type per operation, or a callable pair rule passed as a template argument. More machinery (a type per operation, or a concept for callables) for no present use.

For the wedge's pair rule (plan departure 6):

1. (a) The definition: one `reorder_sign` per disjoint pair, nothing on a shared generator. Chosen. It is the direct form (C3), it does not read the metric, as 4.1 says the wedge does not, and it costs one step per pair. The per-pair sign is per-basis data that `bits.hpp` already provides and proves, so it adds no machinery under C4.
2. (b) Generate e_A ⟑ e_B and keep the terms of grade |A| + |B|. One mechanism in the narrowest sense, but it costs a full geometric product per pair and makes a metric-free operation depend on the metric's generation. It is kept as a test (identity C3 of the plan's section 6).

## Decision

1. `enum class BinaryOperation : std::uint8_t { gp, wedge }`, in `galoot::core::detail`. The plan proposed `binary_operation`; STYLE S10 and `.clang-tidy` (`EnumCase: CamelCase`) name an enumeration type in CamelCase, as `NameError` is, so the name is `BinaryOperation`, with snake_case enumerators. `std::uint8_t` because clang-tidy's `performance-enum-size` asks for the smallest base.
2. `pair_terms<Op, Capacity, N>(g, a, b)`: `blade_product` for `gp`; for `wedge`, `GALOOT_ASSERT(in_algebra(a | b, N), "blade has a generator beyond e_n")`, then nothing when `a & b` is nonzero and otherwise the one term `reorder_sign(a, b)` on `a | b`. The range check uses `in_algebra` from `bits.hpp` rather than the shift form of `blade_product`, because N ≥ 1 for every metric and `in_algebra` handles N = `mask_bits`. The message is the one `bits.hpp` and `blade_product` use. The wedge reads nothing of `g`.
3. `pair_capacity(op, a, b)` is `product_capacity(a, b)` for `gp` and 1 for the wedge, and `plan_capacity(op, as, bs)` takes the operation, so the wedge's scratch is one term per pair. It is a function parameter, since it selects only a number.
4. `generate_plan<Op, Capacity, N>(g, as, bs, out)` has the contract of WU6b's `generate_gp` (order a, b, canonical order within a pair; count returned, first `out.size()` written; signs folded), calling `pair_terms`. `plan_term` with its mask output stays the type the generator writes.
5. The trace row of 4.1, "the wedge of two blades that share a generator is zero", is `yes` from commit `7818335`, pinned by the plan test's wedge-rule case.
6. clang-tidy 23 reports `modernize-avoid-c-style-cast` at the operation argument of `product_plan`'s initializer in each instantiation: the AST of a substituted enumeration template argument is a `SubstNonTypeTemplateParmExpr` over a `CStyleCastExpr` of a literal (seen in a `-Xclang -ast-dump` of the plan test), which the source does not contain. A 16-bit base gives the same finding. The finding is suppressed at that line with the reason (commit `38f2b30`); the alternative of a tag type per operation (rejected above) would avoid it at the cost of a type per operation.

## Evidence

- Tests at `7818335` (`test/core/plan_test.cpp`, CTest `core.plan`): the wedge plan of `full<n>` × `full<n>` for the 42 forms of up to six generators equals the rule written in the test (its own inversion count on A | B), equals the grade |A| + |B| part of `blade_product`, is the same for every form of one n, equals the geometric product in R(0,0,n) for n = 1 to 6, and is associative on every triple of blades; it equals the top-grade part of the four golden product tables; the typed rigid(3) and conformal(3) wedge plans hold their hand-written values.
- Rejections: `core.plan_wedge_beyond_n` (the set check under the wedge), `core.plan_wedge_pair_beyond_n` and `_second` (the pair rule's own range check, each operand).
- Mutants of the rule (shared generator kept, sign +1, sign reversed, sign times (−1)^gr(A), range check removed or of one operand only, the wedge computed as the product, the product for every operation, the capacities swapped or the wedge sized as the product) are in `records/mutations/0013-plan-results/`.

## Consequences

- WU7b adds `antigp` and `antiwedge` to the enumeration and builds them from the two pair rules here by complementing the inputs and outputs.
- WU9 builds the public `gp` and `wedge` on multivectors and the operators `*` and `^` on these plans; the plan names stay in `detail` (decision 0015).
- An operation with a pair rule that needs more than one term per pair names its capacity in `pair_capacity`.
