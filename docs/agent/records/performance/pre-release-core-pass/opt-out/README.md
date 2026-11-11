# Plan 0040 section 18: the six identity units with the forced inlining turned off

The build cost (W10, P1) of the six identity test units that dominated gate 2, under the three presets where the gate found them costly, with `GALOOT_FORCE_INLINE` OFF as those presets now set it. Measured by Claude (agent, Opus 5.5, implementer of plan 0040 section 18) on 2026-11-11 from 09:55:07 to 09:57:54 UTC, in the worktree of `feature/pre-release-core-pass--inline-opt-out` at `2d292ef` (the code commit of the opt-out). The run log marks the revision `-dirty`: the README, the core guide and decision 0244 had uncommitted edits, and no file a compile reads had any.

- Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4, each with libstdc++ 16, on x86-64 Linux (host `ds-001`).
- Units: `identities_rounding_test.cpp`, `identities_five_diagonal_test.cpp`, `identities_four_test.cpp`, `identities_conformal3_test.cpp`, `identities_five_drawn_test.cpp`, `identities_five_dense_test.cpp`, the six object files only, each with its preset's flags (`out/compile-<preset>.tsv`, last column).
- Protocol: fresh build trees from `cmake --preset <preset>`, then, one preset at a time, `ninja -C build/<preset> <the six objects>` through `tools/timed` (campaign log `runs.tsv`) with the shared job pool, so the six compiled in parallel; each compile went through the build's own compile-cost launcher (`tools/measure`), whose rows are the figures below. A watchdog (outside the repository) read every 2 s the largest resident size of a compiler working in the tree and the machine's available memory, and would have stopped the build above 20 GiB for either; it never fired (`out/watchdog-<preset>.txt`).
- Conditions: the timing lock was taken through `lock.sh` at 09:55:14 UTC and released after the last build. Before each preset the pool read 23 of 23 slots free, about 105 GiB were free and the load average was 1.1 to 2.9 (`out/conditions-<preset>.txt`). The compiles were not pinned, so the six of a preset shared the machine with one another.
- Not run: clang-measure, which differs from clang-release only by `-D_GLIBCXX_ASSERTIONS` (inferred to behave as clang-release, as it did at the gate); the whole preset builds and suites, which the next gate runs.

## Results

Instructions are retired user-space instructions on both kinds of core, in 10⁹. The comparison rows are read from the compile-cost records of two other build trees, not rerun here: `ed9288f` is the documentation lane's P9 matrix, the last `develop` before the forced inlining (worktree `documentation-lane`), and `a310a2f` is gate 2 (worktree `pre-release-core-pass-gate2`). clang-asan has no rows at `a310a2f`: the gate stopped it before its compiles wrote one, with 8 to 16 GB per compile. All three are in `out/table.tsv`.

| Preset | Unit | Wall `ed9288f` (s) | Wall `a310a2f` (s) | Wall now (s) | Peak `ed9288f` (MiB) | Peak `a310a2f` (MiB) | Peak now (MiB) | Inst. `ed9288f` | Inst. `a310a2f` | Inst. now |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| gcc-asan | `identities_rounding_test` | 78.4 | 5,302.7 | 32.6 | 1,198 | 6,367 | 1,204 | 375 | 47,076 | 375 |
| gcc-asan | `identities_five_diagonal_test` | 81.1 | 4,845.4 | 25.5 | 838 | 12,257 | 839 | 261 | 32,517 | 261 |
| gcc-asan | `identities_four_test` | 91.0 | 3,459.4 | 42.9 | 1,776 | 21,519 | 1,777 | 506 | 23,715 | 506 |
| gcc-asan | `identities_conformal3_test` | 55.4 | 3,401.6 | 14.9 | 647 | 7,663 | 646 | 152 | 19,790 | 152 |
| gcc-asan | `identities_five_drawn_test` | 41.8 | 1,971.3 | 14.0 | 620 | 8,216 | 628 | 162 | 14,328 | 162 |
| gcc-asan | `identities_five_dense_test` | 89.5 | 1,088.4 | 29.5 | 1,077 | 5,211 | 1,080 | 322 | 7,028 | 322 |
| clang-release | `identities_rounding_test` | 20.1 | 600.0 | 9.5 | 391 | 1,870 | 392 | 120 | 4,275 | 120 |
| clang-release | `identities_five_diagonal_test` | 11.5 | 43.5 | 5.0 | 254 | 550 | 253 | 73 | 339 | 73 |
| clang-release | `identities_four_test` | 20.7 | 53.3 | 12.5 | 378 | 972 | 379 | 150 | 511 | 150 |
| clang-release | `identities_conformal3_test` | 5.7 | 21.5 | 2.8 | 223 | 374 | 222 | 41 | 182 | 41 |
| clang-release | `identities_five_drawn_test` | 8.2 | 25.6 | 4.0 | 235 | 404 | 235 | 58 | 206 | 58 |
| clang-release | `identities_five_dense_test` | 13.2 | 15.7 | 5.9 | 286 | 307 | 286 | 86 | 138 | 86 |
| clang-asan | `identities_rounding_test` | 56.3 | not recorded | 38.9 | 1,100 | not recorded | 1,099 | 377 | not recorded | 378 |
| clang-asan | `identities_five_diagonal_test` | 25.2 | not recorded | 17.0 | 803 | not recorded | 802 | 182 | not recorded | 182 |
| clang-asan | `identities_four_test` | 57.5 | not recorded | 31.5 | 914 | not recorded | 912 | 312 | not recorded | 312 |
| clang-asan | `identities_conformal3_test` | 15.6 | not recorded | 9.6 | 725 | not recorded | 725 | 102 | not recorded | 103 |
| clang-asan | `identities_five_drawn_test` | 18.6 | not recorded | 10.6 | 531 | not recorded | 531 | 121 | not recorded | 121 |
| clang-asan | `identities_five_dense_test` | 43.8 | not recorded | 20.4 | 625 | not recorded | 625 | 205 | not recorded | 205 |

The builds of the six took 43.0 s (gcc-asan), 13.0 s (clang-release) and 39.4 s (clang-asan) of wall time (`runs.tsv`). The largest compile was `identities_four_test.cpp` under gcc-asan at 1,777 MiB, against 21,519 MiB at the gate.

## Reading

- With the opt-out, every unit's instruction count and peak memory equal those at `ed9288f` to within 1% (the largest difference is 1.3% in a peak, `identities_five_drawn_test` under gcc-asan). That is what the mechanism predicts: with `GALOOT_FORCE_INLINE` 0 the attribute expands to nothing, and the headers' other code changes since `ed9288f` are two renamed packs, the deleted `==` of `basis_change` and the deleted `detail::inverse_change`, none of which changes what these units instantiate (inferred from the counts, not traced).
- The wall times are lower than at `ed9288f` because that matrix and gate 2 ran each preset's whole build at once with up to 23 compiles in parallel, while here six ran on an otherwise quiet machine. Compare the instruction counts and the peaks, not the wall times.
- At the gate, with the attribute, these units took up to 125 times their `ed9288f` instructions under gcc-asan (`identities_rounding_test`, 47,076 against 375) and up to 36 times under clang-release (the same unit, 4,275 against 120).
- Checked in the same trees (rows in `records/performance/runs.tsv`, not in this campaign's log): `core.identities` and `core.identities_rounding` build and pass under clang-release and gcc-asan, the identity programs' largest compile peaking at 1.5 GB (gcc-asan); `core.force_inline_build`, `rga3.reference_kernels_unforced` and the structure tests `rga3.kernels_inline` and `rga3.kernels_inline_alone` pass under clang-release and gcc-asan, the last two still checking the attribute with their fixed flags.
