# 0018 WU9: geometric product, wedge, and their operators

- Status: implemented on `feature/core-products` 2026-11-02 by the coordinating agent (Claude, Sonnet 5.5), without sub-agents; all seven presets passed; awaiting the W5 review.
- Branch `feature/core-products`, worktree `.claude/worktrees/core-products`, from `develop` at the claim commit. Decision numbers 0162 to 0169 (0170 belongs to the run log, 0130 to the job pool).
- Source of scope: plan 0001 (WU9, size S), plan 0016 section 15 "Deferred" and its R6, decision 0159, the 2026-11-02 handoff.
- Goal: public `geometric_product(x, y)` and `wedge(x, y)` on multivectors of one algebra, with the operators `*` and `^` (CONVENTIONS 7.1), each a one-line caller of `detail::apply`; the enumerator rename of decision 0159; a mixed-algebra catch-all with a message in the user's terms.

## 1. Departures, ambiguities, and owner questions (C20)

No departure from the roadmap is planned. Readings this plan follows:

1. WU9 builds only `geometric_product`, `wedge`, `operator*`, and `operator^`. The antiproduct, antiwedge, `%`, `&`, `~`, `!`, scaling by a scalar (`s * a`, `a * s`, `a / s`), sums, and differences belong to WU10 (plan 0016 section 15). Plan 0001 lists WU9 as "gp, wedge, `*`, `^`"; this reading agrees with it.
2. `operator*` is constrained to two multivectors. A scalar operand is not accepted until WU10 adds scaling, so `2.0 * x` is a compile-time error with the compiler's message in WU9. The WU10 plan must make the two `operator*` families unambiguous.
3. Operands share an algebra and a scalar type. Two scalar types are a mixed-scalar mistake, covered by the same catch-all as mixed algebras (section 4). No scalar conversion is added; that is the owner's open item 2 of the handoff.
4. The result layout is `product_plan<Op, Alg, A, B>.result`, as `apply` already chooses. WU9 adds no new result-set policy.
5. Handoff carry-in D2 and D3 (plan 0016, delta review 2): deferred by the owner (decision 0162 item 1). TODO comments mark both sites in `mv.hpp`; WU9 does not touch them, and commit 5 of section 4 is dropped.
6. Owner rulings of 2026-11-02 (decision 0162): the mixed-scalar message says to convert explicitly and adds no conversion (item 2); the lossy-conversion message of decision 0155 is reworded on this branch as the reviews propose (item 8); the `truncate` split is fixed in the WU10 plan (item 6). Decision 0141's antiproduct growth exception, which WU9 does not affect (the wedge and geometric product plans are unchanged).

## 2. Definition of done

- `include/galoot/core/plan.hpp`: the enumerators `gp` and `antigp` become `geometric_product` and `geometric_antiproduct`, with every use in `include/`, `test/`, and `tools/` migrated in one commit that changes no expected value (decision 0159 item 2). Test-only helper names and the golden namespaces keep their names (item 3).
- `include/galoot/core/mv.hpp` (or a new header `product.hpp` included by `core.hpp`, the implementer's choice, recorded as a decision): the four public entities of section 1 item 1, each a short caller of `detail::apply`, with a `///` comment saying what it computes, its result layout, and what it requires.
- A `same_algebra` check (concept or constrained overload) whose failure message names the two algebras and the operation as the user wrote it, in the message style of plan 0016 reading 21 (operation first, mistake in the algebra's terms, fix after a semicolon, no masks or positions).
- Tests (the doctest-expert role): the products agree with the exact blade-product tables of WU6 on every blade pair of `rga3`, `pga3`, and one conformal algebra over `int`, with the nonzero-sign, zero, and shared-generator cases; the operators equal the named functions; associativity of `*` and of `^` and the wedge-of-a-vector-with-itself-is-zero identity on randomized integer multivectors (DoD-10, small n so the check is exact); `x * y` with mixed algebras and with mixed scalars fails with the pinned message (compile-fail tests, C9).
- Trace table rows for the wedge in CONVENTIONS 4.1 and the products in 5.4 move to `yes` in this change if they were `no` (trace rules).
- Mutants, W10 costs, and the P2 growth of the new includer are measured and recorded as plans 0014 and 0016 do.
- The plan's outcome is written; W5 blind review requested by the coordinator; merge by the coordinator.

## 3. Acceptance criteria

1. Same values. Every product of two blades and of two multivectors from the tests of WU7 and WU8 is unchanged by the rename; the rename commit shows no expected-value diff.
2. Equality with the plan. For each algebra above, `geometric_product(x, y)` and `apply<BinaryOperation::geometric_product>(x, y)` return equal values of equal type.
3. Operators. `x * y` and `x ^ y` return the same type and value as the named functions for all tested layouts.
4. Diagnostics. Each mixed-algebra and mixed-scalar mistake gives one error whose first line names both algebras; the texts are pinned by `expect_output` compile-fail tests on GCC and Clang, and their sizes (longest line, line count) are recorded beside the WU8 diagnostic table.
5. Mutants. Each mutant of section 6 is killed on gcc-debug and clang-debug by a test, never by a warning.
6. Checks. P8 and P9: only the tests a change can affect, widened at the merge; every build and test timed with `tools/timed`; all seven presets pass from new empty trees at the final revision of the build inputs.
7. No citation of project records in code, comments, tests, or build files (STYLE S11).

## 4. Approach

1. Claim commit: this plan on `develop`. Branch and worktree as above.
2. Commit 1, the rename: a mechanical change of the two enumerators and their uses. Run the plan and blade-product tests first, then widen.
3. Commit 2, the catch-all and the functions: add the functions beside `apply`, constrained to `mv<Alg, A, T>` and `mv<Alg, B, T>` with one template-parameter pack for the algebra and the scalar. Add the catch-all overloads for the other combinations with `static_assert(false-dependent, "...")` messages, one per cause (mixed algebras, mixed scalars), following plan 0016 section 5 route 2. Check that the catch-all does not capture valid calls and does not make `std::is_invocable` answer yes for a wrong call (a probe test, as decision 0152 did).
4. Commit 3, the operators, defined in `galoot::core` so that argument-dependent lookup finds them, with the same constraints. Probe that they do not hijack operators of other types (`std::complex`, `std::bitset`).
5. Commit 4, the tests of section 2 and the registration of new tests with CTest; the compile-fail tests with their expected-output files.
6. Commit 5, the rewording of the lossy-conversion message (decision 0162 item 8), with its test expectations.
7. Commit 6, the records: diagnostics table, mutant table, W10 and P2 rows, decisions, the outcome.

## 5. Evidence

The WU6 tables are the oracle for blade products (V12-style exact checks over `int`). Randomized identities use the support rational scalar where the check is exact over a field. Doubles appear only in the operator-equals-function test, which compares two computations of the same plan and so is exact.

## 6. Mutants

Planned, each to be run on gcc-debug and clang-debug: swap the two operands of the product inside `geometric_product`; make `wedge` call the geometric-product plan; make `operator^` call `geometric_product`; drop the algebra constraint from the catch-all so that mixed algebras reach `apply`; drop the scalar constraint likewise; flip the sign of one table entry for a shared-generator pair. An equivalent survivor is recorded with its reason.

## 7. Risks

1. The `operator*` overload set grows in WU10 (scaling); the constraint written here must leave room (section 1 item 2).
2. A header-level catch-all can change the answers of `is_invocable`/`is_constructible` that tests pin (decisions 0033 and 0152). Each catch-all gets an `is_invocable` probe in the tests.
3. `operator^` binds below `==`; the tests and the examples parenthesize, as CONVENTIONS 7.1 requires.
4. The compile cost of a new includer: `mv_test.cpp` is already the largest unit (64.5 G instructions on GCC). New tests go in a new file, not in `mv_test.cpp`.

## 8. Open questions for the owner

None. The open items of the handoff are ruled in decision 0162. Still with the owner: the M2 gate (item 10).

## 9. Agent cost estimate

One implementer (cpp-expert role, Opus, high), one doctest-expert for the tests, one blind-reviewer (W5), one delta reviewer if the first review asks for fixes. Size S: the rename is mechanical, and the new code is under 100 lines plus tests. The estimate is recorded in `docs/agent/records/agent-cost.tsv` when the agents finish.

## 10. Outcome

Departures from section 4 (C20): commits 2 to 4 were made as one commit (functions, operators, catch-alls and tests together), then a second commit for lint fixes; the lossy-conversion rewording is commit 5 of the branch as planned, and a final commit fixes the reference markers of the new tests after the first full run failed two reference checks; no sub-agents were used, so there is no agent cost row and no independent test author. The new code is `include/galoot/core/product.hpp` (included by `core.hpp`); tests are `test/core/product_test.cpp` and eight compile-fail sources.

Evidence: all seven presets passed from new empty trees at the final revision of the build inputs (gcc-debug, gcc-release 248; gcc-asan 240; clang-debug, clang-release, clang-asan, lint 250). An earlier full run failed `tests.reference_markers` and `tests.golden_facts` on the new test file; the markers were rewritten and every preset rerun. The trace gate exits 0 with row 7.1 `yes`; six mutants were killed on gcc-debug and clang-debug (records/mutations/0018-products).

Not yet done: the W5 blind review and the merge by the coordinator.
