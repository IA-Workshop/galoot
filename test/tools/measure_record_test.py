"""tools.measure_record: what the cost recorder writes in each row, and how
its reader fills cells that older rows lack.

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

import os
import shutil
import subprocess
import sys

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
        self.git_log = os.path.join(self.top, "git.log")
        self.perf_events = os.path.join(self.top, "perf-events.txt")

        # bin holds the logging git, stand-ins for the build programs, and,
        # when a case asks for it, the stand-in perf. The real perf is never
        # on this PATH.
        real_git = shutil.which("git")
        self.git = os.path.join(self.top, "bin", "git")
        self.write_script(self.git, FAKE_GIT.format(git=real_git))
        for program in ("ninja", "ctest"):
            os.symlink("/bin/true", os.path.join(self.top, "bin", program))
        git = [real_git, "-C", self.top, "-c", "user.name=test",
               "-c", "user.email=test@example.invalid"]
        subprocess.run([real_git, "init", "-q", "-b", "main", self.top],
                       check=True)
        subprocess.run([*git, "add", "tools/measure"], check=True)
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
    for name, (lines, inst_e_u) in cases.items():
        s.measure_cmd(["tu", "p", "c", "types.tsv", "--", "/bin/true"],
                      perf_output="\n".join(lines))
        row = s.rows("types.tsv")[-1]
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
    # compile that runs outside it, and git status takes no index lock.
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
    check("git status calls", len(status_calls), 1)
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


def main(measure, directory):
    s = Scratch(measure, directory)
    for case in (perf_ran_but_named_no_event, command_cannot_start,
                 compile_options, counters_per_core_type, run_options, reader,
                 record_with_another_header, git_attribution):
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
