# 0225 WU12 final rulings

Date: 2026-11-09. The owner accepted all recommendations made in the report that held the merge of the change-of-basis unit.

1. **STYLE S7.** A catch-all that takes a value as a tag may use an unconstrained `auto` parameter. S7 gains one sentence saying so; the code in the unit stays as written (decision 0224 item 5, option a).
2. **GCC constexpr-initializer defect.** The kernels `run_map`, `run_product` and `run_sum` change to one initializer per place (form A), in a small core unit with a canary test, a mutant that restores `out{}`, a before and after build-cost measurement, and a blind review. `test/support/diagonalize.hpp` takes the safe form in a commit of its own on the WU12 unit. Lesson 0065, the knowledge-base note on shared array initializers and the upstream-awareness note are corrected. `minimal.cpp` is retested on each new GCC. Nothing is reported upstream.
3. **Equality on `basis_change`.** Renamed (for example `same_rows`) before release, in a later unit.
4. **N3, spurious overflow on large unreduced rows.** Each row is reduced by its gcd before the checks; a backlog item before release. N2 (`eliminate` returns `.fits = fits`) goes with it.
5. **Later units.** The hci reading's N6 and the README section at the M5 gate; no change-finding factory in v1; `witt_change` after v1; the `tools/mutate` items as a tools unit.
