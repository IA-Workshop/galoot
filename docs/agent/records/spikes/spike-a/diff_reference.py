"""Diff the Spike A generator, run at run time, against clifford_reference.py.

The two compute the table by different routes (Spike A: contraction recursion
in the stored basis with integers; reference: LDL^T over Fraction, single-term
rule, change back), so agreement on every blade pair is a kind-3 cross-check
(VVUQ V1) of both. Run from the repository root after building
out/runtime_check_<compiler>:

  python3 -I docs/agent/records/spikes/spike-a/diff_reference.py gcc clang
"""

import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[5]
here = Path(__file__).resolve().parent
sys.path.insert(0, str(root / "test/reference"))
import clifford_reference as ref  # noqa: E402

signatures = [("conformal 3", ref.conformal(3)), ("R 1 3 0", ref.r_pqr(1, 3, 0)),
              ("rigid 3", ref.rigid(3)),
              ("gram 0,1,0;1,0,0;0,0,0", [[0, 1, 0], [1, 0, 0], [0, 0, 0]])]
signatures += [(f"random {k}", g) for k, g in enumerate(ref.random_forms())]

ok = True
for compiler in sys.argv[1:]:
    exe = here / "out" / f"runtime_check_{compiler}"
    for spec, g in signatures:
        expected = subprocess.run(
            [sys.executable, "-I", str(root / "test/reference/clifford_reference.py"),
             "table", *spec.split()], capture_output=True, text=True,
            check=True).stdout
        expected = [ln for ln in expected.splitlines() if not ln.startswith("#")]
        flat = ";".join(",".join(str(x) for x in row) for row in g)
        got = subprocess.run([str(exe), str(len(g)), flat], capture_output=True,
                             text=True, check=True).stdout.splitlines()
        same = got == expected
        ok &= same
        print(f"{compiler} {spec}: {'agree' if same else 'DIFFER'} "
              f"({len(expected)} pairs)")
print("all tables agree" if ok else "TABLES DIFFER")
sys.exit(0 if ok else 1)
