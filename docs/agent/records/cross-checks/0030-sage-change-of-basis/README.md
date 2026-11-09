# The change of basis against Sage's `lift_isometry`: a black-box cross-check (V1 kind 4)

- Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-09 UTC, for plan 0030 part A item (h), criterion 10, on `records/wu12-sage-cross-check` from `develop` at `2b733f4`.
- Library compared: the unit `feature/core-basis-change` at `37f1729`, read from its worktree and not modified.
- Kind of evidence: V1 kind 4, a comparison of outputs with another implementation. No Sage value is an expected value anywhere (C15, decision 0219 item 6), and nothing here enters `test/`, `include/`, or a comment of the library.
- Result: **0 mismatches** over 63 changes and 71,864 image entries, both directions; the library's inverses equal Sage's entry by entry; the four conformal tables of `golden.hpp` equal Sage's maps; every control found the difference it was built to find. Separately, **a GCC 16.2.1 wrong-code defect** came up while building the dump (section 5).

## 1. Departures, skipped checks, and ambiguities (C20)

1. **Where the comparison is recorded.** Criterion 10 and decision 0219 item 6 say research note 0026 records it, and criterion 10 says the dump program lives in math-tools. The brief for this item asked for a record under `docs/agent/records/` built from the library's actual outputs. Reading followed: this record, with the dump program, the Sage script, and their outputs committed beside it, so that every number here is reproducible from the repository (V10). Note 0026 lives on the unit branch, which this item does not touch; its line "Item (h) ... will be added here" still stands there, and a pointer to this record belongs in it when the unit merges.
2. **Criteria 1 to 9 passing is taken from the unit's records, not rerun.** Plan 0030's outcome and decision 0221 at `37f1729` report the unit's tests, mutants, and seed campaign as passing. I ran none of the unit's tests.
3. **More than criterion 10 lists.** Criterion 10 names conformal(1) to (3), both Witt changes, the degenerate form, rigid(3) with its null generator first, and the ten random forms. This adds conformal(4) and the rigid(3) change with e4 = 2 e'4 (det P = 2) on the public route, and every form of the exhaustive list (44, including R(p, q, r), the radical-off-the-axes form, and the all-ones form) on the terms route (section 2).
4. **"A det-2 change"** is read as the tests' rigid(3) change with e4 = 2 e'4, det P = 2. Several LDLᵀ changes also have |det P| ≠ 1: random 5 (2), random 9 (2/3), random 3 (2/7755), random 7 (5/18), random 2 (1/30).
5. **The planning probe's Sage-internal homomorphism check is not repeated.** It compares Sage with itself, not with the library. Products are not compared here; research note 0014 compared the product tables with Sage.
6. **Run-log rows discarded, and untimed scratch compiles (P13).** The first build of the dump program, as one translation unit, and the investigation builds that followed it (GCC at -O0, -O2, and with ASan and UBSan, each through `tools/timed`) went to this record's campaign log, which I reset before the final runs; those rows are not kept. Six reductions of the defect (single small sources, under a second each) and one -O0 build under gdb were compiled outside `tools/timed`, in scratch space. The committed reproducer's runs (section 5) are the evidence for the defect, and they are in `runs.tsv`.

## 2. What was compared

For a change P from an algebra with Gram matrix G to one with G′ (row i of P writes e_i in the e′_j, P G′ Pᵀ = G), the library sends e_A to Σ_B det P[A, B] e′_B. Sage computes its own map:

- `Cl = CliffordAlgebra(QuadraticForm(QQ, 2G))`, whose relation is e_i e_j + e_j e_i = 2 G_ij (the factor 2 is the known trap of math-tools' `sage-route-a.md`; a control below shows it bites).
- `phi = Cl.lift_isometry(m)` with m = Pᵀ: it sends e_i to Σ_j m[j, i] e′_j and extends it multiplicatively; its codomain is Cl(m⁻ᵀ G m⁻¹).
- Route A: Sage's basis is the ordered products e_i1 ⋯ e_ik, so each image is read in wedge coordinates by building the wedge monomials inside Sage and solving the unitriangular system (`sage-route-a.md`).
- The back change is `lift_isometry(m⁻¹)` from Cl(G′), whose codomain must be Cl(G).

No minor is computed on the Sage side. Per change, the script checks: P G′ Pᵀ = G; Sage's form is G (not G/2); Sage's codomain Gram matrix m⁻ᵀ G m⁻¹ equals the library's target G′; the back codomain is G; P⁻¹ from Sage equals the inverse the library computes (`detail::inverse_change`), entry by entry; and every coefficient of every blade's image, forward and back, equals the library's. For the named changes it also checks the library's P against P written from the definitions in the script (CONVENTIONS 1.4 for conformal(d), the scaled Witt change, diag(1, 1, 1, 2)); for the tags restated from the exhaustive list, the dump checks that each source form equals the listed one (`listed same`).

The library's outputs come from `dump_changes.cpp`, built against the unit's `include/` and `test/`, by two routes:

- **public**: each unit blade of `full<n>` over the test rational through `change_basis` and `change_basis_back`, as a user calls them. 19 changes: conformal(1) to (4) by `conformal_change`, the Witt changes of one and two modes, rigid(3) with e4 doubled, and the test diagonalization's changes for the degenerate form, rigid(3) null first, and random 0 to 9.
- **terms**: the per-blade images the library generates (`detail::blade_image`, over the product of the blade's row denominators) for the change and for its computed inverse, at run time, for all 44 forms of the exhaustive list against their diagonal counterparts. This is what the structure-layer identity test reads.

The diagonal counterparts and their changes come from the test-side LDLᵀ. The library's own check (P G′ Pᵀ = G at first use) and Sage's codomain check both validate them, so a wrong diagonalization would show here as a failed line, not as a silent agreement.

GCC 16.2.1 and Clang 23.1.4 give byte-identical dumps for both routes.

## 3. Results (`sage-out.txt`)

| Group | Changes | Entries compared (forward + back) | Mismatches |
| --- | --- | --- | --- |
| public: conformal(1) to (4) | 4 | 10,880 | 0 |
| public: Witt, one and two modes | 2 | 544 | 0 |
| public: rigid(3), e4 doubled (det P = 2) | 1 | 512 | 0 |
| public: degenerate, rigid(3) null first | 2 | 640 | 0 |
| public: random 0 to 9 (LDLᵀ) | 10 | 5,600 | 0 |
| terms: every form of the exhaustive list (LDLᵀ) | 44 | 42,808 | 0 |
| `golden.hpp` `basis_changes`, d = 1 to 4, against Sage | 8 tables | 10,880 | 0 |
| Total | 63 changes and 8 tables | 71,864 | 0 |

Every line also reports: the codomain Gram matrix equals the target's, the back codomain equals the source's, the inverse agrees on all n² entries, and det P. The hand values of plan 0030 criterion 2 agree with the dump: in conformal(3), e4 ↦ −½ e4′ + ½ e5′, e5 ↦ e4′ + e5′, e45 ↦ −e45′, and 𝟙 ↦ −𝟙′. For rigid(3) with e4 doubled, e4 ↦ 2 e4′ and 𝟙 ↦ 2 𝟙′, and back 𝟙′ ↦ ½ 𝟙.

Controls, each of which must find a difference:

| Control | Found |
| --- | --- |
| G passed in place of 2G (conformal(3)) | Sage's form differs from G |
| m = P in place of Pᵀ (conformal(3), Witt two modes, random 1, random 7) | a difference in 4 of 4 |
| The sign of e45's image flipped in the dump (conformal(3)) | exactly 1 mismatch |

No disagreement arose, so nothing needed investigating under C11.

What this shows: an implementation that shares no code and no method with Galoot's (Sage lifts the vector map multiplicatively; Galoot wedges rows into minors) produces the same map and the same inverse on every blade of every listed change, including degenerate forms, a change with det P = 2, and dense LDLᵀ changes of five generators with non-dyadic denominators. What it does not show: anything about the products, the duals, or the factors of what a change preserves (those rest on note 0026's proof and the unit's tests); anything about layouts other than `full<n>` over the test rational; or the floating-point scalars.

## 4. Environment and pins

- Sage: math-tools `toolset/env/py-sage` (not recreated), passagemath-modules 10.8.13, passagemath-categories 10.8.13, gmpy2 2.3.2, CPython 3.14.9; `uv.lock` sha256 `5396cb034418174f742908149d8c63a264fbdd7d42f6c6b087467ba8ab7d3b89`. `CliffordAlgebra.quadratic_form()` is public in this version, so the private `_quadratic_form` that the planning probe used is not needed.
- Compilers: g++ (GCC) 16.2.1 20260810 and clang 23.1.4, `-std=c++23 -O1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -D_GLIBCXX_ASSERTIONS`.
- Times (`runs.tsv`, the campaign log of this record): each dump build 0.6 to 1.9 s, each dump run under 0.04 s, the Sage comparison 1.4 s.

Commands, from this branch's worktree, with `U` the unit's worktree at `37f1729` and `B` a scratch build directory:

```
eval "$(tools/jobpool env)"
R=docs/agent/records/cross-checks/0030-sage-change-of-basis
F="-std=c++23 -O1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -D_GLIBCXX_ASSERTIONS -I $U/include -I $U/test $R/dump_changes.cpp"
tools/timed --log $R/runs.tsv build - dump-public-gcc -- g++ $F -o $B/public-gcc
tools/timed --log $R/runs.tsv build - dump-terms-gcc -- g++ -DDUMP_TERMS $F -o $B/terms-gcc
$B/public-gcc > $B/public.txt; $B/terms-gcc > $B/terms.txt; cat $B/public.txt $B/terms.txt > $R/library-dump.txt
tools/timed --log $R/runs.tsv check - sage_cross_check -- \
  /home/brianm/projects/utils/math-core/math-tools/toolset/env/py-sage/.venv/bin/python -I \
  $R/sage_cross_check.py $R/library-dump.txt $U/test/reference/golden.hpp > $R/sage-out.txt
```

The same with `clang++` gives the same dump. The committed `runs.tsv` rows of the dump builds and runs carry the unit's revision, since they ran from its worktree; the Sage row and the reproducer rows carry this branch's.

## 5. A GCC 16.2.1 wrong-code defect

Built as one translation unit, the dump program aborted under GCC ("rational: the denominator is zero") at the run-time diagonalization of R(3, 0, 0), at -O0, -O1, and -O2 and with ASan and UBSan silent; Clang's build ran to the end. In that unit the test diagonalization `diagonalize<3, int>` was also evaluated in constant expressions, for the tags of the degenerate form and of random 1, 5, and 9. Under gdb the run-time call's matrix `t`, value-initialized with `{}`, held two entries with denominator 0 where the default member initializer gives 1.

`gcc_constexpr_init_repro.cpp` reduces it to 44 lines with no Galoot header: a constexpr function value-initializes a 3 × 3 `std::array` of a class with default member initializers and stores three elements. Called at run time in a translation unit that also evaluates it in a constant expression, it returns two elements that are wrong. `repro-out.txt`:

| Compiler and options | Wrong elements |
| --- | --- |
| g++ -O0 and -O2, with the constant evaluation | 2 of 9 |
| g++ -O0 and -O2, without it | 0 |
| clang++ -O0 and -O2, either way | 0 |

During reduction, GCC also gave 2 wrong elements at `-std=c++20`, none with two stores in place of three, and none for a one-dimensional array; the source is standard C++, and the correct result is 9 elements with d = 1.

The dump program now builds each route as its own program, so neither one both constant-evaluates and run-time-calls the diagonalization, and its GCC output is byte-identical to Clang's. What this means beyond the dump is not established here:

- The unit's own tests appear not to meet the pattern. The only test source that evaluates the diagonalization in a constant expression, `diagonalize_test.cpp`, does so for two generators, which did not trigger it here. This is inferred from reading the sources, not tested.
- Library code is also constexpr and is called both ways in many test sources (a `static_assert` beside a run-time check), and the test rational has default member initializers. A miscompiled run-time path would usually fail a test against independent values, but it could pass silently where the wrong value happens to be what is compared. No scan for the pattern was made.

No report was sent to the compiler's maintainers.
