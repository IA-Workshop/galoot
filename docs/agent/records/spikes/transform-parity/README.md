# Transform parity: the sandwich forms, the dropped parts, and the Projections page

`check.py` is the mechanical check of the proofs in decision 0177 (the parity rule of the versor transform, the sign of the odd form, and the dropped parts in rigid(3)) and of the formula recomputation in research note 0021 (the rigid wiki's Projections page). It reproduces, as a committed and seeded script, the planning probes `parity.py` and the dropped-part half of `antiproject.py` of plan 0021 departure 6.

Written by Claude (agent, Opus 5.5, math-expert role) for WU13a, 2026-11-07.

## Command

From the root of a Galoot checkout:

    python3 -I -B docs/agent/records/spikes/transform-parity/check.py .

The argument is the checkout to read `test/reference/clifford_reference.py` from (default: the checkout that holds the script). The script writes nothing and exits 1 if any check fails. Timed as `tools/timed check - transform-parity -- python3 -I -B docs/agent/records/spikes/transform-parity/check.py .`: 4.1 s wall, Python 3.14.9, at revision db1a349 with this script uncommitted (the row in `records/performance/runs.tsv` of 2026-11-07T11:48:30Z); the committed script is byte for byte the one that ran.

## Seed and sample counts

- Seed 20261107 (`random.Random`), entries of random vectors drawn from the integers in [-3, 3].
- Eight forms: rigid(2), R(3,0,0) (n = 3); rigid(3), R(2,2,0), conformal(2) (n = 4); rigid(4), conformal(3), R(4,1,0) (n = 5). The conformal forms are Gram matrices that are not diagonal, multiplied by `clifford_reference.py`; on the diagonal forms the script's own product table is compared with `clifford_reference.py`'s on every blade pair.
- Per form, 10 versors for each count of 1, 2, and 3 hyperplanes (30 versors), each hyperplane the wedge of n − 1 random vectors with a nonzero V ⟇ V̰. For each versor and each of the twelve candidate forms (sign ±; involution of x none, grade, or antigrade; V̰ or the reverse Ṽ), the outermorphism property λ^(k−1) T(a1 ∧ … ∧ ak) = T(a1) ∧ … ∧ T(ak) on one random k-tuple of vectors for each k from 1 to n; for the single hyperplanes, the mirror property T(a) = λa on the n − 1 vectors that span it, and T(b) ≠ λb for one vector b off it.
- Composition T_(W⟇V) = T_W ∘ T_V on every blade, for 3 pairs of each combination of parities (12 pairs per form), for the rule and for the two families that follow the rule by the grade involution at one parity.
- The antigrade lemma on 10 random one-parity elements V (not versors) and random x of one antigrade per form, and symbolically in rigid(3).
- Symbolic (polynomial) checks: the dropped parts of the five rigid(3) layouts under generic motor and flector layouts; the antigrade lemma in rigid(3); the ten formulas of the Projections page.

## Output

```
python 3.14.9; seed 20261107; 10 versors per form and number of hyperplanes (1, 2, 3); 3 pairs per pair of parities; entries in [-3, 3]
rigid(2) | n 3 | even: +V % x % V~, +V % x^ % V~, -V % x' % V~, -V % x % rev(V), -V % x^ % rev(V), +V % x' % rev(V) | odd, outermorphism: +V % x % V~, +V % x^ % V~, -V % x' % V~, -V % x % rev(V), -V % x^ % rev(V), +V % x' % rev(V) | odd, and mirror fixed: +V % x^ % V~, -V % x' % V~, -V % x^ % rev(V), +V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [1] | composes: rule True, odd twisted True, even twisted False
R(3,0,0) | n 3 | even: +V % x % V~, +V % x^ % V~, -V % x' % V~, -V % x % rev(V), -V % x^ % rev(V), +V % x' % rev(V) | odd, outermorphism: +V % x % V~, +V % x^ % V~, -V % x' % V~, -V % x % rev(V), -V % x^ % rev(V), +V % x' % rev(V) | odd, and mirror fixed: +V % x^ % V~, -V % x' % V~, -V % x^ % rev(V), +V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [1] | composes: rule True, odd twisted True, even twisted False
rigid(3) | n 4 | even: +V % x % V~, +V % x^ % V~, +V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, outermorphism: -V % x % V~, -V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, and mirror fixed: -V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), +V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [-1] | composes: rule True, odd twisted True, even twisted False
R(2,2,0) | n 4 | even: +V % x % V~, +V % x^ % V~, +V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, outermorphism: -V % x % V~, -V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, and mirror fixed: -V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), +V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [-1] | composes: rule True, odd twisted True, even twisted False
conformal(2) | n 4 | even: +V % x % V~, +V % x^ % V~, +V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, outermorphism: -V % x % V~, -V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), +V % x' % rev(V) | odd, and mirror fixed: -V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), +V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [-1] | composes: rule True, odd twisted True, even twisted False
rigid(4) | n 5 | even: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, outermorphism: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, and mirror fixed: +V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), -V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [1] | composes: rule True, odd twisted True, even twisted False
conformal(3) | n 5 | even: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, outermorphism: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, and mirror fixed: +V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), -V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [1] | composes: rule True, odd twisted True, even twisted False
R(4,1,0) | n 5 | even: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, outermorphism: +V % x % V~, +V % x^ % V~, -V % x' % V~, +V % x % rev(V), +V % x^ % rev(V), -V % x' % rev(V) | odd, and mirror fixed: +V % x^ % V~, -V % x' % V~, +V % x^ % rev(V), -V % x' % rev(V) | odd sign s in s V % x^ % V~ by the outermorphism check: [1] | composes: rule True, odd twisted True, even twisted False
motor on point: reaches 8, drops 4, nonzero dropped 0
motor on line: reaches 8, drops 2, nonzero dropped 0
motor on plane: reaches 8, drops 4, nonzero dropped 0
motor on motor: reaches 8, drops 0, nonzero dropped 0
motor on flector: reaches 8, drops 0, nonzero dropped 0
flector on point: reaches 8, drops 4, nonzero dropped 0
flector on line: reaches 8, drops 2, nonzero dropped 0
flector on plane: reaches 8, drops 4, nonzero dropped 0
flector on motor: reaches 8, drops 0, nonzero dropped 0
flector on flector: reaches 8, drops 0, nonzero dropped 0
R(4,0,0), V = 1 + I, x = 1: V % x % V~ = 2 + 2 I, the I part dropped; V % V~ = 2 + 2 I
R(6,0,0), V = 1 + I: V % V~ = 2 I, yet V % e1 % V~ = -2 e23456
rigid(3): rotor (3/5) e43 + (4/5) I on (1, 0, 0, 1) -> (7/25, 24/25, 0, 1)
rigid(3): rotor 3 e43 + 4 I on (1, 0, 0, 1) -> (7, 24, 0, 25)
rigid(3): translator e23 + 2 e31 - e12 + I on (5, 7, -1, 1) -> (7, 11, -3, 1)
rigid(3): mirror e412 on (1, 2, 3, 1) -> (1, 2, -3, 1)
rigid(3): rotor (3/5) e43 + (4/5) I composed with itself on (1, 0, 0, 1) -> (-527/625, 336/625, 0, 1)
rigid(3): rotor 3 e43 + 4 I composed with itself on (1, 0, 0, 1) -> (-527, 336, 0, 625)
rigid(2): mirror e23 on (1, 2, 1) -> (-1, 2, 1)
projection of point p onto plane g: agrees
projection of point p onto line l: agrees
projection of line l onto plane g: agrees
antiprojection of plane g onto point p: agrees
antiprojection of line l onto point p: agrees
antiprojection of plane g onto line l: agrees
projection of the origin onto plane g: agrees
projection of the origin onto line l: agrees
antiprojection of the horizon onto point p, bulk dual: is the negation of the page's row
antiprojection of the horizon onto line l, bulk dual: agrees
forms                  3 checks
product tables        12 checks
complements          320 checks
involutions          480 checks
versors              480 checks
parity               168 checks
composition          120 checks
antigrade lemma       90 checks
dropped parts         20 checks
hand values           12 checks
projections page      15 checks
total 1720 checks
all checks passed (seed 20261107)
```

In the per-form lines, "even" lists the candidates that pass the outermorphism check for even V; "odd, outermorphism" those for odd V; "odd, and mirror fixed" those that also fix the mirror. The rule is V ⟇ x ⟇ V̰ for even antigrades and −V ⟇ x̌ ⟇ V̰ for odd ones (x̌ the antigrade involution, written x' in the output).

## What it is evidence for

- **Proofs by exhaustive or symbolic check (V1 kind 1, for the form and layouts named).** The polynomial checks are identities in the variables. They prove, in rigid(3): no layout of the five (point, line, plane, motor, flector) drops a nonzero coefficient under a generic V of the motor or flector layout, which need not be a versor; V ⟇ x ⟇ V̰ keeps the antigrades of x modulo 4 for every one-parity V; and the ten Projections page formulas, nine as written and the horizon-onto-point row as its negation. The checks on every blade (complements, involutions, the complement identities right(x̌) = right(x)^ and right(x̰) = reverse(right(x)), x̌ = (−1)^n x̂, the product tables) are exhaustive for each form.
- **Samples (V9: hypotheses, not theorems).** The classification of the candidate forms on 30 versors per form, and the composition checks on 12 pairs per form, are samples, seed 20261107. They agree with decision 0177's proof, which covers every n and every form under the hypothesis stated there; the samples cover only antiproducts of hyperplanes. Observed: for every form, the outermorphism check passes exactly the candidates equal to the rule or to the rule followed by x̂ (two maps for each parity); the mirror check keeps only the rule for odd V; among s V ⟇ x̂ ⟇ V̰ for odd V the outermorphism check alone passes s = +1 for n = 3 and 5 and s = −1 for n = 4; the rule composes, the family with odd versors followed by x̂ also composes, and the family with even versors followed by x̂ does not.
- **Hand values** (exact): R(4,0,0) with V = 1 + 𝟙 and x = 1 gives V ⟇ x ⟇ V̰ = 2 + 2𝟙, of which 2𝟙 is dropped, and V ⟇ V̰ = 2 + 2𝟙 is not a multiple of 𝟙; R(6,0,0) with V = 1 + 𝟙 has V ⟇ V̰ = 2𝟙 and still sends e1 to −2 e23456, so V ⟇ V̰ = λ𝟙 alone does not make V act on vectors; the rigid(3) rotor, translator, mirror, and composed-rotor values; the rigid(2) mirror e23.
- **Not evidence for** the library: nothing here runs Galoot's code. Not validation (V8) except where the Projections page and the rigid(3) values agree with a published source (research notes 0005 and 0021).

Ten deliberately wrong variants of the script were each rejected (exit 1): the odd sign +1; the grade involution in place of the antigrade involution in the odd rule; no involution in the odd rule (the single-sign form); the reverse in the even rule; the antiprojection with the bulk dual; the projection in place of the antiprojection; the antireverse computed as the reverse; the diagonal product table without its reordering signs; the transform's parity taken from the grade; and one sign flipped in a transcribed page formula. Those runs were timed into a scratch log, not the run log.
