# Convention traceability table

This table maps CONVENTIONS items to the facts that pin them. Tests never cite item numbers (STYLE S11, decision 0034): a test pins an item by stating one of its facts in its `// Reference:` marker, and the generated golden header states the fact of each of its namespaces on a `// Fact:` line. Decisions 0071 and 0072 give the design.

## The gate command

An agent runs this at each merge into `develop` and at each milestone gate, and quotes its output in the merge message or gate report. It is not a CTest, because it reads this table and the conventions specification, which are agent documents (decision 0072, review finding B1 of `reviews/2026-10-31-m0-scripts.md`).

```bash
python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test
```

It fails when an item here is missing from CONVENTIONS.md, when a fact of `golden.hpp` is missing here, when Implemented is not `yes` or `no`, or when an implemented item has no test that states one of its facts. It prints each item with the test cases and marked `static_assert`s that pin it, and the items with no test. The CTests `tests.reference_markers` and `tests.golden_facts` check the markers and the golden facts on every build; they read only `test/`.

## Rules for editing

- A fact is a short phrase of mathematics that a reader of the test understands without this table. It is matched ignoring case and runs of whitespace, with ∧ read as ^ and − as -.
- Each fact of `golden.hpp` appears here, as written on its `// Fact:` line. A new golden namespace needs its row in the same change.
- Set Implemented to `yes` in the change that implements the item in `include/galoot/core/` or in a model, and to `no` otherwise. From then on the gate command fails until a test states one of the item's facts.
- Change a fact only together with every marker and `// Fact:` line that states it.

| Item | Fact | Implemented |
| --- | --- | --- |
| 1.1 | bit i - 1 of a blade mask stands for e_i | no |
| 1.1 | e45 = e4 ^ e5 in conformal(3) | no |
| 1.2 | R(p, q, r) squares p generators to +1, q to -1, and r to 0 | yes |
| 1.3 | rigid(d) is R(d, 0, 1) | yes |
| 1.4 | conformal(d) has null e(d+1) and e(d+2) with e(d+1) . e(d+2) = -1 | yes |
| 1.5 | the antiscalar is e12...n with sign +1 | no |
| 1.6 | an oriented name lists generators in the order of their product | no |
| 1.7 | results are ordered by grade, then by index list | no |
| 2.1 | the reverse of a grade-k blade is (-1)^(k(k-1)/2) times it | no |
| 3.1 | u ^ right(u) = antiscalar and left(u) ^ u = antiscalar | no |
