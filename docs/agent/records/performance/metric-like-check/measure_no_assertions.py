#!/usr/bin/env -S python3 -I
"""The concept units of measure.py without -D_GLIBCXX_ASSERTIONS, as a user's
build usually is, for the trees base, cached and cached-flat at n = 4 and 16.

Usage:
  measure_no_assertions.py <root> <variants-dir> <out-dir> <cpu> [repeats]

Writes rows.tsv and summary.txt in <out-dir>: c-use1 minus c-base, and
c-sa-use2 minus c-base, per tree, compiler and n.
"""

import pathlib
import statistics
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import measure  # noqa: E402

TREES = ("base", "cached", "cached-flat")
SIZES = (4, 16)
VARIANTS = {"c-base": (False, 0), "c-use1": (False, 1),
            "c-sa-use2": (True, 2)}


def main(argv):
    if len(argv) < 4:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    variants = pathlib.Path(argv[1]).resolve()
    out = pathlib.Path(argv[2]).resolve()
    cpu = argv[3]
    repeats = int(argv[4]) if len(argv) > 4 else 3
    out.mkdir(parents=True, exist_ok=True)
    measure.FLAGS = [f for f in measure.FLAGS if f != "-D_GLIBCXX_ASSERTIONS"]
    units = variants / "units"
    units.mkdir(exist_ok=True)
    rows = []
    for rep in range(1, repeats + 1):
        for n in SIZES:
            for v, (sa, uses) in VARIANTS.items():
                src = units / f"concept-n{n}-{v}.cpp"
                src.write_text(measure.concept_unit(n, sa, uses),
                               encoding="utf-8")
                for t in TREES:
                    for c in measure.COMPILERS:
                        sel = (f"metric-like-cost-no-assertions:{c}:{t}:n{n}:"
                               f"{v}:{rep}")
                        r = measure.compile_one(root, cpu, sel, c, src,
                                                variants / t / "include")
                        rows.append({"compiler": c, "tree": t, "n": n,
                                     "variant": v, "repeat": rep, **r})
                        print(sel, r["inst_p_u"], flush=True)
    measure.write(out / "rows.tsv", rows)
    lines = ["Without -D_GLIBCXX_ASSERTIONS. Millions of instructions above "
             "c-base, mean of the repeats; spread in parentheses.", ""]
    for c in measure.COMPILERS:
        for t in TREES:
            for v in ("c-use1", "c-sa-use2"):
                parts = []
                for n in SIZES:
                    def stat(variant):
                        x = [int(r["inst_p_u"]) for r in rows
                             if r["compiler"] == c and r["tree"] == t
                             and r["n"] == n and r["variant"] == variant]
                        return statistics.mean(x), max(x) - min(x)
                    a, sa_ = stat(v)
                    b, sb = stat("c-base")
                    parts.append(f"n{n} {(a - b) / 1e6:.2f} "
                                 f"({max(sa_, sb) / 1e6:.2f})")
                lines.append(f"{c} {t} {v}: " + "; ".join(parts))
    (out / "summary.txt").write_text("\n".join(lines) + "\n",
                                     encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
