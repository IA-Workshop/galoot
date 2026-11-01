# W5 delta review: feature/core-metric, b12f07d to 0e2d3ef (WU3, plan 0005)

- Reviewer: blind-reviewer, Claude Opus 5.5, 2026-10-31, in a detached worktree of its own (removed after use; its 73 run rows are in the session scratchpad as `review-wu3b-runs.tsv`).
- Reviewed: `b12f07d..0e2d3ef`. The branch moved during the review to `22983f6`, after `911b4e8` (a merge of develop) and `22983f6` (the tests compared with the golden values and marked); those two commits were not reviewed, and `metric.hpp` and `.clang-tidy` are unchanged since `0e2d3ef`.
- Verdict: merge after fixes. D1 is covered by the metric implementer's later pass but needs its own delta review and P9 on the merged head; D2 is small and required; D3 and D4 are recommended.
- Not run: gcc-release, gcc-asan, clang-debug, clang-release (the author's record shows 19 of 19 at ed2a3e2); clang-tidy on a TU that includes both headers. Not checked: the content of the lesson file, decision numbering, and the three-size cost measurement (not done by the author).

## Checks

- Clean P9 builds at 0e2d3ef: gcc-debug 5.67 s, clang-asan 6.43 s, lint 3.39 s; 19 of 19 each; lint clean.
- All 94 Gram matrices re-derived independently (83 splits of R(p,q,z) for n up to 6, rigid(0..5), conformal(0..4)) and checked by generated `static_assert`s on both compilers: 0 mismatches; a flipped-entry control fails. The change of basis holds exactly in rationals for d = 0 to 4.
- `R<P,Q,Z>()` compiles and reads correctly with `-Wshadow -Wconversion -Werror`; a negative count fails to compile on both compilers.
- Every count-overflow and wrap case is rejected on both compilers with the right message; the entry bound is 32767 for `int`, exact (2k² ≤ INT_MAX < 2(k+1)²), `isqrt` exact over three ranges, no bisection overflow, and `long long` gives 2147483647.
- A downstream `template <metric_like Alg>` accepts `R<18,1,1,20>`, `conformal<30,32>`, `rigid<31,32>`, `from_gram<1,17>`, and a hand-built `metric<32>`; n = 32 is accepted and n = 33 is rejected in every form.
- The `metric_like` guard returns false on both compilers for a lookalike, a private `value`, a static member function named `value`, a non-static member (n = 1, 2), and a non-constexpr static with n = 2; it fails for a non-constexpr static with n = 1 (D3).
- The `mask_t` redeclaration compiles and runs on both compilers in both include orders with `bits.hpp` from core-bits at c9d9cad.
- Fifteen header mutants and three fail-test mutants: 13 header mutants killed on both compilers; one survivor is equivalent (`<` for `<=` in `isqrt`) and one is a real gap (D4); each fail test fails for its stated reason when its check is removed and an unrelated `static_assert(false)` is added.
- `.clang-tidy` adds only the two bounds exclusions; the remaining suppressions carry reasons on the same line apart from one. Tests and code are in separate commits. No added line cites a provision or agent work.
- Trial merge of develop (8327147): conflicts in `test/CMakeLists.txt` only (keep both); gcc-debug passes 29 of 30 with the marker check failing on nine unmarked test cases (D1).

## Findings

- D1 (blocks). Develop's marker check reports nine test cases without markers at 0e2d3ef. The later commits address this and need their own delta review and P9 on the merged head.
- D2 (small, required). The two new `.clang-tidy` exclusions carry no reason, against the file's own first line and decision 0035 item 3.
- D3 (recommended). `metric_like` is only partly fixed for an unusable `value`: the guard reads no entry when n = 1, so a non-constexpr static `metric<1>` is a hard error. Guard on `std::bool_constant<Alg::value.entries_in_range()>`, which reads every entry for n ≥ 1; checked in a scratch copy on both compilers. Add an n = 1 test.
- D4 (non-blocking). The `static_assert(D <= MaxN)` in `conformal` is untested: without it `conformal<SIZE_MAX>()` is still rejected, but with raw text. Add a compile-fail test.
- D5 (nits). The tests hardcode `int` instead of `core::entry_type`; one `NOLINT(*-avoid-c-arrays)` has no same-line reason; counts that wrap to zero also print a misleading "n = 0" message; raising MaxN in `from_gram<N, MaxN>` means spelling N; after the bits merge, `metric.hpp` should use `mask_bits` from `bits.hpp`.

Earlier findings: B1, N2 to N7, N9, N10 resolved; N1 partly (D3); N8 present, lesson file not read; N11 not done; N13 comment only; N12 superseded by D1. Governance note: when an interface changes, W9 (tests in a separate commit) and W2 (building at every step) cannot both hold; the author chose W9 over W2 at 708c5dc and c8dfa51.
