"""Boundary checks of the default limits of gp_plan, full<n> x full<n>,
written independently of measure_plan.py. Usage: run.py <worktree> <outdir>"""
import subprocess, sys, pathlib
wt = pathlib.Path(sys.argv[1]); out = pathlib.Path(sys.argv[2]); out.mkdir(exist_ok=True)
GAL = ["-std=c++23","-Wall","-Wextra","-Wpedantic","-Wconversion","-Wshadow","-Werror",
       "-ffp-contract=off","-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {"galoot-O0": GAL+["-O0","-DGALOOT_CHECKING=1"],
           "galoot-O2": GAL+["-O2","-DGALOOT_CHECKING=0"],
           "bare-O0": ["-std=c++23","-pedantic-errors"],
           "bare-O2-noassert": ["-std=c++23","-pedantic-errors","-O2"]}
def form(f, n):
    if f == "r": return f"galoot::core::R<{n}, 0, 0>()"
    if f == "rigid": return f"galoot::core::R<{n-1}, 0, 1>()"
    if f == "conformal": return f"galoot::core::conformal<{n-2}>()"
    rows = ", ".join("{"+", ".join("2" if i==j else "1" for j in range(n))+"}" for i in range(n))
    return f"galoot::core::from_gram({{{rows}}})"
CASES = {"clang++": [("r",5),("r",6),("rigid",5),("rigid",6),("conformal",5),("conformal",6),("dense",4),("dense",5)],
         "g++": [("r",7),("r",8),("rigid",7),("rigid",8),("conformal",7),("conformal",8),("dense",5),("dense",6)]}
for cc, cases in CASES.items():
    for cfg, flags in CONFIGS.items():
        for f, n in cases:
            src = out / f"u-{f}{n}.cpp"
            src.write_text("#include <galoot/core/blade_set.hpp>\n#include <galoot/core/metric.hpp>\n"
                           "#include <galoot/core/plan.hpp>\n"
                           f"struct alg {{ static constexpr auto value = {form(f, n)}; }};\n"
                           f"static_assert(galoot::core::detail::gp_plan<alg, galoot::core::full<{n}>, galoot::core::full<{n}>>.size() > 0);\n")
            sel = f"review-limits:{cc}:{cfg}:{f}{n}"
            r = subprocess.run([str(wt/"tools/timed"), "measure", "-", sel, "--", cc, *flags,
                                f"-I{wt/'include'}", "-fsyntax-only", str(src)],
                               cwd=wt, capture_output=True, text=True)
            lim = next((l.strip()[:110] for l in r.stderr.splitlines() if "limit" in l), "-")
            print(cc, cfg, f"{f}{n}", r.returncode, lim, flush=True)
