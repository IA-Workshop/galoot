#!/usr/bin/env -S python3 -I
"""Compare routes to the geometric antiproduct of two basis blades, term for
term, against the definition a ⟇ b = left(right(a) ⟑ right(b)) computed with
the repository's exact reference scripts.

Routes compared with the oracle:

  oracle    the definition: the right complements from golden.complements,
            their product from clifford_reference.Algebra (which multiplies in
            a diagonalizing basis over fractions), each output term replaced
            by its left complement from golden.complements.
  full      conjugation over all n generators with this script's own product
            (the Chevalley recursion the library uses) and its own sign
            functions: the construction the plan generator implements.
  reduced   conjugation inside the closure C of the inputs' generators under
            the form's coupling graph (i ~ j when g_ij != 0, i != j); the
            generators Q outside C enter only through one scalar, the square
            of their blade, and per-blade signs (the reduction theorem of the
            research note).
  reduced+  the same with C enlarged by one more component, to show that any
            union of components holding the inputs works.
  closed    for a diagonal form only: tau(complement B, complement A) times the
            product of g_jj over the generators in neither input, on the blade
            of the complement of A xor B.
  naive     the geometric product of the ORIGINAL masks with the antimetric on
            vectors (the cofactor matrix) in place of the metric, output blade
            complemented: a reading of "the same rule with the antimetric" that
            the note shows is wrong. For each form the script reports whether
            the supports agree and whether per-blade signs (one per first
            factor, one per second factor, one per output blade) could repair
            the signs, by solving the GF(2) system.

Usage, from the repository root:
  check_routes.py check [out-file]     every pair of blades of every form
  check_routes.py growth [out-file]    the term counts that are inherent
"""

import os
import pathlib
import sys
import time
from fractions import Fraction

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[4]
sys.path.insert(0, str(ROOT / "test" / "reference"))

import clifford_reference as ref  # noqa: E402
import golden  # noqa: E402


# --- Masks and signs, written here without the reference scripts' code -----

def popcount(x):
    return bin(x).count("1")


def beta(x, y):
    """Parity of the number of pairs (i in x, j in y) with i > j, by a
    word-parallel prefix parity: bit k of p is the parity of the bits of x
    strictly above k, so the answer is the parity of popcount(p & y). The
    cost is a fixed number of word operations, whatever the masks."""
    p = x >> 1
    shift = 1
    while shift < 64:
        p ^= p >> shift
        shift <<= 1
    return popcount(p & y) & 1


def tau(x, y):
    return -1 if beta(x, y) else 1


def canonical_key(mask):
    return (popcount(mask), [i for i in range(64) if mask >> i & 1])


def canonical_less(a, b):
    return canonical_key(a) < canonical_key(b)


# --- The product: the Chevalley recursion, with loop counters ---------------

class Counter:
    """Loop iterations of the generator, named after the loops of the
    library's blade_product and term_list::add."""

    def __init__(self):
        self.calls = 0           # add_blade_product calls
        self.term_visits = 0     # outer loop of add_generator_product
        self.contraction_bits = 0  # bit loops over a blade or the rest
        self.adds = 0            # term_list::add calls
        self.scan_steps = 0      # canonical_less steps inside add
        self.sign_bits = 0       # reorder_sign loop steps
        self.max_capacity = 0    # largest product_capacity requested

    def total(self):
        return (self.calls + self.term_visits + self.contraction_bits +
                self.adds + self.scan_steps + self.sign_bits)


NO_COUNT = None


def reorder_sign_loop(a, b, counter):
    """The library's reorder_sign: one step per generator of a."""
    swaps = 0
    while a:
        if counter:
            counter.sign_bits += 1
        i = (a & -a).bit_length() - 1
        swaps += popcount(b & ((1 << i) - 1))
        a &= a - 1
    return -1 if swaps % 2 else 1


class TermList:
    def __init__(self, counter):
        self.terms = []  # [mask, coefficient] in canonical order
        self.counter = counter

    def add(self, blade, c):
        if c == 0:
            return
        k = self.counter
        if k:
            k.adds += 1
        at = 0
        while at < len(self.terms) and canonical_less(self.terms[at][0], blade):
            at += 1
            if k:
                k.scan_steps += 1
        if at < len(self.terms) and self.terms[at][0] == blade:
            self.terms[at][1] += c
            if self.terms[at][1] == 0:
                del self.terms[at]
            return
        self.terms.insert(at, [blade, c])


def add_generator_product(out, g, i, x, scale, counter):
    gen = 1 << i
    for blade, coef in x.terms:
        if counter:
            counter.term_visits += 1
        c = coef * scale
        if blade & gen == 0:
            out.add(blade | gen, reorder_sign_loop(gen, blade, counter) * c)
        sign = 1
        rem = blade
        while rem:
            if counter:
                counter.contraction_bits += 1
            j = (rem & -rem).bit_length() - 1
            if g[i][j] != 0:
                out.add(blade & ~(1 << j), sign * g[i][j] * c)
            sign = -sign
            rem &= rem - 1


def add_blade_product(out, g, a, b, scale, counter):
    if counter:
        counter.calls += 1
    if a == 0:
        out.add(b, scale)
        return
    low = (a & -a).bit_length() - 1
    rest = a & (a - 1)
    rest_product = TermList(counter)
    add_blade_product(rest_product, g, rest, b, 1, counter)
    add_generator_product(out, g, low, rest_product, scale, counter)
    sign = -1
    rem = rest
    while rem:
        if counter:
            counter.contraction_bits += 1
        j = (rem & -rem).bit_length() - 1
        if g[low][j] != 0:
            add_blade_product(out, g, rest & ~(1 << j), b,
                              sign * g[low][j] * scale, counter)
        sign = -sign
        rem &= rem - 1


def gp(g, a, b, counter=NO_COUNT):
    """e_A e_B in the wedge basis, as a list of (mask, coefficient) in
    canonical order."""
    if counter:
        counter.max_capacity = max(counter.max_capacity,
                                   1 << max(popcount(a | b) - 1, 0))
    out = TermList(counter)
    add_blade_product(out, g, a, b, 1, counter)
    return [(m, c) for m, c in out.terms]


# --- Routes ----------------------------------------------------------------

def full_mask(n):
    return (1 << n) - 1


def oracle_antiproduct(alg, right, left, a, b):
    ra, sa = right[a]
    rb, sb = right[b]
    product = alg.mul({ra: Fraction(sa)}, {rb: Fraction(sb)})
    out = {}
    for d, c in product.items():
        lm, ls = left[d]
        out[lm] = out.get(lm, 0) + c * ls
    return {m: c for m, c in out.items() if c != 0}


def components(g):
    """The connected components of the coupling graph, as masks."""
    n = len(g)
    seen, comps = 0, []
    for start in range(n):
        if seen >> start & 1:
            continue
        comp, frontier = 1 << start, [start]
        while frontier:
            i = frontier.pop()
            for j in range(n):
                if j != i and g[i][j] != 0 and not comp >> j & 1:
                    comp |= 1 << j
                    frontier.append(j)
        seen |= comp
        comps.append(comp)
    return comps


def closure(comps, mask):
    c = 0
    for k in comps:
        if k & mask:
            c |= k
    return c


def blade_square(g, k):
    """e_K e_K, a scalar (the square of a blade)."""
    terms = gp(g, k, k)
    assert all(m == 0 for m, _ in terms), "the square of a blade is a scalar"
    return terms[0][1] if terms else 0


def spectator_square(g, comps, q):
    """gamma_Q = e_Q e_Q for Q a union of components, from the squares of the
    components: (-1)^(|Q|(|Q|-1)/2) times the product of the component
    determinants, each det_K = (-1)^(|K|(|K|-1)/2) e_K e_K."""
    det = 1
    for k in comps:
        if k & q:
            assert k & q == k, "Q must be a union of components"
            s = popcount(k)
            det *= (-1) ** (s * (s - 1) // 2) * blade_square(g, k)
    s = popcount(q)
    return (-1) ** (s * (s - 1) // 2) * det


def reduced_antiproduct(g, comps, n, a, b, c_mask, counter=NO_COUNT):
    """e_A ⟇ e_B by conjugation inside C, a union of components holding A
    and B, with the generators Q outside C entering as one scalar and signs.

    e_A ⟇ e_B = gamma_Q (-1)^(|Q|(|C| + |A|)) tau(C, Q)
                * sum over the terms a_E e_E of the antiproduct of the
                  subalgebra on C of a_E tau(E, Q) e_(E ∪ Q)
    """
    q = full_mask(n) & ~c_mask
    assert (a | b) & q == 0
    gamma = spectator_square(g, comps, q)
    if gamma == 0:
        return {}
    ca, cb = c_mask & ~a, c_mask & ~b
    # Right complements inside C, with the |Q||A| sign folded into the first.
    sa = tau(a, ca) * (-1) ** (popcount(q) * popcount(a))
    sb = tau(b, cb)
    constant = gamma * (-1) ** (popcount(q) * popcount(c_mask)) * tau(c_mask, q)
    out = {}
    for d, coef in gp(g, ca, cb, counter):
        e = c_mask & ~d
        # left complement inside C: tau(C \ D, D); embedding: tau(E, Q).
        out[e | q] = coef * sa * sb * constant * tau(e, d) * tau(e, q)
    return out


def full_conjugation(g, n, right_sign, a, b, counter=NO_COUNT):
    """WU7b's construction with this script's own pieces: right complements
    of both inputs over all n generators, their product, the left
    complement of each output term."""
    full = full_mask(n)
    ca, cb = full & ~a, full & ~b
    sa, sb = right_sign(a, ca, counter), right_sign(b, cb, counter)
    out = {}
    for d, coef in gp(g, ca, cb, counter):
        e = full & ~d
        out[e] = coef * sa * sb * reorder_sign_loop(e, d, counter)
    return out


def closed_diagonal(g, n, a, b):
    full = full_mask(n)
    ca, cb = full & ~a, full & ~b
    factor = 1
    for j in range(n):
        if (ca & cb) >> j & 1:
            factor *= g[j][j]
    if factor == 0:
        return {}
    return {full & ~(a ^ b): tau(cb, ca) * factor}


def antimetric_on_vectors(g):
    """e_i ∘ e_j = right(e_i) • right(e_j) 𝟙: the signed cofactor of g."""
    n = len(g)
    full = full_mask(n)
    out = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            ri = [r for r in range(n) if r != i]
            rj = [r for r in range(n) if r != j]
            minor = ref.det([[g[r][s] for s in rj] for r in ri])
            si = tau(1 << i, full & ~(1 << i))
            sj = tau(1 << j, full & ~(1 << j))
            out[i][j] = int(minor) * si * sj
    return out


def naive_antimetric(gbar, n, a, b):
    full = full_mask(n)
    return {full & ~m: c for m, c in gp(gbar, a, b)}


def gf2_solvable(equations, unknowns):
    """Whether the GF(2) system of (bitmask of unknowns, right side) has a
    solution, by elimination on Python integers."""
    pivots = {}  # leading bit -> (row, rhs)
    for row, rhs in equations:
        while row:
            lead = row.bit_length() - 1
            if lead not in pivots:
                pivots[lead] = (row, rhs)
                break
            prow, prhs = pivots[lead]
            row ^= prow
            rhs ^= prhs
        else:
            if rhs:
                return False
    return True


# --- Signature set -----------------------------------------------------------

def diag(values):
    n = len(values)
    return [[values[i] if i == j else 0 for j in range(n)] for i in range(n)]


def block(*blocks):
    n = sum(len(b) for b in blocks)
    g = [[0] * n for _ in range(n)]
    at = 0
    for b in blocks:
        for i, row in enumerate(b):
            for j, x in enumerate(row):
                g[at + i][at + j] = x
        at += len(b)
    return g


def signature_set():
    forms = []
    for n in range(1, 7):
        for p, q, r, _ in golden.diagonal_signatures(n):
            forms.append((f"R({p},{q},{r})", ref.r_pqr(p, q, r)))
    forms += [
        ("diag(0,1,1) null first", diag([0, 1, 1])),
        ("diag(0,1,1,1) null first", diag([0, 1, 1, 1])),
        ("diag(2,-3,0,5)", diag([2, -3, 0, 5])),
        ("diag(3,1,-2,1,0)", diag([3, 1, -2, 1, 0])),
    ]
    for d in range(1, 5):
        forms.append((f"conformal({d})", ref.conformal(d)))
    witt = [[0, 1], [1, 0]]
    forms += [
        ("Witt, one mode", witt),
        ("Witt, two modes", block(witt, witt)),
        ("Witt, two modes + e+ + e0", block(witt, witt, [[1]], [[0]])),
        ("degenerate [[0,1,0],[1,0,0],[0,0,0]]",
         [[0, 1, 0], [1, 0, 0], [0, 0, 0]]),
        ("radical off the generators [[1,1,0],[1,0,1],[0,1,-1]]",
         [[1, 1, 0], [1, 0, 1], [0, 1, -1]]),
        ("conformal(2) + null + negative",
         block(ref.conformal(2), [[0]], [[-1]])),
        ("e+ + conformal(1) + e0 + e0", block([[1]], ref.conformal(1),
                                              [[0]], [[0]])),
    ]
    for n in range(2, 7):
        forms.append((f"dense n={n} (2 diagonal, 1 elsewhere)",
                      [[2 if i == j else 1 for j in range(n)]
                       for i in range(n)]))
    for k, g in enumerate(ref.random_forms()):
        forms.append((f"random[{k}] seed {ref.SEED}", g))
    return forms


# --- The check ---------------------------------------------------------------

def right_sign_loop(a, ca, counter):
    return reorder_sign_loop(a, ca, counter)


def check_form(name, g):
    n = len(g)
    alg = ref.Algebra(g)
    right, left = golden.complements(n)
    comps = components(g)
    is_diagonal = all(g[i][j] == 0 for i in range(n) for j in range(n)
                      if i != j)
    gbar = antimetric_on_vectors(g)
    full = full_mask(n)
    stats = {"pairs": 0, "terms": 0, "full": 0, "reduced": 0, "reduced+": 0,
             "closed": 0, "order": 0, "naive_support": 0, "naive_sign": 0}
    equations = []
    first_diff = {}
    size = 1 << n
    for a in range(size):
        for b in range(size):
            stats["pairs"] += 1
            o = oracle_antiproduct(alg, right, left, a, b)
            stats["terms"] += len(o)
            f = full_conjugation(g, n, right_sign_loop, a, b)
            if f != o:
                stats["full"] += 1
                first_diff.setdefault("full", (a, b, o, f))
            c = closure(comps, a | b)
            r = reduced_antiproduct(g, comps, n, a, b, c)
            if r != o:
                stats["reduced"] += 1
                first_diff.setdefault("reduced", (a, b, o, r))
            extra = next((k for k in comps if not k & c), 0)
            r2 = reduced_antiproduct(g, comps, n, a, b, c | extra)
            if r2 != o:
                stats["reduced+"] += 1
                first_diff.setdefault("reduced+", (a, b, o, r2))
            # The run order: the outputs of one pair, read from the last term
            # of the sub-product and embedded, must be in canonical order.
            q = full & ~c
            sub =[full & ~d & c for d, _ in gp(g, c & ~a, c & ~b)]
            embedded = [e | q for e in reversed(sub)]
            if any(not canonical_less(x, y)
                   for x, y in zip(embedded, embedded[1:])):
                stats["order"] += 1
            if is_diagonal:
                d = closed_diagonal(g, n, a, b)
                if d != o:
                    stats["closed"] += 1
                    first_diff.setdefault("closed", (a, b, o, d))
            nv = naive_antimetric(gbar, n, a, b)
            if set(nv) != set(o) or any(abs(nv[m]) != abs(o[m]) for m in o):
                stats["naive_support"] += 1
            else:
                for m in o:
                    row = (1 << a) | (1 << (size + b)) | (1 << (2 * size + m))
                    equations.append((row, int(nv[m] != o[m])))
                    if nv[m] != o[m]:
                        stats["naive_sign"] += 1
    stats["naive_repairable"] = (stats["naive_support"] == 0 and
                                 gf2_solvable(equations, 3 * size))
    stats["components"] = [popcount(k) for k in comps]
    return stats, first_diff


def run_check(out):
    lines = []
    total = {"pairs": 0, "terms": 0}
    bad = 0
    start = time.time()
    for name, g in signature_set():
        stats, diffs = check_form(name, g)
        total["pairs"] += stats["pairs"]
        total["terms"] += stats["terms"]
        failures = (stats["full"] + stats["reduced"] + stats["reduced+"] +
                    stats["closed"] + stats["order"])
        bad += failures
        naive = ("naive: supports agree, signs " +
                 ("repairable by per-blade signs" if stats["naive_repairable"]
                  else "NOT repairable by per-blade signs")
                 if stats["naive_support"] == 0 else
                 f"naive: support or magnitude differs on "
                 f"{stats['naive_support']} pairs")
        line = (f"{name}: n={len(g)} components={stats['components']} "
                f"pairs={stats['pairs']} terms={stats['terms']} "
                f"differ: full={stats['full']} reduced={stats['reduced']} "
                f"reduced+={stats['reduced+']} closed={stats['closed']} "
                f"order={stats['order']}; {naive}")
        lines.append(line)
        print(line, flush=True)
        for route, (a, b, o, x) in diffs.items():
            detail = f"  first {route} difference: A={a:b} B={b:b} " \
                     f"oracle={o} route={x}"
            lines.append(detail)
            print(detail, flush=True)
    summary = (f"forms={len(signature_set())} pairs={total['pairs']} "
               f"oracle terms={total['terms']} route differences={bad} "
               f"seconds={time.time() - start:.1f}")
    lines.append(summary)
    print(summary)
    if out:
        pathlib.Path(out).write_text("\n".join(lines) + "\n", encoding="utf-8")
    return 0 if bad == 0 else 1


# --- The inherent part: term counts and dependence on spectators ------------

def run_growth(out):
    lines = []
    # 1. In a connected (dense) form the result of two vectors grows with n.
    # The output blades of e1 ⟇ e2 have grade n or n - 2, so 1 + C(n, 2) is
    # an upper bound on its terms. The form 2 on the diagonal and 1 elsewhere
    # is special (a rank-one change of the identity); the second family has
    # entries g_ij = 1 + ((3 i + 5 j + 7 i j) mod 5) symmetrized, with no
    # structure chosen.
    def generic(n):
        g = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(i, n):
                g[i][j] = g[j][i] = 1 + (3 * i + 5 * j + 7 * i * j) % 5
        return g

    for n in range(2, 8):
        for label, g in (("dense", [[2 if i == j else 1 for j in range(n)]
                                    for i in range(n)]),
                         ("generic", generic(n))):
            r = reduced_antiproduct(g, components(g), n, 1, 2, full_mask(n))
            lines.append(f"{label} n={n}: e1 ⟇ e2 has {len(r)} terms "
                         f"(bound 1 + C(n,2) = {1 + n * (n - 1) // 2}); "
                         f"e1 ⟑ e2 has {len(gp(g, 1, 2))}")
    # 2. In R(n-1,0,1) the fixed pairs keep their term counts; in R(n,0,0)
    # the same pairs are not zero: the result depends on generators that
    # neither input names.
    for n in (4, 8, 12, 16):
        en = 1 << (n - 1)
        g1 = ref.r_pqr(n - 1, 0, 1)
        g0 = ref.r_pqr(n, 0, 0)
        c1, c0 = components(g1), components(g0)
        e14 = 1 | en
        x1 = reduced_antiproduct(g1, c1, n, e14, e14, closure(c1, e14))
        e1 = 1
        y1 = reduced_antiproduct(g1, c1, n, e1, e1, closure(c1, e1))
        y0 = reduced_antiproduct(g0, c0, n, e1, e1, closure(c0, e1))
        lines.append(f"n={n}: in R(n-1,0,1) e1n ⟇ e1n = {x1}, e1 ⟇ e1 = {y1};"
                     f" in R(n,0,0) e1 ⟇ e1 = {y0}")
    text = "\n".join(lines) + "\n"
    print(text, end="")
    if out:
        pathlib.Path(out).write_text(text, encoding="utf-8")
    return 0


def check_closed_sign(max_n=8):
    """tau(complement A, complement B) * r(A) r(B) l(D) = tau(complement B,
    complement A) for every pair of masks, n = 1 to max_n (the sign algebra
    of the closed diagonal form, without the metric factor)."""
    for n in range(1, max_n + 1):
        full = full_mask(n)
        for a in range(1 << n):
            for b in range(1 << n):
                ca, cb = full & ~a, full & ~b
                d = ca ^ cb
                lhs = (tau(a, ca) * tau(b, cb) * tau(ca, cb) *
                       tau(full & ~d, d))
                if lhs != tau(cb, ca):
                    return f"closed sign fails at n={n} A={a:b} B={b:b}"
    return f"closed sign identity holds for every pair of masks, n = 1 to {max_n}"


def main(argv):
    if not argv or argv[0] not in ("check", "growth", "sign"):
        sys.exit(__doc__)
    out = argv[1] if len(argv) > 1 else None
    if argv[0] == "check":
        return run_check(out)
    if argv[0] == "growth":
        return run_growth(out)
    text = check_closed_sign()
    print(text)
    if out:
        pathlib.Path(out).write_text(text + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
