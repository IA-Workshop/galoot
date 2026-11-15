"""Black-box comparison of galoot's change of basis with Sage's lift_isometry.

Usage (the math-tools Sage environment, passagemath-modules 10.8.13):
  <py-sage>/.venv/bin/python -I sage_cross_check.py library-dump.txt golden.hpp

library-dump.txt is the output of dump_changes.cpp, both routes; golden.hpp is
the unit's test/reference/golden.hpp, whose namespace basis_changes holds the
images of conformal(1) to conformal(4) that clifford_reference.py computed.

For each change P (row i writes e_i of the source in the target's e'_j, with
P G' P^T = G), Sage builds Cl(G) as CliffordAlgebra(QuadraticForm(QQ, 2G)),
whose relation is e_i e_j + e_j e_i = 2 G_ij, and lifts the isometry m = P^T:
lift_isometry(m) sends e_i to sum_j m[j, i] e'_j = sum_j P[i, j] e'_j, and its
codomain is Cl(m^-T G m^-1) = Cl(G'). The images of the wedge monomials are
read in wedge coordinates by Route A (Sage's basis is the ordered products)
and compared, entry by entry, with the library's images. The back change is
lift_isometry(m^-1) from Cl(G'), whose codomain must be Cl(G). Nothing here
computes a minor: Sage's map is its own.

The script prints one line per change and the totals, and exits 0 only when
every comparison agrees and every control fails as it must.
"""
import re
import sys
from fractions import Fraction
from itertools import combinations

from sage.all__sagemath_modules import QQ, CliffordAlgebra, QuadraticForm, matrix, vector


def parse_dump(path):
    changes = []
    current = None
    listed = {}
    with open(path) as f:
        for line in f:
            line = line.rstrip("\n")
            if line.startswith("listed "):
                name, verdict = line[len("listed "):].rsplit(" ", 1)
                listed[name] = verdict
            elif line.startswith("change "):
                current = {"name": line[len("change "):], "forward": {}, "back": {}}
            elif line == "end":
                current["listed"] = listed.get(current["name"])
                changes.append(current)
                current = None
            else:
                key, _, rest = line.partition(" ")
                if key in ("forward", "back"):
                    blade, _, terms = rest.partition(":")
                    items = terms.split()
                    current[key][int(blade)] = {
                        int(items[k]): Fraction(items[k + 1]) for k in range(0, len(items), 2)}
                elif key == "n":
                    current["n"] = int(rest)
                elif key == "route":
                    current["route"] = rest
                elif key.startswith("denominators_"):
                    current[key] = [int(x) for x in rest.split()]
                else:
                    current[key] = [[int(x) for x in row.split()] for row in rest.split(";")]
    return changes


def rational_matrix(rows, denominators):
    return matrix(QQ, [[QQ(x) / d for x in row] for row, d in zip(rows, denominators)])


def mask_of(indices):
    return sum(1 << i for i in indices)


def wedge_basis(Cl, n):
    """Route A: the wedge monomials X_S built in Sage, and a reader of wedge
    coordinates by mask."""
    e = Cl.gens()
    order = [S for k in range(n + 1) for S in combinations(range(n), k)]
    X = {(): Cl.one()}
    for S in order[1:]:
        i, rest = S[0], S[1:]
        X[S] = (e[i] * X[rest] + (-1) ** len(rest) * X[rest] * e[i]) / 2
    position = {S: r for r, S in enumerate(order)}

    def ordered_coords(x):
        out = [QQ(0)] * len(order)
        for key, c in x.monomial_coefficients().items():
            out[position[tuple(key)]] = QQ(c)
        return vector(QQ, out)

    T = matrix(QQ, [list(ordered_coords(X[S])) for S in order]).transpose()
    assert all(T[i, i] == 1 for i in range(len(order)))
    assert all(T[i, j] == 0 for i in range(len(order)) for j in range(i))

    def wedge_coords(x):
        c = T.solve_right(ordered_coords(x))
        return {mask_of(S): Fraction(int(c[r].numerator()), int(c[r].denominator()))
                for r, S in enumerate(order) if c[r] != 0}

    return order, X, wedge_coords


def images(phi, n, source, target):
    order, X, _ = wedge_basis(source, n)
    _, _, coords = wedge_basis(target, n)
    return {mask_of(A): coords(phi(X[A])) for A in order}


def mismatches(sage, library, n):
    bad = 0
    for a in range(1 << n):
        for b in range(1 << n):
            bad += sage[a].get(b, 0) != library.get(a, {}).get(b, 0)
    return bad


def gram(rows):
    return matrix(QQ, rows)


def clifford(G):
    return CliffordAlgebra(QuadraticForm(QQ, 2 * G))


def compare(change):
    n = change["n"]
    G = gram(change["source_gram"])
    Gt = gram(change["target_gram"])
    P = rational_matrix(change["rows"], change["denominators_rows"])
    P_inverse_library = rational_matrix(change["inverse"], change["denominators_inverse"])
    m = P.transpose()
    out = {"isometry": P * Gt * P.transpose() == G}

    Cl = clifford(G)
    out["form"] = Cl.quadratic_form().Gram_matrix() == G
    phi = Cl.lift_isometry(m, "a")
    out["codomain"] = phi.codomain().quadratic_form().Gram_matrix() == Gt
    forward = images(phi, n, Cl, phi.codomain())
    out["forward"] = mismatches(forward, change["forward"], n)

    out["inverse"] = sum(P.inverse()[i, j] != P_inverse_library[i, j]
                         for i in range(n) for j in range(n))
    Cl_t = clifford(Gt)
    psi = Cl_t.lift_isometry(m.inverse(), "b")
    out["back_codomain"] = psi.codomain().quadratic_form().Gram_matrix() == G
    back = images(psi, n, Cl_t, psi.codomain())
    out["back"] = mismatches(back, change["back"], n)
    out["det"] = P.det()
    out["entries"] = (1 << n) ** 2
    return out, forward, back


def conformal_P(d):
    n = d + 2
    P = matrix(QQ, n, n, 0)
    for i in range(d):
        P[i, i] = 1
    P[d, d], P[d, d + 1] = QQ(-1) / 2, QQ(1) / 2
    P[d + 1, d], P[d + 1, d + 1] = 1, 1
    return P


def witt_P(modes):
    n = 2 * modes
    P = matrix(QQ, n, n, 0)
    for j in range(modes):
        P[2 * j, j], P[2 * j, modes + j] = QQ(1) / 2, QQ(1) / 2
        P[2 * j + 1, j], P[2 * j + 1, modes + j] = 1, -1
    return P


# P written from the definitions, for the changes the library or the tests
# name: CONVENTIONS 1.4 for conformal(d), the scaled Witt change, and rigid(3)
# with e4 = 2 e'4.
INDEPENDENT_P = {f"conformal({d})": conformal_P(d) for d in (1, 2, 3, 4)}
INDEPENDENT_P["Witt, one mode"] = witt_P(1)
INDEPENDENT_P["Witt, two modes"] = witt_P(2)
INDEPENDENT_P["rigid(3), e4 doubled"] = matrix.diagonal(QQ, [1, 1, 1, 2])


def parse_golden(path):
    text = open(path).read()
    body = text[text.index("namespace basis_changes {"):text.index("} // namespace basis_changes")]
    tables = {}
    for match in re.finditer(r"change_image, \d+> (d\d)_(to|from)_diagonal = \{\{(.*?)\}\};", body, re.S):
        table = {}
        for entry in re.finditer(r"\{0b([01]+), (\d+), \{\{(.*?)\}\}\},", match.group(3)):
            terms = re.findall(r"\{\{(-?\d+), (\d+)\}, 0b([01]+)\}", entry.group(3))
            table[int(entry.group(1), 2)] = {
                int(b, 2): Fraction(int(p), int(q)) for p, q, b in terms[:int(entry.group(2))]}
        tables[(match.group(1), match.group(2))] = table
    return tables


def main():
    changes = parse_dump(sys.argv[1])
    golden = parse_golden(sys.argv[2])
    failures = 0
    totals = {"changes": 0, "entries": 0, "mismatches": 0}
    sage_conformal = {}
    for change in changes:
        out, forward, back = compare(change)
        n = change["n"]
        P = rational_matrix(change["rows"], change["denominators_rows"])
        independent = INDEPENDENT_P.get(change["name"]) if change["route"] == "public" else None
        p_matches = "n/a" if independent is None else str(independent == P)
        ok = (out["isometry"] and out["form"] and out["codomain"] and out["back_codomain"]
              and out["forward"] == 0 and out["back"] == 0 and out["inverse"] == 0
              and p_matches != "False" and change["listed"] in (None, "same"))
        failures += not ok
        totals["changes"] += 1
        totals["entries"] += 2 * out["entries"]
        totals["mismatches"] += out["forward"] + out["back"]
        if change["route"] == "public" and change["name"].startswith("conformal("):
            sage_conformal[int(change["name"][10])] = (forward, back)
        print(f"{change['route']:6} {change['name']}: n = {n}, det P = {out['det']}; "
              f"P G' P^T = G {out['isometry']}; Sage form is G {out['form']}; "
              f"codomain Gram is G' {out['codomain']}; back codomain is G {out['back_codomain']}; "
              f"P from the definition {p_matches}; listed {change['listed'] or 'n/a'}; "
              f"inverse entries differing {out['inverse']} of {n * n}; "
              f"forward mismatches {out['forward']} of {out['entries']}; "
              f"back mismatches {out['back']} of {out['entries']}"
              f"{'' if ok else '  <-- DISAGREES'}")

    # golden.hpp's basis_changes against Sage's maps of conformal(1) to (4).
    for d in (1, 2, 3, 4):
        forward, back = sage_conformal[d]
        n = d + 2
        bad_to = mismatches(forward, golden[(f"d{d}", "to")], n)
        bad_from = mismatches(back, golden[(f"d{d}", "from")], n)
        failures += bad_to + bad_from != 0
        totals["entries"] += 2 * (1 << n) ** 2
        totals["mismatches"] += bad_to + bad_from
        print(f"golden d{d}_to_diagonal against Sage: {bad_to} mismatches of {(1 << n) ** 2}; "
              f"d{d}_from_diagonal: {bad_from} of {(1 << n) ** 2}")

    # Controls: each must find a difference, or the comparison could not fail.
    by_name = {c["name"]: c for c in changes if c["route"] == "public"}
    c3 = by_name["conformal(3)"]
    G = gram(c3["source_gram"])
    halved = CliffordAlgebra(QuadraticForm(QQ, G)).quadratic_form().Gram_matrix() != G
    print(f"control, G passed for 2G (conformal(3)): Sage's form differs from G {halved}")
    failures += not halved

    transposed_found = 0
    for name in ("conformal(3)", "Witt, two modes", "random 1", "random 7"):
        c = by_name[name]
        P = rational_matrix(c["rows"], c["denominators_rows"])
        Cl = clifford(gram(c["source_gram"]))
        phi = Cl.lift_isometry(P, "a")
        wrong = (phi.codomain().quadratic_form().Gram_matrix() != gram(c["target_gram"])
                 or mismatches(images(phi, c["n"], Cl, phi.codomain()), c["forward"], c["n"]) != 0)
        transposed_found += wrong
    print(f"control, m = P in place of P^T: differences found in {transposed_found} of 4 changes")
    failures += transposed_found != 4

    Cl = clifford(G)
    phi = Cl.lift_isometry(rational_matrix(c3["rows"], c3["denominators_rows"]).transpose(), "a")
    forward = images(phi, 5, Cl, phi.codomain())
    perturbed = {a: dict(t) for a, t in c3["forward"].items()}
    perturbed[0b11000][0b11000] = -perturbed[0b11000][0b11000]
    one = mismatches(forward, perturbed, 5)
    print(f"control, the sign of e45's image flipped in the dump: {one} mismatch")
    failures += one != 1

    print(f"total: {totals['changes']} changes, {totals['entries']} image entries compared, "
          f"{totals['mismatches']} mismatches; {failures} failed lines")
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
