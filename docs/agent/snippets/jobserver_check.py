"""Check how Ninja uses a FIFO jobserver when its build is killed or the
pool is restarted or missing. Snippet.

Each case builds a scratch build.ninja of jobs that only sleep, against a
private pool: a FIFO in a temporary directory, held open by this script and
filled with TOKENS tokens, the same mechanism as tools/jobpool. The shared
pool is never opened. The jobs log their start and end times, from which the
most jobs that ran at once is computed. A Ninja client runs one job without
a token, so a pool of TOKENS allows TOKENS + 1 jobs per client.

Cases:
  kill       SIGKILL to Ninja while it holds every token; its jobs finish.
  interrupt  SIGINT to Ninja's process group, as Ctrl-C sends it.
  terminate  SIGTERM to Ninja alone.
  restart    the pool is replaced while a build runs, as `tools/jobpool stop
             --force` and `start` would, and a second build starts on the
             new pool.
  missing    Ninja starts with MAKEFLAGS naming a FIFO that does not exist.
For each it prints the tokens back in the pool afterwards, the most jobs at
once, and whether Ninja held the FIFO open while it ran.

Usage: python3 -I docs/agent/snippets/jobserver_check.py [case...]

Written to check token loss and pool restarts with the platform's Ninja,
and to recheck them when Ninja changes. Last checked on 2026-11-12 at
7276f82 (feature/release-1.0.0, the code of the release commit 7580e88),
with Ninja 1.13.2 and tools/jobpool of 053179d: the same results as on
2026-11-10 at 6a3cf13 and as the first check, on 2026-10-31 at b28cc34.
"""

import fcntl
import os
import signal
import struct
import subprocess
import sys
import tempfile
import termios
import time

TOKENS = 3
JOBS = 8
JOB_S = 2.0
NINJA_TIMEOUT_S = 60


def free_tokens(fd):
    return struct.unpack("i", fcntl.ioctl(fd, termios.FIONREAD,
                                          b"\0" * 4))[0]


def make_pool(path):
    """Creates the FIFO and returns the holder's descriptor."""
    os.mkfifo(path, 0o600)
    fd = os.open(path, os.O_RDWR | os.O_NONBLOCK)
    os.write(fd, b"+" * TOKENS)
    return fd


def write_build(directory, log, jobs=JOBS, seconds=JOB_S):
    os.makedirs(directory, exist_ok=True)
    lines = ["rule job",
             f"  command = sh -c 'echo s $$(date +%s.%N) >> {log}; "
             f"sleep {seconds}; echo e $$(date +%s.%N) >> {log}; touch $out'",
             ""]
    lines += [f"build out{i}: job" for i in range(jobs)]
    with open(os.path.join(directory, "build.ninja"), "w",
              encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def start_ninja(directory, fifo):
    env = dict(os.environ,
               MAKEFLAGS=f"-j{TOKENS + 1} --jobserver-auth=fifo:{fifo}")
    return subprocess.Popen(["ninja", "-C", directory], env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, start_new_session=True)


def holds_fifo(pid, fifo):
    try:
        fds = os.listdir(f"/proc/{pid}/fd")
    except OSError:
        return False
    for fd in fds:
        try:
            if os.readlink(f"/proc/{pid}/fd/{fd}").startswith(fifo):
                return True
        except OSError:
            continue
    return False


def most_at_once(*logs):
    events = []
    for log in logs:
        if not os.path.exists(log):
            continue
        with open(log, encoding="ascii") as f:
            for line in f:
                kind, stamp = line.split()
                events.append((float(stamp), 1 if kind == "s" else -1))
    running = peak = 0
    for _, step in sorted(events):
        running += step
        peak = max(peak, running)
    return peak


def wait_until(condition, timeout_s=10.0):
    deadline = time.monotonic() + timeout_s
    while not condition():
        if time.monotonic() > deadline:
            return False
        time.sleep(0.02)
    return True


def started(log):
    if not os.path.exists(log):
        return 0
    with open(log, encoding="ascii") as f:
        return sum(1 for line in f if line.startswith("s "))


def signal_case(tmp, name, send):
    fifo = os.path.join(tmp, f"{name}.fifo")
    holder = make_pool(fifo)
    build, log = os.path.join(tmp, name), os.path.join(tmp, f"{name}.log")
    write_build(build, log)
    ninja = start_ninja(build, fifo)
    ok = wait_until(lambda: started(log) == TOKENS + 1
                    and free_tokens(holder) == 0)
    held = holds_fifo(ninja.pid, fifo)
    send(ninja)
    ninja.wait(NINJA_TIMEOUT_S)
    # Orphaned jobs of a killed Ninja keep running; wait for them.
    time.sleep(JOB_S + 0.5)
    print(f"{name}: setup {'ok' if ok else 'timed out'}, Ninja held the FIFO: "
          f"{held}, exit {ninja.returncode}, tokens back {free_tokens(holder)}"
          f" of {TOKENS}, most jobs at once {most_at_once(log)}")
    os.close(holder)


def restart_case(tmp):
    fifo = os.path.join(tmp, "restart.fifo")
    holder = make_pool(fifo)
    first, log1 = os.path.join(tmp, "first"), os.path.join(tmp, "first.log")
    second, log2 = os.path.join(tmp, "second"), os.path.join(tmp,
                                                             "second.log")
    write_build(first, log1)
    write_build(second, log2)
    ninja1 = start_ninja(first, fifo)
    wait_until(lambda: started(log1) == TOKENS + 1)

    # What stop --force and start do: the holder exits, the path is removed,
    # and a new FIFO with fresh tokens takes its place.
    os.close(holder)
    os.unlink(fifo)
    holder = make_pool(fifo)
    ninja2 = start_ninja(second, fifo)
    ninja1.wait(NINJA_TIMEOUT_S)
    ninja2.wait(NINJA_TIMEOUT_S)
    print(f"restart: exits {ninja1.returncode} and {ninja2.returncode}, most "
          f"jobs at once across both builds {most_at_once(log1, log2)} "
          f"(one pool allows {TOKENS + 2} for two clients), tokens back in "
          f"the new pool {free_tokens(holder)} of {TOKENS}")
    os.close(holder)


def missing_case(tmp):
    build, log = os.path.join(tmp, "missing"), os.path.join(tmp,
                                                            "missing.log")
    write_build(build, log, jobs=2 * os.cpu_count(), seconds=0.5)
    ninja = start_ninja(build, os.path.join(tmp, "no-such.fifo"))
    output, _ = ninja.communicate(timeout=NINJA_TIMEOUT_S)
    warnings = [line for line in output.splitlines()
                if line.startswith("ninja:") and "jobserver" in line.lower()]
    print(f"missing: exit {ninja.returncode}, most jobs at once "
          f"{most_at_once(log)} on {os.cpu_count()} CPUs, Ninja said: "
          f"{warnings or 'nothing about the jobserver'}")


CASES = {
    "kill": lambda tmp: signal_case(tmp, "kill",
                                    lambda p: p.send_signal(signal.SIGKILL)),
    "interrupt": lambda tmp: signal_case(
        tmp, "interrupt", lambda p: os.killpg(p.pid, signal.SIGINT)),
    "terminate": lambda tmp: signal_case(
        tmp, "terminate", lambda p: p.send_signal(signal.SIGTERM)),
    "restart": restart_case,
    "missing": missing_case,
}


def main(names):
    unknown = [n for n in names if n not in CASES]
    if unknown:
        sys.exit(f"unknown case {', '.join(unknown)}; cases: "
                 f"{', '.join(CASES)}")
    with tempfile.TemporaryDirectory(prefix="pool-check-") as tmp:
        for name in names or CASES:
            CASES[name](tmp)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
