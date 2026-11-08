# Mutants of the normalization in `rotation` (plan 0024)

`mutate.py` applies one mutant at a time to `include/galoot/rga3/motions.hpp`, builds `rga3_motions_test` and the two checking-mode programs, runs the CTests `rga3.motions`, `rga3.rotation_checking_*`, and `rga3.rotation_unchecked_*`, and restores the header. Results: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, test exit, the failed CTests and test cases); every build and test is a `mutant:<name>` row in `runs.tsv`.

Run by the plan 0024 implementer (Claude, agent, Opus 5.5) in `.claude/worktrees/rotation-normalize`, one preset after the other. A first run on gcc-debug, on the header of `3ab7f85` with the tests of `03c678f`, had a `division_omitted` that replaced only the sine, so its build failed on the unused `length` (a warning made an error, which does not count); the mutant now also removes that line, and the set was run again on both presets. After lint asked for parentheses in the length (`4577f85`, same evaluation order) and the tests' lint fixes (`451e03f`), the mutants' texts were updated to match and the set was run a third time on both presets, on the header of `4577f85`; `out/` holds that run, and its kills equal the second run's. The rows of all runs are in `runs.tsv`.

## After the blind review (2026-11-08)

Run again by the implementer answering the review, on the header of `184f1eb` with the tests of `f97c68f`, which add the length check, the checking-mode tests, and the half-turn test of exact fifths. The set gains the review's three mutants (`cos_divided`, `length_abs_vz`, `length_l1`) and `assert_removed`, which deletes the check. A first run of this set had `division_omitted` fail to build, since it removed the length that the new check reads (a build error, which does not count); the mutant now removes the check with it, and the whole set was run again on both presets. `out/` holds that run.

**11 mutants, all killed on gcc-debug and clang-debug, each by a failed test (test exit 8), none by a build error; the failed CTests and test cases are the same on both compilers.**

| Mutant | Killed by (both compilers) |
| --- | --- |
| `division_omitted`, `moment_only`, `times_length` | `rga3.motions` (the fifths test among its cases) and the unchecked tests, whose NaN, infinity, or zeros no longer appear |
| `direction_only`, `norm_squared`, `norm_with_moment` | `rga3.motions`, the fifths test among its cases |
| `norm_without_z`, `length_abs_vz` | `rga3.motions`, which aborts in the construction test on an axis whose computed length is now 0; `rga3.rotation_*_length_2`, or the unchecked overflow test |
| `cos_divided` | `rga3.motions` and `rga3.rotation_*_length_2` |
| `length_l1` | `rga3.motions`, the fifths test among its cases, and the overflow and underflow tests |
| `assert_removed` | the four `rga3.rotation_checking_*` tests that expect an abort, and nothing else |

Because the construction test aborts first under `length_abs_vz`, the fifths test was also run alone under it and under `length_l1` on gcc-debug (rows `mutant:<name>:fifths`): it fails its checks under both, so the review's L5 point, that these two were killed by the construction test only, no longer holds.

## Result of the earlier runs

7 mutants, each on gcc-debug and clang-debug: all 7 killed by `rga3.motions` on both compilers, each by a failed check (test exit 8), none by a build error. The test cases listed are the failed ones the runner reported, the same on both compilers.

| Mutant | What it changes | Killed by (both compilers) |
| --- | --- | --- |
| `division_omitted` | no division: the rotor of l as given | the quarter turn about the z axis of length 2 or 5; the same-motor test; the construction test |
| `moment_only` | the direction times the plain sine, the moment divided | the same three |
| `direction_only` | the direction divided, the moment times the plain sine | the same-motor test (its off-axis lines of length 4 and 5); the construction test |
| `norm_squared` | divides by the squared length | the quarter turn of length 2 or 5; the same-motor test; the construction test |
| `norm_with_moment` | the length of all six coefficients | the same-motor test; the construction test (lines off the origin) |
| `norm_without_z` | the length of (v_x, v_y) only | the unit quarter turn over float (a zero length for the z axis); the same-motor test; the construction test. The runner reported 5 of 15 test cases failed and 4 skipped, in CTest's output and when the program was run on its own once more (a `mutant:norm_without_z` build row of its own); why it skipped four was not investigated |
| `times_length` | multiplies by the length | the quarter turn of length 2 or 5; the same-motor test; the construction test |

`direction_only` and `norm_with_moment` pass the quarter-turn tests, whose axes pass through the origin (moment zero); the lines off the origin of the other two tests kill them.

2026-11-08: `mutate.py` now names `include/galoot/presets/rga3/motions.hpp`, the header's path after plan 0025's move (`26c8a82`), and runs again (plan 0026); the runs above were made at the old path, and their outputs are kept as they are.
