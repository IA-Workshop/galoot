#!/usr/bin/env -S python3 -I
"""Count the loop iterations of the two antiproduct routes for a fixed use
embedded in R(n-1,0,1): the four typed rigid(3) pairs of the growth record
(point x point, line x plane, motor x point, point x motor), e4 read as e_n.

The counts follow the loops of the library's generator, transcribed in
check_routes.py: add_blade_product calls, the term and bit loops of
add_generator_product and of the contraction of the rest, term_list::add
calls and their canonical-order scan, and the reorder_sign steps of the
complement signs. The reduced route also counts its per-plan work: the
closure of the used generators (one row of the form per generator reached)
and the squares of the spectator components (one row per spectator, since a
diagonal generator is its own component).

Usage: op_counts.py [out-file]
"""

import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))

import check_routes as cr  # noqa: E402
import clifford_reference as ref  # noqa: E402

RIGID = {
    "point": "e1 e2 e3 e4",
    "line": "e41 e42 e43 e23 e31 e12",
    "plane": "e423 e431 e412 e321",
    "motor": "1 e41 e42 e43 e23 e31 e12 e1234",
}
PAIRS = [("point", "point"), ("line", "plane"), ("motor", "point"),
         ("point", "motor")]


def mask_of(name, n):
    if name == "1":
        return 0
    return sum(1 << ((n if ch == "4" else int(ch)) - 1) for ch in name[1:])


def blades(set_name, n):
    return [mask_of(x, n) for x in RIGID[set_name].split()]


def count(n):
    g = ref.r_pqr(n - 1, 0, 1)
    comps = cr.components(g)
    full_counter, reduced_counter = cr.Counter(), cr.Counter()
    per_plan = 0
    terms_full = terms_reduced = 0
    for first, second in PAIRS:
        xs, ys = blades(first, n), blades(second, n)
        used = 0
        for m in xs + ys:
            used |= m
        c = cr.closure(comps, used)
        # Per plan: one row of n entries for each generator of the closure,
        # and one per spectator generator for its component and square.
        per_plan += n * cr.popcount(c) + n * (n - cr.popcount(c))
        for a in xs:
            for b in ys:
                f = cr.full_conjugation(g, n, cr.right_sign_loop, a, b,
                                        full_counter)
                r = cr.reduced_antiproduct(g, comps, n, a, b, c,
                                           reduced_counter)
                assert f == r
                terms_full += len(f)
                terms_reduced += len(r)
                # The reduced route's signs are reorder_sign calls whose loops
                # run over the bits of a, of E, and of E again (tau(A, C\A),
                # tau(E, D), tau(E, Q)); count them as the C++ form runs them.
                reduced_counter.sign_bits += cr.popcount(a)
                for e in r:
                    reduced_counter.sign_bits += 2 * cr.popcount(e & c)
    return (full_counter, reduced_counter, per_plan, terms_full,
            terms_reduced)


def main(argv):
    lines = ["n\troute\tcalls\tterm_visits\tcontraction_bits\tadds\t"
             "scan_steps\tsign_bits\ttotal\tper_plan\tmax_capacity\tterms"]
    for n in range(4, 17):
        f, r, per_plan, tf, tr = count(n)
        for name, k, extra, terms in (("conjugation", f, 0, tf),
                                      ("reduced", r, per_plan, tr)):
            lines.append(f"{n}\t{name}\t{k.calls}\t{k.term_visits}\t"
                         f"{k.contraction_bits}\t{k.adds}\t{k.scan_steps}\t"
                         f"{k.sign_bits}\t{k.total()}\t{extra}\t"
                         f"{k.max_capacity}\t{terms}")
    text = "\n".join(lines) + "\n"
    print(text, end="")
    if argv:
        pathlib.Path(argv[0]).write_text(text, encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
