"""Cross-check the geometric product of transform_sandwich_check.py against
galgebra, a library of another lineage, on every pair of basis blades of
R(3, 0, 0), R(2, 1, 0), and R(3, 0, 1), and the sandwich values of the note
in R(3, 0, 0).

Run with an interpreter that has galgebra (0.6.0 was used):
  python -I galgebra_crosscheck.py
"""

import importlib.util
import pathlib
import sys

import sympy
from galgebra.ga import Ga

here = pathlib.Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("check", here / "transform_sandwich_check.py")
check = importlib.util.module_from_spec(spec)
spec.loader.exec_module(check)


def blades_of(ga, gens, n):
    """Map each mask to galgebra's product of its generators in order."""
    out = {}
    for mask in range(1 << n):
        value = ga.mv(sympy.Integer(1), "scalar")
        for i in range(n):
            if mask >> i & 1:
                # Distinct generators of a diagonal form: the product of the
                # generators in order is the blade.
                value = value * gens[i]
        out[mask] = value
    return out


def is_zero(mv):
    return sympy.simplify(mv.obj) == 0


failures = 0
for squares in [(1, 1, 1), (1, 1, -1), (1, 1, 1, 0)]:
    n = len(squares)
    names = " ".join(f"e{i + 1}" for i in range(n))
    ga, *gens = Ga.build(names, g=list(squares))
    theirs = blades_of(ga, gens, n)
    alg = check.Algebra(squares)
    pairs = 0
    for a in range(1 << n):
        for b in range(1 << n):
            c, m = alg.blade_product(a, b)
            if not is_zero(theirs[a] * theirs[b] - sympy.Integer(c) * theirs[m]):
                failures += 1
            pairs += 1
    print(f"{check.name(squares)}: {pairs} blade pairs compared")

# The note's sandwiches, by galgebra alone.
ga, e1, e2, e3 = Ga.build("e1 e2 e3", g=[1, 1, 1])
for mirror, vector, want in [
    (e1 + e2, e1, 2 * e2),
    (e1 + e2, e3, 2 * e3),
    (2 * e1 + e2, e1, 3 * e1 + 4 * e2),
    (2 * e1 + e2, e2, -4 * e1 + 3 * e2),
]:
    r = mirror * e1
    if not is_zero(r * vector * r.rev() - want):
        failures += 1
print("sandwich values compared: 4")
print("agree" if failures == 0 else f"{failures} DISAGREE")
sys.exit(1 if failures else 0)
