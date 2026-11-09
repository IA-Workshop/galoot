# 0025 The math-tools proofs behind the core, rerun on develop

Written by: Claude (agent, Opus 5.5, implementer of plan 0031), 2026-11-09 UTC, on `feature/wu14-follow-ups`, after decision 0218 item 3. Not yet reviewed by the owner or by the unit's blind reviewer.

## What this note is

The owner's math-tools project holds machine-checked proofs and computations about the mathematics Galoot implements, run by its proof harness from one manifest. Galoot does not depend on math-tools: no build, test, or header refers to it (C13). This note records which of those results bear on the core, what each establishes, and the harness's verdict on each when rerun against `develop`'s headers. A result here is evidence about the mathematics, and for three obligations about the library's own functions compiled outside Galoot; it is not a test of Galoot and replaces none.

## Sources, revisions, and how the run was made

| Item | Revision or hash |
| --- | --- |
| math-tools, branch `develop` | `d698d344ba31bf0e72897dbf49a15ad34ca6ce66`, clean |
| Manifest `exercises/logic/proof-obligations.toml` | SHA-256 `3ba3450fe0c066f45619cec941f34dbf77e5e606e79eb9a4cde50f0404d14159` |
| knowledge base (the informal statements) | `e6057ce9377388392282bc57b08f9cb4dd702aa7` |
| Galoot headers read by ga-P8, ga-P11, ga-P15 | the main checkout's `include/` at `develop` `ba12e5a` (tree `4fd84ca51d7a3f3cfdeae5d99e93e1aaa11573cb`); `develop` moved to `7d9769b` during the run, with no change under `include/` |
| Tools, as each row records them | Rocq 9.3.0; Lean 4.36.0-rc2 (commit b35cb10c); Isabelle2026; Z3 5.3.3; Python 3.14.9; g++ 16.2.1 and clang++ 23.1.4 for the conformance programs |

The commands, from the math-tools checkout:

```
GALOOT_INCLUDE=<galoot>/include python3 -I toolset/proof-harness/harness.py run exercises/logic/proof-obligations.toml
MT_SRC=$PWD GALOOT_INCLUDE=<galoot>/include python3 -I toolset/proof-harness/harness.py run exercises/logic/proof-obligations.toml --only ga-P7,ga-P11
```

The first ran from 2026-11-09T03:49:10Z to 04:01:59Z (12 min 49 s wall, not the three to four minutes the logician's records give; logic-E20's 32 mutants took 492 s of it). It ended with 13 of 15 obligations passing. ga-P7 and ga-P11 passed their check commands and audits, and failed only their replay: the replay command names `${MT_SRC:-$PWD}/installed/opam-root` after a `cd` into the proof directory, so with `MT_SRC` unset it looks for the opam root in the wrong place ("Opam has not been initialised"). That is a defect of the manifest, not of the proofs; it is noted here for the owner and not fixed (this unit does not edit math-tools). The second run, with `MT_SRC` set, passed both, replay included (30.7 s). The rows below are the passing rows; the two failed rows are in math-tools' ignored results file, `exercises/logic/build/proof-results.tsv`. The harness was run outside Galoot's `tools/timed`, since it is neither a build nor a test of Galoot.

What the harness checks, and what it does not: for each obligation it runs the check command, requires its PASS line, reads the proof tool's own report of what the theorems trust (axioms, oracles), rejects anything outside the obligation's allowance, scans the sources for skipped proofs and unsafe settings, and for ga-P7, ga-P9, ga-P11, and logic-E19 replays the compiled proof in a separate checker. It does not check that a statement is the right one. The statements were reviewed by the reviewers each manifest entry names (math-expert, numerical-expert, cpp-expert roles, and cold readers, 2026-11-02 and 03), not by me. I read the manifest entries, the theorem names in the sources, and the knowledge base's note on retiring exhaustive tests in full; I did not read the proof scripts.

Levels, as the harness defines them: L1 a computation (exhaustive or symbolic at fixed sizes), L2 a proof checked by one kernel, L2 with replay ("L2+R") a proof whose compiled form a second checker also accepted.

## The results

| Obligation | What it establishes (manifest statement, abbreviated) | Level | Verdict | Entry and sources hash | Bears on in Galoot |
| --- | --- | --- | --- | --- | --- |
| ga-P7 | the sign rule of blade products for all n (Rocq: the cocycle, diagonal associativity, exchange, reverse, complement signs), with sanity theorems; a mutation run kills all nine mutants of its definitions | L2+R | PASS (second run), trusts nothing | `926e0caee0fe` | `bits.hpp`'s reorder and complement signs; CONVENTIONS' sign conventions |
| ga-P8 | the word-operation lemmas that carry the sign rule to 32- and 64-bit masks (Z3), and the library's sign functions agree with the model | L1 | PASS | `1e3419809e63` | `bits.hpp`, compiled from `develop`'s headers by g++, clang++, and g++ with checking |
| ga-P9 | the guard table: the largest admissible Gram entry per n for 64- and 32-bit coefficients (Lean, `by decide`) | L2+R | PASS, trusts `propext` | `d92d8f546d31` | none today (inferred): the core bounds an entry by its 2 × 2 minors (`max_form_entry`) and checks every product coefficient during constant evaluation instead |
| ga-P10 | I I = (−1)^(n(n−1)/2) det G as a polynomial identity for every form with n ≤ 8 | L1 | PASS | `2ec96f80988d` | "The pseudoscalar squares to (-1)^(n(n-1)/2) det g" (`blade_product_test.cpp`), whose expected values it proves for those n |
| ga-P11 | Theorem M4, what the insertion merge of blade sets computes (Rocq), and the library's merge conforms | L2+R | PASS (second run), trusts nothing | `6056669dbbfb` | `blade_set.hpp`'s merge, compiled from `develop`'s headers as for ga-P8 |
| ga-P12 | the split formula equals the product, n ≤ 6, and its sign congruences at 12 generators | L1 | PASS | `fc37ac206d8a` | the integrality of product coefficients (no Galoot test cites it) |
| ga-P13 | the n = 5 rotor shortcut as ideal membership, for every diagonal signature | L1 | PASS | `6ab9c580c33e` | the versor criterion; no Galoot test relies on it (inferred) |
| ga-P14 | Theorem B2, the coefficient bound, for every symmetric G with n ≤ 6, by coefficient domination | L1 | PASS | `5e0dba772ed9` | the size of product coefficients; superseded for the factor of A by logic-E19 |
| ga-P15 | the library's blade products equal the model and an independent oracle, 6,144 products | L1 | PASS | `be8d347294b7` | `blade_product.hpp`, compiled from `develop`'s headers by g++ and clang++ with checking |
| logic-E14 | Theorem B3, (det of a k × k minor)² ≤ kᵏ g^(2k), every size (Isabelle) | L2 | PASS, trusts nothing | `c7db554d8126` | the coefficient bound |
| logic-E17 | no oracle among the theorems logic-E14 depends on | L2 | PASS (declared exceptions: no audit line, an ML block) | `42160799e0b5` | as logic-E14 |
| logic-E18 | Theorem B3's row-norm and H_k forms; the guard term dominates a binomial times a minor (Isabelle) | L2 | PASS, trusts nothing | `a3400e838f18` | the coefficient bound |
| logic-E19 | Theorem B2's factor of A for every n, over a Chevalley antisymmetrization model, with the Clifford relation and the right unit (Lean) | L2+R | PASS, trusts the standard axioms and `native_decide` on six exact sample products, as declared | `d09aa4d06457` | the coefficient bound; the model's Clifford relation is the generator rule's vector case |
| logic-E11 | the timing lock as a TLA+ model, nine models with their expected verdicts | L1 | PASS | `026733712e95` | the lock Galoot's measurement campaigns take (tooling, not the core) |
| logic-E20 | the mutation checks of logic-E14, E18, and E19: 32 mutants predicted before the run, each killed or classified | L1 | PASS | `16db03461e80` | the strength of the three proofs above |

The hash is the harness's hash of the manifest entry and its source files, so a later run whose row shows the same hash ran the same proof.

## What is not machine-checked

- **The characterization of the product by its unit, the generator rule, and the recursion** (the knowledge base's Theorem R, `geometric-algebra/proofs/test-retirement.md`). It has a structured paper proof, by induction on the grade of the left factor, which I read and whose worked case for n = 2 I rechecked: the recursion gives e12 e12 = G₁₂² − G₁₁G₂₂ = −det G. Its conditions were checked on ten oracle tables, and three corrupted tables failed them (math-tools ga-P6, 2026-11-01, not part of the harness). It is not formalized: the backlog lists it with the pseudoscalar square and the versor criterion as needing a formal multivector type with finite sums. In Galoot, plan 0031's two test cases check its conditions exhaustively on every full-layout form of the identity suite; with the paper proof, that makes those forms' products the Clifford product (V1 kind 1, resting on a paper proof).
- **Theorem B2 for n ≥ 7, and its factor of B**, the versor criterion for degenerate forms, and the joining step of logic-E19's model: open in math-tools' backlog.
- **The link from each proof's model to Galoot's code** is made only where an obligation compiles the headers (ga-P8, ga-P11, ga-P15). Those three ran against `develop`'s headers on this date; a later change to `bits.hpp`, `blade_set.hpp`, or `blade_product.hpp` needs a rerun for them to speak of it.

## Confirmed and inferred

- Confirmed by the run: every verdict, hash, level, and trust report in the table, at the revisions above.
- Read: the manifest entries; the knowledge base's Theorem R note.
- Inferred: the "bears on" column, from the obligations' statements and Galoot's headers and tests; and that ga-P9 and ga-P13 bear on no Galoot code today.
