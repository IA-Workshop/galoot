# 0251 Release readiness rulings

Date: 2026-11-12. Source: the owner's acceptance of the recommendations in the U11 report (plan 0042 unit U11).

1. **CONVENTIONS 1.18.** The owner pastes the prepared text (`records/release/conventions-1.18-owner-text.md`) as version 1.18 when back at the machine, then reruns the trace gate and the run-log check. Options were: paste as prepared (recommended; chosen), reword the line-12 sentence first, or decline (the tag message would then say 1.17). The owner is remote; the edit, the merge to `master` and the tag are pending.
2. **DoD-5.** The algebras the tests declare themselves (`test/core/signatures.hpp`, the V8 algebras) are accepted as the named test-only model. Options were: accept (recommended; chosen), or add an STA model under `test/`, which changes `test/`, voids T, and reruns the matrix (about an hour).
3. **DoD-28.** No lesson was written in the M7 and release windows; accepted for the tag. One records-only lesson is written after the tag.
4. **Readings.** The readings of DoD-1, DoD-13, DoD-15 and DoD-21 stated in `records/release/dod-1-to-29.md`, and the untraced items 3 and 6 of plan 0019's post-WU10 audit, are confirmed as readings. They change no result.
5. **Repository URL.** Still deferred (decision 0249 item 1).
