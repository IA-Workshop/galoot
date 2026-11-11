# The examples of the user documentation, compiled and run as tests, so that
# a page cannot drift from the library it describes. Included once from
# test/CMakeLists.txt, which defines expect_output.

set(doc_examples
  ${Python3_EXECUTABLE} -I ${PROJECT_SOURCE_DIR}/tools/doc_examples.py)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/tools/doc_examples.py)

# galoot_add_doc_examples(PAGE <md> MODULE <module> NAME <stem>
#   LINK <target>)
#
# Registers the examples of one Markdown page, each test labelled <module>
# and <module>.docs:
# - <module>.<stem>_example_<n>: the n-th cpp block, extracted into the build
#   tree at configure time, builds with the test flags as a program linked to
#   <target> and runs to exit 0, printing what its "// prints:" comments say.
# - <module>.<stem>_has_example reads the page itself and fails when it has
#   no cpp block, so that a lost fence cannot pass; a malformed page
#   extracts no example and fails it too.
function(galoot_add_doc_examples)
  cmake_parse_arguments(PARSE_ARGV 0 arg "" "PAGE;MODULE;NAME;LINK" "")
  set(page ${PROJECT_SOURCE_DIR}/${arg_PAGE})
  set(out ${CMAKE_CURRENT_BINARY_DIR}/docs/${arg_NAME})
  set(prefix ${arg_MODULE}.${arg_NAME})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${page})

  execute_process(
    COMMAND ${doc_examples} extract ${page} ${out}
    OUTPUT_VARIABLE sources ERROR_VARIABLE defect
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(defect)
    message(WARNING "${arg_PAGE}: no example extracted: ${defect}")
  endif()
  string(REPLACE "\n" ";" sources "${sources}")

  add_test(NAME ${prefix}_has_example COMMAND ${doc_examples} check ${page})
  set(tests ${prefix}_has_example)
  foreach(source IN LISTS sources)
    cmake_path(GET source STEM example)
    set(program ${arg_MODULE}_${arg_NAME}_${example})
    add_executable(${program} ${source})
    target_link_libraries(${program} PRIVATE ${arg_LINK})
    # An example's main may let std::format_error from std::println escape,
    # as a user's would; a try block around it would only hide what it
    # shows.
    if(CMAKE_CXX_CLANG_TIDY)
      set_target_properties(${program} PROPERTIES CXX_CLANG_TIDY
        "${CMAKE_CXX_CLANG_TIDY};--checks=-bugprone-exception-escape")
    endif()
    add_test(NAME ${prefix}_${example} COMMAND ${doc_examples} run
      ${source} $<TARGET_FILE:${program}>)
    list(APPEND tests ${prefix}_${example})
  endforeach()
  set_tests_properties(${tests} PROPERTIES
    LABELS "${arg_MODULE};${arg_MODULE}.docs")
endfunction()

galoot_add_doc_examples(PAGE README.md MODULE rga3 NAME readme
  LINK galoot::presets::rga3)

# tools.doc_examples_*: the page check fails on a page with no cpp block, on
# a C++ block marked c++, and on a fence never closed; a run fails when the
# program exits nonzero or prints other than its "// prints:" lines.
set(doc_fixtures ${CMAKE_CURRENT_SOURCE_DIR}/tools/doc_examples)
add_test(NAME tools.doc_examples_rejects_no_block
  COMMAND ${expect_output} 1 "no_block.md: no ```cpp block"
    -- ${doc_examples} check ${doc_fixtures}/no_block.md)
add_test(NAME tools.doc_examples_rejects_other_name
  COMMAND ${expect_output} 1 "other_name.md:3: the block is marked c++"
    -- ${doc_examples} check ${doc_fixtures}/other_name.md)
add_test(NAME tools.doc_examples_rejects_unclosed
  COMMAND ${expect_output} 1 "unclosed.md:5: the fence opened here is never"
    -- ${doc_examples} check ${doc_fixtures}/unclosed.md)
add_test(NAME tools.doc_examples_rejects_exit
  COMMAND ${expect_output} 1 "the program exited 1, not 0"
    -- ${doc_examples} run ${doc_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E false)
add_test(NAME tools.doc_examples_rejects_output
  COMMAND ${expect_output} 1 "but its // prints: comments say"
    -- ${doc_examples} run ${doc_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E echo "other text")
add_test(NAME tools.doc_examples_accepts_output
  COMMAND ${expect_output} 0 "first line"
    -- ${doc_examples} run ${doc_fixtures}/prints.txt
      ${CMAKE_COMMAND} -E echo "first line")

# tools.doc_examples_*_kinds: a cpp block is extracted as a program and a
# cpp rejected block as a rejection, numbered apart, and neither as CMake;
# with --info cmake only the cmake block is extracted. The check fails on a
# rejected block without exactly one "// rejected:" line, which names the
# message its compile must fail with.
set(doc_fixtures_out ${CMAKE_CURRENT_BINARY_DIR}/tools/doc_examples)
add_test(NAME tools.doc_examples_accepts_kinds
  COMMAND ${expect_output} 0
    "kinds.md: 1 ```cpp block(s) and 1 ```cpp rejected block(s)"
    -- ${doc_examples} check ${doc_fixtures}/kinds.md)
add_test(NAME tools.doc_examples_extracts_kinds
  COMMAND ${expect_output} 0 "/cpp/example_1.cpp" "/cpp/rejected_1.cpp"
    FORBID "example_2" "rejected_2" ".cmake"
    -- ${doc_examples} extract ${doc_fixtures}/kinds.md ${doc_fixtures_out}/cpp)
add_test(NAME tools.doc_examples_extracts_cmake
  COMMAND ${expect_output} 0 "/cmake/example_1.cmake"
    FORBID "example_2" ".cpp"
    -- ${doc_examples} extract --info cmake ${doc_fixtures}/kinds.md
      ${doc_fixtures_out}/cmake)
add_test(NAME tools.doc_examples_rejects_no_literal
  COMMAND ${expect_output} 1
    "rejected_no_literal.md:3: a cpp rejected block needs one line" "has 0"
    -- ${doc_examples} check ${doc_fixtures}/rejected_no_literal.md)
add_test(NAME tools.doc_examples_rejects_two_literals
  COMMAND ${expect_output} 1
    "rejected_two_literals.md:3: a cpp rejected block needs one line" "has 2"
    -- ${doc_examples} check ${doc_fixtures}/rejected_two_literals.md)

# tools.doc_examples_*_registered: every Markdown page under the documentation
# directory, outside its agent/ directory, must be registered; a page left
# out is named, and the agent notes and files that are not Markdown are not.
set(doc_fixture_pages ${doc_fixtures}/pages)
add_test(NAME tools.doc_examples_rejects_unregistered
  COMMAND ${expect_output} 1 "guide/extra.md: no test registers this page"
    FORBID "note.md" "figure.txt" "start.md"
    -- ${doc_examples} pages ${doc_fixture_pages}
      ${doc_fixture_pages}/guide/start.md)
add_test(NAME tools.doc_examples_accepts_registered
  COMMAND ${expect_output} 0 "2 pages, each registered"
    -- ${doc_examples} pages ${doc_fixture_pages}
      ${doc_fixture_pages}/guide/start.md ${doc_fixture_pages}/guide/extra.md)

set_tests_properties(tools.doc_examples_rejects_no_block
  tools.doc_examples_rejects_other_name
  tools.doc_examples_rejects_unclosed tools.doc_examples_rejects_exit
  tools.doc_examples_rejects_output tools.doc_examples_accepts_output
  tools.doc_examples_accepts_kinds tools.doc_examples_extracts_kinds
  tools.doc_examples_extracts_cmake tools.doc_examples_rejects_no_literal
  tools.doc_examples_rejects_two_literals
  tools.doc_examples_rejects_unregistered
  tools.doc_examples_accepts_registered
  PROPERTIES LABELS "tools;tools.doc_examples")
