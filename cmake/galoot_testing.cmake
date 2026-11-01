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
#   COMPONENT <component> SOURCE <file> MESSAGE <literal>
#   [REQUIRE <literal>...] [FORBID <literal>...])
#
# Registers the test <module>.<name>, which builds <file> and passes only if
# the build fails and its output contains <literal> and every REQUIRE literal,
# each as written (test/expect_output.cmake; plan 0002, departure 7). A
# GALOOT_ASSERT message reaches the diagnostic only through the source line
# the compiler echoes, so an unrelated error on that line would show it too;
# such tests also require `assertion_failed`, which only the failed assertion
# names (W5 review of WU1, finding N1). MESSAGE may not contain `;`, which
# CMake would split into two literals (finding N3); a `;` in a REQUIRE value
# cannot be told from two values, so it splits too. The object library is
# outside `all`, and the test builds it in this build tree, so compile-fail
# tests take a lock: two Ninja processes in one tree could corrupt .ninja_log
# and .ninja_deps (inferred). Each FORBID literal must not appear, for a
# misuse whose diagnostic must leave out a message that would be false.
function(galoot_add_compile_fail_test)
  cmake_parse_arguments(PARSE_ARGV 0 arg ""
    "NAME;MODULE;COMPONENT;SOURCE;MESSAGE" "REQUIRE;FORBID")
  if(arg_MESSAGE MATCHES ";")
    message(FATAL_ERROR "galoot_add_compile_fail_test(${arg_NAME}): MESSAGE "
      "may not contain ';': ${arg_MESSAGE}")
  endif()

  set(target ${arg_MODULE}_${arg_NAME}_compile_fail)
  add_library(${target} OBJECT EXCLUDE_FROM_ALL ${arg_SOURCE})
  target_link_libraries(${target} PRIVATE galoot::core)
  # The source is meant not to compile, so there is nothing to lint.
  set_target_properties(${target} PROPERTIES SKIP_LINTING ON)

  set(forbid "")
  if(arg_FORBID)
    set(forbid FORBID ${arg_FORBID})
  endif()
  add_test(NAME ${arg_MODULE}.${arg_NAME}
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/test/expect_output.cmake
      -- 1 ${arg_MESSAGE} ${arg_REQUIRE} ${forbid}
      -- ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${target})
  set_tests_properties(${arg_MODULE}.${arg_NAME} PROPERTIES
    LABELS "${arg_MODULE};${arg_MODULE}.${arg_COMPONENT}"
    RESOURCE_LOCK galoot_build_tree)
endfunction()
