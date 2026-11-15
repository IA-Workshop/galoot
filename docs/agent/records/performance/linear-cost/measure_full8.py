#!/usr/bin/env -S python3 -I
"""Measure what the sum of two full<8> multivectors adds to linear_test.cpp.

Usage, from the repository root:
  measure_full8.py <out-dir> [repeats] [cpu]

Compiles three versions of the checkout's test/core/linear_test.cpp, pinned
to <cpu> through tools/timed as measure.py does, with the same flags:

  whole      the file as committed;
  types      the file with the run-time part of the full<8> test case
             removed (the sum, the difference, and the loops that check
             them), so that only the type assertions of full<8> remain;
  no_full8   the file with every line of that test case that names F8
             removed as well, and the algebra tag of eight generators.

The checkout must have no uncommitted changes under include/ or test/.
Writes <out-dir>/rows.tsv and <out-dir>/summary.txt.
"""

import pathlib
import statistics
import subprocess
import sys

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True).stdout.strip())
HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import measure  # noqa: E402  (the pinned compile of measure.py)

FIRST_RUNTIME = "  const volatile std::int64_t one = 1;\n  F8 a{};\n"
R8_TAG = ("struct r8 {\n  static constexpr auto value = core::R<8, 0, 0>();\n"
          "};\n")
END_OF_CASE = "    CHECK(d.coefficients.at(k) == -static_cast<std::int64_t>(k));\n  }\n"


def variants(source):
    whole = source.read_text()
    start = whole.index(FIRST_RUNTIME)
    end = whole.index(END_OF_CASE) + len(END_OF_CASE)
    types = whole[:start] + whole[end:]
    no_full8 = "".join(line for line in types.splitlines(keepends=True)
                       if "F8" not in line)
    # The algebra tag of eight generators is then unused.
    no_full8 = no_full8.replace(R8_TAG, "")
    return {"whole": whole, "types": types, "no_full8": no_full8}


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    out = pathlib.Path(sys.argv[1]).resolve()
    repeats = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    cpu = sys.argv[3] if len(sys.argv) > 3 else "4"
    dirty = subprocess.run(["git", "status", "--porcelain", "include", "test"],
                           cwd=ROOT, capture_output=True, text=True).stdout
    if dirty.strip():
        sys.exit("include/ or test/ has uncommitted changes")
    units = out / "units"
    units.mkdir(parents=True, exist_ok=True)
    objects = out / "objects"
    objects.mkdir(exist_ok=True)
    sources = {}
    for name, text in variants(ROOT / "test/core/linear_test.cpp").items():
        path = units / f"linear_test_{name}.cpp"
        path.write_text(text)
        sources[name] = path

    rows = []
    for repeat in range(repeats):
        for compiler in measure.COMPILERS:
            for name, path in sources.items():
                selection = f"linear-cost-full8:{compiler}:{name}"
                row = measure.compile_pinned(selection, compiler, path, ROOT,
                                             cpu, objects)
                row.update(compiler=compiler, name=name, repeat=repeat)
                rows.append(row)
                print(f"{selection}\t{row['inst']}\t{row['wall']}", flush=True)

    keys = ["compiler", "name", "repeat", "inst", "wall", "rss", "sibling",
            "other"]
    with open(out / "rows.tsv", "w", encoding="utf-8") as f:
        f.write("\t".join(keys) + "\n")
        for row in rows:
            f.write("\t".join(str(row[k]) for k in keys) + "\n")

    head = subprocess.run(["git", "rev-parse", "--short=12", "HEAD"], cwd=ROOT,
                          capture_output=True, text=True).stdout.strip()
    lines = [f"at {head}; {repeats} repeats pinned to CPU {cpu}; "
             "instructions retired on that core, mean and spread in M; wall "
             "time mean in s; peak memory max in KiB"]
    for compiler in measure.COMPILERS:
        for name in sources:
            sel = [r for r in rows if r["compiler"] == compiler
                   and r["name"] == name]
            inst = [r["inst"] / 1e6 for r in sel]
            lines.append(f"{compiler} {name}: {statistics.mean(inst):.1f} M "
                         f"(spread {max(inst) - min(inst):.1f}), "
                         f"{statistics.mean(r['wall'] for r in sel):.3f} s, "
                         f"{max(r['rss'] for r in sel)} KiB")
    (out / "summary.txt").write_text("\n".join(lines) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()
