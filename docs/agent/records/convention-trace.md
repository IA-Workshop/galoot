# Convention traceability table

This table maps each implemented CONVENTIONS item to the facts that pin it and to the namespaces of the generated golden header `test/reference/golden.hpp` that hold its values. Tests never cite item numbers (STYLE S11, decision 0034); a test pins an item by stating one of its facts, in the words of the Fact column, in its `// Reference:` marker. `tools/test_references.py trace` reads this table (decision 0071):

- every item here exists in CONVENTIONS.md;
- the golden namespaces here are exactly those of `golden.hpp`;
- a test case that uses a golden namespace states that namespace's fact;
- it prints each item with the test cases that state one of its facts, and the items no test case pins.

Rules for editing: a fact is a short phrase of mathematics that a reader of the test understands without this table; it is matched as written, after runs of whitespace are collapsed to one space; no two rows share a golden namespace. A new golden namespace needs its row in the same change, and a new test that pins an item needs no change here if it states an existing fact. Change a fact only together with every marker that states it.

| Item | Fact | Golden values |
| --- | --- | --- |
| 1.1 | bit i - 1 of a blade mask stands for e_i | `blade_names` |
| 1.1 | e45 = e4 ^ e5 in conformal(3) | `wedge_basis` |
| 1.2 | R(p, q, r) squares p generators to +1, q to -1, and r to 0 | `diagonal_signatures` |
| 1.3 | rigid(d) is R(d, 0, 1) | `rigid` |
| 1.4 | conformal(d) has null e(d+1) and e(d+2) with e(d+1) . e(d+2) = -1 | `conformal` |
| 1.5 | the antiscalar is e12...n with sign +1 | `units` |
| 1.6 | an oriented name lists generators in the order of their product | `oriented_names` |
| 1.7 | results are ordered by grade, then by index list | `result_order` |
| 2.1 | the reverse of a grade-k blade is (-1)^(k(k-1)/2) times it | `grade_signs` |
| 3.1 | u ^ right(u) = antiscalar and left(u) ^ u = antiscalar | `complements` |
