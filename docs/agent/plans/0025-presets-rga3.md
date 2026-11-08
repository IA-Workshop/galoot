# 0025 The rga3 algebra preset: `galoot::presets::rga3` and its oriented names

- Status: claimed 2026-11-08 by Claude (agent, Opus 5.5), implementer and coordinator with cpp-expert, build-expert, and hci-expert judgement, on `feature/presets-rga3` (worktree `.claude/worktrees/presets-rga3`), tests on `feature/presets-rga3--tests` (worktree `.claude/worktrees/presets-rga3--tests`), cherry-picked with `-x`. Not merged by its author; a blind review (W5) follows, launched by the coordinator.
- Source: decision 0211 items 1 and 4 (named algebras are layers on the core; the rga3 tag declares its own oriented names before the M4 gate); the WU15 hci reading's S2 (`records/reviews/2026-11-08-wu15-hci-reading.md`); the owner's word "presets" (2026-11-08) for the layer that holds named algebras, and the owner's rulings in the coordinator's brief of 2026-11-08: the layout `include/galoot/presets/rga3.hpp` with `include/galoot/presets/rga3/objects.hpp` and `motions.hpp`, namespace `galoot::presets::rga3` (the tag stays `algebra`), tests under `test/presets/rga3/`, the CMake alias `galoot::presets::rga3`, zero change under `include/galoot/core/`.
- Words: in this unit's prose, "build preset" is a CMake configuration of `CMakePresets.json`, and "preset" or "algebra preset" is a namespace `galoot::presets::*`.

## Goal

Move the rga3 algebra from `galoot/rga3` and `galoot::rga3` into the presets layer, keeping the history of every file, and give its tag the oriented `names` of the WU15 protocol (`format.hpp`, plan 0022 R3), so that a printed line reads e41 e42 e43 e23 e31 e12, a plane e423 e431 e412 e321, and a motor and a flector by their layouts' names, as CONVENTIONS 1.6 and 5.8 publish them.

## Definition of done and acceptance criteria

1. Headers at `include/galoot/presets/rga3.hpp`, `include/galoot/presets/rga3/objects.hpp`, `include/galoot/presets/rga3/motions.hpp`, in namespace `galoot::presets::rga3`, with guards named from the path; nothing under `include/galoot/core/` changes and the core names no preset.
2. The move is one commit made with `git mv` (history kept, `git log --follow` reaches the WU16 commits), before any change of behavior; it builds and passes on its own.
3. CMake: the INTERFACE target `galoot_presets_rga3` with alias `galoot::presets::rga3`, its header set checked header by header, the root check that every header belongs to a header set, and `format-check` over it. Tests under `test/presets/rga3/`; test names and labels `rga3.*` kept, since run selections, the mutation script, and recorded rows name them.
4. `tools/include_scan.py` reads the preset at its new path; its fixtures move to `galoot/presets/rga3` and still show every rejection, and the core's fixture that includes a preset header is still rejected.
5. Tests first: a new `rga3.format` test prints each of the five types exactly, by the published names, and a join, whose layout is the wedge's, by canonical names. Committed on the tests branch and cherry-picked with `-x` immediately before the names; that commit fails until the names land, and says so.
6. `galoot::presets::rga3::algebra` declares `names` with the sixteen oriented names of 1.6 and 5.8; its comment and the README say how values print.
7. README, comments, and the test sources say "preset" for the layer and give the new include, namespace, and target.
8. STYLE S6: an amendment drafted, marked a draft awaiting the owner's confirmation, saying where a preset lives and in which namespace. CONVENTIONS section 8's path to `objects_test.cpp` follows the move, also as a draft wording edit awaiting the owner.
9. Decision 0213 records the layout, the names, and the drafts.
10. Mutants: the set of `records/mutations/0023-rga3/mutate.py`, its paths and anchors updated, run again after the move, and new mutants of the names (a wrong blade name, two names swapped, a name with the wrong sign), each killed on gcc-debug and clang-debug.
11. Checks: P8's selection during work; at the end P9's merge checks from new, empty build trees on gcc-debug, gcc-release, clang-debug, clang-release, gcc-asan, clang-asan, lint, and clang-measure, with `format-check`, the trace gate, and `tools/runlog check`.

## Approach

Commits on `feature/presets-rga3`, in order: the move (paths, guards, namespace, CMake, scan, fixtures); the wording ("model" to "preset" in comments, README, and the scan's findings); the cherry-picked failing test commit; the names; the governance drafts and the decision; the mutation record; the run rows and this plan's outcome. `galoot_add_test` gains an optional `DIRECTORY`, the directory under `test/` that holds the component's source, since the module name `rga3` no longer equals its directory.

## Evidence

Exact expected strings written by hand from 1.6 and 5.8 (V1 kind 1); the trace gate unchanged; the mutants of criterion 10.

## Agent cost

One implementer session, about 150 tool calls, most of it build and test time.
