#!/usr/bin/env -S python3 -I
"""Compare a signature passed as a tag type with one passed as a metric value.

Snippet candidate: a generator-and-compile harness for comparing two spellings
of a template argument by symbol length, diagnostics, and compile cost.

Usage: measure_tag_value.py <repo-root> <out-dir> [cpu]

For GCC and Clang, three measurements over galoot/core/metric.hpp:

  names   One function template instantiated per algebra, once with a tag type
          that carries the metric and once with the metric value itself, for
          rigid(3) (n = 4), conformal(3) (n = 5), and R(16, 0, 0). Records the
          mangled and demangled length of each symbol.
  diag    Two algebras with the same n, conformal(3) and R(4, 1, 0), passed to
          a function that needs both arguments in one algebra. Records the
          diagnostic, and whether it prints conformal(3)'s matrix with every
          entry in place.
  cost    K distinct signatures of n = 16, K in {0, 1, 16, 64, 128}, each
          declared and used once by a probe template: as a tag checked by
          metric_like, as a tag with no constraint, or as a value.
          Records retired user-space instructions, wall time, and peak memory
          of the compile at -O0.
  reuse   One n = 16 tag used by T distinct function templates, T in {1, 64},
          each constrained by metric_like or unconstrained. Shows whether the
          compilers check the concept once per tag or once per use.

Compiles run one at a time, pinned to `cpu` (default 6) so that instruction
counts and times come from one core type. Results go to <out-dir>/*.tsv and the
diagnostics to <out-dir>/diag-*.txt.
"""

import os
import subprocess
import sys
import time
from pathlib import Path

FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", "-fno-diagnostics-color",
         "-O0"]
COMPILERS = {"gcc": "g++", "clang": "clang++"}
PREAMBLE = "#include <galoot/core/metric.hpp>\nnamespace core = galoot::core;\n"

ALGEBRAS = {
    "rigid3": "core::rigid<3>()",
    "conformal3": "core::conformal<3>()",
    "r16": "core::R<16>(16, 0, 0)",
}


def names_unit():
    lines = [PREAMBLE,
             "template <class Alg> requires core::metric_like<Alg>",
             "int probe_tag() { return Alg::value.gram[0][0]; }",
             "template <auto M>",
             "int probe_value() { return M.gram[0][0]; }"]
    for name, factory in ALGEBRAS.items():
        lines.append(f"struct {name} {{ static constexpr auto value = "
                     f"{factory}; }};")
    calls = []
    for name, factory in ALGEBRAS.items():
        calls.append(f"probe_tag<{name}>()")
        calls.append(f"probe_value<{factory}>()")
    lines.append("int main() { return " + " + ".join(calls) + "; }")
    return "\n".join(lines) + "\n"


def diag_unit(spelling):
    lines = [PREAMBLE]
    if spelling == "tag":
        lines += [
            "struct cga3 { static constexpr auto value = "
            "core::conformal<3>(); };",
            "struct r410 { static constexpr auto value = "
            "core::R<5>(4, 1, 0); };",
            "template <class Alg> struct vec { int c; };",
            "template <class Alg> int same(vec<Alg> a, vec<Alg> b) "
            "{ return a.c + b.c; }",
            "int main() { return same(vec<cga3>{1}, vec<r410>{2}); }"]
    else:
        lines += [
            "template <auto M> struct vec { int c; };",
            "template <auto M> int same(vec<M> a, vec<M> b) "
            "{ return a.c + b.c; }",
            "int main() { return same(vec<core::conformal<3>()>{1}, "
            "vec<core::R<5>(4, 1, 0)>{2}); }"]
    return "\n".join(lines) + "\n"


def splits(count):
    out = []
    for p in range(17):
        for q in range(17 - p):
            out.append((p, q, 16 - p - q))
    return out[:count]


def cost_unit(spelling, count):
    lines = [PREAMBLE]
    if spelling in ("tag", "unconstrained_tag"):
        lines.append("template <class Alg>")
        if spelling == "tag":
            lines.append("requires core::metric_like<Alg>")
        lines.append("int probe() { return Alg::value.gram[15][15]; }")
        for k, (p, q, r) in enumerate(splits(count)):
            lines.append(f"struct alg{k} {{ static constexpr auto value = "
                         f"core::R<16>({p}, {q}, {r}); }};")
        uses = [f"probe<alg{k}>()" for k in range(count)]
    else:
        lines += ["template <core::metric<16> M>",
                  "int probe() { return M.gram[15][15]; }"]
        uses = [f"probe<core::R<16>({p}, {q}, {r})>()"
                for (p, q, r) in splits(count)]
    lines.append("int main() { return 0" +
                 "".join(f" + {u}" for u in uses) + "; }")
    return "\n".join(lines) + "\n"


def reuse_unit(constrained, templates):
    lines = [PREAMBLE,
             "struct alg { static constexpr auto value = "
             "core::R<16>(15, 0, 1); };"]
    for t in range(templates):
        lines.append("template <class Alg>")
        if constrained:
            lines.append("requires core::metric_like<Alg>")
        lines.append(f"int probe{t}() {{ return Alg::value.gram[{t % 16}][0]; }}")
    lines.append("int main() { return 0" +
                 "".join(f" + probe{t}<alg>()" for t in range(templates)) +
                 "; }")
    return "\n".join(lines) + "\n"


def compile_unit(compiler, source, obj, include, cpu):
    """Compile once under perf stat; return (exit, output, wall, kib, inst)."""
    perf_out = obj.with_suffix(".perf")
    log = obj.with_suffix(".log")
    cmd = ["taskset", "-c", str(cpu), "perf", "stat", "-x,", "-o",
           str(perf_out), "-e", "instructions:u", "--", compiler, *FLAGS,
           "-I", str(include), "-c", "-o", str(obj), str(source)]
    start = time.monotonic()
    with open(log, "w", encoding="utf-8") as sink:
        child = subprocess.Popen(cmd, stdout=sink, stderr=subprocess.STDOUT)
        # wait4 gives the peak resident set of this compile alone: perf waits
        # for the compiler, so the compiler's peak is part of the child's.
        _, status, usage = os.wait4(child.pid, 0)
    wall = time.monotonic() - start
    exit_code = os.waitstatus_to_exitcode(status)
    # A hybrid processor reports one row per core type (cpu_core, cpu_atom);
    # the one that did not run the compile reads <not counted>.
    counts = [int(line.split(",")[0])
              for line in perf_out.read_text(encoding="utf-8").splitlines()
              if "instructions" in line and line.split(",")[0].isdigit()]
    inst = sum(counts) if counts else None
    if inst is None:
        sys.exit(f"perf reported no instruction count in {perf_out}")
    return (exit_code, log.read_text(encoding="utf-8"), wall, usage.ru_maxrss,
            inst)


def symbols(obj, pattern):
    listed = subprocess.run(["nm", "--defined-only", str(obj)],
                            capture_output=True, text=True, check=True)
    mangled = [line.split()[-1] for line in listed.stdout.splitlines()
               if pattern in line]
    demangled = subprocess.run(["llvm-cxxfilt", *mangled], capture_output=True,
                               text=True, check=True).stdout.splitlines()
    return list(zip(mangled, demangled))


def main():
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    root = Path(sys.argv[1]).resolve()
    out = Path(sys.argv[2]).resolve()
    cpu = int(sys.argv[3]) if len(sys.argv) == 4 else 6
    include = root / "include"
    gen = out / "gen"
    gen.mkdir(parents=True, exist_ok=True)

    with open(out / "names.tsv", "w", encoding="utf-8") as names:
        names.write("compiler\tspelling\tmangled_len\tdemangled_len\t"
                    "demangled\n")
        src = gen / "names.cpp"
        src.write_text(names_unit(), encoding="utf-8")
        for tag, compiler in COMPILERS.items():
            obj = gen / f"names-{tag}.o"
            status, text, *_ = compile_unit(compiler, src, obj, include, cpu)
            if status != 0:
                sys.exit(f"names unit failed on {compiler}:\n{text}")
            for mangled, plain in symbols(obj, "probe_"):
                spelling = "tag" if "probe_tag" in plain else "value"
                names.write(f"{tag}\t{spelling}\t{len(mangled)}\t"
                            f"{len(plain)}\t{plain}\n")

    # conformal(3)'s rows 4 and 5 are (0, 0, 0, 0, -1) and (0, 0, 0, -1, 0).
    # A diagnostic that drops their zeros prints both as {-1}, the rows of a
    # diagonal form.
    with open(out / "diag.tsv", "w", encoding="utf-8") as diag:
        diag.write("compiler\tspelling\texit\tlines\tconflict_line\n")
        for spelling in ("tag", "value"):
            src = gen / f"diag-{spelling}.cpp"
            src.write_text(diag_unit(spelling), encoding="utf-8")
            for tag, compiler in COMPILERS.items():
                obj = gen / f"diag-{spelling}-{tag}.o"
                status, text, *_ = compile_unit(compiler, src, obj, include,
                                                cpu)
                text = text.replace(str(root), "<root>").replace(str(gen),
                                                                 "<gen>")
                (out / f"diag-{spelling}-{tag}.txt").write_text(
                    text, encoding="utf-8")
                conflict = next((line.strip() for line in text.splitlines()
                                 if "conflicting" in line), "-")
                diag.write(f"{tag}\t{spelling}\t{status}\t"
                           f"{len(text.splitlines())}\t{conflict}\n")

    with open(out / "cost.tsv", "w", encoding="utf-8") as cost:
        cost.write("compiler\tspelling\tsignatures\texit\twall_s\t"
                   "peak_kib\tinstructions\n")
        for count in (0, 1, 16, 64, 128):
            for spelling in ("tag", "unconstrained_tag", "value"):
                src = gen / f"cost-{spelling}-{count}.cpp"
                src.write_text(cost_unit(spelling, count), encoding="utf-8")
                for tag, compiler in COMPILERS.items():
                    obj = gen / f"cost-{spelling}-{count}-{tag}.o"
                    status, text, wall, peak, inst = compile_unit(
                        compiler, src, obj, include, cpu)
                    if status != 0:
                        sys.exit(f"cost unit failed on {compiler}:\n{text}")
                    cost.write(f"{tag}\t{spelling}\t{count}\t{status}\t"
                               f"{wall:.3f}\t{peak}\t{inst}\n")

    with open(out / "reuse.tsv", "w", encoding="utf-8") as reuse:
        reuse.write("compiler\tconstrained\ttemplates\texit\twall_s\t"
                    "peak_kib\tinstructions\n")
        for templates in (1, 64):
            for constrained in (True, False):
                src = gen / f"reuse-{constrained}-{templates}.cpp"
                src.write_text(reuse_unit(constrained, templates),
                               encoding="utf-8")
                for tag, compiler in COMPILERS.items():
                    obj = gen / f"reuse-{constrained}-{templates}-{tag}.o"
                    status, text, wall, peak, inst = compile_unit(
                        compiler, src, obj, include, cpu)
                    if status != 0:
                        sys.exit(f"reuse unit failed on {compiler}:\n{text}")
                    reuse.write(f"{tag}\t{constrained}\t{templates}\t"
                                f"{status}\t{wall:.3f}\t{peak}\t{inst}\n")


if __name__ == "__main__":
    main()
