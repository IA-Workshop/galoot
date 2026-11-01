# 0125 Certified limits of product plans in place of a measured table

- Status: proposed. Nothing here takes effect until the owner rules on items 1 to 6.
- Date: 2026-11-01
- Proposed by: Claude (agent, Opus 5.5), performance-engineer role, researcher, from research note 0015.

## Context

Decisions 0103 and 0113 state in `product_plan`'s `///` comment the largest `full<n>` × `full<n>` that builds at the default limits, found by building each n until one fails (Clang 23: n ≤ 5, dense form 4; GCC 16: n ≤ 6, dense 5; the wedge 5 and 6). The owner asked whether a specification or proof lets Galoot certify, for a given operation, algebra, and blade sets, that a plan evaluates on a given compiler without building each case, and noted that the recorded ceilings read as a soft bound, which no one has set.

Note 0015 finds:

1. No compiler-independent specification exists. The standard makes every limit implementation-defined, guarantees no minimum, and gives no counting rule; GCC and Clang document only names and defaults; what each counts is in its source only (GCC: one operation per evaluated node, less what its call cache saves; Clang: one step per statement in the classic evaluator, one per taken jump in the bytecode interpreter, which a Clang build may make the default).
2. For one compiler build the count of each phase is an exact weighted sum of path counts, which a Python copy of the generator computes. On Clang 23.1.4 the model is exact: zero error on 98 held-out phase probes and every whole plan tested, and it reproduces decision 0113's table. On GCC 16.2.1, fitted with the call cache off, it is within 0.1 % and is a proven upper bound on the real count.
3. GCC's operation limit can be evaded inside built-in arguments, so pass and fail are not monotone in the limit; a count below the limit always compiles, so a certificate of the form "predicted < limit" is sound, but bisected thresholds need a check.
4. With a binary search in the two typing phases, the model predicts that Clang's wedge ceiling moves from 5 to 6 and GCC's (cold-certified) from 6 to 8, and that no product ceiling moves on Clang, where the count already stops n = 6.

## Options and recommendation, item by item

1. **What `product_plan`'s comment states.**
   - (a) Keep the measured table (decision 0113), restated as measured examples, not limits.
   - (b) State the cost law and certified examples: that the cost is a fixed function of the sets for a given compiler, which phase dominates and how it grows (T·R for the typing, T times the pair cost for the generation), and a short list of uses certified at the default limits on the named compiler versions, each with its predicted count and the limit, in place of the found-by-trial ceilings.
   - (c) State only the growth law, with no figures.
   - Recommended: (b). It answers what a user needs (does my use fit, and how does the cost grow), every figure in it is reproducible from the predictor, and the certified examples are sufficient conditions, so none reads as a bound. The wording must stand without the project's records (STYLE S11); a draft is in note 0015 section 5.
2. **A CTest that guards the calibration.**
   - (a) None; recalibrate by hand when a compiler changes.
   - (b) `core.plan_cost_model`: compile two or three reference plans (for example `full<5>` × `full<5>` in R(5,0,0) for the product, `full<4>` for the wedge) at their committed predicted thresholds and one below (Clang: exact; GCC: with `-fconstexpr-cache-depth=1`, within the stated margin), and fail with the measured threshold when the compiler counts differently. About six compiles, under a second each.
   - Recommended: (b), on the Clang presets and one GCC preset. It is what makes a certified statement safe to keep: it fails the day a new compiler, standard library, or evaluator invalidates the figures, instead of the figures going stale silently. The committed thresholds are ported values with their derivation (the predictor's output and its revision recorded in `records/`), not a call of the predictor from the test.
3. **Where the predictor lives.**
   - (a) `docs/agent/records/performance/constexpr-certification/` as now, an agent tool used to produce the figures.
   - (b) `docs/agent/snippets/`, maintained at each milestone review with the other reusable tools.
   - (c) A user-facing tool shipped with the library.
   - Recommended: (b) once item 2 is accepted, since the figures in item 1 and the test of item 2 are produced and refreshed with it; (c) has no present user (C12).
4. **How limits are measured from now on.** Any future limit measurement on GCC uses the checked search of `harness.py` (a bisection whose result is confirmed by compiling just above it), because a plain bisection reported a too-low threshold for 20 of 280 probes, by up to a factor of 27. A single trial build at the default limit, as decisions 0103 and 0113 made, is unaffected: its pass or fail is genuine for that compiler, even if a pass was lucky. Recommended: adopt.
5. **Cheaper typing.** Building the binary search for result positions is not needed for any present use (the typed and sparse pairs cost under 0.3 % of either limit). Recommended: not now (C12); note 0015 section 4 is the record of what it would buy, should a use of large full sets at compile time become a requirement.
6. **GCC's certificate.** The cold model (cache off) is a sound upper bound but leaves some plans that build uncertified (the dense `full<5>` product: about 44 M cold against the limit of 33.5 M, though it builds). A model of the cache would need the depth and constancy rules of every call in the generator. Recommended: accept the cold model with its margin (0.2 % plus 500 operations), and state on GCC only what it certifies.

## Consequences if accepted

- The `///` comment of `product_plan` (and later `unary_map` and the conjugated plans) is rewritten as item 1 (b) by a unit with its own plan and review; the measured ceilings of decisions 0103 and 0113 remain as history.
- A compiler or standard-library upgrade fails `core.plan_cost_model` until the weights are refitted (about 15 minutes of compiles on the shared pool) and the committed thresholds and the comment's figures are regenerated.
- A change to the generator's code changes the weights the same way, so the same unit that changes the generator refits the model, as it now remeasures the limits.
