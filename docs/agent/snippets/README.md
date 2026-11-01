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
| [peak-memory launcher](../records/spikes/spike-a/measure.py) | Runs a command and reports peak memory of its children by `getrusage` | Spike A, where GNU time is not installed | Superseded by `tools/measure`; kept so the spike notes that cite it stay valid |
| [table diff against the reference](../records/spikes/spike-a/diff_reference.py) | Compares a generated plan table with `clifford_reference.py` output | Spike A | Candidate for WU6 |
| [fault injection for the reference engine](../records/spikes/spike-b/fault_injection.py) | Shows each check of the reference engine catches its target fault | Spike B | Candidate for DoD-14 |
| [constexpr plan generator](../records/spikes/spike-a/gen.hpp) | Throwaway `constexpr` blade product and plan generator | Spike A | Source of ideas for WU6, not reused as is |
| [evaluation counter](../records/spikes/spike-cdf/count_evals.py) | Counts the expensive constant evaluations in a Clang `-ftime-trace` file | Spike D | Candidate for WU6 |
| [jobserver check](jobserver_check.py) | Shows, on a private FIFO pool, the slots Ninja returns when killed or interrupted, and how many jobs run when the pool restarts under a build or is missing | Plan 0003, the token-loss and pool-restart checks the WU1 review listed as not made | Rerun when Ninja or `tools/jobpool` changes |
| [bits fault injection](bits_fault_injection.py) | Checks that the unmutated tree builds and passes, then applies sign, bit-order, and range mutations to `bits.hpp`, builds, runs the `core.bits` tests, and restores both files; a build failure counts only if it is a static assertion; `--runtime-only` also drops the test's `static_assert`s so the run-time checks must catch each one | WU2, to show the bits tests can fail | Candidate; a general mutation driver for later core headers may replace it at the M1 review. Last checked at d368a0a with gcc-debug, in both modes: the unmutated tree passes and all 13 mutations are caught, each build failure a static assertion |

The spike files stay where Spikes A and B committed them, so the notes that cite them remain valid. The first coalescing review, at M0, decides which move here.
