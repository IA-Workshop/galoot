"""Checks that the emulation of form A and of compose in drift.py gives the
library's results bit for bit, in double and in float, and that the counts
of roundings of the coefficients transform drops in checking mode equal the
library's.

Build the probe (from a checkout root; the command of the README), then:

  <env>/bin/python -B docs/agent/records/spikes/spike-e/bounds/replay.py <probe>

It writes replay.txt next to itself and exits 1 on a difference. A zero of
the other sign counts as a difference of bits and is listed apart.
"""

import os
import random
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import drift              # noqa: E402
import fp                 # noqa: E402

SEED = 20261111
CASES = 300  # per kind of operand and scalar


def bits(x, scalar):
    if scalar == "float":
        return struct.pack("<f", x)
    return struct.pack("<d", x)


def main():
    probe = sys.argv[1]
    rng = random.Random(SEED)
    lines, expect = [], []
    for scalar in ("double", "float"):
        tag = "d" if scalar == "double" else "f"
        p = drift.PRECISION[scalar]
        rnd = fp.to_float32 if scalar == "float" else None
        for i in range(CASES):
            n = (1, 10, 100)[i % 3]
            q, _, _ = drift.chain(scalar, n, rng)
            q1 = [drift.round_to(c, p) for c in drift.unit_motor(rng)]
            lines.append(" ".join(["compose", tag] +
                                  [x.hex() for x in q + q1]))
            expect.append((scalar, "compose", fp.compose(q, q1, rnd)))
            for name, layout, suf in fp.OBJECTS:
                x = [float(c) for c in drift.draw_object(rng, name)]
                if scalar == "float":
                    x = [fp.to_float32(c) for c in x]
                y = drift.PROGRAMS[(name, "A")](scalar, q, x)
                lines.append(" ".join([name, tag] + [c.hex() for c in q + x]))
                expect.append((scalar, name, y))
    run = subprocess.run([probe], input="\n".join(lines) + "\n",
                         capture_output=True, text=True, check=True)
    out = run.stdout.splitlines()
    report = []
    ok = True
    want = {}
    for name, layout, suf in fp.OBJECTS:
        want[name] = fp.dropped_roundings(layout)[0]
    for row in out[:3]:
        words = row.split()
        got = [int(w) for w in words[2:]]
        same = got == want[words[1]]
        ok &= same
        report.append(f"{'PASS' if same else 'FAIL'} dropped K of "
                      f"{words[1]}: library {got}, script {want[words[1]]}")
    diff = {}
    zero_sign = {}
    total = {}
    for (scalar, kind, y), row in zip(expect, out[3:]):
        lib = [float.fromhex(w) for w in row.split()]
        key = (scalar, kind)
        total[key] = total.get(key, 0) + len(y)
        for a, b in zip(y, lib):
            if bits(a, scalar) != bits(b, scalar):
                if a == b:
                    zero_sign[key] = zero_sign.get(key, 0) + 1
                else:
                    diff[key] = diff.get(key, 0) + 1
    if len(out) - 3 != len(expect):
        ok = False
        report.append(f"FAIL the probe printed {len(out) - 3} results for "
                      f"{len(expect)} cases")
    for key in sorted(total):
        d = diff.get(key, 0)
        z = zero_sign.get(key, 0)
        ok &= d == 0 and z == 0
        report.append(f"{'PASS' if d == 0 and z == 0 else 'FAIL'} "
                      f"{key[0]} {key[1]}: {total[key]} coefficients, "
                      f"{d} differ in value, {z} zeros differ in sign")
    with open(os.path.join(HERE, "replay.txt"), "w") as f:
        f.write("\n".join(report) + "\n")
    print("\n".join(report))
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
