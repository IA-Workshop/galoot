# Blind review of WU12, the exact change of basis (plan 0030)

- Reviewer: Claude (agent, Opus 5.5, high effort), blind-reviewer role (W5), in a configuration that shares no working context with the authors of plan 0030's parts A to E.
- Date: 2026-11-09.
- Reviewed: `feature/core-basis-change` at `37f1729` against `develop` at `2b733f4`, which is also the merge base (develop is already merged in). Code (`include/`, `test/`, build files) is unchanged since `404f2fc`.
- Record branch: `records/wu12-blind-review`, branched from `37f1729` (W7: a review record is branched from the revision it reviews and cherry-picked into `develop`). Worktree `.claude/worktrees/wu12-blind-review`. Supporting files are in `2026-11-09-wu12-blind-review/`.
- The brief carried no verdict. It did carry the authors' counts (178 messages in the catalogue, 17 pinned messages); I recounted both rather than taking them.

## Departures, skipped checks, and ambiguities (C20)

1. **I read the plan's outcome section before my own checks.** Decision 0215 item 6 and plan 0030 section 11 ask the reviewer to read the outcome only afterwards. I read the whole plan file at the start, outcome included, before I knew it was there. It claims that every mutant died and that the seed campaign passed. I then checked those claims against the committed `out/*.tsv` and `seeds/runs.tsv` rather than relying on them. My own mutant spec was written from the header and the tests, not from the authors' specs. I listed their mutant names only to avoid duplicating them.
2. **Branch name.** Plan 0030 section 11 names `records/review-wu12`, and the brief names `records/wu12-blind-review`. I followed the brief.
3. **The role file says the reviewer returns the record and the merging agent commits it.** The brief asks me to commit it on a new branch. I followed the brief and committed nothing to the reviewed branch.
4. **Skipped: the full P9 matrix for a merge into `develop`** (eight presets from new trees). It has not run on this revision. The plan runs it after the review and its fixes (section 9 step 7), and any fix commit will change the revision. I ran the unit's P8 selection on gcc-debug and clang-debug from fresh trees instead.
5. **Not rerun (P10): `format-check` and lint.** Both passed on `1a9242d` (run rows of 15:30 UTC), and `1a9242d` has the same code as `37f1729`. I did not configure a lint tree.
6. **Not rerun: the authors' mutation specs, the seed campaign, and the cost campaign.** I checked their committed outputs row by row (below) and ran my own spec instead.
7. **Not checked: criterion 10 (Sage).** Another agent is doing it on `records/wu12-sage-cross-check`. It is marked pending.
8. **Not re-read: the published sources of criterion 9** (Lengyel's wiki, Colapinto, Hestenes, Doran et al.). I fetched nothing from the web. The test markers' readings rest on research note 0026 section B. I checked only that the tests state what those readings say, and recomputed the conformal and Witt images myself.
9. **Partly checked: the header's compile limits.** I probed the Clang limits and the vector limit on both compilers, but not GCC's upper limits for full layouts, which would cost minutes of compile time each.
10. Two of my mutants are **unjudged on clang-debug** by `tools/mutate` (finding N6). From the GCC run and the Clang error text I infer that both were killed. I did not confirm that by a second Clang run.

## What I checked and how

All builds and tests went through `tools/timed` with the shared pool. The pool read "running: 23 of 23 slots free" before and after each heavy run, and I never started or stopped it. My run rows are in `records/performance/runs.tsv` (branch `records/wu12-blind-review`), and the probe and script runs are in `2026-11-09-wu12-blind-review/runs.tsv`. No owner email or third-party contact was involved; nothing was fetched.

| Check | How | Result |
| --- | --- | --- |
| Build, fresh trees | configure and `cmake --build` for gcc-debug and clang-debug at `37f1729` | pass |
| P8 selection | `ctest -L '^(core\.basis_change\|support\.(diagonalize\|tracked)\|tests\.)'`, both presets | 41 of 41 pass on each |
| Trace gate | `python3 -I tools/test_references.py trace …`, from the root | exit 0; 1.4 now names `basis_change_test.cpp:280`; only 6.1 to 6.4 have no test |
| Run log | `tools/runlog check` | pass, before and after my rows |
| Header, line by line | read all 1,094 lines; replicated the elimination and the map generation in Python | see "Correctness of the header" |
| Proofs of note 0026 part A | my own Clifford implementation over `Fraction` in the wedge basis (Chevalley recursion), with complements, exomorphism, duals, antiproducts, and transform written from CONVENTIONS 3 and 4 and `dual.hpp`'s table; the math-tools venv; none of the authors' scripts | every factor and every iff confirmed on 9 changes (below) |
| Hand values of criteria 2 and 6, one LDLᵀ change | the same implementation | all equal to the tests' |
| Mutants | my own spec, 10 mutants (7 of the header, 3 of the tests' expected factors), gcc-debug and clang-debug | 2 survivors (F1, F2) |
| Compile-fail messages | 8 of the 25 tests compiled by hand on both compilers; error counts and first error lines read | 16 of 17 messages give one error with the message first; the overflow message does not (N1, declared) |
| Usability checklist | below, plus three misuse probes of my own | notes N2 to N5 |
| Process | cherry-pick patch-ids, commit labels, file sets of each commit, S11 search, governance diff | conforming |
| Cost | committed records read; Clang limits probed | consistent; notes N7 and N8 |

### Correctness of the header

- **Fraction-free Gauss–Jordan (`eliminate`, lines 346 to 384).** I replicated it exactly in Python, including C++'s truncating division, on 4,000 random integer matrices from 1 × 1 to 7 × 7, a third of them with a forced zero leading pivot. On every one, every division was exact. For singular matrices the pivot was 0. Otherwise |p| = |det M| and the right block was p M⁻¹ (`scripts/bareiss.py`, 0 bad). The pivot search is also correct: a column with no pivot at or below row k implies that M is singular, because the rows above k are zero in the columns before k.
- **The inverse (`inverse_rows`, lines 410 to 429).** P = D⁻¹M, so P⁻¹ = M⁻¹D, and column j is scaled by D_j, as the code does. The sign of p moves into the rows, and each row is reduced by the gcd of its entries and |p|. That is correct, but no test pins it (F2).
- **The isometry check (lines 454 to 486).** P G′ Pᵀ = G is equivalent to M G′ Mᵀ = D G D, and the code computes that, with 2n³ checked multiplications. It is correct, but no test sees its off-diagonal half (F1).
- **The image of a blade (lines 610 to 641).** It folds the rows over increasing generators with the wedge's pair rule. `term_list::add` merges a repeated blade and drops a zero sum, so a partial wedge of grade j holds at most C(n, j) distinct blades. The capacity C(n, min(k, ⌊n/2⌋)) is therefore sufficient, and it is attained. My replica reached exactly the capacity on dense rows for n = 1 to 8 (`scripts/mapcheck.py`).
- **The single division and the reduced denominator (`typed_change`, lines 733 to 777).** The code takes the lcm L of the terms' denominators, scales each coefficient to L, then divides by g = gcd(L, every coefficient). L/g is the least q_B: every valid q divides L, and L/q_min divides every scaled coefficient, so g ≥ L/q_min. Decision 0221 item 1 gives an equivalent argument. My replica of the generation matched the sympy minors of P on every blade of 300 random changes with n up to 5 and denominators up to 6. Every q_B equalled the lcm of the reduced denominators of its terms (0 bad). `run_change` then divides once per output, by the scalar's own division, after the integer sum (lines 843 to 863).
- **Overflow.** Every product, sum, lcm, and scaling goes through `change_multiply`, `change_add`, or `check_fits`, with magnitudes kept in −max to max, so negation never overflows. The `GALOOT_ASSERT` is on one source line between `clang-format off` and `on` (lines 241 to 245), and both compilers quote that line. The diagnostic is a cascade, though (N1).
- **Limits.** I probed at the default limits with `_GLIBCXX_ASSERTIONS` (`probes/limits.cpp`). On Clang, a dense full<5> builds and full<6> fails at the step limit; conformal_change<5> on full<7> builds and <6> fails at the step limit; dense vectors at n = 16 build. On GCC, dense vectors at n = 16 build. All of this agrees with lines 107 to 116.
- **complex<double>.** I suspected that `out / complex(Q)` (line 846) might round more than once for an odd q. On 500,000 draws with q in {3, 5, 7, 15, 62}, on both compilers, it equalled componentwise division every time. There is no finding.

### Independent recomputation of note 0026 part A

The scripts are `scripts/factors.py`, `factors2.py`, and `proj.py`; the outputs sit beside them. For each change, the script finds the single factor f with φ(op(x, y)) = f · op′(φx, φy) over random exact multivectors, or reports that none exists:

| Change | d | (C) P⁻ᵀG′P⁻¹ = G | ⟑ ∧ • ~ antirev ĝ | ∨ | ★, left ★ | ⟇, antidot, ☆, left ☆ | transform (even, odd) | right, left | bulk | weight |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| conformal(2) → R(3,1,0) | −1 | yes | 1 | −1 | −1 | −1 | 1, 1 | none | none | none |
| rigid(3), e4 ↦ 2e′4 | 2 | yes | 1 | 1/2 | 2 | 1/2 | 1/4, 1/4 | none | 1 | 1 |
| M diag(1,1,−1) Mᵀ, det M = 3 | 3 | no | 1 | 1/3 | 3 | none | none | none | none | none |
| rotation of R(2,0,0) by (3/5, 4/5) | 1 | yes | 1 | 1 | 1 | 1 | 1, 1 | 1 | 1 | 1 |
| swap in R(2,1,0) (orthogonal) | −1 | yes | 1 | −1 | −1 | −1 | 1, 1 | −1 | 1 | 1 |
| R(1,0,1), e2 ↦ 2e′2 | 2 | yes | 1 | 1/2 | 2 | 1/2 | 1/4, 1/4 | none | 1 | 1 |
| null shear of R(1,0,1) | 1 | no | 1 | 1 | 1 | none | none | none | none | none |
| Witt, one mode | −1 | yes | 1 | −1 | −1 | −1 | 1, 1 | none | none | none |
| LDLᵀ of [[2,1],[1,3]] → diag(2,10) | 1/2 | no | 1 | 2 | 1/2 | none | none | none | none | none |

Every column agrees with Theorems 1 to 5 and with the header's comment (lines 43 to 73): f = 1 for the products and the involutions, 1/d for ∨, d for both bulk duals, and 1/d for the antiproduct family exactly when (C) holds, 1/d² for transform. The complements are kept with the factor d exactly when P is orthogonal. The bulk and the weight are kept with the factor 1 exactly when P Pᵀ G = G and G′ Pᵀ P = G′ hold. My cases never separate those two conditions, so the iff of each is confirmed only together with the other. project and antiproject, uncut, also have 1/d² (1/4 for d = 2, 1 for conformal(2)). That claim is in the header but in no C++ test (N4). Recomputed hand values (`scripts/hand.py`):

- in conformal(3), e4 ↦ −½e′4 + ½e′5, e5 ↦ e′4 + e′5, e45 ↦ −e′45, 𝟙 ↦ −𝟙′;
- the bulk of e4 is −e5 directly and −½e5 by way of R(4,1,0);
- the weight is e5 against ½e5, and both complements are −e1235 against ½e1235;
- for random form 1, P is an isometry with d = 1/2;
- the same form gives e12 ⟇ e12 = −2e123 against −124e123, the same pair for the antidot, and the weight dual of e3 is (2, −6, 3) against (−2510, −744, 186).

Each equals the tests' value. The remark that (C) forces d = ±1 for a nondegenerate form also holds: taking determinants gives det G = d² det G′ = d⁻² det G′.

## Findings, most severe first

### F1. Blocking (test-only fix): the off-diagonal half of the core's isometry check is pinned by no test

- **Where:** `include/galoot/core/basis_change.hpp:480`; the rejection cases are at `test/CMakeLists.txt:2341` to `2349`.
- **What I ran:** my mutant `isometry_diagonal_only` makes the check compare only the diagonal of M G′ Mᵀ against D G D. It **survived on gcc-debug and clang-debug**: all of `core.basis_change` passes, including the 25 compile-fail tests.
- **Failure scenario:** take conformal_change<3> with the point at infinity negated, so that e5 = −e′4 − e′5 (`probes/pairing.cpp`). It keeps every square but sends e4 · e5 to +1. A regression of this kind in the check would accept that change silently. `change_basis` would then not be an algebra map, and nothing would report it.
- **Why the existing tests miss it:** the not-isometry, swapped, and transposed cases all violate a square. The gap mutant `conformal_pairing_positive` dies at the tip first through the tests' own `static_assert(dual_isometry<…>)`, which reads the mutated factory, not through this check.
- **Fix:** add a compile-fail test `core.basis_change_pairing_not_kept` from `probes/pairing.cpp`, with the not-isometry message.
- **Evidence the fix works:**
  - Unmutated, both compilers reject `pairing.cpp` with one error, the not-isometry message first.
  - On a scratch copy of the headers with the mutant applied, it compiles on both.
  - So the new test kills the mutant.
- **Why blocking:** this is the core's V4 check (DoD-12, criterion 1), and C19 asks for the strongest evidence for the core. The fix changes only tests, so it needs no delta review of core code.

### F2. Should-fix (non-blocking): the inverse's positive denominators and `change_basis_back` over an integer scalar are untested

- **Where:** `include/galoot/core/basis_change.hpp:413` and the guarantee at `:431` to `433`; the promise at `:966` to `968` ("An integer scalar is accepted on a layout that needs no division, … for conformal_change on the Euclidean blades"), which `change_basis_back` inherits through "as for change_basis" (`:1008`).
- **What I ran:** my mutant `inverse_sign_not_moved` leaves the sign of the pivot out of the inverse's rows. It **survived on both presets**.
- **Failure scenario:**
  - For conformal_change<3>, the pivot is −2, so the mutant gives e′1 the row −1 over the denominator −1.
  - The values are unchanged, but q_B becomes −1, which `divides` counts as a division.
  - So `change_basis_back<cga3_to_r410>` of an `std::int64_t` multivector on e1 e2 e3 is rejected with the division message.
  - I confirmed this on a scratch copy of the headers with `probes/intback.cpp`.
  - Unmutated, both compilers accept the same probe.
- **Fix:** add an `std::int64_t` `change_basis_back` case on the Euclidean blades to "change_basis over std::int64_t where no division is needed".

### N1. Note (declared, ruled): the overflow rejection is not one error, and its first line is not the message

- **Where:** `core.basis_change_overflow`, `test/CMakeLists.txt:2354` to `2358`; it has no `ONE_ERROR` and no `FORBID_ON_FIRST_ERROR`.
- **What I observed by hand:**
  - GCC gives 6 errors. The first is "call to non-'constexpr' function 'void galoot::core::detail::assertion_failed(…)'", which names an internal.
  - Clang gives 8. The first is "constexpr variable 'isometry' must be initialized by a constant expression".
  - The message appears only on the quoted source line, and later errors name `filled_change`, `make_change_map`, and `count`.
- **Status:** decision 0220's checklist declares this ("Not met by design for B6", as for every overflow in the core), and the owner accepted 0220's recommendations (decision 0222). So it is a departure from criterion 1's "each rejection gives one error", but a recorded one.
- **A cheaper route exists:** the checks could compute a "fits" flag without asserting, and `static_assert` it with the message. That would give one error with the message first, for the tag checks at least.
- **Before release:** worth a decision, since the catalogue row would change.

### N2. Note (usability): the near-homonyms, and a value passed as the tag, get no message of the library's

These calls are not in the catalogue, and I probed them by hand (`probes/homonym.cpp`):

| Call | GCC | Clang |
| --- | --- | --- |
| `core::basis_change<cga3_to_r410>(p)` | "type/value mismatch … for 'template<long unsigned int N> struct galoot::core::basis_change'" | "template argument for non-type template parameter must be an expression" |
| `core::change_basis<core::conformal_change<3>()>(p)` (a value given where the tag belongs) | one error with three bulleted candidate errors | "no matching function for call to 'change_basis'" |
| `core::change_basis(p)` | "no matching function" | "no matching function" |

- **Why it matters:** the names `basis_change` (the value), `basis_change_like` (the concept), and `change_basis` (the function) are one transposition apart, and the owner confirmed them (decision 0222 item 1). That makes the first two misuses the likely ones.
- **Proposal:** a catch-all for a non-type template argument, the second case, would cost one overload with a message showing the tag. The first case cannot be caught by the library.
- **Before release:** adding these later breaks no user, so they can wait.

### N3. Note (usability): `operator==` compares the representation, not the matrix

- **Where:** `basis_change.hpp:167` to `170`.
- **What:** two values for the same P compare unequal when a row is scaled with its denominator. The `///` comment says so, and the test pins it (`basis_change_test.cpp:171`).
- **Why it matters:** `==` on a value that names a matrix will read as matrix equality to most users. The checklist's "Exact comparisons are `==`" holds only for the representation.
- **Before release:** decide whether to keep `==` or rename it (for example `same_rows`). Changing it after release breaks users.

### N4. Note: some claims are untested in C++

- **project and antiproject (1/d²):** stated at `basis_change.hpp:56`. I confirmed them uncut (above). No C++ test checks them, and note 0026 D says `conventions_check.py` leaves them out too.
- **transform by an odd versor:** `check_transform` (`test/core/basis_change_checks.hpp:333` to `354`) builds only even versors, as the antiproduct of two hyperplanes, while the header states 1/d² for transform in general. My recomputation shows 1/d² for odd versors as well, on the six changes where (C) holds.
- **Proposal:** add a flector, a single hyperplane, to `check_transform`, and one `project` check. Neither blocks.

### N5. Note: a default-initialized `basis_change<N>` (no braces) is indeterminate

- **Where:** `basis_change.hpp:159` and `162` have no default member initializers.
- **Checklist item:** a default-constructed value is defined (zero).
- **Status:** `metric<N>::gram` has the same form, so this follows the core's practice. A value with braces is zero, and a zero value is rejected at first use (denominator not positive).
- **Before release:** defaulting both members to `{}` keeps the type structural and aggregate.

### N6. Note (tooling, outside the unit, W9): `tools/mutate` cannot judge a guard that stops Clang's constant evaluation

- **What happened:** my mutants `image_capacity_full_grade` and `lcm_as_product` were killed on gcc-debug, classified `assert` at `assert.hpp:71`. On clang-debug they were reported **unjudged**.
- **Cause:** Clang's first error line for a `GALOOT_ASSERT` inside a `static constexpr` local is "constexpr variable 'count' (or 'map') must be initialized by a constant expression", which `tools/mutate` classifies as `invalid`, not `assert` (`tools/mutate:46` to `51`).
- **Inference:** the same guards stopped the build on Clang, so I count both as killed, but that is inferred. This defect of the tool is reported here, not fixed.

### N7. Note (cost): the header's operation counts are approximate

- **Where:** `basis_change.hpp:95` to `97` state about 2n³ checked operations for the inner products and 3n³ for the inverse.
- **What the code does:** the elimination makes about 2n(n−1)(2n−1) ≈ 4n³ checked multiplications, and the self-check `inverts` makes n³ more. The inner products need 2n³ multiplications and as many additions.
- **Effect:** small, and no bound applies (C14), but "operations" should say multiplications, and the inverse's count should be about 5n³.

### N8. Note (cost): the agent cost runs past the plan's estimate

- **Rows so far:** `docs/agent/records/agent-cost.tsv` on `develop` gives part D 298k tokens (estimate 100k to 180k), part N 227k (60k to 100k), and part A (a) to (g) 502k (250k to 400k). With the planner's 329k, that is 1.36M before parts B, C, E, this review, and the Sage check.
- **Against the estimate:** section 16's total is 1.4M to 2.3M, so it will likely be exceeded.
- **Missing rows:** parts B and C/E are not yet recorded, and the coordinator owes them.
- **Machine time** was well inside section 12's estimate: my 10 mutants on two presets took 176 s of build and test wall time.

## Process (W2, W7, W9, S11, governance)

- **Cherry-picks.** All 11 commits of part B carry `(cherry picked from commit …)`. Each one's `git patch-id --stable` equals its source's on `feature/core-basis-change--tests`, and the test branch has no other commits past the merge.
- **Labelled failing commits (W2, decision 0216 item 8).**
  - `fa66587`, `7f91c74`, `ca448b1`, `2d23916`, `2406c2e`, and `500a2df` each say "This commit does not build" and name the failing tests.
  - `2e10061`, which adds only the header, `core.hpp`, the root header set, and `internal_messages.txt`, says it makes them pass.
  - `bb5ed99` and `79b35f9`, which pass, sit inside the run. W2's wording ("a run of such labelled test commits may precede the one commit") does not forbid that.
  - The miscounts in three of part B's messages are corrected in decision 0221 rather than by rewriting history, as the coordinator ruled.
- **W9.** The implementer's one test, `404f2fc`, is in its own commit and listed in decision 0221 item 10. Its expected values (1, 2, 3, 6.5, 7.5) I recomputed by hand.
- **S11.** No added line under `include/`, `test/`, `cmake/`, `tools/`, the build files, or the README cites a decision, plan, note, record, or provision (searched the diff); `tests.citations` passes.
- **Governance.** No file under `docs/agent/governance/` changed, CONVENTIONS included.
- **The `rga3.headers` label (`73eb2ac`).** It adds `rga3.headers` to `tests.messages_pinned`, as decision 0219 item 13 asks, and updates the comment. It changes nothing else.
- **Records honesty.**
  - Every verdict in `records/mutations/0030-basis-change/out/*.tsv` and `branch-point/out/*.tsv` is what the README says: all killed at the tip, and the three gap mutants survived at `1818efe` on both presets.
  - `seeds/runs.tsv` has 100 rows, all with exit 0.
  - The cost README's command, revisions, and figures are consistent with `out/summary.txt`.
  - The README says the mutants ran at `752fdea`, which has `404f2fc`'s code; that is the code reviewed.
- **The W3 judging of the gap mutants is sound.**
  - The branch-point suite is exactly the four identity suites of criterion 12, at `1818efe`, without the unit's tests.
  - The tip adds `^core\.basis_change_identities$`.
  - Each mutant survives the first and dies by the second on both presets.
  - The LDLᵀ row's "no counting mutant" prediction was written beforehand, in `gap-predictions.md`. Its argument (a test that derives both its counterpart and its change from the stored form cannot see a self-consistent mutant, by Theorem 1) is correct.
  - Caveat, not a finding: at the tip, the first kill is the tests' own `static_assert` of the contragredient condition or det P, not the core's check. The `--runtime` runs show that the core's check kills each one without those asserts.

## Mutants (my own spec, `2026-11-09-wu12-blind-review/mutants.toml`)

Judged by `^core\.basis_change` (header) or `^core\.basis_change_identities$` (expected factors). The outputs are in `out/`.

| Mutant | Fault | gcc-debug | clang-debug |
| --- | --- | --- | --- |
| `inverse_scaled_by_row_denominator` | P⁻¹ = D M⁻¹ in place of M⁻¹ D | killed | killed |
| `inverse_sign_not_moved` | the inverse's denominators may be negative | **survived** (F2) | **survived** |
| `isometry_diagonal_only` | the isometry check compares squares only | **survived** (F1) | **survived** |
| `image_capacity_full_grade` | capacity C(n, k) in place of C(n, min(k, n/2)) | killed (assert) | unjudged (N6; inferred killed) |
| `division_as_reciprocal_product` | x · (1/q) in place of x / q | killed (`core.basis_change`, the correctly rounded check of regime E) | killed |
| `fold_order_reversed` | rows wedged from the highest generator down | killed | killed |
| `lcm_as_product` | the product of the denominators in place of their lcm | killed (overflow guard) | unjudged (N6; inferred killed) |
| `test_left_bulk_dual_factor_inverse` | expected factor d of the left bulk dual made 1/d | killed | killed |
| `test_antidot_factor_d` | expected factor 1/d of the antidot made d | killed | killed |
| `test_transform_factor_d_squared` | expected factor 1/d² of transform made d² | killed | killed |

`division_as_reciprocal_product` dying confirms what V12 needs from decision 0221's form: regime E's correctly rounded comparison tells one division from a product with a reciprocal. `lcm_as_product` is value-equivalent, since any common multiple reduces to the least q_B. Only the overflow guard tells it apart, on the dense forms. That shows that the lcm is needed for range, not for the values.

## V12: the tolerance rule against the chosen form (item 4 of the brief)

- **The rule assumes the code's form.** The tolerance record's rule assumes the sum by `accumulate` in kernel order, then one division by `static_cast<T>(q_B)` with q_B the reduced lcm. Lines 840 to 863 do exactly that, so the three forms that would void regime E (a reciprocal, a division per term, an unreduced q_B) are not used.
- **Regime E.**
  - Lemma E (every term and partial sum exact for |n| ≤ 2²⁰, b ≤ 20, and C · N_E ≤ 2ᵖ) is a correct argument.
  - The correctly rounded check, through `nearest_double(w e) / w`, is sound, since correct rounding depends only on the real quotient.
  - The authors' `division_per_term` and my `division_as_reciprocal_product` both die there.
- **Regime C.** The ratio bound inherits `ratio_limit`'s five-rounding derivation unchanged. I read that derivation and the record's check of 6u − γ₅ > 0, but did not rerun it.
- **Observed ratios.** The largest observed (0.708, decision 0221 item 2) is below the model's 0.932, so the bound is not loose by an order of magnitude.
- **Float and complex.** Both are claimed exact only for dyadic changes. I found complex division by a real q exact componentwise on both compilers even for odd q.

## Compile-fail messages and the usability checklist (item 5 of the brief)

- **The count.** 17 user messages, each with a compile-fail test, are registered under `core.basis_change` (25 tests). The catalogue scan lists 178 messages, with 22 not pinned, the change's two self-checks among them, which `internal_messages.txt` explains.
- **By hand, 8 tests on both compilers.** For overflow, swapped, target operand, no division, layout beyond n, source count, back narrow scalar, and dependent degenerate, every one gave exactly one error with the literal message on its first error line, and no `detail::` name, except overflow (N1).
- **What the harness checks.** The `ONE_ERROR` check accepts any error line that contains one of the literals, `REQUIRE` texts included. For `source_count` and `target_count` that is fine, since the one error line holds both the message and "4 == 5".

**Task first.** From the examples alone, the three tasks are:

1. Take a conformal(3) point to R(4,1,0) and back.
2. Write a change for a form of my own with `from_rows`.
3. Compute in the diagonal basis and know which results agree with the original basis.

I had to read the implementation, or the long header comment, only for whether `change_basis_back(change_basis(x))` has x's type. It does not (the layout can grow), and the comment says so at lines 79 to 83.

**Names**

- [x] Field terms: `change_basis`, `conformal_change`, `from_rows`, `rows`, `denominators`.
- [~] One meaning each, but `basis_change`, `basis_change_like`, and `change_basis` are near-homonyms (N2). Owner-confirmed.
- [x] Similar things look alike: the `_back` and `_layout` pairs, and the argument order of `transform`.

**Construction and conversion**

- [x] A wrong number of rows or entries does not match `from_rows`, and a fractional entry is a narrowing error.
- [~] A default-initialized value is indeterminate without braces (N5).
- [x] No implicit lossy conversion; the result goes to a named type through `Type{q}` or `truncate`.
- [x] No hidden default: the scalar is the input's, nothing is checked silently, and integer rejection is explicit.

**Misuse and diagnostics**

- [~] The catalogue's misuses all have tests on both compilers; N2's do not.
- [~] One error with the message first, except overflow (N1).
- [x] Messages start with the operation and give the fix after a semicolon. The layout messages ("basis change map: …") give no fix, as the core's "product plan:" and "unary map:" messages do not.
- [x] Distinct causes give distinct messages; the positive-denominator message covers zero and negative alike.
- [x] `basis_change_like` probes data only. The catch-all overloads make a requires-expression on a call answer yes, as `basis_change.hpp:1039` to `1041` documents.

**Semantics and cost**

- [x] Representation change is named; nothing is dropped.
- [x] `auto r = f(x)` is safe: results are returned by value, and the misuse catch-alls are tested through `auto r`.
- [~] `==` is exact but compares the representation (N3).
- [x] Cost and result blades are stated, with measured limits (N7 on the counts).

**Examples and readability**

- [x] The header's example compiles and is tested (`404f2fc`).
- [x] Each behavior lives in one named place: the checks, the elimination, the image, and the typing.

**Before release, interface changes that would break users later:** N3 (the meaning of `==`) and possibly N1, if the overflow message's route changes the catalogue row. N2's catch-alls and N5's initializers break no user.

## Acceptance criteria 1 to 16

| # | Criterion | Evidence | Status |
| --- | --- | --- | --- |
| 1 | Declared and checked at generation | 17 messages with compile-fail tests on both compilers; `change.toml` all killed; 8 checked by hand | **Partly met**: overflow is not one error (N1, declared and ruled); the off-diagonal isometry check is unpinned (F1) |
| 2 | Blade images | golden `basis_changes`, d = 1 to 4, both ways (`basis_change_test.cpp:280`); hand values, which I recomputed | Met |
| 3 | Isomorphism, structure layer | `basis_change_identities_test.cpp:277` and `:292`, over `for_each_signature` and the named changes; I replicated the generation's exactness | Met. It exercises `detail::blade_image` and `inverse_change`, not the typed map, which criterion 4 covers |
| 4 | Isomorphism, kernel layer | `check_products` and `check_round_trips` over the groups of the criterion; seeds 1 to 50 | Met |
| 5 | Agreement with factors | `check_agreement` and `check_transform`; `naturality.toml` and my three factor mutants killed; every factor recomputed by me | Met, with N4 (odd versors, project, antiproject untested) |
| 6 | What is not preserved | `basis_change_test.cpp:404` and `:435`; inequality mutants killed; hand values recomputed | Met |
| 7 | Test-side LDLᵀ | `support.diagonalize` passes; `ldlt.toml` killed with and without `--runtime`; random form 1's change recomputed | Met |
| 8 | Scalars | int64, rational, double, float, complex; integer rejection tests; `rounding.toml` and my reciprocal mutant killed | **Partly met**: `change_basis_back` over an integer scalar is untested (F2) |
| 9 | V8 validation | `basis_change_validation_test.cpp:192` and `:212`; the matrix representations case in `blade_product_test.cpp:598`; the readings in note 0026 B | Met as tests; the sources not re-read by me |
| 10 | Sage cross-check | `records/wu12-sage-cross-check` | **Pending that record** |
| 11 | Conventions 1.17 | trace row 1.4 implemented; `conventions_check.py` gains the checks; CONVENTIONS not edited | **Not yet met**: 1.17 is to be committed at the merge as ratified (decision 0219 item 4) |
| 12 | Mutants | every authors' spec killed on both presets; gap mutants count under W3; my spec | **Partly met**: two survivors of my spec (F1, F2), each with a killing test shown |
| 13 | Derivation first | note 0026 part A; my independent recomputation of all factors and conditions | Met |
| 14 | Seeds | `seeds/runs.tsv`, 100 runs, exit 0 | Met (not rerun) |
| 15 | Cost | `records/performance/wu12-basis-change-cost/`; the Clang limits probed by me | Met, with N7 |
| 16 | Checks | P8 selection passes on both presets (mine); trace gate and runlog check pass (mine); `format-check` and lint at `1a9242d` (same code); S11 clean; W2 labels present | **Partly met**: the full P9 matrix is still to run after the fixes |

## Outcome

**Block, pending F1.** F1 is a test-only change: one compile-fail test, `probes/pairing.cpp` with the not-isometry message. With it, and preferably F2's one-line case in the same commit, the verdict becomes **approve with non-blocking findings**. Neither fix touches `include/`, so no delta review of core code is needed. A short check that the new tests kill `isometry_diagonal_only` and `inverse_sign_not_moved` closes them. The notes N1 to N8 do not block.

## Open items for the owner

1. **CONVENTIONS 1.17** (note 0026 section E, with section 8's "Conformal duals" row to carry decision 0219 item 5's words).
   - Decision 0219 item 4 ratified it on the condition that part A's checks earn its labels, and they have (Checked on eleven changes, proved in note 0026).
   - The alternative is to keep 1.4 as written and state the preserved operations only in the header.
   - The tradeoff: 1.17 puts a convention a user can trip on (the bulk depends on the basis) in the one specification C10 asks for, against a longer 1.4.
   - Recommendation: commit the section E text at the merge, after the Sage record, so that the "(Checked …)" labels stand on all evidence.
2. **N1, the overflow diagnostic.**
   - Option (a): keep the cascade, consistent with every core overflow, at no cost.
   - Option (b): compute a flag in the tag checks and `static_assert` it, for one error with the message first, at about 20 lines in the header and a changed catalogue row.
   - Recommendation: (b) for the tag checks before release, recorded as a decision; the term generation's overflow stays a `GALOOT_ASSERT`.
3. **N3, the meaning of `==` on `basis_change`.**
   - Option (a): keep representation equality, as documented.
   - Option (b): rename it (`same_rows`) and drop `==`.
   - Option (c): make `==` compare P, at the cost of a cross-multiplication per entry.
   - Recommendation: (a) or (b) before release. Matrix equality has no present user (C12), but a user will read `==` as matrix equality, and changing it after release breaks code.
4. **N2, catch-alls for a value given in place of the tag.** One overload pair with a message. It can wait past this unit and breaks no user when added. Recommendation: add it with the next diagnostics pass.
5. **N6, `tools/mutate` on Clang guards.** A defect of a tool outside this unit. Recommendation: classify Clang's "must be initialized by a constant expression" whose notes name `assertion_failed` as `assert`, in a tools unit.
6. **N8, agent cost.** The unit will exceed plan 0030's 1.4M to 2.3M token estimate. No bound applies; the owner sees the rows. This review's own cost is recorded by the coordinator from the harness report.
