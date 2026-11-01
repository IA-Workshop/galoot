# 0114 Unary maps: per-blade signs and complements as typed data

- Status: accepted (recorded by the author; the owner may reverse it)
- Date: 2026-11-01
- Decided by: Claude (agent, Opus 5.5), implementing WU7b of plan 0013 in the role of cpp-expert.

## Context

Plan 0013 (section 3, WU7b) asks for the five per-blade operations that CONVENTIONS fixes, the reverse, the antireverse, and the grade involution (2.1) and the right and left complements (3.1), as data typed like a plan: a result set and one term per input blade. The signs come from `bits.hpp`, already proved against their definitions (trace rows 2.1 and 3.1). The bulk, weight, and duals (3.2, 3.3) need the metric exomorphism and are WU11's (plan departure 9). VVUQ V4 asks for cheap checks at generation; plan criterion 18 asks for a check of the complement signs by a route that does not share the code it checks.

## Options weighed

For the form of a map:

1. A map type of its own, `typed_map<R, K>` of `map_term {a, out, coefficient}`, typed by the functions that type a plan. Chosen: a reader applying a map needs a, out, and the coefficient, and nothing else.
2. A `typed_plan` with every b equal to 0, as the plan of x times the scalar. One type fewer, but every user of a map would have to know that b means nothing.
3. A map that holds a callable per operation. More machinery for five operations that differ only in a sign and a mask.

For the second route of the complement sign:

1. The closed form (−1)^(k(k+1)/2 + i_1 + … + i_k), times (−1)^(k(n−k)) on the left. Chosen: it counts no reordering, so a wrong `reorder_sign` or `complement_mask` disagrees with it. Proved below; the knowledge base proves it with an SMT solver for every mask of up to 16 bits.
2. A second inversion count written differently. It would share the idea, and a common-mode error, with `reorder_sign`.

## Decision

1. `enum class UnaryOperation : std::uint8_t { reverse, antireverse, grade_involution, right_complement, left_complement }`. The plan proposed `unary_operation`; STYLE S10 and `.clang-tidy` name an enumeration type in CamelCase, as for `BinaryOperation` (decision 0110 item 1).
2. `unary_term(op, a, n)` gives the image of one canonical blade as a `term` (coefficient times the canonical blade of a mask): the blade itself with `reverse_sign`, `antireverse_sign`, or `grade_involution_sign`, or `complement_mask(a, n)` with `right_complement_sign` or `left_complement_sign`. It asserts `valid_count(n)` and `in_algebra(a, n)` itself, because `reverse_sign` and `grade_involution_sign` take no n and check nothing; rejections `core.plan_map_term_beyond_n` and `core.plan_map_term_zero_n`. The operation is a function parameter, since it selects only a value (S16).
3. `generate_map(op, n, set, out)` writes one `plan_term` per input position a, with b = 0 and coefficient s_a times the image's, so that `collect_result` and `type_terms` type a map without a second implementation. The b = 0 is internal: `typed_map_of` projects the typed terms onto `map_term`.
4. `map_term {std::size_t a, out; coefficient_type coefficient;}` and `typed_map<R, K> {blade_set<R> result; std::array<map_term, K> terms;}`, both structural, trivially copyable aggregates with defaulted `==`.
5. `unary_map<Op, Alg, S> = make_unary_map<Op, Alg, S>()`, constrained by `metric_like` and `blade_set` (rejection `core.plan_map_not_metric_like`). The algebra supplies n, which the antireverse and the complements depend on, and ties a map to its algebra as a plan is; the metric is not read. `make_unary_map` is `consteval`, every local `static constexpr` (seven), as decision 0102 item 7 requires; `core.plan_map_evaluated_once` checks it on Clang (decision 0112's form).
6. Before generation, the set is checked: "unary map: the blade set names a generator beyond e_n" and "unary map: the blade set holds a sign other than +1 or -1" (`core.plan_map_beyond_n`, `core.plan_map_bad_sign`; each forbids every generation message).
7. After typing, `map_defects_of(op, n, set, map)` computes seven flags, each a `static_assert` with its message: the result set out of strictly increasing canonical order, a result sign other than +1, a position at or past R, a result blade no term uses, a term at place k whose a is not k, a term not on the blade the operation gives its input (the input's mask for the sign operations, its complement for the complements), and a complement's coefficient other than s_a times the closed form. A sign operation's coefficient has no second route and is not checked here; the tests check it against the definitions.
8. A mask listed twice in a set is not rejected (decisions 0092, 0102 item 10): each place gets its own term, and both may land on one result position.

## The closed form of the right-complement sign

For a blade e_A with generators i_1 < … < i_k and C the generators A lacks, e_A ∧ e_C = (−1)^p e_12…n, where p counts the pairs of a generator of A and a generator of C below it. The j-th generator i_j of A has i_j − 1 generators below it, of which j − 1 are in A, so i_j − j are in C. So p = Σ i_j − k(k+1)/2, and since the right complement is s e_C with s (−1)^p = 1, s = (−1)^p = (−1)^(Σ i_j + k(k+1)/2), the same in every n that holds A. For the left complement, e_C ∧ e_A counts the pairs of C above A, which are the k(n−k) pairs less p, so its sign is (−1)^(k(n−k)) times the right one.

## Evidence

- `test/core/plan_test.cpp` from `a913760` (markers fixed at `26a019e`, cases added at `d6cbaba`): every map of every blade of n = 1 to 10 equals the test's own rule, with and without input signs, typed onto its distinct blades in canonical order; the golden grade signs and complements for n = 3, 4, 5; the compositions (reverse and grade involution involutions, left∘right = right∘left = id, right∘right = left∘left = (−1)^(gr·ag), antireverse = left∘reverse∘right) for n = 1 to 10; the hand-derived maps of the oriented rigid(3) line, point, and plane; the closed form against the test's inversion count for every mask of n = 1 to 10; compile time equal to run time for the maps of `full<4>` and `full<5>`; `map_defects_of` on hand-written maps, each defect alone and each bound on both sides.
- Mutants: `records/mutations/0013-plan-maps/`.

## Consequences

- WU10 builds the public `reverse`, `antireverse`, `grade_involution`, `right_complement`, `left_complement` and the operators `~` and `!` on `unary_map`.
- WU11 needs maps with several terms per input (the metric exomorphism in a non-diagonal form); `map_term` already allows several terms, but `map_defects_of`'s per-input check and `typed_map<R, K>`'s one term per input would change with it.
- A complement that ever disagrees with its closed form fails to compile, whatever path computed it.
