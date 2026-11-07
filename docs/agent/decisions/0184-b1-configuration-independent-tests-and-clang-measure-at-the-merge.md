# 0184 B1: the conformal(3) `full<5>` cases become configuration-independent, and `clang-measure`'s full suite joins the merge checks; P9 amended

- Status: accepted and ratified (owner answer 2026-11-07, relayed by coordinator: "proceed with recommendations", M3 gate report owner decision 3). PERFORMANCE P9 amended by this record. The fix of the tests is a separate unit.
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), records clerk role
- Bears on: B1 of the post-WU10 audit and of the M3 gate; decisions 0140 and 0141; plan 0021 R12 and criterion 9; PERFORMANCE P9; W13

## Context

`clang-measure` (Clang, `-O2`, no libstdc++ assertions) fails to build `core_product_test` and `core_identities_test`: the geometric antiproduct of `full<5>` in conformal(3) is not a constant expression there (`plan.hpp:553`, "constexpr variable 'count' must be initialized by a constant expression"). The other presets build it. So the merge checks pass while one supported configuration cannot build two units, and WU13's `transform` inherits the same limit. No decision record held the question.

## Options weighed

1. Make the two tests' conformal(3) `full<5>` case independent of the configuration, by grade subsets as WU11a did for dense forms, and add `clang-measure`'s full suite to the checks of a merge into `develop` (recommended).
2. Raise the antiproduct's limit by decision 0140's fix now. It stays proposed under decision 0141's triggers; nothing here needs it.
3. Leave the presets as they are and state the limit at release.

## Decision

1. Option 1. `core.product`'s and `core.identities`'s conformal(3) `full<5>` cases are rewritten so that they build in every configuration. The fix is its own unit, done before WU13 merges.
2. P9's row for a merge into `develop` adds "the full suite under `clang-measure`". It applies from the merge that fixes B1 on; a merge into `develop` before that runs `clang-measure` as WU11 did, with the two known failures stated.
3. Decision 0140 is taken up at the cga3 model's plan. The release record lists the limit beside the C6 exception of decision 0141.

## Consequences

- B1 closes when the fix merges and `clang-measure`'s full suite passes on `develop`.
- Each merge into `develop` costs one more preset's build and suite.
- WU13a states its measured limits (plan 0021 R12) as before, and its tests build under `clang-measure` (criterion 9).
