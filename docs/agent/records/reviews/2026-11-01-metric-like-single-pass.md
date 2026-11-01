# W5 review: feature/metric-like-single-pass (plan 0014) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), with the cpp-expert role file for the C++ side, 2026-11-01. Agent work.
- Reviewed: `git diff c4b2e70...807369f` (merge base and claim commit `c4b2e70`, head `807369f`, 13 commits). Code and tests at `807369f` equal `a3cd8c5`'s: `git diff a3cd8c5 807369f -- include test CMakeLists.txt cmake tools` is empty. Every changed file was read in full.
- Brief: it gave the branch, the claim commit, the acceptance criteria (plan 0014 sections 2, 3, 5), the owner's ruling, and the author's nine departures, some with the author's reasoning (departures 7 and 8). It carried no verdict on the change. The branch's plan does carry one: its Outcome marks every criterion "Met". I read its first sections (departures and the commit table) by accident while diffing the plan, after my probes and equivalence checks and before my mutation runs finished; I read the rest of it only after my own mutation results were in. So the review is not fully blind to the author's view.
- Departure from the role file: the role file gives the record to the merging agent to commit; the brief asked me to commit it on `review/metric-like-single-pass`, made from the reviewed head, which I did. I changed no file of the change.
- Verdict: **accept** (approve with non-blocking findings). I found no defect in the code. The new verdict equals the old one on every case I tried, on both compilers and on two standard libraries. All 16 of my non-equivalent mutants (of 19) are killed by tests on gcc-debug and clang-debug. The three that survive are equivalent, as I predicted before running them. F1 to F6 are comment, record, and test-cost suggestions; none needs a change to `include/` that changes behavior.
- Job pool: running throughout (11 of 23 slots free before my first build, and saturated at times by other workstreams during my mutation runs). I never started or stopped it.

## Not checked or not run (C20)

- I did not rerun the author's mutation driver (`mutate.py`). I read its result files and compared them with my own runs.
- Presets: I ran gcc-debug, clang-debug, lint, and gcc-asan from new trees at `807369f`. I did not run gcc-release, clang-release, or clang-asan; the author's rows at `d959657` show all seven passing, and their build inputs equal `807369f`'s (P10).
- P2: I did not repeat the pinned run. My instruction counts below are unpinned, on a loaded machine, three repeats. They are retired instructions of the compile process only, which the load does not change.
- I did not rerun the solver check of Theorem 1 (`equivalence_z3.py`). I replaced it with my own exhaustive runs.
- I did not compile on a third compiler. libc++ was checked with Clang only, and only for the probes named below, not for the test suite.
- WU7b (`feature/core-plan-maps`) is not reviewed. I only made the trial merge described below.
- Outside `tools/timed` (P13): my probe compiles (unreadable-value cases, the enumerations, the libc++ checks, the cache pin, the n = 0 diagnostics), the two `-O2` enumeration programs, and the `tests.reference_markers` script run on archives of `6cc0545` and `a3cd8c5`. All of them are recorded here as evidence, with their commands.
- My run-log rows are not committed, as the brief asks. They are in the uncommitted `runs.tsv` of my worktrees: `.claude/worktrees/review-metric-like` (suite, lint, trace gate, the mutants, selections `review-mut:*`), `.claude/worktrees/review-metric-like-asan` (gcc-asan), `.claude/worktrees/review-metric-like-commits` (the per-commit runs, `review-commit:*`), and `.claude/worktrees/review-metric-like-trialmerge` (the trial merge and the cost measurement, `trial-merge:*`, `review-cost:*`). The trial-merge worktree holds an uncommitted merge. Remove it with `git worktree remove --force` once nobody needs it.
- Agent cost: about 85 tool calls. I cannot see the token counts.

## The four W5 checks, and where they ran

All at `807369f`, run from new, empty build trees in my own worktree through `tools/timed`, with GCC 16.2.1 and Clang 23.1.4 (libstdc++ 16):

| Check | Result |
| --- | --- |
| Mechanical style: lint preset (`all` with clang-tidy, warnings as errors, header sets, `format-check`) | pass; build exit 0 in 22.7 s; lint suite 133 of 133 |
| Full suite, gcc-debug | build exit 0; 132 of 132 |
| Full suite, clang-debug | build exit 0; 133 of 133 (`core.plan_evaluated_once` is Clang-only) |
| Sanitizers (P9): gcc-asan | build exit 0; 132 of 132 |
| Trace gate, rerun on a clean tree after the mutation runs (`python3 -I tools/test_references.py trace docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md test/reference/golden.hpp test`, from the root) | exit 0. Items with no test: 3.2 to 3.4, 4.2 to 4.5, 5.x, 6.x, 7.1 (unchanged by this branch) |
| Review | this record |

## Checks and how

### Same verdict, derived independently (criterion 1)

- Theorem 1, which says the one pass equals `is_symmetric() && entries_in_range()`, I proved again by hand. P(G) requires, for every pair j ≤ i, that g_ij = g_ji and that g_ij is in range. Every position (i, j) is covered: if j ≤ i, the pair checks g_ij directly; if j > i, the mirrored pair checks g_ji, which equals g_ij. The converse is immediate. The proof holds.
- My own enumeration used a nine-value set that is not the tests' six: {INT_MIN + 1, −M − 2, −M, −M + 1, 0, M − 1, M, M + 2, INT_MAX − 1}, with M = 32767. It has five values in range, including 0 and both values next to each bound. My predicate is independent of the code: it checks g² ≤ M² in `long long` and compares every ordered pair.
  - At n = 3 (run time, `-O2`): I ran `detail::is_valid_form`, the old members, and my predicate on all 9⁹ = 387,420,489 matrices. There were 0 disagreements on both compilers, and each accepted 15,625 = 5³ · 5³.
  - At n = 1 and 2 (compile time, the concept itself, one tag per matrix, 9 + 6,561 tags): the concept equals my predicate under the old header (`c4b2e70`) and the new one (`a3cd8c5`), on GCC and on Clang, and on Clang with libc++. It accepts 5 and 125. A negative control with the count set to 124 fails as it should.
- Unreadable and unusual values. I compiled 23 tags under the old and the new header on both compilers, and asked for each whether the concept is true, false, or an error. Old and new agree on all 23, on each compiler, and the new header gives the same verdicts on Clang with libc++. No case is an error.
  - False on both compilers: a non-const static; a `static inline const metric<2>` (const, but a class type that is not constexpr); `constinit`; a constexpr reference to a run-time metric at n = 2 and at n = 1; a const reference to a run-time object; `constexpr volatile`; a reference reached through a constexpr pointer to a run-time object; a reference to a `mutable` subobject of a constexpr object; a private `value`; `value` as a static member function; a non-constexpr reference at n = 2; an out-of-range n = 1 value reached by reference; an asymmetric template parameter object; an out-of-range n = 1 template parameter object.
  - True on both: a const non-constexpr reference to a constexpr object; `constexpr thread_local` at n = 2 and at n = 1; a reference through a constexpr pointer to a constexpr object; a reference to a subobject of a constexpr object; a copied template parameter object; a static const member declared in the class and defined `constexpr` later.
  - One case is a compiler divergence that the change did not introduce: see F6.
  - A user type with a throwing or non-constexpr accessor cannot reach the guard, because `is_metric_v` accepts only `metric<N>` itself. The only accessor the guard and the pass call is `std::array::operator[]`. A program that specializes `metric` is outside the threat model (note 0015, decision 0121 question 4).
- Theorem 2 says reading one entry decides compile-time readability exactly as reading all entries does. The conclusion holds, but its step 1 says more than it needs to (see F5). Usability of an entry is decided by the path from the object that `Alg::value` designates, and every entry shares that path, because `metric` and `std::array` have no mutable member and no union. My mutant `r_guard_last_entry` (the guard reads g_(n−1)(n−1) instead of g_00) survives every test, as Theorem 2 predicts.
- Theorem 4 says the cache cannot change a verdict. The conjunction is evaluated in order ([temp.constr.op]), so `is_valid_form_v<Alg>` is instantiated only after the guard has passed. Once readable, the object is const and its value is fixed. For a value declared in the class and defined `constexpr` later, the guard fails before the definition and the cache is not instantiated, so the later true is computed correctly. Case 22 checks this on both compilers. A verdict that changes between points of the program was already ill-formed, no diagnostic required, before this change. I accept the theorem, with a wording note in F5.
- Public interface: the diff to `include/` adds only `detail::is_valid_form`, `detail::is_valid_form_v`, the concept's new body, and its `///` comment. `metric<N>`, `is_symmetric()`, `entries_in_range()`, `is_diagonal()`, `is_null()`, the factories, and `from_gram` with its two messages are byte-identical.
- Old tests: `git diff c4b2e70 a3cd8c5 -- test` contains only additions. The tests written before the code pass on the old header: at `93543c4` and at `30d8391`, gcc-debug passes 132 of 132 and clang-debug 133 of 133, from new trees.

### Departure 7: the n ≥ 1 test and `core.metric_like_no_generators`

- The reasoning is right. Reading `gram[0][0]` of a `metric<0>` is not a constant expression with libstdc++, with or without `_GLIBCXX_ASSERTIONS`, on both compilers. It is not one with libc++ either, with or without `_LIBCPP_HARDENING_MODE_DEBUG`. So no verdict shows the mutant, and only the diagnostic tells the two apart.
- The test is justified. The diagnostic names the requirement in the user's terms (C9), and the standard does not require a library precondition violation to end a constant evaluation, so the verdict for n = 0 should not rest on one.
- It is portable to the second library on this machine. On Clang with libc++, the test source compiled against the new header prints `generator_count` twice and `metric_like` three times; against the header without the count test it prints `generator_count` zero times. So T5 kills the mutant on libc++ as well. My mutant `r_no_count` is killed by `core.metric_like_no_generators` alone, on both presets. This answers the outcome's open item 4 for libc++.

### Mutants: my own, at `807369f`, gcc-debug and clang-debug

The driver is in my scratch directory, not committed. For each mutant it applies one textual change to `metric.hpp`, builds with `ninja -k 0 all` through `tools/timed`, runs the whole suite, restores the header, and records the failing `static_assert`s (file:line), any `-Werror` diagnostic, and the failing CTests. In the run-time mode, every `static_assert(` of `metric_test.cpp` becomes `static_assert(true ? true : `. Line numbers are GCC's; Clang reports the multi-line asserts one line earlier (335 and 337 for 336 and 338).

| Mutant | Change | Normal mode: killed by (GCC lines of `metric_test.cpp`, or CTest), same on both presets | Run-time mode |
| --- | --- | --- | --- |
| `r_mirror_same_entry` | `b` reads g_ij instead of g_ji | `asymmetric` 312, `widest_broken` 326, T1 338, 342 | `core.metric` (T2) |
| `r_symmetry_one_sided` | `a != b` becomes `a < b` | 312, 326, T1 338, 342 | `core.metric` |
| `r_no_range` | both bounds dropped | `entry_too_large` 313, T1 336 to 342 | `core.metric` |
| `r_lower_off_by_one` | `a < -M - 1` | T1 only, 336 to 342 | `core.metric` |
| `r_upper_off_by_one` | `a > M + 1` | 313, T1 336 to 342 | `core.metric` |
| `r_upper_too_tight` | `a > M - 1` | T1 336 to 342; `core.plan_overflow` | `core.metric`, `core.plan_overflow` |
| `r_first_row_skipped` | i starts at 1 | 313, T1 | `core.metric` |
| `r_first_column_skipped` | j starts at 1 | 312, 313, 326, T1 | `core.metric` |
| `r_diagonal_skipped` | `j < i` | 313, T1 | `core.metric` |
| `r_last_row_skipped` | `i + 1 < N` | 312, 313, 326, T1 | `core.metric` |
| `r_upper_triangle` | j runs from i to N − 1 | survives: equivalent | survives |
| `r_no_count` | n ≥ 1 test dropped | `core.metric_like_no_generators` only | not run |
| `r_no_guard` | guard dropped | hard error in the stored verdict, reached from the unreadable-value asserts 307 to 311, 322, 323 | not run |
| `r_guard_no_read` | guard tests `gram.size() > 0`, which reads nothing | hard error, reached from 307, 309, 310, 322, 323 | not run |
| `r_guard_last_entry` | guard reads g_(n−1)(n−1) | survives: equivalent (Theorem 2) | not run |
| `r_cache_before_guard` | stored verdict placed before the guard | hard error, reached from 307 to 311, 322, 323 | not run |
| `r_type_test_structural` | type test becomes `requires { Alg::value.gram; }` | hard error, reached from `lookalike` 306 | not run |
| `r_cache_reads_type` | stored verdict computed from a value-initialized `metric<N>` | 312, 313, 326, T1 | not run |
| `r_no_cache` | concept calls `is_valid_form(Alg::value)` | survives: equivalent in verdict (departure 8, F3) | not run |

That makes 19 mutants: 16 are killed by tests on both presets, and 3 equivalent ones survive. Every mutant of the pass is also killed in the run-time mode by the n = 3 case alone (`core.metric`), so T2 is a test that can fail for every non-equivalent change of the pass.

No mutant was stopped only by a warning: the `-Werror` column is empty in every row. The three survivors are equivalent. `r_upper_triangle` reads each pair from the other triangle, so Theorem 1 holds with i and j swapped. `r_guard_last_entry` follows from Theorem 2. `r_no_cache` changes only cost (see the next section).

I compared these with the author's records (`records/mutations/0014-metric-like-single-pass/`). The author's kill lists for the mutants we share in substance (no range, diagonal skipped, last row skipped, count test, guard, type test) agree with mine. The author records `no_symmetry`'s run-time `-Wunused-variable` honestly, beside the killing `static_assert`s in the normal mode. My two symmetry mutants (`r_mirror_same_entry`, `r_symmetry_one_sided`) are warning-free and are killed in both modes, so the check is shown to be killed by a test without depending on `mirror_read_twice`.

### Departure 8: can a cheap test kill `no_cache`?

Yes, though not by a test that counts evaluations. A test can specialize the stored verdict of one valid tag to false and assert that the concept follows it:

```cpp
struct pinned { static constexpr auto value = core::R<2, 0, 0>(); };
template <> inline constexpr bool core::detail::is_valid_form_v<pinned> = false;
static_assert(!core::metric_like<pinned>);
```

It compiles against `807369f` on both compilers and fails on both under the `no_cache` mutant. It costs nothing measurable. It pins that the concept reads the stored value, which is what the mutant removes. It does not pin that the compiler evaluates the initializer once. The language gives a variable one value, and that the work happens once is compiler behavior, which only the P2 measurement shows. Its drawback: a test that specializes a `detail` template is white-box and has to sit before any use of the tag. See F3.

### Header and test cost (criteria 5 and 6)

My unit includes `metric.hpp`, defines the tag `R<n − 1, 0, 1, 32>`, and has one constrained use, optionally with `static_assert(metric_like<alg>)`. It was built with Galoot's test flags at -O0. Figures are millions of retired instructions, mean of three, minus the same tree's unit with no use. They include the factory's own cost, so they sit above note 0015's concept-only figures by a constant. Spreads are at most 0.2 M on GCC and 2.5 M on Clang.

| | GCC before | GCC after | Clang before | Clang after |
| --- | --- | --- | --- | --- |
| one use, n = 4 / 16 / 32 | 10.7 / 29.2 / 85.9 | 9.7 / 22.1 / 58.6 | 6.3 / 29.5 / 101.7 | 5.2 / 18.9 / 61.9 |
| growth n = 4 to 16 | +18.5 | +12.4 | +23.2 | +13.7 |
| use and `static_assert`, n = 16 / 32 | 29.2 / 85.9 | 22.1 / 58.6 | 53.0 / 194.9 | 19.3 / 62.1 |
| header alone (no use) | 432.6 | 433.0 | 372.2 | 372.9 |

These reproduce the author's direction and size. At n = 16, one check saves 6.7 M on GCC and 10.6 M on Clang. Clang no longer evaluates the concept a second time for the `static_assert` (53.0 → 19.3). The cost of including the header does not move (+0.3 M and +0.7 M, within the spread). The remaining growth of 12 to 14 M is the ruled P2 exception.

`metric_test.cpp` (same flags, `-g`, three repeats): GCC 4,240 → 5,840 M and Clang 2,279 → 7,510 M (3.3×), peak memory +33 MB and +22 MB. This matches the author's W10 rows. With only the n = 2 half of T1 removed, the file costs 4,327 M (GCC) and 2,326 M (Clang). That half is therefore 1.5 of the 1.6 G added on GCC and 5.2 of the 5.2 G added on Clang. See F4.

### Process and records

- Each commit, built from its own tree on new trees (gcc-debug, clang-debug, lint):
  - `93543c4`: 132 and 133 pass; lint fails on three clang-tidy diagnostics from two checks (`bugprone-throwing-static-initialization` at `metric_test.cpp:189` and `:195`, `readability-trailing-comma` at `:231`).
  - `30d8391`: everything passes, lint included.
  - `6cc0545`: only `tests.reference_markers` fails, in all three presets. I confirmed it by running the script on an archive of `6cc0545` (exit 1) and of `a3cd8c5` (exit 0).
  - The departures state all of this correctly, apart from the wording in F2.
- W9: the tests that judge the change (T1, T3, T4, T5) came before the code and pass on the old header. T2 shares a commit with the code because it calls `detail::is_valid_form`, and that commit lists it with its expected values. W7: every commit says what and why and names the agent and model.
- STYLE S11 and decision 0034: no added line in `include/` or `test/` cites a decision, plan, note, provision, or agent work. I checked by reading the lines and by a pattern scan of the added lines (`plan|decision|note N|theorem|W/C/S/P numbers|agent|review|mutant`); the only hit is the identifier `plane`. `tests.citations` passes in my suite runs.
- Decisions:
  - 0107: the appended dated note changes nothing above it. It states that the owner ruled item 4 through decision 0121 item 3, which matches 0121's owner-ruling section.
  - 0121: the claim commit changed only the title suffix and the status line, and added the "Owner ruling" section, which says it supersedes and does not rewrite the proposal. It records A′, no copy and no `bit_cast`, the P2 ruling, and question 4 deferred, all as the brief states them.
  - 0122: records names, order, the n ≥ 1 reasoning, test shape, and the cache survivor. One count is inconsistent with the outcome (F2).
- W10 and cost rows: I reran the author's `w10_summary.py` on the committed `compile.tsv` and `runs.tsv`, and it reproduces the outcome's criterion-6 table. My own `metric_test.cpp` counts agree with those rows within 0.5%. The outcome records the agent cost (about 125 tool calls).

### Readability against the owner's ruling (option A′)

- No flat copy, no `std::bit_cast`, no `sizeof` argument. The pass is the plainest loop that does the job, and its comment states the one non-obvious fact (equal mirrors share one range check).
- Why the guard reads one entry: the comment above it says that an unreadable value would make the stored verdict an error rather than false, and that one entry is readable exactly when the whole value is. A reader new to the project can follow this. It does not say why a failed read inside `requires { … }` gives false rather than an error, which a reader who does not know requires-expressions will not know (F1).
- Why n ≥ 1 comes from the type: the comment says only "Taken from the type alone, so that gram[0][0] exists below." That explains the order (the count before the guard) but not why the type rather than `Alg::value.generator_count`. The reason is that the type is known even when the value cannot be read, or is a non-static member. A newcomer would have to work that out (F1).
- Nothing in the change is a hack. Every non-obvious line has a comment. The variable template's comment is accurate, apart from F1's last point.

### Trial merge with `feature/core-plan-maps` (WU7b)

In the throwaway worktree `.claude/worktrees/review-metric-like-trialmerge`, I ran `git merge --no-commit --no-ff feature/core-plan-maps` (branch head `25c48df`, merge base `7704146`) onto `807369f`. It reported no conflicts: `test/CMakeLists.txt`, `compile.tsv`, and `runs.tsv` auto-merged. The merged tree builds on gcc-debug (140 of 140) and clang-debug (142 of 142), WU7b's `core.plan_map_not_metric_like` included. Nothing was committed. The overlap is textual only, as the author said, on this pair of heads. If either branch moves, the merge should be rerun.

## Findings, most severe first

None blocks the merge.

| # | Where | Finding | Severity | Suggested resolution |
| --- | --- | --- | --- | --- |
| F1 | `include/galoot/core/metric.hpp:303` and `:305` to `:308`; `///` at `:298` and `:299` | (a) The comment on the n ≥ 1 test explains the order but not why the count is read from the type: a reader unfamiliar with the project cannot tell that this is what keeps the test safe for a value that cannot be read, or for a non-static member. (b) The guard's comment does not say that a failed read inside the requires-expression makes the requirement false rather than an error, which is the point of putting the read there. (c) "The matrix is read once per algebra in each translation unit" overstates slightly: the guard reads `gram[0][0]` at every evaluation of the concept (Clang evaluates the concept again as an expression), and only the full check is once. | low, non-blocking (the ruling asks for plain comments on every non-obvious line) | (a) "The number of generators comes from the type, which is known even when the value cannot be read, so the read of gram[0][0] below has an entry to read." (b) Add: "Inside a requires-expression a read that fails makes the requirement false, not an error." (c) "The full check runs once per algebra in each translation unit." Comment-only, so a delta review can be a reading. |
| F2 | test comments `test/core/metric_test.cpp:216` to `:223` and `:556` to `:561`; decision 0122, Consequences; departure 1 wording | (a) The coverage comments overstate. They say any check that agrees with the requirement on the six values agrees on every int matrix. That holds only for a check that, like the requirement, sees an entry only through its class and whether it equals its mirror; for the pass this is known from reading its code (Theorem 1), not from the sample. A check that misbehaves at 0 would pass these tests, and 0 is not among the six values. (WU7a lesson: comments do not overstate.) (b) Decision 0122 counts "the twelve correctness mutants"; the outcome counts 13 out of 14 (with `mirror_read_twice`). (c) Departure 1 says the two commits "fail a check and say so". It is the fix commits (`30d8391`, `a3cd8c5`) and the outcome that say so, not `93543c4` and `6cc0545`, and the lint failure is three diagnostics from two checks. | low, non-blocking | (a) Qualify both comments: "…so a check that, like the requirement, looks at an entry only through its class and its mirror, as the single pass does, agrees on every int matrix." (b) A dated note on 0122, or a line in the outcome. (c) Wording in the review request or the outcome. |
| F3 | departure 8; plan section 6 | The `no_cache` mutant can be killed at no cost and portably by specializing `detail::is_valid_form_v` of one valid tag to false and asserting that the concept is false. Checked on both compilers against `807369f` and the mutant. This guards against a later edit silently dropping the cache, which only a P2 run would notice. It does not test "evaluated once", which is compiler behavior. | low, optional | Add the three lines to `metric_test.cpp` with a comment ("the concept answers from the stored verdict"), or record in decision 0122 why not (it is a white-box test of a `detail` name). I lean toward adding it. |
| F4 | T1 (`metric_test.cpp:333` to `:342`); departure 9 | The n = 2 half of T1 (1,296 tags) is 97% of the added compile cost of `metric_test.cpp`: +1.5 G instructions on GCC and +5.2 G on Clang, about +0.35 s of wall time and +22 to 33 MB per build of that file. No rule is broken (test cost is tracked only), and this half is the only concept-level enumeration with off-diagonal pairs. I would not move it to run time, because a run-time test cannot exercise the concept (guard and cache) and T2 already covers the pass. | recommendation, non-blocking | Keep it, or cut the n = 2 sample to four values {−M − 1, −M, M, M + 1}. Out-of-range pairs reject whether or not they are equal, so four values still give every pattern of classes and equalities that the requirement distinguishes: 256 tags, about a fifth of the cost. INT_MIN and INT_MAX stay at n = 1 and in T2. |
| F5 | research note 0015, Theorem 2 step 1 and Theorem 4 step 3 (not in this diff) | (a) Theorem 2 says usability depends on "the one complete object". It actually depends on the path from the object `Alg::value` designates: a reference to a `mutable` subobject of a constexpr object is unreadable although its complete object is constexpr (my case 14). The conclusion holds because every entry shares that path. (b) Theorem 4 step 3 says `Alg::value` "is a constexpr object". It may also be a template parameter object, or what a const reference names; all are const, so the step stands. | informational | An addendum to the note if it is revised; no code change. |
| F6 | GCC 16 (outside the unit) | `static constexpr const metric<2>& value = R<2, 0, 0>();` (a reference to a lifetime-extended temporary) gives `metric_like` false on GCC and true on Clang, before and after this change. Minimal reproducer without Galoot: a namespace-scope `constexpr const P& g = make();` then `static_assert(g.a[0] == 1);` fails on GCC ("‘<temporary>’ was not declared ‘constexpr’") and passes on Clang. As I recall [expr.const], such a temporary (const-qualified literal type, extended by a variable usable in constant expressions) is usable, so Clang's reading looks right. I recall the clause and have not re-read it. | informational; defect outside the unit (W9), reported to the owner | Note in the record; optionally a known-divergence note for users who write tags this way. No Galoot test uses the form. Do not contact the compiler's maintainers. |

## Criteria (plan 0014 section 3)

| # | Criterion | My finding |
| --- | --- | --- |
| 1 | Same verdict | met: T1 and T2 pass at `807369f` on both compilers; independently, nine values for n ≤ 3, concept-level for n ≤ 2 under old and new, both compilers and libc++ |
| 2 | False, not an error | met: the five existing tags and the four new ones, plus my 23 cases, none an error |
| 3 | Diagnostics | met: `core.plan_not_metric_like` and `core.metric_like_no_generators` pass on gcc-debug, clang-debug, lint, and gcc-asan; T5 also on Clang with libc++ (probe) |
| 4 | Mutants killed by tests, never by a warning | met: the author's records read and agree with mine; my 16 non-equivalent mutants are all killed by tests on both presets, and the run-time mode kills all ten non-equivalent pass mutants through T2 alone |
| 5 | P2 | met in direction and size (my unpinned counts); I did not rerun the pinned run |
| 6 | W10 | met: rows reproduced by the author's script; my `metric_test.cpp` counts agree |
| 7 | Checks at every commit, seven presets | met with departures 1 and 3, both stated (F2 c for wording); I ran four of the seven presets at `807369f` |
| 8 | No citations | met |

## Revisions

- `807369f` (code and tests equal to `a3cd8c5`): the four W5 checks, gcc-asan, the mutants, the probes against the extracted `include/` of `a3cd8c5`.
- `c4b2e70`: the old header for the old-against-new probes and cost.
- `93543c4`, `30d8391`, `6cc0545`: per-commit gcc-debug, clang-debug, and lint.
- Trial merge: `807369f` with `feature/core-plan-maps` at `25c48df`, uncommitted.

## Delta review of the review-fix commits (c21a7f8, 6211734, 5fb4bf6)

Reviewed by the coordinating agent (Claude Sonnet 5.5), 2026-11-01. The diff of `include/` from `807369f` to `7d57f51` has no line that is not a comment. The new comments in `metric.hpp` say why the generator count comes from the type, that a failed read inside a requires-expression gives false, and that the full check runs once per algebra in each translation unit; each matches the code and the review's findings. The added test specializes a `detail` variable template on purpose, says so, and states the expected value (false). No finding.
