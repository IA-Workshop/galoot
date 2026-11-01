# 0081 A citation check over whole files, sharing one pattern file with the marker check

- Status: accepted
- Date: 2026-10-31
- Decided by: Claude (agent, Opus 5.5), in cleanup unit 1 (plan 0007, items 1 and 3, and review finding D4 of the M0 scripts delta review), carrying out the owner's ruling of decision 0034 as amended. The owner may reverse it (C18). Supersedes item 4 of decision 0072 (the pointer pattern); the rest of 0072 stands.

## Context

Decision 0034, as amended, and STYLE S11 as rewritten forbid human-facing files to cite governance provisions or agent work of any sort, and ask for a mechanical check. Decision 0072 item 4 gave the marker check a pointer pattern, and the delta review (D4) listed forms it still passed: "the research note on complements", "§3.1 of Conventions.md", "review finding B1", "decision 34", and bare W5, C20, and P9. Plan 0007 item 3 asks for a CTest over `include/`, `test/`, `cmake/`, `CMakeLists.txt`, `CMakePresets.json`, `tools/`, and generated files, with a rejection test and an accepting case.

## Options weighed

Where the patterns live:

1. In `tools/test_references.py`, as now. The script would then match itself (its own pattern source names the documents in capitals and the agent models), so it would have to be excluded whole, and its docstring and comments would go unchecked.
2. A separate script for the citation check. Two copies of the pattern, which drift.
3. A data file, `tools/citation_patterns.txt`, one commented regular expression per line, read by both the marker check and the new `citations` command. Only that file is excluded, and it holds nothing but patterns. Chosen.

What a whole-file check matches against:

1. Each line alone. Misses a citation broken across two comment lines ("decision" at the end of one, "0022" on the next), which the tree had.
2. The file's text with each line's indentation and comment leader (`//`, `///`, `#`, `/*`, `*`) removed and the lines joined by spaces. Chosen. A spurious join of code across lines is possible and has not occurred.
3. Comments and string literals only, per language. Five languages are in scope (C++, Python, CMake, JSON, shell), and test names and CMake test names are code; rejected as complexity without a present need.

Bare rule numbers: C, S, and V numbers name mathematics (C3, S2, V4), so a bare one passes, as 0072 decided, but a parenthesized list of them, such as "(C13, S6)", is a citation. Bare W and P numbers are rejected, as the review proposed: W5 or P9 is not mathematics in this library, and a point named P1 in a later test would be a lowercase variable or would need a rename. A test of that trade-off: widening the bare rule to C, S, and V fails the accepting fixtures and `markers_good.cpp`.

Paths under `docs/agent`: rejected, except the run log and its directory (`docs/agent/records/performance` and `.../runs.tsv`), which `tools/timed` and `tools/measure` write to and are their interface. The committed compile record is not excepted (decision 0080 took it out of the build).

Expected findings that quote citations: the marker, trace, and citation rejection tests list their expected findings in files beside the fixtures (`*.expected`), read by `test/CMakeLists.txt`, so that the build file holds no citation text and the check needs no exclusion for it. The literals are the same as before. The trace tool's own messages ("item 1.2 is implemented ...") match the "item N.N" pattern, so the trace expectations moved too.

## Decision

- `tools/test_references.py citations [--exclude <path>]... <path>...` reports each match of the pattern file, as `<file>:<line>: cites a project document or record instead of stating the reason: '<match>'`, and exits 1 on any finding, on a file that is not UTF-8, or when no file is found.
- The patterns, in order: a document name with a rule number or "kind N"; the document names in capitals; their `.md` file names in any case; "conventions specification"; "section N.N" and "item N.N"; decision, plan, review, or lesson with any number; "note" with four digits; "research note"; "lessons learned"; spike A to F and `spike-<letters>`; workbench; `docs/agent` outside the run log; WU numbers; DoD-N; milestone gates; "finding" with a letter and number; "departure N"; bare W and P numbers; parenthesized lists of C, S, and V numbers; agent model names; the three agent names with hyphens and "auditor"; "snippet candidate".
- `tests.citations` (labels `tests;tests.citations`) runs it over `include/galoot`, `cmake`, `test`, `tools`, `CMakeLists.txt`, `CMakePresets.json`, `.clang-format`, and `.clang-tidy`, excluding the pattern file, `markers_bad.cpp`, the three `.expected` files, and `citations_bad/`. `include/ext` is vendored and not ours, so it is not scanned. `tools.test_references_citations_rejects` and `..._accepts` run it on fixtures.
- `.clang-format` and `.clang-tidy` are scanned although the plan's list does not name them: they are the style configuration that the lint preset reads, and they cited STYLE.md. `.gitignore` and `.gitattributes` are not scanned: they must name paths under `docs/agent` to work, and the plan does not list them.

## Evidence

Before the cleanup, the command on the tree reported 106 findings, all genuine except the trace tool's own "item N.N" messages in `test/CMakeLists.txt` and the new docstring's own wording, which were handled as above. After it, `tests.citations` passes. Removing any one of the 23 pattern lines fails a rejection test (23 of 23); so does searching lines one at a time (the split citation is missed) and dropping the findings. Widening bare rule numbers to C, S, and V, or dropping the run-log exception, fails the accepting tests. Runs are in `runs.tsv` under the selections `mutation:no-pattern-line-<n>`, `mutation:lines-not-joined`, `mutation:no-citation-findings`, `mutation:bare-c-s-v-tokens`, and `mutation:no-run-log-exception`.

## Consequences

- A published source cited by section number must not say "section 3.1", which the pattern takes for a conventions section; write "§3.1" or "chapter 3" after the source.
- A new file type in a scanned directory is checked as text; a binary file there fails the check as not UTF-8.
- Not caught: a bare C, S, or V number outside parentheses ("where S12 asks"), and prose that paraphrases a record without a name or number. Review checks these.
