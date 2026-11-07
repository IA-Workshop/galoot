# How often transform's rounding check fires for versors with rounding of their own

WU13b (plan 0021 criterion 5, question O6), by the WU13b implementer (Claude, agent, Opus 5.5, numerical-expert role). Decision 0180 bounds what `transform` drops by this operation's own roundings, taking V and x as exact. A V built in floating point carries rounding error of its own and is only nearly a versor. This probe measures how often that makes the check fire.

## Method

`probe.cpp <cases> <seed>` draws, for each case, two or three hyperplanes with coefficients uniform in [−1, 1) (53-bit, from `std::mt19937_64`), forms the versor as their geometric antiproduct in double with `compose` (which rounds), and draws x the same way. For every dropped coefficient d it computes the tolerance tol of the check (`detail::transform_dropped_tolerances`) and records the largest ratio |d| / tol and the number of cases in which some ratio exceeds 1, the cases in which a checking build would abort. One plane alone is a flector made with no rounding, a control. The rigid(3) families are a second control: there every dropped part is zero for every V of one parity (decision 0177), so V's own error cannot make it nonzero.

Built with `-std=c++23 -O2 -ffp-contract=off -DGALOOT_CHECKING=0` and the warning flags of the test build, against the headers of `c2af6a8`, extracted with `git archive` from the next commit, which changes no header, since the worktree's header was being mutated at the time, on g++ 16.2.1 and clang++ 23.1.4; run as `probe 10000 20261107`. Rows `probe:transform-rounding:*` in `runs.tsv`. Outputs: `out/gcc.tsv` and `out/clang.tsv`, identical. The gcc output file came from a second, untimed run of the same binary with the same arguments; the timed run printed the same table.

## Result

10,000 cases per family, seed 20261107:

| Family | Dropped coefficients | Cases that fire | Largest \|d\| / tol |
| --- | --- | --- | --- |
| conformal(3) vector, 1 flat plane (exact) | 100,000 | 0 | 0.376 |
| conformal(3) vector, motor of 2 flat planes | 110,000 | 11 (0.11%) | 3.06 |
| conformal(3) vector, flector of 3 flat planes | 110,000 | 15 (0.15%) | 3.76 |
| conformal(3) vector, 2 general hyperplanes | 110,000 | 0 | 0.286 |
| conformal(3) vector, 3 general hyperplanes | 110,000 | 0 | 0.298 |
| conformal(3) bivector, motor of 2 flat planes | 60,000 | 0 | 0.203 |
| rigid(3) point, motor of 2 planes | 40,000 | 0 | 0.306 |
| rigid(3) point, flector of 3 planes | 40,000 | 0 | 0.321 |

Flat planes are on e1235, e1245, e1345, e2345, the rigid planes wedged with e5 (CONVENTIONS 6.2); general hyperplanes are on every blade of grade 4, spheres among them.

## Reading

- As decision 0180 expects, the check fires only where a dropped part is zero because V is a versor and not by an identity in V: the antiscalar part of a conformal(3) vector. It fires in about one case in a thousand, for versors made of flat planes, by at most a factor of about 4 over the tolerance. Neither the rigid(3) controls nor the exact flector ever fire, and their ratios stay below 0.4.
- General hyperplanes did not fire in these samples. Why flat planes do and general ones do not was not analyzed; a guess, not checked, is that the null e5 shared by every flat plane leaves the rounding error of V in a direction that the sandwich does not cancel.
- This is information for question O6, which the owner ruled: keep the check as it is for version 1, documented in the comment of `transform`, and revisit with the cga3 model (M6), whose rules for building versors can then be measured. A sampled claim (V9): 10,000 cases per family, one seed.
