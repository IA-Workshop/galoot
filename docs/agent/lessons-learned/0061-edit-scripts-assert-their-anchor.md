# 0061 An edit script must fail when its anchor text is not found

- Date: 2026-10-31
- Unit: WU3, metric (plan 0005), Claude (agent, Opus 5.5)

## What happened

The review follow-up added two exclusions to `.clang-tidy` and their reason in its comment block with one Python script. The script used `str.replace` for both edits without checking that the anchor matched. The exclusions' anchor matched; the reason's anchor did not (it quoted a comment line that the file wraps differently), so the reason was silently dropped. The report then said the reason had been added, and the delta review found the exclusions without one (finding D2).

## Cause

`str.replace` returns the text unchanged when nothing matches, so a mistyped anchor is a silent no-op. The diff was printed, but only the part with the exclusions was read.

## What detects it now

Every scripted edit asserts that its anchor occurs exactly once (`assert s.count(a) == 1`) before replacing, so a missed anchor stops the script. A claim in a report that a file was changed is checked against `git diff` of that file.
