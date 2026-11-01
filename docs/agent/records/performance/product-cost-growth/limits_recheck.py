#!/usr/bin/env -S python3 -I
"""Recheck where the compilers' default limits on constant evaluation stop
full<n> and the plan of full<n> times full<n>, with the checkout's headers.

Usage, from the repository root:
  limits_recheck.py <out-dir>

Each case starts one below the last n recorded to build and rises until the
first n that fails, so that a limit that moved in either direction shows.
Four configurations: Galoot's test flags at -O0 with checking and at -O2
without (both with -D_GLIBCXX_ASSERTIONS), a bare -std=c++23
-pedantic-errors (libstdc++ enables its assertions without -O), and the
same with -O2 (assertions off). Each compile is -fsyntax-only through
tools/timed, selection limits-0012:<compiler>:<config>:<unit>. Writes
<out-dir>/limits.tsv with each exit status and the first diagnostic line
that names a limit.
"""

import pathlib
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare-O0": ["-std=c++23", "-pedantic-errors"],
    "bare-O2": ["-std=c++23", "-pedantic-errors", "-O2"],
}
LIMIT_TEXT = ("step limit", "operation count exceeds", "constexpr-ops-limit",
              "constexpr-loop-limit", "constexpr-depth", "not a constant")
# The last n recorded to build, per compiler: full<n> alone, and the plan of
# full<n> times full<n> in each form.
RECORDED = {
    "clang++": {"full": 13, "r": 5, "rigid": 5, "conformal": 5, "dense": 4},
    "g++": {"full": 15, "r": 7, "rigid": 7, "conformal": 7, "dense": 5},
}


def form(name, n):
    if name == "r":
        return f"galoot::core::R<{n}, 0, 0>()"
    if name == "rigid":
        return f"galoot::core::R<{n - 1}, 0, 1>()"
    if name == "conformal":
        return f"galoot::core::conformal<{n - 2}>()"
    rows = ", ".join("{" + ", ".join("2" if i == j else "1" for j in range(n))
                     + "}" for i in range(n))
    return f"galoot::core::from_gram({{{rows}}})"


def unit(name, n):
    if name == "full":
        return ("#include <galoot/core/blade_set.hpp>\n"
                f"static_assert(galoot::core::full<{n}>.size() == "
                f"{1 << n});\n")
    return ("#include <galoot/core/blade_set.hpp>\n"
            "#include <galoot/core/metric.hpp>\n"
            "#include <galoot/core/plan.hpp>\n"
            f"struct alg {{ static constexpr auto value = {form(name, n)}; }};\n"
            "static_assert(galoot::core::detail::gp_plan<alg, "
            f"galoot::core::full<{n}>, galoot::core::full<{n}>>.size() > 0);\n")


def main(argv):
    if len(argv) != 1:
        sys.exit(__doc__)
    out = pathlib.Path(argv[0]).resolve()
    units = out / "limit-units"
    units.mkdir(parents=True, exist_ok=True)
    rows = ["compiler\tconfig\tunit\tn\texit\tlimit"]
    for compiler, recorded in RECORDED.items():
        for config, flags in CONFIGS.items():
            for name, last in recorded.items():
                n = last - 1
                while True:
                    source = units / f"{name}{n}.cpp"
                    source.write_text(unit(name, n), encoding="utf-8")
                    selection = f"limits-0012:{compiler}:{config}:{name}{n}"
                    r = subprocess.run(
                        [str(ROOT / "tools" / "timed"), "measure", "-",
                         selection, "--", compiler, *flags,
                         f"-I{ROOT / 'include'}", "-fsyntax-only",
                         str(source)],
                        cwd=ROOT, capture_output=True, text=True)
                    limit = next((line.strip().replace(f"{ROOT}/", "")[:160]
                                  for line in r.stderr.splitlines()
                                  if any(t in line for t in LIMIT_TEXT)), "-")
                    rows.append(f"{compiler}\t{config}\t{name}\t{n}\t"
                                f"{r.returncode}\t{limit}")
                    print(rows[-1], flush=True)
                    source.unlink()
                    if r.returncode != 0 or n == 32:
                        break
                    n += 1
    units.rmdir()
    (out / "limits.tsv").write_text("\n".join(rows) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
