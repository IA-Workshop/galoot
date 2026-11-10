"""tools.jobpool: the job pool's argument checks, the slots it writes back
and drains when no client holds it, an idle, full pool that leaves its FIFO
in place, a FIFO path through a symbolic link, a count of clients that
fails, a client that opens the FIFO just after a count, a FIFO deleted by
hand, and the idle stop, on pools of its own.

Usage: python3 -I jobpool_test.py <tools/jobpool> <scratch directory>

Each pool under test is a transient user service with a unit name of its
own and a FIFO in the scratch directory, so the machine's shared pool is
never opened, stopped, or started. The cases with a failing or a slow
count run a copy of the tool in the scratch directory. The test stops its
own units directly with systemctl when it ends. Prints one line per failed check and
exits 1 if there is any, or exits 77 when no user service manager is
available.
"""

import contextlib
import fcntl
import os
import select
import shutil
import struct
import subprocess
import sys
import termios
import time

SKIPPED = 77
SHARED_UNIT = "galoot-jobserver"

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


def wait_for(predicate, seconds=20.0):
    end = time.monotonic() + seconds
    while time.monotonic() < end:
        if predicate():
            return True
        time.sleep(0.1)
    return predicate()


def free(fifo):
    """The tokens in the FIFO, or None when there is no FIFO."""
    try:
        fd = os.open(fifo, os.O_RDWR | os.O_NONBLOCK)
    except FileNotFoundError:
        return None
    try:
        return struct.unpack("i", fcntl.ioctl(fd, termios.FIONREAD,
                                              b"\0" * 4))[0]
    finally:
        os.close(fd)


def take_and_leave(fifo, count):
    """A client that takes tokens and exits without giving them back."""
    fd = os.open(fifo, os.O_RDWR | os.O_NONBLOCK)
    try:
        return len(os.read(fd, count))
    finally:
        os.close(fd)


def give_extra(fifo, count):
    """A client that writes back tokens it never took."""
    fd = os.open(fifo, os.O_RDWR | os.O_NONBLOCK)
    try:
        os.write(fd, b"+" * count)
    finally:
        os.close(fd)


# A client in a process of its own: it opens the path in argv[1] (after
# waiting for it to appear when argv[3] is "wait"), takes argv[2] tokens,
# prints the FIFO's inode, and holds the FIFO until its input closes.
CLIENT = """
import os, sys, time
path, count, wait = sys.argv[1], int(sys.argv[2]), sys.argv[3] == "wait"
while True:
    try:
        fd = os.open(path, os.O_RDWR | os.O_NONBLOCK)
        break
    except FileNotFoundError:
        if not wait:
            raise
        time.sleep(0.005)
taken = len(os.read(fd, count)) if count else 0
print(taken, os.fstat(fd).st_ino, flush=True)
sys.stdin.read()
"""


class Client:
    def __init__(self, path, count, wait=False):
        self.process = subprocess.Popen(
            [sys.executable, "-I", "-c", CLIENT, path, str(count),
             "wait" if wait else "now"],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.pid = self.process.pid
        self.path = path

    def ready(self, seconds=30):
        """(tokens taken, inode of the FIFO it holds), once it holds it."""
        readable, _, _ = select.select([self.process.stdout], [], [], seconds)
        if not readable:
            raise TimeoutError(f"no client holds {self.path} after "
                               f"{seconds} s")
        taken, inode = self.process.stdout.readline().split()
        return int(taken), int(inode)

    def leave(self):
        """Closes the FIFO without giving back what was taken."""
        self.process.stdin.close()
        self.process.wait(timeout=10)

    def kill(self):
        self.process.kill()
        self.process.wait(timeout=10)


class Pool:
    def __init__(self, tool, directory, name, idle_seconds=None, fifo=None):
        self.tool = tool
        self.unit = f"galoot-jobpool-test-{os.getpid()}-{name}"
        self.fifo = fifo or os.path.join(directory, f"{name}.fifo")
        if (self.unit == SHARED_UNIT or self.fifo
                == f"/run/user/{os.getuid()}/jobserver.fifo"):
            raise SystemExit("refusing to test the shared pool")
        self.env = dict(os.environ, JOBPOOL_UNIT=self.unit,
                        JOBPOOL_FIFO=self.fifo, JOBPOOL_POLL_SECONDS="1")
        if idle_seconds is not None:
            self.env["JOBPOOL_IDLE_SECONDS"] = str(idle_seconds)

    def run(self, *args):
        return subprocess.run([self.tool, *args], env=self.env,
                              capture_output=True, text=True, check=False,
                              timeout=60)

    def active(self):
        return subprocess.run(
            ["systemctl", "--user", "is-active", "--quiet",
             f"{self.unit}.service"], check=False).returncode == 0

    def remove(self):
        subprocess.run(["systemctl", "--user", "stop", f"{self.unit}.service"],
                       capture_output=True, check=False)
        subprocess.run(["systemctl", "--user", "reset-failed",
                        f"{self.unit}.service"], capture_output=True,
                       check=False)


def arguments(tool, directory):
    """Each malformed command line is refused with exit 2 and starts
    nothing."""
    pool = Pool(tool, directory, "arguments")
    try:
        for args, message in [
                (["start", "3", "4"], "one slot count"),
                (["start", "--idle"], "--idle takes"),
                (["start", "--slots", "3"], "unknown option --slots"),
                (["start", "0"], "positive integer"),
                (["ensure", "3"], "usage:")]:
            result = pool.run(*args)
            name = " ".join(args)
            check(f"exit of {name}", result.returncode, 2)
            if message not in result.stderr:
                failures.append(f"stderr of {name}: {result.stderr!r}")
            check(f"nothing started by {name}", pool.active(), False)
    finally:
        pool.remove()


def repair(tool, directory):
    """Slots a client took and never gave back are written back, and slots
    it wrote back without taking them are drained, at the next poll that
    finds no client. Never while a client holds the FIFO, since its slots
    in use cannot be told from lost ones. With no idle stop as well."""
    pool = Pool(tool, directory, "repair")
    try:
        result = pool.run("start", "3", "--idle", "0")
        check("start exit", result.returncode, 0)
        check("start report", result.stdout.splitlines()[:1],
              [f"started: 3 slots free at {pool.fifo}"])
        check("env", pool.run("env").stdout,
              f'export MAKEFLAGS="-j4 --jobserver-auth=fifo:{pool.fifo}"\n')
        check("start again", pool.run("start", "3").stdout,
              "already running: 3 of 3 slots free\n")

        give_extra(pool.fifo, 2)
        check("surplus drained", wait_for(lambda: free(pool.fifo) == 3), True)
        # All of them, so that a surplus left undrained cannot hide a
        # missing write-back.
        check("tokens taken and left", take_and_leave(pool.fifo, 3), 3)
        check("lost slots written back",
              wait_for(lambda: free(pool.fifo) == 3), True)

        holder = Client(pool.fifo, 1)
        check("holder took one", holder.ready()[0], 1)
        time.sleep(3.5)
        check("nothing written back while a client holds the pool",
              free(pool.fifo), 2)
        status = pool.run("status")
        check("status exit with a client", status.returncode, 0)
        if f"clients: {holder.pid} (" not in status.stdout:
            failures.append(f"status names the client: {status.stdout!r}")
        check("stop refused while a client holds the pool",
              pool.run("stop").returncode, 1)
        check("still running after the refusal", pool.active(), True)
        holder.kill()
        check("a killed client's slot written back",
              wait_for(lambda: free(pool.fifo) == 3), True)

        result = pool.run("stop")
        check("stop exit", result.returncode, 0)
        check("stopped", wait_for(lambda: not pool.active()), True)
        check("FIFO removed by stop", os.path.exists(pool.fifo), False)
    finally:
        pool.remove()


def idle_full(tool, directory):
    """An idle pool with every slot free has nothing to repair, so it never
    moves its FIFO aside: a build that starts at such a poll finds the pool
    at its path. The path is watched without opening it, across four polls."""
    pool = Pool(tool, directory, "idlefull")
    aside = f"{pool.fifo}.stopping"
    try:
        check("start exit, idle and full",
              pool.run("start", "2", "--idle", "0").returncode, 0)
        missing = moved = 0
        end = time.monotonic() + 4.5
        while time.monotonic() < end:
            missing += not os.path.exists(pool.fifo)
            moved += os.path.exists(aside)
            time.sleep(0.005)
        check("samples with the FIFO missing, idle and full", missing, 0)
        check("samples with the FIFO aside, idle and full", moved, 0)
        check("every slot free, idle and full", free(pool.fifo), 2)
        check("running, idle and full", pool.active(), True)
    finally:
        pool.remove()


def fifo_deleted(tool, directory):
    """A FIFO deleted by hand ends the pool at its next poll with no
    client, so that status says it is not running and ensure starts a new
    one, rather than a service that runs on without a FIFO to serve."""
    pool = Pool(tool, directory, "deleted")
    try:
        check("start exit, FIFO deleted",
              pool.run("start", "2", "--idle", "0").returncode, 0)
        os.unlink(pool.fifo)
        check("service ended after its FIFO was deleted",
              wait_for(lambda: not pool.active(), 10), True)
        status = pool.run("status")
        check("status after the FIFO was deleted",
              (status.returncode, status.stdout), (1, "not running\n"))
        ensure = pool.run("ensure")
        check("ensure exit after the FIFO was deleted", ensure.returncode, 0)
        check("ensure starts a new pool after the FIFO was deleted",
              ensure.stdout.splitlines()[:1],
              [f"started: 23 slots free at {pool.fifo}"])
    finally:
        pool.remove()


def symlinked(tool, directory):
    """A pool whose FIFO path runs through a symbolic link finds its clients
    all the same, so it never writes back the slots a client holds."""
    real = os.path.join(directory, "real")
    link = os.path.join(directory, "link")
    os.makedirs(real)
    os.symlink(real, link)
    pool = Pool(tool, directory, "symlinked",
                fifo=os.path.join(link, "pool.fifo"))
    try:
        check("start exit, symlinked",
              pool.run("start", "3", "--idle", "0").returncode, 0)
        holder = Client(pool.fifo, 3)
        check("holder took all, symlinked", holder.ready()[0], 3)
        time.sleep(3.5)
        check("nothing written back while a client holds a symlinked pool",
              free(pool.fifo), 0)
        if f"clients: {holder.pid} (" not in pool.run("status").stdout:
            failures.append("status names the client of a symlinked pool")
        holder.leave()
        check("lost slots written back, symlinked",
              wait_for(lambda: free(pool.fifo) == 3), True)
    finally:
        pool.remove()


def broken_copy(tool, directory, name, body):
    """A copy of the tool whose count of clients runs body first; body
    fails the count by returning 1."""
    source = open(tool, encoding="utf-8").read()
    anchor = "count_clients() {\n"
    check(f"anchor of the count, {name}", source.count(anchor), 1)
    broken = os.path.join(directory, f"jobpool-{name}")
    with open(broken, "w", encoding="utf-8") as f:
        f.write(source.replace(anchor, f"count_clients() {{ {body}; "
                               "real_count_clients \"$@\"; }\n"
                               "real_count_clients() {\n"))
    os.chmod(broken, 0o755)
    return broken


def count_fails(tool, directory):
    """When the pool cannot count its clients, it acts as if one held the
    FIFO: it writes nothing back and does not stop itself. The case runs a
    copy of the tool whose count always fails."""
    broken = broken_copy(tool, directory, "count-fails", "return 1")
    pool = Pool(broken, directory, "countfails", idle_seconds=2)
    try:
        check("start exit, count fails",
              pool.run("start", "3", "--idle", "1").returncode, 0)
        check("tokens taken and left, count fails",
              take_and_leave(pool.fifo, 2), 2)
        time.sleep(6)
        check("nothing written back when the count fails", free(pool.fifo), 1)
        check("no idle stop when the count fails", pool.active(), True)
    finally:
        pool.remove()


def last_count_fails(tool, directory):
    """When only the last count before an idle stop fails, the one made
    with the FIFO moved aside, the pool puts the FIFO back and keeps
    running, rather than removing it under a client it could not see."""
    broken = broken_copy(tool, directory, "last-count-fails",
                         '[[ $1 == *.stopping ]] && return 1')
    pool = Pool(broken, directory, "lastcountfails", idle_seconds=2)
    try:
        check("start exit, last count fails",
              pool.run("start", "2", "--idle", "1").returncode, 0)
        time.sleep(6)
        check("no idle stop when the last count fails", pool.active(), True)
        check("FIFO back at its path when the last count fails",
              wait_for(lambda: os.path.exists(pool.fifo), 5), True)
    finally:
        pool.remove()


# A client under load, as Ninja and CTest behave: until argv[2] seconds
# have passed it waits a moment, opens the FIFO at argv[1] (skipping the
# turn when the path is missing), takes up to two tokens, holds them
# briefly, gives back exactly what it took, and closes the FIFO. It never
# loses a token.
BUSY_CLIENT = """
import os, random, sys, time
path, end = sys.argv[1], time.monotonic() + float(sys.argv[2])
random.seed(int(sys.argv[3]))
while time.monotonic() < end:
    time.sleep(random.uniform(0.02, 0.3))
    try:
        fd = os.open(path, os.O_RDWR | os.O_NONBLOCK)
    except FileNotFoundError:
        continue
    try:
        taken = os.read(fd, random.randint(1, 2))
    except BlockingIOError:
        taken = b""
    time.sleep(random.uniform(0, 0.15))
    if taken:
        os.write(fd, taken)
    os.close(fd)
"""


def surplus_race(tool, directory):
    """A client that opens the FIFO and takes tokens just after the pool
    has counted no client must keep them: the pool never writes back the
    tokens a live client holds, and never ends up with more free slots
    than its total. The case runs a copy of the tool that, after each count
    of clients, marks the moment with a file and waits half a second. The
    test opens the FIFO in that half second, as a client does by chance on
    a busy machine, in three rounds; then clients that come and go as Ninja
    and CTest do run against the same copy for ten seconds."""
    source = open(tool, encoding="utf-8").read()
    anchor = '  [[ $count == 0 ]]\n}\n'
    check("anchor of the count's result", source.count(anchor), 1)
    slow = os.path.join(directory, "jobpool-slow-count")
    with open(slow, "w", encoding="utf-8") as f:
        f.write(source.replace(anchor, '  touch -- "$1.counted"\n'
                               '  sleep 0.5\n  [[ $count == 0 ]]\n}\n'))
    os.chmod(slow, 0o755)
    pool = Pool(slow, directory, "race")
    slots = 4
    counted = f"{pool.fifo}.counted"
    clients = []
    try:
        check("start exit, race",
              pool.run("start", str(slots), "--idle", "0").returncode, 0)
        for round_ in range(3):
            with contextlib.suppress(FileNotFoundError):
                os.unlink(counted)
            if not wait_for(lambda: os.path.exists(counted), 10):
                failures.append(f"round {round_}: the pool made no count")
                break
            fd = os.open(pool.fifo, os.O_RDWR | os.O_NONBLOCK)
            try:
                taken = os.read(fd, 2)
                check(f"round {round_}: tokens taken", len(taken), 2)
                time.sleep(2.5)
                check(f"round {round_}: nothing written back while the "
                      "late client holds its tokens", free(pool.fifo),
                      slots - 2)
                os.write(fd, taken)
            finally:
                os.close(fd)
            check(f"round {round_}: no surplus once they are given back",
                  free(pool.fifo), slots)

        check("every slot free before the clients start",
              wait_for(lambda: free(pool.fifo) == slots, 10), True)
        clients = [subprocess.Popen([sys.executable, "-I", "-c", BUSY_CLIENT,
                                     pool.fifo, "10", str(seed)])
                   for seed in range(3)]
        most = 0
        while any(c.poll() is None for c in clients):
            found = free(pool.fifo)
            if found is not None:
                most = max(most, found)
            time.sleep(0.02)
        check("most free slots while clients came and went", most <= slots,
              True)
        check("every slot free once the clients are done",
              wait_for(lambda: free(pool.fifo) == slots, 10), True)
    finally:
        for c in clients:
            if c.poll() is None:
                c.kill()
        pool.remove()


def idle_stop(tool, directory):
    """The pool stops itself once no client has held it for the idle time.
    A client that opens the FIFO just as the pool decides to stop keeps the
    pool running at its usual path: the case here opens it the moment the
    pool moves it aside to count its clients a last time."""
    pool = Pool(tool, directory, "idle", idle_seconds=4)
    aside = f"{pool.fifo}.stopping"
    try:
        late = None
        result = pool.run("start", "2", "--idle", "1")
        check("start exit", result.returncode, 0)
        late = Client(aside, 0, wait=True)
        _, inode = late.ready()
        time.sleep(3)
        check("running after a late client", pool.active(), True)
        check("FIFO back at its path",
              os.path.exists(pool.fifo) and os.stat(pool.fifo).st_ino, inode)
        check("nothing left aside", os.path.exists(aside), False)
        late.leave()
        check("stopped after the idle time",
              wait_for(lambda: not pool.active()), True)
        check("FIFO removed by the idle stop", os.path.exists(pool.fifo),
              False)
        check("nothing left aside after the stop", os.path.exists(aside),
              False)
    finally:
        if late is not None and late.process.poll() is None:
            late.kill()
        pool.remove()


def main(tool, directory):
    tool = os.path.realpath(tool)
    directory = os.path.realpath(directory)
    manager = subprocess.run(["systemctl", "--user", "show-environment"],
                             capture_output=True, check=False)
    if manager.returncode != 0 or shutil.which("systemd-run") is None:
        print("no user service manager to run a pool under")
        return SKIPPED
    shutil.rmtree(directory, ignore_errors=True)
    os.makedirs(directory)
    for case in (arguments, repair, idle_full, symlinked, count_fails,
                 last_count_fails, surplus_race, fifo_deleted, idle_stop):
        try:
            case(tool, directory)
        except (OSError, ValueError, subprocess.TimeoutExpired) as error:
            failures.append(f"{case.__name__}: {error!r}")
    for failure in failures:
        print(failure)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(*sys.argv[1:]))
