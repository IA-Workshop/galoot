# 0240 Rulings on the Fable advisory report

Date: 2026-11-10. The owner answered "accept all recommendations and proceed" to the report at `docs/agent/records/advice/2026-11-10-fable-path-to-1-0.md`. Where this decision restates a recommendation, the report's section 8 has the evidence and tradeoffs.

1. **Reorder decision 0238 item 1.** One pre-release core pass runs before the M7 gate: the inlining change, deleting `detail::inverse_change`, the deleted-`==` hint (0236 item 3), and the two `TODO`s in `mv.hpp`. Then E1, E2, G and H run once, then the M7 gate. Decision 0238 item 1 is amended in that ordering only.
2. **Inlining extent.** One macro (`GALOOT_ALWAYS_INLINE`, defined in `assert.hpp`) on the whole kernel chain, including the `apply` overloads and `detail::sum`. The test is structural: a CTest compiles E1's probe at `-O2 -S` on both compilers and fails on a call or jump to a `galoot` symbol. W10 before and after records on debug and release trees. No user knob unless the debug-build cost is notable.
3. **Documentation lane starts now,** in parallel, with the design study moved to the research notes. The README's status line is brought up to date.
4. **DoD-21** is evidenced by bare `-fno-exceptions` compiles of `bare_user.cpp` in the compile-fail lane.
5. **No `install()` in 1.0.** The README names `add_subdirectory` and `FetchContent`. The `CMakeLists.txt` version is set at release.
6. **`basis_change.hpp` in the `core.hpp` umbrella:** measure its parse cost in part H, then decide.
7. **Keep** the `%` and `!` operators (with the traps taught on the guide's first page) and the strict scalar-type rule for 1.0.
8. **Review proportion,** as practice: delta reviews only for changes under `include/`; test, tools or docs-only changes get one Sonnet review; one blind review per feature branch when its parts are small; a separate test author only where tests judge core arithmetic. No governance text changes.
9. **Header comment trimming:** compiler-version limit catalogues move to a guide page, one sentence each left behind, in the core pass.
10. **`bulk_dual` growth:** recorded as an exception beside 0141, not fixed before 1.0.
11. **WU18 records-only work starts now** (DoD-2 inventory, V10 skeleton, release-record draft, plan closures).
12. **Errata-naming row** in CONVENTIONS section 8 is governance; the owner edits it. Recommendation (add the row, leave 5.8) is recorded with the owner's acceptance; the coordinator will give the exact text in the M7 report.
13. **"S2/S3 unit"** in the brief was a mislabel; no such unit exists.
