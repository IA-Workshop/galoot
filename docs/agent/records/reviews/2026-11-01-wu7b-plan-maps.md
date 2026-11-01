# W5 review: feature/core-plan-maps (plan 0013, WU7b) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01, applying the math-expert role file for the mathematics. Agent work.
- Reviewed: `git diff 7704146...feature/core-plan-maps` (merge base `7704146`), head `50c7c70` (18 commits). The code and tests at `50c7c70` are `bb0fde5`'s (`git diff bb0fde5 50c7c70 -- include test` is empty); `bb0fde5` changed only the `///` comments of `plan.hpp` after `d56af9f`. `develop` has moved since (`f556ad8` to `c4b2e70`); those merges were not reviewed.
- Brief: it gave the branch, its base, criteria 12 to 19 of plan 0013 section 5 with sections 2, 6, and 10, and the author's eight departures, and asked for an adversarial review. It carried no verdict. It did name the author's equivalence claim for the left-right swap and asked me to check it, which I did from the definitions before reading the author's proof.
- Blindness: not fully blind. Early on, a `git diff` of the plan file showed me the first part of the plan's "Outcome, WU7b" section (its departures list, its "Done" table, and the first rows of its tests table) before I had read the code. I read the rest of that section only after forming the views below. A second point: the session's scratch directory already held the author's working files (logs, probes, a `check_commit.sh`). I did not read them and moved my work to a separate directory. My first script there was written as `full.sh`, which may have overwritten an author scratch file of that name. Nothing relied upon lives there.
- Verdict: **accept after fixes**. I found no defect in what is generated. Every antiproduct term, map term, sign, result set, and order I derived or computed independently agrees, on both compilers, in nine forms, including non-diagonal, degenerate, and odd-n ones. F1 (a records correction) and F2 (a test-file lint fix) should land before the merge. F3 to F5 do not block it. None needs a change to `include/` unless the author takes up F4's wording, which would then need the delta review of a comment change.
- Job pool: never started or stopped. At my first build it showed 0 of 23 slots free, all used by other workstreams' clients. Later readings showed 6 and 11 free.

## Not checked or not run (C20)

- The author's 68 mutants were not rerun, except the two that plan section 10 names. I ran 26 of my own, plus those two, on gcc-debug and clang-debug in normal mode only, not with the generation-time checks turned off.
- Presets I ran from new, empty trees at `50c7c70`: gcc-debug, clang-debug, lint, and gcc-asan. I did not run gcc-release, clang-release, or clang-asan; the author's rows at `d1017ea` (same code and tests) show all seven passing (P10).
- Limits: boundary points only, bare `-std=c++23 -pedantic-errors -O2 -fsyntax-only`, not the two Galoot-flag configurations, except where `_GLIBCXX_ASSERTIONS` decides the result.
- P2: my own units, two repeats, n = 4, 8, 16 only (not 12). The `metric_like` part of the growth was not separated out.
- W10 rows in `compile.tsv` were spot-checked (the before rows at `7704146` and after rows at `d1017ea` exist for both compilers), not recomputed.
- The knowledge-base branch `kb/antiproduct-identity-proofs` was not reviewed.
- Outside `tools/timed` (P13): the dumper program's sub-second runs, the Python syntax checks of my scratch scripts, git inspection, and the reads of logs. Every build, test, compile, oracle comparison, and measurement went through `tools/timed`.
- My run-log rows (`review-*` and `review-commit:*` selections) are not committed. They are in the uncommitted `runs.tsv` of my worktrees `.claude/worktrees/review-wu7b`, `review-wu7b-mut`, and `review-wu7b-commits`; the per-commit rows were also copied to `pc/runs-rows.tsv` in my scratch directory before each checkout.
- My independent harness (a C++ dumper, a Python oracle, mutation and measurement drivers) is scratch and not committed. Its results are therefore labeled not reproducible as committed (V10), although each step is described here so that it can be redone.
- Agent cost: about 95 tool calls. Token counts are not visible to me.

## The four W5 checks, and where they ran

All at `50c7c70` (code and tests of `bb0fde5`), from new, empty build trees in my worktree `.claude/worktrees/review-wu7b`, through `tools/timed`, with GCC 16.2.1 and Clang 23.1.4:

| Check | Result |
| --- | --- |
| Style: `format-check` and clang-tidy (lint preset: `all`, `all_verify_interface_header_sets`, `format-check`, warnings as errors) | build exit 0; the lint suite passes 141 of 141. clang-tidy does not cover `plan_evaluated_once.cpp`, which fails it when run by hand (F2) |
| Full suite, gcc-debug | `all` and the header sets build (exit 0); 139 of 139 pass |
| Full suite, clang-debug | `all` and the header sets build (exit 0); 141 of 141 pass |
| Also run: gcc-asan (sanitizers, P9) | 139 of 139 |
| Trace gate (`python3 -I tools/test_references.py trace ...`, run from the root) | exit 0. 4.2 is pinned at `plan_test.cpp:2900`. The items with no test are 3.2 to 3.4, 4.3 to 4.5, 5.x, 6.x, and 7.1 |
| Golden check (`golden.py --check test/reference/golden.hpp`) | matches |
| Review | this record |

## Checks and how

### Mathematics, derived independently

- Hand values, rederived from u ∧ right(u) = 𝟙, left(u) ∧ u = 𝟙, and the rigid(3) squares:
  - The maps of the oriented line, point, and plane (D2), every term: for example right(e41) = −e23, right(e42) = +e13, right(e43) = −e12, right(e4) = −e123, and right(e423) = −e1. The left complements of the vectors and trivectors in n = 4 are the negatives of the right ones.
  - The antiproduct pairs of D3: e234 ⟇ e234 = 𝟙; e123 ⟇ e123 = 0; e1 ⟇ e1 = 0; e4 ⟇ e4 = left((−e123)(−e123)) = left(−1) = −𝟙; 𝟙 ⟇ e1 = e1 ⟇ 𝟙 = e1; e234 ∨ e134 = left(−e12) = −e34; e1 ∨ e234 = 1; e234 ∨ e1 = −1.
  - The conformal(3) run e1235 ⟇ e1234 = left((−e4) e5) = left(1 − e45) = −e123 + e12345.
  - The counts of D7: P ⟇ P has 7 terms (only the pairs with −e123 survive), G ⟇ G has 15, G ∨ G has 12 (every term checked, for example (0,2) → left(e13) = −e24), M ⟇ P has 1 + 12 + 3 + 4 = 20, and M ⟇ M has 64 − 16 = 48.
  - The golden tables for n = 3 (`58b7e2d`), checked by hand.

  All agree with the tests.
- An independent oracle (scratch; Python with `fractions`). The geometric product comes by two routes: my own Chevalley recursion (e_A = e_i ∧ e_A′ = e_i e_A′ − e_i ⌋ e_A′) and `test/reference/clifford_reference.Algebra` (diagonalization and change of basis). The two agree on every pair. The complements are computed from the parity of the concatenated index lists, and the antiwedge from that parity on disjoint lists; none of this shares code with the header. A C++ dumper prints `generate_plan<antigp>` and `<antiwedge>` over `full<n>` × `full<n>`, run at run time, together with every `unary_term` for n = 1 to 8 and 13 typed `product_plan`s. Coverage and result:
  - Nine forms: rigid(3), conformal(3), conformal(4) (n = 6), the Witt plane [[0,1],[1,0]], a dense form of four generators, a degenerate non-diagonal form [[1,1,0,0],[1,1,0,0],[0,0,0,0],[0,0,0,−1]], R(1,3,0), a dense form of five generators, and a dense form of three generators.
  - Typed sets with non-canonical oriented names, for example conformal(3) {e21, e54, e3, e15, e5} × {e4, e5, e1, e432, 1}.
  - Result: 12,163 full-plan terms, 313 typed terms (result sets, positions, order), and 2,550 map images, all identical to the oracle.
- Identities on the oracle, in the same nine forms (n = 2 to 6):
  - Both De Morgan laws, for ⟑ and for ∧, with either complement, on every pair: no failure.
  - Associativity of ⟇ and ∨ on every triple for n ≤ 4: no failure.
  - 𝟙 as a two-sided identity of ⟇: no failure.
- The left-right swap (departure 6; decision 0115; `survivors.md`). My derivation, made before I read the author's: right(left(a) ⟑ left(b)) = σ(a)σ(b) Σ_D c_D right(e_D) = Σ_D σ(a)σ(b)σ(D) c_D left(e_D), with σ(u) = (−1)^(gr·ag). Both products keep the parity of grades in every bilinear form (the Clifford algebra is Z2-graded), so gr D ≡ gr a + gr b (mod 2). For odd n every σ is 1. For even n, σ(u) = (−1)^gr(u), so σ(a)σ(b)σ(D) = 1. The swap of the whole conjugation is therefore equivalent in every n and every form. Brute force agrees: in all nine forms the whole swap gives no difference on any pair. Swapping only the inputs, or only the outputs, gives 8 to 1,792 differing pairs in every even-n form and none in odd n. The plan's expectation (section 6, "survives every odd n, so the test must have even n") was mistaken, and the author's reading is correct. The defective partial swaps are killed (below).
- Section 2's order reversal: proved by hand. Within a grade, lexicographic order is decided by the lowest generator of A Δ B, and the complements keep A Δ B while swapping membership. Brute force over every ordered pair of distinct masks for n = 1 to 10 (1,396,054 pairs) finds no violation.
- The closed form of decision 0114: rederived (the j-th generator of A passes i_j − j generators of the complement). The test checks it against inversion counts for every mask up to n = 10.

### Tests against the criteria

- Every comparison in D5, D6, and D8 keeps the plan's own order. The golden composition and the De Morgan comparison sort only the composed side, so a missing run reversal fails them as well as failing the generation-time check.
- Each contract is pinned on both sides:
  - the map defect bounds (acceptance and rejection at each bound, signs −1, 0, and 2, a lower and a higher wrong position, n = 3 and n = 4 for the left factor);
  - `pair_terms`' constraint (accepts gp and the wedge, rejects both antiproducts);
  - the reversal check (the reversed run is accepted, the unreversed one flagged).

  The exception is F3.
- Rejections: each of the eight new compile-fail sources requires its literal message. The set checks forbid every generation message and the complement's range message, and `plan_map_not_metric_like` forbids `make_unary_map`.
- STYLE S11 and decision 0034: the added lines of `include/` and `test/` cite no governance, decision, plan, note, review, agent, or provision. The search for those words and for provision patterns matched only clang-tidy suppression reasons.

### Migration (departure 2, `54b17ae`)

Every changed test line of `54b17ae` adds the algebra's n to a `plan_capacity` or `pair_capacity` call (4, 16, 3, or `n`), plus clang-format reflows of three lines. No expected value, test name, or other line changes. The claim holds.

### Each commit from its committed tree (W2, departure 4)

In the worktree `review-wu7b-commits`, I checked out each commit, built `all` and the header sets, ran the full suite on gcc-debug, clang-debug, and lint (whose build runs clang-tidy and `format-check`):

| Commit | gcc-debug | clang-debug | lint (clang-tidy, format-check, suite) |
| --- | --- | --- | --- |
| `58b7e2d` | 131/131 | 132/132 | builds; 132/132 |
| `54b17ae` | 131/131 | 132/132 | builds; 132/132 |
| `dee3315` | 131/131 | 132/132 | **clang-tidy fails** (`plan.hpp:245` implicit widening, `:654` trailing comma, and others); `format-check` alone passes |
| `d56af9f` | 131/131 | 132/132 | builds; 132/132 |
| `a913760` | **138/139** | **139/140** | builds; **139/140**: `tests.reference_markers` fails on all three |
| `26a019e` | 139/139 | 140/140 | builds; 140/140 |
| `d6cbaba` | 139/139 | 141/141 | builds; 141/141 |
| `c6b7a4c` | 139/139 | 141/141 | builds; 141/141 |
| `bb0fde5` | 139/139 | 141/141 | builds; 141/141 |

This matches the author's departure 6 exactly: clang-tidy failed at `dee3315` only and the marker check at `a913760` only, and each was fixed in the next commit. `bb0fde5`, which the author did not check at itself, passes all three. Unlike WU7a, the record states the failed checks accurately.

### W9, W7, and the records

- W9: the golden data is in `58b7e2d`, before the code. The capacity signature change and the migration of its test calls are one commit (`54b17ae`), with the tests listed in its message. The code is in `dee3315` and `d56af9f`; the tests are in `a913760`, `26a019e`, and `d6cbaba`; the evaluation check is in `c6b7a4c`. Each test commit says it is listed for the review. After `d56af9f`, `include/` changed only at `bb0fde5`, in comments.
- W7: every message says what changed and why, and names the agent and its model, with the trailer.
- C18: decisions 0114, 0115, and 0116 exist, each with context, options, decision, and consequences. The names that differ from the plan (`UnaryOperation`, `map_defects_of`, and others) are recorded with their reasons.

### Mutants (reviewer's own, at `50c7c70`, gcc-debug and clang-debug, identical outcomes)

Driver: each anchor asserted unique; the program deleted before each build; no CTest after a failed build; otherwise the `core.plan` label (the doctest program, the compile-fail tests, and on Clang both evaluation checks); the header restored after each pass. It ran in a separate worktree, `review-wu7b-mut`. For build kills, the error lines were captured and the killing assertion identified.

| Mutant | Change | Result |
| --- | --- | --- |
| `R1_run_reversal_dropped` (plan section 10) | `run_term` reads `terms[k]` | killed at build by the header's "generated terms are out of order or repeated" (`plan.hpp:565`), both compilers |
| `R2_left_right_swapped` (plan section 10) | left sign on the inputs, right on the outputs | survives: equivalent (proved and brute-forced above) |
| `M01` / `M14` | the second or the first factor alone takes the left sign | killed at build by the D3 static_asserts (`plan_test.cpp:2101`, `:2107`, and others) |
| `M02` | the second factor is taken from the first set | killed at build (the point ⟑ point value at `plan_test.cpp:381` and others) |
| `M03` | the second factor's complement sign dropped | killed at build by D3 |
| `M04` | reversal dropped and the generated-order check disabled | killed at build by the typed order check (`plan.hpp:582`) and by the conformal two-term run (`plan_test.cpp:2127`), so the reversal is guarded three ways |
| `M05` | run index off by one | killed at build (libstdc++ subscript assertion in constant evaluation) |
| `M06` | the map's left factor by k² in place of k(n − k) | killed at build by the closed-form check (`plan.hpp:902`); GCC also reports an unused variable, so the check, not only the warning, kills it |
| `M07` | the closed-form check skipped for inputs of sign −1 | killed by "The checks of a typed map find each defect at its bound" |
| `M08` | the blade check applied to complements only | killed by the same case |
| `M09` | `unary_map` generated and checked with n + 1 | killed at build by the D2 static_asserts (the header's checks all use the same wrong n, so only the tests catch it) |
| `M10` | the map's output position set to its input position | killed at build by "a term is not on the blade that the operation gives its input" |
| `M11` | the antiproduct capacity complements only the first blade | killed by the capacity case |
| `M12` | the geometric product's capacity complemented | killed (capacity cases on GCC; constant-evaluation failure on Clang) |
| `M13` | `plan_capacity` given n − 1 | killed at build (complement range assertion) |
| `M15` | the map coefficient set to 1 in typing | killed at build by D2 |
| `M16` | the closed form drops k(k+1)/2 | killed at build by the closed-form check (`plan.hpp:902`; GCC also reports an unused variable) |
| `M17` | the antiwedge outputs left uncomplemented | killed at build by D3 and D7 |
| `M18` | the antireverse as the reverse sign of the complement mask | survives: equivalent (the antigrade is the grade of the complement) |
| `M19` | `reorder_sign` also counts equal generators | survives `core.plan`: equivalent within `plan.hpp`, every call of which has disjoint masks; killed by `bits_test.cpp:281` |
| `M20` | the map check stops at the first term past the result | survives: equivalent in effect (the defect is already flagged) |
| `M21` | `generate_map` ignores `out.size()` | **survives** (F3) |
| `E1` (clang-debug) | every local of `make_unary_map` plain | killed by `core.plan_map_evaluated_once`: ratio 0.8707 |
| `E2` (clang-debug) | every local of `make_product_plan` plain | killed by `core.plan_evaluated_once`: ratio 0.6419 |
| `E3` (clang-debug) | plain map locals, initializer generating twice | killed: ratio 1.9423 |
| unmutated | | ratios 0.0006 (plans) and 0.0007 (maps), both passing |

This covers the evaluation check against the plain-locals copies that plan section 10 asks for.

### Limits (criterion 19), bare -O2, `-fsyntax-only`, against the head's headers

GCC:

- `full<n>` × `full<n>` in R(n,0,0) builds at n = 6 under the antiproduct and fails at n = 7 (operation limit in `span`).
- The antiwedge builds at n = 7 and fails at n = 8.
- `unary_map<reverse>` of `full<n>` builds at n = 9 and fails at n = 10.

Clang:

- The antiproduct builds at n = 5 and fails at n = 6, in `count`.
- The antiwedge builds at n = 5 and fails at n = 6.
- The reverse map builds at n = 7 and fails at n = 8, in `found`.
- The right-complement map of `full<8>` fails at bare -O2 (in `mapped`) and builds with `-D_GLIBCXX_ASSERTIONS`.
- The conformal(3) `full<5>` antiproduct fails at bare -O2 and builds with `-D_GLIBCXX_ASSERTIONS`. This confirms the condition that decision 0116 had shown only for the map unit.

Every point agrees with the `///` comments and decision 0116.

### Cost (departure 8, P2)

My units are the four typed rigid(3) pairs (point × point, line × plane, motor × point, point × motor) and the sparse sets {1, e1, e_n, e1n} × {e1, e2, e_n}, embedded in R(n−1,0,1). Each unit reads every term at run time. Flags: Galoot's at -O0, `-c`. Each compile is pinned to CPU 10 and its retired instructions are recorded. Each figure below is the unit minus its baseline unit at the same n, the mean of two repeats, with a spread of at most 6.3 M. Other workstreams were compiling on the machine; instruction counts do not depend on that load.

| Use | Compiler | n = 4 | n = 8 | n = 16 |
| --- | --- | --- | --- | --- |
| four pairs, ⟑ | GCC | 306.0 M | 309.6 M | 323.8 M |
| four pairs, ⟇ | GCC | 309.0 M | 476.6 M | 1129.7 M (+266%) |
| four pairs, ⟇, capacity forced to 1 | GCC | 298.4 M | 464.3 M | 1121.1 M |
| four pairs, ⟑ | Clang | 548.7 M | 554.1 M | 571.8 M |
| four pairs, ⟇ | Clang | 703.2 M | 1380.9 M | 3941.7 M (+461%) |
| four pairs, ⟇, capacity forced to 1 | Clang | 692.8 M | 1370.7 M | 3925.4 M |
| sparse 4 × 3, ⟑ | GCC | 72.8 M | 76.6 M | 90.5 M |
| sparse 4 × 3, ⟇ | GCC | 76.2 M | 102.1 M | 197.4 M (×2.6) |
| sparse 4 × 3, ⟇ | Clang | 263.1 M | 352.3 M | 678.4 M (×2.6) |

The "capacity forced to 1" copy changes `pair_capacity` for the antiproduct to 1. That capacity is valid in a diagonal form, where every partial product is one term, so the copy generates the same plans. The author's growth figures reproduce within 1%. The experiment shows that the term-list capacity contributes about 1% at every n. Most of the growth therefore comes from the other cause decision 0116 names: `blade_product`'s recursion over the complements, whose grade is n − k. The capacity does not contribute.

## Findings, most severe first

| # | Where | Finding | Suggested resolution | Blocks |
| --- | --- | --- | --- | --- |
| F1 | decision 0116 (P2 section, "Cause"); plan 0013 outcome, open question 2 | **For the owner, with a record correction.** The fixed-use growth of the geometric antiproduct is real and reproduced: +266% (GCC) and +461% (Clang) from n = 4 to 16, against +6% and +4% for the geometric product. It does not follow from the algebra's own structure. The form is diagonal, so its signature data is as sparse as it can be. The result of each pair stays at one term with fixed result sizes, and in such a form e_A ⟇ e_B is one term determined by the two masks and the antimetric. The cost follows the route the code takes: the product of complements of grade n − k through a recursion whose work grows with grade. Under P2's own wording, that is growth contrary to C6, recorded as a defect, not the structural exception. The decision also attributes part of the cause to the term-list capacity, which my experiment shows contributes about 1%. Even at n = 4, Clang pays +28% for ⟇ over ⟑ on the same pairs. Since this is the direct derived form (C3, W4), building it first was right, and the measurement now shows a need. | Correct decision 0116's cause and the plan's open question 2 in a records commit: the recursion over complement grades, with this capacity experiment, and the growth recorded as a C6 and P2 defect for the owner to schedule, not as an open "structure or defect" question. The remedy (an antimetric pair rule, or a pair rule whose cost follows the inputs' grades) needs its own decision under C4, as 0116 says. The owner decides whether it blocks a release. | Fix before merge (records only); the remedy does not block |
| F2 | `test/core/plan_evaluated_once.cpp:70`–`:133` | STYLE S4: the unit fails the repository's `.clang-tidy` with warnings as errors: 14 `misc-use-internal-linkage` findings on the reader functions. WU7b added 8 (the motor ⟇ point and the three maps); 6 are from WU7a (the same run on the file at `7704146` gives 6). No preset runs clang-tidy on this unit, which departure 5 disclosed but did not run by hand. The external linkage is deliberate (the readers keep the plans emitted), so the finding has a reason that S4 wants written beside a suppression. | Add `NOLINT(misc-use-internal-linkage)` with the reason, or one `NOLINTBEGIN`/`NOLINTEND` block with it, and record the WU7a part as a defect found outside the unit (W9; small and in scope, so it can be fixed here). Optionally, give the lint preset a check that runs clang-tidy on this unit. Test-only commit. | Fix before merge (test-only) |
| F3 | `include/galoot/core/plan.hpp:687`–`:697`, `test/core/plan_test.cpp` | `generate_map`'s `///` comment states "Writes the first out.size() terms" and "Places of `out` past set.size() are not written", but no test calls it with an output shorter or longer than the set. The mutant `M21` (the `a < out.size()` bound dropped) survives both compilers. This is equivalent for every present caller, so no wrong map results; it is a contract pinned on neither side (WU7a lesson F3). | Add a case that calls `generate_map` with an output of K − 1 places (only those written) and with K + 1 places holding a sentinel (the last place unchanged). Test-only. | No |
| F4 | `include/galoot/core/plan.hpp:617`–`:620` (plausible) | "those four blades times three cost several times more through the geometric antiproduct at n = 16 than at n = 4" rests on no recorded measurement. `limits.tsv` has those sets at n = 8, 12, and 16 only, and the P2 record measures different sets. My measurement gives a factor of 2.6 on both compilers (table above), which "several times" slightly overstates. | Optional: "about 2.6 times more", with the measurement recorded. A comment change to `include/` needs the delta review of decision 0094 item 3. | No |
| F5 | plan 0013, "Outcome, WU7b", open question 1 | It lists the GCC -O0 debug size of `static constexpr` locals as carried and not decided, but the owner decided it on `develop` in decision 0118 (`f556ad8`, after the branch point): keep the locals. The record will be stale once `develop` is merged in, which W7 requires before the merge. | When merging `develop` in, update the open question to cite 0118 as closing it. Records only. | No |

Confirmations, not findings: the author's departure 6 (the whole-conjugation swap is equivalent) is correct, and the plan's section 6 expectation was the error. Departure 4's account of failed checks is exact. Departure 2's migration changes no expected value.

## Criteria (WU7b)

| # | Status | Evidence |
| --- | --- | --- |
| 12 | Met | D1 at n = 1 to 10 with the test's own rules and golden n = 3, 4, 5; my oracle agrees on all 2,550 map images for n ≤ 8; both parities of n are included; mutants M06, M09, M15, M16 are killed |
| 13 | Met | D4 composes the maps for n = 1 to 10; decision 0115's map relations rederived |
| 14 | Met | D6 over the 42 forms, n ≤ 6, as term lists in plan order; independent De Morgan checks in nine further forms |
| 15 | Met | D5 over the four golden forms; my oracle agrees in nine forms; the typed sets of D7 |
| 16 | Met | D8 at n ≤ 6 (associativity, identities, u ∨ right(u) = left(u) ∨ u = 1, reversal rules); my oracle checks associativity at n ≤ 4 in nine forms |
| 17 | Met | D2, D3, and D7 rederived by hand; the reversal mutant is killed by the generated-order check, and with that check disabled by the typed check and by D3 |
| 18 | Met | every map check fires on a mutant (the author's table, and my M06, M07, M08, M10, M16); the closed-form check kills the map-level swap and the `reorder_sign` mutant (author); the swap of the whole conjugation is equivalent, proved |
| 19 | Met, with F1 and F4 | limits spot-checked on both compilers and agree; evaluation checks pass and fail as required (my ratios above); P2 reproduced, with its cause corrected by F1; presets: four by me at `50c7c70`, seven by the author at `d1017ea`; W10 rows present |

V10 for this review. Verification was performed and passed: hand derivations, an oracle sharing no code with the header in nine forms, brute-force identities, and the mutants. Validation was not performed beyond rigid(3): the rigid(3) values agree with Lengyel's tables as the reference scripts encode them, and for non-diagonal antiproducts no published worked value was read, as plan departure 8 says. My harness results are not reproducible as committed (scratch).

## Revisions

- Head reviewed: `50c7c70`. Code and tests: `bb0fde5`. Base: `7704146`.
- The four W5 checks, the oracle dump, the limits, and P2: `50c7c70`.
- Per-commit runs: `58b7e2d`, `54b17ae`, `dee3315` (and `format-check` alone there), `d56af9f`, `a913760`, `26a019e`, `d6cbaba`, `c6b7a4c`, `bb0fde5`.
- The clang-tidy comparison for F2: `plan_evaluated_once.cpp` at `50c7c70` and at `7704146`, both against the head's headers.

## Delta review of 8cb3409 (comment-only header change after the review)

Reviewed by the coordinating agent (Claude Sonnet 5.5), 2026-11-01. The diff of `include/` from `438dbc1` to the merge result contains only the reworded comment in `plan.hpp` about the sparse sets. It now states the measured factors (2.5 times on GCC 16 and 2.6 times on Clang 23 through the antiproduct, against 1.2 and 1.1 through the product, n = 4 to 16), which match the measurement rows of `records/performance/plan-maps/out/sparse-438dbc1-lock-held/`. No finding.
