"""Mutation runs for the merge and index_of fixes of blade_set.hpp: apply
one mutation, build the blade set tests, run the core.blade_set selection,
and restore the header. With --merged, the mutations are the survivors of the
review, written against the header as merged before the fix. A mutant is
killed when the build fails or any test fails; the build status is recorded
before the tests, because CTest runs whatever program is on disk."""
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()
HEADER = f"{ROOT}/include/galoot/core/blade_set.hpp"

MERGED_HEADER_MUTATIONS = [
    ("merge_sign_first_only", "u.bad_sign = u.bad_sign || !is_sign(blade.sign);",
     "u.bad_sign = u.bad_sign || (!from_second && !is_sign(blade.sign));"),
    ("merge_no_flag_shift", "      u.from_second[j] = u.from_second[j - 1];\n", ""),
    ("merge_repeat_first_only", "      if (u.from_second[at] == from_second) {\n",
     "      if (!from_second && u.from_second[at] == from_second) {\n"),
    ("index_count_exactly_one", "static_assert(found.count <= 1,",
     "static_assert(found.count == 1,"),
]

MUTATIONS = [
    # The four mutants that survived the review of the merged header, and the
    # analogues of each for the fixed one.
    ("merge_sign_first_only", "u.bad_sign = u.bad_sign || !is_sign(blade.sign);",
     "u.bad_sign = u.bad_sign || (!second && !is_sign(blade.sign));"),
    ("merge_no_flag_shift", "      u.in_first[j] = u.in_first[j - 1];\n",
     ""),
    ("merge_no_second_flag_shift", "      u.in_second[j] = u.in_second[j - 1];\n",
     ""),
    ("merge_repeat_first_only", "if (seen) {", "if (seen && !second) {"),
    ("index_absent_says_twice", "if constexpr (at < S.size()) {", "if constexpr (true) {"),
    # The other checks of the fix.
    ("merge_opposite_any_sign", "if (is_sign(listed.sign) && is_sign(blade.sign) &&",
     "if ("),
    ("merge_repeat_check", "      u.repeated_mask = true;\n", "      u.repeated_mask = false;\n"),
    ("merge_mark_seen", "      seen = true;\n", ""),
    ("index_sign_check", "static_assert(signs_valid<S>,", "static_assert(true || signs_valid<S>,"),
    ("index_repeat_check", "static_assert(!repeats_after(S, at),", "static_assert(true || !repeats_after(S, at),"),
    ("index_repeat_from_next", "for (std::size_t j = at + 1; j < K; ++j) {",
     "for (std::size_t j = at + 2; j < K; ++j) {"),
    ("is_sign_zero", "return sign == 1 || sign == -1;", "return sign == 1 || sign == -1 || sign == 0;"),
    ("is_sign_minus", "return sign == 1 || sign == -1;", "return sign == 1;"),
]


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def main():
    args = sys.argv[1:]
    mutations = MUTATIONS
    if args[0] == "--merged":
        mutations = MERGED_HEADER_MUTATIONS
        args = args[1:]
    preset = args[0]
    only = set(args[1:])
    original = open(HEADER, encoding="utf-8").read()
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled\t"
          "first_build_error")
    try:
        for name, anchor, replacement in mutations:
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
            # Why the build failed, so that a kill by a broken build can be
            # told apart from a kill by the tests.
            errors = [line for line in build.stdout.splitlines()
                      if "error:" in line]
            first_error = errors[0].split(ROOT + "/")[-1] if errors else "-"
            print(f"{name}\t{preset}\t{build.returncode}\t{test.returncode}\t"
                  f"{','.join(failed) or '-'}\t{'killed' if killed else 'SURVIVED'}"
                  f"\t{first_error[:200]}", flush=True)
    finally:
        with open(HEADER, "w", encoding="utf-8") as f:
            f.write(original)


main()
