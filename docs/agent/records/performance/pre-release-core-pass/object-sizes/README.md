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

`data` is 8 bytes on GCC at `-O0` and 0 at `-O2`, and 0 on Clang, before and after; `bss` is 0 on every row.

The GCC `-O0` after-size depends on the include path: the after object holds a string that grows with the path (the blind review found it to be the path of `transform.hpp`, which the before object does not hold; not traced further here), and the after-side is compiled with `-Iinclude`. With a longer path the same object is larger: at `a6f3196`, 123,767 with `-Iinclude` and 123,911 with a 61-character path to the same directory. The comparison is like with like only when the after-side's include path is `include`, as `run.sh` gives it.

At `-O2` the code does not grow on either compiler. On Clang it shrinks, inferred to be the out-of-line `transform` and `run_product` bodies and E1's five calls to them, which `rga3.kernels_inline` shows gone; the objects were not compared function by function. On GCC, which already inlined them, the size is about the same. At `-O0` GCC's object grows by a tenth, because every call of a forced function now carries its body at `-O0` too, while Clang's shrinks a little. The advisory report's one-compile observation of assembly size at `-O0 -g` on a five-function probe (GCC +4.6%, Clang -2%) has the same direction; this is the object's text, on E1's seven functions, without `-g`.

These are sizes, which do not vary between runs; one compile each. Build time and memory are the W10 campaign's (`../cost/`), at the feature tip.

## At the feature tip

Run again at `7a27f76`, after the merge of the comment parts (C, D and E), from a copy of `run.sh` writing to a scratch directory: every size is the same as at `d382516`, as expected of a change to comments only. Its eight compile rows are appended to `runs.tsv`.

## With the public operations forced

Run again at `a6f3196` (the blind review's M1 fix: eleven public functions forced as well), from a copy of `run.sh` writing to a scratch directory; rows in `out/sizes-public-ops.tsv`, the eight compile rows appended to `runs.tsv`.

| Compiler | Level | text before (`8058589`) | text at `d382516` | text at `a6f3196` | Change from before |
| --- | --- | --- | --- | --- | --- |
| GCC | `-O0` | 110,511 | 121,910 | 123,767 | +13,256 (+12.0%) |
| GCC | `-O2` | 4,513 | 4,497 | 4,497 | -16 (-0.4%) |
| Clang | `-O0` | 74,739 | 73,755 | 76,085 | +1,346 (+1.8%) |
| Clang | `-O2` | 4,766 | 4,347 | 4,347 | -419 (-8.8%) |

At `-O2` E1's probe is the same as with the chain alone forced: its seven operations already inlined there. At `-O0` the public products' bodies are now copied into the probe's wedge and antiwedge functions as well, so GCC's object grows by 12.0% in all and Clang's, which shrank before, grows by 1.8%. `data` and `bss` are as above.
