#!/usr/bin/env -S python3 -I
"""The cost model's predictions for the two plans of each case's transform:
the geometric antiproduct of the versor with the object, and of the middle
layout with the versor. Writes predictions.tsv beside it: per case, plan,
and compiler, the largest phase and its predicted threshold, and whether
the prediction lies inside the span of the fit.

    python3 -I predict.py
"""

import json
import pathlib
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[6]
COST = ROOT / "docs/agent/snippets/plan_cost_model/cost.py"
WEIGHTS = ROOT / "docs/agent/records/performance/plan-cost-model/out"
FORM = {"rigid3": "rigid", "conformal3": "conformal"}


def text(blades):
    return ",".join(f"{m}:{s}" for m, s in blades)


def main():
    cases = json.loads((HERE / "cases.json").read_text())
    rows = []
    for name, case in cases.items():
        plans = (("first", case["versor"], case["object"]),
                 ("second", case["middle"], case["versor"]))
        for plan, a, b in plans:
            for compiler in ("gcc", "clang"):
                r = subprocess.run(
                    [sys.executable, "-I", str(COST), "predict",
                     str(WEIGHTS / f"weights-{compiler}-galoot-O0.tsv"),
                     "antigp", FORM[case["algebra"]], str(case["n"]),
                     text(a), text(b)],
                    capture_output=True, text=True, check=True)
                lines = r.stdout.splitlines()
                phase, value, *rest = lines[1].split()
                rows.append([name, plan, compiler, phase, value,
                             "extrapolated" if rest else "inside",
                             lines[0]])
    with open(HERE / "predictions.tsv", "w", encoding="utf-8") as f:
        f.write("case\tplan\tcompiler\tlargest_phase\tpredicted\tspan\tsizes\n")
        for row in rows:
            f.write("\t".join(row) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
