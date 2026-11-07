# The padding λ/u, checked by the review's simulation

After the WU13b blind review (finding N2), the padding of transform's magnitude became λ/u in place of λ. Decision 0180's addendum re-derives Lemmas A to C for it; this is the numerical check, by the WU13b implementer (Claude, agent, Opus 5.5).

- `kernels.jsonl`: the first and dropped kernels, the counts K, and a probe input's dropped values and tolerances of the 11 transforms of the reviewer's `records/reviews/2026-11-07-wu13b-review-probes/dump.cpp`, built against the header with the λ/u padding (`-std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0`, g++ 16.2.1). The header was the one committed as the rounding check's code after the review; it was built from the working tree before that commit, and is byte-identical to it.
- The reviewer's `sim.py`, run with one change, the default padding of its `tolerance` function: `pad=LAM / U` in place of `pad=LAM`. Command: `python3 -I sim.py kernels.jsonl <seed> 600`, seeds 7 (the review's) and 11. Outputs `seed7.txt` and `seed11.txt`; rows `review-sim:*` in `runs.tsv`.

Results, both seeds:

- The simulation equals the core bit for bit on the probe inputs of all 11 transforms, dropped values and tolerances, so it computes the new padding as the header does.
- |d − e| ≤ tol holds for every dropped coefficient of 6,600 inputs per seed in six regimes (ordinary, mixed scales from 2^−400 to 2^300, deep underflow, underflow with 30% zeros, one huge coefficient among tiny ones, near overflow), for any input, not only versors: 0 violations. The largest |d − e| / tol is 0.287 (seed 7) and 0.318 (seed 11). With the padding λ, the review reported 0.287 at seed 7.
- R9's additive bound γ_K M + K · 2^−1074 fails as before (5,219 and 5,202 comparisons).

A sampled check (V9) of what the derivation proves.
