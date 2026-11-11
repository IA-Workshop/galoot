# Release record: galoot core 1.0.0 (draft)

- Status: draft. Records only (decision 0240 item 11). Blanks marked `____` are not yet known and are filled at the release revision. The owner merges to `master` and tags (W13); nothing here is a release until then.
- Written: 2026-11-10 by Claude (agent, Sonnet 5.5) at `develop` `1ed0e6a`.

## Departures and unknowns, first (C20)

1. Everything marked `____` is unknown today. The exception statements below describe the state at `1ed0e6a`; they change if the pre-release core pass (decision 0240 item 1) alters the code they describe.
2. The DoD-24 departure is described from the first E2 run and its inline rerun (`records/performance/wu17-bench/`, `wu17-asm/`). The owner's ruling (decision 0238 item 1, amended by 0240 item 1) is that a core inlining pass runs and E1, E2, G, and H rerun at its closing revision. The text below must be rewritten from those reruns; until then it records the departure "as measured".
3. The "known exceptions" list is what the owner's rulings and the Fable advisory report name. I did not search the records for others; the auditor's release audit may find more.
4. The platform assumption is from the advisory report's R9; it was not derived again here.
5. Whether the presets declare options that make the statement true (for example `-ffp-contract=off`) was read in `CMakePresets.json` (lines 46, 54, 62); I did not check every preset.

## Release

- Component: galoot core, version 1.0.0 (plan 0001 D17; models ship beside it at 0.x, and `rga3` is a preset at ____).
- Revision: ____ . Tag: ____ (owner).
- CONVENTIONS version implemented: ____ (1.17 at `1ed0e6a`).
- Model versions: `rga3` ____ . cga3: not in this release (decision 0211).
- CMake project version: `0.1.0` at `1ed0e6a` (`CMakeLists.txt:6`); to be set to 1.0.0 in the last commit before the tag (decision 0240 item 5 and the advisory report's R4b). Current value shown here only to mark the open step: ____.
- No `install()` rule in 1.0 (decision 0240 item 5); the README names `add_subdirectory` and `FetchContent`.

## Class covered (DoD-1)

Diagonal R(p, q, r) and symmetric rational forms with an exact change of basis, for 1 ≤ n ≤ 16 (compile-time rejection outside it). Named blades, `get<"...">`, and printing work up to n = 9 (`max_compact_generators`); beyond that a user writes `blade_set` literals by hand. A two-digit name syntax is after 1.0. Graded signatures are rejected at compile time. Exact statement to be confirmed against the guide: ____.

## Platform and toolchain assumptions

- Linux x86-64, GCC 16.2 and Clang 23.1, libstdc++; MSVC unsupported; no untested minimum claimed (D6). Versions at the release run: ____.
- **Rounding check assumption.** The forward bound of the rounding check in `transform` (and the tolerances derived for rounding scalars) rests on x86-64 SSE2 arithmetic with no extended-precision intermediate, round to nearest, and gradual underflow (no flush to zero). A platform that differs is outside the claim. The presets set `-ffp-contract=off`; a fused multiply-add only tightens a bound of this form, but a user's build is not checked. Source: advisory report R9; statement in the headers and guide: ____.
- `rotation` is run-time only and not verified beyond the derived case or off glibc 2.44 (M4 table); to confirm: ____.
- The core is exception-free and throws nothing; the test runner (doctest) is not built with `-fno-exceptions`. Bare `-fno-exceptions` compiles stand for DoD-21 (decision 0240 item 4): evidence ____.

## Known exceptions

Each is a departure the owner has accepted or deferred. None is a blocking defect unless the owner says so.

1. **The antiproduct's growth at fixed use (decision 0141, C6).** The geometric antiproduct and the antiwedge are built by conjugating the base product with the complements, so their compile-time cost grows with n at fixed use: at 0141's measurement, +265% on GCC and +464% on Clang from n = 4 to 16, against about 6% and 4% for the geometric product. Accepted as an exception for twelve months, to 2027-11-01, ending at the first of: a production report of a build-time problem; a model or use needing more than six generators while using few; or 2027-11-01. Decision 0140's remedy stays proposed. Current figures from the WU17 growth grid (`records/performance/wu17-growth/README.md`, "The antiproduct's figures for decision 0141"): ____. Listing in this record is required by 0141 item 5.
2. **`bulk_dual`'s growth at fixed use (decision 0240 item 10, beside 0141).** From n = 4 to 16 at fixed use, +1.8% on GCC and +7.3% on Clang (decision 0237 item 5; `wu17-growth`), most likely because it multiplies by the unit antiscalar, a blade of grade n. Not fixed before 1.0. Recorded under the same end conditions and review date as exception 1: ____ (the exception decision's number, if the owner writes one).
3. **`basis_change`: `==` replaced by `same_rows` (decision 0235, from 0225 items 3 and 4).** `basis_change<N>` no longer has `operator==`; `same_rows(a, b)` compares the stored rows and denominators entry by entry, and a program using `a == b` fails to compile. Rows are also reduced by their gcd before the checks, so changes with large common factors are accepted where they were rejected as overflow. 1.0 is the first release, so this is informational under W13 (a list of removed or changed interfaces): none in a prior release.
4. **DoD-24: out-of-line calls on Clang, and Clang's loop vectorization of the reference.** DoD-24 asks for no calls. At E1, GCC has none. Clang emits an out-of-line `transform` (a tail `jmp`) for the point and plane, a call from `transform` to `run_product<first_kernel>` for the line, and a call to `run_product<product_kernel>` for `compose`. E2's inline rerun (`wu17-bench/README.md`, at `2de0d36`) finds Galoot beyond the criterion on Clang for all three transforms and `compose` (both loops; compose latency at two of three sizes), and on GCC for `compose`; the arithmetic retired is the same or close, so the cost is the call and the memory round trip of its result. Strictly, 16 of 22 loops show a difference at every size; Galoot is slower in 10 and faster in 6. The ruling (0238 item 1, amended by 0240 items 1 and 2) is a core pass forcing the kernels inline with one macro; the departure stands as measured until E1, E2, G, and H rerun at its closing revision. **This item departs from the E2 records as they stand today, and it is to be rewritten from the reruns**: result ____ (calls remaining ____; gaps ____). Clang's cross-iteration vector loop is not traced unless a gap remains (0238 item 2).
5. **D8, collected plans, deferred (decision 0239 item 1).** 1.0 ships without collected plans for repeated inputs. Operation counts of composed plans therefore may exceed collected-term counts where components cancel; the P2 and DoD-4 reading is "declared-output check holds": ____ (confirm which DoD-4 branch the record claims). The first candidate after 1.0 is a named batch-under-one-motor interface needing a commutative-scalar concept; it needs core work or public term lists (C13). The effect of collection on the rounding check in conformal(3) was not computed.
6. **Platform assumption of the rounding check**: see above (x86-64 SSE2, round to nearest, gradual underflow). Listed here because a user can break it without a compile error.
7. **Strict scalar-type rule (decision 0240 item 7).** Integer literals are rejected where the scalar type is not an integer, in the linear and motion operations (`linear.hpp`, `motions.hpp`; exact cases to be listed from the guide); the likeliest user complaint (advisory R10). Kept for 1.0 and taught in the guide: ____.
8. **Operators `%` and `!`** (kept, 0240 item 7): `!x` is the right complement, not a boolean. Taught on the guide's first page: ____.
9. **Compile-time limits of each operation** (the certified `full<N>` limits per compiler) are recorded in the guide, not the headers (decision 0240 item 9): ____.
10. **Other open items at release**: ____ (the auditor's release audit, the owner's walkthrough, and the M7 gate report add here).

## Interfaces removed or changed incompatibly (W13)

1.0 is the first release, so this list is informational. Changes during the 0.x development that a reader of the history may meet:

- `basis_change` `==` to `same_rows` (above).
- The `rga3` model moved from the core to `galoot::presets::rga3` (decision 0213).
- `detail::inverse_change` deleted and a deleted-`==` hint added (decision 0240 item 1; to confirm merged): ____.
- The sixteen `*_layout` names and `compose_layout` and `antiproject_layout` are public interface from 1.0 (decision 0187).

## Evidence

- V10 table: `v10-table-skeleton.md` filled at the release revision: ____.
- Every check ran on the released revision (DoD-29): ____.
- DoD-2 inventory: `dod-2-inventory.md`: ____.
- Auditor's release audit: ____ (no blocking finding: ____).
- Owner walkthrough: ____. Open blocking defects: ____.
- Plans closed (DoD-28): see `plan-closures.md`: ____.
- Version and tag: owner.

## Must fill before the release commit

- The README's `FetchContent` block carries the placeholders `<galoot-repository-url>` and `<release-tag>`: owner supplies both (decision 0241 item 4): ____.
- `CMakeLists.txt` `VERSION` (now 0.1.0): ____.
- The study byline, if the owner names an author (decision 0241 item 3): ____.
- Known friction to state: debug-build compile cost after forced inlining (the pre-release core pass's cost records), `-Os` builds still calling into the library, the one-blade conversion message (the compiler's own error), and contraction changing the last bits under `-ffp-contract=fast` (decision 0243).
