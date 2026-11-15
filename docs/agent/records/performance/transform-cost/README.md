# What transform.hpp and antiproject cost: run code, limits, and build cost

WU13a (plan 0021), measured by the WU13a implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on the development machine. Every compile is a row in the log named below, with its revision.

## Run code (plan 0021 criterion 10, PERFORMANCE P4)

`asm/asm.cpp`, one function per operation, compiled with `-std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0 -Iinclude -S` against the headers of `1ca4508`, and again, with `asm.cpp` reformatted by clang-format, against those of `34cd05d`, which gave the same `.s` files byte for byte; outputs `asm/asm.gcc.s` and `asm/asm.clang.s`; rows `asm:transform:gcc` and `asm:transform:clang` in `runs.tsv`. Products count a `mulpd` as two. The hand counts are those of `counts.py`, which counts the nonzero pairs of each stage in rigid(3) by its own rule and is the reference of the kernel test's counts in `transform_test.cpp`.

| Function | GCC 16: products (instructions) | Clang 23: products (instructions) | Hand count (the kernels' terms) |
| --- | --- | --- | --- |
| `transform(point, motor)` | 48 (32 `mulsd`, 8 `mulpd`) | 48 (28 `mulsd`, 10 `mulpd`) | 20 + 28 = 48 |
| `transform(point, flector)` | 48 (48 `mulsd`) | 48 (48 `mulsd`) | 48 |
| `transform(line, motor)` | 72 (72 `mulsd`) | 72: 36 `mulsd` in `transform`, and 36 (26 `mulsd`, 5 `mulpd`) in the first stage's kernel, out of line | 36 + 36 = 72 |
| `transform(plane, motor)` | 48 (48 `mulsd`) | 48 (48 `mulsd`) | 28 + 20 = 48 |
| `compose` of two motors | 48 (48 `mulsd`) | 49 (35 `mulsd`, 7 `mulpd`), in the kernel, out of line | 48 terms |
| `antiproject(plane, point)` | 9 | 9 | |

Calls: GCC inlines everything; no function calls anything, and there is no stack protector call. Clang keeps each `transform` instantiation out of line, reached from the probe function by a tail jump; inside `transform(line, motor)` it calls the first stage's kernel (`run_product`) and `__stack_chk_fail`; `compose` is a tail jump to its kernel. So on Clang criterion 10's "no call but the stack protector" holds for the point and plane cases and not for the line (one call) or `compose` (one tail jump to the kernel). Clang's count for `compose`, one product more than the plan's 48 terms, is not explained: no coefficient of the plan is other than ±1 (inferred, not checked in the assembly line by line). The planner's probe through public functions saw 62 products for `transform(point, motor)` on Clang; the restricted kernels give 48.

## Limits (plan 0021 reading R12, criterion 9)

`limits/gen.py` writes one unit per operation and form: x = `full<n>` and a versor V on every blade of even antigrade (the largest layout of one parity) for `transform(x, V)` and `compose(V, V)`, and `antiproject(x, x)`; `limits/run_one.sh` compiles one unit with the test warnings as errors at `-O0` (libstdc++'s assertions on, as it defines them there) and `-O2` (off, as in `clang-measure`), at the compilers' default limits. Rows: `limits/results.tsv`; runs: `limits/runs.tsv`, at `1ca4508`.

| Operation | R(n,0,0), largest n that builds: GCC | Clang | conformal(3), n = 5 | dense five-generator form |
| --- | --- | --- | --- | --- |
| transform | 6 (7 fails) | 5 (6 fails) | builds on both | GCC only |
| compose | 7, the largest tried | 5 (6 fails) | builds on both | GCC only |
| antiproject | 6 (7 fails) | 5 (6 fails) | builds on both | builds on both |

The two optimization levels agree in every case. Failures: on Clang, "constexpr variable 'count' must be initialized by a constant expression" in a plan's generation; on GCC at n = 7, the operation count over the limit at `-O2` and a libstdc++ assertion that is not a constant expression at `-O0`. The dense form is 2 on the diagonal and 1 elsewhere. `transform_test.cpp`, `transform_identities_test.cpp`, and `dual_test.cpp` build and pass under `clang-measure` and `gcc-measure` (run log, selection `core.transform+dual`).

## Build cost before and after (W10, P1)

`measure.py 51c18de out 3 4`, at `f2d8322` (before: `develop` at `51c18de`, which the branch merged): every compile pinned to CPU 4, a performance core, with the gcc-debug and clang-debug test flags and `-c`, three repeats interleaved, through `tools/timed measure` (rows `transform-cost:*` in `runs.tsv`), under the shared timing lock, with no other build running. Instructions retired on that core, mean of three; the spreads are in `out/summary.txt`, every compile in `out/rows.tsv`.

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `smoke_test.cpp` | 1,153.3 M | 1,160.3 M (+0.6%) | 1,065.2 M | 1,073.7 M (+0.8%) |
| a unit that includes only `core.hpp` | 955.5 M | 963.2 M (+0.8%) | 965.1 M | 973.0 M (+0.8%) |
| `dual_test.cpp` (gains the antiproject tests) | 38,539.4 M, 3.1 s | 46,996.1 M (+21.9%), 3.7 s, 491 MiB | 35,999.8 M, 2.8 s | 43,261.6 M (+20.2%), 3.2 s, 395 MiB |
| `transform_test.cpp` | | 125,113.0 M, 8.0 s, 911 MiB | | 111,725.5 M, 7.0 s, 506 MiB |
| `transform_identities_test.cpp` | | 402,250.9 M, 25.7 s, 2,439 MiB | | 365,622.3 M, 22.7 s, 1,207 MiB |

`smoke_test.cpp` and the core-only unit are the suites that include `core.hpp` (the other includers are compile-fail sources and `bare_user.cpp`, which only compile). `transform.hpp` adds about 8 M instructions (0.8%) to every includer of `core.hpp`, what `dual.hpp` added. `transform_identities_test.cpp` is now the largest unit of the suite, larger than `identities_test.cpp` was before its split (295 G and 299 G); it is not labeled `slow`, since P9 asks the label only of a test slow to run, and it runs in under 0.1 s; its build is the longest single compile of a debug build. Growth with n at fixed use (P2) was not measured for these operations; `transform` and `compose` are geometric antiproducts, whose growth decision 0141 records.
