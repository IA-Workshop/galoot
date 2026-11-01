# Mutants of plan 0015 and what killed them

Driver: `mutate.py` in this directory, at `cd7dcf2` (canary) and with the weight anchors on whole lines (self-check; the first self-check run stopped on an anchor that occurred twice, `typed\ttype.scan\t24` inside `map_typed\ttype.scan\t24`, before any mutant ran). Results: `canary-cd7dcf2-<preset>.tsv`, `selfcheck-cd7dcf2.tsv`. Job pool: 23 of 23 slots free before and after the canary runs.

## The canary, `core.plan_cost_model`, on clang-debug and gcc-debug

A header mutant adds one statement, `[[maybe_unused]] const std::size_t extra_step = 0;`, which changes no value; every one built without a warning on both presets (no "other" or "build fails" row). A threshold mutant changes one threshold of `test/CMakeLists.txt` by +1 or −1.

| Mutant | clang-debug | gcc-debug | The reference that moved |
| --- | --- | --- | --- |
| `typed_scan_step` (`type_terms`' search) | killed | killed | `gp_grades_3_7`, `right_complement_full8` |
| `collect_scan_step` (`collect_result`'s scan) | killed | killed | `gp_full5` (Clang), `reverse_full7` |
| `product_term_step` (`add_generator_product`, per term) | killed | killed | `gp_dense4`, `antigp_sparse16`, `gp_full5` (GCC) |
| `factor_step`, `run_term_step` (the antiproducts' `factor_of`, `run_term`) | killed | killed | `antigp_sparse16` |
| `map_copy_step` (`typed_map_of`'s copy loop) | killed | killed | `right_complement_full8` |
| 12 threshold mutants per compiler (+1 and −1 on each of six) | 12 killed | 12 killed | the one changed |
| `map_term_step` (`generate_map`, per blade) | survived | survived | none: `map_fill` is not the largest evaluation of any reference |
| `defects_step`, `typing_step`, `capacity_step`, `map_defects_step` | survived | survived | none: these phases are not the largest evaluation of any reference |

The survivors are the five phases that decision 0131 item 4 lists as outside the canary's reach. Over the 485 distinct inputs of the refit, none of them is the largest evaluation of a unit, except on Clang `map_defects` once (671 steps) and `map_fill` once (500 steps), both for maps of a few blades. A change in one of them therefore changes no figure that the comments state. It is seen only when the phase probes are measured again, as every refit does: the weights' prediction of those probes would then differ from the new thresholds.

## The self-check, `cost.py selfcheck`

See `selfcheck-cd7dcf2.tsv`: per compiler, one weight changed by one (`typed`/`type.scan`, `fill`/`gen.term`, `map_typed`/`map_typed.term`), one figure of `figures-<compiler>.txt` changed by one, and one event dropped from `mirror.py` (`type.scan`, `gen.term`, `collect.scan`).
