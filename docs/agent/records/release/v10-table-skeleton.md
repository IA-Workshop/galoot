# V10 table skeleton for the 1.0.0 release record (draft)

- Status: draft. Records only (decision 0240 item 11). Every result cell is empty on purpose: a cell is filled only by a run at the final release revision (DoD-18, DoD-29).
- Written: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`. Rows come from the V10 tables of the M3, M4, and M5 gate reports (`records/gates/2026-11-04-M3.md`, `2026-11-08-M4-audit.md`, `2026-11-10-M5-audit.md`), the item list of `records/convention-trace.md`, and DoD-10, DoD-16, DoD-17, DoD-22 to DoD-25 of plan 0001.
- V10 requires, for every result: whether verification was performed and whether it passed, whether validation was performed and whether it passed, and the revision, command, and seed. A result that lacks either is labeled unverified or unvalidated.

## Departures and unknowns, first (C20)

1. **Rows from the gates are not copied as results.** The prior tables are at `b504aa7`, `26c8a82`, and `1aa1e40`, not at the release revision, so every cell here is empty.
2. **Rows the gate tables do not have.** The M3 to M5 tables list only the items each gate touched. Items 1.1, 1.2, 1.3, 1.5, 1.6 (core part), 1.7, 2.1, 3.1, 4.1, 4.2 appear only as "pinned" in `convention-trace.md` and the M2 gate; no V10 row for them exists to copy. Their validation column is unknown until a note or earlier record is found.
3. **Sections 6.1 to 6.4** (cga3) have `Implemented: no` in the trace table; the cga3 preset ships after v1 (decision 0211). They are listed as "not in 1.0", not as failures.
4. **Not in any prior table**: the DoD-17 point-group and Denavit-Hartenberg cases appear only in the M4 table's prose ("as above"); they get their own rows here. The plan's DoD-16 and DoD-17 case lists were taken from plan 0001, not re-read from the research notes.
5. **Section 8 rows**: the errata-naming row is governance work for the owner (decision 0240 item 12) and is not yet in CONVENTIONS; its row is a placeholder.
6. **Performance rows** are track-only (C14); "performed" means recorded, never "passed".
7. **The command and seed column** is a template; the commands come from the M5 audit's preface (per-preset workflow, `tools/timed ... gate:all@<rev>`, the 50-seed campaign, `conventions_check.py` seed 20261031, `golden.py` and `clifford_reference.py` fixed seeds) and are to be re-run, not quoted.

## Header block (fill at release)

- Release revision: ____
- CONVENTIONS version implemented: ____ (1.17 at `1ed0e6a`)
- Presets run, each from a new empty tree: gcc-debug, gcc-release, gcc-asan, clang-debug, clang-release, clang-asan, lint, clang-measure: ____
- Commands: `cmake --preset P && cmake --build --preset P && ctest --preset P`, or `cmake --workflow --preset P`, each through `tools/timed ... gate:all@<rev>`: ____
- Seeds: doctest default 0; the identity-program campaign ____ to ____; `conventions_check.py` 20261031; fixed seeds of `golden.py` and `clifford_reference.py`: ____
- Also: trace gate ____, `format-check` ____, `tools/runlog check` ____, `tests.citations` ____.

## 1. Convention families

Columns: Verification (performed? passed? kinds per V1), Validation (performed? passed? referent), Tests, Command and seed.

| Item | Verification | Validation | Tests | Command, seed |
| --- | --- | --- | --- | --- |
| 1.1 generators, blades, masks | | | | |
| 1.2 diagonal R(p, q, r), null last | | | | |
| 1.3 rigid(d) | | | | |
| 1.4 conformal change (WU12) | | | | |
| 1.5 scalar and antiscalar | | | | |
| 1.6 oriented names | | | | |
| 1.7 result order | | | | |
| 1.8 what a change preserves | | | | |
| 2.1 reverse, antireverse, grade and antigrade involutions | | | | |
| 3.1 complements | | | | |
| 3.2 bulk and weight | | | | |
| 3.3 bulk and weight duals, left duals | | | | |
| 3.4 double duals of a degenerate metric | | | | |
| 4.1 gp, wedge, dot, antidot | | | | |
| 4.2 antiproduct, antiwedge, De Morgan | | | | |
| 4.3 project, antiproject | | | | |
| 4.4 transform, compose, parity rule | | | | |
| 4.5 `truncate`, `part`, checking mode | | | | |
| 5.1 to 5.3 point, line, plane layouts | | | | |
| 5.4 motor transform | | | | |
| 5.5 translator, rotor | | | | |
| 5.6 flector transform | | | | |
| 5.7 `rotation`, `translation` | | | | |
| 5.8 motor and flector component order | | | | |
| 6.1 to 6.4 conformal objects | not in 1.0 (cga3 after v1) | not in 1.0 | none | n/a |
| 7.1 operators | | | | |
| 7.2 printed form | | | | |

### Section 8, recorded disagreements

| Row | Verification | Validation | Tests | Command, seed |
| --- | --- | --- | --- | --- |
| Flector transform (both forms told apart) | | The Terathon side not tested (C15, C16): confirm at release | | |
| Dual sign | | The Terathon side not tested: confirm | | |
| Partner of a circle | not implemented (6.4) | | | |
| Conformal duals (confirmed by the owner 2026-11-09) | | | | |
| Errata-naming row (owner to add, decision 0240 item 12) | | | | |

## 2. Identity families (DoD-10)

| Family | Verification | Tests | Seed |
| --- | --- | --- | --- |
| Associativity of gp, antigp, wedge, antiwedge | | | |
| The four De Morgan laws | | | |
| left∘right = id | | | |
| u ∧ ū = 𝟙 | | | |
| Reverse and antireverse involutions, product rule | | | |
| e_i e_j + e_j e_i = 2 g_ij | | | |
| a ∧ b★ = (a • b)𝟙 | | | |
| G𝔾 = det(g)·I | | | |
| u★★ = 0 exactly when degenerate | | | |
| Change of basis: algebra isomorphism and round trip | | | |
| Diagonal and null-basis results agree | | | |
| Vector-space and module laws; distributivity | | | |
| Composition identities of decision 0172 (`records/spikes/dual-identities/check.py`) | | | none (exhaustive) |
| Floating identities within the tolerance rule (`core.identities_rounding`) | | | |
| Seeded identity campaign, six randomized programs | | | |
| Sparse use at n = 8, 12, 16 | | | |

## 3. Other verification claims

| Claim | Verification | Validation | Tests, command |
| --- | --- | --- | --- |
| V4 generation checks, each with a rejection test (DoD-12) | | n/a | |
| Misuse catalogue on both compilers, messages pinned (DoD-19) | | n/a | |
| Scalars: int64, test rational, float, double, `std::complex<double>` (DoD-7) | | n/a | |
| Rounding tolerances derived before the result (DoD-9) | | | |
| Fault injection: flipped reorder sign, flipped complement sign, dropped term, wrong coefficient, swapped 1.7 order (DoD-14) | | n/a | |
| Mutation records (`records/mutations/`) | | n/a | |
| `-fno-exceptions` bare compiles (DoD-21, decision 0240 item 4) | | n/a | |
| ASan and UBSan; TSan recorded as not applicable (DoD-26) | | n/a | |
| Include scan, every header alone, bare user file (DoD-6) | | n/a | |
| README examples compiled and run on both compilers (DoD-27) | | n/a | |
| Kernel initializers canaries (GCC workaround) | | n/a | |
| Zero diff under `include/galoot/core/` when the model and test-only models are added (DoD-5) | | n/a | |

## 4. V8 cases

### Core (DoD-16)

| Case | Verification | Validation (referent, read under V9) | Test |
| --- | --- | --- | --- |
| Cl(0,1) ≅ ℂ | | | |
| Cl(0,2) ≅ ℍ | | | |
| Cl(1,1) ≅ M₂(ℝ) | | | |
| Cl(3,0), Pauli | | | |
| Cl(1,3), Dirac | | | |
| R(0,0,n): gp equals wedge | | | |
| Witt-basis CAR relations | | | |
| Grade counts C(n, k) | | | |
| Pseudoscalar square from the research note derivation | | | |
| conformal(d) ≅ R(d+1, 1, 0) | | | |

### Model `rga3` (DoD-17)

| Case | Verification | Validation | Test |
| --- | --- | --- | --- |
| Every Checked value of CONVENTIONS section 5 | | | |
| Point groups closed under compose, orders from the International Tables (families realizable over ℚ; others deferred or run under V12) | | | |
| Denavit-Hartenberg chains against independent 4×4 matrices | | | |
| Incidence | | | |
| Rotors with rational half angles against Rodrigues' matrices | | | |

State, beside each validation cell, whether the referent was read directly or through a secondary account (V9), as the M4 and M5 tables do.

## 5. Performance claims (track-only, C14)

"Performed" means recorded; nothing here passes or fails.

| Claim | Status | Source, command |
| --- | --- | --- |
| DoD-22: per-TU wall time and peak memory, both compilers, release revision; before and after for each core unit | | |
| DoD-23: growth benchmark, n in {4, 8, 16} at fixed use; 1, 4, 16 distinct operations at fixed n; instruction counts beside wall time | | |
| DoD-23: the exceptions recorded for growth (antiproduct, `bulk_dual`) | | |
| DoD-24: operation counts equal the reference script's collected-term counts, five rga3 operations | | |
| DoD-24: `-O2` assembly read, no blade branches, calls, or allocation | | |
| DoD-24: pinned benchmark against the in-repo straight-line form | | |
| DoD-25: performance history baseline committed; `runs.tsv` rows kept through compaction | | |
| Limits (largest `full<N>` per operation and compiler) | | |
| Header and umbrella parse cost (includes the `basis_change.hpp` measurement, decision 0240 item 6) | | |
| Build cost of the inlining pass, before and after (W10, debug and release trees) | | |
