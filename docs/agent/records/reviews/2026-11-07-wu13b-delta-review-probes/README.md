# Probes of the WU13b delta review

These are the probes behind `../2026-11-07-wu13b-delta-review.md`, written and run by the reviewer (Claude, agent, Opus 5.5) against `c98eee2`. Each C++ probe is built from the repository root with `-std=c++23 -O2 -ffp-contract=off -Iinclude`.

| File | What it shows | How to run |
| --- | --- | --- |
| `sim_tau.py` | `../2026-11-07-wu13b-review-probes/sim.py` with the padding λ/u. Run against kernels dumped by that directory's `dump.cpp` from `c98eee2`, it reproduces the core bit for bit, and \|d − e\| ≤ tol holds for 6,600 inputs per seed | `python3 -I sim_tau.py kernels.jsonl <seed> 600`; outputs in `sim-seed101.txt` to `sim-seed404.txt` |
| `floor.cpp` | The smallest input scale at which a gross non-versor is still detected, with padding λ and with λ/u | `-DGALOOT_CHECKING=0`, against the headers of `43355bb` and of `c98eee2` |
| `gate.cpp` | Which scalars and transforms the gated check covers | `-O1 -DGALOOT_CHECKING=1`, on both compilers |
| `bf_rand.cpp` | The mutant `gate_removed` false-aborts on random bfloat16 inputs (GCC) | `-O1 -DGALOOT_CHECKING=1`, against a header copy with the mutant applied |
| `mutants.py`, `mutants-out.jsonl` | The delta mutants and their results | `python3 -I mutants.py gcc-debug clang-debug`, with `MAKEFLAGS` from `tools/jobpool env`; `ROOT` names the delta worktree |
