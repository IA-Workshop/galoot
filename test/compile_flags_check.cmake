# Checks that a compile command has the compiler and arguments that CMake
# generated for a reference source, apart from the source and the object.
#
# Usage: cmake -P compile_flags_check.cmake -- <compile_commands.json>
#          <reference source> -- <compiler> <argument>...
#
# The reference's command is read from the build's compile_commands.json,
# which CMake writes when CMAKE_EXPORT_COMPILE_COMMANDS is on. -o and -c are
# dropped with their values, and so is -x c++, which CMake adds for a source
# whose LANGUAGE property is set, as it is for the units that check headers.
# The script fails, printing both commands, if they differ in any argument or
# in the order of the arguments.

# CMAKE_ARGV3 is the "--" that keeps cmake from parsing what follows.
set(database "${CMAKE_ARGV4}")
set(reference "${CMAKE_ARGV5}")
if(NOT CMAKE_ARGV6 STREQUAL "--" OR CMAKE_ARGC LESS 8)
  message(FATAL_ERROR "usage: cmake -P compile_flags_check.cmake -- <compile_commands.json> <reference source> -- <compiler> <argument>...")
endif()
set(command "")
math(EXPR last "${CMAKE_ARGC} - 1")
foreach(i RANGE 7 ${last})
  list(APPEND command "${CMAKE_ARGV${i}}")
endforeach()

if(NOT EXISTS "${database}")
  message(FATAL_ERROR "${database} does not exist; configure with "
    "CMAKE_EXPORT_COMPILE_COMMANDS ON, as every preset does")
endif()
file(READ "${database}" json)
string(JSON entries LENGTH "${json}")
set(generated "")
if(entries GREATER 0)
  math(EXPR last "${entries} - 1")
  foreach(i RANGE ${last})
    string(JSON file GET "${json}" ${i} file)
    if(file STREQUAL reference)
      string(JSON generated GET "${json}" ${i} command)
      break()
    endif()
  endforeach()
endif()
if(generated STREQUAL "")
  message(FATAL_ERROR "${database} has no command for ${reference}")
endif()

separate_arguments(words UNIX_COMMAND "${generated}")
set(expected "")
set(skip_next FALSE)
foreach(word IN LISTS words)
  if(skip_next)
    set(skip_next FALSE)
  elseif(word STREQUAL "-o" OR word STREQUAL "-c" OR word STREQUAL "-x")
    set(skip_next TRUE)
  else()
    list(APPEND expected "${word}")
  endif()
endforeach()

if(NOT command STREQUAL expected)
  list(JOIN expected " " expected_text)
  list(JOIN command " " command_text)
  message("compile_flags_check: CMake compiles ${reference} with\n  ${expected_text}\nbut the command given is\n  ${command_text}")
  message(FATAL_ERROR "compile_flags_check: the commands differ")
endif()
message("compile_flags_check: the command matches the one for ${reference}")
