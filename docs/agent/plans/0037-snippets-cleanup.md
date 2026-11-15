# 0037 Snippets cleanup after the M5 review, and the README note on shared tags

- Status: closed; the unit merged into `develop` by `72f3c58` on 2026-11-10 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: claimed 2026-11-10 by Claude (agent, Opus 5.5, implementer) from `develop` at `692d661`, on branch `feature/snippets-cleanup`, worktree `.claude/worktrees/snippets-cleanup`.
- Scope: decision 0229 items 1 and 3, carrying out the recommendations of `docs/agent/snippets/reviews/M5.md`.
  1. README, section "Your own algebra, and a change of basis": one sentence saying that a tag copied into a header that several files include goes in a named namespace, not an anonymous one, since otherwise each file gets its own algebra type. No code block changes.
  2. The snippets index (`docs/agent/snippets/README.md`) as the review's "Index changes recommended" table says: remove the three tool rows (mutation driver, cost and diagnostics harness, misuse catalogue scan); mark the table diff superseded by `test/reference/golden.py` and the constexpr plan generator historical for note 0008; relabel the Spike B row (it demonstrates V2 for `clifford_reference.validate`; DoD-14 is met by `faults.toml`); keep the evaluation counter for WU17; record the reruns in the jobserver and plan cost model rows; drop the README's last sentence about the M0 review. The peak-memory row stays, as the precedent the review names.
  3. Path repairs: `spike-a/diff_reference.py` and `spike-b/fault_injection.py` take the root as `parents[5]`, and their docstrings, with `spike-a/run_grid.py`'s, name `docs/agent/records/spikes/`. `plan_cost_model/probe.py` writes the C++ enumerators `geometric_product` and `geometric_antiproduct` for its internal names `gp` and `antigp`.
  4. Reruns, each through `tools/timed`: `cost.py selfcheck` on both compilers' data, `cost.py comments` on `plan.hpp`, and `jobserver_check.py`, with outputs recorded under the snippets' records and the run rows committed.
  5. Delete `docs/agent/snippets/bits_fault_injection.py` (nothing under `include/`, `test/`, `tools/`, or the build names it; the records that cite it are history) and its row.
- Not in scope: the evaluation counter stays (decision 0215 item 10); no limit-search tool is added; no refit of the cost model. If `comments` reports counts the model does not know, the finding is recorded and the remedy is left for the owner unless it is a change to the snippet's own checking that the review asked for.
- Done when:
  - the README examples still build and pass (`rga3.readme`), and the sentence is in the section;
  - each repaired script is shown failing before its repair and working after it: `fault_injection.py` reports all faults caught; `diff_reference.py` reports all tables agree; `probe.py verify` compiles a sample of cases (one or more per kind and product) with no difference from the mirror on both compilers;
  - `selfcheck` and `comments` and `jobserver_check.py` have run, with their outputs and the run rows recorded, and the index rows name the revision;
  - a blind review (Sonnet, blind-reviewer role) is recorded and its findings resolved;
  - the P9 checks: no file under `include/`, `test/`, `tools/`, `cmake/`, or the build files changes, so the matrix is not rerun and the merge cites the latest gate rows; `format-check`, the trace gate, `tests.citations`, and `tools/runlog check` run at the final tip;
  - merged into `develop` with a P13 line.
- Evidence: the scripts' own outputs (V2 for `fault_injection.py`; V1 kind 3 for `diff_reference.py`; the mirror check of `probe.py verify`; the self-check of the cost model).
- Agent cost estimate: about 250k tokens for the implementer and 60k for the reviewer.

## Outcome

Done, 2026-11-10, merged into `develop` (the merge commit names the checks).

- README (`142f700`): one paragraph after the first change-of-basis example says that the examples' tags are in an anonymous namespace because each is a program of one file, and that a tag copied into a header that several files include goes in a named namespace, since an anonymous one gives each file its own algebra type. `rga3.readme` (7 tests) and `tests.citations` pass on gcc-debug and clang-debug.
- Repairs (`6a3cf13`): `probe.py` maps `gp` and `antigp` to `geometric_product` and `geometric_antiproduct`. Before, `verify` of a 72-case sample failed 17 cases on Clang, each "no member named 'gp'" or "'antigp'"; after, 0 on both compilers, and the `test` and `maps` sets give the committed verify files. The blind reviewer ran `verify` on all seven sets at `753a334`: 0 differences. `diff_reference.py` and `fault_injection.py` failed on import before (`ModuleNotFoundError`) and pass after: 28 tables agree on gcc and clang; all three faults caught. `run_grid.py`'s `ROOT` named `docs/` too and was repaired with it (one grid point run by the reviewer in a scratch mirror).
- Reruns at `6a3cf13`: `selfcheck` 0 differences on both compilers' data; `comments` exit 1 on four counts of `product_plan`'s comment that are sizes or a flag value from the scalar-full10-gcc record (recorded in `records/performance/plan-cost-model/README.md`); `jobserver_check.py` gives the results of `b28cc34` with Ninja 1.13.2.
- Index and deletion (`753a334`, review fixes after it): the review's table carried out; `bits_fault_injection.py` deleted.
- Review: blind review by Claude Sonnet 5.5 (`records/reviews/2026-11-10-snippets-cleanup-blind-review.md`), approve with four non-blocking findings, resolved there.
- P9: no file under `include/`, `test/`, `tools/`, `cmake/`, or the build files changed, so the matrix was not rerun; the merge cites the `gate:all` rows at `fe4fb3b`, whose code equals `develop`'s. `format-check`, the trace gate, `tests.citations`, and `tools/runlog check` ran at the final tip.
- V10: the repaired scripts are verified by their own outputs against the committed records; nothing is validated beyond them.
- Open for the owner: `cost.py comments` fails on four counts of `plan.hpp` that are not model costs; the two extra rows of `verify-train-gen-*.tsv`.
