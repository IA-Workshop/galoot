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
| [bits fault injection](bits_fault_injection.py) | Checks that the unmutated tree builds and passes, then applies sign, bit-order, and range mutations to `bits.hpp`, builds, runs the `core.bits` tests, and restores both files; a build failure counts only if it is a static assertion; `--runtime-only` also drops the test's `static_assert`s so the run-time checks must catch each one | WU2, to show the bits tests can fail | Candidate. The general driver now exists (`tools/mutate`, plan 0026); this snippet stays until the bits tests have a mutation spec, and its row then says superseded. Last checked at 52ed97f, after its two `reorder_sign` mutations moved to the set-bit loop: all 13 caught on gcc-debug in both modes and on clang-debug in normal mode. Its `--runtime-only` mode refuses on clang-debug, because removing the `static_assert` lines leaves `all_agree` unused and Clang's `-Wunused-function` fails the baseline build |
| [plan cost model](plan_cost_model/) | `mirror.py` copies the plan and map generators and counts the paths they take; `cost.py` fits the compilers' weights from measured thresholds, predicts and certifies the constant-evaluation cost of a plan or map, prints the figures of `plan.hpp`'s comments and the thresholds of `core.plan_cost_model` (`figures`), and checks itself against the committed data (`selfcheck`); `probe.py` writes and measures the probe units, and `harness.py` finds a threshold by a checked search | Research note 0017; refitted and extended to the antiproducts and maps by plan 0015 (decision 0125, owner ruling, item 3) | In use: rerun `probe.py` and `cost.py fit` whenever `core.plan_cost_model` fails, the compiler or standard library changes, or a generator changes, then regenerate the figures, the comments, and the canary's thresholds. Every limit search on GCC is the checked search of `harness.py` (lesson 0064). Data and commands: `records/performance/plan-cost-model/`. Last checked at `cd7dcf2` (headers of `4895a40`): `selfcheck` passes for both compilers |
| [mutation driver](../../../tools/mutate) | Runs a record's mutants from a TOML spec on each build preset and reports each as killed, equivalent, survived, or unjudged, with the stale-binary rule and run-time mode built in | Plan 0026, replacing the drivers copied into `records/mutations/` (M4 snippets review item 5) | In `tools/`, tested by `tools.mutate`; not a snippet. The copied drivers stay as records of their runs. Specs: `records/mutations/0023-rga3/mutants.toml`, `0011-blade-product/mutants.toml`, `0026-tools/tools.toml` |
| [cost and diagnostics harness](../../../tools/compiles) | `cost`: pinned before-and-after compiles of a spec's units into a campaign log; `diagnostics`: lines and errors of compile-fail sources on both compilers; flags from the build trees | Plan 0026, replacing the `measure.py` and `diagnostics.py` copies (M4 snippets review items 6 and 7) | In `tools/`, tested by `tools.compiles`; not a snippet. Growth units and run-time timing are WU17's. Spec: `records/performance/rga3-cost/units.toml` |

The spike files stay where Spikes A and B committed them, so the notes that cite them remain valid. The first coalescing review, at M0, decides which move here.
