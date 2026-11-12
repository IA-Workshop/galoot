# Blind review of the 1.0.0 release commit `7580e88`

- Reviewer: blind-reviewer W5 (Claude Sonnet 5.5), 2026-11-12. Commit `7580e88` on `feature/release-1.0.0` (parent chain from `7dbb516`), read with `git show`; no build.
- Departures and ambiguities (C20): a grep for the old wording also printed lines of decision 0248, decision 0245, plan 0041 and plan 0042 (the grep scope covered `docs/agent`); I saw their lines about the placeholders, the tag spelling (`1.0.0`) and the rga3 version (0.1.0), but did not read plan 0042's conclusions or the implementer's report beyond those lines. I did not run the tests or a build. I did not check that the tag `1.0.0` exists (it is the owner's, per W-release).

## What I checked

- Diff: `CMakeLists.txt` `VERSION 0.1.0` to `1.0.0`; README status paragraph and FetchContent block. Nothing else.
- Other carriers of the version or old wording, over README, `docs/guide`, `cmake`, `CMakePresets.json`, `include`, `tools`, `test`: none. `0.1.0` survives only in historical records (`docs/agent/research-notes/0002-cmake.md`, `plans/0002-...`), which are records and correct as history. No config-version file, version header or `install()` rule exists, so the CMake version is metadata (`galoot_VERSION` for FetchContent users). "close to its first stable release" and "<release-tag>" occur nowhere else.
- Tests: no test reads the project version, the status line or the placeholders. The only README consumers are the example extractor (`galoot_add_doc_examples`) and the citation check (`tests.citations_*`, README has no plan, decision or rule numbers; the new lines have none). The `_cmake_` doc test points FetchContent at the source tree offline (`FETCHCONTENT_SOURCE_DIR_GALOOT`), so a placeholder URL and tag 1.0.0 do not affect it, and the test's own comment says it cannot check a URL or tag. Unaffected.
- Style: American English, no agent notes, no citations in the user page. Commit message names the model; body short and true.

## Findings

No blocker, no major.

- **m1 (minor), README line 23-25, rga3 "versioned separately".** No file in the repository carries an rga3 version (no CMake target version, no header constant); the claim is a policy statement. It is true as policy, but the preset ships in the same tree and the same tag, so a user reading "separately" cannot find the number. Also the old wording warned that interfaces may change; the new text drops that for rga3 and keeps only "has not reached its first stable release", which implies it but is softer. Suggest: "The rga3 preset ships in the same tree but is versioned separately and is below 1.0; its interface may still change." Only if the owner agrees the interface may change; otherwise leave. Decision 0248 item 6 sets rga3 0.1.0 for the record.
- **m2 (minor), README line 23, "This is the 1.0.0 release of the core."** Reads as a permanent statement, in a file that will outlive 1.0.0 and goes stale on 1.0.1. Suggest "This is version 1.0.0 of the core." only if the file is edited each release anyway; as written it is accurate at the release and merely a maintenance note.
- **m3 (minor, the known open item), README FetchContent block.** A released README still holds `<galoot-repository-url>`. The text is honest: it says to replace the URL and the tag now names the release, and the comment matches. A user is not misled, but the block does not work copy-pasted; and until the owner creates the tag `1.0.0` (on the records-only tip, per decision 0248 O4) the tag text is aspirational. Both are owner items: the URL and the tag. Nothing to fix in the commit; confirm the tag spelling `1.0.0` (no `v`) is what the owner creates.
- **n1 (nit), wording "the tag names this release".** Fine; "this release" could be misread as the repository's tag name vs. the version. Optional: "`1.0.0` is the tag of this release".

## Verdict

Approve. The change is minimal, complete (no other carrier of the version or old status), consistent with the guide's "this release" wording, and untouched by any test. m1 is the one item worth the owner's thought; m2, m3, n1 are optional.
