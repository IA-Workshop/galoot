# 0122 How the one-pass metric_like is written and tested

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), cpp-expert, implementing plan 0014 under the owner's ruling in decision 0121 (option A′).

## Context

Decision 0121's ruling fixes the mechanism: one pass over the pairs j ≤ i that checks symmetry and range together, the verdict held per algebra in an `inline constexpr bool` variable template, a guard that reads only `gram[0][0]`, no copy of the matrix. Plan 0014 leaves the names, the place of each comment, the shape of the tests, and what to do with a check whose mutant no verdict can show.

## Decisions

1. Names. `detail::is_valid_form(const metric<N>&)` for the pass and `detail::is_valid_form_v<Alg>` for the stored verdict, so the variable reads as the value of the function, as `is_metric_v` already does in the same header (STYLE S10, the standard library's `_v` form). Note 0015's prototype called the variable `valid_form_v`; the change is the name only.
2. Order and comments. The concept is the type test, n ≥ 1 from the type, the guard, then the stored verdict, each conjunct reached only when the earlier ones hold ([temp.constr.op]/2). Two plain comments inside the concept say why the n ≥ 1 test reads the type (so that `gram[0][0]` exists) and why the guard reads one entry (a value that cannot be read at compile time would make the stored verdict a hard error, and an entry can be read exactly when the whole value can). The `///` comment states the requirement and that the concept is false, not an error, for a member that cannot be read at compile time, and that the matrix is read once per algebra in each translation unit. Nothing in the header cites a project record.
3. The n ≥ 1 test stays though no verdict shows it with libstdc++. With it removed, the guard reads `gram[0][0]` of a `metric<0>`, whose zero-size `std::array` subscript is not a constant expression in libstdc++ on either compiler, so the concept is still false. Kept because the standard does not require a library precondition violation to end a constant evaluation, so without the test the verdict for n = 0 would rest on one library's implementation; and because it makes the diagnostic name the requirement in the algebra's terms (C9). Its mutant is killed by `core.metric_like_no_generators`, which requires `generator_count` and `metric_like` in the diagnostic; without the test both compilers name the guard instead.
4. Test shape. Six sample entries, two per class (below, within, above the range), with the in-range pair at the bounds, so that an off-by-one in either bound changes a verdict. The concept is compared with a requirement written in the test, read over every position, on every 1 × 1 and 2 × 2 matrix at compile time (one tag per matrix, 1,302 tags, a fold under Clang's 2,048-operand limit, so no blocks are needed). The pass is compared with that requirement and with `is_symmetric() && entries_in_range()` on every 3 × 3 matrix at run time (10,077,696 matrices, about 1.1 s in the debug presets). Expected accepted counts come from the count 2^(n(n+1)/2), derived in the test's comment.
5. The cache mutant (`no_cache`: the concept calls `is_valid_form(Alg::value)` after the guard) is an equivalent mutant: it gives every verdict the same, and only its cost differs. It is recorded as a survivor by design. The P2 measurement of plan 0014 is what shows the cache works. A CTest that fails when one algebra's form is read twice in a unit, in the form of `core.plan_evaluated_once`, is not added in this unit; it is listed as an open item.
6. Unchanged: `metric<N>`, `is_symmetric()`, `entries_in_range()`, `is_diagonal()`, `is_null()`, the factories, and `from_gram` with its two separate messages. No other header changes.

## Consequences

- The mutation record of plan 0014 lists which test kills each mutant; the twelve correctness mutants are killed by tests on gcc-debug and clang-debug (see the plan's outcome for the record and its revision).
- A later unit that adds a second library or compiler should rerun `core.metric_like_no_generators` and the n ≥ 1 mutant there: the reason the test is needed (item 3) is about libraries other than libstdc++.

## Correction, 2026-11-01 (W5 review, findings F1 to F3)

Added by Claude (agent, Opus 5.5); nothing above is rewritten.

- Consequences, first item: "the twelve correctness mutants" is wrong. With `mirror_read_twice`, added after this record was written, the driver has 14 mutants, of which 13 are correctness mutants; all 13 are killed by tests on gcc-debug and clang-debug.
- Item 5 is superseded. The review showed a cheap test that kills `no_cache`: specialize `detail::is_valid_form_v` of one valid tag to false and assert that the concept is false (`stored_verdict_false` in `metric_test.cpp`, commit `5fb4bf6`). It is a white-box test of a `detail` name, accepted because it pins what the mutant removes, that the concept reads the stored verdict, at no measurable cost. It does not show that the compiler evaluates the initializer once; the P2 measurement does. `no_cache` is killed by it on both presets at `9f5f545`, so no mutant survives. The open item "a CTest that fails when one algebra's form is read twice" is withdrawn.
- Item 2: the comments inside the concept now also say why the count comes from the type (the type is known even when the value cannot be read at compile time or is a non-static member), that a failed read inside the requires-expression makes the requirement false rather than an error, and, in the `///` comment, that the full check runs once per algebra and unit; the guard's one read happens at every evaluation (commit `c21a7f8`).
