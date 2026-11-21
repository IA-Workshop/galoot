"""Summarize a build tree's compile.tsv: largest units by peak and by
instructions, totals, and the units of three test programs."""
import csv
import sys


def num(x):
    try:
        return float(x)
    except ValueError:
        return None


def rows(path):
    with open(path, newline="") as f:
        return [r for r in csv.DictReader(f, delimiter="\t") if r["utc_start"] != "utc_start"]


def inst(r):
    p, e = num(r["inst_p_u"]), num(r["inst_e_u"])
    return None if p is None or e is None else p + e


for path in sys.argv[1:]:
    rs = rows(path)
    print(f"== {path}: {len(rs)} rows, revisions {sorted({r['revision'] for r in rs})}")
    bad = [r for r in rs if r["exit"] != "0"]
    print(f"  nonzero exits: {len(bad)} (compile-fail tests expect some)")
    ok = [r for r in rs if r["exit"] == "0"]
    for key, label in ((lambda r: int(r["maxrss_kib"]), "peak"), (lambda r: inst(r) or 0, "inst")):
        top = sorted(ok, key=key, reverse=True)[:4]
        for r in top:
            print(f"  top {label}: {r['source']} peak {r['maxrss_kib']} KiB, inst {inst(r)/1e9:.1f} G, wall {r['wall_s']} s")
    tot = sum(inst(r) or 0 for r in rs)
    print(f"  total inst {tot/1e9:.1f} G over {len(rs)} rows")
    for prog in ("core_identities_rounding_test", "core_transform_identities_test", "core_identities_test"):
        us = [r for r in rs if f"/{prog}.dir/" in r["object"]]
        if not us:
            continue
        s = sum(inst(r) or 0 for r in us)
        big = max(us, key=lambda r: inst(r) or 0)
        pk = max(us, key=lambda r: int(r["maxrss_kib"]))
        print(f"  {prog}: {len(us)} units, {s/1e9:.1f} G, largest inst {big['source'].split('/')[-1]} {inst(big)/1e9:.1f} G wall {big['wall_s']}, largest peak {pk['source'].split('/')[-1]} {pk['maxrss_kib']} KiB")
