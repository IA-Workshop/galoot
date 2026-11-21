import re, sys
rows = []
for line in open(sys.argv[1], encoding="utf-8"):
    if re.match(r"\| [PQKX]\d+ \|", line):
        cells = [c.strip() for c in line.strip()[1:-1].split("|")]
        assert len(cells) == 5, cells
        rows.append((cells[0], cells[1], cells[3]))
seen, bad = set(), 0
ids = [r[0] for r in rows]
dups = {i for i in ids if ids.count(i) > 1}
print("rows", len(rows), "dups", dups)
for i, concept, deps in rows:
    for d in [x.strip() for x in deps.split(",") if x.strip()]:
        if d not in seen:
            print("FORWARD/MISSING", i, "->", d); bad += 1
    seen.add(i)
# words used before the row that defines them (term-level forward use)
terms = {"blade": "K210", "grade": "K210", "weight": "K701", "antiscalar": "K501", "complement": "K503",
         "bivector": "K201", "trivector": "K211", "rotor": "K403", "versor": "K605", "motor": "K805",
         "flector": "K801", "line": "K706", "plane": "K708", "meet": "K507", "antiproduct": "K602",
         "antigrade": "K502", "unitized": "K703", "metric": "K114", "layout": "K104", "multivector": "K105"}
order = {r[0]: n for n, r in enumerate(rows)}
for i, concept, deps in rows:
    if i[0] != "K": continue
    low = re.sub(r"`[^`]*`", "", concept.lower())
    for t, d in terms.items():
        if re.search(r"\b" + t + r"s?\b", low) and order[i] < order[d]:
            print("TERM USED EARLY", i, repr(t), "defined at", d)
print("bad", bad)
