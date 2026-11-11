"""Checks the rigid(3) results of compiled cprobe builds against exact
arithmetic: every kept coefficient within gamma_K P of its exact value, with
K the core's count and P the exact padded magnitude (padding lambda, the
smallest the derivation admits). It also says, for each build, how many
coefficients equal sim.py's emulation under each contraction pattern (off,
current, first), as a description of what the compiler did.

  <env>/bin/python -B verify.py <seed> <cases> <out file>...

Each <out file> is cprobe's standard output for `cprobe <cases> <seed>`.
Only the first <cases> cases of each family are read. The inputs are
regenerated here from the seed (splitmix64, as cprobe draws
them). Writes nothing; prints one line per build and family and exits 1 if
any coefficient is outside its bound.
"""

import os
import sys

from flint import fmpq

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import sim  # noqa: E402

MASK = (1 << 64) - 1


class Source:
    def __init__(self, seed):
        self.state = seed & MASK

    def next(self):
        self.state = (self.state + 0x9E3779B97F4A7C15) & MASK
        z = self.state
        z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & MASK
        z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK
        return z ^ (z >> 31)

    def uniform(self):
        # (k * 2^-53 * 2) - 1, exact in double for k < 2^53
        k = self.next() >> 11
        return fmpq(k, 2**52) - 1


FAMILIES = [("rga3-point", sim.R.POINT), ("rga3-line", sim.R.LINE),
            ("rga3-plane", sim.R.PLANE)]


def main():
    seed = int(sys.argv[1])
    limit = int(sys.argv[2])
    ok = True
    for path in sys.argv[3:]:
        rows = {}
        for line in open(path):
            w = line.split()
            if w[0].startswith("rga3"):
                rows[(w[0], w[1], int(w[2]))] = [
                    fmpq(*float.fromhex(c).as_integer_ratio()) for c in w[3:]]
        for scalar, fmt, base in (("d", "double", seed),
                                  ("f", "float", seed + 100)):
            p, emin = sim.FORMATS[fmt]
            u = sim.pow2(-p)
            lam = sim.pow2(emin)
            for fi, (name, layout) in enumerate(FAMILIES):
                f_all, f_kept, second, dropped, n_mid, n_drop = \
                    sim.kernels(layout)
                n_obj = len(layout)
                k_mid = sim.roundings(f_all, n_mid, [0] * 8, [0] * n_obj)
                k_kept = sim.roundings(second, n_obj, k_mid, [0] * 8)
                src = Source(base + fi)
                n = 0
                bad = 0
                worst = 0.0
                match = {"off": 0, "current": 0, "first": 0}
                i = 0
                while i < limit and (name, scalar, i) in rows:
                    q = [sim.rnd(src.uniform(), fmt) for _ in range(8)]
                    x = [sim.rnd(src.uniform(), fmt) for _ in range(n_obj)]
                    got = rows[(name, scalar, i)]
                    mid_e = sim.exact_kernel(f_all, n_mid, q, x)
                    kept_e = sim.exact_kernel(second, n_obj, mid_e, q)
                    mid_p = sim.exact_kernel(f_all, n_mid, q, x, True, lam)
                    kept_p = sim.exact_kernel(second, n_obj, mid_p, q, True,
                                              lam)
                    for y, e, pp, k in zip(got, kept_e, kept_p, k_kept):
                        n += 1
                        bound = sim.gamma(k, u) * pp
                        err = abs(y - e)
                        if err > bound:
                            bad += 1
                        worst = max(worst, float(err / bound))
                    for pat in match:
                        mid, km = sim.run_kernel(f_kept, n_mid, q, x,
                                                 [0] * 8, [0] * n_obj, fmt,
                                                 pat, None)
                        kept, _ = sim.run_kernel(second, n_obj, mid, q, km,
                                                 [0] * 8, fmt, pat, None)
                        match[pat] += sum(a == b for a, b in zip(kept, got))
                    i += 1
                ok &= bad == 0
                ms = ", ".join(f"{k} {v}" for k, v in match.items())
                print(f"{'PASS' if bad == 0 else 'FAIL'} "
                      f"{os.path.basename(path)} {name} {fmt}: {i} cases, "
                      f"{n} coefficients, {bad} outside gamma_K P, largest "
                      f"|err|/(gamma_K P) {worst:.3f}; equal to the emulation "
                      f"under {ms}")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
