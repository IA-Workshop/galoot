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

## After the W5 review, at `0aeb46b`

The canary is now eight tests, `core.plan_cost_model.<unit>`, with two more units (`antiwedge_full5`, `gp_vectors_full6`) and, on GCC, the check above the threshold (decision 0132). The driver gains the reviewer's two surviving mutants, `wedge_branch_step` (one statement in the wedge rule of `pair_terms`) and `collect_shift_step` (one statement in `collect_result`'s shift loop). Results: `canary-0aeb46b-<preset>.tsv`.

| Mutant | clang-debug | gcc-debug | The units that moved |
| --- | --- | --- | --- |
| `wedge_branch_step` | killed | killed | `antiwedge_full5` |
| `collect_shift_step` | killed | killed | `gp_vectors_full6` |
| `typed_scan_step` | killed | killed | `gp_grades_3_7`, `right_complement_full8` |
| `collect_scan_step` | killed | killed | `reverse_full7`, `gp_vectors_full6`, and `gp_full5` on Clang |
| `product_term_step` | killed | killed | `gp_dense4`, `antigp_sparse16`, and `gp_full5` on GCC |
| `factor_step`, `run_term_step` | killed | killed | `antigp_sparse16`, `antiwedge_full5` |
| `map_copy_step` | killed | killed | `right_complement_full8` |
| 16 threshold mutants per compiler | 16 killed | 16 killed | the one changed |
| `map_term_step`, `defects_step`, `typing_step`, `capacity_step`, `map_defects_step` | survived | survived | none, as before |

24 of 29 killed on each preset, every kill by a moved count; no mutant built with a warning. The F1 case (a GCC unit that compiles at its threshold although its count rose) is now judged by the stand-in test `tools.plan_cost_model_evades_at_threshold`.

The comments check (`cost.py comments`, F10), `comments-0aeb46b.tsv`: it passes on `plan.hpp` and fails when one count of the comments is changed by one (two mutants, `655,239` to `655,240` and `22,509,577` to `22,509,576`).

The self-check (F9), `selfcheck-0aeb46b.tsv`: 18 of 18 killed, the 14 mutants of the first run and, per compiler, `row_deleted` (the measured row of `full-gp-r-n6` removed from `plans-<compiler>-*.tsv`) and `row_fails` (its threshold set to `fails`), which the reviewer showed surviving at `26cc06e`.
