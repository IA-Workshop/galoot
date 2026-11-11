#!/usr/bin/env -S python3 -I
"""Find the constant-evaluation threshold of every unit of units/ by the
plan cost model's checked search (docs/agent/snippets/plan_cost_model/
harness.py): the smallest -fconstexpr-ops-limit (GCC) or -fconstexpr-steps
(Clang) under which the unit compiles, which follows its largest
evaluation.

Usage, from the repository root, with the job pool exported
(eval "$(tools/jobpool env)") and TIMED_LOG naming this directory's
runs.tsv:

  thresholds.py <out.tsv> [jobs]

Configurations, as the cost model measures them: GCC with its call cache
off (-fconstexpr-cache-depth=1, "cold"), GCC with its defaults ("warm"),
and Clang (no cache), each with the galoot-O0 flags of harness.py. Each
search compiles with -fsyntax-only and holds a slot of the pool while it
compiles.
"""

import concurrent.futures
import importlib.util
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[6]
SNIPPET = ROOT / "docs" / "agent" / "snippets" / "plan_cost_model"

spec = importlib.util.spec_from_file_location("harness",
                                              SNIPPET / "harness.py")
harness = importlib.util.module_from_spec(spec)
spec.loader.exec_module(harness)

CONFIGS = (("gcc", "cold", ["-fconstexpr-cache-depth=1"]),
           ("gcc", "warm", []),
           ("clang", "warm", []))
# The compilers' default limits, for "fits by default".
DEFAULT = {"gcc": 1 << 25, "clang": 1 << 20}


def main(argv):
    out = pathlib.Path(argv[0]).resolve()
    jobs = int(argv[1]) if len(argv) > 1 else 8
    pool = harness.Pool()
    units = sorted((HERE / "units").glob("*.cpp"))
    tasks = [(u, c, cache, extra) for u in units for c, cache, extra in CONFIGS]

    def one(task):
        unit, compiler, cache, extra = task
        name = f"spike-e-proto:{unit.stem}:{cache}"
        r = harness.threshold(pool, compiler, "galoot-O0", unit, name,
                              extra=extra)
        return unit, compiler, cache, r

    rows = []
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        for unit, compiler, cache, r in ex.map(one, tasks):
            case, kind = unit.stem.split(".")
            t = r["threshold"]
            fits = ("fails at every limit" if t == "fails" else
                    "yes" if t <= DEFAULT[compiler] else "no")
            rows.append([case, kind, compiler, cache, str(t), fits, r["kind"],
                         r["where"], str(r["compiles"])])
            print(*rows[-1], flush=True)
    with open(out, "w", encoding="utf-8") as f:
        f.write("\t".join(["case", "unit", "compiler", "cache", "threshold",
                           "fits_default", "failure_below", "where",
                           "compiles"]) + "\n")
        for row in sorted(rows):
            f.write("\t".join(row) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
