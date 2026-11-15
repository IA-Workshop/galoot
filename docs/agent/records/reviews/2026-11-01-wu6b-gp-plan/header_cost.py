"""What plan.hpp adds to a unit that includes it and uses nothing.
Usage: run.py <worktree> <outdir> <repeats>"""
import collections, json, os, pathlib, sys
wt = pathlib.Path(sys.argv[1]); out = pathlib.Path(sys.argv[2]); reps = int(sys.argv[3])
sys.path.insert(0, str(wt / "docs/agent/records/performance/plan-growth"))
os.chdir(wt)
import measure_plan as mp
out.mkdir(parents=True, exist_ok=True)
units = {
  "blade_product": "#include <galoot/core/blade_product.hpp>\n",
  "blade_product+span": "#include <span>\n#include <galoot/core/blade_product.hpp>\n",
  "blade_product+span-uses": "#include <span>\n#include <galoot/core/blade_product.hpp>\n"
      "void f(std::span<const galoot::core::oriented_blade>);\n"
      "void g(std::span<galoot::core::detail::term>);\nvoid h(std::span<const galoot::core::detail::term>);\n",
  "plan": "#include <galoot/core/plan.hpp>\n",
  "core": "#include <galoot/core.hpp>\n",
}
rows = []
for rep in range(1, reps + 1):
    for name, text in units.items():
        src = out / f"{name}.cpp"; src.write_text(text)
        for cc in ("gcc", "clang"):
            r = mp.compile_unit(f"review-header:{cc}:{name}:{rep}", cc, mp.CONFIGS["galoot-O0"], src, wt / "include", "6")
            assert r["exit"] == 0, r["stderr"][:2000]
            rows.append((cc, name, rep, r["instructions"], r["peak_kib"]))
            print(*rows[-1], flush=True)
# One Clang trace of the plan unit: the costliest instantiations by name.
src = out / "plan.cpp"
r = mp.compile_unit("review-header:clang:plan:trace", "clang", mp.CONFIGS["galoot-O0"], src, wt / "include", "6",
                    extra=["-ftime-trace", "-ftime-trace-granularity=0"])
ev = [e for e in json.loads(src.with_suffix(".clang.json").read_text())["traceEvents"] if e.get("ph") == "X"]
tot = collections.Counter(); span = 0
for e in ev:
    if e["name"] in ("Total Frontend", "Total ExecuteCompiler", "Total InstantiateClass", "Total InstantiateFunction",
                     "Total Source", "Total ParseClass", "Total PerformPendingInstantiations"):
        tot[e["name"]] += e["dur"]
top = [e for e in ev if e["name"] in ("InstantiateClass", "InstantiateFunction", "ParseClass", "Source")]
span_classes = sum(e["dur"] for e in top if e["name"] == "InstantiateClass" and "span<" in e.get("args", {}).get("detail", ""))
with open(out / "header.tsv", "w") as f:
    f.write("compiler\tunit\trepeat\tinstructions\tpeak_kib\n")
    for row in rows: f.write("\t".join(map(str, row)) + "\n")
    f.write("\n# Clang trace of plan.cpp, microseconds\n")
    for k, v in sorted(tot.items()): f.write(f"# {k}\t{v}\n")
    f.write(f"# InstantiateClass events naming span<, summed (nested events overlap)\t{span_classes}\n")
    for e in sorted(top, key=lambda e: -e["dur"])[:25]:
        f.write(f"# {e['name']}\t{e['dur']}\t{e.get('args', {}).get('detail', '')[:120]}\n")
for p in out.glob("*.o"): p.unlink()
