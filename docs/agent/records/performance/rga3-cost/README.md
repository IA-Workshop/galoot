# What the rga3 model costs to compile

WU16 (plan 0023), measured by the WU16 implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on the development machine. No bound applies (C14); these are records (P1).

## Method

`measure.py 88c9484 out 3 4` at `c799821`, with a clean `include/` and `test/`, under the shared timing lock (`lock.sh take`), with the job pool idle (23 of 23 slots free) and no other build running. `88c9484` is the `develop` revision `feature/rga3` branched from. Every compile is pinned to CPU 4, a performance core, through `tools/timed measure` (rows `rga3-cost:*` in `runs.tsv` at `c79982113b35`), three repeats interleaved; the figures are the instructions retired on that core, mean of three, with every row in `out/rows.tsv` and the ranges in `out/summary.txt`.

- The includers: `out/units/core_only.cpp` (`#include <galoot/core.hpp>`) before and after, and `out/units/rga3_only.cpp` (`#include <galoot/rga3.hpp>`) after, with the flags of the `gcc-measure` and `clang-measure` presets (`-DGALOOT_CHECKING=0`, the warning flags, `-O2 -std=c++23`) and `-c`. The rows name those presets.
- The five rga3 test units, after, with the flags of the gcc-debug and clang-debug test builds and `-c`. The rows name those presets.

The per-unit rows of the fresh gcc-debug and clang-debug builds at `c799821` (301 and 303 rows, every unit of the suite) are in `compile.tsv`, copied by decision 0080's step. They were built in parallel (mode `shared`), so their wall times are not comparable with the pinned ones below; their peak memory is.

## The includers

| Unit | GCC | Clang |
| --- | --- | --- |
| `core.hpp` only, before | 913.6 M | 974.0 M |
| `core.hpp` only, after | 913.6 M (+0.0%) | 972.6 M (−0.1%) |
| `rga3.hpp` only, after | 1,396.0 M, 0.16 s, 95 MiB | 1,357.0 M, 0.18 s, 122 MiB |

`git diff --stat develop...feature/rga3 -- include/galoot/core include/galoot/core.hpp` is empty, so the core's includer compiles the same text before and after; Clang's −1.4 M is beyond the ranges of its repeats (1.0 M and 0.5 M) but a difference of the extracted tree's path, not of the code (inferred: the "before" sources are compiled from `out/before`, a longer path). Including the model adds about 482 M instructions on GCC and 384 M on Clang over the core alone (+53% and +40%); no time report attributes it, so how much of it is `<cmath>`, which `motions.hpp` includes, is not known.

## The rga3 test units

Pinned, gcc-debug and clang-debug flags, mean of three:

| Unit | GCC | Clang |
| --- | --- | --- |
| `objects_test.cpp` | 12,016.9 M, 0.88 s, 270 MiB | 11,100.8 M, 0.83 s, 197 MiB |
| `motions_test.cpp` | 12,720.3 M, 1.14 s, 338 MiB | 9,984.1 M, 0.94 s, 274 MiB |
| `point_groups_test.cpp` | 11,216.2 M, 1.02 s, 319 MiB | 10,134.5 M, 0.89 s, 259 MiB |
| `chains_test.cpp` | 4,136.0 M, 0.36 s, 157 MiB | 4,041.0 M, 0.34 s, 165 MiB |
| `incidence_test.cpp` | 4,118.9 M, 0.37 s, 157 MiB | 3,641.5 M, 0.34 s, 167 MiB |

From `compile.tsv` (shared mode): the README examples took 1.1 s and 263 MiB (`example_1.cpp`) and 0.4 s and 111 MiB (`example_2.cpp`) on GCC, 1.4 s and 230 MiB and 0.2 s and 136 MiB on Clang; each of the nine compile-fail sources compiled in under 0.31 s and 106 MiB on GCC and 0.29 s and 126 MiB on Clang. The point-group closure runs at run time over the test rational (plan 0023 R10), so its unit costs about what the objects' unit does. The full gcc-debug suite of 434 tests ran in 6.7 s.

2026-11-08: `measure.py` now includes `<galoot/presets/rga3.hpp>` and compiles `test/presets/rga3/<unit>_test.cpp`, the paths after plan 0025's move (`26c8a82`), and runs again (plan 0026); the run above was made at the old paths, and `out/`, `out/units/rga3_only.cpp` among it, is kept as it is.
