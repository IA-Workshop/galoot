"""tools.measure_record: what the cost recorder writes in each row, and how
its reader fills cells that older rows lack.

Usage: python3 -I measure_record_test.py <tools/measure> <scratch directory>

Runs tools/measure inside a fresh git repository in the scratch directory, so
that run rows go to that repository's run log and not to the real one. A
stand-in `perf` on PATH writes chosen counter lines and runs the command, so
the cases do not depend on what the machine's counters report. Prints one
line per failed check and exits 1 if there is any.
"""

import os
import shutil
import subprocess
import sys

FAKE_PERF = """#!/bin/sh
# Stands in for perf stat: runs the command after -- and writes the lines in
# FAKE_PERF_OUTPUT to the file named by -o, as perf does when it exits.
while [ "$1" != "--" ]; do
  if [ "$1" = "-o" ]; then out=$2; fi
  shift
done
shift
"$@"
status=$?
printf '%s\\n' "$FAKE_PERF_OUTPUT" > "$out"
exit $status
"""

RUN_LOG = "docs/agent/records/performance/runs.tsv"

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


class Scratch:
    def __init__(self, measure, directory):
        self.measure = os.path.abspath(measure)
        self.top = os.path.realpath(directory)
        shutil.rmtree(self.top, ignore_errors=True)
        os.makedirs(os.path.join(self.top, "bin"))
        subprocess.run(["git", "init", "-q", self.top], check=True)

        # bin holds git and, when a case asks for it, the stand-in perf. The
        # real perf is never on this PATH.
        os.symlink(shutil.which("git"), os.path.join(self.top, "bin", "git"))

    def use_fake_perf(self, enabled):
        path = os.path.join(self.top, "bin", "perf")
        if enabled and not os.path.exists(path):
            with open(path, "w", encoding="utf-8") as f:
                f.write(FAKE_PERF)
            os.chmod(path, 0o755)
        elif not enabled and os.path.exists(path):
            os.unlink(path)

    def measure_cmd(self, args, perf_output=""):
        env = {"PATH": os.path.join(self.top, "bin"),
               "FAKE_PERF_OUTPUT": perf_output}
        return subprocess.run([sys.executable, "-I", self.measure, *args],
                              cwd=self.top, env=env, capture_output=True,
                              text=True, check=False)

    def rows(self, record):
        """Returns the record's rows as dicts keyed by its header."""
        with open(os.path.join(self.top, record), encoding="utf-8") as f:
            lines = f.read().splitlines()
        header = lines[0].split("\t")
        return [dict(zip(header, line.split("\t"))) for line in lines[1:]]


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
                   "-O2", "-g", f"-I{s.top}/include", "-MD", "-MT", "x.o",
                   "-MF", "x.o.d", "-o", "x.o", "-c", f"{s.top}/src/x.cpp"])
    row = s.rows("options.tsv")[0]
    check("options of a compile row", row["options"], "-O2 -g -Iinclude")
    check("source of a compile row", row["source"], "src/x.cpp")
    check("object of a compile row", row["object"], "x.o")
    check("pinned_other_s of a shared row", row["pinned_other_s"],
          "unavailable")


def counters_per_core_type(s):
    # Unpinned, one instruction event per core type. A core type the command
    # never ran on reports <not counted> with zero running time, which is a
    # count of zero; <not supported> is no count at all.
    s.use_fake_perf(True)
    s.measure_cmd(
        ["tu", "p", "c", "types.tsv", "--", "/bin/true"],
        perf_output="1000,,cpu_core/instructions/u,5000,100.00,,\n"
        "<not counted>,,cpu_atom/instructions/u,0,0.00,,")
    s.measure_cmd(
        ["tu", "p", "c", "types.tsv", "--", "/bin/true"],
        perf_output="1000,,cpu_core/instructions/u,5000,100.00,,\n"
        "<not supported>,,cpu_atom/instructions/u,0,0.00,,")
    counted, unsupported = s.rows("types.tsv")
    check("mode of an unpinned row", counted["mode"], "shared")
    check("instructions on the core type the command ran on",
          counted["inst_p_u"], "1000")
    check("instructions on a core type never run on", counted["inst_e_u"], "0")
    check("instructions of an unsupported event", unsupported["inst_e_u"],
          "unavailable")


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
                   "/bin/true", "-C", tree])
    s.measure_cmd(["run", "test", "p", "preset-tree", "--", "/bin/true"])
    s.measure_cmd(["run", "test", "-", "no-tree", "--", "/bin/true"])
    s.measure_cmd(["run", "test", "r", "missing-tree", "--", "/bin/true"])
    options = {row["selection"]: row["options"] for row in s.rows(RUN_LOG)}
    check("options of a run that names its tree", options["named-tree"],
          "-Wall -Werror -O1 -g")
    check("options of a run in its preset's tree", options["preset-tree"],
          "-Wall -Werror -O1 -g")
    check("options of a run with no tree", options["no-tree"], "-")
    check("options of a run whose tree has no cache", options["missing-tree"],
          "unavailable")


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

    with open(record, "w", encoding="utf-8") as f:
        f.write("a\tb\n1\t2\na\tb\n")
    check("exit of a read with a repeated header",
          s.measure_cmd(["read", record]).returncode, 1)
    check("exit of a read of an unknown column",
          s.measure_cmd(["read", record, "z"]).returncode, 1)


def main(measure, directory):
    s = Scratch(measure, directory)
    for case in (perf_ran_but_named_no_event, command_cannot_start,
                 compile_options, counters_per_core_type, run_options, reader):
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
