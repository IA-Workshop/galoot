# Configures Galoot's own project in a scratch build tree with the options
# given with -D, for the tests tools.configure_rejects_*. SOURCE is the
# project's source tree, SCRATCH the build tree (removed before and after),
# COMPILER the C++ compiler, CHECKING and FORCE_INLINE the values of
# GALOOT_CHECKING and GALOOT_FORCE_INLINE; FORCE_INLINE may be left out.
# Prints what CMake prints and fails if the configuration fails.
set(options -DGALOOT_CHECKING=${CHECKING})
if(DEFINED FORCE_INLINE)
  list(APPEND options -DGALOOT_FORCE_INLINE=${FORCE_INLINE})
endif()
file(REMOVE_RECURSE "${SCRATCH}")
execute_process(
  COMMAND ${CMAKE_COMMAND} -S ${SOURCE} -B ${SCRATCH} -G Ninja
    -DCMAKE_CXX_COMPILER=${COMPILER} ${options}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE output)
file(REMOVE_RECURSE "${SCRATCH}")
message("${output}")
if(NOT result EQUAL 0)
  message(FATAL_ERROR "configure_check: the configuration failed")
endif()
