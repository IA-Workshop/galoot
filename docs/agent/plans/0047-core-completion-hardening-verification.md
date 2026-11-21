# 0047 Core completion, hardening and verification after 1.0.0

- Status: proposed 2026-11-21 by Claude (agent, Fable 5.2), chief planner, with the math-expert, numerical-expert and cpp-expert role files read. The owner authorized the Fable level for this planning unit (stated in the coordinator's brief). From `main` at `c95fee5`, on branch `plan/0047` in the worktree `galoot-wt/plan-0047`. No unit below has started. The coordinator's merge of this plan into `main` is the claim (W3, W7).
- Source of scope: the coordinator's brief (complete, harden and verify the core after 1.0.0); the handoff of 2026-11-15 ("Backlog after 1.0"); plan 0042 section 9; plan 0001 sections 3 and 4 and its outcome; `records/release/release-record.md` (known exceptions 1 to 12, known friction 1 to 13), `v10-table.md`, `dod-1-to-29.md` and `dod-2-inventory.md`; decisions 0140, 0141, 0163, 0187, 0211, 0218, 0219, 0223, 0225, 0227, 0236, 0239, 0241, 0243, 0244, 0246, 0248 and 0251; the knowledge base's note on Theorem R (`geometric-algebra/proofs/test-retirement.md`).
- Siblings read: plans 0043 (cost cleanup, `plan/0043` at `0d066fd`), 0044 (header hygiene, `plan/0044` at `8eb88fa`), 0045 (the 1.0.1 release procedure, `plan/0045` at `4f7f744`), and section 8 of plan 0046 (Garnet, `plan/0046` at `6e32aa4`). None is merged into `main` yet. Siblings not read: plans 0048 (aggressive simplification) and 0049 (pedagogical documentation) are being written at the same time and had no branch when this plan was written; section 9 states the interfaces this plan needs from them.
- Facts at `c95fee5`, each from a command run for this plan: 8,410 lines under `include/galoot/` (fourteen core headers, the umbrella header, the rga3 preset); 48 `_test.cpp` and 330 `_fail.cpp` sources under `test/core/` and `test/presets/rga3/`; eleven named build presets, two of them with the address and undefined-behavior sanitizers; no fuzz target, no coverage tool and no generated-mutant tool in the tree (`grep -i 'fuzz\|gcov\|llvm-cov\|coverage'` over the build files and `tools/` is empty); one cross-check record (`records/cross-checks/0030-sage-change-of-basis`); CONVENTIONS 1.18 carries 54 status labels (25 Checked, 19 Confirmed, 10 Proposed); the math tools' Python environment holds sympy 1.14.0 and galgebra 0.6.0; the next free decision number is 0252 (plans 0043 to 0045 may take numbers first).
- Shared resources: every unit that builds reads `tools/jobpool status` before and after, starts the pool with `tools/jobpool ensure` only when the status says "not running", never twice, and never stops it. Heavy runs go under `math-tools/exercises/performance/pe-common/memguard.sh`; timed runs take the lock with `pe-common/lock.sh take` and `release`. No processor configuration is touched or proposed. No third-party maintainer is contacted; a defect found in a tool is noted in the records. No web request carries a personal identifier. GCC and Clang only. Resources are recorded, and no local bound is set.

## 1. Departures, ambiguities, and readings (C20)

1. **The integration branch is `main`.** The governance says `develop`; every "`develop`" in W5, W7 and P9 is read as `main` (plan 0045 section 6 brings the text in line). `master` is not touched.
2. **The plan is on `plan/0047`, not on `main`.** W7 claims a unit by committing its plan to the integration branch and has no `plan/<name>` row. The brief asks for this branch and no merge, so the coordinator's records-only merge is the claim, as for plans 0043 to 0046.
3. **Plans 0048 and 0049 were not read**, because they did not exist. Everything this plan says about them (section 9) is a request, not a description. The coordinator reconciles the three plans when all are committed; a conflict found then is an owner item.
4. **"Placement search" has no record under that name** (plan 0042 section 1 item 3 says the same). Reading followed: it is option B of decision 0227 item 2, which replaces the linear searches of `collect_result` and `type_terms` (`plan.hpp:308` and `:357`) by a direct append and a binary search, so that placing T terms on R result blades stops costing T times R. Plan 0042 section 9 lists "placement search" and "option B of `collect_result` (0227 item 2)" as two entries; I found nothing that tells them apart. If the owner meant a different thing, item O3 is rescoped.
5. **"The core" is read as the constitution defines it**: the headers under `include/galoot/core/`. M6 (cga3) is an algebra preset on the core's public interface with zero core diff (decisions 0211 and 0213), so it is not core by definition. What this plan asks of M6 is only the part that bears on the core: evidence that the core needs no change for it (unit K1).
6. **"ABI/API stability" is read for a header-only library.** Galoot ships no binary, so there is no library ABI. Three things remain and are planned: the source interface (names, signatures, constraints, macros, and literal diagnostics users may match); the layout of the value types (size, component order, trivial copyability), which a user may store or send; and the one-definition rule across the translation units of one program that include Galoot with different settings of `GALOOT_CHECKING` or `GALOOT_FORCE_INLINE`, or at two versions.
7. **"Fuzz" is read in two parts.** The run-time surface (values through kernels) already has seeded randomized tests; the compile-time surface (signatures, blade sets, names, changes of basis) cannot be fed by a fuzzer while it runs in a constant evaluation. `plan.hpp` states that the generator is `constexpr` code that serves a run-time call as well, so the plan's fuzz unit calls the generators at run time, under the sanitizers, against an independent reference. Whether every generator entry point can be called at run time was not checked (some are `consteval`); unit H4 states what it could reach.
8. **"Mutation testing" is read as a sweep.** The project has 35 mutation records, each a hand-written spec for one unit's tests (`tools/mutate`). No record gives a mutation score for a header. The plan adds generated mutants over each header and a classification of every survivor.
9. **"Coverage of every public name" is read as two checks**: that every entry of the public inventory has a test that uses it and can fail, and that the inventory itself is pinned by a test. Line and branch coverage is a third thing; it is blind to code that runs only in constant evaluation, which is most of the generator, so it is planned only for the run-time harness of H4 (section 5.3).
10. **Candidate defects named in this plan are not findings.** Section 5.1 lists places I read and suspect. None was compiled, run or reproduced. Each is input to the audit unit H0, which confirms or dismisses it.
11. **Not verified here:** no build, test, lint or mutant ran. The pool read "not running" at 2026-11-21 14:10 UTC and was not started, since nothing heavy ran. The figures quoted from plans 0043 to 0045 and from the release records are theirs, not re-measured. The machine-time estimates of section 11 are inferred from the release matrix and the mutation records, not measured.
12. **Checks of this commit.** Records only (P8: documentation, no tests). The citation check reads code paths and the guide, not `docs/agent/plans/`, so it cannot be affected; it was not run.

## 2. Goal and definition of done

Bring the released core to the state where it can be simplified, documented and built on without fear: every deferred item has a disposition, every way to misuse the core or drive it to an edge has a test that fails when the behavior changes, and every claim the release makes has evidence that does not rest on the project's own earlier reading.

Done when:

1. **Complete.** Each deferred item of section 4 is closed: done, or deferred by a recorded ruling with the event that reopens it. The core's sufficiency for the next presets is shown by a test-only model and a conformal feasibility probe with no diff under `include/galoot/core/` (K1).
2. **Hardened.** The audit register of H0 exists and every row has a test or a recorded reason for none. The public interface is pinned by a manifest test. The generators run at run time under the sanitizers against an independent reference. Every defect found has a test committed before its fix.
3. **Verified.** The evidence-gap register of section 6 has no open row: each gap is closed, narrowed with the remainder stated, or kept by a ruling. Every labeled item of CONVENTIONS has been derived again by a route that shares nothing with `test/reference/`. Each core header has a recorded mutation result with every surviving mutant classified.
4. **Ordered.** The tests of phase 1 are merged into `main` before any unit of plan 0048 changes `include/` (section 9).
5. **Recorded.** The outcome is written here; the gap register, the manifest and the mutation baseline are handed to the next release's record.

This plan releases nothing. Its merges ride the next release (owner item O11).

## 3. Acceptance criteria

1. **Manifest.** A CTest test compares the public declarations of `galoot::core` and `galoot::presets::rga3` (name, kind, signature, constraints) with a committed manifest and fails on any difference. At the end of the plan, the manifest differs from the one taken at the 1.0.1 tag only where a decision record says so, and no entry is removed.
2. **Names.** For every manifest entry a check finds at least one test that uses the name and carries a reference marker, and, for every entry with a stated requirement a compiler can reject, a compile-fail test on both compilers. The check lists the entries without either; the list is empty or each remaining entry has a recorded reason.
3. **Mutants.** For `bits.hpp`, `blade_product.hpp` and `plan.hpp` (the generative mechanism, C4 and C12's "strongest evidence") every generated mutant is run and none is left unclassified. For the other headers the sample of O6 is run and classified. A survivor is "equivalent" with a reason a reviewer accepted, or a gap closed by a test that the survivor fails (W3's rule for a gap's mutants).
4. **No test without a mutant.** Each test this plan adds names a mutant that the suite without it lets survive and that it kills, or, for a test of an edge the code already handles, the mutant that removes the handling. A test that cannot meet this is not added (V2: every test is able to fail).
5. **Conventions.** Each of the 54 labeled items of CONVENTIONS sections 1 to 5 and 7 has a row in the re-derivation record: derived again and equal, derived again and different (with a C11 record and a test that tells the alternatives apart), or not derivable with the reason. No row is blank.
6. **Edges.** Every integer and rounding site in the H0 register has a boundary test at its limit and one step past it, with the expected result taken from the stated requirement or an exact computation, not from the implementation.
7. **Sanitizers.** The run-time harness of H4 passes its recorded seed campaign on gcc-asan and clang-asan with the standard library's assertions on, and its line coverage of the generator functions it calls is recorded.
8. **Cost.** The build and suite cost of each phase 1 merge is recorded before and after on gcc-debug and clang-measure (P1). A new unit above 2.5 GiB on gcc-debug or above 500 G instructions on clang-measure (the criteria plan 0043 fixes for its splits) is reported with its cause and split before the merge.
9. **Style.** No test, comment, message or documentation line added by this plan cites a decision, plan, provision or agent (S11).
10. **Review.** Every merge into `main` passes W5: the style checks, the P9 `main` row, the trace gate, and a blind review by an agent that does not share the author's context.

## 4. Completion: what is deferred, and where each item belongs

"Complete" is judged against plan 0001 section 3 as amended to the DoD-2 inventory (decision 0248 item 9). Everything on that list shipped in 1.0.0. What remains is the set of deferrals; the table gives each a proposed disposition. The owner rules the ones marked O.

| Item | Source | What it is | Proposed disposition |
| --- | --- | --- | --- |
| Theorem R at the structure layer | decisions 0218 item 1, 0219 item 11 | A family in the identity suites: the unit, the generator rule and the recursion identity pin the whole table to the Clifford product, including the basis orientation, which the associativity loop cannot see | **Core, now.** Tests only; unit V4. Already ruled by the owner; the triple loop stays |
| Placement search | decision 0227 item 2 (reading of section 1 item 4) | A faster placement of a plan's terms, lifting the documented compile-time limits for a small set times a large one | **Core internals, deferred with a trigger** (O3). No shipped preset or planned one needs a dense product at eight generators or more; C12 makes an optimization wait for its requirement; and it would add code to `plan.hpp` while plan 0048 removes code. Reopened by the first of: a preset or a user report that reaches the limit; plan 0048 touching the two functions anyway |
| `witt_change` | decisions 0223 item 2, 0225 item 5 | A factory for the change of basis from R(m,m) to its Witt (null-pair) basis. Today a test helper (`test/core/basis_change_forms.hpp:87`) | **Not core** (O4). A user builds it from public names without editing the core (C5), and it has no present user (C12). Proposed: it stays a test helper and becomes a worked example in the guide, handed to plan 0049 |
| M6, the cga3 preset | decision 0211 | Named conformal types and their operations | **A preset, later, under its own plan** (O5). CONVENTIONS 6.2 and 6.4 are still marked Study, so the preset cannot be tested against a specification yet. Plan 0046 section 8.3 proposes it for 1.2.0. This plan does only K1: the evidence that the core needs no change for it |
| DoD-5's test-only model | plan 0001 DoD-5; `dod-1-to-29.md`, a gap no ruling covers | A model added in the tests with no diff under `include/galoot/core/` | **Core evidence, now.** Unit K1 (spacetime algebra R(1,3,0) as the test-only model) |
| One-blade catch-all | decision 0241 item 1; release exception 8 | A conversion that a one-blade layout rejects gets the compiler's own error, not a message in the terms of the algebra (C9) | **Core, now, as hardening.** The compile-fail test in H1, the fix in H6 |
| Spurious overflows after the row reduction | decision 0236 item 2; exception 9 | A change of basis rejected as overflowing where the reduced inner products would fit | **Characterize now, fix later.** H2 pins which inputs are rejected and records the smallest one; the fix waits for a user who meets it, as ruled |
| Antiproduct and `bulk_dual` growth at fixed use | decisions 0141, 0247; exceptions 1 and 2 | Compile cost that grows with n at fixed use | **Accepted to 2027-11-01 by ruling.** Not planned here. K1 reports whether conformal(3) work meets it |
| Batch under one motor | decision 0239 item 1; exception 3 | A named interface that collects a transform's terms once for many points | **A new feature, later.** Not core completion; a minor release with a present user (Garnet's rigid motions are the first candidate, plan 0046 section 8.2 use 4) |
| A contraction build preset | decision 0243 item 3 | A preset with `-ffp-contract=fast` | **At the M6 revisit**, as ruled. H3 adds evidence (section 5.2) but no preset |
| WU17 parts G and H, the E1 and E2 reruns | decision 0246 item 2; exception 4 | The cost-model refresh, pinned debug baselines, the assembly re-read and the benchmark against the hand-written form | **Verification, in this plan** (V7), run after plan 0048's kernel-affecting units so that the code measured is the code that ships (O10) |
| Graded signatures; exp, log, norms, `unitize`; contractions; SIMD | plan 0001 section 3 "Out" | Later tiers of the design study | **Out.** Unchanged. Norms and `unitize` for rga3 are plan 0046's request for 1.1.0, at the preset layer |

After the rulings the core's feature list for the 1.x line is the 1.0.0 inventory. Nothing in this plan adds a public name to the core.

## 5. Hardening: what is audited and tested

### 5.1 The audit register (unit H0) and candidate sites

H0 is a read-only audit by a cpp-expert and a numerical-expert working separately, each producing a register that the coordinator merges. A row is one site: file, line, the condition that can go wrong, what happens today (rejected at compile time with message M; `GALOOT_ASSERT`; returned failure; undefined; silently wrong), the test that pins it or "none", and the proposed test. The classes:

| Class | Scope | Candidates read while planning (unconfirmed, section 1 item 10) |
| --- | --- | --- |
| Error paths | every `static_assert`, catch-all overload, `GALOOT_ASSERT`, `std::expected` and `std::optional` return, and non-`constexpr` failure call in the fourteen headers, against the 330 compile-fail sources, the misuse catalogue and `test/internal_messages.txt` | The one-blade catch-all (exception 8). "A mask listed twice in one set is not checked" (`plan.hpp:605`, `:926`): documented, accepted, and worth one test that pins what happens. `collect_result`'s capacity check is a `GALOOT_ASSERT`, so in a non-checking constant evaluation the failure is the compiler's bounds wording (decision 0227 item 3) |
| Integer edges | every arithmetic operation on `coefficient_type`, `entry_type`, `mask_t` and the index types: overflow, negation of the least value, shifts by the width, narrowing, and the bounds `max_generators` 16, `mask_bits` 32, `max_form_entry`, `max_compact_generators` 9 | The magnitudes `x < 0 ? -x : x` at `basis_change.hpp:266-267`, for the least `coefficient_type`. Shifts of a `mask_t` when a factory's `MaxN` is 32. The release record's "untested above 16" for 17 to 32 generators (O9) |
| Rounding edges | every place a rounding scalar meets a branch, a division, a bound or a comparison: `transform`'s declared-output check and its forward bound, the rga3 `rotation` and its normalization, the change of basis over a rounding scalar | Inputs at the subnormal boundary, at overflow, with NaN and infinity; an axis of zero or near-zero length in `rotation`; the bound under `-ffp-contract=fast` (friction 7) |
| Undefined behavior at run time | every loop and index that runs at run time in a kernel or in `format.hpp` | Formatting above nine generators; `get<"...">` on absent blades |
| Configuration | `GALOOT_CHECKING` and `GALOOT_FORCE_INLINE` across the units of one program; two Galoot versions in one program; a forced-inline operation called from a function built for fewer instruction-set features (friction 8) | An inline function whose body differs between two units of one program breaks the one-definition rule; whether Galoot's bodies differ by setting, and what the guide should say, is unit H5's question |

Constant evaluation is itself a check: undefined behavior of the core language in a constant expression is ill-formed, so every plan the suite generates at compile time has already been checked for it on the inputs the suite uses. H0 states which sites that covers and which run only at run time.

### 5.2 The test units

- **H1, error paths and diagnostics.** A compile-fail test on both compilers for each register row without one, with the literal message, the error count, and the catalogue entry; a pin for friction 8 (both compilers stop, and the message names the attribute) so that the limits page stays true; and the `[[nodiscard]]` half of DoD-21, which no test checks today (V10 table): a compile-fail source per header that discards each result under `-Werror=unused-result`, or a scan, whichever H1 shows cheaper. Messages are judged against the usability checklist by the blind review.
- **H2, integer edges.** Boundary tests at each limit and one step past it, over the test rational and `std::int64_t`, with expected values from exact arithmetic in `test/reference/`. The spurious-overflow characterization of section 4. The 17 to 32 generator range under O9.
- **H3, rounding edges** (numerical-expert). For each rounding site: the bound restated with its constants and its conditions, adversarial inputs checked against high-precision ground truth (MPFR or `python-flint`, run in the math tools; only values with their derivation enter the repository), the observed worst ratio of error to bound recorded, and a test at each condition's edge. It also measures the contraction case of friction 7 again at the tip. H3 produces the analysis that plan 0046's requests 1 and 2 (enclosure scalars, the evaluation-order statement) need; it does not add those guarantees, which belong to the release plan that takes them.
- **H4, the run-time harness and the sanitizers.** A seeded doctest program that draws a signature (diagonal, degenerate, indefinite and Gram forms within `max_form_entry`, up to six generators), two oriented blade sets (with repeats and both signs), and an operation, calls the generator at run time, and compares the plan with an independent product written in the test from the definition (the contraction-and-wedge recursion of Theorem R, which shares no code with `blade_product.hpp`). A second target does the same for the blade-name parser, the formatter (format, parse, compare), and the change-of-basis validation (random integer rows: every accepted change is an isometry by an exact check, every rejected one fails a named condition). Both run on gcc-asan and clang-asan with the library assertions. Failures become fixed tests (V7). The memory sanitizer is recorded as not applicable (it needs an instrumented standard library); the thread sanitizer stays not applicable (no threads). A coverage-guided fuzzer is O7.
- **H5, one program, one Galoot** (cpp-expert). An investigation with reproducers: what differs between units that set `GALOOT_CHECKING` or `GALOOT_FORCE_INLINE` differently, whether any entity's definition differs, and what two versions in one program do. The output is a decision record with options (document the rule; make the setting part of the entities' names through an inline namespace; refuse at link time), the measured cost of each, and a guide passage for plan 0049. No header changes in H5 itself (O8).
- **H6, fixes.** One subunit per confirmed defect, each starting from its failing test (committed first and labeled, W2) and ending with a delta review. A fix that would change a public name, a result or an accepted input stops and goes to the owner (W8); one that changes a message updates the catalogue and the guide in the same unit (W12). H6's size is unknown until H0 to H4 report; section 11 budgets it and the stopping rule bounds it.

### 5.3 The interface pin (unit V2, shared with verification)

A manifest of the public interface, generated from the headers and committed under `test/`, with a test that regenerates and compares. It lists every declaration at namespace scope of `galoot::core` and `galoot::presets::rga3` outside `detail`: kind, name, template head with its constraints, and signature; for each value type, its size over `double`, its component order and `std::is_trivially_copyable`; the public macros are left to plan 0044's `core.public_macros` test, which already pins them. The generator is a script over the compiler's own view of the headers (Clang's AST dump is in the toolchain) or over the stripped sources as the DoD-2 scan did; V2 picks by which is stable across the two compilers and says why. A change to the manifest is committed apart from the change that causes it and listed in the review request, as W9 asks for expected values.

The manifest is the tool that keeps 2.0 away: an accidental break fails a test, and a deliberate one must be written down. With it goes a policy, recorded as a decision (O8): a public name is removed only in a major release; before that it is marked `[[deprecated("use ...")]]` for at least one minor release; `detail` names, the helper macros and the order of evaluation inside a kernel are not interface; literal diagnostic messages are interface only as far as the guide quotes them.

Coverage by lines is measured for H4's harness only, with the compiler's own tool on a tree of its own outside the named presets, and recorded once. It is not a gate: a line reached in a constant evaluation counts as unreached, so the figure understates and cannot be given a threshold.

## 6. Verification: the evidence gaps

The register below starts from the V10 table, the DoD table and the release record at 1.0.0. Unit V1 completes it (every cell marked unverified, unvalidated, partly, inferred or "not re-read") and keeps it under `records/` as the plan's working list.

| # | Gap at 1.0.0 | Source | Closing unit and method |
| --- | --- | --- | --- |
| G1 | Conventions were derived and checked by the project's own scripts; no route outside `test/reference/` has derived them | V1 kinds 1 to 3; the brief | V3: re-derivation in the math tools |
| G2 | Antiproduct, antiwedge and De Morgan are not validated for non-diagonal forms: they were checked against a composition built on their own definition | decision 0163 item 3; V10 row 4.2 | V3 and V5: derived from the complement definitions over symbolic Gram forms in sympy, compared on the non-diagonal forms of the signature set. A published table is still wanted; its absence is restated if none is found |
| G3 | Grade involution (5.6) and antigrade involution (4.4) are Proposed, not validated | V10 row 2.1 | V3: derivation plus a read of the sources the knowledge base lists (`involutions-and-quaternion-types.md`); the status label changes only by the owner's edit of CONVENTIONS |
| G4 | `transform` and `compose` are not validated for odd n or non-diagonal forms | V10 row 4.4 | V5: sandwich through independent matrix representations for n = 3 and 5 and two Gram forms |
| G5 | Meets are validated up to sign only | V10 rows 5.1 to 5.3, incidence | V3: the sign derived from the complement convention; V5: against determinants computed exactly |
| G6 | `rotation` is not validated beyond the derived case | V10 row 5.7 | H3 |
| G7 | The basis orientation is not pinned by the identity suite | knowledge base, Theorem R note | V4 |
| G8 | No mutation score exists for any header | section 1 item 8 | V6 |
| G9 | Only one black-box cross-check exists (Sage, change of basis) | `records/cross-checks/` | V5: galgebra and sympy |
| G10 | `[[nodiscard]]` is not checked by a test | V10 table, DoD-21 row | H1 |
| G11 | "No blade branch, no allocation" is inferred at R; the structure test checks calls and jumps into `galoot` only | exception 4 | V7a (phase 1): the structure test also rejects any call and any conditional jump in the probe functions, or states which it must allow |
| G12 | The release claims no benchmark of its own code; E1 and E2 predate the forced inlining; G and H not run | exception 4; decision 0246 item 2 | V7b (phase 3) |
| G13 | Other layouts and algebras, `-Os`, and `GALOOT_FORCE_INLINE` 0 are outside the inlining claim | exception 4; friction 6 | V7b measures and states them; no claim is added without a test |
| G14 | 17 to 32 generators are untested | release record, class covered | H2 under O9 |
| G15 | The public inventory is a hand-read record, not a test | `dod-2-inventory.md` | V2 |
| G16 | The trace gate lists 6.1 to 6.4 with no test | release record, other checks | K1 covers 6.1 and 6.3 in a test-only model if the owner agrees (O5); 6.2 and 6.4 wait for their specification |
| G17 | Point groups: three classes differ from the table by setting, conjugacy inferred | V10 table, point groups row | V3: the conjugating motor exhibited for each, exactly |

### 6.1 How the re-derivation is kept independent (V3)

The math-expert works in the math tools (`math-tools/toolset/env/py/.venv`, sympy 1.14.0 and galgebra 0.6.0 pinned in the record), from the definitions in CONVENTIONS and the published sources, and does not read `test/reference/`, `golden.hpp` or the headers until the comparison step. Each item is derived by hand or symbolically from a stated definition (the Chevalley construction for products, the defining relation for each complement), then compared with the golden values. galgebra's conventions differ from Galoot's (its dual and its basis order among them), so a comparison through galgebra is a black-box check of kind 4 with the translation written out per item; it never supplies an expected value (C15). A disagreement stops the item: it is recorded under C11 with both readings and a distinguishing test, and goes to the owner, since only the owner changes CONVENTIONS. Notes that generalize go to the knowledge base under its own rules; exercise code stays in `math-tools/exercises/`; no tool path appears in a test.

### 6.2 The mutation sweep (V6)

A small generator (a script beside `tools/mutate`, which keeps its spec format) writes specs of mutants per header from fixed operators: a relational operator swapped, an arithmetic operator swapped, a constant moved by one, a sign flipped, a condition negated, a statement deleted, an index moved by one. Each mutant runs against the selection of the header's own component and the identity suites, on clang-debug (the cheaper tree) and, for the mechanism headers, gcc-debug too. Known limit: GCC reports "unjudged" where Clang says "killed" for a guard reached through a `consteval` call (decisions 0231 and 0232), so Clang's verdict is the score and GCC's is recorded. Results per header: generated, not compiled, killed, survived; then each survivor's class. Survivors classed as gaps are handed to H1 to H4 or closed in V6 by a test. The sweep runs once on the tree at the 1.0.1 tag plus the phase 1 tests (the baseline) and again in phase 3 on the files that plan 0048 and H6 changed.

### 6.3 Theorem R (V4)

For each form of the structure layer (n up to 6, with the Witt and random forms decision 0219 names): the unit, the generator rule against a contraction-and-wedge computed in the test from the form and the bit-mask sign rule, and the recursion identity for every blade of grade two or more. The mutants are the three the knowledge-base note used: one flipped coefficient, rows of grade two or more from another form, and an associative table in a re-signed basis, which the triple loop accepts and this family must kill. The family is added in the layout plan 0043 gives the identity suites (a driver, checks in sources of their own, one function per law), and its compile cost is recorded before the merge.

## 7. Units

Sizes: S under 100k tokens, M 100k to 250k, L 250k to 500k, each without its review. "Tests" means `test/` and the registration files only.

| # | Unit | Role, model | Size | Changes | Phase |
| --- | --- | --- | --- | --- | --- |
| V1 | The evidence-gap register completed from the release records | planner, Sonnet high | S | `records/` | 0 |
| V3 | Conventions derived again in the math tools (section 6.1); G1, G2, G3, G5, G17 | math-expert, Opus high | L | a research note, `records/cross-checks/0047-conventions/`; the knowledge base and math tools outside the repository | 0 |
| H0 | The audit register (section 5.1), two independent reads | cpp-expert and numerical-expert, Opus high | L | `records/` | 0 |
| B0 | Test registration by file: one included list per phase 1 unit, so that parallel units do not edit `test/CMakeLists.txt` | build-expert, Sonnet high | S | `test/CMakeLists.txt` (one line per list), new list files | 1, first |
| V2 | The interface manifest, its test, the names check (section 5.3); the stability decision | cpp-expert, Opus high | M | `tools/` (the generator), tests, a decision | 1 |
| V4 | Theorem R at the structure layer (section 6.3) | math-expert writes the checks' specification, doctest-expert the tests, Opus high | M | tests | 1 |
| H1 | Error paths, diagnostics, `[[nodiscard]]` | doctest-expert with an hci-expert read of the messages, Opus high | M | tests, the catalogue records | 1 |
| H2 | Integer edges | doctest-expert, Opus high | M | tests, `test/reference/` | 1 |
| H3 | Rounding edges | numerical-expert, Opus high | L | tests, `records/numerical/` | 1 |
| H4 | The run-time harness under the sanitizers | cpp-expert and doctest-expert, Opus high | L | tests | 1 |
| V7a | The structure test also rejects calls and conditional jumps (G11) | performance-engineer, Opus high | S | `test/tools/kernels_inline.py`, its test | 1 |
| V6 | The mutation sweep: the generator, the baseline, the classification (section 6.2) | doctest-expert, Opus high; Sonnet for the classification tables | L | `tools/` (the generator and its test), `records/mutations/0047-sweep/` | 1, after the others' tests exist |
| V5 | Cross-checks and validation: galgebra and sympy as black boxes, matrix representations for G4, exact determinants for G5 | math-expert, Opus high | M | `records/cross-checks/`, tests where a value is ported with its derivation | 1 |
| K1 | The test-only model (spacetime algebra) and the conformal feasibility probe: round point and the four-part split of CONVENTIONS 6.1 and 6.3, with `git diff -- include/galoot/core/` empty; a list of what the core lacked, if anything | math-expert and doctest-expert, Opus high | M | tests; a spike record | 1 |
| H5 | One program, one Galoot: reproducers and the decision | cpp-expert, Opus high | M | `records/`, a decision | 1 |
| H6 | Fixes, one subunit per defect, tests first | implementer by field, Opus high | M to L, unknown | `include/`, tests, the guide where a message or limit changes | 2 |
| V6b | The sweep again on the changed files | as V6 | M | `records/mutations/0047-sweep/` | 3 |
| V7b | E1 re-read, E2 with its full protocol under the lock, WU17 parts G and H (G12, G13) | performance-engineer, Opus high | L | `records/performance/` | 3 |
| C1 | Closure: the register's final state, the V10 table's changed rows for the next release record, this plan's outcome, the handoff lines | planner, Sonnet high | S | `records/`, this plan | 3 |

Every unit that changes tests, tools or headers gets a blind review (W5) by an agent of a different configuration, given the diff, the unit's criteria and the author's departures, and not the author's view; a fix after review gets a delta review. Records-only units need none (decision 0215); V3's note and H0's register are checked instead by a second specialist reading a sample of rows chosen by the reviewer, because later units build on them (W14: a delegate's result is checked before it is used).

## 8. Order, branches, and merges

**Phase 0, now, beside the 1.0.1 work.** V1, V3 and H0 change only `docs/agent/` (and, for V3, files outside the repository), so they cannot collide with plans 0043 to 0045. Branches `records/gap-register`, `research/conventions-rederived`, `records/hardening-audit`. H0 reads the tree at `c95fee5` and re-reads the lines plan 0044 changes once 1.0.1 is tagged (four headers, small diffs).

**Phase 1, after the 1.0.1 tag: the safety net.** One feature branch, `feature/core-safety-net`, from `main` at the tag's tree; B0 first; then V2, V4, H1, H2, H3, H4, V7a, V5, K1 and H5 as subfeatures in their own worktrees, in parallel where their files differ (B0 makes the registration files differ; `test/reference/` is shared by H2 and V5, which run one after the other). V6 starts when the others' tests are on the feature branch, since its baseline must include them. Two merges into `main`, each with one P9 matrix: M1 with V2, V4, H1, H2 and V7a (the part plan 0048 waits for, section 9), and M2 with H3, H4, V5, K1, H5 and V6. Tests that expose a defect are committed labeled as failing (W2) on a subfeature and reach `main` only with their fix, so `main` stays green; the defect is listed in the register meanwhile.

**Phase 2: fixes.** `feature/core-hardening-fixes`, one subfeature per defect, merged as M3. H6 touches `include/`, so it runs when no unit of plan 0048 has the same header open (section 9).

**Phase 3: after plan 0048's code units.** V6b, V7b and C1.

Why phase 1 waits for the tag: plan 0043 restructures the identity sources that V4 extends and changes `tools/mutate`, which V6 builds on; plan 0044 changes four headers and adds tests in `test/CMakeLists.txt`; and plan 0045's release commit must have only records after its audited tip. Starting the test units earlier would mean rebasing them over both and measuring a baseline on headers about to change.

## 9. Interfaces with the other plans

### 9.1 Plans 0043, 0044 and 0045 (1.0.1): no duplication

| Topic | Owner | This plan |
| --- | --- | --- |
| The split of `identities_rounding_test.cpp` and `transform_identities_test.cpp`; `tools/measure`'s dirty mark; `tools/mutate --parallel`; the job pool findings | 0043 | Not repeated. V4 follows the split's layout; V6 uses the parallel CTest |
| Users' build cost under the forced inlining (0043's O3: a default that follows the optimization level) | 0043 defers it to 1.1.0 "with a measurement" | V7b supplies the measurement; the choice stays the owner's and is not planned here |
| Helper macros, the empty `GALOOT_FORCE_INLINE`, the cache option, confusable locals, the two `apply` overloads, the README's copy route, the `==` hint, the "preset" wording | 0044 | Not repeated. V2's manifest is taken after 0044's deletions; macros stay pinned by `core.public_macros` |
| The release matrix, the release record, the audits, the branch-name text, the run-log compaction | 0045 | Not repeated. This plan hands the register and the changed V10 rows to the next release's record and edits nothing under `records/release/` |

Shared files, and the rule for each:

| File | 0043 to 0045 | This plan | Rule |
| --- | --- | --- | --- |
| `test/CMakeLists.txt` | 0043 U1, U2; 0044 U1 to U3 | B0 adds one line per list file; later units edit only their own list | B0 branches after the 1.0.1 tag |
| `test/core/identities_*` | 0043 U1, U2 (and O2 (b)) | V4 adds sources, edits no existing check | after 0043 |
| `tools/mutate` | 0043 U4 | V6 adds a generator beside it and changes it only if the spec format must grow, as a unit with its own test | after 0043 |
| `include/galoot/core/{assert,bits,mv,basis_change}.hpp` | 0044 U1, U3, U4 | H6 only | after the tag |
| `CMakePresets.json` | 0044 U2 | none: coverage and any fuzz target live outside the named presets, so the release matrix of eleven does not grow | none shared |
| `docs/guide/` | 0044 U1, U5 | H6, only where a message or limit changes; otherwise text is handed to plan 0049 | section 9.3 |
| `records/release/` | 0045 | none | hand-over through C1 |

### 9.2 Plan 0048 (aggressive simplification): tests first

The request to plan 0048, to be confirmed against its text:

1. **The gate.** No unit of plan 0048 changes a file under `include/` before merge M1 is in `main`. Plan 0048's reading, measuring and candidate-listing units may run during phases 0 and 1.
2. **The candidate list, early.** Plan 0048 gives H0 its list of code it expects to remove or rewrite before H1 to H4 start. For those sites this plan writes tests at the public boundary only, so that no new test pins an internal that is about to go.
3. **Where tests attach.** Phase 1 tests use public names wherever the behavior is reachable from one. A test that must name a `detail` entity says so in its registration (a label, `internal`), and plan 0048 may retire it together with the entity by a recorded decision that names the public-level test covering the behavior (W9: a test is not weakened to make a change pass).
4. **What each simplification must keep.** The manifest unchanged, or its change listed with a decision; every phase 1 test passing unmodified, except those of item 3; `rga3.kernels_inline` and V7a passing; the counting-scalar operation counts unchanged; and, on each header it touches, no mutant of the V6 baseline that was killed before surviving after (V6b checks this).
5. **What this plan leaves alone for 0048's sake.** The placement search is not implemented (O3), no optimization is added, and H6's fixes take the most direct form (S16). If plan 0048 rewrites `collect_result` or `type_terms` anyway, the direct form stays in a test as the reference (C3).
6. **Removal of public names.** If plan 0048 proposes to remove or change a public name (the two helpers without a user, `sum_layout` and `basis_change_like`, are the likely candidates), the manifest makes it visible and the stability decision of V2 applies: deprecate in a minor release, remove in 2.0.0. Whether to do that is plan 0048's owner item, not this plan's.
7. **Shared files.** Any header: one plan's unit at a time per header, the second merging `main` first. `test/`: plan 0048 edits tests only under item 3.

### 9.3 Plan 0049 (pedagogical documentation)

1. **What this plan hands over**, as records that plan 0049 restates in its own words (user documentation cites no record, S11): V3's derivations of each convention, with worked examples; the `witt_change` construction as a worked example of a user's own change of basis (O4); H5's rule on settings and versions within one program; H3's statement of the rounding bound's conditions; K1's spacetime-algebra model as an example of a user's own algebra.
2. **What this plan needs from 0049.** The documentation tests stay the gate for every example (W12), and the guide keeps quoting diagnostics literally, since H1 pins them and the catalogue check compares.
3. **Shared files.** `docs/guide/core.md`, `limits.md`, `rga3.md` and the README belong to plan 0049 while its branch is open. An H6 fix that changes a message or a limit changes the guide's sentence in the same unit (W12); if plan 0049 has that page open, H6 hands it the exact sentence and merges after it. The misuse catalogue under `records/diagnostics/` is this plan's.

### 9.4 Plan 0046 (Garnet)

Nothing here blocks or is blocked by it. H3's analysis is the evidence for Garnet's requests 1 and 2 (plan 0046 section 8.3); the guarantees themselves, the rga3 norms, rga2 and the conformal presets are for the release plans that take them. Section 8.5 of plan 0046 (what Galoot must not absorb) is respected: no predicate, filter, tolerance or number type is added.

## 10. Evidence and risks

Evidence this plan produces: the gap register and its closure; the audit register; the manifest and its test; the tests of H1 to H4, V4, V7a and K1, each with the mutant it kills; the re-derivation record and the cross-check records with tool versions; the mutation baseline and its second run; the cost rows of each merge (P1, P13); the benchmark and assembly records of V7b; a blind review record for each merge; agent cost rows in `records/agent-cost.tsv` at each hand-back.

Risks:

1. **Tests that pin the implementation** would make plan 0048's work harder and would be deleted with the code. Section 9.2 items 2 and 3 and criterion 4 (a test needs a mutant) are the guards.
2. **Test cost grows.** The suite already has units of several GiB and minutes. Criterion 8 fixes the reporting thresholds; new sources follow plan 0043's layout; H4's seed campaign is sized by measurement and labeled `slow`.
3. **The sweep's machine time.** Each mutant rebuilds and runs a selection. The estimate (section 11) is hours; O6 chooses the extent. It runs one tree at a time on the pool and never overlaps a timed run.
4. **False disagreements in V3 and V5** from a wrong translation between conventions. Each comparison writes its translation down, and a disagreement is reproduced by hand on the smallest case before it is reported.
5. **H0 finds much, or little.** Much: H6 grows, and the stopping rule (W8) and the owner's ranking bound it. Little: phase 1's tests still stand as the net for plan 0048. Either is reported as found.
6. **A defect whose fix changes behavior users may rely on.** It goes to the owner with the options (fix in a patch, deprecate, document as a limit); nothing is changed silently.
7. **The manifest is brittle or differs between compilers.** V2 chooses the generator by that test and normalizes what must be normalized, stated in the test's comment.
8. **Three plans edit the same headers and pages.** Section 9's rules; the coordinator sequences; a fix that would touch another active unit's file is reported, not made (W7).
9. **Coverage figures mislead**, since constant evaluation is not counted. They are recorded, never gated.
10. **GCC's "unjudged" verdicts** shrink the GCC score. Clang's verdict is the score; the limit is restated in the record.
11. **Other sessions' load** slows runs; instructions and peak memory are compared, wall times reported as loaded (P14).

## 11. Agent cost estimate (W3)

| Work | Model | Tokens |
| --- | --- | --- |
| This plan | Fable 5.2 | see the hand-back |
| Phase 0: V1, V3, H0 | Sonnet, Opus | 0.66M to 1.05M |
| Phase 1 to M1: B0, V2, V4, H1, H2, V7a | Opus, Sonnet | 0.70M to 1.15M |
| Phase 1 to M2: H3, H4, V5, K1, H5, V6 | Opus | 1.45M to 2.25M |
| Blind and delta reviews of phase 1 (about a third of the authored work) | Opus, Sonnet | 0.70M to 1.10M |
| Phase 2: H6 and its reviews | Opus | 0.40M to 0.90M, unknown until H0 reports |
| Phase 3: V6b, V7b, C1 | Opus, Sonnet | 0.50M to 0.85M |
| Coordinator: briefs, merges, matrices, cost rows | coordinator | 0.20M to 0.35M |
| Subtotal | | 4.6M to 7.6M |
| With the history's factor of about 1.5 for fixes and reruns | | about 7M to 11M |

The safety net alone (phase 0's H0 and V1, phase 1 to M1 with its reviews) is about 1.6M to 2.6M before the factor, and is the part plan 0048 waits for (O1).

Machine time, inferred: three P9 matrices at about 40 to 48 minutes each; the mutation baseline about 6 to 10 hours for the extent O6 (a) (about 350 to 450 mutants at one to two minutes each on clang-debug, the mechanism headers twice), and about a third of that for V6b; H4's seed campaigns about 30 minutes on the two sanitizer trees; V7b about 2 hours under the lock. V3 and V5 run in the math tools and use no build tree.

## 12. Owner decisions (each with a default if not ruled)

- **O1. Extent.** The plan is large against the history (plan 0042 used 2.3M tokens). (a) Approve all of it; (b) approve phase 0 and the safety net to M1 now, and rule on the rest when H0's register and V1's register show what is there; (c) only the safety net, dropping V3, V5 and the sweep. Tradeoff: (a) gives one ruling and the full evidence, at 7M to 11M tokens; (b) costs about 2.5M to 4M before the next ruling and lets the registers size the remainder, with one more owner round; (c) is cheapest and leaves the "independent re-derivation" and mutation parts of the brief undone. Recommendation and default: (b).
- **O2. The order against plan 0048.** (a) The gate of section 9.2 item 1: no change under `include/` by plan 0048 before M1; (b) let the two run together and rely on the existing suite. Tradeoff: (a) delays plan 0048's code units by the length of phase 1 to M1 (its reading units are not delayed); (b) saves that time and removes code whose edges are untested. Recommendation and default: (a).
- **O3. Placement search.** First confirm the reading (section 1 item 4). Then: (a) deferred, with the triggers of section 4; (b) implemented now as a core unit, tests first, with the direct form kept as the reference and the limits page recalibrated (about 0.4M tokens); (c) dropped from the backlog. Tradeoff: (b) lifts a documented limit (a scalar times a dense set of ten generators on GCC, eight on Clang) that no present user meets, and adds code to the mechanism just before a simplification pass; (c) loses a known remedy. Recommendation and default: (a).
- **O4. `witt_change`.** (a) Not in the core: it stays a test helper and becomes a worked example in the guide through plan 0049; (b) a public factory beside `conformal_change` in 1.1.0, with tests, a guide entry and a manifest change; (c) dropped. Tradeoff: (b) is convenient for anyone working in a null basis and adds a public name with no present user, which C12 argues against and which could only be removed again in 2.0.0; (a) costs nothing in interface and teaches the general method. Recommendation and default: (a).
- **O5. M6 (cga3).** (a) Its own plan, as a preset, proposed for 1.2.0 as plan 0046 sequences it, after CONVENTIONS 6.2 and 6.4 are specified; this plan does only K1, with tests for 6.1 and 6.3 in a test-only model; (b) M6 in this plan; (c) no probe either. Tradeoff: (b) mixes a feature into a hardening plan and needs convention text only the owner ratifies; (c) saves about 0.2M tokens and leaves "zero core diff" for conformal work, and DoD-5's gap, unshown before the simplification pass. Recommendation and default: (a).
- **O6. Extent of the mutation sweep.** (a) Every generated mutant for `bits.hpp`, `blade_product.hpp` and `plan.hpp`, and a stratified sample of about 40 per other header; (b) every mutant of every header (roughly four times the machine time and twice the tokens for classification); (c) a sample everywhere. Tradeoff: (a) puts the strongest evidence on the mechanism, as C12 asks, and gives an estimate elsewhere; (b) gives a full score with many equivalent mutants to read; (c) is cheapest and can miss a gap in the mechanism. Recommendation and default: (a).
- **O7. Tools for fuzzing and coverage (a C13 decision).** (a) Seeded doctest programs in the suite, the compilers' own coverage tools on a tree outside the named presets, no new package and no new preset; (b) also a coverage-guided fuzz target (the fuzzer that ships with Clang), outside the presets and run by hand; (c) a twelfth preset for it. Tradeoff: (a) adds nothing to maintain and explores inputs only as widely as the seeds; (b) finds inputs a seed campaign does not, at the cost of a Clang-only target and a corpus to keep; (c) adds about 4 minutes to every release matrix. Recommendation and default: (a), with (b) proposed again only if H4 reports that the seeded harness stopped finding new behavior too early.
- **O8. Stability policy and the one-program rule.** (a) Record as a decision: the manifest test is the gate; removal only in a major release after a deprecation in a minor one; what is not interface (section 5.3); and H5 investigates settings and versions within one program and proposes text, with no header change until its decision; (b) also put the policy into WORKFLOW W13, by the owner's edit; (c) no policy, the manifest only. Tradeoff: (b) binds every later plan and needs a governance pass; (a) takes effect now and can be promoted later. Recommendation and default: (a).
- **O9. Generators 17 to 32.** The release record says a factory's `MaxN` raises the bound to at most 32, "untested above 16". (a) H2 tests sparse use at 17, 24 and 32 and the claim stays; (b) the guide states the range as untested and nothing is added; (c) the bound is lowered to 16, which removes accepted input and so belongs to 2.0.0. Tradeoff: (a) costs a few tests and may find the shift edges of section 5.1; (b) is free and leaves a public claim without evidence. Recommendation and default: (a); if (a) finds a defect that cannot be fixed without changing accepted input, (b) until 2.0.0.
- **O10. The deferred cost evidence (V7b).** The handoff recommended it as the first unit after 1.0. (a) In this plan, in phase 3, after plan 0048's kernel-affecting units, so that one campaign measures the code that ships; V7a's structure check goes in phase 1 as part of the net; (b) now, before plan 0048, and again after (two campaigns, about 0.4M tokens and 2 hours under the lock each); (c) its own plan. Tradeoff: (b) gives plan 0048 a measured baseline to compare against, where (a) gives it only the structure and operation-count tests. Recommendation and default: (a), unless plan 0048 proposes to change a kernel's generated code, in which case (b).
- **O11. The release that carries this work.** (a) This plan releases nothing: test-only merges and fixes ride the next minor release, and a fix for a defect that gives a wrong result ships earlier as a patch release under plan 0045's procedure; (b) a 1.0.2 after M3 whatever is found. Recommendation and default: (a).
- **O12. Model level.** Policy: Opus with high effort, Sonnet for scoped records work, Fable only by permission. The two units where a stronger model would most reduce the risk of a missed finding are H0 (the audit) and V3 (the re-derivation). (a) Opus for all, with the second-specialist sample check of section 7; (b) Fable for H0 and V3. Tradeoff: (b) costs more per token for the units everything else builds on. Recommendation and default: (a); (b) if the sample check of either finds an error.
- **O13. Confirm the readings of section 1**, items 5 to 9 in particular (what "core", "ABI", "fuzz", "mutation testing" and "coverage of every public name" mean here). Default: as read.

Only the owner can: rule O1 to O13; change CONVENTIONS (a status label in G3, any disagreement V3 finds, the specification of 6.2 and 6.4); change governance (O8 (b)); merge to `master` and tag.

## 13. Next step

The coordinator merges this plan into `main` (the claim), reconciles section 9 with plans 0048 and 0049 when they are committed, and reports O1 to O13 to the owner. On the rulings, phase 0 starts at once (V1, V3, H0), and B0 is briefed when 1.0.1 is tagged.

## Addendum, 2026-11-21: facts corrected after unit V1

- Status: added 2026-11-21 by Claude (agent, Opus 5.5), planner, on `records/gap-register` in the worktree `galoot-wt/gap-register`, from the evidence-gap register `records/verification/0047-gap-register.md` (section C, as corrected after the independent sample check `records/reviews/2026-11-21-0047-v1-sample-check.md`). Records only.
- Scope: this addendum corrects statements of fact in sections 1 to 13 that the register shows to be out of date, and records one reading by the coordinator. It adds, removes and resizes no unit, and changes no criterion, phase or owner item. The text above is not edited; read each line named below through this addendum. Record paths are under `docs/agent/` unless they start with `test/` or `knowledge-base/`.

### Corrected facts

1. **G7, the basis orientation** (line 117: "The basis orientation is not pinned by the identity suite"). Partly out of date. Plan 0031 added the generator-rule and recursion cases to `core.identities` (`test/core/identities_test.cpp:170` and `:189`) over every form of up to four generators and the forms `plans/0031-wu14-follow-ups.md:13` lists. The mutant `resigned_e12`, an associative table in a re-signed basis, survived the suite before that unit and is killed after it on gcc-debug and clang-debug (`records/mutations/0031-wu14-follow-ups/README.md:12` and `:15`). What stays open is the structure layer: n up to 6 and the Witt and random forms (`plans/0031-wu14-follow-ups.md:90`, its O2). That is the scope that line 57 and section 6.3 already give V4. Register row G7; section C item 1.
2. **G14, generators 17 to 32** (line 124: "17 to 32 generators are untested", taken over from `records/release/release-record.md:43`, which line 81 and O9 at line 273 also quote). Too wide. Construction at the widest mask is tested: the factories accept `mask_bits` generators when `MaxN` is raised (`test/core/metric_test.cpp:610` to `:617`), `of_grade<mask_width, mask_width>` has mask `~0` (`test/core/blade_set_test.cpp:603` to `:613`), and the antiscalar of `mask_bits` generators is the widest accepted input of the `bits` checks (`test/core/bits_assert_fail.cpp:8`). The top bit is rejected in R(31,0,0) by two compile-fail tests (`test/core/blade_product_beyond_n_31_fail.cpp:1` to `:18`, `test/core/mv_beyond_n_31_fail.cpp:1` to `:20`). What is untested above 16 is use of the products, duals and `transform`; the dense and sparse identity suites stop at 8, 12 and 16 (`records/release/v10-table.md:81` and `:89`). H2's sparse use at 17, 24 and 32 is that remainder. Register row G14; section C item 2.
3. **G1, no independent derivation** (line 111: "no route outside `test/reference/` has derived them"). Too strong. The knowledge base's proofs index (`knowledge-base/geometric-algebra/proofs/README.md:28` and `:31`, at knowledge-base `80f318a`) marks the sign rule with the right and left complement signs, and blade-set order, as machine-checked for all n. Research note 0025 reran those proofs at math-tools `d698d34` (`research-notes/0025-math-tools-proofs-behind-the-core.md:13`, `:36` and `:40`). The pseudoscalar square is checked by computer algebra for n ≤ 8 only (`proofs/README.md:30`; note 0025:39). Theorem R has a paper proof that is not machine-checked (`proofs/README.md:33`), and note 0025 did not rerun it (`:56`, "not part of the harness"). The gap holds as stated for the dual, antiproduct, transform and rigid-object conventions (items 3.2 to 3.4, 4.2 to 4.4 and section 5). Register row G1; section C item 3.
4. **DoD-5's test-only model** (line 61: "`dod-1-to-29.md`, a gap no ruling covers"). Out of date. Decision 0251 item 2 accepted "the algebras the tests declare themselves (`test/core/signatures.hpp`, the V8 algebras)" as the named test-only model (`decisions/0251-release-walkthrough-rulings.md:6`). K1's spacetime-algebra model is further evidence, not the closure of a gap. "DoD-5's gap" in O5's tradeoff (line 269) reads the same way. Register row A23; section C item 4.
5. **The figure 54** (line 6: "CONVENTIONS 1.18 carries 54 status labels (25 Checked, 19 Confirmed, 10 Proposed)"; line 44: "Each of the 54 labeled items of CONVENTIONS sections 1 to 5 and 7"). A count of words, not of labels or items. Over the whole of `governance/CONVENTIONS.md`, the words `Checked`, `Confirmed` and `Proposed` occur 25, 19 and 10 times, and `Study` 4 times. These counts include the legend (`governance/CONVENTIONS.md:7` to `:10`), section 8 and the revision history. Sections 1 to 5 and 7 (lines 18 to 84 and 98 to 116) hold 28 numbered items: 1.1 to 1.8, 2.1, 3.1 to 3.4, 4.1 to 4.5, 5.1 to 5.8, 7.1 and 7.2. Their labels carry the words 21, 14, 9 and 1 times. One of the 21 Checked is a cross-reference, not a label: "which every Checked transform below uses" (`:40`). Register section C item 5.

### The coordinator's reading of criterion 5 (unit V3)

Criterion 5 (line 44) is read per numbered item of CONVENTIONS sections 1 to 5 and 7. Each item is split where its status label splits, that is, where the label gives different parts of the item different statuses. By my count at CONVENTIONS 1.18 this gives 28 items and 44 rows:

- One row each, 14 items: 1.1, 1.2, 1.3, 1.5, 1.7, 1.8, 3.1, 4.2, 4.5, 5.1, 5.2, 5.3, 5.6 and 7.2.
- Two rows each, 13 items, split as their labels split: 1.4 (Checked; Confirmed for d = 3), 1.6 (Checked; Confirmed), 3.2 (Confirmed; the diagonal case Checked), 3.3 (Checked; Confirmed), 3.4 (Checked for rigid(3); Confirmed), 4.1 (Confirmed; the metric-independent wording Checked), 4.3 (Checked; Confirmed), 4.4 (Proposed; the odd sign Checked), 5.4 (Checked; Confirmed except the order of composition), 5.5 (Checked for l the z axis; Confirmed), 5.7 (Proposed; both actions Checked), 5.8 (Confirmed; closure under the antiproduct Checked) and 7.1 (Study; `compose` and `transform` Checked).
- Four rows, 1 item: 2.1 (the reverse and antireverse Confirmed; the grade involution Proposed; the antigrade involution Proposed; the antigrade involution's relations Checked).

The total, 14 + 26 + 4 = 44, equals the label words in range less the cross-reference in 2.1. "No row is blank" applies to these 44 rows. Section 6's items (6.1 to 6.4) are outside the criterion's range, as before.

### Put to the owner

The register suggests these changes of scope. None is made here. Each needs a ruling, which can come with the ruling on the remainder of decision 0253 item 1 (the units past M1).

1. **Rerun the Sage change-of-basis cross-check at the released header** (register row A7). The comparison read `basis_change.hpp` at `37f1729`, and eight commits changed the header before 1.0.0 (`records/cross-checks/0030-sage-change-of-basis/README.md:4` and `:6`; `git log --oneline 37f1729..1.0.0 -- include/galoot/core/basis_change.hpp`). Options: (a) V5 reruns the Sage route at the 1.0.1 tag beside its galgebra and sympy checks; (b) V5's new black-box checks stand in for it, and the old record stays dated; (c) a separate small rerun before V5. Tradeoffs: (a) adds work to an unauthorized unit but keeps the only existing black-box check current; (b) costs nothing and leaves "0 mismatches over 63 changes" about code that no longer ships; (c) gives a result sooner but needs its own brief. Recommendation: (a), if V5 is authorized; (b) otherwise.
2. **Rerun the hand-written mutation specs, or read the sweep as superseding them** (register row A8). Fault injection ran at `d5ffafd`, and the 35 records were not rerun at R (`records/release/v10-table.md:101` and `:102`; `records/release/release-record.md:125`); 32 commits changed `include/` after `d5ffafd` (`git log --oneline d5ffafd..1.0.0 -- include`). Options: (a) V6's generated baseline at the 1.0.1 tag supersedes the old specs, and C1 records that; (b) V6 also reruns the 35 specs once; (c) neither. Tradeoffs: (a) adds no machine time, but "supersedes" is a reading, not a result; (b) adds hours of pool time and gives a direct comparison; (c) leaves DoD-14's evidence dated. Recommendation: (a).
3. **Add the growth grid E3 to V7b** (register rows G12 and A17). V7b names E1, E2 and WU17 parts G and H (line 164), but E3 also predates the forced inlining, and the cause of `bulk_dual`'s growth is inferred (`records/release/release-record.md:78` and `:81`). Options: (a) V7b also reruns E3 at its tip; (b) E3 waits for the review of decisions 0141 and 0247 on 2027-11-01; (c) K1 reports conformal(3)'s growth only, as line 64 says. Tradeoffs: (a) adds about one more measurement campaign under the lock to an unauthorized unit; (b) and (c) cost nothing now and leave the 12-month acceptance resting on pre-inlining figures. Recommendation: (b), since the ruled review date already reopens it.
4. **`rotation` off this C library** (register row G6). H3 checks against high-precision ground truth from MPFR or `python-flint` in the math tools (line 92) and names no other C library, so G6's "nor off glibc 2.44" (`records/release/v10-table.md:52` and `:137`) is outside every unit. Options: (a) C1 states the remainder as "this C library only"; (b) H3 adds a second libm. Tradeoffs: (a) is free and leaves a stated boundary; (b) adds a toolchain axis that no other unit has. Recommendation: (a).
5. **DoD-5's zero core diff at the released tree** (register row A33). The diff was recorded at the M4 gate and not re-derived at R (`records/release/v10-table.md:108`); K1 shows it only for its own model (line 160). Options: (a) K1 also re-derives the diff for the M4 merges' models; (b) the row stays open as a dated record. Tradeoffs: (a) is a few `git` commands; (b) is free. Recommendation: (a), if K1 is authorized.
6. **The rows with no closing unit** (register section D, "None planned": A1, A12 to A17, A20 to A22, A24, A26, A27, A29 to A31 and A34 to A37). Options: (a) C1 records each as a stated limit of the evidence, with its source line; (b) the owner assigns some of them to units; (c) a later plan takes them. Tradeoffs: (a) costs only C1's write-up and closes nothing; (b) grows units the owner has not yet authorized; (c) defers the choice. Recommendation: (a), with (b) for any row the owner judges to weaken a claim users rely on.

## 14. Outcome

Not started.
