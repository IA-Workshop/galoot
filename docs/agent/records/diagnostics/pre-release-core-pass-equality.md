# Plan 0040: what `a == b` and `a != b` on two `basis_change` values print, before and after the deleted `==`

Plan 0040 part B (its criterion 6). Recorded by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-10 in the `--code` worktree: before, at `01045de` (the tests of part B2, whose `include/` is that of the branch point `8058589`); after, with the header of `09bebcd` (part B3), compiled from the uncommitted tree identical to it in `include/` and `test/core/`.

- Sources: `test/core/basis_change_equality_fail.cpp` and `basis_change_inequality_fail.cpp`, which compare two `from_rows` values for one matrix (the identity, and its rows doubled over denominators of 2).
- Command: the gcc-debug and clang-debug flags (`-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -g -DGALOOT_CHECKING=1 -Iinclude -c <source>`), with `g++` 16.2.1 20260810 and `clang++` 23.1.4, libstdc++ 16, run through `tools/timed` (rows `pre-release-core-pass:diagnostic:<compiler>:<test>:<side>` of `records/performance/runs.tsv`). The worktree's absolute path is cut from each line.
- The tests: `core.basis_change_equality` and `core.basis_change_inequality` require one error, the compiler's text for a deleted `==` (GCC "use of deleted function", Clang "overload resolution selected deleted operator '=='"), the literal `// use same_rows(a, b)`, and no `detail::` on the first error line. Before, both fail on both compilers; after, both pass.

## Summary

| Compiler | Comparison | Before: lines, first error | After: lines, first error | After: names `same_rows` |
| --- | --- | --- | --- | --- |
| GCC 16.2.1 | `a == b` | 7, no match for ‘operator==’ (operand types are ‘const galoot::core::basis_change<2>’ and ‘const galoot::core::basis_change<2>’) | 11, use of deleted function ‘bool galoot::core::basis_change<N>::operator==(const galoot::core::basis_change<N>&) const [with long unsigned int N = 2]’ | yes |
| GCC 16.2.1 | `a != b` | 7, no match for ‘operator!=’ (operand types are ‘const galoot::core::basis_change<2>’ and ‘const galoot::core::basis_change<2>’) | 11, use of deleted function ‘bool galoot::core::basis_change<N>::operator==(const galoot::core::basis_change<N>&) const [with long unsigned int N = 2]’ | yes |
| Clang 23.1.4 | `a == b` | 4, invalid operands to binary expression ('const basis_change<2UL>' and 'const basis_change<2UL>') | 8, overload resolution selected deleted operator '==' | yes |
| Clang 23.1.4 | `a != b` | 4, invalid operands to binary expression ('const basis_change<2UL>' and 'const basis_change<2UL>') | 8, overload resolution selected deleted operator '==' | yes |

Both compilers show the deleted declaration's line, with its end-of-line comment, in the note that follows the error: GCC as "declared here", Clang as "candidate function has been explicitly deleted" (twice, once for the reversed candidate that C++20 adds). For `!=` both compilers name `==`, since `a != b` is rewritten as `!(a == b)`; GCC's error line names `operator==` with the type's template parameter, and neither first line names `detail::`.

## GCC 16.2.1, `a == b`, before

```
test/core/basis_change_equality_fail.cpp: In function ‘bool {anonymous}::compare()’:
test/core/basis_change_equality_fail.cpp:14:12: error: no match for ‘operator==’ (operand types are ‘const galoot::core::basis_change<2>’ and ‘const galoot::core::basis_change<2>’)
   14 |   return a == b;
      |          ~ ^~ ~
      |          |    |
      |          |    basis_change<[...]>
      |          basis_change<[...]>
```

## GCC 16.2.1, `a == b`, after

```
test/core/basis_change_equality_fail.cpp: In function ‘bool {anonymous}::compare()’:
test/core/basis_change_equality_fail.cpp:14:15: error: use of deleted function ‘bool galoot::core::basis_change<N>::operator==(const galoot::core::basis_change<N>&) const [with long unsigned int N = 2]’
   14 |   return a == b;
      |               ^
In file included from test/core/basis_change_equality_fail.cpp:5:
include/galoot/core/basis_change.hpp:186:8: note: declared here
  186 |   bool operator==(const basis_change&) const = delete; // use same_rows(a, b)
      |        ^~~~~~~~
test/core/basis_change_equality_fail.cpp:14:15: note: use ‘-fdiagnostics-all-candidates’ to display considered candidates
   14 |   return a == b;
      |               ^
```

## GCC 16.2.1, `a != b`, before

```
test/core/basis_change_inequality_fail.cpp: In function ‘bool {anonymous}::compare()’:
test/core/basis_change_inequality_fail.cpp:14:12: error: no match for ‘operator!=’ (operand types are ‘const galoot::core::basis_change<2>’ and ‘const galoot::core::basis_change<2>’)
   14 |   return a != b;
      |          ~ ^~ ~
      |          |    |
      |          |    basis_change<[...]>
      |          basis_change<[...]>
```

## GCC 16.2.1, `a != b`, after

```
test/core/basis_change_inequality_fail.cpp: In function ‘bool {anonymous}::compare()’:
test/core/basis_change_inequality_fail.cpp:14:15: error: use of deleted function ‘bool galoot::core::basis_change<N>::operator==(const galoot::core::basis_change<N>&) const [with long unsigned int N = 2]’
   14 |   return a != b;
      |               ^
In file included from test/core/basis_change_inequality_fail.cpp:5:
include/galoot/core/basis_change.hpp:186:8: note: declared here
  186 |   bool operator==(const basis_change&) const = delete; // use same_rows(a, b)
      |        ^~~~~~~~
test/core/basis_change_inequality_fail.cpp:14:15: note: use ‘-fdiagnostics-all-candidates’ to display considered candidates
   14 |   return a != b;
      |               ^
```

## Clang 23.1.4, `a == b`, before

```
test/core/basis_change_equality_fail.cpp:14:12: error: invalid operands to binary expression ('const basis_change<2UL>' and 'const basis_change<2UL>')
   14 |   return a == b;
      |          ~ ^  ~
1 error generated.
```

## Clang 23.1.4, `a == b`, after

```
test/core/basis_change_equality_fail.cpp:14:12: error: overload resolution selected deleted operator '=='
   14 |   return a == b;
      |          ~ ^  ~
include/galoot/core/basis_change.hpp:186:8: note: candidate function has been explicitly deleted
  186 |   bool operator==(const basis_change&) const = delete; // use same_rows(a, b)
      |        ^
include/galoot/core/basis_change.hpp:186:8: note: candidate function (with reversed parameter order) has been explicitly deleted
1 error generated.
```

## Clang 23.1.4, `a != b`, before

```
test/core/basis_change_inequality_fail.cpp:14:12: error: invalid operands to binary expression ('const basis_change<2UL>' and 'const basis_change<2UL>')
   14 |   return a != b;
      |          ~ ^  ~
1 error generated.
```

## Clang 23.1.4, `a != b`, after

```
test/core/basis_change_inequality_fail.cpp:14:12: error: overload resolution selected deleted operator '=='
   14 |   return a != b;
      |          ~ ^  ~
include/galoot/core/basis_change.hpp:186:8: note: candidate function has been explicitly deleted
  186 |   bool operator==(const basis_change&) const = delete; // use same_rows(a, b)
      |        ^
include/galoot/core/basis_change.hpp:186:8: note: candidate function (with reversed parameter order) has been explicitly deleted
1 error generated.
```
