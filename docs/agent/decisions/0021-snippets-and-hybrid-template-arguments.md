# 0021 Snippets directory, and hybrid template arguments

- Status: accepted; the STYLE S7 amendment and the W6 row were ratified by the owner on 2026-10-31
- Date: 2026-10-31
- Decided by: the owner, 2026-10-31 ("make a new snippets/ folder ... the coalesce analysis should run at the end of a milestone. Approve hybrid"). Recorded by Claude (agent).

## Snippets

Context: tools made in the workbench are lost when it is cleared, and several are reusable (the peak-memory launcher, the table diff, the fault injector).

Decision: `docs/agent/snippets/` holds coalesced reusable tools, with a README of rules and an index. A coalescing review runs at the end of every milestone and is cited in the gate report (plan 0001 section 6). W6 gains a row for the directory; that governance edit is proposed below.

## Hybrid template arguments

Context: Spike A (note 0008) measured value, tag, and hybrid template arguments. Mangled name length for a vector-by-vector product was 350 (GCC) and 418 (Clang) for values, 111 and 111 for tags, and 235 and 256 for the hybrid. GCC's diagnostic for value arguments misprints a metric (conformal(3) read as diag(1,1,1,-1,-1)); tag and hybrid diagnostics name the algebras on both compilers.

Options: values only (S7 as written); tags only; hybrid.

Decision: a signature is a tag type that carries its value (the algebra's name appears in diagnostics and mangled names), and blade sets are values passed as class-type template arguments. Generation remains ordinary `constexpr` code over the values (S7). Spike A's finding stands that the tag form costs a type per signature, and that is accepted.

Consequences: S7 changes when the owner ratifies the amendment below. The amendment was ratified on 2026-10-31.

Proposed S7 amendment, replacing the first sentence: "A signature is a tag type that carries its value, so diagnostics and symbols name the algebra, and blade sets and other structural data are values passed as class-type template arguments, not encoded as lists of types."

Proposed W6 row: "`docs/agent/snippets/` | Reusable tools coalesced from the workbench and spikes, with the milestone reviews that maintain them."
