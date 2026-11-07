# Probes of the WU13b blind review

These are the probes behind the findings of `../2026-11-07-wu13b-review.md`. They were written and run by the reviewer (Claude, agent, Opus 5.5) at `43355bb`, with g++ 16.2.1, clang++ 23.1.4, and Python 3 on the development machine, an i9-14900K. Their runs are rows `review:*`, `probe`, and `mutant:*` in `runs.tsv`. Every C++ probe is built from the repository root with `-std=c++23 -O2 -ffp-contract=off -Iinclude`, and with `-Itest` where it includes test support. Each also gets `-DGALOOT_CHECKING=0`, except where the table says otherwise.

| File | What it shows | How to run |
| --- | --- | --- |
| `dump.cpp`, `sim.py` | The kernels of 11 transforms, simulated over IEEE doubles and exact Fractions; the core's tolerance reproduced bit for bit; \|d − e\| ≤ tol over 6,600 inputs in six regimes; R9's additive bound violated | `dump > kernels.jsonl`, then `python3 -I sim.py kernels.jsonl 7 600` |
| `drift.cpp` | The rate at which the check fires for chains of N composed motors (N1) | `drift 20261107` |
| `drift_dump.cpp`, `drift_check.py` | 30 firing cases at N = 16, recomputed exactly: e ≠ 0 and \|d − e\| ≤ 0.055 tol | `drift_dump > drift_cases.json`, then `python3 -I drift_check.py drift_cases.json <dir of simlib.py>`; `simlib.py` is `sim.py` up to its line `kernels = [json.loads` |
| `rounded_once.cpp` | Exact rational versors rounded once never fire; the same versors composed in double do | `rounded_once` |
| `cost2.cpp` | Checking mode's cycles on inputs with zeros and on inputs without them (N2); counted with `perf stat -e cpu_core/assists.fp/`, pinned with `taskset -c 4` under the timing lock | `-DDENSE=0` or `1`, and `-DGALOOT_CHECKING=0` or `1`; `cost2 10000000 1` |
| `f16.cpp`, `bf16_use.cpp` | float16 holds the check; bfloat16 no longer compiles (N5) | compile only; `bf16_use.cpp` with `-c`, against the headers of `01835b7` and of `43355bb` |
| `mutants.py` | The reviewer's 15 mutants | `python3 -I mutants.py gcc-debug clang-debug`, with `MAKEFLAGS` set from `tools/jobpool env` |

B1's reproduction is `test/core/transform_rounding_assert_fail.cpp`, built with `-O2 -ffinite-math-only -DGALOOT_CHECKING=1` and run with `infinite`, `overflow`, `nan`, and `not_a_versor_double`.
