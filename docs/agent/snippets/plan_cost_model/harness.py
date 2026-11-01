"""Compile a unit under a lowered constant-evaluation limit and find the
smallest limit under which it compiles.

Imported by probe.py. Every compile runs through `tools/timed measure`, so
that it has a row in runs.tsv, and holds one slot of the machine's shared
job pool while it runs, so that parallel searches share the processors with
other work instead of adding to it.

The counters:

  gcc    -fconstexpr-ops-limit=L. GCC counts each call of its evaluator on a
         node that is not a plain constant, per outermost evaluation, and
         fails when the count reaches L. So the unit compiles exactly when
         every evaluation in it counts fewer than L, and the smallest passing
         L is the largest count plus one.
  clang  -fconstexpr-steps=L. Clang's classic evaluator takes one step per
         statement it evaluates and fails when a step is needed after L have
         been taken; its bytecode interpreter takes one step per jump taken
         and fails when L reach zero. Either way the smallest passing L
         follows the largest evaluation of the unit.

A count below the limit always compiles, but on GCC a count at or above it
does not always fail (see threshold()), so the search confirms each result
by compiling just above it.
"""

import os
import pathlib
import subprocess
import threading

ROOT = pathlib.Path(subprocess.run(
    ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True,
    check=True, cwd=pathlib.Path(__file__).parent).stdout.strip())
COMPILERS = {"gcc": "g++", "clang": "clang++"}
GALOOT = ["-std=c++23", "-Wall", "-Wextra", "-Wpedantic", "-Wconversion",
          "-Wshadow", "-Werror", "-ffp-contract=off", "-D_GLIBCXX_ASSERTIONS"]
CONFIGS = {
    "galoot-O0": GALOOT + ["-O0", "-DGALOOT_CHECKING=1"],
    "galoot-O2": GALOOT + ["-O2", "-DGALOOT_CHECKING=0"],
    "bare-O2": ["-std=c++23", "-pedantic-errors", "-O2"],
}
LIMIT_FLAG = {"gcc": "-fconstexpr-ops-limit=", "clang": "-fconstexpr-steps="}
LIMIT_TEXT = {"gcc": "operation count exceeds limit",
              "clang": "maximum step limit"}
# Clang also refuses an array with more elements than the step limit.
ARRAY_TEXT = "exceeds the limit"
# The largest limit tried: 2^40 for GCC, and for Clang the largest value its
# unsigned counter holds.
CEILING = {"gcc": 1 << 40, "clang": (1 << 32) - 1}


class Pool:
    """Slots of the shared FIFO jobserver named in MAKEFLAGS."""

    def __init__(self):
        flags = os.environ.get("MAKEFLAGS", "")
        marker = "--jobserver-auth=fifo:"
        if marker not in flags:
            raise SystemExit("no shared job pool: run "
                             'eval "$(tools/jobpool env)" first')
        path = flags.split(marker, 1)[1].split()[0]
        self.fd = os.open(path, os.O_RDWR)
        self.lock = threading.Lock()

    def take(self):
        return os.read(self.fd, 1)

    def give(self, token):
        os.write(self.fd, token)


def compile_once(pool, compiler, config, source, selection, extra=()):
    """Compile `source` with -fsyntax-only; returns (exit status, stderr)."""
    command = [str(ROOT / "tools" / "timed"), "measure", "-", selection, "--",
               COMPILERS[compiler], *CONFIGS[config], *extra,
               f"-I{ROOT / 'include'}", "-fsyntax-only", str(source)]
    token = pool.take()
    try:
        result = subprocess.run(command, cwd=ROOT, capture_output=True,
                                text=True)
    finally:
        pool.give(token)
    return result.returncode, result.stderr


def failure_kind(compiler, stderr):
    if LIMIT_TEXT[compiler] in stderr:
        return "limit"
    if compiler == "clang" and ARRAY_TEXT in stderr:
        return "array"
    return "other"


def first_location(stderr):
    """The first file:line of an error, for naming the evaluation that
    stopped."""
    for line in stderr.splitlines():
        if ": error:" in line or ": note:" in line:
            return line.split(": ", 1)[0].replace(f"{ROOT}/", "")
    return "-"


def threshold(pool, compiler, config, source, name, guess=1 << 16,
              extra=()):
    """The smallest limit under which `source` compiles, with the location
    and kind of the failure one below it, and the number of compiles."""
    flag = LIMIT_FLAG[compiler]
    compiles = 0

    passed = set()

    def passes(limit):
        nonlocal compiles
        compiles += 1
        status, err = compile_once(
            pool, compiler, config, source,
            f"ceiling-cert:{name}:{compiler}:{config}:L{limit}",
            [*extra, f"{flag}{limit}"])
        if status == 0:
            passed.add(limit)
        return status == 0, err

    ok, err = passes(CEILING[compiler])
    if not ok:
        return {"threshold": "fails", "kind": failure_kind(compiler, err),
                "where": first_location(err), "compiles": compiles,
                "stderr": err}
    # Bracket: a failing low and a passing high, widening around the guess,
    # so that an exact guess costs two compiles.
    low, high, last_err = 0, None, ""
    guess = max(1, guess)
    ok, err = passes(guess)
    if ok:
        high = guess
    else:
        low, last_err = guess, err
    for step in (1, 16, guess // 100, guess // 4):
        if high is not None and low != 0:
            break
        limit = high - step if high is not None else low + step
        if step < 1 or limit < 1:
            continue
        ok, err = passes(limit)
        if ok:
            high = limit
        else:
            low, last_err = limit, err
    while high is None or (low == 0 and high > 1):
        limit = (high // 4 if high is not None else
                 min(low * 4, CEILING[compiler]))
        ok, err = passes(max(1, limit))
        if ok:
            high = max(1, limit)
        else:
            low, last_err = max(1, limit), err
    # GCC's count is not monotone in the limit: when the limit is reached
    # inside the argument of a built-in function, which GCC evaluates
    # quietly, the error is dropped and the count restarts from its most
    # negative value, so the rest of that evaluation runs unlimited and the
    # unit can compile under a limit below its count. So a threshold found
    # by bisection is accepted only when limits just above it pass as well;
    # a limit above it that fails proves the count higher, and the search
    # continues from there.
    while True:
        while high - low > 1:
            mid = (low + high) // 2
            ok, err = passes(mid)
            if ok:
                high = mid
            else:
                low, last_err = mid, err
        above = sorted({high + d for d in (1, 2, 3, 7, 31, 127)} |
                       {high + max(1, high // f) for f in (10000, 1000, 100,
                                                            10)})
        failed = None
        for limit in above:
            ok, err = passes(limit)
            if not ok:
                failed = limit, err
                break
        if failed is None:
            break
        low, last_err = failed
        high = min(p for p in passed if p > low)
    return {"threshold": high, "kind": failure_kind(compiler, last_err),
            "where": first_location(last_err), "compiles": compiles,
            "stderr": last_err}
