# The benchmarks of the rga3 operations and the straight-line reference
# kernels they are timed against, with the smoke tests of those kernels.

# The reference kernels, written by test/reference/reference_kernels.py from
# the definitions, never from the library, so this target does not link
# galoot::core. A program that needs them links it and includes
# <bench/reference_kernels.hpp>; the header also compiles on its own, in
# all_verify_interface_header_sets.
add_library(galoot_reference_kernels INTERFACE)
target_sources(galoot_reference_kernels INTERFACE
  FILE_SET HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/test
    FILES reference_kernels.hpp)
target_compile_features(galoot_reference_kernels INTERFACE cxx_std_23)
set_property(TARGET galoot_reference_kernels
  PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# reference.kernels: the committed reference_kernels.hpp is byte for byte
# what reference_kernels.py writes now, so a hand edit, or a header left
# stale after a change to the reference scripts, fails.
add_test(NAME reference.kernels
  COMMAND ${Python3_EXECUTABLE} -I
    ${PROJECT_SOURCE_DIR}/test/reference/reference_kernels.py
    --check ${CMAKE_CURRENT_SOURCE_DIR}/reference_kernels.hpp)
set_tests_properties(reference.kernels PROPERTIES
  LABELS "reference;reference.kernels")

# rga3.reference_kernels: each reference kernel gives what its rga3
# operation gives on seeded operands, exactly over the test rational, and
# the same values, or for a transform the same bits, over double.
galoot_add_test(MODULE rga3 COMPONENT reference_kernels DIRECTORY .)
target_link_libraries(rga3_reference_kernels_test PRIVATE
  galoot::presets::rga3 galoot_reference_kernels galoot_test_support)

# The harness, which needs the rga3 preset; it too compiles on its own in
# all_verify_interface_header_sets.
add_library(galoot_bench_harness INTERFACE)
target_sources(galoot_bench_harness INTERFACE
  FILE_SET HEADERS BASE_DIRS ${PROJECT_SOURCE_DIR}/test FILES harness.hpp)
target_link_libraries(galoot_bench_harness INTERFACE galoot::presets::rga3)
set_property(TARGET galoot_bench_harness
  PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# bench_<operation>: one program per benchmarked operation, which times the
# rga3 operation and its reference kernel over double (harness.hpp), built
# in every preset so that none goes stale. Each test
# bench.<operation>_<form>_<loop> runs it once. They carry the label bench,
# which the presets' test runs leave out, so they run only by hand or as
# ctest -L '^bench$', one at a time, since each times itself. A measurement
# worth recording runs the program directly, pinned, and repeats it in
# separate processes. Only an operation whose result has its first operand's
# type, a transform or compose, has a latency loop.
set(bench_operations wedge_point_point wedge_point_point_point
  antiwedge_line_plane transform_point transform_line transform_plane
  compose_motors)
set(bench_passes 4096)
set(bench_seed 20261110)
foreach(operation IN LISTS bench_operations)
  add_executable(bench_${operation} ${operation}_bench.cpp)
  target_link_libraries(bench_${operation} PRIVATE galoot_bench_harness
    galoot_reference_kernels)
  set(loops throughput)
  if(operation MATCHES "^(transform|compose)_")
    list(APPEND loops latency)
  endif()
  foreach(form IN ITEMS galoot reference)
    foreach(loop IN LISTS loops)
      set(test bench.${operation}_${form}_${loop})
      add_test(NAME ${test} COMMAND bench_${operation} ${form} ${loop}
        ${bench_passes} ${bench_seed})
      set_tests_properties(${test} PROPERTIES
        LABELS "bench;bench.${operation}" RUN_SERIAL ON)
    endforeach()
  endforeach()

  # rga3.benchmark_<operation>: in every preset, one pass of each loop of the
  # program gives the same checksum in both forms, so the benchmark runs and
  # its two forms compute the same results at the preset's optimization.
  add_test(NAME rga3.benchmark_${operation}
    COMMAND bench_${operation} check 1 ${bench_seed})

  # rga3.benchmark_<operation>_inline: the program defines no function of
  # reference_kernels and neither of its two forms' lambdas, so the
  # reference form runs inline in the timed loop, as straight-line code,
  # and each form calls no wrapper of the program's own; any call left is
  # the library's. In the demangled symbol table a lambda of main is a
  # symbol whose name starts with main::, after its type letter (local t,
  # global T, or weak W); elsewhere main:: shows only inside the template
  # arguments of the harness's functions. The forms must stay non-generic
  # lambdas: a generic one's operator() demangles after its return type,
  # where these patterns would not find it.
  add_test(NAME rga3.benchmark_${operation}_inline
    COMMAND ${expect_output} 0 " T main"
      FORBID "reference_kernels::" " t main::" " T main::" " W main::"
      -- ${CMAKE_NM} -C $<TARGET_FILE:bench_${operation}>)
  set_tests_properties(rga3.benchmark_${operation}
    rga3.benchmark_${operation}_inline PROPERTIES
    LABELS "rga3;rga3.benchmarks")
endforeach()

# rga3.kernels_inline: each function of kernels_inline.cpp, one operation of
# the rga3 preset over double, compiles at -O2 to code that calls or jumps
# to no function of the library (tools/kernels_inline.py states the rule).
# The flags are fixed rather than the build's: those of an optimized build
# without the checking mode, which is the code users build for speed, with
# the warnings of every build; the include directories and the language
# standard are read as galoot_compile_command reads them, the standard being
# the last of its arguments. So the test gives one answer for each compiler
# whatever the preset. It runs the compiler itself, outside the compile-cost
# launcher, since its output is assembly and not an object. The probe is
# also compiled once with the build's own flags, so that every build's
# warnings and the static checks see it.
set(kernels_inline_functions wedge_point_point wedge_point_point_point
  antiwedge_line_plane transform_point transform_line transform_plane
  compose_motors sum_lines reverse_line dot_points change_point)
galoot_compile_command(compile)
list(GET compile -1 kernels_inline_standard)
add_test(NAME rga3.kernels_inline
  COMMAND ${Python3_EXECUTABLE} -I
    ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py
    ${kernels_inline_functions}
    -- ${CMAKE_CXX_COMPILER}
      $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>
      ${kernels_inline_standard} -DGALOOT_CHECKING=0 -O2 -ffp-contract=off
      -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
      -S -o - ${CMAKE_CURRENT_SOURCE_DIR}/kernels_inline.cpp
  COMMAND_EXPAND_LISTS)
set_tests_properties(rga3.kernels_inline PROPERTIES
  LABELS "rga3;rga3.kernels_inline")
add_library(rga3_kernels_inline_probe OBJECT kernels_inline.cpp)
target_link_libraries(rga3_kernels_inline_probe PRIVATE galoot::presets::rga3)

# tools.kernels_inline_*: the check passes on assembly whose only jumps are
# local and whose calls are to other symbols than the library's, and fails,
# naming the line, on a call, a tail jump, a call through the PLT or the
# global offset table, a call to a const member function, and a call to an
# entity local to a function of the library; it fails, naming the function,
# when a function is not defined. Each fixture defines first and second.
set(kernels_inline ${Python3_EXECUTABLE} -I
  ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py first second --)
set(kernels_inline_fixtures ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline)
add_test(NAME tools.kernels_inline_accepts
  COMMAND ${expect_output} 0 "2 functions defined, none calls into the library"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/accepts.s)
add_test(NAME tools.kernels_inline_rejects_call
  COMMAND ${expect_output} 1 "line 4, in _Z5firstRKd: callq"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/call.s)
add_test(NAME tools.kernels_inline_rejects_jmp
  COMMAND ${expect_output} 1 "line 6, in _Z6secondv: jmp"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/jmp.s)
add_test(NAME tools.kernels_inline_rejects_plt
  COMMAND ${expect_output} 1 "_ZN6galoot4core5applyIdEEvv@PLT"
    "*_ZN6galoot4core3sumIdEEvv@GOTPCREL(%rip)"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/plt.s)
add_test(NAME tools.kernels_inline_rejects_const_member
  COMMAND ${expect_output} 1 "_ZNK6galoot4core2mvIdE4sizeEv"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/const_member.s)
add_test(NAME tools.kernels_inline_rejects_local_entity
  COMMAND ${expect_output} 1 "_ZZN6galoot4core3fooEvENKUlvE_clEv"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/local_entity.s)
add_test(NAME tools.kernels_inline_rejects_missing_function
  COMMAND ${expect_output} 1 "not defined in the output: second"
    FORBID "not defined in the output: first"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/missing.s)
set_tests_properties(tools.kernels_inline_accepts
  tools.kernels_inline_rejects_call tools.kernels_inline_rejects_jmp
  tools.kernels_inline_rejects_plt tools.kernels_inline_rejects_const_member
  tools.kernels_inline_rejects_local_entity
  tools.kernels_inline_rejects_missing_function
  PROPERTIES LABELS "tools;tools.kernels_inline")
