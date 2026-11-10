#!/usr/bin/env -S python3 -I
"""Run the rga3 benchmark programs in separate, interleaved, pinned processes
and summarize them.

Usage, from the root of a checkout whose gcc-measure and clang-measure trees
have the bench_<operation> programs built:
  drive.py calibrate <out-dir> --cpu <cpu>
  drive.py run <out-dir> --cpu <cpu> --env <bytes> [--repeats <n>]
  drive.py summarize <out-dir>
  drive.py stalls <out-dir> --cpu <cpu>

Every process runs as `taskset -c <cpu> tools/timed --log <out-dir>/runs.tsv
bench <preset> <selection> -- env -i [BENCH_ENV_PAD=<pad>] <program> <form>
<loop> <passes> <seed>`, so the row tools/measure writes for it is pinned and
holds the instructions and cycles retired on that core, the wall time, and
the busy time of the sibling thread and of other processes on the CPU. The
timing lock is not taken here; take it around each command that runs.

calibrate  One throughput run, and one latency run where the operation has
           one, of the library's form of each operation on each compiler,
           at CALIBRATION_PASSES, printing the nanoseconds per operation, so
           that PASSES can be chosen for runs of about TARGET_S seconds.
run        One batch at one environment size: the program's environment is
           empty but for BENCH_ENV_PAD, which holds <bytes> characters, or is
           left out for 0, so its size moves the start of the stack (the
           layout-bias check). Each of <repeats> rounds runs every
           (compiler, operation, loop, form) once, in an order that reverses
           the compilers and the forms on every other round; each run is a
           pair of processes at PASSES / SHORT_DIVISOR and PASSES passes, so
           that the difference of their counters, divided by the difference
           of their operation counts, leaves out the process's start, the
           drawing of operands, and the printing. A pair whose sibling
           thread, or whose pinned CPU beyond the pair's own work, was busy
           for more than BUSY_TICKS clock ticks of /proc/stat (TICK_S each),
           summed over its two processes, is run again once and both are
           kept; the later one is used, and marked when it was busy too.
           Each process's busy time is a difference of two tick counts, so
           it can be one tick off with nothing running, and the pair's sum
           two. Appends to <out-dir>/rows.tsv, one row per process.
summarize  Writes <out-dir>/results.tsv (per compiler, operation, loop, form,
           and environment size: the median and range over the rounds of the
           nanoseconds, instructions, and cycles per operation) and
           <out-dir>/comparison.tsv (per compiler, operation, loop, and
           environment size: the two forms' medians, twice the larger range,
           and whether the medians differ by more than that), and
           <out-dir>/verdict.tsv (whether the criterion holds at every
           environment size, or at the smallest, with the same form faster
           at every size), and checks that the two forms' checksums agree in
           every pair of runs with the same passes and seed.
stalls     A diagnostic, not a timing: for every (compiler, operation, loop,
           form), a pair of processes at PASSES / SHORT_DIVISOR and PASSES
           passes, each as `taskset -c <cpu> perf stat -e
           <STALL_EVENTS>`, giving per operation the loads that a store
           could not forward to, the calls, and the scalar and packed
           double-precision arithmetic instructions retired, from the
           difference of the pair. Not through tools/timed, whose own eight counters
           would leave this one multiplexed. Writes <out-dir>/stalls.tsv.
"""

import argparse
import csv
import os
import tempfile
import statistics
import subprocess
import sys
from pathlib import Path

ROOT = Path(subprocess.run(["git", "rev-parse", "--show-toplevel"],
                           capture_output=True, text=True,
                           check=True).stdout.strip())
COMPILERS = {"gcc": "gcc-measure", "clang": "clang-measure"}
OPERATIONS = ("wedge_point_point", "wedge_point_point_point",
              "antiwedge_line_plane", "transform_point", "transform_line",
              "transform_plane", "compose_motors")
FORMS = ("galoot", "reference")
SEED = 20261110
CALIBRATION_PASSES = 65536
TARGET_S = 0.8
SHORT_DIVISOR = 8
TICK_S = 0.01
BUSY_TICKS = 2

# Passes per (operation, loop), from the calibration (out/calibration.txt):
# TARGET_S / (128 x the slower compiler's nanoseconds per operation),
# rounded to a multiple of 8192; the same for both forms and both compilers,
# so that every pair of runs compared has the same work.
PASSES = {
    ("wedge_point_point", "throughput"): 1286144,
    ("wedge_point_point_point", "throughput"): 2146304,
    ("antiwedge_line_plane", "throughput"): 1245184,
    ("transform_point", "throughput"): 802816,
    ("transform_point", "latency"): 557056,
    ("transform_line", "throughput"): 606208,
    ("transform_line", "latency"): 385024,
    ("transform_plane", "throughput"): 843776,
    ("transform_plane", "latency"): 655360,
    ("compose_motors", "throughput"): 614400,
    ("compose_motors", "latency"): 491520,
}

ROW_COLUMNS = ["env", "round", "compiler", "operation", "loop", "form",
               "attempt", "kind", "passes", "operations", "ns_per_op",
               "checksum", "wall_s", "inst_p_u", "cycles_u", "sibling_busy_s",
               "pinned_other_s", "busy", "selection"]


def loops(operation):
    chains = operation.startswith(("transform_", "compose_"))
    return ("throughput", "latency") if chains else ("throughput",)


def program(compiler, operation):
    return ROOT / "build" / COMPILERS[compiler] / "test" / "bench" / (
        f"bench_{operation}")


def last_row(log):
    with open(log, encoding="utf-8") as f:
        lines = f.read().splitlines()
    return dict(zip(lines[0].split("\t"), lines[-1].split("\t")))


def number(text):
    try:
        return float(text)
    except ValueError:
        return None


def one_process(out, cpu, env, selection, compiler, operation, form, loop,
                passes):
    """Runs one process and returns its fields, or exits on any surprise."""
    log = out / "runs.tsv"
    pad = [f"BENCH_ENV_PAD={'x' * env}"] if env else []
    command = ["taskset", "-c", str(cpu), str(ROOT / "tools" / "timed"),
               "--log", str(log), "bench", COMPILERS[compiler], selection,
               "--", "env", "-i", *pad, str(program(compiler, operation)), form,
               loop, str(passes), str(SEED)]
    result = subprocess.run(command, cwd=ROOT, capture_output=True,
                            text=True)
    if result.returncode:
        sys.exit(f"drive: {selection} exited {result.returncode}:\n"
                 f"{result.stdout}{result.stderr}")
    row = last_row(log)
    if row.get("selection") != selection or row.get("mode") != "pinned":
        sys.exit(f"drive: the last row of {log} is not a pinned row of "
                 f"{selection}")
    words = result.stdout.split()
    if len(words) != 7 or words[:3] != [operation, form, loop]:
        sys.exit(f"drive: {selection} printed {result.stdout!r}")
    return {"passes": passes, "operations": words[3], "ns_per_op": words[5],
            "checksum": words[6], "wall_s": row["wall_s"],
            "inst_p_u": row.get("inst_p_u", "unavailable"),
            "cycles_u": row.get("cycles_u", "unavailable"),
            "sibling_busy_s": row.get("sibling_busy_s", "unavailable"),
            "pinned_other_s": row.get("pinned_other_s", "unavailable"),
            "selection": selection}


def busy_pair(processes):
    """Whether the sibling thread, or other processes on the pinned CPU,
    were busy for more than BUSY_TICKS ticks over the processes."""
    processes = list(processes)
    for field in ("sibling_busy_s", "pinned_other_s"):
        busy = [number(p[field]) for p in processes]
        # The figures have two decimals, whole ticks at 100 Hz.
        if None in busy or sum(busy) > BUSY_TICKS * TICK_S + 1e-9:
            return True
    return False


def calibrate(args):
    out = Path(args.out_dir).resolve()
    out.mkdir(parents=True, exist_ok=True)
    for compiler in COMPILERS:
        for operation in OPERATIONS:
            for loop in loops(operation):
                selection = (f"wu17-bench-inline:calibrate:{compiler}:{operation}:"
                             f"galoot:{loop}")
                r = one_process(out, args.cpu, 0, selection, compiler,
                                operation, "galoot", loop, CALIBRATION_PASSES)
                print(compiler, operation, loop, r["ns_per_op"], r["wall_s"],
                      r["sibling_busy_s"], r["pinned_other_s"], flush=True)
    return 0


def run(args):
    if None in PASSES.values():
        sys.exit("drive: PASSES is not filled in; run calibrate first")
    out = Path(args.out_dir).resolve()
    out.mkdir(parents=True, exist_ok=True)
    rows_path = out / "rows.tsv"
    new = not rows_path.exists()
    with open(rows_path, "a", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", lineterminator="\n")
        if new:
            writer.writerow(ROW_COLUMNS)
        for rnd in range(1, args.repeats + 1):
            compilers = list(COMPILERS)
            forms = list(FORMS)
            if rnd % 2 == 0:
                compilers.reverse()
                forms.reverse()
            for compiler in compilers:
                for operation in OPERATIONS:
                    for loop in loops(operation):
                        for form in forms:
                            pair(writer, f, out, args, rnd, compiler,
                                 operation, loop, form)
    return 0


def pair(writer, f, out, args, rnd, compiler, operation, loop, form):
    passes = PASSES[operation, loop]
    for attempt in (1, 2):
        results = {}
        for kind, p in (("short", passes // SHORT_DIVISOR), ("long", passes)):
            selection = (f"wu17-bench-inline:{compiler}:{operation}:{form}:{loop}:"
                         f"env{args.env}:r{rnd}:a{attempt}:{kind}")
            results[kind] = one_process(out, args.cpu, args.env, selection,
                                        compiler, operation, form, loop, p)
        over = busy_pair(results.values())
        busy = ("no" if not over else
                "repeated" if attempt == 1 else "busy-again")
        for kind, r in results.items():
            writer.writerow([args.env, rnd, compiler, operation, loop, form,
                             attempt, kind, r["passes"], r["operations"],
                             r["ns_per_op"], r["checksum"], r["wall_s"],
                             r["inst_p_u"], r["cycles_u"],
                             r["sibling_busy_s"], r["pinned_other_s"], busy,
                             r["selection"]])
        f.flush()
        print(f"env {args.env} r{rnd} {compiler} {operation} {loop} {form} "
              f"a{attempt}: {results['long']['ns_per_op']} ns/op, busy "
              f"{busy}", flush=True)
        if not over:
            return


STALL_EVENTS = ("cpu_core/ld_blocks.store_forward/u",
                "cpu_core/br_inst_retired.near_call/u",
                "cpu_core/fp_arith_inst_retired.scalar_double/u",
                "cpu_core/fp_arith_inst_retired.128b_packed_double/u")


def stall_counts(cpu, selection, compiler, operation, form, loop, passes):
    """The counts of STALL_EVENTS of one pinned process, by perf stat, and
    its number of operations."""
    fd, counts = tempfile.mkstemp(prefix="drive-stalls-", suffix=".csv")
    os.close(fd)
    try:
        command = ["taskset", "-c", str(cpu), "perf", "stat", "-x", ",",
                   "-e", ",".join(STALL_EVENTS), "-o", counts, "env", "-i",
                   str(program(compiler, operation)), form, loop, str(passes),
                   str(SEED)]
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True)
        if result.returncode:
            sys.exit(f"drive: {selection} exited {result.returncode}:\n"
                     f"{result.stdout}{result.stderr}")
        found = {}
        with open(counts, encoding="utf-8") as f:
            for line in f:
                fields = line.strip().split(",")
                if len(fields) > 4 and fields[2] in STALL_EVENTS:
                    if not fields[0].isdigit() or fields[4] != "100.00":
                        sys.exit(f"drive: {selection}: {line.strip()}")
                    found[fields[2]] = int(fields[0])
        if len(found) != len(STALL_EVENTS):
            sys.exit(f"drive: {selection}: counts missing: {found}")
        return ([found[e] for e in STALL_EVENTS],
                int(result.stdout.split()[3]))
    finally:
        os.unlink(counts)


def stalls(args):
    out = Path(args.out_dir).resolve()
    out.mkdir(parents=True, exist_ok=True)
    lines = []
    for compiler in COMPILERS:
        for operation in OPERATIONS:
            for loop in loops(operation):
                for form in FORMS:
                    passes = PASSES[operation, loop]
                    counts = []
                    for kind, p in (("short", passes // SHORT_DIVISOR),
                                    ("long", passes)):
                        selection = (f"wu17-bench-inline:stalls:{compiler}:"
                                     f"{operation}:{form}:{loop}:{kind}")
                        counts.append(stall_counts(args.cpu, selection,
                                                   compiler, operation, form,
                                                   loop, p))
                    (a, na), (b, nb) = counts
                    lines.append([compiler, operation, loop, form] + [
                        f"{(y - x) / (nb - na):.3f}" for x, y in zip(a, b)])
                    print(*lines[-1], flush=True)
    write(out / "stalls.tsv", ["compiler", "operation", "loop", "form",
                               "store_forward_blocks_per_op",
                               "calls_per_op", "scalar_double_per_op",
                               "packed_double_per_op"], lines)
    return 0


def read_rows(out):
    with open(out / "rows.tsv", encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def used_pairs(rows):
    """The pair used for each (env, round, compiler, operation, loop, form):
    the last attempt, as {"short": row, "long": row}."""
    pairs = {}
    for row in rows:
        key = (int(row["env"]), int(row["round"]), row["compiler"],
               row["operation"], row["loop"], row["form"])
        attempt = int(row["attempt"])
        held = pairs.get(key)
        if held is None or held["attempt"] < attempt:
            pairs[key] = {"attempt": attempt}
        if pairs[key]["attempt"] == attempt:
            pairs[key][row["kind"]] = row
    return pairs


def per_operation(pair, field):
    short, long = pair["short"], pair["long"]
    a, b = number(short[field]), number(long[field])
    if a is None or b is None:
        return None
    return (b - a) / (int(long["operations"]) - int(short["operations"]))


def summarize(args):
    out = Path(args.out_dir).resolve()
    rows = read_rows(out)
    pairs = used_pairs(rows)

    # The two forms compute the same results: equal checksums for every
    # (env, round, compiler, operation, loop, kind).
    checks = {}
    for row in rows:
        key = (row["env"], row["round"], row["compiler"], row["operation"],
               row["loop"], row["kind"], row["passes"])
        checks.setdefault(key, set()).add(row["checksum"])
    disagree = [k for k, v in checks.items() if len(v) != 1]
    if disagree:
        sys.exit(f"drive: the forms' checksums disagree for {disagree}")

    groups = {}
    for (env, _, compiler, operation, loop, form), pair in pairs.items():
        g = groups.setdefault((compiler, operation, loop, form, env), {
            "ns": [], "inst": [], "cycles": [], "flags": 0})
        g["ns"].append(float(pair["long"]["ns_per_op"]))
        g["inst"].append(per_operation(pair, "inst_p_u"))
        g["cycles"].append(per_operation(pair, "cycles_u"))
        g["flags"] += pair["long"]["busy"] == "busy-again"

    def stats(values):
        if not values or None in values:
            return "unavailable", "unavailable"
        return (f"{statistics.median(values):.3f}",
                f"{max(values) - min(values):.3f}")

    lines = []
    for key in sorted(groups):
        g = groups[key]
        cells = [*key[:4], str(key[4]), str(len(g["ns"]))]
        for name in ("ns", "inst", "cycles"):
            cells += stats(g[name])
        cells.append(str(g["flags"]))
        lines.append(cells)
    write(out / "results.tsv",
          ["compiler", "operation", "loop", "form", "env", "runs",
           "ns_median", "ns_range", "inst_median", "inst_range",
           "cycles_median", "cycles_range", "busy_again"], lines)

    comparison = []
    for (compiler, operation, loop, form, env) in sorted(groups):
        if form != "galoot":
            continue
        g = groups[compiler, operation, loop, "galoot", env]
        r = groups[compiler, operation, loop, "reference", env]
        mg, mr = statistics.median(g["ns"]), statistics.median(r["ns"])
        limit = 2 * max(max(g["ns"]) - min(g["ns"]),
                        max(r["ns"]) - min(r["ns"]))
        faster = "galoot" if mg < mr else "reference" if mr < mg else "-"
        comparison.append([compiler, operation, loop, str(env),
                           f"{mg:.3f}", f"{mr:.3f}", f"{mg - mr:+.3f}",
                           f"{limit:.3f}",
                           "yes" if abs(mg - mr) > limit else "no", faster])
    write(out / "comparison.tsv",
          ["compiler", "operation", "loop", "env", "galoot_ns",
           "reference_ns", "difference_ns", "twice_larger_range",
           "beyond", "faster"], comparison)

    # The criterion over the environment sizes: "every size" calls a
    # difference when the medians differ beyond twice the larger range at
    # each size and the same form is faster at each; "first size" when they
    # differ beyond it at the smallest size and the same form is faster at
    # each.
    verdicts = {}
    for compiler, operation, loop, env, *_, beyond, faster in comparison:
        verdicts.setdefault((compiler, operation, loop), []).append(
            (int(env), beyond == "yes", faster))
    lines = []
    for key, sizes in sorted(verdicts.items()):
        sizes.sort()
        same = len({f for _, _, f in sizes}) == 1
        every = same and all(b for _, b, _ in sizes)
        first = same and sizes[0][1]
        lines.append([*key, "/".join(str(e) for e, _, _ in sizes),
                      "/".join("yes" if b else "no" for _, b, _ in sizes),
                      "/".join(f for _, _, f in sizes),
                      "yes" if every else "no", "yes" if first else "no",
                      sizes[0][2] if first else "-"])
    write(out / "verdict.tsv",
          ["compiler", "operation", "loop", "envs", "beyond", "faster",
           "every_size", "first_size", "faster_form"], lines)
    print(f"{len(pairs)} pairs; results.tsv, comparison.tsv, and "
          "verdict.tsv written")
    return 0


def write(path, columns, lines):
    with open(path, "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", lineterminator="\n")
        writer.writerow(columns)
        writer.writerows(lines)


def main(argv):
    parser = argparse.ArgumentParser(prog="drive.py", usage=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    c = commands.add_parser("calibrate")
    c.add_argument("out_dir")
    c.add_argument("--cpu", required=True, type=int)
    r = commands.add_parser("run")
    r.add_argument("out_dir")
    r.add_argument("--cpu", required=True, type=int)
    r.add_argument("--env", required=True, type=int)
    r.add_argument("--repeats", type=int, default=11)
    s = commands.add_parser("summarize")
    s.add_argument("out_dir")
    d = commands.add_parser("stalls")
    d.add_argument("out_dir")
    d.add_argument("--cpu", required=True, type=int)
    args = parser.parse_args(argv)
    return {"calibrate": calibrate, "run": run, "summarize": summarize,
            "stalls": stalls}[args.command](args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
