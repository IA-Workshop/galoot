"""Mutation runs for WU8a (plan 0016) and its review fixes: mv.hpp, the blades<Alg, ...> change
in blade_set.hpp, the test rational, and the one-error option of the
compile-fail helper. Apply one mutation, build `all` on each preset given,
run the whole suite there, and restore the file. Each mutation's anchor must
occur exactly once.

A mutant is killed by a test when the build fails on an error that is not a
warning, or when CTest reports a failing test (a compile-fail test is a
test). For a failed build the record gives the static_asserts that failed
and the lines of test files the compiler names, so that a kill by a test can
be told from a kill by anything else; a build whose only errors are warnings
promoted by -Werror is recorded as "warning", which counts as a survivor.
CTest is not run after a failed build: the mutant is decided already, and
CTest under the shared job pool can keep a slot it took for a test whose
program is missing. After the last mutant every file is restored and `all`
is built again on each preset.

With --runtime, every static_assert of the doctest sources that read the
mutated headers (mv_test.cpp, rational_test.cpp, blade_set_test.cpp)
becomes `static_assert(true ? true : ...)` for the run, so that their
programs build on a mutant those checks would reject, and the record lists
the test cases that fail at run time and the compile-fail tests that fail.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0016-mv-a/mutate.py \\
      <preset>[,<preset>...] [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
MV = f"{ROOT}/include/galoot/core/mv.hpp"
BLADE_SET = f"{ROOT}/include/galoot/core/blade_set.hpp"
RATIONAL = f"{ROOT}/test/support/rational.hpp"
TESTING = f"{ROOT}/cmake/galoot_testing.cmake"
TESTS = [f"{ROOT}/test/core/mv_test.cpp",
         f"{ROOT}/test/support/rational_test.cpp",
         f"{ROOT}/test/core/blade_set_test.cpp"]

RANGE_CHECK = (
    "  static_assert(layout_names_only_generators_of_the_algebra,\n"
    "                \"mv: the layout names a generator that the algebra does "
    "not \"\n")
SIGN_CHECK = "  static_assert(layout_signs_are_plus_or_minus_one,\n"
REPEAT_CHECK = "  static_assert(layout_holds_each_blade_once,\n"
SCALAR_CHECK = "  static_assert(ring_scalar<T>,\n"
ORIENTATION = (
    "      const NameLookup result = layout.blades[at].sign == "
    "parsed.blade.sign\n"
    "                                    ? NameLookup::found\n"
    "                                    : NameLookup::other_orientation;\n")
EQUALITY = "    return coefficients == other.coefficients;\n"
EQUALITY_BUT_LAST = (
    "    for (std::size_t i = 0; i + 1 < sizeof...(Ts); ++i) {\n"
    "      if (!(coefficients[i] == other.coefficients[i])) {\n"
    "        return false;\n"
    "      }\n"
    "    }\n"
    "    return true;\n")
INVALID_NAME = (
    "  default:\n"
    "    return {.result = NameLookup::not_a_name, .at = K};\n")
INITIALIZER = "  std::array<T, sizeof...(Ts)> coefficients{Ts{}...};\n"
MV_HEAD = "template <metric_like Alg, blade_set Blades, std::semiregular T>\n"

# A check made always true by turning its static_assert's condition into
# `true ||`, so that the message and layout of the source are kept.
def disabled(anchor):
    return anchor.replace("static_assert(", "static_assert(true || ", 1)

# (name, file, anchor, replacement[, count]): count is how many times the
# anchor occurs and is replaced, 1 unless given.
MUTATIONS = [
    ("no_range_check", MV, RANGE_CHECK, disabled(RANGE_CHECK)),
    ("no_sign_check", MV, SIGN_CHECK, disabled(SIGN_CHECK)),
    ("no_repeat_check", MV, REPEAT_CHECK, disabled(REPEAT_CHECK)),
    ("no_scalar_check", MV, SCALAR_CHECK, disabled(SCALAR_CHECK)),
    ("range_ignores_algebra", MV,
     "inline constexpr bool blades_in_algebra = all_in_algebra(S, N);\n",
     "inline constexpr bool blades_in_algebra = all_in_algebra(S, mask_bits);\n"),
    ("repeat_scan_two_on", MV,
     "    for (std::size_t j = i + 1; j < K; ++j) {\n",
     "    for (std::size_t j = i + 2; j < K; ++j) {\n"),
    ("repeat_compares_orientation", MV,
     "      if (set.blades[i].mask == set.blades[j].mask) {\n",
     "      if (set.blades[i] == set.blades[j]) {\n"),
    ("ring_scalar_no_unary_minus", MV,
     "  { -a } -> std::convertible_to<T>;\n", ""),
    ("ring_requires_equality", MV,
     "  { -a } -> std::convertible_to<T>;\n",
     "  { -a } -> std::convertible_to<T>;\n"
     "  { a == b } -> std::convertible_to<bool>;\n"),
    ("exact_scalar_ignores_limits", MV,
     "concept exact_scalar = ring_scalar<T> && "
     "std::numeric_limits<T>::is_exact;\n",
     "concept exact_scalar = ring_scalar<T>;\n"),
    ("multivector_keeps_reference", MV,
     "concept multivector = detail::is_multivector_v<std::remove_cvref_t<X>>;\n",
     "concept multivector = detail::is_multivector_v<X>;\n"),
    ("mv_alg_unconstrained", MV, MV_HEAD,
     "template <class Alg, blade_set Blades, std::semiregular T>\n", 3),
    ("no_default_initializer", MV, INITIALIZER,
     "  std::array<T, sizeof...(Ts)> coefficients;\n"),
    ("shared_zero_initializer", MV, INITIALIZER,
     "  std::array<T, sizeof...(Ts)> coefficients{};\n"),
    ("one_value_implicit", MV,
     "  constexpr explicit(sizeof...(Ts) == 1) coefficient_storage(Ts... values)",
     "  constexpr explicit(false) coefficient_storage(Ts... values)"),
    ("equality_skips_last", MV, EQUALITY, EQUALITY_BUT_LAST),
    ("equality_requires_removed", MV,
     "    requires std::equality_comparable<T>\n", ""),
    ("count_accepts_one_short", MV,
     "    static_assert(sizeof...(Values) == component_count,\n",
     "    static_assert(sizeof...(Values) == component_count ||\n"
     "                      sizeof...(Values) + 1 == component_count,\n"),
    ("catchall_k1_not_explicit", MV,
     "  constexpr explicit(sizeof...(Values) == 1) mv(",
     "  constexpr explicit(false) mv("),
    ("catchall_counts_ignored_for_one", MV,
     "    requires(sizeof...(Values) > 0 && sizeof...(Values) != "
     "component_count &&\n",
     "    requires(sizeof...(Values) > 1 && sizeof...(Values) != "
     "component_count &&\n"),
    ("get_ignores_sign", MV, ORIENTATION,
     "      const NameLookup result = NameLookup::found;\n"),
    ("get_orientation_as_absent", MV,
     "    if (layout.blades[at].mask == parsed.blade.mask) {\n",
     "    if (layout.blades[at] == parsed.blade) {\n"),
    ("get_one_place_off", MV,
     "  return std::forward_like<X>(x.coefficients[at]);\n",
     "  return std::forward_like<X>(\n"
     "      x.coefficients[(at + 1) % std::remove_cvref_t<X>::"
     "component_count]);\n"),
    ("has_blade_errors_on_invalid_name", MV, INVALID_NAME,
     "  default:\n"
     "    assertion_failed(\"not a name\", __FILE__, __LINE__);\n"),
    ("absent_message_lost", MV,
     "  } else if constexpr (cause == absent) {\n",
     "  } else if constexpr (cause == index_above_n) {\n"),
    ("blades_fixed_four", BLADE_SET,
     "    std::remove_cvref_t<decltype(Alg::value)>::generator_count, "
     "Names>();\n",
     "    4, Names>();\n"),
    ("blades_unconstrained", BLADE_SET,
     "template <metric_like Alg, detail::name_list Names>\n",
     "template <class Alg, detail::name_list Names>\n"),
    ("sign_zero_accepted", BLADE_SET,
     "  return sign == 1 || sign == -1;\n",
     "  return sign == 1 || sign == -1 || sign == 0;\n"),
    ("rational_not_normalized", RATIONAL,
     "    const std::int64_t g = detail::gcd(n, d);\n",
     "    const std::int64_t g = 1;\n"),
    ("rational_negative_denominator_kept", RATIONAL,
     "    const std::int64_t sign = d < 0 ? -1 : 1;\n",
     "    const std::int64_t sign = 1;\n"),
    ("rational_no_sum_guard", RATIONAL,
     "  const bool fits = b >= 0 ? a <= max - b : a >= -max - b;\n",
     "  const bool fits = true;\n"),
    ("rational_no_product_guard", RATIONAL,
     "  const bool fits = a == 0 || magnitude(b) <= max / magnitude(a);\n",
     "  const bool fits = true;\n"),
    ("rational_product_no_cancel", RATIONAL,
     "  const std::int64_t g = detail::gcd(a.numerator, b.denominator);\n"
     "  const std::int64_t h = detail::gcd(b.numerator, a.denominator);\n",
     "  const std::int64_t g = 1;\n"
     "  const std::int64_t h = 1;\n"),
    ("rational_min_accepted", RATIONAL,
     "    if (n < -detail::max || d < -detail::max) {\n",
     "    if (d < -detail::max) {\n"),
    ("one_error_inert", TESTING,
     "    set(only ONLY \"error:\")\n", "    set(only \"\")\n"),
    ("base_drops_alg", MV,
     "  using type = coefficient_storage<Alg, Blades, T, same<I>...>;\n",
     "  using type = coefficient_storage<void, Blades, T, same<I>...>;\n"),
    ("base_drops_layout", MV,
     "  using type = coefficient_storage<Alg, Blades, T, same<I>...>;\n",
     "  using type = coefficient_storage<Alg, blade_set<0>{}, T, same<I>...>;\n"),
    ("mv_scalar_unconstrained", MV, MV_HEAD,
     "template <metric_like Alg, blade_set Blades, class T>\n", 3),
    ("lookup_wide_at_nine", MV,
     "  if (n > max_compact_generators) {\n",
     "  if (n >= max_compact_generators) {\n"),
    ("rational_sum_no_scale_cancel", RATIONAL,
     "  const std::int64_t g = detail::gcd(a.denominator, b.denominator);\n",
     "  const std::int64_t g = 1;\n"),
    ("literal_in_path_check_inert", TESTING,
     "    if(NOT at EQUAL -1)\n      message(FATAL_ERROR \"${name}: the required",
     "    if(FALSE)\n      message(FATAL_ERROR \"${name}: the required"),
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
    presets = args[0].split(",")
    runtime = "--runtime" in args[1:]
    only = {a for a in args[1:] if not a.startswith("--")}
    unknown = only - {m[0] for m in MUTATIONS}
    assert not unknown, unknown
    files = {MV, BLADE_SET, RATIONAL, TESTING, *TESTS}
    originals = {path: open(path, encoding="utf-8").read() for path in files}

    print("mutation\tpreset\tmode\tbuild_exit\ttest_exit\tverdict\t"
          "failed_tests\tstatic_asserts\ttest_lines\tfailed_cases\t"
          "first_build_error", flush=True)
    mode = "runtime" if runtime else "normal"
    try:
        if runtime:
            for path in TESTS:
                text = originals[path]
                assert text.count("static_assert(") > 0, path
                write(path, text.replace("static_assert(",
                                         "static_assert(true ? true : "))
        for name, path, anchor, replacement, *count in MUTATIONS:
            if only and name not in only:
                continue
            source = originals[path]
            expected = count[0] if count else 1
            assert source.count(anchor) == expected, (name, anchor)
            write(path, source.replace(anchor, replacement))
            for preset in presets:
                build = run(["tools/timed", "build", preset,
                             f"mutation:{name}:{mode}", "--", "cmake",
                             "--build", "--preset", preset])
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
            write(path, source)
    finally:
        for path, text in originals.items():
            write(path, text)
        for preset in presets:
            run(["tools/timed", "build", preset, "mutation:restored", "--",
                 "cmake", "--build", "--preset", preset])


if __name__ == "__main__":
    main()
