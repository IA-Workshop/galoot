# Plan 0040 mutants: the forced inlining (part A) and `basis_change` (part B)

Plan 0040 section 8. Run by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-10 with `tools/mutate`, on gcc-debug (g++ 16.2.1) and clang-debug (clang++ 23.1.4, both with libstdc++ 16), at `d382516` (part A2) for part A and at `09bebcd` (part B3) for part B, in the `--code` worktree, with only records dirty. Rows: `out/<spec>-<preset>.tsv`; the runs are the rows `mutant:<name>`, `mutant:baseline` and `mutant:restore` of `records/performance/runs.tsv`. The job pool read 23 of 23 slots free before the first runs; before `gap.toml` it read 0 of 23 free, with two other workstreams' `ctest` as clients, so its wall times are those of a loaded pool.

`rga3.kernels_inline` compiles its probe with fixed flags (`-O2`, checking off), so a tree's preset matters to the attribute mutants only for its compiler; their selection's one target is the probe's object library, built with the tree's flags, so that a mutant breaking the build would be classified.

## Summary

| Spec | Mutants | gcc-debug | clang-debug |
| --- | --- | --- | --- |
| `attributes-gcc.toml`, `attributes-clang.toml` | 13 each | 3 killed, 8 survived, 2 equivalent | 5 killed, 6 survived, 2 equivalent |
| `check.toml` | 3 | 3 killed | 3 killed |
| `gap.toml` (the suite without the check) | 1 | survived, as declared | survived, as declared |
| `gap-gcc.toml`, `gap-clang.toml` (the check alone) | 1 | survived, as declared | killed, as declared |
| `basis-change.toml` | 3 | 3 killed | 3 killed |

None was unjudged. `tools/mutate` exited 1 on both attribute specs, for the four kills below that the plan expected to survive; every other verdict is as declared.

## The gap (W3)

`macro_empty` defines `GALOOT_ALWAYS_INLINE` as nothing, so that no function of the chain is forced. The suite of the core and rga3 modules without `rga3.kernels_inline` lets it live on both presets (`gap.toml`): no other test reads where the library's code is placed, and the values do not change. `rga3.kernels_inline` alone kills it on clang-debug (`gap-clang.toml`), with E1's five calls back. So the mutant counts for the gap on Clang. On gcc-debug the check lets it live too (`gap-gcc.toml`), as the plan expected: without any attribute GCC inlines all eleven probe functions, the four the probe adds to E1's seven included.

## Each attribute removed alone

| Mutant (attribute removed from) | gcc-debug | clang-debug | Plan's expectation |
| --- | --- | --- | --- |
| `apply_not_forced` (binary `apply`, target given) | killed | killed | killed on both |
| `run_product_not_forced` | survived | killed | killed on Clang |
| `transform_not_forced` | **killed** | killed | killed on Clang; survives on GCC |
| `run_map_not_forced` | survived | survived | survives |
| `unary_apply_not_forced` (unary `apply`, target given) | survived | survived | survives |
| `run_sum_not_forced` | survived | survived | survives |
| `sum_not_forced` | survived | survived | survives |
| `run_change_not_forced` | survived | survived | survives |
| `transform_middle_not_forced` | survived | **killed** | survives |
| `transform_outer_not_forced` | survived | survived | survives |
| `compose_not_forced` | **killed** | **killed** | survives |
| `apply_result_layout_not_forced` | equivalent | equivalent | equivalent |
| `unary_apply_result_layout_not_forced` | equivalent | equivalent | equivalent |

The calls that come back in the four unexpected kills, found again with scratch copies of the headers carrying each mutant's edit and the check run by hand on the probe:

- GCC, `transform` not forced: `call transform` from `transform_point`, `transform_line` and `transform_plane`. Without any attribute GCC inlines `transform`. Inferred, not traced in GCC's inliner dumps: with the functions below it forced into it, `transform`'s body grows past what GCC's heuristic inlines.
- GCC, `compose` not forced: `call compose` from `compose_motors`, for the same reason.
- Clang, `transform_middle` not forced: `callq transform_middle` from `transform_line`.
- Clang, `compose` not forced: `callq compose` from `compose_motors`.

By the plan's reading, a kill of a mutant declared to survive is evidence that the attribute is needed now, not a failure: four of the eight attributes the plan held to be guards are load-bearing on at least one compiler at `-O2`, because forcing a function inline moves its callers' inlining budget, as the advisory report's experiment found for `apply`. The surviving six (`run_map`, the unary `apply`, `run_sum`, `sum`, `run_change`, `transform_outer`) stay as guards. The two `apply` overloads that take no target layout have no caller in the library, so no probe reaches them: equivalent, as declared.

## The check's own mutants

`check_ignores_jmp` (the branch pattern loses `jmp` and `jmpq`), `check_ignores_plt` (the symbol must end the operand, so `@PLT` and `@GOTPCREL` forms are missed), and `check_skips_missing` (a missing function is not reported) are each killed on both presets by the fixture test written for it: `tools.kernels_inline_rejects_jmp`, `_rejects_plt`, `_rejects_missing_function`.

## Part B

| Mutant | Edit | Killed by, both presets | Written for |
| --- | --- | --- | --- |
| `inverse_rows_sign` | entry (0, 0) of the checked inverse negated | `core.basis_change_identities`, `core.basis_change` | the identities test, which now reads `check_change`'s inverse |
| `equality_hint_removed` | the deleted `==` removed | `core.basis_change_equality`, `core.basis_change_inequality` (the generic error, without the hint) | the two compile-fail tests |
| `equality_defaulted` | `= default` in place of `= delete` | the two compile-fail tests (they compile), and the `static_assert` of `basis_change_test.cpp:187`, `!std::equality_comparable` | the same, and the type's guarantee |

Each new or changed test is among the killers of the mutant written for it.

## Part D1: the hand mutants of `cost.py comments`

`comments_test.py` is not a CTest test, so `tools/mutate` cannot judge these; `cost-comments/run.py` runs them, each as one edit of `cost.py` in a temporary copy of `docs/agent/snippets/plan_cost_model/` under the checkout's git-ignored `build/` (inside the checkout, because `harness.py` asks git for the root), never in the snippet itself. The mutants are those implementer 2 ran by hand for the D1 commit (`99beea3`), in place with a saved copy, with the same anchors and edits. Run again by implementer 1 at `7a27f76` through `tools/timed` (selection `cost-comments-hand-mutants`, exit 0); rows in `out/cost-comments.tsv`.

| Mutant | Edit | Verdict | Killed by (among others) |
| --- | --- | --- | --- |
| `control_unmutated` | none | survived, as a control must | none |
| `section_ignored` | the whole page read | killed | `test_unlisted_count_outside_the_section_passes`, the plan's `section_ignored` |
| `missing_section_accepted` | the refusal of a page without the section removed | killed | `test_page_without_the_section_is_refused` |
| `twice_accepted` | the refusal of a section named twice removed | killed | `test_section_named_twice_is_refused` |
| `empty_section_accepted` | the refusal of a section with no count removed | killed | `test_section_without_a_count_is_refused` |
| `markdown_without_section_accepted` | a Markdown page read without `--section` | killed | `test_markdown_page_without_a_section_is_refused` |
| `fence_ignored` | a heading inside a code block ends the section | killed | `test_heading_in_a_code_block_does_not_end_the_section` |
| `section_never_ends` | the section runs to the end of the page | killed | `test_unlisted_count_outside_the_section_passes` |
| `subsection_ends_section` | a deeper heading ends the section | killed | `test_count_in_a_deeper_heading_of_the_section_is_read` |

Each refusal and each rule of the section's extent is killed by the test written for it, as in `99beea3`'s message.

## After the blind review's M1: every attribute again, and the check's new rules

The blind review found that the forced chain left the public products out of line on GCC, where they had been inlined before. The fix forces eleven public functions too, and the structure check gains a second test, `rga3.kernels_inline_alone`, which compiles each of the probe's 48 functions in a unit of its own. Run by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11 with `tools/mutate` at `d47e8f9` (the attributes; the tests at `3a74650`), in the `--public-ops` worktree, with only records dirty; pool 23 of 23 free before and after.

`attributes-public-gcc.toml` and `attributes-public-clang.toml` remove one of the 24 attributes at a time, judged by both structure tests. The thirteen of part A keep their first run's verdicts as expectations. The eleven new ones expect a kill where a structure test showed the call on that compiler.

| Attribute removed from | gcc-debug | clang-debug | First run (old probe), gcc / clang |
| --- | --- | --- | --- |
| `run_map` | survived | survived | survived / survived |
| `run_product` | **killed** | killed | survived / killed |
| binary `apply` with a target | killed | killed | killed / killed |
| unary `apply` with a target | survived | survived | survived / survived |
| `run_sum`, `sum`, `run_change`, `transform_outer` | survived | survived | survived / survived |
| `transform_middle` | survived | killed | survived / killed |
| `compose` | killed | killed | killed / killed |
| `transform` | killed | killed | killed / killed |
| `geometric_product`, `wedge`, `geometric_antiproduct`, `antiwedge`, and `*`, `^`, `%`, `&` | killed | killed | new |
| `weight`, `left_weight_dual`, `antidot` | killed (by the alone test only) | survived, as declared | new |
| the two `apply` overloads with no target layout | equivalent | equivalent | equivalent |

The only verdict that differs from its expectation is `run_product` on GCC, now killed (so `tools/mutate` exits 1 on the GCC spec): with the public products forced, GCC keeps `run_product` out of line when it alone is left unforced. Each of the eleven new attributes is killed on GCC, so each is needed on at least one compiler; none was added that a test did not show. Of part A's thirteen, `run_map`, the unary `apply`, `run_sum`, `sum`, `run_change` and `transform_outer` survive on both compilers and `transform_middle` on GCC; they stay as guards.

`check-review.toml` holds the mutants of the check's rules added after the review's m2 and M1, and `check.toml`'s three with their anchors rewritten for the script as it is now. On both presets each is killed by the fixture test written for it, except `check_keeps_overwritten_register`, declared equivalent there. That was wrong: the mutant changes behavior (it reports a call after the register holding a library address is overwritten), and no fixture killed it, so it marked a gap in the tests, not an equivalent mutant (the delta review's D6). The next section closes it.

## After the delta review's D1 to D6: 26 attributes, and the check's mutants again

The delta review found `project` and `antiproject` called out of line by GCC on rga3's full multivectors and motors once `wedge` and `antiwedge` were forced, and Clang calling them there at every revision (its D1); the fix forces both, and the probe gains four projections. Run by Claude (agent, Opus 5.5, implementer 1 of plan 0040) on 2026-11-11 with `tools/mutate` at `b5d7ad0` (the specs committed at `8ba0e08`), in the `--public-ops` worktree, with only records dirty; pool 23 of 23 free before and 22 of 23 after.

`attributes-d1-gcc.toml` and `attributes-d1-clang.toml` remove each of the 26 attributes alone, judged by both structure tests, with the run before's verdicts as expectations and a kill expected for `project` and `antiproject`. Every verdict is as declared (`tools/mutate` exits 0 on both):

- gcc-debug: 17 killed, among them `project` and `antiproject` (by both structure tests) and `weight`, `left_weight_dual` and `antidot` (by `rga3.kernels_inline_alone`); 7 survive (`run_map`, the unary `apply`, `run_sum`, `sum`, `run_change`, `transform_middle`, `transform_outer`); the two `apply` overloads with no target layout equivalent.
- clang-debug: 15 killed, `project` and `antiproject` among them; 9 survive (those six without `transform_middle`, and `weight`, `left_weight_dual`, `antidot`, which were added for GCC); 2 equivalent.

`check-d1.toml` holds `check-review.toml`'s ten with their anchors rewritten for the script as it is now, and five for the rules added after the delta review: `check_ignores_immediate`, `check_ignores_register_width` (both killed by `tools.kernels_inline_rejects_immediate`), `check_ignores_notrack` (`_rejects_notrack`), `check_refuses_bare_return` (`_accepts_bare_return`), and `alone_accepts_twice` (`_alone_rejects_twice`, the branch of `--alone` that had no test). `check_keeps_overwritten_register` is now declared killed, and `_accepts_overwritten_register` kills it. All 15 are killed on both presets, each by the test written for it.

## After the second delta review

The per-output refusal of assembly that names no x86-64 register is dropped from the script (the second delta review's E2), since the build registers the structure tests only where the target is x86-64. Two mutants of `check-d1.toml` were of that refusal, `check_accepts_other_target` and `check_refuses_bare_return`; their code is gone, so their anchors no longer match and the spec is the record of its run at `b5d7ad0`, not a spec to run again. The new fixture `cet.s` (two bare returns built with `-fcf-protection`, and the numeric labels of `.note.gnu.property`) guards against a refusal or a label rule that misreads such output; no mutant is written for it, since the script now has no rule that reads those lines. No attribute changed in this round, so the attribute mutants and the W10 campaign were not run again.

## After gate 2: the opt-out of the forced inlining (plan 0040 section 18)

`opt-out.toml` holds a mutant for each new test of `GALOOT_FORCE_INLINE`, run with `tools/mutate` at `20fa092` on gcc-debug and clang-debug (selection `^core\.assert$`; `out/opt-out-<preset>.tsv`). All three are killed on both presets, each by the test it was written for: `opt_out_ignored` (the mode not read, so the attribute stays on with 0) by `core.force_inline_off`; `opt_out_always` (the attribute never on) by `core.force_inline_default` and `core.force_inline_build`; `value_unchecked` (the `#error` removed, so 2 reads as 0) by `core.force_inline_value`. The opt-out of the presets themselves, a CMake setting, is not mutated: `core.force_inline_build` checks it in each tree where it runs, and passed under clang-release and gcc-asan with the mode 0.

`opt_out_always` is the gap mutant `macro_empty` of the first section in another form. It now has killers on both compilers, the two programs that check the expansion directly, where `macro_empty` had only `rga3.kernels_inline` on Clang; the earlier verdicts stay as recorded for their revisions.

## After the delta review of section 18: the mode's spelling

`opt-out-spelling.toml` holds two mutants of the spelling check that now reads `GALOOT_FORCE_INLINE`, run by Claude (agent, Opus 5.5, merge-gate runner) on 2026-11-11 with `tools/mutate` at `4b87d8c` on gcc-debug and clang-debug (selection `^core\.assert$`; `out/opt-out-spelling-<preset>.tsv`; the revision is marked dirty because the spec and the run log had uncommitted edits, and no file a compile reads had any). The pool read 23 of 23 slots free before and after. Both are killed on both presets, each by the tests written for it:

- `spelling_unchecked` (the numeric check of before, which reads `ON` as 0 and lets an empty value through to the preprocessor's own error) by `core.force_inline_on` and `core.force_inline_empty`.
- `cascade_after_error` (the attribute left undefined after the `#error`, so that every header that uses it adds errors of its own) by `core.force_inline_on`. It was added at the merge gate for the second delta review's m1, after that test came to include the whole core (`4b87d8c`). Before, the test included only `assert.hpp`, which does not use the attribute, so this mutant would have survived it; that is inferred from the test's source, not run.
