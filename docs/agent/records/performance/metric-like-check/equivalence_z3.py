#!/usr/bin/env -S python3 -I
"""Proves, for every n from 1 to 32, that the one-pass check accepts exactly
the matrices that the three conditions of the current concept accept.

Usage (z3-solver as a Python package):
  equivalence_z3.py

Each entry is a 32-bit two's-complement integer, as entry_type (int) is on
the compilers in use, and comparisons are signed. The two predicates are
transcribed from their loops:

  current(G)  = for all i, j < i: G[i][j] == G[j][i]          (is_symmetric)
                and for all i, j: -M <= G[i][j] <= M          (entries_in_range)
  one_pass(G) = for all i, j <= i: G[i][j] == G[j][i]
                                   and -M <= G[i][j] <= M

with M = max_form_entry = 32767, which the script recomputes as the largest k
with 2 k^2 <= 2^31 - 1. For each n the solver is asked for a matrix on which
the two differ; "unsat" means there is none, which is a proof for that n.
"""

import sys

import z3


def max_form_entry():
    int_max = 2**31 - 1
    k = 0
    while 2 * (k + 1) ** 2 <= int_max:
        k += 1
    return k


def predicates(n, m):
    g = [[z3.BitVec(f"g_{i}_{j}", 32) for j in range(n)] for i in range(n)]
    lo, hi = z3.BitVecVal(-m, 32), z3.BitVecVal(m, 32)

    def in_range(x):
        return z3.And(lo <= x, x <= hi)  # signed comparison on BitVec

    current = z3.And(
        z3.And([g[i][j] == g[j][i] for i in range(n) for j in range(i)]
               or [z3.BoolVal(True)]),
        z3.And([in_range(g[i][j]) for i in range(n) for j in range(n)]))
    one_pass = z3.And([z3.And(g[i][j] == g[j][i], in_range(g[i][j]))
                       for i in range(n) for j in range(i + 1)])
    return current, one_pass


def main():
    m = max_form_entry()
    assert m == 32767, m
    print(f"z3 {z3.get_version_string()}, max_form_entry = {m}")
    failures = 0
    for n in range(1, 33):
        current, one_pass = predicates(n, m)
        s = z3.Solver()
        s.add(current != one_pass)
        verdict = s.check()
        print(f"n = {n:2d}: a matrix where they differ: {verdict}")
        failures += verdict != z3.unsat
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
