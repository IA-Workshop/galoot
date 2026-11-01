# 0008 WU4: oriented blade sets

- Status: claimed; branch `feature/core-blade-set`, worktree `.claude/worktrees/core-blade-set`; decision numbers 0090 to 0099.
- Goal: `include/galoot/core/blade_set.hpp`, the storage layout of a named type as an ordered list of oriented blades, with the canonical result order and the name parser (CONVENTIONS 1.1, 1.6, 1.7).
- Content, in `galoot::core`:
  - `oriented_blade`: a mask and a sign of plus or minus one, as a structural value.
  - `blade_set<K>`: a structural aggregate of K oriented blades, usable as a class-type template argument (the hybrid ruling: the algebra is a tag, blade sets are values).
  - Canonical order of a result without a declared type: grade first, then lexicographic order of the index lists (not numeric mask order), so for four generators e12, e13, e14, e23, e24, e34.
  - Constructors of blade sets: `full<N>` (every blade in canonical order), `of_grade<N, k>`, `merge` of two sets in canonical order, and `index_of` a mask in a set, which is a compile-time error when absent.
  - A name parser `blades<N>("e41 e42 e43 e23 e31 e12")` that turns oriented names into oriented blades: the mask from the indices, the sign from the sign of the permutation that sorts the indices. Compact names are parsed only for n up to 9; above that, masks only. Invalid or repeated names, indices above N, and a mask given twice are compile-time errors with messages in the algebra's terms (the catch-all form for functions, the plain static assertions in consteval code, as established).
  - Canonical names from masks (`e124` for the mask with bits 0, 1, 3), for diagnostics and the tests.
- Dependencies: `bits.hpp` (masks, grade, reorder sign) and `metric.hpp` (generator count). The core stays free of anything beyond the standard library. The generator count is `std::size_t`, grades are `int`.
- Definition of done: the header exists, is self-contained, documented (`///`, STYLE), and the tests pin the oriented-name table and the result order.
- Acceptance criteria:
  1. The oriented-name list of rigid(3) from the golden values (`golden::oriented_names`) parses to exactly the golden masks and signs, and the signs are cross-checked by a test-side permutation sign (an independent route).
  2. The result order for n = 4 and n = 5 equals `golden::result_order`, and for n up to 6 it equals the order produced by a test-side sort by (grade, index list), shown to differ from numeric mask order.
  3. `full<N>`, `of_grade<N, k>`, `merge`, and `index_of` are tested exhaustively for n up to 6 against the test-side definitions; sizes equal the binomial counts.
  4. Every rejection (unknown or repeated name, index above N, duplicate mask, n above 9 for compact names, a name with a bad leading letter or digit) has a compile-fail test on both compilers that requires the literal message, and the tests can fail (mutation shown).
  5. The set is a valid class-type template argument on both compilers, with equal values giving the same specialization.
  6. The tests carry reference markers and compare with the golden values where it covers the fact; the trace table rows 1.6 and 1.7 are set to yes only when a test states their facts; the trace gate passes.
  7. Lint, format, include scan, the citation check, and all existing tests pass on all seven presets.
- Approach: direct form; ordinary `constexpr` code over values (S7); no table is precomputed; sizes derived from the types (`numeric_limits`), no literal limits.
- Evidence: test output per preset, mutation runs, the trace gate output, and before and after build costs of the affected suites (W10).
- Agent cost: tracked for information only.
- Review: W5 blind review before the merge.
- Open questions: whether the parser reads names with a leading `e` only; whether `merge` must also accept sets with differing signs for one mask (the unit proposes and records).
