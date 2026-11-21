# Galoot's build. The core is headers alone, usable without any build
# system; this file builds Galoot's own tests and checks, and declares their
# options rather than leaving them to defaults, so that every result can be
# traced to a setting someone chose.
cmake_minimum_required(VERSION 4.4)
project(galoot VERSION 1.0.0 LANGUAGES CXX)

# Compiler extensions off, so the code stays standard C++23. Module scanning
# off: Galoot ships headers, not modules, and the scan would otherwise run for
# every C++23 source.
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# The core. Its interface carries no warning, -Werror, or sanitizer flags:
# those belong to Galoot's own builds, not to its users'.
add_library(galoot_core INTERFACE)
add_library(galoot::core ALIAS galoot_core)
target_sources(galoot_core INTERFACE
  FILE_SET HEADERS BASE_DIRS include FILES
    include/galoot/core.hpp
    include/galoot/core/assert.hpp
    include/galoot/core/basis_change.hpp
    include/galoot/core/bits.hpp
    include/galoot/core/blade_product.hpp
    include/galoot/core/blade_set.hpp
    include/galoot/core/dual.hpp
    include/galoot/core/format.hpp
    include/galoot/core/linear.hpp
    include/galoot/core/metric.hpp
    include/galoot/core/mv.hpp
    include/galoot/core/plan.hpp
    include/galoot/core/product.hpp
    include/galoot/core/transform.hpp
    include/galoot/core/unary.hpp)
target_compile_features(galoot_core INTERFACE cxx_std_23)

# The rga3 preset, the rigid geometric algebra of three dimensions: an
# algebra preset, a named algebra built on the core. It is headers alone,
# written on the core's public headers, and like the core its interface
# carries no flags of Galoot's own builds.
add_library(galoot_presets_rga3 INTERFACE)
add_library(galoot::presets::rga3 ALIAS galoot_presets_rga3)
target_sources(galoot_presets_rga3 INTERFACE
  FILE_SET HEADERS BASE_DIRS include FILES
    include/galoot/presets/rga3.hpp
    include/galoot/presets/rga3/motions.hpp
    include/galoot/presets/rga3/objects.hpp)
target_link_libraries(galoot_presets_rga3 INTERFACE galoot_core)

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Every header under include/galoot belongs to a header set, the rga3
# preset's for galoot/presets/rga3.hpp and the headers under
# galoot/presets/rga3/, and the core's for the rest. The header checks
# compile each set's headers one by one and the format check reads both
# sets, so a header left out is never checked on its own. Stop configuring
# if one is missing or in the other set; CONFIGURE_DEPENDS reconfigures when
# a header is added.
file(GLOB_RECURSE galoot_headers_on_disk CONFIGURE_DEPENDS
  RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include/galoot/*.hpp)
foreach(header IN LISTS galoot_headers_on_disk)
  if(header MATCHES "^include/galoot/presets/rga3(\\.hpp$|/)")
    set(galoot_header_target galoot_presets_rga3)
  else()
    set(galoot_header_target galoot_core)
  endif()
  get_target_property(galoot_header_set ${galoot_header_target} HEADER_SET)
  if(NOT ${PROJECT_SOURCE_DIR}/${header} IN_LIST galoot_header_set)
    message(FATAL_ERROR "${header} is not in ${galoot_header_target}'s "
      "header set; add it to the FILES of "
      "target_sources(${galoot_header_target} ...)")
  endif()
endforeach()

# The checking mode changes what the tests check, so it is never a default:
# every build preset declares it.
if(NOT DEFINED GALOOT_CHECKING)
  message(FATAL_ERROR
    "GALOOT_CHECKING is not set. Configure with a build preset "
    "(cmake --list-presets) or pass -DGALOOT_CHECKING=ON or OFF.")
endif()
if(NOT GALOOT_CHECKING MATCHES "^(ON|OFF)$")
  message(FATAL_ERROR "GALOOT_CHECKING must be ON or OFF, not '${GALOOT_CHECKING}'.")
endif()
set(GALOOT_CHECKING "${GALOOT_CHECKING}" CACHE BOOL
  "ON checks GALOOT_ASSERT conditions at run time; OFF checks them only in constant evaluation")
# Every target that links galoot::core in this build gets the mode, including
# the header-check targets of VERIFY_INTERFACE_HEADER_SETS. BUILD_INTERFACE
# keeps it out of anything installed. A target that fixes its own mode does
# not link galoot::core (test/CMakeLists.txt), because a second definition
# with another value is an error under -Werror.
target_compile_definitions(galoot_core INTERFACE
  $<BUILD_INTERFACE:GALOOT_CHECKING=$<BOOL:${GALOOT_CHECKING}>>)

# The forced-inline mode. ON leaves the header's default, which forces the
# operations inline; OFF passes GALOOT_FORCE_INLINE=0 to every target that
# links galoot::core in this build, for the build presets whose units with
# many operations cost far more to compile when forced. Unlike the checking
# mode it changes no value an operation computes, so it has a default, and
# the build presets declare it anyway, so that each one's mode reads in
# CMakePresets.json. Only ON and OFF are accepted, as for the checking mode;
# a test checks that the mode reaches the code. ON passes nothing, so that a
# target may fix the mode 0 for itself in any build; a second, identical
# definition is no error. The structure tests in test/bench fix their own
# flags and do not see this option.
option(GALOOT_FORCE_INLINE
  "ON forces the core's operations inline; OFF leaves inlining to the compiler"
  ON)
if(NOT GALOOT_FORCE_INLINE MATCHES "^(ON|OFF)$")
  message(FATAL_ERROR
    "GALOOT_FORCE_INLINE must be ON or OFF, not '${GALOOT_FORCE_INLINE}'.")
endif()
target_compile_definitions(galoot_core INTERFACE
  $<BUILD_INTERFACE:$<$<NOT:$<BOOL:${GALOOT_FORCE_INLINE}>>:GALOOT_FORCE_INLINE=0>>)

# Per-translation-unit cost record: each compile runs through tools/measure,
# which appends a row naming GALOOT_PRESET to GALOOT_COMPILE_RECORD. Every
# build preset but lint sets GALOOT_PRESET, since a compile under clang-tidy
# does not cost what a user's compile does. The record is the build tree's
# own compile.tsv by default, so a tree configured with -B beside a build
# preset's own tree keeps its rows apart, and the build writes nothing
# outside its source and build trees. A record elsewhere, from -D or from an
# older cache, gets a warning at every configure. The launcher applies to
# every compile in the tree, the header-check units included.
set(GALOOT_PRESET "" CACHE STRING
  "Build preset named in each compile cost row; empty records no compile costs")
set(GALOOT_COMPILE_RECORD "${CMAKE_BINARY_DIR}/compile.tsv" CACHE FILEPATH
  "compile.tsv that each compile appends its cost row to, with GALOOT_PRESET")
if(NOT GALOOT_PRESET STREQUAL "")
  if(GALOOT_COMPILE_RECORD STREQUAL "")
    message(FATAL_ERROR "GALOOT_PRESET is '${GALOOT_PRESET}', so each compile "
      "records its cost, but GALOOT_COMPILE_RECORD names no file for the rows.")
  endif()
  cmake_path(IS_PREFIX CMAKE_BINARY_DIR "${GALOOT_COMPILE_RECORD}" NORMALIZE
    record_in_tree)
  if(NOT record_in_tree)
    message(WARNING "GALOOT_COMPILE_RECORD is ${GALOOT_COMPILE_RECORD}, "
      "outside this build tree, so every compile here appends its cost row "
      "to that file. Remove the entry from CMakeCache.txt to use the tree's "
      "own compile.tsv.")
  endif()
  set(CMAKE_CXX_COMPILER_LAUNCHER
    ${PROJECT_SOURCE_DIR}/tools/measure tu ${GALOOT_PRESET}
    ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}
    ${GALOOT_COMPILE_RECORD} --)
endif()

# Each header compiles on its own, so it includes what it uses: one generated
# translation unit per header, built by all_verify_interface_header_sets and
# not by all.
set_property(TARGET galoot_core galoot_presets_rga3
  PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Layout check over the headers of the core and the rga3 preset and the test
# sources: clang-format with the repository's .clang-format must leave them
# unchanged. It does not compile. include/ext keeps its upstream layout.
find_program(GALOOT_CLANG_FORMAT clang-format REQUIRED)
# CONFIGURE_DEPENDS, so that a test source added later is checked without a
# manual reconfigure.
file(GLOB_RECURSE galoot_test_sources CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.hpp)
add_custom_target(format-check
  COMMAND ${GALOOT_CLANG_FORMAT} --dry-run -Werror
    $<TARGET_PROPERTY:galoot_core,HEADER_SET>
    $<TARGET_PROPERTY:galoot_presets_rga3,HEADER_SET> ${galoot_test_sources}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND_EXPAND_LISTS
  VERBATIM)

enable_testing()
add_subdirectory(test)
