"""Measure one compile pinned to one performance core (snippet candidate;
written for Spikes D and F, last checked at aa8051c0).

Runs a command under tools/timed (PERFORMANCE P13), pinned with taskset to one
performance core whose sibling thread is watched (P14), through Spike A's
measure.py (peak memory of children, since GNU time is not installed) and
perf stat (user-space counters on the cpu_core PMU, decision 0018). Reads the
package energy counter before and after; the figure is comparable only on an
otherwise idle machine, which this machine was not (shared, P11).

Import it (`from pinned import measure`) or run it:

  python3 -I pinned.py SELECTION -- compiler args...

and it prints one tab-separated line of the fields below. Spike A's
run_grid.py point() is the same procedure for one fixed command line.
"""

import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(subprocess.run(["git", "-C", str(HERE), "rev-parse",
                            "--show-toplevel"], capture_output=True,
                           text=True, check=True).stdout.strip())
MEASURE = ROOT / "docs/agent/records/spikes/spike-a/measure.py"
CPU = 6          # performance core 3; its other hardware thread is CPU 7
SIBLING = 7
EVENTS = {"instructions": "instructions:u", "cycles": "cycles:u",
          "cache_refs": "cache-references:u", "cache_misses": "cache-misses:u",
          "l1d_load_misses": "L1-dcache-load-misses:u",
          "l2_refs": "cpu_core/l2_rqsts.references/u",
          "l2_misses": "cpu_core/l2_rqsts.miss/u"}
FIELDS = ["exit", "wall_s", "user_s", "sys_s", "maxrss_kib", *EVENTS,
          "counter_pct_min", "sibling_busy", "load1", "package_energy_j",
          "cpu"]
RAPL = Path("/sys/class/powercap/intel-rapl:0/energy_uj")


def _energy_uj():
    try:
        return int(RAPL.read_text())
    except OSError:
        return None


def _cpu_times(cpu):
    with open("/proc/stat") as f:
        for line in f:
            if line.startswith(f"cpu{cpu} "):
                v = [int(x) for x in line.split()[1:]]
                return sum(v), v[3] + v[4]
    raise RuntimeError("cpu not found")


def measure(selection, command, cwd=None, perf_out=None):
    """Run command pinned and measured; return (fields dict, stderr text)."""
    perf_out = perf_out or HERE / "out" / "perf.txt"
    Path(perf_out).parent.mkdir(parents=True, exist_ok=True)
    cmd = [str(ROOT / "tools/timed"), "build", "-", selection, "--",
           "taskset", "-c", str(CPU), sys.executable, "-I", str(MEASURE),
           "--", "perf", "stat", "-x,", "-o", str(perf_out), "-e",
           ",".join(EVENTS.values()), "--", *command]
    load1 = os.getloadavg()[0]
    t0, i0 = _cpu_times(SIBLING)
    e0 = _energy_uj()
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd or ROOT)
    e1 = _energy_uj()
    t1, i1 = _cpu_times(SIBLING)

    m = next(ln for ln in r.stderr.splitlines() if ln.startswith("measure\t"))
    _, wall, user, sys_s, rss, status, _ = m.split("\t")
    counts = {k: "unavailable" for k in EVENTS}
    pct = []
    for ln in Path(perf_out).read_text().splitlines():
        f = ln.split(",")
        if len(f) < 5 or "cpu_core" not in f[2]:
            continue
        for k, ev in EVENTS.items():
            name = ev.split("/")[1] if "/" in ev else ev.split(":")[0]
            if f"cpu_core/{name}/" in f[2] and f[0][:1].isdigit():
                counts[k] = f[0]
                pct.append(float(f[4] or 0))
    out = {"exit": status, "wall_s": wall, "user_s": user, "sys_s": sys_s,
           "maxrss_kib": rss, **counts,
           "counter_pct_min": f"{min(pct):.0f}" if pct else "unavailable",
           "sibling_busy": f"{1 - (i1 - i0) / (t1 - t0):.2f}"
           if t1 > t0 else "0.00",
           "load1": f"{load1:.2f}",
           # The counter wraps at max_energy_range_uj; a run is far shorter.
           "package_energy_j": f"{(e1 - e0) / 1e6:.1f}"
           if e0 is not None and e1 is not None and e1 >= e0
           else "unavailable",
           "cpu": str(CPU)}
    stderr = "\n".join(ln for ln in r.stderr.splitlines()
                       if not ln.startswith(("measure\t", "timed: ")))
    return out, stderr


if __name__ == "__main__":
    if len(sys.argv) < 4 or sys.argv[2] != "--":
        sys.exit("usage: pinned.py SELECTION -- command [args...]")
    fields, err = measure(sys.argv[1], sys.argv[3:], cwd=Path.cwd())
    sys.stderr.write(err + ("\n" if err else ""))
    print("\t".join(FIELDS))
    print("\t".join(fields[k] for k in FIELDS))
