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
| 2.1 | the antireverse of a blade of antigrade m is (-1)^(m(m-1)/2) times it | yes |
| 2.1 | the grade involution of a grade-k blade is (-1)^k times it | yes |
| 3.1 | u ^ right(u) = antiscalar and left(u) ^ u = antiscalar | yes |
| 3.1 | the left complement of the right complement is the identity | yes |
| 3.2 | in rigid(3) the bulk is the part without e4 and the weight the part with it | yes |
| 3.2 | the metric exomorphism sends e_A to the sum of det g[A, B] e_B | yes |
| 3.3 | a ^ bulk_dual(b) = (a . b) antiscalar for a and b of equal grade | yes |
| 3.3 | the left duals are the right duals times (-1)^(gr ag) | yes |
| 3.4 | in a degenerate metric the double bulk dual is zero | yes |
| 4.1 | the wedge of two blades that share a generator is zero | yes |
| 4.1 | e_i e_j + e_j e_i = 2 g_ij | yes |
| 4.1 | the dot is a^T G b and the antidot a^T antiG b times the antiscalar | yes |
| 4.2 | right(a ^ b) = right(a) v right(b) | yes |
| 4.3 | a point projected onto a unitized plane has weight +1 | yes |
| 4.3 | the antiprojection of a plane onto a unitized point is the plane through the point with the same normal | yes |
| 4.3 | the antiprojection of a line onto a unitized point is the line through the point with the same direction | yes |
| 4.4 | transform(x, V) has the blade set of x | yes |
| 4.4 | the parity of a versor is the parity of the antigrades of its blades | yes |
| 4.4 | an odd versor V transforms x as the negated antiproduct of V, the antigrade involution of x, and antireverse(V) | yes |
| 4.4 | compose(Q2, Q1) applies Q1 first | yes |
| 4.4 | the transform by V is lambda times an outermorphism | yes |
| 4.4 | a single mirror maps each vector of the mirror to itself times lambda | yes |
| 4.5 | truncate checks that the part it drops is zero | yes |
| 4.5 | part never checks what it drops | yes |
| 4.5 | transform checks a rounding scalar's dropped part against its forward error bound | yes |
| 5.1 | a point x e1 + y e2 + z e3 + w e4 has weight w | yes |
| 5.2 | the line through unitized points p and q has direction q - p | yes |
| 5.2 | the line p ^ q has direction w_p q - w_q p and moment p x q | yes |
| 5.3 | a point p lies on the plane g when p v g = 0 | yes |
| 5.3 | the plane p ^ q ^ r takes the value det[x, p, q, r] at a point x | yes |
| 5.3 | the meets of lines and planes are the points and lines common to them | yes |
| 5.3 | p v g, p ^ l, and l v k are zero exactly when the objects are incident | yes |
| 5.4 | applying Q1 and then Q2 is one transform by geometric_antiproduct(Q2, Q1) | yes |
| 5.4 | a chain of joint motors moves points as its product of 4x4 matrices | yes |
| 5.5 | the translator t_x e23 + t_y e31 + t_z e12 + antiscalar moves points by 2t | yes |
| 5.5 | the rotor l sin(phi) + antiscalar cos(phi) turns by 2 phi about l | yes |
| 5.6 | a reflected point keeps a positive weight | yes |
| 5.6 | the 32 crystal classes close under compose with their published orders | yes |
| 5.7 | rotation(l, theta) turns by theta | yes |
| 5.7 | rotation(l, theta) turns by theta about a line whose direction has any positive length | yes |
| 5.7 | translation(dx, dy, dz) moves points by d = (dx, dy, dz) | yes |
| 5.8 | a motor is stored on e41, e42, e43, e1234, e23, e31, e12, 1 in that order | yes |
| 5.8 | a flector is stored on e1, e2, e3, e4, e423, e431, e412, e321 in that order | yes |
| 5.8 | the antiproduct of two motors or two flectors is a motor, and of a motor and a flector a flector | yes |
| 6.1 | the round point with center p and squared radius r^2 is p + e4 + (p^2 + r^2)/2 e5 | no |
| 6.2 | a flat object is a rigid object wedged with e5 | no |
| 6.3 | the round bulk has neither e4 nor e5 | no |
| 6.4 | rounds and flats have a carrier, cocarrier, center, container, partner, attitude, and radius | no |
| 7.1 | a * b is the geometric product geometric_product(a, b) | yes |
| 7.1 | a ^ b is the wedge | yes |
| 7.1 | a % b is the geometric antiproduct geometric_antiproduct(a, b) | yes |
| 7.1 | a & b is the antiwedge antiwedge(a, b) | yes |
| 7.1 | ~a is the reverse reverse(a) | yes |
| 7.1 | !a is the right complement right_complement(a) | yes |
| 7.1 | the sum and the difference of two values of one type add and subtract their coefficients place by place | yes |
| 7.1 | a scalar s times a multivector, on either side, multiplies each coefficient by s | yes |
| 7.1 | a / s divides each coefficient by s | yes |
| 7.1 | the negation of a multivector negates each coefficient | yes |
| 7.2 | format writes every component in the order of the layout | yes |
| 7.2 | a blade the algebra tag names with the layout's orientation prints by that name | yes |
| 7.2 | a blade stored with orientation -1 and no declared name prints as its canonical name preceded by - | yes |
| 7.2 | a format specification applies to each coefficient | yes |
