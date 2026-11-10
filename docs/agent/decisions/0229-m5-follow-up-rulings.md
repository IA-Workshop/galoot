# 0229 M5 follow-up rulings

Date: 2026-11-10. The owner replied "Accept all recommendations" to the report on the M5 documentation unit (plan 0035). Each item quotes the options and the recommendation accepted.

1. **Tags in shared headers.** Options: (a) add one sentence to the README, at the next README touch, saying that a tag copied into a header that several files include goes in a named namespace, not an anonymous one; (b) leave it. Accepted: (a).
2. **README test labels.** Options: (a) per-block or neutral labels, a CMake change to the README mechanism; (b) leave `rga3.readme`. Accepted: (b), to be settled with the user guide (DoD-27).
3. **The snippets review's five decisions** (`docs/agent/snippets/reviews/M5.md`). Accepted as recommended: one small unit for the index and path edits and the reruns of `plan_cost_model`'s `selfcheck` and `comments` and of `jobserver_check.py`, before WU17; delete `bits_fault_injection.py`; keep the Spike B script, relabelled and repaired; keep the evaluation counter for WU17 (decision 0215); no general limit-search tool yet.
4. **Worktrees.** The five worktrees and branches of plan 0035 are removed after the commits are verified in `develop`. Done: the feature branch is an ancestor of `develop`; each review file in `develop` holds its branch's text with only added lines; the tests branch's `test/` tree equals `develop`'s.
