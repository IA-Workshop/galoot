"""tools.measure_header: concurrent first writers of a record write one
header (W5 review of WU1, finding B1).

Usage: python3 -I measure_header_test.py <tools/measure> <scratch directory>

Holds an exclusive lock on an empty record file while several launchers
(`tools/measure tu ... -- true`) open it and wait for the lock, then releases
it. Each launcher opened the file while it was empty, so a launcher that
decides whether to write the header from its position at open writes one
header each. Exits 1 unless the file then holds exactly one header line and
one row per launcher.
"""

import fcntl
import os
import subprocess
import sys
import time

LAUNCHERS = 4
WAIT_S = 30  # how long the launchers may take to queue on the lock


def waiting_on(path):
    """Counts processes blocked on a lock of path, from /proc/locks."""
    inode = os.stat(path).st_ino
    with open("/proc/locks", encoding="ascii") as f:
        return sum(1 for line in f
                   if "->" in line and line.split()[-3].endswith(f":{inode}"))


def main(measure, scratch):
    os.makedirs(scratch, exist_ok=True)
    record = os.path.join(scratch, "header-race.tsv")
    if os.path.exists(record):
        os.unlink(record)
    open(record, "w", encoding="utf-8").close()

    with open(record, "a", encoding="utf-8") as held:
        fcntl.flock(held, fcntl.LOCK_EX)
        launchers = [subprocess.Popen(
            [measure, "tu", "header-race", "none", record, "--", "true"])
            for _ in range(LAUNCHERS)]
        deadline = time.monotonic() + WAIT_S
        while waiting_on(record) < LAUNCHERS:
            if time.monotonic() > deadline:
                sys.exit(f"only {waiting_on(record)} of {LAUNCHERS} "
                         "launchers reached the lock")
            time.sleep(0.01)
        fcntl.flock(held, fcntl.LOCK_UN)
    for launcher in launchers:
        launcher.wait()

    lines = open(record, encoding="utf-8").read().splitlines()
    headers = sum(1 for line in lines if line.startswith("utc_start\t"))
    rows = len(lines) - headers
    print(f"{headers} header lines, {rows} rows, from {LAUNCHERS} launchers")
    return 0 if headers == 1 and rows == LAUNCHERS and lines[0].startswith(
        "utc_start\t") else 1


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("usage: measure_header_test.py <tools/measure> <scratch>")
    sys.exit(main(*sys.argv[1:]))
