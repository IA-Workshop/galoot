"""Checks, from a Clang -ftime-trace of one translation unit, that each plan
the unit uses is generated once, however many times it is read.

Usage:
  python3 -I evaluated_once.py <trace> <variable> <generator> [-- <command>...]

With a command, runs it first; it is the compile that writes <trace>, and it
must succeed. Then reads the complete events of <trace>:

- the evaluations of the initializer of the variable template <variable>,
  events named EvaluateAsInitializer whose detail is <variable>, which the
  trace names without its template arguments;
- the instantiations of the function template <generator> that the
  initializer calls, events named InstantiateFunction whose detail starts
  with <generator><.

The generator computes the plan into static constexpr locals, which are
evaluated once, when the generator is instantiated, so the initializer only
copies the result and takes a small part of the time of the instantiation.
If the initializer generated the plan again, it would take about as long as
the instantiation. The check passes when the median duration of the
evaluations is below a tenth of the median duration of the instantiations.
Medians, because the trace names the variable template without its
arguments, so an evaluation cannot be paired with its instantiation, and
because one interval stretched by other work on the machine moves a median
of three or more only to its neighbor.

Exits 1, naming the counts, if either kind of event occurs fewer than three
times, so that a renamed variable or generator cannot pass the check
vacuously; exits 1 if the medians fail the check; exits 0 otherwise. Prints
the counts and medians in either case.
"""

import json
import statistics
import subprocess
import sys

# At least this many plans, so that a median is not one event.
MINIMUM_EVENTS = 3
# The initializer takes under a thousandth of the instantiation when it only
# copies the plan, and from a quarter to most of it when it generates the
# plan again, so a tenth lies far from both.
LARGEST_RATIO = 0.1


def durations(events, name, matches):
    return [e["dur"] for e in events
            if e.get("name") == name
            and matches(e.get("args", {}).get("detail", ""))]


def main(argv):
    if "--" in argv:
        split = argv.index("--")
        arguments, command = argv[:split], argv[split + 1:]
    else:
        arguments, command = argv, []
    if len(arguments) != 3:
        sys.exit(__doc__)
    trace, variable, generator = arguments

    if command:
        compile_run = subprocess.run(command, capture_output=True, text=True,
                                     check=False)
        if compile_run.returncode != 0:
            print(compile_run.stdout + compile_run.stderr)
            print(f"the compile exited with {compile_run.returncode}")
            return 1

    with open(trace, encoding="utf-8") as f:
        events = [e for e in json.load(f)["traceEvents"] if e.get("ph") == "X"]
    evaluations = durations(events, "EvaluateAsInitializer",
                            lambda detail: detail == variable)
    instantiations = durations(events, "InstantiateFunction",
                               lambda detail: detail.startswith(generator + "<"))
    print(f"{len(evaluations)} evaluations of the initializer of {variable}, "
          f"{len(instantiations)} instantiations of {generator}")
    if len(evaluations) < MINIMUM_EVENTS or len(instantiations) < MINIMUM_EVENTS:
        print(f"fewer than {MINIMUM_EVENTS} of either: the trace cannot show "
              "whether a plan is generated once")
        return 1

    evaluation = statistics.median(evaluations)
    instantiation = statistics.median(instantiations)
    print(f"median evaluation {evaluation} us, median instantiation "
          f"{instantiation} us, ratio {evaluation / instantiation:.4f}")
    if evaluation >= LARGEST_RATIO * instantiation:
        print(f"the initializer takes at least {LARGEST_RATIO} of the "
              "instantiation, so it generates the plan again")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
