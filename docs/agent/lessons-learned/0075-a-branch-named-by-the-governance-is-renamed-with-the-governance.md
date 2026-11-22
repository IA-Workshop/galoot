# 0075 A branch that the governance names is renamed together with the governance

- Date: 2026-11-21
- Unit: plan 0045 (release 1.0.1), units R2 and G2

## What happened

The integration branch became `main` after 1.0.0. Twenty-nine places in the governance, `AGENTS.md` and the agents' role files still said `develop`, and every brief written in between had to say which name was meant.

## Cause

The rename was made in git alone. Nothing ties a branch's name in the governing text to the branch that exists, and no check read the text for the old name.

## What now detects it

A rename of anything the governance names is a decision under C18 whose unit edits the governing text and the role files in one commit (`01dce7f` for this one, by the owner's ratified wording). The readiness checklist of a release greps the governance, `AGENTS.md` and `.claude/agents/` for the old name as a whole word; gate G2 of plan 0045 and the release audit both ran it and found nothing.
