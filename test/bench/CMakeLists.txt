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

# rga3.reference_kernels_unforced: the same test built with the forced-inline
# mode 0, so that the operations are shown to give the same values, and the
# same bits for a transform, without the attribute. A build whose mode is
# already 0 passes the same definition through galoot::core, and an identical
# second definition is no error.
add_executable(rga3_reference_kernels_unforced_test reference_kernels_test.cpp)
target_link_libraries(rga3_reference_kernels_unforced_test PRIVATE
  galoot::presets::rga3 galoot_reference_kernels galoot_test_support
  galoot_doctest_main)
target_compile_definitions(rga3_reference_kernels_unforced_test PRIVATE
  GALOOT_FORCE_INLINE=0)
add_test(NAME rga3.reference_kernels_unforced
  COMMAND rga3_reference_kernels_unforced_test)
set_tests_properties(rga3.reference_kernels_unforced PROPERTIES
  LABELS "rga3;rga3.reference_kernels"
  FAIL_REGULAR_EXPRESSION "${GALOOT_ZERO_CASES_REGEX}")

# rga3.reference_kernels_forced: the same test with no forced-inline mode
# given, so that the header's default forces the operations inline in every
# preset, those that turn the forcing off included. It takes the core's
# headers directly instead of linking galoot::core, whose build interface
# carries the build's mode, so the forced code a user gets by default also
# runs under the sanitizers and at -O2 on Clang.
add_executable(rga3_reference_kernels_forced_test reference_kernels_test.cpp)
target_include_directories(rga3_reference_kernels_forced_test PRIVATE
  ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(rga3_reference_kernels_forced_test PRIVATE
  GALOOT_CHECKING=$<BOOL:${GALOOT_CHECKING}>)
target_link_libraries(rga3_reference_kernels_forced_test PRIVATE
  galoot_reference_kernels galoot_test_support galoot_doctest_main)
add_test(NAME rga3.reference_kernels_forced
  COMMAND rga3_reference_kernels_forced_test)
set_tests_properties(rga3.reference_kernels_forced PROPERTIES
  LABELS "rga3;rga3.reference_kernels"
  FAIL_REGULAR_EXPRESSION "${GALOOT_ZERO_CASES_REGEX}")

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
# launcher, since its output is assembly and not an object. The script reads
# x86-64 assembly, so the two tests exist only where the target is x86-64; on
# another target they are not registered, rather than fail on assembly they
# cannot read. The guard reads the host's CMAKE_SYSTEM_PROCESSOR, so an
# x86-64 host that cross-builds for another target without a toolchain file
# that sets it registers the tests, and the script then passes that target's
# assembly unread. The probe is also compiled once with the build's own flags,
# so that every build's warnings and the static checks see it.
set(kernels_inline_functions wedge_point_point wedge_point_point_point
  antiwedge_line_plane transform_point transform_line transform_plane
  compose_motors sum_lines reverse_line dot_points change_point
  geometric_product_motors times_motors geometric_product_full times_full
  wedge_full caret_full antiproduct_motors antiproduct_motor_flector
  percent_motor_flector antiproduct_full percent_full antiwedge_full
  ampersand_full reverse_full tilde_full antireverse_full
  grade_involution_full right_complement_full bang_full left_complement_full
  plus_full minus_full bulk_full weight_full bulk_dual_full weight_dual_full
  left_bulk_dual_full left_weight_dual_full dot_full antidot_full
  project_point_plane antiproject_plane_point project_full antiproject_full
  project_motors project_motor_flector change_full change_back_full
  convert_motor truncate_full part_full)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
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
  # rga3.kernels_inline_alone: the same functions, each compiled in a unit of
  # its own, the probe's other text and that one function, as a user's small
  # file would hold one operation. A compiler weighs the growth of a unit
  # against its size, so an operation that inlines among many others can stay
  # a call when it is alone.
  add_test(NAME rga3.kernels_inline_alone
    COMMAND ${Python3_EXECUTABLE} -I
      ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py
      --alone ${CMAKE_CURRENT_SOURCE_DIR}/kernels_inline.cpp
      ${kernels_inline_functions}
      -- ${CMAKE_CXX_COMPILER}
        $<LIST:TRANSFORM,$<TARGET_PROPERTY:galoot::core,INTERFACE_INCLUDE_DIRECTORIES>,PREPEND,-I>
        ${kernels_inline_standard} -DGALOOT_CHECKING=0 -O2 -ffp-contract=off
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror -S -o -
    COMMAND_EXPAND_LISTS)
  set_tests_properties(rga3.kernels_inline rga3.kernels_inline_alone PROPERTIES
    LABELS "rga3;rga3.kernels_inline")
endif()
add_library(rga3_kernels_inline_probe OBJECT kernels_inline.cpp)
target_link_libraries(rga3_kernels_inline_probe PRIVATE galoot::presets::rga3)

# tools.kernels_inline_*: the check passes on assembly whose only jumps are
# local and whose calls are to other symbols than the library's, and fails,
# naming the line, on a call, a tail jump, a conditional tail jump, a call
# through the PLT or the global offset table, a call or a notrack jump
# through a register loaded with a library function's address (by lea, or
# by mov of an immediate, into the register or a narrower part of it), a
# call to a const member function, to an explicit-object member function,
# and to an entity local to a function of the library; it passes on
# functions that are a return alone, built with or without -fcf-protection,
# and on a register overwritten before the call through it; and it fails,
# naming the function, when a function is not defined. Each fixture defines
# first and second. With --alone, it compiles each named function of a
# source in a unit of its own, with the source's other text, and fails on a
# call in any one of them or on a name the source does not define or
# defines in more than one paragraph; the stand-in compiler writes, for
# each function in the unit it is given, a definition that calls the
# library when the function is second or when the unit holds more than one
# function, so that a unit that is not alone fails.
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
add_test(NAME tools.kernels_inline_rejects_conditional_jump
  COMMAND ${expect_output} 1 "line 6, in _Z6secondv: jne"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/conditional.s)
add_test(NAME tools.kernels_inline_rejects_register_call
  COMMAND ${expect_output} 1 "line 6, in _Z5firstRKd: call"
    "through %rax, which holds _ZN6galoot4core3fooEv"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/register.s)
add_test(NAME tools.kernels_inline_rejects_explicit_object
  COMMAND ${expect_output} 1 "_ZNH6galoot4core3fooIdEEvRKNS0_2mvIdEE"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/explicit_object.s)
add_test(NAME tools.kernels_inline_accepts_bare_return
  COMMAND ${expect_output} 0 "2 functions defined, none calls into the library"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/bare_return.s)
add_test(NAME tools.kernels_inline_accepts_cet
  COMMAND ${expect_output} 0 "2 functions defined, none calls into the library"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/cet.s)
add_test(NAME tools.kernels_inline_rejects_immediate
  COMMAND ${expect_output} 1 "through %rax, which holds _ZN6galoot4core3fooEv"
    "through %rdx, which holds _ZN6galoot4core3barEv"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/immediate.s)
add_test(NAME tools.kernels_inline_rejects_notrack
  COMMAND ${expect_output} 1 "line 6, in _Z5firstRKd: notrack jmp"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/notrack.s)
add_test(NAME tools.kernels_inline_accepts_overwritten_register
  COMMAND ${expect_output} 0 "2 functions defined, none calls into the library"
    -- ${kernels_inline} ${CMAKE_COMMAND} -E cat ${kernels_inline_fixtures}/overwritten.s)
set(kernels_inline_alone ${Python3_EXECUTABLE} -I
  ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py
  --alone ${kernels_inline_fixtures}/alone.cpp)
set(kernels_inline_stand_in ${Python3_EXECUTABLE} -I
  ${kernels_inline_fixtures}/stand_in_compiler.py)
add_test(NAME tools.kernels_inline_alone_accepts
  COMMAND ${expect_output} 0 "2 functions compiled alone, none calls into the library"
    -- ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py
      --alone ${kernels_inline_fixtures}/alone_inline.cpp first third
      -- ${kernels_inline_stand_in})
add_test(NAME tools.kernels_inline_alone_rejects_call
  COMMAND ${expect_output} 1 "compiled alone: a call into the library: line"
    "in _Z6secondv"
    FORBID "in _Z5firstv"
    -- ${kernels_inline_alone} first second -- ${kernels_inline_stand_in})
add_test(NAME tools.kernels_inline_alone_rejects_missing_function
  COMMAND ${expect_output} 1 "the source defines no function third"
    -- ${kernels_inline_alone} first third -- ${kernels_inline_stand_in})
add_test(NAME tools.kernels_inline_alone_rejects_twice
  COMMAND ${expect_output} 1 "the source defines first in 2 paragraphs"
    -- ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/test/tools/kernels_inline.py
      --alone ${kernels_inline_fixtures}/alone_twice.cpp first
      -- ${kernels_inline_stand_in})
set_tests_properties(tools.kernels_inline_accepts
  tools.kernels_inline_accepts_bare_return tools.kernels_inline_accepts_cet
  tools.kernels_inline_rejects_immediate
  tools.kernels_inline_rejects_notrack
  tools.kernels_inline_accepts_overwritten_register
  tools.kernels_inline_alone_rejects_twice
  tools.kernels_inline_rejects_conditional_jump
  tools.kernels_inline_rejects_register_call
  tools.kernels_inline_rejects_explicit_object
  tools.kernels_inline_alone_accepts tools.kernels_inline_alone_rejects_call
  tools.kernels_inline_alone_rejects_missing_function
  tools.kernels_inline_rejects_call tools.kernels_inline_rejects_jmp
  tools.kernels_inline_rejects_plt tools.kernels_inline_rejects_const_member
  tools.kernels_inline_rejects_local_entity
  tools.kernels_inline_rejects_missing_function
  PROPERTIES LABELS "tools;tools.kernels_inline")
