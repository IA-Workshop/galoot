# The mutants of plan 0041

Run by the tooling part's implementer (Claude, agent, Opus 5.5, build-expert role) with `tools/mutate` on gcc-debug and clang-debug, in the worktree of `feature/documentation-lane--tooling`. Every build and test went through `tools/timed`; the driver's rows are `mutant:*` in `records/performance/runs.tsv`, and the whole run is the row `mutate - tooling.toml`.

| File | What it is |
| --- | --- |
| `tooling.toml`, `out/tooling-gcc-debug.tsv`, `out/tooling-clang-debug.tsv` | The ten mutants of the tooling part: a `throw` in the core, the transform, and the formatter against the bare compiles without exceptions, and seven edits of the new modes of `tools/doc_examples.py` against its fixture tests |
| `pages.toml`, `out/pages-gcc-debug.tsv`, `out/pages-clang-debug.tsv` | The four mutants of the pages part, which edit the pages and their registration (run by the pages part's author, Claude, agent, Opus 5.5, hci-expert role, in the worktree of `feature/documentation-lane--pages`) |
| `claim-trace.md` | The pages' factual claims that no example shows, each with the header comment or convention it is taken from |

## Results

- **`tooling.toml` at `20cb4d6`: 10 killed on each preset, exit 0** (16.7 s for both presets). Each mutant is killed by every test its spec names as a killer. The selections build nothing.
- After the run, the comment above `throw_in_transform` in the spec was corrected; the edits and the expectations are unchanged. It had said that Clang rejects the `throw` in the template's definition, so that the core bare user would kill it too. The run shows otherwise: on both compilers only `rga3.bare_compile_no_exceptions` kills it, because the `throw` sits inside an `if constexpr` on the template's parameters, which neither compiler checks until it instantiates `transform`.

- **`pages.toml` at `f44b332`: 4 killed on each preset, exit 0** (19.6 s for both presets, the row `mutate - pages.toml`). Each mutant is killed by the one test written for it, and by no other test of its selection: `page_unregistered` by `tests.docs_pages`, `prints_wrong` by `rga3.start_example_2`, `rejected_message_changed` by `rga3.start_rejected_1`, and `cmake_block_broken` by `rga3.readme_cmake_1`. Each selection names targets, so the driver's build re-ran CMake on the mutated page or registration before the tests ran.

- **Both specs rerun against the final pages at `6ca81ad`** (the feature branch after the blind review's fixes and a merge of `develop`), by the merge-gate runner (Claude, agent, Opus 5.5), on the gcc-debug and clang-debug trees of that revision's P9 matrix: `tooling.toml` 10 killed on each preset (16.7 s, the row `mutate - tooling.toml`), `pages.toml` 5 killed on each preset (40.1 s, the row `mutate - pages.toml`), both exit 0, each mutant killed by the tests its spec names. `out/*.tsv` hold these rows and replace the earlier ones.
- **`rejected_ignored` against the pages**, added to `pages.toml` at `6ca81ad` with a selection of every program and every `core.docs` and `rga3.docs` test: killed on both presets as the plan expects. Each of the seven `cpp rejected` blocks, read as a program, stops its own build at the static assertion its `// rejected:` line quotes (first page programs 2, 3 and 8; core guide 4, 11 and 17; limits page 2), and its rejected test is no longer registered, so the spec names no test as a killer.

What the bare-compile mutants show, on both compilers:

| Mutant | Killed by |
| --- | --- |
| `throw_in_assert` (`detail::assertion_failed`, not a template) | `core.bare_compile_no_exceptions`, `rga3.bare_compile_no_exceptions`, `core.bare_compile_format_no_exceptions` |
| `throw_in_transform` (the checking branch of `transform`) | `rga3.bare_compile_no_exceptions` |
| `throw_in_format` (the formatter's `write`) | `core.bare_compile_format_no_exceptions` |

## Departures from the plan's table of mutants

- **`throw_in_format` is killed, where the plan expected it to survive.** The plan's open item on `format.hpp` was ruled in favor of a third bare compile that prints one value, `test/docs/bare_format.cpp`, so the mutant now has a test written for it.
- **Six mutants were added** for the tool's new modes (`rejected_literal_unchecked`, `rejected_literal_at_least_one`, `cmake_not_extracted`, `cmake_extracted_with_programs`, `pages_registration_ignored`, `pages_agent_read`), so that each rejection the fixture tests show is seen to fail. `rejected_ignored` is the plan's mutant, killed here by the fixture tests; the plan also expects it to fail the pages' rejected tests, which do not exist yet.
- **Four mutants of the plan waited for the pages**, because what they edit did not exist at the tooling part's revision: `page_unregistered` (no `docs/guide/rga3.md`), `prints_wrong` (no first page), `rejected_message_changed` (no `cpp rejected` block in any page), and `cmake_block_broken` (no `cmake` block in the README). The pages part ran them from `pages.toml`, above.
- **`rejected_ignored` against the pages' rejected tests** was not run by the pages part, whose brief listed only the four mutants above; the merge-gate runner ran it at `6ca81ad` (Results, above). It is killed by the blocks' failed builds, not by a named test, since the mutant unregisters the rejected tests.

## Notes for the pages' mutants

- Each of the four edits a file that CMake reads at configure time: a page, whose blocks are extracted then and whose `// rejected:` messages are written into the test commands, or `test/documentation.cmake`. The selection must therefore name at least one target, such as one of the page's example programs, so that the driver's Ninja build re-runs CMake before the tests run; a selection with no targets would test the old configuration.
- `tests.docs_pages` and `tests.citations_guide` fail at this revision, `tests.docs_pages` because `docs/galoot-design-study.md` is not registered and `tests.citations_guide` because `docs/guide/` does not exist, so no selection can include them until the study moves and the first page lands: the driver refuses a selection whose baseline fails.
