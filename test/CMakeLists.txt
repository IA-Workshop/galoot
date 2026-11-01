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
# The mode the configuration declares, passed to the test directly rather
# than through galoot::core, so that a mode lost on the way fails the test.
target_compile_definitions(core_smoke_test PRIVATE
  GALOOT_EXPECTED_CHECKING=$<BOOL:${GALOOT_CHECKING}>)

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
    "galoot/core/include_next.hpp:4: #include_next depends on the order of the user's include paths"
    "galoot/core/computed.hpp:5: computed #include GALOOT_CORE_COMPUTED_TARGET cannot be checked"
    "galoot/core/not_hpp.h: not a .hpp header"
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

# tools.measure_record: the fields of a cost row, and the reader's `--` for
# cells that rows written before a column existed lack.
add_test(NAME tools.measure_record
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/measure_record_test.py
    ${PROJECT_SOURCE_DIR}/tools/measure
    ${CMAKE_CURRENT_BINARY_DIR}/measure_record)
set_tests_properties(tools.measure_record PROPERTIES
  LABELS "tools;tools.measure")

# The golden values that tests compare the library with. They come from the
# reference scripts in test/reference/, never from the library, so this
# target does not link galoot::core. A test that needs them links it and
# includes <reference/golden.hpp>; the header also compiles on its own, in
# all_verify_interface_header_sets.
add_library(galoot_golden INTERFACE)
target_sources(galoot_golden INTERFACE
  FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES reference/golden.hpp)
target_compile_features(galoot_golden INTERFACE cxx_std_23)
set_property(TARGET galoot_golden PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# reference.golden: the committed golden.hpp is byte for byte what golden.py
# writes now, so a hand edit, or a header left stale after a change to the
# reference scripts, fails. reference.golden_rejects shows that the check
# fails on a copy changed in one place.
add_test(NAME reference.golden
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/reference/golden.py
    --check ${CMAKE_CURRENT_SOURCE_DIR}/reference/golden.hpp)
add_test(NAME reference.golden_rejects
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/golden_check_test.py
    ${CMAKE_CURRENT_SOURCE_DIR}/reference/golden.py
    ${CMAKE_CURRENT_SOURCE_DIR}/reference/golden.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/golden_check)
set_tests_properties(reference.golden reference.golden_rejects PROPERTIES
  LABELS "reference;reference.golden")

# tests.reference_markers: every test case states the evidence it rests on,
# in a '// Reference:' comment of one fixed form (tools/test_references.py).
# tests.golden_facts: every test case or static_assert that uses golden
# values states, in that comment, the fact that golden.hpp gives for them.
add_test(NAME tests.reference_markers
  COMMAND ${Python3_EXECUTABLE} -I
    ${PROJECT_SOURCE_DIR}/tools/test_references.py markers
    ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME tests.golden_facts
  COMMAND ${Python3_EXECUTABLE} -I
    ${PROJECT_SOURCE_DIR}/tools/test_references.py golden
    ${CMAKE_CURRENT_SOURCE_DIR}/reference/golden.hpp
    ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(tests.reference_markers tests.golden_facts
  PROPERTIES LABELS "tests;tests.references")

# tools.test_references_*: each command of tools/test_references.py reports
# every defect of its rejecting fixtures, one per test case or table row,
# and passes its accepting fixtures.
set(reference_fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_references)
set(test_references
  ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/test_references.py)
set(pointer "reference points to a document or a record instead of stating the reason")
add_test(NAME tools.test_references_markers_rejects
  COMMAND ${expect_output} 1
    "markers_bad.cpp:5: TEST_CASE has no reference marker"
    "markers_bad.cpp:9: TEST_CASE has no reference marker"
    "markers_bad.cpp:11: reference source is not '<kind>: <text>'"
    "markers_bad.cpp:14: a regression guard is the only source of its reference"
    "markers_bad.cpp:18: test case named as a regression test is not labeled"
    "markers_bad.cpp:20: ${pointer}: 'decision 9999'"
    "markers_bad.cpp:23: ${pointer}: 'section 3.1'"
    "markers_bad.cpp:26: ${pointer}: 'WU1'"
    "markers_bad.cpp:29: ${pointer}: 'VVUQ kind 3'"
    "markers_bad.cpp:32: ${pointer}: 'STYLE S9'"
    "markers_bad.cpp:35: ${pointer}: 'M0 gate'"
    "markers_bad.cpp:38: ${pointer}: 'lessons-learned'"
    "markers_bad.cpp:41: ${pointer}: 'constitution C13'"
    "markers_bad.cpp:44: ${pointer}: 'DoD-6'"
    -- ${test_references} markers ${reference_fixtures}/markers_bad.cpp)
add_test(NAME tools.test_references_markers_accepts
  COMMAND ${expect_output} 0
    "5 test cases, each with a reference marker"
    -- ${test_references} markers ${reference_fixtures}/markers_good.cpp)
add_test(NAME tools.test_references_golden_rejects
  COMMAND ${expect_output} 1
    "golden_bad_header.hpp:24: namespace alpha is defined twice"
    "golden_bad_header.hpp:29: namespace delta states no fact"
    "golden_bad.cpp:6: 'using' hides which test uses golden values"
    "golden_bad.cpp:11: uses golden values of alpha but its reference does not state 'the fact of alpha'"
    "golden_bad.cpp:16: uses golden values of beta but its reference does not state 'e1 ^ e2 = -e2 ^ e1'"
    "golden_bad.cpp:20: static_assert has no reference marker"
    "golden_bad.cpp:20: uses golden values of gamma but its reference does not state 'the fact of gamma'"
    "golden_bad.cpp:8: golden values of alpha are used outside a test case or a static_assert"
    -- ${test_references} golden ${reference_fixtures}/golden_bad_header.hpp
      ${reference_fixtures}/golden_bad.cpp)
add_test(NAME tools.test_references_golden_accepts
  COMMAND ${expect_output} 0
    "every use of golden values states its fact"
    -- ${test_references} golden ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
add_test(NAME tools.test_references_trace_rejects
  COMMAND ${expect_output} 1
    "trace_bad_table.md:7: item 1.2 is implemented and no test states its fact"
    "trace_bad_table.md:8: ${reference_fixtures}/trace_conventions.md has no item 9.9"
    "trace_bad_table.md:9: Implemented is 'maybe', not yes or no"
    "trace_bad_table.md:10: item 3.1 has an empty fact"
    "golden_fixture.hpp: the fact of namespace beta, 'e1 ^ e2 = -e2 ^ e1', is not in"
    -- ${test_references} trace ${reference_fixtures}/trace_bad_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
add_test(NAME tools.test_references_trace_no_rows
  COMMAND ${expect_output} 1
    "trace_empty_table.md: no rows of the form | item | fact | implemented |"
    -- ${test_references} trace ${reference_fixtures}/trace_empty_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
add_test(NAME tools.test_references_trace_accepts
  COMMAND ${expect_output} 0
    "1.1: ${reference_fixtures}/golden_good.cpp:12"
    "1.2: ${reference_fixtures}/golden_good.cpp:17"
    "2.1: ${reference_fixtures}/golden_good.cpp:23"
    "items with no test: 3.1"
    -- ${test_references} trace ${reference_fixtures}/trace_good_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
set_tests_properties(
  tools.test_references_markers_rejects tools.test_references_markers_accepts
  tools.test_references_golden_rejects tools.test_references_golden_accepts
  tools.test_references_trace_rejects tools.test_references_trace_no_rows
  tools.test_references_trace_accepts
  PROPERTIES LABELS "tools;tools.test_references")
