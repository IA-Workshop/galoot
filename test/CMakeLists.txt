# Galoot's tests: doctest programs, compile-fail tests, and checks of the
# headers, the reference values, and the tools.
include(${PROJECT_SOURCE_DIR}/cmake/galoot_testing.cmake)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# The doctest runner, compiled once with doctest's built-in main, so that
# each test program compiles only its own test cases and not doctest's
# implementation. DOCTEST_CONFIG_SUPER_FAST_ASSERTS, which makes the binary
# assertions cheaper to compile, reaches every test through the PUBLIC
# definition.
add_library(galoot_doctest_main OBJECT
  ${PROJECT_SOURCE_DIR}/include/ext/doctest/doctest.cpp)
target_compile_definitions(galoot_doctest_main
  PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  PUBLIC DOCTEST_CONFIG_SUPER_FAST_ASSERTS)
target_include_directories(galoot_doctest_main SYSTEM PUBLIC
  ${PROJECT_SOURCE_DIR}/include/ext)
target_compile_features(galoot_doctest_main PUBLIC cxx_std_23)
# include/ext is vendored code with its upstream style, so it is not linted.
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
# so check here that galoot_add_test gave core.smoke the same rule; otherwise
# a change to one copy would leave core.zero_cases testing a rule no test
# program uses.
get_test_property(core.smoke FAIL_REGULAR_EXPRESSION smoke_zero_cases)
if(NOT smoke_zero_cases STREQUAL GALOOT_ZERO_CASES_REGEX)
  message(FATAL_ERROR "core.smoke's FAIL_REGULAR_EXPRESSION is "
    "'${smoke_zero_cases}', not the zero-cases rule '${GALOOT_ZERO_CASES_REGEX}'")
endif()

# core.zero_cases: a filter that matches no test case fails. WILL_FAIL
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
# CTest reports a process killed by SIGABRT as "Subprocess aborted", and
# neither WILL_FAIL nor PASS_REGULAR_EXPRESSION turns that into a pass, so a
# script checks the abort and the literal message.
add_test(NAME core.assert_checking
  COMMAND ${expect_output} "Subprocess aborted"
    "galoot: assertion failed: assert_fail: argc is never zero ("
    -- $<TARGET_FILE:core_assert_mode_1>)
add_test(NAME core.assert_unchecked COMMAND core_assert_mode_0)
set_tests_properties(core.assert_checking core.assert_unchecked PROPERTIES
  LABELS "core;core.assert")

# core.assert_consteval: a false GALOOT_ASSERT ends a constant evaluation,
# and the diagnostic shows its message: assertion_failed is not constexpr,
# so a call to it cannot be part of a constant expression.
galoot_add_compile_fail_test(NAME assert_consteval MODULE core COMPONENT assert
  SOURCE core/assert_consteval_fail.cpp
  MESSAGE "assert_consteval_fail: odd input to checked_half"
  REQUIRE assertion_failed)
# Its twin expects a message the source does not contain, so the helper can
# fail; WILL_FAIL turns that failure into a pass.
galoot_add_compile_fail_test(NAME assert_consteval_wrong_message
  MODULE core COMPONENT assert
  SOURCE core/assert_consteval_fail.cpp
  MESSAGE "assert_consteval_fail: a message the source does not contain"
  REQUIRE assertion_failed)
set_tests_properties(core.assert_consteval_wrong_message PROPERTIES
  WILL_FAIL ON)

# core.bits: the blade-mask functions against their definitions and against
# the golden values (galoot_golden, below).
galoot_add_test(MODULE core COMPONENT bits)
target_link_libraries(core_bits_test PRIVATE galoot_golden)

# core.bits_rejects_<function>_<case>: out-of-range input aborts with the
# precondition's message at run time. The program fixes the checking mode, as
# the assert programs above do, so these run in every preset. Each function
# that takes n is tried with n below and above the range and with e3 in an
# algebra of two generators; core.bits_accepts_<function> gives it the widest
# input in range, the antiscalar of mask_bits generators, and must exit 0.
add_executable(core_bits_assert_fail core/bits_assert_fail.cpp)
target_include_directories(core_bits_assert_fail PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_bits_assert_fail PRIVATE cxx_std_23)
target_compile_definitions(core_bits_assert_fail PRIVATE GALOOT_CHECKING=1)
set(bits_count_message "galoot: assertion failed: n must be from 1 to 32 (")
set(bits_mask_message
  "galoot: assertion failed: blade has a generator beyond e_n (")
foreach(function IN ITEMS antigrade complement_mask right_complement_sign
    left_complement_sign antireverse_sign)
  set(program $<TARGET_FILE:core_bits_assert_fail> ${function})
  add_test(NAME core.bits_rejects_${function}_n_0
    COMMAND ${expect_output} "Subprocess aborted" ${bits_count_message}
      bits.hpp: -- ${program} n_0)
  add_test(NAME core.bits_rejects_${function}_n_above
    COMMAND ${expect_output} "Subprocess aborted" ${bits_count_message}
      bits.hpp: -- ${program} n_above)
  add_test(NAME core.bits_rejects_${function}_mask
    COMMAND ${expect_output} "Subprocess aborted" ${bits_mask_message}
      bits.hpp: -- ${program} mask)
  add_test(NAME core.bits_accepts_${function}
    COMMAND ${program} widest)
  set_tests_properties(core.bits_rejects_${function}_n_0
    core.bits_rejects_${function}_n_above core.bits_rejects_${function}_mask
    core.bits_accepts_${function} PROPERTIES LABELS "core;core.bits")
endforeach()

# core.bits_consteval: a blade beyond the algebra ends a constant evaluation,
# and the diagnostic shows the precondition.
galoot_add_compile_fail_test(NAME bits_consteval MODULE core COMPONENT bits
  SOURCE core/bits_consteval_fail.cpp
  MESSAGE "blade has a generator beyond e_n"
  REQUIRE assertion_failed)

# tests.compile_fail_flags: compile-fail tests compile with the compiler and
# arguments that CMake generates for a target linking galoot::core and nothing
# else, here the unit that checks galoot/core.hpp on its own, so that a flag
# lost from galoot_compile_command fails a test even when every rejection
# still appears without it. tests.compile_fail_flags_rejects shows that one
# argument more fails the check.
galoot_compile_command(compile_fail_command)
set(compile_flags_check
  ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/compile_flags_check.cmake --
  ${CMAKE_BINARY_DIR}/compile_commands.json
  ${CMAKE_BINARY_DIR}/galoot_core_verify_interface_header_sets/galoot/core.hpp.cxx
  --)
add_test(NAME tests.compile_fail_flags
  COMMAND ${compile_flags_check} ${compile_fail_command}
  COMMAND_EXPAND_LISTS)
add_test(NAME tests.compile_fail_flags_rejects
  COMMAND ${expect_output} 1 "compile_flags_check: the commands differ"
    -- ${compile_flags_check} ${compile_fail_command} -DGALOOT_UNEXPECTED
  COMMAND_EXPAND_LISTS)
set_tests_properties(tests.compile_fail_flags tests.compile_fail_flags_rejects
  PROPERTIES LABELS "tests;tests.compile_fail")

# core.bare_compile: the core works with no build system and no options
# beyond the language edition, so a user can add its headers to any project.
add_test(NAME core.bare_compile
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -pedantic-errors
    -I ${PROJECT_SOURCE_DIR}/include -fsyntax-only
    ${CMAKE_CURRENT_SOURCE_DIR}/core/bare_user.cpp)

# core.include_scan: the core includes only standard and core headers, with
# no cycle, guards named from paths, and snake_case names.
add_test(NAME core.include_scan
  COMMAND ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/include_scan.py
    ${PROJECT_SOURCE_DIR}/include)
set_tests_properties(core.bare_compile core.include_scan PROPERTIES
  LABELS "core;core.headers")

# tools.include_scan_rejects: the scan reports each fixture's defect.
set(fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/include_scan)
add_test(NAME tools.include_scan_rejects
  COMMAND ${expect_output} 1 ONLY "."
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
# header between them, not one each.
add_test(NAME tools.measure_header
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/measure_header_test.py
    ${PROJECT_SOURCE_DIR}/tools/measure
    ${CMAKE_CURRENT_BINARY_DIR}/measure_header)
set_tests_properties(tools.measure_header PROPERTIES
  LABELS "tools;tools.measure")

# core.metric: the factories, accessors, and concept of metric.hpp, compared
# with the golden values.
galoot_add_test(MODULE core COMPONENT metric)
target_link_libraries(core_metric_test PRIVATE galoot_golden)

# Each check of metric.hpp rejects its misuse at compile time with a literal
# message. A GALOOT_ASSERT message reaches the diagnostic only through the
# echoed source line, so those tests also require `assertion_failed`; a
# static_assert prints its message itself, after `static assertion failed`.
galoot_add_compile_fail_test(NAME metric_asymmetric MODULE core COMPONENT metric
  SOURCE core/metric_asymmetric_fail.cpp
  MESSAGE "from_gram: the form must be symmetric"
  REQUIRE "assertion_failed")
galoot_add_compile_fail_test(NAME metric_entry_range MODULE core COMPONENT metric
  SOURCE core/metric_entry_range_fail.cpp
  MESSAGE "from_gram: an entry is beyond max_form_entry"
  REQUIRE "assertion_failed")
galoot_add_compile_fail_test(NAME metric_zero MODULE core COMPONENT metric
  SOURCE core/metric_zero_fail.cpp
  MESSAGE "metric: an algebra has at least one generator, so n = 0 is rejected"
  REQUIRE "static assertion failed")
galoot_add_compile_fail_test(NAME metric_bound MODULE core COMPONENT metric
  SOURCE core/metric_bound_fail.cpp
  MESSAGE "metric: n is larger than MaxN, the largest number of generators accepted"
  REQUIRE "static assertion failed")
galoot_add_compile_fail_test(NAME metric_count_wrap MODULE core COMPONENT metric
  SOURCE core/metric_count_wrap_fail.cpp
  MESSAGE "metric: n is larger than MaxN, the largest number of generators accepted"
  REQUIRE "static assertion failed")
galoot_add_compile_fail_test(NAME metric_count_wrap_zero
  MODULE core COMPONENT metric
  SOURCE core/metric_count_wrap_zero_fail.cpp
  MESSAGE "metric: n is larger than MaxN, the largest number of generators accepted"
  REQUIRE "static assertion failed"
  FORBID "at least one generator")
galoot_add_compile_fail_test(NAME metric_conformal_wrap
  MODULE core COMPONENT metric
  SOURCE core/metric_conformal_wrap_fail.cpp
  MESSAGE "metric: n is larger than MaxN, the largest number of generators accepted"
  REQUIRE "static assertion failed")
galoot_add_compile_fail_test(NAME metric_conformal_wrap_zero
  MODULE core COMPONENT metric
  SOURCE core/metric_conformal_wrap_zero_fail.cpp
  MESSAGE "metric: n is larger than MaxN, the largest number of generators accepted"
  REQUIRE "static assertion failed"
  FORBID "at least one generator")
galoot_add_compile_fail_test(NAME metric_max_n_width MODULE core COMPONENT metric
  SOURCE core/metric_max_n_width_fail.cpp
  MESSAGE "metric: MaxN is larger than the number of bits in a blade mask"
  REQUIRE "static assertion failed")
galoot_add_compile_fail_test(NAME metric_type_width MODULE core COMPONENT metric
  SOURCE core/metric_type_width_fail.cpp
  MESSAGE "metric: n is larger than the number of bits in a blade mask"
  REQUIRE "static assertion failed")

# core.metric_like_no_generators: a tag whose metric has no generators fails
# metric_like, and the diagnostic names the generator count, not a later
# requirement that would also fail.
galoot_add_compile_fail_test(NAME metric_like_no_generators
  MODULE core COMPONENT metric
  SOURCE core/metric_like_no_generators_fail.cpp
  MESSAGE "generator_count"
  REQUIRE "metric_like")

# tools.expect_output_forbid: a forbidden literal in the output fails the
# check, which WILL_FAIL turns into a pass; its twin passes because the
# literal is absent.
add_test(NAME tools.expect_output_forbid
  COMMAND ${expect_output} 0 FORBID "forbidden text"
    -- ${CMAKE_COMMAND} -E echo "this line has forbidden text")
set_tests_properties(tools.expect_output_forbid PROPERTIES WILL_FAIL ON)
add_test(NAME tools.expect_output_forbid_absent
  COMMAND ${expect_output} 0 "allowed" FORBID "forbidden text"
    -- ${CMAKE_COMMAND} -E echo "allowed")
set_tests_properties(tools.expect_output_forbid
  tools.expect_output_forbid_absent PROPERTIES
  LABELS "tools;tools.expect_output")

# core.metric_is_null: an index past the last generator aborts at run time in
# a checking build. Like the assert programs, it fixes its own checking mode.
add_executable(core_metric_is_null_checking core/metric_is_null_fail.cpp)
target_include_directories(core_metric_is_null_checking PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_metric_is_null_checking PRIVATE cxx_std_23)
target_compile_definitions(core_metric_is_null_checking PRIVATE
  GALOOT_CHECKING=1)
add_test(NAME core.metric_is_null
  COMMAND ${expect_output} "Subprocess aborted"
    "galoot: assertion failed: metric::is_null: the index must be below n ("
    -- $<TARGET_FILE:core_metric_is_null_checking>)
set_tests_properties(core.metric_is_null PROPERTIES
  LABELS "core;core.metric")

# core.blade_set: blade sets, their canonical order, and the name parser,
# against definitions in the test and the golden values.
galoot_add_test(MODULE core COMPONENT blade_set)
target_link_libraries(core_blade_set_test PRIVATE galoot_golden)

# core.blade_set_<case>: each misuse of a blade set constructor fails to
# compile with its literal message, which a static_assert prints after
# `static assertion failed`. Several misuses share a message, so a test whose
# misuse could also trip a neighboring check forbids that check's message.
function(blade_set_rejects case message)
  galoot_add_compile_fail_test(NAME blade_set_${case}
    MODULE core COMPONENT blade_set
    SOURCE core/blade_set_${case}_fail.cpp
    MESSAGE "${message}"
    REQUIRE "static assertion failed" ${ARGN})
endfunction()
set(not_a_name "blades: a name must be 1 or start with e, as in e12")
set(bad_index
  "blades: after the e, a name lists generator indices, each a digit from 1 to 9")
set(index_above_n
  "blades: a name has an index above n, the number of generators")
set(repeated_generator "blades: a name lists one generator twice")
set(same_blade "blades: two names stand for the same blade")
set(no_generators
  "blade_set: an algebra has at least one generator, so n = 0 is rejected")
set(grade_range "of_grade: the grade must be from 0 to n")
blade_set_rejects(no_names "blades: the list names no blade")
blade_set_rejects(not_a_name_letter "${not_a_name}")
blade_set_rejects(not_a_name_digit "${not_a_name}")
blade_set_rejects(bad_index_zero "${bad_index}")
blade_set_rejects(bad_index_letter "${bad_index}")
blade_set_rejects(no_index "${bad_index}")
blade_set_rejects(index_above_n "${index_above_n}")
blade_set_rejects(repeated_generator "${repeated_generator}"
  FORBID "${same_blade}")
blade_set_rejects(repeated_name "${same_blade}")
blade_set_rejects(same_blade "${same_blade}" FORBID "${repeated_generator}")
# The names of these two are not read, so no message about them may appear.
blade_set_rejects(names_above_9
  "blades: names such as e123 have one digit per index, so they are read only for n up to 9, and a larger algebra takes masks"
  FORBID "${repeated_generator}")
# blades takes an algebra tag, so a type that is not one, and a tag whose
# metric has no generators, fail its metric_like constraint before a name is
# read. The function that parses the names carries the concept's argument
# too, so the tests forbid its name, which would point the user at internal
# code.
galoot_add_compile_fail_test(NAME blade_set_blades_not_metric_like
  MODULE core COMPONENT blade_set
  SOURCE core/blade_set_blades_not_metric_like_fail.cpp
  MESSAGE "metric_like"
  FORBID "make_blades")
galoot_add_compile_fail_test(NAME blade_set_names_zero
  MODULE core COMPONENT blade_set
  SOURCE core/blade_set_names_zero_fail.cpp
  MESSAGE "metric_like"
  FORBID "make_blades" "${index_above_n}")
blade_set_rejects(full_zero "${no_generators}")
blade_set_rejects(full_wide
  "blade_set: n is larger than the number of bits in a blade mask")
blade_set_rejects(of_grade_zero "${no_generators}")
blade_set_rejects(of_grade_above_n "${grade_range}")
blade_set_rejects(of_grade_negative "${grade_range}")
blade_set_rejects(merge_opposite_signs
  "merge: the two sets hold one blade with opposite signs")
blade_set_rejects(index_of_absent
  "index_of: the set holds no blade with this mask" FORBID "twice")
# Sets written out by hand that break the rules every constructor keeps.
blade_set_rejects(merge_bad_sign
  "merge: a set holds a sign other than +1 or -1")
blade_set_rejects(merge_bad_sign_second
  "merge: a set holds a sign other than +1 or -1")
blade_set_rejects(merge_repeated_mask "merge: a set holds one mask twice"
  FORBID "opposite signs")
blade_set_rejects(merge_repeated_in_second
  "merge: a set holds one mask twice")
blade_set_rejects(merge_repeated_in_second_signs
  "merge: a set holds one mask twice" FORBID "opposite signs")
blade_set_rejects(merge_bad_sign_no_conflict
  "merge: a set holds a sign other than +1 or -1" FORBID "opposite signs")
blade_set_rejects(merge_repeated_after_shift
  "merge: a set holds one mask twice")
blade_set_rejects(merge_repeated_in_second_only
  "merge: a set holds one mask twice" FORBID "opposite signs")
blade_set_rejects(merge_repeated_in_second_empty_first
  "merge: a set holds one mask twice" FORBID "opposite signs")
blade_set_rejects(merge_bad_sign_second_shared
  "merge: a set holds a sign other than +1 or -1" FORBID "opposite signs")
blade_set_rejects(index_of_bad_sign
  "index_of: the set holds a sign other than +1 or -1")
blade_set_rejects(index_of_repeated_mask
  "index_of: the set holds this mask twice")
blade_set_rejects(index_of_repeated_adjacent
  "index_of: the set holds this mask twice")

# core.blade_set_name_beyond_e9: a blade with no compact name ends the
# constant evaluation of canonical_name, and the diagnostic shows the line
# of its assertion.
galoot_add_compile_fail_test(NAME blade_set_name_beyond_e9
  MODULE core COMPONENT blade_set
  SOURCE core/blade_set_name_beyond_e9_fail.cpp
  MESSAGE "canonical_name: a generator beyond e9"
  REQUIRE "assertion_failed")

# core.blade_set_name_assert: the same blade aborts at run time in a checking
# build. Like the assert programs, it fixes its own checking mode.
add_executable(core_blade_set_name_assert core/blade_set_name_assert_fail.cpp)
target_include_directories(core_blade_set_name_assert PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_blade_set_name_assert PRIVATE cxx_std_23)
target_compile_definitions(core_blade_set_name_assert PRIVATE
  GALOOT_CHECKING=1)
add_test(NAME core.blade_set_name_assert
  COMMAND ${expect_output} "Subprocess aborted"
    "galoot: assertion failed: canonical_name: a generator beyond e9 ("
    blade_set.hpp: -- $<TARGET_FILE:core_blade_set_name_assert>)
set_tests_properties(core.blade_set_name_assert PROPERTIES
  LABELS "core;core.blade_set")

# core.blade_product: products of blades against the defining relations,
# associativity, the wedge-monomial basis, rules computed in the test, and
# the golden product tables.
galoot_add_test(MODULE core COMPONENT blade_product)
target_link_libraries(core_blade_product_test PRIVATE galoot_golden)

# core.blade_product_<case>: each misuse of blade_product ends a constant
# evaluation, and the diagnostic shows the line of the failed assertion.
set(product_overflow "a product coefficient does not fit coefficient_type")
foreach(case IN ITEMS beyond_n beyond_n_second beyond_n_31 capacity overflow
    sum_overflow)
  if(case MATCHES "^beyond_n")
    set(message "blade has a generator beyond e_n")
  elseif(case STREQUAL "capacity")
    set(message "more product terms than the capacity")
  else()
    set(message "${product_overflow}")
  endif()
  galoot_add_compile_fail_test(NAME blade_product_${case}
    MODULE core COMPONENT blade_product
    SOURCE core/blade_product_${case}_fail.cpp
    MESSAGE "${message}"
    REQUIRE "assertion_failed" "blade_product.hpp")
endforeach()

# core.blade_product_rejects_<case>: the same misuse, and a sum or product
# just past either bound of coefficient_type, abort at run time with the
# message. Like the assert programs, the program fixes its own checking mode,
# so these run in every preset.
add_executable(core_blade_product_assert_fail core/blade_product_assert_fail.cpp)
target_include_directories(core_blade_product_assert_fail PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_blade_product_assert_fail PRIVATE cxx_std_23)
target_compile_definitions(core_blade_product_assert_fail PRIVATE
  GALOOT_CHECKING=1)
foreach(case IN ITEMS beyond_n beyond_n_second beyond_n_31 capacity overflow
    sum_above sum_below product_above product_below product_below_second)
  if(case MATCHES "^beyond_n")
    set(message "blade has a generator beyond e_n")
  elseif(case STREQUAL "capacity")
    set(message "more product terms than the capacity")
  else()
    set(message "${product_overflow}")
  endif()
  add_test(NAME core.blade_product_rejects_${case}
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${message} (" blade_product.hpp:
      -- $<TARGET_FILE:core_blade_product_assert_fail> ${case})
  set_tests_properties(core.blade_product_rejects_${case} PROPERTIES
    LABELS "core;core.blade_product")
endforeach()

# core.plan: product plans over two blade sets against the products of their
# pairs, the single-term rule of diagonal forms, values worked by hand, and
# the golden product tables.
galoot_add_test(MODULE core COMPONENT plan)
target_link_libraries(core_plan_test PRIVATE galoot_golden)

# core.plan_<case>: each misuse of product_plan fails to compile with its
# message. The sets are checked before anything is generated, so a set's test
# forbids the messages that generation would add: the product's own range
# check and the checks of a generated plan and of its typing.
set(plan_range "product plan: a blade set names a generator beyond e_n")
set(plan_sign "product plan: a blade set holds a sign other than +1 or -1")
set(plan_generated
  "product plan: a generated term has a zero coefficient"
  "product plan: a generated term points past the first set"
  "product plan: a generated term points past the second set"
  "product plan: a generated term has a generator beyond e_n"
  "product plan: generated terms are out of order or repeated"
  "product plan: the result set is out of canonical order or repeats a blade"
  "product plan: the result set holds a sign other than +1"
  "product plan: a typed term points past the result set"
  "product plan: the result set holds a blade that no term uses"
  "product plan: typed terms are out of order or repeated")
foreach(case IN ITEMS beyond_n beyond_n_second beyond_n_31 wedge_beyond_n
    antigp_beyond_n)
  galoot_add_compile_fail_test(NAME plan_${case} MODULE core COMPONENT plan
    SOURCE core/plan_${case}_fail.cpp
    MESSAGE "${plan_range}"
    REQUIRE "static assertion failed"
    FORBID "${plan_sign}" "blade has a generator beyond e_n" "${plan_generated}")
endforeach()
foreach(case IN ITEMS bad_sign bad_sign_second)
  galoot_add_compile_fail_test(NAME plan_${case} MODULE core COMPONENT plan
    SOURCE core/plan_${case}_fail.cpp
    MESSAGE "${plan_sign}"
    REQUIRE "static assertion failed"
    FORBID "${plan_range}" "${plan_generated}")
endforeach()
galoot_add_compile_fail_test(NAME plan_overflow MODULE core COMPONENT plan
  SOURCE core/plan_overflow_fail.cpp
  MESSAGE "${product_overflow}"
  REQUIRE "assertion_failed" "blade_product.hpp"
  FORBID "product plan:")
# The wedge's own range check, reached by a direct call, which no set check
# precedes, with the stray generator in either blade; and the room for the
# result blades, which the plan always gives in full.
foreach(case IN ITEMS wedge_pair_beyond_n wedge_pair_beyond_n_second)
  galoot_add_compile_fail_test(NAME plan_${case} MODULE core COMPONENT plan
    SOURCE core/plan_${case}_fail.cpp
    MESSAGE "blade has a generator beyond e_n"
    REQUIRE "assertion_failed" "plan.hpp")
endforeach()
galoot_add_compile_fail_test(NAME plan_result_room MODULE core COMPONENT plan
  SOURCE core/plan_result_room_fail.cpp
  MESSAGE "more result blades than places for them"
  REQUIRE "assertion_failed" "plan.hpp")
# An antiproduct generated directly with a stray generator in either blade:
# the complement of that blade is not defined, and its own range check
# rejects it.
foreach(case IN ITEMS antiproduct_pair_beyond_n antiproduct_pair_beyond_n_second)
  galoot_add_compile_fail_test(NAME plan_${case} MODULE core COMPONENT plan
    SOURCE core/plan_${case}_fail.cpp
    MESSAGE "blade has a generator beyond e_n"
    REQUIRE "assertion_failed" "bits.hpp")
endforeach()

# core.plan_map_<case>: each misuse of unary_map or of a map term fails to
# compile with its message. The set is checked before anything is generated,
# so a set's test forbids the messages that generation would add.
set(map_range "unary map: the blade set names a generator beyond e_n")
set(map_sign "unary map: the blade set holds a sign other than +1 or -1")
set(map_generated
  "unary map: the result set is out of canonical order or repeats a blade"
  "unary map: the result set holds a sign other than +1"
  "unary map: a term points past the result set"
  "unary map: the result set holds a blade that no term uses"
  "unary map: the terms are not one for each input blade, in order"
  "unary map: a term is not on the blade that the operation gives its input"
  "unary map: a complement's sign differs from its closed form")
galoot_add_compile_fail_test(NAME plan_map_beyond_n MODULE core COMPONENT plan
  SOURCE core/plan_map_beyond_n_fail.cpp
  MESSAGE "${map_range}"
  REQUIRE "static assertion failed"
  FORBID "${map_sign}" "blade has a generator beyond e_n" "${map_generated}")
galoot_add_compile_fail_test(NAME plan_map_bad_sign MODULE core COMPONENT plan
  SOURCE core/plan_map_bad_sign_fail.cpp
  MESSAGE "${map_sign}"
  REQUIRE "static assertion failed"
  FORBID "${map_range}" "${map_generated}")
galoot_add_compile_fail_test(NAME plan_map_term_beyond_n
  MODULE core COMPONENT plan
  SOURCE core/plan_map_term_beyond_n_fail.cpp
  MESSAGE "blade has a generator beyond e_n"
  REQUIRE "assertion_failed" "plan.hpp")
galoot_add_compile_fail_test(NAME plan_map_term_zero_n
  MODULE core COMPONENT plan
  SOURCE core/plan_map_term_zero_n_fail.cpp
  MESSAGE "n must be from 1 to 32"
  REQUIRE "assertion_failed" "plan.hpp")

# core.plan_evaluated_once and core.plan_map_evaluated_once: a plan or a map
# that one unit reads several times is generated once. The unit
# core/plan_evaluated_once.cpp is compiled with the library's flags and
# Clang's -ftime-trace, and tools/evaluated_once.py compares the time of each
# plan's or map's initializer with the instantiation of the function that
# generates it, which holds the generation (the script states the rule). Each
# test compiles the unit into its own trace and object, so that the two can
# run at once. The tests exist only where the compiler is Clang: GCC writes
# no such trace, and its cache of constant-evaluated calls would serve a
# second generation from the first, so a GCC unit could not show one.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  galoot_compile_command(compile)
  foreach(kind IN ITEMS plan map)
    if(kind STREQUAL "plan")
      set(test_name core.plan_evaluated_once)
      set(variable galoot::core::detail::product_plan)
      set(generator galoot::core::detail::make_product_plan)
    else()
      set(test_name core.plan_map_evaluated_once)
      set(variable galoot::core::detail::unary_map)
      set(generator galoot::core::detail::make_unary_map)
    endif()
    set(evaluated_once_trace
      ${CMAKE_CURRENT_BINARY_DIR}/core_${kind}_evaluated_once.json)
    add_test(NAME ${test_name}
      COMMAND ${Python3_EXECUTABLE} -I
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/evaluated_once.py
        ${evaluated_once_trace} ${variable} ${generator}
        -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${compile}
          -ftime-trace=${evaluated_once_trace} -ftime-trace-granularity=0
          -o core_${kind}_evaluated_once.o
          -c ${CMAKE_CURRENT_SOURCE_DIR}/core/plan_evaluated_once.cpp
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMAND_EXPAND_LISTS)
    set_tests_properties(${test_name} PROPERTIES LABELS "core;core.plan")
  endforeach()
endif()

# tools.evaluated_once_*: the trace check passes on a trace of three plans
# whose initializers copy them, and fails on one whose initializers generate
# them again and on one of two plans, too few to tell.
set(evaluated_once ${Python3_EXECUTABLE} -I
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/evaluated_once.py)
set(evaluated_once_names galoot::core::detail::product_plan
  galoot::core::detail::make_product_plan)
set(evaluated_once_fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/evaluated_once)
add_test(NAME tools.evaluated_once_passes
  COMMAND ${expect_output} 0 "ratio 0.0007"
    -- ${evaluated_once} ${evaluated_once_fixtures}/once.json
      ${evaluated_once_names})
add_test(NAME tools.evaluated_once_rejects_twice
  COMMAND ${expect_output} 1 "so it generates the plan again"
    -- ${evaluated_once} ${evaluated_once_fixtures}/twice.json
      ${evaluated_once_names})
add_test(NAME tools.evaluated_once_rejects_few
  COMMAND ${expect_output} 1 "fewer than 3 of either"
    -- ${evaluated_once} ${evaluated_once_fixtures}/few.json
      ${evaluated_once_names})
set_tests_properties(tools.evaluated_once_passes
  tools.evaluated_once_rejects_twice tools.evaluated_once_rejects_few
  PROPERTIES LABELS "tools;tools.evaluated_once")
# The tag is checked by the concept on product_plan itself, whose failure the
# compiler reports in its own words; both name the concept. The function that
# generates the plan carries the concept too, so the test forbids its name:
# a diagnostic that leads with it would point the user at internal code.
galoot_add_compile_fail_test(NAME plan_not_metric_like
  MODULE core COMPONENT plan
  SOURCE core/plan_not_metric_like_fail.cpp
  MESSAGE "metric_like"
  FORBID "make_product_plan")
galoot_add_compile_fail_test(NAME plan_map_not_metric_like
  MODULE core COMPONENT plan
  SOURCE core/plan_map_not_metric_like_fail.cpp
  MESSAGE "metric_like"
  FORBID "make_unary_map")

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

# The test-only rational scalar: an exact scalar whose values need not be
# integers, which tests use as an oracle. It belongs to the tests, not the
# library, so it does not link galoot::core. A test that needs it links this
# target and includes <support/rational.hpp>; the header also compiles on its
# own, in all_verify_interface_header_sets.
add_library(galoot_test_support INTERFACE)
target_sources(galoot_test_support INTERFACE
  FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES support/rational.hpp)
target_compile_features(galoot_test_support INTERFACE cxx_std_23)
set_property(TARGET galoot_test_support
  PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# support.rational: hand values, the field identities on seeded random
# fractions, equality against cross-multiplication, and results at the bounds
# of std::int64_t.
galoot_add_test(MODULE support COMPONENT rational)
target_link_libraries(support_rational_test PRIVATE galoot_test_support)

# support.rational_rejects_<case>: a result that does not fit, a zero
# denominator, or a division by zero aborts at run time with its message, in
# every preset, since the rational checks whatever the checking mode.
add_executable(support_rational_overflow_fail support/rational_overflow_fail.cpp)
target_link_libraries(support_rational_overflow_fail PRIVATE
  galoot_test_support)
set(rational_does_not_fit "rational: a result does not fit std::int64_t")
foreach(case IN ITEMS add_above subtract_below multiply_above numerator_min)
  add_test(NAME support.rational_rejects_${case}
    COMMAND ${expect_output} "Subprocess aborted" ${rational_does_not_fit}
      -- $<TARGET_FILE:support_rational_overflow_fail> ${case})
  set_tests_properties(support.rational_rejects_${case} PROPERTIES
    LABELS "support;support.rational")
endforeach()
add_test(NAME support.rational_rejects_zero_denominator
  COMMAND ${expect_output} "Subprocess aborted"
    "rational: the denominator is zero"
    -- $<TARGET_FILE:support_rational_overflow_fail> zero_denominator)
add_test(NAME support.rational_rejects_divide_by_zero
  COMMAND ${expect_output} "Subprocess aborted" "rational: division by zero"
    -- $<TARGET_FILE:support_rational_overflow_fail> divide_by_zero)
set_tests_properties(support.rational_rejects_zero_denominator
  support.rational_rejects_divide_by_zero PROPERTIES
  LABELS "support;support.rational")

# support.rational_consteval_<operation>: the same overflow ends a constant
# evaluation, and the diagnostic shows the message through the line of the
# failing call, which only that call names.
foreach(operation IN ITEMS add multiply)
  galoot_add_compile_fail_test(NAME rational_consteval_${operation}
    MODULE support COMPONENT rational
    SOURCE support/rational_consteval_${operation}_fail.cpp
    MESSAGE "${rational_does_not_fit}"
    REQUIRE rational_failed)
endforeach()

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
# and nothing else (ONLY "^/": every output line that starts with a path
# must hold an expected finding), and passes its accepting fixtures.
set(reference_fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/test_references)
set(test_references
  ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/test_references.py)
# The findings that a rejecting fixture must produce, one per line, are read
# from a file in the fixture directory, because they quote the citations the
# fixtures hold on purpose, and tests.citations skips that text. In the file,
# @reference_fixtures@ stands for the fixture directory.
function(galoot_expected_findings file out)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${file})
  file(READ ${file} text)
  string(CONFIGURE "${text}" text @ONLY)
  string(STRIP "${text}" text)
  # With no line, a rejecting test would pass on its exit status alone.
  if(text STREQUAL "")
    message(FATAL_ERROR "${file} lists no expected finding")
  endif()
  # A ; inside a line is passed as character 1, which expect_output.cmake
  # reads as a ;, so that CMake does not split the line into two literals.
  string(ASCII 1 semicolon)
  string(REPLACE ";" "${semicolon}" text "${text}")
  string(REPLACE "\n" ";" lines "${text}")
  set(${out} ${lines} PARENT_SCOPE)
endfunction()

# tests.citations: no build file, header, test, script, or generated file
# points the reader to a project document, a numbered rule, or a project
# record in place of the reason itself, because a reader of the code has none
# of them. The file of patterns and the fixtures that hold such text on
# purpose are skipped. tools.test_references_citations_* show that the check
# rejects each pattern and passes text that only resembles one.
add_test(NAME tests.citations
  COMMAND ${test_references} citations
    --exclude ${PROJECT_SOURCE_DIR}/tools/citation_patterns.txt
    --exclude ${reference_fixtures}/markers_bad.cpp
    --exclude ${reference_fixtures}/markers_bad.expected
    --exclude ${reference_fixtures}/trace_bad_table.expected
    --exclude ${reference_fixtures}/trace_empty_table.expected
    --exclude ${reference_fixtures}/citations_bad
    --exclude ${reference_fixtures}/citations_bad.expected
    ${PROJECT_SOURCE_DIR}/include/galoot ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/tools
    ${PROJECT_SOURCE_DIR}/CMakeLists.txt ${PROJECT_SOURCE_DIR}/CMakePresets.json
    ${PROJECT_SOURCE_DIR}/.clang-format ${PROJECT_SOURCE_DIR}/.clang-tidy)
set_tests_properties(tests.citations PROPERTIES
  LABELS "tests;tests.citations")
galoot_expected_findings(${reference_fixtures}/citations_bad.expected
  citations_bad_findings)
add_test(NAME tools.test_references_citations_rejects
  COMMAND ${expect_output} 1 ONLY "^/" ${citations_bad_findings}
    -- ${test_references} citations ${reference_fixtures}/citations_bad)
add_test(NAME tools.test_references_citations_accepts
  COMMAND ${expect_output} 0
    "3 files, none citing a project document or record"
    -- ${test_references} citations ${reference_fixtures}/citations_good)
galoot_expected_findings(${reference_fixtures}/markers_bad.expected
  markers_bad_findings)
add_test(NAME tools.test_references_markers_rejects
  COMMAND ${expect_output} 1 ONLY "^/" ${markers_bad_findings}
    -- ${test_references} markers ${reference_fixtures}/markers_bad.cpp)
add_test(NAME tools.test_references_markers_accepts
  COMMAND ${expect_output} 0
    "5 test cases, each with a reference marker"
    -- ${test_references} markers ${reference_fixtures}/markers_good.cpp)
add_test(NAME tools.test_references_golden_rejects
  COMMAND ${expect_output} 1 ONLY "^/"
    "golden_bad_header.hpp:24: namespace alpha is defined twice"
    "golden_bad_header.hpp:29: namespace delta states no fact"
    "golden_bad.cpp:6: 'using' hides which test uses golden values"
    "golden_bad.cpp:11: uses golden values of alpha but its reference does not state 'the fact of alpha'"
    "golden_bad.cpp:16: uses golden values of beta but its reference does not state 'e1 ^ e2 = -e2 ^ e1'"
    "golden_bad.cpp:20: static_assert has no reference marker"
    "golden_bad.cpp:20: uses golden values of gamma but its reference does not state 'the fact of gamma'"
    "golden_bad.cpp:8: golden values of alpha are used outside a test case or a static_assert"
    "golden_bad.cpp:6: golden values of gamma are used outside a test case or a static_assert"
    -- ${test_references} golden ${reference_fixtures}/golden_bad_header.hpp
      ${reference_fixtures}/golden_bad.cpp)
add_test(NAME tools.test_references_golden_accepts
  COMMAND ${expect_output} 0
    "every use of golden values states its fact"
    -- ${test_references} golden ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
add_test(NAME tools.test_references_golden_helper_header
  COMMAND ${expect_output} 1 ONLY "^/"
    "helper_headers/helper.hpp:7: golden values of alpha are used outside a test case"
    -- ${test_references} golden ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp ${reference_fixtures}/helper_headers)
galoot_expected_findings(${reference_fixtures}/trace_bad_table.expected
  trace_bad_findings)
add_test(NAME tools.test_references_trace_rejects
  COMMAND ${expect_output} 1 ONLY "^/" ${trace_bad_findings}
    -- ${test_references} trace ${reference_fixtures}/trace_bad_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
galoot_expected_findings(${reference_fixtures}/trace_empty_table.expected
  trace_empty_findings)
add_test(NAME tools.test_references_trace_no_rows
  COMMAND ${expect_output} 1 ONLY "^/" ${trace_empty_findings}
    -- ${test_references} trace ${reference_fixtures}/trace_empty_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
add_test(NAME tools.test_references_trace_accepts
  COMMAND ${expect_output} 0
    "1.1: ${reference_fixtures}/golden_good.cpp:12"
    "1.2: ${reference_fixtures}/golden_good.cpp:17"
    "2.1: ${reference_fixtures}/golden_good.cpp:23"
    "items with no test: 3.1, 4.1"
    -- ${test_references} trace ${reference_fixtures}/trace_good_table.md
      ${reference_fixtures}/trace_conventions.md
      ${reference_fixtures}/golden_fixture.hpp
      ${reference_fixtures}/golden_good.cpp)
set_tests_properties(
  tools.test_references_markers_rejects tools.test_references_markers_accepts
  tools.test_references_golden_rejects tools.test_references_golden_accepts
  tools.test_references_golden_helper_header
  tools.test_references_trace_rejects tools.test_references_trace_no_rows
  tools.test_references_trace_accepts
  tools.test_references_citations_rejects tools.test_references_citations_accepts
  PROPERTIES LABELS "tools;tools.test_references")
