"""Reduced rerun of Spike E's batch loop with drive.py's own pair method.

    python3 -I recheck.py <drive.py> <out-dir> <rounds>

Batch loop at 1,024 objects per motor, environment size 0, forms galoot, a,
b, c, operations transform_point and transform_line, both compilers, the
compilers and forms reversed on every other round, CPU 4. Run from the root
of the checkout, under the timing lock.
"""
import importlib.util
import sys
from pathlib import Path

spec = importlib.util.spec_from_file_location("drive", sys.argv[1])
drive = importlib.util.module_from_spec(spec)
spec.loader.exec_module(drive)

out = Path(sys.argv[2]).resolve()
out.mkdir(parents=True, exist_ok=True)
rounds = int(sys.argv[3])
f, writer = drive.open_rows(out)
with f:
    for rnd in range(1, rounds + 1):
        compilers = list(drive.COMPILERS)
        forms = ["galoot", "a", "b", "c"]
        if rnd % 2 == 0:
            compilers.reverse()
            forms.reverse()
        for compiler in compilers:
            for operation in ("transform_point", "transform_line"):
                for form in forms:
                    drive.pair(writer, f, out, 4, 0, rnd, compiler, operation,
                               "batch", drive.BATCH, form,
                               drive.PASSES[operation, "batch"], "check")
