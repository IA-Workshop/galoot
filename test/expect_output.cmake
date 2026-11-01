# Runs a command and checks how it ended and what it wrote. Snippet candidate.
#
# Usage: cmake -P expect_output.cmake -- <result> <literal>...
#          [FORBID <literal>...] -- <command>...
#
# <result> is the exit code the command must return, or the text CMake's
# execute_process gives for a process killed by a signal, such as
# "Subprocess aborted". Each <literal> must appear, as written, in the
# command's combined stdout and stderr, and each literal after FORBID must
# not. The script fails, naming what was missing or present, if any check
# fails.
#
# CTest cannot pass a test whose process aborts, whatever WILL_FAIL says
# (plan 0002, departure 4), so a test that expects an abort runs through this.

# CMAKE_ARGV3 is the "--" that keeps cmake from parsing what follows.
set(expected_result "${CMAKE_ARGV4}")
set(literals "")
set(forbidden "")
set(command "")
set(after_separator FALSE)
set(after_forbid FALSE)
math(EXPR last "${CMAKE_ARGC} - 1")
foreach(i RANGE 5 ${last})
  if(after_separator)
    list(APPEND command "${CMAKE_ARGV${i}}")
  elseif(CMAKE_ARGV${i} STREQUAL "--")
    set(after_separator TRUE)
  elseif(CMAKE_ARGV${i} STREQUAL "FORBID")
    set(after_forbid TRUE)
  elseif(after_forbid)
    list(APPEND forbidden "${CMAKE_ARGV${i}}")
  else()
    list(APPEND literals "${CMAKE_ARGV${i}}")
  endif()
endforeach()
if(command STREQUAL "")
  message(FATAL_ERROR "usage: cmake -P expect_output.cmake -- <result> <literal>... [FORBID <literal>...] -- <command>...")
endif()

execute_process(COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE output)
message("${output}")

set(failed FALSE)
if(NOT result STREQUAL expected_result)
  message("expect_output: result was '${result}', expected '${expected_result}'")
  set(failed TRUE)
endif()
foreach(literal IN LISTS literals)
  string(FIND "${output}" "${literal}" at)
  if(at EQUAL -1)
    message("expect_output: output lacks '${literal}'")
    set(failed TRUE)
  endif()
endforeach()
foreach(literal IN LISTS forbidden)
  string(FIND "${output}" "${literal}" at)
  if(NOT at EQUAL -1)
    message("expect_output: output contains forbidden '${literal}'")
    set(failed TRUE)
  endif()
endforeach()
if(failed)
  message(FATAL_ERROR "expect_output: check failed")
endif()
