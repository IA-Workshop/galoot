# 0174 `project` is built in WU11; CONVENTIONS version 1.9 rewords 3.2 and 4.5; the dot and the antidot return multivectors; M3's checking mode and rounding tests are read as plan 0020 proposes

- Status: accepted. The owner answered plan 0020's questions O1 to O4 on 2026-11-04: "take all recommendations" (relayed by the coordinating agent). CONVENTIONS version 1.9 is ratified by that answer. Recorded by the WU11a implementer.
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11a, cpp-expert role
- Bears on: plan 0001 (WU11, WU13, M3), plan 0020 departure 2, readings R6, R7, R8, R14, R15, and section 8; CONVENTIONS 3.2, 4.3, 4.5; decisions 0010, 0161, 0168; the post-WU10 audit (M4(c), the 4.5 wording of review F6 of WU10b)

## Context

Plan 0020 left four questions to the owner:

- O1: plan 0001 puts `project` and `antiproject` in WU13 (M4) but asks for CONVENTIONS 4.3 to be pinned at M3, which holds WU9 to WU11.
- O2: the second sentence of 3.2 says that in a diagonal metric "the bulk keeps the blades whose square is nonzero", which omits the factor the bulk multiplies each blade by (in R(1,1,0), bulk(e2) = −e2), and it does not say that with two or more null generators some blades are in neither part. The audit proposed, for the same version, that 4.5 say what `part` drops "may be" nonzero where it says "is meant to be".
- O3: what M3's "checking mode" and "V12 double tests with forward bounds" mean.
- O4: whether the dot and the antidot return numbers or multivectors.

## Options weighed

O1: (a) build `project` in WU11a, a one-line composition of `weight_dual` with the products, needing no transform machinery (recommended); (b) leave it to WU13 and report 4.3 as pinned at M4. `antiproject` has no definition in CONVENTIONS, so it stays in WU13 either way.

O2: (a) the plan's text for 3.2, with the audit's 4.5 wording, in one version (recommended); (b) leave 3.2 as it is, followed by its first sentence, which the implementation does either way.

O3: (a) M3's checking mode is the existing exact-scalar check of `truncate` with its tests, plus WU11b's inventory showing that no other M3 operation assumes anything it drops is zero; the rounding-scalar check of decision 0010 is built with `transform` in WU13, verified against WU11b's test-side bound; and the V12 tests are WU11b's per-coefficient forward bounds (recommended); (b) build a rounding-scalar check at M3 with no operation that needs it.

O4: (a) both return multivectors, on {1} and {𝟙} or an empty layout, the idiom of `p & g` (recommended); (b) `dot` returns T and `antidot` a multivector; (c) both return T, hiding the antidot's 𝟙.

## Decision

The owner took every recommendation:

1. O1 (a). `project(a, b)` is b ∨ (a ∧ weight_dual(b)), the object first, in `dual.hpp` with `project_layout<Alg, A, B>` and its messages (decisions 0171 to 0173). `antiproject` stays in WU13.
2. O2 (a). CONVENTIONS version 1.9:
   - 3.2, second sentence: "In a diagonal metric, the bulk keeps the blades whose square is nonzero, each times the product of the squares of its generators, and the weight keeps the blades whose complement's square is nonzero, each times the product of the squares of the generators it lacks. In rigid(d) these factors are 1, the bulk is the part without e(d+1), and the weight is the part with it; with two or more null generators some blades are in neither."
   - 4.5: "what they drop may be nonzero" in place of "what they drop is meant to be nonzero".
   The C8 amendment the audit proposed separately is not part of this version.
3. O3 (a). WU11a's operations check nothing (each comment says "never checks"): the bulk and the weight drop blades by definition, which decision 0168's reading of C8 covers; the dot and the antidot compute one coefficient; the duals and the projection drop nothing.
4. O4 (a). `dot` and `antidot` return multivectors. Read the number with `get<"1">(dot(a, b))`; an empty layout says at compile time that the dot is zero.

## Consequences

- CONVENTIONS 4.3 is pinned at M3: its trace row, "a point projected onto a unitized plane has weight +1", is implemented and tested.
- The trace table gains the WU11a rows of plan 0020 section 2.
- WU13's plan proposes the 4.3 text that defines `antiproject`.
- Plan 0020 departure 5 (W7's `feature/<name>/<part>` form cannot be created beside `feature/<name>`) remains with the owner; it is not part of this decision.
