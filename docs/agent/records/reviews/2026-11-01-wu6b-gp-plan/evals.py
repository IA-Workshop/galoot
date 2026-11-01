"""Can the evaluation count fail? Runs the count of measure_plan.py evals on
the header as it is and on mutants that generate each plan twice, and also
sums the durations of the evaluations. Usage: run.py <worktree> <outdir>"""
import collections, json, os, pathlib, re, shutil, sys
wt = pathlib.Path(sys.argv[1]); out = pathlib.Path(sys.argv[2])
sys.path.insert(0, str(wt / "docs/agent/records/performance/plan-growth"))
os.chdir(wt)
import measure_plan as mp
out.mkdir(parents=True, exist_ok=True)

def variant(name, edit):
    inc = out / name / "include"
    if inc.exists(): shutil.rmtree(inc)
    shutil.copytree(wt / "include", inc)
    h = inc / "galoot/core/plan.hpp"
    h.write_text(edit(h.read_text()))
    return inc

def all_plain(t):
    s = t.index("consteval auto make_gp_plan() {"); e = t.index("\n}\n", s)
    body = t[s:e]; assert body.count("static constexpr ") == 7
    return t[:s] + body.replace("static constexpr ", "constexpr ") + t[e:]

def evaluated_twice(t):
    # The initializer calls the generator twice; the second call's result is used.
    old = "inline constexpr auto gp_plan = make_gp_plan<Alg, A, B>();"
    assert t.count(old) == 1
    return t.replace(old, "inline constexpr auto gp_plan =\n"
                     "    (static_cast<void>(make_gp_plan<Alg, A, B>()), make_gp_plan<Alg, A, B>());")

def twice_plain(t):
    return evaluated_twice(all_plain(t))

includes = {"as-is": wt / "include", "all-plain": variant("all-plain", all_plain),
            "init-twice": variant("init-twice", evaluated_twice),
            "init-twice-plain": variant("init-twice-plain", twice_plain)}
work = out / "work"; work.mkdir(exist_ok=True)
two = work / "two_uses.cpp"; two.write_text(mp.TWO_USES)
shutil.copy(wt / "test/core/plan_test.cpp", work / "plan_test.cpp")
shutil.copy(wt / "test/core/signatures.hpp", work / "signatures.hpp")
lines = ["include\tunit\tinstantiations\tgp_plan_evaluations\tfill_evaluations\tgp_plan_init_us\tinstantiation_us\tinstructions"]
for form, inc in includes.items():
    for unit in ("plan_test", "two_uses"):
        src = work / f"{unit}.cpp"
        r = mp.compile_unit(f"review-evals:clang:{form}:{unit}", "clang", mp.CONFIGS["galoot-O0"],
                            src, inc, "6", extra=["-ftime-trace", "-ftime-trace-granularity=0",
                                                  f"-I{wt / 'include' / 'ext'}"])
        assert r["exit"] == 0, r["stderr"][:3000]
        ev = [e for e in json.loads(src.with_suffix(".clang.json").read_text())["traceEvents"] if e.get("ph") == "X"]
        inst = [(e["ts"], e["ts"] + e["dur"]) for e in ev if e["name"] == "InstantiateFunction"
                and re.match(r"galoot::core::detail::make_gp_plan<", e.get("args", {}).get("detail", ""))]
        c = collections.Counter(); init_us = 0; inst_us = sum(b - a for a, b in inst)
        for e in ev:
            d = e.get("args", {}).get("detail", "")
            if e["name"] != "EvaluateAsInitializer": continue
            if d == "galoot::core::detail::gp_plan":
                c["gp"] += 1; init_us += e["dur"]
            elif d == "plan" and any(a <= e["ts"] < b for a, b in inst):
                c["fill"] += 1
        lines.append(f"{form}\t{unit}\t{len(inst)}\t{c['gp']}\t{c['fill']}\t{init_us}\t{inst_us}\t{r['instructions']}")
        print(lines[-1], flush=True)
        src.with_suffix(".clang.json").unlink(); src.with_suffix(".clang.o").unlink()
(out / "evals.tsv").write_text("\n".join(lines) + "\n")
