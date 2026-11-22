"""Tree-level figures of the 1.0.1 release matrix against the 1.0.0 release
matrix, the cost-cleanup merge matrix and the header-hygiene gate: wall time
of each step, build instructions, the largest process, the sum of the
build's compile rows, and the part of the build outside the compile rows.

Usage: python3 -I compare.py <runs.tsv> <compile dir of this matrix>
Run from the worktree root; the earlier gates' compile copies are read from
their committed record directories."""
import csv
import os
import sys

R = "578aa15"
PRESETS = ["gcc-debug", "clang-debug", "gcc-release", "gcc-asan", "clang-release",
           "clang-asan", "lint", "gcc-measure", "gcc-measure-o3", "clang-measure",
           "clang-measure-o3"]
GATES = "docs/agent/records/gates"
EARLIER = {
    "05229a7": "docs/agent/records/performance/cost-cleanup-1-0-1/matrix/compile-{p}.tsv",
    "41d8029": GATES + "/2026-11-21-header-hygiene-1-0-1-gate/out/compile-{p}.tsv",
}


def inst(r):
    try:
        return float(r["inst_p_u"]) + float(r["inst_e_u"])
    except ValueError:
        return None


def runs(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def compile_sum(path):
    if not os.path.exists(path):
        return None, 0
    with open(path, newline="") as f:
        rs = [r for r in csv.DictReader(f, delimiter="\t") if r["utc_start"] != "utc_start"]
    return sum(inst(r) or 0 for r in rs), len(rs)


def main(runs_path, here):
    rows = runs(runs_path)
    by = {}
    for r in rows:
        sel = r["selection"]
        if sel.startswith("gate:all@"):
            by[(sel[9:], r["preset"], r["step"])] = r
    revs = ["7580e88", "05229a7", "41d8029", R]
    print("preset\trev\twall_total\tconfigure\tbuild\ttest\tbuild_inst_G\tbuild_maxrss_kib\t"
          "compile_sum_G\tcompile_rows\toutside_G\ttest_maxrss_kib")
    for p in PRESETS:
        for rev in revs:
            st = {s: by.get((rev, p, s)) for s in ("configure", "build", "test")}
            if not st["build"]:
                continue
            wall = sum(float(x["wall_s"]) for x in st.values() if x)
            b = st["build"]
            bi = inst(b)
            if rev == R:
                cpath = os.path.join(here, f"compile-{p}-after-build.tsv")
            else:
                cpath = EARLIER.get(rev, "").format(p=p)
            cs, n = compile_sum(cpath) if cpath else (None, 0)
            outside = None if cs is None or bi is None or n == 0 else bi - cs
            f = lambda v: "" if v is None else f"{v / 1e9:.1f}"
            print("\t".join([p, rev, f"{wall:.1f}",
                             *(st[s]["wall_s"] if st[s] else "" for s in ("configure", "build", "test")),
                             f(bi), b["maxrss_kib"], f(cs), str(n), f(outside),
                             st["test"]["maxrss_kib"] if st["test"] else ""]))
    for rev in revs:
        rs = [r for (v, _, _), r in by.items() if v == rev]
        print(f"# {rev}: {len(rs)} rows, wall {sum(float(r['wall_s']) for r in rs):.2f} s, "
              f"exits {sorted({r['exit'] for r in rs})}, revisions {sorted({r['revision'] for r in rs})}")


main(*sys.argv[1:])
