#!/usr/bin/env -S python3 -I
"""Compiles the checks that judge metric_like against every variant tree.

Usage:
  check.py <root> <variants-dir> <out.tsv>

<root> is the checkout whose tests are compiled and whose tools/timed records
each compile. <variants-dir> is the output of variants.py. For each variant
and compiler (g++, clang++), with Galoot's test flags at -O0:

  verdict    verdict_check.cpp beside this script, -fsyntax-only, must compile
  metric     test/core/metric_test.cpp, -c, must compile (its static_asserts
             fix the verdict on the library's own tags)
  plan       test/core/plan_test.cpp, -c, must compile (constrained uses)
  not-alg    test/core/plan_not_metric_like_fail.cpp must fail, and its
             diagnostic must contain "metric_like" and not "make_product_plan",
             as the library's compile-fail test requires

Writes one row per check: variant, compiler, check, expected, outcome.
"""

import pathlib
import subprocess
import sys

FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-O0", "-DGALOOT_CHECKING=1"]
HERE = pathlib.Path(__file__).resolve().parent


def run(root, selection, cmd):
    full = [str(root / "tools/timed"), "check", "-", selection, "--", *cmd]
    return subprocess.run(full, cwd=root, capture_output=True, text=True)


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    variants = pathlib.Path(argv[1]).resolve()
    out = pathlib.Path(argv[2]).resolve()
    scratch = variants / "objects"
    scratch.mkdir(exist_ok=True)
    tests = root / "test"
    rows = ["variant\tcompiler\tcheck\texpected\toutcome"]
    for tree in sorted(p for p in variants.iterdir() if (p / "include").is_dir()):
        include = tree / "include"
        for cxx in ("g++", "clang++"):
            common = [cxx, *FLAGS, f"-I{include}",
                      f"-isystem{root / 'include/ext'}", f"-I{tests}",
                      "-DDOCTEST_CONFIG_SUPER_FAST_ASSERTS"]
            checks = {
                "verdict": [*common, "-fsyntax-only",
                            str(HERE / "verdict_check.cpp")],
                "metric": [*common, "-c", "-o",
                           str(scratch / f"metric.{tree.name}.{cxx}.o"),
                           str(tests / "core/metric_test.cpp")],
                "plan": [*common, "-c", "-o",
                         str(scratch / f"plan.{tree.name}.{cxx}.o"),
                         str(tests / "core/plan_test.cpp")],
                "not-alg": [*common, "-fsyntax-only",
                            str(tests / "core/plan_not_metric_like_fail.cpp")],
            }
            for name, cmd in checks.items():
                sel = f"metric-like-check:{tree.name}:{cxx}:{name}"
                r = run(root, sel, cmd)
                text = r.stdout + r.stderr
                if name == "not-alg":
                    ok = (r.returncode != 0 and "metric_like" in text
                          and "make_product_plan" not in text)
                    expected = "fails, names metric_like"
                else:
                    ok = r.returncode == 0
                    expected = "compiles"
                outcome = "as expected" if ok else f"NOT as expected (exit {r.returncode})"
                rows.append(f"{tree.name}\t{cxx}\t{name}\t{expected}\t{outcome}")
                print(rows[-1], flush=True)
                if not ok:
                    (variants / f"{tree.name}.{cxx}.{name}.log").write_text(
                        text, encoding="utf-8")
    out.write_text("\n".join(rows) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
