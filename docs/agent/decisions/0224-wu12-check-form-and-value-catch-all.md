# 0224 The change of basis's tag checks: one verdict, one static_assert per fault; and an unconstrained value catch-all

- Status: recorded. The implementer's choices in the header change `30c2275` that applies the owner's rulings of decision 0223 items 1 and 3, open to the delta review and to the owner (C18). It supersedes items 5 to 7 of decision 0221 where they describe the checks; the rest of 0221 stands.
- Date: 2026-11-09
- Recorded by: Claude (agent, Opus 5.5), implementer of plan 0030, cpp-expert role, on `feature/core-basis-change` (worktree `.claude/worktrees/core-basis-change`).
- Number: 0224, the next free number on `develop` (last 0223) and on every branch when recorded.
- Bears on: `include/galoot/core/basis_change.hpp`; `test/CMakeLists.txt` (`core.basis_change_overflow`, `core.basis_change_both_counts`, `core.change_basis_value_as_tag`, `core.change_basis_back_value_as_tag`); `records/diagnostics/misuse-catalogue.md`; STYLE S7.

## Context

Decision 0223 item 3 rules that an overflow in a change tag's checks gives one error with the message first, by computing a flag and rejecting it with one `static_assert`. Item 1 (the hci reading's S1) rules that a tag with several faults gives one error. Item 1 (S6) asks for a catch-all for a value given in place of the tag's template argument, after a cpp-expert confirms the overload set. Decision 0221 items 5 to 7 described the checks as a nest of `if constexpr` with `static_assert`s whose arithmetic stopped the constant evaluation at an overflow (`GALOOT_ASSERT` through `check_fits`).

## Decision

1. **Noting arithmetic.** `detail::multiply_noting` and `detail::add_noting` compute x y and x + y within −max to max; on an overflow they clear a `bool& fits` and return 0, and never stop the evaluation. The elimination, the inverse's rows, the check that the inverse inverts, and the isometry check use them and return, beside their result, whether every value fit (`elimination::fits`, `noted_rows::fits`, `identity_check::fits`). The elimination ends at the step where an overflow happens, so no later step divides by a pivot that an overflow made 0.
2. **One verdict.** `detail::check_change(change, source, target)` runs the checks after the counts in the order a user fixes them (denominators, independence, inner products, the inverse's self-check, the inverse's rows) and returns a `checked_change_data` whose `fault` is one `ChangeFault`: `denominator`, `overflow`, `dependent`, `isometry`, `inverse`, or `none`, the first fault found. An overflow in any check is the fault `overflow`, since a check that overflowed decides nothing. `generator_count` marks the data of a tag whose counts failed.
3. **One `static_assert` per fault.** `make_checked_change<C>` asserts the source's count, then, only when it is right, the target's (S1), each written as the comparison so that both compilers print the counts. When both are right it reads `check_change` once into a `static constexpr` and asserts one named `bool` per fault (`positive`, `fits`, `independent`, `isometry`, `inverse_holds`), each with its message. The faults are exclusive, so at most one assertion fails and a tag gives one error, the message on its first line. Each condition is a named local, so that Clang's "requirement" text names the local and not `detail::ChangeFault`, which the tests forbid on the first error line.
4. **The term generation keeps stopping.** `change_multiply` and `change_add`, used by `blade_image`, `image_denominator`, and `typed_change`, are the noting operations followed by `check_fits`, the one-line `GALOOT_ASSERT` of decision 0221 item 7. So an overflow while generating a layout's terms still ends the constant evaluation with the message in the quoted source line, as for every overflow in the core's term generation; the reviewer's recommendation and decision 0223 item 3 left that route as it was. `detail::inverse_change`, callable at run time, asserts the elimination's and the rows' `fits` through `check_fits`.
5. **The value catch-alls, and S7.** `template <auto Value, multivector X> change_basis(const X&)`, and the same for `change_basis_back`, reject a value given where a change tag belongs with one error. A type argument does not match an `auto` non-type parameter, so these templates are never viable for a valid call or for the other catch-alls' cases, and the overload resolution of every other call is unchanged; the P8 selection passes on both compilers, and the two new compile-fail tests give one error with the message first on GCC and Clang. `Value` is constrained by no named concept, a departure from STYLE S7's "every template parameter is constrained by a named concept": any value given there is the same mistake, and a constraint (for example to `basis_change<N>` values) would leave other values, such as a metric passed by mistake, to the compiler's unrelated account. Its `///` comment says so.

## Options weighed

- For items 1 to 3, a flag per check inside the existing `if constexpr` nest: the same messages, but about seven levels of nesting (inferred, not tried, to exceed `clang-tidy`'s cognitive-complexity threshold of 25, which `eliminate` already reached), and each check's arithmetic spread over the nest. One verdict computed by a plain constexpr function keeps the order of the checks in one readable place.
- For item 3, conditions written as `checked.fault != ChangeFault::overflow`: shorter, but Clang prints the condition, naming `detail::`, on the first error line.
- For item 4, the noting form in the term generation too: one error there as well, but every map of every layout would carry the flags through `blade_image` and `typed_change`, and the ruling asked for the tag checks only.
- For item 5, a constraint `requires (!std::is_class_v<decltype(Value)> || ...)` or to change values: no present case it would serve, and it narrows what the message catches.

## Consequences

- The misuse catalogue's row for the overflow says one error with the message first (`0fa98ac`); the overflow message has two places, the `static_assert` and `check_fits`.
- `change.toml`'s anchors for the checks were rewritten to this form (mutation README, "After the review rulings").
- Decision 0221 items 5 to 7 carry a line pointing here.
