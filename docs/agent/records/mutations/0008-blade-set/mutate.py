"""Mutation runs for blade_set.hpp: apply one mutation, build the blade set
test, run the core.blade_set selection, and restore the header. A mutant is
killed when the build fails or any test fails; the build status is recorded
before the tests, because CTest runs whatever program is on disk."""
import subprocess
import sys

ROOT = "/home/brianm/projects/utils/math-core/galoot/.claude/worktrees/core-blade-set"
HEADER = f"{ROOT}/include/galoot/core/blade_set.hpp"

MUTATIONS = [
    ("parse_sign", "blade.sign *= reorder_sign(blade.mask, generator);",
     "blade.sign *= reorder_sign(generator, blade.mask);"),
    ("parse_sign_trivectors", "blade.sign *= reorder_sign(blade.mask, generator);",
     "blade.sign *= grade(blade.mask) < 2 ? reorder_sign(blade.mask, generator) : 1;"),
    ("canonical_tie", "return ((a >> first_difference) & 1U) == 1U;",
     "return ((a >> first_difference) & 1U) == 0U;"),
    ("full_grade_order", "      append_grade(set.blades, at, N, k);\n    }\n    return set;",
     "      append_grade(set.blades, at, N, N - k);\n    }\n    return set;"),
    ("binomial", "count = count * (n - i) / (i + 1);", "count = count * (n - i) / (i + 2);"),
    ("merge_duplicates", "if (at < u.size && u.blades[at].mask == blade.mask) {",
     "if (false && at < u.size && u.blades[at].mask == blade.mask) {"),
    ("merge_numeric_order", "while (at < u.size && canonical_less(u.blades[at].mask, blade.mask)) {",
     "while (at < u.size && u.blades[at].mask < blade.mask) {"),
    ("merge_conflict", "u.opposite_signs || u.blades[at].sign != blade.sign;",
     "u.opposite_signs || false;"),
    ("index_check", "static_assert(at < S.size(),", "static_assert(true || at < S.size(),"),
    ("index_position", "while (at < K && set.blades[at].mask != mask) {",
     "while (at < K && set.blades[at].mask < mask) {"),
    ("set_size", "constexpr std::size_t size() const noexcept { return K; }",
     "constexpr std::size_t size() const noexcept { return K + 1; }"),
    ("no_names", "return count == 0 ? NameError::no_names : NameError::none;",
     "return count == 0 ? NameError::none : NameError::none;"),
    ("not_a_name", "if (name.front() != 'e') {", "if (false && name.front() != 'e') {"),
    ("digit_zero", "if (digit < '1' || digit > '9') {", "if (digit < '0' || digit > '9') {"),
    ("digit_letter", "if (digit < '1' || digit > '9') {", "if (digit < '1') {"),
    ("no_index", "if (name.size() == 1) {", "if (false && name.size() == 1) {"),
    ("index_above_n", "if (index > n) {", "if (index > n + 1) {"),
    ("repeated_generator", "if ((blade.mask & generator) != 0) {",
     "if (false && (blade.mask & generator) != 0) {"),
    ("repeated_blade", "if (named[parsed.blade.mask]) {", "if (false && named[parsed.blade.mask]) {"),
    ("names_above_9_assert", "static_assert(N <= max_compact_generators,\n",
     "static_assert(true || N <= max_compact_generators,\n"),
    ("names_above_9_guard",
     "if constexpr (valid_generator_count<N>() && N <= max_compact_generators) {",
     "if constexpr (valid_generator_count<N>()) {"),
    ("zero_assert", "static_assert(N >= 1, \"blade_set:", "static_assert(N >= 0, \"blade_set:"),
    ("zero_guard", "return N >= 1 && N <= mask_bits;", "return N <= mask_bits;"),
    ("wide_assert", "static_assert(N <= mask_bits, \"blade_set:",
     "static_assert(N <= mask_bits + 1, \"blade_set:"),
    ("grade_above_n", "static_assert(K >= 0 && std::cmp_less_equal(K, N),",
     "static_assert(K >= 0,"),
    ("grade_negative", "static_assert(K >= 0 && std::cmp_less_equal(K, N),",
     "static_assert(std::cmp_less_equal(K, N),"),
    ("name_assert", "GALOOT_ASSERT(compact,", "GALOOT_ASSERT(true || compact,"),
    ("name_digit", "static_cast<char>('1' + bit)", "static_cast<char>('0' + bit)"),
    ("separator", "while (at < text.size() && text[at] == ' ') {",
     "while (false && at < text.size() && text[at] == ' ') {"),
]


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def main():
    preset = sys.argv[1]
    only = set(sys.argv[2:])
    original = open(HEADER, encoding="utf-8").read()
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled")
    try:
        for name, anchor, replacement in MUTATIONS:
            if only and name not in only:
                continue
            assert original.count(anchor) == 1, (name, original.count(anchor))
            with open(HEADER, "w", encoding="utf-8") as f:
                f.write(original.replace(anchor, replacement))
            build = run(["tools/timed", "build", preset, f"mutation:{name}", "--",
                         "cmake", "--build", "--preset", preset, "--target",
                         "core_blade_set_test", "core_blade_set_name_assert"])
            test = run(["tools/timed", "test", preset, f"mutation:{name}", "--",
                        "ctest", "--preset", preset, "-L", r"^core\.blade_set$"])
            failed = [line.split(" - ")[1].split()[0]
                      for line in test.stdout.splitlines() if " - core." in line]
            killed = build.returncode != 0 or test.returncode != 0
            print(f"{name}\t{preset}\t{build.returncode}\t{test.returncode}\t"
                  f"{','.join(failed) or '-'}\t{'killed' if killed else 'SURVIVED'}",
                  flush=True)
    finally:
        with open(HEADER, "w", encoding="utf-8") as f:
            f.write(original)


main()
