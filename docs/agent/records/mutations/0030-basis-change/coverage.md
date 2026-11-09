# WU12 coverage: the change of basis in DoD-10, DoD-12, and DoD-16

Written 2026-11-09 by Claude (agent, Opus 5.5, implementer of plan 0030 part C) on `feature/core-basis-change`, against the tests at `404f2fc` (part B's eleven commits cherry-picked, `basis_change.hpp` added, the header's example tested). It fills the three rows of plan 0029's coverage table (`../0029-identities/coverage.md`) that were marked "WU12", and lists the other criteria of plan 0030 beside them. Test cases are named by their `TEST_CASE` text and file. The mutants that judge these cells are in this directory's `README.md` (part E).

## Groups

Structure layer (V1 kind 1, every blade or pair of blades, from the change's generated terms at run time, `basis_change_identities_test.cpp`):

| Group | Changes |
| --- | --- |
| SL | every one of the 44 forms of `for_each_signature` against the exact diagonal counterpart that `test/support/diagonalize.hpp` gives it, P = T⁻¹ with T G Tᵀ = D |
| SN | `conformal_change<d>()` for d = 1 to 4, the Witt changes of one and two modes to R(m, m, 0), and rigid(3) to itself with e4 = 2 e′4 (det 2) |

Kernel layer (V1 kind 2, seeded random integer multivectors from −3 to 3 over the test rational, through `change_basis` and `change_basis_back`, `basis_change_checks.hpp`):

| Group | Changes | Layouts | Source |
| --- | --- | --- | --- |
| KC | `conformal_change<1>()`, `<2>()`; `<4>()` | every blade; vectors × bivectors and bivectors × bivectors | `basis_change_conformal_test.cpp` |
| KC3 | `conformal_change<3>()`, under the raised Clang step limit | every blade | `basis_change_conformal3_test.cpp` |
| KW | both Witt changes, the degenerate form [[0, 1, 0], [1, 0, 0], [0, 0, 0]] and rigid(3) with its null generator first against their diagonal counterparts, rigid(3) with e4 = 2 e′4 | every blade | `basis_change_witt_test.cpp` |
| KR | random forms 0, 1, 2, 4, 5, 6, 8, 9 against their diagonal counterparts | every blade | `basis_change_random_test.cpp` |
| KR5 | random forms 3 and 7 (n = 5, dense) | vectors and bivectors, in each pairing | `basis_change_random_five_test.cpp` |

## DoD-10: the three rows left to WU12

| Family | Structure | Kernel | Cases |
| --- | --- | --- | --- |
| Change of basis is an algebra isomorphism and round-trips | SL and SN: φ(e_A e_B) = φ(e_A) φ(e_B) on every pair, φ(1) = 1, every image of its blade's grade, the antiscalar to det(P) times the target's, and the library's inverse undoing φ on every blade both ways | KC, KC3, KW, KR, KR5: φ(x ⟑ y) = φ(x) ⟑ φ(y), φ(x ∧ y) = φ(x) ∧ φ(y), and truncate(φ⁻¹(φ(x))) = x both ways | "each form's change to its diagonal counterpart is an isomorphism", "the conformal, Witt, and doubled changes are isomorphisms" (`basis_change_identities_test.cpp`); "conformal_change is an isomorphism for d = 1, 2, and 4", "... for d = 3", "the Witt, degenerate, and rigid changes are isomorphisms", "the random forms' changes to their diagonals are isomorphisms", "the dense random forms' changes to their diagonals are isomorphisms" |
| Diagonal and null-basis results agree: what is preserved, with its factor | not at this layer: the statements are proved in research note 0026 (Theorems 3 and 4), and their factors rest on the antiscalar's image, which SL and SN check | KC, KC3, KW, KR, KR5: the dot, reverse, antireverse, grade involution (1), antiwedge (1/d), bulk and left bulk duals (d) for every change; where P⁻ᵀ G′ P⁻¹ = G (the conformal, Witt, rigid, and doubled changes), ⟇, antidot, both weight duals (1/d), compose (1/d), and transform of whole-grade objects by an even and by an odd versor, the projection of a vector onto a hyperplane, and the hyperplane's antiprojection onto the vector (1/d², the last three added for the blind review's N4 at `a83eec3` and `9cd86f0`); the doubled rigid(3) change is the one where 1/d and d differ for that family | "conformal_change preserves the operations it must, d = 1, 2, 4", "... d = 3", "the Witt, degenerate, and rigid changes preserve what they must", "the random forms' changes preserve the operations they must", "the dense random forms' changes to their diagonals are isomorphisms" (its agreement checks) |
| Diagonal and null-basis results agree: what is not preserved | hand values from the definitions | not needed: one value each tells the alternatives apart | "the bulk, the weight, and the complements depend on the basis" (conformal(3)); "the antiproduct family depends on the basis for a diagonalizing change" (random form 1 to diag(2, 1, −62)) (`basis_change_test.cpp`) |
| Test-side rational LDLᵀ for the random forms (plan 0001 section 5) | T G Tᵀ = D on every form of `for_each_signature`, each square squarefree within `max_form_entry`, T invertible, the radical's dimension | used by SL, KR, KR5 | "diagonalize gives T G T^T = D on every listed form", "a zero pivot is swapped with the next nonzero square", "two null vectors are paired, and the radical stays", "each square is scaled to the squarefree part of p q" (`support/diagonalize_test.cpp`) |

## The other criteria of plan 0030

| Criterion | Evidence | Cases or tests |
| --- | --- | --- |
| 1, checked at generation (DoD-12) | one compile-fail test per rejection, on GCC and Clang, each pinned by `tests.messages_pinned`; the concept's accepting and rejecting `static_assert`s | the 25 tests `core.basis_change_*` and `core.change_basis_*` of `test/CMakeLists.txt`, and since `0c5f0e0` a 26th, `core.basis_change_pairing_not_kept`, for the inner products off the diagonal (the blind review's F1); `basis_change_test.cpp`'s concept block |
| 2, blade images (1.4) | golden `basis_changes` (V1 kind 3) and hand values (kind 1) | "conformal_change maps every blade to its minors, both ways", "conformal_change in conformal(3) on hand values", "the origin's round trip and an oriented layout in conformal(3)", "changes with a determinant of 2 and of -1 on hand values", "the header's example: a conformal(3) point to R(4, 1, 0) and back" |
| 8, scalars (DoD-7) | exact equality for integer and dyadic cases, the rule of `records/numerical/wu12-change-basis-tolerances.md` otherwise | the four cases of `basis_change_scalars_test.cpp` (the integer case with `change_basis_back` since `cc923ac`, the blind review's F2); `core.change_basis_integer_division`, `core.change_basis_no_division`, `core.change_basis_back_integer_division` |
| 9, V8 (DoD-16) | published referents read under V9 (research note 0026 B) | "conformal_change sends the null pair to the published pair", "the Witt change sends the fermion operators to the Witt basis" (`basis_change_validation_test.cpp`); "The products of five algebras equal their matrix representations" (`blade_product_test.cpp`) |
