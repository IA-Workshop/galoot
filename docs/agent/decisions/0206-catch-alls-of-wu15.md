# 0206 Three catch-alls and a literal for blades: WU15's rejections

- Status: recorded. The owner chose them on 2026-11-07 ("accept all recommendations", decision 0196 item 5: plan 0022 O4 and O5 as recommended). O4 rests on the STYLE S7 sentence, which is a draft whose wording the owner has not yet confirmed.
- Date: 2026-11-08
- Recorded by: Claude (agent, Opus 5.5), implementer of WU15
- Bears on: `mv.hpp`, `linear.hpp`, `blade_set.hpp`; STYLE S7; plan 0022 R7, R8, O4, O5; decisions 0033, 0205

## Context

Plan 0022 R7 measured ten misuses that the misuse catalogue did not cover. Three of them gave the compiler's own error at length:
- `Motor{float_line}`: 114 lines on GCC, 43 on Clang;
- `Motor{line_of_another_algebra}`: the same;
- `p + 1.0`: 47 and 19.

A fourth, `blades<Alg, "...">` on a tag with no metric, gave the compiler's account of the `metric_like` constraint, and was routed to the owner (O4).

## Options weighed

- For each case: a catch-all with a literal (decision 0033's form P), or the compiler's error left as it is.
- R7 recommended against catch-alls for the other cases:
  - case 2, `truncate<5>(motor)`: rare, and Clang already reports it in 13 lines;
  - cases 4 and 8, comparisons: a catch-all `==` would make `std::equality_comparable_with` answer yes for pairs that do not compare;
  - cases 5 and 7, a number passed where a multivector belongs: short errors on both compilers.
- For `blades`, a weaker head with a `static_assert` in the helper, which needs STYLE S7 to cover variable templates, or no change.

## Decision

1. **`mv` rejects a source of another algebra or another scalar type.** A single constructor takes such a source, or a class derived from one, and gives one error. When both differ, the error names the algebra. The messages are:
   - "mv: the source is in another algebra; a conversion between layouts keeps the algebra, so give both types the same algebra tag"
   - "mv: the source has another scalar type; a conversion between layouts keeps the scalar type, so convert the coefficients one by one"
2. **`x + s`, `s + x`, `x - s`, and `s - x` are rejected for a multivector x and a number s.** A number here is a ring scalar that is not a multivector or derived from one. There are four overloads, each returning x's `mv` type, so that `auto r = p + 1.0;` adds no second error. The messages are:
   - "a + b: a multivector and a number have no sum; make the number a multivector on the blade 1 and add that"
   - "a - b: a multivector and a number have no difference; make the number a multivector on the blade 1 and subtract that"
3. **`blades` constrains its tag only by `std::destructible`, and its helper rejects with two messages.** The test author split the plan's one text into two, because a tag holding `metric<0>` has a metric and lacks only generators:
   - "blades: the tag is not an algebra; an algebra tag holds its metric as static constexpr value, as in struct rga3 { static constexpr auto value = rigid<3>(); }"
   - "blades: the algebra tag's metric has no generators, and an algebra has at least one; give the tag a metric such as rigid<3>()"

   A tag whose metric is malformed, such as an asymmetric `metric<N>` written out by hand, gets the first message, which is less precise. No test covers that case.
4. **`mv` keeps its `metric_like` constraint**, so that `mv<int, ...>` is not a well-formed type name.

## Consequences

- Each catch-all makes generic probes answer yes, as each comment says:
  - `std::is_constructible_v<Line<double>, Line<float>>` is true, and `std::is_convertible_v` stays false;
  - `requires { x + 1.0; }` is true;
  - `blades<int, "e1">` names a variable whose initialization fails.
- Expected values of earlier tests changed, each in its test group's commit and listed there (W9):
  - `mv_test.cpp`: four `is_constructible` probes;
  - `linear_test.cpp`: five `can_add`/`can_subtract` checks with a number;
  - `core.blade_set_blades_not_metric_like` and `core.blade_set_names_zero`: the message they require.
- The R7 table was measured again after the change (`records/diagnostics/wu15-messages.md`). The catalogue was regenerated with the new rows.
