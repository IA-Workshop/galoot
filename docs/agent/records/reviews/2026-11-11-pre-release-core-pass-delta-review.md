# Delta review: plan 0040, the fix of the blind review's M1 (public operations forced)

- Reviewer: blind-reviewer (Claude, agent, Opus 5.5), 2026-11-11 UTC.
- Range: `feature/pre-release-core-pass--public-ops`, `721afe7..414879e` (nine commits). Review worktree `.claude/worktrees/review-pre-release-core-pass-delta`, branch `review/pre-release-core-pass-delta` from `414879e`.
- Given: the range, plan 0040 sections 1 to 5, the earlier review's record (`review/pre-release-core-pass`, findings M1, m1, m2, m6, m7), and the governance. The brief carried no verdict. The commit messages and the amended decision 0244 inside the range state the author's own conclusions ("No other public function showed a call"); I checked those claims rather than relying on them. I did not read the author's report.
- Run rows: [`2026-11-11-pre-release-core-pass-delta-review/runs.tsv`](2026-11-11-pre-release-core-pass-delta-review/runs.tsv). The survey script and its results are in `survey/`, the W10 rerun in `w10/`, the mutant reruns in `mutants-rerun.tsv`.

## Outcome

**Block**, on finding D1. The change is valid C++ and changes nothing but placement. The structure tests work as claimed, and the mutant and W10 numbers reproduce. However, `project` and `antiproject` were left out. On rga3 full multivectors and motors, GCC at `-O2` now calls them out of line where the branch point inlined them, and two of those three regressions, like six of the nine GCC regressions overall, come from this fix itself. This is M1's mechanism moved up one more level. The fix needs a code change under `include/`, so it needs another delta review. The coordinator and owner may instead accept D1 as a recorded limitation; that choice is the first open item below. The other findings are comments, records, or test-script rules.

## Departures and checks not made (C20)

- The P8 selection was the full suite on gcc-debug and clang-debug (583 of 583 on each, at `414879e`, new trees in my worktree). I did not run the release, asan, lint or measure presets.
- W10: I reran one unit (GCC, debug, `core/product`), not the campaign. I did not rerun the object sizes.
- Mutants: six runs (three mutants on each compiler), not the 24 × 2 of the record, and none of `check-review.toml` (I read those, and checked the script's rules by hand, below).
- `tools/mutate` writes its results beside the spec and its rows to the shared run log. I moved my rows into this record's files and restored both to `HEAD`, so the branch's mutation records are unchanged.
- One of my survey runs overlapped a `tools/mutate` run that was editing the worktree's headers. I discarded it and reran every tip survey against a clean `git archive 414879e` copy (`survey/tip-414879e.tsv`). The 8058589 and 721afe7 surveys always used archived trees.
- The survey's compiles ran outside the job pool, six at a time (one `tools/timed` row each). The load average was 5 to 7 from other work on the machine during the W10 rerun, which was pinned to CPU 4 under the timing lock.
- No web request, no third-party contact, no processor configuration touched. I did not see the tokens this review used.

## Checks made

| Check | How | Result |
| --- | --- | --- |
| Full suite | `tools/timed` build and `ctest --preset` on gcc-debug and clang-debug | 583/583 on each |
| Convention trace gate | `python3 -I tools/test_references.py trace ...` | exit 0 |
| Only placement changed | comments stripped and the three headers tokenized (`g++ -fpreprocessed -dD -E -P`), `721afe7` against `414879e` | The only token changes are 11 `GALOOT_ALWAYS_INLINE` and the two `#include <galoot/core/assert.hpp>` |
| `-pedantic-errors` | the probe, `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -pedantic-errors`, at `-O0` and `-O2`, `GALOOT_CHECKING` 0 and 1, g++ 16.2.1 and clang++ 23.1.4 | all 8 compile |
| Placement | read every hunk | Each attribute comes after the template head and before `[[nodiscard]] constexpr`, the same order as the thirteen of part A; the clang-format `AttributeMacros` line keeps the layout. No requires-clause sits on an attributed declaration. The attributes are on the constrained templates only, never on the `mismatched_operands` catch-alls, so the one-error rejections are unchanged. The operator aliases carry the attribute as their named functions do. An attribute takes no part in overload resolution, and the macro expands the same way in every unit of one compiler, so there is no ODR change |
| m1 wording | the comment's example on both compilers, plus three variants (`-mmovbe` unit with a `target("avx2")` caller; `-march=x86-64` with `target("avx2")`; `-mmovbe -mavx2` with `target("avx2")`) | The example fails on both, as stated. The three variants compile on both: a `target` attribute without `arch=` adds to the unit's features, so only a caller with fewer features fails. "Fewer" is accurate |
| `--alone` is alone | ran the script with a wrapper compiler that copies each unit it is given, on the probe's 48 names | 48 units, each defining exactly one probe function, plus the probe's common text (includes, aliases, the `doubled_e4` tag) |
| S11 over the diff | grep of added lines under `include/`, `test/`, `tools/` for decisions, plans, records, reviews, provisions, agents | none |
| Public operations alone (point 1 of the brief) | `survey/survey.py`: 320 one-function units (17 unary operations on 8 types, 15 binary ones on 12 pairs, transform on 8 pairs, changes of basis, conversion, truncate, part). The types are rga3 `Full`, motor, line, point, plane and flector; conformal(3) (`full<5>`, even, vectors, bivectors); and R(3,0,0) (`full<3>`, a rotor, vectors). Each compiled alone with the structure test's flags through the tip's `kernels_inline.py`, at `-O2`, `-Os` and `-O3` on both compilers at `414879e`, and at `-O2` at `8058589` and `721afe7` | see D1 and the next section |

## Inlining across levels (the brief's point 1)

Counts of the 297 units that compile and are read (three `compose` of full multivectors are rejected by design, and the 20 per compiler in D3 are refused):

| Compiler | `-O2` at `8058589` | `-O2` at `721afe7` | `-O2` at `414879e` | `-Os` at tip | `-O3` at tip |
| --- | ---: | ---: | ---: | ---: | ---: |
| GCC, units with a call | 15 | 47 | 20 | 26 | 7 |
| Clang, units with a call | 78 | 67 | 25 | 77 | 20 |

- Clang at `-O2`: every unit that calls at the tip also called at the branch point, so Clang has no regression. Its remaining calls are tail jumps to the public function itself, mostly operations on conformal(3) `full<5>` (the unary operations, `+`, `-`, the duals, `dot`), and `project`/`antiproject` on larger layouts in all three algebras.
- GCC at `-O2`: 9 units that the branch point inlined now call (D1). The other 11 called at the branch point too (`bulk` of conformal(3) even and full multivectors, among others).
- At `-O3` the tip's rga3 calls are `project(Full, Full)` and `antiproject(Full, Full)` on both compilers.
- At `-Os` GCC calls 8 rga3 operations (`bulk`, `left_bulk_dual`, `dot`, `project`, `antiproject` of full multivectors or motors). Clang calls 30, among them `Line + Line`, `project(Point, Plane)` and the conversion `Full{motor}`, which are probe functions. So the structure test fails at `-Os` on Clang, as the first review found. Decision 0244 states it does not claim other levels.

## Findings, most severe first

### D1 (major, blocks): `project` and `antiproject` are not forced, and GCC now calls them out of line where it did not before, three times in rga3; six of the nine come from the fix itself

- Where: `include/galoot/core/dual.hpp:614-615` (`project`) and `:651-652` (`antiproject`), which call the now-forced `wedge` and `antiwedge`. Outside rga3 also `dot` (`dual.hpp:527-528`) and `bulk` (`:378-379`, reached from `left_bulk_dual`). The probe reaches `project` and `antiproject` only on a point and a plane (`test/bench/kernels_inline.cpp:160-166`), while it reaches every other binary operation on `Full` too.
- Scenario: each function alone, `-O2 -DGALOOT_CHECKING=0 -ffp-contract=off`, GCC 16.2.1, checked by the tip's script:

  | Unit | `8058589` | `721afe7` | `414879e` |
  | --- | --- | --- | --- |
  | rga3 `project(Full, Full)` | inline | `call project` | `call project` |
  | rga3 `antiproject(Full, Full)` | inline | inline | `call antiproject` |
  | rga3 `project(Motor, Motor)` | inline | inline | `call project` |
  | R(3,0,0) `project(full<3>, full<3>)`, `antiproject(...)` | inline | inline | `call` |
  | conformal(3) `project(vector, bivector)` | inline | inline | `call project` |
  | conformal(3) `left_bulk_dual(even)` | inline | inline | `call bulk` |
  | conformal(3) `dot(even, even)`, `dot(full, full)` | inline | `jmp dot` | `jmp dot` |

  Clang calls `project` and `antiproject` in all of these cases, at the branch point and at the tip. The rga3 projections of a point, a line and a plane onto one another inline on both compilers at all three revisions. So the gap is in large layouts, which is the class the fix added to the probe for the products.
- Cause: the same as M1's. Forcing `wedge` and `antiwedge` makes `project`'s body large, so GCC's heuristic stops inlining `project` into the user's function. Each forced level moves the budget one level up. The author's rule "force only what a structure test showed called" therefore has to be applied again after every round, and the 61-case survey did not include these layouts. Decision 0244 item 4 records this effect itself for `run_product`, `weight` and `left_weight_dual`.
- Consequences: plan goal 1 is not met on GCC for these operations, and six of the nine GCC regressions are new in this range. The `///` comment of the macro and decision 0244 item 1 say more than holds (D2).
- Suggested fix, and its trade: either (a) attribute `project` and `antiproject`, add `project_full`, `antiproject_full` and `project_motors` to the probe, and survey again; or (b) attribute every public operation whose body calls an attributed function (`project`, `antiproject`, `bulk`, `bulk_dual`, `weight_dual`, `left_bulk_dual`, `dot`, the unary operations, `+`, `-`, `change_basis`, `change_basis_back`), which ends the level-by-level chase. Option (b) also covers the Clang `full<5>` calls. The cost of (b) is a larger `-O0` cost (the W10 numbers grow again) and m1's target error reachable from more operations. Either option needs the mutants, W10 and a delta review again.

### D2 (minor): the macro's comment and decision 0244 item 1 claim more than the evidence

- `include/galoot/core/assert.hpp:26-28`: "and on the public operations that a compiler would otherwise keep out of line". D1 shows public operations that GCC keeps out of line at `-O2` and that carry no attribute. Wording that holds: "and on the public operations that the structure test found out of line".
- Decision 0244 item 1: "No other public function showed a call, in the probe, alone, or in a survey of 61 operations each compiled alone, so ... `dot`, `project`, `antiproject` ... have no attribute". The survey is not recorded. Only its run-log row exists (`runs.tsv`, 2026-11-11T05:10:33Z), with no list of cases, so a reader cannot tell what it reached. The sentence is true of that survey but not of the operations it names. The first consequence is true as worded, because it says "the probe reaches" and excludes "other layouts, other algebras".
- The rest of the m1 paragraph (`assert.hpp:35-42`) is accurate (checked above).

### D3 (minor): the x86-64 refusal fails a valid x86-64 compile whose functions are a bare `ret`

- Where: `test/tools/kernels_inline.py:82-83` and `:63`. An operation whose result layout is empty compiles to `ret` alone, and no instruction then names a 64-bit register. Examples are `dot(Point, Plane)`, `Line & Point`, `project(Line, Point)` and `antidot(Motor, Flector)`: 20 of my survey's units per compiler.
- Scenario: a maintainer adds `auto dot_point_plane(const Point& p, const Plane& g) { return core::dot(p, g); }` to the probe. `rga3.kernels_inline_alone` then fails with "the output is not x86-64 assembly", which misleads. The combined unit still passes, because other functions name registers.
- Suggested fix: decide the target from the compiler (`<compiler> -dumpmachine`), or from the build (D5), rather than from the instructions.

### D4 (minor): the script's comment claims the immediate form, which its pattern excludes, and a register is tracked only by its exact name

- Where: `test/tools/kernels_inline.py:58-61`. The comment says "after a $ for an immediate", but the lookbehind `(?<![\w.$])` refuses a `$` before the name. Verified: `movl $_ZN6galoot4core3fooEv, %eax` followed by `call *%rax` passes, and so does `movabsq $_ZN..., %rdx` followed by `call *%rdx`. Both are what a non-PIC build writes for a function's address. Neither the width alias (`%eax` loaded, `%rax` called) nor a copy between registers is read. The docstring discloses the copy ("a register loaded in any other way") but not the alias. Prefixed branches (`notrack jmp`) are not matched either.
- Scenario: none at `-O2` today, since the library takes no function addresses. This is a gap in a check whose purpose is to catch what comes back.
- Suggested fix: allow `$` before the name, compare registers by their 64-bit name, and say what is not read. Or narrow the comment to the forms that are read.

### D5 (minor): on a non-x86-64 host the two structure tests now fail instead of skipping

- Where: `test/bench/CMakeLists.txt:126-152`, which has no architecture guard. The refusal makes `rga3.kernels_inline` and `rga3.kernels_inline_alone` exit 1 on AArch64.
- Scenario: a contributor on ARM Linux or Apple silicon runs the full suite, and two tests fail with "not x86-64 assembly".
- Suggested fix: register them only when `CMAKE_SYSTEM_PROCESSOR` is x86-64, or exit with a skip code under `SKIP_RETURN_CODE`, as `test/CMakeLists.txt:922` does. This is the first review's m2 option that was not taken.

### D6 (nit): a test gap recorded as an equivalent mutant

- `records/mutations/0040-pre-release-core-pass/check-review.toml:29-35` and README line 104: `check_keeps_overwritten_register` is declared "equivalent" and "found so". The mutant changes behavior: after `leaq <library>(%rip), %rax; movq %rdi, %rax; call *%rax` it reports a call the original does not. So it is a mutant that no fixture kills, not an equivalent one. A two-line fixture would kill it. The "defines `<f>` in N paragraphs" branch of `--alone` (`kernels_inline.py:150-153`) has no test either.

## What I confirmed (no finding)

- **Structure tests (point 2).** The 14 fixture tests each require the specific line or name, so a broken refusal or a broken rule makes them fail rather than pass vacuously. The `movq %rdi, %rax` lines added to the old fixtures exist only to pass the refusal. `alone_accepts` would fail if units held more than one function, `alone_rejects_call` forbids the other function's line, and the stand-in refuses a unit without the common text. The script also scans the out-of-line library functions in a unit, so a call nested inside an emitted `galoot::` function fails too. Wrong failures: D3. Gaps: D4 and D6.
- **Mutants (point 3).** Reran with `tools/mutate` at `414879e` (`mutants-rerun.tsv`). Clang: `weight_not_forced` and `antidot_not_forced` survive and `geometric_product_not_forced` is killed by both structure tests, as recorded. GCC: `weight_not_forced` and `antidot_not_forced` are killed by `rga3.kernels_inline_alone` only, and `run_product_not_forced` is killed by `rga3.kernels_inline` against its expectation "survived", as recorded (`tools/mutate` exits 1 on the GCC spec for that reason).
- **W10 (point 4).** GCC, debug, `core/product`, `tools/compiles cost` with `--before 8058589 --cpu 4 --repeats 3`, under the timing lock (taken at once, released after), with the pool 23 of 23 free before and after. Before: 32,446.7 × 10⁶ instructions (range 17.2), 2.17 s, 418 MiB. After: 58,594.2 (range 36.0), 4.34 s, 829 MiB. The change is +80.59%; the record gives 32,472.9, 58,621.7 and +80.53%. Confirmed within 0.1%.
- **Decision 0244 consequences (point 5).** The first is true as worded, because of its hedge (D2 is about item 1). The second, the target error, is checked above. The third, the object sizes, was not rerun, but `out/sizes-public-ops.tsv` supports it. The m6 wording of the object-size record (data 8 bytes on GCC at `-O0` only, and the dependence on the include path) is correct, and the m7 wording of the two compile-fail sources ("deletes") is correct.
- **Process.** The tests came before the code they judge, in labelled failing commits (`ce70998`, `edf0994`, `fdba160`), then the code (`d47e8f9`) (W2, W9). Each commit names its agent and model. Decision 0244 was amended in place rather than superseded. W6 says a recorded decision is not rewritten, but 0244's status line says it is open to the review, so I note this and do not count it as a finding.

## Usability (public headers)

The interface does not change: no name, signature, or result changes, and the attributes are invisible to overload resolution and to concepts. Hyrum's law: the set of forced operations is observable through m1's target error, which is a compile error. Every operation added to the set, under either option of D1, can break a caller that compiled before. A later removal cannot break anyone.

## Open items for the owner and coordinator (with trade-offs)

1. **D1: how far to force.** (a) Attribute `project` and `antiproject` and extend the probe: the least code and the least `-O0` cost, but the next round may find the next level, and conformal(3) `full<5>` calls on Clang stay. (b) Force every public operation that calls the chain: ends the chase and covers the Clang `full<5>` calls, at a further `-O0` build-cost increase and with m1's error reachable from every operation. (c) Accept the gap, and record in decision 0244 and the macro's comment that large layouts may compile to a call on GCC. Recommendation: (b). Each round so far has found the next level, and the evidence-only rule has become a moving target. W10 must rerun for any of (a) and (b).
2. **O2, the knob, again.** The debug cost of `product_test.cpp` on GCC is now +80.6% (2.2 s to 4.3 s, 418 to 829 MiB), confirmed here. Option (b) of item 1 would raise it further. Recommendation: rule on O2 after item 1, from the W10 numbers of the option chosen.
3. **D3 and D5: how the structure test knows its target.** Recommendation: decide it from the build (register the tests only on x86-64) and drop the per-instruction guess. This fixes both, and a non-x86 host then skips the tests rather than failing them.

## Resolution

To be filled in by the coordinator: each finding's resolution, and for D1 the delta review of its fix.
