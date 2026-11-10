# Galoot development workflow

Status: ratified by the owner on 2026-11-01 (decision 0094). W5 amended on 2026-11-01, ratified by the owner: adopt the usability checklist (decision 0160). W5 and W7 amended on 2026-11-07, ratified by the owner: proceed with recommendations (decisions 0183 and 0188). The `records/<name>` row of W7 was ratified on 2026-11-07 (decision 0196). W5's treatment of records-only merges was ratified on 2026-11-08 (decision 0215). W2 was widened on 2026-11-08 (decision 0216 item 8). W3's sentence on judging a gap's mutants was ratified on 2026-11-09 (decision 0219 item 10); this status line was brought up to date on 2026-11-10 (decision 0228).

This document says how a change to Galoot is planned, made, checked, reviewed, and recorded. It adds detail to the [constitution](CONSTITUTION.md), cited here as C1 to C20, and [VVUQ.md](VVUQ.md) defines the evidence each step refers to.

## Units of work

### W1 Definition of done

Each unit of work has a goal, a definition of done, and acceptance criteria, stated before the work starts. A request that states them meets this provision. A unit is done when its criteria are met and the evidence is recorded, or when it is halted under W8 and reported.

### W2 Small units

A unit is one change that a reviewer can understand in one sitting. It leaves the project building, with its tests passing, at every recorded step. One exception: a test committed before the code it judges may leave that commit failing if the commit message says so and names the test that fails, and a run of such labelled test commits may precede the one commit that makes them pass (decision 0216). Larger work is split into units that each meet this rule.

### W3 Plans

A unit that changes the core, a convention, a public interface, or a dependency, or that spans more than one working session, has a written plan in `docs/agent/plans/` before implementation starts. The plan states the goal, the definition of done, the acceptance criteria, the approach, the evidence it will produce (where it adds tests to close a gap in an existing suite, it judges the gap's mutants by that suite: a mutant counts for the gap when the suite without the new tests lets it survive and the suite with them kills it, and a mutant written for a new test counts when that test is among those it fails), and, for agent work, an estimate of the agent cost. Other units need no written plan. A plan is closed by recording its outcome in it. A session that continues a unit starts by reading the unit's plan and records, and ends by writing into the plan what is done, what is in progress, the next step, and the open questions. Nothing a later session needs is left only in agent memory, the workbench, or an uncommitted file.

## Steps

### W4 Steps of a unit

1. Plan: state the goal and criteria (W1), with a written plan where W3 requires one.
2. Implement: make the smallest change that meets the goal, in the style of the surrounding code. Look for existing parts before adding new ones, and delete what the change makes dead. For a new capability, write the direct derived form first (C3), and optimize only after measurement shows the need.
3. Verify: produce the evidence VVUQ.md asks for, with tests written alongside the code they test. Run the tests the change can affect, widening the run at each merge as [PERFORMANCE.md](PERFORMANCE.md) P8 and P9 state.
4. Review: someone other than the author reviews the change before it merges to `develop` (W5).
5. Record: write the decisions, notes, and lessons the unit produced (W6), and state the verification and validation status of its results (V10).

### W5 Review

A change merges to `develop` only after four checks pass: the mechanical style checks of [STYLE.md](STYLE.md) (S3, S4), the full test suite, the convention trace gate (`python3 -I tools/test_references.py trace docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md test/reference/golden.hpp test`, run from the repository root), and a review against the constitution, this document, VVUQ.md, and STYLE.md by someone other than the author. A change written by an agent is reviewed by the owner or by a reviewer independent of the authoring agent: a person, or an agent with a different configuration that does not share the author's working context. The reviewer gets the change, its acceptance criteria, and the author's list of departures (C20), but not the author's view that the change is correct, and does not edit the change it reviews. The review record names the reviewer and its model, says what it checked and what it did not, and lists each finding with its resolution; it is recorded with the change. A fix to core code made after its review gets a delta review of the fix before the merge (decision 0094), except a change only to comments, which the review record lists with its resolution. The `blind-reviewer` agent performs such reviews, and the `auditor` agent checks a branch or release against the governance. A review of a change to a public interface also applies the usability checklist in the `blind-reviewer` role file (adopted by the owner on 2026-11-01, decision 0160). The owner's review of what reaches users happens at release (W13). A merge of records only, or of a research note, into `develop` needs the checks of P9 that its files can affect (at least the citation and style checks) but no independent review (decision 0215).

### W6 Records

| Location | Holds |
| --- | --- |
| `docs/agent/governance/` | The constitution, this document, VVUQ.md, PERFORMANCE.md, STYLE.md, and the conventions specification CONVENTIONS.md (C10). Each changes only by a change the owner ratifies. |
| `docs/agent/decisions/` | One file per decision (C18): context, options weighed, decision, consequences, and status. Any contributor enters a decision record without the owner's approval. A recorded decision is not rewritten; a later decision supersedes it, and both stay. |
| `docs/agent/plans/` | Plans under W3, each closed with its outcome. |
| `docs/agent/records/` | Measurements, including the performance history of PERFORMANCE.md P7 and the run log of P13, and review records (W5) in `reviews/`. |
| `docs/agent/research-notes/` | Findings from sources: the claim, the source, who checked it and how, and what was confirmed as opposed to inferred (V9). |
| `docs/agent/lessons-learned/` | Mistakes and surprises: what happened, the cause, and what now detects it. |
| `docs/agent/snippets/` | Reusable tools coalesced from the workbench and spikes, with the milestone reviews that maintain them. |
| `docs/agent/workbench/` | Scratch work. Nothing relied upon lives here, and it may be cleared at any time. |
| `docs/` | Documentation for users, including the design study. |

Record files carry a sortable prefix and a short descriptive name, such as `0001-dual-sign-convention.md`.

### W7 Version control and branches

Every change is committed to version control with a message that says what changed and why. A change made by an agent is identified as such in its message. Version control is the recovery path, so no archive copies of deleted files are kept. Agents do not rewrite or force-push shared history, and do not delete work that version control does not hold; only the owner does these things. Before a command that deletes, moves, or overwrites files, an agent resolves each target to its absolute path and lists the targets, so that a relative path or a pattern evaluated in the wrong directory, such as another worktree, cannot misapply.

| Branch | Holds | Merged into it by |
| --- | --- | --- |
| `master` | Releases only, each with its tag (W13) | The owner |
| `develop` | Integrated work that has passed W5 | Any contributor, agents included |
| `feature/<name>` | One unit of work or a group of related units, branched from `develop` | Any contributor, agents included |
| `feature/<name>--<part>` | A subfeature: one part of a feature, branched from its feature | Any contributor, agents included |
| `feature/<name>--tests` | A test author's commits for `feature/<name>`, branched from it; never merged, its commits are cherry-picked into it (decision 0185) | Nobody |
| `research/<name>`, `review/<name>`, `gate/<milestone>` | A research note or spike, a review record (W5), or a gate report and its records, branched from `develop` and merged into it | Any contributor, agents included |
| `records/<name>` | Records only (plans, decisions, review and audit records, gate reports, handoffs, run rows), and the text of a governance change the owner has ratified; no change to `include/`, `test/`, `tools/`, `cmake/`, or the build files. Branched from `develop` and merged into it; a review record is branched from the revision it reviews, never merged, and its commits are cherry-picked into `develop` | Any contributor, agents included |

A test author's branch and the branch of a review record are not merged; their commits are cherry-picked with `-x`.

Merges into `develop`, feature, and subfeature branches need no authorization or action from the owner. A branch merges only into the branch it was made from, one merge at a time into the same branch. Before the merge, the parent's current state is merged into the branch and the checks of [PERFORMANCE.md](PERFORMANCE.md) P9 for that level pass on the result; a merge into `develop` also needs W5. A fix is made on a branch from the most upstream branch the defect affects and merged there, and each descendant branch receives it by merging its parent in. Feature and subfeature work is done in a git worktree of its own, one per branch, under `.claude/worktrees/` or beside the main checkout, so that workstreams running at the same time never share a checkout or build directory (P11). The main checkout stays on `develop`. A unit is claimed before work starts by committing its plan (W3), or for a unit without one a one-line entry in `docs/agent/plans/`, to `develop`, naming the unit and its branch. Units that run at the same time change different files and do not rely on each other's unfinished results, and a fix that would touch another active unit's files is reported rather than made.

### W8 Stopping rule

A unit halts and is reported to the owner when the same check still fails after two distinct attempts at a fix that produced no new understanding, when the work needs a decision the constitution assigns to the owner, or when it would have to grow beyond its stated scope. The report states what was tried, what was learned, and the options. The owner resumes, rescopes, or closes halted work; it is never silently dropped. Work that continues a halted unit keeps its count of attempts, and before a third attempt someone without the failed attempts' context reviews the plan. A halting agent leaves its work committed, continues work that does not depend on the halt, and, faced with an ambiguous instruction, does only what every reading permits.

### W9 Defects and broken checks

A defect found outside the current unit is reported to the owner and noted in the record of the unit that found it. It is fixed within that unit only if the fix is small and within scope. An author whose change breaks a check restores the check before the change is relied upon. A test is never weakened, skipped, or rewritten to match wrong output; a test shown to be wrong is corrected by a recorded decision that states why. A change to a test, expected value, or tolerance that judges the author's own change is committed separately and listed in the review request; a change that cannot build without both may be one commit if the review request lists the tests and the expected values. A failing regression guard is a defect unless the change said beforehand that it alters the guarded result.

### W10 Resource records

A unit that changes the core records the build time, build memory, and test time of the affected suites before and after the change (C14, PERFORMANCE.md P1). Agent work records its agent cost. No bounds apply unless the owner sets them.

### W11 Phases

Work follows the project's current roadmap, at present [plan 0001](../plans/0001-core-v1-roadmap.md). A phase closes at its gate, when the gate's evidence exists and the owner accepts it. A module waits for the gates of the modules it depends on; exploratory work in the workbench may run ahead of them.

### W12 Documentation

Documentation is written for human readers in plain American English, and it changes in the same unit as the code it describes so that the two agree. Process records (W6) stay out of user documentation. Every code example in user documentation is compiled and run as a test.

### W13 Releases

A release is a merge of `develop` into `master` together with a tag naming the release's version. Only the owner merges into `master` or creates release tags, and does so with no open blocking defect and with evidence current for the released revision. Each component is versioned semantically, and each release of the core records the version of the conventions specification it implements. The release record lists each public interface the release removes or changes incompatibly (C12).

### W14 Delegation

An agent that delegates gives the delegate the goal, the definition of done, the scope, and the governance that applies, and states its question without its expected answer. It checks the delegate's result against those criteria before using it, and reports any result it has not checked as unchecked.
