# Spike E, part F: benchmarks of the transform's forms

Plan 0038 part F (section 4 item 8), performance-engineer half, run by Claude (agent, Opus 5.5) on 2026-11-10 in the worktree `.claude/worktrees/spike-e--bench`, branch `research/spike-e--bench` from `develop` at `d0bbe49`. Recorded, never asserted (decision 0013). The protocol below was committed (`00d26b9`) before the first timed process; the results follow it.

## Departures, skipped checks, and ambiguities (C20)

1. **Branch base and harness.** The brief says to branch from `develop` and use part D's harness, which is on `feature/cost-evidence` (`2de0d36`), not on `develop`. `include/` is identical on the two (`git diff develop feature/cost-evidence -- include/` is empty), so the branch is from `develop` and the harness here, `harness.hpp`, is written from part D's `test/bench/harness.hpp` at `2de0d36` (same draws, unitized motors, checksum, steady clock, pair method), extended with forms, a batch loop, and a double-precision `check`. `drive.py` and `batch.sh` are E2's from `records/cost-evidence-measurements` at `526bba7`, extended the same way. The programs are compiled by `build.sh` with the flags the measure presets give a test source, not by CMake. Nothing under `include/` or `test/` changed.
2. **The library calls `transform` out of line in these programs, on both compilers.** In each program, which holds five forms, GCC and Clang keep `galoot::core::transform` out of line: 1 call per transform on GCC, 1 (2 for the line on Clang) on Clang (`out/stalls.tsv`). In E2's one-form programs GCC inlined it. So the `galoot` rows measure the library as it compiles in such a program, call included, and form `a` (the same terms in the same order, the same bits: `out/check.*.txt` shows a difference of 0) is the call-free baseline the brief asks for. Every gain below is stated against `a` first and against `galoot` second. That the library's inlining depends on what else the translation unit holds is a finding for the core inlining unit of decision 0238 item 1.
3. **The clock moved between rounds; a reading in cycles is added.** CPU 4 ran at about 5.70 GHz in rounds 3 to 7 of size 0 and at 5.9 to 6.0 GHz otherwise (cycles over nanoseconds of the long runs), so the nanosecond ranges are about 5% of the medians in nearly every row while the cycle ranges are about 0.2%, and the size-0 medians in nanoseconds are about 4% above those of the other two sizes for that reason, not for layout. No processor setting was touched. The criterion was applied to nanoseconds, as fixed, and also to cycles per transform, a reading chosen after seeing the nanosecond ranges and labeled as such (`out/tables.md`, "The criterion, pair by pair"). The tables below give cycles first.
4. **One unexplained run.** GCC transform(line) throughput, form `bp`, size 0, round 9: 74.9 cycles per transform against 55.5 in all 32 other runs of that form and loop, with the same instructions and no busy time on the sibling or the CPU. Not traced. It sets that row's range (22.1 cycles) and so its verdicts.
5. **Busy rule.** 54 of the 3,810 pairs were repeated, and 1 was busy again (used and marked). Long runs took 0.14 to 2 s, so 2 ticks were from 1% to 13% of a pair's wall time, as the protocol says.
6. **Load.** The one-minute load average was 1.1 to 1.9 throughout (other users of the machine), never below 1, so no energy figure is attributed. The pool was not running at the start; it was started with `tools/jobpool ensure`, showed 23 of 23 free at every reading until it stopped itself after 30 idle minutes during the size-1352 batch, and was started again with `ensure` for the compile-cost runs.
7. **Revision.** Every row reads `00d26b9258af-dirty`: the committed sources at `00d26b9`, with the campaign's own log as the uncommitted change. `build.sh` was rerun at `96ac07c` after the prototype's compaction step took its summary as a reference (for `../proto/cost/`); the probe's assembly came out byte for byte the same, so the timed code is the same. After all measurements the hand-written C++ here and in `../proto/` was run through `clang-format` (layout only); `build.sh` was rerun once more and the probe's assembly, `out/asm-counts.md`, and both checks came out the same.
8. **Sweep.** A reduced protocol, stated before the run: one environment size, five rounds, four forms.
9. **Form C's arrangement** is "motor doubled first" (`../counts.md`), which has a motor-only part for a batch; "factor 2 on results" was not timed.
10. **Assembly counts** (`out/asm-counts.md`) are computed lanes from the `.s` files (a packed instruction counts two), not E1's live lanes from a symbolic trace, so a dead lane of a packed instruction counts. GCC folded the probe's `plane_c_setup` into a call of the identical `line_c_setup`; that is the probe only, not the timed loops (0 calls there).
11. **Not done:** no knowledge-base note; no symbolic trace of the assembly; the cause of GCC's slower `bp` in the plane's batch loop (2 store-forward blocks per transform against 1 for `b`) and of Clang's in the plane's per-call loops was not traced.

## The protocol, fixed before the first timed process

- **Forms**, for transform(point, motor), transform(line, motor), transform(plane, motor) in rigid(3), over double, each a struct of two always-inlined functions, `setup(q)` (what reads only the motor) and `apply(m, q, x)`:
  - `galoot`: `galoot::core::transform` itself, as the library is at `d0bbe49`.
  - `a`: form A (the library's restricted terms, the same terms in the same order, signs carried into subtractions) as straight-line code, generated from `../forms.py` by `gen_forms.py` into `forms.hpp`.
  - `b`: form B, collected, from `../forms.py`'s `form_B(grouped=True)`, the form whose counts are `../counts.md`'s B rows.
  - `bp`: form B as `../proto/collect.hpp` derives it at compile time from the library's two kernels (the collection-pass prototype), run by always-inlined fold expressions.
  - `c`: form C, "motor doubled first" (`../forms.py`'s `form_C_*(True)`), which assumes |v|² + s² = 1 and v · m + s t = 0.
- **Loops** (`harness.hpp`): `throughput` (independent calls), `latency` (each result is the next call's x), and `batch` (setup once per motor, then 1,024 objects under it). A call of a form is `apply(setup(q), q, x)`, so per call `b` and `bp` pay their motor work every time.
- **Inputs:** 128 motors and 128 objects drawn from seed 20261110 at run time (motors unitized as part D's harness does, so form C applies to them up to rounding), a batch's 1,024 objects drawn after them; every result added as bits into a printed checksum.
- **Build:** `build.sh`, g++ 16.2.1 and clang++ 23.1.4 with libstdc++ 16, the flags CMake gives a test source under the `gcc-measure` and `clang-measure` presets (`-DGALOOT_CHECKING=0 -Iinclude -Itest -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -O2 -std=c++23`), linked with lld, every compile through `tools/timed` into `runs.tsv` here.
- **Method:** E2's (`records/performance/wu17-bench/` on `records/cost-evidence-measurements`, `drive.py` there): every process `taskset -c 4 tools/timed --log out/runs.tsv bench <preset> spike-e-bench:... -- env -i [BENCH_ENV_PAD=...] <program> <form> <loop> <passes> 20261110 [1024]`; CPU 4 pinned with its sibling CPU 5 left idle by this work; pairs of processes at 1/8 and all of the passes, instructions and cycles per transform from the pair's difference; eleven rounds per environment size, interleaved, compilers and forms reversed on every other round; environment sizes 0, 1352, 2768; the lock taken and released per batch with `lock.sh` by `batch.sh`, the pool and load read before and after.
- **Passes** (`drive.py`'s `PASSES`, from `out/calibration.txt`): long runs of 0.5 s for the slowest (compiler, form) of each (operation, loop), the same for every form and compiler; the batch loop four times that (its fastest form would otherwise run 0.035 s). So long runs take from about 0.14 s to 2 s.
- **Busy rule, in ticks** (decision 0237 item 4): a pair whose sibling thread's busy time, or its CPU's busy time beyond its own, summed over its two processes, is more than 2 ticks (0.02 s) is repeated once, both kept, the later used and marked if busy again. For the pair lengths above, 2 ticks is from 1% to 13% of a pair's wall time.
- **Criterion** (E2's): a difference is called when the medians differ by more than twice the larger of the two ranges, with the same form faster, at every environment size (strict) or at the smallest with the same order at all three (weaker). Compared pairs: `drive.py`'s `COMPARED`.
- **Break-even sweep** (a reduced protocol): the batch loop at 1, 2, 3, 4, 6, 8, 16 objects per motor for `galoot`, `a`, `b`, `c`, environment size 0 only, five rounds, a quarter of the batch loop's transforms at each size.
- **Diagnostic** (`drive.py stalls`): per form and loop, calls, store-forward blocks, scalar and packed double arithmetic retired per transform, under `perf stat`, not through `tools/timed`.
- **Correctness before timing:** `check.cpp` (exact, over the test rational) and each program's `check` (double, relative difference from `galoot` at most 1e-12) must pass on both compilers.

## What ran

- `build.sh` at `00d26b9`: `out/check.gcc.txt` and `out/check.clang.txt` show the exact check (`check.cpp`) passing (forms `a`, `b`, `bp` equal the sandwich for 200 random and 200 unit motors per object, `c` for every unit motor and for none of the random ones) and every form within 2.6e-15 of `galoot` over double.
- Commands, from the worktree's root, with the pool's environment set:

```bash
bash docs/agent/records/spikes/spike-e/bench/build.sh
docs/agent/records/spikes/spike-e/bench/batch.sh calibrate
docs/agent/records/spikes/spike-e/bench/batch.sh run 0
docs/agent/records/spikes/spike-e/bench/batch.sh run 1352
docs/agent/records/spikes/spike-e/bench/batch.sh run 2768
docs/agent/records/spikes/spike-e/bench/batch.sh sweep
docs/agent/records/spikes/spike-e/bench/batch.sh stalls
python3 -I docs/agent/records/spikes/spike-e/bench/drive.py summarize docs/agent/records/spikes/spike-e/bench/out
python3 -I docs/agent/records/spikes/spike-e/bench/drive.py summarize docs/agent/records/spikes/spike-e/bench/out/sweep
python3 -I docs/agent/records/spikes/spike-e/bench/analyze.py docs/agent/records/spikes/spike-e/bench
```

- Lock held (UTC, 2026-11-11): calibration 00:24:37 to 00:24:51; size 0 00:26:57 to 00:39:59; size 1352 00:39:59 to 00:52:27; size 2768 00:52:27 to 01:04:52; sweep 01:04:52 to 01:11:25; diagnostic 01:11:25 to 01:12:26 (`out/protocol.log`).
- Toolchain and machine: g++ 16.2.1 20260810, clang++ 23.1.4, libstdc++ 16, Intel Core i9-14900K, CPU 4 (a performance core), x86-64 baseline target (SSE2).

## Results

Per transform, median over 11 rounds at environment size 0: cycles (nanoseconds, retired instructions). Cycle ranges are at most 2.6 (Clang's `galoot` plane rows) except in the row of departure 4; every median, range, and the other two sizes are in `out/tables.md` and `out/results.tsv`.

#### GCC 16

| Operation | Loop | galoot | a | b | bp | c |
| --- | --- | --- | --- | --- | --- | --- |
| point | throughput | 50.7 (8.91 ns, 209 i) | 43.4 (7.63 ns, 151 i) | 37.3 (6.55 ns, 148 i) | 42.5 (7.42 ns, 149 i) | 34.4 (6.05 ns, 88 i) |
| point | latency | 88.8 (15.57 ns, 204 i) | 54.8 (9.63 ns, 148 i) | 56.8 (9.98 ns, 144 i) | 54.4 (9.56 ns, 141 i) | 31.8 (5.59 ns, 79 i) |
| point | batch | 49.8 (8.73 ns, 204 i) | 55.4 (9.73 ns, 147 i) | 26.2 (4.60 ns, 43 i) | 29.3 (5.15 ns, 48 i) | 31.2 (5.47 ns, 73 i) |
| line | throughput | 57.5 (10.10 ns, 266 i) | 48.7 (8.54 ns, 227 i) | 67.2 (11.79 ns, 245 i) | 55.5 (9.75 ns, 256 i) | 63.1 (11.08 ns, 170 i) |
| line | latency | 85.0 (14.91 ns, 260 i) | 70.5 (12.38 ns, 222 i) | 75.9 (13.32 ns, 257 i) | 74.6 (13.10 ns, 255 i) | 65.5 (11.50 ns, 172 i) |
| line | batch | 56.8 (9.95 ns, 260 i) | 68.4 (12.01 ns, 229 i) | 52.1 (9.14 ns, 93 i) | 52.0 (9.12 ns, 94 i) | 55.4 (9.72 ns, 107 i) |
| plane | throughput | 47.2 (8.27 ns, 180 i) | 48.9 (8.56 ns, 151 i) | 41.5 (7.29 ns, 147 i) | 38.9 (6.83 ns, 145 i) | 36.5 (6.40 ns, 109 i) |
| plane | latency | 75.0 (13.18 ns, 175 i) | 56.2 (9.87 ns, 146 i) | 40.3 (7.07 ns, 136 i) | 40.2 (7.06 ns, 136 i) | 47.2 (8.27 ns, 105 i) |
| plane | batch | 46.2 (8.10 ns, 175 i) | 48.1 (8.44 ns, 145 i) | 28.0 (4.91 ns, 49 i) | 41.5 (7.29 ns, 48 i) | 45.0 (7.90 ns, 65 i) |

#### Clang 23

| Operation | Loop | galoot | a | b | bp | c |
| --- | --- | --- | --- | --- | --- | --- |
| point | throughput | 50.0 (8.54 ns, 189 i) | 33.7 (5.78 ns, 166 i) | 32.1 (5.47 ns, 147 i) | 32.0 (5.41 ns, 135 i) | 20.3 (3.44 ns, 83 i) |
| point | latency | 48.9 (8.28 ns, 181 i) | 51.7 (8.75 ns, 161 i) | 31.3 (5.40 ns, 140 i) | 28.5 (4.90 ns, 148 i) | 28.8 (4.87 ns, 76 i) |
| point | batch | 49.0 (8.26 ns, 185 i) | 16.2 (2.75 ns, 73 i) | 5.2 (0.89 ns, 23 i) | 5.2 (0.89 ns, 23 i) | 8.7 (1.48 ns, 37 i) |
| line | throughput | 63.6 (10.86 ns, 298 i) | 47.0 (8.04 ns, 233 i) | 50.8 (8.71 ns, 247 i) | 51.9 (8.89 ns, 246 i) | 35.4 (6.05 ns, 170 i) |
| line | latency | 82.7 (14.52 ns, 289 i) | 65.6 (11.52 ns, 244 i) | 54.8 (9.27 ns, 222 i) | 54.1 (9.17 ns, 215 i) | 36.7 (6.30 ns, 179 i) |
| line | batch | 63.0 (11.06 ns, 292 i) | 25.5 (4.42 ns, 119 i) | 10.2 (1.78 ns, 52 i) | 10.2 (1.78 ns, 52 i) | 14.4 (2.52 ns, 65 i) |
| plane | throughput | 77.3 (13.57 ns, 190 i) | 31.9 (5.60 ns, 152 i) | 27.9 (4.91 ns, 129 i) | 33.1 (5.82 ns, 130 i) | 23.4 (4.12 ns, 107 i) |
| plane | latency | 70.7 (12.42 ns, 182 i) | 37.5 (6.58 ns, 149 i) | 28.1 (4.93 ns, 123 i) | 34.0 (5.98 ns, 140 i) | 25.9 (4.55 ns, 105 i) |
| plane | batch | 77.5 (13.58 ns, 186 i) | 15.9 (2.79 ns, 76 i) | 5.2 (0.92 ns, 23 i) | 5.2 (0.91 ns, 24 i) | 7.6 (1.29 ns, 32 i) |

Operation counts (rule S, `../counts.md`): per call, `a` 48 mul and 36 add (point and plane) or 72 and 58 (line); `b` 35, 35, 9 doublings (point, plane) or 53, 62, 15 (line); `c` 22, 18, 4 (point), 48, 39, 7 (line), 30, 24, 7 (plane). Per object of a batch, `b` 13 mul and 9 add (point, plane) or 27 and 21 (line). `out/stalls.tsv` shows the retired double arithmetic equal to these counts where a loop is not vectorized: GCC's per-call line retires 130 scalar lanes in `a` (72 + 58) and in `b` (53 + 62 + 15), so collection removes no arithmetic per call for the line, only moves it from products to sums. In the batch loop Clang vectorizes every inline form across objects (packed double only), and neither compiler vectorizes `galoot`, which calls.

### Sweep: cycles per transform against objects per motor

The batch loop, size 0, median of five rounds (`out/tables.md`):

| Compiler | Operation | Form | 1 | 2 | 3 | 4 | 6 | 8 | 16 | 1024 (run) |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| gcc | point | galoot | 50.6 | 50.2 | 50.1 | 50.0 | 49.9 | 49.9 | 49.8 | 49.8 |
| gcc | point | a | 55.7 | 55.7 | 55.5 | 55.6 | 55.5 | 55.5 | 55.5 | 55.4 |
| gcc | point | b | 40.8 | 33.9 | 31.5 | 30.2 | 28.7 | 28.0 | 27.1 | 26.2 |
| gcc | point | c | 34.4 | 33.8 | 33.6 | 32.5 | 31.6 | 31.5 | 31.4 | 31.2 |
| gcc | line | galoot | 57.6 | 57.0 | 56.9 | 56.9 | 56.9 | 56.8 | 56.8 | 56.8 |
| gcc | line | a | 70.0 | 69.3 | 69.1 | 68.9 | 68.7 | 68.7 | 68.6 | 68.4 |
| gcc | line | b | 73.9 | 63.6 | 59.6 | 57.7 | 55.7 | 54.8 | 53.5 | 52.1 |
| gcc | line | c | 63.1 | 59.4 | 58.0 | 57.6 | 56.6 | 56.2 | 55.6 | 55.4 |
| gcc | plane | galoot | 47.0 | 46.6 | 46.5 | 46.4 | 46.3 | 46.3 | 46.3 | 46.2 |
| gcc | plane | a | 49.7 | 49.2 | 48.9 | 48.6 | 48.4 | 48.4 | 48.2 | 48.1 |
| gcc | plane | b | 42.6 | 35.4 | 33.4 | 32.2 | 30.6 | 30.2 | 29.1 | 28.0 |
| gcc | plane | c | 52.3 | 49.4 | 47.2 | 47.0 | 46.5 | 46.1 | 45.6 | 45.0 |
| clang | point | galoot | 50.0 | 49.5 | 49.2 | 49.2 | 49.1 | 49.1 | 49.0 | 49.0 |
| clang | point | a | 33.2 | 21.1 | 26.2 | 19.3 | 19.4 | 18.1 | 17.3 | 16.2 |
| clang | point | b | 33.7 | 22.0 | 19.4 | 15.7 | 14.2 | 11.5 | 8.5 | 5.2 |
| clang | point | c | 21.8 | 14.3 | 16.6 | 12.9 | 14.0 | 11.1 | 10.0 | 8.7 |
| clang | line | galoot | 64.3 | 63.6 | 63.4 | 63.3 | 63.2 | 63.2 | 63.2 | 63.0 |
| clang | line | a | 49.6 | 39.4 | 38.9 | 30.2 | 28.4 | 27.5 | 26.1 | 25.5 |
| clang | line | b | 55.0 | 37.0 | 29.3 | 28.0 | 23.7 | 19.0 | 14.8 | 10.2 |
| clang | line | c | 37.4 | 23.3 | 27.1 | 21.4 | 18.1 | 18.2 | 16.2 | 14.4 |
| clang | plane | galoot | 78.0 | 79.7 | 77.6 | 77.6 | 77.7 | 77.7 | 79.1 | 77.5 |
| clang | plane | a | 38.8 | 21.1 | 26.4 | 19.3 | 19.8 | 17.9 | 17.1 | 15.9 |
| clang | plane | b | 28.8 | 19.7 | 17.1 | 14.2 | 15.5 | 12.4 | 8.8 | 5.2 |
| clang | plane | c | 24.8 | 15.6 | 15.5 | 12.1 | 12.8 | 10.9 | 9.5 | 7.6 |

Break-even: the smallest batch size from which the first form takes fewer cycles per transform (medians) than the second at that size and every larger one, the run's 1,024 included.

| Compiler | Operation | b against a | b against galoot | c against a | b against c |
| --- | --- | --- | --- | --- | --- |
| gcc | point | 1 | 1 | 1 | 3 |
| gcc | line | 2 | 6 | 1 | 6 |
| gcc | plane | 1 | 1 | 3 | 1 |
| clang | point | 3 | 1 | 1 | 16 |
| clang | line | 2 | 1 | 1 | 16 |
| clang | plane | 1 | 1 | 1 | 16 |


## For research note 0027

Measured, in cycles per transform at environment size 0, ratio of the first form to the second (below 1 is faster); "beyond" when the criterion holds at all three sizes, in cycles and in nanoseconds alike unless stated (`out/tables.md` has every pair).

**Gain of collection per call (B against A inline):**

| | point thr. | point lat. | line thr. | line lat. | plane thr. | plane lat. |
| --- | --- | --- | --- | --- | --- | --- |
| GCC, `b` / `a` | 0.86 beyond | 1.04 (slower; beyond in cycles only) | 1.38 (slower) beyond | 1.08 (slower; cycles only) | 0.85 beyond | 0.72 beyond |
| Clang, `b` / `a` | 0.95 (cycles only) | 0.61 beyond | 1.08 (slower; cycles only) | 0.84 beyond | 0.88 beyond | 0.75 beyond |

So per call collection gains 5% to 39% in 7 of the 8 point and plane loops (GCC's point latency is 4% slower), and loses for the line on GCC in both loops and on Clang's throughput loop: it trades 19 products for 19 more sums and doublings there, at a longer dependence chain. Against the library as compiled here (`galoot`, with its call), `b` is faster in every per-call loop but GCC's line throughput (1.17).

**Gain per batch of 1,024 under one motor:**

| | point | line | plane |
| --- | --- | --- | --- |
| GCC, `b` / `a` | 0.47 | 0.76 | 0.58 |
| Clang, `b` / `a` | 0.32 | 0.40 | 0.33 |
| GCC, `b` / `galoot` | 0.53 | 0.92 | 0.61 |
| Clang, `b` / `galoot` | 0.11 | 0.16 | 0.07 |

All beyond the criterion, except GCC's line against `galoot`, which is beyond in cycles only. GCC does not vectorize the batch (`b` retires 14 scalar and 4 packed lanes per point); Clang does, two objects per instruction, which is most of the larger gain on Clang. Per object, `b` costs 26 (GCC) or 5.2 (Clang) cycles per point against `a`'s 55 or 16.

**Break-even batch size** (the smallest number of objects per motor from which `b` takes fewer cycles than the other form at that size and every larger one, from the sweep): against `a`, GCC 1, 2, 1 and Clang 3, 2, 1 (point, line, plane); against `galoot`, GCC 1, 6, 1 and Clang 1, 1, 1. At one object per motor the batch loop is a per-call loop with the motor work hoisted by hand, so "1" means `b` already wins per call there. Clang's `a` is uneven at small sizes (21.1 cycles at 2 objects, 26.2 at 3) because it vectorizes pairs of objects; the break-even of 3 for the point is that. Counted in `../counts.md`'s units, a point under `b` costs 79 operations per call against `a`'s 84, so `b` is cheaper from 1 object by mul + add + doublings; in multiply-adds with doublings (61 per call, 13 per object, against 48) it is cheaper from 2, the figure decision 0234 item 6 gives. Measured, it is 1 to 3.

**Form C against form B:** per call, `c` is faster than `b` in 11 of the 12 loops (median ratios 0.56 to 0.94; GCC's plane latency is 1.17, slower), and against `a` it gains 7% to 44% except GCC's line throughput (1.30, slower). Per batch it loses to `b` everywhere (GCC 1.19, 1.06, 1.61; Clang 1.67, 1.42, 1.46), because its object part (22 mul + 18 add for a point) is larger than `b`'s matrix product (13 + 9). From the sweep, `b` overtakes `c` from GCC 3, 6, 1 and Clang 16, 16, 16 objects per motor, against about 5 in the multiply-add count of decision 0234 item 6. So the constraint form is the per-call choice and the collected form the batch choice, and C's gain over B exists only for small batches.

**The prototype's form against the hand-generated one (`bp` / `b`):** within the criterion in 14 of the 18 (compiler, operation, loop) cells in nanoseconds; beyond it, `bp` is slower in GCC's point throughput (1.13), GCC's plane batch (1.48: 41.5 cycles against 28.0, with 2 store-forward blocks per transform against 1), and Clang's plane throughput and latency (1.18, 1.21), and faster nowhere in nanoseconds (in cycles, faster in GCC's point latency and plane throughput and in Clang's point and line latency, slower also in GCC's point batch and Clang's line throughput). Its arithmetic is the same but for one more addition per motor (`../proto/README.md`); the differences are code generation of the fold expressions and were not traced.

**Inferred, not measured:** that a core collected plan would time like `bp` (it would be the same pass and the same runner, but in the library's kernels, whose inlining depends on the translation unit, departure 2); that the core inlining unit of decision 0238 would bring `galoot` to `a`; any figure for other machines or for `-O3`.

## Files

- `gen_forms.py` writes `forms.hpp` (forms `a`, `b`, `c` from `../forms.py`; checks their counts against `../counts.json`; `--check` diffs).
- `harness.hpp`, `wrappers.hpp`, `transform_{point,line,plane}.cpp`: the programs. `check.cpp`: the exact check. `probe.cpp`, `probe.{gcc,clang}.s`, `count_asm.py`, `out/asm-counts.md`: the static counts.
- `build.sh`, `drive.py`, `batch.sh`, `analyze.py`: build, run, summarize, tabulate.
- `runs.tsv`: the build and check runs. `out/trial-runs.tsv`: trial builds before the protocol commit (uncommitted tree). `out/calibration.txt`, `out/calibration/runs.tsv`: calibration. `out/rows.tsv`, `out/runs.tsv`: every timed process. `out/results.tsv`, `out/comparison.tsv`, `out/verdict.tsv`: drive.py's summaries. `out/sweep/`: the sweep. `out/stalls.tsv`: the diagnostic. `out/tables.md`: analyze.py's tables. `out/protocol.log`: the readings.

Verification: `check.cpp` (exact) and the programs' `check` (double) on both compilers, at the measured revision; within each form, every pair of the same work gave the same checksum in every round and at every size (`drive.py summarize`). Validation: none applies.
