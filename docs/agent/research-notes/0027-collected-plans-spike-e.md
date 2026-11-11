# 0027 Collected plans for transform: what Spike E measured, and D8

- Date: 2026-11-10 (UTC; the benchmark runs it reports ran in the first hours of 2026-11-11 UTC).
- Author: Claude (agent, Opus 5.5), researcher and writer in the numerical and performance roles, for plan 0038 part F (section 4 items 8 and 12). This note writes up evidence that other agents produced; it adds no measurement of its own.
- Branch: `research/spike-e--note`, from `develop` at `9aa1d24`, with `research/spike-e--bench` (tip `ae09ba3`) and `research/spike-e--bounds` (tip `5897365`) merged into it (`befa222`, `83ee173`). Nothing under `include/` or `test/` changed.
- Evidence, all under `docs/agent/records/spikes/spike-e/`: part B's derivations and counts (`README.md`, `COUNTING-RULE.md`, `counts.md`; Claude, Sonnet 5.5, checked independently by an Opus agent in `records/reviews/2026-11-10-wu17-part-b-check.md`); the benchmarks (`bench/README.md`, `bench/out/tables.md`) and the collection-pass prototype (`proto/README.md`), both by Claude (Opus 5.5) as performance-engineer; the forward error bounds and drift runs (`bounds/README.md`), by Claude (Opus 5.5) as numerical-expert. Every number below comes from one of these files; section 12 marks which claims are measured, proved, or inferred.
- Question (D8, plan 0001 section 2, decision 0008): should collected plans enter the core in 1.0? Plan 0038 asks this note to state what a collected plan (form B) and a constraint form (form C) would gain and cost, and to separate what the core could do as an exact post-pass from what needs a named assumption.
- Status: given the independent check that plan 0038 item 12 asks for, `records/reviews/2026-11-10-spike-e-note-check.md` (Claude, Opus 5.5, numerical-expert and performance-engineer roles, on `research/spike-e--check` at `9544c4d`, merged here). It reran part B's counts, the bounds and drift scripts, and the batch cell for the point and the line on both compilers, and reproduced them. This revision of the note applies its six findings: Lemma 1 scoped to motors, with the even versor's cancellation recorded (sections 1, 6, 12); the commutativity requirement stated as the check found it (sections 1, 8, 10, 12); the z coordinate's bound ratio; the errata spread; marks for median-ratio and cycles-only statements; and the register (section 13). Not reviewed by the owner.

## Departures, ambiguities, and open points first

1. **File name.** Plan 0038 reserves `0027-spike-e-collected-plans.md`; the brief for this note names `0027-collected-plans-spike-e.md`. I followed the brief. No record links to either name.
2. **The inlining caveat governs every comparison with the library.** In the benchmark programs, which hold five forms each, both compilers kept `galoot::core::transform` out of line (one call per transform; two for the line on Clang). In E2's one-form programs GCC inlined it. So the `galoot` rows include a call, and every gain below is stated first against form A written inline (the same terms in the same order, bit for bit the same results), which is the call-free baseline, and only second against the library as it compiled. Decision 0238's core inlining unit has not run; that it would bring `galoot` to form A's times is inferred.
3. **Two unexplained readings in the benchmark records**, neither traced: one GCC line throughput run of the prototype's form at 74.9 cycles against 55.5 in its 32 other runs (it sets that row's range), and GCC's batch loop, where the library with its call is faster than inline form A (ratio of A to the library 1.11 for the point, 1.21 for the line).
4. **The criterion was applied in nanoseconds as fixed, and also in cycles, a reading chosen after the clock was seen to move** (about 5.70 GHz in some rounds, 5.9 to 6.0 GHz in others). The note quotes cycles and marks a verdict that holds in cycles only.
5. **The errata counts are not reproduced as a pair.** Form C of the line gives 54 multiply-adds in one of its two arrangements; form C of the plane gives 30 or 33, never 35. The book was not read, so this is recorded as a difference of counting rule, not an erratum (section 3).
6. **Naming disagreement, carried from decision 0234 item 7.** The errata page says Q_vw is the scalar's coefficient and Q_mw the antiscalar's; CONVENTIONS 5.8 and the preset put Q_vw on 𝟙 (the antiscalar) and Q_mw on the scalar, as the wiki's Motor page does. No count or polynomial here depends on it (the constraint h is symmetric in the two). No change to CONVENTIONS is proposed; it goes to the owner at the M7 gate.
7. **Conformal(3) consequences for the rounding check are inferred**, from research note 0022's identities and the shape of a collected program, not computed.
8. **The P2 run** (E3, `records/performance/wu17-growth/` on the unmerged `records/cost-evidence-measurements`) is the other half of D8's recorded evidence. I read only its transform and antiproduct rows. It measures compile-time growth with n (transform +41% on GCC and +182% on Clang from n = 4 to 16, attributed to the antiproduct's exception of decision 0141), not which components a result keeps, so it bears on C6's "cost follows use" and not on cancelling components. Inferred, not measured: a collection pass of the prototype's design would not remove that growth, since it reads `detail::first_kernel` and `detail::second_kernel`, which are built from the antiproduct plans whose generation grows. A collected form generated by another route is not excluded, and the pass's own growth with n at fixed use was not measured.

## Answer

Collection is a batch feature, not a per-call one, and the constraint form is a different operation, not a faster `transform`.

1. **Per call, form B gains modestly for points and planes and loses for lines.** Against inline form A it is 5% to 39% faster in 7 of the 8 point and plane loops on the two compilers (the median of the eight cycle ratios is 0.85, a gain of about 15%, computed from `bench/out/tables.md`; GCC's point latency loop is 4% slower), and it loses on the line: 38% slower in GCC's throughput loop, 8% slower in GCC's latency and Clang's throughput loops (cycles only), 16% faster only in Clang's latency loop. For the line it trades 19 products for 19 more additions and doublings.
2. **Under one motor for a batch of objects, form B gains a lot.** With the motor's quadratic terms built once and 1,024 objects each costing one matrix product, B takes 0.47, 0.76, 0.58 of A's cycles on GCC (point, line, plane) and 0.32, 0.40, 0.33 on Clang, all beyond the criterion. It pays off from 1 to 3 objects per motor.
3. **Form C is the fastest per call and loses in batch.** Its median is below B's in 11 of 12 per-call loops (7 of them beyond the criterion in nanoseconds, 4 in cycles only) and below A's by 7% to 44% except GCC's line throughput (30% slower), but in a batch its object part is larger than B's matrix product, so B beats it from 1 to 16 objects per motor.
4. **Form B is exact as polynomials and as values over an exact scalar, but not bit for bit over double.** Its output polynomials equal the sandwich; for motors its magnitudes equal form A's (for a general even versor they are smaller); its rounding count K is at most A's. It changes 57% to 77% of the result coefficients' bits over a rounding scalar, so every test that pins a transform's bits over double changes. It also assumes that the object's scalars commute with the motor's: the library computes each term in the order v · x · v, and B computes (v v) · x.
5. **Form C needs named assumptions (C8): λ = 1, and for a line's moment also h = 0.** Off them it is not the sandwich. On composed chains of 1,000 motors its error against the sandwich reached 14 times form A's whole rounding bound in double (38 times in float with biased steps).
6. **A collection pass at compile time is feasible but not free.** Its largest constant evaluation is 1.8 to 4.2 times the plan's in rigid(3), and Clang's default step limit refuses it for one n = 5 stress case (the 16-blade even versor on grade 2).
7. **Recommendation for D8: no collected plans in core 1.0.** D8's recorded criterion for 1.0 (composed plans keep cancelling components, contrary to C6) is not met: the restricted kernels write only the object's blades, in every algebra and layout, so no result keeps a component off them. The evidence supports a named batch-under-one-motor interface, built on an exact collection, as the right shape if one is added; I recommend recording it as the first candidate after 1.0, to be built when a user batches transforms (section 11).

## Summary

Per transform, rigid(3) over double, cycles at environment size 0, from `bench/out/tables.md`; operation counts by rule S of `COUNTING-RULE.md`. "Per call" ratios are the range over the throughput and latency loops of both compilers.

| | Form A (today's kernels) | Form B per call (collected) | Form B per batch (one motor, many objects) | Form C (unit and geometric constraints) |
| --- | --- | --- | --- | --- |
| Equals the sandwich | yes, as polynomials and bit for bit with the library | yes, as polynomials; other bits over double | yes, as polynomials; other bits over double | only when λ = 1 (and h = 0 for a line's moment) |
| Assumption to name (C8) | none | the object's scalars commute with the motor's | the object's scalars commute with the motor's | λ = 1; h = 0 for lines |
| mul, add, doublings per call: point, line, plane | 48, 36, 0; 72, 58, 0; 48, 36, 0 | 35, 35, 9; 53, 62, 15; 35, 35, 9 | motor once: 22, 26, 9 (point, plane), 26, 41, 15 (line); per object 13, 9 or 27, 21 | 22, 18, 4; 48, 39, 7; 30, 24, 7 |
| Cycles against A, per call | 1 | point, plane 0.61 to 1.04; line 0.84 to 1.38 | (not a per-call form) | 0.56 to 0.93, but 1.30 for GCC line throughput |
| Cycles against A, batch of 1,024 | 1 | (as A plus motor work per call) | GCC 0.47, 0.76, 0.58; Clang 0.32, 0.40, 0.33 | GCC 0.56, 0.81, 0.94 (the plane's in cycles only); Clang 0.53, 0.57, 0.48 |
| Break-even objects per motor | | | against A: 1 to 3; against C: 1 to 16 | |
| Rounding count K (largest output) | point 12, line 14, plane 12 | 8, 11 (10), 9 (8) | same as per call | 9 (8), 14 (12), 15 (13) |
| Magnitude M | reference | equal to A's for motors (proved in rigid(3), checked for conformal(3) motors); smaller for the conformal(3) 16-blade even versor | as per call | up to 3.96 times A's (sampled) |
| Error against the sandwich, chains of 1,000 (max, in units of A's bound) | 0.40 point, 0.22 line, 0.25 plane | 0.33, 0.24, 0.25 | as per call | 14.1, 8.4, 7.1 (double) |
| Checking-mode extra work | 20, 12, 28 more mul and 4, 2, 4 compares | none in rigid(3) (nothing to drop; inferred for a pass) | none in rigid(3) | not applicable (no drop) |
| Compile cost | the plans | a new constant evaluation 1.8 to 4.2 times the plan's (rigid(3)) | the same | written by hand per object (no generator) |
| Tests with changed expected bits or counts | | reference kernels, op counts, kernel sizes, rounding tests | the same if it replaces `transform`; none if separately named | every exact test with a non-unit motor if it replaced `transform` |

In parentheses, K with a doubling counted as exact (0 roundings) rather than as the core's rule counts it (1).

## Terms

- **Motor, sandwich.** A rigid(3) motor Q has weight r = (v, s) = (Q_vx, Q_vy, Q_vz, Q_vw) and bulk b = (m, t) = (Q_mx, Q_my, Q_mz, Q_mw) (CONVENTIONS 5.8). `transform(x, Q)` computes the sandwich S = Q ⟇ x ⟇ Q̰, which is linear in the object x and quadratic in Q.
- **λ and h.** λ = |v|² + s² (the unit constraint is λ = 1) and h = v · m + s t (the geometric constraint is h = 0). A correctly rounded unit motor has |λ − 1| and |h| of a few roundings; a motor composed from many rounded ones drifts off both.
- **Form A** is what the library's kernels compute: the restricted composition, V ⟇ x kept where the second stage reads it, then the middle times V̰ kept on x's blades (plan 0021 R5).
- **Form B, a collected plan.** The sandwich written as a matrix M(Q) times x: like terms combined (Q_a Q_b + Q_b Q_a = 2 Q_a Q_b), each distinct product of two motor coefficients computed once and shared by every entry that uses it. `proto/collect.hpp` derives it at compile time from the library's two kernels; the benchmarks call that version `bp` and the hand-generated one `b`.
- **Form C, a constraint form.** A form that uses λ = 1 and h = 0 to drop terms, the kind of form the errata page counts. Two arrangements: the motor's coefficients doubled first, or the factor 2 applied to results (same bits; different counts).
- **Batch under one motor.** Many objects transformed by the same motor, with the work that reads only the motor (B's matrix, C's translation T) done once.
- **Rule S, rule K.** The counting rules of `COUNTING-RULE.md`: rule K counts what the kernels do, rule S counts a straight-line program in which a sign is free while it can be carried. A doubling (`scl`) is counted apart from additions. A multiply-add (MA) counts a product and the sum that takes it as one unit, for comparison with published counts only.
- **K, M, γ_K.** For one output, K is the largest number of roundings on a path from an input to it, and M is the same program run on absolute values. The forward bound is |ŷ − y| ≤ γ_K M with γ_K = Ku/(1 − Ku), u = 2⁻⁵³ in double (decision 0180's Lemma A).
- **Loops.** Throughput: independent calls. Latency: each result is the next call's input. Batch: setup once per motor, then 1,024 objects.
- **The criterion.** A difference is called when the medians differ by more than twice the larger of the two ranges, with the same form faster at all three environment sizes (0, 1,352, 2,768 bytes of padding). "Beyond" means the criterion holds.
- **Break-even.** The smallest number of objects per motor from which the first form takes fewer cycles per transform than the second at that size and every larger one measured.

## 1. What "exact" means for form B

Decision 0008 admits collected plans "under C4 as composition plus exact collection", and plan 0001 calls them an exact post-pass. The records let me say precisely what is exact and what is not.

Exact:

- **The structure.** The pass composes the two kernels into a table of exact integer coefficients, divides each entry by the gcd of its coefficients, and shares equal entries (`proto/README.md`). Every coefficient is exact (C7); no floating-point step enters generation.
- **The polynomial.** Form B's outputs equal the sandwich as polynomials in Q and x (sympy over the rationals, `derivations.txt`), and the prototype's outputs equal the sandwich and the library's `transform` over the test rational for 200 random and 200 unit motors per object, and for 50 conformal(3) motors on grades 1 to 4 (`bench/check.cpp`, `bench/out/check.*.txt`).
- **The value over an exact scalar.** It follows from the polynomial identity that over an exact scalar B and A return equal results for every input.
- **The magnitude, for motors.** In rigid(3), for every kept output of a motor's transform of a point, line, or plane, M_B = M_A as polynomials: the like terms collection combines always have the same sign, so collection removes no cancellation and adds none (`bounds/README.md` Lemma 1, proved and checked for all 14 outputs). The independent check extended this to the conformal(3) motor on grades 1 to 4 by counting: the composed paths equal the sum of the collected coefficients' absolute values (100, 376, 376, 100), with no monomial gathering paths of both signs (`records/reviews/2026-11-10-spike-e-note-check/cancel.txt`).
- **Not for a general even versor.** For the conformal(3) 16-blade even versor the same count shows cancellation inside kept outputs: on grade 1, 400 paths against a sum of 384, with 8 monomials whose paths cancel to exactly 0 (all on the outputs e4 and e5); on grade 2, 1,600 against 1,456, with 72. So the restricted kernels compute 16 of 400 (4%) and 144 of 1,600 (9%) multiplications whose sums are identically zero, and collection would remove that work. For those outputs M_B < M_A, so B's bound there is tighter than A's by more than the change of K. This is the one structurally cancelling saving of collection in the spike's data. It keeps no component off x's blades, so it does not bear on D8's criterion (section 12), and no 1.0 user transforms by a general even versor (rga3 is required; cga3 is the optional M6).
- **The dropped part.** In rigid(3) every coefficient `transform` drops is the zero polynomial, so an exact collection across both stages leaves nothing on the dropped blades (verified for the polynomials; that a pass emits no term there depends on its design, inferred).

Not exact:

- **The bits over a rounding scalar.** B adds in a different grouping and applies doublings, so it rounds differently. On the drift runs A and B gave different values in 57% to 73% of output coefficients (57% to 77% with the second sampler). A collected `transform` would not reproduce today's bits.
- **Commutativity.** The library computes each sandwich term in the order v_a · x_b · v_c: `transform_middle` runs the product of v and x, then `transform_outer` the product of that middle and v, and `linear.hpp` keeps the order of s x_i for "a scalar whose product does not commute, such as a matrix". Form B computes entry(v_p v_q) · x_j (the apply kernel of `collect.hpp`). So M(Q) x needs the object's scalars to commute with the motor's, not only v_p v_q = v_q v_p, which is the assumption `proto/README.md` names. The core's `ring_scalar` concept does not require either. There is no partial form of B without the requirement: over a scalar whose product does not commute, no motor-only matrix can be built, so the whole of the batch gain depends on it. Every scalar the core's tests use commutes, so the practical impact is small, but a core collected plan or batch interface must be constrained by a commutative-scalar concept, or state the requirement in its interface (C8).

So "exact post-pass" holds in the sense of C7: exact structure, exact value over exact scalars, with rounding entering only through the scalar. It does not hold in the sense of "the same floating-point results as today".

## 2. What form C assumes

Form C equals the sandwich modulo the ideal (λ − 1, h), checked by Gröbner reduction (`README.md`, "Form C"). Dividing by one generator at a time shows which constraint each output needs:

| Object | needs λ = 1 | needs h = 0 |
| --- | --- | --- |
| point | yes, and enough alone | no |
| plane | yes, and enough alone | no |
| line, direction | yes, and enough alone | no |
| line, moment | yes | yes |

Off the constraints, as polynomial identities (`bounds/README.md` section 5): S − C = (λ − 1) p for a point; (λ − 1) d for a line's direction; (λ − 1)(k + T × d − T × R̂ d) + 2h R̂ d for its moment; (λ − 1) n and (λ − 1)(g_w − n · T + R̂ n · T) for a plane. C misplaces a point by (λ − 1) times the distance the motion moves it. C cannot replace `transform`: every exact test with a non-unit motor would change, as would `transform`'s documented behavior that a versor with λ = 2 doubles a point's weight. It can only be a separately named operation whose interface states λ = 1 (and h = 0 for lines).

Forms A and B see h only in a line's moment (off by 2h R̂ d, a screw of pitch 2h/λ); for points and planes they give the exact image under the rigid motion the motor represents, scaled by λ (`bounds/README.md` D1 to D3).

The bounds record also notes, counted by hand and not built or tested, that C plus the correction (λ − 1) p is an exact form of a point's sandwich for any motor at about 30 multiplications against B's 35 and A's 48; the same holds for a line's direction and a plane's normal, not for the moment or the offset.

## 3. Operation counts, and the errata page

From `counts.md` (rule S, per call; rule K gives the same mul and add for form A):

| Transform | A | B | C, motor doubled first | C, factor 2 on results |
| --- | --- | --- | --- | --- |
| point | 48 mul, 36 add | 35 mul, 35 add, 9 scl | 22, 18, 4 scl | 22, 18, 3 scl |
| line | 72, 58 | 53, 62, 15 scl | 48, 39, 7 scl | 48, 39, 9 scl |
| plane | 48, 36 | 35, 35, 9 scl | 30, 24, 7 scl | 30, 24, 6 scl |

Checking mode on (exact scalar) costs 68, 84, 76 multiplications and 4, 2, 4 comparisons, against 48, 72, 48 off. The products and `compose` have no like terms (each output's monomial count equals form A's term count), so collection changes nothing there, and a constraint form is not defined for a bilinear product.

Per object in a batch under one motor: B 13 mul + 9 add (point, plane) or 27 + 21 (line), after a motor-only part of 22 mul + 26 add + 9 scl (point, plane) or 26 + 41 + 15 (line). C per object, motor doubled first: 22 + 18 (point), 36 + 30 (line), 18 + 15 (plane), with 12 mul + 9 add + 7 scl once per motor (line, plane).

**The errata counts.** The knowledge base's copy of the rigid wiki's Errata page states 54 multiply-adds for transforming a line and 35 for a plane (first printing of *Projective Geometric Algebra Illuminated*, page 156). In the multiply-add unit, form C of the line is 48 or 54 (motor doubled first, or factor 2 on results) and 55 or 63 with doublings counted; the plane's is 30 or 33, and 37 or 39 with doublings. So 54 appears for the line in one arrangement and 35 does not appear for the plane, though it lies between the plane's 33 (without doublings) and 37 (with them). The two arrangements of the line differ by 6 (MA) or 8 (MA with doublings), and those of the plane by 3 or 2. The published 54 is one of the line's values, and the published 35 is within the range the plane's counts span once doublings are counted or not, so the recomputation neither confirms nor refutes an erratum; the supported reading is a difference of counting rule (what a doubling is, how a sum is fused, whether the motor's own setup is in the total). These are forms derived here, not the book's formulas, which nobody on the project has read.

The counts also predicted break-evens that the measurements then moved (section 5): decision 0234 item 6 gives "from 2 points" for B against A (the multiply-add count with doublings) and "about 5" for B against C. Measured, B beats A from 1 to 3 objects, and beats C from 1 to 16.

## 4. Measured time

Machine and method (`bench/README.md`): Intel Core i9-14900K, CPU 4 pinned with its sibling idle, under the timing lock; g++ 16.2.1 and clang++ 23.1.4 at `-O2 -ffp-contract=off`, checking off, x86-64 baseline (SSE2); eleven interleaved rounds at each of three environment sizes; 128 unitized motors and 128 objects from seed 20261110. The protocol was committed (`00d26b9`) before the first timed process.

**Per call, B against A inline** (ratio of cycles; below 1 is faster):

| | point thr. | point lat. | line thr. | line lat. | plane thr. | plane lat. |
| --- | --- | --- | --- | --- | --- | --- |
| GCC | 0.86 beyond | 1.04 (A faster; cycles only) | 1.38 (A faster) beyond | 1.08 (A faster; cycles only) | 0.85 beyond | 0.72 beyond |
| Clang | 0.95 (cycles only) | 0.61 beyond | 1.08 (A faster; cycles only) | 0.84 beyond | 0.88 beyond | 0.75 beyond |

Per call the retired double-precision arithmetic (perf counters, `bench/out/stalls.tsv`) equals the counts where a loop is not vectorized: GCC's per-call line retires 130 scalar lanes in both A (72 + 58) and B (53 + 62 + 15). Collection removes no arithmetic per call for the line; it moves it from products to sums, on a longer dependence chain. Against the library as it compiled here, with its call, B is faster in every per-call loop but GCC's line throughput (1.17, slower).

**Per batch of 1,024 under one motor:**

| | point | line | plane |
| --- | --- | --- | --- |
| GCC, B / A | 0.47 | 0.76 | 0.58 |
| Clang, B / A | 0.32 | 0.40 | 0.33 |
| GCC, B / library | 0.53 | 0.92 (cycles only) | 0.61 |
| Clang, B / library | 0.11 | 0.16 | 0.07 |

All beyond the criterion except the one marked. Clang vectorizes every inline form across objects, two per instruction, and neither compiler vectorizes the library's call; that is much of Clang's larger ratio against the library, and it is an inlining effect (inline A alone is 0.33, 0.40, 0.21 of the library on Clang's batch loop), not a collection effect. Against A, which is inline too, the gain of collection itself is the B / A row. GCC does not vectorize the batch.

**Form C:**

- Per call against A: 0.79 and 0.58 (point), 1.30 and 0.93 (line, GCC; the 1.30 is A faster, beyond; the 0.93 is beyond in cycles only), 0.75 and 0.84 (plane) on GCC throughput and latency; 0.60, 0.56, 0.75, 0.56, 0.74, 0.69 on Clang, all beyond. So 7% to 44% faster in cycles, except GCC's line throughput.
- Per call against B: the median is lower in 11 of 12 loops (ratios 0.56 to 0.94); beyond the criterion in nanoseconds in 7 of them, and in cycles only in 4 (GCC's point, line, and plane throughput, and Clang's plane latency). GCC's plane latency is 1.17, slower, beyond.
- Per batch against B: slower everywhere, 1.19, 1.06, 1.61 (GCC) and 1.67, 1.42, 1.46 (Clang), all beyond except GCC's line, beyond in cycles only in the record (the independent check's size-0 rerun found it beyond in nanoseconds too), because C's object part (22 mul + 18 add for a point) is larger than B's matrix product (13 + 9).

**The prototype's form (`bp`) against the hand-generated B (`b`):** within the criterion in 14 of 18 cells in nanoseconds; beyond it `bp` is slower in GCC's point throughput (1.13), GCC's plane batch (1.48, with two store-forward blocks per transform against one), and Clang's plane throughput and latency (1.18, 1.21), and faster nowhere in nanoseconds. Its arithmetic is B's but for one more addition per motor; the differences are code generation and were not traced.

## 5. Break-even batch sizes

From the sweep (batch loop at 1, 2, 3, 4, 6, 8, 16 objects per motor; environment size 0, five rounds, a reduced protocol stated before the run):

| Compiler | Operation | B against A | B against the library | C against A | B against C |
| --- | --- | ---: | ---: | ---: | ---: |
| GCC | point | 1 | 1 | 1 | 3 |
| GCC | line | 2 | 6 | 1 | 6 |
| GCC | plane | 1 | 1 | 3 | 1 |
| Clang | point | 3 | 1 | 1 | 16 |
| Clang | line | 2 | 1 | 1 | 16 |
| Clang | plane | 1 | 1 | 1 | 16 |

At one object per motor the batch loop is a per-call loop with the motor work hoisted by hand, so "1" means B already wins per call in that loop. Clang's A is uneven at small sizes (21.1 cycles at 2 objects, 26.2 at 3) because it vectorizes pairs of objects; the break-even of 3 for Clang's point is that. Clang also vectorizes C across objects, which is why B needs 16 objects to beat C there.

## 6. Rounding: K, the bound, and the check

K per output (`bounds/README.md` section 1; core rule / exact doubling):

| Object | Output | A | B | C |
| --- | --- | ---: | ---: | ---: |
| point | x, y | 12 | 8/8 | 9/8 |
| point | z | 12 | 7/7 | 9/8 |
| point | w | 5 | 5/5 | 0/0 |
| line | vx, vy | 7 | 7/7 | 7/6 |
| line | vz | 7 | 6/6 | 7/6 |
| line | mx, my, mz | 14 | 11/10 | 14/12 |
| plane | gx, gy | 7 | 7/7 | 7/6 |
| plane | gz | 7 | 6/6 | 7/6 |
| plane | gw | 12 | 9/8 | 15/13 |

K depends on the summation order of the program as well as the form; these are the programs of `forms.py` (left folds in term order), and no order was searched.

- **B's bound is at most A's** (for motors the same M, and K no larger): about 2/3 of A's on a point's x and y (K 8 against 12) and 7/12 on its z (K 7 against 12), 0.71 to 0.79 on a line's moment, 0.67 to 0.75 on a plane's offset, and 1 or 6/7 elsewhere. The batch bound equals the per-call bound.
- **In practice A and B are equally accurate.** Over 1,000 rounded motors and 430 chains in each of double and float, under two samplers, both stayed within 0.62 of their bounds, and B's median error against the sandwich was 0.77 to 1.39 times A's. Against the intended motion they are indistinguishable. Collection buys operations, not accuracy.
- **B correlates errors in a batch.** One rounded matrix serves every object, so a whole batch moves by one perturbed linear map, where A's errors are separate per object. Derived, not measured.
- **C's own bound** is up to about 4.3 times A's (the plane's offset, γ_13 · 3.96 against γ_12, from a larger magnitude M_C, the maximum over 200 sampled unit motors), while its observed rounding error is similar to A's.
- **The rounding check of `transform`.** In rigid(3), a collected plan has nothing to drop, so checking mode's extra work (20, 12, 28 multiplications and 4, 2, 4 comparisons for point, line, plane) would go, and "nothing is lost" would be a structural fact the generator proves rather than a run-time check. In conformal(3), where the dropped part is not identically zero (note 0022's flat motor, dropped antiscalar 2h x4), the magnitude is unchanged and K falls from 9 to 6 (5 with exact doubling), so the tolerance would fall from γ_10 to γ_7 (or γ_6): the check would get stricter and note 0022's drifted chains would trip it more often. Inferred from structure, not computed. The machinery that computes the check's K and tolerance (`transform_dropped_roundings`, `transform_dropped_tolerances`, `padded_magnitude`) would have to run over the collected program; decision 0180's lemmas hold for any straight-line program.

## 7. Drifted motors

`bounds/drift.py`: seed 20261110, 21-bit dyadic stereographic step motors, N = 1 (1,000 correctly rounded unit motors) and chains of N = 10, 100, 1000 steps (300, 100, 30 chains) composed by the library's `compose` bit for bit; exact comparison in rationals.

Error against the sandwich of the stored motor (Galoot's definition of the result), max over coefficients, in units of form A's bound γ_(K_A) M_A:

| Scalar, N | point A | point B | point C | line A | line B | line C | plane A | plane B | plane C |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| double, 1 | 0.50 | 0.39 | 0.30 | 0.27 | 0.32 | 0.46 | 0.31 | 0.30 | 0.58 |
| double, 10 | 0.41 | 0.40 | 2.3 | 0.27 | 0.29 | 1.3 | 0.30 | 0.25 | 1.1 |
| double, 100 | 0.38 | 0.50 | 6.2 | 0.27 | 0.21 | 2.6 | 0.24 | 0.22 | 2.6 |
| double, 1000 | 0.40 | 0.33 | 14.1 | 0.22 | 0.24 | 8.4 | 0.25 | 0.25 | 7.1 |
| float, 1000 | 0.27 | 0.38 | 14.0 | 0.26 | 0.38 | 8.9 | 0.24 | 0.17 | 8.9 |

So C's error passes A's whole bound from N = 10 on and reaches 14 times it at N = 1000. With the first sampler, whose steps round the same way and bias λ, it reaches 16.8 times in double and 38.2 times in float at N = 1000 (`bounds/drift-small.md`). Against the intended motion (the exact product of the unrounded steps), where the chain's own rounding dominates every form, C's median geometric error at N = 1000 is 1.1 to 1.6 times A's with unbiased steps and up to 6.3 times in float with biased ones. A form C therefore needs λ restored (a unitization, which the core does not have in 1.0) or the correction of section 2, and in every case the assumption named in its interface.

## 8. Compile cost and generator complexity

**Thresholds** (largest single constant evaluation, `proto/README.md`): the pass alone, on the kernels written as literals, against the plan's largest evaluation.

| Case | GCC cold: pass / plan | Clang: pass / plan | Clang pass steps (limit 1,048,576) |
| --- | ---: | ---: | ---: |
| rigid(3) motor on point, line, plane | 2.10, 4.16, 2.11 | 1.76, 3.50, 1.77 | 69,953 to 139,513 |
| conformal(3) motor, grades 1 to 4 | 0.98 to 3.36 | 0.83 to 2.86 | 101,417 to 351,437 |
| conformal(3) 16-blade even versor, grade 1 | 2.43 | 2.03 | 481,823 |
| conformal(3) 16-blade even versor, grade 2 | 9.62 | 8.07 | 1,913,176 (refused) |

At n = 5 the pass is feasible for conformal(3) motors on every grade on both compilers (largest 2.2 M of GCC's 33.6 M; 351 k on Clang, a third of its limit). It is infeasible at Clang's default limit for the 16-blade even versor on grade 2 (1.8 times the limit); GCC fits it at 37% of its limit. The cause is that the prototype's `summarize` scans a dense table of X² V² coefficients several times, so its cost follows that size rather than the number of composed terms (Clang's steps divided by X² V² are 55 to 75 for all four cases measured). A sparse table, the composed terms sorted and merged, would cost about T log T for T composed terms and would bring the 16-blade case under the limit: inferred from the scaling, not built.

**Compiler instructions, time, memory** (pinned, three repeats): the pass adds 1.1 to 1.9 times the plan's own instructions on GCC in rigid(3) and 0.4 to 0.7 times on Clang; 0.3 to 1.7 times for the conformal(3) motor. Per unit, for the rigid(3) and conformal(3) motor cases, it adds 0.013 to 0.08 s of wall time and at most 16 MiB of peak memory; for the 16-blade stress case 0.09 to 0.38 s and up to 83 MiB on GCC. A program pays this once per (versor layout, object layout) pair it transforms, as it pays the plans. The wall times were measured with a load average of 2.8 to 3.3 from other users and are less reliable than the instruction counts.

**Generator complexity.** The prototype is one header of 408 lines (the pass and its run-time runner), reading the library's two kernels and working for any algebra and layouts. A core version would add, beyond the pass itself:

- a commutative-scalar concept, under which the object's scalars commute with the motor's, or an interface statement of that requirement (section 1);
- a place in the core: the prototype reads `detail::first_kernel` and `detail::second_kernel`, and C13 lets an extension reach the core only through its public interface, so a collected plan or batch interface is either core work or needs the kernels' term lists made public;
- a phase of its own in the cost model, which has no weights for it (the pass was measured, not predicted; the plans' thresholds equal the model's prediction in all 18 cases);
- the rounding-check machinery run over the new program, and a re-derived K for each tolerance (section 6);
- probably a sparse version for larger layouts (inferred);
- for a batch interface, the split into motor-only and per-object parts that the prototype's `setup` and `apply` already make.

Decision 0008 already reads collection as composition plus exact per-basis data, so it needs no new mechanism under C4, but it is a second program derived from the same terms, verified against the direct form (C3, C12).

## 9. Tests whose expected values or bits would change

From `bounds/README.md` section 8, if the collected form replaced `transform`'s kernels ("read" means the test was read; "inferred" from its description):

| Test | What changes | Status |
| --- | --- | --- |
| `test/bench/reference_kernels_test.cpp` (on `feature/cost-evidence`) | requires equal bits between `transform` and the form-A reference kernels over double; B differs in 57% to 77% of coefficients, so the reference kernels are regenerated from B | read |
| `rga3.op_counts_checking`, `rga3.op_counts_unchecked` (same branch) | counts 48, 72, 48 and checking-on 68, 84, 76 become B's 35, 53, 35 with no checking-mode extra; a W9 expected-value change, as decision 0233 item 2 anticipates | header read |
| `test/core/transform_test.cpp`, the kernels' kept and dropped sizes | pinned sizes change | read |
| `test/core/transform_rounding_test.cpp` | rigid(3) families would have nothing dropped; other families' K change with the program | read; consequences inferred |
| `test/core/identities_rounding_test.cpp` | tolerances come from `tracked` running the same computation, so they follow | inferred: no change of expectations |
| rga3 preset tests (`objects`, `motions`, `chains`, `point_groups`) | exact inputs, so values unchanged; a zero's sign may differ, which `==` ignores | bit comparisons read; the rest inferred |

A collected form under its own name, beside an unchanged `transform`, changes none of these; it adds its own tests. A form C in place of `transform` would change every exact test with a non-unit motor.

## 10. What the core could do exactly, and what needs a named assumption

| | Exact post-pass (form B) | Needs a named assumption (form C) |
| --- | --- | --- |
| Holds for | every motor, every object, any algebra the kernels cover | motors with λ = 1 (and h = 0 for a line's moment) |
| Equal to the sandwich | as polynomials, and in value over an exact scalar | only on the constraints |
| Over double | other bits than today (57% to 77% of coefficients), same magnitude for motors (smaller for a general even versor), K no larger, same observed accuracy | adds an error first order in λ − 1, up to 14 times A's bound on chains of 1,000 |
| Hidden property it relies on (C8) | the object's scalars commute with the motor's | λ = 1, h = 0 |
| Where it pays | a batch under one motor (from 1 to 3 objects) | a single call (and batches below 1 to 16 objects) |
| Could replace `transform` | yes in value; no in bits | no |
| Generated from the signature | yes, a compile-time pass over the existing kernels | the forms here were written by hand from closed forms (`forms.py`) and checked by Gröbner reduction; inferred: no core part reduces modulo an ideal, so a generator would need the constraints as data |

## 11. Does a named batch interface follow from the data?

The benchmark agent's reading, which decision 0234 item 6 also gives, is that the real D8 decision is a batch-under-one-motor interface rather than collection as `transform`'s default. Checked against the data:

- **That the default should not be collected follows.** Per call, B's gain against inline A is modest for points and planes (from 4% slower to 39% faster; the median of the eight cycle ratios, computed from `bench/out/tables.md`, is about 15%) and negative for lines on GCC in both loops and on Clang's throughput loop; it changes bits for every rounding scalar; it adds a compile-time phase. The size of the gain does not justify those costs for a single call.
- **That the gain lives in a batch follows.** Every (compiler, operation) pair shows B beyond the criterion against A in the batch loop, by a factor of 1.3 to 3.1, with break-even at 1 to 3 objects per motor. The saving is the motor's quadratic terms built once; only an interface that receives the motor once and many objects can use it.
- **That the batch interface belongs in 1.0 does not follow from the data.** The data show the shape, not a present user. C12 asks for one before an interface is added, and the measured gain against the library today includes the call that decision 0238's inlining unit will remove. Nor does the data choose between B and C for small batches: C wins below 1 to 16 objects per motor, at the price of its assumption.
- **A caveat on the size of the gain.** Against the library as it compiled here, much of Clang's batch gain (0.07 to 0.16) comes from the library's call blocking vectorization; inline A alone is 0.21 to 0.40 of the library there. After the inlining unit, the expected gain of a batch interface is the B / A row (inferred).
- **Two requirements the candidate carries, whenever it is built.** It needs a commutative-scalar concept, under which the object's scalars commute with the motor's (section 1), since no part of B's gain survives without it. And it cannot be an extension today: the prototype reads the core's `detail::first_kernel` and `detail::second_kernel`, and C13 lets an extension reach the core only through its public interface. So a batch interface is core work, or it needs the kernels' term lists made public first. This decides where the candidate unit lives, not whether it is wanted.

## 12. The D8 question for the owner

**D8: do collected plans enter core 1.0?** Plan 0001 records the criterion: in 1.0 only if Spike E and the P2 run show that composed plans violate C6 (cancelling components kept); otherwise defer by decision. The criterion is met or not by what the generator writes. The restricted kernels write only the object's blades, in every algebra and layout: the second stage keeps only the terms that land on positions of x, and checking mode verifies that the rest is zero. So no transform result keeps a component off x's blades, cancelling or not, and the criterion for 1.0 is not met. Lemma 1 is a separate and narrower fact: for motors (rigid(3), and conformal(3) on grades 1 to 4) the kernels also compute no term that cancels inside a kept output. For the conformal(3) 16-blade even versor they do (4% of the multiplications on grade 1, 9% on grade 2; section 1), which is wasted work rather than a kept component, in a case with no 1.0 user. The options:

- **(a) Defer collected plans past 1.0.** A decision record defers them with this evidence; `transform` keeps form A; DoD-4 stands on the declared-output check; DoD-24's counts stay those of form A (decision 0233 item 2 reading (a)). The batch interface (B) and a constraint form (C) are recorded as notes for after 1.0. Tradeoff: no batch speedup in 1.0 (0.32 to 0.76 of A's time per object, measured); nothing in the release train moves.
- **(b) Collected kernels as `transform`'s default in 1.0.** Tradeoff: per call up to 39% faster for points and planes (one loop 4% slower), up to 38% slower for lines on GCC; every bit-pinning test of a transform over double changes; the rounding check's K is re-derived and gets stricter on conformal(3); a new compile phase of 1.8 to 4.2 times the plan's largest evaluation, refused by Clang's default limit for one n = 5 case; a commutative-scalar concept (the object's scalars commuting with the motor's), which `transform` does not require today; a core unit late in the release train, after which E1, E2, G, and H rerun (plan 0038 section 5 step 7).
- **(c) A named batch-under-one-motor interface in core 1.0, built on an exact collection; `transform` unchanged.** Tradeoff: the measured batch gain (break-even 1 to 3 objects) with no change to existing tests or bits; but a new public interface without a present user (C12), its own plan, W5 review with the usability checklist, W10 records, a compile phase as in (b) for each pair it is used on, a commutative-scalar concept, and its own rounding tests; it must be core work (it reads the core's internal kernels, which C13 keeps from extensions) or wait for the term lists to be made public; it delays WU18.
- **(d) A named constraint-form operation (C) in 1.0.** Tradeoff: the fastest per call (7% to 44% against A); but an assumption the caller must keep (λ = 1, h = 0 for lines), errors up to 14 times A's bound on drifted chains, no unitization in 1.0 to restore λ, and forms derived by hand rather than generated (C3).

**Recommendation: (a)**, with (c) recorded as the first candidate after 1.0, to be planned when a user transforms many objects under one motor, after the inlining unit has run so that its gain is measured against an inline `transform`. The decision record should carry the candidate's two requirements: a commutative-scalar concept, and a home in the core (or public term lists) under C13. (b) is not supported by the per-call data, and (d) needs a unitization and a generator the core does not have. **Default** if the owner does not rule: none; the M7 gate waits for the ruling (plan 0038 O1).

## 13. Register: measured, proved, inferred

| Claim | Kind | Source |
| --- | --- | --- |
| A, B, C output polynomials; C equal modulo (λ − 1, h); which constraint each output needs | proved (exact symbolic) | `derivations.txt`, `derive.py` |
| Operation counts of A, B, C | counted by two independent counters, rechecked by the part B check | `counts.md`, `COUNTING-RULE.md` |
| M_B = M_A for rigid(3) motors (Lemma 1); K tables; S − C identities; D1 to D3 | proved (polynomial identities); rerun by the independent check | `bounds/derive_bounds.txt` |
| No cancellation inside kept outputs for conformal(3) motors on grades 1 to 4 | checked by counting paths against collected coefficients | `records/reviews/2026-11-10-spike-e-note-check/cancel.txt` |
| Cancellation for the conformal(3) 16-blade even versor (16 of 400 and 144 of 1,600 multiplications) | checked the same way; Lemma 1 is false there | the same file |
| B needs the object's scalars to commute with the motor's; no partial form exists | read from `transform.hpp`, `linear.hpp`, `collect.hpp` by the independent check; stated here | check record, finding 2 |
| A batch interface cannot be an extension (it reads `detail::` kernels; C13) | read from `collect.hpp` by the independent check | check record, check 4 |
| Bounds hold, A and B accuracy equal, C drift up to 14.1 (38.2) times A's bound | measured on seeded samples, not worst cases | `bounds/drift.md`, `drift-small.md` |
| A and B differ in 57% to 77% of coefficients | measured on the emulation (form A replayed bit for bit against the library) | `bounds/README.md` section 7, `replay.txt` |
| Per-call, batch, sweep times and verdicts | measured, one machine, `-O2`, SSE2; the batch cell for point and line on both compilers rerun at size 0 by the independent check, within 0.004 of the ratios | `bench/out/tables.md`, `results.tsv`, `verdict.tsv`; check record, check 2 |
| "Faster" where the criterion holds in cycles only, or a median ratio is lower without a verdict | marked as such in section 4 and the Answer | `bench/out/tables.md`, "The criterion, pair by pair" |
| B's per-call gain of about 15% for points and planes | computed: the median of the eight cycle ratios | `bench/out/tables.md` |
| Library kept out of line in the benchmark programs | measured (calls per transform) | `bench/out/stalls.tsv` |
| Pass thresholds, instructions, the Clang refusal | measured (deterministic thresholds, pinned compiles) | `proto/cost/thresholds.tsv`, `out/summary.txt`, `out-refused/runs.tsv` |
| A collected pass emits nothing on rigid(3)'s dropped blades | inferred (depends on the pass's design) | `bounds/README.md` section 6 |
| Conformal(3) check gets stricter (K 9 to about 6) | inferred from structure | `bounds/README.md` section 6 |
| Batch errors correlated under B | derived, not measured | `bounds/README.md` section 2 |
| A sparse pass fits Clang's limit for the 16-blade case | inferred from scaling | `proto/README.md` |
| The inlining unit brings the library to form A | inferred | `bench/README.md` |
| A core collected plan times like `bp` | inferred | `bench/README.md` |
| C plus (λ − 1) p is exact at about 30 multiplications | inferred (hand count, not built) | `bounds/README.md` section 5 |
| Form C is not generated by the core's machinery; a generator would need the constraints as data | inferred (`forms.py` writes C by hand; no core part reduces modulo an ideal) | `forms.py`, `derive.py`; check record, check 3 |
| A collection pass of the prototype's design would not remove E3's compile-time growth | inferred (the pass reads kernels built from the growing antiproduct plans; another route is not excluded; the pass's own growth with n not measured) | E3 README; `proto/README.md` |
| No transform result keeps a component off x's blades (D8's criterion) | read from the generator (the second kernel keeps only positions of x; checking mode verifies the rest), confirmed by the independent check; not a separate test | this note, section 12; check record, check 3 |

## What remains unverified

- Any machine other than the i9-14900K, any flag set other than `-O2` with SSE2, and `-O3` or wider vector targets.
- The two unexplained benchmark readings (departure 3) and the code-generation causes of `bp`'s slower cells.
- Worst-case inputs: every error ratio is sampled.
- Flector layouts, odd versors, and conformal(3) objects beyond the prototype's correctness check: no timing or bound there, and Lemma 1's cancellation count was not made for flectors or odd versors.
- The parts of the evidence the independent check did not rerun: the plane, the per-call loops, `bp`, the sweep, environment sizes 1,352 and 2,768, `drift.py --small-steps`, and the prototype's compile-cost runs.

## Reproduction

Each record gives its commands; from a checkout of this branch:

- Part B: `<math-tools>/toolset/env/py/.venv/bin/python -B docs/agent/records/spikes/spike-e/derive.py` (`README.md`).
- Benchmarks and sweep: `bench/build.sh`, then `bench/batch.sh calibrate`, `run 0`, `run 1352`, `run 2768`, `sweep`, `stalls`, then `drive.py summarize` and `analyze.py` (`bench/README.md`, "What ran"); the timed runs need the timing lock and CPU 4.
- Prototype compile cost: `proto/cost/units.py`, `predict.py`, `thresholds.py`, then `tools/compiles cost` on `units.toml` and `units-gcc.toml` (`proto/README.md`).
- Bounds and drift: `bounds/derive_bounds.py`, `bounds/drift.py` (about 10 minutes), `drift.py --small-steps`, and `replay.py` against the probe (`bounds/README.md`, "Reproduction").

No Terathon source, no book, and no web page was read for this note, and no web request was made.
