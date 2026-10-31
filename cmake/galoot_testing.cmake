# Test registration for Galoot (plan 0002, section 4.3; STYLE S14).
#
# Every test carries two labels, <module> and <module>.<component>, so that a
# selection is named by anchored label regexes: ctest -L '^core$' or
# ctest -L '^core\.assert$' (PERFORMANCE P8; research note 0002, F4).

# A doctest run that executes no test case prints "test cases: 0 |", and
# doctest still exits 0; this makes CTest fail it (research note 0001, F28).
set(GALOOT_ZERO_CASES_REGEX "test cases: +0 \\|")

# galoot_add_test(MODULE <module> COMPONENT <component>)
#
# Builds test/<module>/<component>_test.cpp into the program
# <module>_<component>_test, linked to the doctest runner, and registers it as
# the test <module>.<component>.
function(galoot_add_test)
  cmake_parse_arguments(PARSE_ARGV 0 arg "" "MODULE;COMPONENT" "")
  set(program ${arg_MODULE}_${arg_COMPONENT}_test)
  add_executable(${program} ${arg_MODULE}/${arg_COMPONENT}_test.cpp)
  target_link_libraries(${program} PRIVATE galoot::core galoot_doctest_main)
  add_test(NAME ${arg_MODULE}.${arg_COMPONENT} COMMAND ${program})
  set_tests_properties(${arg_MODULE}.${arg_COMPONENT} PROPERTIES
    LABELS "${arg_MODULE};${arg_MODULE}.${arg_COMPONENT}"
    FAIL_REGULAR_EXPRESSION "${GALOOT_ZERO_CASES_REGEX}")
endfunction()

# galoot_add_compile_fail_test(NAME <name> MODULE <module>
#   COMPONENT <component> SOURCE <file> MESSAGE <literal>)
#
# Registers the test <module>.<name>, which compiles <file> and passes only if
# the compiler's output contains <literal>. The literal is escaped here, so a
# test cannot pass on a loose pattern (plan 0002, departure 7). The exit code
# is ignored once a pass expression is set, and a different error does not
# contain the literal, so an unrelated failure fails the test (research note
# 0001, F11). The object library is outside `all`, and the test builds it in
# this build tree, so compile-fail tests take a lock: two Ninja processes in
# one tree could corrupt .ninja_log and .ninja_deps (inferred).
function(galoot_add_compile_fail_test)
  cmake_parse_arguments(PARSE_ARGV 0 arg ""
    "NAME;MODULE;COMPONENT;SOURCE;MESSAGE" "")
  set(target ${arg_MODULE}_${arg_NAME}_compile_fail)
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${arg_SOURCE})
  target_link_libraries(${target} PRIVATE galoot::core)
  # The source is meant not to compile, so there is nothing to lint.
  set_target_properties(${target} PROPERTIES SKIP_LINTING ON)

  string(REGEX REPLACE "([][.*+?^$|(){}\\\\])" "\\\\\\1" pattern
    "${arg_MESSAGE}")
  add_test(NAME ${arg_MODULE}.${arg_NAME}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target})
  set_tests_properties(${arg_MODULE}.${arg_NAME} PROPERTIES
    LABELS "${arg_MODULE};${arg_MODULE}.${arg_COMPONENT}"
    PASS_REGULAR_EXPRESSION "${pattern}"
    RESOURCE_LOCK galoot_build_tree)
endfunction()
