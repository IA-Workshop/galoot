# Handoff, 2026-11-07, second session

Start here. `develop` is at the commit that adds this file; `master` is untouched. The main checkout stays on `develop`.

## Standing instruction (still in force)

Proceed with galoot development until an action needs the owner or the owner asks for a pause. Delegate to Opus agents (planners, test authors, implementers, blind reviewers, auditors), merge to `develop` after blind review and checks, and open every report with departures, skipped checks and ambiguities (C20). Every report lists all open owner items, each with an explanation, tradeoffs and a recommendation.

## State

- M3 is accepted. WU13 (`compose`, `transform`, `antiproject`, the rounding check) is merged (decisions 0176 to 0195). CONVENTIONS 1.10 is ratified.
- The release, asan and lint presets passed on `develop` at d68881f (gcc and clang for release and asan, lint). Last full runs from empty trees on the WU13 branch: gcc-debug 409, clang-debug 409, clang-measure 401.
- Plans claimed on `develop`: 0022 (WU15, format and catch-alls) and 0023 (WU16, the rga3 model). No code has started for either.
- Merged records: the WU9 merge audit (`records/gates/2026-11-07-wu9-merge-audit.md`) and research note 0022 on the flat-plane chain rounding. math-tools `develop` merged the section 1 mutation rerun record.
- Job pool: 23 of 23 slots free, restarted by the owner. Start only if stopped (`tools/jobpool ensure`), never stop. `tools/timed` needs `eval "$(tools/jobpool env)"`.

## Owner rulings already given (decision 0195)

O6 investigate first (done, below); overflow keeps failing the check; the fast-math `#error` stays broad; the CONVENTIONS 4.5 amendment for coarse types (draft 1.11, wording unconfirmed); decision 0193's raised Clang step limit stays; combining a test author's build-failing commit with its fix is allowed under the stated terms; the pool is restarted.

## Open owner items

Each has my recommendation. Nothing below proceeds until the owner rules, except where marked.

1. **CONVENTIONS 1.11 wording** (4.5, the coarse-type clause). It is a draft; confirm the sentence. Recommend confirm.
2. **O6 outcome** (research note 0022). The cause is input outside the bound's hypothesis: the composed flat motor drifts off the versors, and the defect adds up under composition. The bound is sound. Recommended: add a renormalization helper (m minus (v·m/|v|²) v; 12 multiplications, 10 additions, 1 division) in the M6 rigid and conformal models, and apply the note's section 9 comment text to `transform` now. Rejected: a looser normwise bound. Optional: a componentwise margin of γ_(K+3), about 20% less sharp, needing a lemma first. Recommend the helper and the comment, and no margin change.
3. **WU9 audit finding A1** (decision 0183 says the fix pass changed only comments, but it changed code in `product.hpp` and `plan.hpp`, so the post-WU10 audit's M1, no delta review, is still open), with A2 (W9: code and its judging tests changed together) and the carried M2 (W11: WU9 began before the M2 gate was accepted). Recommend a new decision that supersedes 0183's statement and accepts the fix pass as covered by WU10a's later reviews of `product.hpp`, an inferred mitigation, and rules on W11 as a past departure with no further action.
4. **W7 `records/<name>` row.** The audit proposes the table row and a sentence that test-author and review-record branches are cherry-picked, not merged, and suggests retiring `review/` and `gate/` into `records/`. Also, `0fb3a57` merged a subfeature branch straight into `develop`, which W7 does not allow. Recommend ratify the row and declare the subfeature merge a past departure. Governance, so only the owner edits it.
5. **Reviews missing for three merges:** `feature/jobpool-idle-shutdown`, `feature/runlog-0170`, `feature/test-split` (decision 0194 mentions a reviewer but no record was committed). Recommend one blind review of all three at the M4 gate rather than three now.
6. **WU9 and earlier audit gap:** 423 commits from 9d82f30 to a75ce5e (WU6a to WU8b) have never had an independent audit. Recommend one auditor pass at the M4 gate.
7. **WU15 plan 0022 questions** (O1 to O9 in the plan). Needing rulings: O1 how a blade's name is chosen when printed (recommended: the algebra tag may declare oriented names; otherwise canonical name with a leading `-` for orientation -1); O2 printed form (recommended: `{e41: 1, e42: 0}`, new CONVENTIONS 7.2); O3 ratify the misuse catalogue; O4 a library message for `blades<Alg,...>` on a tag with no metric, which needs a STYLE S7 amendment; O5 three new catch-alls (scalar conversion, algebra conversion, multivector plus a number). O6 to O9 are recommended defaults.
8. **WU16 plan 0023 questions.** O1 run WU16 before WU15 and before the scalar-conversion unit (recommend yes; the M4 gate still waits for WU15); O2 named types as alias templates, not classes derived from `mv` (recommend aliases); O4 public names; O5 CONVENTIONS 5.8, the motor and flector component order (confirmed only after the math-expert reads Lengyel's pages directly); O6 `translation(dx, dy, dz)` as three scalars; O8 amend DoD-17's reading so all 32 crystal classes are used with axes along [111], since the planner's probe found rational versors for the hexagonal and trigonal classes; O9 a README compiled by a CTest. Recommended as in the plan.
9. **Cleanup** of merged worktrees and branches (about 22 worktrees, including `review-wu13*`, `core-transform*`, the audit, plan and O6 worktrees once their branches are merged). Not yet ruled on. Recommend approve.
10. **Smaller:** the decision 0080 compile-record copy (needs trees rebuilt at the final revision, so do it at the M4 gate); the test-split agent-cost row (token figures lost); a knowledge-base note on rotation drift (see the O6 report); the unexplained Clang 49-versus-48 multiplication count; the `mutate.py` gap (`static_assert` at line 856 in `--runtime` mode, stale-binary behavior), bits and blade-set tests with no mutation set, a lost tripwire on debug Clang presets, and a CMake comment's wording (recommend one small test-quality unit after WU16); no tests retired so far.

## Next steps

1. Report the open items above to the owner and take rulings on 1 to 4, 7 and 8.
2. Launch WU16 first if the owner allows (item 8, O1): the plan's order starts with the math-expert on `feature/rga3--math`. Launch WU15 in parallel only after its O1 and O2 are ruled, since M4 requires a zero core diff from the rga3 model.
3. At the next merge, run all presets from empty trees (P9) and `tools/runlog check`.
4. Record the owner's rulings in a new decision. The next free decision number is 0196.

## Method reminders

Tests before code on a separate branch, cherry-picked with `-x` (decision 0185); a delta review after any non-comment fix; branch names `feature/<name>--<part>` (decision 0188), `records/<name>` for records; agent commits name the model that did the work; no decision or plan citations in human-facing files outside docs/agent; commands the owner must run go in the reply, not only in a file.
