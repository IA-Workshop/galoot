# Runs a command and checks how it ended and what it wrote.
#
# Usage: cmake -P expect_output.cmake -- <result> [ONLY <regex>] <literal>...
#          [FORBID <literal>...] [FORBID_FIRST <literal>...] -- <command>...
#
# <result> is the exit code the command must return, or the text CMake's
# execute_process gives for a process killed by a signal, such as
# "Subprocess aborted". Each <literal> must appear, as written, in the
# command's combined stdout and stderr, and each literal after FORBID must
# not. With ONLY, every line of the output that matches <regex> must contain
# one of the literals, so that a check expected to report exactly the
# literals fails when it reports something more. Each literal after
# FORBID_FIRST must not appear on the first line that matches the ONLY regex,
# which must be given and must match some line: a compiler's first error line
# is the one a reader sees first, so it must not name internals that later
# lines, such as the notes of an instantiation, may name. The script fails,
# naming what was missing, present, or unexpected, if any check fails.
#
# CTest cannot pass a test whose process aborts, whatever WILL_FAIL or
# PASS_REGULAR_EXPRESSION says, so a test that expects an abort runs through
# this.

# Literals and lines are kept in CMake lists, which split on ; and treat
# [ and ] as brackets, so those three characters are held as the control
# characters 1, 2, and 3 in the output and in every literal while they are
# compared. CMake splits a ; out of an argument before this script sees it,
# so a literal that holds one is passed with character 1 in its place, as
# galoot_expected_findings in test/CMakeLists.txt does.
string(ASCII 1 semicolon)
string(ASCII 2 open_bracket)
string(ASCII 3 close_bracket)
macro(guard variable)
  string(REPLACE ";" "${semicolon}" ${variable} "${${variable}}")
  string(REPLACE "[" "${open_bracket}" ${variable} "${${variable}}")
  string(REPLACE "]" "${close_bracket}" ${variable} "${${variable}}")
endmacro()
macro(unguard variable)
  string(REPLACE "${semicolon}" ";" ${variable} "${${variable}}")
  string(REPLACE "${open_bracket}" "[" ${variable} "${${variable}}")
  string(REPLACE "${close_bracket}" "]" ${variable} "${${variable}}")
endmacro()

# CMAKE_ARGV3 is the "--" that keeps cmake from parsing what follows.
set(expected_result "${CMAKE_ARGV4}")
set(literals "")
set(forbidden "")
set(first_forbidden "")
set(command "")
set(only "")
set(after_separator FALSE)
set(after_forbid FALSE)
set(after_forbid_first FALSE)
set(after_only FALSE)
math(EXPR last "${CMAKE_ARGC} - 1")
foreach(i RANGE 5 ${last})
  set(argument "${CMAKE_ARGV${i}}")
  if(after_separator)
    list(APPEND command "${argument}")
  elseif(after_only)
    set(only "${argument}")
    set(after_only FALSE)
  elseif(argument STREQUAL "--")
    set(after_separator TRUE)
  elseif(argument STREQUAL "ONLY")
    set(after_only TRUE)
  elseif(argument STREQUAL "FORBID")
    set(after_forbid TRUE)
    set(after_forbid_first FALSE)
  elseif(argument STREQUAL "FORBID_FIRST")
    set(after_forbid_first TRUE)
    set(after_forbid FALSE)
  elseif(after_forbid_first)
    guard(argument)
    list(APPEND first_forbidden "${argument}")
  elseif(after_forbid)
    guard(argument)
    list(APPEND forbidden "${argument}")
  else()
    guard(argument)
    list(APPEND literals "${argument}")
  endif()
endforeach()
if(command STREQUAL "")
  message(FATAL_ERROR "usage: cmake -P expect_output.cmake -- <result> [ONLY <regex>] <literal>... [FORBID <literal>...] [FORBID_FIRST <literal>...] -- <command>...")
endif()
if(NOT first_forbidden STREQUAL "" AND only STREQUAL "")
  message(FATAL_ERROR "expect_output: FORBID_FIRST needs ONLY <regex>, which names the line it checks")
endif()

execute_process(COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE output)
message("${output}")
set(text "${output}")
guard(text)

set(failed FALSE)
if(NOT result STREQUAL expected_result)
  message("expect_output: result was '${result}', expected '${expected_result}'")
  set(failed TRUE)
endif()
foreach(literal IN LISTS literals)
  string(FIND "${text}" "${literal}" at)
  if(at EQUAL -1)
    unguard(literal)
    message("expect_output: output lacks '${literal}'")
    set(failed TRUE)
  endif()
endforeach()
foreach(literal IN LISTS forbidden)
  string(FIND "${text}" "${literal}" at)
  if(NOT at EQUAL -1)
    unguard(literal)
    message("expect_output: output contains forbidden '${literal}'")
    set(failed TRUE)
  endif()
endforeach()
if(NOT only STREQUAL "")
  string(REPLACE "\n" ";" lines "${text}")
  foreach(line IN LISTS lines)
    if(NOT line MATCHES "${only}")
      continue()
    endif()
    set(listed FALSE)
    foreach(literal IN LISTS literals)
      string(FIND "${line}" "${literal}" at)
      if(NOT at EQUAL -1)
        set(listed TRUE)
        break()
      endif()
    endforeach()
    if(NOT listed)
      unguard(line)
      message("expect_output: output has an unexpected line: '${line}'")
      set(failed TRUE)
    endif()
  endforeach()
endif()
if(NOT first_forbidden STREQUAL "")
  string(REPLACE "\n" ";" lines "${text}")
  set(first_line "")
  set(found_first FALSE)
  foreach(line IN LISTS lines)
    if(line MATCHES "${only}")
      set(first_line "${line}")
      set(found_first TRUE)
      break()
    endif()
  endforeach()
  if(NOT found_first)
    message("expect_output: no line matches '${only}'")
    set(failed TRUE)
  endif()
  foreach(literal IN LISTS first_forbidden)
    string(FIND "${first_line}" "${literal}" at)
    if(found_first AND NOT at EQUAL -1)
      set(shown "${first_line}")
      unguard(literal)
      unguard(shown)
      message("expect_output: the first line that matches '${only}' contains forbidden '${literal}': '${shown}'")
      set(failed TRUE)
    endif()
  endforeach()
endif()
if(failed)
  message(FATAL_ERROR "expect_output: check failed")
endif()
