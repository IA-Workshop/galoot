"""A line-for-line Python copy of the product-plan generator of
include/galoot/core/plan.hpp and blade_product.hpp, which counts how often
each path through each loop and branch is taken.

The count of a path is the number the compilers' counters multiply: a
compiler spends a fixed number of steps or operations on one pass along one
path of straight-line code, so the cost of an evaluation is a weighted sum of
these counts with weights that depend on the compiler and its options but not
on the input. cost.py fits and checks those weights.

Each function returns what its C++ counterpart returns and adds to `ev`, a
collections.Counter, one count per event. Event names are
"<function>.<path>". Each phase of make_product_plan is one constant
evaluation, so each gets its own counter (phases()).

Generators are bits 0 to n-1 of a mask, as in the headers.
"""

import collections


def popcount(x):
    return bin(x).count("1")


def ctz(x):
    return (x & -x).bit_length() - 1


def grade(a):
    return popcount(a)


# canonical_less(a, b): by grade, then lexicographically by index list.
def canonical_less(a, b, ev, site):
    if grade(a) != grade(b):
        ev[f"{site}.cl_grade"] += 1
        return grade(a) < grade(b)
    if a == b:
        ev[f"{site}.cl_equal"] += 1
        return False
    ev[f"{site}.cl_index"] += 1
    first = ctz(a ^ b)
    return (a >> first) & 1 == 1


def canonical_key(a):
    """A sort key equal to canonical order, for checking only."""
    return (grade(a), [i for i in range(32) if a >> i & 1])


def reorder_sign(a, b, ev, site):
    swaps = 0
    while a != 0:
        ev[f"{site}.rs_bit"] += 1
        i = ctz(a)
        swaps += popcount(b & ((1 << i) - 1))
        a &= a - 1
    return 1 if swaps % 2 == 0 else -1


MAX = (1 << 63) - 1


def add_checked(x, y, ev):
    ev["add_checked.pos" if y > 0 else "add_checked.nonpos"] += 1
    assert (x <= MAX - y) if y > 0 else (x >= -MAX - y)
    return x + y


def multiply_checked(x, y, ev):
    ev["mul_checked.call"] += 1
    if x < 0:
        ev["mul_checked.neg_x"] += 1
    if y < 0:
        ev["mul_checked.neg_y"] += 1
    if x == 0:
        ev["mul_checked.zero_x"] += 1
    assert abs(x * y) <= MAX
    return x * y


class TermList:
    """term_list<Capacity>: terms in canonical order of their blades."""

    def __init__(self, capacity, ev):
        ev["term_list.init"] += 1
        # Places that a zero-initialization writes, which GCC may count.
        ev["term_list.slots"] += capacity
        self.capacity = capacity
        self.terms = []

    def add(self, blade, coefficient, ev):
        ev["add.call"] += 1
        if coefficient == 0:
            ev["add.zero"] += 1
            return
        at = 0
        while at < len(self.terms) and canonical_less(
                self.terms[at][0], blade, ev, "add"):
            ev["add.scan"] += 1
            at += 1
        if at < len(self.terms) and self.terms[at][0] == blade:
            ev["add.merge"] += 1
            total = add_checked(self.terms[at][1], coefficient, ev)
            self.terms[at] = (blade, total)
            if total == 0:
                ev["add.cancel"] += 1
                ev["add.cancel_shift"] += len(self.terms) - 1 - at
                del self.terms[at]
            return
        if at < len(self.terms):
            ev["add.insert_mid"] += 1
        else:
            ev["add.insert_end"] += 1
        assert len(self.terms) < self.capacity
        ev["add.insert_shift"] += len(self.terms) - at
        self.terms.insert(at, (blade, coefficient))


def product_capacity(a, b):
    g = grade(a | b)
    return 1 if g == 0 else 1 << (g - 1)


class Form:
    """A Gram matrix and which of its entries the metric's factory wrote.

    GCC holds a constant aggregate with the elements that were written, the
    rest being implied zeros, and reading an implied element costs it more
    than reading a written one; so the cost of a product depends on how the
    metric was built, not only on its entries. R, rigid, and conformal write
    only their nonzero entries; from_gram writes every entry."""

    def __init__(self, values, written=None):
        self.values = values
        n = len(values)
        self.written = written or [[True] * n for _ in range(n)]

    def __len__(self):
        return len(self.values)


def entry(form, i, j, ev, site):
    """gram[i][j], counting a read of an implied zero, or of a row with no
    written entry at all, as its own event."""
    if not any(form.written[i]):
        ev[f"{site}.entry_row_implied"] += 1
    elif not form.written[i][j]:
        ev[f"{site}.entry_implied"] += 1
    return form.values[i][j]


def add_generator_product(out, gram, i, x, scale, ev):
    ev["agp.call"] += 1
    generator = 1 << i
    for blade, coefficient in list(x.terms):
        ev["agp.term"] += 1
        c = multiply_checked(coefficient, scale, ev)
        if blade & generator == 0:
            ev["agp.wedge"] += 1
            out.add(blade | generator,
                    multiply_checked(reorder_sign(generator, blade, ev, "agp"),
                                     c, ev), ev)
        sign = 1
        remaining = blade
        while remaining != 0:
            ev["agp.bit"] += 1
            j = ctz(remaining)
            value = entry(gram, i, j, ev, "agp")
            if value != 0:
                ev["agp.entry"] += 1
                out.add(blade & ~(1 << j),
                        multiply_checked(multiply_checked(sign, value, ev), c,
                                         ev), ev)
            sign = -sign
            remaining &= remaining - 1


def add_blade_product(out, gram, a, b, scale, ev):
    if a == 0:
        ev["abp.base"] += 1
        out.add(b, scale, ev)
        return
    ev["abp.step"] += 1
    low = ctz(a)
    rest = a & (a - 1)
    rest_product = TermList(out.capacity, ev)
    add_blade_product(rest_product, gram, rest, b, 1, ev)
    add_generator_product(out, gram, low, rest_product, scale, ev)
    sign = -1
    remaining = rest
    while remaining != 0:
        ev["abp.bit"] += 1
        j = ctz(remaining)
        value = entry(gram, low, j, ev, "abp")
        if value != 0:
            ev["abp.entry"] += 1
            add_blade_product(out, gram, rest & ~(1 << j), b,
                              multiply_checked(multiply_checked(sign, value,
                                                                ev),
                                               scale, ev), ev)
        sign = -sign
        remaining &= remaining - 1


def blade_product(gram, a, b, capacity, ev):
    ev["bp.call"] += 1
    out = TermList(capacity, ev)
    add_blade_product(out, gram, a, b, 1, ev)
    return out


def pair_terms(op, gram, a, b, capacity, ev):
    if op == "gp":
        return blade_product(gram, a, b, capacity, ev)
    ev["wedge.call"] += 1
    out = TermList(capacity, ev)
    if a & b == 0:
        ev["wedge.disjoint"] += 1
        out.add(a | b, reorder_sign(a, b, ev, "wedge"), ev)
    return out


def plan_capacity(op, as_, bs, ev):
    capacity = 1
    for x, _ in as_:
        ev["capacity.a"] += 1
        for y, _ in bs:
            ev["capacity.pair"] += 1
            if op == "gp":
                ev["capacity.product" if x | y else "capacity.scalar"] += 1
                pair = product_capacity(x, y)
            else:
                pair = 1
            if pair > capacity:
                ev["capacity.raise"] += 1
                capacity = pair
    return capacity


def generate_plan(op, gram, as_, bs, capacity, write, ev):
    """The terms (a, b, out, coefficient); `write` says whether the call
    writes them (the fill) or only counts them."""
    ev["gen.call"] += 1
    plan = []
    for a, (ma, sa) in enumerate(as_):
        ev["gen.a"] += 1
        for b, (mb, sb) in enumerate(bs):
            ev["gen.pair"] += 1
            # The pair's term list is copied into the loop's local.
            ev["gen.pair_slots"] += capacity
            terms = pair_terms(op, gram, ma, mb, capacity, ev)
            sign = sa * sb
            for blade, coefficient in terms.terms:
                ev["gen.term"] += 1
                if write:
                    ev["gen.write"] += 1
                plan.append((a, b, blade, sign * coefficient))
    return plan


def collect_result(plan, ev):
    out = []
    for _, _, mask, _ in plan:
        ev["collect.term"] += 1
        at = 0
        while at < len(out) and canonical_less(out[at], mask, ev, "collect"):
            ev["collect.scan"] += 1
            at += 1
        if at < len(out) and out[at] == mask:
            ev["collect.repeat"] += 1
            continue
        ev["collect.new_mid" if at < len(out) else "collect.new_end"] += 1
        ev["collect.shift"] += len(out) - at
        out.insert(at, mask)
    return out


def type_terms(plan, result, ev):
    position = {}
    typed = []
    for a, b, mask, coefficient in plan:
        ev["type.term"] += 1
        at = 0
        while at < len(result) and result[at] != mask:
            ev["type.scan"] += 1
            at += 1
        position[mask] = at
        typed.append((a, b, at, coefficient))
    return typed


def typed_copy(result, ev):
    ev["typed.blade"] += len(result)


def in_order(before, after, less, ev, site):
    """The ordering test of the two checks: by a, then b, then `less`."""
    if before[0] < after[0]:
        ev[f"{site}.next_a"] += 1
        return True
    if before[0] == after[0] and before[1] < after[1]:
        ev[f"{site}.next_b"] += 1
        return True
    ev[f"{site}.same_pair"] += 1
    return before[0] == after[0] and before[1] == after[1] and less()


def defects_of(plan, n, ev):
    for k, t in enumerate(plan):
        ev["defects.term"] += 1
        if k > 0:
            before = plan[k - 1]
            in_order(before, t,
                     lambda: canonical_less(before[2], t[2], ev, "defects"),
                     ev, "defects")


def typed_defects_of(result, typed, ev):
    for r in range(len(result)):
        ev["typing.blade"] += 1
        if r > 0:
            canonical_less(result[r - 1], result[r], ev, "typing")
    for k, t in enumerate(typed):
        ev["typing.term"] += 1
        if k > 0:
            before = typed[k - 1]
            in_order(before, t, lambda: before[2] < t[2], ev, "typing")


PHASES = ("capacity", "count", "fill", "defects", "collect", "typed",
          "typing")


def phases(op, gram, as_, bs):
    """The plan, its result set, and one event counter per phase of
    make_product_plan, in the order the generator evaluates them. `gram` is
    a Form, or a matrix taken as written in full."""
    if not hasattr(gram, "written"):
        gram = Form(gram)
    ev = {p: collections.Counter() for p in PHASES}
    capacity = plan_capacity(op, as_, bs, ev["capacity"])
    plan = generate_plan(op, gram, as_, bs, capacity, False, ev["count"])
    plan2 = generate_plan(op, gram, as_, bs, capacity, True, ev["fill"])
    assert plan == plan2
    ev["fill"]["fill.array"] += len(plan)
    defects_of(plan, len(gram), ev["defects"])
    result = collect_result(plan, ev["collect"])
    typed_copy(result, ev["typed"])
    typed = type_terms(plan, result, ev["typed"])
    typed_defects_of(result, typed, ev["typing"])
    return plan, result, typed, ev


# Metrics, as Gram matrices with gram[i][j] for the generators of bits i, j.

def diagonal(signs):
    n = len(signs)
    return [[signs[i] if i == j else 0 for j in range(n)] for i in range(n)]


def form(name, n):
    """The named form, with the entries its factory writes: R(n,0,0) and
    R(n-1,0,1) write the diagonal of their nonnull generators, conformal(d)
    the d ones and the two entries of the null pair, and the dense form,
    built by from_gram, every entry."""
    if name == "r":
        g = diagonal([1] * n)
    elif name == "rigid":
        g = diagonal([1] * (n - 1) + [0])
    elif name == "conformal":
        g = diagonal([1] * (n - 2) + [0, 0])
        g[n - 2][n - 1] = g[n - 1][n - 2] = -1
    elif name == "dense":
        return Form([[2 if i == j else 1 for j in range(n)]
                     for i in range(n)])
    else:
        raise ValueError(name)
    return Form(g, [[v != 0 for v in row] for row in g])


def full(n):
    """full<n>: every blade, with sign +1, in canonical order."""
    masks = sorted(range(1 << n), key=canonical_key)
    return [(m, 1) for m in masks]
