# 0034 No agent notes in human-facing files

- Status: accepted; the STYLE S11 paragraph is ratified by the owner's ruling
- Date: 2026-10-31
- Decided by: the owner, 2026-10-31 ("Never put things like '(decision 0022):' in human facing code. these are agent notes, not human notes. This is a style ruling per governance"). Recorded by Claude (agent).

## Context

Comments in `include/galoot/core.hpp`, `assert.hpp`, tests, CMake files, and tools cite decisions, plans, research notes, and provision numbers. Those records are for agents and may change or be cleared; a human reading the code cannot rely on them.

## Decision

STYLE S11 gains a paragraph: code, comments, build files, scripts, and user documentation never cite agent records (decisions, plans, research notes, reviews, lessons, the workbench) or the numbers of governance provisions. A comment states the reason in place. Citing the conventions specification by section title, or a published source, is allowed. Agent records may cite each other.

Existing citations are removed in a cleanup unit after the units in flight (recorder, bits, metric, M0 closure) merge, so that the cleanup does not conflict with their branches. `core.hpp` is fixed now. A mechanical check (a script like the include scan) that flags the patterns "decision NNNN", "plan NNNN", "research note", and provision numbers in human-facing files joins that unit.

## Amendment, 2026-10-31

The owner tightened the ruling: "there should be no citations of any provisions or agent work of any sort in code. refer to the code style in the upper directory. the code and comments should read as if a HUMAN wrote them and as if a human will be reading them. if the agent must make a note for future reference, record it in the agent documents area." STYLE S11 was rewritten accordingly. The allowance for citing the conventions specification by section title is withdrawn: a convention is restated where it is applied. Only published sources may be cited. The code style guide at `utils/code-style` applies to all code, comments, tests (names and messages included), build files, scripts, generated files, and user documentation. The cleanup unit covers all of these, including citations of provisions and of the conventions specification, and its mechanical check flags decision, plan, note, review, spike, and workbench references, provision numbers, and the name of the conventions specification.

## Consequences

Reviewers check new code against the paragraph at once. Header comments that explain a rule state the rule's reason, not its number.
