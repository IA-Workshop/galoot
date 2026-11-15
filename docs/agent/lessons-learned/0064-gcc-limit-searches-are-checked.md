# 0064 A threshold found by bisection on GCC is checked above before it is used

- Date: 2026-11-01
- Unit: plan 0015 (certified plan limits), Claude (agent, Opus 5.5), recording the owner's ruling on decision 0125 item 4; the finding is research note 0017's.

## What happened

The first calibration run of note 0017 bisected GCC's `-fconstexpr-ops-limit` for 280 probe units. For 20 of them it reported a threshold too low: by under 1 % in five, by 2 % to 35 % in eleven, and by factors of 3.5 to 27 in four. The fit of those thresholds left residuals up to 8 × 10⁵ operations, which looked like a missing term of the model and was not.

## Cause

GCC evaluates the arguments of a built-in function quietly. When the operation count reaches the limit inside such an argument (for example the `__builtin_expect` of libstdc++'s assertions), the error is dropped and the counter is set to its most negative value, so the rest of the evaluation runs without a limit. A unit can then compile under a limit below its count, and whether it compiles is not monotone in the limit: one probe compiled at 262,144, failed at every limit tried from 262,145 to 6,000,000, and compiled again from 8,000,000. A bisection that meets such a limit converges on it.

## What detects it now

Every limit search on GCC is a checked search: the bisected result is accepted only if limits just above it also compile (+1 to +127 and +0.01 % to +10 %), and a failure there restarts the search above it (`docs/agent/snippets/plan_cost_model/harness.py`, `threshold`; the canary `test/tools/plan_cost_model.py` does the same when it reports a measured threshold). The owner made this the rule for any future GCC limit work (decision 0125, owner ruling, item 4). A single trial build at the default limit is unaffected: its pass or fail is real for that compiler and source, though a pass may be a lucky one. The direction a certificate relies on is safe: a count below the limit always compiles.
