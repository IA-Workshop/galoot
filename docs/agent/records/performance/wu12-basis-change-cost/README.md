# WU12: the compile and test cost of the change of basis

Plan 0030 criterion 15 (W10, P1, C14; track-only, no bound applies). Measured by Claude (agent, Opus 5.5, implementer of plan 0030 part E) on 2026-11-09 from 15:23:07 to 15:27:17 UTC. The command was `tools/compiles cost docs/agent/records/performance/wu12-basis-change-cost/units.toml docs/agent/records/performance/wu12-basis-change-cost/out --before 1818efe --cpu 4 --repeats 3`, run in the unit's worktree at `752fdea`, whose `include/` and `test/` are those of `404f2fc`. "Before" is `1818efe`, the unit before part B's tests and part C's header. It ran under the shared timing lock (`lock.sh take`, taken at once, released at the end). The job pool read 23 of 23 slots free before and after, and none of this unit's builds or mutants ran then; the load average was 2.15 at the start (other load on the machine, not this unit's, of unknown source) and 1.05 at the end. Every compile was pinned to CPU 4, a performance core, with flags from the debug trees (gcc-debug: g++ 16.2.1; clang-debug: clang++ 23.1.4), three repeats interleaved; the sibling thread's busy time per compile was at most 0.01 s. Rows: `out/rows.tsv`; campaign log: `out/runs.tsv`; means and ranges: `out/summary.txt`. The extracted trees, the text units, and the objects are not committed.

The units are every source that includes `galoot/core.hpp` (the umbrella alone, the core's sources that include it or `format.hpp`, the rga3 preset's test sources, and the README's three examples as text), before and after; the two test sources part B changed; and the new sources, after only.

## Mean retired instructions (10⁹), before → after

Includers of `galoot/core.hpp`, which now parses `basis_change.hpp`:

| Unit | GCC | Clang |
| --- | --- | --- |
| `#include <galoot/core.hpp>` alone | 0.968 → 0.992 (+2.4%) | 0.978 → 0.995 (+1.7%) |
| `core/smoke` | 1.167 → 1.190 (+2.0%) | 1.079 → 1.095 (+1.5%) |
| `core/bare_user` | 1.237 → 1.261 (+1.9%) | 1.156 → 1.172 (+1.3%) |
| `core/format_outside_core` | 3.559 → 3.581 (+0.6%) | 4.205 → 4.221 (+0.4%) |
| `core/format` | 34.567 → 34.543 (−0.1%) | 23.034 → 23.031 (−0.0%) |
| `presets/rga3/bare_user` | 3.141 → 3.163 (+0.7%) | 3.037 → 3.052 (+0.5%) |
| `presets/rga3/chains` | 4.365 → 4.381 (+0.4%) | 4.296 → 4.313 (+0.4%) |
| `presets/rga3/format` | 7.853 → 7.877 (+0.3%) | 6.485 → 6.504 (+0.3%) |
| `presets/rga3/incidence` | 4.345 → 4.367 (+0.5%) | 3.894 → 3.908 (+0.4%) |
| `presets/rga3/motions` | 14.741 → 14.754 (+0.1%) | 10.930 → 10.949 (+0.2%) |
| `presets/rga3/objects` | 12.933 → 12.939 (+0.0%) | 11.842 → 11.844 (+0.0%) |
| `presets/rga3/point_groups` | 11.454 → 11.471 (+0.2%) | 10.384 → 10.404 (+0.2%) |
| README example 1 | 8.000 → 8.018 (+0.2%) | 6.911 → 6.912 (+0.0%) |
| README example 2 | 2.085 → 2.107 (+1.0%) | 2.403 → 2.413 (+0.4%) |
| README example 3 | 6.688 → 6.710 (+0.3%) | 5.531 → 5.550 (+0.4%) |

The parse cost of the new header is about 0.02 × 10⁹ instructions per includer on both compilers, a few milliseconds (the umbrella alone: 0.126 s on GCC and 0.143 s on Clang after). `core/format` is unchanged within its range: it includes `format.hpp`, which includes the umbrella, and its cost is dominated by its own instantiations.

Sources part B changed:

| Unit | GCC | Clang |
| --- | --- | --- |
| `core/blade_product` (the matrix representations case) | 8.987 → 9.075 (+1.0%) | 6.095 → 6.135 (+0.7%) |
| `support/tracked` (division, correct rounding, the quotient check) | 1.727 → 1.993 (+15.4%) | 1.054 → 1.171 (+11.1%) |

New sources (after only), with mean wall time on CPU 4 and peak memory:

| Unit | GCC | Clang |
| --- | --- | --- |
| `core/basis_change` | 9.41, 0.73 s, 240 MiB | 8.88, 0.69 s, 186 MiB |
| `core/basis_change_scalars` | 27.93, 2.20 s, 389 MiB | 20.49, 1.61 s, 326 MiB |
| `core/basis_change_validation` | 14.14, 1.03 s, 299 MiB | 11.95, 0.90 s, 201 MiB |
| `core/basis_change_identities` | 4.74, 0.48 s, 152 MiB | 2.43, 0.28 s, 147 MiB |
| `core/basis_change_conformal` | 50.87, 3.28 s, 416 MiB | 52.37, 3.24 s, 297 MiB |
| `core/basis_change_conformal3` (raised Clang step limit) | 63.59, 3.91 s, 455 MiB | 69.73, 4.01 s, 290 MiB |
| `core/basis_change_witt` | 41.23, 2.74 s, 397 MiB | 37.75, 2.50 s, 289 MiB |
| `core/basis_change_random` | 47.78, 3.23 s, 463 MiB | 39.85, 2.66 s, 301 MiB |
| `core/basis_change_random_five` | 31.65, 2.20 s, 371 MiB | 26.03, 1.81 s, 263 MiB |
| `support/diagonalize` | 3.62, 0.36 s, 120 MiB | 1.74, 0.21 s, 131 MiB |

- **The program `core_basis_change_test`** (three sources): 51.5 × 10⁹ on GCC, 41.3 × 10⁹ on Clang. **`core_basis_change_identities_test`** (six sources): 239.9 × 10⁹ on GCC, 228.2 × 10⁹ on Clang. Its largest source, conformal(3)'s, takes about 4 s on either compiler, well below the 10.6 s of `dual_identities_test.cpp` (WU14's record), so no source was split and no layout narrowed.
- The ranges over three repeats are at most 0.64% of a mean (`out/summary.txt`), so every change above 1% is beyond the variation; the changes of the rga3 and README units are near it.
- The compile-fail tests of the change (25) each compile one small source; their cost was not measured separately.

## Test time

From the seed campaign (`records/mutations/0030-basis-change/seeds/runs.tsv`, gcc-debug, 50 runs each, unpinned, 2026-11-09 15:22 UTC): `core_basis_change_test` median 0.056 s (0.049 to 0.099 s); `core_basis_change_identities_test` median 0.172 s (0.163 to 0.212 s). `support.diagonalize` ran in 0.05 to 0.08 s in part B's runs. None is labeled `slow`.

## The limits of the header

The compile limits stated in `basis_change.hpp`'s comment (which layouts build at the default limits of constant evaluation) were found with the probe `limits/probe.cpp`: a change of R(0, 0, n) with every entry nonzero (2 on the diagonal and 1 elsewhere, which keeps the all-zero inner products), and `conformal_change<D>`, on full layouts, vectors, and bivectors, forward and back. Each compile was `g++` or `clang++ -std=c++23 -fsyntax-only -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -D_GLIBCXX_ASSERTIONS -O0 -DGALOOT_CHECKING=1 -I include -DNGEN=<n> -DKIND_CONFORMAL=<0|1> -DLAYOUT_FULL=<0|1> -DLAYOUT_BIVECTORS=<0|1> -DBACK=<0|1> limits/probe.cpp`, from the repository root on the uncommitted tree before `2e10061` (the rows say `79b35f9-dirty`), whose header code is that of `2e10061`: only the header's comment changed between them, to state these limits, and, at the boundary points, the same with `-D_GLIBCXX_NO_ASSERTIONS`, each through `tools/timed --log` into `limits/runs.tsv`, whose selection column names the compiler, kind, layout, direction, and n, and whose exit column says whether it built. They ran outside CMake and the pool: each is one compile.

## Again after the review fixes and rulings

The same command into `out-rulings/`, at `8c717f1` (whose `include/` and `test/` are those of `30c2275` and the test commits before it; the tree was dirty only in a decision record), 2026-11-09 from 16:47:13 to 16:51:42 UTC, under the timing lock (taken at once), the job pool 23 of 23 free before and after, pinned to CPU 4, the sibling's busy time at most 0.16 s per compile; the load average was 1.90 at the start and 2.39 at the end, from other work on the machine of unknown source. "Before" is again `1818efe`, whose figures repeat the first campaign's within 0.5%.

Against the first campaign's "after" (mean retired instructions):

- **Includers of `galoot/core.hpp`** (the umbrella alone, `core/smoke`, `core/bare_user`, `core/format_outside_core`, `core/format`, the seven rga3 sources, the README's three examples): within −0.1% to +0.3% on both compilers. The umbrella alone is 0.992 → 0.994 × 10⁹ on GCC and 0.995 → 0.998 × 10⁹ on Clang. The header's longer checks and the two catch-alls add about 2 to 3 × 10⁶ instructions of parse to each.
- **The change's sources whose tests changed**, through the N4 checks in `check_transform` (a flector, the projection, and the antiprojection) and the header example's origin case: `basis_change_conformal` +8.1% on GCC and +5.6% on Clang, `basis_change_conformal3` +6.2% and +5.2% (4.46 s and 4.43 s), `basis_change_witt` +13.5% and +11.4%, `basis_change` +2.4% and +1.9%, `basis_change_scalars` (the integer `change_basis_back` case) +0.8% and +1.0%.
- **The others** (`basis_change_identities`, `_random`, `_random_five`, `_validation`, `support/diagonalize`, `support/tracked`, `core/blade_product`): within 0.7%, the cost of the header's tag checks in a translation unit that checks a few tags.
- The largest source is still conformal(3)'s, 4.5 s on either compiler, 473 MiB on GCC.

The seed campaign was run again after the fixes (`records/mutations/0030-basis-change/seeds/runs-after-rulings.tsv`): `core_basis_change_test` median 0.056 s, `core_basis_change_identities_test` 0.173 s on gcc-debug.
