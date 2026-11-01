# Galoot's build. The core is headers alone, usable without any build
# system; this file builds Galoot's own tests and checks, and declares their
# options rather than leaving them to defaults, so that every result can be
# traced to a setting someone chose.
cmake_minimum_required(VERSION 4.4)
project(galoot VERSION 0.1.0 LANGUAGES CXX)

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
    include/galoot/core/bits.hpp
    include/galoot/core/metric.hpp)
target_compile_features(galoot_core INTERFACE cxx_std_23)

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# The checking mode changes what the tests check, so it is never a default:
# every preset declares it.
if(NOT DEFINED GALOOT_CHECKING)
  message(FATAL_ERROR
    "GALOOT_CHECKING is not set. Configure with a preset "
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

# Per-translation-unit cost record: each compile runs through tools/measure,
# which appends a row naming GALOOT_PRESET to GALOOT_COMPILE_RECORD. The
# presets that measure set both, and the lint preset neither, since a compile
# under clang-tidy does not cost what a user's compile does. The launcher
# applies to every compile in the tree, the header-check units included.
set(GALOOT_PRESET "" CACHE STRING
  "Preset named in each compile cost row; empty records no compile costs")
set(GALOOT_COMPILE_RECORD "" CACHE FILEPATH
  "compile.tsv that each compile appends its cost row to, with GALOOT_PRESET")
if(NOT GALOOT_PRESET STREQUAL "")
  if(GALOOT_COMPILE_RECORD STREQUAL "")
    message(FATAL_ERROR "GALOOT_PRESET is '${GALOOT_PRESET}', so each compile "
      "records its cost, but GALOOT_COMPILE_RECORD names no file for the rows.")
  endif()
  set(CMAKE_CXX_COMPILER_LAUNCHER
    ${PROJECT_SOURCE_DIR}/tools/measure tu ${GALOOT_PRESET}
    ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}
    ${GALOOT_COMPILE_RECORD} --)
elseif(NOT GALOOT_COMPILE_RECORD STREQUAL "")
  message(FATAL_ERROR "GALOOT_COMPILE_RECORD is set, but without "
    "GALOOT_PRESET no compile records its cost there.")
endif()

# Each header compiles on its own, so it includes what it uses: one generated
# translation unit per header, built by all_verify_interface_header_sets and
# not by all.
set_property(TARGET galoot_core PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Layout check over the core's headers and the test sources: clang-format
# with the repository's .clang-format must leave them unchanged. It does not
# compile. include/ext keeps its upstream layout.
find_program(GALOOT_CLANG_FORMAT clang-format REQUIRED)
# CONFIGURE_DEPENDS, so that a test source added later is checked without a
# manual reconfigure.
file(GLOB_RECURSE galoot_test_sources CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.hpp)
add_custom_target(format-check
  COMMAND ${GALOOT_CLANG_FORMAT} --dry-run -Werror
    $<TARGET_PROPERTY:galoot_core,HEADER_SET> ${galoot_test_sources}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND_EXPAND_LISTS
  VERBATIM)

enable_testing()
add_subdirectory(test)
