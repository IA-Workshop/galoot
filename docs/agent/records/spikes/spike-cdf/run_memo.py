"""Spike D: build cost and evaluation count of the memoization forms.

Sets (run any of them, in order):

  main    pinned and measured (pinned.py): each compiler, -O0 and -O2,
          make_plan consteval and constexpr, memo forms 0 to 3, and 0, 1, 4,
          16 uses (0 uses once per compiler, opt, and init: no plan at all).
          Rows go to out/memo/main.tsv, with the number of 24 KiB plan
          tables in the object (`nm -S`), one per emitted copy.
  main2   the same points again, a second sample
  second  memo forms 1 to 3, 16 uses, plus a second plan used once; -O0,
          consteval; checks that each form keeps two argument sets apart
          (two tables, two evaluations). Pinned and measured.
  calib   calibration of the counting method: memo form 4 (each use calls a
          distinct make_plan specialization), 1, 4, 16 uses, both compilers,
          -O0 and -O2, consteval. Pinned and measured; also traced.
  limits  Clang's default step limit on this plan: memo.cpp (form 1, one
          use) at -O0, -O1, -O2 with default limits, with __OPTIMIZE__
          toggled, and at -O2 with 2^20 and 2^21 steps; and Spike A's
          plan_tu.cpp (n = 5, euclidean) at -O0 and -O2. Exit status and the
          first error go to out/memo/limits.tsv, with each memo form compiled
          by both compilers under bare -std=c++23 -pedantic-errors (limits
          raised). Not pinned.
  trace   evaluation counts. Clang: -ftime-trace with granularity 0, then
          count (count_evals.py, since deleted; recover it with
          `git show 3a2aa32:docs/agent/records/spikes/spike-cdf/count_evals.py`
          to rerun this set) the outermost Evaluate* events longer than
          20 ms, which are
          the plan fills (the size pass takes about half as long and is
          counted separately by its variable name, `size`). GCC: -ftime-report
          "constant expression evaluation" time and memory. Not pinned; every
          compile still runs under tools/timed. Rows go to out/memo/trace.tsv.

Every compile uses -std=c++23 with the S4 warnings, -Werror, and
-ffp-contract=off. Constant-evaluation limits are raised as in Spike A
(g++ -fconstexpr-ops-limit=2^40, clang++ -fconstexpr-steps=0), because Clang
at -O1 and above exceeds its default step limit on this plan (see memo.cpp).

Usage, from anywhere:  python3 -I run_memo.py SET [SET ...]
"""

import re
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from count_evals import outermost_evaluations  # noqa: E402
from pinned import FIELDS, ROOT, measure  # noqa: E402

SPIKE_A = ROOT / "docs/agent/records/spikes/spike-a"
OUT = HERE / "out/memo"
FLAGS = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
         "-Wshadow", "-Werror", "-ffp-contract=off", f"-I{SPIKE_A}"]
RAISED = {"g++": ["-fconstexpr-ops-limit=1099511627776"],
          "clang++": ["-fconstexpr-steps=0"]}
PLAN_BYTES = 1024 * 24       # 32 x 32 blade pairs, one term each, 24 B/term
SECOND_BYTES = 5 * 10 * 24   # vectors x bivectors in R(5,0,0)
MEMO = {0: "none", 1: "variable_template", 2: "class_static",
        3: "function_local", 4: "distinct"}
COMPILERS = ("g++", "clang++")


def tables(obj):
    out = subprocess.run(["nm", "-S", str(obj)], capture_output=True,
                         text=True).stdout
    sizes = [int(ln.split()[1], 16) for ln in out.splitlines()
             if len(ln.split()) == 4]
    return sizes.count(PLAN_BYTES), sizes.count(SECOND_BYTES)


def defines(memo, uses, init, second):
    d = [f"-DSPIKE_MEMO={memo}", f"-DSPIKE_USES={uses}",
         f"-DSPIKE_INIT={init}"]
    return d + (["-DSPIKE_SECOND"] if second else [])


def points(set_name):
    if set_name in ("main", "main2", "trace"):
        for cxx in COMPILERS:
            for opt in ("O0", "O2"):
                for init in ("consteval", "constexpr"):
                    yield cxx, opt, init, 1, 0, False
                    for memo in (0, 1, 2, 3):
                        for uses in ((1, 16) if set_name == "trace"
                                     else (1, 4, 16)):
                            yield cxx, opt, init, memo, uses, False
    elif set_name in ("calib", "calib_trace"):
        for cxx in COMPILERS:
            for opt in ("O0", "O2"):
                for uses in (1, 4, 16):
                    yield cxx, opt, "consteval", 4, uses, False
    elif set_name == "second":
        for cxx in COMPILERS:
            for memo in (1, 2, 3):
                yield cxx, "O0", "consteval", memo, 16, True
    else:
        raise SystemExit(f"unknown set {set_name}")


def run_measured(set_name):
    head = ["set", "compiler", "opt", "init", "memo", "uses", "second",
            *FIELDS, "plan_tables", "second_tables"]
    rows = [head]
    for cxx, opt, init, memo, uses, second in points(set_name):
        obj = OUT / "memo.o"
        sel = (f"spike-d:{set_name}:{cxx}:{opt}:{init}:"
               f"{MEMO[memo] if uses else 'baseline'}:u{uses}")
        fields, err = measure(sel, [cxx, *FLAGS, *RAISED[cxx], f"-{opt}",
                                    *defines(memo, uses, init, second), "-c",
                                    "-o", str(obj), str(HERE / "memo.cpp")])
        if err:
            print(err, file=sys.stderr)
        t1, t2 = tables(obj) if obj.exists() else ("", "")
        rows.append([set_name, cxx, opt, init,
                     MEMO[memo] if uses else "baseline", str(uses),
                     "yes" if second else "no",
                     *[fields[k] for k in FIELDS], str(t1), str(t2)])
        print("\t".join(rows[-1][:13] + rows[-1][-2:]), flush=True)
        if obj.exists():
            obj.unlink()
    (OUT / f"{set_name}.tsv").write_text(
        "\n".join("\t".join(r) for r in rows) + "\n")


def clang_evaluations(trace):
    """Plan fills (outermost Evaluate* events over 20 ms, not `size`)."""
    long = outermost_evaluations(trace, 20.0)
    fills = [e for e in long if e.get("args", {}).get("detail") != "size"]
    sizes = [e for e in long if e.get("args", {}).get("detail") == "size"]
    names = sorted({f"{e['name']}:{e.get('args', {}).get('detail', '')}"
                    for e in fills})
    return len(fills), len(sizes), sum(e["dur"] for e in fills) / 1e6, \
        ";".join(names)


def gcc_report(text):
    m = re.search(r"constant expression evaluation\s*:\s*([\d.]+)\s*\(\s*\d+%\)"
                  r"\s*([\d.]+[kMG]?)", text)
    return (m.group(1), m.group(2)) if m else ("", "")


def run_trace():
    head = ["compiler", "opt", "init", "memo", "uses", "exit",
            "clang_fill_evaluations", "clang_size_evaluations",
            "clang_fill_eval_s", "clang_fill_events",
            "gcc_constexpr_eval_s", "gcc_constexpr_eval_mem"]
    rows = [head]
    for cxx, opt, init, memo, uses, second in [*points("trace"),
                                               *points("calib_trace")]:
        name = f"{cxx.rstrip('+')}-{opt}-{init}-{memo}-u{uses}"
        obj = OUT / "trace" / f"{name}.o"
        obj.parent.mkdir(parents=True, exist_ok=True)
        extra = (["-ftime-trace", "-ftime-trace-granularity=0"]
                 if cxx == "clang++" else ["-ftime-report"])
        sel = f"spike-d:trace:{name}"
        cmd = [str(ROOT / "tools/timed"), "build", "-", sel, "--", cxx,
               *FLAGS, *RAISED[cxx], f"-{opt}", *defines(memo, uses, init, second), *extra,
               "-c", "-o", str(obj), str(HERE / "memo.cpp")]
        r = subprocess.run(cmd, capture_output=True, text=True)
        row = [cxx, opt, init, MEMO[memo] if uses else "baseline", str(uses),
               str(r.returncode)]
        if cxx == "clang++":
            n, s, secs, names = clang_evaluations(obj.with_suffix(".json"))
            row += [str(n), str(s), f"{secs:.3f}", names, "", ""]
            obj.with_suffix(".json").unlink()
        else:
            (OUT / "trace" / f"{name}.time-report.txt").write_text(r.stderr)
            row += ["", "", "", "", *gcc_report(r.stderr)]
        if obj.exists():
            obj.unlink()
        rows.append(row)
        print("\t".join(row), flush=True)
    (OUT / "trace.tsv").write_text("\n".join("\t".join(r) for r in rows)
                                   + "\n")


LIMIT_POINTS = [
    ("memo", ["-O0"]), ("memo", ["-O1"]), ("memo", ["-O2"]),
    ("memo", ["-O0", "-D__OPTIMIZE__=1"]), ("memo", ["-O2", "-U__OPTIMIZE__"]),
    ("memo", ["-O2", "-fconstexpr-steps=1048576"]),
    ("memo", ["-O2", "-fconstexpr-steps=2097152"]),
    ("plan_tu", ["-O0"]), ("plan_tu", ["-O2"]),
]


def run_limits():
    rows = [["unit", "flags", "exit", "first_error"]]
    for unit, flags in LIMIT_POINTS:
        if unit == "memo":
            src = [*defines(1, 1, "consteval", False), str(HERE / "memo.cpp")]
        else:
            src = ["-DSPIKE_N=5", "-DSPIKE_METRIC=euclidean",
                   str(SPIKE_A / "plan_tu.cpp")]
        sel = f"spike-d:limits:{unit}:{'_'.join(flags)}"
        r = subprocess.run([str(ROOT / "tools/timed"), "build", "-", sel, "--",
                            "clang++", *FLAGS, "-fno-diagnostics-color",
                            *flags, *src, "-c", "-o", "/dev/null"],
                           capture_output=True, text=True)
        err = next((ln for ln in r.stderr.splitlines()
                    if "step limit" in ln), "")
        rows.append([unit, " ".join(flags), str(r.returncode),
                     err.split("note: ")[-1][:120]])
        print("\t".join(rows[-1]), flush=True)
    # A user's build with no warning flags (DoD-6), every form, one use.
    for cxx in COMPILERS:
        for memo in (0, 1, 2, 3):
            r = subprocess.run(
                [str(ROOT / "tools/timed"), "build", "-",
                 f"spike-d:pedantic:{cxx}:{MEMO[memo]}", "--", cxx,
                 "-std=c++23", "-pedantic-errors", f"-I{SPIKE_A}",
                 *RAISED[cxx], *defines(memo, 1, "consteval", False),
                 "-fsyntax-only", str(HERE / "memo.cpp")],
                capture_output=True, text=True)
            rows.append([f"memo:{MEMO[memo]}", f"{cxx} -pedantic-errors",
                         str(r.returncode), ""])
            print("\t".join(rows[-1]), flush=True)
    (OUT / "limits.tsv").write_text("\n".join("\t".join(r) for r in rows)
                                    + "\n")


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    for set_name in sys.argv[1:]:
        if set_name == "limits":
            run_limits()
        elif set_name == "calib":
            run_measured("calib")
        elif set_name == "trace":
            run_trace()
        else:
            run_measured(set_name)


if __name__ == "__main__":
    main()
