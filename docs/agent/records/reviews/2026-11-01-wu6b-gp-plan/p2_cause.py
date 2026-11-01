"""Fixed-use growth with n, for the header as it is and for two copies:
loops: blade_product.hpp's two per-term loops over all N generators visit
only the set bits; loops+sign: also reorder_sign visits only the set bits
of its first argument. Usage: run.py <worktree> <outdir> <repeats>"""
import pathlib, shutil, sys, re
wt = pathlib.Path(sys.argv[1]); out = pathlib.Path(sys.argv[2]); reps = int(sys.argv[3])
sys.path.insert(0, str(wt / "docs/agent/records/performance/plan-growth"))
import os; os.chdir(wt)
import measure_plan as mp
out.mkdir(parents=True, exist_ok=True)

def copy(name, sign_too):
    inc = out / name / "include"
    if inc.exists(): shutil.rmtree(inc)
    shutil.copytree(wt / "include", inc)
    bp = inc / "galoot/core/blade_product.hpp"
    t = bp.read_text()
    old1 = ("    for (std::size_t j = 0; j < N; ++j) {\n"
            "      if (((blade >> j) & 1U) == 0U) {\n        continue;\n      }\n")
    new1 = ("    for (mask_t bits = blade; bits != 0; bits &= bits - 1) {\n"
            "      const auto j = static_cast<std::size_t>(std::countr_zero(bits));\n")
    old2 = ("  for (std::size_t j = 0; j < N; ++j) {\n"
            "    if (((rest >> j) & 1U) == 0U) {\n      continue;\n    }\n")
    new2 = ("  for (mask_t bits = rest; bits != 0; bits &= bits - 1) {\n"
            "    const auto j = static_cast<std::size_t>(std::countr_zero(bits));\n")
    assert t.count(old1) == 1 and t.count(old2) == 1
    bp.write_text(t.replace(old1, new1).replace(old2, new2))
    if sign_too:
        bits = inc / "galoot/core/bits.hpp"
        t = bits.read_text()
        old = ("  for (a >>= 1U; a != 0; a >>= 1U) {\n"
               "    swaps += std::popcount(a & b);\n  }\n")
        new = ("  for (; a != 0; a &= a - 1) {\n"
               "    const int i = std::countr_zero(a);\n"
               "    swaps += std::popcount(b & ((mask_t{1} << i) - 1U));\n  }\n")
        assert t.count(old) == 1
        bits.write_text(t.replace(old, new))
    return inc

includes = {"as-is": wt / "include", "loops": copy("loops", False),
            "loops+sign": copy("loops-sign", True)}
units = out / "units"; units.mkdir(exist_ok=True)
rows = []
for rep in range(1, reps + 1):
    for n in (4, 8, 16):
        for count, pairs in ((0, []), (4, mp.TYPED_PAIRS)):
            src = units / f"n{n}-p{count}.cpp"
            src.write_text(mp.plans_unit(n, pairs))
            for cc in ("gcc", "clang"):
                for form, inc in includes.items():
                    r = mp.compile_unit(f"review-p2:{cc}:{form}:n{n}:plans{count}:{rep}",
                                        cc, mp.CONFIGS["galoot-O0"], src, inc, "6")
                    assert r["exit"] == 0, r["stderr"][:2000]
                    rows.append((cc, form, n, count, rep, r["instructions"]))
                    print(cc, form, n, count, rep, r["instructions"], flush=True)
with open(out / "p2.tsv", "w") as f:
    f.write("compiler\tform\tn\tplans\trepeat\tinstructions\n")
    for r in rows: f.write("\t".join(map(str, r)) + "\n")
for o in out.rglob("*.o"): o.unlink()
