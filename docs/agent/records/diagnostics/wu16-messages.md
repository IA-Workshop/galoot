# WU16: the messages of rotation and translation as the compilers print them

Plan 0023 reading R6 and criterion 9. The five messages are the `static_assert`s of the catch-alls in `include/galoot/rga3/motions.hpp`; each is pinned by a compile-fail test of `test/CMakeLists.txt` (`motions_rejects`, label `rga3.motions`) with `ONE_ERROR` and with `detail::` and `apply:` forbidden on the first error line. Each source writes its result into a variable (`[[maybe_unused]] const auto r = ...;`), so criterion 9's "also as `auto r = ...;`" is the form every source uses. The catch-all for scalars of two types was added after test group 2 (`d44ec64`, tested by `bf3cbbb`), so there are five messages where R6 proposed four.

## How the lines were collected

`wu16/diagnostics.py` compiles each of the nine `test/rga3/*_fail.cpp` sources with the test flags (`-DGALOOT_CHECKING=1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -std=c++23`) and `-fsyntax-only`, on g++ 16.2.1 and clang++ 23.1.4, against the headers of `e18be9c` (the motions code of `9813525` and `d44ec64`, with `554d3ba`'s include comments). Each compile is timed through `tools/timed` (selection `diagnostics:<compiler>:<source>`). Rows, with the first error line of each, are in `wu16/sizes.tsv`. Every source gives exactly one line holding `error:` on both compilers.

## The first error line of each message

Paths are relative to the repository root. GCC prints the message alone after "static assertion failed:"; Clang adds "due to requirement '...'" with the failed condition, except for the `std::floating_point` check of `rotation`, where it adds nothing.

**1. `rotation` over a scalar with no `std::sin` and `std::cos`** (`rotation_integer_scalar_fail.cpp`, `std::int64_t`; `rotation_rational_scalar_fail.cpp`, the test rational; the same line for both):

- GCC: `include/galoot/rga3/motions.hpp:65:22: error: static assertion failed: rotation: the angle needs std::sin and std::cos, which this scalar type lacks; build the Motor from its coefficients, the line times sin(theta/2) and cos(theta/2) on e1234`
- Clang: `include/galoot/rga3/motions.hpp:65:17: error: static assertion failed: rotation: the angle needs std::sin and std::cos, which this scalar type lacks; build the Motor from its coefficients, the line times sin(theta/2) and cos(theta/2) on e1234`

**2. `rotation` with an angle of another type than the line's scalar** (`rotation_angle_int_fail.cpp`, an `int` angle for a `Line<double>`; `rotation_angle_double_for_float_fail.cpp`, a `double` angle for a `Line<float>`):

- GCC (both): `include/galoot/rga3/motions.hpp:79:22: error: static assertion failed: rotation: the angle is not of the line's scalar type, the second of the two types shown; convert it to that type first, as 0.5 over double or 0.5f over float`. The two types appear on a later note line, "‘int’ is not the same as ‘double’", which the test requires.
- Clang, `int` for `double`: `include/galoot/rga3/motions.hpp:79:17: error: static assertion failed due to requirement 'std::is_same_v<int, double>': rotation: the angle is not of the line's scalar type, the second of the two types shown; convert it to that type first, as 0.5 over double or 0.5f over float`
- Clang, `double` for `float`: the same, with `'std::is_same_v<double, float>'`.

**3. `rotation` about a multivector that is not a `Line`** (`rotation_axis_point_fail.cpp`, a `Point<double>`; `rotation_axis_join_fail.cpp`, `p ^ q` unconverted):

- GCC (both): `include/galoot/rga3/motions.hpp:93:22: error: static assertion failed: rotation: the axis must be an rga3 Line; convert a join such as p ^ q with Line<T>{p ^ q}`
- Clang, the point: `include/galoot/rga3/motions.hpp:93:17: error: static assertion failed due to requirement 'std::is_same_v<galoot::core::mv<galoot::rga3::algebra, galoot::core::blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}, double>, galoot::core::mv<galoot::rga3::algebra, galoot::core::blade_set<6>{{{{9, -1}, {10, -1}, {12, -1}, {6, 1}, {5, -1}, {3, 1}}}}, double>>': rotation: the axis must be an rga3 Line; convert a join such as p ^ q with Line<T>{p ^ q}`
- Clang, the join: the same, with the join's layout `blade_set<6>{{{{3, 1}, {5, 1}, {9, 1}, {6, 1}, {10, 1}, {12, 1}}}}` as the first type.

  Clang spells both types by their blade masks and signs, not by the aliases, so this is the longest first line of the set (444 and 462 characters).

**4. `translation` over a built-in integer type** (`translation_int_fail.cpp`, `int`; `translation_int64_fail.cpp`, `std::int64_t`):

- GCC (both): `include/galoot/rga3/motions.hpp:127:17: error: static assertion failed: translation: halving d would truncate in an integer scalar type; use a floating or exact rational scalar type`
- Clang, `int`: `include/galoot/rga3/motions.hpp:127:17: error: static assertion failed due to requirement '!std::integral<int>': translation: halving d would truncate in an integer scalar type; use a floating or exact rational scalar type`
- Clang, `std::int64_t`: the same, with `'!std::integral<long>'`.

**5. `translation` over scalars of two types** (`translation_mixed_fail.cpp`, `translation(1.0, 0, 2)`):

- GCC: `include/galoot/rga3/motions.hpp:140:22: error: static assertion failed: translation: dx, dy, and dz are not all of one scalar type, the types shown; give all three the same type, as translation(1.0, 0.0, 2.0) over double`, with "‘double’ is not the same as ‘int’" on a later note line, which the test requires.
- Clang: `include/galoot/rga3/motions.hpp:140:17: error: static assertion failed due to requirement 'std::is_same_v<double, int>': translation: dx, dy, and dz are not all of one scalar type, the types shown; give all three the same type, as translation(1.0, 0.0, 2.0) over double`

## Sizes

Columns: the length of the first error line, the longest line, and the number of lines of the whole diagnostic.

| Source | GCC first / longest / lines | Clang first / longest / lines |
| --- | --- | --- |
| `rotation_angle_double_for_float_fail.cpp` | 228 / 1024 / 10 | 279 / 279 / 9 |
| `rotation_angle_int_fail.cpp` | 228 / 1024 / 10 | 277 / 277 / 9 |
| `rotation_axis_join_fail.cpp` | 160 / 1083 / 10 | 462 / 462 / 9 |
| `rotation_axis_point_fail.cpp` | 160 / 1009 / 10 | 444 / 444 / 9 |
| `rotation_integer_scalar_fail.cpp` | 242 / 1035 / 24 | 242 / 242 / 15 |
| `rotation_rational_scalar_fail.cpp` | 242 / 1091 / 24 | 242 / 242 / 15 |
| `translation_int64_fail.cpp` | 181 / 610 / 10 | 223 / 223 / 9 |
| `translation_int_fail.cpp` | 181 / 600 / 10 | 222 / 222 / 9 |
| `translation_mixed_fail.cpp` | 220 / 645 / 10 | 269 / 269 / 9 |

GCC's longest line is its "required from" line, which spells the call's full `mv` types (plan 0023 R3's cost of alias templates). The message itself is on the first error line in every case, so a reader sees it before the long lines.
