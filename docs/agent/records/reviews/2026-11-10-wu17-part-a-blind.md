# Blind review: WU17 part A, `cost.py comments` list (plan 0038, section 4, item 1)

Reviewer: Claude Sonnet 5.5, blind (W5); not told the author's verdict. Inputs: `feature/cost-evidence--model` (tip `db26a8f`, base `develop` at `eaf3807`) and `feature/cost-evidence--model--tests` (tip `34e19e8`). The plan is the only statement of intent.

## Departures, skipped checks, ambiguities (C20)

- No departures from the brief. Nothing built; no job pool, no master.
- The test must run inside a git checkout (`harness.py` calls `git rev-parse`), so I ran the old side from a detached worktree at `eaf3807` and the new side from the existing `cost-evidence--model` worktree, with the tests-branch file copied over. The model branch's copy of the test is byte-identical to the tests branch's (clean `git status` after the copy). Scratch worktrees `wu17-a-old` and `wu17-a-new` were created under `.claude/worktrees/` for the first attempt; `wu17-a-new` failed to create (branch already checked out) and was not used.
- Mutation testing of the test was not run (no mutant spec is required for part A by the plan); I probed `cost.py` by hand instead.
- Ambiguity: the plan says the sources are `records/research/...`; the file is at `docs/agent/records/research/...`. The code follows the plan's spelling. Not a defect, see finding 2.

## Results of the runs

- Old `cost.py` (`eaf3807`) with the test: 5 of 5 tests error (`TypeError: comments() got an unexpected keyword argument 'non_cost'`).
- New `cost.py` with the test: 5 of 5 pass.
- `python3 -I cost.py comments include/galoot/core/plan.hpp out/figures-gcc.txt out/figures-clang.txt` (both under `docs/agent/records/performance/plan-cost-model/out/`): new prints "0 counts in the comments are not in the figures", exit 0. Old: names `1,024` (twice), `4,096`, `134217728`, exit 1. The `89 million` in `plan.hpp` is now read and accepted via the list.

## Findings, ranked

1. **Low, test fails for an interface reason, not a behavior one.** Against the old code every test fails on the unknown keyword `non_cost`, including the two that should hold in both versions in spirit (figures row passes; unlisted count named). The red state is therefore real but uninformative: it shows the parameter is missing, not that the old pattern misses "89 million" or that old code accepts non-cost values. Acceptable for a test-first commit, since the new parameter is part of the interface; a reader should not take "5 failures" as five behavior gaps. Passing is for the right reason: each assertion exercises the list, the million pattern, or the refusal.
2. **Low, source check is shallow, as the brief suspected.** `listed_counts` only requires a non-empty `str`. A typo or a stale path passes. The recorded path (`records/research/2026-11-09-scalar-full10-gcc.md`) exists, but under `docs/agent/`, so it is not resolvable from the repo root as written. Adequate for criterion 1 (the plan asks for "an entry with no source (refused)"), but a cheap strengthening is available: in `comments` for the default list only, check that the source exists under `docs/agent/` and refuse otherwise (the test passes its own list with a nonexistent path, so the check would have to apply to `NON_COST` alone or the test would take a temporary directory). Also, a malformed entry (wrong tuple length) raises `ValueError` only by accident of unpacking; fine, but untested.
3. **Low, the million pattern over-matches.** `COUNT` includes `\b\d+ million\b`. "1.5 million" yields `5 million` (5,000,000); "1,024 million" yields `024 million` (24,000,000, since `\b` falls after the comma). Both give a spurious "unknown count" (a failure, the safe direction), not a false pass. The plan's pattern is "<digits> million", so this meets it; a lookbehind excluding `[\d.,]` would be tidier. "billion" is not read.
4. **Low, the list is by value, not by place.** A cost count elsewhere in a comment that happens to equal 1024 or 4096 would pass silently. Inherent to the option-b design the owner chose (decision 0230 item 1); worth one sentence in the docstring, not a change.
5. **Low, README row.** Accurate against my runs ("exits 0 on `plan.hpp` with both figures files", the four entries with source, the million pattern, the test run as `python3 -I`). The sentence now has "(`selfcheck`) and checks the comments' counts ..." after a comma list, which reads as a run-on; and "Last checked at `6a3cf13` ... `comments` exits 0 ... at `eaf3807`" mixes two revisions in one sentence (probably intended, since the headers changed between them; say so or use one). The row is long but matches its neighbors' style.
6. **Info, S11 and language.** `cost.py`, the test and the README row are under `docs/agent/`, where citing the research record is allowed; no agent citations appear in `include/`, and `plan.hpp` is unchanged. Test names and messages read as a human's; American English throughout. The `cost.py` usage text and docstrings are accurate for the new behavior.

## Criterion 1, fixture by fixture

| Plan fixture | Test | Verdict |
| --- | --- | --- |
| count that is a figures row (passes) | `test_figures_row_count_passes` (3,611,426 in a one-row figures file) | covered |
| listed count with source (passes) | `test_listed_count_with_source_passes` (1,024 and 4096, two spellings) | covered |
| unlisted count (fails, named) | `test_unlisted_count_fails_and_is_named`, also asserts the listed count is not named | covered |
| count in words, "about 89 million" (fails unless listed) | `test_count_in_words_fails_unless_listed`, both halves | covered |
| entry with no source (refused) | `test_entry_without_source_is_refused` for `""`, `None`, `"   "` | covered (see finding 2 for depth) |

The change meets the criterion: the list carries the four entries (1024, 4096, 134217728, 89,000,000) each with the scalar-full10 record, the pattern catches "<digits> million", the real run exits 0, and the README row is updated.

## Verdict

No blocking finding. Findings 2 and 3 are optional hardening; the rest are notes.
