# 0160 The usability checklist for public interfaces is adopted

- Status: accepted by the owner, 2026-11-01
- Date: 2026-11-01
- Decided by: the owner; recorded by Claude (agent, Sonnet 5.5)

## Context

Research note 0019, section 7, proposed a one-page checklist for reviews of public interfaces. It restates rulings already made (explicit, named lossy conversion; counted construction with a zero default; the diagnostic form of decision 0033) and adds checks drawn from the usability literature.

## Decision

1. The checklist is adopted as written in the note, and it is part of the `blind-reviewer` role file under "Usability of public interfaces". Each unchecked item is reported as a finding.
2. WORKFLOW W5 says that a review of a change to a public interface applies it.
3. The note keeps the sources and the reasoning. A change to the checklist is a change to the role file, and to W5 only if its wording of the reference changes.

## Consequences

- The WU8a review is the first to apply it.
- Items that duplicate a ratified rule (S7, S8, decision 0033) stay in the checklist so a reviewer needs one list.
