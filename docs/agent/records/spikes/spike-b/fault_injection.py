"""Show that each check in clifford_reference.validate can fail (VVUQ V2).

Each fault changes one thing and must make validate() report the check it
targets. Run from the repository root:

  python3 -I docs/agent/workbench/spike-b/fault_injection.py
"""

import sys
from fractions import Fraction
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[4] / "test/reference"))
import clifford_reference as ref  # noqa: E402

G = ref.conformal(3)


def run(label, alg, table, expect):
    failures = ref.validate(alg, table)
    hit = any(f.startswith(expect) for f in failures)
    print(f"{'caught' if hit else 'MISSED'}: {label} -> "
          f"{len(failures)} failures, first: {failures[:1]}")
    return hit


def main():
    ok = True

    # 1. A wrong sign on one vector pair breaks the defining relations.
    alg = ref.Algebra(G)
    t = alg.table()
    t[(1 << 3, 1 << 4)] = ref.clean({0: Fraction(1), 0b11000: Fraction(1)})
    ok &= run("e4 e5 scalar term +1 instead of -1", alg, t, "anticommutator")

    # 2. A wrong coefficient on a bivector-bivector pair breaks associativity
    #    while leaving every vector pair, and so check 1, unchanged.
    alg = ref.Algebra(G)
    t = alg.table()
    t[(0b00011, 0b00110)] = {m: 2 * c for m, c in t[(0b00011, 0b00110)].items()}
    ok &= run("e12 e23 doubled", alg, t, "associativity")

    # 3. Storing Clifford-product monomials instead of wedge monomials
    #    (f basis read as the stored basis) keeps checks 1 and 2 for the
    #    diagonal form but must fail the wedge-monomial check.
    class ProductBasis(ref.Algebra):
        """Blade A taken as the product e_a1 e_a2 ... in the stored basis."""

        def table(self):
            size = 1 << self.n
            out = {}
            for a in range(size):
                for b in range(size):
                    x = {0: Fraction(1)}
                    for i in ref.indices(a) + ref.indices(b):
                        x = self.mul(x, {1 << i: Fraction(1)})
                    out[(a, b)] = self.product_to_monomial(x)
            return out

        def product_to_monomial(self, x):
            # Re-express wedge blades as product monomials: e_A(product) =
            # e_A(wedge) + lower terms. Only the 4,5 pair differs in
            # conformal(3): e4 e5 = e45 - 1, so e45(wedge) = p45 + 1 * p_rest.
            out = {}
            for m, c in x.items():
                out[m] = out.get(m, 0) + c
                if m & 0b11000 == 0b11000:
                    rest = m & ~0b11000
                    sign = ref.reorder_sign(rest, 0b11000)
                    out[rest] = out.get(rest, 0) + sign * c
            return ref.clean(out)

    alg = ProductBasis(G)
    ok &= run("product monomials as stored basis", alg, alg.table(),
              "wedge monomial")

    print("all faults caught" if ok else "SOME FAULT MISSED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
