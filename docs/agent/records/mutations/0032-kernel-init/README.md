# Plan 0032's mutants: the kernels' value-initialized result

Run on 2026-11-10 (UTC) by Claude (agent, Opus 5.5, cpp-expert, implementer of plan 0032) with `tools/mutate`, every build and test through `tools/timed` with the shared job pool (23 of 23 slots free before and after each run). Compilers: g++ 16.2.1 (gcc-debug, gcc-release), clang++ 23.1.4 (clang-debug).

- **Tip:** `feature/core-kernel-init` at `20ae7ae`, `tools/mutate gcc.toml` (gcc-debug, gcc-release) and then `tools/mutate clang.toml` (clang-debug), from 01:26:50 to 01:32:15. Rows: `out/gcc-<preset>.tsv`, `out/clang-clang-debug.tsv`; builds and tests are the rows `mutant:<name>` of `records/performance/runs.tsv` on this branch.
- **Gap:** a worktree detached at `20ae7ae` (since removed) with `test/core/mv_test.cpp` checked out from `e2f317a`, the file before the canaries, so the tree is the fixed kernels with the suite before this unit. `tools/mutate gap/gap.toml --preset gcc-debug run_map_shared_initializer run_product_shared_initializer`, the spec a copy of `gcc.toml` kept outside the repository during the run. Rows: `gap/out/gap-gcc-debug.tsv` (revision `20ae7aecc31e+dirty`: the test file), campaign log `gap/runs.tsv`.

Each mutant restores `std::array<T, R> out{};` in one kernel, the form GCC 16.2.1 compiles wrongly after a constant evaluation of the same kernel in the translation unit. Judged by `core.mv` (the two killable mutants) and by `core.mv` with `core.linear` (the sum's).

## Verdicts

| Mutant | Gap: gcc-debug, suite before the canaries | Tip: gcc-debug | Tip: gcc-release | Tip: clang-debug | Failed cases on GCC at the tip |
| --- | --- | --- | --- | --- | --- |
| `run_map_shared_initializer` | survived | killed | killed | equivalent | "A conversion is zero on the blades its source lacks after a constant evaluation of it" |
| `run_product_shared_initializer` | survived | killed | killed | equivalent | "apply onto a wider target is zero where no term reaches after a constant evaluation of it" |
| `run_sum_shared_initializer` | not run | equivalent | equivalent | equivalent | none |

- **The gap.** The canaries close a gap in `core.mv`: each of the first two mutants survives the suite without them and is killed by the suite with them, on gcc-debug. `tools/mutate` exits 1 on the gap run because its spec expects a kill, which is the expectation of the tip.
- **Each canary kills its own mutant and only it.** The conversion canary is the only failed case of the `run_map` mutant, and the product canary of the `run_product` one, on both GCC presets.
- **Clang.** All three are equivalent on clang-debug, as declared: Clang 23 compiles the value-initialization correctly.
- **`run_sum_shared_initializer` is equivalent on every compiler.** A sum's target is `sum_layout`, the union of its operands' layouts, and a term of `run_sum` is first on each of its places (x's terms on x's blades, y's on the blades x lacks), so a place zero-filled at run time is assigned before it is read. No public or internal caller passes a wider target. The fix in `run_sum` is kept because the kernel has the same shape as the other two; no test can see whether it is there.

## Departures

- One configure of gcc-debug in the gap worktree ran through `tools/timed` before the campaign log was named, so its row went to that worktree's `runs.tsv` and was lost when the worktree was removed (exit 0, a few seconds).
- The first run of both specs was refused before building: `core.mv_truncate_nonzero_int64` runs `core_mv_truncate_mode_1`, which no selection built. The specs were corrected at `20ae7ae` and rerun; the refused run built nothing and wrote no rows.
