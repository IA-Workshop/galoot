# Blind review: WU10a of plan 0019 (antiproducts, involutions, complements, `%` `&` `~` `!`)

- Reviewer: blind-reviewer role, with the usability checklist of the role file (decision 0160). Claude, agent, Opus 5.5 (`claude-opus-5-5`), 2026-11-02.
- Range: `develop...feature/core-antiproducts`, merge base `91fffe3`, branch head `47dab5c`, 13 commits (`2c969a8` to `47dab5c`). `develop` has not moved since the merge base, and `git merge-tree` with `develop` is clean.
- Acceptance criteria: plan 0019, sections 2 and 3 (WU10a only). Author's departures: plan 0019, sections 1 and 10, decisions 0165 and 0166, and `records/mutations/0019-antiproducts/README.md`.
- Outcome: **merge after fixes.** The mathematics is right. I checked it against a reference of my own, written from the definitions and sharing no code with the library or with `golden.py`. It covered every blade pair of nine forms, including non-diagonal forms at even n (n = 2, 4, 6) and at odd n, and found no mismatch. The diagnostics in the catalogue behave as specified on both compilers. One finding blocks, and its fix is small (F1). The public antiproduct on `full<5>` of conformal(3) does not compile on Clang 23 with libstdc++ in an ordinary optimized build. No preset builds that configuration, and the user documentation says nothing about the limit; it points into a `detail` comment whose stated bound is wrong for this case. The fix is to document the limit where users read, correct the internal comment, and record the gap in the build matrix. The other findings do not block. A change to a header after this review needs a delta review.

## 1. Departures, skipped checks, ambiguities (C20)

1. **Did the brief carry a verdict?** No. The brief named the range, the criteria, the departures, and what to look for. The branch itself holds the author's account in plan 0019 section 10. I read it as a set of claims to check (section 6).
2. **Independence.** The test author and the implementer were both Claude Opus 5.5 agents, with the doctest-expert and cpp-expert role briefs. I am the same model with the blind-reviewer role brief and a separate context. The configurations differ by role and context, not by model. W5 asks for "a different configuration that does not share the author's working context", and I read that as met, but the owner may want a different model for a core change.
3. **Nothing edited in the reviewed worktree.** `git status` in the worktree was clean at the end. I built in my own trees: `scratchpad/build/gcc-debug` and `scratchpad/build/clang-debug`, plus `scratchpad/w5/build-{lint,clang-asan,clang-measure,gcc-measure}`, all configured from the worktree's sources. Mutants ran in `scratchpad/w5/src`, a `git archive` copy of the branch head with its own tree `scratchpad/w5/mbuild`. Probes are in `scratchpad/w5/probes`.
4. **Shared scratch area.** The scratchpad is shared with the coordinator's earlier agents. My first gcc-debug and clang-debug runs wrote `build-gcc-debug.log`, `build-clang-debug.log`, `test-gcc-debug.log`, and `test-clang-debug.log` at the scratchpad root, which overwrote earlier agents' files of the same names. Nothing relies on scratch (W6), but I report it. I did not read the other agents' scratch files.
5. **Run log.** Every build, test, mutant, trace, format, and run-log check went through `tools/timed`. The log is `scratchpad/w5/runs.tsv`, 43 rows at `47dab5c`, not the repository's run log. My first six rows also went to `scratchpad/runs.tsv` before I moved to my own file. **Not run through `tools/timed`:** about 40 `-fsyntax-only` and `-c` diagnostic and limit probes with g++ and clang++, the two dump programs' runs, the Python reference, and `git` inspection. That is a P13 lapse, reported here.
6. **Not checked:**
   - The gcc-release, gcc-asan, and clang-release presets. I ran gcc-debug, clang-debug, clang-asan, and lint at `47dab5c` from new trees. The author's rows cover all seven presets, but on a dirty tree (F3).
   - The W10 and P2 figures and the growth figures of question 3. I read them and checked that the antiproduct comment's "about twice" and "about four and a half times" match the recorded +106.8% and +356.5%. I did not re-measure.
   - My mutants on clang-debug. I ran them on gcc-debug only.
   - The assembly that the new functions produce (P4). They forward to `detail::apply`, which this unit does not change.
   - `mutate.py`, `measure.py`, and `diagnostics.py` beyond their headers and output rows.
   - WU10b's sections of the plan.
7. **Ambiguity.** The brief asks whether the Clang limit on dense n = 5 forms is documented for users. I read "users" as readers of the public headers (`product.hpp`, `unary.hpp`, `core.hpp`), not of `detail` comments or agent records.

## 2. What I checked, and how

| Check | Result |
| --- | --- |
| Full suite, gcc-debug, own new tree at `47dab5c` | 260 of 260 passed (`-LE runlog`; the preset's `^runlog$` filter gives 261) |
| Full suite, clang-debug, own new tree | 262 of 262 passed (263 with the preset filter) |
| Full suite, clang-asan, own new tree (ASan + UBSan, halt on error) | 263 of 263 passed |
| lint preset (clang-tidy, warnings as errors), own new tree | build exit 0, no warnings; 263 of 263 passed |
| `all_verify_interface_header_sets`, gcc-debug and clang-debug | built; `product.hpp` and `unary.hpp` each compile on their own |
| `format-check` target and `clang-format --dry-run -Werror` on the 18 changed C++ files | clean (`product.hpp:198` is 87 columns, and clang-format leaves it) |
| Convention trace gate (W5 command, from the root) | exit 0; 2.1, 3.1, and 7.1 gain rows that `unary_test.cpp` and `product_test.cpp` state |
| `tools/runlog check` | exit 0 |
| Record citations in added code, test, and build lines (decision, plan, WU, F-numbers, provision numbers, agent words) | none |
| Independent reference (`scratchpad/w5/ref.py`) against a dump of the library (`scratchpad/w5/dump.cpp`) | 8,990 comparisons on GCC and 7,820 on Clang, 0 mismatches (below) |
| Typed builds without `_GLIBCXX_ASSERTIONS`: `clang-measure` and `gcc-measure` presets, targets `core_product_test`, `core_unary_test`, `core_identities_test` | **Clang fails** to compile `product_test.cpp` and `identities_test.cpp`; GCC builds all three (F1) |
| Diagnostic and constraint probes (section 5) | g++ 16.2.1 and clang++ 23.1.4, `-std=c++23`, the test warning flags |
| My mutants (section 4) | 7 on gcc-debug: 5 killed, 2 survive (F2) |

Mathematics. `ref.py` builds the Clifford algebra of an arbitrary symmetric form in the wedge basis from v X = v ⌋ X + v ∧ X. It extends that to blades through e_a ∧ R = e_a R − e_a ⌋ R. It takes the complements from u ∧ right(u) = 𝟙 and left(u) ∧ u = 𝟙 (CONVENTIONS 3.1), forms a ⟇ b = left(right(a) ⟑ right(b)) and a ∨ b = left(right(a) ∧ right(b)) (4.2), and takes the grade signs from 2.1. I checked it on two hand values: e4 e5 = −1 + e45 in conformal(3) (CONVENTIONS 1.1), and e423 ∨ e431 = e34 in rigid(3). The dump prints the library's `%`, `&`, `*`, `~`, `antireverse`, `grade_involution`, `!`, and `left_complement` of every basis blade or blade pair, by canonical mask. The forms:

- n = 2: R(1,1,0), and the Witt form [[0,1],[1,0]];
- n = 3: a random non-diagonal form;
- n = 4: rigid(3), plain and under its oriented names, conformal(2), a random non-diagonal non-degenerate form, and a non-diagonal degenerate form;
- n = 5: conformal(3);
- n = 6: a dense random non-diagonal form, on grade 2 × grade 3 and grade 4 × grade 5 pairs (GCC only), and on all 64 blades for the unary maps.

Every value agrees. The even-n non-diagonal forms are the case the golden composition test does not reach (F5).

## 3. Criteria (plan 0019 section 3, WU10a)

| # | Verdict | Evidence |
| --- | --- | --- |
| 1 Values against independent references | met | `unary_test.cpp:346-395` against `grade_signs` and `complements` for `full<3>` to `full<5>` and rigid(3) oriented; `product_test.cpp:473-519`, the golden composition over four forms and rigid(3) oriented; the identities over the signature set. My independent reference agrees on nine forms, including the even-n non-diagonal ones. |
| 2 Layouts | met | Sign maps keep `Line`, `Motor`, and a conformal(3) flat point (`unary_test.cpp:419-430`). Complements and antiproducts give canonical sets written from the definition (`unary_test.cpp:432-442`, `product_test.cpp:577-626`). |
| 3 Operators | met | `%` and `&` (`product_test.cpp:541-575`); `~` and `!` within `sign_mismatches` and `complement_mismatches`, which check type and value. |
| 4 Diagnostics | met for the catalogue | One error on both compilers in the `const auto r = ...;` form, message on the first error line, no `apply:` (20 compile-fail tests, my probes). Clang's first line for a mismatched `(p % q) == l` names `geometric_antiproduct_layout<rga3, ...>`, with no `product_plan` or `detail::`. Outside the catalogue, the Clang constant-evaluation limit gives three errors that begin inside `plan.hpp` (F1). |
| 5 Detection | met | `can_antimultiply`, `can_antiwedge`, `can_call_*` (`product_test.cpp:688-713`); the `user_code` scopes with `widget` and `gadget` for `* ^ % &` and for `~ !`. |
| 6 Mutants | met as planned; two of my own survive | All 15 of the author's rows show kills on both presets. Two of my seven survive (F2). |
| 7 Checks | met with a reporting defect | The author's final rows are on `28dd10cad9d0-dirty` (F3). My runs on the clean head cover gcc-debug, clang-debug, clang-asan, and lint. |
| 8 No record citations | met | grep above |

## 4. Mutants of my own

Applied by `scratchpad/w5/mut.py` to the copied source, built with gcc-debug flags (`core_product_test`, `core_unary_test`, `core_identities_test`), and run against labels `core.product|core.unary` (23 tests, including the 20 product compile-fail tests and `core.identities`). Stale binaries of programs that failed to build were deleted, so CTest could not run an old build.

| Mutant | Result |
| --- | --- |
| `geometric_antiproduct` checks the **geometric product's** coefficients instead of the antiproduct's | **survives** |
| `operator%` checks the geometric product's coefficients | **survives** |
| `operator%` returns `geometric_antiproduct(y, x)` | killed: `core.product`, `core.identities` at run time |
| `operator&` returns `antiwedge(y, x)` | killed: `core.product`, `core.identities` at run time |
| The `static_assert` of `geometric_antiproduct`'s check is dropped and the `if constexpr` guard kept, so a narrow scalar silently returns zero | killed: `core.product_narrow_scalar_geometric_antiproduct` |
| `antiwedge_layout` is taken from the wedge's plan | killed: `product_test.cpp:534` and `:608` (`static_assert`s of the test) |
| `antireverse` uses the grade involution's map | killed: `core.unary`, `core.identities` at run time |

The survivors are explained in F2: on `full<2>`, the only input of the narrow-scalar tests, the two plans have the same coefficients up to sign.

## 5. Findings, most severe first

### F1. On Clang 23 with libstdc++, the public antiproduct of `full<5>` in conformal(3) does not compile in an optimized build. No preset builds that configuration, and users are not told. Major; blocks until documented (small fix).

`include/galoot/core/product.hpp:31-32`, `:326-333`, `:371`; the `apply` comment in `include/galoot/core/mv.hpp:779-783` (pre-existing, now reached through the public spelling).

Probe `scratchpad/w5/cga_ap.cpp` computes `a % b` with `a` and `b` both `mv<cga3, full<5>, std::int64_t>`:

| Compiler, flags | Result |
| --- | --- |
| clang++ 23.1.4, `-fsyntax-only` or `-c -O0` | compiles |
| clang++, `-c -O1` or `-c -O2` | **fails**: "constexpr evaluation hit maximum step limit" in `reorder_sign`, reached through `make_product_plan<geometric_antiproduct, ...>` |
| clang++, `-c -O0 -D_GLIBCXX_NO_ASSERTIONS` | **fails**, the same way |
| clang++, `-c -O2 -D_GLIBCXX_ASSERTIONS` | compiles |
| clang++, `-c -O2 -fconstexpr-steps=2000000` | compiles |
| clang++ with `-stdlib=libc++`, `-O0` and `-O2` | compiles |
| g++ 16.2.1, `-O0`, `-O2`, `-O0 -D_GLIBCXX_NO_ASSERTIONS` | compiles |
| clang++, `-O2`: `&` and `*` on the same operands | compile |

So the deciding factor is libstdc++'s assertions, which libstdc++ 16 turns on by itself only in unoptimized builds. `plan.hpp`'s comment records the conformal(3) antiproduct at 947,839 of Clang's 1,048,576 steps with assertions on, and says that without them "both compilers count as more". That is enough to cross the limit. All seven test presets define `-D_GLIBCXX_ASSERTIONS` (the `checks` and `sanitizers` blocks of `CMakePresets.json`), so the suite never builds the configuration most Clang users ship. Building the `clang-measure` preset (`-O2`, no assertions) confirms it: `core_product_test` and `core_identities_test` fail to compile, both at the conformal(3) `full<5>` antiproduct (`product_test.cpp:257` in the golden composition and `:646` in associativity; `identities_test.cpp:285-333`). `gcc-measure` builds all three programs.

What the user sees: three errors. The first is `plan.hpp:553: constexpr variable 'count' must be initialized by a constant expression`, then `no matching function for call to 'filled_plan'`, then `invalid operands to binary expression`. No message names the operation or the algebra (C9).

What the documentation says:

- `product.hpp`'s header comment: "each pair of operand layouts builds one plan, within the limits that detail::apply states". That sends the user into `detail` (as the WU9 review's F7b warned).
- `apply`'s comment: "Measured at the default limits, at -O0 and -O2, with and without _GLIBCXX_ASSERTIONS: apply builds wherever its plan builds, up to full<5> times full<5> on Clang 23". That is false for this antiproduct without assertions.
- `geometric_antiproduct`'s comment: it gives the growth with n, but no bound.
- The test author's finding about the dense n = 5 forms is in the plan's outcome "for the coordinator", not in any header. Plan section 1 question 3 also says "users of models with n ≤ 6 see the figures of question 3". A user of conformal(3), the shipped five-generator model, finds no statement that a general multivector of it may fail on Clang.

Typed conformal(3) layouts are fine at `-O2` on Clang: vectors × vectors, grade 2 × grade 3, grade 3 × grade 3, grade 4 × grade 4, even (16 blades) × even, odd × odd, even × vectors, and `full<5>` × vectors all compile (`scratchpad/w5/cga_typed.cpp`). The limit is about the full 32-blade multivector, and about dense non-diagonal forms of five generators, which also fail for the geometric product.

Suggested fix, within this unit:

1. In `geometric_antiproduct`'s and `%`'s comments, and in `product.hpp`'s header comment, state the bound a user can predict. On Clang 23 with libstdc++, an antiproduct of `full<5>` by `full<5>` in conformal(3), and any product of `full<5>` in a dense form of five generators, exceeds the default constant-evaluation limit unless libstdc++'s assertions are on. Give the workaround (`-fconstexpr-steps=2000000` works, as do typed layouts) and describe the error the user will see.
2. Correct `apply`'s comment so it no longer claims `full<5>` × `full<5>` at `-O2` without assertions for the antiproduct.
3. Record the configuration gap in the plan's outcome. Whether a no-assertions optimized Clang build of `core.product` joins the merge checks is the coordinator's or owner's call. It would cost one more tree and would have caught this.

Making the plan cheaper (decision 0140) is out of this unit's scope. The documentation is not.

### F2. The narrow-scalar tests of the antiproduct cannot tell the antiproduct's check from the product's. Minor; does not block (test only, cheap).

`test/core/product_narrow_scalar_{geometric_antiproduct,percent}_fail.cpp`, `test/CMakeLists.txt:1379-1393`. Both tests use `mv<diag(100, 100), full<2>, std::int8_t>`. Right complement is a bijection on the blades of `full<n>`, so on `full × full` the antiproduct's plan and the product's plan have the same coefficients up to sign. My two mutants replace `coefficients_exact<geometric_antiproduct, ...>` by `coefficients_exact<geometric_product, ...>`, in `geometric_antiproduct` and in `operator%`. Both pass every test.

The layouts do differ elsewhere. On the scalar layout `of_grade<2, 0>` in the same form, the product's only coefficient is 1, and the antiproduct's is −10,000, since right(1) = e12. Probe `scratchpad/w5/probes/narrow_scalar_layout.cpp`:

- The branch gives one error: "a % b: the scalar type cannot hold one of the antiproduct's exact coefficients; ...".
- With `operator%`'s check mutated, the user gets "geometric_antiproduct: ..." for `a % b`, the wrong spelling.
- With both checks mutated, the kernel's "apply:" message returns, which F5 of the WU9 review removed.

Suggested fix: change the two antiproduct compile-fail sources to the scalar layout, or add two that use it. One expected text needs no change.

### F3. Process: the final runs were on a dirty tree, and six commits in a row do not build. Minor; does not block; correct the record.

- The outcome says "all seven presets from new empty build trees at `28dd10c`". The run-log rows say `28dd10cad9d0-dirty` (2026-11-02T13:06 to 13:17). C20: "a check is reported as passing only if it ran on the revision the report describes". The dirt was probably the uncommitted records of `47dab5c`, but the record does not say so. My runs at the clean head `47dab5c` pass on gcc-debug, clang-debug, clang-asan, and lint (section 2). gcc-release, gcc-asan, and clang-release have no clean-tree run. Suggested fix: say in the outcome what was uncommitted, or rerun those three.
- W2 allows "that single commit" with a new test to leave the project failing. Here the test author's five test commits and their lint pass came first, and the code followed. So `3a22686`, `fdc1cb6`, `0d3d990`, `c120caa`, and `a53d386` all leave `product_test.cpp` unbuildable, although none of them is the test commit that caused it. Section 4 of the plan interleaved test and code, which would have satisfied W2. Each message says it does not build, which helps bisection. The ordering is not listed among the outcome's departures. Suggested fix: list it.
- `28dd10c` combines a header comment change with the `slow` label on the test author's test. No expected value changes, so W9 is not engaged, but the commit does two things.

### F4. The six public layout variable templates are justified, but they lack their own accept/reject assertions and each lacks its own `///` comment. Low; does not block; before release.

`product.hpp:186-209`, `unary.hpp:31-44`.

- **Justified?** Yes, on the evidence of decision 0166. A `detail` name leaves `detail::` on Clang's first line, and an alias template prints its expansion with `product_plan`. My probe of a mismatched `(p % q) == l` on Clang shows `mv<rga3, geometric_antiproduct_layout<rga3, blade_set<4>{...}, blade_set<4>{...}>, double>` and no internal name. They also give users a way to name a result type, which the rule of decision 0165 makes a stable promise. The cost is six names that are expensive to rename after release (W13). The `_layout` suffix reads like the user's own `point_layout` and `line_layout` in the examples, which is consistent.
- **S7.** They are metaprogramming facilities, and S7 asks for `static_assert`s in the header or the test for at least one argument each accepts. They are reached only through `decltype` of the products' results. Those expected types are independent, so the evidence is real but indirect. Decision 0166 says "the review may ask for those". I do: one line each, for example `static_assert(core::wedge_layout<rga3, point_layout, point_layout> == core::of_grade<4, 2>)`.
- **S11.** "Each declaration in a public header has a `///` comment immediately before it." The four product layouts share one `///` block, and the second to fourth follow the NOLINT lines and the previous declaration. The same holds for `left_complement_layout`. A one-line `///` before each would meet the rule.

### F5. Even-n non-diagonal antiproducts are checked only relationally in the suite. Low; does not block.

The golden composition (`product_test.cpp:473-496`) covers rigid(3) (n = 4, diagonal), conformal(3) (n = 5), and the two n = 3 forms. The forms that are not diagonal are all odd n, where left and right complements agree. Even-n non-diagonal forms (the Witt forms, conformal(2), the random n = 4 forms, conformal(4)) appear only in `identities_test.cpp`, on three random samples per form. The De Morgan laws there would catch most errors, but they lean on a geometric product that is golden-checked only for the four golden forms. My independent reference found the library correct on four even-n non-diagonal forms at n = 4 and one at n = 6 (section 2), so this is a gap in evidence, not a defect. Suggested fix, later: a golden product table for one non-diagonal even form, for example conformal(2), or an exhaustive blade-pair De Morgan check for one such form.

### F6. The header-set fix is right, but nothing stops the gap from recurring. Low; does not block.

`CMakeLists.txt:18-28`. `product.hpp` was missing from `galoot_core`'s `FILE_SET` from WU9 until this branch. As a result, `all_verify_interface_header_sets` never compiled it on its own, and `format-check`, which also reads the header set, never checked its layout. Both now include it, and I built both targets on both compilers. A configure-time comparison of `file(GLOB include/galoot/core/*.hpp)` with the header set would turn the next omission into an error.

### F7. Comment gaps a user meets. Low; does not block.

- (a) The examples in `product.hpp:7-15` and `unary.hpp:7-10` use `rga3`, `point_layout`, `plane_layout`, and `line_layout` without declaring them. With a four-line prelude (`scratchpad/w5/probes/examples.cpp`), they compile with `-Wall -Wextra -Werror`, and every stated value holds: `p & g` is 0, and `reverse(l)` is −l. The checklist asks that every example compile. Name the declarations, or say where they come from.
- (b) Only `geometric_product`'s catch-all comment says that a requires-expression answers yes for a rejected call. The function catch-alls refer back with "as for geometric_product". The operator catch-alls (`*`, `^`, `%`, `&`) say only "Rejects x % y for operands ...", and the header comment does not mention it either.
- (c) `!p == t`, with `t` of the complement's layout, means `(!p) == t`, as intended, but GCC with `-Wall` warns `-Wlogical-not-parentheses`, and under `-Werror` that is an error (probe `bang_eq.cpp`). Clang is silent. The `!` comment could say to write `(!x) == y`.
- (d) For a class derived from `mv`, `~d` and `!d` work by deduction and return the base `mv` type, not the derived class (probe `derived_ok.cpp`). `unary.hpp` says "~line is a Line" without that caveat. This is for WU16's model classes.
- (e) The `reverse` comment names `using namespace std::ranges`. `using namespace std::views` hides it the same way. Mentioning both costs nothing.

### F8. The `slow` label: justified. Info.

`test/CMakeLists.txt:1315-1324`. The measurement supports it: 295 G instructions, 19 s, and 1.4 GB on GCC, 4.6 times `mv_test.cpp`. My gcc-debug full build took 19.6 s of wall time, which is about this one unit. P9 allows the label and says deferral never omits a check, and the test is in every merge selection. Two notes for the coordinator:

- The cost is the build, but the label gates the test. It only helps if the pre-commit selection builds only selected targets (P12).
- Because `core.identities` also carries `core.product` and `core.unary`, `-L core.product -LE slow` would drop it even for a product change. The rule "runs when the change touches the products or the unary operations" is followed by the agent, not by a tool.

Peak memory of 1.4 GB per GCC compile matters under P11 only if several such units compile at once, which no other unit does today.

## 6. The author's departures and outcome claims, judged

1. Section 1 departures 1 to 5, readings R1 to R6, questions 1 to 7, and the owner's rulings O1 and O2: accepted for WU10a. Departure 2 (no conjugation) and R6 (the `std::ranges` case documented, not worked around) match the code and the tests.
2. Outcome departure 1, public layout names: accepted, with F4.
3. Outcome departure 2, the header-set fix: accepted, with F6.
4. Outcome departure 3, the `a ^ b` trace row: accepted. The trace gate passes.
5. Outcome departure 4, the `slow` label: accepted (F8).
6. Outcome departure 5, one rejecting function with 16 messages: accepted. The messages are verbatim from `records/diagnostics/wu10a-messages.md`, and each condition compares the types, so Clang prints `std::is_same_v<rga3, cga3>`.
7. Outcome departure 6, the rewritten `%` mutant: accepted. The rows of the second run show kills on both presets.
8. Outcome departure 7, probes outside `tools/timed`: accepted as reported. I have the same lapse (section 1).
9. The test author's finding on Clang and dense n = 5 forms: accepted as a finding. It is incomplete, though, because conformal(3) `full<5>` fails for the antiproduct in optimized builds, and it is not carried into user documentation (F1).
10. "All seven presets ... at `28dd10c`": not accepted as written (F3).
11. "All 15 killed": consistent with `out/gcc-debug.tsv` and `out/clang-debug.tsv`. My F2 shows the planned set missed one weakness.
12. Diagnostics, sizes, Clang's first line: consistent with my probes. The 0018 README reports an untraced increase of about 450 characters in GCC's longest line against the WU9 figure. It is reported as untraced, which C20 allows.

## 7. Usability checklist

Task first. From the examples alone, the three tasks are:

1. Test whether a point lies on a plane: `p & g`, then read the scalar. The example says "zero, p is on g" but does not show how to read it. I used `core::get<"1">(p & g)`, which I knew from `mv.hpp` and not from `product.hpp`. With p = (2, 3, 5, 7) and g = z − 5, it gives 5 − 35 = −30, as CONVENTIONS 5.3 predicts.
2. Reverse a line: `Line r = ~l;` works, and `~l` is a `Line`, as the comment promises.
3. Turn a point into the plane of its complement: `Plane{!p}`, as `unary.hpp`'s header comment suggests. It gives the plane coefficients (x, y, z, w) unchanged, and `left_complement(!p)` gives `p` back.

I read the implementation only for F1, which needed `plan.hpp`'s and `mv.hpp`'s comments to explain a compile failure the public headers do not mention.

Names:
- [x] Field terms or full English words: `geometric_antiproduct`, `antiwedge`, `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`.
- [x] One meaning, and nothing the standard library means differently. `reverse` collides with `std::ranges::reverse`; that is documented and tested (F7e adds `std::views`).
- [x] Similar things look alike: the `anti` prefix, `left_`/`right_` prefixes, `(x, y)` order everywhere, and `<op>_layout<Alg, ...>` for all six result layouts.

Construction and conversion:
- [x] No new constructors. Results are values, and the zero places are T{}.
- [x] No implicit lossy conversion. The sign maps keep the input's layout (nothing lost), and complements go to canonical sets that users convert from explicitly (`Plane{!p}`).
- [x] No hidden default: the result's scalar is the operands' scalar, and the layout rule is stated in each header.

Misuse and diagnostics:
- [x] Each misuse in the catalogue (mixed algebras, mixed scalars, narrow scalar, for the four new spellings, and rewording for the four old ones) has a compile-fail test on both compilers with the literal message.
- [x] One error, message on the first error line, in the `auto r = ...` form (tests and my `both_differ.cpp`).
- [~] The message starts with the operation and has no internal names. True for the catalogue. Not true for the Clang constant-evaluation limit (F1), whose first error is inside `plan.hpp`.
- [x] Context lines show the user's terms: the algebra tags, and on Clang `geometric_antiproduct_layout<rga3, ...>`. Layouts still print as masks, a known limit.
- [x] Distinct causes give distinct messages. Algebra and scalar both mixed gives the algebra message.
- [~] The documentation says which probes give a wrong answer, for the functions only, not for the operators (F7b).

Semantics and cost:
- [x] Nothing discarded. The comments state the layout rule, that `!!x` is ±x by grade, that `!x` is not a bool (`if (!x)` fails on both compilers, `if_bang.cpp`), and that the antiwedge does not depend on the metric.
- [x] `auto r = f(a, b);` is safe. The results are values, and `constexpr` use works (`constexpr_use.cpp`: `~~l == l`, `(p & g)` in a `static_assert`).
- [x] Exact comparisons use `==`.
- [ ] The comment states the cost a user can predict: the growth with n, yes; the bound on Clang, no (F1).

Examples and readability:
- [~] Examples are the idiom to copy and parenthesize `&` and `^`, but they rely on undeclared names (F7a).
- [x] A maintainer finds each behavior from its name. The products' checks and messages are in `product.hpp`, and the maps are in `unary.hpp`.

Before release, the interface choices this review touches that cost more to change later (Hyrum's law):

1. The six public names `geometric_product_layout`, `wedge_layout`, `geometric_antiproduct_layout`, `antiwedge_layout`, `right_complement_layout`, `left_complement_layout`, and their argument order `<Alg, A, B>` and `<Alg, S>` (F4).
2. The layout rule of decision 0165: sign maps keep the input's type, and everything else gives the canonical set. Users will write `~line` as a `Line` and `Plane{!point}`.
3. `!` returning a multivector, so `!x` is never a truth test, and `!!x` is not `x` at even n.
4. The catch-alls make requires-expressions answer yes for mismatched operands of `%` and `&`, as for `*` and `^`.
5. The 16 pinned message texts.
