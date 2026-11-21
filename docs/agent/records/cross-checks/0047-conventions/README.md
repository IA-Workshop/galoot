# The conventions derived again, independently, and compared (plan 0047 unit V3)

- Written by: Claude (agent, Opus 5.5, math-expert role), 2026-11-21 UTC, for plan 0047 unit V3 (sections 6, 6.1, 7 and 9.3 item 1), under decision 0253 item 1 (phase 0 is records only), on `research/conventions-rederived` from `main` at `01dce7f`.
- Specification derived against: CONVENTIONS 1.18. Library compared: `main` at `01dce7f`, read in this worktree and not modified.
- Kind of evidence: V1 kind 1 and 3 for the derivation (a derivation from the definitions, and exact values computed by a route that shares no code with Galoot); kind 4 for the galgebra check of the geometric product. Nothing here enters `test/`, `include/`, or a comment of the library, and no tool path is cited from them.
- Result: **no disagreement.** Every derivable claim of sections 1 to 5 and 7, and the specified parts of section 6, came out as CONVENTIONS states it. In the comparison step all 3,109 checks against the 23 convention namespaces of `golden.hpp` and all 43 hand values transcribed from the tests' reference comments agreed. A negative control (one flipped sign in a copy of `golden.hpp`) was caught. The gap rows G1, G2, G3, G5 and G17 of plan 0047 section 6 are closed or narrowed as section 4 below says.

Files in this record:

| File | Holds |
| --- | --- |
| [rows.md](rows.md) | One row per convention claim: the item, the derivation's result, the route, the tool and version, the command that reruns it, agreement with the specification and with what the tests pin, and what the comparison does not show |
| [worked-examples.md](worked-examples.md) | Small worked examples of each convention, by hand, for plan 0049 to restate (plan 0047 section 9.3 item 1) |

The research note behind this record is [0028](../../../research-notes/0028-conventions-rederived.md).

## 1. Departures, skipped checks, and ambiguities (C20)

1. **What counts as an item.** Acceptance criterion 5 speaks of "the 54 labeled items of CONVENTIONS sections 1 to 5 and 7", and the plan's status line counts 54 status labels in CONVENTIONS 1.18 (25 Checked, 19 Confirmed, 10 Proposed). Those are the counts of the label words in the whole file. Inside sections 1 to 5 and 7 the words occur 45 times (21 Checked, 14 Confirmed, 9 Proposed, 1 Study); the rest are in the preamble, section 6, and the revision history. Reading followed: one row per claim that carries a label, splitting a numbered item where its label splits (2.1 has five rows, 1.8 nine, 4.4 four). [rows.md](rows.md) has 56 rows on the 28 numbered items of sections 1 to 5 and 7, six on the gaps G5 and G17, and five on section 6, outside the criterion. Every numbered item of sections 1 to 5 and 7 has at least one row, and no row is blank.
2. **Where the scripts live.** The rerunnable scripts are on the math-tools branch `tools/conventions-rederived` in the worktree `math-tools-worktrees/conventions-rederived`, not merged (the brief says I merge none of the branches). Until the coordinator merges it, the commands below run from that worktree; after the merge the path is the same under `math-tools/`.
3. **The antiexomorphism is read, not given.** CONVENTIONS 3.2 says the weight "applies the antiexomorphism" and describes it only for a diagonal metric. The derivation reads it as the complement conjugate left ∘ G ∘ right. That reading reproduces 3.2's diagonal description exactly, equals det(G) times the exomorphism of G⁻¹ on every nondegenerate form tried, and is the definition `golden.hpp` states in its `duals` comment (seen only at the comparison step). Every row on the weight, the weight duals, the antidot and the projections rests on it.
4. **The antireverse and the antigrade involution are derived as complement conjugates** (left ∘ op ∘ right). CONVENTIONS defines them by their signs. The derivation shows the two agree on every blade; it does not show that the complement conjugate is the only reasonable definition.
5. **Section 6 is outside criterion 5** and was derived only where it is specified: the round point's square (6.1) and, up to a factor (sign included) the specification does not fix, the bulk dual of a sphere. 6.2 and 6.4 are Study; of 6.2 only one consequence is derived (section 7, m2), and 6.4 is not specified.
6. **Not derivable, and said so in the row**: the bitmask encoding (1.1), the order of untyped results (1.7), the operator table (7.1) and the printed form (7.2) are naming or interface choices; 4.4's compile-time rejection and checking mode and all of 4.5 are interface behavior. For these the row says what was checked instead (consistency with other items, the C++ precedence claim on both compilers), or "not derivable".
7. **The International Tables were not read** (as for research note 0023). G17 is closed for the realization of Hestenes' angles that the earlier rebuild used, not for the Tables' standard settings.
8. **Lengyel's wiki was not re-read.** Rows whose label is Confirmed are confirmed here by derivation, not by a fresh reading of the source; the reading of note 0005 stands as it was.
9. **The second-specialist sample check** that plan 0047 section 7 asks for on V3's note is not part of this unit; it is the coordinator's to brief.
10. **No build, no test run, no job-pool use.** The pool read "running: 0 of 23 slots free" (other sessions) at 17:22 UTC and was not touched. I ran two read-only project scripts in this worktree: the trace gate (`python3 -I tools/test_references.py trace ...`, exit 0, "items with no test: 6.1, 6.2, 6.3, 6.4") and `python3 -I test/reference/conventions_check.py` ("all convention checks passed (seed 20261031)", 2.7 s). The C++ probe of 7.1 was two `-fsyntax-only` compiles of a 12-line file in the math-tools exercise folder, outside `tools/timed`, since it builds nothing of Galoot's.

## 2. How the derivation was kept independent (plan 0047 section 6.1)

Rule: derive from the definitions in CONVENTIONS and the published sources; do not read `test/reference/`, `golden.hpp` or the headers until the comparison step.

- **Read before deriving**: AGENTS.md, the constitution, WORKFLOW, VVUQ, CONVENTIONS 1.18, plan 0047, decision 0253, the math-expert role file, the math-tools and knowledge-base agent rules, the point-group row of `records/release/v10-table.md` and section G of research note 0023 (to learn what G17 asks; they name the three classes and the source but give no values), and the head of the math-tools exercise log (tool versions and the names of earlier exercises). File listings of `math-tools/exercises/ga/` and `knowledge-base/geometric-algebra/` were seen; no file in them was opened.
- **Not read before deriving**: `test/` (all of it), `include/`, `tools/`, `docs/agent/records/convention-trace.md`, the knowledge-base notes, and the earlier math-tools exercises (`ga/py/handcliff.py` and the rest).
- **Order on record**: the derivation scripts and their outputs were committed to math-tools as `dace5b74f70577fc5daf14e82d641a82bc9e1a14` at 17:11:59 UTC. The trace table, the golden header and the tests were first opened after that commit. The comparison scripts were committed as `f8416365aae1266a622424f86c35e71173092963`.
- **Shared nothing in code**: the engine (`chevalley.py`) was written for this unit. It stores blades as index tuples, not masks; it has no sign rule; it multiplies by the Chevalley construction with the antisymmetrized product of the generators' actions; complements are solved from their defining relations, the exomorphism is built by wedging images, minors and determinants come from python-flint.
- **What is shared**: the mathematics. The library's reference for general forms (`clifford_reference.py`, the contraction recursion) is the same Chevalley construction family. The independence is in the code, the order of work, and the antisymmetrization step with its own check, not in the construction. galgebra's conventions differ from Galoot's in its duals and basis names; it was used only for the geometric product, with blades built as wedges of its basis vectors, and supplies no expected value.

## 3. How to rerun

From `math-core/`, with the shared Python environment (`math-tools/toolset/env/py/.venv`):

```
MT_SRC=$PWD/math-tools GALOOT=$PWD/galoot math-tools-worktrees/conventions-rederived/exercises/ga/ga-E22-conventions-rederived/run_all.sh
```

It ends in `PASS ga-E22`. One script: `math-tools/toolset/env/py/.venv/bin/python -I <folder>/<script>.py`, and for `c1_compare_golden.py` and `c3_point_group_settings.py` add the path of `golden.hpp`. About 11 s on one core on a quiet machine (21 s at load average 11), peak memory 83 MiB; the rerun of section 7 took 12.1 s at load average 3.7, peak 77 MiB.

Tools (the Tool column of [rows.md](rows.md)):

| Code | Tool and version |
| --- | --- |
| E | the Chevalley engine `chevalley.py` on CPython 3.14.10, exact `fractions.Fraction` |
| S | sympy 1.14.0 (symbolic Gram matrices, symbolic rigid objects, nullspaces, radicals) |
| F | python-flint 0.9.0 (determinants, minors, inverses) |
| G | galgebra 0.6.0 (black box for the geometric product) |
| C | GCC 16.2.1 and Clang 23.1.5 (`-std=c++23 -fsyntax-only`) |

All from `math-tools/toolset/env/py/uv.lock`, sha256 prefix `f702149dc36559c4`. Random inputs: `random.Random(20261121)`.

Scripts (the Rerun column): `s0` engine self-checks; `s1` section 1; `s23` sections 2 and 3; `s4` section 4; `s5` section 5 and the meet signs; `s67` sections 6 and 7; `c1` golden comparison; `c2` test hand values; `c3` point-group settings. The full names are `s0_engine.py`, `s1_blades_and_bases.py`, `s2_s3_grades_complements.py`, `s4_products.py`, `s5_rigid.py`, `s6_s7_conformal_operators.py`, `c1_compare_golden.py`, `c2_compare_tests.py`, `c3_point_group_settings.py`.

## 4. The evidence gaps of plan 0047 section 6

| Gap | Disposition | Evidence |
| --- | --- | --- |
| G1, no route outside `test/reference/` | **Closed** for every derivable item: each was derived by a route that shares no code with `test/reference/`, and the golden values and the tests' hand values were recomputed by it | [rows.md](rows.md); `s0` to `s67`, `c1`, `c2` |
| G2, antiproduct, antiwedge and De Morgan on non-diagonal forms | **Narrowed.** Derived for a general symmetric Gram matrix: the De Morgan laws with both complements on every blade pair for n = 3 and 4, symbolically; antiproduct associativity on all 512 blade triples for n = 3 and on 768 of 4,096 for n = 4, symbolically; numerically on every blade pair of 16 forms. The geometric product under it was checked against galgebra on four non-diagonal forms. Remaining: n = 4 associativity symbolically on the other 3,328 triples, and n ≥ 5 symbolically; a published table for non-diagonal antiproducts is still not known to exist (none was sought in this unit) | rows 4.2a to 4.2c |
| G3, grade and antigrade involutions | **Grade involution: derived and read.** Shirokov (arXiv:1109.2322v1, section 2) defines it as the unique automorphism α with α ∘ α = id and α(v) = −v; on a wedge of k vectors that gives (−1)^k, as 2.1 states. **Antigrade involution: derived, no source.** It is the complement conjugate of the grade involution, and an automorphism of the antiproduct; no published source defining it was found or sought beyond the knowledge base's list. The status labels stay as they are; only the owner changes CONVENTIONS | rows 2.1c, 2.1d; research note 0028 section 3 |
| G5, meets validated up to sign only | **Closed for the sign.** Derived symbolically in rigid(3): g ∨ h has direction n_h × n_g and moment h_w n_g − g_w n_h; (g ∨ h) ∨ k has weight det[n_g; n_h; n_k] and its unitized point solves the three plane equations (Cramer); l ∨ g has weight −(v · n_g); p ∧ q ∧ r ∧ s has the determinant of the 4 × 4 coordinate matrix on 𝟙; l1 ∨ l2 = −(v1 · m2 + v2 · m1). Each equals the formula `golden.hpp` states in `rigid_meets` and `rigid_incidence` and all 48 seeded values there | rows G5a to G5e; worked examples |
| G17, three point groups differ from the table by setting | **Closed for the realization compared.** For 2/m, −42m and 32 a unitized motor is exhibited exactly that conjugates the library's group onto the group of Hestenes' Table 2 code in the realization of the earlier rebuild, as sets, and each library generator conjugated by it acts as an element of that group. All three motors are half-turns: 2/m about (1, 0, −1)/√2; −42m about (cos 22.5°, sin 22.5°, 0); 32 about the unit axis ((3 + √3)/6, (√3 − 3)/6, −√3/3). Hestenes' code fixes angles, not coordinates, so each library group is itself a realization of the code (`c3` exhibits the vectors; for 2/m also a = e3, b = e1, c = e2): the groups differ from the earlier rebuild's coordinates, not from Hestenes | row G17; `c3` |

## 5. Open items for the owner

None of these is a disagreement; each is a choice the specification leaves open.

1. **The antiexomorphism has no general definition in CONVENTIONS.** 3.2 defines it only through the diagonal case; the library and this derivation both use left ∘ G ∘ right. Options: (a) add one sentence to 3.2: "The antiexomorphism is left ∘ G ∘ right; for a nondegenerate form it is det(G) times the exomorphism of G⁻¹"; (b) leave it to the golden header's comment. Tradeoff: (a) is a specification change the owner must ratify, and it makes every weight row derivable from the specification alone; (b) costs nothing and leaves a reader of CONVENTIONS to guess for non-diagonal forms. Recommendation: (a), at the next CONVENTIONS revision.
2. **The 4.4 odd-sign alternative and odd n.** For odd n, −V ⟇ x ⟇ Ṽ and the specified −V ⟇ x̌ ⟇ Ṽ agree on vectors, so a test of reflections of vectors alone cannot tell them apart. The library's mirror tests use every blade (`golden::mirrors` for rigid(2), R(3,0,0), rigid(4), R(5,0,0)), so nothing is unpinned today. Options: (a) nothing; (b) a sentence in the guide's transform page (plan 0049). Recommendation: (b), as a teaching point, not a test change.
3. **A rotor about a point in rigid(2) turns clockwise.** The test "a mirror in an odd number of generators fixes its vectors times lambda" pins (3/5) e3 + (4/5) e123 taking (1, 0, 1) to (7/25, −24/25, 1). CONVENTIONS says nothing about rigid(2), so nothing disagrees; but a future rga2 preset that copies 5.5's "counterclockwise" wording would be wrong. Options: (a) record it now in CONVENTIONS as a fact about rigid(2); (b) leave it to the rga2 preset's plan. Recommendation: (b), with this record as its input.

## 6. What this record does not show

- That the specification is the mathematics users mean (validation): the derivation checks the specification against its own definitions and against classical facts (Rodrigues' formula, Cramer's rule, determinants), not against users' intent.
- That the library's code computes what the golden values say: the comparison is with `golden.hpp` and the tests' stated values, not with the library's output. The tests compare the library with those values.
- Universal statements over all inputs where a row says "random inputs" or "on these forms": those rows are sampled (seeded), and the general statement rests on the derivation written in research note 0028.

## 7. Corrections after the sample check (2026-11-21)

An independent sample check by a second math-expert agent (branch `review/conventions-rederived`, `docs/agent/records/reviews/2026-11-21-0047-v3-sample-check.md`) sampled 51 of the 67 rows, found none wrong in value, sign, verdict or classification, and made five minor findings, m1 to m5. Each was judged here against the rows, CONVENTIONS 1.18 and, where a value was at stake, a rerun. All five are confirmed and corrected; none is rejected. The exercise changes are math-tools commit `5785b29` on `tools/conventions-rederived`.

**m1 (coverage, row 1.8e): confirmed, corrected.** Of the 12 changes, the condition P⁻ᵀ G′ P⁻¹ = G held on six, all with d = ±1, where 1/d = d and 1/d² = d², so the check could not tell the stated factors from d and d². Such a change must be degenerate: det G = d² det G′ and the condition gives det G = det G′ / d², so d⁴ = 1 whenever det G ≠ 0. For G′ = diag(I, 0) and P = [[R, U], [0, S]] the condition holds exactly when R is orthogonal and U = 0, for any invertible S (derived by hand; checked with sympy for symbolic U). `s1` now adds three such changes: rigid(3) with e4 scaled by 2 (d = 2), a 3-4-5 rotation of rigid(3) with e4 scaled by −3 (d = −3), and R(2,0,2) with its radical mixed by [[1, 2], [3, 4]] (d = −2). On each, all eight operations are nonzero on the sampled inputs, equal with the stated factor, and differ with d or d² in its place. A negative control (a copy of `s1` that expects d and d²) fails on all eight operations of all three changes. Rows 1.8a to 1.8h now say 15 changes.

**m2 (scope, row 6.2): confirmed, corrected.** The row said "not derived" while one consequence is cheap to derive. `s67` now derives it symbolically: for round points a, b, c of any weight and radius, a ∧ e5, a ∧ b ∧ e5 and a ∧ b ∧ c ∧ e5 are the rigid(3) joins of their (x, y, z, w) parts wedged with e5, with the same coefficients (by hand, e5 ∧ e5 = 0 removes every e5 term of a round point). A negative control that wedges with e4 + e5 instead fails. The row's Spec cell now reads "n/d for the definition; agree for the consequence". 6.2 stays a Study definition; nothing in CONVENTIONS changes.

**m3 (wording, row 6.1b): confirmed, corrected.** "Up to the sphere's scale" hid that the factor includes the sign, which follows the order of the four points: exchanging two of them negates the wedge and so its bulk dual. The Spec cell now reads "agree up to a factor, sign included, that 6.1 does not fix; with the test's sphere coordinates, exactly", and the result cell says the −54 is for the script's order.

**m4 (wording, G17): confirmed, corrected with a run.** Hestenes' Table 2 code fixes angles, not coordinates. `c3` now carries the realization's vectors by the conjugating rotation and shows that the carried vectors have the code's angles and generate exactly the library's group, for all three classes, and that for 2/m the rational realization a = e3, b = e1, c = e2 does too. So each library group is itself a realization of the code, and "differs by setting" means differs from the earlier rebuild's coordinates. Row G17, section 4 above, the 2/m worked example, and note 0028 D11 now say so.

**m5 (rendering, row 1.8f): confirmed, corrected.** The cell "|d| = 1" had unescaped pipes, which split the row into 12 cells under a 10-column header. It now reads `\|d\|`, and its wording is corrected as well: with the m1 changes, the condition also holds with |d| ≠ 1, on degenerate forms, which the item's "for a nondegenerate form" excludes. Every table of this record and of note 0028 was then checked by splitting each row on unescaped pipes (code spans included, as GitHub does): the seven tables of `rows.md`, the three of this README, the one of `worked-examples.md` and the one of note 0028 each have the same cell count in every row as in their header. No Markdown renderer was available on the machine, so the rendering itself was not viewed.

**The rerun.** From `math-core/`, after removing the exercise's `out/` and `__pycache__/`, under `memguard.sh`:

```
$ MT_SRC=$PWD/math-tools GALOOT=$PWD/galoot math-tools-worktrees/conventions-rederived/exercises/ga/ga-E22-conventions-rederived/run_all.sh
PASS s0_engine
PASS s1_blades_and_bases
PASS s2_s3_grades_complements
PASS s4_products
PASS s5_rigid
PASS s6_s7_conformal_operators
PASS c2_compare_tests
PASS c1_compare_golden
PASS c3_point_group_settings
PASS ga-E22
```

12.1 s wall, peak tree memory 77 MiB, load average 3.7; the job pool read 23 of 23 slots free and was not used. `golden.hpp` on `main` (`c9e03bc`) is identical to the base `01dce7f`. The outputs of every script not changed (`s0`, `s23`, `s4`, `s5`, `c1`, `c2`) were reproduced byte for byte, so the counts of 3,109 golden comparisons and 43 hand values stand. The changed rows print:

```
[1.8/C1] yes: ... preserved exactly when P^-T G' P^-1 = G -> preserved iff condition on all 15 changes (condition held on 9)
[1.8/C1-factors] yes: the factors are 1/d (antiproduct, antidot, weight duals, compose) and 1/d^2 (transform, project, antiproject), not d and d^2 -> rigid(3) scale e4 by 2 (d = 2): all eight nonzero with the stated factor, wrong factor fails; rigid(3) 3-4-5 rotation, e4 scaled by -3 (d = -3): all eight nonzero with the stated factor, wrong factor fails; R(2,0,2) mix the radical e3, e4 (d = -2): all eight nonzero with the stated factor, wrong factor fails
[1.8/complements] yes: complements preserved (factor d) exactly when P orthogonal -> iff on all 15 changes (condition held on 2)
[1.8/bulk] yes: bulk preserved (factor 1) exactly when P P^T G = G -> iff on all 15 changes (condition held on 5)
[1.8/weight] yes: weight preserved (factor 1) exactly when G' P^T P = G' -> iff on all 15 changes (condition held on 5)
[6.2] n/a: ... a^e5, a^b^e5 and a^b^c^e5 equal the rigid(3) joins of their (x, y, z, w) parts wedged with e5, same coefficients: True
[G17/realizations] n/a: ... 2/m: ... True; -42m: ... True; 32: ... True; 2/m also with a = e3, b = e1, c = e2: True
```

The sha256 prefixes of the changed scripts: `s1_blades_and_bases.py` `702b69d6370f675b`, `s6_s7_conformal_operators.py` `a4cf604f5331480a`, `c3_point_group_settings.py` `a7c137aefcdc7977`.

What the corrections do not show: the "only if" halves of 1.8 are still sampled, now on 15 changes; the three new changes are all block-diagonal on the radical, so a degenerate change with a non-diagonal G′ on which the condition holds with |d| ≠ 1 was not tried.
