"""tools.compiles: the before tree, where each compile takes its flags, how
include paths and the launcher are rewritten, the refusal of a dirty
checkout, the order and log of cost compiles, shared mode with unavailable
counters, and the sizes of diagnostics.

Usage: python3 -I compiles_test.py <tools directory> <scratch directory>

The tools directory holds compiles, timed, and measure. The scratch
directory is wiped and gets a fresh git repository with the files in
compiles/ beside this script at two commits, the second adding thrice() to
include/lib.hpp, and copies of the three tools under tools/, so that every
run row goes to the scratch repository's own run log and the real one is
never read or written. Its build trees are written by hand:
compile_commands.json and a CTestTestfile.cmake of compile-fail tests per
compiler, whose commands carry the compile-cost launcher and include paths
under the root, each source of flags with its own -D macro. A perf that
fails at once is first on PATH, so every counter is unavailable and no run
depends on the machine's counters. Cost runs use --cpu shared. TIMED_LOG is
removed from the environment the tool gets. Every expected value is worked
out from the fixture, or, for the sizes of a diagnostic, from the
compiler's own output for the same source and flags. Prints one line per
failed check and exits 1 if there is any.
"""

import json
import os
import shlex
import shutil
import subprocess
import sys

FIXTURE = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       "compiles")
RUN_LOG = "docs/agent/records/performance/runs.tsv"
SOURCES = ["unit_test.cpp", "one_error_fail.cpp", "two_errors_fail.cpp",
           "clean_fail.cpp", "flags_source_fail.cpp"]
COMPILERS = {"gcc": ("g++", "GNU"), "clang": ("clang++", "Clang")}
OUTSIDE = "-I/opt/compiles-fixture-outside"
ROWS = ("repeat compiler unit tree preset mode inst_u wall_s maxrss_kib "
        "sibling_busy_s pinned_other_s command").split()
SUMMARY = ("compiler unit tree repeats inst_mean inst_range wall_mean_s "
           "maxrss_max_kib command").split()
DIAGNOSTICS = ("label source compiler exit lines error_lines longest "
               "first_length first_error command").split()
# A perf that never starts the command: tools/measure then runs the command
# without counters and records them as unavailable.
FAILING_PERF = "#!/bin/sh\nexit 1\n"
# A perf that runs the command and then writes, in perf stat's -x, form,
# the instructions of the n-th call on each core type from the table, n
# counted in the file COUNTING_PERF_STATE names.
COUNTING_PERF = """#!/bin/sh
while [ "$1" != "--" ]; do
  if [ "$1" = "-o" ]; then out=$2; fi
  shift
done
shift
"$@"
status=$?
n=$(($(cat "$COUNTING_PERF_STATE" 2>/dev/null || echo 0) + 1))
echo "$n" > "$COUNTING_PERF_STATE"
case $n in
  1) p=1000 e=10 ;;
  2) p=4000 e=40 ;;
  *) p=1600 e=25 ;;
esac
printf '%s\\n' "$p,,cpu_core/instructions/u,1000,100.00" \\
  "$e,,cpu_atom/instructions/u,1000,100.00" \\
  "1.00,msec,task-clock:u,1000,100.00" > "$out"
exit $status
"""
# The instructions of the three calls, performance and efficiency cores
# together, as the table above gives them.
COUNTED = [1000 + 10, 4000 + 40, 1600 + 25]

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


def fixture_text(name):
    with open(os.path.join(FIXTURE, name), encoding="utf-8") as f:
        return f.read()


class Repo:
    def __init__(self, tools, directory):
        self.top = os.path.realpath(directory)
        shutil.rmtree(self.top, ignore_errors=True)
        for sub in ("tools", "bin", "include", "test", "out",
                    os.path.dirname(RUN_LOG)):
            os.makedirs(self.path(sub))
        for tool in ("compiles", "timed", "measure"):
            shutil.copy(os.path.join(tools, tool), self.path("tools", tool))
        self.write("bin/perf", FAILING_PERF)
        os.chmod(self.path("bin/perf"), 0o755)
        os.makedirs(self.path("counting"))
        self.write("counting/perf", COUNTING_PERF)
        os.chmod(self.path("counting/perf"), 0o755)
        self.env = {k: v for k, v in os.environ.items() if k != "TIMED_LOG"}
        self.env["PATH"] = self.path("bin") + os.pathsep + self.env["PATH"]

        self.write(".gitignore", "build/\nbin/\ncounting/\nout/\n")
        self.write("include/lib.hpp", fixture_text("first/lib.hpp"))
        for name in SOURCES:
            self.write(f"test/{name}", fixture_text(f"test/{name}"))
        self.git("init", "-q", "-b", "main")
        self.first = self.commit("first", ".")
        self.write("include/lib.hpp", fixture_text("include/lib.hpp"))
        self.second = self.commit("add thrice", "include/lib.hpp")
        for compiler in COMPILERS:
            self.write_tree(compiler)

    def write_tree(self, compiler):
        """build/<compiler>-debug, as CMake would leave it for the parts
        the tool reads."""
        program, compiler_id = COMPILERS[compiler]
        program = shutil.which(program)
        preset = f"{compiler}-debug"
        tree = self.path("build", preset)
        os.makedirs(tree)
        launcher = [self.path("tools", "measure"), "tu", preset, compiler_id,
                    os.path.join(tree, "compile.tsv"), "--"]
        own = ["-std=c++23", "-Wall", "-Werror", "-DFLAGS_FROM_OWN",
               f"-I{self.top}/include", "-isystem", f"{self.top}/third_party"]
        unit = self.path("test", "unit_test.cpp")
        entry = {"directory": tree, "file": unit,
                 "output": "CMakeFiles/unit.dir/test/unit_test.cpp.o",
                 "command": shlex.join([
                     *launcher, program, "-std=c++23", "-Wall", "-Werror",
                     "-DFLAGS_FROM_COMPILE_COMMANDS", f"-I{self.top}/include",
                     "-isystem", f"{self.top}/third_party", "-o",
                     "CMakeFiles/unit.dir/test/unit_test.cpp.o", "-c",
                     unit])}
        self.write(f"build/{preset}/compile_commands.json",
                   json.dumps([entry], indent=2) + "\n")
        # The lending test comes first, so that taking the first test's
        # flags for every source would show.
        tests = [
            ("flags_source", ["-std=c++23", "-Wall", "-Werror",
                              "-DFLAGS_FROM_OTHER", "-I",
                              f"{self.top}/include",
                              f"-isystem{self.top}/third_party", OUTSIDE],
             "flags_source_fail.cpp"),
            ("one_error_fail", own, "one_error_fail.cpp"),
            ("two_errors_fail", own, "two_errors_fail.cpp"),
        ]
        lines = []
        for name, flags, source in tests:
            command = [shutil.which("cmake"), "-P",
                       self.path("test", "expect.cmake"), "--", "1",
                       "error", "--", *launcher, program, *flags, "-o",
                       os.path.join(tree, f"{name}.o"), "-c",
                       self.path("test", source)]
            quoted = " ".join(f'"{argument}"' for argument in command)
            lines.append(f"add_test({name} {quoted})")
        self.write(f"build/{preset}/CTestTestfile.cmake",
                   "\n".join(lines) + "\n")

    def git(self, *args):
        result = subprocess.run(
            ["git", "-C", self.top, "-c", "user.name=test",
             "-c", "user.email=test@example.invalid", *args],
            capture_output=True, text=True, check=False, env=self.env)
        if result.returncode != 0:
            raise RuntimeError(f"git {' '.join(args)}: {result.stderr}")
        return result.stdout

    def commit(self, message, *paths):
        self.git("add", *paths)
        self.git("commit", "-q", "-m", message)
        return self.git("rev-parse", "HEAD").strip()

    def path(self, *names):
        return os.path.join(self.top, *names)

    def write(self, name, text):
        with open(self.path(name), "w", encoding="utf-8") as f:
            f.write(text)

    def read(self, name):
        try:
            with open(self.path(name), encoding="utf-8") as f:
                return f.read()
        except FileNotFoundError:
            return None

    def compiles(self, *args, env=None):
        return subprocess.run(
            [sys.executable, "-I", "tools/compiles", *args], cwd=self.top,
            capture_output=True, text=True, check=False,
            env=env or self.env)

    def cost(self, spec_name, spec_text, repeats=1, cpu="shared", env=None):
        self.write(f"out/{spec_name}.toml", spec_text)
        return self.compiles("cost", f"out/{spec_name}.toml",
                             f"out/{spec_name}", "--before", self.first,
                             "--cpu", cpu, "--repeats", str(repeats),
                             env=env)

    def log_selections(self, name):
        """The selection and exit of each row of a run log."""
        text = self.read(name)
        if text is None:
            return []
        lines = text.splitlines()
        header = lines[0].split("\t")
        s, e = header.index("selection"), header.index("exit")
        return [(cells[s], cells[e])
                for cells in (line.split("\t") for line in lines[1:])]


def table(text, columns):
    """The rows of a tab-separated output, each a dict by column, with a
    header line left out if there is one."""
    rows = []
    for line in (text or "").splitlines():
        cells = line.split("\t")
        if cells != columns:
            rows.append(dict(zip(columns, cells)))
    return rows


def unit_spec(units, compilers=("gcc",), flags_from="flags_source"):
    lines = [f"compilers = {json.dumps(list(compilers))}",
             f"flags_from = {json.dumps(flags_from)}"]
    for unit in units:
        lines += ["", "[[unit]]"]
        lines += [f"{k} = {json.dumps(v)}" for k, v in unit.items()]
    return "\n".join(lines) + "\n"


USES_THRICE = {"name": "includer/uses_thrice",
               "text": "#include <lib.hpp>\n\n"
                       "static_assert(lib::thrice(2) == 6);\n",
               "kind": "debug"}
USES_TWICE = {"name": "includer/uses_twice",
              "text": "#include <lib.hpp>\n\n"
                      "static_assert(lib::twice(2) == 4);\n",
              "trees": ["before", "after"], "kind": "debug"}
UNIT = {"name": "unit", "source": "test/unit_test.cpp",
        "trees": ["before", "after"], "kind": "debug"}


def dirty_checkout(repo):
    """An uncommitted change to include/ is refused by both commands."""
    committed = repo.read("include/lib.hpp")
    repo.write("include/lib.hpp", committed + "// not committed\n")
    try:
        result = repo.cost("dirty", unit_spec([dict(USES_THRICE,
                                                    trees=["after"])]))
        check("dirty: cost exit", result.returncode, 2)
        result = repo.compiles("diagnostics", "out/dirty.tsv", "--label",
                               "dirty", "--flags-from", "flags_source",
                               "test/clean_fail.cpp")
        check("dirty: diagnostics exit", result.returncode, 2)
    finally:
        repo.write("include/lib.hpp", committed)


def before_tree(repo):
    """The before tree is the named commit's, which lacks thrice()."""
    result = repo.cost("before_after", unit_spec([dict(
        USES_THRICE, trees=["before", "after"])]))
    check("before and after: exit", result.returncode, 1)
    check("before and after: the compiler's message names thrice",
          "thrice" in result.stderr, True)
    check("before and after: the before tree's header is the first commit's",
          repo.read("out/before_after/before/include/lib.hpp"),
          fixture_text("first/lib.hpp"))
    rows = repo.log_selections("out/before_after/runs.tsv")
    first = rows[0] if rows else (None, None)
    check("before and after: the first compile is the before one",
          first[0], "cost:gcc:includer/uses_thrice/before")
    check("before and after: the before compile failed",
          first[1] not in (None, "0"), True)

    result = repo.cost("after_only", unit_spec([dict(USES_THRICE,
                                                     trees=["after"])]))
    check("after only: exit", result.returncode, 0)


def failing_unit(repo):
    result = repo.cost("failing", unit_spec([{
        "name": "fails", "kind": "debug", "trees": ["after"],
        "text": 'static_assert(false, "this unit does not compile");\n'}]))
    check("failing unit: exit", result.returncode, 1)
    check("failing unit: the compiler's message on stderr",
          "this unit does not compile" in result.stderr, True)


def tokens_after(tokens, flag):
    return [tokens[i + 1] for i, t in enumerate(tokens[:-1]) if t == flag]


def check_command(case, root, command, macro, include, isystem, source,
                  outside=False):
    """include and isystem: the path each include flag must name, relative
    to the root, in the form the flag source writes it."""
    tokens = command.split(" ")
    check(f"{case}: flags from the expected source", macro in tokens, True)
    check(f"{case}: no launcher", any("tools/measure" in t or
                                      "compile.tsv" in t for t in tokens),
          False)
    check(f"{case}: the root prefix removed",
          any(root in t for t in tokens), False)
    check(f"{case}: source", tokens[-1:], [source])
    if macro == "-DFLAGS_FROM_OTHER":
        check(f"{case}: -I <path>", tokens_after(tokens, "-I"), [include])
        check(f"{case}: -isystem<path>", f"-isystem{isystem}" in tokens,
              True)
    else:
        check(f"{case}: -I<path>", f"-I{include}" in tokens, True)
        check(f"{case}: -isystem <path>", tokens_after(tokens, "-isystem"),
              [isystem])
    check(f"{case}: the path outside the root kept", OUTSIDE in tokens,
          outside)


def interleaving(repo):
    """Two repeats of two units on both compilers, in order repeat,
    compiler, unit, tree, into the campaign log."""
    result = repo.cost("campaign", unit_spec([USES_TWICE, UNIT],
                                             compilers=["gcc", "clang"]),
                       repeats=2)
    check("campaign: exit", result.returncode, 0)
    order = [(str(r), c, u["name"], t) for r in (1, 2) for c in COMPILERS
             for u in (USES_TWICE, UNIT) for t in u["trees"]]
    rows = table(repo.read("out/campaign/rows.tsv"), ROWS)
    check("campaign: rows in order repeat, compiler, unit, tree",
          [(r.get("repeat"), r.get("compiler"), r.get("unit"), r.get("tree"))
           for r in rows], order)
    check("campaign: campaign log rows in the same order",
          [s for s, _ in repo.log_selections("out/campaign/runs.tsv")],
          [f"cost:{c}:{u}/{t}" for _, c, u, t in order])
    check("campaign: no cost row in the repository's run log",
          [s for s, _ in repo.log_selections(RUN_LOG)
           if s.startswith("cost:")], [])
    for compiler in COMPILERS:
        check(f"campaign: no compile record in build/{compiler}-debug",
              os.path.exists(repo.path("build", f"{compiler}-debug",
                                       "compile.tsv")), False)
    check("campaign: the text unit's file",
          repo.read("out/campaign/units/includer_uses_twice.cpp"),
          USES_TWICE["text"])

    before = "out/campaign/before/"
    unit_file = "out/campaign/units/includer_uses_twice.cpp"
    for row in rows:
        case = (f"campaign {row.get('repeat')} {row.get('compiler')} "
                f"{row.get('unit')}/{row.get('tree')}")
        check(f"{case}: preset", row.get("preset"),
              f"{row.get('compiler')}-debug")
        check(f"{case}: mode", row.get("mode"), "shared")
        # A counter that is unavailable stays so, never 0.
        check(f"{case}: inst_u", row.get("inst_u"), "unavailable")
        prefix = before if row.get("tree") == "before" else ""
        if row.get("unit") == UNIT["name"]:
            check_command(case, repo.top, row.get("command", ""),
                          "-DFLAGS_FROM_COMPILE_COMMANDS", prefix + "include",
                          prefix + "third_party", prefix + UNIT["source"])
        else:
            check_command(case, repo.top, row.get("command", ""),
                          "-DFLAGS_FROM_OTHER",
                          prefix + "include", prefix + "third_party",
                          unit_file, outside=True)

    summary = (repo.read("out/campaign/summary.txt") or "").splitlines()
    check("campaign: shared mode is marked",
          bool(summary) and summary[0].startswith("not a measurement"), True)
    lines = table("\n".join(summary[1:]), SUMMARY)
    check("campaign: one summary line per compiler, unit, and tree",
          sorted((s.get("compiler"), s.get("unit"), s.get("tree"))
                 for s in lines),
          sorted({(c, u, t) for _, c, u, t in order}))
    for line in lines:
        case = (f"campaign summary {line.get('compiler')} {line.get('unit')}"
                f"/{line.get('tree')}")
        check(f"{case}: repeats", line.get("repeats"), "2")
        check(f"{case}: inst_mean", line.get("inst_mean"), "unavailable")
        check(f"{case}: inst_range", line.get("inst_range"), "unavailable")


def counters(repo):
    """Known counters: the mean and range of the summary, and the sum of
    both core types' instructions in each row of shared mode."""
    env = dict(repo.env, PATH=repo.path("counting") + os.pathsep
               + repo.env["PATH"],
               COUNTING_PERF_STATE=repo.path("out", "perf-calls"))
    result = repo.cost("counted", unit_spec([dict(USES_TWICE,
                                                  trees=["after"])]),
                       repeats=3, env=env)
    check("counted: exit", result.returncode, 0)
    rows = table(repo.read("out/counted/rows.tsv"), ROWS)
    check("counted: inst_u is inst_p_u + inst_e_u in each row",
          [r.get("inst_u") for r in rows], [str(n) for n in COUNTED])
    summary = (repo.read("out/counted/summary.txt") or "").splitlines()
    lines = table("\n".join(summary[1:]), SUMMARY)
    line = lines[0] if lines else {}
    # (1010 + 4040 + 1625) / 3 = 2225; 4040 - 1010 = 3030.
    check("counted: inst_mean", line.get("inst_mean"), "2225")
    check("counted: inst_range is max - min", line.get("inst_range"), "3030")


def wrong_mode(repo):
    """A compile pinned to an efficiency core makes a shared row, which is
    not the pinned row a pinned run must read back."""
    try:
        with open("/sys/bus/event_source/devices/cpu_atom/cpus",
                  encoding="ascii") as f:
            listed = f.read().strip()
    except OSError:
        listed = ""
    efficiency = set()
    for part in filter(None, listed.split(",")):
        first, _, last = part.partition("-")
        efficiency.update(range(int(first), int(last or first) + 1))
    usable = sorted(efficiency & os.sched_getaffinity(0))
    if not usable:
        print("note: no efficiency-core CPU here; the read-back mode check "
              "was not run")
        return
    result = repo.cost("wrong_mode", unit_spec([dict(USES_TWICE,
                                                     trees=["after"])]),
                       cpu=str(usable[0]))
    check("wrong mode: exit", result.returncode, 1)


def compiler_output(repo, compiler, source):
    """The stderr lines of the compiler for source, with the flags of the
    source's own compile-fail test, and the root prefix removed."""
    program = shutil.which(COMPILERS[compiler][0])
    result = subprocess.run(
        [program, "-std=c++23", "-Wall", "-Werror", "-DFLAGS_FROM_OWN",
         f"-I{repo.top}/include", "-isystem", f"{repo.top}/third_party",
         "-fsyntax-only", repo.path(source)],
        cwd=repo.top, capture_output=True, text=True, check=False,
        env=repo.env)
    return result.stderr.replace(repo.top + "/", "").splitlines()


def diagnostics(repo):
    """Sizes of diagnostics, each source compiled on both compilers."""
    result = repo.compiles("diagnostics", "out/diag/errors.tsv", "--label",
                           "fixture", "--flags-from", "flags_source",
                           "test/*error*_fail.cpp")
    check("diagnostics: exit", result.returncode, 0)
    rows = table(repo.read("out/diag/errors.tsv"), DIAGNOSTICS)
    sources = {"test/one_error_fail.cpp": 1, "test/two_errors_fail.cpp": 2}
    check("diagnostics: rows by source, gcc then clang",
          [(r.get("source"), r.get("compiler")) for r in rows],
          [(s, c) for s in sources for c in COMPILERS])
    for row in rows:
        source, compiler = row.get("source"), row.get("compiler")
        case = f"diagnostics {source} {compiler}"
        if source not in sources or compiler not in COMPILERS:
            continue
        lines = compiler_output(repo, compiler, source)
        errors = [line for line in lines if "error:" in line]
        check(f"{case}: label", row.get("label"), "fixture")
        check(f"{case}: exit", row.get("exit") != "0", True)
        # Lines holding error:, which leaves out the notes and the
        # "1 error generated." line.
        check(f"{case}: error_lines", row.get("error_lines"),
              str(sources[source]))
        check(f"{case}: lines", row.get("lines"), str(len(lines)))
        check(f"{case}: longest", row.get("longest"),
              str(max(len(line) for line in lines)))
        check(f"{case}: first_error", row.get("first_error"), errors[0])
        check(f"{case}: first_length", row.get("first_length"),
              str(len(errors[0])))
        check(f"{case}: first error relative to the tree",
              row.get("first_error", "").startswith(source + ":"), True)
        tokens = row.get("command", "").split(" ")
        check(f"{case}: flags of its own test",
              ("-DFLAGS_FROM_OWN" in tokens, "-DFLAGS_FROM_OTHER" in tokens),
              (True, False))
        check(f"{case}: -fsyntax-only for -c and the object",
              ("-fsyntax-only" in tokens, "-c" in tokens, "-o" in tokens),
              (True, False, False))
        check(f"{case}: no launcher",
              any("tools/measure" in t for t in tokens), False)
        check(f"{case}: source last", tokens[-1:], [source])

    # A source with no test of its own takes the named test's flags, and a
    # source that compiles fails the run.
    result = repo.compiles("diagnostics", "out/diag/clean.tsv", "--label",
                           "fixture", "--flags-from", "flags_source",
                           "test/clean_fail.cpp")
    check("clean source: exit", result.returncode, 1)
    rows = table(repo.read("out/diag/clean.tsv"), DIAGNOSTICS)
    check("clean source: one row per compiler",
          [r.get("compiler") for r in rows], list(COMPILERS))
    for row in rows:
        case = f"clean source {row.get('compiler')}"
        check(f"{case}: exit", row.get("exit"), "0")
        check(f"{case}: error_lines", row.get("error_lines"), "0")
        check(f"{case}: first_error", row.get("first_error"), "-")
        check(f"{case}: first_length", row.get("first_length"), "0")
        check(f"{case}: flags of the named test",
              "-DFLAGS_FROM_OTHER" in row.get("command", "").split(" "),
              True)


def main(tools, directory):
    tools = os.path.realpath(tools)
    try:
        repo = Repo(tools, directory)
    except (OSError, RuntimeError) as error:
        print(f"setup: {error}")
        return 1
    for case in (dirty_checkout, before_tree, failing_unit, interleaving,
                 counters, wrong_mode, diagnostics):
        try:
            case(repo)
        except (OSError, RuntimeError, ValueError, KeyError,
                IndexError) as error:
            failures.append(f"{case.__name__}: {error!r}")
    for failure in failures:
        print(failure)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(*sys.argv[1:]))
