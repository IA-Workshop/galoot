"""tools.measure_record: what the cost recorder writes in each row, how its
reader fills cells that older rows lack, and the logs and the job pool that
tools/timed asks for.

Usage: python3 -I measure_record_test.py <tools/measure> <scratch directory>

Copies tools/measure into a fresh git repository in the scratch directory and
runs the copy there, because the recorder takes its revision from, and writes
run rows into, the checkout that holds it; the real run log is never touched.
A stand-in `perf` on PATH records the events it was asked for, writes chosen
counter lines, and runs the command, so the cases do not depend on what the
machine's counters report. A `git` on PATH logs its arguments and then runs
the real git, or fails when a case asks it to. Prints one line per failed
check and exits 1 if there is any.
"""

import contextlib
import os
import shutil
import subprocess
import sys
import threading
import time

FAKE_PERF = """#!/bin/sh
# Stands in for perf stat: runs the command after -- and writes the lines in
# FAKE_PERF_OUTPUT to the file named by -o, as perf does when it exits. The
# event list after -e is written to FAKE_PERF_EVENTS.
while [ "$1" != "--" ]; do
  if [ "$1" = "-o" ]; then out=$2; fi
  if [ "$1" = "-e" ]; then printf '%s\\n' "$2" > "$FAKE_PERF_EVENTS"; fi
  shift
done
shift
"$@"
status=$?
printf '%s\\n' "$FAKE_PERF_OUTPUT" > "$out"
exit $status
"""

FAKE_GIT = """#!/bin/sh
# Logs its arguments, then fails every command when FAKE_GIT_FAIL is "all",
# fails only git status when it is "status", and otherwise runs the real git.
printf '%s\\n' "$*" >> "$GIT_LOG"
case "${{FAKE_GIT_FAIL:-}}: $* " in
  all:*) exit 128 ;;
  status:*" status "*) exit 128 ;;
esac
exec {git} "$@"
"""

RUN_LOG = "docs/agent/records/performance/runs.tsv"

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


class Scratch:
    def __init__(self, measure, directory):
        self.top = os.path.realpath(directory)
        shutil.rmtree(self.top, ignore_errors=True)
        os.makedirs(os.path.join(self.top, "bin"))
        os.makedirs(os.path.join(self.top, "tools"))
        self.measure = os.path.join(self.top, "tools", "measure")
        shutil.copy(measure, self.measure)
        self.timed = os.path.join(self.top, "tools", "timed")
        shutil.copy(os.path.join(os.path.dirname(measure), "timed"),
                    self.timed)
        self.git_log = os.path.join(self.top, "git.log")
        self.perf_events = os.path.join(self.top, "perf-events.txt")

        # bin holds the logging git, stand-ins for the build programs, and,
        # when a case asks for it, the stand-in perf. The real perf is never
        # on this PATH.
        real_git = shutil.which("git")
        self.git = os.path.join(self.top, "bin", "git")
        self.write_script(self.git, FAKE_GIT.format(git=real_git))
        for program in ("ninja", "ctest", "cmake"):
            os.symlink("/bin/true", os.path.join(self.top, "bin", program))
        # tools/timed runs under bash, waits with sleep, and starts the
        # recorder with python3.
        os.symlink(shutil.which("bash"), os.path.join(self.top, "bin", "bash"))
        os.symlink(shutil.which("sleep"),
                   os.path.join(self.top, "bin", "sleep"))
        os.symlink(sys.executable, os.path.join(self.top, "bin", "python3"))
        git = [real_git, "-C", self.top, "-c", "user.name=test",
               "-c", "user.email=test@example.invalid"]
        subprocess.run([real_git, "init", "-q", "-b", "main", self.top],
                       check=True)
        subprocess.run([*git, "add", "tools/measure", "tools/timed"],
                       check=True)
        subprocess.run([*git, "commit", "-q", "-m", "recorder"], check=True)
        self.revision = subprocess.run(
            [*git, "rev-parse", "--short=12", "HEAD"], check=True,
            capture_output=True, text=True).stdout.strip()

    @staticmethod
    def write_script(path, text):
        with open(path, "w", encoding="utf-8") as f:
            f.write(text)
        os.chmod(path, 0o755)

    def use_fake_perf(self, enabled):
        path = os.path.join(self.top, "bin", "perf")
        if enabled and not os.path.exists(path):
            self.write_script(path, FAKE_PERF)
        elif not enabled and os.path.exists(path):
            os.unlink(path)

    def measure_cmd(self, args, perf_output="", cwd=None, git_fails=""):
        env = {"PATH": os.path.join(self.top, "bin"),
               "FAKE_PERF_OUTPUT": perf_output,
               "FAKE_PERF_EVENTS": self.perf_events, "GIT_LOG": self.git_log,
               "FAKE_GIT_FAIL": git_fails}
        return subprocess.run([sys.executable, "-I", self.measure, *args],
                              cwd=cwd or self.top, env=env,
                              capture_output=True, text=True, check=False)

    def timed_cmd(self, args, cwd=None, timed_log=None, makeflags=None):
        env = {"PATH": os.path.join(self.top, "bin"), "GIT_LOG": self.git_log}
        if timed_log is not None:
            env["TIMED_LOG"] = timed_log
        if makeflags is not None:
            env["MAKEFLAGS"] = makeflags
        return subprocess.run([self.timed, *args], cwd=cwd or self.top,
                              env=env, capture_output=True, text=True,
                              check=False)

    def run_log(self):
        try:
            with open(os.path.join(self.top, RUN_LOG), encoding="utf-8") as f:
                return f.read()
        except FileNotFoundError:
            return ""

    def rows(self, record):
        """Returns the record's rows as dicts keyed by its header."""
        with open(os.path.join(self.top, record), encoding="utf-8") as f:
            lines = f.read().splitlines()
        header = lines[0].split("\t")
        return [dict(zip(header, line.split("\t"), strict=True))
                for line in lines[1:]]


def perf_ran_but_named_no_event(s):
    # perf ran the command and printed a counter line, but under an event
    # name the recorder did not ask for. Running the command again without
    # perf would build or test twice.
    s.use_fake_perf(True)
    counter = os.path.join(s.top, "ran.txt")
    result = s.measure_cmd(
        ["tu", "p", "c", "once.tsv", "--", "/bin/sh", "-c",
         f"echo ran >> {counter}"],
        perf_output="1000,,cpu_core/renamed-event/u,1000,100.00,,")
    with open(counter, encoding="utf-8") as f:
        check("command runs once when perf ran it", f.read(), "ran\n")
    check("exit of a command perf ran", result.returncode, 0)
    row = s.rows("once.tsv")[0]
    check("counter perf did not report", row["inst_p_u"], "unavailable")


def command_cannot_start(s):
    s.use_fake_perf(False)
    result = s.measure_cmd(["tu", "p", "c", "nostart.tsv", "--",
                            "/no/such/compiler", "-c", "a.cpp", "-o", "a.o"])
    check("exit of a command that cannot start", result.returncode, 127)
    rows = s.rows("nostart.tsv")
    check("rows for a command that cannot start", len(rows), 1)
    check("its exit cell", rows[0]["exit"], "127")
    check("its wall time", rows[0]["wall_s"], "unavailable")


def compile_options(s):
    s.use_fake_perf(False)
    s.measure_cmd(["tu", "p", "c", "options.tsv", "--", "/bin/true",
                   "-O2", "-g", f"-I{s.top}/include", "-MD", "-MMD",
                   "-MT", "x.o", "-MF", "x.o.d", "-o", "x.o",
                   "-c", f"{s.top}/src/x.cpp"])
    row = s.rows("options.tsv")[0]
    check("options of a compile row", row["options"], "-O2 -g -Iinclude")
    check("source of a compile row", row["source"], "src/x.cpp")
    check("object of a compile row", row["object"], "x.o")
    check("pinned_other_s of a shared row", row["pinned_other_s"],
          "unavailable")


def counters_per_core_type(s):
    # Unpinned, one instruction event per core type and task-clock. A core
    # type reported <not counted> retired nothing only if the other type ran
    # for all of task-clock's time; otherwise its count is unknown.
    s.use_fake_perf(True)
    counted = "1000,,cpu_core/instructions/u,5000,100.00,,"
    not_counted = "<not counted>,,cpu_atom/instructions/u,0,0.00,,"
    task_clock = "5.00,msec,task-clock:u,{},100.00,,"
    cases = {
        "all time on the other type":
            ([counted, not_counted, task_clock.format(5000)], "0"),
        "other type short of task-clock":
            ([counted, not_counted, task_clock.format(6000)], "unavailable"),
        "no task-clock line": ([counted, not_counted], "unavailable"),
        "unsupported event":
            ([counted, "<not supported>,,cpu_atom/instructions/u,0,0.00,,",
              task_clock.format(5000)], "unavailable"),
    }
    for index, (name, (lines, inst_e_u)) in enumerate(cases.items()):
        s.measure_cmd(["tu", "p", "c", "types.tsv", "--", "/bin/true"],
                      perf_output="\n".join(lines))
        # Each case appends one row, so a case that writes none cannot be
        # judged by the row of the case before it.
        rows = s.rows("types.tsv")
        check(f"rows after the case {name}", len(rows), index + 1)
        row = rows[-1]
        check(f"mode, {name}", row["mode"], "shared")
        check(f"instructions on the type the command ran on, {name}",
              row["inst_p_u"], "1000")
        check(f"instructions on the other type, {name}", row["inst_e_u"],
              inst_e_u)

    # The check above needs task-clock's running time, so the shared event
    # list must ask perf for it.
    with open(s.perf_events, encoding="utf-8") as f:
        events = f.read().strip().split(",")
    check("shared events requested of perf", events,
          ["cpu_core/instructions/u", "cpu_atom/instructions/u",
           "task-clock"])


def run_options(s):
    s.use_fake_perf(False)
    tree = os.path.join(s.top, "build", "p")
    os.makedirs(tree)
    with open(os.path.join(tree, "CMakeCache.txt"), "w", encoding="utf-8") as f:
        f.write("CMAKE_BUILD_TYPE:STRING=Debug\n"
                "CMAKE_CXX_FLAGS:STRING=-Wall -Werror\n"
                "CMAKE_CXX_FLAGS_DEBUG:UNINITIALIZED=-O1 -g\n"
                "CMAKE_CXX_FLAGS_RELEASE:STRING=-O2\n")
    s.measure_cmd(["run", "build", "q", "named-tree", "--",
                   "ninja", "-C", tree])
    s.measure_cmd(["run", "test", "p", "ctest-configuration", "--",
                   "ctest", "-C", "Debug"])
    s.measure_cmd(["run", "build", "q", "not-a-build-program", "--",
                   "/bin/true", "-C", tree])
    s.measure_cmd(["run", "test", "p", "preset-tree", "--", "/bin/true"])
    s.measure_cmd(["run", "test", "-", "no-tree", "--", "/bin/true"])
    s.measure_cmd(["run", "test", "r", "missing-tree", "--", "/bin/true"])
    # A label or regex value that names a build program is not the program.
    s.measure_cmd(["run", "test", "p", "label-named-make", "--",
                   "ctest", "-L", "make", "-C", "Debug"])
    s.measure_cmd(["run", "test", "q", "regex-named-ninja", "--",
                   "ctest", "-R", "ninja", "-C", "Debug", "--test-dir", tree])
    # cmake -E runs another program, which names the tree.
    s.measure_cmd(["run", "test", "q", "cmake-e-env", "--",
                   "cmake", "-E", "env", "FOO=1", "ctest", "--test-dir", tree])
    s.measure_cmd(["run", "build", "q", "cmake-e-time", "--",
                   "cmake", "-E", "time", "ninja", "-C", tree])
    # cmake -E chdir runs the program in its directory, which is then its
    # tree, or the base of a relative tree it names.
    s.measure_cmd(["run", "build", "q", "cmake-e-chdir", "--",
                   "cmake", "-E", "chdir", tree, "ninja"])
    s.measure_cmd(["run", "build", "q", "cmake-e-chdir-relative", "--",
                   "cmake", "-E", "chdir", os.path.join(s.top, "build"),
                   "ninja", "-C", "p"])
    options = {row["selection"]: row["options"] for row in s.rows(RUN_LOG)}
    check("options of a run that names its tree", options["named-tree"],
          "-Wall -Werror -O1 -g")
    check("options of a run in its preset's tree", options["preset-tree"],
          "-Wall -Werror -O1 -g")
    check("options of ctest -C, a configuration, not a tree",
          options["ctest-configuration"], "-Wall -Werror -O1 -g")
    check("options of -C for a program that takes no tree",
          options["not-a-build-program"], "unavailable")
    check("options of a run with no tree", options["no-tree"], "-")
    check("options of a run whose tree has no cache", options["missing-tree"],
          "unavailable")
    check("options of ctest -L make, a label, not a program",
          options["label-named-make"], "-Wall -Werror -O1 -g")
    check("options of ctest -R ninja with --test-dir",
          options["regex-named-ninja"], "-Wall -Werror -O1 -g")
    check("options of ctest run by cmake -E env",
          options["cmake-e-env"], "-Wall -Werror -O1 -g")
    check("options of ninja run by cmake -E time",
          options["cmake-e-time"], "-Wall -Werror -O1 -g")
    check("options of ninja run by cmake -E chdir",
          options["cmake-e-chdir"], "-Wall -Werror -O1 -g")
    check("options of ninja -C relative to cmake -E chdir's directory",
          options["cmake-e-chdir-relative"], "-Wall -Werror -O1 -g")


def reader(s):
    # Rows written before a column existed are read as -- in that column.
    record = os.path.join(s.top, "old.tsv")
    with open(record, "w", encoding="utf-8") as f:
        f.write("a\tb\tc\n1\n1\t2\t3\n")
    result = s.measure_cmd(["read", record, "c", "a"])
    check("read of short and full rows", result.stdout, "c\ta\n--\t1\n3\t1\n")
    check("exit of a good read", result.returncode, 0)

    with open(record, "a", encoding="utf-8") as f:
        f.write("1\t2\t3\t4\n")
    check("exit of a read with a row longer than the header",
          s.measure_cmd(["read", record]).returncode, 1)

    for name, text in (("repeated header", "a\tb\n1\t2\na\tb\n"),
                       ("older, shorter header", "a\tb\n1\t2\na\n3\n"),
                       ("blank line", "a\tb\n1\t2\n\n3\t4\n"),
                       ("blank header line", "\n"),
                       ("empty file", "")):
        with open(record, "w", encoding="utf-8") as f:
            f.write(text)
        result = s.measure_cmd(["read", record])
        check(f"exit of a read with this defect: {name}", result.returncode, 1)
        check(f"a read with this defect names the file: {name}",
              record in result.stderr, True)
    with open(record, "w", encoding="utf-8") as f:
        f.write("a\tb\n1\t2\n")
    check("exit of a read of an unknown column",
          s.measure_cmd(["read", record, "z"]).returncode, 1)

    missing = os.path.join(s.top, "no-such-record.tsv")
    result = s.measure_cmd(["read", missing])
    check("exit of a read of a missing file", result.returncode, 1)
    check("a read of a missing file names it without a traceback",
          (missing in result.stderr, "Traceback" in result.stderr),
          (True, False))


def record_with_another_header(s):
    # Appending under a header that describes other columns would mislabel
    # every cell, so the row is refused and the file left as it was.
    s.use_fake_perf(False)
    record = os.path.join(s.top, "other-header.tsv")
    with open(record, "w", encoding="utf-8") as f:
        f.write("utc_start\trevision\n")
    result = s.measure_cmd(["tu", "p", "c", record, "--", "/bin/true"])
    check("exit of a command whose record has another header",
          result.returncode, 0)
    with open(record, encoding="utf-8") as f:
        check("record with another header after a row was refused",
              f.read(), "utc_start\trevision\n")
    check("warning for a record with another header",
          "not recorded" in result.stderr, True)


def git_attribution(s):
    # The revision and branch are the recorder's checkout's, even for a
    # compile that runs outside it, and git status takes no index lock. It
    # runs once before the command and once after.
    s.use_fake_perf(False)
    outside = s.top + "-outside"
    shutil.rmtree(outside, ignore_errors=True)
    os.makedirs(outside)
    if os.path.exists(s.git_log):
        os.unlink(s.git_log)
    s.measure_cmd(["tu", "p", "c", "outside.tsv", "--", "/bin/true"],
                  cwd=outside)
    row = s.rows(os.path.join(outside, "outside.tsv"))[0]
    check("revision of a compile outside the checkout", row["revision"],
          s.revision)
    check("branch of a compile outside the checkout", row["branch"], "main")
    with open(s.git_log, encoding="utf-8") as f:
        status_calls = [line for line in f if " status " in f" {line}"]
    check("git status calls", len(status_calls), 2)
    check("git status runs without optional locks",
          all("--no-optional-locks status" in line for line in status_calls),
          True)

    os.rename(s.git, s.git + ".off")
    try:
        s.measure_cmd(["tu", "p", "c", "nogit.tsv", "--", "/bin/true"])
    finally:
        os.rename(s.git + ".off", s.git)
    row = s.rows("nogit.tsv")[0]
    check("revision without git", row["revision"], "unavailable")
    check("branch without git", row["branch"], "unavailable")

    # git present but failing is not a detached HEAD, and a revision whose
    # status git could not report is not known to be clean.
    s.measure_cmd(["tu", "p", "c", "gitfails.tsv", "--", "/bin/true"],
                  git_fails="all")
    row = s.rows("gitfails.tsv")[0]
    check("revision when git fails", row["revision"], "unavailable")
    check("branch when git fails", row["branch"], "unavailable")
    s.measure_cmd(["tu", "p", "c", "statusfails.tsv", "--", "/bin/true"],
                  git_fails="status")
    row = s.rows("statusfails.tsv")[0]
    check("revision when git status fails", row["revision"], "unavailable")
    check("branch when only git status fails", row["branch"], "main")


def dirty_mark(s):
    # A row is marked dirty when the tree was dirty when the command started
    # or when it ended, or when HEAD moved while it ran, and then carries the
    # revision the command started on. Each case leaves the scratch
    # repository clean again.
    s.use_fake_perf(False)
    real_git = shutil.which("git")

    def git(*args):
        return subprocess.run(
            [real_git, "-C", s.top, "-c", "user.name=test",
             "-c", "user.email=test@example.invalid", *args],
            check=True, capture_output=True, text=True).stdout.strip()

    def head():
        return git("rev-parse", "--short=12", "HEAD")

    def edit(path):
        with open(os.path.join(s.top, path), "a", encoding="utf-8") as f:
            f.write("edit\n")

    log = os.path.join(s.top, "dirty-mark.tsv")
    campaign = "docs/agent/records/performance/campaign.tsv"
    os.makedirs(os.path.dirname(os.path.join(s.top, campaign)), exist_ok=True)
    for path in ("tracked.txt", campaign):
        with open(os.path.join(s.top, path), "w", encoding="utf-8") as f:
            f.write("line\n")
    git("add", "tracked.txt", campaign)
    git("commit", "-q", "-m", "files the cases change")

    def revision_of(name, script, before=lambda: None):
        before()
        s.measure_cmd(["run", "--log", log, "step", "-", name, "--",
                       "/bin/sh", "-c", script])
        return [r["revision"] for r in s.rows(log)
                if r["selection"] == name][0]

    base = head()
    check("a clean tree throughout", revision_of("clean", "true"), base)

    # The index counts: a change that is staged and not committed is dirty.
    edit("tracked.txt")
    git("add", "tracked.txt")
    check("a staged change", revision_of("staged", "true"), base + "-dirty")
    git("reset", "-q", "--hard", base)

    edit("tracked.txt")
    check("an edit before the command",
          revision_of("edit-before", "true"), base + "-dirty")
    git("reset", "-q", "--hard", base)

    check("an edit during the command",
          revision_of("edit-during", "echo edit >> tracked.txt"),
          base + "-dirty")
    git("reset", "-q", "--hard", base)

    check("an edit before the command, undone during it",
          revision_of("undone-during", "git checkout -q -- tracked.txt",
                      lambda: edit("tracked.txt")), base + "-dirty")

    check("a change before the command, committed during it",
          revision_of("committed-during",
                      "git -c user.name=t -c user.email=t@example.invalid "
                      "commit -q -a -m change",
                      lambda: edit("tracked.txt")), base + "-dirty")
    check("the commit moved HEAD", head() != base, True)
    base = head()

    check("a commit during the command",
          revision_of("empty-commit-during",
                      "git -c user.name=t -c user.email=t@example.invalid "
                      "commit -q --allow-empty -m move"), base + "-dirty")
    base = head()

    # The performance records change with every run and are never the mark.
    edit(campaign)
    check("a records edit before the command",
          revision_of("records-before", "echo edit >> " + campaign), base)
    git("reset", "-q", "--hard", base)

    # The compile launcher reads the mark the same way.
    edit("tracked.txt")
    s.measure_cmd(["tu", "p", "c", "dirty-mark-tu.tsv", "--", "/bin/sh", "-c",
                   "git checkout -q -- tracked.txt"])
    check("a compile started dirty",
          s.rows("dirty-mark-tu.tsv")[0]["revision"], base + "-dirty")
    check("the tree is clean again",
          git("status", "--porcelain", "--untracked-files=no"), "")


def log_option(s):
    # A campaign names its own log; the run log is left as it was.
    s.use_fake_perf(False)
    before = s.run_log()
    campaign = os.path.join(s.top, "campaign", "c.tsv")
    result = s.measure_cmd(["run", "--log", campaign, "other", "-", "sel",
                            "--", "/bin/true"])
    check("exit with --log", result.returncode, 0)
    rows = s.rows(campaign)
    check("rows in the named log", [r["selection"] for r in rows], ["sel"])
    check("run log after --log", s.run_log(), before)
    check("stderr names the log", f"row in {campaign}" in result.stderr, True)

    # tools/timed: a relative --log is taken from the current directory.
    os.makedirs(os.path.join(s.top, "sub"), exist_ok=True)
    result = s.timed_cmd(["--log", "c2.tsv", "other", "-", "rel", "--",
                          "/bin/true"], cwd=os.path.join(s.top, "sub"))
    check("exit of timed --log", result.returncode, 0)
    check("rows of a relative --log",
          [r["selection"] for r in s.rows(os.path.join("sub", "c2.tsv"))],
          ["rel"])

    env_log = os.path.join(s.top, "env.tsv")
    s.timed_cmd(["other", "-", "env", "--", "/bin/true"], timed_log=env_log)
    s.timed_cmd(["--log", campaign, "other", "-", "both", "--", "/bin/true"],
                timed_log=env_log)
    check("rows of TIMED_LOG", [r["selection"] for r in s.rows(env_log)],
          ["env"])
    check("--log wins over TIMED_LOG",
          [r["selection"] for r in s.rows(campaign)], ["sel", "both"])
    check("run log after TIMED_LOG", s.run_log(), before)

    s.timed_cmd(["other", "-", "default", "--", "/bin/true"], timed_log="")
    check("an empty TIMED_LOG means the run log",
          [r["selection"] for r in s.rows(RUN_LOG)][-1], "default")

    # A log with another recorder's header is refused, and the command's
    # status still passes through.
    other = os.path.join(s.top, "other.tsv")
    with open(other, "w", encoding="utf-8") as f:
        f.write("utc_start\trevision\n")
    result = s.timed_cmd(["--log", other, "other", "-", "x", "--", "/bin/sh",
                          "-c", "exit 3"])
    check("exit through a refused log", result.returncode, 3)
    with open(other, encoding="utf-8") as f:
        check("log with another header", f.read(), "utc_start\trevision\n")
    check("warning names the refused log",
          "not recorded" in result.stderr and other in result.stderr, True)

    # A relative TIMED_LOG would name a different file from each directory
    # a script runs in, so it is refused and the command is not run.
    marker = os.path.join(s.top, "ran-relative.txt")
    result = s.timed_cmd(["other", "-", "rel-env", "--", "/bin/sh", "-c",
                          f"echo ran > {marker}"], timed_log="rel.tsv")
    check("exit with a relative TIMED_LOG", result.returncode, 2)
    check("command not run with a relative TIMED_LOG",
          os.path.exists(marker), False)
    check("message for a relative TIMED_LOG",
          "TIMED_LOG must be an absolute path" in result.stderr, True)
    check("no log from a relative TIMED_LOG",
          os.path.exists(os.path.join(s.top, "rel.tsv")), False)

    result = s.timed_cmd(["--log"])
    check("exit of --log without a file", result.returncode, 2)
    check("usage for --log without a file",
          result.stderr.startswith("usage: tools/timed [--log <file>]"), True)


def timed_pool(s):
    """A build step needs the FIFO that MAKEFLAGS names. The pool moves its
    FIFO aside to <fifo>.stopping for a moment while it counts its clients,
    so tools/timed waits up to about 2 s while that file exists, and
    refuses at once when neither exists."""
    pools = os.path.join(s.top, "pools")
    os.makedirs(pools, exist_ok=True)
    fifo = os.path.join(pools, "pool.fifo")
    aside = f"{fifo}.stopping"
    makeflags = f"-j3 --jobserver-auth=fifo:{fifo}"
    marker = os.path.join(s.top, "built.txt")

    def build(selection):
        with contextlib.suppress(FileNotFoundError):
            os.unlink(marker)
        start = time.monotonic()
        result = s.timed_cmd(["build", "-", selection, "--", "/bin/sh", "-c",
                              f"echo built > {marker}"], makeflags=makeflags)
        return result, time.monotonic() - start, os.path.exists(marker)

    check("pool: refused with no MAKEFLAGS",
          s.timed_cmd(["build", "-", "none", "--", "/bin/true"]).returncode, 2)

    os.mkfifo(fifo)
    result, _, built = build("present")
    check("pool: exit with the FIFO present", result.returncode, 0)
    check("pool: built with the FIFO present", built, True)

    os.unlink(fifo)
    result, seconds, built = build("missing")
    check("pool: exit with the FIFO missing", result.returncode, 2)
    check("pool: refused with the FIFO missing",
          result.stderr.startswith("timed: refused:"), True)
    check("pool: not built with the FIFO missing", built, False)
    check("pool: refused at once with nothing moved aside", seconds < 1.0,
          True)

    os.mkfifo(aside)
    mover = threading.Timer(0.5, os.rename, (aside, fifo))
    mover.start()
    try:
        result, seconds, built = build("moved-back")
    finally:
        mover.join()
    check("pool: exit when the FIFO comes back", result.returncode, 0)
    check("pool: built when the FIFO comes back", built, True)
    check("pool: waited for the FIFO to come back", seconds >= 0.4, True)

    os.rename(fifo, aside)
    result, seconds, built = build("stays-aside")
    check("pool: exit with the FIFO left aside", result.returncode, 2)
    check("pool: refused with the FIFO left aside",
          result.stderr.startswith("timed: refused:"), True)
    check("pool: the refusal names the FIFO left aside",
          f"moved aside to {aside}" in result.stderr, True)
    check("pool: not built with the FIFO left aside", built, False)
    check("pool: waited about 2 s with the FIFO left aside",
          1.5 <= seconds < 3.5, True)
    check("pool: rows of the runs that built",
          [r["selection"] for r in s.rows(RUN_LOG) if r["selection"] in (
              "present", "missing", "moved-back", "stays-aside")],
          ["present", "moved-back"])


def main(measure, directory):
    s = Scratch(measure, directory)
    for case in (perf_ran_but_named_no_event, command_cannot_start,
                 compile_options, counters_per_core_type, run_options, reader,
                 record_with_another_header, git_attribution, dirty_mark,
                 log_option,
                 timed_pool):
        try:
            case(s)
        except (OSError, KeyError, IndexError, ValueError) as error:
            failures.append(f"{case.__name__}: {type(error).__name__}: {error}")
    for failure in failures:
        print(failure)
    return 1 if failures else 0


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit("usage: measure_record_test.py <tools/measure> <scratch>")
    sys.exit(main(*sys.argv[1:]))
