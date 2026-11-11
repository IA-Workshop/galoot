# 0242 Every example of the user documentation is a test: programs, rejections, and CMake projects

- Status: recorded. The owner ruled for the tested kinds `cpp`, `cpp rejected`, and the README's `cmake` blocks, for the tool's new name and the labels `<module>.docs` (decision 0241 item 5, settling decision 0229 item 2); the mechanism below is the documentation lane's (plan 0041 section 5.1), open to its blind review and the owner (C18).
- Date: 2026-11-10
- Recorded by: Claude (agent, Opus 5.5), author of the `--pages` part of plan 0041, describing the mechanism the `--tooling` part built (`feature/documentation-lane--tooling`, merged into `feature/documentation-lane` at `818eb4c`) and the pages registered with it.
- Supersedes: decision 0199 items 1, 2, and 4. Item 3 (a lost fence fails) stands, under the new names.
- Bears on: WORKFLOW W12 ("every code example in user documentation is compiled and run as a test"); DoD-27; plan 0041 section 1 item 4 and O7, O8.

## Context

Decision 0199 compiled and ran the `cpp` blocks of `README.md` alone, with `tools/readme_examples.py` and the tests `rga3.readme_example_<n>`, and left the rest of `docs/` undecided. Plan 0041 adds four pages under `docs/guide/` (the first page `README.md`, `core.md`, `rga3.md`, `limits.md`), shows rejections that Galoot reports with its own messages, and puts CMake projects in the README. W12 asks that every code example be compiled and run; DoD-27 names every fenced C++ example.

## Options weighed

- **The kinds tested.** (a) `cpp` programs, `cpp rejected` blocks, and the README's `cmake` blocks, each tested; (b) `cpp` only, with rejections in prose and CMake as `text`, DoD-27's letter. (a) costs a tool mode and one compile-fail test per rejected block, and catches drift in the messages and the CMake text that a user meets first; (b) leaves W12 unmet for CMake. The owner chose (a).
- **The tool's name and labels.** (a) `tools/doc_examples.py` and labels `<module>;<module>.docs`; (b) keep `readme_examples.py` and `rga3.readme`. The owner chose (a).
- **One registration per page, or a scan of `docs/`.** A scan would compile a page nobody meant as documentation and could not say which module or target a page needs. One registration per page states both, and a check that every page is registered keeps a new page from going untested. Chosen: registration plus the check.

## Decision

1. **The tool.** `tools/doc_examples.py` replaces `tools/readme_examples.py`, with its fixtures in `test/tools/doc_examples/`. `extract <page> <out>` writes the n-th `cpp` block to `example_<n>.cpp` and the n-th `cpp rejected` block to `rejected_<n>.cpp`, each with a `#line` directive naming the page; `extract --info cmake` writes the n-th `cmake` block to `example_<n>.cmake`. A file whose text has not changed is not rewritten, and a file beyond the last block is removed. `check <page>` fails on a page with no `cpp` or `cpp rejected` block, on a fence never closed, on C++ marked with another info string (`c++`, `cxx`), and on a `cpp rejected` block without exactly one line `// rejected: <literal>`. `run <example> <program>` passes when the program exits 0 and prints exactly its `// prints:` lines in order. `pages <docs-dir> <registered>...` fails, naming each, on a Markdown file under `docs/` outside `docs/agent/` that no test registers.
2. **One registration per page.** `test/documentation.cmake`, included once from `test/CMakeLists.txt`, defines `galoot_add_doc_examples(PAGE <md> MODULE <module> NAME <stem> LINK <target>)` and registers five pages: `README.md` (module `rga3`, stem `readme`, linking `galoot::presets::rga3`), `docs/guide/README.md` (`rga3`, `start`), `docs/guide/core.md` (`core`, `core_guide`, linking `galoot::core` alone, so the core guide cannot depend on the preset), `docs/guide/rga3.md` (`rga3`, `rga3_guide`), and `docs/guide/limits.md` (`core`, `limits`). Every test of a page is labelled `<module>` and `<module>.docs`.
3. **The tests of a page**, on every preset:
   - `<module>.<stem>_has_example`: `check` on the page (decision 0199 item 3, renamed).
   - `<module>.<stem>_example_<n>`: the n-th `cpp` block is a whole program, built with the preset's test flags and linked to the page's target, then run by `run`. Under the lint preset, `bugprone-exception-escape` is off for these programs, as decision 0199 item 2 said.
   - `<module>.<stem>_rejected_<n>`: the n-th `cpp rejected` block is a whole translation unit that must fail to compile with one error holding its literal, through `galoot_add_compile_fail_test` with `ONE_ERROR`, as the misuse catalogue's tests do. So the block must raise no other error: the preset's `-Werror` makes a warning an error, and GCC's `-Wunused-function` fires on a function whose parameter types have internal linkage, so a rejected block calls its function from `main`. The literal must not contain `;`, which CMake would split, so it is the part of a message on one side of a `;`, and it must name one message only, since `tests.messages_pinned` requires a test that names a part of a longer message to forbid the longer one.
   - `<module>.<stem>_cmake_<n>`: the n-th `cmake` block is written as the `CMakeLists.txt` of a project beside `test/docs/consumer/main.cpp` and a link named `galoot` to the source tree, configured with the outer build's generator and compiler, `-DFETCHCONTENT_SOURCE_DIR_GALOOT=<source>` and `-DFETCHCONTENT_FULLY_DISCONNECTED=ON`, built, and its one `add_executable` program run. It shows that the block is a working project that names real targets; it cannot check a repository URL or a tag, and the README's `FetchContent` block carries a marked placeholder for both until the release fills them (decision 0241 item 4).
4. **Pages under test.** `tests.docs_pages` runs `pages` over `docs/` with the registered pages, so a page added without a registration fails. `tests.citations_guide` runs the citation check of `tests.citations` over `docs/guide/`.
5. **Blocks that are not tested** carry another info string, `text`: include lines, command lines, compiler output. A fragment of C++ is not fenced; it is written inline. This replaces decision 0199 item 4 and is the reading of plan 0041 section 1 item 4.

## Consequences

- At `feature/documentation-lane--pages`, the five pages give 54 tests on each preset: 41 programs, 6 rejected blocks, 2 CMake projects, and the 5 page checks, besides `tests.docs_pages` and `tests.citations_guide`; all pass on gcc-debug and clang-debug.
- An example's expected values are derived in its text or comments and checked by its `// prints:` lines or `static_assert`s; a wrong value fails its test.
- A Galoot message that a page quotes is pinned by that page's rejected test as well as by the misuse catalogue, so rewording it fails both.
- Each registered page adds its programs to every preset's build: 41 translation units, whose cost the build rows of the run log record.
- The design study, which decision 0199 left outside the check, no longer lives under `docs/`; it is the research note `0000-design-study.md`, so `tests.docs_pages` does not ask for it.
