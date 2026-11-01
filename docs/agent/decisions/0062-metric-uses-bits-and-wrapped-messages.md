# 0062 The metric takes its mask from bits.hpp; one message for a doubly wrong call

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), after `feature/core-bits` merged into `develop` and the second delta review of WU3 (`docs/agent/records/reviews/2026-10-31-wu3-metric-delta2.md`). Supersedes decision 0061, item 3.

## Decisions

1. `metric.hpp` includes `bits.hpp` and bounds n by `mask_bits`. Its own `mask_t` declaration is gone, so the core has one definition of the mask type and of its width.
2. Review N3: when a count exceeds MaxN and MaxN is also wider than a blade mask, as in `conformal<SIZE_MAX, 33>()`, only the count message appears; the MaxN message is suppressed with the other generator-count checks. Accepted rather than moving the MaxN check outside the `if constexpr`. Both alternatives keep every misuse rejected with a true message; accepting needs no change, and moving the check would add a third copy of the MaxN assertion (in R, conformal, and from_gram) for a call that is wrong twice over. The user who fixes the count then sees the MaxN message.
3. Review N2: the compile-fail helper gains `FORBID <literal>...`, carried by `test/expect_output.cmake`, so a test can require that a false message is absent. `core.metric_count_wrap_zero` uses it.

## Consequences

The `mask_t` follow-up of decisions 0061 and the WU3 plan is closed.
