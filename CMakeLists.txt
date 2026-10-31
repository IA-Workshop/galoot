# Galoot's build (plan 0002). The core is headers alone (constitution C13);
# this file builds Galoot's own tests and checks, and declares their options
# rather than leaving them to defaults (C8).
cmake_minimum_required(VERSION 4.4)
project(galoot VERSION 0.1.0 LANGUAGES CXX)

# Extensions off (STYLE S2); module scanning off until a decision adopts
# modules (research note 0002, F11).
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Per-translation-unit cost record (PERFORMANCE P1; plan 0002, 4.7). The
# presets that measure set GALOOT_PRESET and GALOOT_COMPILE_RECORD; the lint
# preset sets neither. Every compile in the tree gets it, including the
# header-check translation units (checked with ninja -t commands).
if(PROJECT_IS_TOP_LEVEL AND DEFINED GALOOT_PRESET)
  set(CMAKE_CXX_COMPILER_LAUNCHER
    ${PROJECT_SOURCE_DIR}/tools/measure tu ${GALOOT_PRESET}
    ${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}
    ${GALOOT_COMPILE_RECORD} --)
endif()

# The core. Its interface carries no warning, -Werror, or sanitizer flags:
# those belong to Galoot's own builds, not to its users'.
add_library(galoot_core INTERFACE)
add_library(galoot::core ALIAS galoot_core)
target_sources(galoot_core INTERFACE
  FILE_SET HEADERS BASE_DIRS include FILES
    include/galoot/core.hpp
    include/galoot/core/assert.hpp
    include/galoot/core/metric.hpp)
target_compile_features(galoot_core INTERFACE cxx_std_23)

if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# The checking mode is declared by every preset (plan 0001, DoD-20).
if(NOT DEFINED GALOOT_CHECKING)
  message(FATAL_ERROR
    "GALOOT_CHECKING is not set. Configure with a preset "
    "(cmake --list-presets) or pass -DGALOOT_CHECKING=ON or OFF.")
endif()
if(NOT GALOOT_CHECKING MATCHES "^(ON|OFF)$")
  message(FATAL_ERROR "GALOOT_CHECKING must be ON or OFF, not '${GALOOT_CHECKING}'.")
endif()
# Every target that links galoot::core in this build gets the mode, including
# the header-check targets of VERIFY_INTERFACE_HEADER_SETS. BUILD_INTERFACE
# keeps it out of anything installed. A target that fixes its own mode does
# not link galoot::core (test/CMakeLists.txt), because a second definition
# with another value is an error under -Werror.
target_compile_definitions(galoot_core INTERFACE
  $<BUILD_INTERFACE:GALOOT_CHECKING=$<BOOL:${GALOOT_CHECKING}>>)

# Each header compiles on its own (STYLE S6): one generated translation unit
# per header, built by all_verify_interface_header_sets and not by all.
set_property(TARGET galoot_core PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)

# Layout check (STYLE S3) over the core's headers and the test sources,
# without compiling. include/ext keeps its upstream layout.
find_program(GALOOT_CLANG_FORMAT clang-format REQUIRED)
file(GLOB_RECURSE galoot_test_sources
  ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.hpp)
add_custom_target(format-check
  COMMAND ${GALOOT_CLANG_FORMAT} --dry-run -Werror
    $<TARGET_PROPERTY:galoot_core,HEADER_SET> ${galoot_test_sources}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMAND_EXPAND_LISTS
  VERBATIM)

enable_testing()
add_subdirectory(test)
