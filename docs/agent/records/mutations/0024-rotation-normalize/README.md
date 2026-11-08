# Mutants of the normalization in `rotation` (plan 0024)

`mutate.py` applies one mutant at a time to `include/galoot/rga3/motions.hpp`, builds `rga3_motions_test`, runs the CTest `rga3.motions`, and restores the header. Results: `out/gcc-debug.tsv` and `out/clang-debug.tsv` (mutant, build exit, test exit, the failed test cases); every build and test is a `mutant:<name>` row in `runs.tsv`.

Run by the plan 0024 implementer (Claude, agent, Opus 5.5) in `.claude/worktrees/rotation-normalize`, one preset after the other, on the header of `3ab7f85` with the tests of `03c678f`. A first run of `division_omitted` replaced only the sine, so its build failed on the unused `length` (a warning made an error, which does not count); the mutant now also removes that line, and the whole set was run again. The rows of both runs are in `runs.tsv`; `out/` holds the second.

## Result

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
