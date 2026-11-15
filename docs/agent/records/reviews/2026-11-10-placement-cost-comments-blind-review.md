# Blind review of plan 0034, the placement cost of a plan in the comments

- Reviewer: blind-reviewer role (a general-purpose agent briefed with `.claude/agents/blind-reviewer.md`), Claude Sonnet 5.5, 2026-11-10 UTC, launched by the author (Claude Opus 5.5, cpp-expert) with no share of its context. Sonnet rather than Opus because the change is comments only.
- Range: `develop..5baa462` on `feature/placement-cost-comments`, parent `develop` at `8bd4efb`: `70f91fa` (comments) and `5baa462` (run rows).
- The brief carried no verdict: it gave the ruling's goal, five acceptance criteria, and the author's departures (a: only `core.mv` on gcc-debug and clang-debug, format-check, the trace gate, and `tests.citations`, not the full P9 matrix; b: the Clang flag `-fconstexpr-steps` named without a value; c: "GCC 16.2.1" beside "GCC 16" and "GCC 16.2").
- Files: the reviewer's campaign log `2026-11-10-placement-cost-comments-blind-review/runs.tsv` (its configure and build rows and its `-fsyntax-only` probes, in a worktree detached at `5baa462`, since removed). Its `ctest` runs of `core.mv` have no rows there; the record states their results without a timed row. The reviewer's record follows as it returned it, edited only for layout.

## Outcome

Approve with non-blocking findings.

## What was checked and what was not

- Not run: the P9 matrix (gcc-release, clang-release, sanitizers, clang-measure), accepted for a comment-only diff whose comment-stripped headers are identical; `tests.citations`, recorded as passing on `70f91fa`, with only `runs.tsv` changed after it; a value of `-fconstexpr-steps` for the `full<8>` case (the comment gives none); Clang on the plan alone beyond the record's n = 7 to 9; a recount of the 89,102,056 operations (read in `results/bisect-unit2.tsv` and `.txt`); other products and metrics for the scalar times `full<10>`.

## Acceptance criteria

1. **Comment-only: met.** Every changed line under `include/` begins with `///`; the comment-stripped output of `g++ -fpreprocessed -dD -E -P` of both versions of `plan.hpp` and `mv.hpp` is byte-identical; the only other file is the run log, 18 appended rows. The first six rows, at `8bd4efb9ba9f-dirty`, record a build exit 1 and a test exit 8, a first attempt rerun successfully.
2. **Claims against the evidence: met, with the findings below.** In the record's evidence directory: 1,024 terms on 1,024 blades (`runtime-check-gcc.txt`); the failure at GCC 16.2.1's default limit (`debug-n10.txt`, the span wording; `release-n10.txt`, the limit wording; `product.cpp` is the geometric product in R(10,0,0)); about 89 million operations with assertions and the cache on (`bisect-unit2.tsv`, `bisect.sh`); the build at `-fconstexpr-ops-limit=134217728` (`debug-n10-ops-2p27.txt`, and 2^27 + 1), at `-O0` with assertions only; Clang 23 failing the scalar times `full<8>` (`plan-clang-debug-n7.txt` builds, `-n8` and `-n9` fail on the step limit), for the plan alone at `-O0` with assertions; the span wording with every index in range (`debug-n10.txt`, section 3.2). `full<6>` times `full<6>` and "mv on full<8> builds on Clang" are existing header text. The reviewer's own probes: GCC at `-O2` without assertions builds the unit at 2^27 and 2^28; the reversed order `full<10>` times the scalar fails at the default limit with the span wording and builds at 2^27; Clang 23 on the whole `mv` unit builds n = 7 at `-O0` with assertions and at `-O2`, and fails n = 8 at both.
3. **No contradicting comment: met.** Every size or limit statement in `include/galoot/core/` was read (`product.hpp:45-51` and `346-359`, `dual.hpp:77-90`, `linear.hpp:104-111`, `blade_set.hpp:575-605`, `mv.hpp:941`, `transform.hpp`, `basis_change.hpp`); none concerns a small set times a large one or implies the two cases build.
4. **STYLE S11: met.** No citation of decisions, plans, records, notes, reviews, provisions, or agents; plain American English; readable by a library user.
5. **Mechanical checks: met.** `core.mv` 100 of 100 on gcc-debug and clang-debug; `format-check` exit 0; the trace gate exit 0 with its line "items with no test: 6.1, 6.2, 6.3, 6.4", not compared against `develop`.

Departures: (a) accepted for a comment-only diff; (b) correct usage, exercised in the record (`plan-clang-debug-n10-steps-2p30.txt`) and named already in `product.hpp:354`; (c) see finding 3.

## Findings, most severe first (none blocking)

1. **`plan.hpp` near 620-626, `mv.hpp` near 893 (nit, unbacked as written).** The remedy follows "measured with _GLIBCXX_ASSERTIONS and GCC's cache of constant calls on", but the record has the 2^27 build only at `-O0` with assertions. The reviewer's `-O2` probe shows the claim true. The cache-off count is not measured; the comment says "cache on".
2. **`mv.hpp`, `apply` near 886-896 and `mv` near 490-493 (nit, scope).** "The plan of the scalar times full<10>" names neither the operation nor the metric; the evidence is the geometric product in R(10,0,0).
3. **`plan.hpp` (nit, wording).** "GCC 16.2.1" beside "GCC 16.2" and "GCC 16"; the 89 million figure is the unit's largest evaluation, the typing; in `apply`, "builds the first on GCC" is ambiguous.
4. **Process: none.**

## Resolutions (author)

All in `16b4d56`, comments only, so no delta review (W5); the comment-stripped headers are again byte-identical to `develop`'s, and `core.mv` (100 of 100) on gcc-debug and clang-debug, `format-check`, and `tests.citations` passed on it.

1. Fixed. The author reproduced the `-O2` build at 2^27 (run-log row `placement:release-n10-ops-2p27`, exit 0, with `placement:release-n10-default` failing on the limit wording), and the comment now says the flag builds the product at `-O0` with `_GLIBCXX_ASSERTIONS` and at `-O2` without.
2. Fixed. `apply`'s and `mv`'s comments name the geometric product in R(n,0,0).
3. Fixed in part. The 89 million figure is now "the largest evaluation of the scalar's plan, the typing of its terms" (the typing is part of the plan's generation, so it is one of the plan's own evaluations); `apply` says "builds the product with full<10> on GCC". The version stays 16.2.1 where the new sentences state measurements, as the record's compiler is 16.2.1.
