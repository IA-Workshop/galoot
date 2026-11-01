"""Checks that the compiler counts the constant evaluation of reference
plans exactly as the library's documented figures say.

Usage:
  python3 -I plan_cost_model.py <clang|gcc> <source>=<threshold>...
      -- <compile command>...

Each threshold is the smallest limit on constant evaluation under which its
source compiles: Clang's -fconstexpr-steps, or GCC's -fconstexpr-ops-limit
with its cache of constant calls turned off (-fconstexpr-cache-depth=1), so
that the count does not depend on what the unit evaluated before. For each
source the check compiles it with -fsyntax-only at its threshold, where it
must compile, and at one below, where it must fail with the compiler's
message for that limit.

On Clang that is enough: Clang counts one step per statement and stops at
the limit, so a unit compiles exactly under the limits at or above its
count. GCC is not so regular. When its count runs out inside the argument of
a built-in function, it drops the error and stops counting, so a unit can
compile under a limit below its count, and a unit whose count rose could
still compile at the recorded threshold. So on GCC the unit must also
compile at a set of limits just above the threshold, from one above to a
tenth above. With that, a pass means that the compiler's count of the unit's
largest evaluation is the one recorded, as far as compiles can tell: a
count that falls exactly where GCC stops counting at every limit tried is
the one case no compile can tell apart.

When a source does not behave so, the check searches for the threshold the
compiler gives it now and prints both, so that the figures can be worked
out again. The search confirms its result above it in the same way.

Exits 1 if any source fails the check, 0 otherwise, and prints one line per
source either way.
"""

import subprocess
import sys

LIMIT_FLAG = {"clang": "-fconstexpr-steps=", "gcc": "-fconstexpr-ops-limit="}
LIMIT_MESSAGE = {"clang": "maximum step limit",
                 "gcc": "operation count exceeds limit"}
OTHER_FLAGS = {"clang": [], "gcc": ["-fconstexpr-cache-depth=1"]}
# The largest limit the search tries: what Clang's unsigned counter holds,
# and for GCC more than any plan the library can generate needs.
LARGEST_LIMIT = {"clang": (1 << 32) - 1, "gcc": 1 << 40}


def compile_at(kind, command, source, limit):
    """Whether `source` compiles under `limit`, and the compiler's
    output."""
    result = subprocess.run(
        [*command, *OTHER_FLAGS[kind], f"{LIMIT_FLAG[kind]}{limit}",
         "-fsyntax-only", source],
        capture_output=True, text=True, check=False)
    return result.returncode == 0, result.stdout + result.stderr


def just_above(limit):
    """The limits just above `limit` that a GCC result must also compile
    under."""
    return sorted({limit + d for d in (1, 2, 3, 7, 31, 127)} |
                  {limit + max(1, limit // f) for f in (1000, 100, 10)})


def search(kind, command, source, start):
    """The smallest limit under which `source` compiles now, or None if it
    does not compile under the largest limit."""
    def passes(limit):
        return compile_at(kind, command, source, limit)[0]

    # A failing limit below and a passing one above, widening from start.
    if passes(start):
        high, low = start, start // 2
        while low > 0 and passes(low):
            high, low = low, low // 2
    else:
        low, high = start, start * 2
        while not passes(high):
            if high >= LARGEST_LIMIT[kind]:
                return None
            low, high = high, min(high * 2, LARGEST_LIMIT[kind])

    while True:
        while high - low > 1:
            middle = (low + high) // 2
            if passes(middle):
                high = middle
            else:
                low = middle
        # Accept the result only if limits just above it compile too; one
        # that fails shows the count is higher, so search on above it.
        failing = next((limit for limit in just_above(high)
                        if not passes(limit)), None)
        if failing is None:
            return high
        low, high = failing, failing * 2
        while not passes(high):
            if high >= LARGEST_LIMIT[kind]:
                return None
            low, high = high, min(high * 2, LARGEST_LIMIT[kind])


GCC_BUILTIN_MESSAGE = "is not a constant expression"


def check(kind, command, source, threshold):
    """An empty string if the source passes, else what went wrong."""
    at_ok, at_output = compile_at(kind, command, source, threshold)
    below_ok, below_output = compile_at(kind, command, source, threshold - 1)
    limit_below = LIMIT_MESSAGE[kind] in below_output

    if at_ok and not below_ok and limit_below:
        if kind != "gcc":
            return ""
        failing = next((limit for limit in just_above(threshold)
                        if not compile_at(kind, command, source, limit)[0]),
                       None)
        if failing is None:
            return ""
        problem = (f"compiles at its threshold, but it does not compile just "
                   f"above its threshold, at {failing}")
    # GCC does not always name the limit when its count runs out: inside the
    # argument of a built-in function it reports that argument as not a
    # constant expression instead. So a failure at the threshold is put down
    # to the limit when the source compiles under the largest limit.
    elif not at_ok and not compile_at(kind, command, source,
                                      LARGEST_LIMIT[kind])[0]:
        return ("does not compile under any limit, so not because of the "
                "limit:\n" + at_output)
    elif not at_ok:
        problem = "does not compile at its threshold"
    elif below_ok:
        # On GCC this can also be the count running out inside the argument
        # of a built-in function, which no compile can tell from a lower
        # count.
        problem = "compiles one below its threshold"
    elif kind == "gcc" and GCC_BUILTIN_MESSAGE in below_output:
        problem = ("fails one below its threshold with GCC's message for a "
                   "count that runs out inside the argument of a built-in "
                   "function, not with the message for its limit")
    else:
        return ("fails one below its threshold for another reason than the "
                "limit:\n" + below_output)
    measured = search(kind, command, source, threshold)
    if measured is None:
        return f"{problem}, and does not compile at any limit tried"
    return f"{problem}; its measured threshold is {measured}"


def main(argv):
    if "--" not in argv or len(argv) < 4 or argv[0] not in LIMIT_FLAG:
        sys.exit(__doc__)
    kind = argv[0]
    split = argv.index("--")
    cases, command = argv[1:split], argv[split + 1:]
    if not cases or not command:
        sys.exit(__doc__)

    failed = 0
    for case in cases:
        source, threshold = case.rsplit("=", 1)
        problem = check(kind, command, source, int(threshold))
        if problem:
            failed += 1
            print(f"{source}: threshold {threshold} recorded; {problem}")
        else:
            above = " and just above it" if kind == "gcc" else ""
            print(f"{source}: compiles at {threshold}{above} and fails at "
                  f"{int(threshold) - 1}, as recorded")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main(sys.argv[1:])
