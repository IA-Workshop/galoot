# Verification, validation, and uncertainty quantification

Status: ratified by the owner on 2026-11-01 (decision 0094).

This document defines what counts as evidence that Galoot is correct and how numerical uncertainty is handled. It adds detail to C7, C11, C14, and C15 of the [constitution](CONSTITUTION.md); [WORKFLOW.md](WORKFLOW.md) says when in a unit of work the evidence is produced.

Verification shows that the implementation meets its specification: the conventions specification and the mathematical definitions it cites. Validation shows that the specification is the mathematics users mean, and that a model serves the uses it was built for. Uncertainty quantification states how far a computed value can be from the exact one. Passing one of the three is never evidence for another.

Galoot's structure is exact (C7), so with an exact scalar type the uncertainty of a result is zero and the claim is equality. Uncertainty enters only through rounding in the scalar type, through sampling in randomized tests, and through sources not yet confirmed.

## Evidence

### V1 Kinds of evidence

Evidence is one of the following, in order of preference:

1. A proof or derivation from the definitions. An exhaustive check over a finite structure, such as every entry of a generated table, is a proof.
2. An identity the structure must satisfy for all inputs, checked exhaustively where the inputs are finite and by randomized tests otherwise.
3. An exact value computed independently from the definitions, by a route that shares no code with Galoot.
4. A black-box comparison with another implementation, made only after evidence of kinds 1 to 3 passes. Outputs are compared, never code. A disagreement is investigated and recorded (C11), and never settled by adopting the other implementation's answer without independent evidence.

A test that only restates the current output is a regression guard. It is labeled as one and is never cited as evidence of correctness.

### V2 Tests

Each test fixes one behavior, names its reference, and states its tolerance with the reason for it (V12). Every test is able to fail; one that cannot is removed.

### V3 Identity suite

The core's identity suite covers at least associativity of each product; the De Morgan laws that relate each product to its antiproduct through complements; the involution and complement relations the mathematics states; the defining relations of each signature; and agreement between different bases of one algebra under the exact change of basis. It runs over many signatures, including degenerate, indefinite, and non-diagonal ones, not only the shipped models.

### V4 Checks at generation

Properties of generated data that are cheap to check are checked when the data is generated, so a wrong table cannot be used. Checks too costly for every build run in the test suite.

### V5 Bottom-up verification

Verification follows the dependency order: basis rules, signatures, basis sets, generated term lists, multivectors, operations, models, and extensions. A failure at a higher level is traced to the lowest level where a test can detect it, and a test is added at that level.

### V6 Conventions under test

Each convention in the conventions specification has a test that fails if the convention changes. Each recorded disagreement between sources (C11) has a test that tells the alternatives apart. Code conventions are checked mechanically, by the formatter and static checks of [STYLE.md](STYLE.md) (S3, S4), before a change merges to `develop`.

### V7 Reproducible randomized tests

Randomized tests draw from a seeded source, report the seed on failure, and vary signatures as well as values. Each failure found this way is kept as a fixed test.

## Validation

### V8 Referents

A generated structure is validated against the mathematics it claims to be: definitions, theorems, classifications, and worked examples in the published literature. A model is validated against its intended uses through end-to-end examples whose answers are known independently, such as incidences, transforms, and predicates checked against exact arithmetic. Where no independent referent exists, the report says that validation was not possible.

### V9 Sources and sampled claims

A source is cited only after someone has read it and confirmed that it supports the specific claim, and the research note records who did. A claim supported only by random sampling states the number of samples, the seed, and what was observed; it supports a hypothesis, not a theorem. Such a claim, including those the design study reports as checked numerically, becomes a reference for the core only after evidence of kind 1 or 2 (V1). A formula or table from a published source is a reference only after it is recomputed from the definitions, and an erratum found this way is recorded in a research note. A source that could not be retrieved is recorded as not read. A substitute, such as a preprint, an abstract, or a secondary account, is named as such, with how it may differ from the source. Quoted external text is marked as a quotation with its source.

### V10 Status in reports

Every reported result and every release states, separately, whether verification and validation were performed and whether they passed. A result that lacks either is labeled unverified or unvalidated. A reported value or finding names the revision, command, and seed that produce it, and a result that comes from unrecorded or scratch steps is labeled not reproducible until its steps are committed.

## Uncertainty

### V11 Exact scalars

With an exact scalar type, results are compared by exact equality, and any difference is a defect. Such results are reproducible bit for bit.

### V12 Rounding scalars

With a scalar type that rounds, each comparison uses a tolerance fixed before the result is seen, derived from an error bound for the computation or from an exact or higher-precision reference. The generator knows the terms and coefficients of every generated operation, so a forward error bound is available for each. A tolerance is the tightest one that admits every known source of difference. Changing a tolerance is a decision (C18) that keeps the original in the record and rests on evidence other than the comparison that failed. A tolerance is never loosened only to make a test pass.

### V13 Signs and zero tests

Where a computation is used for its sign or for a zero test, the correct answer is the exact sign. A rounded evaluation decides only when its error bound excludes zero; otherwise the decision falls to exact arithmetic. Such predicates are tested on near-degenerate inputs whose answers come from exact arithmetic.

### V14 Declared variation

Rounded results may vary with operation order, fused operations, or platform. Such variation is declared, and variation within the tolerance is not a defect. Bitwise identity of rounded results is pursued only where an acceptance criterion requires it.

## Cost

### V15 Performance evidence

A claim about run time or generation cost rests on measurements with recorded inputs: the source revision, toolchain, machine, options, and workload. An optimized form is compared against the direct derived form it replaces (C3), for both results and cost.

## Maintaining evidence

### V16 Evidence after a change

Each change reruns the evidence it can affect. Failing evidence is repaired, or the claim it supported is withdrawn. An expected value changes only when its reference is shown to be wrong, and the change is recorded with the reason.
