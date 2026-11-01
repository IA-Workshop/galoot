# 0107 The growth that remains is the metric_like check, on both compilers

- Status: accepted (recorded by the author; the owner may reverse it, and item 4 restates the question of decision 0105 item 3 for the owner)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), applying the W5 review of plan 0012 (`records/reviews/2026-11-01-plan-0012-cost-growth.md`, findings F1 and F3).
- Supersedes: in decision 0105, Decision item 1 ("the growth of a fixed use is gone on GCC"), the sentence "On GCC the plans do not grow in either copy", and the framing of the remainder as Clang's alone; in decision 0104, the sentence "so every index stays below N", as qualified in item 5 below. The rest of 0104 and 0105 stands.

## Context

Decision 0105 measured the growth of a fixed use, the four typed rigid(3) pairs in R(n−1,0,1), as the plans' unit minus a baseline unit, both from `measure_plan.py`. Both units contain `static_assert(core::metric_like<alg>)`, so the baseline evaluates the concept once before any plan is formed. The review (F1) measured the same use without that `static_assert` in either unit, which is how a program that only uses the algebra is written.

## Measurements

Two independent runs, both pinned (the reviewer's on CPU 12, mine on CPU 10), three repeats, Galoot's flags at -O0, `-c`, n = 4 to 16, retired instructions of the plans minus the baseline:

| Units without the concept `static_assert` | GCC 16.2.1 | Clang 23.1.4 |
| --- | --- | --- |
| Four plans, headers of `0f1039a` (review) | 187.8 → 234.9 M (+25.0%) | 347.1 → 439.2 M (+26.5%) |
| Four plans, headers of `c97fac5` (review) | 182.0 → 199.9 M (+9.9%) | 342.2 → 366.1 M (+7.0%) |
| Four plans, headers of `c97fac5` (mine, `out/probe-no-assert-c97fac5/`) | 183.3 → 201.3 M (+9.8%) | 345.5 → 366.7 M (+6.1%) |
| One plan, headers of `c97fac5` (mine) | 48.1 → 65.7 M | 127.6 → 147.6 M |
| Four plans, `metric_like` reduced to its type test (mine) | 180.2 → 180.9 M | 341.4 → 341.7 M |

My run: other work on CPU 10 or its sibling at most 0.02 s per compile, spread at most 3.6 M, 13:04:41Z to 13:05:03Z; headers at `45c2153`, whose `include/` is `c97fac5`'s. Without the `static_assert` the baselines are flat on both compilers (GCC 803.2 → 804.1 M, Clang 743.9 → 745.9 M). The reviewer's trace counts the three value conditions of `metric_like` (`metric.hpp:276` to `:278`): once in a unit with four plans and no `static_assert`, never in a unit with neither.

## Decision

1. After the set-bit loops, the growth of a fixed use from n = 4 to 16 is about 18 M instructions on GCC and 20 to 24 M on Clang, in units that do not assert the concept themselves; +9.8% and +6.1% to +7.0% of the four plans. It is the same for one plan as for four, and it disappears on both compilers when `metric_like` keeps only its type test.
2. Its cause is one evaluation of `metric_like`, which reads all n² Gram entries, in each translation unit that uses the algebra in a constrained template. GCC reuses an evaluation the unit has already made (the `static_assert`), which is why decision 0105's figure for GCC, against a baseline holding that `static_assert`, showed +0.2%. Clang makes a second evaluation when the unit also evaluates the concept outside a constraint (decision 0105's trace counts). Why Clang does not reuse it is still inferred, not read in its source.
3. So the loop change of decision 0104 removed the growth that came from the generator (GCC +25.0% to +9.9%, Clang +26.5% to +7.0%, without the `static_assert`), and what remains on both compilers is the concept check, a cost per algebra and unit, not per plan.
4. For the owner, in place of decision 0105 item 3: whether this growth, on GCC and on Clang alike, falls under PERFORMANCE P2's exception for growth that follows from the signature data. The reviewer's view, given as input and not as a decision: the check validates the signature's own n² entries, once per algebra per unit (twice on Clang when the program also asserts the concept), and does not grow with the number of plans, which fits the letter of the exception; but most of its size is redundancy, because the concept makes three passes over the entries (`entries_in_range()` twice and `is_symmetric()` once) and Clang may add a second evaluation, up to six passes where one would do. The reviewer would accept the exception for the n² check itself and record the redundant passes as a cost to remove in a later unit, for example by a single pass or by a result held in a variable template per tag. No change to `metric.hpp` is made here; it is outside plan 0012.
5. Out-of-range masks in unchecked builds (F3). With `GALOOT_CHECKING` 0 at run time, `blade_product`'s range assertion is not evaluated, and a `b` naming a generator beyond e_N now makes the contraction and correction loops (`blade_product.hpp:219`, `:254`) read `g.gram[i][j]` with j ≥ N, where the old loops stopped at N and skipped it (the reviewer's `oob_unchecked.cpp`: old `+1 e10`, new a libstdc++ subscript assertion, or a read past the row without `_GLIBCXX_ASSERTIONS`). An out-of-range `a` already read past the matrix in the old code. This happens only when the stated precondition is violated. Decision 0104's "every index stays below N" holds in constant evaluation and in checking builds, not in unchecked run-time builds. No public path reaches it: `blade_product`, `generate_gp`, and the recursion helpers are in `detail`, and their only caller outside the tests is the `consteval` `make_gp_plan`, which checks the range of both sets before generating, at compile time, in every build. So no code changes.

## Consequences

- The P2 measurement of a fixed use should state whether its units evaluate the concept outside a constraint; with the `static_assert` in the baseline, GCC hides the check, and Clang shows one of the two evaluations it makes.
- The plan's outcome is corrected in a dated section of plan 0012.

## Note, 2026-11-01: item 4 is ruled

Added by Claude (agent, Opus 5.5) in plan 0014; nothing above is changed. The owner ruled item 4 together with decision 0121 (its owner-ruling section, item 3): the growth that remains after `metric_like` reads the form in one pass, once per algebra and unit (about 11 M instructions from n = 4 to 16 on each compiler), is accepted as PERFORMANCE P2's exception for growth that follows from the signature data. Plan 0014 implements that pass and records the growth before and after.
