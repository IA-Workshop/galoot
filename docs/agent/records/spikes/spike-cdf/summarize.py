"""Spikes C, D, F: print the tables of research notes 0010 to 0012 from the
TSV files under out/. Reads only; run it after run_diag.py, run_named.py, and
run_memo.py (main main2 calib second trace).

Usage, from anywhere:  python3 -I summarize.py [c|d|f]
"""

import csv
import statistics
import sys
from pathlib import Path

OUT = Path(__file__).resolve().parent / "out"


def load(path):
    with open(path) as f:
        return list(csv.DictReader(f, delimiter="\t"))


def spike_c():
    rows = load(OUT / "diag/summary.tsv")
    cell = {}
    for r in rows:
        cell[(r["compiler"], r["std"], r["form"], r["case"])] = r
    cols = [("c++23", "R"), ("c++23", "K"), ("c++23", "A"), ("c++23", "P"),
            ("c++23", "D"), ("c++23", "G"), ("c++26", "G")]
    print("Lines / error lines / line of the library's message")
    for cxx in ("g++", "clang++"):
        print(f"\n{cxx}")
        print("| Case | " + " | ".join(f"{f} ({s[-2:]})" for s, f in cols)
              + " |")
        print("| --- |" + " --- |" * len(cols))
        for case in ("10", "1", "21", "22", "23", "31", "32", "4", "5"):
            out = []
            for s, f in cols:
                r = cell.get((cxx, s, f, case))
                if r is None:
                    out.append("")
                    continue
                at = r["message_at_line"] or "-"
                out.append(f"{r['lines']} / {r['error_lines']} / {at}")
            what = next(r["what"] for r in rows if r["case"] == case)
            print(f"| {case} {what} | " + " | ".join(out) + " |")
    print("\nFirst error line, forms R and P, c++23")
    for r in rows:
        if r["form"] in ("R", "P") and r["std"] == "c++23" and \
                r["case"] != "10":
            first = r["first_error_line"].split("/")[-1][:200]
            print(f"{r['compiler']}\t{r['form']}\t{r['case']}\t{first}")


def spike_d():
    m1 = load(OUT / "memo/main.tsv")
    m2 = {(r["compiler"], r["opt"], r["init"], r["memo"], r["uses"]): r
          for r in load(OUT / "memo/main2.tsv")}
    trace = {(r["compiler"], r["opt"], r["init"], r["memo"], r["uses"]): r
             for r in load(OUT / "memo/trace.tsv")}
    print("| Compiler | -O | Form | Uses | Instructions (1e9), two samples |"
          " Wall s (two samples) | Peak MiB | Tables | Evaluations |")
    print("| --- | --- | --- | --- | --- | --- | --- | --- | --- |")
    for r in m1:
        if r["init"] != "consteval":
            continue
        k = (r["compiler"], r["opt"], r["init"], r["memo"], r["uses"])
        s = m2[k]
        t = trace.get(k)
        ev = ""
        if t and r["compiler"] == "clang++":
            ev = t["clang_fill_evaluations"]
        elif t:
            ev = f"cee {t['gcc_constexpr_eval_s']} s, {t['gcc_constexpr_eval_mem']}"
        print(f"| {r['compiler']} | {r['opt']} | {r['memo']} | {r['uses']} | "
              f"{int(r['instructions']) / 1e9:.2f}, "
              f"{int(s['instructions']) / 1e9:.2f} | {r['wall_s']}, "
              f"{s['wall_s']} | {int(r['maxrss_kib']) / 1024:.1f} | "
              f"{r['plan_tables']} | {ev} |")
    print("\nconsteval against constexpr make_plan, instructions 1e9 "
          "(mean of two samples), 16 uses")
    for cxx in ("g++", "clang++"):
        for opt in ("O0", "O2"):
            for memo in ("none", "variable_template", "class_static",
                         "function_local"):
                v = []
                for init in ("consteval", "constexpr"):
                    a = next(r for r in m1 if (r["compiler"], r["opt"],
                             r["init"], r["memo"], r["uses"]) ==
                             (cxx, opt, init, memo, "16"))
                    b = m2[(cxx, opt, init, memo, "16")]
                    v.append(statistics.mean([int(a["instructions"]),
                                              int(b["instructions"])]) / 1e9)
                print(f"{cxx}\t{opt}\t{memo}\t{v[0]:.2f}\t{v[1]:.2f}")
    print("\ncalibration (distinct specializations), instructions 1e9, "
          "peak MiB, tables, evaluations")
    for r in load(OUT / "memo/calib.tsv"):
        t = trace[(r["compiler"], r["opt"], r["init"], r["memo"], r["uses"])]
        ev = t["clang_fill_evaluations"] if r["compiler"] == "clang++" else \
            f"cee {t['gcc_constexpr_eval_s']} s, {t['gcc_constexpr_eval_mem']}"
        print(f"{r['compiler']}\t{r['opt']}\t{r['uses']}\t"
              f"{int(r['instructions']) / 1e9:.2f}\t"
              f"{int(r['maxrss_kib']) / 1024:.1f}\t{r['plan_tables']}\t{ev}")
    print("\nsecond plan beside the first (16 uses)")
    for r in load(OUT / "memo/second.tsv"):
        print(f"{r['compiler']}\t{r['memo']}\t{r['plan_tables']}\t"
              f"{r['second_tables']}\t{int(r['instructions']) / 1e9:.2f}")


def spike_f():
    rows = load(OUT / "named/named.tsv")
    print("| Compiler | -O | Spelling | Reps | Uses | Instructions (1e9) |"
          " Wall s | Peak MiB | .text B | Accessor symbols, mangled length |")
    print("| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |")
    for r in rows:
        sym = (f"{r['accessor_syms']}, {r['accessor_mangled_min']} to "
               f"{r['accessor_mangled_max']}" if r["accessor_syms"] != "0"
               else "0")
        print(f"| {r['compiler']} | {r['opt']} | {r['form']} | {r['reps']} | "
              f"{r['uses']} | {int(r['instructions']) / 1e9:.3f} | "
              f"{r['wall_s']} | {int(r['maxrss_kib']) / 1024:.1f} | "
              f"{r['text_bytes']} | {sym} |")
    print("\nPer added use, 64 to 256 reps (3456 uses): instructions 1e6, "
          "peak KiB")
    for cxx in ("g++", "clang++"):
        for opt in ("O0", "O2"):
            for form in ("string", "index", "subscript"):
                a = next(r for r in rows if (r["compiler"], r["opt"],
                         r["form"], r["reps"]) == (cxx, opt, form, "64"))
                b = next(r for r in rows if (r["compiler"], r["opt"],
                         r["form"], r["reps"]) == (cxx, opt, form, "256"))
                base = next(r for r in rows if (r["compiler"], r["opt"],
                            r["form"]) == (cxx, opt, "none"))
                one = next(r for r in rows if (r["compiler"], r["opt"],
                           r["form"], r["reps"]) == (cxx, opt, form, "1"))
                per = (int(b["instructions"]) - int(a["instructions"])) / 3456
                mem = (int(b["maxrss_kib"]) - int(a["maxrss_kib"])) / 3456
                first = int(one["instructions"]) - int(base["instructions"])
                print(f"{cxx}\t{opt}\t{form}\tper use {per / 1e6:.3f}e6\t"
                      f"{mem:.3f} KiB\t18 distinct + 18 uses over baseline "
                      f"{first / 1e6:.1f}e6")


if __name__ == "__main__":
    which = sys.argv[1:] or ["c", "d", "f"]
    for w in which:
        {"c": spike_c, "d": spike_d, "f": spike_f}[w]()
        print()
