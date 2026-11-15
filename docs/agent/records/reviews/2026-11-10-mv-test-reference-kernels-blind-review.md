# Blind review of plan 0033, the safe form in mv_test.cpp's reference kernels

- Reviewer: blind-reviewer role (a general-purpose agent briefed with `.claude/agents/blind-reviewer.md`), Claude Sonnet 5.5, 2026-11-10 UTC, launched by the author (Claude Opus 5.5, cpp-expert) with no share of its context. Sonnet rather than Opus because the change is three declarations of a test file.
- Range: `0ca88bf..386383a` on `feature/core-kernel-init-tests-ref`, parent `develop`.
- The brief carried no verdict: it gave the acceptance criteria, the author's one reading (`counting` counted among the "reference kernels"), and the checks the author ran, which the reviewer took as claims.
- Files: the reviewer's probe `2026-11-10-mv-test-reference-kernels-blind-review/p.cpp` and its campaign log `runs.tsv` (the probe compiles and the reviewer's own configure, build, and `core.mv` runs on gcc-debug and clang-debug, in a worktree detached at `386383a`, since removed). The reviewer's record follows as it returned it, edited only for layout.

## Outcome

Approve. No finding blocks.

## Acceptance criteria

1. **Met.** `counting` (`mv_test.cpp:118`), `product_by_loop` (`:1061`), and `map_by_loop` (`:1074`) read `std::array<T, N> x = std::array<T, N>();`, the `T t = T()` form the research record finds compiled correctly. No multivector local changed. The other `{}` value-initializations of arrays in the file are multivector locals (excluded), `std::array<unsigned char, …> storage{}` (all zero bits), and `std::vector<T>(blades, T{})` in `composed_product` (a heap vector filled from one value, not a shared array initializer; the record does not list it).
2. **Met.** Only the three declarations and their comments change; no expected value, `CHECK`, case, or tolerance. One code commit; the run-log rows are those P13 requires.
3. **Met.** The comments state why in terms of the compiler defect and cite no provision, plan, decision, or agent record.
4. **Met.** `core.mv` passed on gcc-debug and clang-debug in the reviewer's worktree, through `tools/timed`: 84 doctest cases, 470 assertions, on the gcc-debug program.

## Checks

- Defect probe (`p.cpp`, g++ 16.2.1): a templated form of the record's `minimal.cpp`, a struct whose zero is {0, 1}, a constant evaluation writing index 3 of 5, and a run-time call. `std::array<q, N> a{}` exits 1 (defect present) at -O0 and -O2; `std::array<q, N> a = std::array<q, N>()` exits 0 at both, in a template too.
- The convention trace gate: exit 0.
- Changed lines within 80 columns; plan 0033 claimed on `develop`; the commit trailer names the model.

## Not checked

- gcc-release, clang-release, the lint build of `core_mv_test`, format-check (the author's rows record exit 0; their revision column reads `0ca88bf2208c-dirty`, the tree before the commit, and the reviewer did not diff the committed file against it).
- No mutant restoring `{}`: no test sees these sites (none was damaged in the suite), as expected for a latent-exposure change.
- No wider selection; no other flags or GCC versions; the usability checklist does not apply (no public header).

## Findings (none blocks)

1. `counting` (`:118`) writes every place before any read, so it is not exposed in practice; the change there is harmless, and including it matches the research record's list and the claim.
2. The `counting` comment is long for what it says, but it gives the reason a maintainer would otherwise undo the change; keep it.
3. The three sites are protected only by their comments; a canary rule for such sites is a separate item, out of scope.

## Resolution (by the author)

- Findings 1 to 3: no change; each is an observation the reviewer accepts.
- On criterion 4's count: the author's "100 of 100" is the CTest count of `ctest -L '^core\.mv$'`, which selects the 100 tests labeled `core.mv` (the program `core_mv_test` and the label's other tests); the reviewer ran `-R '^core\.mv$'`, the one test of that name.
- On "not checked", the revision column: the author's rows were taken on the tree whose only uncommitted change was `test/core/mv_test.cpp` as then committed in `386383a`, with no edit between the runs and the commit (inferred from the order of the session's commands; not verifiable from the rows).

Review cost: 73,547 tokens, 13 tool calls, 74.5 s, as the harness reported.
