# 0171 WU11 runs in two parts; the bulk, the weight, the duals, the dot, the antidot, and the projection get `dual.hpp`; their results follow the layout rule of decision 0165, named by nine public layout templates

- Status: accepted (plan 0020 section 1, departure 1, readings R1, R3, and R13; recorded by the WU11a implementer)
- Date: 2026-11-04
- Recorded by: Claude (agent, Opus 5.5), implementer of WU11a, cpp-expert role
- Bears on: plan 0001 (WU11, M3), plan 0020, decisions 0165 and 0166 (the layout rule and the public layout names), decision 0164 (where the products live), the post-WU10 audit owner item 7 (whether the `*_layout` names stay public)

## Context

Plan 0001 lists WU11 as one unit of size M: bulk, weight, duals, dot, antidot. M3 also needs rounding tests with forward error bounds for every M3 operation, which no unit owned (plan 0019 R5, audit Info 6). The new operations need a home: decision 0165 put the products in `product.hpp` and the involutions and complements in `unary.hpp`, and left WU11 to decide. Their result types need a rule and a spelling that a compiler prints in the user's terms (decision 0166).

## Options weighed

Split:
1. One unit. Eight to nine public functions with their layouts and messages, plus a test-only tracking scalar and rounding tests for every M3 operation: more than a reviewer reads in one sitting (W2).
2. Two parts (chosen): WU11a, the roadmap row (`feature/core-duals`); WU11b, the rounding tests (`feature/core-rounding`), which change only test files and can run beside WU11a, with their WU11 group after WU11a merges.

Header:
1. Unary operations in `unary.hpp` and binary ones in `product.hpp`. `unary.hpp` would include `product.hpp` for the bulk dual's product, and `product.hpp` would include `unary.hpp` for the dot's reverse: a cycle.
2. Calls of `detail::apply` in place of the public functions, so that the existing headers could hold the operations. The operations would no longer be plain compositions a reader checks against their definitions.
3. A new header, `include/galoot/core/dual.hpp` (chosen), that includes `product.hpp` and `unary.hpp`, which do not change. `core.hpp` includes it, and `galoot_core`'s header set lists it. A program that includes only `product.hpp` or `unary.hpp` pays nothing for it.

Result layouts:
1. The input's orientations for the part of the input that the operation keeps. It would keep `get<"e31">(bulk(l))` working in rigid(3), but it needs a second rule that holds only where the bulk keeps blades in place, which is not so in a non-diagonal metric (in conformal(3), G e4 = −e5).
2. Decision 0165's rule (chosen): none of the new operations sends every blade to itself times a sign, so each returns the canonical set of the blades it reaches, orientation +1, in order of grade and then of index list. `Line{bulk(l)}` reads a bulk back under a named type's orientations.

## Decision

1. WU11 runs as WU11a and WU11b, as plan 0020 section 4 lays out.
2. `dual.hpp` holds `bulk`, `weight`, `bulk_dual`, `weight_dual`, `left_bulk_dual`, `left_weight_dual`, `dot`, `antidot`, and `project` (the last after the owner's answer to O1, decision 0174), each a composition of public functions (decision 0172), with their messages (decision 0173).
3. Each result type is declared through a public variable template, as for the products: `bulk_layout<Alg, S>`, `weight_layout`, `bulk_dual_layout`, `weight_dual_layout`, `left_bulk_dual_layout`, `left_weight_dual_layout`, `dot_layout<Alg, A, B>`, `antidot_layout<Alg, A, B>`, and `project_layout<Alg, A, B>`. Each holds the layout its composition gives, so a result type is the same type as the composition's, and Clang prints the name of the operation's layout on the first line of a mismatched comparison (pinned by the `core.dual_compare_*` compile-fail tests). Each is constrained by `metric_like`, so a non-algebra is rejected.
4. The audit's owner item 7 (whether the `*_layout` names stay public) had no ruling when WU11a started, so the nine names are added in the existing form; a later move to `detail` covers all sixteen at the same cost.
5. No operator is added (CONVENTIONS 7.1), and no norm function: `dot(x, x)` and `antidot(x, x)` are the squared norms, and square-root norms are out of version 1 (plan 0001 section 3).

## Consequences

- Nine more public names, which are renamed only at a cost to users after release (W13).
- Every includer of `core.hpp` parses one more header; measured in the WU11a records (`records/performance/duals-cost/`).
- `std::execution::bulk` (C++26) is a function object, so under `using namespace std::execution` an unqualified `bulk(x)` finds it and does not compile; `dual.hpp` says so, as `unary.hpp` does for `std::ranges::reverse`. A variable named `bulk`, `weight`, or `dot` in scope stops an unqualified call, as for any function found by argument-dependent lookup.
