# WU10a antiproducts, involutions, complements: mutants, diagnostics, and costs

Revision: `feature/core-antiproducts` at `5f209d4` (the trace commit) for the mutants and the costs; the diagnostics at the same headers. Toolchain: g++ 16.2.1 and clang++ 23.1.4, on ds-001.

## Mutants

`mutate.py <preset>` applied each mutant to one header, built `core_product_test`, `core_unary_test`, and `core_identities_test` with `ninja -k 0`, ran the tests labeled `core.product` or `core.unary` (the three programs, the 20 product compile-fail tests, and `core.identities`), and restored the header. The rows of each run are in `out/gcc-debug.tsv` and `out/clang-debug.tsv`, with the first error line of every program that did not build; the runs are in `runs.tsv` under the selection `mutant:<name>`. The first run of `percent_calls_geometric_product` was not a valid mutant on Clang (its deduced return type kept the guarded `return {};`, which Clang rejects at the template's definition, so every includer of `product.hpp` failed); it was rewritten to replace the whole body and run again on both presets, and the rows above are the second run.

| Mutant | gcc-debug | clang-debug | Killed by |
| --- | --- | --- | --- |
| `reverse` calls the antireverse map | killed | killed | `core.unary` (reference signs) and `core.identities` (product rules), at run time |
| `grade_involution` calls the reverse map | killed | killed | the same |
| `!` calls `left_complement` | killed | killed | `core.unary`, at run time: the n = 4 reference complements, where the two complements differ |
| `reverse` and `~` return the canonical layout | killed | killed | a `static_assert` of `unary_test.cpp` (line 419, `decltype(~l)` is not `Line`) |
| `%` calls `geometric_product` | killed | killed | a `static_assert` of `product_test.cpp` (line 616, the result type), `core.identities` at run time, and `core.product_narrow_scalar_percent` (the message names `geometric_product`) |
| `antiwedge` uses the geometric-antiproduct plan and layout | killed | killed | a `static_assert` of `product_test.cpp` (line 534, a result type) and `core.identities` at run time |
| `&` calls `wedge` | killed | killed | the same |
| the operands of `geometric_antiproduct` are swapped | killed | killed | `core.product` (the composed tables) and `core.identities`, at run time |
| the coefficient check of `geometric_product` is dropped | killed | killed | `core.product_narrow_scalar_geometric_product` (the output begins `apply:`) |
| the same, of `a * b` | killed | killed | `core.product_narrow_scalar_times` |
| the same, of `geometric_antiproduct` | killed | killed | `core.product_narrow_scalar_geometric_antiproduct` |
| the same, of `a % b` | killed | killed | `core.product_narrow_scalar_percent` |
| the algebra constraint is dropped from the four new catch-alls | killed | killed | a `static_assert` of `product_test.cpp` (line 698, `can_antiwedge<R, C>`) and the four `core.product_mixed_algebras_*` tests of the new spellings |
| the scalar constraint is dropped from the four new catch-alls | killed | killed | a `static_assert` of `product_test.cpp` (line 697) and the four `core.product_mixed_scalars_*` tests of the new spellings |
| `left_complement_sign` is flipped for e12 in `bits.hpp` | killed | killed | `core.product` at run time (the composed antiproduct tables) |

No mutant was killed by a warning: every compile failure in the records is a `static_assert` of a test file, except for the last mutant, where `unary_test.cpp` and `identities_test.cpp` did not compile because the unary map's own generation check (`plan.hpp`, "a complement's sign differs from its closed form") caught the flipped sign. That check is the library's, not a test's, so the kill counted is `core.product`'s, whose program built and failed at run time on both compilers.

One caution in reading the records: when a program does not build, CTest runs the previous build of it, which passes, so a failed build appears with that test absent from the failed list. Every kill above is either a fresh build that failed in a test's own `static_assert`, or a fresh program or compile-fail test that failed.

No equivalent survivor.

## Diagnostics

Each mixed or narrow-scalar call gives one error, whose first line holds the message. Sizes, measured by `diagnostics.py` with the test flags and `-fsyntax-only`, are in `out/diagnostics.tsv` and summarized beside the WU9 table in `../0018-products/README.md`. No public spelling's output contains `apply:`. Clang's first line for `(p ^ q) == line` (`compare_probe.cpp`) is

```
error: invalid operands to binary expression ('mv<(anonymous namespace)::rga3, wedge_layout<(anonymous namespace)::rga3, blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}, blade_set<4>{{{{1, 1}, {2, 1}, {4, 1}, {8, 1}}}}>, double>' and 'const Line' (aka ...))
```

with no `product_plan`, `detail::`, or enum cast. GCC prints the value of the layout, as before (966 characters on its first line).

## Costs

Measured by `../../performance/antiproducts-cost/measure.py develop <out> 3 4`: every compile pinned to CPU 4 (a performance core) through `tools/timed`, three repeats interleaved, the gcc-debug and clang-debug test flags with `-c`. "Before" is `develop` at `91fffe3`, the branch's base. Instructions are those retired on the pinned core, means of three runs in millions, with the spread (maximum minus minimum) in parentheses; wall time is the mean, and peak memory the maximum. The machine had other light load (a language server and short processes); the instruction counts do not depend on it. Figures in `../../performance/antiproducts-cost/out/summary.txt` and `rows.tsv`.

### W10: the affected suites

| Unit | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| `product_test.cpp` | 21,584 M (6), 1.47 s, 396 MiB | 43,001 M (42), 2.81 s, 404 MiB | 19,151 M (48), 1.30 s, 224 MiB | 42,687 M (33), 2.76 s, 267 MiB |
| `unary_test.cpp` (new) | | 14,954 M (5), 1.32 s, 337 MiB | | 11,509 M (26), 1.17 s, 237 MiB |
| `identities_test.cpp` (new) | | 295,000 M (500), 18.9 s, 1,380 MiB | | 299,107 M (842), 21.9 s, 699 MiB |
| `smoke_test.cpp` | 1,130.9 M (0.1), 0.161 s | 1,135.1 M (0.0), 0.153 s | 1,044.0 M (2.7), 0.165 s | 1,047.8 M (1.5), 0.171 s |

`product_test.cpp` doubles because it now also tests the antiproducts over every blade pair of four forms. The test programs run in under 0.1 s each (`core.product` 0.07 s before and after, `core.unary` 0.07 s, `core.identities` 0.06 s on gcc-debug).

`identities_test.cpp` is the largest unit of the suite, about 4.6 times `mv_test.cpp` (64.5 G on GCC), and sets the pace of any parallel build that includes it. It carries the `slow` label, so it runs before a commit only when the change touches the products or the unary operations, and otherwise from the next merge level up.

### P2: the new includer of `core.hpp`

`core.hpp` now includes `unary.hpp`, and `product.hpp` grew by the antiproducts. A unit that includes only `core.hpp`:

| | GCC | Clang |
| --- | --- | --- |
| before | 932.7 M (0.4), 0.131 s, 78 MiB | 942.4 M (1.4), 0.138 s, 113 MiB |
| after | 937.9 M (0.2), 0.138 s, 80 MiB | 949.1 M (2.5), 0.139 s, 113 MiB |
| change | +5.2 M (+0.6%) | +6.7 M (+0.7%) |

The smoke test, which also includes doctest: +4.2 M (+0.4%) on GCC and +3.8 M (+0.4%) on Clang. Both changes are beyond the spread of the repeats. The headers cost parsing only for a program that uses none of the new operations (P3). The smoke-test reading of WU9 (101,752 instructions) was a failed counter; these rows are pinned and read the core's counter directly.

### The antiproduct's growth at fixed use, through `%`

The four typed rigid(3) pairs of decision 0116 (point × point, line × plane, motor × point, point × motor), embedded in R(n−1,0,1) with e4 read as e_n, as `mv<alg, layout, double>` values multiplied with the public `%`, and for comparison with `*`, each result used at run time; a baseline unit has the same algebra, layouts, and values and no product. "Use" is the unit minus the baseline at the same n.

| | GCC n = 4 | GCC n = 16 | GCC growth | Clang n = 4 | Clang n = 16 | Clang growth |
| --- | --- | --- | --- | --- | --- | --- |
| `%` | 747.3 M | 1,545.3 M | +106.8% | 914.3 M | 4,174.0 M | +356.5% |
| `*` | 837.9 M | 838.9 M | +0.1% | 811.5 M | 808.8 M | −0.3% |

The percentages are lower than decision 0141's (+265% and +464%) because the public path adds the kernel and the code of `apply`, which do not grow with n, to the plans that do. The growth itself is the same as 0141's within a few percent: +798 M on GCC and +3,260 M on Clang here, against +820 M and +3,265 M for the four plans alone at `50c7c70`. At n = 4, the size of rigid(3), the antiproduct costs GCC 11% less than the product and Clang 13% more. The canary `core.plan_cost_model.antigp_sparse16` (443,211 operations on GCC, 77,513 steps on Clang) is unchanged: this unit does not touch generation, and the canary passes in the final runs of every preset.
