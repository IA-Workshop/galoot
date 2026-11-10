# The benchmarks of the rga3 operations and the straight-line reference
# kernels they are timed against, with the smoke tests of those kernels.

# rga3.reference_kernels: each reference kernel gives what its rga3
# operation gives on seeded operands, exactly over the test rational, and
# the same values, or for a transform the same bits, over double.
galoot_add_test(MODULE rga3 COMPONENT reference_kernels DIRECTORY .)
target_link_libraries(rga3_reference_kernels_test PRIVATE
  galoot::presets::rga3 galoot_reference_kernels galoot_test_support)
