# 0012 Remove the growth of a fixed use's cost with n

- Status: closed; the unit merged into `develop` by `fc964a8` on 2026-11-01 (status corrected 2026-11-11, from `git log --merges` on `develop`). Status text as written before that correction, kept as history: claimed 2026-11-01; branch `feature/product-cost-growth`, worktree `.claude/worktrees/product-cost-growth`; decision numbers 0104 to 0109. Implemented, awaiting the W5 review (outcome below).
- Goal: a plan or product over a fixed set of blades must cost the same at n = 4 and n = 16 (PERFORMANCE P2). The W5 review of WU6b (finding F8) measured, pinned with three repeats, the cost of four typed pairs embedded in R(n-1,0,1) from n = 4 to n = 16: +27% on Clang and +16% on GCC. Visiting only the set bits in the per-term loops of `blade_product.hpp` gave +9% on Clang and +0% on GCC (0.180 G instructions at every n); also doing so in `reorder_sign` (`bits.hpp`) gave +7% on Clang. The remaining +7% on Clang is unexplained.
- Definition of done: the loops that visit all n generators for every term visit only the generators involved; `reorder_sign` costs in proportion to the set bits of its first argument; every existing test, golden table, and mutant result is unchanged; the growth with n of a fixed use is measured before and after on both compilers, and the Clang remainder is traced to a cause or recorded as unexplained with what was tried.
- Acceptance criteria: (1) all tests of WU6a and WU6b pass unchanged on the seven presets; (2) no output of `blade_product`, `reorder_sign`, or any plan changes for any input the tests, the golden tables, and an exhaustive comparison against the old code over all masks up to n = 8 and seeded random inputs up to n = 32 can reach; (3) the growth is measured at n = 4, 8, 12, 16 on GCC and Clang, pinned, three repeats, before and after, and recorded in `records/performance/`; (4) the constexpr limits (`full<n>` ceilings and the plan limits recorded in decision 0103) are rechecked and either unchanged or improved, and the statements in the headers updated by this unit if they changed; (5) every new or changed guard has a mutant that fails a test; (6) the cost of `bits.hpp` and `blade_product.hpp` on the unit costs of `core.hpp` and `blade_product_test.cpp` does not rise.
- Approach: first reproduce the review's measurement (its scripts are under `records/reviews/2026-11-01-wu6b-gp-plan/`). Change the loops in `blade_product.hpp` and `reorder_sign` to iterate set bits (`std::countr_zero` and clearing the lowest bit). Keep every public contract. Trace the Clang remainder with `-ftime-trace` and the evaluation count; state what was measured and what is inferred.
- Evidence: test output per preset; the old-against-new exhaustive comparison; mutation records under `records/mutations/`; the pinned measurements.
- Review: W5 blind review (core code), then merge. The owner's role file for the work is `performance-engineer`, with `cpp-expert` for the loops.
- Agent cost: tracked for information only.

## Outcome (2026-11-01, Claude, agent, Opus 5.5, performance-engineer, implementer)

Status: done on `feature/product-cost-growth`, awaiting the W5 blind review. Code and tests at `c97fac5`; all seven presets from fresh trees at `f812191`, whose code and tests are `c97fac5`'s; this section and the W10 after rows are committed after it and change records only. Decisions 0104 (the loops), 0105 (the growth and the Clang remainder), 0106 (the limits); 0107 to 0109 are unused.

### Departures, skipped checks, and ambiguities (C20)

1. Lint was not run per commit until `c97fac5`. The fresh seven-preset run at `115b53e` failed lint: `bugprone-signed-bitwise` at `bits.hpp:107`, because `mask_t{1} << i` had the `int` that `std::countr_zero` returns. So `77ed567` to `115b53e` do not pass lint; their messages do not say so. Fixed in `c97fac5` (an unsigned index, the same value); from it on lint builds clean. After the fix the comparison, the six `reorder_sign` mutants, the bits snippet, the measurements, the trace and probe, and the limits were all rerun at `c97fac5`.
2. The first run-time mutation runs of my driver were invalid and were discarded: `static_assert(true || a && b)` failed the build with `-Wparentheses` under `-Werror` (`plan_test.cpp:471`), so every mutant was "killed" by the warning. The driver now writes `static_assert(true ? true : ...)`; the run-time results are from the corrected driver. The normal-mode results come from the driver at `20dc2ea`, which differs only in that edit, used only in run-time mode.
3. Runs outside `tools/timed` (P13): one direct run of `core_blade_product_test -tc=...` for the new case (sub-second), one `ctest -R tests.reference_markers --output-on-failure` to read a marker failure, the manual compile and run of `compare.cpp` that showed the first capacity abort, the `g++ -fsyntax-only` that wrote the build lines of `out/compare-sanity-77ed567.txt`, and the GCC conformal n = 8 compile at the default limit whose diagnostic is quoted in decision 0106. Their results are either redone through committed scripts or quoted where used.
4. Two commit messages were amended before anything was pushed: `971bc9e` (I had written 172 forms without counting; the count is 210, which agrees with the 4,203,996 products) and `a023495` (the bare -O2 `full<13>` exception was missing). No history was rewritten after that, and nothing has been pushed.
5. The reproduction of the review's figures (`review-p2:*` rows) ran the review's script unchanged, pinned by `taskset` inside the command, so its rows are in shared mode, as the review's were, while two single-threaded Python jobs of other sessions (`z3_checks.py` and `galgebra_symbolic.py` from the math tools) ran on CPUs 4 to 6. My own measurements ran later, pinned on CPU 10, after those jobs had ended: every performance core under 10% busy before each run, and at most 0.02 s of other work on CPU 10 or its sibling during any compile.
6. The old-against-new comparison is a recorded script (`records/performance/product-cost-growth/compare.py`), not a committed test: it needs the old headers, which version control holds, and a test would have to keep the old loops in the test tree (C12). It runs at -O2 with checking on, not under sanitizers (the asan presets ran the suite). Compile-time plans are compared directly only for the typed pairs at n = 4, 8, 12, 16; for other plans the equality rests on `generate_gp` being the same `constexpr` code at compile and run time (the language's guarantee, and test A11), which is inferred, not compared.
7. Beyond the plan: `reorder_sign` is compared on every pair below 2^12 (the plan asks for masks up to n = 8); its `///` comment in the public header now states its cost (a comment change that the review should read); the two `reorder_sign` mutations of `docs/agent/snippets/bits_fault_injection.py` moved to the new loop's bound, and the snippet index records the new check.
8. Defects found outside this unit, reported and not fixed (W9): (a) `blade_set.hpp`'s comment of `full` says Clang 23 builds `full<13>`; at bare `-O2` (libstdc++'s assertions off) it stops at `full<13>`, with the old headers too (decision 0106). (b) The bits snippet's `--runtime-only` mode refuses on clang-debug: removing the `static_assert` lines leaves `all_agree` unused, and Clang's `-Wunused-function` fails the baseline build (`records/mutations/0012-product-cost-growth/survivors.md`).
9. Criterion 6 is met within the spread, not shown below it, for `core.hpp` on Clang: +0.5 M instructions (+0.07%) against spreads of 1.6 and 2.8 M. Rises outside the two units the criterion names: `bits.hpp` alone (+0.4 M GCC, +0.9 M Clang, beyond their spreads) and `bits_test.cpp` on Clang (+17.2 M, +0.58%), and in the shared-mode W10 rows `blade_set_test.cpp` on Clang (+0.34 G, +1.5%). The last two constant-evaluate `reorder_sign` on many small masks (the name parser calls it per index), where the new loop does more per step than the old one did; inferred from what those units evaluate, not traced.
10. For the owner: decision 0105 item 3 reads the Clang remainder as growth that follows from the signature data (P2's exception), because it is the check of every entry of the n × n form, paid once per algebra; confirming or reversing that reading is the owner's.
11. Scope of each commit's checks, as briefed (affected tests plus the fast gates, the full gcc-debug suite at the end of each test-and-code pair): `971bc9e` `core.blade_product` with the gates (markers, citations, golden check, include scan) on gcc-debug and `core.blade_product` on clang-debug, trace gate, format-check; `4e69ce8` the full gcc-debug suite, `core.blade_product`, `core.plan`, `core.smoke`, `core.headers` and the gates on clang-debug, trace gate, format-check; `77ed567` the full suite on both (`bits.hpp` reaches every test), header sets, trace gate, format-check; `c97fac5` the full suite, header sets, and format-check on gcc-debug, clang-debug, and lint, and the trace gate. Record commits build nothing.
12. Job pool: 23 of 23 free at every check: 11:33:29Z, 11:34:56Z, 11:35:16Z, 11:50:46Z, 11:51:18Z, 11:55:22Z, 11:59:47Z and after each of the twelve rerun steps to 12:10:57Z, 12:12:43Z, 12:22:45Z, 12:23:40Z, 12:32:02Z, 12:33:01Z. No slot was lost. I did not stop or start the pool. No CTest ran after a failed build in a tree whose programs a driver had deleted: my driver deletes no programs and skips CTest after a failed build, as the WU6a and WU6b drivers do.

### Done

| Commit | What |
| --- | --- |
| `4e8b55d` | W10 before rows (fresh gcc-debug and clang-debug trees at `0f1039a`), and the review's P2 figures reproduced |
| `971bc9e` | Test: a pairing with e32 in the widest algebra (the tests before the code); `compare.py` and `compare.cpp` |
| `4e69ce8` | Code: the contraction and correction loops of `blade_product.hpp` visit only set bits |
| `77ed567` | Code: `reorder_sign` visits only the set bits of its first argument; snippet anchors |
| `20dc2ea`, `52ed97f`, `9ff402c` | Comparison results, the loop mutation driver and its results, the reruns of the earlier sets |
| `a023495` | Measurements before and after, the Clang trace and probe, the limits |
| `115b53e` | Decisions 0104 to 0106 |
| `c97fac5` | Code: an unsigned index in `reorder_sign` (the lint fix) |
| `f812191` | The evidence rerun at `c97fac5`, cited in the decisions |

### Acceptance criteria

1. Met. Every test of WU6a and WU6b passes unchanged on the seven presets: fresh trees at `f812191`, each 124 of 124; gcc-debug, clang-debug, and lint also built the header sets and format-check clean, and lint's clang-tidy is clean. The trace gate exits 0. One test case was added (the e32 pairing); no existing test or expected value changed (`git diff 0f1039a -- test` shows only that case).
2. Met. No output of `blade_product`, `reorder_sign`, the complement signs, `generate_gp`, or the typed `gp_plan` pairs differs between `0f1039a` and `c97fac5` on either compiler (`out/compare-c97fac5-0f1039a-*.txt`, 0 mismatches; counts in decision 0104). The golden tables are untouched (`git diff 0f1039a -- test/reference` is empty), and the test suite, which compares against them, passes.
3. Met. Pinned on CPU 10, three repeats, n = 4, 8, 12, 16, both compilers, before (`0f1039a`) and after (`c97fac5`), `records/performance/product-cost-growth/out/growth-c97fac5/`: the four typed pairs' plans grow by +15.7% before and +0.2% after on GCC (0.180 G at every n), and by +26.5% before and +6.5% after on Clang. The run at `77ed567` agrees (`out/growth-77ed567/`).
4. Met, unchanged. The `full<n>` ceilings and the plan limits are at the same n as recorded, in four configurations on both compilers (decision 0106); no header statement changed. One pre-existing inaccuracy of the `full` comment is reported (departure 8a).
5. Met. All 16 mutants of the changed loop bounds, including an off-by-one in the set-bit index, a skipped lowest bit, a skipped highest bit, and bit 31 ignored at N = `mask_bits`, are killed on gcc-debug and clang-debug in both modes; without the new test case, `cr_top_bit_ignored` survives, so that case is what kills it. No survivor is unresolved. The earlier sets keep their recorded status, mutant by mutant (`records/mutations/0012-product-cost-growth/survivors.md`).
6. Met within the spread (departure 9): `core.hpp` alone −0.04% on GCC and +0.07% on Clang; `blade_product_test.cpp` −0.29% and −0.05%.

### The Clang remainder

Traced to a cause (decision 0105). Measured: under `-ftime-trace`, every count of evaluations and instantiations is the same at each n; the events that grow are the three conditions of `metric_like` that read all n² Gram entries (`metric.hpp:276` to `:278`), evaluated once in the baseline unit and twice in a unit with one plan or with four; with a probe copy whose `metric_like` keeps only its type test, the Clang growth from n = 4 to 16 falls from about 22 M instructions to under 3 M (within the spread), and as committed one plan grows as much as four. Inferred, not measured: that Clang evaluates the concept a second time because the satisfaction check of the first constrained template does not reuse the value of the concept-id expression in the baseline's `static_assert`; and that a program which never evaluates the concept outside a constraint pays it once.

### Costs (W10; fresh trees, shared mode, track-only)

Before at `0f1039a`, after at `f812191` (code of `c97fac5`); instructions are both kinds of core added; the rows are in `compile.tsv`.

| Unit | gcc-debug before | gcc-debug after | clang-debug before | clang-debug after |
| --- | --- | --- | --- | --- |
| `bits.hpp` alone (header set) | 0.246 G, 38 MiB | 0.247 G, 38 MiB | 0.203 G, 90 MiB | 0.204 G, 90 MiB |
| `blade_product.hpp` alone | 0.693 G, 66 MiB | 0.693 G, 66 MiB | 0.553 G, 100 MiB | 0.555 G, 100 MiB |
| `core.hpp` alone | 0.807 G, 72 MiB | 0.807 G, 72 MiB | 0.748 G, 106 MiB | 0.748 G, 106 MiB |
| `bits_test.cpp` | 3.461 G | 3.471 G | 3.002 G | 3.020 G |
| `blade_product_test.cpp` (after: with the new case) | 8.268 G, 217 MiB | 8.272 G, 216 MiB | 5.319 G, 197 MiB | 5.328 G, 197 MiB |
| `plan_test.cpp` | 9.652 G | 9.553 G | 11.337 G | 11.339 G |
| `blade_set_test.cpp` | 10.574 G | 10.585 G | 22.834 G | 23.178 G |
| All 80 compiles | 93.7 G | 93.7 G | 91.4 G | 91.8 G |
| Build of `all`, wall | 1.44 s | 1.80 s | 1.50 s | 1.67 s |
| CTest, whole suite | 124 tests, 1.50 s | 124 tests, 1.59 s | 124 tests, 1.56 s | 124 tests, 2.26 s |

The wall times are shared-mode and not compared. The pinned figures of criterion 6 are the comparison.

### Verification and validation

Verification: performed by the old-against-new comparison (exhaustive over the stated ranges, seeded random beyond), the unchanged suite on seven presets, and the mutants; passed at `c97fac5`. Validation: not applicable; no result changed. Reproduction, from the worktree root with the pool exported: `python3 -I docs/agent/records/performance/product-cost-growth/compare.py 0f1039a <out>`, `measure_growth.py 0f1039a <out> 3 <cpu>`, `trace_remainder.py <out> 3 <cpu>`, `remainder_probe.py <out> 3 <cpu>`, `limits_recheck.py <out>` (same directory), and `python3 -I docs/agent/records/mutations/0012-product-cost-growth/mutate.py <preset> [--runtime] [--without-top-pairing]`.

### Next

The W5 blind review of `bits.hpp`, `blade_product.hpp`, and the new test case, then the merge into `develop`, which the coordinator makes. Owner questions: decision 0105 item 3 (the P2 reading of the remainder), and departure 8 (the two defects outside the unit). Agent cost: about 160 tool calls; token counts are not visible to me.

## W5 review fixes (2026-11-01, Claude, agent, Opus 5.5)

The W5 review (`records/reviews/2026-11-01-plan-0012-cost-growth.md`, blind-reviewer, Opus 5.5) accepted the change after fixes: the code is correct and every old-against-new comparison matched. This section corrects the outcome above, which is left as written.

### Departures, skipped checks, and ambiguities (C20)

1. The outcome's claim that the growth is gone on GCC and that only Clang has a remainder is wrong (F1). The +0.2% GCC figure holds only because both measured units contain `static_assert(core::metric_like<alg>)`, whose evaluation GCC reuses for the constraint. Without it, the four plans grow from n = 4 to 16 by +9.9% on GCC and +7.0% on Clang (the reviewer's run), and by +9.8% and +6.1% in my own rerun (`records/performance/product-cost-growth/out/probe-no-assert-c97fac5/`, `remainder_probe.py --without-concept-assert`, 13:04:41Z to 13:05:03Z, pinned on CPU 10). The probe with `metric_like` reduced to its type test is flat on both compilers. Decision 0107 supersedes the affected statements of decision 0105 and restates the owner's P2 question for both compilers, with the reviewer's view recorded as input.
2. Six commits do not build under the lint preset, and their messages do not say so (F4): `77ed567`, `20dc2ea`, `52ed97f`, `9ff402c`, `a023495`, and `115b53e`, all with `77ed567`'s build inputs and its `bugprone-signed-bitwise` finding at `bits.hpp:107`. History is not rewritten; the merge message is to name them.
3. Run outside `tools/timed`: none in this pass that I know of; the probe compiles ran through it.
4. The reviewer's figures that I did not rerun myself: the old-header rows of the table in decision 0107 (`0f1039a`, +25.0% and +26.5% without the `static_assert`), the reviewer's trace counts without the `static_assert`, the header-unit figures of F2, and the reviewer's `oob_unchecked.cpp` (F3). They are cited as the reviewer's.

### Corrections to the outcome

- Criterion 3. The before-and-after figures stand as measured, but they are of units that assert the concept. The growth of a fixed use as a program writes it (no such `static_assert`) is, from n = 4 to 16: GCC +25.0% before and +9.9% after, Clang +26.5% before and +7.0% after (the reviewer's run; mine at `c97fac5`: +9.8% and +6.1%). The criterion, to measure the growth before and after on both compilers, is met; the reading that GCC's growth is gone is withdrawn.
- The Clang remainder section. The cause stands, `metric_like` reading the n² Gram entries once per algebra and unit, but it applies to both compilers: GCC pays one evaluation per unit (about 18 M instructions more at n = 16 than at n = 4), and Clang pays one (about 20 to 24 M) plus a second when the unit also evaluates the concept outside a constraint. The inference that a program evaluating the concept only through constraints pays it once is now measured, on both compilers.
- Departure 9 and criterion 6 (F2). The reviewer's pinned run gives `core.hpp` alone +0.6 M (+0.08%) on GCC against a spread of 0.2 M, not −0.04%; the rise is `bits.hpp`'s (+0.7 M alone). So criterion 6 is not met on GCC for `core.hpp` by a margin beyond the spread, by 0.08%. The reviewer notes that differences this small are near what the length of the include path can move (my two header trees had paths of different length), plausible and not tested.
- Departure 10. The owner's question is decision 0107 item 4, for both compilers.
- Decision 0104's "every index stays below N" holds in constant evaluation and checking builds only (F3, decision 0107 item 5); no public path reaches the unchecked case, so no code changed.
- Decision 0105 item 1 and its table quote different runs (F6), and decision 0106's description of GCC's conformal n = 8 diagnostic is from the `77ed567` run (F7); both are corrected by dated notes appended to those decisions.

### Done in this pass

| Commit | What |
| --- | --- |
| `45c2153` | `develop` merged in (agent-cost rows, note 0014, a line in each specialist agent file; no build input) |
| `bea00a1` | F5: `reorder_sign`'s comment says "whatever the indices of its generators"; a public-header comment, for the delta review |
| this commit | Decision 0107; dated notes on decisions 0104, 0105, 0106; `remainder_probe.py --without-concept-assert` and its output; this section |

### Verification at the final revision

All seven presets from fresh trees at `bb5120a` (code and tests of `bea00a1`; the code differs from `c97fac5` only in that comment), 13:07:32Z to 13:08:27Z: each builds `all` and passes 124 of 124, the full gcc-debug suite among them; gcc-debug, clang-debug, and lint also build the header sets and `format-check` clean; lint's clang-tidy is clean; the trace gate exits 0. Job pool: 23 of 23 free at every check in this pass (before the merge, before and after the F5 checks, 13:07:32Z, 13:08:27Z); no slot lost, the pool not stopped or started.

### Next

The delta review of `bea00a1` (decision 0094 item 3), then the merge, whose message names the six commits of item 2. Owner: decision 0107 item 4, and the two defects outside the unit (outcome departure 8).
