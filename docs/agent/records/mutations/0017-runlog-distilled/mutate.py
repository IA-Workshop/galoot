"""Mutation runs for the self-checks of tools/runlog and of the recorder's
log option.

  mutate.py <scratch directory> <out.tsv> [mutation...]

Each mutant is one text edit of a copy of tools/runlog, tools/measure, or
tools/timed, made in the scratch directory; the checkout's own files are
never changed. The edit's anchor must occur exactly once, and the edited
Python must still compile. The mutant's test (test/tools/runlog_test.py, or
test/tools/measure_record_test.py) then runs against the copy, and kills the
mutant when it exits nonzero. One row per mutant goes to out.tsv: name,
file, test, killed or survived, and the test's first failing line.

Run from the worktree root. The tests need no build and no job slot.
"""

import os
import pathlib
import py_compile
import shutil
import subprocess
import sys

RUNLOG = "tools/runlog"
MEASURE = "tools/measure"
TIMED = "tools/timed"
RUNLOG_TEST = "test/tools/runlog_test.py"
MEASURE_TEST = "test/tools/measure_record_test.py"

# name: (file, anchor, replacement, test)
MUTANTS = {
    "median_high": (
        RUNLOG, "median = ordered[(len(ordered) - 1) // 2]",
        "median = ordered[len(ordered) // 2]", RUNLOG_TEST),
    "range_from_first": (
        RUNLOG, "ordered[-1] - ordered[0]]", "ordered[-1] - values[0]]",
        RUNLOG_TEST),
    "ties_reversed": (
        RUNLOG, "key=lambda m: (m[0], m[1])", "key=lambda m: (m[0], -m[1])",
        RUNLOG_TEST),
    "pinned_inst_dropped": (
        RUNLOG, 'if p_core is not None and row["mode"] == "pinned":',
        "if False:", RUNLOG_TEST),
    "outcome_not_in_key": (
        RUNLOG, 'outcome = "pass" if row["exit"] == "0" else "fail"',
        'outcome = "pass"', RUNLOG_TEST),
    "dirty_kept": (
        RUNLOG, 'if not COMMITTED.fullmatch(row.get("revision", "")):',
        'if not COMMITTED.match(row.get("revision", "")):', RUNLOG_TEST),
    "unnamed_preset_kept": (
        RUNLOG, 'return row.get("preset", NO_TREE) != NO_TREE', "return True",
        RUNLOG_TEST),
    "cheap_gates_dropped": (
        RUNLOG, 'return family(row.get("selection", "")) in CHEAP_GATES',
        "return False", RUNLOG_TEST),
    "base_end_only": (
        RUNLOG, "in insertions(base_lines, tip_lines, branch):",
        "in [(len(base_lines) - 1, None)]:", RUNLOG_TEST),
    "no_before_anchor": (
        RUNLOG,
        "                anchors.add(number)\n                if number < last:",
        "                if number < last:", RUNLOG_TEST),
    "no_successor_anchor": (
        RUNLOG, "anchors.add(number + 1)", "pass", RUNLOG_TEST),
    "no_after_anchor": (
        RUNLOG,
        "                anchors.add(number)\n                if number > 2:",
        "                if number > 2:", RUNLOG_TEST),
    "no_predecessor_anchor": (
        RUNLOG, "anchors.add(number - 1)", "pass", RUNLOG_TEST),
    "era_holders_not_skipped": (
        RUNLOG, ("        if holds_era(top, branch, revision, log):\n"
                 "            continue"),
        "        if False:\n            continue",
        RUNLOG_TEST),
    "no_boundary_anchor": (
        RUNLOG, "anchors = {last}", "anchors = set()", RUNLOG_TEST),
    "no_containment_check": (
        RUNLOG, "    if missing:\n", "    if False:\n", RUNLOG_TEST),
    "no_repeat_check": (
        RUNLOG, "if any(era[0] == revision and era[1] == log for era in eras):",
        "if False:", RUNLOG_TEST),
    "no_header_check": (
        RUNLOG, "if header != list(columns):", "if False:", RUNLOG_TEST),
    "no_resurrection_check": (
        RUNLOG, 'what = "a row an era removed is back"', "continue",
        RUNLOG_TEST),
    "groups_not_compared": (
        RUNLOG, "if committed != history:", "if False:", RUNLOG_TEST),
    "eras_not_compared": (
        RUNLOG, "if era != record:", "if False:", RUNLOG_TEST),
    "earlier_era_resummarized": (
        RUNLOG, "        kept_before += kept\n        yield (",
        "        yield (", RUNLOG_TEST),
    "prune_keeps_removed": (
        RUNLOG, "            elif allowed[line] > 0:",
        "            elif True:", RUNLOG_TEST),
    "toolchain_end_inclusive": (
        RUNLOG, "(ends is None or ends > first)",
        "(ends is None or ends >= first)", RUNLOG_TEST),
    "toolchain_start_exclusive": (
        RUNLOG, "if since <= latest and", "if since < latest and",
        RUNLOG_TEST),
    "mode_default_shared": (
        RUNLOG, 'row.setdefault("mode", MISSING)',
        'row.setdefault("mode", "shared")', RUNLOG_TEST),
    "earlier_kept_not_required": (
        RUNLOG, "    if +left:\n", "    if False:\n", RUNLOG_TEST),
    "unknown_era_groups_ok": (
        RUNLOG, "if (cells[0], cells[1]) not in known:", "if False:",
        RUNLOG_TEST),
    "no_second_header_check": (
        RUNLOG, "if cells[0] == columns[0]:", "if False:", RUNLOG_TEST),
    "check_ignores_log_option": (
        RUNLOG, "    if log is not None:\n        return [log]",
        "    if False:\n        return [log]", RUNLOG_TEST),
    "citations_not_cheap": (
        RUNLOG, '"citations", "golden-facts"', '"golden-facts"', RUNLOG_TEST),
    "golden_check_not_cheap": (
        RUNLOG, '"golden-check", "conventions_check"}', '"conventions_check"}',
        RUNLOG_TEST),
    "workflow_needs_commit": (
        RUNLOG, ('if step == "workflow" and row.get("preset", NO_TREE) != '
                 'NO_TREE:'), "if False:", RUNLOG_TEST),
    "lint_dropped": (
        RUNLOG, 'if step in ("gate", "lint"):', 'if step == "gate":',
        RUNLOG_TEST),
    "no_order_check": (
        RUNLOG, "problems += order_problems(name, working, orders)", "pass",
        RUNLOG_TEST),
    "missing_history_ok": (
        RUNLOG, "        if had:\n", "        if False:\n", RUNLOG_TEST),
    "clone_check_dropped": (
        RUNLOG, "if not has_commit(top, era[0]):", "if False:", RUNLOG_TEST),
    "relative_env_allowed": (
        TIMED, "if [[ -n $log && $log != /* && ${1:-} != --log ]]; then",
        "if false; then", MEASURE_TEST),
    "blank_line_allowed": (
        RUNLOG, "        if not line:\n            raise Refused",
        "        if False:\n            raise Refused", RUNLOG_TEST),
    "relative_keeps_abs": (
        RUNLOG, "        return os.path.relpath(log, top)",
        "        return log", RUNLOG_TEST),
    "checkout_message_reworded": (
        RUNLOG, 'raise Refused("not inside a git checkout")',
        'raise Refused("not in a git checkout")', RUNLOG_TEST),
    "clone_message_reworded": (
        RUNLOG, "is not in this clone's history, so",
        "is missing from this clone, so", RUNLOG_TEST),
    "log_ignored": (
        MEASURE, "log = log_path or os.path.join(",
        "log = os.path.join(", MEASURE_TEST),
    "env_ignored": (
        TIMED, 'log=${TIMED_LOG:-}', "log=", MEASURE_TEST),
    "option_beats_env_reversed": (
        TIMED, 'log=$2', "log=${TIMED_LOG:-$2}", MEASURE_TEST),
}


def first_failure(output):
    lines = [line for line in output.splitlines() if line.strip()]
    return lines[0][:160] if lines else "-"


def run(name, scratch):
    path, anchor, replacement, test = MUTANTS[name]
    root = pathlib.Path(scratch) / name
    shutil.rmtree(root, ignore_errors=True)
    (root / "tools").mkdir(parents=True)
    for tool in (RUNLOG, MEASURE, TIMED):
        shutil.copy2(tool, root / tool)
    text = (root / path).read_text(encoding="utf-8")
    if text.count(anchor) != 1:
        sys.exit(f"{name}: the anchor occurs {text.count(anchor)} times")
    (root / path).write_text(text.replace(anchor, replacement),
                             encoding="utf-8")
    if path != TIMED:
        py_compile.compile(str(root / path), cfile=str(root / "compiled"),
                           doraise=True)
    tool = RUNLOG if test == RUNLOG_TEST else MEASURE
    result = subprocess.run(
        [sys.executable, "-I", test, str(root / tool), str(root / "test")],
        capture_output=True, text=True, check=False)
    verdict = "killed" if result.returncode != 0 else "survived"
    return [name, path, test, verdict,
            first_failure(result.stdout + result.stderr)]


def main(scratch, out, names):
    rows = [run(name, scratch) for name in names or MUTANTS]
    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    with open(out, "w", encoding="utf-8") as f:
        f.write("mutant\tfile\ttest\tverdict\tfirst_failure\n")
        f.writelines("\t".join(c.replace("\t", " ") for c in cells) + "\n"
                     for cells in rows)
    for cells in rows:
        print(f"{cells[0]}: {cells[3]}")
    return 1 if any(cells[3] == "survived" for cells in rows) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1], sys.argv[2], sys.argv[3:]))
