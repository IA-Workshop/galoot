"""Run one command and print its wall time, CPU time, and peak memory.

GNU time is not installed on the development machine (research note 0002,
open question 5), so this stands in for `/usr/bin/time -v`. Peak memory is
the largest resident set of any process the command waited for, from
getrusage(RUSAGE_CHILDREN); for a compiler driver that is cc1plus or clang.
The command's exit status is passed through, and one tab-separated line is
printed to stderr:

  measure<TAB>wall_s<TAB>user_s<TAB>sys_s<TAB>maxrss_kib<TAB>exit<TAB>cpus

Usage: python3 -I measure.py -- command [args...]
"""

import os
import resource
import subprocess
import sys
import time

if len(sys.argv) < 3 or sys.argv[1] != "--":
    sys.exit("usage: measure.py -- command [args...]")

start = time.perf_counter()
status = subprocess.call(sys.argv[2:])
wall = time.perf_counter() - start
ru = resource.getrusage(resource.RUSAGE_CHILDREN)
cpus = ",".join(str(c) for c in sorted(os.sched_getaffinity(0)))
print(f"measure\t{wall:.3f}\t{ru.ru_utime:.3f}\t{ru.ru_stime:.3f}\t"
      f"{ru.ru_maxrss}\t{status}\t{cpus}", file=sys.stderr)
sys.exit(status)
