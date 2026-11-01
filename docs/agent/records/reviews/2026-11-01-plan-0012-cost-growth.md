# W5 review: feature/product-cost-growth (plan 0012) into develop

- Reviewer: blind-reviewer, Claude Opus 5.5 (`claude-opus-5-5`), 2026-11-01. Agent work.
- Reviewed: `git diff develop...feature/product-cost-growth`, merge base `0f1039a`, head `58f4aa1` (12 commits; code and tests are `c97fac5`'s, `f812191` and `58f4aa1` change records only). `develop` is at `860f9c9`, four commits ahead with records only (note 0014, agent-cost rows); `git merge-tree --write-tree develop 58f4aa1` merges clean. No merge was made.
- Brief: it named the plan, the governance, nine priorities, the author's departures (the lint gap of `77ed567..115b53e`), and called the outcome and decisions 0104 to 0106 claims to check. It carried no verdict of its own. The plan's outcome does (criteria marked "Met"); I read the diff, the headers, and the test first, and the outcome and decisions after.
- Departure from the role file: the role file gives the reviewer no write tools and has the merging agent commit the record; the brief asked me to write and commit this record and its scripts on the branch, which I did. I changed no file of the change itself.
- Verdict: **accept after fixes**. The code is correct: no output changed on any input I compared, old against new, on both compilers, and the new headers reproduce the exact-oracle tables byte for byte. F1 is a misstatement in decision 0105 and the plan outcome that the owner's P2 question rests on, and should be corrected (by a superseding decision, since decisions are not rewritten) before the merge. F2 to F7 are minor and block nothing.
- Job pool: 23 of 23 free at 12:36:34Z, 12:41:06Z, 12:48:33Z, 12:50:06Z, 12:53:12Z, 12:54:08Z, after the snippet runs, and 12:57:32Z. At 12:46:03Z it showed 0 of 23 free, at 12:46:47Z 14 and at 12:47:24Z 16, with four `make` clients of another session (pid 2863166, `make` under `exercises/numerical/tools-domains/install/sollya.sh` in `math-tools/src/sollya/fplll-5.5.0`, started about 12:46:00Z); those slots were back by 12:48:33Z. No slot was lost. I did not stop or start the pool. No CTest ran after a failed build in any tree (my drivers delete no programs and skip CTest after a failed build).

## Departures, skipped checks, and ambiguities (C20)

1. My first seven-preset run at the head (12:46:03Z to 12:46:47Z) configured with `-DCMAKE_COLOR_DIAGNOSTICS=OFF`. That flag reaches the header-set compile command, so `tests.compile_fail_flags` failed in every preset (123 of 124). My error, not the change's. I deleted those seven trees (`.claude/worktrees/review-0012-head/build`) and reran all seven from fresh trees without the flag (12:47:24Z to 12:48:33Z); the results below are from the rerun. The failed run's rows stay in `runs.tsv` (exit 8, selection `all`, 12:46Z).
2. Runs outside `tools/timed` (P13): the two executions of `oob_unchecked.cpp`, the oracle `dump_tables` executions (their builds were timed), `cmp`/`diff` of outputs, the reading of trace JSON, and `git merge-tree`. Each is sub-second; outputs are quoted below or committed under `out/`.
3. I cleared `*.log` files in the session scratch `logs/` directory before my rerun. As far as I saw, it held only my first run's logs, but that scratch area is shared with other agents of this session; nothing there is relied upon.
4. Ambiguity, "run rows only in your worktree copy": I ran every build and measurement in scratch worktrees (`.claude/worktrees/review-0012-{head,mut,old,971bc9e,4e69ce8,77ed567}`), then appended their 897 rows, sorted by start time, to this branch's `runs.tsv` in the record commit, as the WU6b review did. No row went to the main checkout.

## Not checked or not run

- The earlier mutation sets (WU6a's 42, WU6b's 49, the WU6b review's 27) were not rerun; I relied on the author's rerun. I ran the 16 loop mutants, the bits snippet, and 15 of my own, on gcc-debug and clang-debug only (not release, asan, or lint).
- The W10 before and after rows in `compile.tsv` were not recomputed.
- Per-commit builds: gcc-debug, clang-debug, and lint at the three code commits; not release or asan per commit. The record-only commits were not rebuilt one by one; their build inputs equal a built commit (`out/per-commit.txt`).
- Clang's satisfaction cache was not read in Clang's source; what the trace and probe show is below.
- The oracle trial's `reorder` mode was not run (no stored output); `reorder_sign` and the complement signs were checked against a direct count instead.
- The old-against-new comparison ran at -O2 with checking on, and under Clang's sanitizers; not under GCC's.
- Agent cost: about 60 tool calls; token counts are not visible to me.

## Checks and how

### Behavior preserved, old against new (priority 1)

`old_new.cpp` (this record's directory) is one source built twice, against the headers of `develop` (`860f9c9`, whose `include/` equals `0f1039a`'s) and of `58f4aa1`, with `-std=c++23 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Werror -O2 -D_GLIBCXX_ASSERTIONS -DGALOOT_CHECKING=1`, on GCC 16.2.1 and Clang 23.1.4. It shares no code with the author's `compare.cpp`. It prints:

- `reorder_sign` on every pair of masks below 2^12 (16,777,216, hashed in eight blocks and whole), on every pair of 14 edge masks (0, 1, 2, 3, bit 31, bits 30 and 31, bits 0 and 31, 0x7FFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xAAAAAAAA, 0x55555555, bit 16, 0xFFFF) printed in full, and 100,000 seeded random pairs at each n from 13 to 32 with e_n forced in a quarter of `a` and a seventh of `b`. Each sampled value (about 1.8 M of the exhaustive pairs, every edge pair, every random pair) is also compared with a direct count of the pairs i > j over 32 bits: 0 disagreements.
- `complement_mask`, `right_complement_sign`, `left_complement_sign` for every mask at n = 1 to 14 and 20,004 masks (0, the antiscalar, e_n alone, all but e_n, random) at each n from 15 to 32, each sign compared with the direct count: 0 disagreements.
- `blade_product` over every pair of masks for 216 forms of n = 1 to 8: every R(p, q, r), a Witt pair on e1 and e_n, a null pair g(e_{n−1}, e_n) = −1 after n − 2 Euclidean generators, the form with 2 on the diagonal and 1 elsewhere, three seeded random forms per n (entries −3 to 3, with zeros), and conformal(1) to conformal(6): 4,777,737 terms, hashed per form.
- Seeded random products at n = 9 to 32 (three forms per n, one with g(e1, e_n) = 2; 12,000 pairs per n, grades up to 5, e_n forced often).
- Run-time `generate_gp` plans of every blade times every blade with seeded signs (R(3,0,1), conformal(3), a random form and R(5,0,1) at n = 6), and compile-time values: eight products in a 32-generator form with g(e1, e32) = 1, g(e31, e32) = −1, g(e6, e6) = 1 (every pair reads bit 31), a 64 × 64 `reorder_sign` table and nine edge values evaluated in constant evaluation, and five `gp_plan`s (rigid R(15,0,1) point × motor and motor × motor, conformal(3) and the dense n = 4 form full × full, and the degenerate form [[0,1,0],[1,0,0],[0,0,0]] full × full).

Result (`out/old_new-compare.txt`, `out/old_new-output.txt`): the old and new outputs are identical on each compiler, and GCC's equal Clang's. The same program built with Clang `-fsanitize=address,undefined,integer,implicit-conversion` on the new headers gives the same output, with no report located in a Galoot header (its three reports are the harness's own hash and xorshift, which wrap on purpose). I checked the printed compile-time products by hand: e1 e32 = 1 + e1∧e32, e32 (e1∧e32) = e32, (e1∧e32) e32 = −e32, (e1∧e32)² = 1, and (e31∧e32) e32 = e32 with g(e31, e32) = −1.

Second reference (`out/oracle-compare.txt`): `math-tools/trial-wu6a/dump_tables.cpp`, copied unchanged and built against the new headers, reproduces `out/galoot_{builtin,extra,big}_{g++,clang++}.jsonl` byte for byte on both compilers, and the same eight forms abort on coefficient overflow with the same message. The trial found those stored tables equal to `route_w.py` on all 117,184 pairs of its 100 forms (`compare_exact.json`), so the new headers agree with that independent route.

### The set-bit loops at their edges (priority 2)

- Read against the old loops. `reorder_sign` now adds, for each set bit i of `a` (lowest first), `popcount(b & ((1 << i) − 1))`: the same count of pairs i > j as the old right shifts. `a != 0` holds inside the loop, so `countr_zero(a)` is 0 to 31 and the shift `mask_t{1} << i` is defined at i = 31; `(1 << 31) − 1U` is unsigned. The contraction and correction loops visit the set bits in increasing order, as the old index loops did, and flip `sign` once per visited bit, as the old loops did (the `continue` skipped the flip for absent bits), so the alternating sign is unchanged.
- The lint fix at `c97fac5`: `static_cast<unsigned>(std::countr_zero(a))`. `countr_zero` returns `int`; the value is 0 to 31 there, so the cast is exact. Correct and minimal. The index in `blade_product.hpp` was already `std::size_t`.
- Conversions: both header sets compile with `-Wconversion -Wsign-conversion` under `-Werror` on both compilers (the harness), and a unit that instantiates `blade_product`, `reorder_sign` and `left_complement_sign` gives no warning from `bits.hpp` or `blade_product.hpp` under Clang `-Weverything` (less the C++98 and padding groups) or GCC `-Wconversion -Wsign-conversion -Warith-conversion -Wuseless-cast -Wold-style-cast`; GCC's only warnings are six `-Wuseless-cast` in `assert.hpp:69`, unchanged.
- Constant evaluation at bit 31 and at N = `mask_bits` is covered by the compile-time section above and by the new test case.
- Outside the contract (F3): with `GALOOT_CHECKING` 0 at run time, an out-of-range `b` now makes the contraction read `gram[i][j]` with j ≥ N.

### Mutants (priority 3)

- The author's 16 (`records/mutations/0012-product-cost-growth/mutate.py`, unchanged, at `58f4aa1` in my trees): killed on gcc-debug and clang-debug in normal and run-time mode (`out/impl-*.tsv`). With `--without-top-pairing`, `cr_top_bit_ignored` survives on both compilers and `rs_` and `ct_top_bit_ignored` are still killed, as recorded. The record is accurate.
- My 15 (`mutate_review.py`, `out/mine-*.tsv`), written from the loops: `reorder_sign` counting whether any generator of B is below e_i rather than how many, counting only the one just below, counting A's own bits, the index taken modulo 16 and modulo 8; in the contraction, the sign flipped only on a nonzero entry, e_N skipped, the loop over `blade` without e_i, over `blade | generator`, the index modulo 16; in the correction, the sign flipped only on a nonzero entry, e_N skipped, the loop over `a` in place of `rest`, the index modulo 16, the sign starting at +1. All 15 are killed on both compilers in both modes. In run-time mode the index-modulo-16 mutants are killed only by tests at n > 16 (the widest-algebra cases and `plan_test.cpp:812`), and the two sign-on-nonzero mutants can differ only in a form with a zero entry before a nonzero one in a row of the Gram matrix (inferred from the loop; the run-time kills are in `core.blade_product` and `core.plan`, whose golden tables include random forms of that kind). No survivor.
- The bits snippet, rerun: 13 of 13 caught on gcc-debug in both modes and on clang-debug in normal mode; its `--runtime-only` mode refuses on clang-debug before any mutation, as the author reported (`out/snip-*.txt`).

### Cost (priority 4)

`growth.py`, pinned to CPU 12 (sibling 13), three repeats interleaved, Galoot's test flags at -O0, `-c`, 12:41:06Z to 12:43:28Z, headers of `860f9c9` ("old"), `58f4aa1` ("new"), and a probe copy of the new headers whose `metric_like` keeps only its type test. The units are my own text of the same use (the four typed rigid(3) pairs in R(n−1,0,1), e4 read as e_n, signs from the parity of the written indices), in five variants: with `static_assert(core::metric_like<alg>)` and 0, 1, or 4 plans, and without it and 0 or 4 plans. Load: no other work of mine ran; the sibling thread was busy at most 0.01 s and other processes on CPU 12 at most 0.02 s per compile; the rest of the machine used 0.45 CPU-seconds per wall second on average (editors, other agent sessions, a browser). The `sollya` build of another session started after the measurement ended.

Plans minus baseline, millions of instructions (`out/growth-summary.txt`; spreads at most 0.8 M on GCC and 3.8 M on Clang):

| Metric | Tree | GCC n = 4 → 16 | Clang n = 4 → 16 |
| --- | --- | --- | --- |
| 4 plans − baseline, both with the static_assert (the author's metric) | old | 184.5 → 213.6 (+15.8%) | 345.6 → 437.1 (+26.5%) |
| | new | 178.3 → 178.5 (+0.1%) | 342.5 → 364.5 (+6.4%) |
| | probe | 179.9 → 179.5 (−0.2%) | 341.2 → 340.9 (−0.1%) |
| 1 plan − baseline, with the static_assert | new | 44.4 → 44.1 (−0.7%) | 125.4 → 148.1 (+18.1%) |
| 4 plans − baseline, neither with the static_assert | old | 187.8 → 234.9 (+25.0%) | 347.1 → 439.2 (+26.5%) |
| | new | 182.0 → 199.9 (**+9.9%**) | 342.2 → 366.1 (+7.0%) |
| | probe | 179.6 → 179.7 (+0.0%) | 340.8 → 341.7 (+0.2%) |

The author's figures reproduce (GCC +15.7% → +0.2%, Clang +26.5% → +6.5%). The baselines: with the static_assert, 806.6 → 825.3 M (GCC) and 746.3 → 769.1 M (Clang) as committed, flat in the probe; without it, flat in every tree.

The Clang remainder, measured: (a) the probe removes it on both compilers in every variant; (b) `-ftime-trace` (`out/trace-counts.txt`) shows the three value conditions of `metric_like` (`metric.hpp:276` to `:278`) evaluated once in the baseline with the static_assert, twice in a unit with one or four plans and the static_assert, once in a unit with four plans and no static_assert, and never in a unit with neither; at n = 16 one evaluation takes about 1.2 ms; (c) one plan and four plans add the same growth (22.7 M and 22.0 M with the static_assert). So the author's account is confirmed: the remainder is `metric_like` reading the n² Gram entries, once per algebra and unit for the constraint, and Clang does not reuse the static_assert's evaluation for the constraint while GCC does. Inferred, not read in Clang's source: why the two evaluations are not shared. The author's inference that a program that evaluates the concept only through constraints pays it once is now measured (the no-static_assert row on Clang). But see F1: GCC pays that one evaluation too.

Header and test units (old → new, change; spread in parentheses): `bits.hpp` alone GCC +0.7 M (+0.31%, 0.0 M), Clang +1.0 M (+0.53%, 0.6 M); `blade_product.hpp` +0.4 M and −0.4 M; `core.hpp` GCC +0.6 M (+0.08%, 0.2 M), Clang −0.2 M; `bits_test.cpp` GCC +10.3 M (+0.33%, 1.7 M), Clang +19.0 M (+0.67%, 5.6 M); `blade_product_test.cpp` −23.5 M (−0.31%) and +2.0 M (+0.04%); `blade_set_test.cpp` GCC +5.9 M (+0.06%), Clang +348.9 M (+1.54%, 23.3 M); `plan_test.cpp` −92.9 M (−1.05%) and −16.5 M (−0.15%). The claimed rises of `bits.hpp`, `bits_test.cpp` on Clang (+0.58% claimed) and `blade_set_test.cpp` on Clang (+1.5%, claimed from shared-mode rows) are confirmed pinned. The test units were compiled with `-isystem include/ext -DDOCTEST_CONFIG_SUPER_FAST_ASSERTS`, as the build compiles them.

### Limits (priority 5)

`limits.py`, `-fsyntax-only`, headers of `58f4aa1`, four configurations (Galoot's flags at -O0 with checking and -O2 without, bare `-std=c++23 -pedantic-errors`, and bare with -O2), at the recorded boundary points (`out/limits.tsv`): `full<13>` builds and `full<14>` stops on Clang, except bare -O2, where `full<13>` stops; `full<15>` builds and `full<16>` stops on GCC in all four. Full × full builds at n = 5 and stops at 6 on Clang, builds at 7 and stops at 8 on GCC, in R(n,0,0), R(n−1,0,1), and conformal(n−2), in all four; in the dense form, Clang 4 and 5, GCC 5 and 6. Every boundary is where the `gp_plan` and `full` comments and decision 0106 put it. Clang's bare -O2 `full<13>` also stops with the old headers (checked), so that defect predates the change, as the author reported. Limits unchanged.

### The public comment (priority 6)

"The cost is one step per generator of A, whatever the width of the masks." The loop runs `popcount(a)` times, so the count is accurate. It is in user terms and cites no governance or agent work (S11-clean). The internal comments in `blade_product.hpp` are accurate and state why. One wording point: F5.

### Commits, W2 and W9 (priority 7)

Fresh trees per code state (`out/per-commit.txt`): `971bc9e`, `4e69ce8`, and `77ed567` build all and the header sets and pass 124 of 124 on gcc-debug and clang-debug; lint builds and passes at `971bc9e` and `4e69ce8`, and fails at `77ed567` at `bits.hpp:107:47`, `bugprone-signed-bitwise`, as the author says. `20dc2ea` to `115b53e` have `77ed567`'s build inputs (empty diff over `include`, `test`, the CMake files, presets, `.clang-tidy`, `.clang-format`, `tools`), so six commits do not build under lint (F4). `f812191` and `58f4aa1` have `c97fac5`'s. The test (`971bc9e`) precedes the code it judges, and passes on the old code. The snippet's two mutation anchors changed in `77ed567` with the code they name, which W9 allows when the request lists them; the plan's departure 7 does. Messages say what and why and identify agent work.

### Gates at the head (priority 8)

All seven presets from fresh trees at `58f4aa1` (code of `c97fac5`), 12:47:24Z to 12:48:33Z: each builds all and passes 124 of 124, including `tests.reference_markers`, `tests.citations`, `tests.golden_facts`, and `core.include_scan`; gcc-debug and clang-debug also build `all_verify_interface_header_sets` and `format-check`; lint builds all, the header sets, and `format-check` with clang-tidy clean. The trace gate exits 0. W5's mechanical checks, full suite, and trace gate pass on this revision.

## Findings

F1. Medium; should be corrected before the merge (records). Decision 0105 (item 1, "the growth of a fixed use is gone on GCC", and "On GCC the plans do not grow in either copy") and the outcome's criterion 3 present the remaining growth as Clang's alone. Measured above: in units without `static_assert(metric_like<alg>)`, which is how a user writes a program, four plans grow from n = 4 to 16 by +9.9% on GCC (182.0 → 199.9 M) and +7.0% on Clang, and the probe removes both. GCC's +0.1% holds only against a baseline that has already evaluated the concept, which GCC then reuses for the constraint. So every unit that uses an algebra pays one `metric_like` evaluation on both compilers, about 18 M (GCC) and 23 M (Clang) instructions more at n = 16 than at n = 4, and Clang pays it a second time when the program also evaluates the concept itself. Fix: a later decision that supersedes 0105's statements (W6), and a corrected line in the outcome, so that the owner's question in item 3 is posed for both compilers.

F2. Low (records). Criterion 6 is met in substance but not shown on GCC for `core.hpp`: my pinned measurement gives +0.6 M (+0.08%) against a spread of 0.2 M (the author's −0.04%); the rise is `bits.hpp`'s (+0.7 M alone). Differences this small are near what include-path length alone can move (the author's two trees had paths of different length, mine nearly equal); plausible, not tested. The outcome's departure 9 should say the rise appears on GCC as well.

F3. Low; confirmed, unreachable through the public interface. `blade_product.hpp:219` and `:254`: with `GALOOT_CHECKING` 0 at run time, a `b` naming a generator beyond e_N made the old contraction skip it (the loop stopped at N); the new one reads `g.gram[i][j]` with j ≥ N. `oob_unchecked.cpp`, R(4,0,0), e1 times e1∧e5: old prints `+1 e10`, new trips libstdc++'s subscript assertion, and without `_GLIBCXX_ASSERTIONS` reads past the row. This violates a stated precondition, the old code already read out of bounds for an out-of-range `a`, and `gp_plan` always evaluates at compile time, where the check runs. Decision 0104's "which blade_product checks ... so every index stays below N" holds in constant evaluation and checking builds only; a later record should say so. No code change needed.

F4. Low (process, W2). `77ed567`, `20dc2ea`, `52ed97f`, `9ff402c`, `a023495`, and `115b53e` do not build under the lint preset, and their messages do not say so (`77ed567`'s lists what it checked, without lint). The plan's departure 1 and `c97fac5`'s message disclose it, and nothing has been pushed. I would not rewrite history to fix it (the records cite these revisions); the merge message should name it.

F5. Low (wording, public header). `bits.hpp:99`-`100`: "whatever the width of the masks". Masks have one width (32 bits); what the sentence means is that the cost does not depend on which generators A holds or on the size of the algebra, for example "whatever the indices of its generators". Optional; a change to a `///` comment needs the delta review.

F6. Low (records). Decision 0105 is inconsistent with itself: its table gives +15.7% → +0.2% (GCC) and +26.5% → +6.5% (Clang) at `c97fac5`, while its Decision item 1 quotes +15.4% → +0.1% and +27.1% → +6.2%, the `77ed567` run.

F7. Low (records). Decision 0106 says GCC's conformal n = 8 failure "is now reported at libstdc++'s subscript check (`array:219`, `__builtin_expect(...)` is not a constant expression)". That is the `77ed567` run (`out/limits-77ed567/limits.tsv`). At `c97fac5` the author's own `limits-c97fac5/limits.tsv`, and my run, report the plain operation-count limit at `blade_product.hpp:219:44` (`array:221` at bare -O2). The boundary is the same; the description is stale.

## Decision 0105 item 3, for the owner

My view, not a decision: the remainder is the validation of the signature's own data, n² entries read once per algebra per unit (twice on Clang when the program also asserts the concept), and it does not grow with the number of plans. That fits the letter of P2's exception, "the sparsity and symmetry pattern of its signature data", and it is per algebra, not per use. Two cautions before confirming it. First, as F1 shows, it applies to both compilers, so the reading should be confirmed for both. Second, most of its size is avoidable: the concept makes three passes over the entries (`entries_in_range()` twice and `is_symmetric()` once), and Clang adds a second evaluation, so up to six passes where one would do. A single-pass check, or one cached in a variable template per tag (the option decision 0105 lists), would shrink it without changing what is checked. I would accept the exception for the n² check itself and record the redundant passes as a cost to remove in a later unit, not as structure.

## Resolution

Open: F1 to F7, for the author or the merging agent. F1 should be resolved before the merge; the rest do not block it.

## Files

This record's directory holds `old_new.cpp` (the comparison), `growth.py` (pinned growth, header units, probe), `limits.py`, `mutate_review.py`, `oob_unchecked.cpp` (F3), and `out/` with the outputs cited above. Reproduction: build `old_new.cpp` with each header set using the flags above and `cmp` the outputs; `growth.py <checkout> <old-include> <new-include> <out> <cpu> 3`; `limits.py <checkout> <include> <out.tsv>`; `mutate_review.py <checkout> <preset> [--runtime]`, each from a scratch checkout with the pool exported.
