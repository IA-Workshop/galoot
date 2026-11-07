# 0178 WU13a: transform.hpp, the result layouts, the transform's kernels, and its checks

- Status: recorded
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU13a
- Bears on: plan 0021 (departures 3 to 5; readings R1, R2, R4 to R6, R8, R11, R12; criteria 6 to 10); CONVENTIONS 4.3, 4.4, 4.5 (1.10); decisions 0009, 0165, 0171 to 0173, 0176, 0177, 0187, 0192

## Context

WU13a adds three public functions: `compose(Q2, Q1)` and `transform(x, V)` of CONVENTIONS 4.4, and `antiproject(a, b)` of 4.3. Plan 0021 proposed where they live, what they return, how `transform` is computed, and what it checks; the owner approved its questions (decision 0192). This record states what was built and why, where the plan left a choice.

## Decision

1. **Headers.** `compose` and `transform` are in a new header, `include/galoot/core/transform.hpp`, which includes `product.hpp` and not `dual.hpp`; `core.hpp` includes it, and `galoot_core`'s header set lists it. `antiproject` and `antiproject_layout` are in `dual.hpp`, beside `project` (O3), with the spelling `antiproject_function` added to `DualSpelling` and a row in the header's table.
2. **Result layouts.**
   - `transform` returns x's own type, `mv<Alg, X, T>`, with x's orientations: CONVENTIONS 4.4 says the result has the blade set of x (plan departure 5). The terms that reach x's blades are moved to x's positions and multiplied by x's orientation there, as `make_product_kernel` does for a target.
   - `compose` returns `mv<Alg, compose_layout<Alg, A, B>, T>`. `compose_layout` is public and equals `geometric_antiproduct_layout<Alg, A, B>`, canonical, so that Clang's first line of a mismatched comparison names the operation's own layout (decision 0171's reason); decision 0187 makes it public interface.
   - `antiproject` returns `mv<Alg, antiproject_layout<Alg, A, B>, T>`, with `antiproject_layout` = `wedge_layout<Alg, B, antiwedge_layout<Alg, A, weight_dual_layout<Alg, B>>>`, canonical.
3. **The parity.** `detail::antigrade_parity<Alg, S>` is a `detail::Parity` (`even`, `odd`, `mixed`) computed from the antigrades n − k of S's blades. An empty layout is `even`, vacuously, and so is accepted: both forms give zero. `transform` rejects a `mixed` V and `compose` a `mixed` operand at compile time (D9, O4). There is no public antigrade involution: its sign is folded into the first stage's terms (R1).
4. **The kernels.** `transform` is the two plans the core already generates, `product_plan` of the geometric antiproduct of V's layout with X, and of that result (the middle layout) with V's layout, composed with per-blade signs (C4, as decision 0172 admitted the dot's kernel):
   - the second stage's kept kernel: the terms of the second plan whose output is a blade of X, with the antireverse's sign of V's blade and, for an odd V, −1 folded into each coefficient;
   - the second stage's dropped kernel: the other terms of the second plan, onto the dropped blades in the order of the plan's result;
   - the first stage's kernel, in two selections: every term of the first plan, or only the terms whose output the kept second-stage terms read; for an odd V, the antigrade involution's sign of x's blade, (−1)^ag, is folded into each coefficient.

   Both selections of the first stage keep the plan's order and write each middle blade at its place in the middle layout, so a blade that both reach gets the same sum in the same order, and the kept coefficients are bitwise the same in both modes. Folding a sign is exact under IEEE 754: a coefficient of −1 turns an addition into a subtraction, and fl(a − b) = −fl(b − a) under rounding to nearest, so the result equals the unfolded antiproducts written out term by term.
5. **The exact check (C8, 4.4, 4.5, R8).** For an `exact_scalar` with `==` (as `truncate`), when `detail::checking_now()` is true (every constant evaluation; run time with `GALOOT_CHECKING` 1), `transform` computes the whole first stage, the kept and the dropped second-stage terms, and compares each dropped coefficient with `T{}` once, by `GALOOT_ASSERT`. Otherwise it computes only the restricted kernels. Nothing is checked for a scalar that is not exact; the rounding check of decision 0010, for the IEEE binary types that 4.5 names, is WU13b's (O5, O8).
6. **The narrow-scalar check.** `transform` is rejected when the scalar cannot hold a coefficient of either whole plan, V ⟇ x or the middle layout ⟇ V; `compose` when it cannot hold one of the plan of Q2 ⟇ Q1; `antiproject` as `project` (the weight dual's coefficients of b).
7. **Rejections of mismatched operands.** One catch-all each for `transform` and `compose`, through `detail::reject_transform_operands` with a `TransformSpelling` of their own; `product.hpp`'s and `dual.hpp`'s rejecting functions gain nothing for them. A requires-expression that asks a catch-all is told yes, as for the products.

## Options weighed

1. **`transform` as a composition of public functions and `part`** (the planner's `compose.cpp` probe). It needs no new `detail` code. The probe saw it reach the restricted count of multiplications on GCC but not on Clang (62 for a point, against 48), and GCC kept the odd composition out of line. WU13b's bound needs the terms with absolute coefficients, which no composition of public functions gives. Not chosen.
2. **Restricted kernels over the existing plans** (chosen). About 200 lines of `detail` code over `product_plan`, `kernel_term`, and `run_product`, with S7 assertions on the term counts in `transform_test.cpp` (the implementer's test, as decision 0173 placed the dot kernel's).
3. **A kernel that collects like terms of the sandwich** (v_i v_k + v_k v_i), toward Lengyel's hand forms. That is plan 0001's D8 question (Spike E, M7), not WU13's; the counts below are its baseline.

For the narrow-scalar check, the alternative was to check only the terms the unchecked path computes, as `dot` does. But checking mode computes every term of both plans, so a scalar accepted in an unchecked build could overflow in a checking one; checking both whole plans makes the answer the same in every mode. For the checking path, the alternative was to restrict the first stage to the terms the dropped terms read as well. The whole first stage is simpler, gives the same kept coefficients (item 4), and its cost is in checking mode only, which no bound covers (C14).

## Consequences

- Term counts in rigid(3) (pinned by the kernel test): a motor or a flector on a point 20 + 28 = 48 products in the unchecked path, 20 + 48 = 68 with the check; on a line 72 and 84; on a plane 48 and 76; on a motor or a flector 96, nothing dropped. For these layouts the first stage's restriction removes no term.
- Run code, limits, and build cost are measured in `records/performance/transform-cost/` and stated in `transform.hpp`'s comment. GCC 16 inlines every kernel; Clang 23 keeps `transform` out of line behind a tail jump, and in `transform(line, motor)` also keeps the first stage's kernel out of line (one call); `compose` of two motors there is a tail jump to its kernel.
- `transform` does not unitize, and nothing checks V ⟇ V̰ = λ𝟙: the comment says so (R3, C8).
