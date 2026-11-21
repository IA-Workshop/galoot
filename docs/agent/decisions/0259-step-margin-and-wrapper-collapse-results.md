# 0259 The step-margin and wrapper-collapse experiments: what the plans take from them

Date: 2026-11-21. Source: the two experiments decision 0253 item 38 asked for, run on copies of Galoot, each reviewed by an independent agent that repeated the main measurements with tools of its own. Their records are in math-tools (`docs/agent/cpp-expert/galoot-step-margin-findings.md`, `step-margin-runs.md`, `wrapper-collapse-galoot-findings.md`; exercises `exercises/cpp/sm-step-margin/` and `exercises/cpp/cr05-wrapper-collapse/`), merged into its `develop` on this date. Galoot's tree was not changed.

## 1. What was measured

Constant-evaluation budgets, Clang 23.1.5 and GCC 16.2.1, exact thresholds, the author's and the reviewer's figures equal:

- The four tightest test units (the antiproduct of two full multivectors of conformal(3)) need 947,839 of Clang's default 1,048,576 steps in the debug, release and sanitizer presets, a margin of 1.106. The test build already raises their limit to 1,200,000.
- In the measure preset (`-O2`, no `_GLIBCXX_ASSERTIONS`) the same units need 1,091,711 steps, which is over the default and 1.099 under the raised limit. Two units with no raised limit are at 1.033 and 1.046 (`identities_five_diagonal_test`, `transform_identities_test`).
- GCC's smallest margin against its operations limit is 5.88.
- Declarations a user can write from the guide that fail on Clang with plain flags: a full product in six generators (rigid(5), conformal(4), R(6,0,0)) stops at Clang's limit of 2,048 terms in a fold expression at any step limit, and builds and runs with `-fbracket-depth=4096` and a raised step limit; several products of large layouts need 1.2 to 4.3 million steps; the composition of two rigid(5) motors builds at `-O0` and fails at `-O2`. GCC builds all of them. Clang's output is 38 to 108 lines and never names `-fconstexpr-steps`.
- A prototype that replaces two linear scans of the generator by binary search gives byte-identical plans on both compilers, cuts Clang's steps to 0.16 to 0.34 of the present count where placement is the largest stage, does not move the four tightest units, and changes five pinned counts of `core.plan_cost_model`.

Wrapper collapse of `product.hpp`'s overload pairs:

- Code lines went from 274 to 276 (one shared body) and to 268 (pairs merged); total lines rose from 457 to 466 in both. The 20 message literals, one declaration and one comment per public name cannot go while each spelling keeps its own message.
- Both variants add notes that name internal templates to compile-fail output, which the suite's tests do not read. The merged variant also puts `[[nodiscard]]` on the rejected path, so a discarded misuse gets a second error.
- Optimized code is identical; compile instructions rise by 0.9% to 2.8% where products dominate.

## 2. Settled by the coordinator

1. **Plan 0048 does not collapse the product wrappers as a unit of their own.** The saving is nil while messages stay per spelling. E1 and E2, which share messages under the owner's ruling (decision 0253 item 15), are the place where the question returns; their briefs name the three costs measured here: internal names in notes, attributes reaching the rejected path, and the compile cost.
2. **Tests blind to notes.** E1's and E2's review compares each changed compile-fail output in full, before and after, not only its first error line and count.
3. **The step margin is reported per preset.** A unit that touches the generator reports the exact threshold of the tightest units in the debug and the measure presets, against the compiler's default and the build's raised limit. This replaces the figure 1.49 of decision 0253's last section.
4. **New rows for the hardening register.** The fold-expression limit and the optimized-build thresholds are sites of plan 0047's kind. They are added to the register when phase 1 starts, for H3 and H5 to judge.
5. **Peak memory as a criterion.** GCC's peak moved by +7.9% and -9.1% under changes that barely changed the work. Until the math-tools backlog's check of that (section 17, item 2) is done, a peak-memory difference under 10% on GCC is reported and not judged.

## 3. Put to the owner

1. **The binary-search change to the generator.** Options: (a) a planned unit after M1, in plan 0048's first code phase, that re-derives the five pinned counts and the guide's figures in separate commits; (b) not now. Tradeoffs: (a) makes three failing user declarations build on Clang, cuts GCC's operations by about two thirds on the affected plans, and is small; it is an algorithm change, not a simplification, and it moves pinned numbers. Recommendation: (a).
2. **The limits the guide does not state.** The fold-expression limit with its remedy, the higher step counts of optimized Clang builds, a rule of thumb for mid-sized layouts, and the shape of Clang's error. Options: (a) one paragraph on the first two in 1.0.1's `limits.md`, text only, and the rest with plan 0049's limits page; (b) all of it with plan 0049; (c) all of it in 1.0.1. Tradeoffs: a user of 1.0.0 who writes a conformal(4) product on Clang gets a long error that names no remedy, and the remedy is one flag; decision 0252 keeps additions out of 1.0.1, and a paragraph needs its own review. Recommendation: (a).
3. **The two test units at margins 1.033 and 1.046 in the measure preset.** Options: (a) raise their limit as the build already does for four units, in the first unit after 1.0.1 that changes `test/CMakeLists.txt`; (b) leave them and rely on the build failing visibly if a compiler release moves the count. Recommendation: (a).

Status: in force. Section 3 is open.
