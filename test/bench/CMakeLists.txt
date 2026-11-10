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
  set_tests_properties(rga3.benchmark_${operation} PROPERTIES
    LABELS "rga3;rga3.benchmarks")
endforeach()
