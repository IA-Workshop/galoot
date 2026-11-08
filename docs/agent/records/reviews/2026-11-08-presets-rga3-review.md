# Blind review of plan 0025, the rga3 algebra preset

- Reviewer: blind-reviewer role (Claude, agent, Opus 5.5), with the usability checklist of decision 0160. Date: 2026-11-08.
- Range: `git diff develop...feature/presets-rga3`, `develop` at `42d7695`, `feature/presets-rga3` at `b9e6fb5`. Record branch `records/review-presets-rga3`, worktree `.claude/worktrees/review-presets-rga3`, from `b9e6fb5`.
- The brief carried no verdict. It listed the authors' departures, which are taken as given below and checked where they could be. Plan 0025's outcome section states the authors' own check results. This review did not rely on them and reran the checks listed below.

## Departures, skipped checks, ambiguities (C20)

- The role file gives this reviewer no Write tool, but the brief asks for this record to be committed on the record branch. Only this file is committed.
- Run rows went to a scratch log, not to `runs.tsv`, so that this branch commits only this record. That is 36 rows: configure, build and test on gcc-debug and clang-debug, mutants, the two historical commits, and lint.
- Not run here: gcc-release, clang-release, gcc-asan, clang-asan, clang-measure, the full lint preset, `tools/runlog check`, and the authors' 50 mutants (their records were read, not rerun). Also not run: clang-debug mutants, a Python lint of `tools/include_scan.py`, and any search outside the galoot repository, such as the knowledge base. The decision 0080 compile-record copy was skipped by the authors and not done here.
- Ambiguity: "core mentions nothing of presets." `include/galoot/core/` names the layer nowhere. It does have local example tags called `struct rga3` (in format.hpp, linear.hpp, unary.hpp, dual.hpp and blade_set.hpp), and `assert.hpp:9` says "every preset", meaning a build preset. Neither one names the preset layer. `tools/include_scan.py` has to name `presets/rga3` as a module. `cmake/` names no preset.

## What was checked, and how

- **Zero core diff.** `git diff --stat develop...HEAD -- include/galoot/core include/galoot/core.hpp` prints nothing.
- **Old path and namespace.** `git grep` for `galoot::rga3`, `galoot/rga3`, `include/galoot/rga3`, `galoot_rga3`, `GALOOT_RGA3_` and `test/rga3` was run over the whole tree except agent records, decisions, plans and research notes. The only hit is CONVENTIONS' 1.15 history row, which is historical. Inside records, the hits are in decisions 0198, 0199 and 0210, plans 0023 and 0025, the WU16 diagnostics, mutations 0023 and 0024, rga3-cost, the WU16 reviews and the WU15 review. All describe past runs. Decision 0213 lists the three record scripts that a rerun must fix first.
- **Build, gcc-debug and clang-debug, in a new tree.** Every target built, and the full suite passed 493 of 493 on each. `format-check` exits 0. `all_verify_interface_header_sets` passes and compiles the three preset headers. `ctest -L '^tests'` passes 5 of 5. The trace gate exits 0, with 6.1 to 6.4 untested as before. `include_scan.py include` reports nothing. On its fixtures it exits 1 and reports every expected finding, `includes_preset.hpp` among them.
- **Lint.** The lint build preset with clang-tidy built every `rga3_*` target and `galoot_presets_rga3_verify_interface_header_sets` with no warning, and `ctest -L '^(rga3|tools)'` passed 70 of 70.
- **Commits.** `01d8cd2`, the move, was built alone in a new tree on gcc-debug: the full suite passed 492 of 492. `f1992d7`, alone, fails `rga3.format` in 6 of its 7 cases, and `rga3.readme_example_3` fails too. Each failure prints the fallback names, such as `-e14` and `e234`. That is exactly what the commit message says. Every commit names agent authorship and the model. `f1992d7` carries `-x`. W9: the test commit comes before the code. Plan 0025 lists the tests that judge the change, `tools.include_scan_rejects` among them. The departure that one agent wrote both the tests and the code was confirmed.
- **Mathematics.** By hand: e41 = −e14, e42 = −e24, e43 = −e34 and e31 = −e13. e423 = +e234, since 4 passes two indices. e431 = −e134, with three inversions, e412 = +e124, and e321 = −e123. These match CONVENTIONS 1.6. The join (e1+e4)∧(e1+2e2+e4) = 2e12 − 2e24, which as a Line is {e41: 0, e42: 2, …, e12: 2}, as 5.2 and 7.2 say. The motor and flector orders match 5.8.
- **Names matched by blade.** `format.hpp:112` compares `parse_name(text).blade` with the layout's oriented blade. That supports the authors' finding that `names_swapped` is an equivalent mutant.
- **Own mutants, gcc-debug.** These mutants change the tag's `names`; each rebuilds `rga3_format_test` and README example 3:

  | Mutant | Result |
  | --- | --- |
  | e34 for e43 (wrong sign, line) | killed by `rga3.format` and `readme_example_3` |
  | e124 for e412 (same orientation, other spelling) | killed by `rga3.format` |
  | e2143 for e1234 (same orientation, other spelling) | killed by `rga3.format` |
  | e42 dropped | killed by `rga3.format` and `readme_example_3` |
  | e413 for e431 (wrong sign, plane) | killed by `rga3.format` |
  | e32 for e23 | survives, and is equivalent. The layout stores +e23, so the output falls back to the canonical name `e23`, the same text. This is the class of `names_swapped`. |

  The header was restored, and both tests pass on it.
- **Wording.** Every "preset" added in the diff means the preset layer, except the three places where "build preset" is meant: the CMake comments, the configure error, and the `GALOOT_PRESET` help text. README uses only the first sense.
- **Drafts.** The CONVENTIONS 1.15 draft changes only the path in section 8 and matches the move. The STYLE S6 draft matches the layout, the guards and the namespace, apart from L3 below.

## Usability checklist

The tasks, from the examples:

1. Include the preset, join two points into a line, and print it.
2. Move a point by `rga3::translation` and `rotation`.
3. Link `galoot::presets::rga3` in CMake.

No task needed a reading of the implementation. Results by item:

- **Names.** The term "presets" is the owner's choice. It does not clash with CMake's sense in the README, the build files say "build preset", and the namespace alias idiom is shown.
- **Construction, conversion, semantics and cost.** Unchanged since WU16.
- **Diagnostics.** The compile-fail tests pass on both compilers. Context lines now spell the tag `galoot::presets::rga3::algebra`, nine characters longer per mention.
- **Examples.** README examples 1 to 3 compile and run. A first user's printed line now reads `{e41: 0, e42: 0, e43: 1, e23: 0, e31: 0, e12: 0}`.
- **Hyrum's law, before release.** These would cost more to change later: the include path, the namespace `galoot::presets::rga3`, the alias `galoot::presets::rga3`, the printed names, which users store in logs and golden files, and the canonical names that joins print.

## Findings

No finding blocks the merge.

- **L1, low.** Decision 0213 item 6 (`decisions/0213-rga3-as-an-algebra-preset.md:19`) says "Naming only the ten blades whose name differs from the canonical one prints the same." Eight names differ: e41, e42, e43, e31, e423, e431, e412 and e321. e23 and e12 are canonical. This is a factual slip in the record.
- **L2, low.** Item 2 of decision 0213 says "the five rga3 programs pass `DIRECTORY presets/rga3`". `test/CMakeLists.txt` has six, format among them.
- **L3, low, plausible.** The STYLE S6 draft says a preset includes "the core's public headers". `presets/rga3/motions.hpp:27` includes `<galoot/core/assert.hpp>`, and `core.hpp:4-6` places assert.hpp outside "every public core header". The include scan accepts any core header. Owner's wording: either "the core's headers", or a statement that assert.hpp is public.
- **L4, low.** Decision 0211 item 1 still prescribes `include/galoot/<model>/` and `galoot::<model>`, and decision 0213 only "bears on" it. A cga3 unit that follows 0211 would use the old layout. CONVENTIONS line 14, and section 5's marker "Component: model (rga3)", keep the old term. Recommendation: say in 0213 that it amends 0211 item 1. The marker wording is for the owner.
- **L5, low, usability.** `std::println("{}", p ^ q)` prints `e24: -2` where the Line prints `e42: 2`. The objects.hpp `names` comment says this, and `rga3.format` pins it. The README's printing paragraph does not. One sentence there would cover it.
- **L6, informational.** The header-set rule (`CMakeLists.txt:63`) and the scan's `MODULES` name only `presets/rga3`. A future `include/galoot/presets/<other>.hpp` would join `galoot_core`'s header set and escape the default scan. No such header exists (C12). This is a note for the next preset unit.

## Outcome

Approve with non-blocking findings.
