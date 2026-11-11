# Independent check: research note 0027, Spike E's collected plans and D8 (plan 0038, section 4, item 12)

Checker: Claude (agent, Opus 5.5), in the numerical-expert and performance-engineer roles. I did not write the note or any of its evidence. Inputs: `research/spike-e--note` at `88585ab` (the note `docs/agent/research-notes/0027-collected-plans-spike-e.md` and everything under `docs/agent/records/spikes/spike-e/`), the core headers at that revision (`include/galoot/core/{transform,mv,linear}.hpp`), plan 0001 (D8, DoD-4), decision 0008, and the E3 record on `records/cost-evidence-measurements` (`docs/agent/records/performance/wu17-growth/README.md`, its transform and antiproduct rows only). Branch `research/spike-e--check` from `88585ab`, worktree `.claude/worktrees/spike-e--check`. The files this record cites are in [`2026-11-10-spike-e-note-check/`](2026-11-10-spike-e-note-check/).

## Departures, skipped checks, ambiguities (C20)

- **Departure: a directory beside the record.** The brief names one file. The benchmark rerun's per-process log and rows (P13: a campaign's runs go to its own log, committed with its records), its summaries, the build log, and two short scripts are in the directory named above, as the part B check did.
- **Reduced benchmark protocol, chosen before the run.** The batch loop at 1,024 objects per motor, for transform(point) and transform(line), forms `galoot`, `a`, `b`, `c`, both compilers, 11 interleaved rounds, environment size 0 only, CPU 4 with its sibling idle, by `drive.py`'s own `pair` (same passes, short/long pairs, and busy rule) through `recheck.py`. The plane, the per-call loops, `bp`, the sweep, and environment sizes 1,352 and 2,768 did not run, so the criterion's "at every environment size" is applied at size 0 only.
- **Not rerun:** `drift.py --small-steps` (`drift-small.*`); the prototype's compile-cost thresholds and `tools/compiles` runs (I checked the note's figures against `proto/README.md` and `proto/cost/thresholds.tsv` only); `bench/build.sh` as written, because it writes into the spike's own `runs.tsv`, `out/check.*.txt`, and `probe.*.s`. Instead I built `transform_point`, `transform_line`, `check`, and the probe's assembly with build.sh's flags, logging to my own `build-runs.tsv`.
- **One failed row kept.** The first probe build in `build-runs.tsv` exits 1 because its output path was an existing directory in my scratch space. It was rebuilt to another path, and both rows stay.
- **The pool stopped itself** during the rerun. It read "23 of 23 free" before and at the lock, and "not running" after it (it stops after 30 idle minutes). I did not start or stop it.
- **Load.** The one-minute load average was 1.63 at the start and 2.02 at the end, from other users. No energy figure is used.
- **Read in part:** VVUQ, STYLE, and CONVENTIONS (by heading only). I read CONSTITUTION, WORKFLOW, and PERFORMANCE in full. No web request, no Terathon source, no book, no third-party contact.

## Check 1: operation counts and bounds, rerun. Verdict: reproduced

All runs used the math-tools environment (`<math-tools>/toolset/env/py/.venv/bin/python -B`) from the worktree's root, and `git status` showed no change to any spike file afterward.

| Script | Result |
| --- | --- |
| `derive.py` | exit 0, 65 PASS, 0 FAIL; `derivations.txt`, `counts.json`, `counts.md` byte for byte the same |
| `bounds/derive_bounds.py` | exit 0, "all checks passed"; `derive_bounds.txt` byte for byte the same |
| `bounds/probe.cpp` (g++ 16.2.1, the README's flags) then `bounds/replay.py` | exit 0, 11 PASS: dropped K (12, 5, 5, 5), (14, 7), (12, 12, 12, 7) from the library; 0 of 13,200 coefficients differ in double and float; `replay.txt` the same |
| `bounds/drift.py` (7 min 42 s) | exit 0; `drift.md`, `drift.txt`, `drift.json` byte for byte the same |

Every count, K, M ratio, drift figure, and share the note quotes matches these files. In particular: sections 3 and 6 and the summary table match `counts.md` and the K table; the A/B differing shares are 0.568 to 0.733 (main) and 0.567 to 0.767 (second sampler); B's median over A's is 0.769 (float, N = 1000, plane) to 1.388 (double, N = 1000, line); and the drift maxima are 14.1, 8.37, 7.08 (double) and 16.8, 38.2 (`drift-small.md`).

## Check 2: one benchmark cell, rerun. Verdict: reproduced

Toolchain: g++ 16.2.1 20260810, clang++ 23.1.4, the same as the record's. The rebuilt `probe.gcc.s` and `probe.clang.s` are byte for byte the committed ones, `check` passes on both compilers with the committed output, and each program's own `check` gives the committed differences. The lock was held from 01:47:39 to 01:51:56 UTC on 2026-11-11 (`protocol.log`). 176 pairs ran; 4 were repeated under the busy rule, none was busy again, and the checksums agree within each form.

Cycles per transform, median [range] over 11 rounds, batch loop, 1,024 objects, size 0 (`bench/results.tsv` here), against `bench/out/tables.md`:

| Compiler | Operation | galoot | a | b | c | b / a, rerun (recorded) |
| --- | --- | --- | --- | --- | --- | --- |
| GCC | point | 49.76 [0.40] (49.8) | 55.46 [0.03] (55.4) | 26.23 [0.07] (26.2) | 31.13 [0.15] (31.2) | 0.473 (0.473) |
| GCC | line | 56.74 [1.80] (56.8) | 68.43 [0.16] (68.4) | 52.08 [0.06] (52.1) | 55.42 [1.15] (55.4) | 0.761 (0.761) |
| Clang | point | 49.04 [0.10] (49.0) | 16.21 [0.01] (16.2) | 5.19 [0.01] (5.2) | 8.66 [0.01] (8.7) | 0.320 (0.320) |
| Clang | line | 62.95 [0.24] (63.0) | 25.44 [0.16] (25.5) | 10.17 [0.02] (10.2) | 14.44 [0.13] (14.4) | 0.400 (0.399) |

The other ratios agree to within 0.004 of the recorded ones:

- b / galoot: 0.527, 0.918, 0.106, 0.162.
- c / b: 1.187, 1.064, 1.670, 1.420.
- a / galoot on GCC: 1.115 (point) and 1.206 (line). This reproduces the note's unexplained reading in departure 3, where the library with its call is faster than inline A in GCC's batch loop.

Every verdict at size 0 has the same faster form as the record (`bench/verdict.tsv`). Two cells the record calls "cycles only" (GCC line, b / galoot and c / b) are beyond in nanoseconds too at size 0; with one size I cannot say whether they hold at all three. In nanoseconds the medians are about 4% below the record's size-0 figures (for example, GCC's point `a` is 9.34 ns here and 9.73 ns there) at the same cycles: the clock ran at about 5.94 GHz here, against the 5.70 GHz the record's departure 3 reports for its size-0 rounds. This supports the note's decision to quote cycles.

## Check 3: tables and claim markings. Verdict: tables correct, with the findings below

I checked every table and every number in the Answer, the Summary, and sections 1 to 12 against the record each one cites:

- section 4 and the summary's cycle ratios against `bench/out/tables.md` (all 24 pairs, by recomputing from the medians);
- section 5 against the sweep table, recomputing each break-even;
- section 6 against `bounds/README.md` section 1;
- section 7 against `drift.md`;
- section 8 against `proto/README.md` and `proto/cost/thresholds.tsv`, recomputing the instruction, wall-time, and memory ranges;
- section 9 against `bounds/README.md` section 8;
- departure 8 against the E3 README: +41.3% on GCC and +181.9% on Clang.

Apart from the findings below, every figure is traced and correctly rounded.

The five claims the author lists as not traced to records:

| Claim | Assessment |
| --- | --- |
| D8's criterion is not met (Lemma 1 and the counting rule) | **Holds for the criterion's words; the reasoning given is too broad.** See finding 1. What decides the criterion is that the restricted kernels write only x's blades, for every algebra and layout (`make_second_kernel<…, Kept>` writes only positions in X, and the checking mode verifies the rest), so no result keeps a cancelling component. That is a property of the generator, but the note gives Lemma 1 as the reason, and Lemma 1 is about terms inside kept outputs. It is proved for rigid(3) only, and it is false for a case in the spike's own data. |
| "15% in the middle case" | **Correct.** It is the median of the eight point and plane per-call cycle ratios (0.849 and 0.859, mean 0.854, a 14.6% gain), or 0.849 (15.1%) over the seven faster loops. It is arithmetic on `tables.md` and should say so. |
| A core pass needs commutative multiplication | **Correct, but understated.** See finding 2. |
| Form C is not generated by existing machinery | **Supported; mark it inferred.** `forms.py` writes C by hand from closed forms (`form_C_point`, `_line`, `_plane`, with cross products), and `derive.py` checks it by Gröbner reduction modulo (λ − 1, h). No core part reduces modulo an ideal, and the prototype pass only collects exact terms. "A generator would need the constraints as data" is an inference. |
| Collection would not remove E3's compile-time growth | **Follows for the prototype's design; mark it inferred.** The pass reads `detail::first_kernel` and `detail::second_kernel`, which are built from the two antiproduct plans whose generation grows (E3: the transform's growth is about twice compose's, the antiproduct exception of decision 0141). A collected form generated by another route is not excluded, and the pass's own growth with n at fixed use was not measured. |

## Check 4: the D8 recommendation. Verdict: (a) follows, and (c) as the first candidate after 1.0 follows, with two additions

(a), deferring past 1.0, follows. D8's recorded condition for 1.0 is "composed plans violate C6 (cancelling components kept)". No transform result keeps a component off x's blades, whatever the algebra. For motors, in rigid(3) (Lemma 1) and in conformal(3) (my finding 1), the kernels also compute no term that cancels. The P2 run's growth is a separate, recorded exception (decision 0141), and collection does not address it. Per call, the data do not support (b): B loses on the line on GCC and gains a median of about 15% on points and planes. It also changes bits and adds a compile phase.

(c) as the first candidate after 1.0 follows from the batch data, which I reproduced: B / A of 0.32 to 0.76 at 1,024 objects. The note's caveats on inlining and on C12's present user are right. Two points belong in the decision record:

1. A batch interface needs a scalar concept stronger than "v_p v_q = v_q v_p" (finding 2).
2. It cannot be built as an extension today. The prototype reads `detail::first_kernel` and `detail::second_kernel`, and C13 lets an extension reach the core only through its public interface. So a batch interface is either core work or needs the term lists made public. The note does not say this. It does not change the recommendation, but it does decide where the candidate unit lives.

I disagree with none of the note's verdicts. I disagree with how two of its statements are scoped (findings 1 and 2).

## Findings, ranked

1. **Lemma 1 is stated more broadly than it holds (moderate).** Sections 1, 6, 12, and the Answer say that the like terms collection combines "always have the same sign", and section 12 rests D8 on it without saying "in rigid(3)". `cancel.py` composes the two kernels of each case in `proto/cost/cases.json` as `collect.hpp`'s `summarize` does, and compares the number of composed paths with Σ|C| over kept outputs. Equal means that no monomial gathers paths of both signs, which is Lemma 1's test. Output in `cancel.txt`:
   - rigid(3) motor on point, line, plane: equal (64, 144, 64), with no mixed-sign monomial. This reproduces Lemma 1, at 20 paths and 12 monomials per point coordinate.
   - conformal(3) motor on grades 1 to 4: equal (100, 376, 376, 100), with no mixed-sign monomial. **Lemma 1 extends to conformal(3) motors**; this was not shown before.
   - conformal(3) 16-blade even versor: **not equal.** Grade 1 has 400 paths and Σ|C| = 384, with 8 monomials whose paths cancel to exactly 0 (all on outputs e4 and e5, 8 paths each). Grade 2 has 1,600 paths and Σ|C| = 1,456, with 72 cancelling monomials. So the restricted kernels compute 16 of 400 (4%) and 144 of 1,600 (9%) multiplications whose sum is identically zero.

   Three consequences:
   - The note's "the restricted kernels keep no cancelling component" holds for components, but "collection removes no cancellation" holds only for motors. For a general even versor, collection removes structurally cancelling work, which is the one C6-type gain of collection the spike's data contain.
   - For those outputs M_B < M_A, so B's bound is tighter there than "same M" says.
   - None of this changes D8 for 1.0: the case keeps no component, and a general even versor in conformal(3) has no 1.0 user (rga3 is required; cga3 is optional M6). The note should scope Lemma 1 and the D8 reading to motors, and record the even-versor result.

2. **The commutativity requirement is stronger than the note says (moderate).** The library computes each sandwich term in the order v_a · x_b · v_c: `transform_middle` runs `run_product(v, x)`, then `transform_outer` runs `run_product(middle, v)`, and `linear.hpp` keeps the order of s x_i for a scalar whose product does not commute. Form B computes entry(v_p v_q) · x_j (`collect.hpp`'s apply kernel). The matrix M(Q) x therefore needs x's scalars to commute with the motor's, not only "v_p v_q and v_q v_p as one product" (note section 1, and `collect.hpp`'s own comment). The fallback in section 1, "merge only products in the same factor order and give up part of the saving", does not exist for B. Over a noncommutative scalar no motor-only matrix can be built, so the batch interface's whole gain needs the requirement. The "What remains unverified" item on the size of that saving is moot for B. In practice the impact is small, since every DoD-7 scalar commutes, but a batch interface must be constrained by a commutative-scalar concept or say so in its interface (C8).

3. **"About 2/3 of A's on a point's coordinates" (minor).** That holds for x and y (K 8 against 12). For z, K is 7 against 12, a ratio of 0.58. Carried from `bounds/README.md` section 2.

4. **"The spread between arrangements of one form (6 to 9)" (minor).** From `counts.md`, the line's two arrangements differ by 6 (MA) or 8 (MA + scl), and the plane's by 3 or 2. "6 to 9" is not reproducible from the counts, and for the plane the spread is smaller than its distance from the published 35. The conclusion still stands, because 35 lies between the plane's 33 and 37 and 54 is one of the line's values. Carried from part B's README.

5. **Some "faster" statements are median ratios and not verdicts (minor).** Of the 11 per-call loops where C is "faster than B", four are within the criterion in nanoseconds and beyond only in cycles: GCC's point, line, and plane throughput, and Clang's plane latency. GCC's line latency C / A (0.93) is cycles only but is not marked as such in section 4. The batch "C slower than B everywhere" is cycles only for GCC's line in the record, though beyond in both units in my size-0 rerun. The note marks cycles-only verdicts elsewhere, so these should be marked the same way.

6. **Register (minor).** Section 13 should add: the 15% figure (computed from `tables.md`); form C not generated (inferred); collection not removing E3 growth (inferred); and Lemma 1's scope (rigid(3) proved, conformal(3) motors checked here, general even versors false). Its last row, "my reading of Lemma 1 and rule K", should rest on the restriction to x's blades (finding 1).

## Not verified

- The plane, the per-call and latency loops, `bp`, the sweep, and environment sizes 1,352 and 2,768 (not rerun).
- The compile-cost thresholds and `tools/compiles` figures (checked against their records only).
- `drift.py --small-steps`.
- The flector layouts and odd versors, for Lemma 1 or the cancellation count.
- The departure-3 readings: I reproduced GCC's library-faster-than-inline-A batch result, but did not trace it.

## Reproduction

From the worktree's root:

    <env>/bin/python -B docs/agent/records/spikes/spike-e/derive.py
    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/derive_bounds.py
    <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/drift.py
    python3 -I docs/agent/records/reviews/2026-11-10-spike-e-note-check/cancel.py docs/agent/records/spikes/spike-e/proto/cost/cases.json
    # programs built with build.sh's flags into build/spike-e-bench/<compiler>/, then under the lock:
    python3 -I docs/agent/records/reviews/2026-11-10-spike-e-note-check/recheck.py docs/agent/records/spikes/spike-e/bench/drive.py docs/agent/records/reviews/2026-11-10-spike-e-note-check/bench 11
    python3 -I docs/agent/records/spikes/spike-e/bench/drive.py summarize docs/agent/records/reviews/2026-11-10-spike-e-note-check/bench
