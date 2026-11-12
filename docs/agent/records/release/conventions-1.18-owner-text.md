# CONVENTIONS 1.18: the text for the owner to paste

- Status: prepared 2026-11-12 by Claude (agent, Opus 5.5), plan 0042 unit U11, records only. Nothing here is in force: CONVENTIONS changes only by the owner's edit (C10). The owner ruled the content (decision 0248 item 2, the errata row; decision 0250 item 5, the sources sentence in the same edit) but cannot edit yet, so the edit is pending.
- Source of each text: the errata row is the corrected text of the M7 audit's MA1 (`records/gates/2026-11-11-M7-audit.md`), copied unchanged. The sources sentence, the status-line sentence and the revision-history row are drafted here; their wording is the owner's to change.
- Checked: a scratch copy of CONVENTIONS 1.17 with the four edits below passed the trace gate (the "Check of this text" section). The two tests the row names exist: `test/presets/rga3/objects_test.cpp:173` and `test/presets/rga3/format_test.cpp:59`.

## 1. The status line (line 3)

Replace the opening words `Status: version 1.17, in which` with:

```
Status: version 1.18, in which section 8 records the disagreement between the rigid wiki's Errata page and its Motor page on the names of the motor's scalar and antiscalar coefficients, and the paragraph on sources names this specification and plan 0001 as Galoot's references, with the design study kept as history, ratified by the owner on 2026-11-12 ("Accept all recommendations", decisions 0248 item 2 and 0250 item 5). Version 1.17, in which
```

The rest of the line stays as it is.

## 2. The sources sentence (line 12)

Replace the sentence `The design study is the most canonical reference for Galoot.` with:

```
This specification and plan 0001, the roadmap of record ([0001-core-v1-roadmap.md](../plans/0001-core-v1-roadmap.md)), are the references for Galoot; the design study is kept as history, and where it differs from them, they govern.
```

The line then reads: "Sources rank as follows. This specification and plan 0001, ... they govern. For mathematical conventions, Lengyel's wikis (...) are the preferred source. ..." The "Study" label above it keeps its meaning (a value taken from the design study and not yet confirmed), since the study stays a source of values while it is no longer the reference.

## 3. The section 8 row

Add after the "Conformal duals" row of the table in section 8:

```
| Motor coefficient names | The rigid wiki's Errata page, correcting Equation (3.219) of the fifth and earlier printings of *Projective Geometric Algebra Illuminated*, says that Q_vw is the coefficient of the scalar **1** and Q_mw that of the antiscalar 𝟙. The wiki's Motor page, which 5.8 follows, puts Q_vw on 𝟙 and Q_mw on **1**, as the rga3 preset's Motor does. The two sources disagree on the letters only: the motor's blades are the same either way, and the constraint Q_vx Q_mx + Q_vy Q_my + Q_vz Q_mz + Q_vw Q_mw = 0 is symmetric in Q_vw and Q_mw, so no operation count and no polynomial in blade coefficients depends on the choice; formulas written in the letters, such as the unit constraint, read differently. The book has not been read; the Errata page was read in a copy. | 5.8, the Motor page | "the rga3 layouts hold the oriented blades of their names" in `test/presets/rga3/objects_test.cpp`, whose motor layout puts e1234 fourth (Q_vw) and **1** eighth (Q_mw), and "an rga3 Motor prints on e41, e42, e43, e1234, e23, e31, e12, 1" in `test/presets/rga3/format_test.cpp`, which prints the fourth coefficient on e1234 and the eighth on 1 |
```

## 4. The revision-history row

Add as the last row of the table under "Revision history" (change the date if the edit lands on another day):

```
| 1.18 | 2026-11-12 | Section 8 adds the "Motor coefficient names" row: the rigid wiki's Errata page and its Motor page name the motor's scalar and antiscalar coefficients Q_vw and Q_mw the other way round; 5.8 follows the Motor page, and two tests pin which blade each position holds; the paragraph on sources names this specification and plan 0001 as the references and the design study as history, in place of "The design study is the most canonical reference for Galoot"; no convention changes; ratified by the owner: accept all recommendations (decisions 0248 item 2 and 0250 item 5) |
```

## 5. After the edit: the commands

From the main checkout on `develop`. The CONVENTIONS commit holds that file only, so that the trace gate's row names its revision; the second commit adds only the run rows. Only the trace gate reads CONVENTIONS.md (`conventions_check.py` reads no copy of it, by grep), and `tools/runlog check` reruns because the trace gate's row changes `runs.tsv`.

```bash
cd /home/brianm/projects/utils/math-core/galoot
git status --short                                   # must print only " M docs/agent/governance/CONVENTIONS.md"
git commit -m "CONVENTIONS 1.18: the motor coefficient names row and the sources sentence" -- docs/agent/governance/CONVENTIONS.md
tools/timed trace - gate:trace@conventions-1.18 -- python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test                     # must exit 0; items with no test: 6.1 to 6.4 only
tools/timed check - gate:runlog@conventions-1.18 -- tools/runlog check
git commit -m "Records: trace gate and run-log check after CONVENTIONS 1.18" -- docs/agent/records/performance/runs.tsv
```

The coordinator may run the last three commands for the owner after the owner's commit. The release commands of plan 0042 section 8 then check that the trace gate passed on the CONVENTIONS commit itself.

## Check of this text

A copy of `docs/agent/governance/CONVENTIONS.md` at `300f6b4` with edits 1 to 4 applied, in the unit's scratch directory, was given to the trace gate in place of the file. Its run row is in `runs.tsv` (selection `check:trace-1.18-draft@U11`, committed in T; plan 0042 section 11). The trace gate reads the numbered items of CONVENTIONS, so neither the new section 8 row nor the sentence of line 12 adds or removes an item; the run is a check that the edited file still parses as the gate expects.
