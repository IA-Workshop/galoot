# WU14 coverage: DoD-10's identity families, DoD-11's conventions, DoD-19's catalogue

Written 2026-11-08 by Claude (agent, Opus 5.5, implementer of plan 0029 part C2) on `feature/core-identities`, against the tests at `cd623d7` (part B's commits cherry-picked, the trace rows, CONVENTIONS 1.16, the catalogue regenerated). Test cases are named by their `TEST_CASE` text and file, not by line, so that the table outlives edits. "WU12" marks a family left to the exact change of basis (decision 0216 item 1).

## Signature groups

Structure layer (V1 kind 1, over plans and blade products): S, every signature of `test/core/signatures.hpp`'s `for_each_signature`, which holds 28 named diagonal and conformal forms of up to six generators, the Witt forms of one and two modes, the degenerate non-diagonal form, rigid(3) with its null generator first, and ten seeded random symmetric forms.

Kernel layer (V1 kind 2, seeded random integer multivectors through the public functions), the groups of `test/core/identities_checks.hpp`:

| Group | Forms | Layouts |
| --- | --- | --- |
| K1 | 22 forms of one to three generators and 12 of four | every blade |
| K2 | R(4,1,0), R(4,0,1) | every blade |
| K3 | conformal(3), under the raised Clang step limit | every blade |
| K4 | two dense random forms of five | vectors, bivectors (per identity, as the cases say) |
| K5 | R(4,2,0), R(3,3,0), conformal(4) | vectors, bivectors |
| K6 | R(8,0,0), R(6,6,0), R(15,0,1), R(8,8,0) | sparse sets |
| K7 | R(0,3,2), R(0,2,3), drawn at random (research note 0024 B1) | every blade |
| K8 | R(1,3,2), R(1,1,4), drawn at random | vectors, bivectors |
| P | the basis-blade proof of K1, K2, K3, K7 on every blade, and of the others on their layouts (V1 kind 1 through `mv`) | one coefficient 1 |
| F | rigid(3), conformal(3) over `double`, within forward error bounds (V12) | as the case says |

## DoD-10, by family

Cases of `test/core/identities_test.cpp` are named by a short tag: AP "the products are associative with the scalar 1 as identity over many forms"; AA "the antiproducts are associative over many forms"; DM "the De Morgan laws hold on multivectors over many forms"; IP "the involutions follow their product rules over many forms"; IA "the involutions follow their antiproduct rules over many forms"; DR "vectors anticommute to twice their inner product over many forms"; BP "the laws hold on every pair and triple of basis blades over many forms". FR is "the identities hold over double within their forward error bounds" (`identities_rounding_test.cpp`); DD "the duals compose to det g over many forms" and DO "the dot and the antidot are symmetric De Morgan pairs over many forms" (`dual_identities_test.cpp`).

| Family | Structure (S) | Kernel K1 to K8 | Proof P | Floating F |
| --- | --- | --- | --- | --- |
| Associativity of ⟑, and 1 as its identity | "The product of blades is associative on every triple" (`blade_product_test.cpp`) | AP on K1 to K8 (K4 on vectors) | BP | FR |
| Associativity of ∧ | "The wedge plan is associative on every triple of blades" (`plan_test.cpp`) | AP on K1 to K8 (K4 on vectors) | BP | not in FR |
| Associativity of ⟇ and ∨, the antiscalar as identity | "The antiproducts are associative, with their identities and ..." (`plan_test.cpp`) | AA on K1 to K8 (K4 on vectors) | BP | FR (⟇) |
| The four De Morgan laws and their converses | "The complements turn each product into its antiproduct" (`plan_test.cpp`) | DM on K1 to K8 (K4 on bivectors) | BP | FR |
| left ∘ right = id, right ∘ right = (−1)^(gr ag) | `bits_test.cpp` complement cases (n ≤ 8); "The maps compose as the involutions and complements do" (`plan_test.cpp`, n ≤ 10) | "the involutions and complements satisfy their relations" (`unary_test.cpp`, n = 1 to 6 and sparse) | not needed: one blade at a time already | not in FR |
| u ∧ ū = 𝟙, u̲ ∧ u = 𝟙 | "u ∧ right(u) and left(u) ∧ u are the antiscalar, for every blade ..." (`bits_test.cpp`, n ≤ 8) | metric-independent; no kernel group needed | the structure test is exhaustive | not needed |
| Reverse and antireverse are involutions | "The maps compose as the involutions and complements do" | "the involutions and complements satisfy their relations" | as above | not needed |
| Product rules of the involutions over ⟑ and ∧, ⟇ and ∨, with (−1)^n over the antiproducts | "The antiproducts are associative, with their identities and ..." (reverse over ⟑, antireverse over ⟇) | IP and IA on K1 to K8 (K4 on bivectors); the sign (−1)^n told apart only on odd n (K1's forms of one and three, K3, K4, K7) | BP | not in FR |
| e_i e_j + e_j e_i = 2 g_ij | "e_i e_j + e_j e_i = 2 g_ij for every pair of generators" (`blade_product_test.cpp`) | DR on every group's vectors, g read from the algebra's metric | BP (every pair of generators) | not in FR |
| a ∧ b★ = (a • b)𝟙 | "a wedged with the dual of b is the dot times the antiscalar" (`dual_test.cpp`, five forms) | DO on K1, K2, K3 and every form of up to five, K4 and K5 on vectors and bivectors, K6, K7, K8 | not in BP | not in FR |
| G𝔾 = 𝔾G = det(g) I, the double duals | "the four duals of each basis blade are composed from G" (`dual_test.cpp`); the Python proofs of `records/spikes/dual-identities/check.py` | DD on every form of up to five, K4, K5, K6, K7, K8 | not in BP | not in FR |
| u★★ = 0 exactly when degenerate | "the double duals are zero exactly when the metric is degenerate" (`dual_test.cpp`) | DD | not in BP | not in FR |
| Pseudoscalar square (DoD-16) | "The pseudoscalar squares to (-1)^(n(n-1)/2) det g" (`blade_product_test.cpp`) | not needed | not needed | not needed |
| The bulk and weight in diagonal and conformal forms (3.2) | "bulk and weight in a diagonal form are the products of squares" (`dual_test.cpp`) | "the bulk and the weight in diagonal and conformal forms" (`dual_identities_test.cpp`), every diagonal form of up to six, K7, K8, K6, conformal(1) to conformal(4) | not needed | not needed |
| Transforms are λ times an outermorphism and compose | "transform is the sandwich of the tables" (`transform_test.cpp`) | "versors of hyperplanes act as outermorphisms and compose over ..." (`transform_identities_test.cpp`: forms of two to five, a dense form, R(8,0,0), rigid(7)) | not in BP | FR (composition) |
| Change of basis is an algebra isomorphism and round-trips | WU12 | WU12 | WU12 | WU12 |
| Diagonal and null-basis results agree | WU12 | WU12 | WU12 | WU12 |
| Test-side rational LDLᵀ for the random forms (plan 0001 section 5) | WU12 | WU12 | WU12 | WU12 |

The three kernel suites keep different form lists on purpose (plan 0029 section 5.2): the transform suite has rigid(2), rigid(4), and rigid(7) and not the drawn forms; the dual suite has the drawn forms. A shared list would make each suite pay the largest one's compile cost.

## DoD-11, by convention

- The trace gate at `cd623d7` passes; its only items without a test are 6.1 to 6.4, the cga3 preset's, after v1 (decision 0211). This unit added nine rows: three of 2.1 (the antigrade involution), two of 3.2 (the diagonal bulk and the blades in neither part), and three of 4.2 (the De Morgan law of ⟑, the associativity of all four products, the sign (−1)^n of the grade involution over the antiproducts).
- Golden values from `golden.py` (O2): the namespaces `duals` (3.3) and `mirrors` (4.4), compared with the library by "the four duals of every blade are the reference's images" (`dual_test.cpp`) and "a mirror maps every blade to the reference's image" (`transform_test.cpp`).
- Section 8 (CONVENTIONS 1.16): "Dual sign" names "the duals of a point are planes with the dual-sign convention", which fails under left duals, closing the M3 gate's m10; "Conformal duals" names "the bulk dual of a conformal sphere is a round point", which fails under the wiki's Duals table (research note 0024 D1, D2). The flector row is unchanged. "Partner of a circle" stays with cga3.
- Items stated as rules rather than values (4.5, 7.1, 7.2) keep their C++ tests of the rule, as O2 (b) chose.

## DoD-19, the catalogue

`tests.messages_pinned` passes on gcc-debug and clang-debug at `cd623d7`: 159 messages of `include/galoot/`, 153 of the core and 6 of the rga3 preset, 139 pinned by the same tests on both compilers, 20 internal with their reasons in `test/internal_messages.txt`, and no test that allows its message's longer twin. The record is `records/diagnostics/misuse-catalogue.md`, with `catalogue/scan-output.md`; every one of its 187 test names is in both listings.
