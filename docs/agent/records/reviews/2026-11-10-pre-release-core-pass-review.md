# Blind review: plan 0040, the pre-release core pass

- Reviewer: blind-reviewer (Claude, agent, Opus 5.5), 2026-11-10 to 2026-11-11 UTC.
- Range: `feature/pre-release-core-pass`, branch point `8058589`, tip `721afe7`. Review worktree `.claude/worktrees/review-pre-release-core-pass`, branch `review/pre-release-core-pass` from the tip.
- Given: the diff (`include`, `test`, `tools`, `docs/agent/snippets`), plan 0040 sections 1 to 5, and the governance. The brief carried no verdict. The diff itself holds the implementers' decision 0244 and their records. I read them after forming the findings below, and I checked their claims rather than relying on them.
- Run rows: [`2026-11-10-pre-release-core-pass-review/runs.tsv`](2026-11-10-pre-release-core-pass-review/runs.tsv) (every build, test and compile through `tools/timed`).

## Outcome

**Block**, on finding M1. The change is correct as C++ and the comment trims are clean. However, the forced inlining stops short of the public operations. Because of that, GCC at `-O2` now calls out of line some rga3 operations that it inlined before the change, and the structure test does not see it. M1 needs a code change under `include/`, so it needs a delta review. Every other finding can be fixed in comments or records, or is a note for the owner.

## Departures and checks not made (C20)

- I ran the full suite on gcc-debug and clang-debug only: 575 of 575 pass on each, at `721afe7`, from new trees in my worktree. I did not run gcc-release, gcc-asan, clang-release, clang-asan, lint (clang-tidy), gcc-measure or clang-measure. The P9 matrix from empty trees at the final tip is still owed (plan step 6), and no run log row shows it at `721afe7`.
- I ran one W10 unit, not the whole campaign (see below).
- I did not reread the documentation lane's page beyond its compile-time limit sections, its "Raising the limits" section and its rounding-check section.
- I did not review `comments_test.py` line by line. I read `cost.py`'s diff and the hand-mutant table.
- I made no web request and contacted no third party. Tokens used by this review are not visible to me.

## Checks made

| Check | How | Result |
| --- | --- | --- |
| Full suite | `tools/timed` build and `ctest --preset` on gcc-debug, clang-debug | 575/575 on each |
| Convention trace gate | `python3 -I tools/test_references.py trace ...` | exit 0 |
| `tests.citations`, `format-check` | gcc-debug tree | pass |
| Only comments changed, apart from the stated code | Comments stripped and every header tokenized (`g++ -fpreprocessed -E -P`). Also a full preprocess of `core.hpp` and `presets/rga3.hpp` on g++ and clang++, with checking 0 and 1, tokenized and diffed `8058589` against `721afe7` | The only token changes are the macro block, 13 `GALOOT_ALWAYS_INLINE` (in expanded form, 13 `[[gnu::always_inline]]`), the deleted `operator==`, the deletion of `inverse_change`, and the `__FILE__`/`__LINE__` values that `GALOOT_ASSERT` expands to. The result is the same on both compilers and both checking modes |
| `-pedantic-errors` | the probe at `-O0` and `-O2`, checking on, S4 warnings, on g++ 16.2.1 and clang++ 23.1.4 | compiles. `[[gnu::always_inline]] [[nodiscard]]` before `constexpr` is a valid attribute-specifier-seq |
| Every removed passage is recorded | script: every removed `//` or `///` line of `git diff -U0 8058589..721afe7 -- include` against the code blocks of `limits-removed.md` | Every catalogue line is present. The 23 removed lines that are not in the record are the `inverse_change` comment, the type's old "has no ==" sentence, the two `TODO`s, the reflowed lines of `apply` (part C), `transform`'s old guarantee (part E) and "the scalar alone" (`dual.hpp`). None of these is a catalogue line |
| Pointer headings exist | `git show feature/documentation-lane:docs/guide/limits.md` (`6c278df`) | All five headings exist, and each holds the figures its pointer says it holds, with the exceptions in m3 |
| T1, T2 against `records/numerical/0040-contraction-check.md` | Compared word for word. Reran: a transform of an rga3 line by a motor on g++ and clang++ at `-O2 -march=x86-64-v3` under `-ffp-contract=off/on/fast`, checking 0 and 1, comparing constant evaluation with run time | T1 and T2 are the record's sections 7.1, verbatim, and claim no more than its sections 3 and 5 show. Bits were the same under `off` and `on` (both compilers, both checking modes). Under `fast`, 2 of 6 coefficients differed on GCC and 3 of 6 on Clang. This agrees with T2 |
| S11 over the diff | grep of added lines under `include`, `test`, `tools` for decisions, plans, records, provisions, E1/E2, WU, spikes, agents | none found |
| Usability checklist (the deleted `==`) | compiled both compile-fail sources on both compilers | see the section below |

## Five claims re-derived

1. **The object-size table** (`records/performance/pre-release-core-pass/object-sizes/`). I recompiled E1's probe at `8058589` and `721afe7`. GCC `-O0`: 110,511 before and 121,910 after. GCC `-O2`: 4,513 and 4,497. Clang `-O0`: 74,739 and 73,755. Clang `-O2`: 4,766 and 4,347. These match the table, provided the after-side is compiled with `-Iinclude`. With a longer include path GCC `-O0` gives 122,022, because the after object embeds the path of `transform.hpp` and the before object embeds none. See m6.
2. **One W10 row: GCC, debug, `rga3/chains`.** I took the timing lock with `lock.sh take`, at once, with no queue. The pool showed 23 of 23 free before and after. I ran `tools/compiles cost` with that one unit, `--cpu 4 --repeats 3`. Before: 4,389.5 × 10⁶ instructions (range 5.8). After: 5,154.3 (range 2.4), +17.42%. The record gives 4,389.7, 5,153.6 and +17.40%. Peak memory was 173 and 190 MiB, as the record says. Confirmed.
3. **`weight_dual_layout` of a rigid(3) point is e123 alone** (`dual.hpp:313`). A probe prints a layout of one blade, mask 7, sign +1. Confirmed. The old text ("the scalar alone") was wrong.
4. **The gap mutant and `transform_not_forced`.** With `GALOOT_ALWAYS_INLINE` defined empty in a copy of the headers, the check passes on GCC and reports 5 calls on Clang. This holds also under `-fPIC`, `-fPIC -fno-plt` and `-fno-semantic-interposition`, so the PLT, GOT and local-alias forms are caught. With only `transform`'s attribute removed, GCC calls `transform` from all three transform probes. This matches `records/mutations/0040-pre-release-core-pass/README.md`.
5. **Contraction, record section 5.2.** `on` gives the same bits as `off`, and `fast` changes them, on both compilers. I checked this for `transform` (above) and for `geometric_product` of two motors. For the product, GCC's default (`fast`) gave 3 of 8 coefficients that differ between constant evaluation and run time, and Clang's default (`on`) gave none. Consistent with the record. This experiment also found M2.

## Findings, most severe first

### M1 (major, blocks): the public operations are not forced inline, and GCC now calls some of them out of line where it did not before

- Where: `include/galoot/core/product.hpp:230` (`geometric_product`), `:266` (`wedge`), `:346` (`geometric_antiproduct`), `:385` (`antiwedge`), and the operators at `:284`, `:312`, `:405` and `:434`. The same holds, untested, for the one-line public wrappers in `unary.hpp`, `linear.hpp` (`+`, `-`), `dual.hpp` and `basis_change.hpp`. Also `test/bench/kernels_inline.cpp`, whose probe does not reach these cases.
- Scenario: compile each function alone, with the structure test's own flags (`-O2 -DGALOOT_CHECKING=0 -ffp-contract=off`), through `test/tools/kernels_inline.py`, against the headers of `8058589` and of `721afe7`:

  | Operation | GCC before | GCC after | Clang before | Clang after |
  | --- | --- | --- | --- | --- |
  | `geometric_product(Motor, Motor)` | inline | `call geometric_product` | `jmp run_product` | `jmp geometric_product` |
  | `geometric_antiproduct(Motor, Motor)` | inline | `call geometric_antiproduct` | `jmp run_product` | `jmp geometric_antiproduct` |
  | `geometric_antiproduct(Motor, Flector)` | inline | `call geometric_antiproduct` | `jmp run_product` | `jmp geometric_antiproduct` |
  | `Motor % Flector` | inline | `call geometric_antiproduct` | `jmp run_product` | `jmp geometric_antiproduct` |
  | `geometric_product` of two `full<4>` of rga3 | inline | `call geometric_product` | `jmp run_product` | `jmp geometric_product` |
  | `compose(Motor, Flector)`, `Line ^ Point` | inline | inline | (compose) `jmp run_product`, (wedge) inline | inline |

  These operations use the rga3 preset's own `double` types. On GCC each was straight-line code before the change and is a call after it. On Clang the call only moves from `run_product` to the public function. GCC also calls `wedge` out of line for two `full<4>` multivectors. I tested that case in one translation unit with seven other operations, not on its own.

  The cause is the one the mutation record found for `transform` and `compose`. Forcing `apply` and the runners into a wrapper makes the wrapper's body large, so the compiler's own heuristic no longer inlines the wrapper into the user's function. The attributes stop one level short of the user's call. The probe misses this because its operations are small, and for those the wrappers still inline.
- Consequences: plan goal 1 ("the code of an operation is the straight-line arithmetic of its terms at its call site on both compilers") is not met for these operations, and on GCC this is a regression. Decision 0244's first consequence (`0244...md:44`, "An operation of the rga3 preset compiles to the arithmetic of its terms at its call site on both compilers at `-O2`") is not true at the tip. Acceptance criterion 1 passes only because of what the probe chooses to reach.
- Suggested fix: put the attribute on the public operations that call an attributed function, and on their operator aliases. Add to the probe at least `Motor * Motor`, `geometric_antiproduct(Motor, Motor)`, `Motor % Flector` and one product of a larger layout. Then rerun the mutants of section 8 and the W10 campaign, since the chain changes. This is the plan's own section 1 item 3 rule ("any the test shows out of line gets the attribute"), applied to a probe that reaches these cases. It is a non-comment fix under `include/`, so it needs a delta review.

### M2 (major, does not block): `apply`'s "same result" guarantee is false under contraction, and the unit now qualifies the identical guarantee of `transform` only

- Where: `include/galoot/core/mv.hpp:891-892`: "A constant evaluation and a run-time call give the same result." This is in the `///` comment that part C and part D2 edited.
- Scenario: g++ 16.2.1 at `-O2 -march=x86-64-v3`, with no `-ffp-contract` flag (GCC's default for C++, `fast`). `geometric_product` of two rga3 motors gives 3 of 8 coefficients whose bits differ between a `constexpr` result and the run-time call. Clang with `-ffp-contract=fast` gives 1 of 8. Part E now tells the reader of `transform` that this guarantee holds only without contraction across statements, but the same reader finds it unqualified in `apply`, which every product goes through. The defect was there before this unit (W9: reported here). The fix is comment-only: T2's qualification, worded for `apply`.

### m1 (minor): the forced attribute can turn a build that compiled into a hard error

- Where: `include/galoot/core/assert.hpp:35-36`, and every attributed function.
- Scenario: a translation unit built with `-march=x86-64-v3` contains a baseline fallback function, `[[gnu::target("arch=x86-64")]] Point f(const Point& x, const Motor& q) { return core::transform(x, q); }`. This is a manual-dispatch pattern. At `8058589` it compiles on both compilers. At `721afe7`, GCC stops with "inlining failed in call to 'always_inline' ... target specific option mismatch", and Clang stops with "always_inline function 'transform' requires target feature 'movbe', but would be inlined into ...". Caller attributes `optimize("O0")`, `optimize("Os")`, `cold` and `target("avx2")` (a wider target) still compile.
- No other change of meaning was found. Values were unchanged across the suite, and no function of the chain recurses. The empty fallback is safe on other compilers. Under ODR, a program whose translation units come from one compiler sees one token sequence.
- Suggested fix: the macro's `///` comment, or the limits page, should state the case: a caller that targets fewer instruction-set features than its translation unit cannot use these operations.

### m2 (minor): the structure test can pass vacuously off x86-64, and its rule does not cover every way a call can be written

- Where: `test/tools/kernels_inline.py:36` (`BRANCH`), and the registration at `test/bench/CMakeLists.txt:117`, which has no architecture guard.
- Scenarios:
  1. On AArch64 a call is `bl` and a tail call is `b`. `BRANCH` matches neither, so the missing-function check is the only part that runs, and `rga3.kernels_inline` passes whatever the code calls. The docstring says the input is x86-64 assembly, but nothing refuses other assembly.
  2. A conditional tail call (`jne _ZN6galoot... # TAILCALL`, which LLVM emits on x86-64) and a call through a register are not matched. The probe has no branches with checking off, so neither arises today. The docstring's "in any of its forms" claims more than the rule checks.
  3. An explicit-object member function mangles as `_ZNH...` (C++23 "deducing this"). `H` is not among the qualifiers `[KVrRO]`. The library has no such function today (`git grep 'this auto'` is empty).
- Suggested fix: register the test only when `CMAKE_SYSTEM_PROCESSOR` is x86-64, or have the script fail on assembly that has no x86 mnemonic. Add the `j<cc>` forms and `H` to the patterns, and narrow the docstring's claim to what is checked.
- What the test does check, as I found it: mangled names with clones (`.constprop`, `.isra`, `.part`), local entities, `@PLT`, `*@GOTPCREL`, Clang's `$local` aliases (the `-fno-semantic-interposition` run above), and missing functions. The fixtures are each killed by the mutant written for them. At `-Os` the check correctly fails: GCC reports 2 calls to `wedge`, and Clang reports 5 calls to `wedge`, `antiwedge` and `operator+`. That failure is the same mechanism as M1, which the fixed `-O2` keeps out of view.

### m3 (minor): some pointers leave out what DoD-6 asks of the sentence, or point at the wrong heading for the flags

- `mv.hpp:886-887` (binary `apply`) says "the flags that raise the limits are in docs/guide/limits.md, under 'Compile-time limits: products and plans'". The flags are in the page's table under "Raising the limits". "Products and plans" names only two flag values, as cases.
- `mv.hpp:936` (unary `apply`), `linear.hpp:61-63` (the header comment), `blade_set.hpp:588-591` (`of_grade`) and `blade_set.hpp:603-606` (`merge`) name no flag. The other eight pointers do ("Clang's -fconstexpr-steps and GCC's -fconstexpr-ops-limit raise those limits"). DoD-6 asks each sentence to say what bounds the size, which flag raises the limit, and where the figures are.
- `mv.hpp:881-883`: "a compiler's limit on the length of a fold bounds it as well as the limits that bound its plan". The text this replaces said that in practice the fold limit never binds ("no plan that Clang 23 builds at its default limits has more than 1,024 terms ... apply builds wherever its plan builds"). The new sentence reads as if the fold limit can bind first. It is true as a statement of a bound but misleads.
- `blade_set.hpp:589` (`of_grade`): "the sizes measured on each compiler". The page has Clang's sizes only, as the old comment did.
- Scenario: a user follows the binary `apply` pointer to find the flag and does not find the flag table under the heading named.

### m4 (minor): the pointers name a page that is not on `develop`

- The thirteen pointers name `docs/guide/limits.md`, which exists only on `feature/documentation-lane`. If this branch merges into `develop` first, every pointer leads nowhere until the documentation lane merges, and `limits-removed.md` is then the only place that holds the figures. Decision 0244 records this as K10. Recommendation: merge the documentation lane's page first, or make it a release blocker that both are on `develop`.

### m5 (minor): the hint of the deleted `==` is only in the echoed source line

- The checklist results are in the next section. The fix (`same_rows`) appears only in the caret excerpt of a note, as an end-of-line comment. The first error line on GCC is "use of deleted function 'bool galoot::core::basis_change<N>::operator==(...) const [with long unsigned int N = 2]'"; on Clang it is "overload resolution selected deleted operator '=='". The first does not state the mistake in the algebra's terms, and neither says what to do.
- Where the excerpt is not shown, the hint is lost: in an IDE problem list that shows only messages, or under GCC's `-fdiagnostics-plain-output` or `-fno-diagnostics-show-caret`, or in SARIF or JSON output.
- For `a != b`, Clang names `'=='`, not the operation as written.
- Comparing values of two different sizes, `basis_change<2>` with `basis_change<3>`, gets the generic "no match" error with no hint.
- This is the design decision 0236 item 3 chose. It keeps `!std::equality_comparable` true, which a catch-all with a `static_assert` would not. C++26's `= delete("reason")` (`__cpp_deleted_function`) would put the hint on the error line, but S2 allows a later-edition feature only in an optional header, so it cannot go here today. This is an owner note, not a defect.

### m6 (minor): small inaccuracies in the records

- `records/performance/pre-release-core-pass/object-sizes/README.md:23`: "`data` is 8 bytes on GCC ... on every row". `out/sizes.tsv` and my rerun give 8 at `-O0` and 0 at `-O2` on GCC.
- The same record does not say that the GCC `-O0` after-size depends on the include path (claim 1 above): the forced `transform` body embeds a header path string. The comparison is like with like only when the path's length is held fixed.
- Decision 0244 consequence 1: see M1.

### m7 (minor): wording in comments

- `test/core/basis_change_equality_fail.cpp:1-2` and `basis_change_inequality_fail.cpp:2`: "==, which basis_change does not define". After B3, `==` is declared, as deleted. "Whose == is deleted" would be accurate.
- `mv.hpp:397`: "which it now reports as false". "Now" ages. Also, the example ("an int for a rational whose constructor from an integer is explicit") describes a hypothetical type. That is fine, but the old text's pinned trade (three `std::is_constructible` answers that the tests fix) is now unstated in the header. That is acceptable because the owner ruled O1 (a).
- `GALOOT_ALWAYS_INLINE` lives in `assert.hpp`, a header named for assertions. A maintainer looking for it by name will not find it there. Nit.

### Not findings (checked, none found)

- `inverse_change`: `git grep inverse_change -- include test` is empty, and `check_fits`, `eliminate` and `inverse_rows` keep callers. The identities test reads `check_change(change, source, target).inverse` after `REQUIRE(fault == none)`. Every call is an isometry: `check` requires `keeps_inner_products` first, which is the test's own reference. The mutant `inverse_rows_sign` is killed by `core.basis_change_identities`, as recorded. The test's references (round trips, P G′ Pᵀ, the determinant) are independent of the library's inverse, so the test is not circular.
- The deleted `==`: the class is still an aggregate, structural and trivially copyable, and `basis_change_test.cpp:187` (`!std::equality_comparable`) and the template-argument uses still compile. One error on each compiler. The literal `// use same_rows(a, b)` is in the output of both. The tests require each compiler's own text, chosen by `CMAKE_CXX_COMPILER_ID`.
- S11: no citation was added under `include/`, `test/` or `tools/`. `cost.py`'s new comment ("until plan 0040 they were in those functions' comments") cites a plan. It lives under `docs/agent/snippets/`, which already names records in its `NON_COST` list, so I read it as agent documentation, not a finding.
- `cost.py comments` in header mode still passes when the header holds no count (`plan.hpp` now holds none). The plan's guard (section 1 item 9) is the Markdown mode, which refuses an empty section. Header mode on `plan.hpp` is now a check of nothing, so a later gate should not count it as evidence.
- Comment trims: no sentence that holds on any compiler was removed. Each removed sentence is in `limits-removed.md` and on the page under the heading cited. The exception is m3's flag table, which is under its own heading.

## Usability checklist (the deleted `==` on `basis_change`)

Tasks, from the examples: (1) decide whether two change tags hold the same matrix; (2) use a change as a template argument; (3) find out why `a == b` does not compile and what to write instead.

| Item | Result |
| --- | --- |
| Names | `same_rows` is a plain-English name and says what it compares (rows as written), not equality of matrices. Pass |
| Construction and conversion | unchanged by this unit |
| Compile-fail test on both compilers requiring the literal | `core.basis_change_equality` and `_inequality`. Pass |
| One error, message on the first error line | One error on each compiler. The fix is not on the first error line, only in the note's source excerpt (m5) |
| Starts with the operation as written, mistake in the algebra's terms, fix after a semicolon | Not met: compiler wording, no algebra terms; for `!=` Clang names `==` (m5) |
| Context lines in the user's terms | Show `basis_change<N>` with `N = 2`. Acceptable |
| Distinct causes, distinct messages | `==` and `!=` have one cause and one message. Comparing different sizes gets the generic error (m5) |
| Concepts that generic code may probe | `std::equality_comparable` is still false, as pinned. Pass |
| Exact comparisons are `==` | `==` is deleted on purpose because two values for one matrix differ. The `///` says so. Pass |
| `///` states what to call | Pass (`basis_change.hpp:146-148`, `:171-177`) |

**Before release (Hyrum's law).**

- Turning the deleted `==` later into a defined one would change `std::equality_comparable` for `basis_change`. Generic code may come to depend on the current `false`.
- `GALOOT_ALWAYS_INLINE` is a macro visible to every includer. Its comment says it is not part of the interface, but a user can still use it, and renaming or removing it would then break that user. Consider whether it should be `#undef`'d where possible, or named as internal (`GALOOT_DETAIL_...`).
- M1's fix adds the attribute to public functions. That is not an interface change, but it makes m1's hard error reachable from more call sites.

## Resolution

Filled in on 2026-11-11 by Claude (agent, Opus 5.5), implementer of plan 0040 section 18, at the coordinator's request, from the commits of `feature/pre-release-core-pass` and its subfeatures and of `merge/pre-release-core-pass-gate2`. Each commit named below is on `merge/pre-release-core-pass-gate2`.

| Finding | Resolution |
| --- | --- |
| M1 | Fixed. The probe reaches the public operations, alone too (`ce70998`, `edf0994`, `fdba160`, labelled failing); the attribute goes on the four products, their operators, `weight`, `left_weight_dual` and `antidot` (`d47e8f9`). Its delta review, `2026-11-11-pre-release-core-pass-delta-review.md`, blocked on D1 (`project` and `antiproject`), fixed by `390b55e`; the second delta review approved. |
| M2 | Fixed in a comment: `apply`'s guarantee is qualified as `transform`'s is (`ad56c9b`). |
| m1 | Fixed in a comment: the macro's comment states the target-mismatch error and how to avoid it (`fa482be`). |
| m2 | Fixed. The check reads conditional jumps, calls through a register and `H` members (`c0e33e4`); the structure tests are registered only where `CMAKE_SYSTEM_PROCESSOR` is x86-64 (`617aab0`), and the per-output guess of the target is dropped (`36575d5`). |
| m3 | Fixed in comments: each pointer names the flags and the right heading, and the fold sentence again says the fold's limit does not bind first (`ad56c9b`). |
| m4 | Closed: the documentation lane's `docs/guide/limits.md` is on `develop`, and came into this branch with `ebe7336`. |
| m5 | Deferred, to the owner (plan 0040 section 17, "Deferred from the review records"). It is the form decision 0236 item 3 chose; a hint on the error line needs C++26's `= delete("reason")`. |
| m6 | Fixed: the object-size record states that `data` is 8 bytes on GCC at `-O0` only and that the GCC `-O0` size depends on the include path, and decision 0244's first consequence is reworded to what the probe reaches (`414879e`, then `c89ada0`). The first delta review confirmed both. |
| m7 | Fixed: the wording of `mv.hpp` (`ad56c9b`) and of the two compile-fail sources (`a6f3196`). The nit on where `GALOOT_ALWAYS_INLINE` lives: no change, since `assert.hpp` is the header every core header includes; the configuration macro `GALOOT_FORCE_INLINE` added after gate 2 sits beside `GALOOT_CHECKING` there. |
| Before release: `GALOOT_ALWAYS_INLINE` visible to includers | No change. Its comment says it is not part of the interface; the public control is `GALOOT_FORCE_INLINE` (decision 0244 item 16), documented in the README and the core guide. |
| Before release: m1 reachable from more call sites | Recorded in decision 0244's consequences. |
