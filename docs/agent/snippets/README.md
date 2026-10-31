# Snippets

Reusable tools and fragments that agents made in the workbench or in a spike and that other work may want again. The workbench is scratch (W6) and is cleared at any time, so anything worth keeping is coalesced here.

## Rules

- A snippet has a present user or a stated likely one (C12). Dead snippets are deleted at the next review.
- Each snippet is a tracked file with a header comment saying what it does, how to run it, what it was written for, and the revision it was last checked at.
- Snippets are tools and fragments, not product code. Nothing under `include/` or `test/` depends on a snippet; when one becomes part of the build or the tests, it moves to `tools/` or `test/` by a unit with a plan and review, and its entry here is removed.
- External code is not copied in (C16).

## Coalescing review

At the end of each milestone, before its gate report, an agent surveys the workbench, `docs/agent/records/spikes/`, and the units' scratch files, and writes `docs/agent/snippets/reviews/<milestone>.md`: what is reusable, what is a duplicate of an existing snippet or of `tools/`, what to merge, what to delete, and what to promote to `tools/` or `test/`. The gate report cites the review. The review is read by the owner at the gate and changes nothing outside this directory without a unit of its own.

## Index

| Snippet | Does | Written for | Status |
| --- | --- | --- | --- |
| [peak-memory launcher](../records/spikes/spike-a/measure.py) | Runs a command and reports peak memory of its children by `getrusage` | Spike A, where GNU time is not installed | Candidate for WU1's launcher; review at M0 |
| [table diff against the reference](../records/spikes/spike-a/diff_reference.py) | Compares a generated plan table with `clifford_reference.py` output | Spike A | Candidate for WU6 |
| [fault injection for the reference engine](../records/spikes/spike-b/fault_injection.py) | Shows each check of the reference engine catches its target fault | Spike B | Candidate for DoD-14 |
| [constexpr plan generator](../records/spikes/spike-a/gen.hpp) | Throwaway `constexpr` blade product and plan generator | Spike A | Source of ideas for WU6, not reused as is |
| [runs.tsv merge rebuild](runs_tsv_merge.py) | Rebuilds `runs.tsv` after a union merge doubled it, padding old rows with `--` | WU1, for merges across the one-time header extension | In use until every branch has merged WU1; delete at the M0 review if no branch needs it |
| [bits fault injection](bits_fault_injection.py) | Applies sign and range mutations to `bits.hpp`, builds, runs the `core.bits` tests, and restores the header; `--runtime-only` also drops the test's `static_assert`s so the run-time checks must catch each one | WU2, to show the bits tests can fail | Candidate; a general mutation driver for later core headers may replace it at the M1 review |

The spike files stay where Spikes A and B committed them, so the notes that cite them remain valid. The first coalescing review, at M0, decides which move here.
