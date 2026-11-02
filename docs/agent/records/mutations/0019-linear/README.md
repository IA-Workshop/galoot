# WU10b sums, scaling, negation, part, and the ring-operation messages: mutants, diagnostics, and costs

Revision: `feature/core-linear` at `b668de7` (the trace commit) for the mutants; the diagnostics and the limits of `sum_layout` at the same headers; the costs at `e2be608`, whose headers differ from `b668de7` in comments only. Toolchain: g++ 16.2.1 and clang++ 23.1.4, on ds-001.

## Mutants

`mutate.py <preset>` applied each mutant to one header, built `core_linear_test`, `core_linear_divide_assert_fail`, `core_mv_test`, and `core_mv_part_checking` with `ninja -k 0`, ran the tests labeled `core.linear` or `core.mv` (the two test programs, their compile-fail tests, and the run-time checks of division and of `part`), and restored the header. The rows of each run are in `out/gcc-debug.tsv` and `out/clang-debug.tsv`, with the first error line of every program that did not build; the runs are in `runs.tsv` under the selection `mutant:<name>`.

The nine mutants of plan 0019 section 6, and two of the implementer's own (the last two rows). No list of further mutants from the test author was found in the branch's records or commit messages, so none was added from that source.

| Mutant | gcc-debug | clang-debug | Killed by |
| --- | --- | --- | --- |
| the sum kernel adds its first term to zero | killed | killed | `core.linear` at run time: the test case "A blade that only one operand holds keeps the sign of a zero" (0.0 + −0.0 is +0.0) |
| a difference takes y with +1 | killed | killed | a `static_assert` of `linear_test.cpp` (line 177, `(b - a) == L{9, ...}`) |
| a two-layout sum drops the blades only y holds | killed | killed | a `static_assert` of `linear_test.cpp` (line 236, the line plus the antiscalar) |
| `s * a` computes x_i * s | killed | killed | `core.linear` at run time: the 2 by 2 matrix scalar, E12 on the left of E21 |
| `/` multiplies by the reciprocal | killed | killed | a `static_assert` of `linear_test.cpp` (line 623: over `std::int64_t` the reciprocal of 2 is 0), and `core.linear_divide_accepts_nonzero` |
| the integer zero assertion is removed | killed | killed | `core.linear_divide_consteval_zero` (no `assertion_failed` in the output) and `core.linear_divide_rejects_zero` (no abort with the message) |
| the scalar catch-all's constraint is widened to every type (`ring_scalar<S>` dropped) | killed | killed | a `static_assert` of `linear_test.cpp` (line 773, `!can_scale_left<std::string, L>`) |
| `part` calls `truncate` | killed | killed | a `static_assert` of `mv_test.cpp` (line 2255, `parts_in_a_constant_evaluation<std::int64_t, 7>`), and the four `core.mv_part_never_checks_*` run-time tests |
| one ring-operations fault reports the next clause's message (no addition reported as no subtraction) | killed | killed | `core.mv_scalar_lacks_addition` and `core.mv_scalar_without_arithmetic` (each requires the addition message and forbids the others) |
| negation subtracts from zero, `T{} - x_i` | killed | killed | `core.linear` at run time: "Negation negates each coefficient and keeps the layout" (−(+0.0) must be −0.0) |
| `sum_layout` keeps the operands' orientations (`merge<A, B>`) | killed | killed | a `static_assert` of `linear_test.cpp` (line 222, `decltype(L{} + AS{})` is the canonical set) |

No mutant was killed by a warning: every compile failure in the records is a `static_assert` of a test file. When a program does not build, CTest may run its previous build, which passes, so a failed build can appear without that program's test in the failed list (the row of `scalar_catch_all_any_type`); every kill above is either a fresh build that failed in a test's own `static_assert`, or a fresh compile-fail or run-time test that failed. No equivalent survivor.

## Diagnostics

Each rejection gives one error, whose first line holds the message, on both compilers. `diagnostics.py` compiled each new compile-fail source of the sums, the scalings, `part`, and the ring operations, and the two existing sources whose message changed (`mv_not_ring_scalar_fail.cpp` and `mv_scalar_without_multiplication_fail.cpp`), with the test flags and `-fsyntax-only`; the rows are in `out/diagnostics.tsv`. Every source gives exactly one line that holds `error:`.

| Sources | First line, GCC | First line, Clang | Longest line, GCC | Longest line, Clang | Lines, GCC | Lines, Clang |
| --- | --- | --- | --- | --- | --- | --- |
| `a + b`, `a - b`, mixed algebras (2) | 154 | 248 | 1,402 to 1,403 | 438 to 439 | 12 | 11 |
| `a + b`, `a - b`, mixed scalars (2) | 156 | 207 | 1,515 to 1,516 | 467 to 468 | 12 | 11 |
| `s * a`, `a * s`, `a / s` with an int for double (3) | 196 | 245 | 390 | 275 to 280 | 9 | 8 |
| `s * a`, 2.5 for an integer multivector | 196 | 246 | 395 | 285 | 9 | 8 |
| `a * 2` with every core header | 196 | 245 | 390 | 284 | 10 | 9 |
| a double line times a float motor with every core header (the products' message) | 157 | 208 | 1,529 | 477 | 15 | 12 |
| `a / s` by zero in a constant evaluation | 154 | 136 | 391 | 213 | 16 | 16 |
| `part` with a bad target (5) | 210 | 210 | 470 to 722 | 273 to 393 | 9 | 8 |
| the ring operations, one requirement lacking (5) | 154 to 207 | 195 to 271 | 479 to 502 | 256 to 280 | 9 | 8 |
| the ring operations, several lacking, and the two existing sources (5) | 154 to 170 | 195 to 214 | 469 to 484 | 245 to 276 | 9 | 8 |

Clang's first line for an int given to a double multivector holds `static assertion failed due to requirement 'std::is_same_v<int, double>'`, the scalar's type first; GCC's next line is the note `'int' is not the same as 'double'`. GCC's longest lines are its account of the template arguments, which prints each layout by value, as for the products.

## The limits of a sum of two layouts

Measured with `sum_limits.cpp`, here, compiled from the worktree root with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -fsyntax-only -DGALOOT_CHECKING=1 -Iinclude -DCASE_N=<n>` (and `-DDISJOINT` for the second case), at the default limits, `-fsyntax-only`, through `tools/timed` under the selections `sum_limits:*`: in R(n,0,0), `full<n>` plus `of_grade<n, 2>`, and `full<n-1>` plus the 2^(n−1) blades that hold e_n.

| | Builds | Fails |
| --- | --- | --- |
| Clang 23 | n = 7, at -O0 with `_GLIBCXX_ASSERTIONS`, at -O2, and at -O2 with `_GLIBCXX_NO_ASSERTIONS` | n = 8 at -O0 with the assertions ("constexpr variable 'u' must be initialized by a constant expression", in `unite`); n = 9 fails already in `mv` |
| GCC 16 | n = 9, at -O0 with the assertions and at -O2 | n = 10 at both ("operation count exceeds limit", in `unite`) |

`mv` itself builds `full<8>` on Clang and `full<10>` on GCC (its comment). Converting the smaller operand first, `a + A{b}` (the same probe with that return statement), builds at n = 8 on Clang (-O0 with the assertions and -O2) and n = 10 on GCC. The Clang failure at n = 8 was not run at -O2; without the assertions the evaluation takes more steps, so it is inferred to fail there too.

## Costs

Measured by `../../performance/linear-cost/measure.py a2468eb <scratch> 3 4`: every compile pinned to CPU 4 (a performance core) through `tools/timed`, three repeats interleaved, the gcc-debug and clang-debug test flags with `-c`. "Before" is `develop` at `a2468eb`, the branch's base, with its own tests; "after" is `e2be608` with its tests. Instructions are those retired on the pinned core, means of three runs in millions, with the spread (maximum minus minimum) in parentheses; wall time is the mean, and peak memory the maximum. The sibling thread of CPU 4 was idle in every row and other processes took at most 0.04 s of it; the instruction counts do not depend on that load. Figures in `../../performance/linear-cost/out/summary.txt` and `rows.tsv`.

### W10: the affected suites

`mv.hpp` changed, so the suites of every component that includes it are affected: `mv`, `product`, `unary`, `identities`, and the smoke test.

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `mv_test.cpp` | 64,989 M (10), 4.62 s, 675 MiB | 65,535 M (38), 4.66 s, 692 MiB | 57,581 M (126), 3.99 s, 439 MiB | 58,198 M (137), 4.03 s, 442 MiB |
| `product_test.cpp` | 48,077 M (60), 2.91 s, 429 MiB | 48,012 M (57), 2.90 s, 429 MiB | 47,939 M (58), 2.84 s, 278 MiB | 47,973 M (27), 2.84 s, 278 MiB |
| `unary_test.cpp` | 15,062 M (31), 1.18 s, 337 MiB | 15,088 M (9), 1.17 s, 337 MiB | 11,608 M (15), 0.96 s, 238 MiB | 11,643 M (18), 0.97 s, 238 MiB |
| `identities_test.cpp` | 294,990 M (207), 17.3 s, 1,380 MiB | 295,611 M (595), 17.4 s, 1,385 MiB | 299,142 M (365), 17.1 s, 697 MiB | 300,157 M (593), 17.1 s, 701 MiB |
| `smoke_test.cpp` | 1,135.8 M (0.1), 0.136 s | 1,142.6 M (0.1), 0.140 s | 1,046.5 M (1.4), 0.149 s | 1,056.7 M (1.0), 0.149 s |
| `linear_test.cpp` (new) | | 34,061 M (26), 2.43 s, 403 MiB | | 29,930 M (130), 2.16 s, 327 MiB |

`mv_test.cpp` grows by 0.8% on GCC and 1.1% on Clang, from the tests of `part`, `has_name`, and the fault scalars; the test author's estimate was 0.8% on GCC. The other suites change by less than 0.4%, with tests unchanged: the headers' parse cost, and on GCC a fall of 0.13% for `product_test.cpp`, within twice the spread. `linear_test.cpp` costs about half of `mv_test.cpp`. In the final gcc-debug run the test programs took 0.00 s (`core.linear`), 0.02 s (`core.mv`, `core.unary`), 0.01 s (`core.identities`), and 0.16 s (`core.product`).

### P2: the includer of `core.hpp`

`core.hpp` now includes `linear.hpp`, and `mv.hpp` grew by `part` and the five fault checks. A unit that includes only `core.hpp`:

| | GCC | Clang |
| --- | --- | --- |
| before | 938.0 M (0.1), 0.141 s, 80 MiB | 949.1 M (2.6), 0.136 s, 113 MiB |
| after | 944.9 M (0.4), 0.124 s, 80 MiB | 957.0 M (3.9), 0.136 s, 113 MiB |
| change | +6.9 M (+0.7%) | +7.9 M (+0.8%) |

The smoke test, which also includes doctest: +6.7 M (+0.6%) on GCC and +10.2 M (+1.0%) on Clang. Each change is beyond the spread of its repeats. The headers cost parsing only for a program that uses none of the new operations (P3). The wall times of these units are too short to compare (0.124 s against 0.141 s on GCC goes the other way from the instructions).
