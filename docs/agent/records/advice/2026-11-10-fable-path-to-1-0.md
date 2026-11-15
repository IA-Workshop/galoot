# Advice: the path from WU17 to core 1.0

- Date: 2026-11-10
- Author: Claude (agent, Fable 5.1), advisory reviewer under decision 0239 item 3. Advisory only: nothing here takes effect without an owner ruling, and this file is the only change on `records/fable-advice` (from `develop` at `1fe8834`).
- Read in full: AGENTS.md; the six governance documents; plan 0001 with its addenda; plan 0038; research note 0027; decisions 0229 to 0239, 0141, 0159; the second 2026-11-07 handoff; the M5 audit; `agent-cost.tsv`; every header under `include/galoot/core` and `include/galoot/presets`; the README; the CMake presets; `test/bench/` and the E1, E2 and E3 records on the unmerged `records/cost-evidence-measurements` and `feature/cost-evidence` branches; the math-tools backlog.
- Run: read-only git and grep; one scratchpad experiment (section 6) of about twenty single-file compiles of a five-function probe against patched copies of the headers, each under a second, in the scratchpad, with no lock, no pool slot beyond the compiler's own process, and no timing claim. No build tree of the repository was touched; no test ran.

## 1. Departures, skipped checks, ambiguities (C20)

1. **No claim entry on `develop`.** W7 claims a unit by a plan or one-line entry committed to `develop`. The brief confines me to this one file on `records/fable-advice`, so I made no entry; decision 0239 item 3 is the record that this agent runs. The owner may want a one-line entry when the branch is merged.
2. **Not run: any build, test, lint, matrix, benchmark, or measurement of record.** Every figure below is either quoted from a committed record (cited by path) or comes from the scratchpad experiment, which is evidence for the shape of the inlining change and not a measurement under P6 or P14. Where I write "measured", the record named did the measuring.
3. **The "S2/S3 unit" in the brief's backlog list** matches nothing I can find. The only S2 and S3 in the recent records are the WU12 hci-reading items (reworded messages), applied in that unit (decision 0223 item 1). STYLE S2 and S3 are the language edition and layout rules, which have no open item. Section 8 asks the owner to name it; nothing below depends on it.
4. **The design study was skimmed, not read in full** (its outline, executive summary and the parts the roadmap cites). Its length is 996 lines and most of it is post-1.0 scope; the one recommendation I make about it (section 5) rests on what plan 0001 section 2 already records as wrong or superseded and on its byline, which I did read.
5. **Knowledge-base index not read.** A grep for prior inlining notes found only vendored compiler manuals, so nothing there bears on section 6.
6. **The experiment's attribute placement would be reflowed by clang-format** (checked). A real unit runs `format-check`; the finding about which functions need the attribute does not depend on layout.

## 2. Ranked recommendations

Twelve, most valuable first. Each gives the evidence, the tradeoff, and what I recommend. Section 8 lists every item that needs the owner.

### R1. One pre-release core pass, run before the M7 gate, so the measurements run once

- **Evidence.** Decision 0238 item 1 puts the inlining unit after the M7 gate and makes G and H wait for it, with E1, E2, G and H rerun at its close. E1 and E2 have already run twice (`wu17-asm`, `wu17-bench/first-run-out-of-line`, `wu17-bench`); each E2 campaign holds the timing lock for about 22 minutes and cost about 80k to 320k tokens (`agent-cost.tsv`, rows `wu17-e2-e3`, `wu17-e2-rerun`). Three other small core edits are already ruled or queued and touch the same headers: delete `detail::inverse_change` (decision 0236 item 1; `basis_change.hpp:532-553`, one test user at `basis_change_identities_test.cpp:223`), add the deleted `operator==` hint (0236 item 3), and the two `TODO` comments in a public header (`mv.hpp:392-396`, `mv.hpp:725-728`).
- **Tradeoff.** Reordering delays the M7 gate by the unit (about one agent-day) and puts one more core change under the gate's audit, which is where it should be audited. Keeping 0238's order means G and H cannot close WU17 until after the gate, so the gate accepts a unit whose own plan (0038 items 9 and 10) is not done, and E1, E2, G, H run a third time. Folding four small edits into one unit makes a diff of perhaps 80 lines, still one sitting (W2), against four units each with its own review, matrix and W10 record.
- **Recommendation.** One unit, "pre-release core pass": the inlining attribute (R2), the two `basis_change` items, resolve or delete the two TODOs, and the comment trims of R8. Blind review plus delta review as W5 requires. Then E1, E2, G and H once, at its closing revision; then the M7 gate. Record the reordering as a decision that supersedes 0238 item 1's ordering.

### R2. The inlining change: attribute the whole chain, through one macro, and test it as structure

- **Evidence.** Section 6 reproduces E1's finding on the original headers (Clang: three tail jumps to `transform` plus `run_product` calls for the line and for `compose`; GCC: none) and tests four attribute sets. Decision 0238's wording, "the vendor attribute on `transform` and `run_product`", leaves exactly one call on each compiler: both then keep `detail::apply` out of line, because forcing its caller inline moves the inliner's budget. Marking every function on the chain from the public operation to the runner (`run_map`, `run_product`, `run_sum`, `run_change`, the four `apply` overloads, `detail::sum`, `transform_middle`, `transform_outer`, `transform`, `compose`) gives zero calls on both compilers at `-O2`, compiles under the S4 flags with checking on and off, and passes the DoD-6 bare compile (`-std=c++23 -pedantic-errors`). `[[gnu::flatten]]` on the public operations alone works on GCC and not on Clang (two `run_product` calls remain), so it is not the answer.
- **Tradeoff.** Forced inlining puts a kernel body at every call site: that is P4's stated intent ("what a careful person would write by hand"), and the probe's `-O0` code changed by a few percent in each direction (GCC +4.6%, Clang −2% in assembly lines, one shot each), but a program with many call sites pays code size, and `-O0` debugging of `transform` loses its frame. A knob to turn it off has no present user (C12); add one only if the W10 record shows a notable debug-build cost.
- **Recommendation.** A macro `GALOOT_ALWAYS_INLINE`, defined in `assert.hpp` (the header every other includes) as `[[gnu::always_inline]]` under `__GNUC__` or `__clang__` and empty otherwise, applied to the twelve functions above; acceptance tested as structure, not cost (decision 0233 item 10's reading): a CTest that compiles E1's `probe.cpp` with `-O2 -S` on each compiler and fails on any `call` or `jmp` to a `galoot` symbol (the mechanism of `rga3.benchmark_*_inline` already does a symbol-table check). W10 before-and-after on both compilers, debug and release. Details and the full experiment table are in section 6.

### R3. Start the DoD-27 documentation now, in a parallel lane

- **Evidence.** DoD-27 asks for a README quickstart, a core guide and a model guide, every example compiled and run on both compilers. The README exists and its blocks are tested (`test/CMakeLists.txt:2505-2540`). No core guide or model guide exists, nothing claims them, and plan 0001 section 8 gives WU18 size S. The README still says "Galoot is in early development, and its interface may still change" (README.md line 22). The largest friendliness facts a user meets are not in any one place: results of products live on canonical layouts and must be converted with braces (`Line{p ^ q}`; `const Motor m = compose(...)` fails), scalars must match the type exactly (`2 * line` is rejected, `linear.hpp:443-452`), `%` is the antiproduct and `!` the right complement (`product.hpp:416-419`, `unary.hpp:120-128`), `get<"e41">` is oriented, names work only for n ≤ 9 (`blade_set.hpp:81-84`), and `transform` does not unitize.
- **Tradeoff.** Writing the guides before the pre-release pass merges risks a few sentences going stale (the inlining change alters nothing a user sees; the `basis_change` cleanup removes a `detail` function). Waiting serializes the longest remaining piece of work behind the shortest.
- **Recommendation.** Claim a docs unit now: `docs/core-guide.md` and `docs/rga3-guide.md`, written by an agent in the hci-expert role from the header comments (which already hold the content), with the fenced examples extracted and compiled by `tools/readme_examples.py` generalized to any Markdown file (decision 0229 item 2 left the label question to this unit). Review with the usability checklist. The guide's first page carries the six facts above.

### R4. Close four definition-of-done gaps that no unit owns

- **Evidence.** (a) DoD-21 says the suite builds with `-fno-exceptions`; no preset, CMake file or flag mentions it (grep of `CMakePresets.json`, `CMakeLists.txt`, `test/CMakeLists.txt`, `cmake/`). (b) `CMakeLists.txt:6` says `project(galoot VERSION 0.1.0 ...)`. (c) There is no `install()` or package-config export; the README documents `add_subdirectory` only. (d) DoD-2's public inventory table (each entry with its CONVENTIONS section, tests, documentation page and present user) does not exist yet, and DoD-29's release record must name the CONVENTIONS version (1.17 today).
- **Tradeoff.** (a) doctest needs exceptions unless configured without them, so a whole-suite build under `-fno-exceptions` may be large; a cheaper reading that still honors S9 is one compile-fail-style check that `bare_user.cpp` and the rga3 bare user compile and link with `-fno-exceptions`, since the core itself throws nothing. (c) An install rule is a small CMake addition users of `find_package` expect from a 1.0; it also widens the build surface the auditor checks. Documenting `add_subdirectory` and `FetchContent` as the supported routes costs nothing.
- **Recommendation.** (a) Add the `-fno-exceptions` bare compiles to the compile-fail lane in WU18 and state in the release record that the core is exception-free and the test runner is not. (b) Set the version in WU18, as the last commit before the tag. (c) Decide now (section 8); my recommendation is no `install()` in 1.0, named as such in the README. (d) The inventory table and the V10 skeleton are records-only work that can start today (R11).

### R5. Record the `bulk_dual` growth as a time-limited exception and fix nothing

- **Evidence.** E3 (`wu17-growth/README.md`, "Each growth and its cause") found `bulk_dual` growing +1.8% on GCC and +7.3% on Clang from n = 4 to 16 at fixed use, beyond the ranges, with the inferred cause that it multiplies by the unit antiscalar, a blade of grade n, so the generation runs the grade recursion over an operand of grade n (`dual.hpp:439-451`). Decision 0141 accepted the antiproduct's much larger growth (+111% to +374%) as an exception for twelve months with named end conditions.
- **Tradeoff.** A fix is a second generation route for the bulk (direct minors of the metric instead of a product with the antiscalar), a core change late in the train, with its own identities and cost records, for 8 to 34 million compiler instructions per unit. Recording it costs one decision and one row in the release record (0141 item 5 already requires listing exceptions).
- **Recommendation.** Decision: `bulk_dual`'s growth joins 0141's exception with the same end conditions and review date. Raise at the M7 gate as planned (0238 item 4).

### R6. Scale review and test-authoring ceremony to the change class

- **Evidence.** `agent-cost.tsv` holds 266 rows and 63.7 M tokens; by role, 28.7 M (45%) went to review, check and audit, 26.9 M (42%) to authoring, 2.6 M to planning. WU17 stands at 4.56 M against plan 0038's estimate of 2.3 M. The test-only inline-reference-kernel fix cost 66k to write and 193k to blind-review plus 59k to fix and merge (rows `wu17-part-d-inline`): the review cost three times the change. Part C (a test scalar and a counting test, no change under `include/`) cost 334k to author, 229k blind review, 108k fixes, 167k delta review, 15k merge: 853k, with a delta review that W5 requires only for core code. Decision 0236 item 5 already let the blind review's mutant probes stand in for a separate test author on parts C and D.
- **Tradeoff.** Lighter review on non-core changes trades a little assurance on tests and tools for about a third of the remaining spend; the auditor still reads every merge at the gate. C19 asks for exactly this proportion. The governance is the owner's; the changes below need no governance text, only briefs and one decision.
- **Recommendation.** For the rest of the road to 1.0: (a) delta reviews only for changes under `include/` (W5's literal rule); (b) a change only under `test/`, `tools/`, or `docs/` gets one Sonnet blind review against the acceptance criteria, no delta review; (c) one blind review per feature branch when its parts are each under about 200 lines, instead of one per part; (d) the separate test-author agent (decision 0185 mechanics) only where the tests judge core arithmetic, with the mutation evidence standing in elsewhere, as 0236 item 5 did; (e) coordinator rulings batched into one decision per report, as 0234 and 0236 already are; (f) measurement campaigns briefed with their protocol file written first, as E2's rerun did, so no run repeats for a protocol slip.

### R7. Keep the operator set, and teach its two traps on the guide's first page

- **Evidence.** CONVENTIONS 7.1 (ratified) makes `%` the geometric antiproduct and `!` the right complement; decision 0159 kept `%`. `!x` is a multivector, so `if (!x)` does not compile, and `!!x` is x times (−1)^(gr·ag), not x (`unary.hpp:120-128`); `^` and `&` bind below `==` (S13, every mixed expression parenthesized). The compile-fail catalogue and the trace gate pin them.
- **Tradeoff.** Removing `%` and `!` before 1.0 is free of W13's removal notice and removes two surprises; it also removes two ratified conventions, their tests and their trace rows, and a reader of Lengyel's pages gets no operator for the two operations they use most. After 1.0 a removal costs a release note. Keeping them costs a paragraph of documentation.
- **Recommendation.** Keep them. The guide's first page shows `(p ^ q) == l`, `p % q`, `!p`, and says what `!` is not. No code change.

### R8. Trim the measured-limit catalogues out of the public header comments

- **Evidence.** `plan.hpp:602-674` carries 73 lines of step and operation counts for two compiler versions, with the GCC flag values that `cost.py comments` had to be taught to ignore (decision 0230 item 1); `mv.hpp:482-493` and `883-905`, `product.hpp:337-360`, `transform.hpp:66-82`, `dual.hpp:75-95`, `blade_set.hpp:574-577`, `585-588`, `600-605`, and `basis_change.hpp:103-124` repeat the pattern. Each number is true for GCC 16.2 and Clang 23.1 with libstdc++ 16 and will be false for the next versions, in a 1.0 header that S11 says a user reads "without the project's records".
- **Tradeoff.** The content is valuable and hard-won; moving it loses nothing if the core guide gets a "limits and compile cost" page that the comments point to by name (a documentation page is not an agent record, so S11 allows the pointer). A comment-only change needs no delta review (decision 0183) but does need the matrix, so it belongs in the pre-release pass (R1) rather than a unit of its own.
- **Recommendation.** In each header keep one sentence (what grows, which flag raises the limit, where the failure is reported) and move the tables to the guide. Fold into R1.

### R9. The rounding check is sound as read; close two statements it leaves implicit

- **Evidence.** I checked the derivation at `transform.hpp:465-489` and `504-521` against the code: `kernel_roundings` counts one rounding per product, one more for |c| ≠ 1, and one per accumulation (`430-447`); `padded_magnitude` pads every rounded product with τ = λ/u ≥ λ, which covers the underflow term η; margin 1 is justified for (K+1)(2K+1)u < 1 and `margin_holds` tests it in integers; the checked path computes the middle with every term (`All = true`) and the unchecked path with the read terms only, and for every read output both kernels hold the same terms in the same order with the same `first` marks (`339-344`, `369`), so the "same result with and without the check" guarantee at `767-768` holds. NaN and infinity fail the check through `size <= tolerance && tolerance <= max` (`623-626`). Two things are not stated: (a) multiply-add contraction (`-ffp-contract=on`, Clang's default under `-std=c++23`) fuses a product and its sum into one rounding, which only tightens a forward bound of this form, but the header says "each operation rounds once" (`100-107`) and the presets set `-ffp-contract=off`, so a user cannot tell whether the check is sound under contraction; (b) a type too coarse for the margin (`std::bfloat16_t`) is silently unchecked (`549-562`), which C8 reads as a hidden rule.
- **Tradeoff.** Both are sentences, not code. Exposing `transform_rounding_checked` as a public trait would add an interface without a present user; a sentence in the `transform` comment and in the guide costs nothing.
- **Recommendation.** Add the two sentences in the pre-release pass. The release record states the platform assumption the bound rests on: x86-64 with SSE2 arithmetic (no extended-precision intermediate), round to nearest, gradual underflow.

### R10. Keep the strict scalar rule for 1.0, and name it as the likeliest complaint

- **Evidence.** `2 * line` over double is rejected with a message that says to write `2.0` (`linear.hpp:438-452`); `translation(1.0, 0, 2)` is rejected (`motions.hpp:150-161`); `rotation(l, 1)` likewise. Decision 0167 ruled this. The messages are good. Every GA library I know of accepts an integer literal here, and users write `2 * v` by reflex.
- **Tradeoff.** Accepting an integral literal exactly (it converts to any floating type without loss up to 2^53) is a small, C8-compatible widening, but it is an interface change late in the train with its own catch-all adjustments and tests. Keeping the rule costs nothing now and the message carries the fix.
- **Recommendation.** Keep for 1.0; note it in the release record under "known friction", and in the guide's first page. Reconsider after 1.0 with a user report.

### R11. Start the records-only work of WU18 today

- **Evidence.** Several WU18 pieces touch no code and need no review (decision 0215 item 7): the DoD-2 inventory table; the V10 table skeleton for every convention family, identity family, V8 case and performance claim (DoD-18), which the M5 audit already drafted most of; the release record with the exceptions it must list (0141 and R5), the CONVENTIONS version, and the removed or changed interfaces (W13; `==` on `basis_change` became `same_rows`, decision 0235); the closure of plans 0038 and 0039; the errata-naming register row (0234 item 7).
- **Tradeoff.** None beyond the tokens; these documents are inputs to the release audit and the owner's walkthrough, and they are on the critical path only if written last.
- **Recommendation.** One records branch, started now by a Sonnet-level agent from the M5 audit's tables, kept current as the core pass and docs merge.

### R12. Run the release matrix and the pinned compile record in one sitting at the final revision

- **Evidence.** P9's release row asks for the full suite in every configuration, the benchmarks, and a history update; DoD-22 asks for pinned serial builds at the release revision (plan 0038 item 10 defines the procedure, H); DoD-25 asks for the baseline. The last full matrix took about 21 minutes of pool time (`runs.tsv`, `gate:all` rows of 2026-11-10 13:48 to 14:09); H's two pinned builds about 20 minutes under the lock (plan 0038 O5); E2 about 22 minutes under the lock.
- **Tradeoff.** The release revision exists only after the version bump, so these runs happen once more after everything else merges; sequencing them in one locked window, pinned builds first and benchmarks after, keeps the lock held for about 45 minutes once instead of several times. The release audit can run on the same revision in parallel, since it does not time anything.
- **Recommendation.** WU18's last step: version bump commit; then, in one window, H's pinned builds, the eight-preset matrix from empty trees (`gate:all`), the benchmarks under the `bench` label, and the history update; the auditor's release audit on that revision at the same time; then the owner's walkthrough and the tag.

## 3. The shortest sound sequence, and what runs in parallel

Three lanes, each in its own worktree (W7), changing different files (R1 touches `include/` and `test/`; R3 touches `docs/`, `tools/readme_examples.py` and `test/CMakeLists.txt`'s README block; R11 touches `docs/agent/` only), so they do not rely on each other's unfinished results.

| Step | Lane 1: core and measurements | Lane 2: documentation | Lane 3: records |
| --- | --- | --- | --- |
| 1 | Pre-release core pass (R1, R2, R8, R9 sentences): plan, tests first, implementation, blind review, delta review, matrix, merge | Core guide and rga3 guide drafted (R3); README's status line; design-study status (section 5) | DoD-2 inventory, V10 skeleton, release-record draft, plan closures, `bulk_dual` exception decision, errata-naming row (R5, R11) |
| 2 | E1 and E2 reruns at the closing revision (E2 under the lock, about 22 min); G (verify refresh, pool, no lock); H (pinned builds, lock, about 20 min); merge `feature/cost-evidence` and `records/cost-evidence-measurements` into `develop` with the full matrix | Review with the usability checklist; examples compiled on both compilers; merge | Kept current with lanes 1 and 2 |
| 3 | M7 gate: auditor on `develop`, snippets review, gate report with V10 table, owner acceptance and the rulings of section 8 | | |
| 4 | WU18: `-fno-exceptions` bare compiles (R4a); version bump; one locked window for H, the matrix, the benchmarks and the history (R12); release audit in parallel; owner walkthrough; the owner merges to `master` and tags | | |

The timing lock is needed only in step 2 (E2 and H) and step 4; nothing else waits on it. The pool is shared by every build and is read before and after each heavy run, as plan 0038 section 6 says.

Agent cost, estimated from the rows of comparable units in `agent-cost.tsv`: lane 1 step 1 about 0.6 M (author 250k, blind 200k, delta 120k, merge 30k); step 2 about 0.5 M (E1 150k, E2 100k, G 80k, H 100k, merges 70k); step 3 about 0.4 M (auditor 250k, snippets 60k, report 100k); lane 2 about 0.8 M (author 350k, review 250k, fixes and merge 200k); lane 3 about 0.2 M; WU18 about 0.5 M (audit 250k, records 150k, runs 100k). About 3 M tokens in all, against a daily spend of 6 to 14 M over the last ten days. Under R6 the review share falls and the total with it.

## 4. Risks in the core code, by reading

Nothing I read is a correctness defect in the mathematics or the arithmetic. The structure checks at generation (`plan.hpp:558-586`, `935-953`, `basis_change.hpp:740-755`, `1012-1013`) are thorough, the kernels are straight-line with every position a constant, and the rounding check holds up (R9). The risks are of the kind that embarrass at 1.0 rather than fail a test.

| Where | What | Kind | Recommendation |
| --- | --- | --- | --- |
| `mv.hpp:392-396`, `725-728` | `TODO` comments in a public header, one deferring a diagnostic "until the long message becomes a problem for a user" | Hygiene; S11 (comments state why, not plans) | Resolve or delete in R1 |
| `plan.hpp:602-674` and the other catalogues of R8 | Compiler-version-specific step counts in header comments | Rot; S11 reader without records | Move to the guide (R8) |
| `linear.hpp:438-477`, `motions.hpp:139-161`, `184-194` | Exact-scalar-type rule rejects integer literals | Friendliness; the likeliest complaint | Keep, document (R10) |
| `product.hpp:416-419`, `unary.hpp:120-128` | `%` and `!` as operators; `!x` is not a bool | Friendliness trap, ratified | Keep, teach (R7) |
| `blade_set.hpp:81-84`, `503-506`; `mv.hpp:1197-1199`; `format.hpp:220-224` | Names, `get`, and printing stop at n = 9, while DoD-1 claims n ≤ 16; a user at n ≥ 10 writes `blade_set` literals by hand (as E3's generator did) | Documentation gap; the release record must state the class covered precisely | State in DoD-1's release text and the guide; a two-digit name syntax is post-1.0 |
| `transform.hpp:100-113`, `549-562` | Soundness under `-ffp-contract`, and coarse types silently unchecked, not stated | Documentation; C8 | Two sentences (R9) |
| `basis_change.hpp:532-553` | `detail::inverse_change`, test-only code in a header (ruled, 0236 item 1) | C12 | Delete in R1 |
| `basis_change.hpp:144-178` | No `operator==`, no hint (ruled, 0236 item 3) | Diagnostics | Add the deleted hint in R1 |
| `transform.hpp:684-768` | An 85-line documentation comment on one function | Readability of the header; the content belongs in the guide | Keep the requires/guarantees; move the conformal(3) chain discussion to the guide (R8) |
| `metric.hpp:130` against `bits.hpp:84-85` | One `GALOOT_ASSERT` message names its function, the others by design do not | Inconsistency, trivial | Leave, or align in R1 |
| `dual.hpp:97-102`, `unary.hpp:64-69`, `transform.hpp:59-64` | ADL hazards of `bulk`, `weight`, `dot`, `reverse`, `transform` under `using namespace std::ranges` | Friendliness; documented in each header | Guide's first page |
| `mv.hpp:912-936`, `transform.hpp:769-817` | Generated kernels can be left out of line by Clang's inliner (E1); forcing them inline shifts GCC's budget (section 6) | Run cost; DoD-24 | R2 |
| `mv.hpp:1051-1062` | `truncate` checks only exact scalars with `==`; for `double` it never checks | By design and documented; a reader of C8 may expect more | One sentence in the guide on `truncate` against `part` |
| `include/galoot/core.hpp` | The umbrella includes `basis_change.hpp` (1,308 lines) for every user, most of whom never change basis | Header cost (P3: "a program includes only the modules it uses") | Measure the umbrella's parse cost in H; consider leaving `basis_change.hpp` out of the umbrella as `format.hpp` is, before 1.0 while the removal is free (owner item) |

Diagnostics: the catch-all pattern (decision 0033) is applied consistently and the messages are in the algebra's terms. Its cost, that requires-expressions are "told yes", is stated in every header; the guide should say it once for generic-code authors.

## 5. What in the remaining plan is not worth doing before 1.0, and what is missing

Not before 1.0 (defer by decision, each with its evidence already recorded):

- Collected plans and the batch interface (decision 0239 item 1; done).
- A fix for the `bulk_dual` growth (R5) and for the antiproduct's (0141 stands).
- `tools/mutate` running its CTest selections in parallel: a tooling convenience with no bearing on the released headers; backlog.
- A compaction of `runs.tsv` (plan 0038 O9, accepted as none in WU17): after 1.0.
- A full refit of the cost model (O6: verify refresh only): after 1.0 or on its triggers.
- The knowledge-base notes from the bounds work and the plane and per-call benchmark rerun (0239 item 2): after 1.0.
- `basis_change`'s remaining spurious overflows (0236 item 2): after 1.0.
- A Terathon or other black-box comparison (0233 item 4): optional, after 1.0.
- Anything under the design study's later sections (geom, mesh, robust, alg, anyon, qsys, sym): out of the core's scope (C2).

Missing from the plan:

- **The guides** (R3): the largest piece of user-facing work with no plan.
- **DoD-21's `-fno-exceptions` evidence, the CMake version, and the install decision** (R4).
- **The design study's status.** `docs/galoot-design-study.md` is user documentation by W6's table, is 996 lines, carries a byline handle rather than a name, and recommends things plan 0001 section 2 records as wrong (dyadic coefficients, `static_assert` text that prints blade names, the fourteen-repository split, the "within 10% of Terathon" gate). A 1.0 user who reads it will take it for the design. Options: move it to `docs/agent/research-notes/` as the historical study (W6 keeps it, W12 keeps process out of user docs), or leave it in `docs/` with a status banner naming plan 0001 as what was built. I recommend the move, in the docs unit (R3). Owner item.
- **The rga3 preset's own limits in the guide:** no `unitize`, no norms, no `exp`/`log` (plan 0001 section 3, out), and `rotation` run-time only (`motions.hpp:92-94`).
- **A statement of the platform the rounding check assumes** (R9) in the release record.
- **The W13 list of removed or changed interfaces:** `basis_change`'s `==` (0235) is the only one so far; the record should say so, and say that 1.0 is the first release so the list is informational.

## 6. The core inlining unit: design

### What the experiment did

A five-function probe (`transform` of a point, a line and a plane by a motor; `Motor{compose(a, b)}`; `Line{p ^ q}`) compiled with each compiler at `-O2 -S` under the S4 flags with checking off, against the committed headers and against scratchpad copies with `[[gnu::always_inline]]` added to different sets of functions. Calls and tail jumps to `galoot` symbols were counted in the assembly. The probe is E1's probe in miniature; nothing here is a time.

| Attribute set | GCC 16.2: calls | Clang 23.1: calls | What remains |
| --- | --- | --- | --- |
| None (committed headers) | 0 | 5 | Clang: tail jumps to `transform` (point, line, plane), `run_product<first_kernel>` inside the line's transform, `run_product<product_kernel>` for compose; the E1 finding reproduced |
| Decision 0238's wording: runners (`run_map`, `run_product`, `run_sum`, `run_change`), `transform_middle`, `transform_outer`, `transform`, `compose` | 1 | 1 | Both compilers now keep `detail::apply<geometric_antiproduct, ...>` out of line in the compose probe: forcing its caller inline moved the inliner's budget |
| The above plus the layout-conversion constructor `mv(const mv<Alg, From, T>&)` | 1 | 1 | The same `apply` |
| The above plus the four `apply` overloads and `detail::sum` (the whole chain) | 0 | 0 | Nothing; checking-mode build also compiles; `-pedantic-errors` bare compile passes |
| `[[gnu::flatten]] [[gnu::always_inline]]` on `transform` and `compose` only, no detail attributes | 0 | 2 | Clang's `flatten` leaves both `run_product` calls; not reliable |

At `-O0 -g` with the whole-chain set, both compilers honor the attribute (GCC's assembly grew 4.6%, Clang's shrank 2%, one compile each; compile wall times were indistinguishable at this size). These are observations about shape, not measurements of record; the unit's W10 record does the measuring.

### Recommended shape

- **Smallest sound change: the attribute on the whole chain**, not on `transform` and `run_product` alone. Twelve functions: `run_map`, `run_product` (`mv.hpp`), `run_sum` (`linear.hpp`), `run_change` (`basis_change.hpp`), the four `apply` overloads (`mv.hpp`), `detail::sum` (`linear.hpp`), `transform_middle`, `transform_outer`, `transform`, `compose` (`transform.hpp`). The public one-line wrappers (`geometric_product`, `wedge`, the operators, `bulk`, `dot`, `truncate`, `part`, `change_basis`, the conversion constructor) were inlined by both compilers in every variant; adding the attribute to them is harmless and guards against a future heuristic, but C12 says wait for the evidence. The acceptance test catches the regression if it comes.
- **One macro, not bare attributes.** `GALOOT_ALWAYS_INLINE` in `assert.hpp`, `[[gnu::always_inline]]` when `__GNUC__` or `__clang__` is defined and empty otherwise, with a comment that says why (P4: the generated kernel is the straight-line code a user would write in place; a call boundary costs 42% to 81% of the operation on Clang, `wu17-bench/README.md`). STYLE S2's "compiler extensions are disabled" is about language modes; a standard-syntax vendor attribute behind a guard is portable C++23 (an unknown attribute is ignored by a conforming compiler, and the guard keeps even the warning away). Decision 0238 item 3 already accepted the attribute in test code.
- **No knob.** A `GALOOT_NO_FORCED_INLINE` has no present user (C12). If the W10 record shows the debug trees' build cost notably up, add it then with the measurement as its reason.
- **Code-size and compile-time risk.** Each call site of a transform gets about 150 instructions at `-O2`; that is what P4 asks for and what GCC already did in E1 for the transforms. In the test suite at `-O0`, every test that calls these operations gains their bodies; the per-TU before-and-after record (W10, P1) on gcc-debug and clang-debug is the evidence, and the criterion of decision 0005 (twice the historical spread) is the one to read it by. GCC refuses with an error when it cannot inline an `always_inline` function (recursion, or inlining across an `-fno-inline` boundary is still allowed); none of these functions recurses. Taking a function's address still works: the compiler emits a body for that.
- **Tests.** (1) Structure, as a CTest on both compilers: compile E1's `probe.cpp` (or the five-function one) with `-O2 -S` and the measure presets' flags and fail on any `call` or `jmp` to a `galoot` symbol; label it with the module so it runs in the matrix, and read it as a structure check under decision 0233 item 10's reasoning, not as a cost assertion (0013). (2) The existing suite: no value changes, so every test passes unchanged; the `rga3.op_counts` tests still hold (the arithmetic is the same). (3) E1 rerun: the Clang `compose` padding lane should go (E1 finding 2 showed 48 with the kernel inlined). (4) E2 rerun: the Galoot rows should move to the reference rows' neighborhood on Clang (inferred from E2's cause table; the rerun decides). (5) W10 before-and-after on both debug trees and both release trees.
- **Mutants for the plan (W3).** Remove the attribute from one function of the chain: the structure test must fail on at least one compiler (the experiment shows `apply` is such a function). A kernel with one term's sign flipped: the existing suite kills it, so it is not a gap mutant; say so.
- **Order with the other edits of R1.** The `basis_change` deletion and hint, the TODOs and the comment trims are independent of the attribute; one branch, with the attribute commit separate so the delta review can read it alone.

## 7. Agent process cost

Where the ceremony costs more than it buys, with the figures from `agent-cost.tsv` and the records:

1. **Review proportion.** 45% of all tokens are review, check and audit; on test-only and tools units the review has cost one to three times the change (R6). W5's delta review is required for core code only; practice applies it everywhere. Recommendation R6(a) to (c).
2. **A separate test author per part.** Decision 0185's mechanics (tests on `--tests`, cherry-picked with `-x`) need a second agent, a second worktree and a merge dance for every part; on WU17 the coordinator waived it twice (0236 item 5) with the mutant probes standing in. Make the waiver the rule for non-core parts (R6(d)); keep it where tests judge core arithmetic.
3. **Worktrees.** Twenty-four worktrees and sixty-six branches exist today, most merged; a cleanup pass cost 74k. Fewer parts per feature (R6(c)) and cleanup at each merge by the merging agent, not a separate pass.
4. **Reruns from protocol slips.** E2 ran twice (reference out of line, then inline), part of F's benchmark went unreported (`wu17-part-f-bench`, "activity after the hand-back"). The protocol-first commit E2's rerun used (`wu17-bench/README.md`, "The protocol, fixed before the run") prevented a third; make it the rule in every measurement brief (R6(f)).
5. **Plan estimates.** Plan 0038 estimated 2.3 M and WU17 is at 4.6 M with G and H to go. The estimate counted authors and one review per part; the delta reviews, fix passes and reruns were the difference. Estimates should carry the review and rerun multiplier the history shows (about 2).
6. **What not to cut.** The eight-preset matrix per `develop` merge costs about 21 minutes of pool time and no tokens; the trace gate, format check and citation check are seconds; the auditor at each gate has found real process defects (MA1 at M5). The mutation specs on core changes are the evidence that the tests can fail (V2) and are worth their cost. The records-only exemption (0215 item 7) is already right.

Everything here is a recommendation; the governance stays the owner's, and none of it needs a governance text change, only briefs and one decision that states the practice.

## 8. Open owner items

Every item that needs a ruling, with my recommendation. Nothing proceeds on these until the owner rules; the lanes of section 3 that need none (the docs draft, the records work) can start.

1. **Reorder decision 0238 item 1** (R1): run the pre-release core pass before the M7 gate, with E1, E2, G and H once at its close. Options: (a) reorder; (b) keep 0238's order and run the measurements a third time. Tradeoff in R1. Recommend (a).
2. **The inlining attribute's extent** (R2, section 6): (a) the whole chain through one macro; (b) 0238's wording, which leaves one call per compiler; (c) `flatten`, which fails on Clang. Recommend (a).
3. **Fold the `basis_change` items, the TODOs and the comment trims into the same unit** (R1, R8): (a) one unit; (b) separate units. Recommend (a).
4. **Claim the documentation unit now** (R3), with the design study moved to `docs/agent/research-notes/` or banner-marked (section 5): (a) move; (b) banner; (c) leave. Recommend (a).
5. **`bulk_dual` growth** (R5): (a) exception beside 0141; (b) a fix before 1.0. Recommend (a).
6. **DoD-21's reading** (R4a): (a) bare compiles with `-fno-exceptions` and a release-record sentence; (b) the whole suite under `-fno-exceptions` with doctest reconfigured. Recommend (a).
7. **`install()` and package config** (R4c): (a) none in 1.0, README names `add_subdirectory` and `FetchContent`; (b) add them in WU18. Recommend (a).
8. **`basis_change.hpp` in the umbrella header** (section 4, last row): (a) keep it in `core.hpp`; (b) leave it out, as `format.hpp` is, before 1.0 while the removal is free. Tradeoff: (b) saves every user who never changes basis the parse of 1,308 lines (unmeasured; H can measure it), at the cost of one more include line for those who do. Recommend measure in H, then decide; if the umbrella's parse cost is dominated by it, (b).
9. **Review proportion for the rest of the road** (R6): adopt (a) to (f) as practice by decision, or keep the current practice. Recommend adopt.
10. **Operators `%` and `!`** (R7): keep or remove before 1.0. Recommend keep.
11. **The scalar-type rule** (R10): keep for 1.0. Recommend keep.
12. **The "S2/S3 unit"** named in the brief: please name what it refers to; I found only WU12's hci items S2 and S3, applied under decision 0223.
13. **Carried from WU17, unchanged:** the errata-naming register row (0234 item 7; no CONVENTIONS change recommended), D8 (ruled, 0239), and the one-line W7 claim for this branch (section 1 item 1).

## 9. What was verified, and how; what was not

- Verified by reading: every header under `include/galoot`, against the governance and the records cited; the rounding-check derivation against its code (R9); the DoD gaps of R4 by grep of the build files; the review-cost figures by `awk` over `agent-cost.tsv`; the run times from `runs.tsv`'s `gate:all` rows.
- Verified by the scratchpad experiment (section 6): which functions must carry the attribute for zero calls on both compilers at `-O2`; that the patched headers compile under the S4 flags, with checking on and off, and under `-pedantic-errors`; that `flatten` alone fails on Clang. Twenty compiles of one probe, not a measurement.
- Not verified: any time, memory or instruction figure of my own; the effect of the attribute on the suite's build cost (the unit's W10 record); the E2 outcome after the change (inferred from E2's cause table); the design study's body beyond its outline and summary; the knowledge base.
- Inferred: the token estimates of section 3 (from comparable rows); that the guides can be written from the header comments without new derivations (the comments hold the content, read in full).

## 10. Reproduction of the experiment

From a checkout at `1fe8834`, with `$S` a scratch directory: copy `include/` to `$S/patched/include`; add `[[gnu::always_inline]]` before `constexpr` on `run_map` and `run_product` (`mv.hpp`), `run_sum` and `sum` (`linear.hpp`), `run_change` (`basis_change.hpp`), the four `apply` overloads (`mv.hpp`), and `transform_middle`, `transform_outer`, `transform`, `compose` (`transform.hpp`); write the five-function probe of section 6 as `probe.cpp`; compile with

```
g++     -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -DGALOOT_CHECKING=0 -O2 -S -I$S/patched/include probe.cpp -o g.s
clang++ -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -ffp-contract=off -DGALOOT_CHECKING=0 -O2 -S -I$S/patched/include probe.cpp -o c.s
grep -cE '^\s+(call|callq|jmp|jmpq)\s+_ZN6galoot' g.s c.s
```

and expect 0 and 0; against the committed `include/` expect 0 and 5. Toolchain: g++ 16.2.1 20260810 and clang++ 23.1.4 with libstdc++ 16, as the records name them.
