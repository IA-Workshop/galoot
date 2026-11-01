# Test registration for Galoot: one doctest program per component, built from
# test/<module>/<component>_test.cpp.
#
# Every test carries two labels, <module> and <module>.<component>, so that a
# run can select the tests a change affects by anchored label regexes:
# ctest -L '^core$' or ctest -L '^core\.assert$'. -L matches a regex
# against each label, so an unanchored 'core' would also select 'core.assert'.

# A doctest run that executes no test case prints "test cases: 0 |", and
# doctest still exits 0; this makes CTest fail it, so that a filter or a
# build that drops every test case cannot pass as a success.
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
# the build fails, its output contains <literal> and every REQUIRE literal,
# and it contains no FORBID literal, each matched as written
# (test/expect_output.cmake). FORBID is for a misuse whose diagnostic must
# leave out a message that would be false. The values are literals rather
# than regular expressions, so that no test passes on a loose pattern such as
# .*, and because diagnostics quote C++ source full of regex metacharacters.
#
# A GALOOT_ASSERT message reaches the diagnostic only through the source line
# the compiler echoes, so an unrelated error on that line would show it too;
# such tests also require `assertion_failed`, which only the failed assertion
# names. MESSAGE may not contain `;`, which CMake would split into two
# literals. A `;` in a REQUIRE or FORBID value cannot be told from two values,
# so it splits too: FORBID "a;b" forbids both "a" and "b", which is stricter
# than forbidding "a;b", but REQUIRE "a;b" requires "a" and "b" anywhere in
# the output, which is looser than requiring "a;b". Write neither with a `;`.
#
# The object library is outside `all`, and the test builds it in this build
# tree, so compile-fail tests take a lock: two Ninja processes in one tree
# could corrupt its build and dependency logs (.ninja_log, .ninja_deps),
# because Ninja is not known to lock them (inferred, not checked against
# Ninja's source).
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
