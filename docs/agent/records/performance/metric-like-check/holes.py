#!/usr/bin/env -S python3 -I
"""Compiles the probes in holes/ and records which compile.

Usage:
  holes.py <root> <variants-dir> <out.tsv>

Each compile runs through <root>/tools/timed with Galoot's test flags,
-fsyntax-only, on g++ and clang++. A row gives the probe, the variant tree
(or "-" for a stand-alone sketch), the macro, the compiler, and whether it
compiled.

The concept probes compile only when the concept rejects a bad metric
(hand_aggregate, mutated_copy, forged_by_bit_cast) or, for the two
specializations, only when it accepts one; they run against the trees base,
type, single, cached and cached-flat. The sketches sealed.cpp,
sealed_pointer.cpp and wrapped.cpp run once with no macro (they must compile)
and once with each TRY_ macro.
"""

import pathlib
import subprocess
import sys

FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS",
         "-O0", "-DGALOOT_CHECKING=1", "-fsyntax-only"]
HERE = pathlib.Path(__file__).resolve().parent / "holes"
TREES = ("base", "type", "single", "cached", "cached-flat")
CONCEPT_PROBES = ("hand_aggregate", "mutated_copy", "forged_by_bit_cast",
                  "specialized", "specialized_data")
SKETCHES = {
    "const_class_member": ("",),
    "forged_by_memcpy": ("",),
    "sealed": ("", "TRY_AGGREGATE", "TRY_CONSTRUCTOR", "TRY_MUTATION",
               "TRY_ASSIGNMENT"),
    "sealed_pointer": ("", "TRY_BIT_CAST"),
    "wrapped": ("", "TRY_INVALID", "TRY_SPECIALIZED"),
}


def compile_probe(root, selection, cxx, include, source, macro):
    cmd = [str(root / "tools/timed"), "check", "-", selection, "--", cxx,
           *FLAGS, f"-I{include}", *([f"-D{macro}"] if macro else []),
           str(source)]
    r = subprocess.run(cmd, cwd=root, capture_output=True, text=True)
    first = next((ln for ln in (r.stdout + r.stderr).splitlines()
                  if "error" in ln), "")
    return r.returncode == 0, first[:200].replace("\t", " ")


def main(argv):
    if len(argv) != 3:
        sys.exit(__doc__)
    root = pathlib.Path(argv[0]).resolve()
    variants = pathlib.Path(argv[1]).resolve()
    out = pathlib.Path(argv[2]).resolve()
    rows = ["probe\ttree\tmacro\tcompiler\tcompiles\tfirst error"]
    for cxx in ("g++", "clang++"):
        for probe in CONCEPT_PROBES:
            for tree in TREES:
                ok, err = compile_probe(
                    root, f"metric-like-holes:{probe}:{tree}:{cxx}", cxx,
                    variants / tree / "include", HERE / f"{probe}.cpp", "")
                rows.append(f"{probe}\t{tree}\t-\t{cxx}\t{ok}\t{err}")
                print(rows[-1], flush=True)
        for probe, macros in SKETCHES.items():
            for macro in macros:
                ok, err = compile_probe(
                    root, f"metric-like-holes:{probe}:{macro or 'none'}:{cxx}",
                    cxx, variants / "base" / "include", HERE / f"{probe}.cpp",
                    macro)
                rows.append(f"{probe}\t-\t{macro or '-'}\t{cxx}\t{ok}\t{err}")
                print(rows[-1], flush=True)
    out.write_text("\n".join(rows) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main(sys.argv[1:])
