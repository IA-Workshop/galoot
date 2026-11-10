# WU17 E1: the seven rigid(3) operations read in the assembly

Plan 0038 criterion 5 (part E1, PERFORMANCE P4), measured on 2026-11-10 by Claude (agent, Opus 5.5) in the performance-engineer role, on branch `records/cost-evidence-measurements--asm`, from `feature/cost-evidence` at `18849b5` (which includes the `basis_change` unit). The branch adds only files in this directory, so every compile here read the headers of `18849b5`. Nothing was timed; no lock was taken.

Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4, both with libstdc++ 16 (`__GLIBCXX__` 20260810), x86-64 baseline target (SSE2, no `-march`), host `ds-001`. Flags: those CMake gives a `test/bench/` source under the `gcc-measure` and `clang-measure` presets, read from each tree's `compile_commands.json`, with `-S`:

```
-DGALOOT_CHECKING=0 -Itest -Iinclude -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -O2 -std=c++23 -S
```

## Departures, skipped checks, ambiguities (C20)

1. **Branch base.** The plan creates `records/cost-evidence-measurements` from `develop` after parts C and D merge. Parts C and D are only on `feature/cost-evidence` until it closes, so this branch is `records/cost-evidence-measurements--asm` from `feature/cost-evidence` at `18849b5`, as the brief says.
2. **Probe and benchmark programs.** The counts are those of a probe unit, `probe.cpp`: one external function per operation whose body is exactly the expression the benchmark program in `test/bench/` times (for example `Motor{core::compose(a, b)}`, `Plane{(p ^ q) ^ r}`), over the rga3 preset's types. A benchmark program inlines the operation into a loop with the checksum, so the operation's own instructions cannot be separated there. The seven benchmark programs were also compiled, with the same flags, and read only for the calls in their timed loops (section "Calls in the benchmark programs"); their assembly is not kept (about 0.6 to 0.9 MB each) and `run.sh` rewrites it.
3. **Stack protector.** The flags name no `-fstack-protector*`, but both compilers enable `-fstack-protector-strong` by default on this system (`g++ -Q --help=common`, `clang++ -###` shows `-stack-protector 2`). GCC emits no check in any probe function; Clang emits one where an out-of-line kernel's result goes through a stack buffer. That check's branch is counted apart as a guard branch.
4. **Negations** are counted as found and compared with rule K's only in total; the compilers move signs between negations, subtractions and shared operands, and the difference was not traced term by term (finding 4).
5. **Cause of the 49th product** is shown by a one-switch ablation (`-mllvm -slp-copyable-elements=false`) and by the symbolic trace; LLVM's source was not read, so the description of the transform rests on its option's help text and on the code it produced.
6. **WU13's 49** (records `transform-cost/asm/asm.clang.s`, headers of `1ca4508`) shows the same `[1.0, 0.0]` constant in its `compose` kernel, so the same cause is likely there (inferred; that file was not traced).
7. **The variants** of the Clang probe (without the SLP vectorizer, without copyable elements, with an inlining threshold of 2000) are used only to find causes. They are not the measured configuration.
8. **Run log.** `runs.tsv` here has the two configure runs of the measure presets (only for `compile_commands.json`) and two runs of the compile set, the first before `run.sh` had its variant section, each row at `18849b5` on this branch, mode `shared`.

## Commands

From the repository's root, with the job pool's environment set (the compiles take one slot each, one at a time, about 0.3 to 1.8 s each):

```bash
eval "$(tools/jobpool env)"
cmake --preset gcc-measure -DCMAKE_EXPORT_COMPILE_COMMANDS=ON     # only to read the flags
cmake --preset clang-measure -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
bash docs/agent/records/performance/wu17-asm/run.sh                # compiles, counts, traces
```

`run.sh` runs every compile through `tools/timed` into this directory's `runs.tsv` (selections `wu17-asm:probe:<compiler>`, `wu17-asm:bench_<operation>:<compiler>`, `wu17-asm:probe:clang:<variant>`), keeps `probe.gcc.s` and `probe.clang.s` here, writes the rest to `build/wu17-asm/`, and writes `out/count.md`, `out/trace.md` and `out/trace-variants.md`.

## How the counts are made

- `count.py` counts instructions per operation: the probe function and every function of the same file it reaches by a call or a jump. A `mulpd` or `addpd`/`subpd` on an `xmm` register counts as two operations. It lists conditional branches (with the stack-protector branch apart), compares, calls and symbol jumps.
- `trace.py` runs each operation's code symbolically, lane by lane, over the instructions these functions use (it stops on any other instruction and on any conditional branch other than the stack protector's), with every double an expression node. A lane is **live** when a stored result depends on it, **dead** when none does (the unused half of a packed instruction), and **padding** when it is live but its value cannot change the result: a product of two constants, or a sum or difference with a constant zero. It then expands every stored result into a polynomial in the operands' coefficients and compares it, term by term with exact rational coefficients, with the reference kernel's in `test/bench/reference_kernels.hpp`, which `reference_kernels.py` derives without the library's code.
- The expected counts are rule K's (`records/spikes/spike-e/COUNTING-RULE.md`, `counts.md`): for each product and `compose` its "declared type" row; for each transform its form A (the kernels) row.

## Per-operation table

Rule K, then each compiler: multiplications as live lanes (with computed lanes and the instruction mix), additions and subtractions as live lanes (computed lanes), negated lanes live, instructions, conditional branches on blade data, calls, allocations. The full output, with the parts of each out-of-line operation, is in `out/count.md` and `out/trace.md`.

| Operation | Rule K: mul, add/sub, neg | GCC 16: mul | add/sub | neg | instr. | Clang 23: mul | add/sub | neg | instr. |
| --- | --- | --- | --- | ---: | ---: | --- | --- | ---: | ---: |
| point ∧ point | 12, 6, 4 | 12 (8 `mulsd`, 2 `mulpd`) | 6 | 4 | 46 | 12 (12 `mulsd`) | 6 | 4 | 44 |
| point ∧ point ∧ point | 24, 14, 2 | 24 (24 `mulsd`) | 14 | 2 | 70 | 24 (24 `mulsd`) | 14 | 2 | 69 |
| line ∨ plane | 12, 8, 1 | 12 (12 `mulsd`) | 8 | 1 | 42 | 12 (12 `mulsd`) | 8 | 1 | 41 |
| transform(point, motor) | 48, 36, 3 | 48 (32 `mulsd`, 8 `mulpd`) | 36 (44 computed) | 1 | 174 | 48 (48 `mulsd`) | 36 | 1 | 149 |
| transform(line, motor) | 72, 58, 9 | 72 (72 `mulsd`) | 58 | 4 | 225 | 72 (62 `mulsd`, 5 `mulpd`) | 58 (60 computed) | 4 | 251 |
| transform(plane, motor) | 48, 36, 5 | 48 (48 `mulsd`) | 36 | 2 | 145 | 48 (28 `mulsd`, 10 `mulpd`) | 36 (42 computed) | 2 | 150 |
| compose(motor, motor) | 48, 40, 4 | 48 (48 `mulsd`) | 40 | 7 | 166 | 48 + 1 padding (35 `mulsd`, 7 `mulpd`) | 40 + 1 padding | 7 | 199 |

On both compilers and in every operation: no conditional branch on blade data (no conditional branch at all except Clang's stack-protector check in `transform(line, motor)` and `compose`), no compare (checking is off), no allocation (no call to `operator new` or `malloc`), no fused multiply-add (the baseline target has none, and `-ffp-contract=off` forbids contraction anyway). Calls: none on GCC; on Clang, as in the next section. Every stored result equals the reference kernel's polynomial exactly, on both compilers and in all three variants, so no term is missing, added, or folded into another (no coefficient other than ±1 occurs, so there is nothing to fold).

## Findings

1. **Multiplications equal rule K's on every operation and both compilers**, counted as lanes whose value the result depends on. The one difference is Clang's `compose`: 49 lanes computed, one of them padding (finding 2).
2. **Clang's 49 for `compose`: a padding lane of the SLP vectorizer's copyable-element transform.** In the out-of-line kernel `run_product<product_kernel>`, Clang builds two result places, e14 and e23 of the kernel's layout (e41 and e23 of `Motor`), as one two-lane chain. e23 has eight terms and e14 four, so at one step the e14 lane takes the term a3·b0 (the first motor's e1234 coefficient times the second's e41) and the e23 lane has none. Clang fills it with an identity: `[a3, 1.0] × [b0, 0.0]` (`mulpd`; the 0.0 is the upper half that `movsd` cleared when it loaded b0, the 1.0 a constant `.LCPI11_1`), then `subpd`, so the e23 lane computes x − 1.0·0.0 = x. That is the 49th multiplication and a 41st subtraction lane; neither changes a bit of the result (x − (+0) = x for every x, a zero's sign included). Evidence: the trace marks exactly these two lanes as padding, reaching result place 4 (e23) only; with `-mllvm -slp-copyable-elements=false` (the option's help: "Try to replace values with the idempotent instructions for better vectorization") Clang computes 48 and 40 with no padding and nothing else changes in any of the seven operations; without the SLP vectorizer (`-fno-slp-vectorize`) and with the kernel inlined (inlining threshold 2000) it is also 48. GCC inlines the kernel and does not vectorize it (48 `mulsd`). The cost is not the lane but the instructions around it: the kernel is 167 instructions with the padding and the build of `[a3, 1.0]` (a `shufpd` and a spill), and the whole operation 188 instead of 199 without the transform.
3. **Additions and subtractions equal rule K's as live lanes everywhere; the computed lanes exceed them where packed instructions have an unused half.** GCC's `transform(point, motor)` computes 44 lanes for 36 (4 `addpd` and 4 `subpd` lanes no result reads), Clang's `transform(line, motor)` 60 for 58 and `transform(plane, motor)` 42 for 36. A dead lane costs no instruction of its own: the packed instruction does a needed sum in its other lane.
4. **Negations differ from rule K's in both directions** (transforms 1, 4, 2 against 3, 9, 5; `compose` 7 against 4). Rule K counts a negation where the kernel's term order puts one; a compiler may write x + (−y) as x − y and −(a·b) as (−a)·b, both exact, and negate a coefficient once for several products; in `compose` the conversion `Motor{...}` from the kernel's layout (e14, e13) to the preset's (e41, e31) adds sign flips. The counts above are negated lanes that a result reads; Clang also negates some lanes no result reads (the unused half of an `xorpd`). Not traced term by term (departure 4).
5. **Vectorization differs between the compilers and from WU13.** GCC packs part of point ∧ point and `transform(point, motor)`; Clang packs part of the line and plane transforms and `compose`. Against WU13's read (headers of `1ca4508`, its own types), Clang's `transform(point, motor)` is now all scalar (WU13: 28 `mulsd`, 10 `mulpd`) and `transform(plane, motor)` partly packed (WU13: 48 `mulsd`); the totals are the same. The cause of that change was not looked for.

## Calls found (recorded, not fixed)

DoD-24 asks for no calls. GCC has none in any probe function. Clang has these, each recorded as a departure from DoD-24 under the owner's ruling (decision 0233 item 3):

| Operation | What Clang emits | Instruction-level context |
| --- | --- | --- |
| transform(point, motor), transform(plane, motor) | The probe function is one `jmp` to an out-of-line `galoot::core::transform` instance (148 and 149 instructions) | A tail jump: no frame, no stack buffer; the instance itself is straight-line |
| transform(line, motor) | `jmp` to `transform`, which `call`s the first stage's kernel `run_product<first_kernel>` (118 instructions, 36 products) | 251 instructions in all against 236 with the kernel inlined (threshold 2000): the call and return, a 72-byte frame with the first stage's eight results passed through it, and the stack protector's load, store, compare and branch; the inlined form also loses the five `mulpd` |
| compose(motor, motor) | The probe function `call`s `run_product<product_kernel>` (167 instructions), then reorders its eight results into `Motor`'s layout with four sign flips (32 instructions with the stack protector's check) | 199 instructions against 166 inlined: the call, the eight results through a buffer in an 80-byte frame and back, the check, and the padding of finding 2 |

### Calls in the benchmark programs

In the benchmark programs the inliner sees larger callers (the timed loops of both forms in one function), and more stays out of line. Per program, the calls inside the timed loops (from `out/count.md`, its "Benchmark programs" section; the loops were read in the Clang line program):

| Program | GCC 16: Galoot form | GCC 16: reference form | Clang 23: Galoot form | Clang 23: reference form |
| --- | --- | --- | --- | --- |
| point ∧ point, point ∧ point ∧ point, line ∨ plane | inline | inline | inline | inline |
| transform(point, motor) | inline | call `reference_kernels::transform_point` (157 instructions) | call `transform` (148) | call `reference_kernels::transform_point` (156) |
| transform(line, motor) | inline | call `reference_kernels::transform_line` (219) | call `transform` (250 with its call to `run_product<first_kernel>`) | call `reference_kernels::transform_line` (240) |
| transform(plane, motor) | inline | call `reference_kernels::transform_plane` (145) | call `transform` (149) | call `reference_kernels::transform_plane` (148) |
| compose(motor, motor) | call `run_product<product_kernel>` (159) | call `reference_kernels::compose_motors` (159) | call `run_product<product_kernel>` (167, with the padding lane) | call `reference_kernels::compose_motors` (137: 24 `mulpd`, 52 add/sub lanes) |

`operator new` is called only where the programs draw their operand pools and parse their arguments, outside the timed loops. So E2 compares, for the transforms on GCC, an inlined Galoot kernel with a called reference kernel; on Clang and for `compose` on both, a call with a call. E2's reading of any difference has to take this table with it.

## Comparison with rule K (summary)

| Count | Agrees with rule K | Differs, and why |
| --- | --- | --- |
| Multiplications | 13 of 14 (operation, compiler) pairs exactly | Clang `compose`: one padding lane (finding 2) |
| Additions and subtractions, live | all 14 | none; computed lanes exceed them in 3 pairs by unused packed halves (finding 3), and in Clang's `compose` by the padding lane |
| Negations | products and `line ∨ plane`, both compilers | transforms fewer, `compose` more: signs moved by the compiler and by the `Motor` conversion (finding 4) |
| Calls, branches on blade data, allocations | GCC: none of any; Clang: no branches, no allocations | Clang's calls above |

## Files

- `probe.cpp`: the probe unit.
- `probe.gcc.s`, `probe.clang.s`: its assembly at `18849b5`.
- `run.sh`: every compile, count and trace of this record.
- `count.py`, `trace.py`: the instruction counts and the symbolic trace (standard library only, run as `python3 -I`).
- `out/count.md`, `out/trace.md`, `out/trace-variants.md`: their output.
- `runs.tsv`: this campaign's run log.
