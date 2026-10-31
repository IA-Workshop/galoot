# 0005 WU3: metric

- Status: claimed; branch `feature/core-metric`, worktree `.claude/worktrees/core-metric`
- Goal: `include/galoot/core/metric.hpp`, the signature as data (C2, C5).
- Content: a symmetric integer Gram matrix over N generators held in a structural value, with the signature carried by a tag type that holds its value (decision 0021, STYLE S7); the largest N is a template parameter, not a constant (decision 0004), with 1 ≤ n ≤ 16 as the v1 default and n = 0 rejected; elements are `int`, not `int8_t` (note 0010); factories `R(p, q, r)`, `rigid(d)`, and `conformal(d)` (CONVENTIONS 1.2 to 1.4); a concept `metric_like` and accessors; no basis names inside the metric (architecture finding, plan 0001).
- Definition of done: the header exists, is self-contained, documented (S11), and the tests pin 1.2, 1.3, and 1.4.
- Acceptance criteria: (1) each factory's matrix equals the matrix in CONVENTIONS for n up to 6, written out in the test from the definition; (2) generation-time checks (V4) reject an asymmetric form, p+q+r ≠ N, an entry out of range, and n outside the bound, each with a compile-fail test on both compilers that requires the literal message (C9; the catch-all form of decision 0033 where a function is called); (3) one accept and one reject `static_assert` per concept (S7); (4) `is_diagonal` and the null-generator position are tested on rigid(3) and conformal(3); (5) conformal(3) differs from R(4,1,0) as a signature but the test pins the exact change of basis of 1.4 as data only (the conversion itself is WU12); (6) lint, format, and include scan pass; tests pass on both compilers.
- Approach: direct form; `consteval` factories returning the structural value; no dependence on `bits.hpp` unless needed.
- Evidence: test output per preset and the P1 before and after record.
- Agent cost: tracked for information only (decision 0019).
- Review: W5 blind review before the merge into `develop`.
- Open questions: whether the tag-and-value split costs more than Spike A measured for the real types; the unit reports it.
