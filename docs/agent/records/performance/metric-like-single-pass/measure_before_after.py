#!/usr/bin/env -S python3 -I
"""The pinned P2 measurement of metric_like, before and after a change.

Usage:
  measure_before_after.py <root> <before-rev> <after-rev> <out-dir> <cpu> [repeats]

Writes the include/ trees of the two revisions (git archive) to
<out-dir>/trees/before/include and <out-dir>/trees/after/include, and runs
the measurement script ../metric-like-check/measure.py on them, unchanged
except that its trees are these two instead of its variants: the concept
units at n = 4, 8, 12, 16, 32 and the four typed rigid(3) plans at
n = 4 to 16, each with and without static_assert(metric_like<alg>), on GCC
and Clang, pinned to <cpu>. rows.tsv and summary.txt go to <out-dir>.
"""

import importlib.util
import io
import pathlib
import subprocess
import sys
import tarfile

HERE = pathlib.Path(__file__).resolve().parent
MEASURE = HERE.parent / "metric-like-check" / "measure.py"


def load_measure():
    spec = importlib.util.spec_from_file_location("measure", MEASURE)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def write_tree(root, revision, dest):
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=root, capture_output=True, check=True).stdout
    dest.mkdir(parents=True, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(dest, filter="data")


def main(argv):
    if len(argv) < 5:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    before, after = argv[1], argv[2]
    out = pathlib.Path(argv[3]).resolve()
    cpu = argv[4]
    repeats = argv[5] if len(argv) > 5 else "3"

    trees = out / "trees"
    for name, revision in (("before", before), ("after", after)):
        if (trees / name).exists():
            sys.exit(f"{trees / name} exists; give a new <out-dir>")
        write_tree(root, revision, trees / name)

    measure = load_measure()
    measure.CONCEPT_TREES = ("before", "after")
    measure.PLAN_TREES = ("before", "after")
    measure.main([str(root), str(trees), str(out), cpu, repeats])


if __name__ == "__main__":
    main(sys.argv[1:])
