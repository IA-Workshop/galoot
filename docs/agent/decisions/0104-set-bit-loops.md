# 0104 The per-term loops of blade_product and the loop of reorder_sign visit only set bits

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing plan 0012 in the role of performance-engineer.

## Context

The W5 review of WU6b (finding F8, `records/reviews/2026-11-01-wu6b-gp-plan.md`) measured that a fixed use, the plans of four typed rigid(3) pairs embedded in R(n−1,0,1), costs 27% more instructions on Clang and 16% more on GCC at n = 16 than at n = 4. PERFORMANCE P2 records such growth as a defect unless it follows from the algebra's own structure. The review traced most of it to two loops in `blade_product.hpp`, which ran over all N generators for every term and skipped those the blade lacks, and a further part to `reorder_sign` in `bits.hpp`, which shifted its first argument right one step at a time until it was zero, so that its cost was the index of the highest generator of A rather than the number of generators. Plan 0012 asks for both to visit only the generators involved, with every output unchanged.

## Options weighed

1. Visit the set bits, lowest first: `for (mask_t remaining = m; remaining != 0; remaining &= remaining - 1)` with `j = std::countr_zero(remaining)`. The order of visits is the order of the old loops, so the alternating contraction sign needs no change, and each loop costs one step per generator of the blade. The plan's approach and the review's measured copy. Chosen.
2. Keep the index loop and stop it at the highest generator, `j < std::bit_width(m)`. Removes the cost of the generators above the blade but keeps the cost of the gaps below it, so a blade such as e1 ∧ e16 still costs 16 steps. Rejected: it leaves the growth for blades that hold e_n, which is every blade of the measured pairs that holds e4.
3. For `reorder_sign`, the parity of the count by a prefix XOR of `b` (five shifts and XORs give, at bit i, the parity of the generators of B below e_i; the sign is then the parity of `popcount(a & prefix)`). Constant cost, so cheaper than option 1 for blades of high grade (where the two cross was not measured). Not taken: the plan names option 1, the callers on the path of a product pass a first argument of small grade (`add_generator_product` one generator, the name parser a blade of at most nine generators), and it reads less plainly (STYLE S16). `left_complement_sign` passes the complement first, of grade up to n, so option 3 would make it cheaper at large n. It is the change to make if a later measurement shows `reorder_sign` costly on blades of high grade; not measured here.

## Decision

- `add_generator_product` (contraction) and `add_blade_product` (correction) loop over the set bits of the blade and of A′, lowest first, as option 1. Each blade is a subset of A ∪ B, which `blade_product` checks lies within e_1 to e_N, so every index stays below N. Commit `4e69ce8`.
- `reorder_sign(a, b)` visits the set bits of `a`, lowest first, and for each e_i adds `popcount(b & ((1 << i) − 1))`, the generators of B below it: the same count of pairs i > j as before. Its `///` comment now states the cost, one step per generator of A. Commit `77ed567`, with the index made unsigned in `c97fac5` (clang-tidy's `bugprone-signed-bitwise` rejected `mask_t{1} << i` with the `int` that `std::countr_zero` returns; lint had not run on `77ed567`).
- No interface changes. The two mutations of `docs/agent/snippets/bits_fault_injection.py` that named the old `reorder_sign` loop now name the new bound.

## Evidence

- Tests before the code (`971bc9e`): a case with the form whose only entries are g(e1, e32) = g(e32, e1) = 1, checking e1 e32 = 1 + e1 ∧ e32, e32 (e1 ∧ e32) = e32, (e1 ∧ e32) e32 = −e32, and (e1 ∧ e32)² = 1 (values from `test/reference/clifford_reference.py` on the form [[0, 1], [1, 0]]). In R(32,0,0) every pairing off the diagonal is zero, so before it no test read a pairing at the top bit in the correction.
- Old against new (`records/performance/product-cost-growth/compare.py`, outputs `out/compare-c97fac5-0f1039a-*.txt`, and the same at `77ed567`): a renamed copy of the headers at `0f1039a` and the headers at `c97fac5` in one program, GCC 16.2.1 and Clang 23.1.4 at -O2 with checking. 0 mismatches in 20,777,236 `reorder_sign` pairs (every pair below 2^12, then 200,000 random pairs at each n from 13 to 32), 2,008,190 complement signs, 4,203,996 products (every pair of masks of 210 forms of 1 to 8 generators: every diagonal signature, conformal, a Witt pair on e1 and e_n, a form with every entry nonzero, and three seeded random forms per n), 249 full-set plans with two sign patterns (n ≤ 5), 216,000 products and 2,880 plans of seeded random forms and masks at n = 9 to 32 (masks holding e_n a quarter of the time), and the typed `gp_plan` pairs at n = 4, 8, 12, 16 compared in `static_assert`s. Against copies of the new headers with one loop mutant each, it reports mismatches or fails its `static_assert`s (`out/compare-sanity-77ed567.txt`).
- Mutation (`records/mutations/0012-product-cost-growth/`): 16 mutants of the three loops' bounds (lowest bit skipped, highest skipped, bit 31 ignored, index one too high, visiting from the top, and two bounds of the `reorder_sign` count), all killed on gcc-debug and clang-debug in normal and run-time mode at `20dc2ea` (the six of `reorder_sign` again at `c97fac5`); without the new case, `cr_top_bit_ignored` survives, so the case is needed. The WU6a, WU6b, and WU6b-review sets (42, 49, 27 mutants) have their recorded status, mutant by mutant, in both modes on both compilers; the WU2 snippet catches its 13 (see `survivors.md` for its run-time mode on Clang).
- Every test passes unchanged; the golden tables are untouched (`git diff 0f1039a -- test/reference` is empty).

## Consequences

- The cost of a product of fixed blades no longer depends on N on GCC; on Clang the part that still grows is the concept check of decision 0105, which is paid once per algebra.
- `reorder_sign` costs the grade of its first argument, and its comment states that cost. On the path of a product the first argument is one generator; `left_complement_sign` passes a complement, of grade up to n, and costs that many steps (before, it cost the index of the highest generator, at most n as well).

## Note after the W5 review (2026-11-01)

Added after the W5 review (`records/reviews/2026-11-01-plan-0012-cost-growth.md`, F3, F5); the record above is unchanged.

- "so every index stays below N" holds in constant evaluation and in checking builds only. In an unchecked run-time build an out-of-range `b` now reads `g.gram[i][j]` with j ≥ N; no public path reaches it. Decision 0107 item 5.
- The cost sentence of `reorder_sign`'s comment now ends "whatever the indices of its generators" (`bea00a1`), since masks have one width.
