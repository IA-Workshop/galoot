"""Compares the outputs of the cprobe builds bit for bit, and gathers each
build's count of constant-evaluated coefficients that differ from the same
cases at run time.

  python3 compare.py <dir>

<dir> holds <build>.out and <build>.err for each build that build.sh makes.
Prints the comparisons; writes nothing.
"""

import os
import re
import sys

PAIRS = [
    # checked (GALOOT_CHECKING=1) against unchecked, per compiler and mode
    ("gcc-off-c0", "gcc-off-c1"), ("gcc-on-c0", "gcc-on-c1"),
    ("gcc-fast-c0", "gcc-fast-c1"), ("gcc-fastnv-c0", "gcc-fastnv-c1"),
    ("clang-off-c0", "clang-off-c1"), ("clang-on-c0", "clang-on-c1"),
    ("clang-fast-c0", "clang-fast-c1"),
    ("clang-fastnv-c0", "clang-fastnv-c1"),
    # modes against each other
    ("gcc-off-c0", "clang-off-c0"), ("gcc-off-c0", "gcc-on-c0"),
    ("clang-off-c0", "clang-on-c0"), ("gcc-off-c0", "gcc-fast-c0"),
    ("clang-off-c0", "clang-fast-c0"), ("gcc-fast-c0", "clang-fast-c0"),
    ("gcc-fastnv-c0", "gcc-fast-c0"), ("clang-fastnv-c0", "clang-fast-c0"),
]


def load(path):
    rows = {}
    with open(path) as f:
        for line in f:
            w = line.split()
            rows[(w[0], w[1], w[2])] = w[3:]
    return rows


def main():
    d = sys.argv[1]
    cache = {}

    def get(b):
        if b not in cache:
            cache[b] = load(os.path.join(d, b + ".out"))
        return cache[b]

    print("Checked against unchecked, and mode against mode: coefficients "
          "whose bits differ")
    for a, b in PAIRS:
        ra, rb = get(a), get(b)
        assert ra.keys() == rb.keys()
        fam = {}
        for k, va in ra.items():
            t = fam.setdefault((k[0], k[1]), [0, 0])
            for x, y in zip(va, rb[k]):
                t[1] += 1
                t[0] += x != y
        diff = sum(v[0] for v in fam.values())
        total = sum(v[1] for v in fam.values())
        parts = "; ".join(f"{f[0]} {f[1]} {v[0]}/{v[1]}"
                          for f, v in sorted(fam.items()) if v[0])
        print(f"  {a} vs {b}: {diff} of {total}" +
              (f" ({parts})" if parts else ""))
    print()
    print("Constant evaluation against run time (32 cases per family and "
          "scalar): coefficients whose bits differ")
    pat = re.compile(r"constant-vs-runtime (\S+) (\S) (\d+) of (\d+) differ")
    for name in sorted(f[:-4] for f in os.listdir(d) if f.endswith(".err")):
        diff = total = 0
        with open(os.path.join(d, name + ".err")) as f:
            for line in f:
                m = pat.match(line)
                if m:
                    diff += int(m.group(3))
                    total += int(m.group(4))
        print(f"  {name}: {diff} of {total}")


if __name__ == "__main__":
    main()
