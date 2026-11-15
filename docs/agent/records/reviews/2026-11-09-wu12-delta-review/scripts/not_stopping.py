"""Why the mutant elimination_overflow_not_stopping accepts elim_overflow.cpp:
the replica of noting.py with the elimination's early exit removed."""
import sys
src = open(sys.argv[1]).read().replace("\nmain()\n", "\n")
src = src.replace("        if not note.fits:\n            return 0, None, False\n", "")
src = src.replace("    return prev, [a[i][N:] for i in range(N)], True",
                  "    print('  left block', [r[:N] for r in a], 'note.fits', note.fits)\n    return prev, [a[i][N:] for i in range(N)], True")
ns = {}
exec(compile(src, "noting-mutant", "exec"), ns)
m = 2**31 - 1
rows, dens = [[m, m], [-m, m]], [m, m]
p, inv, fits = ns["eliminate"](rows)
print("pivot", p, "scaled inverse", inv, "fits reported", fits)
print("inverts (fits, holds)", ns["inverts"](rows, p, inv))
print("stage", ns["stage"](rows, dens, [[2, 0], [0, 2]], [[1, 0], [0, 1]]))
