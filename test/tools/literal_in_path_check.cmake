# Runs galoot_check_literals_outside_path on the SOURCE and LITERAL given
# with -D, for the tests tools.compile_fail_literal_in_path_*.
include(${CMAKE_CURRENT_LIST_DIR}/../../cmake/galoot_testing.cmake)
galoot_check_literals_outside_path(fixture "${SOURCE}" "${LITERAL}")
message("the literal is outside the path")
