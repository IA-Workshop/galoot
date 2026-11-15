# V10 table of the 1.0.0 release

- Status: filled 2026-11-12 by Claude (agent, Opus 5.5), planner, plan 0042 unit U9, on branch `feature/release-1.0.0`, from the release matrix (`records/gates/2026-11-12-release-matrix.md`, unit U8) and the records each row names. Records only. This file was the skeleton `v10-table-skeleton.md` (2026-11-10, Sonnet 5.5, at `develop` `1ed0e6a`; header brought up to decision 0248 by U6); it is renamed now that every cell is filled. Updated 2026-11-12 by Claude (agent, Opus 5.5), plan 0042 unit U11, from the release audit's m5 and m8 (departure 6 and the CONVENTIONS line of the header block). The DoD of plan 0001, criterion by criterion, is in `dod-1-to-29.md`.
- V10 requires, for every result: whether verification was performed and whether it passed, whether validation was performed and whether it passed, and the revision, command and seed. A result that lacks either is labeled unverified or unvalidated.

## Departures, unknowns, and readings, first (C20)

1. **The runs name `7276f82`, not R.** The release commit R is `7580e88`. U8 ran at the branch tip `7276f82`, which adds one run row to R, so every run row and compile row says `7276f826533f`. `git diff --stat 7580e88 7276f82 -- . ':!docs/agent'` prints nothing, so the code, tests, tools and build files that ran are R's. "At R" below means exactly this.
2. **No failure was found.** Every preset, check, seed and benchmark of the matrix exited 0. No cell below records a failure, and none was hidden: the matrix record lists each step with its exit.
3. **Validation is restated, not redone.** The matrix verifies; it does not validate. A validation cell names the referent and the record where the comparison was made and read (V9), from CONVENTIONS' own labels (Checked, Confirmed), research notes 0005, 0021, 0023, 0024 and 0026, and the M3, M4 and M5 gate tables. "Checked" in CONVENTIONS is verification by `conventions_check.py`, which shares no code with the library; "Confirmed" is a reading of Lengyel's wikis or posters (note 0005). A referent was not re-read for 1.0.
4. **Items with no label.** 1.1, 1.2, 1.7, 4.4, 4.5, 7.1 and 7.2 carry no Checked or Confirmed label in CONVENTIONS 1.17. 1.1, 1.2, 1.7, 4.5, 7.1 and 7.2 are definitions or interface conventions, so their validation is "not applicable". 4.4's validation comes through 5.4 and is partial (its row).
5. **Recorded results not rerun at R.** Fault injection (DoD-14), the mutation records, DoD-23's growth grid, DoD-24's assembly read and pinned benchmark, and the core V8 cross-checks in Sage ran at earlier revisions. Each row names its revision and date and says "not rerun". The tests that pin their claims ran at R.
6. **Two runs after R were not timed.** After the README's rga3 sentence was reworded at `1cd7ac8` (decision 0249 item 2), the README, documentation-page and citation tests ran in the gcc-debug and clang-debug trees directly, not through `tools/timed`: the job pool had stopped itself (it stops after 30 minutes without a client), `tools/timed` refuses a build or test step without it, and this unit's brief allows no pool change. Those direct runs have no row in `runs.tsv`; their results are in the release record, "Evidence". The same trees' documentation tests were rerun through `tools/timed` at `8118033`, whose README is `1cd7ac8`'s (rows 21958 and 21959, `gate:docs@8118033`, gcc-debug and clang-debug, exit 0; the command is not recorded), and by the release audit at `18eb677` with `-R "readme|docs_pages|citations"` (rows 21969 to 21971, 11 of 11 on each compiler). The release record's departure 3 has the detail.

## Header block

- Release commit R: `7580e88` (2026-11-12T02:23:01Z). Runs at `7276f82`, code of R (departure 1). The tag names the records tip T (release record, "Release").
- CONVENTIONS version implemented: 1.17 at R and at the U9 tip. It becomes 1.18 when the owner adds the errata-naming row and rewords line 12 before the tag (decisions 0248 item 2 and 0250 item 5; the text is `conventions-1.18-owner-text.md`). The trace gate then reruns on that commit, with `tools/runlog check` for its row; `conventions_check.py` does not, since it reads no copy of CONVENTIONS.md (release record, "The tip T", rule 4).
- Presets, each from a new empty tree, all passed: gcc-debug, clang-debug, gcc-release, clang-release, clang-asan and lint 657 of 657; gcc-asan, gcc-measure, gcc-measure-o3, clang-measure and clang-measure-o3 649 of 649, each under its test preset's filter, which leaves out the labels `runlog` and `bench`; the counts equal gate 3's. `runs.tsv` rows 21905 to 21937, 2026-11-12 from 02:28:25 to 03:16:56 UTC, 2,883.7 s in all.
- Commands, from the worktree root with `eval "$(tools/jobpool env)"`, for each preset P: `tools/timed configure P gate:all@7580e88 -- cmake --preset P`, then `tools/timed build P gate:all@7580e88 -- cmake --build --preset P`, then `tools/timed test P gate:all@7580e88 -- ctest --preset P`. Below, **matrix** means these runs.
- Seeds: doctest's default, 0, in every preset (no preset passes `--rand-seed`); the identity campaign 1 to 50 for six programs on gcc-debug, 300 of 300 passed (`records/gates/2026-11-12-release-seeds.tsv`), below **campaign**; `conventions_check.py` 20261031 (row 21939), below **cc**; the fixed seeds of `golden.py` and `clifford_reference.py`, checked by `reference.golden` in every preset; the benchmarks 20261110.
- Also at `7276f82`, each exit 0: trace gate (row 21938; items with no test 6.1 to 6.4 only); `format-check` (row 21940, and inside the lint build); `tools/runlog check` (row 21949); `comments_test.py`, `cost.py comments` on `plan.hpp` and on the limits page, `cost.py selfcheck` on both compilers' data, `jobserver_check.py` (rows 21941 to 21946); `tests.citations` and `tests.citations_guide` in every preset's suite. After R, on records only: `tools/runlog check` and the trace gate at `4e7d4aa` (rows 21950, 21951); the trace gate at `eea9132` (row 21952); decision 0172's composition check at `1cd7ac8` (row 21953, marked dirty by the snippets edits then uncommitted, which the check does not read); and the closing gates at the U9 tip (release record, "Evidence").
- DoD-22's rows: shared mode, with the P14 note of the release record (decision 0248 item 5).
- Tests are named by CTest name or label; the trace gate's output at R lists each item's test cases by file and line.

## 1. Convention families

| Item | Verification | Validation | Tests | Command, seed |
| --- | --- | --- | --- | --- |
| 1.1 generators, blades, masks | Performed, passed at R | Not applicable: a definition of the representation (no label) | `core.bits`, `core.blade_set`, `core.blade_product`, `core.mv` | matrix, 0 |
| 1.2 diagonal R(p, q, r), null last | Performed, passed at R | Not applicable: a definition (no label) | `core.metric`, `core.dual` | matrix, 0 |
| 1.3 rigid(d) | Performed, passed at R | Performed, passed: Confirmed (note 0005) | `core.metric` | matrix, 0 |
| 1.4 conformal change (WU12) | Performed, passed at R: every blade image against golden `basis_changes` and hand values, d = 1 to 4 and back; Checked | Performed, passed: Confirmed for d = 3 against Lengyel's conformal wiki, Colapinto eqs. 2.39 to 2.42 and Hestenes eqs. 1 to 7, read and recomputed (note 0026 B1 to B3; M5 gate) | `core.metric`, `core.basis_change` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_basis_change_test`) |
| 1.5 scalar and antiscalar | Performed, passed at R | Performed, passed: Confirmed (note 0005) | `core.bits`, `core.product` | matrix, 0 |
| 1.6 oriented names | Performed, passed at R; Checked | Performed, passed: Confirmed (note 0005); the rga3 names against the Motor and Flector pages (note 0023) | `core.blade_set`, `core.dual`, `core.mv`, `core.plan`, `core.product`, `core.unary`, `rga3.objects` | matrix, 0; cc, 20261031 |
| 1.7 result order | Performed, passed at R; also a V4 check at generation (section 3) | Not applicable: a convention of order (no label) | `core.blade_product`, `core.blade_set`, `core.format`, `core.mv`, `core.plan`, `core.unary` | matrix, 0 |
| 1.8 what a change preserves | Performed, passed at R: the factors on seeded multivectors over every group; Checked on eleven changes; the general statements proved in note 0026 A1 to A5 | Not applicable: a theorem about the specification's own mathematics (M5 gate) | `core.basis_change`, `core.basis_change_identities` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_basis_change_test`, `core_basis_change_identities_test`) |
| 2.1 reverse, antireverse, grade and antigrade involutions | Performed, passed at R; the relations Checked | Partly: the reverse and antireverse Confirmed (note 0005); the grade involution (5.6) and the antigrade involution (4.4) are Proposed, not validated | `core.bits`, `core.plan`, `core.rounding`, `core.transform`, `core.unary`, `core.identities` | matrix, 0; cc, 20261031 |
| 3.1 complements | Performed, passed at R; Checked | Not performed separately: labeled Checked only | `core.bits`, `core.dual`, `core.mv`, `core.plan`, `core.product`, `core.rounding`, `core.transform`, `core.unary` | matrix, 0; cc, 20261031 |
| 3.2 bulk and weight | Performed, passed at R: every blade of the five golden forms against golden G; the diagonal case Checked | Performed, passed for rigid(3) (note 0005 B6) and conformal(3), weight = −bulk (C1); not performed for other non-diagonal forms, no published table (M3 gate) | `core.dual`, `core.rounding`, `core.dual_identities` | matrix, 0; cc, 20261031 |
| 3.3 bulk and weight duals, left duals | Performed, passed at R; Checked | Performed, passed for rigid(3) against Lengyel's complement table (note 0005); section 8's dual-sign and conformal-duals rows below | `core.dual`, `core.dual_identities` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_dual_identities_test`) |
| 3.4 double duals of a degenerate metric | Performed, passed at R, by value in rigid(3), R(0,0,3), R(2,0,2) and a degenerate non-diagonal form; Checked for rigid(3) | Performed: Confirmed (note 0005); not compared separately (M3 gate) | `core.dual`, `core.dual_identities` | matrix, 0; cc, 20261031 |
| 4.1 gp, wedge, dot, antidot | Performed, passed at R | Performed, passed: Confirmed; products and wedges against an independent reference (M2 gate); P • P = −r² for a round point (note 0005 C2); the core V8 matrices (section 4) | `core.blade_product`, `core.plan`, `core.product`, `core.dual`, `core.identities`, `core.mv`, `core.rounding`, `core.transform` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_identities_test`) |
| 4.2 antiproduct, antiwedge, De Morgan | Performed, passed at R; Checked | Not validated for non-diagonal forms: checked against a composition built on their own definition (decision 0163 item 3; release record, exception 1) | `core.identities`, `core.identities_rounding`, `core.plan` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_identities_test`, `core_identities_rounding_test`) |
| 4.3 project, antiproject | Performed, passed at R; Checked | Performed, passed: Confirmed against the Projections page, read directly (note 0021) | `core.dual` | matrix, 0; cc, 20261031 |
| 4.4 transform, compose, parity rule | Performed, passed at R: against the sandwich built from golden tables on five forms, exact; the odd sign for n = 3, 4, 5 by cc; the declared-output check of DoD-4 (section 3) | Partly: the even case through 5.4 (Confirmed); not validated for odd n or non-diagonal forms (M4 gate) | `core.transform`, `core.transform_identities`, `core.transform_counts`, `core.identities_rounding`, `rga3.op_counts_checking`, `rga3.op_counts_unchecked` | matrix, 0; cc, 20261031; campaign 1 to 50 (`core_transform_identities_test`) |
| 4.5 `truncate`, `part`, checking mode | Performed, passed at R in checking builds; the rounding check's bound derived before the tests (decisions 0180, 0243) | Not applicable: an interface convention; the bound is a theorem about the computation | `core.mv`, `core.transform_rounding`, `core.mv_truncate_nonzero_*` | matrix, 0 |
| 5.1 to 5.3 point, line, plane layouts | Performed, passed at R: `static_assert`s over `std::int64_t` and the test rational; seeded golden joins and meets; 5.2 and 5.3 Checked | Performed, passed: 5.1 Confirmed; the layouts against Lengyel's Motor and Flector pages, read directly (note 0023 M1, F1); meets only up to sign (note 0023, "Not verified" item 5) | `rga3.objects`, `rga3.motions`, `rga3.incidence` | matrix, 0; cc, 20261031 |
| 5.4 motor transform | Performed, passed at R; Checked | Performed: Confirmed except the order of composition, which Lengyel's pages do not state; the chains against a secondary account (note 0023 J2) | `core.transform`, `rga3.objects`, `rga3.chains` | matrix, 0; cc, 20261031 |
| 5.5 translator, rotor | Performed, passed at R; Checked for l the z axis | Performed, passed: Confirmed | `core.transform`, `rga3.motions`, `rga3.objects` | matrix, 0; cc, 20261031 |
| 5.6 flector transform | Performed, passed at R; Checked for a plane and a plane composed with a motor | Performed: point-group orders against Hestenes' Table 4, a primary source (note 0023 G5); section 8's flector row below | `core.transform`, `rga3.objects`, `rga3.point_groups` | matrix, 0; cc, 20261031 |
| 5.7 `rotation`, `translation` | Performed, passed at R: bit for bit against l sin(θ/2) + 𝟙 cos(θ/2) after the division by \|v\|; the quarter-turn tolerances derived before the test (decision 0201) | Partly: rotors with rational half angles against Rodrigues' matrices; `rotation` not validated beyond the derived case or off glibc 2.44 (the machine's glibc at R is 2.44) | `rga3.motions`, `rga3.rotation_*`, `rga3.translation_*` | matrix, 0 |
| 5.8 motor and flector component order | Performed, passed at R | Performed, passed: Confirmed against the Motor and Flector pages (note 0023); the coefficient letters disagree with the Errata page (section 8's errata row) | `rga3.objects`, `rga3.format` | matrix, 0 |
| 6.1 to 6.4 conformal objects | Not in 1.0 (cga3 after v1, decision 0211); the trace gate's only items with no test | Not in 1.0 | none | n/a |
| 7.1 operators | Performed, passed at R | Not applicable: an interface convention | `core.product`, `core.unary`, `core.linear` | matrix, 0 |
| 7.2 printed form | Performed, passed at R | Not applicable: an interface convention | `core.format`, `rga3.format` | matrix, 0 |

### Section 8, recorded disagreements

| Row | Verification | Validation | Tests | Command, seed |
| --- | --- | --- | --- | --- |
| Flector transform (both forms told apart) | Performed, passed at R: the single-sign form gives the weight −1 and the negated plane, 5.6's form does not | The Terathon side is not tested and cannot be under C15 and C16 (no Terathon source); the row stands as recorded | `check_versors` (cc); "a reflected point keeps a positive weight" (`core.transform`); "the rga3 Flector e412 reflects ..." (`rga3.objects`) | matrix, 0; cc, 20261031 |
| Dual sign | Performed, passed at R: right against left duals at odd grades, n = 4 | The Terathon side is not tested, as above | `check_identities` (cc); "the duals of a point are planes with the dual-sign convention" (`core.dual`) | matrix, 0; cc, 20261031 |
| Partner of a circle | Not implemented (6.4, cga3 after v1) | n/a | "To be written" | n/a |
| Conformal duals (confirmed by the owner 2026-11-09) | Performed, passed at R | Performed: the Duals table's side tested by hand value (note 0024 D2); the wiki's Duals page read directly at revid 151 | "the bulk dual of a conformal sphere is a round point" (`core.dual`) | matrix, 0 |
| Errata-naming row | Not in CONVENTIONS 1.17. If the owner adds it as 1.18 (decision 0248 item 2, the text of the M7 audit's MA1), the tests it names passed at R | The Errata page was read in a copy; the book is unread (secondary, V9) | "the rga3 layouts hold the oriented blades of their names" (`rga3.objects`); "an rga3 Motor prints on e41, e42, e43, e1234, e23, e31, e12, 1" (`rga3.format`) | matrix, 0 |

## 2. Identity families (DoD-10)

Verification only: these are identities of the specification's own mathematics, so validation does not apply (plan 0029 section 10).

| Family | Verification | Tests | Seed |
| --- | --- | --- | --- |
| Associativity of gp, antigp, wedge, antiwedge | Performed, passed at R | `core.identities`, `core.product` | 0; campaign 1 to 50 |
| The four De Morgan laws | Performed, passed at R | `core.identities`, `core.plan` | 0; campaign 1 to 50 |
| left∘right = id | Performed, passed at R | `core.bits`, `core.unary` | 0 |
| u ∧ ū = 𝟙 | Performed, passed at R; also a V4 check at generation | `core.bits`, `core.dual` | 0 |
| Reverse and antireverse involutions, product rule | Performed, passed at R | `core.identities`, `core.product`, `core.unary` | 0; campaign 1 to 50 |
| e_i e_j + e_j e_i = 2 g_ij | Performed, passed at R | `core.blade_product`, `core.identities` | 0; campaign 1 to 50 |
| a ∧ b★ = (a • b)𝟙 | Performed, passed at R | `core.dual`, `core.dual_identities` | 0; campaign 1 to 50 |
| G𝔾 = det(g)·I | Performed, passed at R, with sparse n = 8, 12, 16 | `core.dual_identities` | 0; campaign 1 to 50 |
| u★★ = 0 exactly when degenerate | Performed, passed at R | `core.dual`, `core.dual_identities` | 0; campaign 1 to 50 |
| Change of basis: algebra isomorphism and round trip | Performed, passed at R: structure layer over all 44 forms of `for_each_signature`; kernel layer on seeded multivectors. The Sage cross-check (Route A, 0 mismatches over 63 changes) is recorded at M5, not rerun | `core.basis_change`, `core.basis_change_identities` | 0; campaign 1 to 50 |
| Diagonal and null-basis results agree | Performed, passed at R, with the factors of 1.8 | `core.basis_change_identities` | 0; campaign 1 to 50 |
| Vector-space and module laws; distributivity | Performed, passed at R | `core.linear` | 0 |
| Composition identities of decision 0172 | Performed, passed after R: "total 17878 checks, all checks passed" at `1cd7ac8` (row 21953, `-dirty` from uncommitted snippets edits the script does not read). The script reads no code under `include/` (proofs over its own model) | `python3 -I -B docs/agent/records/spikes/dual-identities/check.py .` | none (exhaustive) |
| Floating identities within the tolerance rule | Performed, passed at R, against the rule derived before the tests (`records/numerical/wu14-identities-tolerances.md`) | `core.identities_rounding` | 0; campaign 1 to 50 |
| Seeded identity campaign, six randomized programs | Performed, passed at R: 300 of 300. Median wall of `core_identities_test` 21.8 s against M5's 16.2 s, not compared (shared mode both times, and the program changed since `1aa1e40`; P14) | `core_identities_test`, `core_dual_identities_test`, `core_transform_identities_test`, `core_identities_rounding_test`, `core_basis_change_test`, `core_basis_change_identities_test`, each `--rand-seed=<k>` through `tools/timed --log` | 1 to 50 |
| Sparse use at n = 8, 12, 16 | Performed, passed at R | `core.dual_identities`, `core.identities` | 0 |

## 3. Other verification claims

Validation does not apply to any row here: each is a property of the code, not a value with a referent.

| Claim | Verification | Tests, command |
| --- | --- | --- |
| V4 generation checks, each with a rejection test (DoD-12) | Performed, passed at R: each rejection is a compile-fail test, run on both compilers in every preset, its literal message pinned | the compile-fail tests of `test/CMakeLists.txt` (`*_fail.cpp`); `tests.messages_pinned`, `tests.compile_fail_flags`; matrix, 0 |
| Misuse catalogue on both compilers, messages pinned (DoD-19) | Performed, passed at R | `tools.catalogue`, `tests.messages_pinned`; matrix, 0 |
| Scalars: int64, test rational, float, double, `std::complex<double>` (DoD-7) | Performed, passed at R: the five plans equal the golden composition by `==`, both sides of each exactness bound; an inadequate scalar rejected with a message | `core.plan`, `core.mv`, `core.product`, `core.basis_change`, `support.rational`; matrix, 0 |
| Rounding tolerances derived before the result (DoD-9) | Performed, passed at R: each tolerance is a named constant from a derivation recorded before its test (`records/numerical/`: `wu12-change-basis-tolerances`, `wu14-identities-tolerances`, `rotation-tolerance`, `rotation-normalize-tolerance`, `0040-contraction-check`). Every preset builds with `-ffp-contract=off` | `core.rounding`, `core.transform_rounding`, `core.identities_rounding`, `rga3.motions`, `support.tracked`; matrix, 0 |
| Fault injection: flipped reorder sign, flipped complement sign, dropped term, wrong coefficient, swapped 1.7 order (DoD-14) | Recorded, not rerun at R: all six mutants of `records/mutations/0029-identities/faults.toml` killed on gcc-debug and clang-debug at `d5ffafd` (2026-11-08). The marker rule of DoD-14's first sentence passed at R | `tests.reference_markers`, `tests.golden_facts`; matrix, 0 |
| Mutation records (`records/mutations/`) | Recorded, not rerun at R: 35 records, the newest `0040-pre-release-core-pass` and `0041-documentation-lane`. The tool's own tests passed at R | `tools.mutate`; matrix, 0 |
| `-fno-exceptions` bare compiles (DoD-21, decision 0240 item 4) | Performed, passed at R. The `[[nodiscard]]` half of DoD-21 is not checked by a test and was not re-read by this unit | `core.bare_compile_no_exceptions`, `core.bare_compile_format_no_exceptions`, `rga3.bare_compile_no_exceptions`; matrix, 0 |
| ASan and UBSan; TSan not applicable (DoD-26) | Performed, passed at R: gcc-asan 649 of 649, clang-asan 657 of 657. TSan: no test runs code in more than one thread (decision 0031). clang-format and clang-tidy: the lint build printed no diagnostic and `format-check` passed; the S4 warnings are errors in every preset | gcc-asan, clang-asan and lint presets; `gate:format-check@7580e88` (row 21940) |
| Include scan, every header alone, bare user file (DoD-6) | Performed, passed at R | `core.include_scan`, `rga3.include_scan`, `tools.include_scan_rejects`, `core.bare_compile`, `rga3.bare_compile`; `all_verify_interface_header_sets` in the lint build; matrix, 0 |
| README and guide examples compiled and run on both compilers (DoD-27) | Performed, passed at R in every preset; the README's tests passed again at `1cd7ac8`, after the rga3 sentence, in gcc-debug and clang-debug (departure 6) | `rga3.readme_has_example`, `rga3.readme_example_1` to `_3`, `rga3.readme_cmake_1` and `_2`; the 56 tests whose label matches `docs`; `tests.docs_pages`; matrix, 0; after `1cd7ac8`, rows 21958, 21959 and 21969 to 21971 |
| Kernel initializers canaries (GCC workaround) | Performed, passed at R | `core.mv`; matrix, 0 |
| Zero diff under `include/galoot/core/` when the model and test-only models are added (DoD-5) | Recorded at the M4 gate (`26c8a82`): the three model merges touched only `include/galoot/rga3*` and `include/galoot/presets/`. Not re-derived at R; the preset and the test rational build and pass at R in every preset | M4 gate report, exit table; matrix, 0 |
| DoD-2 inventory exactly the public interface | Re-grepped at `a73cd4e` (U6). `git diff --stat a73cd4e 7580e88 -- include/` prints nothing, so the inventory holds at R. Two departures stand (release record, "DoD-2") | `records/release/dod-2-inventory.md` |
| DoD-4, repeated inputs: the declared-output check (decision 0239) | Performed, passed at R in every preset | `core.transform_nonzero_int64`, `core.transform_nonzero_rational`, `core.transform_consteval_nonzero`, `core.transform_consteval_nonzero_rational`, `core.transform_checking_recorded`, the `core.transform_unchecked_*` twins, `core.transform_rounding`; matrix, 0 |

## 4. V8 cases

### Core (DoD-16)

| Case | Verification | Validation (referent, read under V9) | Test |
| --- | --- | --- | --- |
| Cl(0,1) ≅ ℂ | Performed, passed at R | Performed, passed against `golden.py`'s products of the published matrices, faithful by rank; the matrices read in Wikipedia only (secondary) | "The products of five algebras equal their matrix representations" (`core.blade_product`) |
| Cl(0,2) ≅ ℍ | Performed, passed at R | As above (secondary) | as above |
| Cl(1,1) ≅ M₂(ℝ) | Performed, passed at R | As above (secondary) | as above |
| Cl(3,0), Pauli | Performed, passed at R | As above (secondary) | as above |
| Cl(1,3), Dirac | Performed, passed at R | As above (secondary) | as above |
| R(0,0,n): gp equals wedge | Performed, passed at R | Not applicable: a consequence of the definition | `core.blade_product` |
| Witt-basis CAR relations | Performed, passed at R | Performed, passed against Doran, Hestenes, Sommen and Van Acker (primary; its eq. 3.34 found wrong in sign and recorded) and Wikipedia's C*-algebra form (secondary) (note 0026 section B) | "the Witt change sends the fermion operators to the Witt basis" (`core.basis_change`) |
| Grade counts C(n, k) | Performed, passed at R | Not applicable: a count | `core.blade_set` |
| Pseudoscalar square from the research note derivation | Performed, passed at R | Not applicable: a derivation of the specification | `core.blade_product` |
| conformal(d) ≅ R(d+1, 1, 0) | Performed, passed at R | Performed, passed against Lengyel's conformal wiki, Colapinto's thesis and Hestenes' "Old Wine in New Bottles" (primary; Hestenes eq. 7 found wrong in sign and recorded) (note 0026 section B) | "conformal_change sends the null pair to the published pair" (`core.basis_change`) |

### Preset `rga3` (DoD-17)

| Case | Verification | Validation | Test |
| --- | --- | --- | --- |
| Every Checked value of CONVENTIONS section 5 | Performed, passed at R | The Checked values are verification by an independent script; the Confirmed ones are in section 1 | `rga3.objects`, `rga3.motions`, `rga3.incidence`; cc, 20261031 |
| Point groups closed under compose, orders from the International Tables | Performed, passed at R: the 32 point classes close | Performed, passed for the orders against Hestenes' Table 4, a primary source (note 0023 G5; decision 0211 item 3); 29 of 32 classes equal as sets, 2/m, −42m and 32 differ by setting (conjugacy inferred, not proved); the property columns rest on Wikipedia (G2); the International Tables were not read (M4 gate) | `rga3.point_groups` |
| Denavit-Hartenberg chains against independent 4×4 matrices | Performed, passed at R: four joint chains | Partly: against a secondary account only (note 0023 J1, J2); Denavit and Hartenberg not read | `rga3.chains` |
| Incidence | Performed, passed at R | Partly: meets up to sign only (note 0023, "Not verified" item 5) | `rga3.incidence` |
| Rotors with rational half angles against Rodrigues' matrices | Performed, passed at R: 21 rotations | Performed, passed against Rodrigues' matrices for rational half angles; not beyond the derived case or off glibc 2.44 | `rga3.motions` |

## 5. Performance claims (track-only, C14)

"Performed" means recorded; nothing here passes or fails.

| Claim | Status | Source, command |
| --- | --- | --- |
| DoD-22: per-TU wall time and peak memory, both compilers, release revision | Performed at R, shared mode: 944 rows (gcc-debug 471, clang-debug 473), `compile.tsv` lines 4673 to 5616, all at `7276f826533f`. Largest: gcc-debug `transform_identities_test.cpp` 6,214,900 KiB (5.93 GiB), 92.3 s; clang-debug the same unit 1,585,484 KiB (1.51 GiB), 32.8 s. The 662 rows with a nonzero exit are the compile-fail tests' compiles. Not comparable with pinned rows (P14) | matrix's debug pair, then decision 0080's copy step after the build and after the suite |
| DoD-22: before and after for each core unit (W10) | Performed unit by unit; accepted past gaps WU9 (plan 0018) and plan 0009, partial (decision 0248 item 4) | `records/performance/core-unit-w10-index.md` |
| DoD-23: growth benchmark, n in {4, 8, 16} at fixed use; 1, 4, 16 distinct operations at fixed n; instructions beside wall time | Performed at `18849b5` (2026-11-10), `--cpu 4`, three repeats, under the lock; before the forced inlining; not rerun (decision 0246 item 2) | `records/performance/wu17-growth/README.md` |
| DoD-23: the exceptions recorded for growth | Antiproduct, `compose` and `transform` against decision 0141; `bulk_dual` by decision 0247 | same; release record, exceptions 1 and 2 |
| DoD-24: operation counts equal the reference's, five rga3 operations | Performed, passed at R in every preset, checking on and off. Form A's counts (decision 0233 item 2) | `rga3.op_counts_checking`, `rga3.op_counts_unchecked`, against `golden.py`; `records/spikes/spike-e/COUNTING-RULE.md`; matrix, 0 |
| DoD-24: no call at `-O2` | Performed, passed at R in every preset: 52 probe functions, both compilers, in one unit and each alone, x86-64 | `rga3.kernels_inline`, `rga3.kernels_inline_alone`; matrix, 0 |
| DoD-24: no blade branch, no allocation | Performed at `18849b5` (E1, 2026-11-10) on both compilers, before the forced inlining; at R inferred, since the structure test checks calls and jumps only | `records/performance/wu17-asm/README.md`, "Findings" |
| DoD-24: pinned benchmark against the in-repo straight-line form | Performed at `2de0d36` (E2, 2026-11-10), CPU 4, under the lock, seed 20261110, before the forced inlining: 10 of 22 loops slower and 6 faster under the strict reading. At R the 22 benchmark tests ran once on each compiler, pinned to CPU 4 under the lock: 22 of 22 passed and the checksums of the two forms agree in all 11 pairs; one process each, no repeats, so not a P6 measurement. **The release claims no benchmark of its own code** (plan 0042 section 3.4) | `records/performance/wu17-bench/README.md`; at R `taskset -c 4 tools/timed test P gate:bench@7580e88 -- ctest --test-dir build/P -L '^bench$' -V` on gcc-measure and clang-measure (rows 21947, 21948) |
| DoD-25: performance history baseline committed; `runs.tsv` rows kept through compaction | Baseline: era `ebdd06d09289` (rows 2026-10-31T20:40:59Z to 2026-11-01T23:52:19Z); no WU17 baseline (part H deferred). Kept rows: `tools/runlog check` passed at `7276f82` (row 21949), at `4e7d4aa` (row 21950) and at the U9 tip: "1 eras match the history; 1727 kept rows present once; no removed row present" | `records/performance/history/eras.tsv`; `tools/timed check - gate:runlog@7580e88 -- tools/runlog check` |
| Limits (largest `full<N>` per operation and compiler) | Recorded on the limits page; the plan-cost canaries `core.plan_cost_model.*` passed at R in every preset; `cost.py selfcheck` at R: 1228 Clang and 1235 GCC cases predicted, 0 differences | `docs/guide/limits.md`; rows 21944, 21945 |
| Header and umbrella parse cost (the `basis_change.hpp` measurement, decision 0240 item 6) | Not performed: part H deferred (decision 0246 item 2); `basis_change.hpp` stays in `core.hpp` (decision 0248 item 9, O2) | n/a |
| Build cost of the inlining pass, before and after (W10, debug and release trees) | Performed at `7a27f76`, `a6f3196`, `d2568e3` (wall times not comparable, a loaded machine) and `2d292ef`, 2026-11-11. At R: gcc-debug's largest unit 5.93 GiB as at gate 3; clang-measure 667.4 s against gate 3's 664.0 s | `records/performance/pre-release-core-pass/{cost,cost-public-ops,cost-d1,opt-out}/`; the matrix record's table |
