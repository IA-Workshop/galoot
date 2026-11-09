# WU12 planning probe: what a change of basis preserves, and Sage's lift_isometry

Written 2026-11-08 by Claude (agent, Opus 5.5, planner of plan 0030) on `records/plan-wu12`. These are planning probes. They support the hypotheses in plan 0030 section 5. They are sampled evidence (VVUQ V9), not derivations, and nothing here runs the library.

## naturality.py

For nine changes of basis P (row i writes the i-th source generator in the target's generators, P G′ Pᵀ = G), the script draws 20 seeded random multivectors with integer coefficients in [−3, 3]. For each operation it looks for a factor f among ±1, (det P)^±1, (det P)^±2 with φ(op(x, y)) = f · op′(φ(x), φ(y)). Products, minors, and the LDLᵀ come from `test/reference/clifford_reference.py`. The complements, the bulk, the weight, the duals, the dot, and the antidot are written in the script from their definitions in CONVENTIONS 3 and 4, as `conventions_check.py` writes them. Seed 7.

```
python3 -I docs/agent/records/spikes/wu12-planning-probe/naturality.py test/reference > naturality-out.txt
```

Run with CPython 3.14.9 at `774ff66`. It takes under a second and is not run through `tools/timed`, because it builds and tests nothing of the library. The output is in `naturality-out.txt`. What it shows:

- The geometric product, the wedge, the dot, the reverse, the antireverse, and the grade involution are preserved with factor 1 in all nine cases.
- The antiwedge and the bulk dual are preserved in all nine cases, with factor −1 where det P = −1 and 1 where det P = 1. A short argument predicts 1/det P for the antiwedge and det P for the bulk dual (plan 0030 section 5.2). Every case tried has det P = ±1, so the probe cannot tell det P from its inverse for any factor.
- The geometric antiproduct, the weight dual, and the antidot are preserved, with the same factors of ±1, in the seven cases where the contragredient P^(−T) is also an isometry (P^(−T) G′ P^(−1) = G, printed on each case's first line). They are not preserved by the null shear of R(1,0,1) or by the LDLᵀ change of random form 1, where the contragredient is not an isometry.
- The right and left complements, the bulk, and the weight are preserved only by the two orthogonal changes (a rotation and a permutation), with factor 1 and det P. They are not preserved by any other change: the CONVENTIONS 1.4 conformal change, the Witt change, the boost, the shear, or the LDLᵀ change.

## sage_lift_isometry.py

For conformal(1) to conformal(3), the script builds Sage's `CliffordAlgebra(QuadraticForm(QQ, 2G))` and `lift_isometry(Pᵀ)`. It checks that the codomain's Gram matrix is that of R(d+1,1,0), converts each wedge monomial's image into wedge coordinates (Route A), and compares it with the minors of P. It also checks that the lift is a homomorphism on every pair of wedge monomials.

```
/home/brianm/projects/utils/math-core/math-tools/toolset/env/py-sage/.venv/bin/python -I \
  docs/agent/records/spikes/wu12-planning-probe/sage_lift_isometry.py > sage-out.txt
```

The environment is passagemath-modules 10.8.13, passagemath-categories 10.8.13, and gmpy2 2.3.2, with `uv.lock` sha256 `5396cb034418174f…` and CPython 3.14.9. The run took about 1.6 s. Output in `sage-out.txt`: 0 mismatches over 64, 256, and 1,024 entries, 0 homomorphism failures, and det P = −1 for each d.

Before this run, a first run through `uv run --frozen --project …/py-sage` recreated that virtual environment from the frozen lock. It picked CPython 3.14.9 where the math-tools record names 3.14.8, and the packages stayed at the locked versions. The rerun above used the environment's interpreter directly.
