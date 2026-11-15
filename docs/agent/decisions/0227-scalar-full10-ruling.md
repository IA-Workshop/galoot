# 0227 Scalar times full<10> on GCC: ruling

Date: 2026-11-10. The owner accepted the recommendations made in the report on the investigation (research record 2026-11-09-scalar-full10-gcc).

1. **Option A now.** The comments of `product_plan` and `apply` state that placement costs about terms times blades, name the failing cases (scalar times `full<10>` on GCC, scalar times `full<8>` on Clang), name the remedy `-fconstexpr-ops-limit=134217728`, and say that GCC may word the failure as a span bounds check. Comment-only; no code changes.
2. **Option B on the backlog.** Direct append in `collect_result` when the blade sorts last and binary search otherwise, binary search in `type_terms`, optionally `product_capacity` of 1 when either mask is 0; a core unit with tests first and a recalibration of the stated counts. Deferred under C12 because no current preset needs dense products of n of 8 or more. It goes in the next handoff's open items.
3. **Knowledge base.** The new GCC diagnostic symptoms (the misleading bounds-check wording, the limit that gives no reason) are added to `upstream-awareness.md`.
