"""Mutation runs for metric_like (metric.hpp): apply one mutation, build
`all`, run the whole suite, and restore the header. metric.hpp is included
by every core header, so every test can be affected. Each mutation's anchor
must occur exactly once.

A mutant is killed by a test when the build fails on an error that is not a
warning, or when CTest reports a failing test. For a failed build the record
gives the static_asserts that failed and the lines of test files the
compiler names (the test that reached the error), so that a kill by a test
can be told from a kill by anything else; a build whose only errors are
warnings promoted by -Werror is recorded as "warning", which counts as a
survivor. CTest is not run after a failed build: the mutant is decided
already, and CTest under the shared job pool can keep a slot it took for a
test whose program is missing. After the last mutant the header is restored
and `all` is built again.

With --runtime, every static_assert of test/core/metric_test.cpp becomes
`static_assert(true ? true : ...)` for the run, so that its program builds on
a mutant those checks would reject, and the record lists the test cases that
fail at run time. Only the mutants of the pass itself are run in this mode by
default, since the others change no value that a run-time test reads.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0014-metric-like-single-pass/mutate.py \\
      <preset> [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
METRIC = f"{ROOT}/include/galoot/core/metric.hpp"
METRIC_TEST = f"{ROOT}/test/core/metric_test.cpp"

CONDITION = "      if (a != b || a < -max_form_entry || a > max_form_entry) {\n"
LOOPS = ("  for (std::size_t i = 0; i < N; ++i) {\n"
         "    for (std::size_t j = 0; j <= i; ++j) {\n")
COUNT_TEST = ("    // Taken from the type alone, so that gram[0][0] exists below.\n"
              "    (std::remove_cvref_t<decltype(Alg::value)>::generator_count "
              ">= 1) &&\n")
GUARD = ("    requires {\n"
         "      typename std::integral_constant<entry_type, "
         "Alg::value.gram[0][0]>;\n"
         "    } && detail::is_valid_form_v<Alg>;\n")
TYPE_TEST = ("    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> "
             "&&\n")

# (name, anchor, replacement, a mutant of the pass)
MUTATIONS = [
    ("no_symmetry", CONDITION,
     "      if (a < -max_form_entry || a > max_form_entry) {\n", True),
    ("no_range", CONDITION, "      if (a != b) {\n", True),
    ("no_lower_bound", CONDITION,
     "      if (a != b || a > max_form_entry) {\n", True),
    ("no_upper_bound", CONDITION,
     "      if (a != b || a < -max_form_entry) {\n", True),
    ("lower_bound_inclusive", CONDITION,
     "      if (a != b || a <= -max_form_entry || a > max_form_entry) {\n",
     True),
    ("upper_bound_inclusive", CONDITION,
     "      if (a != b || a < -max_form_entry || a >= max_form_entry) {\n",
     True),
    ("diagonal_skipped", LOOPS,
     LOOPS.replace("j <= i", "j < i"), True),
    ("last_row_skipped", LOOPS,
     LOOPS.replace("i < N", "i + 1 < N"), True),
    ("no_count_test", COUNT_TEST, "", False),
    ("no_guard", GUARD, "    detail::is_valid_form_v<Alg>;\n", False),
    ("guard_reads_count", GUARD,
     GUARD.replace("std::integral_constant<entry_type, Alg::value.gram[0][0]>",
                   "std::integral_constant<std::size_t, "
                   "Alg::value.generator_count>"), False),
    ("no_type_test", TYPE_TEST, "", False),
    ("no_cache", GUARD,
     GUARD.replace("detail::is_valid_form_v<Alg>",
                   "detail::is_valid_form(Alg::value)"), False),
]

STATIC_ASSERT = re.compile(
    r"([\w./-]+\.(?:cpp|hpp)):(\d+):\d+: error: "
    r"(?:static assertion|static_assert)")
TEST_LINE = re.compile(r"(test/[\w./-]+\.cpp):(\d+)")


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def failed_cases(output):
    names = []
    for line in output.splitlines():
        if line.startswith("TEST CASE:"):
            name = line[len("TEST CASE:"):].strip()
            if name not in names:
                names.append(name)
    return names


def write(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)


def main():
    args = sys.argv[1:]
    preset = args[0]
    runtime = "--runtime" in args[1:]
    only = {a for a in args[1:] if not a.startswith("--")}
    originals = {path: open(path, encoding="utf-8").read()
                 for path in (METRIC, METRIC_TEST)}

    test_text = originals[METRIC_TEST]
    if runtime:
        assert test_text.count("static_assert(") > 0
        test_text = test_text.replace("static_assert(",
                                      "static_assert(true ? true : ")

    print("mutation\tpreset\tmode\tbuild_exit\ttest_exit\tverdict\t"
          "failed_tests\tstatic_asserts\ttest_lines\tfailed_cases\t"
          "first_build_error")
    mode = "runtime" if runtime else "normal"
    try:
        write(METRIC_TEST, test_text)
        for name, anchor, replacement, of_pass in MUTATIONS:
            if only and name not in only:
                continue
            if runtime and not only and not of_pass:
                continue
            source = originals[METRIC]
            assert source.count(anchor) == 1, (name, anchor)
            write(METRIC, source.replace(anchor, replacement))
            build = run(["tools/timed", "build", preset,
                         f"mutation:{name}:{mode}", "--", "cmake", "--build",
                         "--preset", preset])
            if build.returncode == 0:
                test = run(["tools/timed", "test", preset,
                            f"mutation:{name}:{mode}", "--", "ctest",
                            "--preset", preset, "--output-on-failure"])
                test_exit, output = test.returncode, test.stdout
            else:
                test_exit, output = "-", ""
            failed = [line.split(" - ")[1].split()[0]
                      for line in output.splitlines()
                      if re.match(r"\s*\d+ - ", line)]
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            warnings_only = bool(errors) and all("[-Werror" in line
                                                 for line in errors)
            if build.returncode != 0:
                verdict = "warning" if warnings_only else "killed"
            else:
                verdict = "killed" if test_exit != 0 else "SURVIVED"
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            asserts = sorted({f"{m.group(1).split('/')[-1]}:{m.group(2)}"
                              for m in STATIC_ASSERT.finditer(build.stdout)})
            lines = sorted({f"{m.group(1).split('/')[-1]}:{m.group(2)}"
                            for m in TEST_LINE.finditer(build.stdout)},
                           key=lambda s: (s.split(":")[0],
                                          int(s.split(":")[1])))
            print(f"{name}\t{preset}\t{mode}\t{build.returncode}\t"
                  f"{test_exit}\t{verdict}\t{','.join(failed) or '-'}\t"
                  f"{','.join(asserts) or '-'}\t"
                  f"{','.join(lines[:12]) or '-'}\t"
                  f"{' | '.join(failed_cases(output)) or '-'}\t"
                  f"{first_error[:200]}", flush=True)
    finally:
        for path, text in originals.items():
            write(path, text)
        run(["tools/timed", "build", preset, "mutation:restored", "--", "cmake",
             "--build", "--preset", preset])


if __name__ == "__main__":
    main()
