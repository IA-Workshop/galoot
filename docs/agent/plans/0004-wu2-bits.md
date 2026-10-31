# 0004 WU2: bits

- Status: claimed; branch `feature/core-bits`, worktree `.claude/worktrees/core-bits`
- Goal: `include/galoot/core/bits.hpp`, the constexpr functions on blade masks that every later part uses.
- Content: `mask_t` (`std::uint32_t`, decision 0004), `grade`, `antigrade(mask, n)`, `reorder_sign(a, b)`, the complement mask, the right and left complement signs (CONVENTIONS 3.1), and the reverse, antireverse, and grade-involution signs (2.1). All in `galoot::core`, usable at compile time and at run time (decision 0021 and note 0008).
- Definition of done: the functions exist with `///` comments (S11) and are self-contained (the header-set check); tests pin CONVENTIONS 1.1, 1.5, 2.1, and 3.1.
- Acceptance criteria: (1) every function is checked exhaustively against its definition for n up to 6, by a brute inversion count written in the test and sharing no code with the header (V1 kind 1); (2) the basis-rule functions (reorder sign, complement sign) are checked exhaustively up to n = 8; (3) u ∧ ū = 𝟙 and u̲ ∧ u = 𝟙 hold for every blade, and left(right(u)) = u; (4) the 16-entry table of 3.1 for n = 4 is pinned as golden values derived from the definition; (5) functions reject out-of-range input by `GALOOT_ASSERT` in checking builds; (6) clang-format, clang-tidy, and the include scan pass; (7) the unit's tests pass on both compilers in the correctness presets and the lint preset.
- Approach: direct form first (C3, W4); no table is precomputed. Tests in `test/core/bits_test.cpp`, labeled `core.bits`.
- Evidence: test output per preset, the exhaustive ranges, and the P1 before and after record of the affected suites (W10).
- Agent cost: tracked for information only (decision 0019).
- Review: W5 blind review before the merge into `develop`.
- Open questions: none.
