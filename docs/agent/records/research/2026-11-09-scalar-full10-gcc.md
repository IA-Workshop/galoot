# Scalar times full<10> on GCC 16: the plan's result placement exceeds the operation limit

- Written by: Claude (agent, Opus 5.5, cpp-expert role, researcher), 2026-11-10 UTC, for decision 0226 item 3, on `records/scalar-full10-gcc` from `develop` at `ba16a02`.
- Headers: `develop` at `ba16a02` (before the kernel-init change) and `feature/core-kernel-init` at `1668070` (its `include/` is that of `6d00878`, after the change), the second extracted read-only with `git archive`.
- Compilers: GCC 16.2.1 20260810 and Clang 23.1.4, libstdc++ 16, on the development machine.
- Files: everything this record cites is in `2026-11-09-scalar-full10-gcc/` beside it. Every compile and run is a row of its campaign log `runs.tsv` (P13); the scripts `cases.sh`, `minimal.sh`, `minimal_sweep.sh`, and `bisect.sh` rerun them from the worktree root, and `results/` holds each case's diagnostics.
- Result in one paragraph: the failure is GCC's operation limit, not a defect of the plan builder and not a wrong rejection. The plan of the scalar times `full<10>` has T = 1,024 terms on R = 1,024 result blades, and `collect_result` and `type_terms` each scan linearly, so each costs about T·R/2 comparisons: 45,105,146 operations for the collection and 89,102,056 for the typing, against GCC's default of 33,554,432. GCC reports the exhaustion as libstdc++'s `std::span` bounds check being "not a constant expression" because the count runs out inside that check's `__builtin_expect`, a known GCC diagnostic defect. Every index is in range: the unit builds with `-fconstexpr-ops-limit=134217728`, Clang builds the same plan with more steps, and the phases run clean at run time under libstdc++'s assertions and the sanitizers.

## 1. Departures, skipped checks, and ambiguities (C20)

1. **Two compiles are missing from the campaign log.** My first two compiles (the N = 10 unit, and an N = 9 unit whose macro `N` clashed with the library's template parameters) wrote their rows to `runs.tsv`, and I deleted that file before the first real run of `cases.sh`. The macro is now `GENERATORS`. Every later compile and run is in the log.
2. **76 void rows.** The first runs of `bisect.sh` passed the limit to `g++` as an extra file name as well as in the flag, so every compile also failed on a missing input. The rows are the `scalar-full10:bisect-unit:*` and `scalar-full10:bisect-collect:*` selections of `runs.tsv`; their tables are kept in `results/void/` and not used. The fixed script reran as `bisect-unit2` and `bisect-collect2`. The first run of `runtime_check.cpp` (`runtime-check-gcc`, exit 1, `results/void/runtime-check-gcc-first.txt`) is void too: it expected a term capacity of 1, and the plan's capacity is 512 (section 4.2). The rerun is `runtime-check-gcc2`.
3. **Two sources were reformatted after they ran.** `plan.cpp` and `runtime_check.cpp` were put through `clang-format` after their runs; only the layout changed.
4. **No change to `include/` or `test/`**, as the brief requires. The fix of section 5 is a proposal; no test was written for it.
5. **Ambiguity: "the documented limits (design study and docs: 1,024-term plans)".** The design study states no limit on the size of a plan. The 1,024-term figures are in `apply`'s comment (`mv.hpp`, near line 866), where they refer to Clang's limit on a fold, and in decision 0154 item 5. I checked the failure against what the headers do state: `product_plan`'s comment (`plan.hpp`, near line 615) and `mv`'s (`mv.hpp`, near line 470). Section 3.4 has the comparison.
6. **Clang's limits were outside the brief.** I compiled the plan alone on Clang, without `mv`, to get a second constant evaluator's verdict on the same code (section 3.3). That also gave Clang's ceiling for this product, which is in section 3.3. The `mv` construction that stops Clang at `full<9>` was not investigated.
7. **Not done: merging to `develop`.** That is the coordinator's step; section 6 lists what it needs.

## 2. What was asked and what was found

| Question (decision 0226 item 3, the brief) | Answer |
| --- | --- |
| A Galoot plan-builder defect (an index out of range, a wrong capacity)? | No. No index leaves its range (section 3.3). The term capacity, 512, is larger than needed for a scalar factor but not wrong (section 4.2) |
| A GCC constant-evaluation limit? | Yes: `-fconstexpr-ops-limit` (default 2^25 = 33,554,432). `-fconstexpr-loop-limit` and `-fconstexpr-depth` do not bind (section 3.1) |
| A GCC bug? | The rejection is a conforming implementation limit. The wording of the error is a GCC diagnostic defect, already in the knowledge base (section 3.2) |
| Minimal reproducer | `plan.cpp`: 19 lines of Galoot, with no `mv`. `minimal.cpp`: 52 lines, no Galoot, with the misleading wording at chosen limits (section 3.2) |
| Within the documented limits? | The headers say placement costs a fixed amount times T times R and dominates large sets, so the failure is predicted. They give no T·R ceiling, though, and from the figures they do give (`full<6>` × `full<6>`, 4,096 terms, builds on GCC) a reader would expect this plan to build (section 3.4) |
| Before and after the kernel-init change | It fails alike on both sets of headers and builds alike on both at 2^27 operations (section 3.1) |
| Backlog | Yes, as a deferred core item (section 6) |

## 3. Evidence

### 3.1 The unit, the flags, and the limits

`product.cpp` is the kernel-init cost campaign's unit: `galoot::core::geometric_product` of an `mv` on the scalar layout and an `mv` on `full<GENERATORS>` of R(GENERATORS,0,0), over double. `plan.cpp` keeps only the product plan. "Debug" means the flags of the gcc-debug tree (`-O0 -D_GLIBCXX_ASSERTIONS -DGALOOT_CHECKING=1`, warnings as errors), and "release" means the same flags at `-O2` without `_GLIBCXX_ASSERTIONS`. Each compile is `-fsyntax-only`.

| Case (`results/<name>.txt`) | Exit | First error |
| --- | --- | --- |
| `debug-n9`, `release-n9`, `plan-debug-n9` | 0 | none |
| `debug-n10`, `plan-debug-n10` | 1 | `span:288`: "`__builtin_expect((__idx >= ...size()), 0)` is not a constant expression", reached through `collected` → `collect_result` → `out[at]` at `plan.hpp:313` |
| `release-n10` | 1 | "operation count exceeds limit of 33554432" at `blade_set.hpp:107`, the end of `canonical_less`, in the same phase |
| `debug-n10-loop-2p24` (`-fconstexpr-loop-limit=16777216`) | 1 | the same as `debug-n10` |
| `debug-n10-depth-4096` (`-fconstexpr-depth=4096`) | 1 | the same as `debug-n10` |
| `debug-n10-ops-2p26` (`-fconstexpr-ops-limit=67108864`) | 1 | "operation count exceeds limit of 67108864" at `plan.hpp:362`, in `type_terms`: the collection now completes and the typing stops |
| `release-n10-ops-2p26` | 1 | the same limit message at `span:254`, in the typing |
| `debug-n10-ops-2p27`, `debug-n10-ops-2p27p1`, `plan-debug-n10-ops-2p27` (2^27, 2^27 + 1) | 0 | none |
| `kernel-init-debug-n10-default`, `kernel-init-debug-n10-134217728` (headers of `6d00878`) | 1, 0 | the same as `debug-n10`; none |

Thresholds, from `bisect.sh` (bisection between a failing and a passing limit, then compiles at +1 to +127 and +0.01 % to +10 % above the result, all of which held, as note 0017 section 3.1 requires because GCC's limit is not monotone). GCC's cache of constant calls was left on, as GCC builds by default. A threshold is the operation count plus one.

| Evaluation, debug flags | Operations | Times the default limit | Table |
| --- | --- | --- | --- |
| `collected` (the collection), n = 9 | 11,330,152 | 0.34 | `results/bisect-collect-n9.tsv` |
| `collected`, n = 10 | 45,105,146 | 1.34 | `results/bisect-collect2.tsv` |
| the whole unit, n = 10 (its largest evaluation, the typing) | 89,102,056 | 2.66 | `results/bisect-unit2.tsv` |

The collection grows by 3.98 from n = 9 to n = 10, close to the factor of 4 of T(T−1)/2 when T doubles: about 86 operations per comparison of the scan at both sizes. At n = 10 the unit took 2.7 s and 0.59 GiB to fail at the default limit, and 4.9 s and 1.08 GiB to build at 2^27 (`runs.tsv`).

### 3.2 Why the error names a bounds check

`std::span::operator[]` in libstdc++ 16 checks its index with `__glibcxx_assert(__idx < size())`, which expands to a `__builtin_expect` test of the negated condition. The knowledge base records, from GCC's source (`upstream-awareness.md`, the `-fconstexpr-ops-limit` row; `performance-engineering/constant-evaluation-cost-models.md`), that GCC evaluates a built-in's arguments quietly. When the operation count runs out there, GCC either drops the error and lets the evaluation finish unlimited (note 0017 section 3.3), or reports only that the `__builtin_expect` call is not a constant expression. This unit shows the second case.

`minimal.cpp` shows that the wording depends only on where the count runs out. It has no Galoot code: an insertion of K ordered keys into a sorted list through `std::span`, the loop `collect_result` runs. At K = 1,024, 2,048, and 4,096 and the default limit it fails with the plain limit message, with and without `_GLIBCXX_ASSERTIONS` (`results/minimal-k*.txt`), and Clang fails it too on its step limit (`minimal-k4096-clang-assert`). `minimal_sweep.sh` found the threshold of K = 256 with assertions at `-O0` (2,632,384) and compiled at each of the 300 limits below it (`results/minimal-sweep.tsv`). The outcomes repeat with a period of 79 limits, one pass of the scan loop: 50 limits give "operation count exceeds limit", 28 give "`__builtin_expect(...)` is not a constant expression" at `span:288`, and 1 gives no reason at all, only the failing `static_assert` (`results/minimal-k256-ops-2632100.txt`, `-2632150.txt`, `-2632165.txt`). None of the 300 passed.

So 28 of each 79 positions in the scan produce the message of `debug-n10`. The message points at a bounds check whose condition holds, as the knowledge base records for `array:210`. Rejecting an evaluation that exceeds an implementation limit conforms to the standard: an evaluation that would exceed an implementation-defined limit is not a core constant expression ([expr.const]; [implimits] suggests at least 1,048,576 full-expressions in one core constant expression, and GCC's limit is far above that) [background]. The wording is a quality-of-implementation defect of GCC, not wrong code. No report was sent (policy: no upstream contact).

### 3.3 No index out of range

- **Raised limits build.** If an index were out of range, the assertion would fail at every limit. The unit builds at 2^27 operations, on the headers before and after the kernel-init change.
- **A second evaluator agrees.** Clang 23.1.4 rejects `plan.cpp` at n = 10 at its default of 1,048,576 steps ("constexpr evaluation hit maximum step limit", in `collect_result`) and builds it with `-fconstexpr-steps=1073741824` in 5.1 s (`results/plan-clang-debug-n10*.txt`). Clang diagnoses a failed libstdc++ assertion at any step limit.
- **Run time, checked.** `runtime_check.cpp` runs `generate_plan`, `collect_result`, and `type_terms` at run time, built at `-O0` with `_GLIBCXX_ASSERTIONS`, ASan, and UBSan, with no recovery. It reports capacity 512, 1,024 terms, 1,024 result blades in strictly increasing canonical order, and every typed term inside the result, and exits 0 (`results/runtime-check-gcc.txt`).
- **By reading** `collect_result` (`plan.hpp:308-327`): `out[at]` is read only after `at < size`. The shift writes `out[size]` only after `GALOOT_ASSERT(size < out.size())`. `collected` gives `out` the plan's T places, and T places always suffice, because each term adds at most one blade.

Clang's ceiling for this plan, on the plan alone with debug flags: n = 7 builds; n = 8 and n = 9 fail in `collect_result` (`results/plan-clang-debug-n{7,8,9}.txt`). So on Clang the product of a scalar and `full<8>` fails, although `mv` on `full<8>` builds there (`mv`'s comment). The placement is the binding limit on both compilers.

### 3.4 Against the stated limits

`product_plan`'s comment says: "Placing the T terms on the R blades of the result costs at most a fixed amount times T times R, and is the largest part for large sets." Its tables cover `full<n>` with `full<n>`, where R stays at 2^n while T grows to 4^n. The largest GCC case it says builds is `full<6>` × `full<6>`: T = 4,096 and R = 64, so T·R = 262,144, for 25.8 M operations cold. The scalar times `full<10>` has T·R = 1,048,576, four times as much, so the comment's own model predicts this failure. The comment does not say so, though. `apply`'s comment ("The plans build up to full<6> times full<6> (4,096 terms) on GCC 16") and `mv`'s (an `mv` on `full<10>` builds on GCC 16) read as if a 1,024-term product of two buildable layouts would build. A user who meets this failure gets a message about a span bounds check and nothing about plan size or the remedy.

## 4. Findings

### 4.1 F1: the placement is quadratic in use (the cause)

`collect_result` inserts each term's blade by a linear scan of the result so far, and `type_terms` finds each term's position by a linear scan of the result: O(T·R) each, as the comment states. For a small set times a large one, T and R are both the size of the large set, so the cost grows as the square of the use. It is the binding limit for the scalar times `full<n>` at n = 10 on GCC and n = 8 on Clang, below the sizes at which `mv` itself builds. This is consistent with C6 as written (the cost follows the sets used, not the dimension of the algebra), but it is the largest cost for exactly the dense products a large layout needs.

### 4.2 F2: the term capacity of a pair with the scalar is loose (not a cause)

`product_capacity(a, b)` is 2^(|A ∪ B| − 1), a bound for any metric. For the scalar times e_1...e_10 it gives 512, though the scalar times any blade is one term in every metric. So `generate_plan` builds a `term_list<512>` for each of the 1,024 pairs, twice (count and fill). This adds work to the generation phases but is not what fails; it is correct as a bound. Tightening it to 1 when either mask is 0 would be part of the fix, not a defect fix.

### 4.3 F3: GCC's wording (known)

Already recorded in the knowledge base (2026-11-03). Two details are new: the period-79 pattern of `minimal_sweep.sh`, and a third symptom, a failure with no reason at all. These could be added to `upstream-awareness.md`, which is the knowledge base's business, not this branch's.

## 5. Proposed fix (not applied)

It is a core change, so it needs its own unit with a W3 plan, tests first on a `--tests` branch (decision 0185), and a W5 review.

**Change.** In `collect_result`, append when the term's blade follows the last result blade (true for every term of a small set times a canonical set). Otherwise find the place by binary search under `canonical_less`, then shift as now. In `type_terms`, find each position by binary search over the result, which is in strictly increasing canonical order (checked by `typed_defects_of`). Optionally, make `product_capacity` 1 when `a` or `b` is 0 (F2). The direct linear forms stay in the tests as the reference (C12, C3).

**Tests, before the change:**

1. A compile test that builds `product_plan` of the scalar times `full<10>`, and of `full<10>` times the scalar, on GCC at the default limits, and the same at `full<8>` on Clang, and checks that the typed plan is the identity typing: the result is `full<n>`, and term k is (0, k, k, +1), or (k, 0, k, +1) for the reversed order. It is committed labelled failing (W2, decision 0216).
2. Equivalence with the direct forms, as constant expressions and at run time, over plans whose terms arrive out of canonical order: sets listed in reverse order, random sparse sets, sets with a repeated mask, and the antiproducts, whose runs are read from the last. Both outputs must be equal.
3. Mutants of the new code: binary-search bounds off by one, `<` for `<=` in the search, the fast path's comparison reversed, the fast path without its equality check, and (for F2) a capacity of 1 for a non-scalar pair. Each must be killed by the tests above, and the existing plan tests must still kill the existing mutants.
4. Refit of the stated counts: `product_plan`'s comment, the `plan_cost_*` canaries of decision 0132, and the ceilings of decisions 0113 and 0156 all change with the cost of the phases, so the calibration of note 0017 must be redone for the changed phases.

**Plan outline:** (0) claim on `develop`; (1) tests branch: tests 1 and 2, the first labelled failing; (2) unit: cherry-pick, then the change in its own commit; (3) mutants, the refit, the build cost before and after of the plan's test units and of `product.cpp` at n = 9 and n = 10 (W10, P1); (4) the comments of `product_plan`, `apply`, and `geometric_product` restate the limits as measured; (5) the P9 matrix and review.

**Expected effect (inferred from note 0017 section 4, not measured):** the scalar times `full<n>` would build wherever `mv` on `full<n>` builds (GCC to n = 10, Clang to n = 8). The ceilings of `full<n>` × `full<n>` would not move, because generation sets them once the typing is cheap.

## 6. Recommendation, backlog, and what a merge needs

| Option | Gains | Costs |
| --- | --- | --- |
| A. Document only: state in `product_plan`'s and `apply`'s comments that placement grows as T·R, give the scalar times `full<10>` (GCC) and times `full<8>` (Clang) as cases over the default limits, the flag that admits them (`-fconstexpr-ops-limit=134217728` on GCC), and that GCC may word the failure as a bounds check | Cheap: a comment-only change, which needs no delta review (W5); nothing to recalibrate; users get the cause and the remedy | The limit stays; users of large dense layouts need the flag |
| B. The fix of section 5 | Moves the ceiling for small-times-large products to the `mv` ceiling; less build time for every product with a large result | A core unit with tests first, mutants, and a refit of note 0017's calibration and the canaries; no present user needs n ≥ 8 dense products (rga3 is n = 4, cga3 n = 5) |
| C. Nothing beyond this record | No cost | The failure stays unexplained where users read |

**Recommendation: A now, B on the backlog.** C12 holds optimizations until the requirement is real, and no present user needs these sizes. The comment fix makes the failure explicable (C8, C9 in spirit) at almost no cost. B should be done when a model with eight or more generators and dense layouts arrives, or together with the next refit of the plan counts, so the recalibration is paid once. A static assertion that predicts the limit was considered and rejected: counts differ by compiler, version, flags, and library, so it would be a hidden rule (C8) and would refuse units that a raised limit admits.

**Backlog: yes.** Galoot keeps no backlog file. Deferred core items live as follow-ups in plans and handoffs. The coordinator should carry B, and A if it is not scheduled now, into the next handoff's open items. In the math-tools backlog, item (3) "GCC ceiling monotonicity" of the open-work list already covers GCC's wording. The coordinator may add a pointer to section 3.2 of this record there; that backlog is the coordinator's to edit.

**For the merge into `develop`** (W7 allows a `records/<name>` branch to merge into `develop`; W5 asks a records-only merge for the P9 checks its files can affect, at least the citation and style checks, and no independent review): `tests.citations` scans `include/`, `cmake/`, `test/`, `tools/`, and the build files, not `docs/agent/records/`, so no file here is in its scope. Run over this record's directory anyway, the check flags only the `docs/agent/...` paths that `tools/timed` writes into two saved diagnostics. Those paths are inside records, where the check does not apply. The C++ sources here are formatted (S3) but are not built by any target. `runs.tsv` beside this record is a campaign log. That run of the citation check added one row to the run log, `docs/agent/records/performance/runs.tsv`, so `tools/runlog check` ran on this branch and passed ("1727 kept rows present once; no removed row present").
