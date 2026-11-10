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
