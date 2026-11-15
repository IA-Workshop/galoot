# What dual.hpp costs: run code, limits, build cost, and growth with n

WU11a (plan 0020), measured by the WU11a implementer (Claude, agent, Opus 5.5). Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, on the development machine (`ds-001`). Every compile is a row in the log named below, with its revision.

## Run code (plan 0020 criterion 9, PERFORMANCE P4)

`asm/asm.cpp`, compiled with `-std=c++23 -O2 -ffp-contract=off -Iinclude -S` against the headers of `6837785` (the header is the same at the final revision except comments). The outputs are `asm/asm.gcc.s` and `asm/asm.clang.s`. Counts of scalar double instructions per function:

| Function | GCC 16 | Clang 23 | Hand count |
| --- | --- | --- | --- |
| `dot` of two rigid(3) motors | 4 `mulsd`, 3 `addsd` | 2 `mulpd` (4 products), 3 `addsd` | 4 multiplications, 3 additions |
| `antidot` of two rigid(3) motors | 4 `mulsd`, 3 `addsd` | 2 `mulpd` (4 products), 3 `addsd` | 4 and 3 |
| `bulk` of a line | moves, 1 `xorpd` (the sign of e13) | moves, the sign of e13 flipped by an integer `xorq` | no arithmetic |
| `weight_dual` of a point | moves, 1 `xorpd` | moves, 1 `xorps` | no arithmetic |
| `project` of a point onto a plane | 11 `mulsd` and 5 `mulpd` (21 products), 2 `subsd` and 3 `subpd` (8 subtractions), 3 `addsd`, 3 `xorpd` | 21 `mulsd`, 8 `subsd`, 3 `addsd`, 1 `xorpd` | 21 multiplications |

The counts were taken from the committed `.s` files, counting each packed instruction once by name and giving the number of products it computes. (The first version of this table named every multiplication `mulsd`; the blind review recounted them.) No function calls anything on either compiler; every kernel is inlined.

## Limits (plan 0020 reading R10, criterion 8)

`limits/gen.py` writes one unit per operation and form, each applying the operation to `full<n>` (both operands for the binary ones); `limits/run_one.sh` compiles one unit and prints its row. Rows: `limits/results.tsv`; runs: `limits/runs.tsv`. Run against a copy of the headers of `6837785`, extracted with `git archive` into a scratch directory (so the rows' revision reads `6837785e2bdc-dirty`, the worktree's state while a mutant run was editing its own copy of `dual.hpp`), with the test warnings as errors, at `-O0` (libstdc++'s assertions on, as it defines them there) and `-O2` (off, as in `clang-measure`), at the compilers' default limits. The two levels gave the same answer in every case.

| Operation | R(n,0,0), largest full<n> that builds: GCC | Clang | conformal(3), full<5> | dense five-generator form, full<5> |
| --- | --- | --- | --- | --- |
| bulk, weight, bulk_dual, weight_dual, left_bulk_dual, left_weight_dual | 9 (the largest tried) | 7 | builds on both | builds on both |
| dot, antidot | 6 | 5 | builds on both | GCC only |
| project | 6 | 5 | builds on both | builds on both |

The dense form is 2 on the diagonal and 1 elsewhere. Each failure is a part's own limit: on Clang at full<8> the reverse map's (at `-O2`, without libstdc++'s assertions, the first error is in the plan of the product with the antiscalar, which is over the limit there too), at full<6> the geometric product's plan; on GCC at full<7> the product's plan for the dot and the antidot, and the wedge's plan for the projection (`plan.hpp` gives its count, 39,732,810 operations). At `-O0`, GCC reports that last failure as a libstdc++ assertion that "is not a constant expression" inside the wedge plan's typing, not as its operation count; with `-fconstexpr-ops-limit=400000000` the same unit builds (row `limit:project_r7:g++:O0:ops-limit-raised`), so it is the count.

The antidot of two `full<5>` of conformal(3), which fails through the geometric antiproduct on Clang at `-O2` (audit B1), builds through the complements in both configurations. `dual_test.cpp` and `dual_identities_test.cpp` build and pass under `clang-measure` and `gcc-measure` (run log, selection `core.dual`).

## Build cost before and after (W10, P2)

`measure.py c86cb58 out 3 4`, at `69d6dcc`: every compile pinned to CPU 4, a performance core, with the gcc-debug and clang-debug test flags and `-c`, three repeats interleaved, through `tools/timed measure` (rows `duals-cost:*` in `runs.tsv`). Instructions retired on that core, mean of three; the spreads are in `out/summary.txt`.

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `smoke_test.cpp` | 1,142.7 M | 1,150.5 M (+0.7%) | 1,057.3 M | 1,064.7 M (+0.7%) |
| a unit that includes only `core.hpp` | 945.3 M | 952.9 M (+0.8%) | 956.4 M | 963.3 M (+0.7%) |
| `dual_test.cpp` | | 38,067.8 M, 3.1 s, 458 MiB | | 35,713.4 M, 2.7 s, 372 MiB |
| `dual_identities_test.cpp` | | 137,299.4 M, 9.3 s, 761 MiB | | 153,318.9 M, 9.3 s, 486 MiB |

`smoke_test.cpp` and the core-only unit are the suites that include `core.hpp`; the only other includers are two compile-fail sources and `bare_user.cpp`, which only compile. `dual_identities_test.cpp` is about half of `identities_test.cpp` (295 G and 299 G in the WU10a record), so it is not labeled `slow`.

## Growth with n at fixed use (plan 0020 reading R10, decision 0141)

The four typed rigid(3) objects (point, line, plane, motor) embedded in R(n−1,0,1), e4 becoming e_n, at n = 4 and 16, used at run time; "use" is the unit minus a baseline unit with the same algebra, layouts, and values and no operation. Same campaign as above.

| Operation | GCC use, n = 4 | n = 16 | growth | Clang use, n = 4 | n = 16 | growth |
| --- | --- | --- | --- | --- | --- | --- |
| bulk (four objects) | 711.4 M | 721.9 M | +1.5% | 676.1 M | 732.7 M | +8.4% |
| weight | 986.1 M | 1,108.8 M | +12.4% | 862.4 M | 1,483.6 M | +72.0% |
| weight_dual | 892.0 M | 1,015.4 M | +13.8% | 803.4 M | 1,421.6 M | +76.9% |
| antidot (each object with itself) | 955.9 M | 1,700.1 M | +77.9% | 1,071.4 M | 5,207.1 M | +386.0% |
| project (point onto plane, point onto line, line onto plane, point onto point) | 1,326.2 M | 1,459.8 M | +10.1% | 1,107.2 M | 1,612.2 M | +45.6% |

The bulk goes through the reverse and the product with 𝟙, whose plan has one term per blade at every n, so it barely grows. The others go through the complements, whose blades have grades that grow with n, as the antiproduct's do (decision 0141: +106.8% on GCC and +356.5% on Clang through `%` for four pairs, in the WU10a record). The antidot grows the most: it builds the dot's plan of the two complements, a geometric product of blades of grade n − k, whose cost grows with those grades. This is generation cost that follows from the composition through the complements, measured and reported under P2; it blocks nothing (decision 0005). A cheaper antidot would compute the plan of the antiexomorphism directly, which C4 admits only with a measured need (decision 0172, option 1).
