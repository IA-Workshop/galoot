"""Spike A measurement grid: build cost of the full gp plan.

Each point compiles plan_tu.cpp once, serially, pinned to one performance
core (PERFORMANCE P14), under tools/timed (P13), and appends a row to
out/<set>.tsv. Run from anywhere:

  python3 -I docs/agent/records/spikes/spike-a/run_grid.py SET [SET ...]

Sets:
  main    euclidean and conformal, n = 4..8, g++ and clang++, -O0 and -O2
  main2   the same points again, a second sample, with every P13 counter
  dense   dense metric, n = 4..6, both compilers, -O0 and -O2
  dense7  dense metric, n = 7, both compilers, -O0
  dense8  dense metric, n = 8, both compilers, -O0
  cap     euclidean and conformal with scratch capacity 4, n = 4..8, -O0
  limits  default limits only, every metric, n = 4..8, -O0, both compilers
  sweep   smallest power-of-two limit that passes, from the default upward:
          clang++ -fconstexpr-steps for n = 6..8 (dense to 6), g++
          -fconstexpr-ops-limit for n = 8 (dense 6), -O0

Limits are raised for every set except `limits`, so that a point measures
cost rather than failing: g++ -fconstexpr-ops-limit=2^40 and clang++
-fconstexpr-steps=0 (no limit). Neither loop nor depth limits are reached.
"""

import os
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[4]
OUT = HERE / "out"
CPU = 6          # performance core 3; its other hardware thread is CPU 7
SIBLING = 7
RAISED = {"g++": ["-fconstexpr-ops-limit=1099511627776"],
          "clang++": ["-fconstexpr-steps=0"]}
PLAN_TERM_BYTES = 24  # sizeof(spike::plan_term) on x86-64

COLUMNS = ["set", "compiler", "opt", "metric", "n", "cap", "limits", "exit",
           "wall_s", "user_s", "sys_s", "maxrss_kib", "instructions",
           "plan_terms", "sibling_busy", "load1", "diagnostic", "cycles",
           "cache_refs", "cache_misses", "l1d_load_misses", "l2_refs",
           "l2_misses", "counter_pct_min", "package_energy_j", "cpu"]
# User-space counters on the performance core (P13, decision 0018). The
# first two use fixed counters; perf scales the others if it multiplexes,
# and counter_pct_min is the smallest share of time any event was counted.
EVENTS = {"instructions": "instructions:u", "cycles": "cycles:u",
          "cache_refs": "cache-references:u", "cache_misses": "cache-misses:u",
          "l1d_load_misses": "L1-dcache-load-misses:u",
          "l2_refs": "cpu_core/l2_rqsts.references/u",
          "l2_misses": "cpu_core/l2_rqsts.miss/u"}
RAPL = Path("/sys/class/powercap/intel-rapl:0/energy_uj")


def energy_uj():
    try:
        return int(RAPL.read_text())
    except OSError:
        return None


def cpu_times(cpu):
    with open("/proc/stat") as f:
        for line in f:
            if line.startswith(f"cpu{cpu} "):
                v = [int(x) for x in line.split()[1:]]
                idle = v[3] + v[4]
                return sum(v), idle
    raise RuntimeError("cpu not found")


def plan_terms(obj):
    """Size of the largest object symbol divided by sizeof(plan_term)."""
    out = subprocess.run(["nm", "-S", "--size-sort", str(obj)],
                         capture_output=True, text=True).stdout.split("\n")
    sizes = [int(ln.split()[1], 16) for ln in out if len(ln.split()) == 4]
    return max(sizes) // PLAN_TERM_BYTES if sizes else ""


def point(set_name, compiler, opt, metric, n, cap=None, limits="raised"):
    obj = OUT / f"{set_name}-{compiler}-{opt}-{metric}-{n}.o"
    perf = OUT / "perf.txt"
    flags = ["-std=c++23", f"-{opt}", "-c", f"-DSPIKE_N={n}",
             f"-DSPIKE_METRIC={metric}", "-o", str(obj), str(HERE / "plan_tu.cpp")]
    if cap is not None:
        flags.append(f"-DSPIKE_CAP={cap}")
    if limits == "raised":
        flags = RAISED[compiler] + flags
    elif limits.startswith("-f"):
        flags = [limits] + flags
    selection = f"spike-a:{set_name}:{compiler}:{opt}:{metric}:n{n}"
    cmd = [str(ROOT / "tools/timed"), "build", "-", selection, "--",
           "taskset", "-c", str(CPU), sys.executable, "-I",
           str(HERE / "measure.py"), "--", "perf", "stat", "-x,", "-o",
           str(perf), "-e", ",".join(EVENTS.values()), "--", compiler, *flags]

    load1 = os.getloadavg()[0]
    t0, i0 = cpu_times(SIBLING)
    e0 = energy_uj()
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    e1 = energy_uj()
    t1, i1 = cpu_times(SIBLING)
    # The counter wraps at max_energy_range_uj; a run is far shorter.
    energy = f"{(e1 - e0) / 1e6:.1f}" if e0 is not None and e1 >= e0 \
        else "unavailable"
    busy = 1 - (i1 - i0) / (t1 - t0) if t1 > t0 else 0.0

    m = next(ln for ln in r.stderr.splitlines() if ln.startswith("measure\t"))
    _, wall, user, sys_s, rss, status, _ = m.split("\t")
    counts = {k: "unavailable" for k in EVENTS}
    pct = []
    for ln in perf.read_text().splitlines():
        f = ln.split(",")
        if len(f) < 5 or "cpu_core" not in f[2]:
            continue
        for k, ev in EVENTS.items():
            name = ev.split(":")[0].split("/")[-2] if "/" in ev else \
                ev.split(":")[0]
            if f"cpu_core/{name}/" in f[2]:
                if f[0][:1].isdigit():
                    counts[k] = f[0]
                    pct.append(float(f[4] or 0))
    instr = counts["instructions"]
    diag = ""
    for ln in r.stderr.splitlines():
        if "limit" in ln or "error:" in ln:
            diag = ln.strip()[:160]
            break
    terms = plan_terms(obj) if status == "0" else ""
    if obj.exists():
        obj.unlink()
    row = [set_name, compiler, opt, metric, n, cap or "2^n", limits, status,
           wall, user, sys_s, rss, instr, terms, f"{busy:.2f}", f"{load1:.2f}",
           diag, counts["cycles"], counts["cache_refs"], counts["cache_misses"],
           counts["l1d_load_misses"], counts["l2_refs"], counts["l2_misses"],
           f"{min(pct):.0f}" if pct else "unavailable", energy, CPU]
    return [str(x) for x in row]


def points(set_name):
    both = ("g++", "clang++")
    if set_name in ("main", "main2"):
        for metric in ("euclidean", "conformal"):
            for n in range(4, 9):
                for c in both:
                    for opt in ("O0", "O2"):
                        yield dict(compiler=c, opt=opt, metric=metric, n=n)
    elif set_name == "dense":
        for n in range(4, 7):
            for c in both:
                for opt in ("O0", "O2"):
                    yield dict(compiler=c, opt=opt, metric="dense", n=n)
    elif set_name in ("dense7", "dense8"):
        for c in both:
            yield dict(compiler=c, opt="O0", metric="dense", n=int(set_name[-1]))
    elif set_name == "cap":
        for metric in ("euclidean", "conformal"):
            for n in range(4, 9):
                for c in both:
                    yield dict(compiler=c, opt="O0", metric=metric, n=n, cap=4)
    elif set_name == "limits":
        for metric in ("euclidean", "conformal", "dense"):
            for n in range(4, 9):
                for c in both:
                    yield dict(compiler=c, opt="O0", metric=metric, n=n,
                               limits="default")
    elif set_name == "sweep":
        yield from ()  # handled by sweep() below
    else:
        raise SystemExit(f"unknown set {set_name}")


SWEEP = [("clang++", m, n) for m in ("euclidean", "conformal") for n in (6, 7, 8)]
SWEEP += [("clang++", "dense", n) for n in (5, 6)]
SWEEP += [("g++", m, 8) for m in ("euclidean", "conformal")]
SWEEP += [("g++", "dense", 6)]
FLAG = {"clang++": ("-fconstexpr-steps=", 20), "g++": ("-fconstexpr-ops-limit=", 25)}


def sweep(tsv):
    for compiler, metric, n in SWEEP:
        flag, k = FLAG[compiler]
        while True:
            row = point("sweep", compiler, "O0", metric, n,
                        limits=f"{flag}{2 ** k}")
            with open(tsv, "a") as f:
                f.write("\t".join(row) + "\n")
            print("\t".join(row[:13]), flush=True)
            if row[7] == "0":
                break
            k += 1


def main():
    OUT.mkdir(exist_ok=True)
    for set_name in sys.argv[1:]:
        tsv = OUT / f"{set_name}.tsv"
        with open(tsv, "w") as f:
            f.write("\t".join(COLUMNS) + "\n")
        if set_name == "sweep":
            sweep(tsv)
        for p in points(set_name):
            row = point(set_name, **p)
            with open(tsv, "a") as f:
                f.write("\t".join(row) + "\n")
            print("\t".join(row[:13]), flush=True)
        time.sleep(0)


if __name__ == "__main__":
    main()
