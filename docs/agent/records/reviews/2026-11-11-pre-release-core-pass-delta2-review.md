# Second delta review: plan 0040, the fixes of the delta review's D1 to D6

- Reviewer: blind-reviewer (Claude, agent, Opus 5.5), 2026-11-11 UTC.
- Range: `feature/pre-release-core-pass--public-ops`, `414879e..26f1b62` (ten commits). Review worktree `.claude/worktrees/review-pre-release-core-pass-delta2`, branch `review/pre-release-core-pass-delta2` from `26f1b62`.
- Given: the range, plan 0040 sections 1 to 5, the earlier delta review's record (`review/pre-release-core-pass-delta`, findings D1 to D6) and its survey script, and the governance. The brief carried no verdict, but it did state some of the author's claims as things to check ("the author reports only l, I, rn fire"; "what the author says is new"). The amended decision 0244 and the records in the range also state the author's conclusions. I checked each one and did not rely on any. I did not read the author's report.
- Evidence is in [`2026-11-11-pre-release-core-pass-delta2-review/`](2026-11-11-pre-release-core-pass-delta2-review/): `runs.tsv` (every `tools/timed` row of this review), `survey/`, `tokens/`, `lint/`, `mutants/`, `w10/`, `probes/`.

## Outcome

**Approve with non-blocking findings.** D1 is fixed for the cases claimed. With `project` and `antiproject` forced, no rga3 or R(3,0,0) case in the 320-case survey calls at `-O2` on either compiler. No case calls at the tip that did not call at `414879e`, so this round did not move the problem up another level. Clang no longer calls `project` or `antiproject` anywhere in the survey. The code change is the two attributes and the pack rename, and nothing else. The mutants, the survey counts, the W10 figure and the structure-test guard all reproduce. The four findings below are a lint clash the rename introduces (E1), a remnant of D3 under control-flow protection (E2), a comment that still says slightly more than holds (E3), and a nit (E4). None changes generated code on the tested configurations.

## Departures and checks not made (C20)

- P9 selection: the full suite on gcc-debug and clang-debug only, in new trees in my worktree (588 of 588 on each). I did not run release, asan, lint, measure or `format-check`. In place of the lint preset I ran clang-tidy 23.1.4 by hand on `test/core/linear_user_names.cpp` with the repository's `.clang-tidy`, plus a narrower `misc-confusable-identifiers` scan (below). `clang-format --dry-run --Werror` is clean on the six changed C++ files.
- Survey: `-O2` only, at `26f1b62` and `8058589`, through the tip's `kernels_inline.py`. I did not rerun `-Os` or `-O3`, so the decision's `-O3` sentence ("no rga3 case calls") is the author's, unchecked.
- W10: one unit, GCC debug `core/product`, not the campaign.
- Mutants: 14 runs. That is four attribute mutants on each compiler (`project`, `antiproject`, `weight`, `transform_middle`, copied unchanged from `attributes-d1-*.toml`) and three script mutants on both presets (one from `check-d1.toml` and two of my own). It is not the 26 × 2 + 15 × 2 of the record.
- The survey and the clang-tidy scans ran outside the job pool, six processes at a time (one `tools/timed` row each), and never during the W10 run. The W10 run held the timing lock (taken at once, released after; pool 23 of 23 free before and after; load average 1.4 to 1.6).
- Two clang-tidy rows first went to the shared run log because `TIMED_LOG` was not set in that shell. I moved them into this record's `runs.tsv` and restored `records/performance/runs.tsv` to `HEAD`.
- No web request, no third-party contact, no processor configuration touched, master not touched. I could not see the tokens this review used.

## Checks made

| Check | How | Result |
| --- | --- | --- |
| Full suite | `tools/timed` configure, build and `ctest --preset` on gcc-debug and clang-debug, from new trees | 588/588 on each |
| Convention trace gate | `python3 -I tools/test_references.py trace ...` from the root | exit 0 (items 6.1 to 6.4 have no test, as before) |
| Only the stated changes (point 1) | `tokens/diff.txt`. The three changed headers, comments stripped and not expanded (`g++ -fpreprocessed -dD -E -P`), tokenized, `414879e` against `26f1b62`. Then a unit including all core headers and rga3 (`tokens/unit.cpp`), fully preprocessed by g++ and clang++ with `GALOOT_CHECKING` 0 and 1 | `assert.hpp`: no token change. `dual.hpp`: two `GALOOT_ALWAYS_INLINE`. `linear.hpp`: six `I` → `Is`. Fully preprocessed, on both compilers in both modes: two `[[gnu::always_inline]]` and six `Is`. The only other differences are the checkout paths and one `__LINE__` value (414 → 415) that `GALOOT_ASSERT` expands to in `linear.hpp` below the reflowed fold |
| Overload, ODR | reading | The attributes are on the two constrained templates only, not on the `mismatched_operands` catch-alls (`dual.hpp:633-634`, `:671-672`), so the one-error rejections are unchanged; the suite's compile-fail tests for `project` and `antiproject` pass. An attribute and a template parameter's name take no part in overload resolution or mangling; the macro expands the same way in every unit of one compiler, so there is no ODR change |
| Confusable names (point 1) | `lint/scan.py`: a user unit per name, each defining a namespace-scope `constexpr Line <name>` and using it in a sum and a negation, clang-tidy `misc-confusable-identifiers` with `--header-filter=.*`, 108 names (all 52 letters, `rn`, `ls`, `Is`, `Il`, `O0`, `KX`, …), at `414879e` and `26f1b62`; `lint/forms.py`: 15 names as a local, a parameter and a template parameter as well | At `414879e`: `l` (against `linear.hpp:154` `I`), `I` (against `basis_change.hpp:542` `l`), `rn` (against `bits.hpp:171` `m`), as the author reports; `O` and `m` do not fire. At `26f1b62`: `l` no longer fires, `I` and `rn` still do, and **`ls` now fires against the new `Is`** (E1). No name fires as a local, a parameter or a template parameter. The one other pack named `I` (`mv.hpp:419`, `storage_for`'s partial specialization) fires against no user name; clang-tidy reports clashes only with names visible at namespace scope |
| The lint test | clang-tidy with the repository's `.clang-tidy` on `test/core/linear_user_names.cpp` against each revision's headers | `414879e`: `'I' is confusable with 'l'`, exit 1. `26f1b62`: clean, exit 0. So the test fails without the fix and passes with it |
| Survey (point 2) | the earlier delta review's `survey.py`, unchanged (byte-identical to the branch's copy), on `git archive` copies of `26f1b62` and `8058589`, both through the tip's `kernels_inline.py`, `-O2`; `survey/O2-*.tsv` | see the next section |
| Guard (D5, point 3) | `cmake --preset gcc-debug -B <scratch> -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=<p>`, then `ctest -N` | `aarch64`, `arm64`: 609 tests, neither `rga3.kernels_inline` test registered, all 19 `tools.kernels_inline_*` still registered. `x86_64`, `AMD64`: 611 tests with both. The guard works |
| Fixture tests (D3, D4, D6, point 3) | read each of the 19 registrations and fixtures; mutants below | Each rejecting test requires its specific text: a line number with function and mnemonic (`line 6, in _Z5firstRKd: notrack jmp`), or the register and the symbol it holds (`through %rax, which holds _ZN6galoot4core3fooEv` and the `%rdx` one), or a name. The four accepting tests require exit 0 and the summary line, and each has a rejecting twin that a blanket "accept" mutant fails (`check_accepts_any_target` is killed by `_rejects_other_target`). `immediate.s` would pass a script that ignored `$` or register width, so `_rejects_immediate` cannot pass vacuously; `bare_return.s` contains only `ret`/`retq` and directives. The list of 52 probe functions equals the 52 definitions in `kernels_inline.cpp` |
| Bare returns on real output | the survey at `8058589` against the earlier review's run of the same 320 cases through `414879e`'s script | All 40 differences are `REFUSED` → `inline`, 20 per compiler: the bare-return units (`dot(Point, Plane)`, `Line & Point`, …) now pass. With `-fcf-protection` they do not (E2) |
| Probe under CET | both structure-test modes, both compilers, the tip's probe with `-fcf-protection=full` added | 52 of 52 pass in both modes on both compilers. The `notrack` rule raises no false report on real jump tables |
| Mutants (point 4) | `tools/mutate` at `26f1b62`, `mutants/*.toml`, results in `mutants/out/` | GCC: `project_not_forced` and `antiproject_not_forced` killed by both structure tests, `weight_not_forced` killed by `rga3.kernels_inline_alone` only, `transform_middle_not_forced` survives. Clang: `project` and `antiproject` killed by both, `weight` survives, `transform_middle` killed by both. Script, on both presets: `check_ignores_register_width` killed by `_rejects_immediate`; mine, `check_accepts_any_target` (the refusal dropped) and `check_any_instruction_returns` (every instruction counts as a return), both killed by `_rejects_other_target`. Every verdict and killer set equals the record's (`out/attributes-d1-*.tsv`, `out/check-d1-*.tsv`). `tools/mutate` exited 0 on all three specs, and the worktree was clean after |
| W10 (point 6) | `w10/`: `tools/compiles cost` on GCC debug `core/product`, `--before 8058589 --cpu 4 --repeats 3`, under the timing lock | Before 32,461.1 × 10⁶ instructions (range 15.1), 2.16 s, 418 MiB. After 58,597.0 (range 26.7), 4.32 s, 829 MiB. **+80.51%**, against the record's +80.41% at `d2568e3`: confirmed within 0.1 point. The wall times match the second campaign's quiet-machine figures (2.18 s and 4.32 s) |
| Target error now reachable from `project` | `probes/target_project.cpp`: `project(Point, Plane)` from a `[[gnu::target("arch=x86-64")]]` function in a `-march=x86-64-v3` unit | Compiles at `414879e`, and fails on both compilers at `26f1b62`, as the macro's comment says such a call does. Expected, and covered by decision 0244's consequence, but it is a new break for such callers (see Usability) |
| rga3 preset functions | `rga3::rotation(l, theta)` and `rga3::translation(x, y, z)` alone, `-O2`, the tip's script | inline on both compilers, so the macro comment's "the rga3 preset's operations" has no counterexample there |
| S11 over the diff | grep of added lines under `include/`, `test/`, `tools/` for decisions, plans, reviews, records, research, agents, provisions, finding labels | none |
| Process | `git log` | W9 holds: the probe and the fixtures (`617aab0`, labelled failing) come before the script (`c49f39a`) and the attributes (`390b55e`), and the lint unit (`5f31dd5`, labelled failing) before the rename (`d2568e3`). Each commit names its agent and model. The comment-only commits (`b5d7ad0`, `4a9c2bb`) are separate from the code commits |

## The survey against the branch point (point 2)

All 320 cases at each revision, on each compiler; the 3 `compose` of full multivectors per compiler are rejected by design (`ERROR`).

| Compiler | Revision | rga3 calls | conformal(3) calls | R(3,0,0) calls |
| --- | --- | ---: | ---: | ---: |
| GCC | `8058589` | 1 (`antidot(Full, Full)`) | 14 | 0 |
| GCC | `26f1b62` | **0** | 12 | **0** |
| Clang | `8058589` | 26 | 45 | 7 |
| Clang | `26f1b62` | **0** | 14 | **0** |

- **GCC, new against the branch point: exactly `dot(CFull, CFull)`, `dot(CEven, CEven)` (tail `jmp dot`) and `left_bulk_dual(CEven)` (`call bulk`), as the author says.** The other nine at the tip also called at the branch point: `bulk`, `weight`, `bulk_dual`, `weight_dual`, `left_bulk_dual` and `left_weight_dual` of `CFull`, `bulk` of `CEven`, and `project` and `antiproject` of `CFull`, all reaching `call bulk` except `bulk_dual`. Six that called at the branch point now inline. The earlier delta review's surveys place where the three new ones came from: the two `dot` at `721afe7` (the chain forced) and `left_bulk_dual(CEven)` at `414879e` (the products forced). None of them comes from this round.
- **Clang: no case calls at the tip that did not call at the branch point.** The 14 left are all on `CFull` and all tail jumps to the public function: the seven unary operations, `bulk`, three duals, `+`, `-` and `dot`. The figures match decision 0244 item 1 (GCC 12, Clang 14) and the survey README.
- **Against `414879e`: no case calls now that did not call then.** 19 now inline that called then (the projections of `Full`, `Motor`, `CEven`, `CVec`/`CBiv` and `VFull` on one or both compilers, and `CFull` on Clang). So forcing `project` and `antiproject` did not push a call into their callers at `-O2`, as forcing the products had done.

## Findings, most severe first

### E1 (minor, does not block): the new pack name `Is` is confusable with a user's `ls`, and the lint unit guards only `l`

- Where: `include/galoot/core/linear.hpp:154` (`std::size_t... Is`); `test/core/linear_user_names.cpp:13` names only `l`.
- Scenario: a documentation example or a user's unit under the lint preset declares `constexpr auto ls = ...` (a list of lines, say) at namespace scope. clang-tidy stops with `'Is' is confusable with 'ls' [misc-confusable-identifiers,-warnings-as-errors]` at `linear.hpp:154`. The same unit passed at `414879e`. `lint/scan-26f1b62.tsv` shows it. `core_linear_user_names` does not catch it, because it names `l` only. Decision 0244 item 14's scan of 62 names reports the clashes at `414879e` correctly but does not report this one, which only the renamed header has.
- Cause: clang-tidy reduces both names to a skeleton in which `I` and `l` are the same, so any name containing a capital `I` clashes with the same name spelled with `l`. A pack named `Ix` would clash with `lx`, and so on.
- Suggested fix: a pack name with none of `I`, `l`, `1`, `O`, `0` (or `m`, which `rn` reaches). For example `Xs` beside `J`, or `XTerms` and `YTerms`. Add `ls` (or the scan's set) to the user unit. A comment-free, two-line change, but it is under `include/`, so W5 asks for a delta review of it.

### E2 (minor, does not block): with `-fcf-protection`, a probe function that is a bare return is again refused as another target's (D3 not closed there)

- Where: `test/tools/kernels_inline.py:105-109` and `:134`. `only_returns` holds only if every instruction is `ret`. Under CET each function starts with `endbr64`, and GCC also writes numeric labels (`0:` to `4:`) in `.note.gnu.property`, which `LABEL` (`:58`) does not match and which therefore count as instructions. Decision 0244 item 2 says the refusal passes "a function that is a bare return", without that condition.
- Scenario: `probes/bare_return_cet.txt`. `dot(Point, Plane)` alone passes on both compilers with the test's flags, and fails on both with `-fcf-protection=full` added: "the output is not x86-64 assembly". Some toolchains enable CET by default; Ubuntu's GCC is one (that is from my own knowledge; I did not check it here). A maintainer there who adds such a function to the probe gets D3's misleading failure from `rga3.kernels_inline_alone`. The probe as it stands passes under CET in both modes on both compilers, so there is no failure today.
- Suggested fix: now that registration guards the target (D5), drop the per-output refusal, or base it on `<compiler> -dumpmachine`. Otherwise, treat `endbr64`/`endbr32` as neutral and match numeric local labels. Then word item 2 to match.

### E3 (minor, comments only): the macro's comment still says the attribute is on the public operations "found" out of line, which the survey contradicts

- Where: `include/galoot/core/assert.hpp:27-28` ("on those public operations that GCC or Clang was found to keep out of line"), against `:35-37` of the same comment ("some operations on conformal(3) multivectors of all blades still compile to a call"). The survey found 12 GCC and 14 Clang public operations kept out of line at `-O2`, and none of them carries the attribute.
- Scenario: a maintainer reads the first sentence as "every operation a compiler was seen to call carries the attribute". From it they conclude that `dot` or `bulk` of conformal(3) multivectors was never seen out of line, which the record shows is false. D2's suggested wording ("that the structure test found out of line") would make the two sentences agree. Also, the parenthesis names "multivectors of all blades", while GCC also calls on even multivectors (`bulk`, `left_bulk_dual`, `dot` of `CEven`). As an example it is not false, only narrower than the evidence.
- A comment-only change needs no delta review (W5); the coordinator lists it with its resolution.

### E4 (nit): the reflow of the fixture-test comment was missed, and it understates the script

- `test/bench/CMakeLists.txt:171` is a 114-column comment line ("... Each fixture defines first and second. With --alone, it compiles"). The comment says the script reads an immediate loaded "into the register or its 32-bit half", but it reads any narrower part (`WIDE` covers the 16- and 8-bit names).

## Usability (public headers)

No name, signature or result changes. The pack rename is invisible to users except through E1. Hyrum's law: forcing `project` and `antiproject` makes the target-mismatch error reachable from two more operations (`probes/target_project.txt`). A caller of `project` built for fewer instruction-set features than its unit compiled at `414879e` and does not compile at `26f1b62`. Decision 0244's consequence covers this ("Forcing the public products makes this reachable from more call sites"), but it does not name the projections. Every attribute added before release has this cost, and a later removal breaks no one.

## Open items for the owner and coordinator (with trade-offs)

1. **E1: the pack's name.** (a) Rename again to a name free of `I`, `l`, `1`, `O`, `0`, and widen the user unit to the scan's set: two lines under `include/`, one more delta review, and the clash class is closed for this pack. (b) Keep `Is` and record that `ls` clashes: no review cost, but the documentation lane's short names must avoid `ls`. Recommendation: (a), folded into the next `include/` change rather than given a review of its own, if one is coming.
2. **E2: how the structure test knows its target.** (a) Drop the refusal; registration already guards the target. Least code, and the fixture `aarch64.s` and its test go too. (b) Ask the compiler (`-dumpmachine`): one subprocess per run, and the refusal then means what it says. (c) Teach the refusal about `endbr64` and numeric labels: smallest change, but it keeps guessing from instructions. Recommendation: (a) or (b). The guess has now been wrong twice.
3. **E3: the macro's comment.** Replace "that GCC or Clang was found to keep out of line" with "that the structure test found out of line", and "of all blades" with "of all or of even blades". Comment-only; recommendation: do it before the merge.
4. **O2, the knob, as before.** GCC's debug cost of `product_test.cpp` is +80.5% (2.16 s to 4.32 s, 418 to 829 MiB), confirmed here at the tip. The D1 fix did not change it measurably. Recommendation: the owner rules on O2 from these numbers; nothing in this round changes the trade.

## Resolution

Filled in on 2026-11-11 by Claude (agent, Opus 5.5), implementer of plan 0040 section 18, at the coordinator's request, from the commits of `feature/pre-release-core-pass` and its subfeatures and of `merge/pre-release-core-pass-gate2`. Each commit named below is on `merge/pre-release-core-pass-gate2`.

| Finding | Resolution |
| --- | --- |
| E1 | Fixed by option (a): the packs are `Xs` and `Ys` (`fe839fc`), and the linted user unit names `l` and `ls` (`e59cd1f`, labelled failing). Its delta review, `2026-11-11-pre-release-core-pass-delta3-review.md`, approved. |
| E2 | Fixed by option (a): the per-output refusal is dropped (`36575d5`), with a `-fcf-protection` fixture (`6fcb3fc`, labelled failing). |
| E3 | Fixed in a comment (`ee05a66`). |
| E4 | Fixed: the comment is reflowed and names every narrower register (`36575d5`). |
| Open item 4 (O2) | Ruled by the owner after gate 2 (decision 0244 item 16). |
