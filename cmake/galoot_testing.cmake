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

# galoot_compile_command(<out>)
#
# Sets <out> to the compiler and the arguments that a target linking
# galoot::core is compiled with, without the source and the object: the
# definitions, include directories, CMAKE_CXX_FLAGS and the flags of the build
# type, and the language standard, in the order CMake writes them. They are
# read from galoot::core and the build's flag variables
# rather than written out, so that a compile run outside the build sees what
# the library's tests see. tests.compile_fail_flags compares the result with
# the command CMake generates for galoot::core's header checks.
function(galoot_compile_command out)
  # CMake turns a compile feature into a flag only for a target it compiles,
  # so the standard is read from galoot::core's features and spelled with
  # the flag CMake would use for it.
  get_target_property(features galoot::core INTERFACE_COMPILE_FEATURES)
  list(FILTER features INCLUDE REGEX "^cxx_std_[0-9]+$")
  list(LENGTH features count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "galoot::core must name one C++ standard as a compile "
      "feature, not '${features}'")
  endif()
  string(REPLACE "cxx_std_" "" standard "${features}")
  if(CMAKE_CXX_EXTENSIONS)
    set(standard_flag ${CMAKE_CXX${standard}_EXTENSION_COMPILE_OPTION})
  else()
    set(standard_flag ${CMAKE_CXX${standard}_STANDARD_COMPILE_OPTION})
  endif()

  string(TOUPPER "${CMAKE_BUILD_TYPE}" config)
  separate_arguments(flags UNIX_COMMAND
    "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${config}}")
  # The caller passes COMMAND_EXPAND_LISTS to add_test, which turns each
  # property's list into separate arguments. galoot::core sets no compile
  # options, so none are read; a property with no value would become an
  # empty argument.
  set(${out} ${CMAKE_CXX_COMPILER}
    $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_COMPILE_DEFINITIONS>,PREPEND,-D>
    $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>
    ${flags} ${standard_flag}
    PARENT_SCOPE)
endfunction()

# galoot_add_compile_fail_test(NAME <name> MODULE <module>
#   COMPONENT <component> SOURCE <file> MESSAGE <literal>
#   [REQUIRE <literal>...] [FORBID <literal>...])
#
# Registers the test <module>.<name>, which compiles <file> and passes only if
# the compile fails, its output contains <literal> and every REQUIRE literal,
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
# The test runs the compiler itself, with galoot_compile_command's arguments
# and through the compile-cost launcher when the build has one, so it builds
# nothing in the build tree, takes no lock, and runs in parallel with the
# others. It compiles with -c, as the build does, rather than -fsyntax-only:
# some diagnostics come only from code generation, among them GCC's "control
# reaches end of non-void function" and a call to a function declared with
# the error attribute, and with -fsyntax-only a source that fails only that
# way would compile. Each test names its own object, so that no two write one
# file; the compiler writes it only if the source compiles, which fails the
# test.
function(galoot_add_compile_fail_test)
  cmake_parse_arguments(PARSE_ARGV 0 arg ""
    "NAME;MODULE;COMPONENT;SOURCE;MESSAGE" "REQUIRE;FORBID")
  if(arg_MESSAGE MATCHES ";")
    message(FATAL_ERROR "galoot_add_compile_fail_test(${arg_NAME}): MESSAGE "
      "may not contain ';': ${arg_MESSAGE}")
  endif()

  set(forbid "")
  if(arg_FORBID)
    set(forbid FORBID ${arg_FORBID})
  endif()
  galoot_compile_command(compile)
  cmake_path(ABSOLUTE_PATH arg_SOURCE OUTPUT_VARIABLE source)
  add_test(NAME ${arg_MODULE}.${arg_NAME}
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/test/expect_output.cmake
      -- 1 ${arg_MESSAGE} ${arg_REQUIRE} ${forbid}
      -- ${CMAKE_CXX_COMPILER_LAUNCHER} ${compile}
        -o ${arg_MODULE}_${arg_NAME}_compile_fail.o -c ${source}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_EXPAND_LISTS)
  set_tests_properties(${arg_MODULE}.${arg_NAME} PROPERTIES
    LABELS "${arg_MODULE};${arg_MODULE}.${arg_COMPONENT}")
endfunction()
