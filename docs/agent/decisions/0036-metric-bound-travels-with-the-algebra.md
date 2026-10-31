# 0036 The generator bound belongs to the algebra, and no entry-type parameter yet

- Status: accepted as the recommended default; applies unless the owner objects
- Date: 2026-10-31
- Decided by: Claude (agent), following the review of the metric unit, within decision 0035 and the owner's rulings 0004 and 0035. The owner may reverse it.

## Context

The review of the metric unit (N2) found that the largest number of generators is a per-call default, so later templates constrained by `metric_like` reject any tag with n above 16, even one built with a larger bound. Decision 0004 says the bound is a template parameter that can be raised without an interface change. It also found (owner question 2) that an entry-type parameter would have one present user.

## Decision

1. `metric<N>` and the `metric_like` concept accept any N from 1 up to the width of the mask type (`std::numeric_limits<mask_t>::digits`). The default of 16 is the default `MaxN` of the factories (`R`, `rigid`, `conformal`, `from_gram`) only. A user who wants more generators raises `MaxN` at the factory call, and every later template accepts the result with no change to its interface.
2. The entry bound is derived from `std::numeric_limits<entry_type>::max()` by the rule of decision 0035 item 2, where `entry_type` is a single alias for `int`. There is no entry-type template parameter until a second entry type has a present user. Widening the type is a one-line change that widens the bound with it.

## Consequences

Later core templates use `metric_like` with no bound argument. The reviewers re-review the changed part of the metric unit.
