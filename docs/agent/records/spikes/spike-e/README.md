# Spike E, part B: derivations and operation counts

Written by Claude (agent, Sonnet 5.5, not the Opus the brief named) in the math-expert and numerical-expert roles, 2026-11-10, on `research/spike-e` from `develop` at `2dce8bf`. No change under `include/`; no build, no pool, no lock.

The question: for the five operations of the cost criterion (point ^ point, point ^ point ^ point, line v plane, transform(point, motor), transform(line, motor)) and for transform(plane, motor) and compose(motor, motor) in rigid(3), what are the output polynomials, and what do three forms of each transform cost?

- Form A: the restricted composition the kernels compute (V ⟇ x kept where read, then ⟇ V̰ kept on x's blades).
- Form B: the collected form. The sandwich is linear in x, so it is a matrix M(Q) x whose entries are quadratic in the motor. Like terms are combined (Q_a Q_b + Q_b Q_a = 2 Q_a Q_b), each distinct product of two motor coefficients is made once and shared by every entry that uses it, and a factor 2 shared by all atoms of an entry is applied once to the entry's sum.
- Form C: a form that uses the unit constraint λ = Q_vx² + Q_vy² + Q_vz² + Q_vw² = 1 and the geometric constraint h = Q_v · Q_m + Q_vw Q_mw = 0 (the errata page's kind).

## Files

| File | What |
| --- | --- |
| `rigid3.py` | rigid(3) as exact polynomial algebra, written from the definitions: geometric product, wedge, complements, antiproduct, antiwedge, antireverse, the oriented layouts of the presets |
| `plans.py` | the term lists of form A, in the plan's order, and their kernel accumulation |
| `dag.py` | straight-line programs with shared nodes; counter 1 (tally of nodes) and counter 2 (generated text run on a counting number) |
| `forms.py` | forms A, B, C as programs |
| `derive.py` | every derivation, check, and count; writes `derivations.txt`, `counts.json`, `counts.md` |
| `COUNTING-RULE.md` | the counting rule, in prose |
| `derivations.txt` | the output polynomials and the run's log |
| `counts.md`, `counts.json` | the tables of the run |
| `bench/` | part F: the forms timed against each other on both compilers, with the batch sweep (its README) |
| `proto/` | part F: the collection-pass prototype and its compile cost (its README) |

Command, from a Galoot checkout, with the math-tools Python environment (sympy 1.14):

    <math-tools>/toolset/env/py/.venv/bin/python -B docs/agent/records/spikes/spike-e/derive.py

It takes about two seconds, reads `test/reference/clifford_reference.py` from the checkout, prints PASS or FAIL for each check, and exits 1 on a failure. Seed 20261110 draws the sample points and motors. It was not run through `tools/timed`, which would add a row to the shared log for two seconds of Python.

## The polynomials

All are in `derivations.txt`, in the coefficient letters and layouts of CONVENTIONS 5.1 to 5.3 and 5.8 (line e41 e42 e43 e23 e31 e12, plane e423 e431 e412 e321, motor e41 e42 e43 𝟙 e23 e31 e12 1). The ones worth knowing by heart, with v = (Q_vx, Q_vy, Q_vz), s = Q_vw, m = (Q_mx, Q_my, Q_mz), t = Q_mw:

- point ^ point: direction w_p q - w_q p, moment p × q, for points of any weight.
- point ^ point ^ point: coefficient j is the cofactor det[e_j; p; q; r], j = x, y, z, w.
- line v plane, with the line (d, k) and the plane (n, g_w): the point (g_w d - n × k, -n · d).
- transform(point, motor), with no constraint: (s² - |v|²) u + 2 (v · u) v + 2 s v × u + w T for the position u = (x, y, z), with T = 2 (s m - t v + v × m), and the weight is λ w.
- compose(Qa, Qb) = Qa ⟇ Qb is bilinear; each of its eight outputs has 4 or 8 monomials (the 𝟙 and e41-e43 outputs 4, the moment outputs 8), and transform(x, compose(Qa, Qb)) equals transform(transform(x, Qb), Qa) as a polynomial identity, which the script checks for a point.

For every motor-layout element, whatever its coefficients, the part of the sandwich off x's blades is zero as a polynomial (plan 0021's R10, rechecked here).

## Form C, and which constraint each needs

With a = v × u:

- point: p' = p + 2 (s c + v × c - w t v), c = v × p + w m, and w' = w.
- plane (n, g_w): n' = n + 2 (s a + v × a), g_w' = g_w - n' · T.
- line (d, k): d' = d + 2 (s a + v × a), k' = R k + T × d', R the same rotation.

Checked by dividing the difference of each output from the sandwich by a Gröbner basis of the ideal (λ - 1, h): the remainder is 0 for all outputs of all three. Which constraint is needed, by dividing by one generator alone:

| Object | needs λ = 1 | needs h = 0 |
| --- | --- | --- |
| point | yes, and enough alone | not needed |
| plane | yes, and enough alone | not needed |
| line, direction | yes, and enough alone | not needed |
| line, moment | yes | yes, both |

The log shows the difference at a versor of λ = 4, at λ = 1 with h ≠ 0, and at a random element of the layout: nonzero off λ = 1 for every object; nonzero for the line's moment off h = 0 and zero for the point and the plane there. At a rational unit versor (built as rotor ⟇ translator ⟇ rotor) all three forms equal the sandwich exactly. The geometric constraint matters only for the line's moment: a form for points or planes that needs h = 0 has not used it.

## Counts

`counts.md` has the full tables. In brief, per call, rule S, multiplications and additions (subtractions included), neg 0 in every row:

| Transform | A (kernels) | B (collected) | C (constraints, motor doubled first) | C (factor 2 on results) |
| --- | --- | --- | --- | --- |
| point | 48 mul, 36 add | 35 mul, 35 add, 9 scl | 22 mul, 18 add, 4 scl | 22 mul, 18 add, 3 scl |
| line | 72 mul, 58 add | 53 mul, 62 add, 15 scl | 48 mul, 39 add, 7 scl | 48 mul, 39 add, 9 scl |
| plane | 48 mul, 36 add | 35 mul, 35 add, 9 scl | 30 mul, 24 add, 7 scl | 30 mul, 24 add, 6 scl |

Form A under the kernels' own accumulation (rule K) has the same `mul` and `add + sub`, and `neg` of 3 (point), 9 (line), 5 (plane); in checking mode the first stage is whole and the dropped-blade terms are computed, which gives 68, 84, 76 multiplications. The multiplication counts 48, 72, 48 and compose's 48 equal the restricted counts of plan 0021's probes.

For a batch under one motor, per object: B 13 mul + 9 add (point and plane, same matrix) or 27 + 21 (line), against A's 48 (point, plane) and 72 (line); the motor-only cost of B is 22 mul + 26 add + 9 scl (point, plane) and 26 + 41 + 15 (line). C per object, motor doubled first: 22 + 18 (point), 36 + 30 (line), 18 + 15 (plane), with 12 mul + 9 add + 7 scl once per motor for T (line, plane).

Products and compose, form A (rule K, declared layout of the result): point ^ point 12 mul; point ^ point ^ point 24 mul (two wedges, 12 each, against 48 for the expanded determinants); line v plane 12; compose 48 mul, 16 add, 24 sub, 4 neg on the canonical layout compose returns. Like terms: none in point ^ point, line v plane, or compose (the monomial count of each output polynomial equals the number of terms of form A), so B and C equal A there. In point ^ point ^ point the expanded form has 6 monomials of degree 3 in each of the four outputs; A shares the two-factor minors and is cheaper than evaluating those monomials separately. Form C for compose and the products is not defined: a unit constraint does not remove a term from a bilinear form.

## Do the counts of the errata page come out?

No. The errata copy states 54 (line) and 35 (plane) multiply-adds for the first printing's transform with a motor, each including the cross products that set up the motor's terms (6 for each). These recomputations are of forms I derived, not of the book's formulas, which nobody has read. In the multiply-add unit of COUNTING-RULE.md section 5, form C of the line is 48 or 54 (the doubling applied to the motor first, or to the results) and 55 or 63 with doublings counted as units; the plane's is 30 or 33 and 37 or 39. So 54 appears for the line in one arrangement and 35 does not appear for the plane; the spread between arrangements of one form (6 to 9) is as large as the difference from the published numbers, so the counts neither confirm nor refute an erratum. Recording them as a difference of counting rule (what a doubling is, how a sum is fused, whether the motor's own terms are in the total) is the supported reading; an erratum would need the book's formula.

One more difference the page's own text shows: it says Q_vw is the coefficient of the scalar term and Q_mw of the antiscalar, in a correction to Equation (3.219) of the fifth and earlier printings; the conventions (5.8) and the preset have Q_vw on 𝟙 and Q_mw on the scalar, as the wiki's Motor page does. The constraint h = 0 is symmetric in the two, so no count or polynomial here depends on which.

## Checks, and how each half checked the other

Every check is a PASS line in `derivations.txt`.

- The geometric product table of `rigid3.py` equals `clifford_reference.py` on all 256 blade pairs (the reference diagonalizes a Gram matrix; `rigid3.py` multiplies masks).
- The product polynomials equal closed forms written in plain linear algebra (cross products, determinants, (p ^ q) v g = (g · p) q - (g · q) p from the knowledge base).
- The point transform equals the hand form above with no constraint.
- A, B, and C equal the sandwich as polynomials (C modulo the ideal); the kernels' accumulation run on sympy equals the sandwich and its dropped-blade terms sum to zero.
- Numerical half on the math half: B's counts recomputed from the polynomials alone (22 distinct quadratic monomials, 13 nonzero entries, 9 row additions for the point, and the line's and the plane's); the number of monomials of each product's output against the term counts; every form's generated text executed on counting numbers at random rationals gives the tally of counter 1 and the values of the polynomial; the fused count of counter 1 equals a second tally written from the generated statements; cross and dot products counted by hand.
- Math half on the numerical half: the multiplication counts of the kernel emulation equal the probe counts of plan 0021; the point's additions and subtractions equal the assembly's; the checking-mode multiplications 68, 84, 76 equal the probes' whole-product counts.

What this does not give: the two halves were written by one agent, so the checks catch a program's inconsistencies and misreadings of the rule, not a shared misunderstanding of a definition. An independent reviewer (plan section 9) should re-derive at least the rule K term order from `plan.hpp` and `mv.hpp` (read here, not changed).

## Departures and ambiguities

- The plan names `records/spikes/spike-e/`; records live under `docs/agent/records/spikes/`, so the files are in `docs/agent/records/spikes/spike-e/`.
- One agent did both halves; no second agent ran.
- I read `include/galoot/core/{transform,mv,plan,blade_set}.hpp` (read only) to state rule K, because the counting scalar's counts depend on the kernels' accumulation (a first term of sign -1 is a `neg`). Rule K assumes the term order of the plan (first operand's position, then second's); the multiplication totals are verified against plan 0021's probes, the neg counts only by the same reading.
- Where `wedge` and `antiwedge` write into a declared layout, the orientation's sign is folded into the terms; that is how `transform` writes into x's layout, and it is assumed (not read) for products. The tables give both that and the canonical layout; the neg counts differ.
- Not read: the constitution, the workflow rules, VVUQ, PERFORMANCE, and STYLE (only CONVENTIONS in full, and the parts of plans 0038 and 0021 and research note 0022 that this part touches); decisions 0233, 0008, and 0013 were read in full. No Terathon source, no book, and no web page were read; the errata page's text is the knowledge base's copy and its request to contact the author was not acted on. No web request was made.
- B is not a minimum over evaluation orders; the only sharing is of identical products and of identical sums. C has two arrangements, not a search.
