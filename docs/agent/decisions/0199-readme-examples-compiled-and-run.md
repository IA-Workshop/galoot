# 0199 Every C++ example of the README is extracted, compiled, and run as a test

- Status: recorded. The owner ruled for a short README compiled by a CTest (decision 0196 item 6, plan 0023 O9); the mechanism below is the implementer's, as plan 0023 R8 recommended, open to the blind review and the owner (C18).
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU16
- Bears on: plan 0023 R8, O9, criterion 10; WORKFLOW W12 ("every code example in user documentation is compiled and run as a test"); DoD-27

## Context

`README.md` was empty at `88c9484`. W12 asks that every code example in user documentation be compiled and run. R8 weighed two mechanisms: extract each fenced block into the build tree, or keep each example as a test source with a check that the README's block equals it byte for byte. The README and its check are `b6d8d04` and `27ba27a`; `3d8f5d8` adds `README.md` to `tests.citations`.

## Decision

1. **Extraction.** `tools/readme_examples.py extract` writes each fenced block whose info string is `cpp` to `<build>/test/readme/example_<n>.cpp` at configure time, with a `#line` directive so that a compiler error names the README's line. A file whose text has not changed is not rewritten, so the build does not recompile it; an example beyond the last is removed. The README and the script are configure dependencies, so an edit reconfigures.
2. **One CTest per block, on every preset.** `rga3.readme_example_<n>` builds the block as an executable linked to `galoot::rga3` with the test flags of the preset (so both compilers, the release, sanitizer, measure, and lint presets build it), runs it, and passes when it exits 0 and, if the block holds `// prints: <text>` lines, its output is exactly those texts in order. Under the lint preset the examples are not checked by `bugprone-exception-escape`: a user's `main` may let `std::format_error` from `std::println` escape, and a try block would hide what the example shows.
3. **A lost fence fails.** `rga3.readme_has_example` fails when the README has no `cpp` block, has a fence that is never closed, or marks C++ with another info string (`c++`, `cxx`), which a reader would see as C++ but the extraction would skip. A malformed README extracts nothing and warns at configure time. `tools.readme_examples_*` show each rejection on fixtures in `test/tools/readme_examples/`.
4. **Blocks that are not programs** use another info string (`text`), as the README's include lines and CMake lines do; they are not extracted.

## Options weighed

- The example kept as a test source with a byte-for-byte check against the README (R8's alternative): no generator, but the text lives twice and an edit must be made twice.
- A Markdown test framework or a third-party extraction tool: a dependency for one file (C12).
- The extraction chosen: one script of the project's own, with its rejections tested; the cost is a configure-time step and one executable per block.

## Consequences

- The README's two examples build and pass on gcc-debug and clang-debug (`rga3.readme_example_1` and `_2`), and in every other preset when its suite runs.
- An example that prints a value checks it through its `// prints:` lines, so a wrong number in the text fails the test.
- The check reads only `README.md`. Other user documentation under `docs/`, such as the design study, is not extracted; whether its code blocks fall under W12 is not decided here, and the wider user guide stays with DoD-27 in M8.
