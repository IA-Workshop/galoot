# A small project for the tests of tools/mutate: a header with a function
# computed by two routes, two test programs, a compile-fail test on the
# header's catch-all, and a test that always fails. Configured with Ninja in
# one build tree per compiler.
cmake_minimum_required(VERSION 3.28)
project(mutate_fixture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Without module scanning each source compiles in one Ninja edge, so a failed
# build names only objects.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
add_compile_options(-Wall -Werror)
include_directories(${PROJECT_SOURCE_DIR}/include)
enable_testing()

add_executable(check_test test/check_test.cpp)
add_executable(uses_test test/uses_test.cpp)
add_test(NAME check COMMAND check_test)
add_test(NAME uses COMMAND uses_test)

# The compiler runs here, not a program of the build tree, and the test
# passes only if the output holds the catch-all's message.
add_test(NAME catch_all_fail
  COMMAND ${CMAKE_CXX_COMPILER} -std=c++23 -Wall -Werror
    -I${PROJECT_SOURCE_DIR}/include -fsyntax-only
    ${PROJECT_SOURCE_DIR}/test/catch_all_fail.cpp)
set_tests_properties(catch_all_fail PROPERTIES
  PASS_REGULAR_EXPRESSION "twice: the argument must be an int")
set_tests_properties(check uses catch_all_fail PROPERTIES LABELS lib)

add_test(NAME broken COMMAND ${CMAKE_COMMAND} -E false)
set_tests_properties(broken PROPERTIES LABELS broken)
