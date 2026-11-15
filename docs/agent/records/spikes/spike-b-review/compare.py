"""Compare the reviewer's Chevalley-route tables with the repository script's
LDL-route tables, on the note's 14 signatures and on half-integer forms."""
import random
import sys
from fractions import Fraction

sys.path.insert(0, "/home/brianm/projects/utils/math-core/galoot/test/reference")
sys.path.insert(0, "/tmp/claude-1000/-home-brianm-projects-utils-math-core/"
                   "a4e01acb-9ef0-4fac-8e6b-40081401af04/scratchpad/review")
import clifford_reference as ref  # noqa: E402
import chevalley_check as mine  # noqa: E402


def mask(blade):
    return sum(1 << i for i in blade)


def compare(name, g):
    alg = ref.Algebra(g)
    theirs = alg.table()
    n = len(g)
    bad = 0
    for A in mine.blades(n):
        for B in mine.blades(n):
            m = {mask(C): c for C, c in mine.product(A, B, g).items()}
            t = {k: v for k, v in theirs[(mask(A), mask(B))].items() if v}
            if m != t:
                bad += 1
                if bad < 4:
                    print("  DIFF", name, A, B, m, t)
    coeffs = [c for terms in theirs.values() for c in terms.values()]
    maxabs = max(abs(c) for c in coeffs)
    allint = all(c.denominator == 1 for c in coeffs)
    dyadic = all(ref.is_dyadic(c) for c in coeffs)
    print(f"{name}: n={n} det={ref.det(g)} mismatches={bad} integer={allint} "
          f"dyadic={dyadic} max|c|={maxabs} fails={alg and ref.validate(alg, theirs)}")
    return bad


sigs = [("conformal(3)", ref.conformal(3)), ("R(1,3,0)", ref.r_pqr(1, 3, 0)),
        ("rigid(3)", ref.rigid(3)),
        ("degenerate", [[0, 1, 0], [1, 0, 0], [0, 0, 0]])]
sigs += [(f"random[{k}]", g) for k, g in enumerate(ref.random_forms())]
total = sum(compare(*s) for s in sigs)
print("mismatches on the 14 note signatures:", total)

# half-integer forms through both routes
rng = random.Random(7)
for trial in range(8):
    n = 2 + trial % 3
    g = [[Fraction(0)] * n for _ in range(n)]
    for i in range(n):
        for j in range(i, n):
            g[i][j] = g[j][i] = Fraction(rng.randint(-3, 3), 2)
    total += compare(f"half[{trial}]", g)
compare("hyperbolic g12=1/2", [[0, Fraction(1, 2)], [Fraction(1, 2), 0]])
print("total mismatches:", total)

# max|c| = |det g| hypothesis on nonsingular integer forms (note B2, Inferred)
rng = random.Random(99)
viol = []
for trial in range(300):
    n = 2 + trial % 3
    g = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(i, n):
            g[i][j] = g[j][i] = rng.randint(-3, 3)
    d = ref.det(g)
    if d == 0:
        continue
    t = ref.Algebra(g).table()
    mx = max(abs(c) for terms in t.values() for c in terms.values())
    if mx != abs(d):
        viol.append((g, d, mx))
print(f"max|c| == |det g| violated in {len(viol)} of the nonsingular forms")
for v in viol[:5]:
    print("  ", v)
