# Survivors of the metric_like mutation runs

Driver: `mutate.py` beside this file. Results by revision of the driver and the tree:

| Results | Driver and tree | Mutants |
| --- | --- | --- |
| `results-1c07695-*.tsv` | `1c07695` | the first 13, normal mode on gcc-debug and clang-debug, and the eight of the pass in the run-time mode |
| `results-99b8423-*-mirror.tsv` | `99b8423` | `mirror_read_twice`, both modes, both presets |
| `results-9f5f545-*-after-review.tsv` | `9f5f545` (with the stored-verdict test of `5fb4bf6`) | `no_cache` and `no_count_test`, normal mode, both presets |

## Status, mutant by mutant

| Mutant | Status | Killed by |
| --- | --- | --- |
| `no_symmetry` | killed (normal mode) | `static_assert`s of `asymmetric`, `widest_broken`, and the n = 1 and 2 enumeration; in the run-time mode only `-Wunused-variable` stops the build, which does not count, so `mirror_read_twice` stands in for it there |
| `mirror_read_twice` | killed, both modes | normal: the same `static_assert`s; run-time: the n = 3 test case |
| `no_range`, `no_upper_bound`, `diagonal_skipped` | killed, both modes | `entry_too_large` and the enumeration; run-time: the n = 3 case |
| `no_lower_bound`, `lower_bound_inclusive`, `upper_bound_inclusive` | killed, both modes | the enumeration only; run-time: the n = 3 case (`upper_bound_inclusive` also fails `core.plan_overflow`) |
| `last_row_skipped` | killed, both modes | `asymmetric`, `entry_too_large`, `widest_broken`, the enumeration; run-time: the n = 3 case |
| `no_count_test` | killed | `core.metric_like_no_generators` only (rerun at `9f5f545` after its anchor changed) |
| `no_guard`, `guard_reads_count` | killed | a hard error in the stored verdict, reached from the unreadable-value `static_assert`s |
| `no_type_test` | killed | a hard error reached from `lookalike` |
| `no_cache` | killed at `9f5f545` | the `stored_verdict_false` `static_assert`; it survived at `1c07695`, before that test existed |

No survivor remains. The test that kills `no_cache` shows that the concept reads the stored verdict; that the compiler evaluates the stored verdict's initializer once per unit is compiler behavior, shown only by the P2 measurement.
