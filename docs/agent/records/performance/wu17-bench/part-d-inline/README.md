# The reference kernels inline in the timed loops (plan 0038, part D, decision 0237 item 2)

Recorded by Claude (agent, Opus 5.5), performance-engineer role, 2026-11-10, on `feature/cost-evidence--bench--inline`. The change is at `0c429d5`; the "before" assembly is at `18849b5`, the `feature/cost-evidence` tip it branches from. Toolchain GNU 16.2.1 and Clang 23.1.4. No timing was made, and nothing under `include/` changed.

## Method

`run.sh` writes the assembly of the seven benchmark programs with the flags that gcc-measure and clang-measure give a `test/bench` source, at `-O2`, with `-S` (the flags of E1's `wu17-asm/run.sh`). `calls.py` counts, per file, the calls and symbol jumps to the program's own form lambdas, to `reference_kernels`, and to `galoot::core` functions, each named without its template arguments. `loops.py` counts, for each loop of the program's own functions (a label and a jump back to it), its scalar and packed multiplications and its calls.

    bash docs/agent/records/performance/wu17-bench/part-d-inline/run.sh <scratch directory>

The call counts are over the whole file. The timed loops lie in the timed functions, one instantiation per loop: a lambda local to `bench::main` in every program on Clang and in the transforms and compose on GCC, and `bench::run<...>.isra.0` for the three products on GCC. So a kernel called from both loops counts 2.

## Counts

| Program | Compiler | Before (`18849b5`) | After (`0c429d5`) |
| --- | --- | --- | --- |
| transform_point | GCC | reference_kernels 2 | none |
| transform_point | Clang | reference_kernels 2; galoot::core::transform 2 | galoot::core::transform 2 |
| transform_line | GCC | reference_kernels 2 | none |
| transform_line | Clang | reference_kernels 2; galoot::core::transform 2; galoot::core::detail::run_product 1 | galoot::core::transform 2; galoot::core::detail::run_product 1 |
| transform_plane | GCC | reference_kernels 2 | none |
| transform_plane | Clang | reference_kernels 2; galoot::core::transform 2 | galoot::core::transform 2 |
| compose_motors | GCC | reference_kernels 2; galoot::core::detail::run_product 2 | galoot::core::detail::run_product 2 |
| compose_motors | Clang | reference_kernels 2; galoot::core::detail::run_product 2 | galoot::core::detail::run_product 2 |
| wedge_point_point, wedge_point_point_point, antiwedge_line_plane | both | none | none |

Every reference-kernel call is gone on both compilers. The calls left are the library's and are the same before and after: on Clang, `transform` kept out of line (and, for a line, its first stage's `run_product`); on both compilers, compose's `run_product`.

An intermediate step showed why the forms' lambdas are marked too: with only the kernels marked `always_inline`, GCC kept the reference lambda (main's lambda #2) out of line in the three transforms and compose, and Clang kept it (`$_1`) out of line in transform_line, so the call had moved and not gone. With both forms' lambdas marked as well, neither form adds a call of the program's own; marking the library's form's lambda changes none of the library's calls.

## What the inlined reference loops do (`loops.py`, at `0c429d5`)

Removing the call does more on Clang than remove a call. With the reference kernel inline, Clang vectorizes the reference's throughput loop across iterations, two operations per iteration, one in each lane of a packed register:

| Program (Clang 23, `-O2`) | Reference throughput loop | Reference single-operation loops | Library loops |
| --- | --- | --- | --- |
| transform_point | 48 `mulpd` (96 lanes: two transforms) | 40 `mulsd` + 4 `mulpd`, or 48 `mulsd` | one call of `galoot::core::transform` per operation |
| transform_line | 72 `mulpd` (144 lanes) | 28 `mulsd` + 23 `mulpd`, or 31 + 21 | one call per operation |
| transform_plane | 48 `mulpd` (96 lanes) | 28 `mulsd` + 10 `mulpd`, or 48 `mulsd` | one call per operation |
| compose_motors | 48 `mulpd` (96 lanes) | 20 `mulsd` + 14 `mulpd`, or 28 + 10 | one call of `run_product` per operation |

Before the change the out-of-line kernels were scalar for the transforms (48, 72, 48 `mulsd`) and packed within one call for compose. The results are the same, since no sum is reassociated (`rga3.benchmark_<operation>` passes in clang-measure). Which single-operation loop is the latency loop and which is the scalar remainder of the vectorized throughput loop is not attributed here.

On GCC every timed loop of the transforms and compose does one operation per iteration in both forms: the reference 48, 72, 48 and 48 `mulsd`, the library inline beside it (transform_point 32 `mulsd` + 8 `mulpd`). For the three products both forms are inline on both compilers, and Clang vectorizes some of their loops across iterations as well (wedge_point_point has loops of 24 and 30 `mulpd`, against the 12 multiplications of one join); those loops are not attributed to a form here either.

So on Clang a throughput gap that E2's rerun finds between the library and the reference includes the vectorization across iterations that the library's call prevents, not only the call's own cost: it is nearer to what inlining the library's `transform` and `compose` could buy than to the price of a call (inferred: an inlined library form could be vectorized the same way). Clang's latency loop, where each result feeds the next operation, cannot be vectorized across iterations and is nearer the call's cost; E2 should read the two loops apart. On GCC the comparison is like for like.

## Checks

- `rga3.benchmark_<operation>_inline` (the demangled symbol table defines no `reference_kernels` function and no lambda of `main`) fails before the change in gcc-debug (7 of 7) and clang-measure (the 4 transforms and compose), and passes after it in gcc-debug, clang-debug, gcc-measure and clang-measure, with `rga3.reference_kernels`, `reference.kernels` and `rga3.benchmark_*` (16 of 16 in each).
- Mutants `transform_line_kernel_not_always_inline` and `transform_line_reference_lambda_not_always_inline` (`records/mutations/0038-cost-evidence/bench.toml`) are killed by `rga3.benchmark_transform_line_inline` in gcc-debug, clang-debug, gcc-measure and clang-measure.

## Departures

- **One author for the tests and the code.** The same agent wrote `rga3.benchmark_<operation>_inline` (on `feature/cost-evidence--bench--inline--tests`, cherry-picked with `-x` as `5d2fb38` and `f6b8276`) and the change it judges. The second test commit was written after the author saw the intermediate state (the kernels inlined, GCC keeping the reference lambda out of line), so the tests are not independent of the code. The blind review (`records/reviews/2026-11-10-wu17-bench-inline-blind.md`) checked their power on its own evidence.
- **Mutation records of a dirty tree.** The two inlining mutants' rows name revision `0c429d52e66c+dirty`: they ran before `b480bd3` committed `bench.toml`, with the code of `0c429d5` unchanged.
