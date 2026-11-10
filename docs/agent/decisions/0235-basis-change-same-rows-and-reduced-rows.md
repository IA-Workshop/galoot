# 0235 `basis_change`: `same_rows` in place of `==`, and rows reduced before the checks

- Status: recorded. The owner ruled the rename and the reduction (decision 0225 items 3 and 4); the forms below are the implementer's choices, open to the review and to the owner (C18).
- Date: 2026-11-10
- Recorded by: Claude (agent, Opus 5.5), planner and implementer of plan 0039
- Bears on: `include/galoot/core/basis_change.hpp`, `test/core/basis_change_test.cpp`, the compile-fail tests `core.basis_change_overflow` and `core.basis_change_overflow_elimination`

## Context

`basis_change<N>` had a defaulted `operator==`, which compares the stored rows and denominators entry by entry. Two values for one matrix P, a row scaled with its denominator, compare unequal, so `==` does not mean "the same change" (WU12 blind review N3; decision 0223 item 4 documented it; decision 0225 item 3 renames it before release). Separately, the checks of a change tag ran on the rows as written, so a large common factor of a row and its denominator could overflow `coefficient_type` though P is small (WU12 delta review N3; decision 0225 item 4).

## Decision

1. **The name: `same_rows(a, b)`, a free function**, `[[nodiscard]] constexpr`, true exactly when the rows and the denominators are equal entry by entry, that is when `from_rows` was given the same arguments. `basis_change` has no `operator==`, and a test pins that it is not `std::equality_comparable`.
   - Why this name: a user meets a change through `from_rows(rows, denominators)` and the header's sentence "held exactly, as integer rows over one positive denominator per row", so "rows" is already the word for what is written; `same_rows` reads at the call as the question it answers, and its pairing with `from_rows` tells a reader that it compares what `from_rows` stores. It does not suggest P: a reader who wants matrix equality does not reach for a function named after rows.
   - Weighed: `same_rows_and_denominators` names both members but is long for a rare call and reads as two checks; `same_entries` is ambiguous between the stored integers and the entries of P; `identical` and `same_representation` say nothing a reader can check; a member `a.same_rows(b)` breaks the symmetry of the question and the core's habit of free functions for operations. Keeping `==` with a comment (decision 0223 item 4's state) leaves the misreading that the rename removes. Matrix equality (`==` comparing P) has no present user (C12) and is not added.
   - A program that writes `a == b` now fails to compile with the compilers' own "no match for operator==" (GCC) or "invalid operands to binary expression" (Clang). A deleted `operator==` with a message needs C++26 (`= delete("…")`), beyond the baseline (STYLE S2), so none is declared.
2. **The reduction: in `check_change`, after the denominator check and before every other check**, each row i is divided with its denominator by g_i = gcd(d_i, r_i1, …, r_in), and the reduced rows are the forward rows that the term generation reads. A row with g_i of 0 or 1 is kept, so the function is total. Plan 0039 section 5 proves that the reduction keeps P, that every value the checks and the generation form is the one formed from the rows as written divided by a positive integer, and that the generated maps do not depend on it. The stored value keeps the rows as written: `from_rows` still guarantees its arguments entry by entry, and `same_rows` tells a scaled row from its reduced form.
   - Weighed: reducing in `from_rows`, which would change what a user's value holds and make `same_rows` blind to scaling, against `from_rows`'s documented guarantee; reducing in each check separately, which repeats the work and the code.
   - `detail::inverse_change`, used by one test, is left reading the rows as written (plan 0039 open item O1).
3. **The overflow tests**: the two whose inputs the reduction makes small (`core.basis_change_overflow`, k e1 = m e'1 / m; `core.basis_change_overflow_elimination`, m e'_i over m) take reduced inputs that overflow in the same check, and their old inputs become acceptance tests that compare the maps and results with those of the reduced rows. The messages, the one-error rule, and the FORBID literals stay. The other two overflow tests keep their inputs, which overflow in the same check after the reduction.
4. **`eliminate`**: the two later returns state `.fits = fits` (delta review N2); the value is the same as before, since the loop returns on `!fits` first.

## Consequences

- Code that compared two changes with `==` or `!=` uses `same_rows`; no user code exists before the first release.
- Changes with large common factors in a row are accepted where they were rejected as an overflow. No change that was accepted before is rejected now, and the terms of every accepted change are the same.
- An overflow that the reduced rows still cause is reported as before (plan 0039 open item O2).
