"""tools.mutate: the mutation driver's refusals, its verdict for each kind of
mutant, a library guard failing in a test and in a header, declared
survivals, the stale-binary rule, run-time mode and its leaks, restoring the
edited files, partial reruns, --check, and one output file per build tree.

Usage: python3 -I mutate_test.py <tools directory> <scratch directory>

The tools directory holds mutate, timed, and measure. The scratch directory
is wiped and gets a fresh git repository holding the project in mutate/
beside this script, with copies of the three tools committed under tools/,
so that every run row goes to the scratch repository's own run log and the
real one is never read or written. Two build trees are configured there with
Ninja, build/gcc-debug with g++ and build/clang-debug with clang++, before
the driver runs. TIMED_LOG is removed from the environment the driver gets;
the shared job pool's MAKEFLAGS passes through. Every expected value below
is worked out from the fixture's sources. Prints one line per failed check
and exits 1 if there is any.
"""

import json
import os
import shutil
import signal
import subprocess
import sys
import time

FIXTURE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "mutate")
RUN_LOG = "docs/agent/records/performance/runs.tsv"
PRESETS = {"gcc-debug": "g++", "clang-debug": "clang++"}
PROGRAMS = ["check_test", "uses_test"]
LIB = "include/lib.hpp"
CHECK = "test/check_test.cpp"
USES = "test/uses_test.cpp"
COLUMNS = ("mutant preset revision verdict expected killers build_failures "
           "not_run failed_cases leaks problems").split()

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


def fixture_text(name):
    with open(os.path.join(FIXTURE, name), encoding="utf-8") as f:
        return f.read()


def line_of(name, text):
    """The line of the fixture file that holds text, which must be one."""
    lines = [n for n, line in enumerate(fixture_text(name).splitlines(), 1)
             if text in line]
    if len(lines) != 1:
        raise RuntimeError(f"{name}: {text!r} is on lines {lines}")
    return lines[0]


# Where the fixture's checks are, from its sources.
FIRST_ASSERT = line_of(CHECK, "static_assert(lib::twice(3) == 6);")
SECOND_ASSERT = line_of(CHECK, "static_assert(lib::twice(-4) == -8);")
HALVE_CALL = line_of(USES, "lib::halve(ten)")
HALVE_RETURN = line_of(LIB, "return x / 2;")
RUNTIME_RETURN = line_of(LIB, "return x << 1;")
REJECT_CALL = line_of(LIB, "reject_negative();")
ACCEPTED = line_of(CHECK, "lib::checked(5)")
REJECT_LARGE_CALL = line_of(LIB, "reject_large();")
# below_eight's return, the first "return x;" after its guard's call.
LARGE_RETURN = next(
    n for n, line in enumerate(fixture_text(LIB).splitlines(), 1)
    if n > REJECT_LARGE_CALL and "return x;" in line)
TABLE_VALUE = line_of(LIB, "static constexpr int value = below_eight(N);")


class Repo:
    def __init__(self, tools, directory):
        self.top = os.path.realpath(directory)
        shutil.rmtree(self.top, ignore_errors=True)
        shutil.copytree(FIXTURE, self.top)
        os.makedirs(self.path("tools"))
        for tool in ("mutate", "timed", "measure"):
            shutil.copy(os.path.join(tools, tool), self.path("tools", tool))
        os.makedirs(self.path(os.path.dirname(RUN_LOG)))
        os.makedirs(self.path("specs"))
        self.write(".gitignore", "build/\n")
        self.write("README.md", "A fixture for tools/mutate.\n")
        self.env = {k: v for k, v in os.environ.items() if k != "TIMED_LOG"}
        self.git("init", "-q", "-b", "main")
        self.commit("fixture", ".")
        for preset, compiler in PRESETS.items():
            self.timed("configure", "fixture",
                       ["cmake", "-S", self.top, "-B", self.tree(preset),
                        "-G", "Ninja", f"-DCMAKE_CXX_COMPILER={compiler}",
                        "-DCMAKE_BUILD_TYPE=Debug"])

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
        return self.git("rev-parse", "--short=12", "HEAD").strip()

    def timed(self, step, selection, command):
        result = subprocess.run(
            [self.path("tools", "timed"), step, "-", selection, "--",
             *command], cwd=self.top, capture_output=True, text=True,
            check=False, env=self.env)
        if result.returncode != 0:
            raise RuntimeError(f"{' '.join(command)}: {result.stdout}"
                               f"{result.stderr}")

    def path(self, *names):
        return os.path.join(self.top, *names)

    def tree(self, preset):
        return self.path("build", preset)

    def write(self, name, text):
        with open(self.path(name), "w", encoding="utf-8") as f:
            f.write(text)

    def read(self, name):
        try:
            with open(self.path(name), encoding="utf-8") as f:
                return f.read()
        except FileNotFoundError:
            return None

    def mutate(self, spec_name, *args, env=None):
        return subprocess.run(
            [sys.executable, "-I", "tools/mutate", f"specs/{spec_name}.toml",
             *args], cwd=self.top, capture_output=True, text=True,
            check=False, env=env or self.env)

    def mutant_rows(self):
        """The run-log rows the driver's builds and tests wrote."""
        text = self.read(RUN_LOG)
        if text is None:
            return []
        lines = text.splitlines()
        column = lines[0].split("\t").index("selection")
        return [line for line in lines[1:]
                if line.split("\t")[column].startswith("mutant:")]

    def selections_logged(self):
        text = self.read(RUN_LOG) or "selection\n"
        lines = text.splitlines()
        column = lines[0].split("\t").index("selection")
        return {line.split("\t")[column] for line in lines[1:]}

    def program_times(self):
        """Each program's modification time, or None where it is absent."""
        times = {}
        for preset in PRESETS:
            for program in PROGRAMS:
                try:
                    times[preset, program] = os.stat(
                        os.path.join(self.tree(preset), program)).st_mtime_ns
                except FileNotFoundError:
                    times[preset, program] = None
        return times

    def output(self, spec_name, preset, runtime=False):
        """The header and the rows, by mutant in file order, of an output."""
        suffix = "-runtime" if runtime else ""
        text = self.read(f"specs/out/{spec_name}-{preset}{suffix}.tsv")
        if text is None:
            return None, []
        lines = text.splitlines()
        header = lines[0].split("\t")
        return header, [dict(zip(header, line.split("\t")))
                        for line in lines[1:]]


def toml(value):
    """A TOML value; JSON's strings, with \\n and \\uXXXX escapes, are TOML
    basic strings."""
    if isinstance(value, list):
        return "[" + ", ".join(toml(v) for v in value) + "]"
    if isinstance(value, dict):
        return "{ " + ", ".join(f"{k} = {toml(v)}"
                                for k, v in value.items()) + " }"
    if isinstance(value, int):
        return str(value)
    return json.dumps(value)


def write_spec(repo, name, selections, mutants, runtime=()):
    lines = [f"presets = {toml(list(PRESETS))}"]
    for selection, (targets, labels) in selections.items():
        lines += ["", f"[selection.{selection}]",
                  f"targets = {toml(targets)}", f"labels = {toml(labels)}"]
    for edit in runtime:
        lines += ["", "[[runtime]]"]
        lines += [f"{k} = {toml(v)}" for k, v in edit.items()]
    for mutant in mutants:
        lines += ["", "[[mutant]]"]
        lines += [f"{k} = {toml(v)}" for k, v in mutant.items()]
    repo.write(f"specs/{name}.toml", "\n".join(lines) + "\n")


def edit(anchor, replacement, count=None):
    e = {"anchor": anchor, "replacement": replacement}
    if count is not None:
        e["count"] = count
    return e


def mutant(name, edits, expect="killed", selection="lib", file=LIB,
           **extra):
    return {"name": name, "selection": selection, "file": file,
            "edits": edits, "expect": expect, **extra}


LIB_SELECTION = {"lib": (PROGRAMS, "^lib$")}
TWICE_WRONG_AT_RUN_TIME = [edit("x << 1", "x << 2")]
TWICE_WRONG_AT_COMPILE_TIME = [edit("2 * x", "3 * x")]
TWICE_SAME_VALUE = [edit("2 * x", "x + x")]
MESSAGE_CHANGED = [edit('"twice: the argument must be an int"',
                        '"twice: wrong argument"')]
EQUIVALENT = "x + x equals 2 * x for every int the tests use"
SYNTAX_ERROR = [edit("return x << 1;", "return x << 1")]
GUARD_INVERTED = [edit("if (x < 0)", "if (x >= 0)")]
TABLE_GUARD_NARROWED = [edit("if (x >= 8)", "if (x >= 4)")]
TABLE_OVERFLOW = [edit("    reject_large();\n  }\n  return x;",
                       "    reject_large();\n  }\n  return x * 1000000000;")]
# The table's first entry overflows and its next one trips the guard: the
# first error of the object is the overflow, whose notes name no guard.
OVERFLOW_THEN_GUARD = [
    edit("if (x >= 8)", "if (x >= 6)"),
    edit("    reject_large();\n  }\n  return x;",
         "    reject_large();\n  }\n  return x == 5 ? x * 1000000000 : x;")]
SURVIVES = "x + x equals 2 * x, which the suite before a gap's tests misses"
HALVE_EXPECTED_WRONG = [edit("lib::halve(ten) != 5", "lib::halve(ten) != 6")]
UNUSED_VARIABLE = [edit("  return x / 2;", "  T unused = x;\n  return x / 2;")]

# The main spec: one mutant of each kind, in this order.
MAIN = [
    mutant("runtime_kill", TWICE_WRONG_AT_RUN_TIME, killers=["check"]),
    mutant("assert_kill", TWICE_WRONG_AT_COMPILE_TIME,
           killers=[f"{CHECK}:{FIRST_ASSERT}"]),
    mutant("compile_fail_kill", MESSAGE_CHANGED),
    mutant("test_kill", [edit("constexpr T halve(T x)",
                              "constexpr T halve_renamed(T x)")]),
    mutant("survivor", TWICE_SAME_VALUE),
    mutant("equivalent", TWICE_SAME_VALUE, expect="equivalent",
           reason=EQUIVALENT),
    mutant("declared_equivalent_killed", TWICE_WRONG_AT_RUN_TIME,
           expect="equivalent", reason="declared so on purpose"),
    mutant("wrong_killer", MESSAGE_CHANGED, killers=["uses"]),
    mutant("warning_only", UNUSED_VARIABLE),
    mutant("invalid", SYNTAX_ERROR),
]
MAIN_NAMES = [m["name"] for m in MAIN]


def cells(text):
    return [] if text == "-" else text.split(" | ")


def check_failures(case, failures_cell, expected):
    """expected: (object file name, class, error location) per failed
    object, in any order. The object is named as Ninja names it, relative
    to the build tree, and the error line starts at the location with the
    repository root removed."""
    entries = [entry.partition(": ") for entry in cells(failures_cell)]
    # Sorted, so that the row does not depend on which job Ninja finished
    # first.
    check(f"{case}: build failures in sorted order",
          [entry for entry in cells(failures_cell)],
          sorted(cells(failures_cell)))
    check(f"{case}: number of build failures", len(entries), len(expected))
    for obj, kind, where in expected:
        found = [(name, rest) for name, _, rest in entries
                 if name.endswith(obj)]
        check(f"{case}: failures of {obj}", len(found), 1)
        if len(found) != 1:
            continue
        name, rest = found[0]
        check(f"{case}: {name!r} is relative to the build tree",
              name.startswith("/"), False)
        check(f"{case}: class and first error of {obj}",
              rest.startswith(f"{kind}: {where}") and "error:" in rest,
              True)


def expected_row(name, preset):
    """The row each main mutant must get, from the fixture's sources."""
    check_obj = "check_test.cpp.o"
    uses_obj = "uses_test.cpp.o"
    rows = {
        # A wrong value on the run-time route only: the static_asserts pass,
        # the program's run-time check fails and names itself.
        "runtime_kill": dict(verdict="killed", killers="check",
                             failed_cases=["twice at run time"]),
        # A wrong value at compile time: the first static_assert of the test
        # fails, so the object is an assert kill at that line, and the
        # program's test is listed as not run. A stale program run by CTest
        # would pass and leave not_run empty; a test passed to CTest without
        # its program would fail as not run and appear among the killers.
        "assert_kill": dict(
            verdict="killed", killers=f"{CHECK}:{FIRST_ASSERT}",
            build_failures=[(check_obj, "assert",
                             f"{CHECK}:{FIRST_ASSERT}:")],
            not_run=["check"]),
        # The compile-fail test runs whatever was built, and fails on the
        # changed message.
        "compile_fail_kill": dict(verdict="killed", killers="catch_all_fail"),
        # An error in a test source that is not a static_assert still kills.
        "test_kill": dict(
            verdict="killed", killers=f"{USES}:{HALVE_CALL}",
            build_failures=[(uses_obj, "test", f"{USES}:{HALVE_CALL}:")],
            not_run=["uses"]),
        # Same value by both routes: nothing fails.
        "survivor": dict(verdict="survived", problems=["survived"]),
        "equivalent": dict(verdict="equivalent", expected="equivalent"),
        "declared_equivalent_killed": dict(
            verdict="killed", expected="equivalent", killers="check",
            failed_cases=["twice at run time"],
            problems=["declared equivalent but killed"]),
        "wrong_killer": dict(
            verdict="killed", killers="catch_all_fail",
            problems=["declared killer uses did not kill it"]),
        # A warning made an error, in the template only uses_test calls: not
        # a kill, and uses never ran.
        "warning_only": dict(
            verdict="unjudged",
            build_failures=[(uses_obj, "warning", f"{LIB}:{HALVE_RETURN}:")],
            not_run=["uses"], problems=["unjudged"]),
        # A syntax error in the header breaks both programs and is no check.
        "invalid": dict(
            verdict="unjudged",
            build_failures=[(check_obj, "invalid", f"{LIB}:{RUNTIME_RETURN}:"),
                            (uses_obj, "invalid", f"{LIB}:{RUNTIME_RETURN}:")],
            not_run=["check", "uses"], problems=["unjudged"]),
        # The guard fires for the valid argument the test evaluates at
        # compile time, which is a kill like a failed static_assert. GCC's
        # first error is the call to the non-constexpr function, in the
        # header; Clang's is the constexpr variable in the test, whose note
        # names the call to the non-constexpr function. Both are class
        # assert, at the first error's line.
        "guard_kill": dict(
            verdict="killed",
            killers=(f"{LIB}:{REJECT_CALL}" if preset == "gcc-debug"
                     else f"{CHECK}:{ACCEPTED}"),
            build_failures=[(check_obj, "assert", f"{LIB}:{REJECT_CALL}:")
                            if preset == "gcc-debug" else
                            (check_obj, "assert", f"{CHECK}:{ACCEPTED}:")],
            not_run=["check"]),
        # The same kind of guard, reached only through the header's own
        # constexpr variable: Clang's first error is that variable, in the
        # header, which is no test file, so only its note shows the guard.
        "table_guard_kill": dict(
            verdict="killed",
            killers=(f"{LIB}:{REJECT_LARGE_CALL}" if preset == "gcc-debug"
                     else f"{LIB}:{TABLE_VALUE}"),
            build_failures=[(check_obj, "assert",
                             f"{LIB}:{REJECT_LARGE_CALL}:"
                             if preset == "gcc-debug"
                             else f"{LIB}:{TABLE_VALUE}:")],
            not_run=["check"]),
        # An overflow stops the same constant evaluation, but no check
        # failed: GCC reports the overflow in the header, and Clang the
        # header's variable, whose notes name no call to a function that is
        # not constexpr. Neither is a kill.
        "table_overflow": dict(
            verdict="unjudged",
            build_failures=[(check_obj, "invalid",
                             f"{LIB}:{LARGE_RETURN}:"
                             if preset == "gcc-debug"
                             else f"{LIB}:{TABLE_VALUE}:")],
            not_run=["check"], problems=["unjudged"]),
        # The overflow comes first and the guard's error after it, in the
        # same object: the object is classed by its first error alone, and
        # the guard's note belongs to the later error.
        "overflow_then_guard": dict(
            verdict="unjudged",
            build_failures=[(check_obj, "invalid",
                             f"{LIB}:{LARGE_RETURN}:"
                             if preset == "gcc-debug"
                             else f"{LIB}:{TABLE_VALUE}:")],
            not_run=["check"], problems=["unjudged"]),
        # A run-time change of uses_test alone: its own check fails.
        "uses_wrong": dict(verdict="killed", killers="uses",
                           failed_cases=["halve at run time"]),
        # Declared to survive: a survivor is what was expected, a kill is
        # not, and a mutant that fails to build is unjudged as any other.
        "expected_survivor": dict(verdict="survived", expected="survived"),
        "survivor_killed": dict(
            verdict="killed", expected="survived", killers="check",
            failed_cases=["twice at run time"],
            problems=["expected to survive but killed"]),
        "survivor_unjudged": dict(
            verdict="unjudged", expected="survived",
            build_failures=[(uses_obj, "warning", f"{LIB}:{HALVE_RETURN}:")],
            not_run=["uses"], problems=["unjudged"]),
    }
    row = dict(mutant=name, preset=preset, expected="killed", killers="-",
               build_failures=[], not_run=[], failed_cases=[], leaks="-",
               problems=[])
    row.update(rows[name])
    return row


def check_row(case, actual, expected):
    if actual is None:
        failures.append(f"{case}: no row")
        return
    for key in ("mutant", "preset", "verdict", "expected", "killers",
                "leaks"):
        check(f"{case}: {key}", actual.get(key), expected[key])
    check_failures(case, actual.get("build_failures", "-"),
                   expected["build_failures"])
    not_run = actual.get("not_run", "-")
    check(f"{case}: not_run", sorted([] if not_run == "-"
                                     else not_run.split(" ")),
          sorted(expected["not_run"]))
    # doctest writes "TEST CASE:  <name>"; the name is what follows.
    check(f"{case}: failed_cases",
          [c.strip() for c in cells(actual.get("failed_cases", "-"))],
          expected["failed_cases"])
    check(f"{case}: problems", sorted(cells(actual.get("problems", "-"))),
          sorted(expected["problems"]))


def check_restored(repo, case):
    """Every edited file is byte for byte as committed."""
    check(f"{case}: tracked files equal HEAD",
          repo.git("status", "--porcelain", "--untracked-files=no"), "")
    for name in (LIB, CHECK, USES):
        with open(repo.path(name), "rb") as f:
            on_disk = f.read()
        committed = subprocess.run(
            ["git", "-C", repo.top, "show", f"HEAD:{name}"],
            capture_output=True, check=True).stdout
        check(f"{case}: {name} restored byte for byte", on_disk, committed)


def check_rebuilt(repo, case, presets=PRESETS):
    times = repo.program_times()
    for preset in presets:
        for program in PROGRAMS:
            check(f"{case}: {preset} {program} rebuilt at the end",
                  times[preset, program] is not None, True)


def refusals(repo):
    """Each spec defect refuses with exit 2 before any build: the files are
    unchanged, no driver row reaches the run log, and no program exists,
    since nothing has been built yet in either tree."""
    cases = [
        ("anchor_missing", LIB_SELECTION,
         [mutant("m", [edit("3 * x", "4 * x")])], ()),
        # Occurs twice, declared once: a driver that skipped the count
        # would edit both and run the mutant.
        ("anchor_twice", LIB_SELECTION,
         [mutant("m", [edit("constexpr int twice(",
                            "constexpr long twice(")])], ()),
        ("anchor_twice_check", LIB_SELECTION,
         [mutant("m", [edit("constexpr int twice(",
                            "constexpr long twice(")])], ("--check",)),
        ("no_change", LIB_SELECTION, [mutant("m", [edit("2 * x", "2 * x")])],
         ()),
        # A label that no test carries: a selection that runs nothing must
        # not pass.
        ("empty_selection", {"none": (PROGRAMS, "^nothing$")},
         [mutant("m", TWICE_WRONG_AT_RUN_TIME, selection="none")], ()),
        # uses runs uses_test, which the selection does not build.
        ("unbuilt_program", {"partial": (["check_test"], "^lib$")},
         [mutant("m", TWICE_WRONG_AT_RUN_TIME, selection="partial")], ()),
        ("unknown_expect", LIB_SELECTION,
         [mutant("m", TWICE_SAME_VALUE, expect="survives", reason=SURVIVES)],
         ()),
        # A survival declared without saying why, or with tests that are
        # to kill it, is a spec defect.
        ("survived_without_reason", LIB_SELECTION,
         [mutant("m", TWICE_SAME_VALUE, expect="survived")], ()),
        ("survived_with_killers", LIB_SELECTION,
         [mutant("m", TWICE_SAME_VALUE, expect="survived", reason=SURVIVES,
                 killers=["check"])], ()),
    ]
    # What the refusal must say, where the case is refused only for it.
    messages = {
        "unknown_expect": "expect is killed, equivalent, or survived",
        "survived_without_reason": "is declared survived without a reason",
        "survived_with_killers": "is declared to survive but names killers",
    }
    for name, selections, mutants, args in cases:
        write_spec(repo, name, selections, mutants)
        rows = len(repo.mutant_rows())
        result = repo.mutate(name, *args)
        check(f"{name}: exit", result.returncode, 2)
        if name in messages:
            check(f"{name}: the refusal names the defect",
                  messages[name] in result.stderr, True)
        check(f"{name}: no driver row in the run log",
              len(repo.mutant_rows()), rows)
        check(f"{name}: nothing built",
              set(repo.program_times().values()), {None})
        check_restored(repo, name)

    # An uncommitted edit of the header: refused, and the edit is kept.
    committed = fixture_text(LIB)
    dirty = committed + "// an edit not yet committed\n"
    repo.write(LIB, dirty)
    write_spec(repo, "dirty", LIB_SELECTION,
               [mutant("m", TWICE_WRONG_AT_RUN_TIME)])
    rows = len(repo.mutant_rows())
    result = repo.mutate("dirty")
    check("dirty: exit", result.returncode, 2)
    check("dirty: the edit is kept", repo.read(LIB), dirty)
    check("dirty: no driver row in the run log", len(repo.mutant_rows()),
          rows)
    repo.write(LIB, committed)
    check_restored(repo, "dirty")

    # Under --runtime the run-time edit's file must be clean too: refused,
    # and its uncommitted edit kept.
    committed = fixture_text(CHECK)
    dirty = committed + "// an edit not yet committed\n"
    repo.write(CHECK, dirty)
    write_spec(repo, "dirty_runtime", LIB_SELECTION,
               [mutant("m", TWICE_WRONG_AT_COMPILE_TIME)],
               runtime=[{"file": CHECK, "anchor": "static_assert(lib::twice(",
                         "replacement": "static_assert(true || lib::twice(",
                         "count": 2}])
    rows = len(repo.mutant_rows())
    result = repo.mutate("dirty_runtime", "--runtime")
    check("dirty run-time file: exit", result.returncode, 2)
    check("dirty run-time file: the edit is kept", repo.read(CHECK), dirty)
    check("dirty run-time file: no driver row in the run log",
          len(repo.mutant_rows()), rows)
    check("dirty run-time file: nothing built",
          set(repo.program_times().values()), {None})
    repo.write(CHECK, committed)
    check_restored(repo, "dirty run-time file")


def failing_baselines(repo):
    """A baseline that fails refuses with exit 2, and no mutant runs."""
    # The test broken always fails.
    write_spec(repo, "broken", {"broken": (PROGRAMS, "^(lib|broken)$")},
               [mutant("after_broken", TWICE_WRONG_AT_RUN_TIME,
                       selection="broken")])
    result = repo.mutate("broken", "--preset", "gcc-debug")
    check("broken baseline: exit", result.returncode, 2)
    check("broken baseline: the mutant did not run",
          "mutant:after_broken" in repo.selections_logged(), False)
    check_restored(repo, "broken baseline")

    # Under --runtime the run-time edit, which breaks the run-time check,
    # applies to the baseline too, so the baseline fails. Without it the
    # baseline would pass and the mutant would be killed.
    write_spec(repo, "runtime_baseline", LIB_SELECTION,
               [mutant("after_runtime_baseline", TWICE_SAME_VALUE)],
               runtime=[{"file": CHECK, "anchor": "lib::twice(three) == 6",
                         "replacement": "lib::twice(three) == 7",
                         "count": 1}])
    result = repo.mutate("runtime_baseline", "--runtime", "--preset",
                         "gcc-debug")
    check("run-time baseline: exit", result.returncode, 2)
    check("run-time baseline: the mutant did not run",
          "mutant:after_runtime_baseline" in repo.selections_logged(),
          False)
    check_restored(repo, "run-time baseline")


def main_run(repo):
    """Every kind of mutant on both build trees, then partial reruns."""
    write_spec(repo, "main", LIB_SELECTION, MAIN)
    first = repo.git("rev-parse", "--short=12", "HEAD").strip()
    result = repo.mutate("main")
    # A survivor, unexpected kills, a wrong killer, and unjudged mutants.
    check("main: exit", result.returncode, 1)
    check_restored(repo, "main")
    check_rebuilt(repo, "main")
    # The positive control for the refusals: driver runs do reach the log.
    logged = repo.selections_logged()
    for selection in ["mutant:baseline", "mutant:restore",
                      *[f"mutant:{name}" for name in MAIN_NAMES]]:
        check(f"main: {selection} in the run log", selection in logged, True)

    lines = result.stdout.splitlines()
    problems = {line for line in lines if line.startswith("problem: ")}
    expected_problems = set()
    for preset in PRESETS:
        # One output file per build tree, its rows in spec order.
        header, rows = repo.output("main", preset)
        check(f"main {preset}: header", header, COLUMNS)
        check(f"main {preset}: rows in spec order",
              [r.get("mutant") for r in rows], MAIN_NAMES)
        by_name = {r.get("mutant"): r for r in rows}
        for name in MAIN_NAMES:
            expected = expected_row(name, preset)
            check_row(f"main {preset} {name}", by_name.get(name), expected)
            check(f"main {preset} {name}: revision",
                  by_name.get(name, {}).get("revision"), first)
            expected_problems |= {f"problem: {preset} {name}: {p}"
                                  for p in expected["problems"]}
        check(f"main {preset}: summary",
              f"{preset}: 6 killed, 1 equivalent, 1 survived, 2 unjudged"
              in lines, True)
    check("main: problem lines", problems, expected_problems)

    # Each case alone, on one tree, after a new commit so that a replaced
    # row shows the new revision.
    with open(repo.path("README.md"), "a", encoding="utf-8") as f:
        f.write("Changed so that HEAD moves.\n")
    second = repo.commit("move HEAD", "README.md")
    clang_before = repo.read("specs/out/main-clang-debug.tsv")
    alone = [("equivalent", 0), ("survivor", 1),
             ("declared_equivalent_killed", 1), ("wrong_killer", 1),
             ("warning_only", 1), ("runtime_kill", 0)]
    for name, status in alone:
        result = repo.mutate("main", "--preset", "gcc-debug", name)
        check(f"{name} alone: exit", result.returncode, status)
        check_restored(repo, f"{name} alone")
    check_rebuilt(repo, "reruns", ["gcc-debug"])
    _, rows = repo.output("main", "gcc-debug")
    check("reruns: every row kept, in spec order",
          [r.get("mutant") for r in rows], MAIN_NAMES)
    rerun = {name for name, _ in alone}
    for row in rows:
        name = row.get("mutant")
        check(f"reruns: revision of {name}", row.get("revision"),
              second if name in rerun else first)
        if name in rerun:
            check_row(f"rerun {name}", row, expected_row(name, "gcc-debug"))
    check("reruns: the clang-debug output is untouched",
          repo.read("specs/out/main-clang-debug.tsv"), clang_before)


def rows_in_spec_order(repo):
    """A run that adds a mutant missing from the output file puts its row in
    spec order, not after the rows already there."""
    write_spec(repo, "order", LIB_SELECTION,
               [mutant("first", TWICE_WRONG_AT_RUN_TIME),
                mutant("second", TWICE_SAME_VALUE)])
    repo.mutate("order", "--preset", "gcc-debug", "second")
    repo.mutate("order", "--preset", "gcc-debug", "first")
    check_restored(repo, "order")
    _, rows = repo.output("order", "gcc-debug")
    check("order: rows in spec order after the later mutant ran first",
          [r.get("mutant") for r in rows], ["first", "second"])


def killers_sorted(repo):
    """The killers are the failed tests' names sorted, then the locations
    sorted, so that a row does not depend on the order in which CTest lists
    tests or Ninja finishes objects. CTest numbers check before
    catch_all_fail, which sorts first."""
    write_spec(repo, "killers", LIB_SELECTION, [
        mutant("two_tests", TWICE_WRONG_AT_RUN_TIME + MESSAGE_CHANGED),
        mutant("two_objects", TWICE_WRONG_AT_COMPILE_TIME + [
            edit("constexpr T halve(T x)", "constexpr T halve_renamed(T x)")]),
    ])
    result = repo.mutate("killers")
    check("killers: exit", result.returncode, 0)
    check_restored(repo, "killers")
    for preset in PRESETS:
        _, rows = repo.output("killers", preset)
        by_name = {r.get("mutant"): r for r in rows}
        check(f"killers {preset}: two tests",
              by_name.get("two_tests", {}).get("killers"),
              "catch_all_fail check")
        check(f"killers {preset}: two objects",
              by_name.get("two_objects", {}).get("killers"),
              f"{CHECK}:{FIRST_ASSERT} {USES}:{HALVE_CALL}")


# A ctest that, while the header holds the marker, fails every run other
# than a listing at once, with no output: a CTest that stops before it
# names any test, as when it cannot read its input.
MUTED_CTEST = """#!/bin/sh
case " $* " in
  *" --show-only"*) ;;
  *) if grep -q "mute ctest" {header}; then exit 3; fi ;;
esac
exec {ctest} "$@"
"""


def ctest_without_failures(repo):
    """CTest failing without naming a failed test leaves the mutant
    unjudged, never survived or killed."""
    muted = repo.path("muted")
    os.makedirs(muted, exist_ok=True)
    wrapper = os.path.join(muted, "ctest")
    with open(wrapper, "w", encoding="utf-8") as f:
        f.write(MUTED_CTEST.format(header=repo.path(LIB),
                                   ctest=shutil.which("ctest")))
    os.chmod(wrapper, 0o755)
    env = dict(repo.env, PATH=muted + os.pathsep + repo.env["PATH"])
    write_spec(repo, "muted", LIB_SELECTION, [mutant("muted", [
        edit("namespace lib {", "namespace lib { // mute ctest")])])
    result = repo.mutate("muted", "--preset", "gcc-debug", env=env)
    check("muted CTest: exit", result.returncode, 1)
    check_restored(repo, "muted CTest")
    check_rebuilt(repo, "muted CTest", ["gcc-debug"])
    _, rows = repo.output("muted", "gcc-debug")
    row = rows[0] if rows else {}
    check("muted CTest: verdict", row.get("verdict"), "unjudged")
    check("muted CTest: the problem named",
          "CTest exited 3 without naming a failed test"
          in cells(row.get("problems", "-")), True)


# A ctest that logs its arguments before it runs.
LOGGING_CTEST = """#!/bin/sh
printf '%s\\n' "$*" >> {log}
exec {ctest} "$@"
"""


def ctest_in_parallel(repo):
    """The selected tests run on the shared pool, in the baseline and in each
    mutant: CTest is called with --parallel and no number, which takes one
    token of the pool per test."""
    wrapped = repo.path("logged")
    os.makedirs(wrapped, exist_ok=True)
    log = repo.path("logged", "ctest-arguments.txt")
    if os.path.exists(log):
        os.unlink(log)
    wrapper = os.path.join(wrapped, "ctest")
    with open(wrapper, "w", encoding="utf-8") as f:
        f.write(LOGGING_CTEST.format(log=log, ctest=shutil.which("ctest")))
    os.chmod(wrapper, 0o755)
    env = dict(repo.env, PATH=wrapped + os.pathsep + repo.env["PATH"])
    write_spec(repo, "parallel", LIB_SELECTION, [
        mutant("same_value", TWICE_SAME_VALUE, expect="equivalent",
               reason=EQUIVALENT)])
    result = repo.mutate("parallel", "--preset", "gcc-debug", env=env)
    check("parallel: exit", result.returncode, 0)
    check_restored(repo, "parallel")
    check_rebuilt(repo, "parallel", ["gcc-debug"])
    with open(log, encoding="utf-8") as f:
        runs = [line.split() for line in f.read().splitlines()
                if "--tests-from-file" in line]
    # The baseline and the one mutant.
    check("parallel: runs of the selection", len(runs), 2)
    for arguments in runs:
        where = (arguments.index("--parallel")
                 if "--parallel" in arguments else None)
        check("parallel: the run carries --parallel", where is not None, True)
        if where is not None:
            following = arguments[where + 1:where + 2]
            check("parallel: --parallel takes no number",
                  all(a.startswith("-") for a in following), True)


def restore_between_files(repo):
    """A mutant whose build fails is restored before the next one, which
    edits another file and must get the row it gets alone: built against a
    header left broken, it would be unjudged instead of killed."""
    names = ["invalid", "uses_wrong"]
    write_spec(repo, "two_files", LIB_SELECTION,
               [mutant("invalid", SYNTAX_ERROR),
                mutant("uses_wrong", HALVE_EXPECTED_WRONG, file=USES,
                       killers=["uses"])])
    result = repo.mutate("two_files")
    check("two files: exit", result.returncode, 1)
    check_restored(repo, "two files")
    check_rebuilt(repo, "two files")
    for preset in PRESETS:
        _, rows = repo.output("two_files", preset)
        check(f"two files {preset}: rows", [r.get("mutant") for r in rows],
              names)
        by_name = {r.get("mutant"): r for r in rows}
        for name in names:
            check_row(f"two files {preset} {name}", by_name.get(name),
                      expected_row(name, preset))


def library_guard(repo):
    """A library guard that rejects a value during constant evaluation
    kills the mutant that makes it reject a valid one, whether the failed
    evaluation is the test's or a constexpr variable's in the header. An
    overflow in the header's evaluation is no guard and kills nothing, even
    when a guard fails after it in the same object."""
    names = ["guard_kill", "table_guard_kill", "table_overflow",
             "overflow_then_guard"]
    write_spec(repo, "guard", LIB_SELECTION,
               [mutant("guard_kill", GUARD_INVERTED),
                mutant("table_guard_kill", TABLE_GUARD_NARROWED),
                mutant("table_overflow", TABLE_OVERFLOW),
                mutant("overflow_then_guard", OVERFLOW_THEN_GUARD)])
    result = repo.mutate("guard")
    check("guard: exit", result.returncode, 1)
    check_restored(repo, "guard")
    check_rebuilt(repo, "guard")
    for preset in PRESETS:
        _, rows = repo.output("guard", preset)
        check(f"guard {preset}: rows", [r.get("mutant") for r in rows], names)
        by_name = {r.get("mutant"): r for r in rows}
        for name in names:
            check_row(f"guard {preset} {name}", by_name.get(name),
                      expected_row(name, preset))
    check("guard: problem lines",
          {line for line in result.stdout.splitlines()
           if line.startswith("problem: ")},
          {f"problem: {preset} {name}: unjudged" for preset in PRESETS
           for name in ("table_overflow", "overflow_then_guard")})


def declared_survivals(repo):
    """A mutant declared to survive, with its reason, is no problem when it
    survives, and is one when it is killed or unjudged. A spec of expected
    survivors alone exits 0."""
    names = ["expected_survivor", "survivor_killed", "survivor_unjudged"]
    write_spec(repo, "survivals", LIB_SELECTION, [
        mutant("expected_survivor", TWICE_SAME_VALUE, expect="survived",
               reason=SURVIVES),
        mutant("survivor_killed", TWICE_WRONG_AT_RUN_TIME, expect="survived",
               reason=SURVIVES),
        mutant("survivor_unjudged", UNUSED_VARIABLE, expect="survived",
               reason=SURVIVES)])
    result = repo.mutate("survivals", "--preset", "gcc-debug")
    check("survivals: exit", result.returncode, 1)
    check_restored(repo, "survivals")
    check_rebuilt(repo, "survivals", ["gcc-debug"])
    _, rows = repo.output("survivals", "gcc-debug")
    check("survivals: rows", [r.get("mutant") for r in rows], names)
    by_name = {r.get("mutant"): r for r in rows}
    for name in names:
        check_row(f"survivals {name}", by_name.get(name),
                  expected_row(name, "gcc-debug"))
    check("survivals: problem lines",
          {line for line in result.stdout.splitlines()
           if line.startswith("problem: ")},
          {"problem: gcc-debug survivor_killed: expected to survive but "
           "killed", "problem: gcc-debug survivor_unjudged: unjudged"})
    check("survivals: summary",
          "gcc-debug: 1 killed, 0 equivalent, 1 survived, 1 unjudged"
          in result.stdout.splitlines(), True)

    result = repo.mutate("survivals", "--preset", "gcc-debug",
                         "expected_survivor")
    check("expected survivor alone: exit", result.returncode, 0)
    check_restored(repo, "expected survivor alone")


def runtime_mode(repo):
    """--runtime turns off static_asserts of the test for the whole run."""
    assert_kill = mutant("assert_kill", TWICE_WRONG_AT_COMPILE_TIME)
    # Only the first static_assert is turned off; the second still stops
    # the build, which is a leak at its line.
    write_spec(repo, "leak", LIB_SELECTION, [assert_kill],
               runtime=[{"file": CHECK,
                         "anchor": "static_assert(lib::twice(3)",
                         "replacement": "static_assert(true || lib::twice(3)",
                         "count": 1}])
    result = repo.mutate("leak", "--runtime")
    check("leak: exit", result.returncode, 1)
    check_restored(repo, "leak")
    check_rebuilt(repo, "leak")
    where = f"{CHECK}:{SECOND_ASSERT}"
    for preset in PRESETS:
        _, rows = repo.output("leak", preset, runtime=True)
        row = rows[0] if rows else {}
        check(f"leak {preset}: leaks", row.get("leaks"), where)
        check(f"leak {preset}: problems", row.get("problems"),
              f"run-time leak at {where}")
        check(f"leak {preset}: problem line",
              f"problem: {preset} assert_kill: run-time leak at {where}"
              in result.stdout.splitlines(), True)

    # Both turned off: the mutant compiles and the run-time comparison of
    # the folded value kills it.
    write_spec(repo, "runtime", LIB_SELECTION, [assert_kill],
               runtime=[{"file": CHECK, "anchor": "static_assert(lib::twice(",
                         "replacement": "static_assert(true || lib::twice(",
                         "count": 2}])
    result = repo.mutate("runtime", "--runtime")
    check("run-time mode: exit", result.returncode, 0)
    check_restored(repo, "run-time mode")
    check_rebuilt(repo, "run-time mode")
    for preset in PRESETS:
        _, rows = repo.output("runtime", preset, runtime=True)
        check_row(f"run-time mode {preset}", rows[0] if rows else None,
                  dict(mutant="assert_kill", preset=preset, verdict="killed",
                       expected="killed", killers="check", build_failures=[],
                       not_run=[], leaks="-", problems=[],
                       failed_cases=["twice folded at compile time"]))


# The diff of the two-edit mutant, worked out by hand from lib.hpp: both
# edits fall within one hunk of three lines of context, lines 9 to 17.
TWO_EDIT_DIFF = """\
--- a/include/lib.hpp
+++ b/include/lib.hpp
@@ -9,9 +9,9 @@
 // test can change one route and leave the other.
 constexpr int twice(int x) {
   if consteval {
-    return 2 * x;
+    return x + x;
   } else {
-    return x << 1;
+    return x + x;
   }
 }
""" + " \n"


def check_mode(repo):
    """--check prints each mutant's diff and builds nothing."""
    write_spec(repo, "diff", LIB_SELECTION,
               [mutant("two_edits", [edit("2 * x", "x + x"),
                                     edit("x << 1", "x + x")])])
    rows = len(repo.mutant_rows())
    times = repo.program_times()
    result = repo.mutate("diff", "--check")
    check("--check: exit", result.returncode, 0)
    check("--check: no driver row", len(repo.mutant_rows()), rows)
    check("--check: no program rebuilt", repo.program_times(), times)
    check_restored(repo, "--check")
    lines = [line.rstrip() for line in result.stdout.splitlines()]
    expected = [line.rstrip() for line in TWO_EDIT_DIFF.splitlines()]
    header = f"== two_edits {LIB}"
    start = lines.index(header) + 1 if header in lines else None
    check("--check: the mutant's header line", start is not None, True)
    if start is not None:
        check("--check: the diff", lines[start:start + len(expected)],
              expected)


def stopped_early(repo):
    """SIGTERM, then SIGINT, while a mutant is applied, after its program was
    deleted: the driver exits 128 plus the signal's number with the files as
    committed and the selection's programs built again, so that no later
    CTest run finds one missing. Last of the cases, since the build the
    signal cuts short may still be finishing when the driver exits."""
    for number in (signal.SIGTERM, signal.SIGINT):
        stop_once(repo, number)


def stop_once(repo, number):
    name = signal.Signals(number).name
    write_spec(repo, "stop", LIB_SELECTION,
               [mutant(f"stop_{n}", TWICE_WRONG_AT_COMPILE_TIME)
                for n in range(8)])
    committed = fixture_text(LIB)
    deleted = os.path.join(repo.tree("gcc-debug"), "check_test")
    process = subprocess.Popen(
        [sys.executable, "-I", "tools/mutate", "specs/stop.toml",
         "--preset", "gcc-debug"], cwd=repo.top, env=repo.env,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    signalled = False
    deadline = time.monotonic() + 120
    while process.poll() is None and time.monotonic() < deadline:
        if repo.read(LIB) != committed and not os.path.exists(deleted):
            process.send_signal(number)
            signalled = True
            break
        time.sleep(0.005)
    status = process.wait(timeout=300)
    check(f"{name}: signalled while a mutant was applied", signalled, True)
    check(f"{name}: exit", status, 128 + number)
    check_restored(repo, name)
    check_rebuilt(repo, name, ["gcc-debug"])


def failed_rebuild_reported(repo):
    """A run that stops early still rebuilds, and says so when the rebuild
    fails: with uses_test.cpp broken in a commit, the baseline does not
    build, the run is refused, and the rebuild that follows fails as well,
    which stderr must name. The commit is then undone by another."""
    committed = fixture_text(USES)
    repo.write(USES, committed + "this is not C++\n")
    repo.commit("break uses_test.cpp", USES)
    write_spec(repo, "broken_build", LIB_SELECTION,
               [mutant("after_broken_build", TWICE_WRONG_AT_RUN_TIME)])
    result = repo.mutate("broken_build", "--preset", "gcc-debug")
    check("broken build: exit", result.returncode, 2)
    check("broken build: the failed rebuild is named on stderr",
          "the restored files do not build" in result.stderr, True)
    repo.write(USES, committed)
    repo.commit("mend uses_test.cpp", USES)
    check_restored(repo, "broken build")


def main(tools, directory):
    tools = os.path.realpath(tools)
    try:
        repo = Repo(tools, directory)
    except (OSError, RuntimeError) as error:
        print(f"setup: {error}")
        return 1
    for case in (refusals, failing_baselines, main_run,
                 rows_in_spec_order, killers_sorted, ctest_without_failures,
                 ctest_in_parallel, restore_between_files, library_guard,
                 declared_survivals, runtime_mode, check_mode,
                 failed_rebuild_reported, stopped_early):
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
