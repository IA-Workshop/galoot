"""Mutation runs for the compile-fail lane: apply one mutation to the build
files, reconfigure and build one preset, run its whole suite, and restore
the file. A mutant is killed when the build fails or any test fails. Each
mutation names the file it edits, the text it replaces (which must appear
exactly once), and the replacement.

Usage: python3 -I mutate.py <preset> [mutation...], from the worktree root
with the job pool exported."""
import subprocess
import sys

ROOT = subprocess.run(["git", "rev-parse", "--show-toplevel"],
                      capture_output=True, text=True, check=True).stdout.strip()

HELPER = "cmake/galoot_testing.cmake"
TESTS = "test/CMakeLists.txt"
CHECK = "test/compile_flags_check.cmake"

MUTATIONS = [
    # A compile-fail test whose source compiles cleanly.
    ("source_compiles", TESTS, "  SOURCE core/metric_zero_fail.cpp\n",
     "  SOURCE core/bare_user.cpp\n"),
    # A wrong literal.
    ("wrong_message", TESTS,
     '  MESSAGE "metric: an algebra has at least one generator, so n = 0 is rejected"',
     '  MESSAGE "metric: an algebra has at least one generator, so n = 0 is refused"'),
    # Flags dropped from the compile command.
    ("drop_definitions", HELPER,
     "    $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_COMPILE_DEFINITIONS>,PREPEND,-D>\n",
     ""),
    ("drop_includes", HELPER,
     "    $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>\n",
     ""),
    ("drop_flags", HELPER, "    ${flags} ${standard_flag}\n",
     "    ${standard_flag}\n"),
    ("drop_build_type_flags", HELPER,
     '    "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${config}}")',
     '    "${CMAKE_CXX_FLAGS}")'),
    ("drop_standard", HELPER, "    ${flags} ${standard_flag}\n",
     "    ${flags}\n"),
    ("extension_standard", HELPER, "  if(CMAKE_CXX_EXTENSIONS)\n",
     "  if(NOT CMAKE_CXX_EXTENSIONS)\n"),
    # The launcher and the compile mode.
    ("drop_launcher", HELPER, "      -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${compile}\n",
     "      -- ${compile}\n"),
    ("syntax_only", HELPER,
     "        -o ${arg_MODULE}_${arg_NAME}_compile_fail.o -c ${source}\n",
     "        -fsyntax-only ${source}\n"),
    # The check that compares the command with CMake's.
    ("check_never_differs", CHECK, "if(NOT command STREQUAL expected)\n",
     "if(FALSE)\n"),
]


def run(args):
    return subprocess.run(args, cwd=ROOT, capture_output=True, text=True)


def main():
    preset = sys.argv[1]
    only = set(sys.argv[2:])
    print("mutation\tpreset\tbuild_exit\ttest_exit\tfailed_tests\tkilled")
    for name, path, anchor, replacement in MUTATIONS:
        if only and name not in only:
            continue
        file = f"{ROOT}/{path}"
        original = open(file, encoding="utf-8").read()
        assert original.count(anchor) == 1, (name, original.count(anchor))
        try:
            with open(file, "w", encoding="utf-8") as f:
                f.write(original.replace(anchor, replacement))
            build = run(["tools/timed", "build", preset, f"mutation:{name}", "--",
                         "cmake", "--build", "--preset", preset])
            test = run(["tools/timed", "test", preset, f"mutation:{name}", "--",
                        "ctest", "--preset", preset])
        finally:
            with open(file, "w", encoding="utf-8") as f:
                f.write(original)
        # The names after CTest's summary line; the output before it quotes
        # compiler diagnostics, whose source lines can look like entries.
        summary = test.stdout.partition("The following tests FAILED:")[2]
        failed = [line.split(" - ")[1].split()[0]
                  for line in summary.splitlines() if " - " in line]
        killed = build.returncode != 0 or test.returncode != 0
        print(f"{name}\t{preset}\t{build.returncode}\t{test.returncode}\t"
              f"{','.join(failed) or '-'}\t{'killed' if killed else 'SURVIVED'}",
              flush=True)
    # Leave the build tree configured from the restored files.
    run(["cmake", "--build", "--preset", preset])


main()
