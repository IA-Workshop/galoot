# The CTest label survey

Run by Claude (agent, Sonnet 5.5), plan 0043 unit U6, on 2026-11-21 at `8db4b0b` (`main` after the merge of `feature/cost-cleanup-1-0-1`), in the worktree of `records/cost-cleanup-1-0-1`.

## Question

The handoff of 2026-11-15 lists "the CTest label anomaly" among the backlog's tools and tests items. No record says what the anomaly was, and plan 0042 section 1 item 3 already said so. Plan 0043 section 1 item 4 surveyed the labels at `c95fee5` and found nothing; decision 0252 item 5 (b) accepts closing the item as "no record and no reproduction". This survey repeats the plan's survey on the merged tree and keeps its method and result in a place a later reader can rerun, since the plan's own survey kept no script or output.

## Method

Two fresh trees, configured through `tools/timed` (`runs.tsv`, selection `survey:ctest-labels`, 0.55 s each): `cmake --preset gcc-debug` and `cmake --preset clang-measure`. Configuring is all a label survey needs, since CTest labels are set at configure time and nothing is built or run. For each tree:

```bash
ctest --test-dir build/<preset> --show-only=json-v1 > <preset>.json
python3 -I survey.py <preset>.json <preset>
```

`survey.py` checks every test for five conditions (its docstring lists them): at least one label; a label equal to the module its name starts with; a label of the form `<module>.<component>`; no label twice in one test's list; a name with a module prefix. `labels-<preset>.tsv` lists each test with its labels in CTest's order, and `survey-<preset>.txt` is the script's output. The presets' test filter, the exclusion of the labels `runlog` and `bench` (`CMakePresets.json`, `test-base`), was applied with `ctest -N -LE '^(runlog|bench)$'` through `tools/timed` (selection `survey:ctest-list-preset-filter`; `preset-filter-<preset>.txt`).

## Result

| Tree | Tests | Fail A | Fail B | Fail C | Fail D | Fail E | After the presets' filter |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| gcc-debug | 680 | 0 | 0 | 0 | 0 | 0 | 657 |
| clang-measure | 672 | 0 | 0 | 0 | 0 | 0 | 649 |

- Every test carries its module label and at least one `<module>.<component>` label, and none repeats a label. The module labels are `bench` (22 tests), `core` (483 on gcc-debug, 475 on clang-measure), `reference` (3), `rga3` (75), `support` (19), `tests` (8) and `tools` (70).
- The labels outside a test's own module are four: `core.headers` and `rga3.headers` (one test each, the interface-header checks, which belong to the `tests` module), `runlog` (one test) and `slow` (two tests). Together with the 22 `bench` tests, the one `runlog` test is what the presets' filter leaves out: 23 tests, leaving 657 and 649, the counts of the 1.0.0 release matrix and of the 05229a7 matrix.
- The tree totals, 680 and 672, are the ones plan 0043 found at `c95fee5`. The splits of plan 0043 add sources and no tests, so the counts did not change.
- The two trees share 670 tests, and each test has the same labels in both. gcc-debug has ten tests clang-measure lacks (eight `core.plan_cost_model.*` tests and two `core.transform_rounding_*_bfloat16_unchecked` tests) and clang-measure has two gcc-debug lacks (`core.plan_evaluated_once` and `core.plan_map_evaluated_once`), which accounts for 680 against 672. Why those register on one compiler only was not looked into, since the survey asks about labels.

## What this does not show

The survey cannot rule out an anomaly of some other kind that the backlog's author saw, such as a label that CTest or the presets treated differently from what the lists above say, or a test selected by a label it should not carry. It shows only that no test lacks, repeats or misnames its module and component labels, in two of the eleven configurations, and that the presets' filter leaves the counts the release matrices recorded. The other nine presets were not configured; the labels come from `test/CMakeLists.txt` and do not depend on the preset, except for tests registered only for some presets (the twelve above). If the owner remembers what the anomaly was, a new item reopens it.
