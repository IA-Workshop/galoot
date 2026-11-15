# 0222: The owner's rulings on decision 0220 (the change-of-basis interface) and the part N tolerance rule

Date: 2026-11-09. Status: recorded. Source: the owner's reply "Accept all recommendations" to the open items of part D and part N of plan 0030.

## Interface (decision 0220)

1. OI1: the public names are confirmed as written: `basis_change<N>` (members `rows`, `denominators`), `basis_change_like`, the tag members `source`, `target`, `value`, `from_rows`, `conformal_change<D>()`, `change_basis`, `change_basis_back`, and the two `_layout` names.
2. OI2: the value checks run once per tag at first use, not in `from_rows`.
3. OI3: tag messages carry one `basis_change:` prefix.
4. OI4: one message covers both an integer scalar and a scalar without division.
5. OI5: the determinant and inverse come from fraction-free elimination, recorded as per-basis data composed with the fold.

## Tolerance rule (record `numerical/wu12-change-basis-tolerances.md`)

6. Regime E with non-power-of-two denominators (correctly rounded equality) is adopted.
7. Part C keeps one division per output, not a reciprocal multiply; decision 0221 states the form, and the rule is rechecked against it.
8. Part B fixes `exact_rational` in `test/support/tracked.hpp` (it aborts on 1024.0 against its own comment) with a test; the fix only widens what it accepts.
9. float and complex claims cover exactness only.
10. Regime C uses the ratio check against `ratio_limit`, not the margin-one fallback.
11. Random 3 to diagonal is excluded from regime C; random 7 and random 2 cover the criterion.
12. The survivors the record predicts in `rounding.toml` (per-term division in regime C and on dyadic changes) are not counted as test gaps.
13. A knowledge-base note on testing a quotient against an int64 rational oracle is written once part A has reported.

## Other

14. The math-tools branch `tools/replay-opam-root` merges after ga-P7 and ga-P11 are run from the main checkout and ga-P11's command step passes; `run.sh`'s galoot path is fixed in a separate small branch.
