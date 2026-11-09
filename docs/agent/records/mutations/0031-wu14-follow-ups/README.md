# Plan 0031's mutants and seeds: the generator rule and the recursion

Run on 2026-11-09 (UTC) by Claude (agent, Opus 5.5, implementer of plan 0031), every build and test through `tools/timed` with the shared job pool. Spec: `theorem_r.toml`, judged by `core.identities` (the suite the two new cases extend, decision 0218 item 5). Rows: `out/theorem_r-<preset>.tsv` at the tip, `out/branch-point/` at the branch point, `out/first-run/` for the first run of `recursion_contraction_doubled` before its edit was narrowed.

- **Branch point:** a worktree detached at `774ff66` (the claim; its tests are those of `develop` at `ba12e5a`), the spec copied outside the repository, run as `tools/mutate <copy>` and then `tools/mutate <copy> recursion_contraction_doubled`.
- **Tip:** `feature/wu14-follow-ups`, `tools/mutate theorem_r.toml` at `98fd0c2`, then `tools/mutate theorem_r.toml recursion_contraction_doubled` at `6e34e67` after the edit was narrowed. The test sources are the same at both revisions.

## Verdicts

| Mutant | Branch point, gcc-debug / clang-debug | Tip, gcc-debug / clang-debug | Cases that fail at the tip |
| --- | --- | --- | --- |
| `resigned_e12` | survived / survived | killed / killed | GR "a generator times a blade is its contraction plus its wedge over many forms"; RC "a product of blades follows the recursion on its lowest generator over many forms" |
| `recursion_contraction_doubled` | killed / killed (by AP, AA, IP, BP) | killed / killed | RC, and AP, AA, IP, BP as at the branch point |

- **The gap.** `resigned_e12` is the product on the basis in which e12 stands for −e12, an isomorphic, associative table. The suite before this unit lets it survive on both compilers: associativity, the De Morgan laws (the antiproducts are built from the same table through the complements), the involution rules, the anticommutator of vectors, and the basis-blade proof all hold for it. Both new cases kill it on both compilers, so the family closes a gap of the suite it extends, as decision 0218 item 5 reads criterion 12.
- **Each new case kills a mutant.** GR kills `resigned_e12`; RC kills both. `recursion_contraction_doubled` changes only products whose left blade has grade 2 or more in non-diagonal forms, so the generator rule passes on it, as designed; it is RC's own mutant, and the triple loops kill it too (as expected, since a table that obeys the generator rule and is associative obeys the recursion).
- **The first run of `recursion_contraction_doubled`** (`out/first-run/`) doubled the term at every n. On clang-debug it left the mutant unjudged at both points: `identities_five_dense_test.cpp` failed to compile ("constexpr variable 'count' must be initialized by a constant expression", `plan.hpp:553`), the dense forms of five exceeding Clang's constant-evaluation limit once their products grew. gcc-debug killed it at both points (RC among the failed cases at the tip). The spec now doubles it only for n ≤ 4 (`6e34e67`), and both compilers judge it.
- **No survivor at the tip.** No mutant is declared equivalent.
- **The new forms (K9) have no mutant of their own.** They extend existing cases, not new checks, and no code of the core depends on where a form's radical lies (the products, duals, and complements never test a diagonal entry or the form's shape), so a fault only they could reach would be contrived. They are evidence of signature coverage, not of kill power.

## Seeds (criterion 8)

`seeds/`: `core_identities_test` and `core_dual_identities_test`, each with `--rand-seed=1` to `50`, on gcc-debug at `6e34e67` (the test sources of the tip), rows in `seeds/runs.tsv` (a campaign log) and `seeds/results.tsv`. All 100 runs pass: `core_identities_test` 9 cases and 5,074 assertions per seed, 15.6 to 16.0 s each; `core_dual_identities_test` 3 cases and 2,691 assertions, under 0.1 s each. 64 of the 100 rows (seed 19 onward) carry the revision `6e34e678783a-dirty`: the programs were built from the clean tree at 04:06:49Z and nothing was rebuilt in the worktree until the campaign ended at 04:20:23Z, so every run tested `6e34e67`'s programs; the files that differed were most likely records being written meanwhile (inferred, not recoverable from the rows). Fifty seeds are a sample, not a proof (V9); the generator rule and the recursion draw nothing, so the seeds bear only on the existing cases over the new forms and the old ones.

## Draws unchanged

The new forms are a group checked after every other group (`identities_checks.hpp`'s `groups`) and a list checked last in the dual suite's two cases, and the new cases draw nothing, so every multivector an existing case drew from a seed is drawn in the same order from the same source as before. This is by construction, not by a run.
