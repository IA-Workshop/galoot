"""Compare the reruns of the WU6a and WU6b mutation sets with their recorded
results, mutant by mutant: killed or survived.

For each set the recorded status of a mutant is its latest recorded one: the
files are read in the order listed, and a later file overrides an earlier.
Prints each set, preset, and mode with the number of mutants compared and
every mutant whose status differs or that is missing on one side, and exits
1 if any differs.

Usage, from the worktree root:
  python3 -I docs/agent/records/mutations/0012-product-cost-growth/compare_results.py <rerun-revision>
"""
import csv
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[5]
MUTATIONS = ROOT / "docs/agent/records/mutations"
HERE = MUTATIONS / "0012-product-cost-growth"

# (set, recorded files per preset and mode, with {p} the preset and {m} the
# mode suffix "" or "-runtime")
SETS = {
    "blade-product": ["0011-blade-product/results-b142837-{p}{m}.tsv",
                      "0011-blade-product/results-398463d-{p}-range.tsv|normal",
                      "0011-blade-product/results-65bd869-{p}-fixes{m}.tsv"],
    "gp-plan": ["0011-gp-plan/results-4e5ea90-{p}{m}.tsv"],
    "gp-plan-review": ["0011-gp-plan/results-4e5ea90-{p}{m}-review.tsv"],
}


def statuses(path):
    with open(path, encoding="utf-8") as f:
        lines = [line for line in f if not line.startswith("#")]
    rows = list(csv.DictReader(lines, delimiter="\t"))
    return {r["mutation"]: r["killed"] for r in rows}


def main(revision):
    differs = False
    for name, patterns in SETS.items():
        for preset in ("gcc-debug", "clang-debug"):
            for mode in ("", "-runtime"):
                recorded = {}
                for pattern in patterns:
                    pattern, _, only = pattern.partition("|")
                    if only == "normal" and mode:
                        continue
                    recorded.update(statuses(
                        MUTATIONS / pattern.format(p=preset, m=mode)))
                rerun = statuses(
                    HERE / f"rerun-{revision}-{name}-{preset}{mode}.tsv")
                changed = sorted(m for m in recorded.keys() | rerun.keys()
                                 if recorded.get(m) != rerun.get(m))
                print(f"{name}\t{preset}\t{mode or 'normal'}\t"
                      f"{len(rerun)} rerun, {len(recorded)} recorded\t"
                      f"{sum(s == 'SURVIVED' for s in rerun.values())} "
                      f"survived\t"
                      + (", ".join(f"{m}: {recorded.get(m)} -> {rerun.get(m)}"
                                   for m in changed) or "same"))
                differs = differs or bool(changed)
    sys.exit(1 if differs else 0)


if __name__ == "__main__":
    main(sys.argv[1])
