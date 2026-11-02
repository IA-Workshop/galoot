#!/usr/bin/env -S python3 -I
"""Measure WU8a's mv.hpp: where the default limits stop mv<Alg, full<n>, T>,
what a type, a construction, and a get cost a program, and what the headers
cost every includer, before and after.

Usage, from the worktree root, with the job pool exported:
  measure_mv.py limits <out-dir> [cpu]
  measure_mv.py uses <out-dir> [repeats] [cpu]
  measure_mv.py headers <before-revision> <out-dir> [repeats] [cpu]

Every compile runs as `taskset -c <cpu> tools/timed measure ...` (default
CPU 10, a performance core), so its row in runs.tsv is in pinned mode, with
the instructions retired, the wall time, the peak memory, and the busy time
of the sibling thread and of other processes on the CPU. The headers are the
checkout's include/, which must have no uncommitted changes.

limits   mv<R(n,0,0), full<n>, double>, instantiated and default-constructed,
         from n = 1 up to the first n that fails and one more, in the three
         configurations of the plan limits: Galoot's test flags at -O0 with
         checking and at -O2 without, and a bare -std=c++23 -pedantic-errors
         -O2. One trial build at each n, at the default limits.
uses     Units with REPS = 0, 64, and 256 of one use each, at -O0 and -O2:
         sets      REPS distinct four-blade layouts in R(15,0,1), no mv;
         types     the same layouts, each made an mv type and instantiated;
         construct the same types, each constructed once from four values
                   read at run time;
         repeat    one type constructed REPS times, in REPS functions;
         get       a rigid(3) line read by get<"name"> on all six names in
                   each of REPS functions;
         index     the same reads by position, coefficients[i].
         The cost per use is the slope from 64 to 256: per type, types -
         sets; per construction of a new type, construct - types; per
         construction of an existing type, repeat; per get, get - index,
         against index alone.
headers  An empty unit including galoot/core.hpp, galoot/core/mv.hpp, or
         galoot/core/blade_set.hpp, alone, at -O0 with the test flags, for
         <before-revision> (extracted with git archive; mv.hpp absent there)
         and the checkout, interleaved repeat by repeat.
"""

import io
import pathlib
import statistics
import subprocess
import sys
import tarfile

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare-O2": ["-std=c++23", "-pedantic-errors", "-O2"],
}
LIMIT_TEXT = ("step limit", "operation count exceeds", "constexpr-ops-limit",
              "constexpr-loop-limit", "constexpr-depth", "not a constant",
              "constant expression", "maximum", "exceeded")
KEYS = ("exit", "inst_p_u", "wall_s", "maxrss_kib", "sibling_busy_s",
        "pinned_other_s")
REPS = (0, 64, 256)

PREAMBLE = """#include <array>
#include <cstddef>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace gc = galoot::core;

"""


def read_last_row():
    with open(ROOT / "docs/agent/records/performance/runs.tsv",
              encoding="utf-8") as f:
        lines = f.read().splitlines()
    header = lines[0].split("\t")
    return dict(zip(header, lines[-1].split("\t")))


def compile_pinned(selection, compiler, flags, source, include, cpu):
    obj = source.with_suffix(f".{compiler}.o")
    command = ["taskset", "-c", cpu, str(ROOT / "tools" / "timed"), "measure",
               "-", selection, "--", COMPILERS[compiler], *flags,
               f"-I{include}", "-c", "-o", str(obj), str(source)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    row = read_last_row()
    if row.get("selection") != selection:
        sys.exit(f"the last run row is not {selection}")
    if row.get("mode") != "pinned":
        sys.exit(f"{selection} was not pinned: mode {row.get('mode')}")
    obj.unlink(missing_ok=True)
    limit = next((line.strip().replace(f"{ROOT}/", "")
                  for line in result.stderr.splitlines()
                  if "error" in line and any(t in line for t in LIMIT_TEXT)),
                 next((line.strip() for line in result.stderr.splitlines()
                       if "error" in line), "-"))
    found = {key: row.get(key, "unavailable") for key in KEYS[1:]}
    return {"exit": result.returncode, **found, "limit": limit[:200]}


def write_rows(path, header, rows):
    path.write_text("\t".join(header) + "\n" + "".join(
        "\t".join(str(r[h]) for h in header) + "\n" for r in rows),
        encoding="utf-8")


def clean_checkout():
    status = subprocess.run(
        ["git", "status", "--porcelain", "--", "include"], cwd=ROOT,
        capture_output=True, text=True, check=True).stdout
    if status.strip():
        sys.exit("include/ has uncommitted changes:\n" + status)


def full_unit(n):
    return PREAMBLE + (
        "struct alg {\n"
        f"  static constexpr auto value = gc::R<{n}, 0, 0, 32>();\n"
        "};\n"
        f"using M = gc::mv<alg, gc::full<{n}>, double>;\n"
        "static_assert(std::is_trivially_copyable_v<M>);\n"
        "double first() {\n"
        "  const M zero{};\n"
        "  return zero.coefficients[0];\n"
        "}\n")


def limits(out, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    rows = []
    for compiler in COMPILERS:
        for config in CONFIGS:
            failed = 0
            for n in range(1, 17):
                source = units / f"limits-full-n{n}.cpp"
                source.write_text(full_unit(n), encoding="utf-8")
                r = compile_pinned(f"mv-limits:{compiler}:{config}:full-n{n}",
                                   compiler, CONFIGS[config], source,
                                   ROOT / "include", cpu)
                rows.append({"compiler": compiler, "config": config,
                             "unit": f"full-n{n}", **r})
                print(compiler, config, n, r["exit"], r["inst_p_u"],
                      r["wall_s"], r["maxrss_kib"], r["limit"], flush=True)
                failed += 1 if r["exit"] != 0 else 0
                if failed == 2:
                    break
    write_rows(out / "limits.tsv",
               ["compiler", "config", "unit", *KEYS, "limit"], rows)


def sets_text(reps):
    text = "struct alg {\n  static constexpr auto value = gc::R<15, 0, 1>();\n};\n"
    for i in range(reps):
        masks = [4 * i + k for k in range(1, 5)]
        blades = ", ".join(f"{{.mask = {m}, .sign = 1}}" for m in masks)
        text += (f"inline constexpr gc::blade_set<4> set{i}{{"
                 f"std::to_array<gc::oriented_blade>({{{blades}}})}};\n")
    return text


def uses_unit(kind, reps):
    text = PREAMBLE
    if kind in ("sets", "types", "construct"):
        text += sets_text(reps)
        for i in range(reps):
            if kind == "types":
                text += (f"static_assert(std::is_trivially_copyable_v<"
                         f"gc::mv<alg, set{i}, double>>);\n")
            elif kind == "construct":
                text += (f"double use{i}(const volatile double* v) {{\n"
                         f"  const gc::mv<alg, set{i}, double> x{{v[0], v[1], "
                         f"v[2], v[3]}};\n"
                         f"  return x.coefficients[3];\n}}\n")
    elif kind == "repeat":
        text += sets_text(1)
        for i in range(reps):
            text += (f"double use{i}(const volatile double* v) {{\n"
                     f"  const gc::mv<alg, set0, double> x{{v[0], v[1], v[2], "
                     f"v[{i % 4}]}};\n"
                     f"  return x.coefficients[3];\n}}\n")
    else:
        text += ("struct rga3 {\n"
                 "  static constexpr auto value = gc::rigid<3>();\n};\n"
                 "inline constexpr auto line_layout =\n"
                 "    gc::blades<rga3, \"e41 e42 e43 e23 e31 e12\">;\n"
                 "using Line = gc::mv<rga3, line_layout, double>;\n")
        names = ["e41", "e42", "e43", "e23", "e31", "e12"]
        for i in range(reps):
            text += f"double use{i}(const Line& l) {{\n  return 0.0"
            for k, name in enumerate(names):
                read = (f"get<\"{name}\">(l)" if kind == "get"
                        else f"l.coefficients[{k}]")
                text += f" + {read}"
            text += f" + {i};\n}}\n"
    return text


def uses(out, repeats, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for config in ("galoot-O0", "galoot-O2"):
                for kind in ("sets", "types", "construct", "repeat", "get",
                             "index"):
                    for reps in REPS:
                        source = units / f"uses-{kind}-{reps}.cpp"
                        source.write_text(uses_unit(kind, reps),
                                          encoding="utf-8")
                        r = compile_pinned(
                            f"mv-uses:{compiler}:{config}:{kind}:{reps}:"
                            f"r{repeat}", compiler, CONFIGS[config], source,
                            ROOT / "include", cpu)
                        rows.append({"compiler": compiler, "config": config,
                                     "kind": kind, "reps": reps,
                                     "repeat": repeat, **r})
                        print(compiler, config, kind, reps, repeat, r["exit"],
                              r["inst_p_u"], r["wall_s"], r["maxrss_kib"],
                              flush=True)
    write_rows(out / "uses.tsv", ["compiler", "config", "kind", "reps",
                                  "repeat", *KEYS], rows)
    (out / "uses-summary.txt").write_text(uses_summary(rows),
                                          encoding="utf-8")


def mean(rows, **match):
    values = [int(r["inst_p_u"]) for r in rows
              if all(r[k] == v for k, v in match.items())]
    return statistics.mean(values)


def uses_summary(rows):
    lines = ["Retired instructions per use, slope from 64 to 256 uses, mean "
             "of the repeats", ""]
    for compiler in COMPILERS:
        for config in ("galoot-O0", "galoot-O2"):
            def slope(kind):
                return (mean(rows, compiler=compiler, config=config,
                             kind=kind, reps=256) -
                        mean(rows, compiler=compiler, config=config,
                             kind=kind, reps=64)) / 192
            per = {kind: slope(kind) for kind in
                   ("sets", "types", "construct", "repeat", "get", "index")}
            lines.append(
                f"{compiler} {config}: type {per['types'] - per['sets']:.3e}, "
                f"construction of a new type "
                f"{per['construct'] - per['types']:.3e}, construction of an "
                f"existing type {per['repeat']:.3e} (a function each), get "
                f"{(per['get'] - per['index']) / 6:.3e} above an index read "
                f"(index function of six reads {per['index']:.3e})")
    return "\n".join(lines) + "\n"


def extract(revision, dest):
    archive = subprocess.run(["git", "archive", revision, "include"],
                             cwd=ROOT, capture_output=True, check=True).stdout
    dest.mkdir(parents=True, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(dest, filter="data")


def headers(before, out, repeats, cpu):
    clean_checkout()
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    trees = {"before": out / "before" / "include", "after": ROOT / "include"}
    if not trees["before"].exists():
        extract(before, out / "before")
    rows = []
    for repeat in range(repeats):
        for compiler in COMPILERS:
            for header in ("galoot/core.hpp", "galoot/core/mv.hpp",
                           "galoot/core/blade_set.hpp"):
                name = header.replace("/", "_").replace(".hpp", "")
                source = units / f"header-{name}.cpp"
                source.write_text(f"#include <{header}>\n", encoding="utf-8")
                for tree, include in trees.items():
                    if not (include / header).exists():
                        continue
                    r = compile_pinned(
                        f"mv-headers:{compiler}:{tree}:{name}:r{repeat}",
                        compiler, CONFIGS["galoot-O0"], source, include, cpu)
                    rows.append({"compiler": compiler, "tree": tree,
                                 "header": header, "repeat": repeat, **r})
                    print(compiler, tree, header, repeat, r["exit"],
                          r["inst_p_u"], r["wall_s"], r["maxrss_kib"],
                          flush=True)
    write_rows(out / "headers.tsv", ["compiler", "tree", "header", "repeat",
                                     *KEYS], rows)


def main(argv):
    if not argv:
        sys.exit(__doc__)
    mode, rest = argv[0], argv[1:]
    if mode == "limits" and len(rest) >= 1:
        limits(pathlib.Path(rest[0]).resolve(), rest[1] if len(rest) > 1
               else "10")
    elif mode == "uses" and len(rest) >= 1:
        uses(pathlib.Path(rest[0]).resolve(),
             int(rest[1]) if len(rest) > 1 else 3,
             rest[2] if len(rest) > 2 else "10")
    elif mode == "headers" and len(rest) >= 2:
        headers(rest[0], pathlib.Path(rest[1]).resolve(),
                int(rest[2]) if len(rest) > 2 else 3,
                rest[3] if len(rest) > 3 else "10")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
