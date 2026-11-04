# 0176 Rounding tests and checking-mode inventory for the bulk, the weight, the duals, the dot, the antidot, and the projection

- Status: accepted (extends decision 0175, which is merged and stays as recorded; the owner's answer to plan 0020 question O3, "yes", 2026-11-04, relayed by the coordinator, covers these operations too)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11b's last group, numerical-expert role
- Bears on: decision 0175 (tolerance policy, the check, the M3 checking-mode inventory), decisions 0168 and 0172, VVUQ V12, constitution C8, plan 0020 (R14, R15, section 4 step 4)

## Context

Decision 0175 put the V12 rounding tests and the checking-mode inventory on every M3 operation then on `develop`, and left WU11a's operations as pending: `bulk`, `weight`, `bulk_dual`, `weight_dual`, `left_bulk_dual`, `left_weight_dual`, `dot`, `antidot`, and `project` (`include/galoot/core/dual.hpp`, merged at `535c91e`). Decision 0175 is merged, so this record extends it rather than amending it. The tolerance policy, the check (`within_rounding_bound`, the margin of two roundings, and the ratio limit 1 + 4u), and the tracking scalar are 0175's, unchanged.

## Checking-mode inventory, the rows 0175 left pending

`dual.hpp` contains no `GALOOT_ASSERT`, calls no `truncate`, and its header comment says "Nothing here checks anything". Read against reading R14 and the reading of C8 in decision 0168:

| Operation | Drops part of its value? | Check in checking mode |
| --- | --- | --- |
| `bulk`, `weight` | Yes, by definition: G and its antiexomorphism drop the blades they send to zero | None: an operation whose purpose is to discard assumes nothing (decision 0168) |
| `bulk_dual`, `weight_dual`, `left_bulk_dual`, `left_weight_dual` | They drop what the bulk or the weight drops, by the same definition, and nothing more | None |
| `dot`, `antidot` | They compute one coefficient, the definition's; the restricted kernel is a way of computing it, not a discard from the user's value | None |
| `project` | No | None |

At compile time each spelling checks with a `static_assert` that the scalar type holds every exact coefficient of its composed plan; that is not a checking-mode check. With these rows the M3 inventory of decision 0175 is complete. The rounding-scalar check of decision 0010 still belongs to `transform` (WU13).

## The tests

`core.rounding` adds three families and a hand case, with the check of decision 0175:

1. **The bulk, the weight, and the duals**, against G x and left(G right(x)) from the golden `exomorphisms` and `complements` tables, the duals as right or left complements of those. On rigid(3), conformal(2), conformal(3), and the degenerate form, G sends each blade to at most one blade with coefficient ±1, so these operations only move, negate, or drop coefficients: every result must be the exact value (`Exactness::exact_value`), on full layouts and, in rigid(3), on the point, line, plane, and motor. The tracked run counts one rounding for the product by the unit antiscalar, a product by 1 that the double operation makes exactly, so the bound check already requires equality with fl(e) = e; the exact-value check states the claim directly and is equivalent to it (the mutant that removes it survives, below). On the random form, G has entries other than 0 and ±1, so a product by an integer such as 3 rounds only if the input has more than 51 significant bits: there the inputs are full-width, p / 2^53 with \|p\| < 2^53, and the results are checked within their bounds.
2. **The dot and the antidot**, against aᵀ G b on the scalar and aᵀ left(G right(b)) on the antiscalar, on full layouts of the five forms and on pairs of rigid(3) points, lines, planes, and motors. The hand case pins the restricted kernels: the dot of two rigid(3) points and the antidot of two planes are each three products added in turn, three roundings deep, with the magnitude Σ \|x_i y_i\| over the three Euclidean components.
3. **The projection**, against b ∨ (a ∧ weight_dual(b)) composed from the tables in its three stages (the weight dual from the exomorphism and complement tables, the wedge from the product table, the antiwedge through the complements). Its exact values are products of three inputs, so its inputs are p / 2^18 with \|p\| < 2^18: a product of three needs up to 54 bits and rounds in double, and every exact value and magnitude keeps a denominator of at most 2^54 and a numerator within std::int64_t. Rigid(3) point onto plane, point onto line, line onto plane, and full layouts of the five forms; conformal(3) `full<5>` builds on Clang's optimized build without libstdc++'s assertions.

Alternatives weighed for the random form's duals: keeping the 27-bit inputs, on which products by small integers and sums of three are exact, so the family would never round there and its ratio would be 0, which tests nothing about rounding; or making the inputs wide with shifted exponents, as for sums, whose magnitudes times the entries of G (up to 31) would overflow the rational. Full width without shifts rounds and fits.

## Observed ratios

At `c32c619`, gcc-debug, seed 0: the bulk, the weight, and the duals 0.592 (all from the random form; every other result exact), the dot and the antidot 0.973 (0.441 with two or more roundings), the projection 0.387. Five seeds are in `records/mutations/0020-rounding-wu11/README.md`. No ratio exceeds 1; each family checks its largest computed ratio against the limit 1 + 4u.

## Consequences

- The M3 checking-mode inventory and the V12 double tests cover every M3 operation.
- The mutants of this group (records `mutations/0020-rounding-wu11/`) include one-ulp nudges of the dot's result, of the projection's result, and of the bulk's result, and the kernel-level float and nudge mutants of the first group, which reach the dot, the antidot, and the projection through the shared kernels.
- `dual.hpp` is unchanged; this group changes only `test/core/rounding_test.cpp` and records.
