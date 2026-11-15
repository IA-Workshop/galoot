"""Distinguishing hand values for criterion 6 of plan 0030 (research note
0026, section A6): for each basis-dependent operation op, the value op(x)
in the source and the value phi^-1(op'(phi(x))) computed by way of the
target, which differ. Computed with conventions_check.py's Form (Chevalley's
product) and again with clifford_reference.py's products; the two must agree.

Run from the repository root, with any CPython 3:
  python3 -I docs/agent/records/spikes/wu12-part-a/hand_values.py test/reference
"""

import sys
from fractions import Fraction as F

sys.path.insert(0, sys.argv[1] if len(sys.argv) > 1 else "test/reference")
import clifford_reference as cr  # noqa: E402
import conventions_check as cc  # noqa: E402


def name(x, n):
    if not x:
        return "0"
    parts = []
    for m in sorted(x, key=lambda m: (bin(m).count("1"), cr.indices(m))):
        c = x[m]
        b = cr.blade_name(m)
        parts.append(f"{c} {b}" if b != "1" else f"{c}")
    return " + ".join(parts)


def case(label, g, gt, p, op, args):
    s, t = cc.Form(g), cc.Form(gt)
    images = cc.change_images(p)
    back = cc.change_images(cc.adjugate_inverse(p))
    direct = getattr(s, op)(*args)
    via = cc.apply_images(back, getattr(t, op)(*[cc.apply_images(images, a) for a in args]))
    # The same with clifford_reference's products, for the products only.
    if op in ("antigp", "antidot"):
        rs, rt = cr.Algebra(g), cr.Algebra(gt)
        assert s.gp(*[s.right(a) for a in args]) == cr.clean(rs.mul(*[s.right(a) for a in args]))
        mapped = [s.right(a) for a in args]
        assert t.gp(*[t.right(cc.apply_images(images, a)) for a in args]) == cr.clean(
            rt.mul(*[t.right(cc.apply_images(images, a)) for a in args]))
        del mapped
    n = len(g)
    print(f"{label}: {op}({', '.join(name(a, n) for a in args)}) = {name(direct, n)}; "
          f"by way of the target: {name(via, n)}; differ: {direct != via}")
    return direct != via


def main():
    p3 = cc.conformal_change(3)
    g3, d3 = cc.conformal_gram(3), cc.diagonal_gram([1, 1, 1, 1, -1])
    e4 = {1 << 3: F(1)}
    ok = True
    ok &= case("conformal(3) and R(4,1,0)", g3, d3, p3, "bulk", [e4])
    ok &= case("conformal(3) and R(4,1,0)", g3, d3, p3, "weight", [e4])
    ok &= case("conformal(3) and R(4,1,0)", g3, d3, p3, "right", [e4])
    ok &= case("conformal(3) and R(4,1,0)", g3, d3, p3, "left", [e4])
    g1 = [[2, 0, -3], [0, 1, -3], [-3, -3, -2]]
    d1 = cc.diagonal_gram([2, 1, -62])
    p1 = [[1, 0, 0], [0, 1, 0], [F(-3, 2), -3, F(1, 2)]]
    e3 = {1 << 2: F(1)}
    e12 = {3: F(1)}
    ok &= case("random 1 and diag(2, 1, -62)", g1, d1, p1, "antigp", [e12, e12])
    ok &= case("random 1 and diag(2, 1, -62)", g1, d1, p1, "antidot", [e12, e12])
    ok &= case("random 1 and diag(2, 1, -62)", g1, d1, p1, "weight_dual", [e3])
    print("every value differs" if ok else "FAILED: some value does not differ")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
