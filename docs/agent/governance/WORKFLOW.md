# Galoot development workflow

This document says how a change to Galoot is planned, made, checked, reviewed, and recorded. It adds detail to the [constitution](CONSTITUTION.md), cited here as C1 to C20, and [VVUQ.md](VVUQ.md) defines the evidence each step refers to.

## Units of work

### W1 Definition of done

Each unit of work has a goal, a definition of done, and acceptance criteria, stated before the work starts. A request that states them meets this provision. A unit is done when its criteria are met and the evidence is recorded, or when it is halted under W8 and reported.

### W2 Small units

A unit is one change that a reviewer can understand in one sitting. It leaves the project building, with its tests passing, at every recorded step. Larger work is split into units that each meet this rule.

### W3 Plans

A unit that changes the core, a convention, a public interface, or a dependency, or that spans more than one working session, has a written plan in `docs/agent/plans/` before implementation starts. The plan states the goal, the definition of done, the acceptance criteria, the approach, the evidence it will produce, and, for agent work, an estimate of the agent cost. Other units need no written plan. A plan is closed by recording its outcome in it.

## Steps

### W4 Steps of a unit

1. Plan: state the goal and criteria (W1), with a written plan where W3 requires one.
2. Implement: make the smallest change that meets the goal, in the style of the surrounding code. Look for existing parts before adding new ones, and delete what the change makes dead. For a new capability, write the direct derived form first (C3), and optimize only after measurement shows the need.
3. Verify: produce the evidence VVUQ.md asks for, with tests written alongside the code they test. During development, run the tests the change can affect; before the change merges to `develop`, run the full suite.
4. Review: someone other than the author reviews the change before it merges to `develop` (W5).
5. Record: write the decisions, notes, and lessons the unit produced (W6), and state the verification and validation status of its results (V10).

### W5 Review

A change merges to `develop` only after three checks pass: the mechanical style checks of [STYLE.md](STYLE.md) (S3, S4), the full test suite, and a review against the constitution, this document, VVUQ.md, and STYLE.md by someone other than the author. A change written by an agent is reviewed by the owner or by a reviewer independent of the authoring agent: a person, or an agent with a different configuration that does not share the author's working context. The review and its outcome are recorded with the change. The owner's review of what reaches users happens at release (W13).

### W6 Records

| Location | Holds |
| --- | --- |
| `docs/agent/decisions/` | One file per decision (C18): context, options weighed, decision, consequences, and status. Any contributor enters a decision record without the owner's approval. A recorded decision is not rewritten; a later decision supersedes it, and both stay. |
| `docs/agent/plans/` | Plans under W3, each closed with its outcome. |
| `docs/agent/research-notes/` | Findings from sources: the claim, the source, who checked it and how, and what was confirmed as opposed to inferred (V9). |
| `docs/agent/lessons-learned/` | Mistakes and surprises: what happened, the cause, and what now detects it. |
| `docs/agent/workbench/` | Scratch work. Nothing relied upon lives here, and it may be cleared at any time. |
| `docs/` | Documentation for users, including the design study and the conventions specification. |

Record files carry a sortable prefix and a short descriptive name, such as `0001-dual-sign-convention.md`.

### W7 Version control and branches

Every change is committed to version control with a message that says what changed and why. A change made by an agent is identified as such in its message. Version control is the recovery path, so no archive copies of deleted files are kept.

| Branch | Holds | Merged into it by |
| --- | --- | --- |
| `master` | Releases only, each with its tag (W13) | The owner |
| `develop` | Integrated work that has passed W5 | Any contributor, agents included |
| `feature/<name>` | One unit of work or a group of related units, branched from `develop` | Any contributor, agents included |
| `feature/<name>/<part>` | A subfeature: one part of a feature, branched from its feature | Any contributor, agents included |

Merges into `develop`, feature, and subfeature branches need no authorization or action from the owner. A merge into a feature or subfeature branch needs the tests the change can affect to pass; a merge into `develop` needs W5.

### W8 Stopping rule

A unit halts and is reported to the owner when the same check still fails after two distinct attempts at a fix that produced no new understanding, when the work needs a decision the constitution assigns to the owner, or when it would have to grow beyond its stated scope. The report states what was tried, what was learned, and the options. The owner resumes, rescopes, or closes halted work; it is never silently dropped.

### W9 Defects and broken checks

A defect found outside the current unit is reported to the owner and noted in the record of the unit that found it. It is fixed within that unit only if the fix is small and within scope. An author whose change breaks a check restores the check before the change is relied upon. A test is never weakened, skipped, or rewritten to match wrong output; a test shown to be wrong is corrected by a recorded decision that states why.

### W10 Resource records

A unit that changes the core records the build time, build memory, and test time of the affected suites before and after the change (C14). Agent work records its agent cost. No bounds apply unless the owner sets them.

### W11 Phases

Work follows the project's current roadmap, at present the phased roadmap of the design study. A phase closes at its gate, when the gate's evidence exists and the owner accepts it. A module waits for the gates of the modules it depends on; exploratory work in the workbench may run ahead of them.

### W12 Documentation

Documentation is written for human readers in plain American English, and it changes in the same unit as the code it describes so that the two agree. Process records (W6) stay out of user documentation.

### W13 Releases

A release is a merge of `develop` into `master` together with a tag naming the release's version. Only the owner merges into `master` or creates release tags, and does so with no open blocking defect and with evidence current for the released revision. Each component is versioned semantically, and each release of the core records the version of the conventions specification it implements.
