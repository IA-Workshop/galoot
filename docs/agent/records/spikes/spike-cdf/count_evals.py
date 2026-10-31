"""Count the expensive constant evaluations in a Clang -ftime-trace file
(snippet candidate; written for Spike D, last checked at aa8051c0 with
Clang 23.1.4).

Compile with `clang++ -ftime-trace -ftime-trace-granularity=0 -c -o x.o x.cpp`;
Clang writes x.json beside the object. Each constant evaluation appears as an
event named Evaluate* (EvaluateAsInitializer, EvaluateAsConstantExpr, ...)
whose detail is the variable or source range evaluated. This keeps the
outermost Evaluate* events (an evaluation nested in another is part of it)
longer than a threshold, and prints how many there are per detail. For a
memoized table, each argument set should appear once per translation unit.

GCC has no per-evaluation trace; there, compare -ftime-report's "constant
expression evaluation" line and the retired instruction count (Spike D,
research note 0011).

Usage: python3 -I count_evals.py TRACE.json [MIN_MS]   (default 20 ms)
"""

import collections
import json
import sys


def outermost_evaluations(path, min_ms=20.0):
    """Return the outermost Evaluate* events of at least min_ms."""
    with open(path) as f:
        events = [e for e in json.load(f)["traceEvents"]
                  if e.get("ph") == "X" and e["name"].startswith("Evaluate")]
    events.sort(key=lambda e: (e["tid"], e["ts"], -e["dur"]))
    outer, end = [], {}
    for e in events:
        if e["ts"] < end.get(e["tid"], -1):
            continue  # nested in an outer evaluation
        end[e["tid"]] = e["ts"] + e["dur"]
        outer.append(e)
    return [e for e in outer if e["dur"] >= min_ms * 1000]


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit("usage: count_evals.py TRACE.json [MIN_MS]")
    min_ms = float(sys.argv[2]) if len(sys.argv) == 3 else 20.0
    evs = outermost_evaluations(sys.argv[1], min_ms)
    counts = collections.Counter(
        f"{e['name']}: {e.get('args', {}).get('detail', '')}" for e in evs)
    for what, n in counts.most_common():
        total = sum(e["dur"] for e in evs
                    if f"{e['name']}: {e.get('args', {}).get('detail', '')}"
                    == what) / 1e6
        print(f"{n}\t{total:.3f} s\t{what}")
    print(f"{len(evs)}\ttotal evaluations of at least {min_ms:g} ms")


if __name__ == "__main__":
    main()
