# Convention traceability table

This table maps CONVENTIONS items to the facts that pin them. Tests never cite item numbers (STYLE S11, decision 0034): a test pins an item by stating one of its facts in its `// Reference:` marker, and the generated golden header states the fact of each of its namespaces on a `// Fact:` line. Decisions 0071 and 0072 give the design.

## The gate command

An agent runs this at each merge into `develop` and at each milestone gate, and quotes its output in the merge message or gate report. It is not a CTest, because it reads this table and the conventions specification, which are agent documents (decision 0072, review finding B1 of `reviews/2026-10-31-m0-scripts.md`).

```bash
python3 -I tools/test_references.py trace \
  docs/agent/records/convention-trace.md docs/agent/governance/CONVENTIONS.md \
  test/reference/golden.hpp test
```

It fails when an item here is missing from CONVENTIONS.md, when an item of CONVENTIONS.md has no row here, when a line that starts a row does not have exactly three cells, when a fact of `golden.hpp` is missing here, when Implemented is not `yes` or `no`, or when a row marked implemented has no test that states that row's fact (decision 0082). It prints each item with the test cases and marked `static_assert`s that pin it, and the items with no test. The CTests `tests.reference_markers` and `tests.golden_facts` check the markers and the golden facts on every build; they read only `test/`.

## Rules for editing

- A fact is a short phrase of mathematics that a reader of the test understands without this table. It is matched ignoring case and runs of whitespace, with ∧ read as ^ and − as -.
- Every item of CONVENTIONS.md has at least one row, and a fact never contains `|`.
- Each fact of `golden.hpp` appears here, as written on its `// Fact:` line. A new golden namespace needs its row in the same change.
- Set Implemented to `yes` in the change that implements the item in `include/galoot/core/` or in a model, and to `no` otherwise. From then on the gate command fails until a test states that row's fact; a test of another row's fact does not count.
- Change a fact only together with every marker and `// Fact:` line that states it.

| Item | Fact | Implemented |
| --- | --- | --- |
| 1.1 | bit i - 1 of a blade mask stands for e_i | yes |
| 1.1 | e45 = e4 ^ e5 in conformal(3) | yes |
| 1.2 | R(p, q, r) squares p generators to +1, q to -1, and r to 0 | yes |
| 1.3 | rigid(d) is R(d, 0, 1) | yes |
| 1.4 | conformal(d) has null e(d+1) and e(d+2) with e(d+1) . e(d+2) = -1 | yes |
| 1.5 | the antiscalar is e12...n with sign +1 | yes |
| 1.6 | an oriented name lists generators in the order of their product | yes |
| 1.7 | results are ordered by grade, then by index list | yes |
| 2.1 | the reverse of a grade-k blade is (-1)^(k(k-1)/2) times it | yes |
| 3.1 | u ^ right(u) = antiscalar and left(u) ^ u = antiscalar | yes |
| 3.2 | in rigid(3) the bulk is the part without e4 and the weight the part with it | no |
| 3.3 | a ^ bulk_dual(b) = (a . b) antiscalar for a and b of equal grade | no |
| 3.4 | in a degenerate metric the double bulk dual is zero | no |
| 4.1 | the wedge of two blades that share a generator is zero | yes |
| 4.1 | e_i e_j + e_j e_i = 2 g_ij | yes |
| 4.2 | right(a ^ b) = right(a) v right(b) | yes |
| 4.3 | a point projected onto a unitized plane has weight +1 | no |
| 4.4 | transform(x, V) has the blade set of x | no |
| 4.5 | truncate checks that the part it drops is zero | no |
| 5.1 | a point x e1 + y e2 + z e3 + w e4 has weight w | no |
| 5.2 | the line through unitized points p and q has direction q - p | no |
| 5.3 | a point p lies on the plane g when p v g = 0 | no |
| 5.4 | applying Q1 and then Q2 is one transform by geometric_antiproduct(Q2, Q1) | no |
| 5.5 | the translator t_x e23 + t_y e31 + t_z e12 + antiscalar moves points by 2t | no |
| 5.6 | a reflected point keeps a positive weight | no |
| 5.7 | rotation(l, theta) turns by theta | no |
| 6.1 | the round point with center p and squared radius r^2 is p + e4 + (p^2 + r^2)/2 e5 | no |
| 6.2 | a flat object is a rigid object wedged with e5 | no |
| 6.3 | the round bulk has neither e4 nor e5 | no |
| 6.4 | rounds and flats have a carrier, cocarrier, center, container, partner, attitude, and radius | no |
| 7.1 | a * b is the geometric product geometric_product(a, b) | no |
