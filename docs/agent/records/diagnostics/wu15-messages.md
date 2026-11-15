# WU15: the messages of format.hpp and of the new catch-alls, and their sizes

Plan 0022 R6, R7, and criterion 4. Decisions 0204 and 0206 give the texts, and decision 0212 rewords four of them. Each is pinned by compile-fail tests in `test/CMakeLists.txt` (`core.format_*`, `core.mv_other_*`, `core.linear_*_number_*`, `core.blade_set_blades_*`, `core.blade_set_names_zero`). Each is registered with `ONE_ERROR`, and the internal names are forbidden on the first error line: `detail::`, `__format`, and `basic_format_arg` for the formatter; `detail::`, `component<`, and `blade_set<` for the catch-alls; `detail::` and `make_blades` for `blades`.

## Method

`wu15/diagnostics.py` compiles each source with the test flags and `-fsyntax-only` on g++ 16.2.1 and clang++ 23.1.4. Every compile goes through `tools/timed` (rows `diagnostics:before:*` and `diagnostics:after:*`). For each compile it records the line count, the lines holding `error:`, the longest line, and the first error line.
- The sources are the ten R7 probes in `wu15/gaps/` and every compile-fail source of the formatter and of part 3.
- `wu15/before.tsv` was measured at `62b9286`, after part 2 and before part 3's code.
- `wu15/after.tsv` was first measured with part 3's code, and measured again on 2026-11-08 at `7c07cc4`, after the hci reading's rewordings and the blind review's two tests, with the two later `blades` sources (`blade_set_blades_tag_without_value_fail.cpp`, `blade_set_blades_value_not_metric_fail.cpp`) added to the script's list. Every count of the first measurement was unchanged; the tables below are from the second.

## The R7 probes, before and after (lines / error lines)

| Case | Misuse | GCC before | GCC after | Clang before | Clang after | First error after |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `Motor{float_line}` | 114 / 1 | 11 / 1 | 43 / 1 | 10 / 1 | mv: the source has another scalar type; ... |
| 3 | `Motor{line_of_another_algebra}` | 114 / 1 | 11 / 1 | 43 / 1 | 10 / 1 | mv: the source is in another algebra; ... |
| 6 | `p + 1.0` | 47 / 1 | 10 / 1 | 19 / 1 | 9 / 1 | a + b: a multivector and a number have no sum; ... |
| 2 | `truncate<5>(motor)` | 52 / 5 | 52 / 5 | 13 / 1 | 13 / 1 | unchanged (not admitted) |
| 4, 8 | `line == motor`, `line == float_line` | 30 / 1 | 30 / 1 | 8 / 1 | 8 / 1 | unchanged (not admitted) |
| 5, 7 | `reverse(3.0)`, `transform(p, 2.0)` | 17, 41 / 1 | 17, 41 / 1 | 7, 19 / 1 | 7, 19 / 1 | unchanged (not admitted) |
| 9 | `mv<notag, layout, double>` | 23 / 3 | 23 / 3 | 10 / 1 | 10 / 1 | unchanged: `mv` keeps its constraint (decision 0206) |
| 10 | control, `reverse(fl) + l` | 13 / 1 | 13 / 1 | 12 / 1 | 12 / 1 | the catalogued literal |

These agree with plan 0022's planning probe on every case except case 9 on GCC. The plan had 20 / 2 there, and this probe gives 23 / 3; it writes the misuse differently, as a function returning the type.

## The new tests' sources (after)

| Sources | GCC lines / errors | Clang lines / errors |
| --- | --- | --- |
| `format_*_fail.cpp` (15) | 30 / 1 (33 / 1 for `println`) | 26 / 1 (29 / 1) |
| `mv_other_*_fail.cpp` (15) | 9 / 1 | 8 / 1 |
| `linear_*_number_*_fail.cpp` (12) | 9 / 1 | 8 / 1 |
| `blade_set_blades_*_fail.cpp` (3), `blade_set_names_zero_fail.cpp` | 12 / 1; `blades_not_metric_like` and `names_zero` were 17 / 2 and 17 / 1 | 11 / 1; those two were 10 / 1 |

Every source gives exactly one line holding `error:` on both compilers, with the library's message on it. The formatter's 26 to 33 lines are instantiation notes from inside `<format>` that follow the message. They name `std::format` and the formatter's `format` member, but neither `detail::` nor `__format` is on the first error line.
