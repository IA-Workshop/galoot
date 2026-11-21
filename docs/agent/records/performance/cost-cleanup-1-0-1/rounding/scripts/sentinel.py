# usage: sentinel.py <driver.cpp> <out.cpp>
# A copy of the split driver with one law call deleted, to show that the
# comparison of the per-seed output can fail; never committed.
import sys
s = open(sys.argv[1]).read()
line = '    left_complement_of_antiwedge<Alg>(a, b, da, db, form, out);\n'
assert s.count(line) == 1
open(sys.argv[2], 'w').write(s.replace(line, ''))
