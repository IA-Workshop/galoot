# Plan 0040 mutants: the forced inlining (part A)

Plan 0040 section 8. Run by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-10 with `tools/mutate`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.4, both with libstdc++ 16), at `d382516` (part A2) in the `--code` worktree, with only records dirty. Rows: `out/<spec>-<preset>.tsv`; the runs are the rows `mutant:<name>`, `mutant:baseline` and `mutant:restore` of `records/performance/runs.tsv`. The job pool read 23 of 23 slots free before the first runs; before `gap.toml` it read 0 of 23 free, with two other workstreams' `ctest` as clients, so its wall times are those of a loaded pool.

`rga3.kernels_inline` compiles its probe with fixed flags (`-O2`, checking off), so a tree's preset matters to the attribute mutants only for its compiler; their selection's one target is the probe's object library, built with the tree's flags, so that a mutant breaking the build would be classified.

## Summary

| Spec | Mutants | gcc-debug | clang-debug |
| --- | --- | --- | --- |
| `attributes-gcc.toml`, `attributes-clang.toml` | 13 each | 3 killed, 8 survived, 2 equivalent | 5 killed, 6 survived, 2 equivalent |
| `check.toml` | 3 | 3 killed | 3 killed |
| `gap.toml` (the suite without the check) | 1 | survived, as declared | survived, as declared |
| `gap-gcc.toml`, `gap-clang.toml` (the check alone) | 1 | survived, as declared | killed, as declared |

None was unjudged. `tools/mutate` exited 1 on both attribute specs, for the four kills below that the plan expected to survive; every other verdict is as declared.

## The gap (W3)

`macro_empty` defines `GALOOT_ALWAYS_INLINE` as nothing, so that no function of the chain is forced. The suite of the core and rga3 modules without `rga3.kernels_inline` lets it live on both presets (`gap.toml`): no other test reads where the library's code is placed, and the values do not change. `rga3.kernels_inline` alone kills it on clang-debug (`gap-clang.toml`), with E1's five calls back. So the mutant counts for the gap on Clang. On gcc-debug the check lets it live too (`gap-gcc.toml`), as the plan expected: without any attribute GCC inlines all eleven probe functions, the four the probe adds to E1's seven included.

## Each attribute removed alone

| Mutant (attribute removed from) | gcc-debug | clang-debug | Plan's expectation |
| --- | --- | --- | --- |
| `apply_not_forced` (binary `apply`, target given) | killed | killed | killed on both |
| `run_product_not_forced` | survived | killed | killed on Clang |
| `transform_not_forced` | **killed** | killed | killed on Clang; survives on GCC |
| `run_map_not_forced` | survived | survived | survives |
| `unary_apply_not_forced` (unary `apply`, target given) | survived | survived | survives |
| `run_sum_not_forced` | survived | survived | survives |
| `sum_not_forced` | survived | survived | survives |
| `run_change_not_forced` | survived | survived | survives |
| `transform_middle_not_forced` | survived | **killed** | survives |
| `transform_outer_not_forced` | survived | survived | survives |
| `compose_not_forced` | **killed** | **killed** | survives |
| `apply_result_layout_not_forced` | equivalent | equivalent | equivalent |
| `unary_apply_result_layout_not_forced` | equivalent | equivalent | equivalent |

The calls that come back in the four unexpected kills, found again with scratch copies of the headers carrying each mutant's edit and the check run by hand on the probe:

- GCC, `transform` not forced: `call transform` from `transform_point`, `transform_line` and `transform_plane`. Without any attribute GCC inlines `transform`. Inferred, not traced in GCC's inliner dumps: with the functions below it forced into it, `transform`'s body grows past what GCC's heuristic inlines.
- GCC, `compose` not forced: `call compose` from `compose_motors`, for the same reason.
- Clang, `transform_middle` not forced: `callq transform_middle` from `transform_line`.
- Clang, `compose` not forced: `callq compose` from `compose_motors`.

By the plan's reading, a kill of a mutant declared to survive is evidence that the attribute is needed now, not a failure: four of the eight attributes the plan held to be guards are load-bearing on at least one compiler at `-O2`, because forcing a function inline moves its callers' inlining budget, as the advisory report's experiment found for `apply`. The surviving six (`run_map`, the unary `apply`, `run_sum`, `sum`, `run_change`, `transform_outer`) stay as guards. The two `apply` overloads that take no target layout have no caller in the library, so no probe reaches them: equivalent, as declared.

## The check's own mutants

`check_ignores_jmp` (the branch pattern loses `jmp` and `jmpq`), `check_ignores_plt` (the symbol must end the operand, so `@PLT` and `@GOTPCREL` forms are missed), and `check_skips_missing` (a missing function is not reported) are each killed on both presets by the fixture test written for it: `tools.kernels_inline_rejects_jmp`, `_rejects_plt`, `_rejects_missing_function`.
