# Independent expected plans from test/reference/clifford_reference.py.
import sys
sys.path.insert(0, sys.argv[1])
from fractions import Fraction
import clifford_reference as cr
sys.path.insert(0, sys.argv[2])
from cases import CASES

def oriented(indices):
    # sign of the permutation that sorts the written index list
    inv = sum(1 for p in range(len(indices)) for q in range(p+1, len(indices)) if indices[p] > indices[q])
    mask = sum(1 << (i-1) for i in indices)
    return mask, (-1 if inv % 2 else 1)

def canonical_key(mask):
    return (bin(mask).count("1"), [i for i in range(32) if mask >> i & 1])

for name, c in CASES.items():
    alg = cr.Algebra(c["gram"])
    A = [(oriented(ix)[0], oriented(ix)[1]*s) for ix, s in c["A"]]
    B = [(oriented(ix)[0], oriented(ix)[1]*s) for ix, s in c["B"]]
    for ai, (ma, sa) in enumerate(A):
        for bi, (mb, sb) in enumerate(B):
            prod = alg.mul({ma: Fraction(1)}, {mb: Fraction(1)})
            for m in sorted(prod, key=canonical_key):
                v = prod[m] * sa * sb
                assert v.denominator == 1
                if v != 0:
                    print(name, ai, bi, m, int(v))
