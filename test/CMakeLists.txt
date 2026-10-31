# Galoot's tests (STYLE S14; plan 0002, sections 4.3 to 4.6).
include(${PROJECT_SOURCE_DIR}/cmake/galoot_testing.cmake)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# The doctest runner, compiled once with doctest's built-in main (decision
# 0022; research note 0001, F1 and F2). DOCTEST_CONFIG_SUPER_FAST_ASSERTS
# reaches every test through the PUBLIC definition (note 0001, F5).
add_library(galoot_doctest_main OBJECT
  ${PROJECT_SOURCE_DIR}/include/ext/doctest/doctest.cpp)
target_compile_definitions(galoot_doctest_main
  PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS)
target_include_directories(galoot_doctest_main SYSTEM PUBLIC
  ${PROJECT_SOURCE_DIR}/include/ext)
target_compile_features(galoot_doctest_main PUBLIC cxx_std_23)
# include/ext is never linted (STYLE S3, S4).
set_target_properties(galoot_doctest_main PROPERTIES SKIP_LINTING ON)

# Runs a command and checks its result and output (test/expect_output.cmake).
set(expect_output
  ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/expect_output.cmake --)

# core.smoke: the runner, labels, launcher, and both core headers together.
galoot_add_test(MODULE core COMPONENT smoke)

# core.zero_cases below runs the smoke program with its own copy of the rule,
# so check here that galoot_add_test gave core.smoke the same rule (W5 review
# of WU1, finding N2).
get_test_property(core.smoke FAIL_REGULAR_EXPRESSION smoke_zero_cases)
if(NOT smoke_zero_cases STREQUAL GALOOT_ZERO_CASES_REGEX)
  message(FATAL_ERROR "core.smoke's FAIL_REGULAR_EXPRESSION is "
    "'${smoke_zero_cases}', not the zero-cases rule '${GALOOT_ZERO_CASES_REGEX}'")
endif()

# core.zero_cases: a filter that matches no test case fails (S14). WILL_FAIL
# inverts the zero-cases failure, so this passes only if the rule fires.
add_test(NAME core.zero_cases
  COMMAND core_smoke_test --test-case=no-such-case)
set_tests_properties(core.zero_cases PROPERTIES
  LABELS "core;core.runner"
  FAIL_REGULAR_EXPRESSION "${GALOOT_ZERO_CASES_REGEX}"
  WILL_FAIL ON)

# core.assert_checking and core.assert_unchecked: one source built in each
# checking mode. They take the core's headers directly instead of linking
# galoot::core, whose build interface carries the preset's mode.
foreach(mode IN ITEMS 1 0)
  add_executable(core_assert_mode_${mode} core/assert_fail.cpp)
  target_include_directories(core_assert_mode_${mode} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)
  target_compile_features(core_assert_mode_${mode} PRIVATE cxx_std_23)
  target_compile_definitions(core_assert_mode_${mode} PRIVATE
    GALOOT_CHECKING=${mode})
endforeach()
# A process that aborts cannot pass through WILL_FAIL (plan 0002, departure
# 4), so a script checks the abort and the literal message.
add_test(NAME core.assert_checking
  COMMAND ${expect_output} "Subprocess aborted"
    "galoot: assertion failed: assert_fail: argc is never zero ("
    -- $<TARGET_FILE:core_assert_mode_1>)
add_test(NAME core.assert_unchecked COMMAND core_assert_mode_0)
set_tests_properties(core.assert_checking core.assert_unchecked PROPERTIES
  LABELS "core;core.assert")

# core.assert_consteval: a false GALOOT_ASSERT ends a constant evaluation,
# and the diagnostic shows its message (decision 0012).
galoot_add_compile_fail_test(NAME assert_consteval MODULE core COMPONENT assert
  SOURCE core/assert_consteval_fail.cpp
  MESSAGE "assert_consteval_fail: odd input to checked_half"
  REQUIRE assertion_failed)
# Its twin expects a message the source does not contain, so the helper can
# fail (VVUQ V2); WILL_FAIL turns that failure into a pass.
galoot_add_compile_fail_test(NAME assert_consteval_wrong_message
  MODULE core COMPONENT assert
  SOURCE core/assert_consteval_fail.cpp
  MESSAGE "assert_consteval_fail: a message the source does not contain"
  REQUIRE assertion_failed)
set_tests_properties(core.assert_consteval_wrong_message PROPERTIES
  WILL_FAIL ON)

# core.bare_compile: the core works with no build system and no options
# beyond the language edition (C13; plan 0001, DoD-6).
add_test(NAME core.bare_compile
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -pedantic-errors
    -I ${PROJECT_SOURCE_DIR}/include -fsyntax-only
    ${CMAKE_CURRENT_SOURCE_DIR}/core/bare_user.cpp)

# core.include_scan: the core includes only standard and core headers, with
# no cycle, guards named from paths, and snake_case names (C13, S6).
add_test(NAME core.include_scan
  COMMAND ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/include_scan.py
    ${PROJECT_SOURCE_DIR}/include)
set_tests_properties(core.bare_compile core.include_scan PROPERTIES
  LABELS "core;core.headers")

# tools.include_scan_rejects: the scan reports each fixture's defect.
set(fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/include_scan)
add_test(NAME tools.include_scan_rejects
  COMMAND ${expect_output} 1
    "galoot/core/nonstandard.hpp:4: <vector_math.h> is not a C++23 standard header or a core header"
    "galoot/core/cycle_a.hpp: include cycle: galoot/core/cycle_a.hpp -> galoot/core/cycle_b.hpp -> galoot/core/cycle_a.hpp"
    "galoot/core/wrong_guard.hpp:1: include guard should be GALOOT_CORE_WRONG_GUARD_HPP"
    "galoot/core/Bad-Name.hpp: header name is not snake_case.hpp"
    -- ${Python3_EXECUTABLE} -I
      ${PROJECT_SOURCE_DIR}/tools/include_scan.py ${fixtures})
set_tests_properties(tools.include_scan_rejects PROPERTIES
  LABELS "tools;tools.include_scan")

# tools.measure_header: launchers that open an empty record at once write one
# header between them (W5 review of WU1, finding B1).
add_test(NAME tools.measure_header
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/measure_header_test.py
    ${PROJECT_SOURCE_DIR}/tools/measure
    ${CMAKE_CURRENT_BINARY_DIR}/measure_header)
set_tests_properties(tools.measure_header PROPERTIES
  LABELS "tools;tools.measure")
