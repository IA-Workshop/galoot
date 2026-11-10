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

# The benchmarks, their reference kernels, and the tests of both.
add_subdirectory(bench)

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
# galoot::core, whose build interface carries the build preset's mode.
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

# core.assert_checking_value: a checking mode other than 0 or 1 stops the
# compile with one error that says which values the mode takes.
galoot_add_compile_fail_test(NAME assert_checking_value
  MODULE core COMPONENT assert
  SOURCE core/assert_checking_value_fail.cpp
  MESSAGE "GALOOT_CHECKING must be 0 or 1"
  ONE_ERROR)

# core.bits: the blade-mask functions against their definitions and against
# the golden values (galoot_golden, below).
galoot_add_test(MODULE core COMPONENT bits)
target_link_libraries(core_bits_test PRIVATE galoot_golden)

# core.bits_rejects_<function>_<case>: out-of-range input aborts with the
# precondition's message at run time. The program fixes the checking mode, as
# the assert programs above do, so these run in every build preset. Each
# function that takes n is tried with n below and above the range and with
# e3 in an algebra of two generators; core.bits_accepts_<function> gives it
# the widest input in range, the antiscalar of mask_bits generators, and must
# exit 0.
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

# tests.messages_pinned: every message of include/galoot/ (a static_assert
# literal, a GALOOT_ASSERT message, an #error text) is required by a test of
# this build tree, or is in internal_messages.txt with the reason no user
# reaches it, and every test that requires a part of a longer message also
# forbids the longer one. The test reads this tree's own listing, as ctest
# --show-only gives it with no preset's filter, so a test that a preset
# leaves out of its run still counts; it builds nothing. It also carries
# core.headers and rga3.headers, so that a change to a header of the core
# or of the rga3 preset, which may add a message, selects it.
add_test(NAME tests.messages_pinned
  COMMAND ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/catalogue pinned
    ${PROJECT_SOURCE_DIR}/include/galoot ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/internal_messages.txt
    --ctest ${CMAKE_CTEST_COMMAND})
set_tests_properties(tests.messages_pinned PROPERTIES
  LABELS "tests;tests.catalogue;core.headers;rga3.headers")

# tools.compile_fail_literal_in_path_*: a compile-fail test whose required
# literal occurs in its own source path stops configuring, since any compile
# error would show the literal; one outside the path passes.
set(literal_in_path_check
  ${CMAKE_COMMAND} -DLITERAL=narrow -DSOURCE=/src/core/mv_narrowing_fail.cpp
  -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/literal_in_path_check.cmake)
add_test(NAME tools.compile_fail_literal_in_path_rejects
  COMMAND ${expect_output} 1
    "fixture: the required literal 'narrow' occurs in the source path"
    -- ${literal_in_path_check})
add_test(NAME tools.compile_fail_literal_in_path_accepts
  COMMAND ${expect_output} 0 "the literal is outside the path"
    -- ${CMAKE_COMMAND} -DLITERAL=narrow
      -DSOURCE=/src/core/mv_int_variable_fail.cpp
      -P ${CMAKE_CURRENT_SOURCE_DIR}/tools/literal_in_path_check.cmake)
set_tests_properties(tools.compile_fail_literal_in_path_rejects
  tools.compile_fail_literal_in_path_accepts PROPERTIES
  LABELS "tools;tools.compile_fail")

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
    ${PROJECT_SOURCE_DIR}/include core)
set_tests_properties(core.bare_compile core.include_scan PROPERTIES
  LABELS "core;core.headers")

# tools.include_scan_rejects: the scan reports each fixture's defect, in the
# core and in the rga3 preset, and nothing about the fixtures that include
# what their module may: galoot/presets/rga3.hpp,
# galoot/presets/rga3/uses_core.hpp, and galoot/core.hpp.
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
    "galoot/core/includes_preset.hpp:4: <galoot/presets/rga3/uses_core.hpp> belongs to the rga3 preset, which the core may not include"
    "galoot/presets/rga3/nonstandard.hpp:4: <vector_math.h> is not a C++23 standard header, a core header, or an rga3 header"
    "galoot/presets/rga3/cycle_a.hpp: include cycle: galoot/presets/rga3/cycle_a.hpp -> galoot/presets/rga3/cycle_b.hpp -> galoot/presets/rga3/cycle_a.hpp"
    "galoot/presets/rga3/wrong_guard.hpp:1: include guard should be GALOOT_PRESETS_RGA3_WRONG_GUARD_HPP"
    "galoot/presets/rga3/Bad-Name.hpp: header name is not snake_case.hpp"
    "galoot/presets/rga3/not_hpp.h: not a .hpp header"
    -- ${Python3_EXECUTABLE} -I
      ${PROJECT_SOURCE_DIR}/tools/include_scan.py ${fixtures})
set_tests_properties(tools.include_scan_rejects PROPERTIES
  LABELS "tools;tools.include_scan")

# tools.mutate: the mutation driver's refusals and verdicts on a small
# project of its own, built with both compilers in a scratch repository whose
# run log takes every row the driver writes. The driver's builds and tests
# draw from the job pool that CTest's caller exports.
add_test(NAME tools.mutate
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/mutate_test.py
    ${PROJECT_SOURCE_DIR}/tools ${CMAKE_CURRENT_BINARY_DIR}/mutate)
set_tests_properties(tools.mutate PROPERTIES LABELS "tools;tools.mutate")

# tools.compiles: compile cost before and after a change, and the sizes of
# diagnostics, in a scratch repository at two commits with hand-written
# build trees, so that the flags each compile takes can be told apart.
add_test(NAME tools.compiles
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/compiles_test.py
    ${PROJECT_SOURCE_DIR}/tools ${CMAKE_CURRENT_BINARY_DIR}/compiles)
set_tests_properties(tools.compiles PROPERTIES LABELS "tools;tools.compiles")

# tools.catalogue: which tests pin each message of a fixture header tree, from
# a CTest listing file and from a stand-in for ctest on a build tree; the
# check of a catalogue's test names; and the check that each message is
# pinned or listed as internal, with the lists it rejects.
add_test(NAME tools.catalogue
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/catalogue_test.py
    ${PROJECT_SOURCE_DIR}/tools/catalogue ${CMAKE_CURRENT_BINARY_DIR}/catalogue)
set_tests_properties(tools.catalogue PROPERTIES
  LABELS "tools;tools.catalogue")

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

# How each compiler says that a type does not meet metric_like. The concept's
# name alone would not do: it occurs in the paths of the files that test it.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(not_metric_like "required for the satisfaction of ‘metric_like<Alg>’")
else()
  set(not_metric_like "does not satisfy 'metric_like'")
endif()

# core.metric_like_no_generators: a tag whose metric has no generators fails
# metric_like, and the diagnostic names the generator count, not a later
# requirement that would also fail.
galoot_add_compile_fail_test(NAME metric_like_no_generators
  MODULE core COMPONENT metric
  SOURCE core/metric_like_no_generators_fail.cpp
  MESSAGE "generator_count"
  REQUIRE "${not_metric_like}")

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
# blades takes an algebra tag, so a type that is not one (int, a class with
# no member value, one whose value is no metric) and a tag whose metric has
# no generators are rejected before a name is read, each with one error and
# its message, on a first error line that names no internal code; the notes
# below it may name the function that parses the names. A message's ; is
# held as character 1, which expect_output.cmake reads as a ;.
string(ASCII 1 semicolon)
set(blades_not_an_algebra "blades: the tag is not an algebra${semicolon} an algebra tag holds its metric in a static constexpr member named value, as in struct rga3 { static constexpr auto value = rigid<3>()${semicolon} }")
set(blades_no_generators "blades: the algebra tag's metric has no generators, and an algebra has at least one${semicolon} give the tag a metric such as rigid<3>()")
foreach(case IN ITEMS blades_not_metric_like blades_tag_without_value
    blades_value_not_metric)
  galoot_add_compile_fail_test(NAME blade_set_${case}
    MODULE core COMPONENT blade_set
    SOURCE core/blade_set_${case}_fail.cpp
    MESSAGE "${blades_not_an_algebra}"
    ONE_ERROR FORBID "${blades_no_generators}"
    FORBID_ON_FIRST_ERROR "detail::" "make_blades")
endforeach()
galoot_add_compile_fail_test(NAME blade_set_names_zero
  MODULE core COMPONENT blade_set
  SOURCE core/blade_set_names_zero_fail.cpp
  MESSAGE "${blades_no_generators}"
  ONE_ERROR FORBID "${index_above_n}" "${blades_not_an_algebra}"
  FORBID_ON_FIRST_ERROR "detail::" "make_blades")
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
# so these run in every build preset.
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

# core.plan_cost_model.<unit>: the costs that plan.hpp states for its plans
# and maps are what the compiler counts. Each core/plan_cost_<unit>.cpp reads
# one plan or map, and its threshold below is the smallest limit on constant
# evaluation under which it compiles: Clang's -fconstexpr-steps, or GCC's
# -fconstexpr-ops-limit with GCC's cache of constant calls turned off, so
# that the count does not depend on what else the unit evaluates. Each test
# compiles its unit at its threshold and one below, and on GCC also just
# above (tools/plan_cost_model.py); one test per unit, so that they run in
# parallel and a failure names its unit. Run them all with
# ctest -R '^core\.plan_cost_model\.'. The thresholds were worked out from a
# count of the paths each generator takes and confirmed by these compiles,
# for Clang 23.1 and GCC 16.2 with libstdc++ 16. A different compiler or
# standard library, or a change to the generators on a path that some unit's
# largest evaluation takes, changes such a count, and then these tests fail
# until the thresholds and the costs stated in plan.hpp are worked out again.
# The counts hold only with _GLIBCXX_ASSERTIONS defined, as the stated costs
# say, so the tests are registered only in builds that define it; and GCC's
# sanitizers add checks that its constant evaluation counts too, 2% to 4%
# more for these units, so on GCC only in builds without them. The units are
# also compiled once as an ordinary library, at the default limits, so that
# every build and the static checks see them. The tests compile them outside
# the compile-cost launcher, because each is compiled several times.
if(CMAKE_CXX_FLAGS MATCHES "-D_GLIBCXX_ASSERTIONS")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(plan_cost_compiler clang)
    set(plan_cost_cases
      gp_full5=655239
      gp_dense4=681823
      gp_grades_3_7=601471
      antigp_sparse16=77513
      reverse_full7=313635
      right_complement_full8=813090
      antiwedge_full5=271653
      gp_vectors_full6=474959)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
      NOT CMAKE_CXX_FLAGS MATCHES "-fsanitize")
    set(plan_cost_compiler gcc)
    set(plan_cost_cases
      gp_full5=3611426
      gp_dense4=3645825
      gp_grades_3_7=4235775
      antigp_sparse16=443211
      reverse_full7=1577666
      right_complement_full8=5717129
      antiwedge_full5=1393084
      gp_vectors_full6=2415778)
  endif()
endif()
if(DEFINED plan_cost_compiler)
  galoot_compile_command(compile)
  set(plan_cost_sources "")
  foreach(case IN LISTS plan_cost_cases)
    string(REPLACE "=" ";" unit_and_threshold "${case}")
    list(GET unit_and_threshold 0 unit)
    list(GET unit_and_threshold 1 threshold)
    set(source ${CMAKE_CURRENT_SOURCE_DIR}/core/plan_cost_${unit}.cpp)
    list(APPEND plan_cost_sources ${source})
    add_test(NAME core.plan_cost_model.${unit}
      COMMAND ${Python3_EXECUTABLE} -I
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/plan_cost_model.py
        ${plan_cost_compiler} ${source}=${threshold} -- ${compile}
      COMMAND_EXPAND_LISTS)
    set_tests_properties(core.plan_cost_model.${unit} PROPERTIES
      LABELS "core;core.plan")
  endforeach()
  add_library(core_plan_cost_units OBJECT ${plan_cost_sources})
  target_link_libraries(core_plan_cost_units PRIVATE galoot::core)
endif()

# tools.plan_cost_model_*: the check passes when a unit's count is the one
# recorded, and fails, naming the count it finds, when the count is higher or
# lower; it fails when the compile one below the threshold fails for another
# reason than the limit, when it compiles one below, as GCC can when its
# count runs out inside the argument of a built-in function, and when it
# compiles under no limit at all. On GCC it also fails when the unit compiles
# at its threshold but not just above it, so that a count that rose to where
# GCC stops counting cannot pass, and it names GCC's other message for a count
# that runs out inside a built-in function's argument. The stand-in compiler's
# threshold is set by its first argument.
set(plan_cost_model ${Python3_EXECUTABLE} -I
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/plan_cost_model.py)
set(fake_compiler ${Python3_EXECUTABLE} -I
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/plan_cost_model/fake_compiler.py)
add_test(NAME tools.plan_cost_model_passes
  COMMAND ${expect_output} 0 "unit.cpp: compiles at 1000 and fails at 999"
    -- ${plan_cost_model} clang unit.cpp=1000 -- ${fake_compiler} 1000)
add_test(NAME tools.plan_cost_model_count_higher
  COMMAND ${expect_output} 1 "does not compile at its threshold"
    "its measured threshold is 1005"
    -- ${plan_cost_model} gcc unit.cpp=1000 -- ${fake_compiler} 1005)
add_test(NAME tools.plan_cost_model_count_lower
  COMMAND ${expect_output} 1 "compiles one below its threshold"
    "its measured threshold is 995"
    -- ${plan_cost_model} clang unit.cpp=1000 -- ${fake_compiler} 995)
add_test(NAME tools.plan_cost_model_other_error
  COMMAND ${expect_output} 1 "for another reason than the limit"
    -- ${plan_cost_model} gcc unit.cpp=1000
      -- ${fake_compiler} 1000 other-below)
add_test(NAME tools.plan_cost_model_passes_below
  COMMAND ${expect_output} 1 "compiles one below its threshold"
    -- ${plan_cost_model} gcc unit.cpp=1000 -- ${fake_compiler} 1000 evades=999)
add_test(NAME tools.plan_cost_model_evasion_above
  COMMAND ${expect_output} 1 "its measured threshold is 2000"
    -- ${plan_cost_model} gcc unit.cpp=1000
      -- ${fake_compiler} 2000 evades=1500)
add_test(NAME tools.plan_cost_model_never_compiles
  COMMAND ${expect_output} 1 "does not compile under any limit"
    -- ${plan_cost_model} gcc unit.cpp=1000
      -- ${fake_compiler} 2000000000000)
add_test(NAME tools.plan_cost_model_evades_at_threshold
  COMMAND ${expect_output} 1 "does not compile just above its threshold"
    "its measured threshold is 1005"
    -- ${plan_cost_model} gcc unit.cpp=1000
      -- ${fake_compiler} 1005 evades=1000)
add_test(NAME tools.plan_cost_model_builtin_below
  COMMAND ${expect_output} 1 "inside the argument of a built-in function"
    "its measured threshold is 1000"
    -- ${plan_cost_model} gcc unit.cpp=1000
      -- ${fake_compiler} 1000 builtin-below)
set_tests_properties(tools.plan_cost_model_passes
  tools.plan_cost_model_count_higher tools.plan_cost_model_count_lower
  tools.plan_cost_model_other_error tools.plan_cost_model_passes_below
  tools.plan_cost_model_evasion_above tools.plan_cost_model_never_compiles
  tools.plan_cost_model_evades_at_threshold tools.plan_cost_model_builtin_below
  PROPERTIES LABELS "tools;tools.plan_cost_model")

# The tag is checked by the concept on product_plan itself, whose failure the
# compiler reports in its own words; both name the concept. The function that
# generates the plan carries the concept too, so the test forbids its name:
# a diagnostic that leads with it would point the user at internal code.
galoot_add_compile_fail_test(NAME plan_not_metric_like
  MODULE core COMPONENT plan
  SOURCE core/plan_not_metric_like_fail.cpp
  MESSAGE "${not_metric_like}"
  FORBID "make_product_plan")
galoot_add_compile_fail_test(NAME plan_map_not_metric_like
  MODULE core COMPONENT plan
  SOURCE core/plan_map_not_metric_like_fail.cpp
  MESSAGE "${not_metric_like}"
  FORBID "make_unary_map")

# tools.measure_record: the fields of a cost row, the reader's `--` for cells
# that rows written before a column existed lack, and the logs and the job
# pool's FIFO that tools/timed asks for.
add_test(NAME tools.measure_record
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/measure_record_test.py
    ${PROJECT_SOURCE_DIR}/tools/measure
    ${CMAKE_CURRENT_BINARY_DIR}/measure_record)
set_tests_properties(tools.measure_record PROPERTIES
  LABELS "tools;tools.measure")

# tools.runlog: the history the run-log tool distills, the rows a compaction
# keeps, and union merges across a compaction, in scratch repositories.
add_test(NAME tools.runlog
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/runlog_test.py
    ${PROJECT_SOURCE_DIR}/tools/runlog
    ${CMAKE_CURRENT_BINARY_DIR}/runlog)
set_tests_properties(tools.runlog PROPERTIES
  LABELS "tools;tools.runlog")

# tools.runlog_check: the run log of this checkout and its distilled history
# agree. Each era recomputes from git as recorded, each row an era kept is
# there once and in its order, and no row an era removed has come back, as a
# union merge of a branch from before the era can bring them back. Skipped
# outside a git checkout and in a clone that lacks the eras' commits. Its
# own label, runlog, selects it alone, to run it once for a merge rather
# than in every build preset. The test presets leave that label out, so it
# runs from a build tree: ctest --test-dir <build tree> -L '^runlog$'.
add_test(NAME tools.runlog_check
  COMMAND ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/runlog check
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_tests_properties(tools.runlog_check PROPERTIES
  LABELS "tools;tools.runlog_check;runlog"
  SKIP_REGULAR_EXPRESSION "not inside a git checkout;is not in this clone's history")

# tools.jobpool: the job pool's argument checks, the slots it writes back and
# drains when no client holds it, and its idle stop, including a client that
# opens the pool just as it decides to stop, an idle, full pool that leaves
# its FIFO in place, a FIFO path through a symbolic link, a count of clients
# that fails, a client that opens the pool just after a count, and a FIFO
# deleted by hand. It runs pools of its own, under their own unit names and
# with their FIFOs in the build tree, never the machine's shared pool;
# skipped where there is no user service manager. It waits for the pools'
# polls for about a minute, so it is slow.
add_test(NAME tools.jobpool
  COMMAND ${Python3_EXECUTABLE} -I
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/jobpool_test.py
    ${PROJECT_SOURCE_DIR}/tools/jobpool
    ${CMAKE_CURRENT_BINARY_DIR}/jobpool)
set_tests_properties(tools.jobpool PROPERTIES
  LABELS "tools;tools.jobpool;slow"
  SKIP_RETURN_CODE 77
  TIMEOUT 180)

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
    FILES support/diagonalize.hpp support/rational.hpp support/scalar_faults.hpp
      support/tracked.hpp)
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
# every build preset, since the rational checks whatever the checking mode.
add_executable(support_rational_overflow_fail support/rational_overflow_fail.cpp)
target_link_libraries(support_rational_overflow_fail PRIVATE
  galoot_test_support)
set(rational_does_not_fit "rational: a result does not fit std::int64_t")
# add_far_above's sum, wrapped, would be -2, which no later check catches;
# the others would wrap to values that the constructor's own check rejects.
foreach(case IN ITEMS add_above add_far_above subtract_below multiply_above
    numerator_min)
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

# support.diagonalize: T G T^T = D on every form of the exhaustive tests,
# with integer squarefree squares in the range of a Gram entry, and one hand
# case of each rule of the elimination: a swap, the pairing of two null
# vectors, the radical, and the scaling of the squares.
galoot_add_test(MODULE support COMPONENT diagonalize)
target_link_libraries(support_diagonalize_test PRIVATE galoot_test_support)

# support.tracked: the tracking scalar's rules for counting roundings and
# magnitudes, the conversions between rationals and doubles, and the forward
# error bound check accepting results within the bound and rejecting results
# beyond it.
galoot_add_test(MODULE support COMPONENT tracked)
target_link_libraries(support_tracked_test PRIVATE galoot_test_support)

# support.tracked_rejects_<case>: a double that the rational cannot hold, by
# its magnitude or its denominator, aborts exact_rational at run time with
# its message.
add_executable(support_tracked_exact_rational_fail
  support/tracked_exact_rational_fail.cpp)
target_link_libraries(support_tracked_exact_rational_fail PRIVATE
  galoot_test_support)
foreach(case IN ITEMS magnitude_two_62 magnitude_above denominator_two_63)
  add_test(NAME support.tracked_rejects_${case}
    COMMAND ${expect_output} "Subprocess aborted"
      "exact_rational: the double does not fit the rational"
      -- $<TARGET_FILE:support_tracked_exact_rational_fail> ${case})
  set_tests_properties(support.tracked_rejects_${case} PROPERTIES
    LABELS "support;support.tracked")
endforeach()

# support.tracked_rejects_<case>: a division by a rounded or a zero divisor,
# and the nearest double of a rational whose scaled numerator is not a
# double, abort at run time with their messages.
add_executable(support_tracked_division_fail support/tracked_division_fail.cpp)
target_link_libraries(support_tracked_division_fail PRIVATE
  galoot_test_support)
add_test(NAME support.tracked_rejects_rounded_divisor
  COMMAND ${expect_output} "Subprocess aborted"
    "tracked: the divisor must be exact"
    -- $<TARGET_FILE:support_tracked_division_fail> rounded_divisor)
add_test(NAME support.tracked_rejects_zero_divisor
  COMMAND ${expect_output} "Subprocess aborted" "tracked: division by zero"
    -- $<TARGET_FILE:support_tracked_division_fail> zero_divisor)
add_test(NAME support.tracked_rejects_not_a_double
  COMMAND ${expect_output} "Subprocess aborted"
    "correctly_rounded: w x is not a double"
    -- $<TARGET_FILE:support_tracked_division_fail> not_a_double)
set_tests_properties(support.tracked_rejects_rounded_divisor
  support.tracked_rejects_zero_divisor support.tracked_rejects_not_a_double
  PROPERTIES LABELS "support;support.tracked")

# core.rounding: every product, map, sum, scaling, and copy over double
# within its forward error bound of the exact value from the golden tables,
# with the bound taken from a run over the tracking scalar.
galoot_add_test(MODULE core COMPONENT rounding)
target_link_libraries(core_rounding_test PRIVATE galoot_test_support
  galoot_golden)

# core.mv: the multivector type, its scalar concepts, construction,
# equality, and the checks of its layout, over the five scalar types it must
# serve, the test rational among them.
galoot_add_test(MODULE core COMPONENT mv)
target_link_libraries(core_mv_test PRIVATE galoot_test_support galoot_golden)

# core.mv_<case>: each misuse of the type fails to compile with one error
# that carries its literal message, and no other message of the type. The
# layout checks are tried on sets written out by hand, which no constructor
# of blade sets would build.
function(mv_rejects case message)
  galoot_add_compile_fail_test(NAME mv_${case}
    MODULE core COMPONENT mv
    SOURCE core/mv_${case}_fail.cpp
    MESSAGE "${message}"
    ${ARGN})
endfunction()
# A message's ; would split it into two list items, so it is held as
# character 1, which expect_output.cmake reads as a ;.
string(ASCII 1 semicolon)
set(mv_beyond_n "mv: the layout names a generator that the algebra does not have${semicolon} use a layout whose blades are all in this algebra")
set(mv_bad_sign "mv: the layout holds a sign other than +1 or -1${semicolon} give each blade of a layout written out by hand the sign +1 or -1")
set(mv_repeated "mv: the layout holds one blade twice, perhaps in both orientations${semicolon} list each blade once")
# Clang prints the condition of a failed static_assert before its message;
# each layout check reads a member named for its rule, so that the condition
# Clang prints is the rule and not the layout's masks.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(mv_range_rule "due to requirement 'layout_names_only_generators_of_the_algebra'")
  set(mv_sign_rule "due to requirement 'layout_signs_are_plus_or_minus_one'")
  set(mv_repeat_rule "due to requirement 'layout_holds_each_blade_once'")
else()
  set(mv_range_rule "")
  set(mv_sign_rule "")
  set(mv_repeat_rule "")
endif()
# The five requirements of a ring scalar after semiregular, each with its
# own message, in the order the multivector checks them.
set(mv_no_addition "mv: the scalar type has no addition${semicolon} give it a + b, or use a number type such as double")
set(mv_no_subtraction "mv: the scalar type has no subtraction${semicolon} give it a - b, or use a number type such as double")
set(mv_no_multiplication "mv: the scalar type has no multiplication${semicolon} give it a * b, or use a number type such as double")
set(mv_no_unary_minus "mv: the scalar type has no unary minus${semicolon} give it -a, which the products use for a term of coefficient -1")
set(mv_no_integer_construction "mv: the scalar type cannot be made from an integer coefficient${semicolon} give it a constructor from std::int64_t, or use a number type such as double")
set(mv_ring_messages "${mv_no_addition}" "${mv_no_subtraction}"
  "${mv_no_multiplication}" "${mv_no_unary_minus}"
  "${mv_no_integer_construction}")
set(mv_scalar_qualified "mv: the scalar type is a reference or is const or volatile${semicolon} give the type itself, as in double for const double")
set(mv_scalar_bool "mv: bool cannot be the scalar type, since true + true is true${semicolon} use an integer type such as int")
set(mv_scalar_semiregular "mv: the scalar type must be default constructible, copyable, and assignable${semicolon} give it a default constructor that makes its zero")
set(mv_scalar_copyable "mv: the scalar type must be copyable and assignable, as a number is${semicolon} give it copy construction and copy assignment, which a move-only type or a class with a const member lacks")
set(mv_count
  "mv: give one value for each component, in the order the layout lists them")
foreach(case IN ITEMS beyond_n beyond_n_31)
  mv_rejects(${case} "${mv_beyond_n}" REQUIRE ${mv_range_rule} ONE_ERROR
    FORBID "${mv_bad_sign}" "${mv_repeated}" "blade set")
endforeach()
foreach(case IN ITEMS bad_sign bad_sign_second)
  mv_rejects(${case} "${mv_bad_sign}" REQUIRE ${mv_sign_rule} ONE_ERROR
    FORBID "${mv_beyond_n}" "${mv_repeated}" "blade set")
endforeach()
foreach(case IN ITEMS repeated_mask repeated_orientations repeated_apart
    repeated_after)
  mv_rejects(${case} "${mv_repeated}" REQUIRE ${mv_repeat_rule} ONE_ERROR
    FORBID "${mv_beyond_n}" "${mv_bad_sign}" "blade set")
endforeach()
# core.mv_<case> for a scalar without a requirement of a ring scalar: the
# message of the requirement that fails, and none of the other four. Each
# scalar_lacks case lacks one requirement alone; each other case lacks
# several and must be told the first, in the order addition, subtraction,
# multiplication, unary minus, construction from an integer: a colour code
# has no arithmetic, std::string has + but not - or *, a length has + and -
# but not *, and the last two lack * and -a, and -a and the constructor.
function(mv_ring_rejects case message)
  set(others ${mv_ring_messages})
  list(REMOVE_ITEM others "${message}")
  list(LENGTH mv_ring_messages all)
  list(LENGTH others rest)
  math(EXPR removed "${all} - ${rest}")
  if(NOT removed EQUAL 1)
    message(FATAL_ERROR "mv_ring_rejects(${case}): '${message}' is not one "
      "of the messages of a ring scalar's requirements")
  endif()
  mv_rejects(${case} "${message}" ONE_ERROR
    FORBID ${others} "${mv_scalar_semiregular}")
endfunction()
mv_ring_rejects(scalar_lacks_addition "${mv_no_addition}")
mv_ring_rejects(scalar_lacks_subtraction "${mv_no_subtraction}")
mv_ring_rejects(scalar_lacks_multiplication "${mv_no_multiplication}")
mv_ring_rejects(scalar_lacks_unary_minus "${mv_no_unary_minus}")
mv_ring_rejects(scalar_lacks_integer_construction
  "${mv_no_integer_construction}")
mv_ring_rejects(not_ring_scalar "${mv_no_subtraction}")
mv_ring_rejects(scalar_without_multiplication "${mv_no_multiplication}")
mv_ring_rejects(scalar_without_arithmetic "${mv_no_addition}")
mv_ring_rejects(scalar_without_multiplication_or_negation
  "${mv_no_multiplication}")
mv_ring_rejects(scalar_without_negation_or_construction "${mv_no_unary_minus}")
# A scalar with several faults gives the message of the first alone: a
# const or reference scalar is not semiregular either, and bool has the
# operations of the last check.
foreach(case IN ITEMS scalar_reference scalar_const)
  mv_rejects(${case} "${mv_scalar_qualified}" ONE_ERROR
    FORBID "${mv_scalar_semiregular}" ${mv_ring_messages})
endforeach()
mv_rejects(scalar_bool "${mv_scalar_bool}" ONE_ERROR
  FORBID ${mv_ring_messages})
# A scalar that has a default constructor and cannot be copied or assigned
# is told so, not to give it a default constructor.
foreach(case IN ITEMS scalar_move_only scalar_const_member)
  mv_rejects(${case} "${mv_scalar_copyable}" ONE_ERROR
    FORBID "${mv_scalar_semiregular}" ${mv_ring_messages})
endforeach()
mv_rejects(too_few "${mv_count}" REQUIRE "3 == 6" ONE_ERROR)
mv_rejects(one_too_few "${mv_count}" REQUIRE "3 == 4" ONE_ERROR)
mv_rejects(too_many "${mv_count}" REQUIRE "7 == 6" ONE_ERROR)
mv_rejects(one_value "${mv_count}" REQUIRE "1 == 6" ONE_ERROR)
# An algebra that is not one fails the class template's constraint, so the
# diagnostic is the compiler's account of metric_like.
mv_rejects(not_metric_like "${not_metric_like}" FORBID ${mv_ring_messages})
# These record the compiler's own wording: narrowing in brace initialization,
# and an implicit conversion that no constructor allows ("conversion from").
# A new compiler may change them.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(narrowing_from_int
    "narrowing conversion of ‘w’ from ‘int’ to ‘double’ [-Werror=narrowing]")
  set(narrowing_from_double
    "narrowing conversion of ‘w’ from ‘double’ to ‘float’ [-Werror=narrowing]")
  set(float_conversion
    "conversion from ‘double’ to ‘float’ may change value [-Werror=float-conversion]")
else()
  set(narrowing_from_int
    "cannot be narrowed from type 'int' to 'double' in initializer list")
  set(narrowing_from_double
    "cannot be narrowed from type 'double' to 'float' in initializer list")
  set(float_conversion
    "loses floating-point precision: 'double' to 'float' [-Werror,-Wimplicit-float-conversion]")
endif()
# The constructor's parameters are the scalar type itself, so the messages
# name it and no helper of the library.
mv_rejects(narrowing_int "${narrowing_from_int}" ONE_ERROR FORBID "component<")
# A double variable into a float place is also a -Wconversion error on both
# compilers under the project's flags, on the same line: the language's
# narrowing error is the one the type promises, the second is the compiler's
# warning made an error. ONE_ERROR allows only error lines that hold one of
# those two texts; it does not count the lines.
mv_rejects(narrowing_float "${narrowing_from_double}" REQUIRE
  "${float_conversion}" ONE_ERROR FORBID "component<")
mv_rejects(scalar_implicit "conversion from" ONE_ERROR)
# A scalar without a default constructor is not semiregular; its value of
# the type is made, too, and raises no second error.
mv_rejects(scalar_without_default "${mv_scalar_semiregular}" ONE_ERROR
  FORBID ${mv_ring_messages} "${mv_scalar_copyable}")
# core.mv_get_<case>: get rejects each kind of name it cannot read with
# that kind's message alone: never another kind's, nor the name parser's of
# blades. A message's ; is held as character 1, as for the layout messages.
set(mv_get_messages
  "get: names have one digit per index, so they are read only for n up to 9${semicolon} reach the coefficient by its position"
  "get: give one blade name"
  "get: a name is 1 or e followed by generator indices, as in e41"
  "get: the name lists one generator twice${semicolon} a blade names each generator once"
  "get: the name has an index above n, the number of generators"
  "get: the type has no blade of this name${semicolon} its layout lists the names it holds"
  "get: the type has no blade of this name, in either orientation${semicolon} its layout does not hold it"
  "get: the type stores this blade with the other orientation${semicolon} its layout names each blade by its indices in increasing order, as e12 for e21${semicolon} use that name and negate the value"
  "get: the type stores this blade with the other orientation${semicolon} use the name its layout gives it and negate the value")
function(mv_get_rejects case message)
  set(others ${mv_get_messages})
  list(REMOVE_ITEM others "${message}")
  list(LENGTH mv_get_messages all)
  list(LENGTH others rest)
  math(EXPR removed "${all} - ${rest}")
  if(NOT removed EQUAL 1)
    message(FATAL_ERROR "mv_get_rejects(${case}): '${message}' is not one "
      "of the messages of get")
  endif()
  mv_rejects(get_${case} "${message}" ONE_ERROR FORBID ${others} "blades:")
endfunction()
list(GET mv_get_messages 0 mv_get_wide)
list(GET mv_get_messages 1 mv_get_one_name)
list(GET mv_get_messages 2 mv_get_not_a_name)
list(GET mv_get_messages 3 mv_get_repeated)
list(GET mv_get_messages 4 mv_get_above_n)
list(GET mv_get_messages 5 mv_get_absent)
list(GET mv_get_messages 6 mv_get_absent_ordered)
list(GET mv_get_messages 7 mv_get_opposite_ordered)
list(GET mv_get_messages 8 mv_get_opposite)
mv_get_rejects(wide_algebra "${mv_get_wide}")
mv_get_rejects(two_names "${mv_get_one_name}")
mv_get_rejects(not_a_name "${mv_get_not_a_name}")
mv_get_rejects(index_zero "${mv_get_not_a_name}")
mv_get_rejects(repeated_generator "${mv_get_repeated}")
mv_get_rejects(above_n "${mv_get_above_n}")
mv_get_rejects(absent "${mv_get_absent}")
mv_get_rejects(absent_ordered "${mv_get_absent_ordered}")
mv_get_rejects(opposite_ordered "${mv_get_opposite_ordered}")
mv_get_rejects(opposite "${mv_get_opposite}")

# core.mv_conversion_<case>, core.mv_coefficients_<case>: a conversion
# that drops blades, in each spelling that reaches a constructor, gives the
# conversion's message; coefficients given as one array or one braced list
# give theirs, not the count's. An implicit conversion between layouts
# reaches no constructor, since each is explicit, so its tests record the
# compiler's own wording, which a new compiler may change.
set(mv_lossy "mv: the conversion drops blades of its input${semicolon} it is lossy, so request it with truncate<Type>(x)")
set(mv_one_by_one "mv: give the coefficients one by one, not as one array or braced list${semicolon} write one value for each component, in the order the layout lists them")
set(mv_not_scalar "mv: the value does not convert to the scalar type${semicolon} give one value of the scalar type for each component")
foreach(case IN ITEMS brace paren cast)
  mv_rejects(conversion_lossy_${case} "${mv_lossy}" ONE_ERROR
    FORBID "${mv_count}" "${mv_one_by_one}")
endforeach()
foreach(case IN ITEMS as_array as_braced_list)
  mv_rejects(coefficients_${case} "${mv_one_by_one}" ONE_ERROR
    FORBID "${mv_count}" "${mv_lossy}" "${mv_not_scalar}")
endforeach()
# One value that does not convert to the scalar type, and is no array, is
# told so; one value of a scalar with an explicit constructor from it, such
# as an integer for the test rational, is a wrong count like any other.
mv_rejects(value_not_scalar "${mv_not_scalar}" ONE_ERROR
  FORBID "${mv_count}" "${mv_one_by_one}")
mv_rejects(one_explicit_value "${mv_count}" REQUIRE "1 == 6" ONE_ERROR
  FORBID "${mv_one_by_one}" "${mv_not_scalar}")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(implicit_conversion "to non-scalar type")
else()
  set(implicit_conversion "no viable conversion from")
endif()
foreach(case IN ITEMS lossy lossless)
  mv_rejects(conversion_implicit_${case} "${implicit_conversion}" ONE_ERROR
    FORBID "${mv_lossy}" "${mv_count}")
endforeach()
# core.mv_other_<cause>_<case>: a multivector of another scalar type, or of
# another algebra, given to a constructor, in each spelling, onto another
# layout, also one that would drop blades, of one blade, or of a class
# derived from one, fails with one error that names the cause, on a first
# error line that spells no layout by its masks. When both differ, the
# algebra is told.
set(mv_other_scalar "mv: the source has another scalar type, and a conversion keeps the scalar type${semicolon} convert each coefficient to the target's scalar type and build the target from those values")
set(mv_other_algebra "mv: the source is in another algebra${semicolon} a conversion between layouts keeps the algebra, so give both types the same algebra tag")
set(mv_conversion_internal_names "detail::" "component<" "blade_set<")
foreach(case IN ITEMS brace paren cast auto layout lossy one_blade derived)
  mv_rejects(other_scalar_${case} "${mv_other_scalar}" ONE_ERROR
    FORBID "${mv_other_algebra}" "${mv_lossy}" "${mv_count}"
    FORBID_ON_FIRST_ERROR ${mv_conversion_internal_names})
endforeach()
foreach(case IN ITEMS brace paren cast auto layout derived and_scalar)
  mv_rejects(other_algebra_${case} "${mv_other_algebra}" ONE_ERROR
    FORBID "${mv_other_scalar}" "${mv_lossy}" "${mv_count}"
    FORBID_ON_FIRST_ERROR ${mv_conversion_internal_names})
endforeach()

# core.mv_apply_inexact_<scalar>: a product whose plan has a coefficient
# that the scalar type cannot hold exactly: -(2^24 + 1) in float, 32767^4 in
# double, and -2^24 in short.
set(mv_inexact "apply: the scalar type cannot hold one of the product's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
foreach(case IN ITEMS float double short)
  mv_rejects(apply_inexact_${case} "${mv_inexact}" ONE_ERROR)
endforeach()
# core.mv_apply_target_lacks_blade, core.mv_apply_map_target_lacks_blade: a
# product or a map written onto a layout that lacks a blade of its result.
mv_rejects(apply_target_lacks_blade
  "apply: the target layout does not hold every blade of the product"
  ONE_ERROR)
mv_rejects(apply_map_target_lacks_blade
  "apply: the target layout does not hold every blade of the map" ONE_ERROR)

# core.mv_truncate_<case>: a target that is not a multivector type of the
# input's algebra and scalar type gives truncate's message; and a nonzero
# coefficient dropped in a constant evaluation ends it with the message of
# the check, over std::int64_t and over the test rational.
set(mv_truncate_target "truncate: the target must be an mv type of the same algebra and scalar type, not a class derived from one${semicolon} truncate changes only which blades are held")
foreach(case IN ITEMS other_algebra not_a_type other_scalar derived
    result_used)
  mv_rejects(truncate_${case} "${mv_truncate_target}" ONE_ERROR)
endforeach()
set(mv_truncate_dropped "truncate: a dropped coefficient is not zero")
foreach(case IN ITEMS consteval_nonzero consteval_nonzero_rational)
  mv_rejects(truncate_${case} "${mv_truncate_dropped}"
    REQUIRE assertion_failed)
endforeach()

# core.mv_truncate_nonzero_<scalar>, core.mv_truncate_unchecked_<scalar>: one
# program built in each checking mode truncates a motor whose scalar is not
# zero. With checking on it aborts with the message, over each exact
# scalar; with checking off it keeps the line and, over a scalar whose ==
# counts its calls, compares nothing. The programs take the core's headers
# directly, so that the build preset's mode does not reach them.
foreach(mode IN ITEMS 1 0)
  add_executable(core_mv_truncate_mode_${mode} core/mv_truncate_assert_fail.cpp)
  target_include_directories(core_mv_truncate_mode_${mode} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)
  target_link_libraries(core_mv_truncate_mode_${mode} PRIVATE
    galoot_test_support)
  target_compile_features(core_mv_truncate_mode_${mode} PRIVATE cxx_std_23)
  target_compile_definitions(core_mv_truncate_mode_${mode} PRIVATE
    GALOOT_CHECKING=${mode})
endforeach()
foreach(scalar IN ITEMS int64 int64_antiscalar rational rational_antiscalar
    counted)
  add_test(NAME core.mv_truncate_nonzero_${scalar}
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${mv_truncate_dropped} ("
      -- $<TARGET_FILE:core_mv_truncate_mode_1> ${scalar})
  add_test(NAME core.mv_truncate_unchecked_${scalar}
    COMMAND core_mv_truncate_mode_0 ${scalar})
  set_tests_properties(core.mv_truncate_nonzero_${scalar}
    core.mv_truncate_unchecked_${scalar} PROPERTIES LABELS "core;core.mv")
endforeach()

# core.mv_part_<case>: a target that is not a multivector type of the
# input's algebra and scalar type gives part's message, and not truncate's.
set(mv_part_target "part: the target must be an mv type of the same algebra and scalar type, not a class derived from one${semicolon} part changes only which blades are held")
foreach(case IN ITEMS other_algebra not_a_type other_scalar derived
    result_used)
  mv_rejects(part_${case} "${mv_part_target}" ONE_ERROR FORBID "truncate:")
endforeach()
# A requires-expression that probes part with such a target does not answer:
# the probe compiles the rejection, as the comment of part says.
mv_rejects(part_probe "${mv_part_target}" ONE_ERROR FORBID "truncate:")

# core.mv_part_never_checks_<case>: one program, built with checking on
# whatever the build preset, takes the part on a line of a motor whose
# scalar or antiscalar is not zero, which truncate would reject, over each
# exact scalar; it must keep the line and, over a scalar whose == counts its
# calls, compare nothing.
add_executable(core_mv_part_checking core/mv_part_checking.cpp)
target_include_directories(core_mv_part_checking PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core_mv_part_checking PRIVATE galoot_test_support)
target_compile_features(core_mv_part_checking PRIVATE cxx_std_23)
target_compile_definitions(core_mv_part_checking PRIVATE GALOOT_CHECKING=1)
foreach(case IN ITEMS int64 int64_antiscalar rational counted)
  add_test(NAME core.mv_part_never_checks_${case}
    COMMAND core_mv_part_checking ${case})
  set_tests_properties(core.mv_part_never_checks_${case} PROPERTIES
    LABELS "core;core.mv")
endforeach()

# A file with a second, unrelated error fails a one-error test, so the test
# above can fail; WILL_FAIL turns that failure into a pass.
mv_rejects(two_errors "${mv_count}" ONE_ERROR)
set_tests_properties(core.mv_two_errors PROPERTIES WILL_FAIL ON)

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

# tests.citations: no build file, header, test, script, generated file, or
# README points the reader to a project document, a numbered rule, or a
# project record in place of the reason itself, because a reader of the code
# or of the README has none of them. The file of patterns and the fixtures
# that hold such text on purpose are skipped. tools.test_references_citations_*
# show that the check rejects each pattern and passes text that only resembles
# one.
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
    ${PROJECT_SOURCE_DIR}/README.md
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

# core.unary: the reverse, the antireverse, the grade involution, and the
# complements, as functions and as the operators ~ and !, against the
# reference signs and complements.
galoot_add_test(MODULE core COMPONENT unary)
target_link_libraries(core_unary_test PRIVATE galoot_golden)

# The sources that build the geometric antiproduct or the antiwedge of two
# full<5> multivectors of conformal(3) compile with Clang's limit on the steps
# of one constant evaluation raised from its default of 1,048,576 to
# 1,200,000. That plan takes about 950,000 steps while libstdc++'s
# assertions are on and about 1,092,000 without them, so at the default only
# the configurations with the assertions would build these cases; at the
# raised limit every configuration builds them, and none skips them. GCC
# builds them at its own defaults.
set(full_conformal3_steps
  "$<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=1200000>")
set_source_files_properties(core/identities_conformal3_test.cpp
  core/product_conformal3_test.cpp core/identities_rounding_test.cpp
  PROPERTIES COMPILE_OPTIONS "${full_conformal3_steps}")

# core.identities: the De Morgan laws, the associativity of the products and
# the antiproducts, the defining relation on vectors, and the product rules
# of the involutions, through the public functions and operators over many
# signatures, among them seeded random diagonal forms of five and six
# generators, and on every pair and triple of basis blades of the full
# layouts of up to five generators. It tests product.hpp and
# unary.hpp together, so it also carries their labels. Its test cases are in
# identities_test.cpp, and each group of signatures is checked in a source of
# its own, so that the groups compile in parallel. It is labeled slow: its
# sources together take more than twice the compile work of any other test,
# so it would add the most to every build after a change to a core header it
# does not test.
galoot_add_test(MODULE core COMPONENT identities)
target_sources(core_identities_test PRIVATE
  core/identities_up_to_three_test.cpp
  core/identities_four_test.cpp
  core/identities_five_diagonal_test.cpp
  core/identities_conformal3_test.cpp
  core/identities_five_dense_test.cpp
  core/identities_six_test.cpp
  core/identities_sparse_test.cpp
  core/identities_five_drawn_test.cpp
  core/identities_six_drawn_test.cpp)
set_property(TEST core.identities APPEND PROPERTY LABELS core.product
  core.unary slow)

# core.identities_rounding: associativity of the geometric product and
# antiproduct, the De Morgan laws, and the composition of transforms over
# double in rigid(3) and conformal(3), each side within its forward error
# bound and the two sides within the sum of their bounds, with the bounds
# taken from a run over the tracking scalar. It builds the geometric
# antiproduct of two full<5> multivectors of conformal(3), so it takes the
# raised step limit above.
galoot_add_test(MODULE core COMPONENT identities_rounding)
target_link_libraries(core_identities_rounding_test PRIVATE
  galoot_test_support)
set_property(TEST core.identities_rounding APPEND PROPERTY LABELS
  core.product core.unary core.transform core.rounding)

# core.product: the geometric product, the wedge, and their antiproducts, as
# functions and as the operators * ^ % &, against the reference tables. The
# test cases that need the raised step limit above are in a source of their
# own, so that the others build at the compiler's defaults.
galoot_add_test(MODULE core COMPONENT product)
target_sources(core_product_test PRIVATE core/product_conformal3_test.cpp)
target_link_libraries(core_product_test PRIVATE galoot_test_support
  galoot_golden)

# core.product_mixed_<cause>_<spelling>: a product or an antiproduct of
# operands from two algebras or with two scalar types fails to compile with
# one error, the message of the product as written. The wedge's message is a
# part of the antiwedge's, which would pass the test too, so the wedge's tests
# also forbid the antiwedge's message of the same cause.
set(product_algebras "the operands are in different algebras${semicolon} give both types the same algebra tag")
set(product_scalars "the operands have different scalar types${semicolon} give both types the same scalar type")
foreach(spelling IN ITEMS geometric_product wedge times caret
    geometric_antiproduct antiwedge percent ampersand)
  if(spelling STREQUAL "times")
    set(written "a * b")
  elseif(spelling STREQUAL "caret")
    set(written "a ^ b")
  elseif(spelling STREQUAL "percent")
    set(written "a % b")
  elseif(spelling STREQUAL "ampersand")
    set(written "a & b")
  else()
    set(written "${spelling}")
  endif()
  set(twin_algebras "")
  set(twin_scalars "")
  if(spelling STREQUAL "wedge")
    set(twin_algebras "antiwedge: ${product_algebras}")
    set(twin_scalars "antiwedge: ${product_scalars}")
  endif()
  galoot_add_compile_fail_test(NAME product_mixed_algebras_${spelling}
    MODULE core COMPONENT product
    SOURCE core/product_mixed_algebras_${spelling}_fail.cpp
    MESSAGE "${written}: ${product_algebras}"
    ONE_ERROR FORBID "different scalar types" ${twin_algebras})
  galoot_add_compile_fail_test(NAME product_mixed_scalars_${spelling}
    MODULE core COMPONENT product
    SOURCE core/product_mixed_scalars_${spelling}_fail.cpp
    MESSAGE "${written}: ${product_scalars}"
    ONE_ERROR FORBID "different algebras" ${twin_scalars})
endforeach()

# core.product_narrow_scalar_<spelling>: a product with an exact coefficient
# that the scalar type cannot hold, e12 e12 = -10000 in diag(100, 100) over
# std::int8_t, fails to compile with one error, the message of the product
# as written; the message of the internal kernel must not appear.
set(product_narrow "the scalar type cannot hold one of the product's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
foreach(spelling IN ITEMS geometric_product times)
  if(spelling STREQUAL "times")
    set(written "a * b")
  else()
    set(written "${spelling}")
  endif()
  galoot_add_compile_fail_test(NAME product_narrow_scalar_${spelling}
    MODULE core COMPONENT product
    SOURCE core/product_narrow_scalar_${spelling}_fail.cpp
    MESSAGE "${written}: ${product_narrow}"
    ONE_ERROR FORBID "apply:")
endforeach()
# The same for the geometric antiproduct, whose plan multiplies the
# complements: the right complement of 1 is e12, so it holds -10000 too.
set(antiproduct_narrow "the scalar type cannot hold one of the antiproduct's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
foreach(spelling IN ITEMS geometric_antiproduct percent)
  if(spelling STREQUAL "percent")
    set(written "a % b")
  else()
    set(written "${spelling}")
  endif()
  galoot_add_compile_fail_test(NAME product_narrow_scalar_${spelling}
    MODULE core COMPONENT product
    SOURCE core/product_narrow_scalar_${spelling}_fail.cpp
    MESSAGE "${written}: ${antiproduct_narrow}"
    ONE_ERROR FORBID "apply:")
  # On full<2> the product and the antiproduct have the same coefficients
  # up to sign, so these also take two scalars, whose product is 1 and whose
  # antiproduct is -10000 e12: a check of the product's coefficients in place
  # of the antiproduct's lets them through.
  galoot_add_compile_fail_test(NAME product_narrow_scalar_on_scalars_${spelling}
    MODULE core COMPONENT product
    SOURCE core/product_narrow_scalar_on_scalars_${spelling}_fail.cpp
    MESSAGE "${written}: ${antiproduct_narrow}"
    ONE_ERROR FORBID "apply:")
endforeach()

# core.linear: sums, differences, scaling by a scalar, and negation, with
# values written out from the definitions and the laws of a vector space.
galoot_add_test(MODULE core COMPONENT linear)
target_link_libraries(core_linear_test PRIVATE galoot_test_support)

# core.linear_mixed_<cause>_<spelling>: a sum or a difference of operands
# from two algebras or with two scalar types fails to compile with one
# error, the message of the operation as written.
foreach(spelling IN ITEMS plus minus)
  if(spelling STREQUAL "plus")
    set(written "a + b")
  else()
    set(written "a - b")
  endif()
  galoot_add_compile_fail_test(NAME linear_mixed_algebras_${spelling}
    MODULE core COMPONENT linear
    SOURCE core/linear_mixed_algebras_${spelling}_fail.cpp
    MESSAGE "${written}: ${product_algebras}"
    ONE_ERROR FORBID "different scalar types")
  galoot_add_compile_fail_test(NAME linear_mixed_scalars_${spelling}
    MODULE core COMPONENT linear
    SOURCE core/linear_mixed_scalars_${spelling}_fail.cpp
    MESSAGE "${written}: ${product_scalars}"
    ONE_ERROR FORBID "different algebras")
endforeach()

# core.linear_<spelling>_number_<case>: a multivector and a number, on
# either side, of the multivector's scalar type or another, also into auto
# r, with a class derived from a multivector, and with a ring scalar that is
# not an arithmetic type (the test rational), fail to compile with one
# error, the message of the operation as written, on a first error line that
# spells no layout by its masks.
set(linear_number_plus "a + b: a multivector and a number have no sum${semicolon} make the number a multivector on the scalar blade 1 and add that")
set(linear_number_minus "a - b: a multivector and a number have no difference${semicolon} make the number a multivector on the scalar blade 1 and subtract that")
foreach(spelling IN ITEMS plus minus)
  if(spelling STREQUAL "plus")
    set(message "${linear_number_plus}")
    set(other "${linear_number_minus}")
    set(cases right left other_type left_other_type auto derived rational)
  else()
    set(message "${linear_number_minus}")
    set(other "${linear_number_plus}")
    set(cases right left other_type left_other_type derived)
  endif()
  foreach(case IN LISTS cases)
    galoot_add_compile_fail_test(NAME linear_${spelling}_number_${case}
      MODULE core COMPONENT linear
      SOURCE core/linear_${spelling}_number_${case}_fail.cpp
      MESSAGE "${message}"
      ONE_ERROR FORBID "${other}" "different algebras" "different scalar types"
        "the scalar is not of the multivector's scalar type"
      FORBID_ON_FIRST_ERROR "detail::" "component<" "blade_set<")
  endforeach()
endforeach()

# core.linear_<spelling>_<case>: a scalar of a type other than the
# multivector's scalar type fails to compile with one error, the message of
# the operation as written. The condition compares the two types, so that
# the compilers print them, the scalar's first.
set(linear_scalar_type "the scalar is not of the multivector's scalar type, the second of the two types shown${semicolon} convert it to that type first, as 2.0 for 2 over double or std::int64_t{2} over std::int64_t")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(linear_int_for_double "‘int’ is not the same as ‘double’")
else()
  set(linear_int_for_double "due to requirement 'std::is_same_v<int, double>'")
endif()
galoot_add_compile_fail_test(NAME linear_scale_left_int
  MODULE core COMPONENT linear SOURCE core/linear_scale_left_int_fail.cpp
  MESSAGE "s * a: ${linear_scalar_type}"
  REQUIRE "${linear_int_for_double}" ONE_ERROR)
galoot_add_compile_fail_test(NAME linear_scale_right_int
  MODULE core COMPONENT linear SOURCE core/linear_scale_right_int_fail.cpp
  MESSAGE "a * s: ${linear_scalar_type}"
  REQUIRE "${linear_int_for_double}" ONE_ERROR)
galoot_add_compile_fail_test(NAME linear_divide_int
  MODULE core COMPONENT linear SOURCE core/linear_divide_int_fail.cpp
  MESSAGE "a / s: ${linear_scalar_type}"
  REQUIRE "${linear_int_for_double}" ONE_ERROR)
# 2.5 times an integer multivector, which a conversion to the scalar type
# would truncate.
galoot_add_compile_fail_test(NAME linear_scale_left_fraction
  MODULE core COMPONENT linear SOURCE core/linear_scale_left_fraction_fail.cpp
  MESSAGE "s * a: ${linear_scalar_type}" ONE_ERROR)
# With every core header, a number of another type is rejected by the
# scaling's overload, not the products', and two multivectors of two scalar
# types by the products', not the scaling's.
galoot_add_compile_fail_test(NAME linear_scale_with_products
  MODULE core COMPONENT linear SOURCE core/linear_scale_with_products_fail.cpp
  MESSAGE "a * s: ${linear_scalar_type}" ONE_ERROR FORBID "a * b:")
galoot_add_compile_fail_test(NAME linear_product_with_scaling
  MODULE core COMPONENT linear SOURCE core/linear_product_with_scaling_fail.cpp
  MESSAGE "a * b: ${product_scalars}" ONE_ERROR FORBID "s * a:" "a * s:")

# core.linear_divide_consteval_zero: an integer division by zero in a
# constant evaluation ends it with the message of the check.
set(linear_zero_divisor "a / s: an integer scalar cannot divide by zero")
galoot_add_compile_fail_test(NAME linear_divide_consteval_zero
  MODULE core COMPONENT linear
  SOURCE core/linear_divide_consteval_zero_fail.cpp
  MESSAGE "${linear_zero_divisor}" REQUIRE assertion_failed)
# The same over unsigned and short. A division by zero is never a constant
# expression, so requiring assertion_failed is what shows that the check,
# and not the language alone, stops each of them.
foreach(type IN ITEMS unsigned short)
  galoot_add_compile_fail_test(NAME linear_divide_consteval_zero_${type}
    MODULE core COMPONENT linear
    SOURCE core/linear_divide_consteval_zero_${type}_fail.cpp
    MESSAGE "${linear_zero_divisor}" REQUIRE assertion_failed)
endforeach()

# core.linear_divide_rejects_zero, core.linear_divide_accepts_<case>: one
# program, built with checking on whatever the build preset, divides at run
# time. An integer division by zero aborts with the message; division by 2,
# and a floating division by 0.0, which IEEE 754 defines, exit 0. Division by
# zero with checking off is undefined for an integer type, so it is not run.
add_executable(core_linear_divide_assert_fail
  core/linear_divide_assert_fail.cpp)
target_include_directories(core_linear_divide_assert_fail PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_linear_divide_assert_fail PRIVATE cxx_std_23)
target_compile_definitions(core_linear_divide_assert_fail PRIVATE
  GALOOT_CHECKING=1)
add_test(NAME core.linear_divide_rejects_zero
  COMMAND ${expect_output} "Subprocess aborted"
    "galoot: assertion failed: ${linear_zero_divisor} (" linear.hpp:
    -- $<TARGET_FILE:core_linear_divide_assert_fail> int64_zero)
# The check covers every integer type, and only a zero divisor: unsigned
# and short divisors of zero abort too, and a negative divisor does not.
foreach(type IN ITEMS unsigned short)
  add_test(NAME core.linear_divide_rejects_${type}_zero
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${linear_zero_divisor} (" linear.hpp:
      -- $<TARGET_FILE:core_linear_divide_assert_fail> ${type}_zero)
endforeach()
add_test(NAME core.linear_divide_accepts_nonzero
  COMMAND core_linear_divide_assert_fail int64)
add_test(NAME core.linear_divide_accepts_negative
  COMMAND core_linear_divide_assert_fail int64_negative)
add_test(NAME core.linear_divide_accepts_floating_zero
  COMMAND core_linear_divide_assert_fail double_zero)
set_tests_properties(core.linear_divide_rejects_zero
  core.linear_divide_rejects_unsigned_zero
  core.linear_divide_rejects_short_zero
  core.linear_divide_accepts_nonzero core.linear_divide_accepts_negative
  core.linear_divide_accepts_floating_zero
  PROPERTIES LABELS "core;core.linear")

# tools.expect_output_forbid_first_*: FORBID_FIRST checks only the first line
# that matches the ONLY regex, so a forbidden name on a later line or on a
# line that does not match passes, the same name on the first line fails,
# and a pattern that matches no line fails.
set(first_error_fixture
  ${CMAKE_CURRENT_SOURCE_DIR}/tools/expect_output/first_error.txt)
add_test(NAME tools.expect_output_forbid_first_accepts
  COMMAND ${expect_output} 0 ONLY "error:" "the message"
    FORBID_FIRST "detail::"
    -- ${CMAKE_COMMAND} -E cat ${first_error_fixture})
add_test(NAME tools.expect_output_forbid_first_rejects
  COMMAND ${expect_output} 1 "contains forbidden 'message'"
    -- ${expect_output} 0 ONLY "error:" "the message" FORBID_FIRST "message"
      -- ${CMAKE_COMMAND} -E cat ${first_error_fixture})
add_test(NAME tools.expect_output_forbid_first_no_line
  COMMAND ${expect_output} 1 "no line matches 'warning:'"
    -- ${expect_output} 0 ONLY "warning:" FORBID_FIRST "detail::"
      -- ${CMAKE_COMMAND} -E cat ${first_error_fixture})
set_tests_properties(tools.expect_output_forbid_first_accepts
  tools.expect_output_forbid_first_rejects
  tools.expect_output_forbid_first_no_line
  PROPERTIES LABELS "tools;tools.expect_output")

# core.dual: the bulk, the weight, their duals, the dot, the antidot, and
# the projection, against the reference's metric exomorphism and
# complements.
galoot_add_test(MODULE core COMPONENT dual)
target_link_libraries(core_dual_test PRIVATE galoot_golden galoot_test_support)

# core.dual_identities: the identities that relate the bulk, the weight, the
# duals, the dot, and the antidot to each other and to the metric, over
# many signatures. A program of its own, so that a change to the tests of
# dual_test.cpp does not rebuild it; it also carries the label core.dual.
galoot_add_test(MODULE core COMPONENT dual_identities)
set_property(TEST core.dual_identities APPEND PROPERTY LABELS core.dual)

# The names of the machinery under the operations, which the first error
# line of a rejection must not show: the user wrote the operation, not these.
set(dual_internal_names "apply:" "geometric_product:" left_complement
  product_plan "detail::")

# core.dual_narrow_scalar_<function>: an operation with an exact coefficient
# that the scalar type cannot hold fails to compile with one error, the
# message of the operation as written. Each source picks a layout on which
# the operation's own coefficients overflow std::int8_t while those of the
# other operations that a wrong check could read instead do not. An optional
# third argument names a further case of the same function, with its own
# source, core.dual_narrow_scalar_<function>_<case>.
function(dual_rejects_narrow_scalar function noun)
  set(name ${function})
  if(ARGC GREATER 2)
    set(name ${function}_${ARGV2})
  endif()
  galoot_add_compile_fail_test(NAME dual_narrow_scalar_${name}
    MODULE core COMPONENT dual
    SOURCE core/dual_narrow_scalar_${name}_fail.cpp
    MESSAGE "${function}: the scalar type cannot hold one of the ${noun}'s exact coefficients${semicolon} use a scalar type that holds larger integers exactly"
    ONE_ERROR FORBID_ON_FIRST_ERROR ${dual_internal_names})
endfunction()
dual_rejects_narrow_scalar(bulk "bulk")
dual_rejects_narrow_scalar(weight "weight")
dual_rejects_narrow_scalar(bulk_dual "bulk dual")
dual_rejects_narrow_scalar(weight_dual "weight dual")
dual_rejects_narrow_scalar(left_bulk_dual "left bulk dual")
dual_rejects_narrow_scalar(left_weight_dual "left weight dual")
dual_rejects_narrow_scalar(dot "dot")
dual_rejects_narrow_scalar(antidot "antidot")
dual_rejects_narrow_scalar(project "projection")
# Kept terms of which one is held and another is not; and a projection whose
# second operand overflows while its first does not.
dual_rejects_narrow_scalar(dot "dot" mixed_kernel)
dual_rejects_narrow_scalar(antidot "antidot" mixed_kernel)
dual_rejects_narrow_scalar(project "projection" first_operand)

# core.dual_mixed_<cause>_<function>: a dot, an antidot, or a projection of
# operands from two algebras or with two scalar types fails to compile with
# one error, the message of the function, which names the cause. The
# messages of dot and project are parts of those of antidot and antiproject,
# which would pass their tests too, so those tests also forbid the longer
# message of the same cause.
set(dual_algebras "the operands are in different algebras${semicolon} give both types the same algebra tag")
set(dual_scalars "the operands have different scalar types${semicolon} give both types the same scalar type")
foreach(function IN ITEMS dot antidot project)
  set(twin_algebras "")
  set(twin_scalars "")
  if(NOT function STREQUAL "antidot")
    set(twin_algebras "anti${function}: ${dual_algebras}")
    set(twin_scalars "anti${function}: ${dual_scalars}")
  endif()
  galoot_add_compile_fail_test(NAME dual_mixed_algebras_${function}
    MODULE core COMPONENT dual
    SOURCE core/dual_mixed_algebras_${function}_fail.cpp
    MESSAGE "${function}: ${dual_algebras}"
    ONE_ERROR FORBID "different scalar types" ${twin_algebras}
    FORBID_ON_FIRST_ERROR ${dual_internal_names})
  galoot_add_compile_fail_test(NAME dual_mixed_scalars_${function}
    MODULE core COMPONENT dual
    SOURCE core/dual_mixed_scalars_${function}_fail.cpp
    MESSAGE "${function}: ${dual_scalars}"
    ONE_ERROR FORBID "different algebras" ${twin_scalars}
    FORBID_ON_FIRST_ERROR ${dual_internal_names})
endforeach()

# core.dual_compare_<function>: comparing a result with a value of another
# layout fails to compile; Clang names the result's layout by the
# operation's layout name, and neither compiler shows the plan or detail
# names on its first error line.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(dual_compare_message "invalid operands to binary expression")
else()
  set(dual_compare_message "no match for")
endif()
function(dual_rejects_comparison function)
  set(require "")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(require REQUIRE "${function}_layout<")
  endif()
  galoot_add_compile_fail_test(NAME dual_compare_${function}
    MODULE core COMPONENT dual
    SOURCE core/dual_compare_${function}_fail.cpp
    MESSAGE "${dual_compare_message}" ${require}
    ONE_ERROR FORBID_ON_FIRST_ERROR product_plan "detail::")
endfunction()
dual_rejects_comparison(bulk)
dual_rejects_comparison(weight)
dual_rejects_comparison(bulk_dual)
dual_rejects_comparison(weight_dual)
dual_rejects_comparison(left_bulk_dual)
dual_rejects_comparison(left_weight_dual)
dual_rejects_comparison(dot)
dual_rejects_comparison(antidot)
dual_rejects_comparison(project)

# The antiprojection's rejections, as the projection's: a narrow scalar,
# also where only the second operand's weight dual overflows; operands from
# two algebras or with two scalar types; and a comparison with a value of
# another layout.
dual_rejects_narrow_scalar(antiproject "antiprojection")
dual_rejects_narrow_scalar(antiproject "antiprojection" first_operand)
foreach(function IN ITEMS antiproject)
  galoot_add_compile_fail_test(NAME dual_mixed_algebras_${function}
    MODULE core COMPONENT dual
    SOURCE core/dual_mixed_algebras_${function}_fail.cpp
    MESSAGE "${function}: ${dual_algebras}"
    ONE_ERROR FORBID "different scalar types"
    FORBID_ON_FIRST_ERROR ${dual_internal_names})
  galoot_add_compile_fail_test(NAME dual_mixed_scalars_${function}
    MODULE core COMPONENT dual
    SOURCE core/dual_mixed_scalars_${function}_fail.cpp
    MESSAGE "${function}: ${dual_scalars}"
    ONE_ERROR FORBID "different algebras"
    FORBID_ON_FIRST_ERROR ${dual_internal_names})
endforeach()
dual_rejects_comparison(antiproject)

# core.transform: compose and transform, against the reference's tables of
# the products and the complements, and by hand in rigid(3) and other
# forms.
galoot_add_test(MODULE core COMPONENT transform)
target_link_libraries(core_transform_test PRIVATE galoot_golden
  galoot_test_support)

# The names of the machinery under compose and transform, which the first
# error line of a rejection must not show: the user wrote compose or
# transform, not these.
set(transform_internal_names "apply:" "geometric_antiproduct:" "a % b:"
  product_plan "detail::" "truncate:")

# core.<function>_<case>: a misuse of compose or transform fails to compile
# with one error, the message of the function as written, from the source
# core/<function>_<case>_fail.cpp. Further arguments go to the test.
function(transform_rejects function case message)
  galoot_add_compile_fail_test(NAME ${function}_${case}
    MODULE core COMPONENT transform
    SOURCE core/${function}_${case}_fail.cpp
    MESSAGE "${message}"
    ONE_ERROR ${ARGN}
    FORBID_ON_FIRST_ERROR ${transform_internal_names})
endfunction()

# A versor of both parities, as either operand of compose.
set(compose_mixed_parity "compose: an operand holds blades of both parities, even and odd antigrade${semicolon} compose versors that are each even or odd")
transform_rejects(compose mixed_parity_first "${compose_mixed_parity}")
transform_rejects(compose mixed_parity_second "${compose_mixed_parity}")
# An exact coefficient of the composition that std::int8_t cannot hold.
transform_rejects(compose narrow_scalar "compose: the scalar type cannot hold one of the composition's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
# Operands from two algebras or with two scalar types; the message names the
# cause, and not the other one.
set(transform_algebras "the operands are in different algebras${semicolon} give both types the same algebra tag")
set(transform_scalars "the operands have different scalar types${semicolon} give both types the same scalar type")
transform_rejects(compose mixed_algebras "compose: ${transform_algebras}"
  FORBID "different scalar types")
transform_rejects(compose mixed_scalars "compose: ${transform_scalars}"
  FORBID "different algebras")

# core.compose_compare: comparing a composition with a value of another
# layout fails to compile; Clang names the result's layout by its layout
# name, and neither compiler shows the plan or detail names on its first
# error line.
set(compose_compare_require "")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(compose_compare_require REQUIRE "compose_layout<")
endif()
galoot_add_compile_fail_test(NAME compose_compare
  MODULE core COMPONENT transform
  SOURCE core/compose_compare_fail.cpp
  MESSAGE "${dual_compare_message}" ${compose_compare_require}
  ONE_ERROR FORBID_ON_FIRST_ERROR product_plan "detail::")

# A versor of both parities given to transform.
transform_rejects(transform mixed_parity "transform: the versor holds blades of both parities, even and odd antigrade${semicolon} a versor is even, as a motor, or odd, as a flector")
# An exact coefficient that std::int8_t cannot hold in the first of the two
# antiproducts transform composes, and in the second: a check of one alone
# accepts the other case.
set(transform_narrow_scalar "transform: the scalar type cannot hold one of the transform's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
transform_rejects(transform narrow_scalar_first_plan
  "${transform_narrow_scalar}")
transform_rejects(transform narrow_scalar_second_plan
  "${transform_narrow_scalar}")
transform_rejects(transform mixed_algebras "transform: ${transform_algebras}"
  FORBID "different scalar types")
transform_rejects(transform mixed_scalars "transform: ${transform_scalars}"
  FORBID "different algebras")

# core.transform_consteval_nonzero_<case>: a transform that drops a
# coefficient that is not zero ends a constant evaluation, whatever the
# checking mode, and the diagnostic shows the message on the echoed line.
set(transform_dropped "transform: v does not act as a versor on x")
foreach(case IN ITEMS consteval_nonzero consteval_nonzero_rational)
  galoot_add_compile_fail_test(NAME transform_${case}
    MODULE core COMPONENT transform
    SOURCE core/transform_${case}_fail.cpp
    MESSAGE "${transform_dropped}"
    REQUIRE assertion_failed
    FORBID "truncate:")
endforeach()

# core.transform_<mode>_<case>: one program built in each checking mode
# (core/transform_assert_fail.cpp). With checking on, a transform that drops
# a coefficient that is not zero aborts with the message over each exact
# scalar; with checking off it keeps the scalar 2. Over a scalar whose ==
# records its operands, checking compares exactly the dropped coefficients,
# each once with zero, and no checking compares nothing. In rigid(3), where
# what transform drops is zero, neither mode aborts. The programs take the
# core's headers directly, so that the build preset's mode does not reach them.
foreach(mode IN ITEMS 1 0)
  add_executable(core_transform_mode_${mode} core/transform_assert_fail.cpp)
  target_include_directories(core_transform_mode_${mode} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)
  target_link_libraries(core_transform_mode_${mode} PRIVATE
    galoot_test_support)
  target_compile_features(core_transform_mode_${mode} PRIVATE cxx_std_23)
  target_compile_definitions(core_transform_mode_${mode} PRIVATE
    GALOOT_CHECKING=${mode})
endforeach()
foreach(scalar IN ITEMS int64 rational)
  add_test(NAME core.transform_nonzero_${scalar}
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${transform_dropped} ("
      -- $<TARGET_FILE:core_transform_mode_1> ${scalar})
  add_test(NAME core.transform_unchecked_${scalar}
    COMMAND core_transform_mode_0 ${scalar})
  set_tests_properties(core.transform_nonzero_${scalar}
    core.transform_unchecked_${scalar}
    PROPERTIES LABELS "core;core.transform")
endforeach()
foreach(case IN ITEMS recorded rigid_int64 rigid_rational)
  add_test(NAME core.transform_checking_${case}
    COMMAND core_transform_mode_1 ${case})
  add_test(NAME core.transform_unchecked_${case}
    COMMAND core_transform_mode_0 ${case})
  set_tests_properties(core.transform_checking_${case}
    core.transform_unchecked_${case}
    PROPERTIES LABELS "core;core.transform")
endforeach()

# core.transform_rounding: the rounding check of transform over double and
# float, against the tracking oracle.
galoot_add_test(MODULE core COMPONENT transform_rounding)
target_link_libraries(core_transform_rounding_test PRIVATE
  galoot_test_support)
set_tests_properties(core.transform_rounding PROPERTIES
  LABELS "core;core.transform;core.rounding")

# core.transform_rounding_consteval_<case>: over double and float, a
# transform that drops a coefficient far above its rounding error, positive
# or negative, ends a constant evaluation, whatever the checking mode, with
# the rounding check's message on the echoed line and not the exact check's.
set(transform_rounded "transform: v is not a versor on x within rounding")
foreach(scalar IN ITEMS double float negative_double negative_float)
  galoot_add_compile_fail_test(NAME transform_rounding_consteval_${scalar}
    MODULE core COMPONENT transform
    SOURCE core/transform_rounding_consteval_${scalar}_fail.cpp
    MESSAGE "${transform_rounded}"
    REQUIRE assertion_failed
    FORBID "${transform_dropped}")
  set_tests_properties(core.transform_rounding_consteval_${scalar}
    PROPERTIES LABELS "core;core.transform;core.rounding")
endforeach()

# core.transform_rounding_<mode>_<case>: one program built in each checking
# mode (core/transform_rounding_assert_fail.cpp). With checking on, a
# dropped part that is not rounding error, a NaN or an infinite one, or one
# whose bound overflows aborts with the message; a dropped part within the
# bound, also one whose products underflow, does not. With checking off,
# nothing aborts, and the transform that is not a versor's keeps 2. The
# programs take the core's headers directly, so that the build preset's mode
# does not reach them.
foreach(mode IN ITEMS 1 0)
  add_executable(core_transform_rounding_mode_${mode}
    core/transform_rounding_assert_fail.cpp)
  target_include_directories(core_transform_rounding_mode_${mode} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)
  target_compile_features(core_transform_rounding_mode_${mode} PRIVATE
    cxx_std_23)
  target_compile_definitions(core_transform_rounding_mode_${mode} PRIVATE
    GALOOT_CHECKING=${mode})
endforeach()
foreach(case IN ITEMS not_a_versor_double not_a_versor_float negative_double
    negative_float ten_times ten_times_float nan nan_float infinite
    infinite_float overflow overflow_float)
  add_test(NAME core.transform_rounding_checking_${case}
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${transform_rounded} ("
      -- $<TARGET_FILE:core_transform_rounding_mode_1> ${case})
  add_test(NAME core.transform_rounding_unchecked_${case}
    COMMAND core_transform_rounding_mode_0 ${case})
  set_tests_properties(core.transform_rounding_checking_${case}
    core.transform_rounding_unchecked_${case}
    PROPERTIES LABELS "core;core.transform;core.rounding")
endforeach()
set(transform_rounding_unchecked_cases coarse_unchecked)
# std::bfloat16_t, which libstdc++ provides with GCC and not with Clang.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND transform_rounding_unchecked_cases bfloat16_unchecked)
endif()
foreach(case IN ITEMS tenth underflow_double underflow_float
    ${transform_rounding_unchecked_cases})
  add_test(NAME core.transform_rounding_checking_${case}
    COMMAND core_transform_rounding_mode_1 ${case})
  add_test(NAME core.transform_rounding_unchecked_${case}
    COMMAND core_transform_rounding_mode_0 ${case})
  set_tests_properties(core.transform_rounding_checking_${case}
    core.transform_rounding_unchecked_${case}
    PROPERTIES LABELS "core;core.transform;core.rounding")
endforeach()

# core.transform_fast_math: in a checking build, transform.hpp stops
# compilation under -ffast-math, which reassociates and may flush subnormal
# numbers to zero, so that the rounding check could reject a correct result,
# with its own message and not the one for -ffinite-math-only, which
# -ffast-math implies; core.transform_finite_math_only: it stops under
# -ffinite-math-only alone, which lets the compiler assume that no value is
# a NaN or an infinity, so that the check could pass one, with that
# option's message; core.transform_without_fast_math: the same source
# compiles without either. The compiler runs as galoot_add_compile_fail_test
# runs it, with the option added.
set(transform_fast_math
  "transform: checking mode needs IEEE 754 arithmetic, not -ffast-math")
set(transform_finite_math_only
  "transform: checking mode needs NaN and infinity, not -ffinite-math-only")
set(transform_fast_math_source
  ${CMAKE_CURRENT_SOURCE_DIR}/core/transform_fast_math_fail.cpp)
galoot_check_literals_outside_path(core.transform_fast_math
  "${transform_fast_math_source}" "${transform_fast_math}"
  "${transform_finite_math_only}")
galoot_compile_command(transform_fast_math_compile)
add_test(NAME core.transform_fast_math
  COMMAND ${expect_output} 1 "${transform_fast_math}"
    FORBID "${transform_finite_math_only}"
    -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${transform_fast_math_compile}
      -ffast-math -o core_transform_fast_math.o
      -c ${transform_fast_math_source}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND_EXPAND_LISTS)
add_test(NAME core.transform_finite_math_only
  COMMAND ${expect_output} 1 "${transform_finite_math_only}"
    FORBID "${transform_fast_math}"
    -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${transform_fast_math_compile}
      -ffinite-math-only -o core_transform_finite_math_only.o
      -c ${transform_fast_math_source}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND_EXPAND_LISTS)
add_test(NAME core.transform_without_fast_math
  COMMAND ${expect_output} 0
    FORBID "${transform_fast_math}" "${transform_finite_math_only}"
    -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${transform_fast_math_compile}
      -o core_transform_without_fast_math.o
      -c ${transform_fast_math_source}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND_EXPAND_LISTS)
# core.transform_unchecked_<option>: in an unchecked build, transform.hpp
# compiles under -ffast-math and under -ffinite-math-only, with neither
# error: only a checking build runs the rounding check at run time.
set(transform_unchecked_source
  ${CMAKE_CURRENT_SOURCE_DIR}/core/transform_unchecked_fast_math.cpp)
foreach(option IN ITEMS fast-math finite-math-only)
  string(REPLACE "-" "_" name "${option}")
  add_test(NAME core.transform_unchecked_${name}
    COMMAND ${expect_output} 0
      FORBID "${transform_fast_math}" "${transform_finite_math_only}"
      -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${transform_fast_math_compile}
        -f${option} -o core_transform_unchecked_${name}.o
        -c ${transform_unchecked_source}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS)
  set_tests_properties(core.transform_unchecked_${name}
    PROPERTIES LABELS "core;core.transform;core.rounding")
endforeach()
set_tests_properties(core.transform_fast_math
  core.transform_finite_math_only core.transform_without_fast_math
  PROPERTIES LABELS "core;core.transform;core.rounding")

# core.transform_identities: the outermorphism and composition identities of
# transform over many signatures. A program of its own, so that a change to
# the tests of transform_test.cpp does not rebuild it; it also carries the
# label core.transform.
galoot_add_test(MODULE core COMPONENT transform_identities)
target_link_libraries(core_transform_identities_test PRIVATE
  galoot_test_support)
set_property(TEST core.transform_identities APPEND PROPERTY LABELS
  core.transform)

# core.basis_change: the change of basis of basis_change.hpp: what a change
# tag must be, the image of every blade under conformal_change<D>() and its
# inverse against the golden values, hand values, what depends on the basis,
# in basis_change_scalars_test.cpp, the scalar types it serves, with each
# floating result checked against its exact value or its forward error bound
# from the tracking scalar, and, in basis_change_validation_test.cpp, the
# published null pair of the conformal model and the Witt basis with its
# anticommutation relations.
galoot_add_test(MODULE core COMPONENT basis_change)
target_sources(core_basis_change_test PRIVATE
  core/basis_change_scalars_test.cpp
  core/basis_change_validation_test.cpp)
target_link_libraries(core_basis_change_test PRIVATE galoot_golden
  galoot_test_support)

# core.basis_change_identities: the change of basis as an algebra isomorphism.
# basis_change_identities_test.cpp checks the generated terms of each change
# at run time on every blade and pair of blades, for every form of the
# exhaustive tests against its exact diagonal counterpart and for the
# conformal, Witt, and doubled changes. The other sources check, through the
# public functions on seeded random multivectors, that each change maps
# products to products and that its inverse undoes it, one group of changes
# to a source, so that the groups compile in parallel. A program of its own,
# so that a change to basis_change_test.cpp does not rebuild it; it also
# carries the label core.basis_change. The source of conformal(3), which
# builds the products of two full<5> multivectors of conformal(3) and of
# R(4, 1, 0), takes the raised step limit of the identities of conformal(3).
galoot_add_test(MODULE core COMPONENT basis_change_identities)
target_sources(core_basis_change_identities_test PRIVATE
  core/basis_change_conformal_test.cpp
  core/basis_change_conformal3_test.cpp
  core/basis_change_witt_test.cpp
  core/basis_change_random_test.cpp
  core/basis_change_random_five_test.cpp)
set_source_files_properties(core/basis_change_conformal3_test.cpp
  PROPERTIES COMPILE_OPTIONS "${full_conformal3_steps}")
target_link_libraries(core_basis_change_identities_test PRIVATE
  galoot_test_support)
set_property(TEST core.basis_change_identities APPEND PROPERTY LABELS
  core.basis_change)

# The names of the machinery under a change of basis, which the first error
# line of a rejection must not show: the user wrote the tag or the call.
set(basis_change_internal_names "detail::")

# core.<name>: a misuse of a change of basis fails to compile with one
# error, its message, from the source core/<name>_fail.cpp. Further
# arguments go to the test.
function(basis_change_rejects name message)
  galoot_add_compile_fail_test(NAME ${name}
    MODULE core COMPONENT basis_change
    SOURCE core/${name}_fail.cpp
    MESSAGE "${message}"
    ONE_ERROR ${ARGN}
    FORBID_ON_FIRST_ERROR ${basis_change_internal_names})
endfunction()

# A tag whose algebras and change disagree in their number of generators:
# the check compares the two counts as written, so that both compilers print
# them, 4 against 5. When both disagree, only the source's is reported.
set(dimension "a change maps between algebras of one dimension")
set(source_count "basis_change: the source algebra has a different number of generators than the change has rows${semicolon} ${dimension}")
set(target_count "basis_change: the target algebra has a different number of generators than the change has rows${semicolon} ${dimension}")
basis_change_rejects(basis_change_source_count "${source_count}"
  REQUIRE "4 == 5")
basis_change_rejects(basis_change_target_count "${target_count}"
  REQUIRE "4 == 5")
basis_change_rejects(basis_change_both_counts "${source_count}"
  REQUIRE "5 == 4" FORBID "${target_count}")

# A denominator that is negative or zero, a change without an inverse, and a
# change that does not keep the inner products, including one whose source
# and target are swapped, one whose matrix is transposed, and one that keeps
# every square but not the null pair's inner product. A singular
# change of a form whose inner products are all zero keeps them, and must
# still be reported as singular.
set(not_positive "basis_change: every denominator must be positive${semicolon} put a row's sign in its entries")
set(dependent "basis_change: the rows are linearly dependent, so the change has no inverse${semicolon} the images of the source's generators must be independent")
set(not_isometry "basis_change: the change does not keep the inner products (P G' P^T is not G)${semicolon} check that row i writes e_i in the target's generators, and that source and target are not swapped")
basis_change_rejects(basis_change_denominator_negative "${not_positive}")
basis_change_rejects(basis_change_denominator_zero "${not_positive}")
basis_change_rejects(basis_change_dependent "${dependent}")
basis_change_rejects(basis_change_dependent_degenerate "${dependent}"
  FORBID "${not_isometry}")
basis_change_rejects(basis_change_not_isometry "${not_isometry}")
basis_change_rejects(basis_change_swapped "${not_isometry}")
basis_change_rejects(basis_change_transposed "${not_isometry}")
basis_change_rejects(basis_change_pairing_not_kept "${not_isometry}")

# An exact value of the change's checks that does not fit coefficient_type:
# the checks note it rather than stop, and report it as one error with its
# message first; here in the check of the inner products.
# The same in each other check that can overflow: the elimination (whose
# pivots grow past the range though the inner products fit), the check that
# the inverse inverts the rows, and the inverse's rows. Each must give the
# overflow message, not the message of the check it stopped.
set(overflow "basis_change: an exact value the change computes does not fit coefficient_type${semicolon} use rows with smaller entries")
basis_change_rejects(basis_change_overflow "${overflow}"
  FORBID "${not_isometry}")
basis_change_rejects(basis_change_overflow_elimination "${overflow}"
  FORBID "${dependent}")
basis_change_rejects(basis_change_overflow_inverse_check "${overflow}"
  FORBID "basis_change: the computed inverse does not invert the rows")
basis_change_rejects(basis_change_overflow_inverse_rows "${overflow}")

# A template argument that is not a change tag, and an operand in the wrong
# algebra: the target's or a third one, as a statement and into auto r.
set(not_a_change "the template argument is not a change of basis${semicolon} give a tag with source, target, and value, as in struct cga3_to_r410 { using source = cga3${semicolon} using target = r410${semicolon} static constexpr auto value = conformal_change<3>()${semicolon} }")
basis_change_rejects(change_basis_not_a_change
  "change_basis: ${not_a_change}")
basis_change_rejects(change_basis_back_not_a_change
  "change_basis_back: ${not_a_change}")
# The change's value, as conformal_change<3>(), given where its tag belongs.
set(value_as_tag "the template argument is a value, not a change tag${semicolon} put it in a tag, as in struct cga3_to_r410 { using source = cga3${semicolon} using target = r410${semicolon} static constexpr auto value = conformal_change<3>()${semicolon} }")
basis_change_rejects(change_basis_value_as_tag "change_basis: ${value_as_tag}")
basis_change_rejects(change_basis_back_value_as_tag
  "change_basis_back: ${value_as_tag}")
set(not_in_source "change_basis: x is not in the change's source algebra${semicolon} use change_basis_back to map from the target, or check that the tag's source is x's algebra")
set(not_in_target "change_basis_back: y is not in the change's target algebra${semicolon} use change_basis to map from the source, or check that the tag's target is y's algebra")
foreach(case IN ITEMS target_operand other_algebra other_algebra_auto)
  basis_change_rejects(change_basis_${case} "${not_in_source}")
endforeach()
foreach(case IN ITEMS source_operand other_algebra)
  basis_change_rejects(change_basis_back_${case} "${not_in_target}")
endforeach()

# A scalar that cannot hold one of the change's exact coefficients, float for
# 2^24 + 1; and a scalar that cannot divide exactly, an integer type or a
# ring scalar without division, on a layout whose image needs a division.
set(narrow "the scalar type cannot hold one of the change's exact coefficients${semicolon} use a scalar type that holds larger integers exactly")
basis_change_rejects(change_basis_narrow_scalar "change_basis: ${narrow}")
basis_change_rejects(change_basis_back_narrow_scalar
  "change_basis_back: ${narrow}")
set(division "the change divides by an integer on this layout, which the scalar type cannot do exactly${semicolon} use a rational or floating scalar type")
basis_change_rejects(change_basis_integer_division "change_basis: ${division}")
basis_change_rejects(change_basis_no_division "change_basis: ${division}")
basis_change_rejects(change_basis_back_integer_division
  "change_basis_back: ${division}")

# A change with more generators than a blade mask has bits, written by hand.
basis_change_rejects(basis_change_type_width
  "basis_change: n is larger than the number of bits in a blade mask")

# A blade set written by hand, given to a layout of a change directly, with a
# generator beyond the algebra or a sign other than +1 or -1: the message
# names the layout as written, change_basis_layout or change_basis_back_layout,
# and the algebra the set must be in, the change's source or its target.
set(bad_sign "the blade set holds a sign other than +1 or -1${semicolon} give each blade of a set written out by hand the sign +1 or -1")
set(change_layouts change_basis_layout change_basis_back_layout)
set(change_layout_algebras source target)
foreach(layout algebra IN ZIP_LISTS change_layouts change_layout_algebras)
  basis_change_rejects(${layout}_beyond_n
    "${layout}: the blade set names a generator beyond e_n${semicolon} use a set whose blades are all in the change's ${algebra} algebra")
  basis_change_rejects(${layout}_bad_sign "${layout}: ${bad_sign}")
endforeach()

# core.format: the formatter of a multivector, against names worked out in
# the test from each blade's mask and sign, and texts written out there.
galoot_add_test(MODULE core COMPONENT format)

# core.format_outside_core: core.hpp with <format> gives a multivector no
# formatter. A program of its own, since format_test.cpp sees the
# formatter; it also carries the label core.format.
galoot_add_test(MODULE core COMPONENT format_outside_core)
set_property(TEST core.format_outside_core APPEND PROPERTY LABELS
  core.format)

# core.format_<case>: each misuse of the formatter fails to compile with one
# error, its message, on the line a reader sees first, which names none of
# the machinery of the formatter or of <format>. Each cause is tried as
# return std::format("{}", x) and, for some, as auto s = std::format("{}", x)
# or std::println("{}", x), which reach the formatter by other paths. A
# scalar without a formatter is also tried with a specification, {:.2f},
# which parse must skip so that format gives the one error.
function(format_rejects case message)
  galoot_add_compile_fail_test(NAME format_${case}
    MODULE core COMPONENT format
    SOURCE core/format_${case}_fail.cpp
    MESSAGE "${message}"
    ONE_ERROR FORBID_ON_FIRST_ERROR "detail::" "__format" "basic_format_arg")
endfunction()
set(format_no_formatter "format: the scalar type has no std::formatter${semicolon} give it a std::formatter, as double has")
set(format_wide_algebra "format: names such as e123 have one digit per index, so a multivector prints by name only for n up to 9${semicolon} format its coefficients instead")
set(format_not_a_blade "format: an entry of the algebra tag's names is not a blade of the algebra${semicolon} write 1, or e and distinct indices from 1 to n, the number of generators, as in e41")
set(format_named_twice "format: the algebra tag's names name one blade twice, perhaps in both orientations${semicolon} name each blade once")
foreach(case IN ITEMS scalar_without_formatter scalar_without_formatter_auto
    scalar_without_formatter_print scalar_without_formatter_spec)
  format_rejects(${case} "${format_no_formatter}")
endforeach()
foreach(case IN ITEMS wide_algebra wide_algebra_auto)
  format_rejects(${case} "${format_wide_algebra}")
endforeach()
# A name that is neither 1 nor e and indices, e with no index, a letter or a
# 0 for an index, an index above n, and one generator twice.
foreach(case IN ITEMS names_not_e names_not_e_auto names_bare_e
    names_letter_index names_index_zero names_index_above_n
    names_repeated_generator)
  format_rejects(${case} "${format_not_a_blade}")
endforeach()
# One blade named twice, in one orientation and in both.
foreach(case IN ITEMS names_twice names_twice_opposite)
  format_rejects(${case} "${format_named_twice}")
endforeach()

# rga3.objects: the preset's layouts against the oriented names written in the
# test, its types, and the facts of rigid(3) on them, against hand values and
# the golden values.
galoot_add_test(MODULE rga3 COMPONENT objects DIRECTORY presets/rga3)
target_link_libraries(rga3_objects_test PRIVATE galoot::presets::rga3
  galoot_golden galoot_test_support)

# rga3.format: each type of the preset prints by the names of its layout,
# written out by hand in the test.
galoot_add_test(MODULE rga3 COMPONENT format DIRECTORY presets/rga3)
target_link_libraries(rga3_format_test PRIVATE galoot::presets::rga3)

# rga3.bare_compile: the preset, like the core, works with no build system and
# no options beyond the language edition.
add_test(NAME rga3.bare_compile
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -pedantic-errors
    -I ${PROJECT_SOURCE_DIR}/include -fsyntax-only
    ${CMAKE_CURRENT_SOURCE_DIR}/presets/rga3/bare_user.cpp)

# rga3.include_scan: the preset includes only standard, core, and preset
# headers, with no cycle, guards named from paths, and snake_case names.
add_test(NAME rga3.include_scan
  COMMAND ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/include_scan.py
    ${PROJECT_SOURCE_DIR}/include presets/rga3)
set_tests_properties(rga3.bare_compile rga3.include_scan PROPERTIES
  LABELS "rga3;rga3.headers")

# rga3.readme_example_<n>: the n-th cpp block of README.md, extracted into
# the build tree at configure time, builds with the test flags and runs to
# exit 0, printing what its "// prints:" comments say. rga3.readme_has_example
# reads the README itself and fails when it has no cpp block, so that a lost
# fence cannot pass; a malformed README extracts no example and fails it too.
set(readme ${PROJECT_SOURCE_DIR}/README.md)
set(readme_examples
  ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/readme_examples.py)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${readme}
  ${PROJECT_SOURCE_DIR}/tools/readme_examples.py)
execute_process(
  COMMAND ${readme_examples} extract ${readme} ${CMAKE_CURRENT_BINARY_DIR}/readme
  OUTPUT_VARIABLE readme_sources ERROR_VARIABLE readme_defect
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(readme_defect)
  message(WARNING "README.md: no example extracted: ${readme_defect}")
endif()
string(REPLACE "\n" ";" readme_sources "${readme_sources}")
add_test(NAME rga3.readme_has_example
  COMMAND ${readme_examples} check ${readme})
set(readme_tests rga3.readme_has_example)
foreach(source IN LISTS readme_sources)
  cmake_path(GET source STEM example)
  add_executable(rga3_readme_${example} ${source})
  target_link_libraries(rga3_readme_${example} PRIVATE galoot::presets::rga3)
  # An example's main may let std::format_error from std::println escape, as
  # a user's would; a try block around it would only hide what it shows.
  if(CMAKE_CXX_CLANG_TIDY)
    set_target_properties(rga3_readme_${example} PROPERTIES CXX_CLANG_TIDY
      "${CMAKE_CXX_CLANG_TIDY};--checks=-bugprone-exception-escape")
  endif()
  add_test(NAME rga3.readme_${example} COMMAND ${readme_examples} run
    ${source} $<TARGET_FILE:rga3_readme_${example}>)
  list(APPEND readme_tests rga3.readme_${example})
endforeach()
set_tests_properties(${readme_tests} PROPERTIES LABELS "rga3;rga3.readme")

# tools.readme_examples_*: the README check fails on a README with no cpp
# block, on a C++ block marked c++, and on a fence never closed; a run fails
# when the program exits nonzero or prints other than its "// prints:" lines.
set(readme_fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/readme_examples)
add_test(NAME tools.readme_examples_rejects_no_block
  COMMAND ${expect_output} 1 "no_block.md: no ```cpp block"
    -- ${readme_examples} check ${readme_fixtures}/no_block.md)
add_test(NAME tools.readme_examples_rejects_other_name
  COMMAND ${expect_output} 1 "other_name.md:3: the block is marked c++"
    -- ${readme_examples} check ${readme_fixtures}/other_name.md)
add_test(NAME tools.readme_examples_rejects_unclosed
  COMMAND ${expect_output} 1 "unclosed.md:5: the fence opened here is never"
    -- ${readme_examples} check ${readme_fixtures}/unclosed.md)
add_test(NAME tools.readme_examples_rejects_exit
  COMMAND ${expect_output} 1 "the program exited 1, not 0"
    -- ${readme_examples} run ${readme_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E false)
add_test(NAME tools.readme_examples_rejects_output
  COMMAND ${expect_output} 1 "but its // prints: comments say"
    -- ${readme_examples} run ${readme_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E echo "other text")
add_test(NAME tools.readme_examples_accepts_output
  COMMAND ${expect_output} 0 "first line"
    -- ${readme_examples} run ${readme_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E echo "first line")
set_tests_properties(tools.readme_examples_rejects_no_block
  tools.readme_examples_rejects_other_name
  tools.readme_examples_rejects_unclosed tools.readme_examples_rejects_exit
  tools.readme_examples_rejects_output tools.readme_examples_accepts_output
  PROPERTIES LABELS "tools;tools.readme_examples")

# rga3.motions: translation and rotation against hand values, the
# reference's rotations, and the formula of the rotor written out in the
# test.
galoot_add_test(MODULE rga3 COMPONENT motions DIRECTORY presets/rga3)
target_link_libraries(rga3_motions_test PRIVATE galoot::presets::rga3
  galoot_golden galoot_test_support)

# rga3.point_groups: the 32 crystallographic point groups, generated from
# their rational generators by compose over the test rational, counted, and
# matched one to one with the reference's matrices.
galoot_add_test(MODULE rga3 COMPONENT point_groups DIRECTORY presets/rga3)
target_link_libraries(rga3_point_groups_test PRIVATE galoot::presets::rga3
  galoot_golden galoot_test_support)

# rga3.chains: seeded chains of three to six joint motors, composed, against
# the reference's products of 4 by 4 matrices.
galoot_add_test(MODULE rga3 COMPONENT chains DIRECTORY presets/rga3)
target_link_libraries(rga3_chains_test PRIVATE galoot::presets::rga3
  galoot_golden galoot_test_support)

# rga3.incidence: joins, meets, and incidences of points, lines, and planes
# against the reference's values, exact zeros included.
galoot_add_test(MODULE rga3 COMPONENT incidence DIRECTORY presets/rga3)
target_link_libraries(rga3_incidence_test PRIVATE galoot::presets::rga3
  galoot_golden galoot_test_support)

# Each misuse of rotation and translation is one error with its message, and
# the line a reader sees first names no internal name.
function(motions_rejects case message)
  galoot_add_compile_fail_test(NAME ${case} MODULE rga3 COMPONENT motions
    SOURCE presets/rga3/${case}_fail.cpp MESSAGE "${message}" ONE_ERROR
    ${ARGN} FORBID_ON_FIRST_ERROR "detail::" "apply:")
endfunction()
set(rotation_no_sin "rotation: sin and cos of the angle do not give values of this scalar type, which is not a floating type${semicolon} use Line<double> or Line<float>, or build the Motor from its coefficients, the unitized line times sin(theta/2) and cos(theta/2) on e1234")
set(rotation_not_line "rotation: the axis is not an rga3 Line${semicolon} give a Line<T>, and convert a join such as p ^ q with Line<T>{p ^ q}")
set(rotation_angle_type "rotation: the angle is not of the line's scalar type, the second of the two types shown${semicolon} give it in radians as that type, as std::numbers::pi / 2 over double or std::numbers::pi_v<float> / 2 over float")
set(translation_integer "translation: halving d would truncate in an integer scalar type${semicolon} use a floating or exact rational scalar type")
set(translation_mixed "translation: dx, dy, and dz are not all of one scalar type, the types shown${semicolon} give all three the same type, as translation(1.0, 0.0, 2.0) over double")
# The compilers print the two types of a failed std::is_same_v in their own
# words: the angle's first, or the first two of dx, dy, dz that differ.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(rotation_int_for_double "‘int’ is not the same as ‘double’")
  set(rotation_double_for_float "‘double’ is not the same as ‘float’")
  set(translation_double_and_int "‘double’ is not the same as ‘int’")
else()
  set(rotation_int_for_double "due to requirement 'std::is_same_v<int, double>'")
  set(rotation_double_for_float
    "due to requirement 'std::is_same_v<double, float>'")
  set(translation_double_and_int
    "due to requirement 'std::is_same_v<double, int>'")
endif()
motions_rejects(rotation_integer_scalar "${rotation_no_sin}")
motions_rejects(rotation_rational_scalar "${rotation_no_sin}")
# An axis of the wrong type is named by what it should be, so the first line
# does not spell its type by blade masks.
motions_rejects(rotation_axis_point "${rotation_not_line}"
  FORBID_ON_FIRST_ERROR "blade_set")
motions_rejects(rotation_axis_join "${rotation_not_line}"
  FORBID_ON_FIRST_ERROR "blade_set")
motions_rejects(rotation_angle_int "${rotation_angle_type}"
  REQUIRE "${rotation_int_for_double}")
motions_rejects(rotation_angle_double_for_float "${rotation_angle_type}"
  REQUIRE "${rotation_double_for_float}")
motions_rejects(translation_int "${translation_integer}")
motions_rejects(translation_int64 "${translation_integer}")
motions_rejects(translation_mixed "${translation_mixed}"
  REQUIRE "${translation_double_and_int}")

# rga3.rotation_checking_<case> and rga3.rotation_unchecked_<case>: one
# program built in each checking mode
# (presets/rga3/rotation_assert_fail.cpp). With checking on, an axis whose
# direction has a computed length of zero or infinity aborts with the
# message, over double and float; with checking off the same axes give the
# non-finite or zero coefficients the header states. A direction of length 2
# is accepted in both modes. The programs take the headers directly, so that
# the build preset's mode does not reach them.
set(rotation_direction
  "rotation: the direction of the axis must have a finite nonzero length")
foreach(mode IN ITEMS 1 0)
  add_executable(rga3_rotation_mode_${mode}
    presets/rga3/rotation_assert_fail.cpp)
  target_include_directories(rga3_rotation_mode_${mode} PRIVATE
    ${PROJECT_SOURCE_DIR}/include)
  target_compile_features(rga3_rotation_mode_${mode} PRIVATE cxx_std_23)
  target_compile_definitions(rga3_rotation_mode_${mode} PRIVATE
    GALOOT_CHECKING=${mode})
endforeach()
set(rotation_mode_tests)
foreach(case IN ITEMS zero zero_float underflow overflow)
  add_test(NAME rga3.rotation_checking_${case}
    COMMAND ${expect_output} "Subprocess aborted"
      "galoot: assertion failed: ${rotation_direction} (" motions.hpp:
      -- $<TARGET_FILE:rga3_rotation_mode_1> ${case})
  list(APPEND rotation_mode_tests rga3.rotation_checking_${case})
endforeach()
add_test(NAME rga3.rotation_checking_length_2
  COMMAND rga3_rotation_mode_1 length_2)
foreach(case IN ITEMS zero zero_float underflow overflow length_2)
  add_test(NAME rga3.rotation_unchecked_${case}
    COMMAND rga3_rotation_mode_0 ${case})
  list(APPEND rotation_mode_tests rga3.rotation_unchecked_${case})
endforeach()
set_tests_properties(${rotation_mode_tests} rga3.rotation_checking_length_2
  PROPERTIES LABELS "rga3;rga3.motions")
