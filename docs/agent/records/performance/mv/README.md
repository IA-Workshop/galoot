# Cost and limits of mv.hpp (plan 0016, WU8a)

Measured by Claude (agent, Opus 5.5), cpp-expert role, with `measure_mv.py` here, at `0acef07` (the header of `669f7e2` differs only in a comment). GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16; every compile pinned to CPU 10 (a performance core) through `tools/timed measure`, whose rows in `runs.tsv` carry the selections `mv-limits:*`, `mv-uses:*`, and `mv-headers:*`. The sibling thread was busy at most 0.01 s in any row. The machine was shared with other workstreams (load average 4 to 5 at the start); instruction counts, not times, are compared. No bound applies (C14).

Commands, from the worktree root with the job pool exported:

```bash
python3 -I docs/agent/records/performance/mv/measure_mv.py limits docs/agent/records/performance/mv/out-0acef07 10
python3 -I docs/agent/records/performance/mv/measure_mv.py headers 965059e docs/agent/records/performance/mv/out-0acef07 3 10
python3 -I docs/agent/records/performance/mv/measure_mv.py uses docs/agent/records/performance/mv/out-0acef07 3 10
```

The generated units and the `before` tree are rewritten by each run and are not kept.

## Limits (`out-0acef07/limits.tsv`)

`mv<R(n,0,0), full<n>, double>`, instantiated and default-constructed, one trial build per n at the default limits:

| Compiler | Builds for n up to | First failure | What stops it |
| --- | --- | --- | --- |
| GCC 16.2.1, all three configurations | 10 (1,024 blades; 17.7 G instructions, 0.9 s, 199 MB at -O0) | n = 11 | `-fconstexpr-ops-limit` (33,554,432) in `masks_distinct`, the pair comparison |
| Clang 23.1.4, all three configurations | 8 (256 blades; 2.5 G instructions, 0.26 s) | n = 9 | the step limit, in `distinct_masks` |

`full<n>` alone builds to n = 15 on GCC and 13 on Clang (blade_set.hpp's comment), so the direct repeated-mask check lowers the largest layout an `mv` takes to 1,024 and 256 blades. That is above every present use: the largest result of a plan that builds is 64 blades (`full<6>` times itself on GCC). So the canonical-order fast path of plan 0016 reading 16 is not built. Each limit is a single trial at the default limits, not a searched threshold, so lesson 0064's checked search does not apply; a pass on GCC could in principle be a lucky one.

## Header cost to every includer (`out-0acef07/headers.tsv`)

An empty unit including one header, -O0 with the test flags, three repeats each, interleaved; mean retired instructions (spread across repeats in parentheses):

| Header | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `galoot/core.hpp` | 0.8530 G (0.3 M) | 0.8568 G (0.0 M), +0.45 % | 0.9114 G (2.3 M) | 0.9162 G (1.2 M), +0.53 % |
| `galoot/core/blade_set.hpp` | 0.6439 G (0.3 M) | 0.6484 G (0.0 M), +0.70 % | 0.5356 G (0.3 M) | 0.5393 G (0.4 M), +0.69 % |
| `galoot/core/mv.hpp` | absent | 0.8560 G | absent | 0.9158 G |

"Before" is `965059e`, the claim revision. Each rise exceeds the spread. Causes: `blade_set.hpp` now includes `metric.hpp` (plan 0016 R5, decision 0157), and `core.hpp` now includes `mv.hpp`, whose text is parsed but which instantiates nothing at namespace scope. A Clang `-ftime-trace` of `core.hpp` before and after was taken to attribute the 4 to 5 M instructions, but its wall-clock totals varied more between the two runs (240 against 186 ms) than the change, so the attribution is by construction only, not measured. `mv.hpp` adds no standard header: `<concepts>`, `<string_view>`, and `<cstdint>` are already read by `core.hpp` (the preprocessed `core.hpp` has the same line count, 28,571 on GCC and 28,313 on Clang, with and without `<concepts>`).

## Cost per use (`out-0acef07/uses.tsv`, `uses-summary.txt`)

Slopes from 64 to 256 uses, mean of three repeats, in retired instructions per use. Every unit compiled (exit 0).

| Use | GCC -O0 | GCC -O2 | Clang -O0 | Clang -O2 |
| --- | --- | --- | --- | --- |
| A new mv type over a four-blade layout (instantiating the storage base and the layout checks) | 2.11 M | 2.11 M | 2.00 M | 2.00 M |
| Its first construction from four values (instantiating the inherited four-parameter constructor), above the type | 8.20 M | 6.34 M | 2.23 M | 3.05 M |
| A construction of an existing type, in a function of its own | 3.19 M | 4.32 M | 1.10 M | 1.61 M |
| `get<"name">` above a read of `coefficients[i]` | 0.37 M | 0.31 M | 0.38 M | 0.37 M |
| For comparison: a function of six reads by position | 5.80 M | 7.68 M | 1.22 M | 3.43 M |

The construction rows include the cost of the function that holds each construction; the first construction of a type also generates its constructor. `get` costs about 0.3 to 0.4 M instructions per use more than a read by position, close to research note 0012's figure for its string form against its index form (1.2 M against 0.81 M on GCC -O0, a difference of 0.39 M). A program that declares many types pays about 2 M instructions per type for the type and, on GCC, 6 to 8 M more for its first construction (plan 0016 risk 6).

## W10

Compile rows of fresh gcc-debug and clang-debug trees, each built once with the header sets, copied into `compile.tsv` by decision 0080's step: before at `965059e` (committed in `0782a8e`), after at `c3d4e45` (135 and 137 rows). These builds ran in shared mode on the shared machine, not pinned, so instructions are the sum over both kinds of core and the wall times are not comparable between the two sets (the machine's load differed); the instruction counts and peak memory are. Selected units:

| Unit | Preset | Before (965059e): wall s, G instructions, MiB | After (c3d4e45): wall s, G instructions, MiB |
| --- | --- | --- | --- |
| `galoot/core.hpp.cxx` | gcc-debug | 0.11, 0.91, 77 | 0.13, 0.92, 77 |
| `galoot/core.hpp.cxx` | clang-debug | 0.17, 0.92, 112 | 0.20, 0.93, 112 |
| `galoot/core/blade_set.hpp.cxx` | gcc-debug | 0.08, 0.68, 66 | 0.15, 0.69, 66 |
| `galoot/core/blade_set.hpp.cxx` | clang-debug | 0.09, 0.55, 100 | 0.15, 0.55, 100 |
| `galoot/core/mv.hpp.cxx` | gcc-debug | absent | 0.13, 0.92, 77 |
| `galoot/core/mv.hpp.cxx` | clang-debug | absent | 0.20, 0.93, 112 |
| `test/core/smoke_test.cpp` | gcc-debug | 0.16, 1.11, 87 | 0.27, 1.12, 87 |
| `test/core/smoke_test.cpp` | clang-debug | 0.14, 1.02, 122 | 0.26, 1.03, 124 |
| `test/core/blade_set_test.cpp` | gcc-debug | 0.84, 10.59, 265 | 0.95, 10.83, 269 |
| `test/core/blade_set_test.cpp` | clang-debug | 1.41, 23.14, 182 | 1.64, 23.30, 179 |
| `test/core/plan_test.cpp` | gcc-debug | 2.74, 31.68, 360 | 3.08, 31.69, 360 |
| `test/core/plan_test.cpp` | clang-debug | 2.73, 34.83, 345 | 2.87, 34.77, 345 |
| `test/core/mv_test.cpp` | gcc-debug | absent | 1.41, 11.76, 335 |
| `test/core/mv_test.cpp` | clang-debug | absent | 1.33, 10.82, 271 |
| `test/support/rational_test.cpp` | gcc-debug | absent | 1.10, 7.14, 252 |
| `test/support/rational_test.cpp` | clang-debug | absent | 0.94, 5.51, 223 |

The header units and the tests that existed before move by under 2.5 % in instructions; `blade_set_test.cpp` rises 2.3 % on GCC and 0.7 % on Clang, from its new test case and the algebra tags. The new costs are `mv_test.cpp` and `rational_test.cpp`.

# WU8b: apply, conversions, truncate (plan 0016, criteria 21 to 23)

Measured by Claude (agent, Opus 5.5), cpp-expert role, with `measure_mv_b.py` here, on the headers of `7ee7d32` (the code of `be1b209`; `4253555` changes `mv.hpp`'s comments only). GCC 16.2.1 and Clang 23.1.4 with libstdc++ 16; every compile pinned to CPU 10 through `tools/timed measure`, into this campaign's own log, `runs.tsv` here (selections `mvb-limits:*`, `mvb-growth:*`, `mvb-headers:*`, `mvb-uses:*`). The sibling thread was busy at most 0.01 s and other processes ran on CPU 10 at most 0.02 s per compile. The machine was shared (load average 1 to 5 during the runs); instruction counts are compared, not times. The timing lock of the machine's measurement queue was free when the runs started and was not taken. No bound applies (C14). Outputs in `out-wu8b-7ee7d32/`; the generated units are not kept.

Commands, from the worktree root with the job pool exported:

```bash
export TIMED_LOG=$PWD/docs/agent/records/performance/mv/runs.tsv
M=docs/agent/records/performance/mv/measure_mv_b.py; O=docs/agent/records/performance/mv/out-wu8b-7ee7d32
python3 -I $M limits $O 10
python3 -I $M growth 11d1ec9 $O 3 10
python3 -I $M headers 11d1ec9 $O 3 10
python3 -I $M uses $O 3 10
```

## Limits (criterion 21, `limits.tsv`)

`apply` on `full<n>` times `full<n>`, and on each unary map of `full<n>`, at the largest n whose plan or map builds at the default limits (product_plan's and unary_map's comments), on run-time inputs, one trial build at each point, in the three configurations (Galoot's flags at -O0 with checking, at -O2 without, and bare `-std=c++23 -pedantic-errors -O2`):

| Compiler | Points | Result |
| --- | --- | --- |
| Clang 23.1.4 | geometric product n = 5 in R(5,0,0), R(4,0,1), conformal(3); dense form n = 4; wedge, antiproduct, antiwedge n = 5; reverse, antireverse, grade involution n = 7; complements n = 8 | builds at every point in all three configurations, except the two complements at n = 8 in the bare configuration, which stop in the map's own generation (`plan.hpp:915`, the step limit; without `_GLIBCXX_ASSERTIONS` libstdc++'s subscripts count more, as unary_map's comment says) and so are not points where the map builds |
| GCC 16.2.1 | geometric product n = 6 in the three diagonal-type forms (4,096 terms, one fold of 4,096 statements); dense form n = 5; wedge, antiproduct, antiwedge n = 6; maps n = 9 | builds at every point in all three configurations; the largest, the dense n = 5 product, at 82 G instructions, 3.9 s, and 0.8 GiB at -O0 |

So `apply` builds wherever its plan or map builds. No plan that Clang builds has more than 1,024 terms, half of its fold limit of 2,048 (decision 0154).

## P2, the fixed use through apply (criterion 22, `growth.tsv`, `growth-summary.txt`)

Plan 0013's fixed use, four typed rigid(3) products (point ⟑ point, line ⟑ plane, motor ⟑ point, point ⟑ motor) in R(n−1,0,1) with e4 as e_n, -O0 with the test flags, three repeats. "Plans" reads a term of each plan at run time; "apply" applies the four products to multivectors read at run time. Mean retired instructions above the baseline of the same tree and n (the sets and no product), in M:

| | GCC plans, before (`11d1ec9`) | GCC plans, after | GCC apply, after | Clang plans, before | Clang plans, after | Clang apply, after |
| --- | --- | --- | --- | --- | --- | --- |
| n = 4 | 304.6 | 305.1 | 822.8 | 549.0 | 548.8 | 802.2 |
| n = 8 | 307.3 | 307.7 | 825.7 | 552.2 | 552.9 | 804.6 |
| n = 12 | 311.7 | 311.0 | 829.7 | 557.4 | 557.1 | 808.0 |
| n = 16 | 316.5 | 316.3 | 835.6 | 562.1 | 561.6 | 816.9 |

- Growth with n at fixed use, n = 4 to 16: plans 12 M on GCC and 13 M on Clang, before and after alike; through apply 13 M on GCC and 15 M on Clang. The extra 1 to 2 M is within the repeats' spread at n = 16 on Clang (8.7 M) and is the layout checks of the eight multivector types per n (each type checks its layout once against n). The growth itself is the one evaluation of `metric_like` per algebra that decision 0107 identified, as in decision 0113.
- Cost of applying the four products above reading their plans: +518 M on GCC and +253 M on Clang at every n: the multivector types of the inputs and results, the four kernels, and the functions that make the inputs.
- Baseline: 905 → 915 M on GCC and 924 → 934 M on Clang (the header cost below).

## Header cost to every includer (`headers.tsv`)

An empty unit including one header, -O0 with the test flags, three repeats each, interleaved; mean retired instructions, spread in parentheses:

| Header | GCC before (`11d1ec9`) | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `galoot/core.hpp` | 0.8575 G (0.0 M) | 0.8676 G (0.2 M), +1.2 % | 0.9169 G (4.1 M) | 0.9255 G (2.2 M), +0.9 % |
| `galoot/core/mv.hpp` | 0.8567 G (0.3 M) | 0.8666 G (0.1 M), +1.2 % | 0.9162 G (0.9 M) | 0.9265 G (4.1 M), +1.1 % |

The rise exceeds the spread on both compilers. `mv.hpp` adds no standard header and instantiates nothing at namespace scope; the cause, by construction and not separately measured, is parsing the added declarations (the kernel, `apply`, the conversion constructors, `truncate`, and the scalar checks), about 10 M instructions on GCC and 9 M on Clang.

## Cost per use (criterion 23, `uses.tsv`, `uses-summary.txt`)

Slopes from 64 to 256 uses over distinct four-blade layouts of R(15,0,1), mean of three repeats, in retired instructions per use. Every unit compiled.

| Use | GCC -O0 | GCC -O2 | Clang -O0 | Clang -O2 |
| --- | --- | --- | --- | --- |
| The plan of a new product of a four-blade layout with itself (16 terms), generated | 65 M | 64 M | 157 M | 157 M |
| Its kernel and the result type, applied once, above the plan | 83 M | 44 M | 34 M | 32 M |
| An existing product applied in a function of its own, with the construction of its input | 6.3 M | 5.7 M | 3.0 M | 4.3 M |
| A conversion to the same layout reversed, above making a value of that layout | 18 M | 11 M | 7.8 M | 8.3 M |
| A truncation to the first two blades, above making a value of that layout | 13 M | 7.2 M | 6.9 M | 6.1 M |

The kernel's cost on GCC at -O0 (83 M for 16 terms) exceeds its plan's; it includes a new result type with its layout checks, the kernel's variable template, the instantiation of the expanded function and of its term calls, and their code at -O0. A program pays it once per distinct product, as it pays the plan.

## W10 (criterion 23)

Before: fresh gcc-debug and clang-debug trees at `11d1ec9`, rows copied into `compile.tsv` in `bc4b767` (137 and 139 rows). After: the fresh trees of the final check, in the commit that records it (plan 0016, Outcome WU8b).
