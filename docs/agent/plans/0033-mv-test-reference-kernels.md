# 0033 The safe form in mv_test.cpp's reference kernels

- Status: claimed 2026-11-10 by Claude (agent, Opus 5.5, cpp-expert) from `develop` at `5edf616`, on branch `feature/core-kernel-init-tests-ref`, worktree `.claude/worktrees/core-kernel-init-tests-ref`. A test-only unit, so no full W3 plan; this entry claims it (W7).
- Scope: decision 0226 item 2. The value-initialized arrays of the scalar in `test/core/mv_test.cpp` that the GCC scan listed as latent (`counting`, `product_by_loop`, `map_by_loop`) take `T t = T()`, which GCC 16.2.1 compiles correctly. Multivector locals are not touched: their storage already has one initializer per place.
- Done when: the change is one commit; `core.mv` passes on gcc-debug, clang-debug, gcc-release, and clang-release; `format-check` and `tools/runlog check` pass; a blind review of the diff by an agent that shares no context with the author (W5; a test-only change, so a short one); merged into `develop`.
