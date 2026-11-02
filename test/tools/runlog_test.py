"""tools.runlog: the history that tools/runlog distills from a run log, the
rows a compaction keeps, its refusals, and union merges across a compaction.

Usage: python3 -I runlog_test.py <tools/runlog> <scratch directory>

Each case builds a fresh git repository under the scratch directory, with
the union merge rule the real repository uses for its logs, writes its own
run log there, and runs the tool in it; the real run log is never read.
Prints one line per failed check and exits 1 if there is any.
"""

import os
import re
import shutil
import subprocess
import sys

LOG = "docs/agent/records/performance/runs.tsv"
HISTORY = "docs/agent/records/performance/history"
COLUMNS = ["utc_start", "revision", "branch", "step", "preset", "selection",
           "wall_s", "exit", "host", "mode", "cpus", "user_s", "sys_s",
           "maxrss_kib", "inst_p_u", "inst_e_u", "cycles_u", "l1d_loads_u",
           "l1d_misses_u", "l2_refs_u", "l2_misses_u", "llc_refs_u",
           "llc_misses_u", "energy_pkg_j", "other_cpu_s", "sibling_busy_s",
           "pinned_other_s", "options"]
HEADER = "\t".join(COLUMNS)
GROUP_HEADER = (
    "era\tlog\tstep\tpreset\tfamily\tmode\toutcome\trows\tdirty\trevisions\t"
    "first_utc\tlatest_utc\tfirst_revision\tlatest_revision\thosts\t"
    "toolchains\twall_s_n\twall_s_first\twall_s_latest\twall_s_min\t"
    "wall_s_median\twall_s_range\tmaxrss_kib_n\tmaxrss_kib_first\t"
    "maxrss_kib_latest\tmaxrss_kib_min\tmaxrss_kib_median\tmaxrss_kib_range\t"
    "inst_u_n\tinst_u_first\tinst_u_latest\tinst_u_min\tinst_u_median\t"
    "inst_u_range")
TOOLCHAIN = (
    "since_utc\thost\tgcc\tlibstdcxx\tclang\tcmake\tninja\tpython\tperf\t"
    "kernel\tsource\n"
    "2026-01-01T00:00:00Z\th1\t16\t16\t23\t4.4\t1.13\t3.14\t7.3\t7.3\tfixture\n"
    "2026-01-01T00:05:00Z\th1\t17\t17\t23\t4.4\t1.13\t3.14\t7.3\t7.3\tfixture\n")
NA = "unavailable"

failures = []


def check(name, actual, expected):
    if actual != expected:
        failures.append(f"{name}: got {actual!r}, expected {expected!r}")


def row(utc, revision, step, preset, selection, wall, exit_status="0",
        host="h1", mode="shared", maxrss="100", inst_p=NA, inst_e=NA):
    cells = {c: NA for c in COLUMNS}
    cells.update(utc_start=utc, revision=revision, branch="dev", step=step,
                 preset=preset, selection=selection, wall_s=wall,
                 exit=exit_status, host=host, mode=mode, cpus="0-31",
                 maxrss_kib=maxrss, inst_p_u=inst_p, inst_e_u=inst_e,
                 options="-")
    return "\t".join(cells[c] for c in COLUMNS)


def at(minute, second=0):
    return f"2026-01-01T00:{minute:02d}:{second:02d}Z"


def kept_row(n):
    """A build of a committed revision in a named preset, which stays."""
    return row(at(10, n), f"{n:012x}", "build", "gcc-debug", "all", "1.000")


def probe_row(n):
    """A measurement probe, which a compaction removes."""
    return row(at(20, n), f"{n:012x}", "measure", "-", f"probe:case{n}",
               "0.100")


class Repo:
    def __init__(self, tool, directory):
        self.tool = tool
        self.top = os.path.realpath(directory)
        shutil.rmtree(self.top, ignore_errors=True)
        os.makedirs(os.path.join(self.top, HISTORY))
        self.git("init", "-q", "-b", "dev")
        self.write(".gitattributes", "*.tsv merge=union\n")
        self.write(f"{HISTORY}/toolchain.tsv", TOOLCHAIN)
        self.git("add", ".")
        self.git("commit", "-q", "-m", "start")

    def git(self, *args):
        env = dict(os.environ, GIT_CONFIG_GLOBAL="/dev/null",
                   GIT_CONFIG_NOSYSTEM="1")
        result = subprocess.run(
            ["git", "-C", self.top, "-c", "user.name=test",
             "-c", "user.email=test@example.invalid", *args],
            capture_output=True, text=True, check=False, env=env)
        if result.returncode != 0:
            raise RuntimeError(f"git {' '.join(args)}: {result.stderr}")
        return result.stdout.strip()

    def path(self, name):
        return os.path.join(self.top, name)

    def write(self, name, text):
        with open(self.path(name), "w", encoding="utf-8") as f:
            f.write(text)

    def read(self, name):
        try:
            with open(self.path(name), encoding="utf-8") as f:
                return f.read()
        except FileNotFoundError:
            return None

    def write_log(self, rows):
        self.write(LOG, "\n".join([HEADER, *rows]) + "\n")

    def log_rows(self):
        return self.read(LOG).splitlines()[1:]

    def commit(self, message):
        self.git("add", "-A")
        self.git("commit", "-q", "-m", message)
        return self.git("rev-parse", "--short=12", "HEAD")

    def run(self, *args):
        return subprocess.run([sys.executable, "-I", self.tool, *args],
                              cwd=self.top, capture_output=True, text=True,
                              check=False)

    def history_files(self):
        return [self.read(name) for name in
                (LOG, f"{HISTORY}/eras.tsv", f"{HISTORY}/groups.tsv")]


def summary_values(tool, directory):
    """Case 1: the statistics of each group."""
    repo = Repo(tool, directory)
    rows = [
        # A group of four passing rows, out of time order in the log.
        row(at(0, 1), "a" * 12, "test", "gcc-debug", "^core$", "3.000",
            maxrss="400", inst_p="100", inst_e="50"),
        row(at(0, 4), "b" * 12, "test", "gcc-debug", "^core$", "2.000",
            maxrss="100", inst_p="100", inst_e=NA),
        row(at(0, 2), "a" * 12 + "-dirty", "test", "gcc-debug", "^core$",
            "1.000", maxrss="300", inst_p="10", inst_e="0"),
        row(at(0, 3), "c" * 12, "test", "gcc-debug", "^core$", "4.000",
            maxrss="200", inst_p="7", inst_e=NA),
        # The same selection failing.
        row(at(0, 5), "d" * 12, "test", "gcc-debug", "^core$", "9.000",
            exit_status="1", maxrss="50"),
        # Two pinned rows that started in the same second, on two hosts.
        row(at(1), "e" * 12, "build", "clang-debug", "all:final-x", "5.000",
            mode="pinned", maxrss="10", inst_p="1000"),
        row(at(1), "f" * 12, "build", "clang-debug", "all:final-y", "6.000",
            host="h2", mode="pinned", maxrss="20", inst_p="2000"),
        # A row from before the cost columns, and before the first
        # toolchain state.
        ("2025-12-31T23:00:00Z\tgggggggggggg\tdev\tbuild\tgcc-debug\tall\t"
         "7.500\t0\th1"),
        # Two probes of one campaign family.
        row(at(2), "1" * 12, "measure", "-", "ceiling-cert:case1:L5", "0.010",
            maxrss="20000", inst_p="5", inst_e="7"),
        row(at(2, 1), "1" * 12, "measure", "-", "ceiling-cert:case2:L9",
            "0.012", maxrss="20001", inst_p="6", inst_e="0"),
        # Values with other numbers of decimals.
        row(at(3), "2" * 12, "workflow", "lint", "all-clean", "2.5",
            maxrss="5"),
        row(at(4), "3" * 12, "workflow", "lint", "all-clean", "12.25",
            maxrss="7"),
        # A run at the very second the second toolchain state begins: only
        # that state was in force.
        row(at(5), "4" * 12, "compile", "-", "x", "1.000", maxrss="9"),
    ]
    repo.write_log(rows)
    result = repo.run("summary", repo.path(LOG))
    t0 = "2026-01-01T00:00:00Z"
    expected = [GROUP_HEADER] + ["\t".join(cells) for cells in [
        ["-", "-", "build", "clang-debug", "all", "pinned", "pass", "2", "0",
         "2", at(1), at(1), "e" * 12, "f" * 12, "h1,h2", f"{t0},unrecorded",
         "2", "5.000", "6.000", "5.000", "5.000", "1.000",
         "2", "10", "20", "10", "10", "10",
         "2", "1000", "2000", "1000", "1000", "1000"],
        ["-", "-", "build", "gcc-debug", "all", "--", "pass", "1", "0", "1",
         "2025-12-31T23:00:00Z", "2025-12-31T23:00:00Z", "g" * 12, "g" * 12,
         "h1", "unrecorded",
         "1", "7.500", "7.500", "7.500", "7.500", "0.000",
         "0", NA, NA, NA, NA, NA, "0", NA, NA, NA, NA, NA],
        ["-", "-", "compile", "-", "x", "shared", "pass", "1", "0", "1",
         at(5), at(5), "4" * 12, "4" * 12, "h1", "2026-01-01T00:05:00Z",
         "1", "1.000", "1.000", "1.000", "1.000", "0.000",
         "1", "9", "9", "9", "9", "0", "0", NA, NA, NA, NA, NA],
        ["-", "-", "measure", "-", "ceiling-cert", "shared", "pass", "2", "0",
         "1", at(2), at(2, 1), "1" * 12, "1" * 12, "h1", t0,
         "2", "0.010", "0.012", "0.010", "0.010", "0.002",
         "2", "20000", "20001", "20000", "20000", "1",
         "2", "12", "6", "6", "6", "6"],
        ["-", "-", "test", "gcc-debug", "^core$", "shared", "fail", "1", "0",
         "1", at(0, 5), at(0, 5), "d" * 12, "d" * 12, "h1", t0,
         "1", "9.000", "9.000", "9.000", "9.000", "0.000",
         "1", "50", "50", "50", "50", "0", "0", NA, NA, NA, NA, NA],
        ["-", "-", "test", "gcc-debug", "^core$", "shared", "pass", "4", "1",
         "3", at(0, 1), at(0, 4), "a" * 12, "b" * 12, "h1", t0,
         "4", "3.000", "2.000", "1.000", "2.000", "3.000",
         "4", "400", "100", "100", "200", "300",
         "2", "150", "10", "10", "10", "140"],
        ["-", "-", "workflow", "lint", "all-clean", "shared", "pass", "2",
         "0", "2", at(3), at(4), "2" * 12, "3" * 12, "h1", t0,
         "2", "2.5", "12.25", "2.5", "2.5", "9.75",
         "2", "5", "7", "5", "5", "2", "0", NA, NA, NA, NA, NA],
    ]]
    check("summary exit", result.returncode, 0)
    got = result.stdout.splitlines()
    check("summary row count", len(got), len(expected))
    for i, (g, e) in enumerate(zip(got, expected)):
        check(f"summary row {i}", g, e)

    # A log from before the cost columns has no mode column at all.
    nine = "\t".join(COLUMNS[:9])
    repo.write("nine.tsv", f"{nine}\n{rows[7]}\n")
    result = repo.run("summary", repo.path("nine.tsv"))
    check("summary of a nine-column log", result.stdout.splitlines()[1:],
          [expected[2]])

    # A blank line, as a hand edit or a lost row can leave, is refused.
    repo.write("blank.tsv", f"{HEADER}\n{rows[0]}\n\n{rows[1]}\n")
    result = repo.run("summary", repo.path("blank.tsv"))
    check("summary of a log with a blank line", result.returncode, 2)
    if "blank.tsv:3 is blank" not in result.stderr:
        failures.append(f"blank line stderr: {result.stderr!r}")

    # A second header line, as a doubled union merge leaves, is refused.
    repo.write("doubled.tsv", f"{HEADER}\n{rows[0]}\n{HEADER}\n{rows[1]}\n")
    result = repo.run("summary", repo.path("doubled.tsv"))
    check("summary of a log with two headers", result.returncode, 2)
    if "is a second header line" not in result.stderr:
        failures.append(f"second header stderr: {result.stderr!r}")


def kept_by_rule(tool, directory):
    """Case 2: which rows a compaction leaves in the log."""
    repo = Repo(tool, directory)
    clean, dirty = "a" * 12, "a" * 12 + "-dirty"
    kept = [
        row(at(1), clean, "build", "gcc-debug", "all", "1.0"),
        row(at(2), clean, "test", "clang-asan", "^core$", "1.0"),
        row(at(3), clean, "workflow", "lint", "all-clean", "1.0"),
        row(at(4), clean, "configure", "gcc-debug", "fresh", "1.0"),
        row(at(5), clean, "gate", "-", "trace", "1.0"),
        # A workflow of a named preset stays at any revision, since a
        # merge's checks may run on the uncommitted merged tree.
        row(at(6), dirty, "workflow", "gcc-debug", "all-clean", "1.0"),
        row(at(7), "unavailable", "workflow", "clang-asan", "all", "1.0"),
        # Style checks.
        row(at(8), clean, "lint", "-", "ruff", "1.0"),
        row(at(9), clean, "lint", "lint", "clang-tidy units", "1.0"),
    ]
    # Each cheap gate's family, as a check step and as a trace step.
    gates = ("trace", "trace-gate", "convention-trace", "markers", "golden",
             "citations", "golden-facts", "reference-markers", "golden-check",
             "conventions_check")
    kept += [row(at(10, i), clean, "check", "-", f"{name}:x", "1.0")
             for i, name in enumerate(gates)]
    kept += [row(at(11, i), clean, "trace", "-", name, "1.0")
             for i, name in enumerate(gates)]
    removed = [
        row(at(20), dirty, "build", "gcc-debug", "all", "1.0"),
        row(at(21), "unavailable", "build", "gcc-debug", "all", "1.0"),
        row(at(22), clean, "build", "-", "sketch-a", "1.0"),
        row(at(23), clean, "measure", "-", "ceiling-cert:x:L1", "1.0"),
        row(at(24), clean, "probe", "-", "p", "1.0"),
        row(at(25), clean, "check", "-", "metric-like-holes:a", "1.0"),
        row(at(26), clean, "spike", "-", "api-design:run", "1.0"),
        row(at(27), dirty, "gate", "-", "trace", "1.0"),
        row(at(28), clean, "workflow", "-", "all", "1.0"),
        row(at(29), dirty, "lint", "-", "ruff", "1.0"),
        row(at(30), dirty, "check", "-", "golden-facts", "1.0"),
        row(at(31), clean, "check", "-", "golden-facts-probe", "1.0"),
    ]
    # The last row is one the rule keeps, so that the boundary's anchor
    # adds nothing.
    last = row(at(40), clean, "build", "gcc-release", "all", "1.0")
    mixed = []
    for i in range(max(len(kept), len(removed))):
        mixed += kept[i:i + 1] + removed[i:i + 1]
    repo.write_log(mixed + [last])
    boundary = repo.commit("log")
    result = repo.run("compact", boundary)
    check("compact exit (rule)", result.returncode, 0)
    check("rows kept by the rule", repo.log_rows(), [*kept, last])


def compacted_repo(tool, directory):
    """The repository of cases 3 to 8: commits A and B on dev, a branch
    `old` at A with two rows of its own, and B compacted on branch
    `compacting`, with three rows written after B."""
    repo = Repo(tool, directory)
    a = [probe_row(1), kept_row(2), probe_row(3), probe_row(4), probe_row(5)]
    repo.write_log(a)
    repo.commit("A")
    repo.git("branch", "old")
    b = [probe_row(6), probe_row(7), kept_row(8), probe_row(9),
         probe_row(10), probe_row(11)]
    repo.write_log(a + b)
    boundary = repo.commit("B")
    repo.git("checkout", "-q", "old")
    repo.write_log(a + [kept_row(30), probe_row(31)])
    repo.commit("old rows")
    repo.git("checkout", "-q", "-b", "compacting", "dev")
    later = [kept_row(40), probe_row(41), probe_row(42)]
    repo.write_log(a + b + later)
    return repo, boundary, a, b, later


def compaction(tool, directory):
    """Case 3: the log, era, and groups a compaction writes."""
    repo, boundary, a, b, later = compacted_repo(tool, directory)
    result = repo.run("compact", boundary)
    check("compact exit", result.returncode, 0)
    # Kept: a[1] and b[2] by the rule; a[4] (line 6), the last row at old's
    # base, and b[0] (line 7) after it; b[5] (line 12), the boundary's last.
    check("compacted log", repo.log_rows(),
          [a[1], a[4], b[0], b[2], b[5], *later])
    check("era row", repo.read(f"{HISTORY}/eras.tsv").splitlines(),
          ["era\tlog\trows\tkept\tanchors\tfirst_utc\tlatest_utc",
           f"{boundary}\t{LOG}\t11\t5\t6,7,12\t{at(10, 2)}\t{at(20, 11)}"])
    groups = repo.read(f"{HISTORY}/groups.tsv").splitlines()
    check("group header", groups[0], GROUP_HEADER)
    check("group families", [g.split("\t")[2:8] for g in groups[1:]],
          [["build", "gcc-debug", "all", "shared", "pass", "2"],
           ["measure", "-", "probe", "shared", "pass", "9"]])
    check("compact report", result.stdout.strip(),
          f"runlog: era {boundary} of {LOG}: 11 rows in 2 groups; 5 kept, 3 "
          "of them anchors; the log went from 14 to 8 rows")
    return repo, boundary, a, b, later


def checks(tool, directory):
    """Case 4: check passes on a compaction and finds each kind of damage."""
    repo, boundary, a, b, later = compaction(tool, directory)
    repo.commit("compact")
    check("check after compaction", repo.run("check").returncode, 0)
    good = repo.history_files()

    def damaged(name, edit, message):
        text = repo.read(name)
        repo.write(name, edit(text))
        result = repo.run("check")
        check(f"check exit, {message}", result.returncode, 1)
        if message not in result.stderr:
            failures.append(f"check stderr, {message}: {result.stderr!r}")
        repo.write(name, text)

    damaged(LOG, lambda t: t + a[0] + "\n",
            "a row an era removed is back (line 10)")
    damaged(LOG, lambda t: t.replace(a[1] + "\n", ""),
            "a row an era kept is there 0 times, not 1")
    damaged(LOG, lambda t: t + a[1] + "\n",
            "a row an era kept is there 2 times, not 1")
    damaged(f"{HISTORY}/groups.tsv", lambda t: t.replace("\t9\t", "\t8\t", 1),
            "group row 2 is")
    damaged(f"{HISTORY}/eras.tsv", lambda t: t.replace("\t11\t", "\t12\t"),
            f"era {boundary} of {LOG} is")
    unknown = "\t".join(["f" * 12, LOG] + ["x"] * 32) + "\n"
    damaged(f"{HISTORY}/groups.tsv", lambda t: t + unknown,
            f"has a row of era {'f' * 12} of {LOG}, which")
    # Two kept rows in each other's places: the counts agree, the order
    # does not.
    damaged(LOG, lambda t: t.replace(a[1] + "\n" + a[4] + "\n",
                                     a[4] + "\n" + a[1] + "\n"),
            "out of their order")

    # An era whose commit this clone lacks, as in a shallow clone, cannot
    # be recomputed, and check says so rather than failing on git's words.
    eras_text = repo.read(f"{HISTORY}/eras.tsv")
    repo.write(f"{HISTORY}/eras.tsv", eras_text.replace(boundary, "0" * 12))
    result = repo.run("check")
    check("check exit, era not in the clone", result.returncode, 2)
    if "is not in this clone's history" not in result.stderr:
        failures.append(f"check stderr, era not in the clone: "
                        f"{result.stderr!r}")
    repo.write(f"{HISTORY}/eras.tsv", eras_text)

    # Both history files gone: check fails rather than finding no era.
    eras, groups = (repo.read(f"{HISTORY}/{n}")
                    for n in ("eras.tsv", "groups.tsv"))
    for name in ("eras.tsv", "groups.tsv"):
        os.unlink(repo.path(f"{HISTORY}/{name}"))
    result = repo.run("check")
    check("check exit, history gone", result.returncode, 1)
    if f"{HISTORY}/eras.tsv is missing" not in result.stderr:
        failures.append(f"check stderr, history gone: {result.stderr!r}")
    repo.write(f"{HISTORY}/eras.tsv", eras)
    repo.write(f"{HISTORY}/groups.tsv", groups)
    check("files restored", repo.history_files(), good)
    return repo, boundary, a, b, later


def refusals(tool, directory):
    """Case 5: what compact refuses, leaving every file as it was."""
    repo, boundary, a, b, later = checks(tool, directory)

    def refused(args, message):
        before = repo.history_files()
        result = repo.run(*args)
        check(f"refusal exit, {message}", result.returncode, 2)
        if message not in result.stderr:
            failures.append(f"refusal stderr, {message}: {result.stderr!r}")
        check(f"files unchanged, {message}", repo.history_files(), before)

    refused(["compact", boundary], "already has an era")

    fresh, boundary2, a2, b2, later2 = compacted_repo(
        tool, directory + "-missing")
    fresh.write_log(a2 + b2[:3] + b2[4:] + later2)
    before = fresh.history_files()
    result = fresh.run("compact", boundary2)
    check("refusal exit, missing row", result.returncode, 2)
    if "lacks 1 rows" not in result.stderr:
        failures.append(f"refusal stderr, missing row: {result.stderr!r}")
    check("files unchanged, missing row", fresh.history_files(), before)

    fresh.write_log(a2 + b2 + later2)
    fresh.write(f"{HISTORY}/groups.tsv", "era\tlog\n")
    before = fresh.history_files()
    result = fresh.run("compact", boundary2)
    check("refusal exit, other header", result.returncode, 2)
    if "does not have this tool's columns" not in result.stderr:
        failures.append(f"refusal stderr, header: {result.stderr!r}")
    check("files unchanged, other header", fresh.history_files(), before)
    return repo, boundary, a, b, later


def pruning(tool, directory):
    """Case 6: prune removes rows that came back, and nothing else."""
    repo, _, a, b, _ = refusals(tool, directory)
    good = repo.read(LOG)
    repo.write(LOG, good + a[0] + "\n" + b[1] + "\n")
    result = repo.run("prune")
    check("prune exit", result.returncode, 0)
    check("prune report", result.stdout.strip(), f"runlog: {LOG}: removed 2 "
          "rows")
    check("log after prune", repo.read(LOG), good)
    check("check after prune", repo.run("check").returncode, 0)


def has_once(repo, rows):
    present = repo.log_rows()
    return all(present.count(r) == 1 for r in rows)


def merges(tool, directory):
    """Case 7: union merges across the compaction, in each order."""
    repo, _, _, b, _ = compaction(tool, directory)
    repo.commit("compact")
    old_rows = [kept_row(30), probe_row(31)]

    # The compacted branch merged into a branch based before the boundary,
    # and that branch merged into the compacted one.
    for name, into, other in (("merge-into-old", "old", "compacting"),
                              ("merge-old-in", "compacting", "old")):
        repo.git("checkout", "-q", "-b", name, into)
        repo.git("merge", "-q", "--no-edit", other)
        check(f"check after {name}", repo.run("check").returncode, 0)
        check(f"old's rows once after {name}", has_once(repo, old_rows), True)

    # old merges dev, dev takes that merge, and then the compacted branch
    # merges dev: old's rows now sit inside the boundary's log.
    repo.git("checkout", "-q", "-b", "dev-with-old", "dev")
    repo.git("checkout", "-q", "-b", "old-with-dev", "old")
    repo.git("merge", "-q", "--no-edit", "dev")
    repo.git("checkout", "-q", "dev-with-old")
    repo.git("merge", "-q", "--no-edit", "old-with-dev")
    repo.write(LOG, repo.read(LOG) + probe_row(50) + "\n")
    repo.commit("a row on dev")
    repo.git("checkout", "-q", "-b", "merge-dev-in", "compacting")
    repo.git("merge", "-q", "--no-edit", "dev-with-old")
    check("check after merge-dev-in", repo.run("check").returncode, 0)
    check("old's rows once after merge-dev-in", has_once(repo, old_rows),
          True)

    # The same merge when the row after old's base was not kept: the rows
    # removed after it come back.
    repo.git("checkout", "-q", "-b", "no-fence", "compacting")
    repo.write(LOG, repo.read(LOG).replace(b[0] + "\n", ""))
    repo.commit("drop the row after old's base")
    repo.git("merge", "-q", "--no-edit", "dev-with-old")
    check("removed row back without the fence", b[1] in repo.log_rows(),
          True)


def second_era(tool, directory):
    """Case 8: a second compaction does not count the first one's kept
    rows again."""
    repo, _, a, b, later = compaction(tool, directory)
    repo.commit("compact")
    repo.write(LOG, repo.read(LOG) + kept_row(60) + "\n" + probe_row(61)
               + "\n")
    boundary2 = repo.commit("more rows")
    result = repo.run("compact", boundary2)
    check("second compact exit", result.returncode, 0)
    eras = repo.read(f"{HISTORY}/eras.tsv").splitlines()
    check("second era rows", eras[-1].split("\t")[2:5], ["5", "3", "11"])
    repo.commit("compact again")
    check("check after two eras", repo.run("check").returncode, 0)
    check("log after two eras", repo.log_rows(),
          [a[1], a[4], b[0], b[2], b[5], later[0], kept_row(60),
           probe_row(61)])


def later_era_refusal(tool, directory):
    """Case 11: a later compaction refuses when its boundary's log lacks a
    row an earlier era kept, leaving every file as it was."""
    repo, _, a, _, _ = compaction(tool, directory)
    repo.commit("compact")
    repo.write(LOG, repo.read(LOG).replace(a[1] + "\n", ""))
    boundary2 = repo.commit("a kept row deleted")
    before = repo.history_files()
    result = repo.run("compact", boundary2)
    check("refusal exit, kept row gone", result.returncode, 2)
    if "lacks a row an earlier era kept" not in result.stderr:
        failures.append(f"refusal stderr, kept row gone: {result.stderr!r}")
    check("files unchanged, kept row gone", repo.history_files(), before)


def campaign_log(tool, directory):
    """Case 12: a campaign's own log is compacted with --log, and check
    --log checks that log alone."""
    repo, boundary, a, _, _ = compaction(tool, directory)
    campaign = f"{os.path.dirname(LOG)}/campaign/runs.tsv"
    os.makedirs(os.path.dirname(repo.path(campaign)))
    repo.write(campaign, "\n".join([HEADER, probe_row(90), kept_row(91),
                                    probe_row(92)]) + "\n")
    boundary2 = repo.commit("compact the run log; a campaign log")
    # An absolute path is recorded relative to the checkout, as a relative
    # one is.
    result = repo.run("compact", boundary2, "--log", repo.path(campaign))
    check("compact --log exit", result.returncode, 0)
    check("campaign log after compaction",
          repo.read(campaign).splitlines()[1:], [kept_row(91), probe_row(92)])
    eras = [line.split("\t")[:4]
            for line in repo.read(f"{HISTORY}/eras.tsv").splitlines()[1:]]
    check("eras of two logs", eras,
          [[boundary, LOG, "11", "5"], [boundary2, campaign, "3", "2"]])
    repo.commit("compact the campaign log")
    check("check of both logs", repo.run("check").returncode, 0)

    # A removed row back in the run log: check --log of the campaign log
    # does not look at it, and check of every log finds it.
    repo.write(LOG, repo.read(LOG) + a[0] + "\n")
    result = repo.run("check", "--log", campaign)
    check("check --log of the campaign log", result.returncode, 0)
    check("check --log names only that log",
          result.stdout.strip().startswith(f"runlog: {campaign}:")
          and LOG not in result.stdout, True)
    check("check of every log", repo.run("check").returncode, 1)


def merged_boundary_first(tool, directory):
    """Case 9: a branch that merged the boundary before the compaction has
    its rows inside the boundary's log, before the boundary's later rows."""
    repo = Repo(tool, directory)
    a = [probe_row(1), kept_row(2), probe_row(3), probe_row(4), probe_row(5)]
    repo.write_log(a)
    repo.commit("A")
    repo.git("checkout", "-q", "-b", "old")
    old_rows = [kept_row(30), probe_row(31)]
    repo.write_log(a + old_rows)
    repo.commit("old rows")
    repo.git("checkout", "-q", "dev")
    b = [probe_row(6), probe_row(7), kept_row(8), probe_row(9),
         probe_row(10), probe_row(11)]
    repo.write_log(a + b)
    boundary = repo.commit("B")
    repo.git("checkout", "-q", "old")
    repo.git("merge", "-q", "--no-edit", "dev")
    check("old's rows inside the boundary's log", repo.log_rows(),
          a + old_rows + b)
    repo.git("checkout", "-q", "-b", "compacting", "dev")
    result = repo.run("compact", boundary)
    check("compact exit, merged first", result.returncode, 0)
    eras = repo.read(f"{HISTORY}/eras.tsv").splitlines()
    check("anchors beside old's rows", eras[-1].split("\t")[4], "6,7,12")
    repo.commit("compact")
    for name, into, other in (("into-old", "old", "compacting"),
                              ("old-in", "compacting", "old")):
        repo.git("checkout", "-q", "-b", name, into)
        repo.git("merge", "-q", "--no-edit", other)
        check(f"check after {name}", repo.run("check").returncode, 0)
        check(f"old's rows once after {name}", has_once(repo, old_rows),
              True)

    # The same compaction repeated from the boundary on another branch,
    # while the branches that hold the era are open, makes the same era.
    era, groups = (repo.read(f"{HISTORY}/{name}")
                   for name in ("eras.tsv", "groups.tsv"))
    repo.git("checkout", "-q", "-b", "redo", "dev")
    result = repo.run("compact", boundary)
    check("compact exit, repeated", result.returncode, 0)
    check("repeated era", repo.read(f"{HISTORY}/eras.tsv"), era)
    check("repeated groups", repo.read(f"{HISTORY}/groups.tsv"), groups)


def inserted_mid_log(tool, directory):
    """Case 10: a branch based before the boundary added rows between two
    rows of its base, and the boundary has rows of its own there."""
    repo = Repo(tool, directory)
    r = [probe_row(n) for n in range(1, 8)]
    repo.write_log(r[:2])
    repo.commit("A0")
    repo.git("branch", "p")
    repo.git("branch", "s")
    repo.write_log(r[:3])
    repo.commit("A")
    t1 = kept_row(70)
    repo.git("checkout", "-q", "p")
    repo.write_log(r[:2] + [t1])
    repo.commit("p rows")
    repo.git("merge", "-q", "--no-edit", "dev")
    s_rows = [probe_row(80), probe_row(81)]
    repo.git("checkout", "-q", "s")
    repo.write_log(r[:2] + s_rows)
    repo.commit("s rows")
    repo.git("merge", "-q", "--no-edit", "dev")
    repo.git("checkout", "-q", "dev")
    repo.git("merge", "-q", "--no-edit", "s")
    repo.write_log(r[:2] + s_rows + r[2:])
    boundary = repo.commit("B")
    repo.git("checkout", "-q", "-b", "compacting")
    result = repo.run("compact", boundary)
    check("compact exit, mid-log", result.returncode, 0)
    # p's row sits between r[1] (line 3) and r[2] (line 6) of p's base;
    # s's two rows lie between them at the boundary.
    eras = repo.read(f"{HISTORY}/eras.tsv").splitlines()
    check("anchors of a mid-log insertion", eras[-1].split("\t")[4],
          "3,4,5,6,10")
    repo.commit("compact")
    # dev takes p first, so p's row follows s's rows; or p takes dev first
    # and dev takes that, so p's row precedes them.
    repo.git("checkout", "-q", "-b", "dev-then-p", "dev")
    repo.git("merge", "-q", "--no-edit", "p")
    repo.git("checkout", "-q", "-b", "p-then-dev", "p")
    repo.git("merge", "-q", "--no-edit", "dev")
    for name in ("dev-then-p", "p-then-dev"):
        repo.git("checkout", "-q", "-b", f"compacting-{name}", "compacting")
        repo.git("merge", "-q", "--no-edit", name)
        check(f"check after merging {name}", repo.run("check").returncode, 0)
        check(f"p's row once after {name}", has_once(repo, [t1]), True)


def skip_messages(tool, directory):
    """Case 13: the skip expressions CMake gives tools.runlog_check match
    the messages the tool prints outside a git checkout and in a clone that
    lacks an era's commit, so that rewording either cannot turn a skip into
    a failure. The expressions are plain text, which CMake's regular
    expressions and Python's read alike."""
    cmake = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..",
                         "CMakeLists.txt")
    with open(cmake, encoding="utf-8") as f:
        text = f.read()
    block = text[text.index("set_tests_properties(tools.runlog_check"):]
    found = re.search(r'SKIP_REGULAR_EXPRESSION "([^"]*)"', block)
    expressions = found.group(1).split(";") if found else []
    check("skip expressions of tools.runlog_check", len(expressions), 2)

    outside = os.path.join(directory, "outside")
    shutil.rmtree(outside, ignore_errors=True)
    os.makedirs(outside)
    env = dict(os.environ, GIT_CEILING_DIRECTORIES=directory)
    result = subprocess.run([sys.executable, "-I", tool, "check"],
                            cwd=outside, env=env, capture_output=True,
                            text=True, check=False)
    messages = [("outside a git checkout", result)]

    repo, boundary, _, _, _ = compaction(tool, os.path.join(directory, "repo"))
    repo.commit("compact")
    eras = repo.read(f"{HISTORY}/eras.tsv")
    repo.write(f"{HISTORY}/eras.tsv", eras.replace(boundary, "0" * 12))
    messages.append(("an era not in the clone", repo.run("check")))

    for name, result in messages:
        check(f"exit {name}", result.returncode, 2)
        check(f"a skip expression matches the message {name}",
              any(re.search(e, result.stderr) for e in expressions), True)


def main(tool, directory):
    tool = os.path.realpath(tool)
    directory = os.path.realpath(directory)
    cases = [summary_values, kept_by_rule, compaction, checks, refusals,
             pruning, merges, second_era, merged_boundary_first,
             inserted_mid_log, later_era_refusal, campaign_log,
             skip_messages]
    for case in cases:
        try:
            case(tool, os.path.join(directory, case.__name__))
        except RuntimeError as error:
            failures.append(f"{case.__name__}: {error}")
    for failure in failures:
        print(failure)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(*sys.argv[1:]))
