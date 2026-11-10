# The reference kernels inline in the timed loops (plan 0038, part D, decision 0237 item 2)

Recorded by Claude (agent, Opus 5.5), performance-engineer role, 2026-11-10, on `feature/cost-evidence--bench--inline`. The change is at `0c429d5`; the "before" assembly is at `18849b5`, the `feature/cost-evidence` tip it branches from. Toolchain GNU 16.2.1 and Clang 23.1.4. No timing was made, and nothing under `include/` changed.

## Method

`run.sh` writes the assembly of the seven benchmark programs with the flags that gcc-measure and clang-measure give a `test/bench` source, at `-O2`, with `-S` (the flags of E1's `wu17-asm/run.sh`). `calls.py` counts, per file, the calls and symbol jumps to the program's own form lambdas, to `reference_kernels`, and to `galoot::core` functions, each named without its template arguments.

    bash docs/agent/records/performance/wu17-bench/part-d-inline/run.sh <scratch directory>

The counts are over the whole file. The timed loops are inlined into `bench::main`, one instantiation per loop, so a kernel called from both loops counts 2.

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

## Checks

- `rga3.benchmark_<operation>_inline` (the demangled symbol table defines no `reference_kernels` function and no lambda of `main`) fails before the change in gcc-debug (7 of 7) and clang-measure (the 4 transforms and compose), and passes after it in gcc-debug, clang-debug, gcc-measure and clang-measure, with `rga3.reference_kernels`, `reference.kernels` and `rga3.benchmark_*` (16 of 16 in each).
- Mutants `transform_line_kernel_not_always_inline` and `transform_line_reference_lambda_not_always_inline` (`records/mutations/0038-cost-evidence/bench.toml`) are killed by `rga3.benchmark_transform_line_inline` in gcc-debug, clang-debug, gcc-measure and clang-measure.
