"""Part A item (f) of plan 0030: the predicted constant-evaluation cost of the
kernel identities of criteria 4 and 5 for each group, from galoot's plan cost
model, and the largest integer draw K for which the test rational provably
does not overflow on each group.

Cost. Each identity is traced as the plans and maps it instantiates, in the
source algebra and in the target, as research note 0024 (B2) did for the
seeded R(p, q, r): the product plans with their result sets from the model's
mirror of the generator, and the unary maps. A phase with events outside the
fit is given a margin (each unseen event at the largest weight of its
function's events). The change of basis itself is new code that the model
does not cover; its size is reported as the number of nonzero minors on the
layout and the most terms of one output blade.

Traced per algebra, on a of A and b of B: the geometric product and the wedge
(criterion 4); the dot as the product of a and reverse(b), the antiwedge,
the bulk dual as right(G a) with G a the product with the antiscalar and a
left complement, and the three sign maps (criterion 5); and, for the groups
whose change has an isometric contragredient, the geometric antiproduct, the
weight dual, the antidot as the product of right(a) and reverse(right(b)),
and, for conformal(1) to conformal(3), the transform of a vector by an even
versor.

Overflow. The test rational holds a fraction of two int64 in lowest terms and
stops when an intermediate does not fit. Every value below is bounded by a
magnitude m per blade and a common multiple L of its denominators, both
propagated through each operation as the library computes it:
  - a product sum_t c_t x_A y_B (c_t the integer table entry): each term's
    numerator is at most |c| m_x L_x m_y L_y, and every partial sum's
    numerator at most (sum_t |c| m_x m_y) L_x L_y, with denominators dividing
    L_x L_y;
  - a change of basis (sum_A M_AB x_A) / den_B, with den_B the lcm of the
    products of the row denominators over the rows of each minor (the most a
    generated denominator can be before reduction) and M_AB = det P[A, B]
    den_B: numerators at most (sum_A |M_AB| m_A) L_x, denominators L_x den_B.
Inputs are integers in [-K, K] (m = K, L = 1), so each bound is B(1) K^deg,
deg the degree of the value in the inputs; the largest safe K is the floor of
((2^63 - 1) / B(1))^(1/deg) over every operation of the group, and every L
must stay below 2^63 too. The computations bounded are those of the cost
part, on both sides of each identity, the round trip back(phi(x)), and the
factor of criterion 5 applied to one side.

The LDL^T changes are the reference's (test/reference/clifford_reference.py,
diagonalize) with each square a/b scaled to the squarefree part of a b, as
plan 0030's R5 asks. A test-side LDL^T that pivots differently gives other
changes, and this bound must then be recomputed with them.

Run from the repository root, with any CPython 3:
  python3 -I docs/agent/records/spikes/wu12-part-a/cost_and_overflow.py .
"""

import importlib.util
import itertools
import math
import pathlib
import sys
from fractions import Fraction as F

ROOT = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
SNIPPET = ROOT / "docs/agent/snippets/plan_cost_model"
WEIGHTS = ROOT / "docs/agent/records/performance/plan-cost-model/out"
sys.path.insert(0, str(ROOT / "test/reference"))
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import clifford_reference as cr  # noqa: E402
import preservation_check as pc  # noqa: E402

CLANG_STEPS = 1 << 20
CLANG_RAISED = 1_200_000
GCC_OPS = 1 << 33
GCC_LOOP = 1 << 18
INT64 = (1 << 63) - 1


def load(name):
    spec = importlib.util.spec_from_file_location(name, SNIPPET / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


cost = load("cost")
mirror = cost.mirror


def popcount(m):
    return bin(m).count("1")


def layout_full(n):
    return tuple(m for m, _ in mirror.full(n))


def layout_grades(n, *ks):
    return tuple(m for m in layout_full(n) if popcount(m) in ks)


def even_versor(n):
    return tuple(m for m in layout_full(n) if (n - popcount(m)) % 2 == 0)


# --- Cost ---------------------------------------------------------------------

class Tracer:
    def __init__(self, form):
        self.form = form
        self.n = len(form)
        self.plans, self.maps = {}, {}

    def plan(self, op, a, b):
        key = (op, a, b)
        if key not in self.plans:
            _, result, _, _ = mirror.phases(op, self.form, [(m, 1) for m in a],
                                            [(m, 1) for m in b])
            self.plans[key] = tuple(result)
        return self.plans[key]

    def map(self, op, a):
        key = (op, a)
        if key not in self.maps:
            _, result, _, _ = mirror.map_phases(op, self.n, [(m, 1) for m in a])
            self.maps[key] = tuple(result)
        return self.maps[key]

    def gp(self, a, b): return self.plan("gp", a, b)
    def wedge(self, a, b): return self.plan("wedge", a, b)
    def antigp(self, a, b): return self.plan("antigp", a, b)
    def antiwedge(self, a, b): return self.plan("antiwedge", a, b)
    def right(self, a): return self.map("right_complement", a)
    def left(self, a): return self.map("left_complement", a)
    def rev(self, a): return self.map("reverse", a)
    def antirev(self, a): return self.map("antireverse", a)
    def hat(self, a): return self.map("grade_involution", a)

    def bulk(self, a):
        return self.left(self.gp(a, ((1 << self.n) - 1,)))


def trace(t, a, b, contragredient, transform):
    t.gp(a, b)
    t.wedge(a, b)
    t.gp(a, t.rev(b))
    t.antiwedge(a, b)
    t.right(t.bulk(a))
    t.rev(a), t.antirev(a), t.hat(a)
    if contragredient:
        t.antigp(a, b)
        t.bulk(t.right(a))
        t.gp(t.right(a), t.rev(t.right(b)))
    if transform:
        v = even_versor(t.n)
        x = layout_grades(t.n, 1)
        t.antigp(t.antigp(v, x), t.antirev(v))


def unseen_margin(w, ev):
    extra = 0
    for e, count in ev.items():
        if count and e not in w:
            prefix = e.split(".")[0] + "."
            extra += count * max([v for k, v in w.items() if k.startswith(prefix)] or [0])
    return extra


def worst_phase(t, weights):
    w, rows, names = weights
    worst, longest, outside = (0, ""), 0, 0
    for (op, a, b) in t.plans:
        plan, _, _, ev = mirror.phases(op, t.form, [(m, 1) for m in a], [(m, 1) for m in b])
        longest = max(longest, len(plan), len(a), len(b))
        for p in mirror.PHASES:
            if p in w:
                value, covered = cost.predict_phase(w, rows, names, p, ev[p])
                if not covered:
                    outside += 1
                    value += unseen_margin(w[p], ev[p])
                if value > worst[0]:
                    worst = (value, f"{op} {len(a)}x{len(b)} {p}")
    for (op, a) in t.maps:
        terms, _, _, ev = mirror.map_phases(op, t.n, [(m, 1) for m in a])
        longest = max(longest, len(terms), len(a))
        for p in mirror.MAP_PHASES:
            if p in w:
                value, covered = cost.predict_phase(w, rows, names, p, ev[p])
                if not covered:
                    outside += 1
                    value += unseen_margin(w[p], ev[p])
                if value > worst[0]:
                    worst = (value, f"{op} {len(a)} {p}")
    return worst, longest, outside


# --- Overflow -------------------------------------------------------------------

class Bound:
    """|value at blade| <= m[blade], every denominator divides L, degree deg."""

    def __init__(self, m, L, deg):
        self.m, self.L, self.deg = m, L, deg


class Prover:
    def __init__(self, g):
        self.g = [[F(v) for v in row] for row in g]
        self.n = len(g)
        self.top = (1 << self.n) - 1
        self.alg = cr.Algebra(self.g)
        self.exo = cr.change_of_basis(self.g, self.n)
        self.cache = {}
        self.worst = []  # (B(1), deg, what)

    def table(self, a, b):
        if (a, b) not in self.cache:
            self.cache[(a, b)] = cr.clean(self.alg.mul({a: F(1)}, {b: F(1)}))
        return self.cache[(a, b)]

    def note(self, b1, deg, what):
        self.worst.append((b1, deg, what))

    def bilinear(self, x, y, terms, what):
        out, sums = {}, {}
        top_term = 0
        for a, ma in x.m.items():
            for b, mb in y.m.items():
                for c, coef in terms(a, b).items():
                    t = abs(coef) * ma * mb
                    out[c] = out.get(c, 0) + t
                    top_term = max(top_term, t)
        L = x.L * y.L
        deg = x.deg + y.deg
        peak = max(out.values(), default=0)
        self.note(peak * L, deg, what)
        self.note(top_term * L, deg, what + " (one term)")
        return Bound(out, L, deg)

    def gp(self, x, y):
        return self.bilinear(x, y, self.table, "geometric product")

    def wedge(self, x, y):
        def w(a, b):
            return {} if a & b else {a | b: cr.reorder_sign(a, b)}
        return self.bilinear(x, y, w, "wedge")

    def comp(self, x):
        return Bound({self.top ^ a: v for a, v in x.m.items()}, x.L, x.deg)

    def antigp(self, x, y):
        return self.comp(self.gp(self.comp(x), self.comp(y)))

    def antiwedge(self, x, y):
        return self.comp(self.wedge(self.comp(x), self.comp(y)))

    def bulk(self, x):
        out = {}
        for a, ma in x.m.items():
            for b, c in self.exo[a].items():
                out[b] = out.get(b, 0) + abs(c) * ma
        peak = max(out.values(), default=0)
        self.note(peak * x.L, x.deg, "metric exomorphism")
        return Bound(out, x.L, x.deg)

    def dot(self, x, y):
        gy = self.bulk(y)
        total = sum(ma * gy.m.get(a, 0) for a, ma in x.m.items())
        L = x.L * gy.L
        self.note(total * L, x.deg + y.deg, "dot")
        return Bound({0: total}, L, x.deg + y.deg)

    def scale(self, x, f):
        f = F(f)
        L = x.L * f.denominator
        out = {a: v * abs(f) for a, v in x.m.items()}
        self.note(max(out.values(), default=0) * L, x.deg, "factor")
        return Bound(out, L, x.deg)


REDUCED = [False]


def change_bound(prover, x, p):
    """phi(x) for the change with rows p, with den_B from the row
    denominators before any reduction, or, when REDUCED[0] is set, the lcm
    of the reduced denominators of the minors of column B."""
    n = len(p)
    rowden = [math.lcm(*[F(v).denominator for v in row]) for row in p]
    images = cr.change_of_basis([[F(v) for v in row] for row in p], n)
    columns = {}
    for a, terms in images.items():
        for b, c in terms.items():
            columns.setdefault(b, []).append((a, c))
    out, Lnew = {}, x.L
    for b, entries in columns.items():
        if REDUCED[0]:
            den = math.lcm(*[F(c).denominator for _, c in entries])
        else:
            den = math.lcm(*[math.prod(rowden[i] for i in range(n) if a >> i & 1)
                             for a, _ in entries])
        s = sum(abs(c) * den * x.m.get(a, 0) for a, c in entries)
        if s:
            prover.note(s * x.L, x.deg, "change of basis numerator")
            out[b] = F(s, den)
            Lnew = math.lcm(Lnew, x.L * den)
    return Bound(out, Lnew, x.deg)


def overflow_group(g, gt, p, a_layout, b_layout, contragredient, transform):
    s, t = Prover(g), Prover(gt)
    pinv = cr.inverse([[F(v) for v in row] for row in p])
    d = cr.det([[F(v) for v in row] for row in p])
    x = Bound({m: F(1) for m in a_layout}, 1, 1)
    y = Bound({m: F(1) for m in b_layout}, 1, 1)
    px, py = change_bound(t, x, p), change_bound(t, y, p)
    for side, X, Y in ((s, x, y), (t, px, py)):
        z = side.gp(X, Y)
        side.wedge(X, Y)
        side.dot(X, Y)
        side.scale(side.antiwedge(X, Y), 1 / d)
        side.scale(side.comp(side.bulk(X)), d)
        if side is s:
            change_bound(t, z, p)
            change_bound(t, side.wedge(X, Y), p)
            change_bound(t, side.antiwedge(X, Y), p)
            change_bound(t, side.comp(side.bulk(X)), p)
        if contragredient:
            side.scale(side.antigp(X, Y), 1 / d)
            side.scale(side.bulk(side.comp(X)), 1 / d)
            side.dot(side.comp(X), side.comp(Y))
            if side is s:
                change_bound(t, side.antigp(X, Y), p)
                change_bound(t, side.bulk(side.comp(X)), p)
    change_bound(s, px, pinv)
    if transform:
        n = len(g)
        v = Bound({m: F(1) for m in even_versor(n)}, 1, 1)
        xv = Bound({m: F(1) for m in layout_grades(n, 1)}, 1, 1)
        inner = s.antigp(v, xv)
        out = s.antigp(inner, v)
        change_bound(t, out, p)
        pv, pxv = change_bound(t, v, p), change_bound(t, xv, p)
        t.scale(t.antigp(t.antigp(pv, pxv), pv), 1 / (d * d))
    worst = None
    for prover in (s, t):
        for b1, deg, what in prover.worst:
            if b1 == 0:
                continue
            k = int((F(INT64) / b1) ** (1.0 / deg) + 1e-9)
            while k > 0 and b1 * k ** deg > INT64:
                k -= 1
            while b1 * (k + 1) ** deg <= INT64:
                k += 1
            if worst is None or k < worst[0]:
                worst = (k, what, deg)
    dens = max(px.L, py.L)
    return worst, dens


# --- Groups ---------------------------------------------------------------------

def conformal_rows(d):
    h = F(1, 2)
    n = d + 2
    p = [[F(int(i == j)) for j in range(n)] for i in range(d)]
    p.append([F(0)] * d + [-h, h])
    p.append([F(0)] * d + [F(1), F(1)])
    return p


def groups():
    h = F(1, 2)
    out = []
    for d in (1, 2, 3, 4):
        n = d + 2
        src = mirror.form("conformal", n)
        dst = mirror.Form(mirror.diagonal([1] * (d + 1) + [-1]),
                          [[i == j for j in range(n)] for i in range(n)])
        if n <= 5:
            layouts = [("full x full", layout_full(n), layout_full(n))]
        else:
            layouts = [("vectors x bivectors", layout_grades(n, 1), layout_grades(n, 2)),
                       ("bivectors x bivectors", layout_grades(n, 2), layout_grades(n, 2)),
                       ("full x full (not proposed)", layout_full(n), layout_full(n))]
        out.append((f"conformal({d}) and R({d + 1},1,0)", src, dst, cr.conformal(d),
                    cr.r_pqr(d + 1, 1, 0), conformal_rows(d), layouts, True, d <= 3))
    written = lambda g: mirror.Form(g)  # from_gram writes every entry
    witt1 = [[0, 1], [1, 0]]
    witt2 = [[0, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 0, 1, 0]]
    out.append(("Witt, one mode, and R(1,1,0)", written(witt1),
                mirror.Form(mirror.diagonal([1, -1]), [[True, False], [False, True]]),
                witt1, cr.r_pqr(1, 1, 0), [[h, h], [1, -1]],
                [("full x full", layout_full(2), layout_full(2))], True, False))
    out.append(("Witt, two modes, and R(2,2,0)", written(witt2),
                mirror.Form(mirror.diagonal([1, 1, -1, -1]),
                            [[i == j for j in range(4)] for i in range(4)]),
                witt2, cr.r_pqr(2, 2, 0),
                [[h, 0, h, 0], [1, 0, -1, 0], [0, h, 0, h], [0, 1, 0, -1]],
                [("full x full", layout_full(4), layout_full(4))], True, False))
    deg = [[0, 1, 0], [1, 0, 0], [0, 0, 0]]
    out.append(("the degenerate form and R(1,1,1)", written(deg),
                mirror.Form(mirror.diagonal([1, -1, 0]),
                            [[i == j and i < 2 for j in range(3)] for i in range(3)]),
                deg, cr.r_pqr(1, 1, 1), [[h, h, 0], [1, -1, 0], [0, 0, 1]],
                [("full x full", layout_full(3), layout_full(3))], True, False))
    nf = [[0, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]
    out.append(("rigid(3) with its null generator first, and rigid(3)", written(nf),
                mirror.form("rigid", 4), nf, cr.rigid(3),
                [[0, 0, 0, 1], [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0]],
                [("full x full", layout_full(4), layout_full(4))], True, False))
    for k, g in enumerate(cr.random_forms()):
        n = len(g)
        p, diagonal = pc.ldlt_change(g)
        if n <= 4:
            layouts = [("full x full", layout_full(n), layout_full(n))]
        else:
            layouts = [("vectors x vectors", layout_grades(n, 1), layout_grades(n, 1)),
                       ("vectors x bivectors", layout_grades(n, 1), layout_grades(n, 2)),
                       ("bivectors x bivectors", layout_grades(n, 2), layout_grades(n, 2)),
                       ("full x full (not proposed)", layout_full(n), layout_full(n))]
        squares = [diagonal[i][i] for i in range(n)]
        out.append((f"random {k} and diag{tuple(squares)}", written(g), written(diagonal),
                    g, diagonal, p, layouts, False, False))
    return out


def change_size(p, layout):
    n = len(p)
    images = cr.change_of_basis([[F(v) for v in row] for row in p], n)
    terms = sum(len(images[a]) for a in layout)
    per_output = {}
    for a in layout:
        for b in images[a]:
            per_output[b] = per_output.get(b, 0) + 1
    return terms, max(per_output.values())


def main():
    weights = {c: cost.read_weights(WEIGHTS / f"weights-{c}-galoot-O0.tsv")
               for c in ("clang", "gcc")}
    print("group\tlayouts\tchange terms (most per output blade)\t"
          "clang largest phase (steps)\tgcc largest phase (ops)\tlongest loop\t"
          "phases outside the fit\tlargest safe K\tits binding operation\t"
          "largest denominator of phi(x)\tlargest safe K with reduced output "
          "denominators")
    for name, src, dst, g, gt, p, layouts, contragredient, transform in groups():
        for label, a, b in layouts:
            row = [name, label]
            terms, most = change_size(p, a)
            row.append(f"{terms} ({most})")
            results = {}
            for c in ("clang", "gcc"):
                worst, longest, outside = (0, ""), 0, 0
                for form in (src, dst):
                    t = Tracer(form)
                    trace(t, a, b, contragredient, transform and "not" not in label)
                    (v, where), lg, out = worst_phase(t, weights[c])
                    if v > worst[0]:
                        worst = (v, where)
                    longest, outside = max(longest, lg), outside + out
                results[c] = (worst, longest, outside)
            cv, cw = results["clang"][0]
            gv, gw = results["gcc"][0]
            mark = ("under" if cv <= CLANG_STEPS else
                    "OVER the default; under 1,200,000" if cv <= CLANG_RAISED else "OVER")
            row.append(f"{float(cv):.0f} ({cw}; {mark})")
            row.append(f"{float(gv):.0f} ({gw}; {'under' if gv <= GCC_OPS else 'OVER'})")
            longest = max(results["clang"][1], results["gcc"][1])
            row.append(f"{longest}{' (GCC loop limit)' if longest >= GCC_LOOP else ''}")
            row.append(str(results["clang"][2]))
            if "not proposed" in label:
                row += ["-", "-", "-", "-"]
            else:
                (k, what, deg), dens = overflow_group(g, gt, p, a, b, contragredient,
                                                       transform)
                row += [str(k), f"{what}, degree {deg}", str(dens)]
                REDUCED[0] = True
                (k2, _, _), _ = overflow_group(g, gt, p, a, b, contragredient, transform)
                REDUCED[0] = False
                row.append(str(k2))
            print("\t".join(row), flush=True)


if __name__ == "__main__":
    main()
