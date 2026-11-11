# Plan 0040: the code size of E1's probe before and after the forced inlining

Plan 0040 section 9, "Code size". Measured by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-10 with `run.sh` in this directory, from the root of the `--code` worktree:

```
bash docs/agent/records/performance/pre-release-core-pass/object-sizes/run.sh 8058589 <scratch directory>
```

- Before: the headers of `8058589`, the branch point of `feature/pre-release-core-pass` (its `include/` is that of `77cd947`). After: `d382516`, part A2, the thirteen attributes.
- Workload: `records/performance/wu17-asm/probe.cpp`, E1's seven functions, unchanged, compiled to an object with `-c`. Flags: E1's (`-DGALOOT_CHECKING=0`, the S4 warnings with `-Werror`, `-ffp-contract=off`, `-std=c++23`) with `-O0` or `-O2`; no `-g`.
- Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16. `size` from GNU binutils (Berkeley format).
- Rows: `out/sizes.tsv`; the compiles' run log: `runs.tsv` (step `compile`, selection `pre-release-core-pass:size:<compiler>:<level>:<side>`).

## Result

| Compiler | Level | text before | text after | Change |
| --- | --- | --- | --- | --- |
| GCC | `-O0` | 110,511 | 121,910 | +11,399 (+10.3%) |
| GCC | `-O2` | 4,513 | 4,497 | -16 (-0.4%) |
| Clang | `-O0` | 74,739 | 73,755 | -984 (-1.3%) |
| Clang | `-O2` | 4,766 | 4,347 | -419 (-8.8%) |

`data` is 8 bytes on GCC and 0 on Clang, `bss` 0, on every row, before and after.

At `-O2` the code does not grow on either compiler. On Clang it shrinks, inferred to be the out-of-line `transform` and `run_product` bodies and E1's five calls to them, which `rga3.kernels_inline` shows gone; the objects were not compared function by function. On GCC, which already inlined them, the size is about the same. At `-O0` GCC's object grows by a tenth, because every call of a forced function now carries its body at `-O0` too, while Clang's shrinks a little. The advisory report's one-compile observation of assembly size at `-O0 -g` on a five-function probe (GCC +4.6%, Clang -2%) has the same direction; this is the object's text, on E1's seven functions, without `-g`.

These are sizes, which do not vary between runs; one compile each. Build time and memory are the W10 campaign's (`../cost/`), at the feature tip.
