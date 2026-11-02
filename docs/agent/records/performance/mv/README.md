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

The before rows are `0782a8e` (fresh gcc-debug and clang-debug trees at `965059e`); the after rows are taken from fresh trees at the final revision and named in plan 0016's outcome.
