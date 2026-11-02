"""Mutation runs for WU8b (plan 0016): the code in mv.hpp that applies plans
and maps, the coefficient check, the conversions, truncate, and the scalar
and argument checks that the WU8a reviews left for WU8b. Apply one mutation,
build `all` on each preset given, run the whole suite there, and restore the
file. Each mutation is a list of (anchor, replacement) edits, and each
anchor must occur exactly once.

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

With --runtime, every static_assert of mv_test.cpp becomes
`static_assert(true ? true : ...)` for the run, so that its program builds on
a mutant those checks would reject, and the record lists the test cases that
fail at run time and the compile-fail tests that fail.

Usage, from the worktree root with the job pool exported:
  python3 -I docs/agent/records/mutations/0016-mv-b/mutate.py \\
      <preset>[,<preset>...] [--runtime] [mutation...]
"""
import re
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
MV = f"{ROOT}/include/galoot/core/mv.hpp"
TESTS = [f"{ROOT}/test/core/mv_test.cpp"]


# A check made always true by turning its static_assert's condition into
# `true ||`, so that the message and layout of the source are kept.
def disabled(anchor):
    return anchor.replace("static_assert(", "static_assert(true || ", 1)


PRODUCT_FOLD = (
    "  (accumulate<Kernel[K].coefficient, Kernel[K].first>(\n"
    "       out[Kernel[K].out], static_cast<T>(x[Kernel[K].a] * "
    "y[Kernel[K].b])),\n"
    "   ...);\n")
# (c x) y in place of c (x y): a coefficient other than +1 and -1 scales the
# first input before the product, and the term then adds with +1.
SCALED_FIRST = (
    "  (accumulate<(Kernel[K].coefficient == 1 || Kernel[K].coefficient == -1"
    " ? Kernel[K].coefficient : 1), Kernel[K].first>(\n"
    "       out[Kernel[K].out], static_cast<T>((Kernel[K].coefficient == 1 || "
    "Kernel[K].coefficient == -1 ? x[Kernel[K].a] : static_cast<T>("
    "static_cast<T>(Kernel[K].coefficient) * x[Kernel[K].a])) * "
    "y[Kernel[K].b])),\n"
    "   ...);\n")
EXACT = "  static_assert(scalar_holds_every_coefficient_of_the_product,\n"
PRODUCT_TARGET = (
    "  static_assert(target_holds_result,\n"
    "                \"apply: the target layout does not hold every blade of "
    "the \"\n                \"product\");\n")
MAP_TARGET = (
    "  static_assert(target_holds_result,\n"
    "                \"apply: the target layout does not hold every blade of "
    "the \"\n                \"map\");\n")
HEAD = "template <metric_like Alg, blade_set Blades, std::destructible T>\n"

# (name, [(anchor, replacement), ...]): every anchor must occur once,
# except HEAD, which is replaced at each of its places as one edit.
MUTATIONS = [
    ("first_flag_inverted", [("    t.first = !seen[t.out];\n",
                              "    t.first = seen[t.out];\n")]),
    ("first_term_added_to_zero", [("    t.first = !seen[t.out];\n",
                                   "    t.first = false;\n")]),
    ("plus_minus_swapped", [
        ("  if constexpr (C == 1) {\n", "  if constexpr (C == -2) {\n"),
        ("  } else if constexpr (C == -1) {\n",
         "  } else if constexpr (C == 1) {\n"),
        ("  if constexpr (C == -2) {\n", "  if constexpr (C == -1) {\n")]),
    ("coefficient_times_first_input", [(PRODUCT_FOLD, SCALED_FIRST)]),
    ("float_bound_one_below", [
        ("    return odd_bits <= limits::digits &&\n",
         "    return odd_bits < limits::digits &&\n")]),
    ("float_bound_one_above", [
        ("    return odd_bits <= limits::digits &&\n",
         "    return odd_bits <= limits::digits + 1 &&\n")]),
    ("float_exponent_ignored", [
        ("    return odd_bits <= limits::digits &&\n"
         "           std::bit_width(magnitude) <= limits::max_exponent;\n",
         "    return odd_bits <= limits::digits;\n")]),
    ("float_bound_on_magnitude", [
        ("    return odd_bits <= limits::digits &&\n",
         "    return odd_bits >= 0 &&\n"
         "           std::bit_width(magnitude - 1) <= limits::digits &&\n")]),
    ("integer_min_excluded", [
        ("             (c >= static_cast<coefficient_type>(limits::min()) &&\n",
         "             (c > static_cast<coefficient_type>(limits::min()) &&\n")]),
    ("integer_min_one_below", [
        ("             (c >= static_cast<coefficient_type>(limits::min()) &&\n",
         "             (c + 1 >= static_cast<coefficient_type>(limits::min()) &&\n")]),
    ("integer_max_excluded", [
        ("              c <= static_cast<coefficient_type>(limits::max()));\n",
         "              c < static_cast<coefficient_type>(limits::max()));\n")]),
    ("integer_bound_one_above", [
        ("              c <= static_cast<coefficient_type>(limits::max()));\n",
         "              c - 1 <= static_cast<coefficient_type>(limits::max()));\n")]),
    ("integer_bound_on_magnitude", [
        ("             (c >= static_cast<coefficient_type>(limits::min()) &&\n",
         "             (c >= -static_cast<coefficient_type>(limits::max()) &&\n")]),
    ("no_coefficient_check", [(EXACT, disabled(EXACT))]),
    ("no_product_target_check", [(PRODUCT_TARGET, disabled(PRODUCT_TARGET))]),
    ("no_map_target_check", [(MAP_TARGET, disabled(MAP_TARGET))]),
    ("conversion_sign_of_target_only", [
        ("          .coefficient = static_cast<coefficient_type>(From.blades[a]"
         ".sign) *\n                         To.blades[at].sign,\n",
         "          .coefficient = To.blades[at].sign,\n")]),
    ("product_onto_target_unsigned", [
        ("        .b = t.b,\n        .out = at,\n"
         "        .coefficient = Target.blades[at].sign * t.coefficient,\n",
         "        .b = t.b,\n        .out = at,\n"
         "        .coefficient = t.coefficient,\n")]),
    ("map_onto_target_unsigned", [
        ("        .b = 0,\n        .out = at,\n"
         "        .coefficient = Target.blades[at].sign * t.coefficient,\n",
         "        .b = 0,\n        .out = at,\n"
         "        .coefficient = t.coefficient,\n")]),
    ("covering_reversed", [
        ("    requires(detail::holds_every_mask(Blades, From))\n",
         "    requires(detail::holds_every_mask(From, Blades))\n"),
        ("    requires(!detail::holds_every_mask(Blades, From))\n",
         "    requires(!detail::holds_every_mask(From, Blades))\n")]),
    ("no_lossy_catch_all_message", [
        ("    static_assert(false, \"conversion drops blades of its input: it is "
         "\"\n",
         "    static_assert(true, \"conversion drops blades of its input: it is "
         "\"\n")]),
    ("truncate_checks_rounding", [
        ("  if constexpr (exact_scalar<T> && std::equality_comparable<T>) {\n",
         "  if constexpr (std::equality_comparable<T>) {\n")]),
    ("truncate_skips_rational", [
        ("  if constexpr (exact_scalar<T> && std::equality_comparable<T>) {\n",
         "  if constexpr (exact_scalar<T> && std::equality_comparable<T> &&\n"
         "                detail::integer_limits<T>) {\n")]),
    ("truncate_check_removed", [
        ("    GALOOT_ASSERT(c[at] == T{}, \"truncate: a dropped coefficient is "
         "not zero\");\n", "    (void)c[at];\n")]),
    ("truncate_other_algebra", [
        ("    std::same_as<typename std::remove_cvref_t<Target>::algebra_type, "
         "Alg> &&\n", "")]),
    ("truncate_other_scalar", [
        ("    std::same_as<typename std::remove_cvref_t<Target>::algebra_type, "
         "Alg> &&\n    std::same_as<typename std::remove_cvref_t<Target>::"
         "scalar_type, T>;\n",
         "    std::same_as<typename std::remove_cvref_t<Target>::algebra_type, "
         "Alg>;\n")]),
    ("bool_ring_scalar", [
        ("concept ring_scalar = !std::same_as<std::remove_cv_t<T>, bool> &&\n",
         "concept ring_scalar = true &&\n")]),
    ("no_qualified_scalar_check", [
        ("  static_assert(scalar_is_not_a_reference_or_qualified,\n",
         "  static_assert(true || scalar_is_not_a_reference_or_qualified,\n")]),
    ("no_bool_scalar_check", [
        ("  static_assert(scalar_is_not_bool,\n",
         "  static_assert(true || scalar_is_not_bool,\n")]),
    ("no_semiregular_scalar_check", [
        ("  static_assert(scalar_is_default_constructible,\n",
         "  static_assert(true || scalar_is_default_constructible,\n")]),
    ("no_copyable_scalar_check", [
        ("  static_assert(scalar_is_copyable,\n",
         "  static_assert(true || scalar_is_copyable,\n")]),
    ("semiregular_head", [(HEAD, "template <metric_like Alg, blade_set Blades, "
                                 "std::semiregular T>\n")]),
    ("no_placeholder_storage", [
        ("    std::conditional_t<std::semiregular<std::remove_cvref_t<T>>,\n",
         "    std::conditional_t<true,\n")]),
    ("array_reaches_count", [
        ("    requires detail::not_a_coefficient<Value, T>\n",
         "    requires(detail::not_a_coefficient<Value, T> && false)\n"),
        ("             !(sizeof...(Values) == 1 &&\n"
         "               (detail::not_a_coefficient<Values, T> && ...)))\n",
         "             true)\n")]),
    ("no_braced_list_catch_all", [
        ("  constexpr mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {\n",
         "  constexpr mv(const std::remove_cvref_t<T> (& /*values*/)[N])\n"
         "    requires false\n  {\n")]),
    ("braced_list_catch_all_explicit", [
        ("  constexpr mv(const std::remove_cvref_t<T> (& /*values*/)[N]) {\n",
         "  constexpr explicit mv(const std::remove_cvref_t<T> "
         "(& /*values*/)[N]) {\n")]),
    ("coefficient_by_conversion", [
        ("    !multivector_or_derived<Value> && !std::constructible_from<T, Value>;\n",
         "    !multivector_or_derived<Value> && !std::convertible_to<Value, T>;\n")]),
    ("several_values_always", [
        ("    std::is_array_v<std::remove_cvref_t<Value>> ||\n",
         "    true || std::is_array_v<std::remove_cvref_t<Value>> ||\n")]),
    ("several_values_no_containers", [
        ("    requires(const std::remove_cvref_t<Value>& values) {\n"
         "      values.begin();\n",
         "    requires(const std::remove_cvref_t<Value>& values) {\n"
         "      values.not_a_member();\n")]),
    ("truncate_checks_first_only", [
        ("    GALOOT_ASSERT(c[at] == T{}, \"truncate: a dropped coefficient is "
         "not zero\");\n",
         "    GALOOT_ASSERT(c[at] == T{}, \"truncate: a dropped coefficient is "
         "not zero\");\n    break;\n")]),
    ("truncate_catch_all_returns_source", [
        ("    return std::remove_cvref_t<Target>{};\n", "    return x;\n")]),
    ("opposite_message_not_ordered", [
        ("          in_canonical_order(layout)\n"
         "              ? NameLookup::other_orientation_in_ordered_layout\n",
         "          false\n"
         "              ? NameLookup::other_orientation_in_ordered_layout\n")]),
    ("derived_reaches_count", [
        ("               (detail::multivector_or_derived<Values> && ...)) &&\n",
         "               (multivector<Values> && ...)) &&\n")]),
    ("absent_message_not_ordered", [
        ("  const NameLookup result = in_canonical_order(layout)\n",
         "  const NameLookup result = false\n")]),
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
    files = {MV, *TESTS}
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
        for name, edits in MUTATIONS:
            if only and name not in only:
                continue
            path = MV
            source = originals[path]
            mutated = source
            for anchor, replacement in edits:
                expected = mutated.count(HEAD) if anchor == HEAD else 1
                assert expected >= 1 and mutated.count(anchor) == expected, (
                    name, anchor)
                mutated = mutated.replace(anchor, replacement)
            write(path, mutated)
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
