#!/usr/bin/env -S python3 -I
"""The constant-evaluation limits at their recorded boundary points, with
-fsyntax-only, in four configurations, on both compilers.

Usage: limits.py <root> <include> <out.tsv> [jobs]

<root> is the checkout whose tools/timed records each compile. Cases:
full<n> alone (Clang 13 and 14, GCC 15 and 16); the plan of full<n> times
full<n> in R(n,0,0), R(n-1,0,1), conformal(n-2) (Clang 5 and 6, GCC 7 and
8) and in the form with 2 on the diagonal and 1 elsewhere (Clang 4 and 5,
GCC 5 and 6); and full<13> on Clang at bare -O2.
"""
import concurrent.futures
import pathlib
import subprocess
import sys
import tempfile

GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare": ["-std=c++23", "-pedantic-errors"],
    "bare-O2": ["-std=c++23", "-pedantic-errors", "-O2"],
}
COMPILERS = {"gcc": "g++", "clang": "clang++"}
POINTS = {  # unit kind: {compiler: (builds, fails)}
    "full": {"clang": (13, 14), "gcc": (15, 16)},
    "r": {"clang": (5, 6), "gcc": (7, 8)},
    "rigid": {"clang": (5, 6), "gcc": (7, 8)},
    "conformal": {"clang": (5, 6), "gcc": (7, 8)},
    "dense": {"clang": (4, 5), "gcc": (5, 6)},
}
LIMIT_WORDS = ("step limit", "operation count", "constexpr-ops-limit",
               "not a constant expression", "constexpr-loop-limit",
               "depth")


def form(kind, n):
    if kind == "r":
        return f"core::R<{n}, 0, 0>()"
    if kind == "rigid":
        return f"core::R<{n - 1}, 0, 1>()"
    if kind == "conformal":
        return f"core::conformal<{n - 2}>()"
    rows = ", ".join("{" + ", ".join("2" if i == j else "1" for j in range(n))
                     + "}" for i in range(n))
    return f"core::from_gram({{{rows}}})"


def unit(kind, n):
    head = ("#include <galoot/core/blade_set.hpp>\n"
            "#include <galoot/core/metric.hpp>\n"
            "#include <galoot/core/plan.hpp>\n"
            "namespace core = galoot::core;\n")
    if kind == "full":
        return head + f"static_assert(core::full<{n}>.size() == {1 << n});\n"
    return (head + f"struct alg {{ static constexpr auto value = "
            f"{form(kind, n)}; }};\n"
            f"static_assert(core::detail::gp_plan<alg, core::full<{n}>, "
            f"core::full<{n}>>.size() > 0);\n")


def one(root, include, tmp, compiler, config, kind, n):
    src = pathlib.Path(tmp) / f"{compiler}-{config}-{kind}-{n}.cpp"
    src.write_text(unit(kind, n), encoding="utf-8")
    sel = f"review-0012-limits:{compiler}:{config}:{kind}:{n}"
    r = subprocess.run(
        [str(root / "tools/timed"), "measure", "-", sel, "--",
         COMPILERS[compiler], *CONFIGS[config], f"-I{include}",
         "-fsyntax-only", str(src)],
        cwd=root, capture_output=True, text=True)
    errs = [ln for ln in r.stderr.splitlines() if "error" in ln or
            any(w in ln for w in LIMIT_WORDS)]
    why = next((ln for ln in r.stderr.splitlines()
                if any(w in ln for w in LIMIT_WORDS)), errs[0] if errs else "-")
    return (compiler, config, kind, n, r.returncode, why.split("include/")[-1][:180])


def main(argv):
    root = pathlib.Path(argv[0]).resolve()
    include = pathlib.Path(argv[1]).resolve()
    out = pathlib.Path(argv[2])
    jobs = int(argv[3]) if len(argv) > 3 else 6
    cases = [(c, cfg, k, n) for k, pts in POINTS.items()
             for c, ns in pts.items() for n in ns for cfg in CONFIGS]
    with tempfile.TemporaryDirectory() as tmp, \
            concurrent.futures.ThreadPoolExecutor(jobs) as pool:
        rows = list(pool.map(lambda a: one(root, include, tmp, *a), cases))
    lines = ["compiler\tconfig\tunit\tn\texit\tfirst_limit_or_error"]
    lines += ["\t".join(str(x) for x in r) for r in rows]
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("\n".join(lines))


if __name__ == "__main__":
    main(sys.argv[1:])
