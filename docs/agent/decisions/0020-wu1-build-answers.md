# 0020 Answers to the WU1 open questions

- Status: accepted
- Date: 2026-10-31
- Decided by: the owner, answering plan 0002 section 11. Recorded by Claude (agent).

## Context

Plan 0002 asked 12 questions. The owner answered 1, 2, 5, 7, and 10 on 2026-10-31.

## Decision

1. The shared job-slot pool has 23 tokens, so one client running a single build holds about 24 jobs counting its own tokenless job (a client runs one job without a token). This sets the pool size P11 leaves to the owner.
2. `GALOOT_CHECKING` defaults to 0 when undefined. It is ON in the debug, asan, and lint presets and OFF in release, and every preset declares it in its own body.
3. The existing and future rows of `runs.tsv` migrate directly by extending the header once. A row that lacks a field written after its time records `--` in that cell.
4. `compile.tsv` is committed only for gate and measurement runs.
5. The compile flags of plan 0002 section 4.4 are approved as proposed: `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror` and `-ffp-contract=off` in every preset; `-O2` for release; `-D_GLIBCXX_ASSERTIONS` in every preset used for correctness; the sanitizer flags for asan; and a separate measurement preset without `_GLIBCXX_ASSERTIONS` for P4 reads and P6 benchmarks. `-O3` is added only as a tracked measurement variant, never as a release or correctness preset, and fast-math options are never used (C7, V14).

## Consequences

The commands that start the pool use 23 tokens. P14 says the pool is sized against physical cores (24); the owner's choice of 23 supersedes that figure for the pool size, and P14's reasoning (leave threads free for interactive work) is unchanged.
