# 0093 index_of checks in one pass, with the sign check once per set

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), answering finding A4 of the review `docs/agent/records/reviews/2026-10-31-unreviewed-fix-passes.md`, under plan 0009 criterion 5.

## Context

Decision 0092 gave `index_of` a scan of the whole set that checks every sign and counts the looked-up mask, followed by the position search. The review measured about three times the cost of the search alone. `index_of` is what the multivector unit will use for every coefficient access, so the cost multiplies.

## Measurement

Each figure is the instructions retired, on both kinds of core added, by one `-std=c++23 -fsyntax-only` compile that looks up every blade of `full<n>`. The programs are `index_of_cost_n9.cpp` and `index_of_cost_n10.cpp` in `docs/agent/records/mutations/0009-blade-set-merge-fix/`, compiled with `-I` on a copy of `include/` that holds each variant; the variants are the `index_of_variant_v*.diff` files there, each against the header as merged (v1); each compile ran under `tools/timed` and is logged in `runs.tsv` as `index_of-cost:<variant>:<compiler>:n<n>`. The machine was shared, but instruction counts do not depend on the load. One run each.

| Variant | GCC 16.2.1, n = 9 | n = 10 | Clang 23.1.4, n = 9 | n = 10 |
| --- | --- | --- | --- | --- |
| v0: position search only, the header before 0092 | 5.83 G | 20.97 G | 8.54 G | 32.25 G |
| v1: 0092 as merged, whole-set scan and then the search | 16.18 G | 62.14 G | 25.73 G | 101.68 G |
| v2: the search, then a scan after the position for a repeat, no sign check (the review's alternative) | 8.95 G | 33.50 G | 14.10 G | 55.16 G |
| v3: v2 with a sign scan in every lookup | 8.95 G | 33.37 G | 19.47 G | 75.96 G |
| v4: v2 with the sign check in a variable template, once per set | 10.38 G | 38.85 G | 14.47 G | 54.54 G |
| The header as committed (v4) | 10.39 G | 39.11 G | 14.80 G | 55.48 G |

GCC's v3 equals v2, which suggests it caches the repeated evaluation of the sign scan for one set (inferred); Clang does not.

## Options

1. v2: the cheapest, but it drops the sign check, so the contract would have to narrow, and `core.blade_set_index_of_bad_sign` would be removed.
2. v4: keeps every check the contract promises. It costs about the same as v2 on Clang and 16% more on GCC, and 1.7 to 1.9 times the unchecked search, against 2.8 to 3.2 times for v1.
3. Keep v1 and record its cost.

## Decision

Option 2. The sign check rejects a hand-written set whose coefficient signs would silently be wrong, and with the check held once per set it costs a lookup almost nothing on Clang and about 4 to 5 G of 39 G on GCC at n = 10. The repeat check, which only a scan of the rest of the set can make, is what remains of the extra cost. Option 1 would save at most the GCC share, and it would move the sign check to WU8 without anything checking signs until then.

## Consequences

- `index_of` still does not check that masks other than the looked-up one are distinct (decision 0092).
- If the multivector unit's build cost shows `index_of` as dominant, the next step is to check the whole set once per type there and leave `index_of` with the search alone (v0), which a later decision can do.
