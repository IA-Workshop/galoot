import csv, sys
base = sys.argv[1]
for preset in sys.argv[2:]:
    for mode in "cs":
        rows = list(csv.DictReader(open(f"{base}/{preset}/{mode}.tsv"), delimiter="\t"))
        num = lambda k: sum(int(r[k]) for r in rows if r[k].isdigit())
        print(preset, {"c": "-c", "s": "-fsyntax-only"}[mode], len(rows),
              "wall %.3f s" % sum(float(r["wall_s"]) for r in rows),
              "inst %.3f G" % (num("inst_p_u") / 1e9),
              "cycles %.3f G" % (num("cycles_u") / 1e9),
              "peak rss %d KiB" % max(int(r["maxrss_kib"]) for r in rows),
              "uncounted", sum(not r["inst_p_u"].isdigit() for r in rows),
              "sibling busy max %.2f s" % max(float(r["sibling_busy_s"]) for r in rows),
              rows[0]["mode"], rows[0]["cpus"], sep="\t")
