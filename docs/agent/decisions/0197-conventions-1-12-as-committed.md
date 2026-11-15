# 0197 CONVENTIONS 1.12 as committed: the motor and flector order, `translation(dx, dy, dz)`, and the line's moment

- Status: recorded. The content of 1.12 was ratified by the owner on 2026-11-07 ("accept all recommendations", decision 0196 item 6, plan 0023 O5 and O6, and section 10's 5.2 sentence). The exact wording of 5.8, which uses the pages' coefficient letters in place of the plan's, awaits the owner's confirmation (item 2).
- Date: 2026-11-07
- Recorded by: Claude (agent, Opus 5.5), implementer of WU16
- Bears on: CONVENTIONS 5.2, 5.7, 5.8; plan 0023 section 4 step 2, O5, O6; decision 0196; research note 0023

## Context

Decision 0196 accepted plan 0023's recommendations: O5, the motor and flector layouts as a new CONVENTIONS 5.8, labeled Confirmed only after Lengyel's pages are read directly; O6, `translation(dx, dy, dz)`; and the 5.2 sentence "Its moment is p × q for points of any weight", labeled Checked after step 1. Step 1 (the math-expert, `feature/rga3--math`, merged at `60f6e74`) read the Motor page (revid 494) and the Flector page (revid 495) as raw wikitext (research note 0023, M1 and F1), and added `check_motor_flector_layouts`, `check_line_moment`, `check_translation`, and `check_rotation` to `conventions_check.py`. The note found the layouts equal to the pages' and recommended the pages' coefficient letters for 5.8 (F6).

## Decision

1. **The condition on 5.8's label is met.** The pages store a motor as Q_vx e41 + Q_vy e42 + Q_vz e43 + Q_vw 𝟙 + Q_mx e23 + Q_my e31 + Q_mz e12 + Q_mw 1 and a flector as F_px e1 + F_py e2 + F_pz e3 + F_pw e4 + F_gx e423 + F_gy e431 + F_gz e412 + F_gw e321, the orders of plan 0023 departure 4. `conventions_check.py` passes ("all convention checks passed (seed 20261031)"), including the closure of both layouts under the antiproduct.
2. **The text, as committed** (the changes against 1.11):
   - 5.8, new: "A motor stores (Q_vx, Q_vy, Q_vz, Q_vw) on e41, e42, e43, 𝟙 and then (Q_mx, Q_my, Q_mz, Q_mw) on e23, e31, e12, **1**, and a flector stores (F_px, F_py, F_pz, F_pw) on e1, e2, e3, e4 and then (F_gx, F_gy, F_gz, F_gw) on e423, e431, e412, e321, with the coefficient names of Lengyel's Motor and Flector pages. The motor's blades are those of even grade and the flector's those of odd grade, so the antiproduct of two motors or of two flectors is a motor, and that of a motor and a flector, in either order, is a flector. (Confirmed, research note 0023; closure under the antiproduct Checked)". Against plan 0023 O5's sentence: the letters r, u, s, h become the pages' Q_v, Q_m, F_p, F_g (note 0023 F6), and the second sentence states the closure that the label's "Checked" refers to. The order and the blades are unchanged.
   - 5.7: "`translation(d)` moves by d" becomes "`translation(dx, dy, dz)` moves by d = (dx, dy, dz)", as O6 worded it. The label "(Proposed)" becomes "(Proposed, with both actions Checked from the coefficients they build)": `check_translation` and `check_rotation` check that ½d on e23, e31, e12 plus 𝟙 moves by d and that l sin(θ/2) + 𝟙 cos(θ/2) turns by θ, for rational half angles. The choice of full quantities in the constructors stays Proposed.
   - 5.2: adds "Its moment is p × q for points of any weight." The label stays "(Checked)", now backed by `check_line_moment`.
   - The status line and the 1.12 revision row.
3. **Options weighed for item 2's 5.8 wording.** Committing O5's sentence with r, u, s, h as ratified, which names coefficients no published formula uses; holding 5.8 for the owner, which stops step 3, whose tests need the layouts; or committing the pages' letters and asking the owner to confirm them. The third was chosen, as the note recommended. The owner may reverse it; nothing in the layouts, the code, or the tests depends on the letters.

## Consequences

- The trace table gains rows set to `no` until the model exists: 5.7 "translation(dx, dy, dz) moves points by d = (dx, dy, dz)"; 5.8 "a motor is stored on e41, e42, e43, e1234, e23, e31, e12, 1 in that order", "a flector is stored on e1, e2, e3, e4, e423, e431, e412, e321 in that order", and "the antiproduct of two motors or two flectors is a motor, and of a motor and a flector a flector". 5.2's moment already has its row, the `rigid_lines` golden fact.
- The trace gate passes on this commit, with 5.1, 5.2, 5.3, 5.7, and 5.8 among the items with no test.
