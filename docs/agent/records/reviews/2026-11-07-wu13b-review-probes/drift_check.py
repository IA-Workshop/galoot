import json, sys, math
from fractions import Fraction as Fr
sys.path.insert(0, sys.argv[2])
import importlib.util
spec = importlib.util.spec_from_file_location("simlib", sys.argv[2] + "/simlib.py"); sim = importlib.util.module_from_spec(spec); spec.loader.exec_module(sim)
kd = json.loads(open(sys.argv[1]).read())
for c in kd['cases']:
    v = [float.fromhex(t) for t in c['v']]; x = [float.fromhex(t) for t in c['x']]
    d, e = sim.evaluate(kd, v, x); tol = sim.tolerance(kd, v, x)
    j = max(range(kd['nd']), key=lambda j: abs(d[j]) / tol[j] if tol[j] else 0)
    print("j=%d |d|/tol=%.2f |e|/tol=%.2f |d-e|/tol=%.3f" % (j, abs(d[j]) / tol[j], float(abs(e[j]) / Fr(tol[j])), float(abs(Fr(d[j]) - e[j]) / Fr(tol[j]))))
