# What WU15 costs to compile: format.hpp and the catch-alls

W10 for plan 0022, measured by the WU15 implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, on the development machine. No bound applies (C14); these are records (P1).

## Method

`measure.py bd60589 out 3 4` ran at `889d2ba` with a clean `include/` and `test/`. It held the shared timing lock (`lock.sh take`, free when taken), and the job pool was idle (23 of 23 slots free before and after).
- `bd60589` is the `develop` revision that `feature/core-format` branched from.
- Every compile is pinned to CPU 4 through `tools/timed measure` (rows `format-cost:*` in `runs.tsv`), with three repeats interleaved. Across the 60 compiles the sibling thread was busy for 0.02 s in total and other processes on CPU 4 for 0.41 s.
- The figures are the instructions retired on that core, as the mean of three; every row is in `out/rows.tsv` and the ranges are in `out/summary.txt`.
- The includers are compiled with the measure presets' flags (`-O2`, checking off). The test units are compiled with the debug test flags.
- The "after" test units are the files at `889d2ba`, which hold this unit's new test cases, so the differences in mv and linear mix header cost with test cost.

## Results

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `core.hpp` only | 914.7 M | 915.1 M (+0.04%) | 973.1 M | 974.7 M (+0.16%) |
| `core.hpp` and `format.hpp`, formatting one point | — | 14,216.6 M, 1.51 s, 239 MiB | — | 7,185.5 M, 0.88 s, 210 MiB |
| `smoke_test.cpp` | 1,165.1 M | 1,165.8 M (+0.06%) | 1,078.0 M | 1,078.8 M (+0.07%) |
| `mv_test.cpp` | 65,894.0 M | 65,943.0 M (+0.07%) | 58,663.6 M | 58,698.8 M (+0.06%) |
| `linear_test.cpp` | 42,194.7 M | 42,574.9 M (+0.9%) | 37,819.5 M | 38,044.1 M (+0.6%) |
| `format_test.cpp` | — | 34,301.4 M, 3.28 s, 526 MiB | — | 22,880.1 M, 2.35 s, 425 MiB |

What the plan expected beforehand held.
- An includer of `core.hpp` alone changes only by part 3's catch-alls. On GCC the change is within the repeats' range (0.7 M). On Clang it is +1.6 M against a range of 2.0 M.
- A `format.hpp` includer pays for `<format>`, and more than the plan's estimate. The plan's 3.17 G (GCC) was for including `<format>` alone; formatting one value instantiates `std::format`'s machinery for that type, about 13.3 G more than the core alone on GCC and 6.2 G on Clang. This is the reason the header stays outside `core.hpp`.
- `linear_test.cpp` grows by 380 M on GCC. That is inferred to come mostly from its new test cases (the accept test and the probes); no time report separates the header's share.
- Peak memory is unchanged within 2 MiB for every unit measured before and after, except `linear_test.cpp` on GCC: 434 to 448 MiB.
