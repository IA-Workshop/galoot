# Blind review: WU9 of plan 0018 (geometric_product, wedge, `*`, `^`)

- Reviewer: blind-reviewer role, with the usability checklist of the role file (decision 0160). Claude, agent, Opus 5.5 (`claude-opus-5-5`), 2026-11-02.
- Range: `develop...feature/core-products`, merge base `a75ce5e`, branch head `dd8a77e`, 7 commits (`5c5188f` to `dd8a77e`). `develop` has moved since the branch left it: it is now at `5db4c7e`, five commits ahead (see F14).
- Acceptance criteria: plan 0018, sections 2 and 3. Author's departures: plan 0018, sections 1 and 10.
- Outcome: **block, on one small fix (F1), with F2 and F3 fixed in the same lines.** The arithmetic is right. Every pair of basis blades of rigid(3) and conformal(3) matches the independent tables, and the rename is mechanical. The block is about diagnostics. The most natural way to write a mixed product, `auto r = a * b;`, gives two errors, and on GCC the library's message comes second. On Clang the first line of every one of the eight new messages starts with an internal enum cast and a raw byte. Both problems sit in the one function `detail::reject_product` and its four callers. The fix is a few lines, and I tested it in a scratch copy. The other findings do not block. A fix to the header after this review needs a delta review.

## 1. Departures, skipped checks, ambiguities (C20)

1. **Did the brief carry a verdict?** No. The brief gave the goal, the range, and pointed to the criteria and departures. The branch itself holds the author's account in plan 0018 section 10 ("all seven presets passed", "six mutants were killed on gcc-debug and clang-debug"). That section is part of the change. I read it as a claim to check, and two of its statements do not match the branch's own records (F4).
2. **Independence.** The author was Claude Sonnet 5.5, working as the coordinating agent without sub-agents. I am Opus 5.5 with a separate context, so the configuration differs (W5). I did not see the author's scratch files. All the mutants in section 4 that I ran myself were applied by my own script.
3. **Nothing edited in the reviewed worktree.** I built in my own trees, `scratchpad/build-gcc-debug` and `scratchpad/build-clang-debug`, configured from the worktree's sources. Mutants and probes ran on copies of `include/` in `scratchpad/mut/` and `scratchpad/probes/`. `git status` in the worktree was clean after every step.
4. **Run log.** My builds, tests, mutant compiles, and the trace and run-log checks went through `tools/timed` with `--log scratchpad/runs.tsv`, not the repository's run log, because I may not edit the worktree. The merging agent can move those rows if it wants them. **Not run through `tools/timed`:** about 25 `-fsyntax-only` diagnostic probes with g++ and clang++, `clang-format --dry-run`, `git merge-tree`, and git inspection. That is a P13 lapse on my side, reported here.
5. **Not checked:**
   - The full suite, lint (clang-tidy), and the sanitizer presets. The author's rows show all seven presets passing at `04dc275`, and `dd8a77e` changes only the plan and the run log, so P10 applies. I did not reproduce the claim that these runs started from new, empty trees. The suite must run again after `develop` is merged in (F14).
   - My mutants on clang-debug. I ran three of my own mutants on GCC only, at -O0 with the test flags.
   - The assembly that `geometric_product` and the operators produce (P4). They forward to `detail::apply`, whose code was read in WU8b.
   - The P2 growth and W10 figures, which the branch does not provide beyond one smoke-test pair (F10).
   - `mv_test.cpp` and `plan_test.cpp` in full. For these, and for the other files in the rename commit, I checked mechanically that the commit changes nothing but the two enumerator names and the line breaks clang-format makes around them (section 2). I read `product.hpp`, `product_test.cpp`, the eight compile-fail sources, the CMake additions, `mv.hpp`, and the `plan.hpp` diff in full.
6. **Ambiguity.** Decision 0162 item 8 says the ring-operations message "waits, and is reworded with the WU9 messages once they exist". I read this as possibly due on this branch. The branch does not reword it, and its plan does not say it was deferred again (F11).

## 2. What I checked, and how

| Check | Result |
| --- | --- |
| `core_product_test` and label `core\.product` (1 unit + 8 compile-fail), gcc-debug, own new tree | 9 of 9 passed |
| The same, clang-debug, own new tree | 9 of 9 passed |
| Convention trace gate (W5 command, from the root) | exit 0; 7.1 is pinned at `test/core/product_test.cpp:159`, and 4.1 gains `product_test.cpp:134` |
| `tools/runlog check` on the branch | exit 0 (1 era matches; 1727 kept rows present once) |
| `clang-format --dry-run -Werror` on every changed C++ file | clean |
| `product.hpp` compiled on its own, g++ and clang++, `-Wall -Wextra -Wpedantic -Werror` | clean |
| Rename commit `5c5188f`: for each file, the parent with `gp`/`antigp` replaced by the new names, compared with the commit ignoring whitespace | 23 files identical; in `mv_test.cpp` and `plan_test.cpp` the only differences are line breaks that clang-format moved. No expected value changed. |
| Leftover old names in `include/`, `tools/`, test JSON | none; the test-local aliases `gp`/`antigp` in `plan_test.cpp` and the test names like `plan_antigp_beyond_n` are test-only names, which decision 0159 item 3 keeps |
| Record citations (decision, plan, WU, note, provision numbers, agent words) in added code, test, and build lines | none (criterion 7) |
| Trial merge with current `develop` (`git merge-tree --write-tree`) | clean |
| My probes of operand forms and diagnostics (section 5) | g++ 16.2.1 and clang++ 23.1.4, `-std=c++23` |
| My mutants (section 4) | 3 run on GCC, all killed; 2 compile-only mutants analyzed |

Mathematics: the oracle is independent. `golden::gp_tables` comes from the reference scripts, not from the library. The wedge oracle, the a∪b term of the gp table when a ∩ b = ∅ and zero otherwise, is right in a non-diagonal metric as well, because the top-grade part of e_A e_B for disjoint A and B is e_A ∧ e_B (CONVENTIONS 4.1). I checked by hand that conformal(3) in the golden table is the non-diagonal form, with g45 = −1, so the test covers that case.

## 3. Criteria

| # | Verdict | Evidence |
| --- | --- | --- |
| 1 Same values | met | The rename comparison above. All presets passed at `04dc275` (author's rows). |
| 2 Equality with the plan | met by construction, not tested by value | `geometric_product` is a one-line return of `detail::apply<geometric_product>(x, y)`. No test compares the two directly. "the result has the layout the plan gives" checks the type for the wedge only. |
| 3 Operators | met | `product_test.cpp:159-188`: two layout pairs over `double`, type and value. |
| 4 Diagnostics | **not met** | (a) No message names either algebra on its first line (F3). (b) `auto r = a * b;` gives two errors, and on GCC the message is not on the first error line (F1). (c) Sizes are recorded for 3 of the 8 cases, in the mutation README rather than beside the WU8 table. The eight pinned texts do pass on both compilers in the `(void)(...)` spelling. |
| 5 Mutants | partly met | The author's README marks two mutants "not run separately" on clang-debug and did not run the sixth planned mutant. I ran the sixth (a sign flip for a shared-generator pair) and the operand swaps on GCC: all killed at run time by the table check (section 4). |
| 6 Checks | met for the branch as it stands; must be repeated after the merge of `develop` | Author's rows at `04dc275` for all seven presets. My `core.product` runs on two presets. `develop` has since changed `CMakePresets.json` and the run-log rules (F14). |
| 7 No record citations | met | grep above. |

## 4. Mutants

Mine, in a copy of `include/`, built with the test flags on g++ -O0 and run:

| Mutant | Result |
| --- | --- |
| `geometric_product` calls `apply(y, x)` | killed at run time (2 test cases, 4 assertions) |
| `wedge` calls `apply<wedge>(y, x)` | killed at run time (2 test cases, 4 assertions) |
| Shared-generator sign flip in `blade_product.hpp`: the contraction term is negated when B has grade 3 and i = 1 (the planned sixth mutant) | killed at run time: the table checks at `:135` and `:136`, and associativity at `:222` and `:227` |

Analyzed by compiling `product_test.cpp` against the mutant on both compilers:

| Mutant | Result |
| --- | --- |
| Catch-alls of `*` and `^` unconstrained | killed by `!can_multiply<R, double>`, `!can_multiply<double, R>`, and `!can_wedge<R, int>`. The `std::complex` and `std::bitset` assertions still pass (F8a). |
| `mismatched_operands` without `!product_operands` | **survives** `product_test.cpp` on both compilers. The valid overload still wins by partial ordering. The clause only changes the result for odd operands such as a `volatile` mv, which the mutant would send to the wrong message, "different scalar types" (F8f). |

## 5. Findings, most severe first

### F1. `auto r = a * b;` with mixed operands gives two errors; on GCC the library's message comes second. Major; blocks (small fix).

`include/galoot/core/product.hpp:65-100, 120-176`. The catch-alls return `void`. When a user writes the result into a variable, which is the usual form, the compiler also reports the `void` variable:

- GCC 16.2.1, `auto r = a * b;` (rigid(3) Line times conformal(3) full): `error: deduced type 'void' for 'r' is incomplete` is the **first** error, then `error: static assertion failed: operator*: ...`. That is 2 errors in 18 lines.
- Clang 23: the library's message comes first, then `error: variable has incomplete type 'void'`. That is 2 errors.
- The mixed-scalar case `auto r = wedge(a, b);` behaves the same way on both compilers.

The eight compile-fail tests all write `(void)(a * b);`, which hides this. The checklist asks that "each misuse gives one error, and the message is on the first error line", and criterion 4 asks for "one error". `get` already avoids this problem: its catch-all returns the type that `get` returns, "so that the message is the only error".

Suggested fix: have `reject_product` and the four catch-alls return a value type, for example `std::remove_cvref_t<X>{}`. I tested this in `scratchpad/mut/fix`: `auto r = a * b;` then gives exactly one error on both compilers. Add compile-fail cases in the `auto r = ...` form, at least for `*` and one named function. Generic code can see the catch-all's return type through `decltype`, so this belongs before release.

### F2. On Clang, the first line of each new message starts with an internal enum cast and a raw byte. Minor; should fix with F1.

`product.hpp:73-98`. Each `static_assert` tests `Spelling != times_operator` and so on, so the compilers print that condition:

- Clang: `error: static assertion failed due to requirement '(galoot::core::detail::ProductSpelling)'\x02' != times_operator': operator*: the operands are in different algebras; ...`, then the note `expression evaluates to '2 != 2'`.
- GCC: the note `'(galoot::core::detail::ProductSpelling::times_operator != galoot::core::detail::ProductSpelling::times_operator)' evaluates to false`.

The `'\x02'` appears because `ProductSpelling`'s underlying type is `std::uint8_t`, which Clang prints as a character. My minimal probe `enum_print2.cpp` shows `(E8)'\x01'` for `uint8_t` and `(EI)1` for `int`. The checklist says "no masks, positions, or internal names", and the mutation README's statement that "the message is the first thing on the first line" is not true on Clang. The library's other catch-alls avoid this: `get` puts `static_assert(false, "...")` inside an `if constexpr` chain, and `mv` uses named boolean rules.

Suggested fix: an `if constexpr (Spelling == ...)` chain with `static_assert(false, ...)`, or the F3 form. I checked the chain on both compilers: the first line is then `static assertion failed: <message>`.

### F3. No message names the two algebras, as criterion 4 requires. Minor; should fix with F1 and F2.

Criterion 4: "each mixed-algebra and mixed-scalar mistake gives one error whose first line names both algebras". The messages are fixed literals that name neither algebra. The tags do appear in the instantiation context, inside lines of 1,962 characters on GCC and in the notes on Clang.

Suggested fix, in the spirit of STYLE S7's rule for counts ("its static_assert tests the numeric condition itself, so that the compiler prints the values"): assert the condition on the types, for example `static_assert(std::is_same_v<typename R::algebra_type, typename S::algebra_type>, "a * b: ...")`. My probe `name_algebras.cpp` gives:

- on Clang: `static assertion failed due to requirement 'std::is_same_v<rga3, cga3>': ...` on the first line;
- on GCC: the message, then the note `'struct rga3' is not the same as 'struct cga3'`.

Do the same with `scalar_type` for the scalar case. This also removes F2. If the owner prefers the literal-only form, criterion 4 should be reworded by decision rather than left unmet.

### F4. The Outcome overstates the mutant evidence. Minor; does not block; correct the record.

Plan 0018 section 10 says "six mutants were killed on gcc-debug and clang-debug". The branch's own `records/mutations/0018-products/README.md` disagrees:

- It marks the two constraint mutants "not run separately" on clang-debug.
- It says the sixth planned mutant ("flip one table sign for a shared-generator pair") "was not run here".
- The run log has five failing builds per compiler at `eaa0074` (09:18:24 to 09:18:53) and no test rows for the mutant whose kill is listed as a run-time kill.

My runs settle the substance on GCC (section 4), so the tests are adequate. The report still has to say what ran (C20). Suggested fix: reword the Outcome to match the README, and add my sign-flip result or run the mutant on clang-debug.

### F5. A narrow scalar in a public product gets a message that starts with `apply:`. Minor; should fix in this unit or record a deferral.

`mv.hpp:797-800`, reached through `geometric_product` and `*`. My probe `narrow_scalar.cpp` multiplies `mv<big, full<2>, std::int8_t>` by itself, where `big` has the Gram matrix diag(100, 100). Both compilers give one error:

```
apply: the scalar type cannot hold one of the product's exact coefficients; use a scalar type that holds larger integers exactly
```

On Clang the line is prefixed by `due to requirement 'scalar_holds_every_coefficient_of_the_product'`. The user wrote `a * a` and sees `apply`, an internal name. The `///` comment of `geometric_product` promises this rejection, but no compile-fail test covers it through the public spelling. The WU8b review (F7 there) asked that "WU9 should give each product's catch-all its own prefix".

Suggested fix: check the coefficient condition in `geometric_product` and `wedge` themselves, with their own messages, or pass the spelling through to `apply`. Then add one compile-fail case.

### F6. Derived-from-mv and volatile operands with a mismatch get the compiler's own error, although the header says "anything else is rejected ... with a message". Minor; does not block.

`product.hpp:15-17` and `:53-63`. `multivector<X>` is false for a class derived from `mv`, so the catch-all does not match one:

- `MyLine` (derived from rigid(3) `Line`) times a conformal(3) mv gives `no match for 'operator*'` on GCC (47 lines) and `invalid operands to binary expression` on Clang (19 lines).
- `volatile Line * Line` gives the compiler's error on both.

Valid derived operands do work: `MyLine * Line`, `MyLine ^ Line`, `geometric_product(MyLine, MyLine)`, and unqualified `wedge(a, b)` found by argument-dependent lookup all compile on both compilers (`derived_ok.cpp`). A scalar operand (`2.0 * a`) is the compiler's error, as plan section 1 item 2 says.

Suggested fix: reword the header comment ("operands of two mv types that differ in algebra or scalar are rejected with a message"), and pin the derived-class case in a test. Widening the catch-all to `multivector_or_derived` can wait for the model classes of WU16.

### F7. The header's comments omit facts a user needs. Minor; does not block.

`product.hpp:4-17, 104-176`.

- (a) The catch-alls do not say that a requires-expression asking whether `x * y` is valid answers yes for mismatched operands. The test pins this (`can_multiply<R, F>`, `can_wedge<R, C>`). `get`'s comment states the same property for itself. The checklist asks that "the documentation says which probes give a wrong answer".
- (b) The `geometric_product` comment says "evaluated as detail::apply evaluates it", which points the user into `detail`.
- (c) The comments do not say what the result layout is. It is the reached blades in canonical order with orientation +1: `decltype(V{} ^ V{})` is `mv<rga3, of_grade<4, 2>, int>`, which I checked on both compilers. So `get<"e41">(p ^ q)` is rejected with the orientation message, and `(p ^ q) == line` does not compile when `line` uses another layout. I had to read `plan.hpp`'s comment to learn this.
- (d) The comments do not state the compile-time cost a user can predict: one plan per pair of operand layouts, and the size limits that `apply` states.
- (e) The header example uses `a` and `b` without declaring them and declares a `Line` it never uses. Its `(a ^ b) == c` suggests that `c` can be of any type.

### F8. Test weaknesses. Minor; does not block (test only).

`test/core/product_test.cpp`.

- (a) `:250-256`, the `std::complex` and `std::bitset` checks, cannot fail. The test is at global scope with no `using` of `galoot::core`, and the catch-alls can only be found by argument-dependent lookup on a multivector. My unconstrained-catch-all mutant left both passing. Either remove them (V2) or move the probe into a scope where `galoot::core`'s operators are visible, for example after `using namespace galoot::core;`, with a user type of its own that has `*` and `^`.
- (b) `:192-205` takes its expected type from `detail::product_plan` itself, which is the implementation. An independent expectation is available: `mv<rga3, of_grade<4, 2>, std::int64_t>`. That would also pin the layout users see (F7c).
- (c) `:134-137`: a failure prints only a count. Add `INFO`/`CAPTURE` of the pair (i, j) and the mask in `pair_mismatches`, so a failure names the blades.
- (d) No compile-fail case for `auto r = a * b;` (F1) or for a derived operand (F6).
- (e) Plan section 2 named `rga3`, `pga3`, and a conformal algebra "over `int`". The test covers rigid(3), which is R(3,0,1) (the PGA signature), and conformal(3), over `std::int64_t`. This is acceptable, but it is a quiet departure.
- (f) The `!product_operands` clause of `mismatched_operands` survives every test (section 4). Either pin its effect, or drop it as redundant (S16) and accept the volatile case.

### F9. The new detail traits and concepts lack their S7 statements and accept/reject assertions. Low; does not block.

`product.hpp:41-63`. STYLE S7 asks each metaprogramming facility to state what it computes and to be followed by `static_assert`s for an argument it accepts and, for a constraint, one it rejects. `same_algebra_v` and `same_scalar_v` have no comment. None of the four (`same_algebra_v`, `same_scalar_v`, `product_operands`, `mismatched_operands`) is asserted directly in the header or the test; they are only reached through `can_multiply`. Also, `<concepts>` is included but nothing from it is used directly.

### F10. Process. Low; does not block.

- W9: the tests and the code they judge are one commit (`2604a53`). The lossy message and its pinned expectation are one commit (`664e1a9`). The Outcome names the merged commits but does not list the tests and expected values, as W9's exception requires. No independent test author was used, although section 9 planned a doctest-expert, and the implementer was Sonnet rather than the Opus implementer section 9 names. The oracle is the independent golden table, which limits the risk.
- C18: plan section 2 says the choice of `product.hpp` over `mv.hpp` is "recorded as a decision". The branch adds no decision record.
- W10/P2: the DoD's "P2 growth of the new includer" is not measured. The only cost figure is one smoke-test pair (0.261 s before, 0.247 s after) with a failed instruction counter.
- W7: four of the seven commit messages give no reason, only a title.

### F11. The ring-operations message is not reworded and not deferred. Low; ask the coordinator.

Decision 0162 item 8: the ring-operations message (decision 0152) "waits, and is reworded with the WU9 messages once they exist". `mv.hpp:474-476` still reads "mv: the scalar type must support +, -, *, unary - and construction from an integer coefficient", with no fix after a semicolon. Plan 0018 section 1 item 6 mentions only the lossy message. Either reword it here or record that it waits for a named unit.

### F12. Clang spells every product result type with internal names and a raw byte. Low; pre-existing, newly visible to users; before release.

`plan.hpp:86` (`enum class BinaryOperation : std::uint8_t`). In any compiler error that involves a product's result type, Clang prints the type unevaluated. For `(p ^ q) == line` (probe `task_compare.cpp`), the type appears as:

```
mv<rga3, product_plan<(galoot::core::detail::BinaryOperation)'\x01', rga3, blade_set<4>{...}, blade_set<4>{...}>.result, double>
```

This is the same `uint8_t` effect as F2. Making the underlying type `int` changes `'\x01'` to `1`. Giving the result a layout whose printed form is its value, such as a variable template that holds `product_plan<...>.result` by value, would remove `product_plan` and `detail` from the text. Generic code can observe the result type, so settle this before release.

### F13. Message wording, for the owner. Info.

- The mixed-scalar fix "convert one explicitly first" points to a conversion the library does not have. The owner ruled this (decision 0162 item 2). Until a scalar conversion exists, a user must rebuild the value coefficient by coefficient.
- The mixed-algebra fix "both must be in the same one" restates the requirement rather than giving a step.
- The messages begin `operator*:` and `operator^:`, while the user wrote `a * b`. `a * b:` would match "the operation as written" more closely.
- Two tags with equal metrics (`rga3` and a second tag that is also `rigid<3>()`) are reported as "different algebras". This agrees with `mv`'s rule that the tag is the algebra, and the context shows both tag names, so it is not a defect.

### F14. `develop` has moved; the merge must bring it in first. Info for the merging agent.

`develop` is at `5db4c7e`, five commits past the merge base `a75ce5e`. They change:

- `CMakePresets.json`: the test presets now exclude the `runlog` label;
- the P7, P9, and P13 rules, so a merge that touches `runs.tsv` runs `tools/runlog check` once, and gate runs are named `gate:<selection>`;
- the decision files: decision 0163 is now taken, inside plan 0018's reserved range 0162 to 0169.

My trial merge (`git merge-tree`) is clean, and `tools/runlog check` passes on the branch as it stands. Per W7 and P9, merge `develop` into the branch, run the full suite on each compiler with one sanitizer preset and the style checks on the result, run `tools/runlog check` on the merged tree, and choose decision numbers that avoid 0163.

## 6. The author's departures, judged

1. Section 1 items 1 to 6 (WU9 scope; `*` taking two multivectors only; one algebra and one scalar; the plan's result layout; D2 and D3 deferred; the owner's rulings): accepted. The scalar operand gives the compiler's error, as stated (probe `scalar_left.cpp`).
2. Section 10: commits 2 to 4 were made as one commit. Accepted as reported, with the W9 gap of F10.
3. Section 10: no sub-agents, so no independent test author. Accepted as reported. It is a weaker process than the plan intended (F10), but the independent golden oracle limits the effect.
4. Section 10: "six mutants were killed on gcc-debug and clang-debug". Not accepted as written (F4).

## 7. Usability checklist

Task first. From the examples alone, the three tasks are:

1. Multiply two motors or lines: `auto m = a * b;`. This works. Storing the result in a named type, `Motor{a * b}`, needs `mv.hpp`'s conversion comment, not `product.hpp`'s.
2. Join two points into a line and read a coefficient. `Line{p ^ q}` works (probe `task_store.cpp`). `get<"e41">(p ^ q)` is rejected with the orientation message, because the result is in canonical order. I learned that from `plan.hpp`, not from `product.hpp` (F7c).
3. Check a product against an expected value: `(p ^ q) == line`. This does not compile unless `line` has the result's exact layout, and Clang's error then shows the F12 spelling. The header's `(a ^ b) == c` does not warn about this.

I read the implementation (`plan.hpp`'s description of `product_plan`) only for the result layout in tasks 2 and 3.

Names:
- [x] Field terms or full English words: `geometric_product` and `wedge`. The rename to full words is done.
- [x] One meaning, and none the standard library gives differently.
- [x] Similar things look alike: both functions take `(x, y)`, and each operator is a one-line alias (S13).

Construction and conversion:
- [x] No new constructors. A product's result is a value whose zero places are T{} (inherited from `apply`).
- [x] No implicit lossy conversion is added.
- [x] No hidden default: the scalar of the result is the operands' scalar, and the layout follows from the operands' types.

Misuse and diagnostics:
- [x] Each misuse in the unit's catalogue (mixed algebras, mixed scalars, for four spellings) has a compile-fail test on both compilers that requires the literal message.
- [ ] One error, with the message on the first error line: not for `auto r = a * b;` (F1).
- [ ] The message starts with the operation and has no internal names: the Clang condition text (F2), and `apply:` through the public spelling (F5).
- [~] The context lines show the user's terms: the algebra tags appear, but layouts appear as masks. That is a known limitation from WU8. No message names the algebras (F3).
- [x] Distinct causes give distinct messages. Mixed algebra and scalar together gives the algebra message, which is reasonable.
- [ ] The documentation says which probes give a wrong answer: not in the header (F7a).

Semantics and cost:
- [x] Nothing is discarded. The wedge's comment says it does not depend on the metric.
- [x] `auto r = f(a, b);` is safe: the result is a value, and rvalue operands work at compile time (probe `constexpr_ok.cpp`).
- [x] The tests compare exactly with `==`. The comparison over `double` compares two evaluations of one plan.
- [ ] The comment states a cost the user can predict: the result's blades only loosely, and no compile-time cost (F7c, F7d).

Examples and readability:
- [ ] The header example does not compile as written (F7e). It does parenthesize `^`.
- [x] A maintainer can find each behavior from its name, and the four messages for each cause sit in one function.

Before release, the interface changes this review suggests, each of which breaks users more after release (Hyrum's law):

1. The catch-alls' return type (`void` today) is visible through `decltype(a * b)` for mismatched operands (F1).
2. Requires-expressions answer yes for mismatched operands. Generic code may come to rely on this (F7a).
3. The result type's spelling and layout policy (canonical order, `product_plan<...>.result`) is what users will write as `decltype(a * b)` or name in aliases (F7c, F12).
4. The pinned message texts, including `operator*:` against `a * b:` (F13).
5. The two-multivector constraint of `*` must stay unambiguous when WU10 adds scaling. A catch-all widened to `mv * scalar` would change today's compiler errors into library messages, which is the intended direction.

## Resolution (coordinator, Claude Sonnet 5.5)

F1, F2, F3 fixed together in `product.hpp`; F4 corrected in the plan outcome and the mutation README; F6 and F7 by the header comment; F8 (a, b, d partly, f) and F9 in the tests; F10 by decision 0164; F11 and F12 fixed; F5, F8c, F8d (derived operand), F13 beyond the prefix, and the P2 measurement are deferred as the plan outcome says; F14 done (develop merged in, decision number 0164 chosen).
