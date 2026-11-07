# Why composed flat-plane motors trip transform's rounding check

The probes behind research note `../../../research-notes/0022-flat-plane-chain-rounding.md`, written and run by Claude (agent, Opus 5.5, numerical-expert role) on 2026-11-07, against the headers of `d68881f` (unchanged), with g++ 16.2.1, clang++ 23.1.4, Python 3.14.9, and SymPy 1.14.0 for `identity.py` alone. Their runs are the rows `probe:o6:*` in `../../performance/runs.tsv`.

| File | What it does |
| --- | --- |
| `chains.cpp` | `chains <seed>`: the chains of the WU13b review probe `../../reviews/2026-11-07-wu13b-review-probes/drift.cpp`, same draws in the same order, every case dumped (V, x, the dropped coefficients d, their tolerances); `steps <seed> <trials>`: chains of 128 motors with every factor and partial product dumped; `renorm <seed>`: the flat chains of `chains` with the projection `renormalize()` applied once at the end or after every composition |
| `cga.py` | Exact conformal(3) antiproducts over fractions, from `test/reference/clifford_reference.py` and the complements of CONVENTIONS 4.2, a route the library does not take |
| `analyze.py` | Every case of `chains` recomputed exactly: the exact dropped part e against d and the tolerance, the defect of V̰ ⟇ V, and the identities of the flat case |
| `steps.py` | The defect along each chain of `steps`, its split into the motors' own defects and what each composition's rounding adds, and an exact continuation of 20 chains per family |
| `renorm_check.py` | The defect `renormalize()` leaves, exactly, for every flat chain of `chains` |
| `identity.py` | The polynomial identities F1 to F6 and H1 of the note, proved symbolically |
| `out/` | The outputs below, and the SHA-256 of the two dumps |

## How to run

From the worktree root, with the job pool's environment (`eval "$(tools/jobpool env)"`), `B` a scratch directory:

```
tools/timed build - probe:o6:chains -- g++ -std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0 -Iinclude docs/agent/records/spikes/flat-plane-chain-rounding/chains.cpp -o $B/chains
tools/timed build - probe:o6:chains-clang -- clang++ -std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0 -Iinclude docs/agent/records/spikes/flat-plane-chain-rounding/chains.cpp -o $B/chains-clang
$B/chains chains 20261107 > $B/chains.jsonl          # 14,002 lines; sha256 in out/dumps.sha256
$B/chains steps 20261107 200 > $B/steps.jsonl        # 51,204 lines
python3 -I .../analyze.py $B/chains.jsonl $B/chains.tsv > out/analyze.txt
python3 -I .../steps.py $B/steps.jsonl > out/steps.txt
$B/chains renorm 20261107 > out/renorm-gcc.tsv
$B/chains-clang renorm 20261107 > out/renorm-clang.tsv
python3 -I .../renorm_check.py $B/chains.jsonl > out/renorm-check.txt
<python with SymPy 1.14.0> -I .../identity.py > out/identity.txt
```

(`...` is `docs/agent/records/spikes/flat-plane-chain-rounding`.) Each run takes under 20 s. The dumps (12 MB and 39 MB) are not committed; they are regenerated bit for bit by the commands above, which `out/dumps.sha256` checks.

## Notes on the runs

- `chains` reproduces drift.cpp's draws only on GCC, which evaluates the arguments of `compose(m(), m())` and `compose(rnd(s), rnd(s))` right to left. Its firing counts (2, 4, 22, 36, 60, 114, 227 per 1,000) equal those of the review and of the comment of `transform`. The Clang binary evaluates them left to right and so draws other chains: `out/renorm-clang.tsv` is a second, independent sample, not a repetition.
- `steps` draws its two planes in a fixed order, so its chains are the same on both compilers but differ from those of `chains`.
- `renorm_check.py` repeats `renormalize()`'s operations on Python floats, which are IEEE 754 doubles rounded to nearest, in the same order and without fused multiply-adds; it was not compared bit for bit with the C++ output.
