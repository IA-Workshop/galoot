# 0013 WU7: typed results, the operation parameter, unary maps, and conjugated plans

- Status: claimed 2026-11-01 (planner: Claude, agent, Opus 5.5, math-expert role). WU7a merged into `develop` on 2026-11-01 after its W5 review (Outcome and review fixes, WU7a, below). WU7b claimed 2026-11-01 on `feature/core-plan-maps` (implementer: Claude, agent, Opus 5.5).
- Unit: WU7 of plan 0001, in two parts run one after the other: WU7a on branch `feature/core-plan-results` (worktree `.claude/worktrees/core-plan-results`), then WU7b on branch `feature/core-plan-maps` (worktree `.claude/worktrees/core-plan-maps`), branched from `develop` after WU7a merges. Each part has its own W5 review and merge. WU7b is claimed by a dated line in this plan when WU7a has merged (as plan 0011 did for WU6b).
- Decision numbers 0110 to 0119. Plan 0012 claimed 0104 to 0109; 0108 and 0109 were not used, and are left alone so that no number has two claimants.
- Goal: grow `include/galoot/core/plan.hpp` from "the geometric-product terms of two blade sets" into the data that every product and every per-blade operation of the core needs: a typed result (the blade set a result is stored on, and each term's position in it), one generator with an operation parameter (geometric product and wedge), unary maps (reverse, antireverse, grade involution, right and left complement) as per-blade sign and permutation data, the antiproducts as complement-conjugated plans, and a CTest that fails when a plan is generated twice in one translation unit.

## 1. Departures, ambiguities, and owner questions (C20)

Each item states the reading this plan follows. An owner answer that differs changes the named section and nothing else.

1. Two parts, not one. Plan 0001 lists WU7 as one unit of size M. WU6 was planned as one unit of size M and ran as two (plan 0011, departure 1), each of about 500k tokens with its review fixes. WU7 holds more: result typing, an operation parameter with a second operation, a new CTest form, five unary maps, two conjugated products, and their identity proofs. Split along the dependency: WU7a (typed binary plans, gp and wedge, the evaluation check) does not need complements; WU7b (unary maps, antiproducts, De Morgan proofs) needs WU7a's typed result. Owner question 1: keep the split (recommended), or run both on one branch with the same criteria.
2. Boundary with WU9 (wedge). Plan 0001 places "gp, wedge, `*`, `^`" in WU9. STYLE S16 admits a template parameter only where two present uses differ in it, so the operation parameter needs a second operation now. Reading: WU7a adds the wedge as a plan (in `detail`, used by tests and by WU8 and WU9 later); WU9 keeps the public `gp` and `wedge` functions on multivectors and the operators `*` and `^`. Because WU7a implements the wedge of CONVENTIONS 4.1 in `include/galoot/core/`, the trace row "the wedge of two blades that share a generator is zero" moves to `yes` in WU7a (the trace rules require it in the change that implements the item).
3. Boundary with WU10 (complements, antiproducts, De Morgan laws). Plan 0001 gives WU7 "unary maps, complement conjugation" and WU10 "involutions, complements, antiproducts; De Morgan laws". The two texts overlap. Reading: WU7b builds the plan-level data (maps and conjugated plans, in `detail`) and proves the De Morgan laws and the involution relations on that data exhaustively over finite ranges, because those laws are the cheapest complete check of conjugation (VVUQ V1 kind 1). WU10 provides the public operations on multivectors (`reverse`, `right_complement`, `antigp`, `antiwedge`, and the rest), the operators `~`, `!`, `%`, `&`, and the randomized multivector identities of DoD-10. The trace row of 4.2 moves to `yes` in WU7b. Owner question 2: confirm, or move the plan-level De Morgan proofs to WU10 (then 4.2 stays `no` until WU10, and WU7b's evidence for conjugation is the golden comparison of section 6 alone).
4. "Memoization" in plan 0001's WU7 line. The memoizing form (an inline variable template initialized by a `consteval` function whose locals are `static constexpr`) was fixed in WU6b (decisions 0102 item 7 and 0103). Reading: WU7's memoization work is (a) every new variable template follows that form, and (b) the CTest deferred by plan 0011 section 12. No new memoization mechanism.
5. `gp_plan` is replaced, not kept beside the typed plan. Decision 0091 (consequences) and plan 0011 section 12 say the term's output becomes "an index into" the result set. Reading: the variable template `gp_plan<Alg, A, B>` becomes `product_plan<binary_operation::gp, Alg, A, B>`, whose value holds the result set and terms with an output position; `generate_gp` becomes `generate_plan<binary_operation::gp>`. Keeping `gp_plan` as a second, mask-level variable template would be a layer with no user once the typed plan exists (C12). Consequence: the tests B1 to B8 of `plan_test.cpp` and the seven `core.plan_*` compile-fail tests are migrated, each expected value unchanged (a mask is read as `result.blades[out].mask`), and the messages change their prefix from "gp plan:" to one that names no single operation, because a `static_assert` message must be a literal in C++23. The migration is its own commit, listed for the review (W9). Owner question 3: confirm, or keep `gp_plan` as an alias.
6. The wedge as its definition, not as a filtered geometric product (constitution C4). CONVENTIONS 4.1 defines e_A ∧ e_B = (reorder sign) e_(A ∪ B) for disjoint A and B and 0 otherwise, and states that this equals the part of e_A ⟑ e_B of grade |A| + |B|. Two forms are possible: (a) the pair rule of the definition, one `reorder_sign` per pair; (b) generate e_A ⟑ e_B and keep the terms of grade |A| + |B|. Reading of C4: the one mechanism is the plan generator, which C4 describes as taking "a signature, an operation, and the basis elements of its inputs"; the operation enters as its pair rule, and the wedge's pair rule is per-basis sign data (`reorder_sign`, already in `bits.hpp`), so (a) adds no machinery. (a) is chosen: it is the definition (C3, the direct form), it does not depend on the metric, as 4.1 says the wedge does not, and it costs one step per pair where (b) costs a full geometric product. (b) becomes a test (identity C3 of section 6). Recorded in decision 0110. Owner question 4: confirm the C4 reading, or require (b).
7. The form of the evaluation check. Plan 0011 section 12 offers (a) durations in a Clang `-ftime-trace` and (b) retired instructions against a copy of the header. A third form was considered here: (c) a lowered Clang step limit (`-fconstexpr-steps`) that one generation fits under and two do not. Chosen: (a), comparing medians (section 6, C11), registered only where the compiler is Clang. Reasons: (a) needs one compile and no copy of the header; its margin is large (section 9: the initializer takes under 0.1% of the generator's instantiation as committed and 26% to 85% with plain locals); medians over three or more plans absorb one preempted interval. (b) needs `perf` and two compiles. (c) is deterministic but its margin is about a factor of two and its threshold must be recalibrated whenever Clang counts steps differently. GCC is not tested because its constexpr call cache hides a repeated generation (note 0011, finding 3), so a GCC form would have nothing to detect; the test's registration says so (C8). Recorded in decision 0112. Owner question 5: accept (a) on Clang only.
8. Antiproducts in a non-diagonal metric. CONVENTIONS 4.2 defines a ⟇ b = left(right(a) ⟑ right(b)) for every metric, and WU7b implements exactly that, for every form of the signature set. For rigid(3), `test/reference/conventions_check.py` checks the definition against Lengyel's tables (CONVENTIONS 4.2, Checked). For conformal and other non-diagonal forms, no published worked value of an antiproduct has been read; the evidence there is the definition, the De Morgan proofs, and the golden comparison (verification), not validation (V8). The V10 line says so.
9. Unary maps: the five operations that CONVENTIONS fixes (2.1 and 3.1). The Clifford conjugate and other involutions have no convention and no user, so none is added (C10, C12). The bulk, weight, and duals (3.2, 3.3) need the metric exomorphism, which in a non-diagonal form maps a blade to several blades; they are WU11's. The map term chosen here has room for several terms per input, so WU11 needs no new term type.
10. Repeated masks within one input set. Decisions 0092, 0093, and 0102 item 10 leave the whole-set check to the type that stores a set (WU8). WU7 keeps that: a typed plan or map never creates a repeated result blade (the result set is built from distinct masks), and an input set that lists one mask twice gives each listing its own terms, which may share one result position. A test pins it.
11. Open owner items that bear on WU7 (handoff of 2026-11-01): item 2, the `static constexpr` locals that cost debug object size on GCC -O0 (decision 0102 note). WU7's new generators follow the same form, so they add to that cost; if the owner chooses plain locals, the evaluation check of WU7a is exactly the test that will then fail on Clang, and a different remedy is needed. Item 1, the per-algebra `metric_like` check (decision 0107 item 4): WU7's P2 records measure as decision 0107 did, without the concept `static_assert`, so they include that check's growth and say so.
12. Planning evidence. The values of sections 6 and 9 were computed while planning with the repository's reference scripts and one Clang probe, from scratch scripts that are not committed; they are therefore labeled "planning values, not reproducible as committed" (V10). The unit recomputes every one of them from committed references (golden values, hand derivations stated in the test, or the test's own independent rule) before any test relies on it. The four probe runs were timed with `tools/timed`; their run-log rows are quoted in section 9 and were not committed, because this commit holds the plan alone.

## 2. Background for the reader

This section explains the objects the unit works with, with one worked example. Nothing in it is new mathematics; it fixes the words used below.

### Plans

A multivector of a named type stores one coefficient per blade of its blade set. For example, a rigid(3) point stores x, y, z, w on the set P = (e1, e2, e3, e4). A product of a point x and a point y is a sum over every pair of blades: x_a y_b e_A e_B. Each e_A e_B is a short list of terms, coefficient times blade, which `blade_product` computes exactly from the metric. A plan is that whole double sum written down once, as data: a list of terms (a, b, out, c), meaning "add c x_a y_b onto blade out". At run time a product is then a loop over the plan's terms with no decisions left to make.

For point times point in rigid(3) (e1, e2, e3 square to 1, e4 to 0), the plan has 15 terms. Four of them are (0, 0, scalar, 1), (1, 1, scalar, 1), (2, 2, scalar, 1), and (0, 1, e12, 1); e4 e4 is missing because e4 squares to 0. So the scalar part of the product is x_1 y_1 + x_2 y_2 + x_3 y_3, and its e12 part is x_1 y_2 − x_2 y_1.

### Typed results

Today a plan term names its output blade by its mask. To store the product, the code also needs to know which blades the result has and where each lives in the result's array. Result typing computes that once per plan:

- The result set R is the set of distinct output masks, listed in canonical order (CONVENTIONS 1.7: by grade, then lexicographically by index list) with sign +1 (decision 0091).
- Each term's output is rewritten as its position in R.

For point times point, R = (1, e12, e13, e14, e23, e24, e34), seven blades, and the term (0, 1, e12, 1) becomes (0, 1, 1, 1): position 1 of R is e12.

Why R holds exactly the blades that can be nonzero (DoD-4, constitution C6). Write the product as z_C = Σ c_(a,b,C) x_a y_b over the terms with output C. When x and y are independent inputs, the products x_a y_b for distinct pairs (a, b) are distinct monomials, and distinct monomials are linearly independent polynomials. So z_C is the zero polynomial only if every c_(a,b,C) is zero, and a plan holds no zero coefficient. Hence a blade is in R exactly when some input makes it nonzero. The argument needs x and y independent: for x times x, the monomials x_a x_b and x_b x_a coincide and can cancel (a vector times itself has no bivector part). That case is the collected plans of decision D8, left to M7.

### The operation parameter, and the wedge

The plan generator loops over pairs of blades and asks a pair rule for the terms of one pair. For the geometric product the rule is `blade_product`. For the wedge (exterior product), CONVENTIONS 4.1 gives the rule directly: e_A ∧ e_B is zero when A and B share a generator, and otherwise the reorder sign of e_A e_B times e_(A ∪ B). The wedge does not use the metric. The generator takes the operation as a template parameter, so one loop serves both rules. In conformal(3), where e4 · e5 = −1, the geometric product e4 e5 = −1 + e45 has two terms, while e4 ∧ e5 = e45 has one: the plans differ exactly where the metric is not diagonal.

### Unary maps

The reverse, antireverse, and grade involution multiply each blade by a sign and keep its mask. The right and left complements send each blade to the blade on the other generators, with a sign. Each is therefore per-blade data: for each input position a, one output blade and one sign. A unary map stores that data typed like a plan: a result set (canonical, sign +1) and terms (a, out, c). The signs come from the functions of `bits.hpp`, which are already proved against their definitions (trace rows 2.1 and 3.1).

Example, rigid(3): right(e4) is −e123, because e4 ∧ (−e123) = −e4123 = +e1234 = 𝟙 (moving e4 past three generators is three transpositions).

### Conjugated plans: the antiproducts

CONVENTIONS 4.2 defines the geometric antiproduct a ⟇ b = left(right(a) ⟑ right(b)) and the antiwedge a ∨ b = left(right(a) ∧ right(b)). So an antiproduct plan is a product plan seen through the complements: complement the two input sets, generate the ordinary plan over them, and complement each output blade.

Worked example, rigid(3): e4 ⟇ e4 = left(right(e4) right(e4)) = left((−e123)(−e123)) = left(e123 e123). In rigid(3), e123 e123 = −1 (reversing three generators gives the sign −1, and each squares to 1). The left complement of the scalar 1 is 𝟙, so e4 ⟇ e4 = −𝟙.

One fact keeps conjugation cheap: the complement reverses canonical order. Proof: if two blades have different grades, their complements have the swapped grades. If they have the same grade, the generators in which they differ are the same for their complements, and the lowest such generator belongs to the first blade exactly when it does not belong to the first blade's complement; canonical order decides by that lowest generator. So the terms of one pair, which the base plan lists in increasing canonical order of their blades, are in decreasing order after complementing, and listing each pair's run backwards restores canonical order. (Derivation; checked exhaustively for n = 1 to 8 while planning, section 9.)

## 3. Content

All of it in `galoot::core::detail` (decision 0015), in `include/galoot/core/plan.hpp`. Names below are proposals; the implementer may change them and records the names in the decision with the reason (an `hci-expert` reading is welcome, not required).

### WU7a

- `binary_operation`, an enumeration with `gp` and `wedge` (WU7b adds `antigp` and `antiwedge`). An enumeration is the most direct form of a value that selects a rule (S16); it is a structural template argument.
- The pair rule for each operation, selected with `if constexpr`: `gp` calls `blade_product`; `wedge` returns no term when `a & b` is nonzero and otherwise the one term `reorder_sign(a, b)` on `a | b`, with the same range assertion as `blade_product` ("blade has a generator beyond e_n"). The wedge needs a capacity of 1 per pair; the capacity function takes the operation, so the wedge's scratch is not sized for the geometric product.
- `generate_plan<Op, Capacity, N>(g, as, bs, out)`: `generate_gp` with the operation parameter, the same contract (order a, then b, then canonical order of the output; count returned; signs folded). `plan_term` with its mask output stays as the type the generator writes, because the run-time tests compare masks.
- The result typing, as `constexpr` functions over spans so the tests can run them at run time: the number of distinct output masks, the result set (canonical order, sign +1), and the typed terms (a, b, out position, coefficient). Direct form first: insertion into a list kept in canonical order and a search for each term's position; optimized only if the measured limits (criterion 8) call for it.
- A typed plan type: a structural aggregate of `blade_set<R> result` and `std::array<typed_term, T> terms`.
- `product_plan<Op, Alg, A, B>`: `template <binary_operation Op, metric_like Alg, blade_set A, blade_set B> inline constexpr auto product_plan = make_product_plan<Op, Alg, A, B>();`, with `make_product_plan` a `consteval` function template whose locals are all `static constexpr` (decision 0102 item 7). It keeps the set checks of `make_gp_plan` before generation and the five mask-level checks after it, and adds the cheap checks of the typed result (V4): the result set strictly increasing in canonical order, every sign +1, every output position below R, every result blade used by at least one term, and terms in order of a, then b, then strictly increasing output position. Each check is a `static_assert` with its message, and each has a direct test on hand-written typed plans (as `defects_of` has), so a mutant of each bound is killed.
- `gp_plan`, `make_gp_plan`, and `generate_gp` are removed (departure 5).
- A CTest, registered for Clang only, that compiles one dedicated unit with `-ftime-trace -ftime-trace-granularity=0` using the command of `galoot_compile_command`, and checks the trace with a short Python script under `test/` (section 6, C11).

### WU7b

- `binary_operation` gains `antigp` and `antiwedge`. `make_product_plan` for these complements both input sets (an oriented blade s e_A becomes the oriented blade s · right_sign(A) e_(complement of A), so the existing sign folding carries the complement's sign), generates the plan of the base operation (`gp` or `wedge`) over them, maps each output blade D to its complement with the factor left_sign(D), and reverses each pair's run of terms (section 2). The result is then typed as any plan is. The positions a and b refer to the original sets, because complementing a set keeps its positions.
- `unary_operation`, an enumeration with `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement`.
- A map term (a, out position, coefficient) and a typed map type (`blade_set<R> result`, the terms), and `unary_map<Op, Alg, S>`, a variable template with a `consteval` initializer in the same form. One term per input blade, the coefficient s_a times the operation's sign from `bits.hpp`, the output mask the input mask (sign operations) or its complement (complements). The algebra tag supplies n, which the antireverse and the complements need, and ties the map to an algebra as the plans are tied.
- Generation-time checks of a map (V4, DoD-12): the result set's order and signs, positions in range, every result blade used, one term per input in order of a, and, for the complements, every sign equal to the closed form (−1)^(k(k+1)/2 + i_1 + ... + i_k) for the right complement of a blade of grade k with 1-based indices i_j, and (−1)^(k(n−k)) times that for the left. The closed form is a second route, not the `reorder_sign` the map uses (knowledge-base note on complements, proved there with an SMT solver for every mask of up to 16 bits; rechecked for n = 1 to 10 while planning), so this check can fire when `reorder_sign` or the complement code is wrong. Each check has a direct test on hand-written maps.
- `test/reference/golden.py`: the complement and grade-sign tables, written today for n = 4 and 5, are also written for n = 3, so that every golden product table (two of which have n = 3) can be conjugated in the test from golden values alone (section 6, D5). `golden.hpp` regenerated in the same change.
- The evaluation check covers `unary_map` and the conjugated plans.

### Both parts

- The `///` comment of each public declaration states what it requires, guarantees, and preserves, cites no record (S11), and states the measured default limits as `gp_plan`'s comment does now.
- Decisions: 0110 (the operation parameter, the wedge's pair rule, the C4 reading), 0111 (the typed plan: layout, result order, positions, generation-time checks, removal of `gp_plan`), 0112 (the evaluation check: form, threshold, compiler), 0113 (WU7a limits and the P2 record), 0114 (unary maps), 0115 (conjugated plans: input and output complements, run reversal, the closed-form check), 0116 (WU7b limits and P2). Unused numbers up to 0119 stay with this plan.
- Mutation scripts and results under `docs/agent/records/mutations/0013-plan-results/` and `0013-plan-maps/`, in the form of plan 0011's drivers, including the lesson-0063 rule: never run CTest after a failed build in a tree whose programs were deleted.

## 4. Definition of done

For each part: the header compiles alone and in the umbrella header; every test of section 6 for that part passes on all seven presets, each test case with its reference marker; the trace gate, markers, golden facts, citations, include scan, format, and lint pass; every rejection has a compile-fail test on both compilers that requires its literal message; every test case and every `static_assert` the part adds has a recorded mutant that it kills; the default limits are measured and stated in the `///` comments; the part's decisions, the W10 before-and-after record, the P2 record, and the agent-cost row are written; and this plan records the part's outcome with its V10 line. The unit is done when both parts are merged into `develop` and this plan is closed.

## 5. Acceptance criteria

WU7a:

1. For every signature of plan 0011 section 5 (42 forms, n ≤ 6), at run time over `full<n>` × `full<n>`: the typed plan of `gp` and of `wedge`, read back through its result set, equals the mask-level plan term for term; the result set is exactly the set of output masks, in canonical order with sign +1; every position is in range (exhaustive, so a proof over that range).
2. The wedge plan satisfies, for every pair of every form of the set: no term when the blades share a generator, otherwise one term, the test's own inversion count on A ∪ B; it equals the terms of grade |A| + |B| of the geometric product of the pair; it is the same for every form of one n (metric independence); and in R(0,0,n) it equals the geometric product (Grassmann case).
3. The wedge is associative on every triple of blades of `full<n>`, n ≤ 6, multiplied out in the test over the wedge plan.
4. The hand-written values of section 6 (C2, C5, C6) hold in `static_assert`s; the typed plans of `full<4>` × `full<4>` in rigid(3), R(4,0,0), R(1,3,0), conformal(2), and the degenerate form, for both operations, equal the run-time typing of the same sets.
5. The wedge plan agrees with the golden geometric-product tables: for each of the four golden forms and every pair, the wedge term equals the golden product's terms of grade |A| + |B| (V1 kind 3, LDLᵀ route).
6. Every generation-time check of the typed plan fires on a generator mutant, and `defects_of` or its typed counterpart is tested directly on hand-written plans at each bound.
7. Migration: every test of B1 to B8 and every `core.plan_*` rejection of WU6b passes after migration with the same expected values, in a commit of its own that the review request lists; the messages are the only text that changes, and each compile-fail test still forbids the messages that generation would add.
8. Limits, measured on both compilers in the three configurations of decision 0103 (Galoot's flags at -O0 and -O2, bare `-std=c++23 -pedantic-errors -O2`): the typed rigid(3) pairs, the conformal(3) vector pair, and the sparse pairs at n = 8, 12, 16 build at default limits for both operations; the largest `full<n>` × `full<n>` for each operation is measured and stated in the `///` comment of `product_plan`. If typing lowers a ceiling below decision 0103's, the drop is reported with its cause (C14; no bound).
9. The evaluation check (C11) passes on clang-debug, clang-release, clang-asan, and lint at the final revision, and fails on each of two recorded mutants of the header: every local of the generator plain `constexpr`, and plain locals with an initializer that generates twice. A third mutant, an initializer that calls the generator twice with `static constexpr` locals, passes it, because no generation is repeated; the record says so.
10. P2: the fixed use of decision 0103 (four typed rigid(3) pairs embedded in R(n−1,0,1)) through `product_plan<gp>`, at n = 4, 8, 12, 16, pinned, three repeats, both compilers, without the concept `static_assert` (decision 0107), before (WU6b's `gp_plan` at the claim revision) and after, recorded under `docs/agent/records/performance/plan-results/` with its script. Growth is reported with its cause and blocks nothing (decision 0005).
11. The seven presets from fresh trees at the final revision; `all_verify_interface_header_sets` on gcc-debug and clang-debug; W10 before and after from clean revisions (lesson 0062).

WU7b:

12. For every n from 1 to 10, every unary map over `full<n>` has, per input, the output blade and sign of the definition computed in the test by its own route: the reverse by counting the transpositions that reverse the index list, the grade involution by (−1)^k, the antireverse by the reverse with the antigrade, the right complement as the signed blade e_C with e_A ∧ e_C = 𝟙 by the test's own wedge, the left complement likewise; and for n = 4 and 5 the golden `grade_signs` and `complements` (V1 kind 3). Both even and odd n are included, because in odd n the left and right complements coincide and a swap of the two passes there.
13. The relations of 2.1 and 3.1 hold as compositions of maps, for n = 1 to 10: reverse∘reverse, grade_involution∘grade_involution, left∘right, and right∘left are the identity; right∘right and left∘left are the sign (−1)^(gr·ag); the antireverse is left∘reverse∘right.
14. The four De Morgan laws hold at plan level for every pair of blades of `full<n>` in every form of the set (n ≤ 6): right(e_A ⟑ e_B) = right(e_A) ⟇ right(e_B), right(e_A ∧ e_B) = right(e_A) ∨ right(e_B), and the same with left, each side computed from the library's plans and maps and compared as term lists (exhaustive).
15. The antiproduct plans equal, pair by pair, the left complement of the golden geometric-product table of the right-complemented pair, composed in the test from golden values only (`gp_tables` and `complements`, n = 3, 4, 5), for all four golden forms; and the antiwedge likewise from the test's own wedge (V1 kind 3).
16. ⟇ and ∨ are associative on every triple of blades for the forms of the set with n ≤ 5 (n = 6 if it runs within the edit cycle; otherwise labeled `slow`, P9); 𝟙 is a two-sided identity of ⟇, and 1 of ⟑; u ∨ right(u) = 1 and left(u) ∨ u = 1 for every blade; and the product rules reverse(e_A ⟑ e_B) = reverse(e_B) ⟑ reverse(e_A) and antireverse(e_A ⟇ e_B) = antireverse(e_B) ⟇ antireverse(e_A) hold for every pair (n ≤ 6).
17. The hand-written values of section 6 (D2, D3, D7) hold in `static_assert`s, and the runs of every conjugated plan are in canonical order (the generation-time order check, with the reversal mutant killed by it).
18. Every generation-time check of maps and conjugated plans fires on a mutant, including the closed-form complement check on a mutant of `reorder_sign` and on a mutant that swaps the left and right complement signs.
19. Limits, evaluation check, P2, presets, W10: as criteria 8 to 11, for `unary_map` and the conjugated plans.

## 6. Test list, with expected values and reference sources

Markers follow the forms in use: `proof` (exhaustive over a finite range, V1 kind 1), `identity` (kind 2), `independent value` (kind 3), `contract` (a property the header promises). Every exact comparison is `==` (V11). The values below marked "planning value" were computed while planning (departure 12); the source named beside each is the committed route the test uses.

Notation for terms: (a, b, out, c) for plans, with out a position in the result set; (a, out, c) for maps. Rigid(3) sets: point P = `blades<4, "e1 e2 e3 e4">`, line L = `"e41 e42 e43 e23 e31 e12"`, plane G = `"e423 e431 e412 e321"`, motor M = `"1 e41 e42 e43 e23 e31 e12 e1234"`.

### WU7a, in `test/core/plan_test.cpp` (CTest `core.plan`)

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| C1 | Typed plan equals the mask plan | proof | Criterion 1, both operations, 42 forms |
| C2 | Typed rigid(3) pairs | independent value: the diagonal rule written in the test, and the hand counts below (planning values from `clifford_reference.py`) | P ⟑ P: 15 terms, R = (1, e12, e13, e14, e23, e24, e34), terms (0,0,0,1) (0,1,1,1) (0,2,2,1) (0,3,3,1) (1,0,1,−1) (1,1,0,1) (1,2,4,1) (1,3,5,1) (2,0,2,−1) (2,1,4,−1) (2,2,0,1) (2,3,6,1) (3,0,3,−1) (3,1,5,−1) (3,2,6,−1). P ∧ P: 12 terms, R = (e12, e13, e14, e23, e24, e34), terms (0,1,0,1) (0,2,1,1) (0,3,2,1) (1,0,0,−1) (1,2,3,1) (1,3,4,1) (2,0,1,−1) (2,1,3,−1) (2,3,5,1) (3,0,2,−1) (3,1,4,−1) (3,2,5,−1). L ⟑ G: 15 terms, R = (e1, e2, e3, e4, e124, e134, e234). L ∧ G: no terms, R empty. P ∧ L: 12 terms, R = (e123, e124, e134, e234). M ⟑ P: 28 terms, M ∧ P: 16 terms, both R = (e1, e2, e3, e4, e123, e124, e134, e234). L ⟑ L: 27 terms, R = (1, the six bivectors, e1234). L ∧ L: 6 terms, R = (e1234) |
| C3 | Wedge rule | proof and identity | Criterion 2; the marker states "the wedge of two blades that share a generator is zero", the 4.1 trace fact |
| C4 | Wedge associativity | identity | Criterion 3 |
| C5 | conformal(3) vectors | independent value, worked by hand from e4 · e5 = −1 | V = (e1, ..., e5): V ⟑ V has 25 terms and R = (1, e12, e13, e14, e15, e23, e24, e25, e34, e35, e45), with e4 e5 = −1 + e45 and e5 e4 = −1 − e45; V ∧ V has 20 terms, the same ten bivectors, and no scalar: e4 ∧ e5 = e45 |
| C6 | Sparse use at n = 8, 12, 16 | independent value, the diagonal rule | In R(n−1,0,1), A = {1, e1, e_n, e1 ∧ e_n} and B = {e1, e2, e_n} by mask. Geometric product: 10 terms, R = (1, e1, e2, e_n, e12, e1n, e2n, e12n); at n = 16, writing e_(i,j) for the blade of generators i and j, the terms with their output blades are (0,0,e1,1) (0,1,e2,1) (0,2,e_16,1) (1,0,1,1) (1,1,e12,1) (1,2,e_(1,16),1) (2,0,e_(1,16),−1) (2,1,e_(2,16),−1) (3,0,e_16,−1) (3,1,e_(1,2,16),−1). Wedge: 8 terms (no (1,0) and no (3,0)), R without the scalar, 7 blades |
| C7 | Result order | contract, CONVENTIONS 1.7 ("results are ordered by grade, then by index list") | The R of P ∧ P lists e14 before e23 although the mask of e23 (0b0110) is below that of e14 (0b1001); a mutant ordering R by mask value is killed here |
| C8 | Typed checks at each bound | contract (V4) | The typed defects function on hand-written plans: a result out of order, a sign −1, a position R, a result blade no term uses, terms out of order; each flagged alone |
| C9 | Repeated mask, empty sets | contract | A set listing e1 twice gives two terms on one result position; an empty input gives an empty result and no terms |
| C10 | Compile time equals run time | contract | Criterion 4, `static_assert` and run time on a metric read through `volatile` |
| B1 to B8 | WU6b's tests, migrated | as recorded in plan 0011 | Unchanged expected values (departure 5) |

Rejections (`core.plan_*`): the seven of WU6b, migrated, plus `wedge_beyond_n` (a set naming a generator beyond e_n under `wedge`, which must reach the same set check).

### WU7a, the evaluation check (CTest `core.plan_evaluated_once`, Clang presets only)

| # | Check | Reference | Expected |
| --- | --- | --- | --- |
| C11 | A plan is generated once per translation unit | contract (the header's guarantee "evaluated once in each translation unit") | A dedicated unit uses at least three distinct `product_plan`s of non-trivial size (for example M ⟑ M, L ⟑ G, P ∧ L), each read in two functions and one `static_assert`. In its trace, the median duration of the plan variable template's `EvaluateAsInitializer` events is below one tenth of the median duration of the `InstantiateFunction` events of the generator. The script fails, rather than passing vacuously, when it finds fewer than three of either, so a rename cannot hide the check. The trace's `detail` names the variable template without its arguments (planning probe), which is why the check compares medians rather than pairs |

### WU7b, in `test/core/plan_test.cpp`

| # | Test case | Reference (kind) | Expected |
| --- | --- | --- | --- |
| D1 | Maps by definition | proof and independent value | Criterion 12 |
| D2 | Maps over oriented rigid(3) sets | independent value, hand-derived (planning values from `conventions_check.py`) | Over L, with R = (e12, e13, e14, e23, e24, e34): reverse (0,2,1) (1,4,1) (2,5,1) (3,3,−1) (4,1,1) (5,0,−1); grade involution all +1 times s_a, that is (0,2,−1) (1,4,−1) (2,5,−1) (3,3,1) (4,1,−1) (5,0,1); right complement (0,3,−1) (1,1,1) (2,0,−1) (3,2,1) (4,4,1) (5,5,1), and the left complement the same (grade 2 in n = 4, so (−1)^(gr·ag) = 1). Over P: right complement onto R = (e123, e124, e134, e234), (0,3,1) (1,2,−1) (2,1,1) (3,0,−1); left complement (0,3,−1) (1,2,1) (2,1,−1) (3,0,1); antireverse all −1. Over G: right complement onto (e1, e2, e3, e4), all −1; left complement all +1. Each derived in the test comment from u ∧ ū = 𝟙, for example right(e41) = right(−e14) = −e23 because e14 ∧ e23 = +e1234 |
| D3 | Pair values of the antiproducts in rigid(3) | independent value, hand-derived as in section 2 and confirmed by `conventions_check.py` | e234 ⟇ e234 = 𝟙; e123 ⟇ e123 = 0; e1 ⟇ e1 = 0; e4 ⟇ e4 = −𝟙; 𝟙 ⟇ e1 = e1 ⟇ 𝟙 = e1; e234 ∨ e134 = −e34; e1 ∨ e234 = 1 and e234 ∨ e1 = −1 |
| D4 | Relations of the maps | proof | Criterion 13 |
| D5 | Antiproducts against golden values | independent value (golden `gp_tables` and `complements`, LDLᵀ route) | Criterion 15 |
| D6 | De Morgan laws | proof | Criterion 14; marker states "right(a ^ b) = right(a) v right(b)", the 4.2 trace fact |
| D7 | Typed antiproduct plans of rigid(3) and conformal(3) | independent value (planning values; the test recomputes each from D5's golden composition) | P ⟇ P: 7 terms, R = (e12, e13, e23, e1234). P ∨ P: none. G ⟇ G: 15 terms, R = (e12, e13, e14, e23, e24, e34, e1234). G ∨ G: 12 terms, R = the six bivectors (two planes meet in a line), terms (0,1,e34,1) (0,2,e24,−1) (0,3,e23,1) (1,0,e34,−1) (1,2,e14,1) (1,3,e13,−1) (2,0,e24,1) (2,1,e14,−1) (2,3,e12,1) (3,0,e23,−1) (3,1,e13,1) (3,2,e12,−1), written as masks. L ∨ G: 12 terms, R = (e1, e2, e3, e4) (a line meets a plane in a point). M ⟇ P: 20 terms, R = the eight odd blades. M ⟇ M: 48 terms, R = (1, the six bivectors, e1234). conformal(3) V ⟇ V: 25 terms, R = (e123, e124, e125, e134, e135, e145, e234, e235, e245, e345, e12345); V ∨ V: none |
| D8 | Associativity, identities, product rules | identity | Criterion 16 |
| D9 | Map and conjugation checks at each bound | contract (V4, DoD-12) | Hand-written maps and conjugated plans with one defect each, including a complement sign that disagrees with the closed form |
| D10 | Compile time equals run time | contract | Maps of `full<4>` and `full<5>`, and the conjugated `full<4>` plans of rigid(3) and conformal(2), in a `static_assert` and at run time |

The evaluation check C11 gains one `unary_map` and one conjugated plan in its unit. Rejections: a map over a set beyond e_n and with a bad sign, each requiring its message.

Mutants to be killed, beyond the generation-time checks (each recorded with the test that kills it, both compilers, both modes): the wedge's shared-generator test dropped; the wedge sign replaced by +1; the result set ordered by mask value; a result blade kept with no term; a result sign copied from an input; a position off by one; the wedge capacity sized for the geometric product (equivalent in output, recorded as a cost mutant); plain locals in each new generator; the run reversal dropped; the output complemented on the right instead of the left; one input not complemented; the left and right complement signs swapped (survives every odd n, so the test must have even n); the reverse sign with k(k+1)/2; the antireverse with the grade in place of the antigrade.

## 7. Approach

Direct forms first (C3, W4), written in the style of `plan.hpp` and `blade_set.hpp`: `constexpr` functions over values and spans, `consteval` makers with `static constexpr` locals and `static_assert`s with literal messages, no catch-all overloads (no function here takes a typed operand), `GALOOT_ASSERT` for run-time preconditions. The algebra is a tag constrained by `metric_like` (decision 0021); sets are class-type values; the operation is an enumeration value.

Order of commits, each building and passing on what it was checked on (W2, W9 as amended by decision 0094; test scope as the handoff states: affected tests and the fast gates per commit, the full gcc-debug suite at the end of each test-and-code pair, all seven presets once at the final revision):

WU7a, on `feature/core-plan-results`:

1. The W10 before rows and the P2 before record, from fresh trees at the claim revision.
2. The header change: operation parameter, wedge rule, result typing, `product_plan`; `gp_plan` removed. Then the migration of WU6b's tests and rejections in its own commit (criterion 7). If the header cannot build without the migration, the two land as one commit whose message lists the migrated tests and states that no expected value changed (W9).
3. Tests C1 to C10 and the new rejection, with markers, and the 4.1 wedge trace row set to `yes`.
4. The evaluation check C11 and its CMake block; its mutants run.
5. Decisions 0110 to 0113; the measured limits in the `///` comment.
6. Mutation records; P2 after; W10 after.
7. Outcome with the V10 line; W5 review; fixes in their own commits; delta review of any core fix (decision 0094 item 3); merge after `develop` is merged in and P9 passes.

WU7b, on `feature/core-plan-maps`, after WU7a merges: the claim line in this plan; the `golden.py` change for n = 3 with `golden.hpp` regenerated; maps and conjugation in the header; tests D1 to D10 and the 4.2 trace row; the evaluation check extended; decisions 0114 to 0116; records; outcome, review, merge.

Cost and limits, carried from WU6b: the exhaustive checks run at run time with the same `constexpr` code (decision 0103 item 2); compile-time checks stay at n ≤ 4 for full sets and at the typed and sparse pairs; no preset raises a limit. Result typing adds work proportional to the terms times the result size in its direct form; for the typed rigid(3) pairs that is at most a few hundred steps. Where the full-set ceilings fall, the cheaper form (a binary search for positions, or merging the per-pair runs) is a measured follow-up, not part of the first implementation.

Oracles (optional, VVUQ V1 kind 4, only after the unit's own evidence passes): the math tools named in note 0014 fit as black-box cross-checks. A dump of the typed plans of all four operations over note 0014's 100 forms, compared with route W (python-flint exact harness) and SageMath `CliffordAlgebra` route A for the products, with complements computed in the oracle script, would add an independent implementation for the non-diagonal antiproducts (departure 8). Z3 can prove the order-reversal property of the complement over all masks of up to 16 bits, as it proved the complement signs. Any such check is recorded as a research note, never cited from code, with the tool versions pinned (role file; math-tools README).

## 8. Evidence

- Test output per preset for `core.plan` and `core.plan_evaluated_once`, with the exhaustive ranges stated (n ≤ 6 for C1, C3, C4, D6, D8; n ≤ 10 for D1, D4).
- The trace gate output quoted in each merge message, with the rows of 4.1 (wedge) and 4.2 now pinned.
- Mutation records for both parts, with the first error line of each failed build.
- Compile-fail output on both compilers.
- The measured limits and the P2 records, each as `tools/timed` runs in `runs.tsv` with their commands.
- W10: per-unit instructions, wall time, and peak memory for `core.hpp` alone, `plan.hpp` alone, `smoke_test.cpp`, `plan_test.cpp`, and the compile-fail sources, before and after each part, from clean revisions.
- V10 line, to fill in the outcome. Verification: C1 to C10 and D1 to D10 (kinds 1 to 3 over the finite ranges). Validation: the classification referents already held by `blade_product` (plan 0011 A6 to A8); the Grassmann case of the wedge (C3); for rigid(3), Lengyel's statement that 𝟙 is the identity of the geometric antiproduct (note 0005, D2, read and recomputed) and the rigid(3) values of `conventions_check.py`, which encodes Lengyel's tables; for non-diagonal antiproducts, not performed (departure 8).

## 9. Evidence gathered while planning

Planning values, not reproducible as committed (departure 12). Each run went through `tools/timed`; the rows below were appended to `runs.tsv` in the main checkout and not committed (columns: time, step, selection, wall seconds, exit, instructions).

| Time (UTC) | Step | Selection | Wall | Exit | Instructions |
| --- | --- | --- | --- | --- | --- |
| 2026-11-01T15:56:24Z | probe | wu7-plan-probe:evtrace:committed | 0.193 | 0 | 1,325,454,087 |
| 2026-11-01T15:56:24Z | probe | wu7-plan-probe:evtrace:plain | 0.182 | 0 | 1,478,696,584 |
| 2026-11-01T16:00:53Z | probe | wu7-plan-exercise:complement-order | 0.384 | 0 | 6,942,526,368 |
| 2026-11-01T16:03:38Z | probe | wu7-plan-exercise:expected-values | 0.117 | 0 | 1,062,947,615 |

1. The evaluation-check probe. Clang 23.1.4, `-std=c++23 -O0 -c -ftime-trace -ftime-trace-granularity=0`, on a unit that reads `gp_plan<rga3, L, G>` and `gp_plan<rga3, L, L>` in two functions, with `develop`'s headers at `88c89ef` and with a copy whose seven locals of `make_gp_plan` are plain `constexpr`. As committed: the two `EvaluateAsInitializer` events of `gp_plan` took 5 µs and 5 µs, the two instantiations of `make_gp_plan` 12,945 µs and 5,702 µs. With plain locals: 3,342 µs and 4,837 µs against 12,814 µs and 5,659 µs, ratios of 26% and 85%. Both traces have the same events with the same counts, and no event nests inside the initializers, so only durations tell the forms apart (agreeing with review F3 of WU6b). The `detail` of the initializer event is the variable template's name without its arguments. The first instantiation is the longer in both traces, inferred to include the first instantiation of helpers that later plans reuse; the median in C11 is chosen so that this does not matter.
2. The complement reverses canonical order: checked for every pair of masks for n = 1 to 8. The closed form of the right-complement sign and (−1)^(gr·ag) between left and right: checked for every mask for n = 1 to 10. Conjugation by run reversal: for rigid(3), conformal(3), the degenerate form, R(1,3,0), and seeded random forms 0 to 3, every pair's conjugated run is in canonical order and equals left(right(e_A) right(e_B)) computed through `clifford_reference.Algebra`.
3. The values of section 6 marked as planning values, from `clifford_reference.py` (products of any form), `conventions_check.py` (rigid(3) complements, antiproducts, and antiwedges), and a definitional wedge and complement written in the scratch script. Every rigid(3) pair value in D3 agrees between the hand derivation of section 2 and `conventions_check.py`. The counts C2 of P ⟑ P, L ⟑ G, M ⟑ P, and L ⟑ L agree with the counts in plan 0011 (B2, B3).

## 10. Review

W5 blind review of each part before its merge, by a reviewer independent of the author, given this plan's criteria, section 6, and the author's departures, not the author's view that the change is correct. The reviewer also checks the derivations of section 2 (the result set as exactly the blades that can be nonzero, and the order reversal), since tests cite them; reruns at least the wedge shared-generator mutant and the result-order mutant (WU7a) and the run-reversal and left-right swap mutants (WU7b); and runs the evaluation check against the plain-locals copy itself. A fix to core code after the review gets a delta review (decision 0094 item 3).

## 11. Agent cost estimate (for information only, decision 0019)

From `records/agent-cost.tsv`: the WU6b implementer with review fixes, 547k tokens and 206 tool calls; its reviewer, 344k and 122; the plan 0012 implementer with fixes, 475k and 191; its reviewer, 300k and 95.

| Role | Tokens | Tool calls |
| --- | --- | --- |
| WU7a implementer, with review fixes | 500k to 650k | 190 to 240 |
| WU7a blind reviewer, plus a delta review | 330k + 150k | 110 + 50 |
| WU7b implementer, with review fixes | 500k to 650k | 190 to 240 |
| WU7b blind reviewer, plus a delta review | 330k + 150k | 110 + 50 |
| Optional oracle cross-check (math-expert) | 250k to 300k | 90 to 110 |
| Total | about 2.0M to 2.6M | about 740 to 950 |

No bound applies. The outcome of each part compares these with the actuals.

## 12. Risks

1. Common-mode error between the definitions in the tests and in the header, since both are written from CONVENTIONS by agents. Mitigation: each family has a second route that shares no code: golden tables (LDLᵀ over fractions), `conventions_check.py`, the closed-form complement sign, and hand values.
2. Odd n hides a left-right complement swap (left = right when n is odd). Every complement test includes even n, and the mutant list includes the swap.
3. Result typing in its direct form may lower the full-set ceilings, most for the wedge, whose `full<n>` × `full<n>` plan has 3^n terms over 2^n result blades. Measured and reported (criterion 8); the typed and sparse uses, which are what programs write, are far below.
4. The evaluation check reads durations on a shared machine. Its margin (section 9, item 1, and the WU6b review's figures) is a factor of more than a hundred below the threshold as committed and at least 2.6 above it with plain locals; medians absorb one preempted interval. A failure that does not reproduce is a defect of the test to trace (P10), never a rerun.
5. Migration of WU6b's tests (departure 5) could hide a weakened expectation. The migration commit changes how each value is read and no value; the review compares the two versions of each test line.
6. Compile cost: `plan.hpp` already costs every includer of `core.hpp` about 0.1 to 0.2 G instructions (plan 0011, WU6b departure 9). WU7 adds two enumerations and several function templates; W10 records the change.
7. GCC -O0 debug objects grow with each new `static constexpr` local (decision 0102 note); open owner item, departure 11.
8. The plan-level De Morgan proofs over n ≤ 6 compose plans and maps in the test; for dense forms at n = 6 this may take seconds in a debug build. If so, the case carries the `slow` label (P9) and runs from the next merge level up; it is never omitted.
9. Lessons 0060 to 0063 apply: mutation drivers read the build's status first, scripted edits assert their anchors, before-and-after costs come from clean revisions, and no CTest runs after a failed build in a tree whose programs were deleted.

## 13. Deferred

- To WU8: `mv`, the conversion of `coefficient_type` into the scalar, the whole-set repeated-mask check (departure 10), applying plans and maps to stored coefficients.
- To WU9 and WU10: the public operations on multivectors and the operators of CONVENTIONS 7.1; the randomized multivector identities of DoD-10.
- To WU11: bulk, weight, duals, dot, antidot, which need the metric exomorphism (departure 9).
- To M7 (decision D8): collected plans for repeated inputs, where structural cancellation can occur (section 2).
- Not built, noted only (C12): binary search or run merging in result typing (risk 3); keeping a declared type's own blade set for unary results instead of the canonical set (a choice for WU8 or WU13, where `transform` needs the blade set of its input).

## 14. Owner questions, collected

1. Two parts (departure 1).
2. The WU10 boundary and the 4.2 trace row (departure 3).
3. Replace `gp_plan` (departure 5).
4. The C4 reading for the wedge's pair rule (departure 6).
5. The evaluation check form, Clang only (departure 7).
6. Carried: the GCC -O0 debug size of `static constexpr` locals, and the `metric_like` P2 exception (departure 11).

## Outcome, WU7a (2026-11-01, Claude, agent, Opus 5.5, cpp-expert, implementer)

Status: done on `feature/core-plan-results`, awaiting the W5 blind review. Code and tests final at `d6ea717` (the header last changed there, in its `///` comment; the code last changed at `38f2b30`); all seven presets from new, empty build trees at `86226f7`, whose code and tests are `d6ea717`'s. This section and the W10 after rows are committed after it and change records only.

### Departures, skipped checks, and ambiguities (C20)

1. Owner answers: the owner said to proceed with the plan's defaults for questions 1 to 5, and they are recorded as decisions: 0117 (questions 1 and 2, the split and the WU9 and WU10 boundaries), 0111 (question 3, `gp_plan` replaced), 0110 (question 4, the wedge as its definition), 0112 (question 5, durations on Clang only). Decision 0117 uses the first number of the plan's unassigned range (0117 to 0119). Question 6 (the GCC -O0 debug size of `static constexpr` locals, and the `metric_like` P2 exception) is not decided; see "Open questions".
2. Names that differ from the plan: the enumeration is `BinaryOperation`, not `binary_operation`, because STYLE S10 and `.clang-tidy` require CamelCase for an enumeration type (decision 0110 item 1); the typed plan is `typed_plan<R, T>` with `typed_term`; the typing functions are `collect_result` and `type_terms`, with `typed_defects_of` for the checks.
3. Rejections beyond the plan's one (`wedge_beyond_n`): `wedge_pair_beyond_n` and `_second` (the wedge's own range check, which no set check precedes in a direct call, one per operand) and `result_room` (`collect_result`'s room check). A test of the evaluation script's own rules beyond the plan: `tools.evaluated_once_passes`, `_rejects_twice`, `_rejects_few` on hand-written traces.
4. Commits that change the header after the test commit `7818335`, for the review: `38f2b30` (a format fix and the clang-tidy suppression of decision 0110 item 6; no code change) and `d6ea717` (comment only). Commits that change tests after it: `04c12c1` (lint layout: trailing commas, `const`, `std::ranges::all_of`, named terms; no expected value). Lint and format-check failed at `7818335` and pass from `04c12c1`; `7818335`'s message does not say so.
5. Migration (criterion 7, `9430423`): every expected value of B1 to B8 and of the seven rejections is unchanged. The test reads a typed plan back to mask-level terms (`gp_terms`, `read_back`). Changed in form, as its message lists: the two functions of B7 read `product_plan`'s own terms; B8's assertion that the plan is `std::array<plan_term, 15>` became that its terms are `std::array<typed_term, 15>`; test names and comments name `product_plan` and `generate_plan`; the rejection messages' prefix is "product plan:", and the set rejections forbid every generation message in full, which also closes a gap of the old prefix "gp plan: a generated" (it did not cover "generated terms are out of order or repeated").
6. Verification on the tree before a commit: `3bca0e6`, `9430423`, and `11ad0aa` were checked on the working tree before the commit (runs logged as `<parent>-dirty`), with content equal to the commit except `11ad0aa`, whose unit's `static_assert`s were laid out differently after that run; `11ad0aa` itself was then checked on all four Clang presets. Every other commit was checked after it was made.
7. The first seven-preset run at `86226f7` reconfigured the existing trees with `--fresh`, which keeps the objects, so its builds were incremental (0.7 s on gcc-debug); it is not counted. All seven were then built from new, empty trees (`build/final-<preset>`), which is the run reported below.
8. Steps outside `tools/timed` (P13 lapse): one direct run of `build/final-gcc-debug/test/core_plan_test` to read its counts (20 test cases, 815 assertions), after the timed CTest runs; the earlier probes compiled through `tools/timed`.
9. The limits script's first run had a defect of the script (its Clang wedge unit divided by the size of an empty plan, failing `-Wdivision-by-zero`); the run was repeated whole with the fixed script (`27e227f`).
10. The mutation driver's `wedge_capacity_gp` was first killed only by `-Wunused-parameter`; it was rewritten and rerun with three added mutants (`records/mutations/0013-plan-results/survivors.md`).
11. Not done here, as briefed: the agent-cost row, the W5 review, any delta review, and the merge. The knowledge-base branch `kb/constant-evaluation-memoization` is not merged into the knowledge base's `develop`.

### Done

| Commit | What |
| --- | --- |
| `6c129a8` | W10 before rows (fresh gcc-debug and clang-debug trees at `0bbb968`, 80 rows each; 124 of 124 on both) |
| `3bca0e6` | `BinaryOperation`, `pair_capacity`, `plan_capacity(op, ...)`, `pair_terms` (the two pair rules), `generate_plan`, `collect_result`, `typed_term`, `type_terms`, `typed_plan`, `typed_defects_of`, `make_product_plan`, `product_plan`; `gp_plan` kept, forwarding |
| `9430423` | Migration of WU6b's tests and rejections (criterion 7) |
| `0fce606` | `gp_plan`, `make_gp_plan`, `generate_gp`, the two-set `plan_capacity`, `filled_gp` removed |
| `7818335` | Tests C1 to C10 and criterion 5, four rejections, the 4.1 wedge trace row `yes` |
| `38f2b30`, `04c12c1` | Format and lint fixes (item 4) |
| `11ad0aa` | `core.plan_evaluated_once` (Clang only), `test/tools/evaluated_once.py`, its fixtures and tests |
| `27e227f`, `36e4f0f` | Limits record (`records/performance/plan-results/`), the mutation driver |
| `19b52b6` | Mutation results and `survivors.md`; decisions 0110, 0111, 0112, 0117 |
| `d6ea717` | Measured limits in `product_plan`'s `///` comment |
| `86226f7` | P2 record (`out/growth-d6ea717/`) and decision 0113 |

### Tests, for the reviewer

`core.plan` at `86226f7`: 20 test cases, 815 assertions, and namespace-scope `static_assert`s; 15 compile-fail tests `core.plan_*`; on Clang `core.plan_evaluated_once`. The trace gate finds 4.1's wedge fact at `plan_test.cpp:1539`.

| Case | Expected |
| --- | --- |
| C1 typed reads back | both operations, `full<n>` × `full<n>` of the 42 forms (n ≤ 6), typed at run time: result set = distinct output blades, sign +1, in the test's own order (grade, then index list); every term reads back; positions in range |
| C2 typed rigid(3) pairs | P ⟑ P and P ∧ P typed in full by hand; L ⟑ G 15 terms on (e1, e2, e3, e4, e124, e134, e234), L ∧ G none, P ∧ L 12 on the trivectors, M ⟑ P 28 and M ∧ P 16 on vectors and trivectors, L ⟑ L 27 on (1, bivectors, e1234), L ∧ L 6 on e1234, each term by the test's diagonal or wedge rule |
| C3 wedge rule | every pair of the 42 forms: no term on a shared generator, else the test's inversion sign on A ∪ B; = grade |A| + |B| part of `blade_product`; the same for every form of one n; = the product in R(0,0,n), n = 1 to 6 |
| C4 associativity | every triple of blades of the 42 forms, over the wedge plan |
| C5 conformal(3) vectors | V ⟑ V 25 terms on 1 and ten bivectors, e4 e5 = −1 + e45, e5 e4 = −1 − e45; V ∧ V 20 terms, no scalar |
| C6 sparse, n = 8, 12, 16 | ten product terms on eight result blades; eight wedge terms on seven |
| C7 result order | P ∧ P lists e14 before e23; the results of `full<4>` and `full<5>` in R(n,0,0) equal the golden canonical lists |
| C8 checks at each bound | `typed_defects_of` on hand-written plans, each defect alone; `collect_result` and `type_terms` at their stated bounds |
| C9 repeated, empty | a mask listed twice shares one position; empty sets give `typed_plan<0, 0>` |
| C10 compile time = run time | `full<4>` of rigid(3), R(4,0,0), R(1,3,0), conformal(2), `full<3>` of the degenerate form, both operations, against direct calls in a `static_assert`; rigid(3) also at run time on a metric read through `volatile` |
| Criterion 5 | the wedge = top-grade part of the four golden product tables |
| C11 | `core.plan_evaluated_once`: median initializer below a tenth of the median generator instantiation, three plans |

### Acceptance criteria (WU7a)

1. Met (C1), exhaustive over the 42 forms, on all seven presets at `86226f7`.
2. Met (C3).
3. Met (C4).
4. Met (C2, C5, C6, C10).
5. Met (golden wedge case).
6. Met: every generation-time check of the typed plan fires on a generator or typing mutant, and `defects_of` and `typed_defects_of` are tested at each bound (`survivors.md`).
7. Met (`9430423`, item 5): same expected values; the messages are the only changed text of the rejections, and the set rejections forbid every message generation would add.
8. Met: decision 0113. The typed and sparse pairs and the conformal(3) vectors build for both operations in all three configurations on both compilers; `full<n>` ceilings are stated in the `///` comment. Reported drop (C14): GCC's product ceiling falls from 7 to 6 in the diagonal, rigid, and conformal forms, stopped in the direct typing's linear scans at n = 7; Clang's ceilings and GCC's dense ceiling are unchanged.
9. Met: `core.plan_evaluated_once` passes on clang-debug, clang-release, clang-asan, and lint at `86226f7` (new trees) and at `11ad0aa`; `locals_plain` and `locals_plain_twice` fail it on clang-debug in both mutation modes; `twice_static_locals` passes it, because nothing is generated again.
10. Met: decision 0113 and `records/performance/plan-results/out/growth-d6ea717/`. Growth from n = 4 to 16 is about 18 M (GCC) and 23 M (Clang), before and after alike: the `metric_like` check of decision 0107; typing adds +119 M and +195 M at every n.
11. Met: the seven presets from new, empty trees at `86226f7`, each building and passing every test (131 on GCC presets, 132 on Clang presets, which add `core.plan_evaluated_once`); gcc-debug, clang-debug, and lint also built `all_verify_interface_header_sets` and `format-check`, and lint's clang-tidy is clean. The trace gate and the marker check (78 test cases) exit 0 as scripts at `86226f7`; markers, golden facts, citations, golden regeneration, and the include scan pass as CTests in every preset. W10 before at `0bbb968`, after at `86226f7`, both from new trees.

Mutation summary: 81 mutants on gcc-debug and clang-debug in both modes; 68 killed everywhere, 10 equivalent (removals of checks a correct generator cannot trip), `locals_plain` and `locals_plain_twice` killed on Clang only by design, `twice_static_locals` surviving by design. Unresolved: none. Without a mutant of this unit of their own (`survivors.md`): WU6b's `static_assert` of e123 e123 = 0 in the degenerate form and its capacity test case, and this unit's `terms_of` `static_assert` (a plan as a template argument), whose only mutant is killed earlier by every constant-expression use. Job pool: 23 of 23 free before and after every heavy run; never stopped or started.

### Costs (W10; new trees, shared mode, track-only)

Before at `0bbb968`, after at `86226f7`; instructions of both kinds of core added; rows in `compile.tsv`.

| Unit | gcc-debug before | gcc-debug after | clang-debug before | clang-debug after |
| --- | --- | --- | --- | --- |
| `core.hpp` alone (header set) | 0.808 G, 72 MiB | 0.901 G, 77 MiB (+11.5%) | 0.750 G, 107 MiB | 0.912 G, 110 MiB (+21.7%) |
| `plan.hpp` alone | 0.808 G, 73 MiB | 0.902 G, 77 MiB | 0.750 G, 103 MiB | 0.908 G, 110 MiB |
| `smoke_test.cpp` | 1.006 G, 83 MiB | 1.099 G, 87 MiB | 0.847 G, 119 MiB | 1.008 G, 123 MiB |
| `plan_test.cpp` | 9.548 G, 238 MiB | 21.088 G, 330 MiB, 1.89 s (+121%) | 11.328 G, 206 MiB | 23.099 G, 285 MiB, 1.90 s (+104%) |
| `plan_evaluated_once.cpp` (Clang CTest) | | | | 2.019 G, 136 MiB |
| other test units | | unchanged within 0.1% | | unchanged within 0.1% |
| compile-fail sources | 65, 40.66 G | 69, 44.86 G | 65, 34.18 G | 69, 39.08 G |
| all compiles | 80, 93.6 G | 84, 109.7 G | 80, 91.8 G | 85, 111.0 G |
| CTest, whole suite | 124 tests, 1.58 s | 131 tests, 2.15 s | 124 tests, 1.84 s | 132 tests, 1.66 s |

Notable, reported with cause (P2 note, C14): `plan.hpp` costs every includer of `core.hpp` +0.09 G on GCC and +0.16 G on Clang more than before, attributed by a Clang trace mostly to `std::span` instantiations over the new element types (decision 0113). `plan_test.cpp` doubles: the new compile-time checks (typed rigid(3), conformal(3), and sparse plans of both operations, and the ten `full<4>`/`full<3>` typed plans compared with direct calls in a constant evaluation). The CTest times are shared-mode and not compared.

### Verification and validation

Verification: performed by C1 to C10 and the golden wedge case (kinds 1 to 3 over the stated finite ranges: n ≤ 6 for C1, C3, C4) and passed on all seven presets at `86226f7`. Validation: for the wedge, the Grassmann case (R(0,0,n), C3) and the golden tables (LDLᵀ route); otherwise not applicable beyond what `blade_product`'s referents give (plan 0011, A6 to A8): a typed plan is its mask-level plan with each output written as a position in a canonical list. Reproduction, from the worktree root with the pool exported: `python3 -I docs/agent/records/performance/plan-results/measure_results.py limits <out> [cpu]` and `... growth 0bbb968 <out> 3 10`; `python3 -I docs/agent/records/mutations/0013-plan-results/mutate.py <preset> [--runtime] [mutation...]`.

### Open questions for the owner

1. Carried (departure 11, item 2 of the handoff): keep the `static constexpr` locals, which cost GCC -O0 debug object size, or prefer smaller debug objects? If plain locals are chosen, `core.plan_evaluated_once` fails on Clang by design, and a different remedy for Clang's second evaluation is needed first. Not decided here.
2. Carried (decision 0107 item 4): is the per-algebra `metric_like` check an accepted exception to P2? The P2 record here shows the same growth, of the same size, before and after typing. Not decided here.
3. New: the drop of GCC's `full<7>` product ceiling, and the header cost of `plan.hpp` to every includer of `core.hpp` (decision 0113). Reported with their causes; no bound is set, and no cheaper form is built.

### Knowledge base

Branch `kb/constant-evaluation-memoization` of the knowledge base (worktree `knowledge-base-worktrees/constant-evaluation-memoization`): `performance-engineering/memoizing-constant-evaluation.md` (new, indexed in that directory's README) and `exercise-archive/cpp-repeated-constant-evaluation.md` (new); commits `44c6e9d`, `60b77d8`; `tools/check_links.py` reports 0 problems. Not merged into its `develop`.

### Next

The W5 blind review of WU7a (it reruns the wedge shared-generator and result-order mutants and runs the evaluation check against a plain-locals copy); fixes in their own commits; a delta review of any core fix; then the merge, which the coordinator makes. WU7b is claimed by a dated line here after that merge. Agent cost: about 200 tool calls; token counts are not visible to me.

## W5 review fixes, WU7a (2026-11-01, Claude, agent, Opus 5.5, cpp-expert)

The W5 review (`records/reviews/2026-11-01-wu7a-plan-results.md`, blind-reviewer, Opus 5.5, head `0607515`) accepted WU7a after fixes. F1 to F5 are applied below; F6 is the owner's and is not acted on here. The outcome above is left as written; this section corrects it.

### Departures, skipped checks, and ambiguities (C20)

1. Correction of the outcome's departure 4 (F1). It said lint and format-check failed at `7818335` and passed from `04c12c1`. The review built each commit from its committed tree: `format-check` failed at `3bca0e6`, `9430423`, `0fce606`, and `7818335` (the layout of `pair_capacity`, `plan.hpp:90` and `:91`) and passes from `38f2b30`; clang-tidy (the lint preset's `all`) failed at `9430423`, `0fce606`, and `7818335` (the cast false positive at the `product_plan` line, and at `7818335` also trailing commas in `plan_test.cpp`) and at `38f2b30` (the trailing commas), and passes from `04c12c1`; at `3bca0e6` clang-tidy passes. Two commit messages are wrong in the same way and stay as history: `3bca0e6` reports lint of the header sets only, where the cast finding does not appear, and `38f2b30` says lint and format-check "pass here", while lint does not pass there. Every commit builds and passes its full suite on gcc-debug and clang-debug (the review's table), so the units' tests held at each step; the mechanical checks did not.
2. F2 is killed first by a compiler warning. The mutant `wedge_range_shift` fails the build by `-Wshift-count-overflow` at the wedge's instantiation for N = 32, which only the new test case creates; with that warning off, both compilers still reject the new test's plan in its constant evaluation (`records/mutations/0013-plan-results/survivors.md`, "After the W5 review").
3. F4 is taken by extending the test, not by recording a reading.
4. F5 changes comments of `include/galoot/core/plan.hpp` (`0098409`), so it needs the delta review of decision 0094 item 3; not done here.
5. The reviewer's worktree `.claude/worktrees/review-wu7a` is left in place, as the coordinator instructed.

### Findings and what was done

| Finding | Commit | Result |
| --- | --- | --- |
| F1 record of the mechanical checks | this commit | item 1 above |
| F2 the wedge at the widest algebra | `7d63026` (test) | "The wedge plan accepts the generators of the widest algebra": R(32,0,0), {e1, −e32}, result (e1,32 with +1), terms (0, 1, 0, −1) and (1, 0, 0, +1), and a run-time count of 2. `wedge_range_shift` killed on gcc-debug and clang-debug in both modes at `d4bcb8d` (item 2) |
| F3 one-sided bounds | `f06d035` (test) | result signs 0 and 2 in the bound test of the typed checks; a test case of `pair_capacity` (wedge 1 for disjoint, shared, and scalar pairs; product 2, 8, and 1). `result_sign_negative_only` and `wedge_capacity_zero` killed on both compilers in both modes at `d4bcb8d` |
| F4 run time for every form of criterion 4 | `d4bcb8d` (test) | "product_plan equals its typing run at run time" now covers R(4,0,0), R(1,3,0), conformal(2) (`full<4>`), and the degenerate form (`full<3>`), both operations, on metrics read through `volatile`, beside rigid(3). Criterion 4 is met in full |
| F5 two comments | `0098409` (comments only) | the result set is exactly the blades that can be nonzero for a scalar type in which no nonzero integer vanishes; generating once per unit comes from the generator's static locals, checked on Clang only |
| F6 header cost and ceiling | none | for the owner (open question 3 of the outcome) |

Mutation records: `results-d4bcb8d-<preset>[-runtime]-review.tsv` and the section added to `survivors.md` (`32e600e`).

### Checks per commit

- `7d63026`: `core_plan_test` and `core.plan` with the `tests` label on gcc-debug (17 of 17), clang-debug (18 of 18), and lint (`core.plan`, 13 of 13, clang-tidy of `core_plan_test` clean).
- `f06d035`: the same on gcc-debug (17 of 17) and clang-debug (`core.plan`, 13 of 13).
- `d4bcb8d`: gcc-debug (17 of 17), clang-debug and lint (`core.plan`, 13 of 13 each).
- `0098409`: `all`, the header sets, and `format-check` on gcc-debug with `core.plan`, `core.smoke`, `core.headers`, `tests`, `reference` (22 of 22); clang-debug the same with `tools.evaluated_once` (26 of 26); lint the header sets and `core_plan_test` (15 of 15).
- The four W5 checks at the final head are in the report of this pass and in the merge message.

### Next

The delta review of `0098409` (comments of `plan.hpp`) and of the three test commits; then the merge, which the coordinator makes. The agent-cost row is the coordinator's.
