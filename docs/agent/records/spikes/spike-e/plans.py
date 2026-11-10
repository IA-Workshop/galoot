"""The restricted composition of form (A), emulated from its description.

Stage 1 is the antiproduct V ⟇ x as a list of terms over the layouts' blades;
stage 2 is (the middle) ⟇ V~ restricted to the blades of x; stage 1 keeps
only the terms whose output stage 2 reads. A term is (a, b, out, c): the
coefficient at position a of the first operand times the coefficient at
position b of the second, onto the output blade `out`, with the factor c = +1
or -1 that folds the layouts' orientations and the antireverse's sign. Terms
come in order of a, then b, as the plan lists them. A run assigns an output's
first term (negated when c = -1) and adds or subtracts each later one: this is
the accumulation the library's kernels use, stated in COUNTING-RULE.md as
rule K.
"""

import rigid3 as R


def canonical_key(mask):
    return (R.pop(mask), tuple(R.indices(mask)))


def blade_antiproduct(a, b):
    """(sign, mask) of e_a ⟇ e_b."""
    x = R.antiproduct({a: 1}, {b: 1})
    if not x:
        return 0, 0
    (m, c), = x.items()
    return int(c), m


def product_terms(la, lb, extra=lambda b, out: 1):
    """All terms of the antiproduct of layouts la (first) and lb (second),
    each layout a list of (name, mask, orientation)."""
    terms = []
    for ai, (_, ma, sa) in enumerate(la):
        for bi, (_, mb, sb) in enumerate(lb):
            s, m = blade_antiproduct(ma, mb)
            if s:
                terms.append((ai, bi, m, s * sa * sb))
    return terms


def middle_layout(terms):
    masks = sorted({t[2] for t in terms}, key=canonical_key)
    return [(f"w{R.pop(m)}_{m}", m, 1) for m in masks]


def sandwich_terms(v, x):
    """The kernels of the transform of x (layout x) by the even versor v
    (layout v): returns (stage1_terms, stage2_terms, middle, kept_masks),
    each term list already restricted as the kernels restrict it, with
    outputs of stage 2 given as positions in x's layout and stage 1's as
    positions in the full middle layout."""
    first = product_terms(v, x)
    mid = middle_layout(first)
    mpos = {m: i for i, (_, m, _) in enumerate(mid)}
    # stage 1 terms with outputs as middle positions
    first = [(a, b, mpos[o], c) for a, b, o, c in first]
    # stage 2: middle (first operand) with v (second); the antireverse sign
    # of v's blade multiplies each term
    second_all = []
    for ai, (_, ma, sa) in enumerate(mid):
        for bi, (_, mb, sb) in enumerate(v):
            s, m = blade_antiproduct(ma, mb)
            if s:
                ag = R.N - R.pop(mb)
                ar = -1 if (ag * (ag - 1) // 2) % 2 else 1
                second_all.append((ai, bi, m, s * sa * sb * ar))
    xpos = {m: i for i, (_, m, _) in enumerate(x)}
    second = [(a, b, xpos[o], c * x[xpos[o]][2])
              for a, b, o, c in second_all if o in xpos]
    dropped = [t for t in second_all if t[2] not in xpos]
    read = {a for a, _, _, _ in second}
    first_kept = [t for t in first if t[2] in read]
    sandwich_terms.first_all = first
    return first_kept, second, mid, dropped


def run_kernel(terms, nout, x, y, neg, add, sub, zero):
    """The accumulation of the library's kernels on any number type: `neg`,
    `add`, `sub` are the operators; products are x[a] * y[b]."""
    out = [zero] * nout
    seen = [False] * nout
    for a, b, o, c in terms:
        v = x[a] * y[b]
        assert c in (1, -1)
        if not seen[o]:
            out[o] = v if c == 1 else neg(v)
            seen[o] = True
        else:
            out[o] = add(out[o], v) if c == 1 else sub(out[o], v)
    return out


def kernel_counts(terms, nout):
    """Counts of one kernel by its accumulation rule: multiplications, additions
    and subtractions, negations."""
    seen = set()
    mul = add = sub = neg = 0
    for a, b, o, c in terms:
        mul += 1
        if o not in seen:
            seen.add(o)
            neg += 1 if c == -1 else 0
        elif c == 1:
            add += 1
        else:
            sub += 1
    return mul, add, sub, neg
