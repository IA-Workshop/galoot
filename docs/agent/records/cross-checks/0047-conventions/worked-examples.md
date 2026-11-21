# Worked examples of the conventions, for plan 0049

Part of the [V3 record](README.md). Plan 0047 section 9.3 item 1 hands plan 0049 "V3's derivations of each convention, with worked examples", to restate in its own words in the conventions chapter (decision 0253 item 26: a restatement kept in step by a trace). Each example below is small enough to do by hand, says which convention it shows, and gives the one-line reason. Every number was computed by hand and then by the independent engine of this record (the script named in brackets), and every one that a test or `golden.hpp` also states was compared with it and agreed.

Notation as in CONVENTIONS: n generators, 𝟙 = e12…n, ⟑ the geometric product, ⟇ the antiproduct, ∧ the wedge, ∨ the antiwedge, right and left the complements, u★ the bulk dual, u☆ the weight dual. In rigid(3) a point is (x, y, z, w) on e1, e2, e3, e4, a line (v | m) on e41, e42, e43 | e23, e31, e12, and a plane (gx, gy, gz, gw) on e423, e431, e412, e321.

## 1. Blades and their names

**A mask names a blade.** Bit i − 1 stands for e_i, so the mask 0b1011 has bits 0, 1 and 3 and is e124 = e1 ∧ e2 ∧ e4. [s1]

**A blade is a wedge, not a product.** In conformal(3), e4 and e5 are null with e4 · e5 = −1. For two vectors the geometric product is the dot plus the wedge, so e4 ⟑ e5 = e4 · e5 + e4 ∧ e5 = −1 + e45. The blade e45 is the wedge alone; the product has an extra scalar. In a diagonal metric the two agree because distinct generators have dot 0. [s1, golden `wedge_basis`]

**An oriented name is the product in the written order.** e431 = e4 ∧ e3 ∧ e1. Sorting 4, 3, 1 into 1, 3, 4 takes one swap (exchange the 4 and the 1), so e431 = −e134. Likewise e423: 4, 2, 3 needs two swaps to become 2, 3, 4, so e423 = +e234; and e41 = −e14 by one swap. [s1, golden `oriented_names`]

**The order of an untyped result.** By grade, then by index list: 1; e1, e2, e3, e4; e12, e13, e14, e23, e24, e34; e123, e124, e134, e234; e1234. [s1]

**A conformal change of basis.** conformal(3) to R(4, 1, 0) keeps e1, e2, e3 and writes the origin as e4 = ½(e′5 − e′4) and infinity as e5 = e′5 + e′4. Check: e4 · e4 = ¼(e′5 · e′5 + e′4 · e′4) = ¼(−1 + 1) = 0, and e4 · e5 = ½(e′5 · e′5 − e′4 · e′4) = ½(−1 − 1) = −1, as conformal(3) requires. The blade e45 maps to the 2 × 2 minor of the last two rows, det [[−½, ½], [1, 1]] = −½ − ½ = −1, so e45 ↦ −e′45 and 𝟙 ↦ −𝟙′. [s1, golden `basis_changes`]

**The bulk depends on the basis.** In conformal(3) the bulk of e4 is G e4, the fourth column of the Gram matrix: −e5. Computed in R(4, 1, 0) instead, the image ½(e′5 − e′4) has bulk ½(−e′5 − e′4) = −½(e′5 + e′4), which is −½ e5. The same vector, two bases, two bulks: a change of basis does not preserve the bulk unless P Pᵀ G = G. [s1, c2]

## 2. Grade operations

For n = 4, grade k and antigrade m = 4 − k:

| k | m | reverse (−1)^(k(k−1)/2) | antireverse (−1)^(m(m−1)/2) | grade involution (−1)^k | antigrade involution (−1)^m |
| --- | --- | --- | --- | --- | --- |
| 0 | 4 | + | + | + | + |
| 1 | 3 | + | − | − | − |
| 2 | 2 | − | − | + | + |
| 3 | 1 | − | + | − | − |
| 4 | 0 | + | + | + | + |

Why the reverse sign: reversing e1 ∧ e2 ∧ e3 to e3 ∧ e2 ∧ e1 takes three swaps (k(k − 1)/2 = 3), so e123 reverses to −e123. Why the grade involution: it replaces every vector v by −v, and a grade-k blade has k vectors. When n is even the antigrade involution equals the grade involution (the last two columns agree); when n is odd it is its negation, because (−1)^(n−k) = (−1)^n (−1)^k. [s23, golden `grade_signs`]

## 3. Complements and duals

**Right complements in rigid(3)**, from u ∧ right(u) = 𝟙: e1 ↦ e234, e2 ↦ −e134, e3 ↦ e124, e4 ↦ −e123. Check e2: e2 ∧ (−e134) = −e2134 = +e1234, since moving e2 past e1 is one swap. The left complement of a vector is the negated right one here, because gr · ag = 1 · 3 is odd. [s23, golden `complements`]

**The bulk and the weight in a diagonal metric.** In R(1, 1, 0), e2 squares to −1, so the bulk of e2 is −e2. In rigid(3), the bulk drops every blade with e4 and the weight keeps only those: the bulk of e14 is 0 and its weight is e14. In R(2, 0, 2), the blade e3 has a null generator (so no bulk) and lacks the null e4 (so no weight either): it is in neither. [s23, c2]

**The duals of a point.** For p = (2, 3, 5, 7): the bulk drops e4, so p★ = right(2 e1 + 3 e2 + 5 e3) = 2 e234 − 3 e134 + 5 e124, which in the oriented names is the plane (2, 3, 5, 0). The weight keeps e4 only, so p☆ = right(7 e4) = −7 e123 = the plane (0, 0, 0, 7). The left duals are (−2, −3, −5, 0) and (0, 0, 0, −7). [c2, the dual tests]

**Why the right dual.** For two blades of one grade, a ∧ right(e_C) is 𝟙 when a = e_C and 0 otherwise. So a ∧ b★ = (a · b) 𝟙 for any metric. With the point above, p ∧ p★ = (4 + 9 + 25) 𝟙 = 38 𝟙 (e4 is null, so w does not count), while p ∧ left_bulk_dual(p) = −38 𝟙. Only the right dual keeps the sign. [s23]

**The double dual in rigid(3).** e1★ = e234, which has e4, so its bulk is zero and e1★★ = 0. Every double dual is zero in a degenerate metric, since the first dual always produces something the second one's bulk discards. [s23]

## 4. Products

**The dot is not the scalar part of the product.** In R(3, 0, 0), e12 ⟑ e12 = −1, but e12 · e12 = +1. The dot is the scalar part of reverse(a) ⟑ b: reverse(e12) = −e12, and −e12 ⟑ e12 = +1. [s4]

**De Morgan, by hand.** In rigid(3): right(e1 ∧ e2) = right(e12) = e34. On the other side, right(e1) ∨ right(e2) = e234 ∨ (−e134). By the definition a ∨ b = left(right(a) ∧ right(b)): right(e234) = −e1 and right(−e134) = −e2, their wedge is (−e1) ∧ (−e2) = e12, and left(e12) = e34 (for a bivector in four dimensions gr · ag = 4 is even, so left and right agree). Both sides are e34. Why it always works: applying the right complement twice multiplies a grade-k blade by (−1)^(k(n−k)), which is the identity when n is odd and the grade involution when n is even; both are compatible with every product, so the complement turns each product into its antiproduct in every metric. [s4]

**Projecting a point onto a plane.** The plane z = 2 is (0, 0, 1, −2), since a point lies on it when z − 2w = 0. project((1, 1, 7, 1), g) = g ∨ (p ∧ g☆) = (1, 1, 2, 1): the foot of the perpendicular, with weight +1 because the plane's normal has length 1. For a normal of length N the weight is N². [s5, c2]

**A reflection in odd n.** In R(3, 0, 0) the hyperplane h = e12 + e13 has λ = 2 (h ⟇ antireverse(h) = 2𝟙). The vectors x with x ∨ h = 0 are spanned by e1 and e2 + e3, and the transform doubles them: e1 ↦ 2 e1, (0, 1, 1) ↦ (0, 2, 2). The normal (0, 1, −1) goes to (0, −2, 2), minus twice itself. A general vector (1, 2, 3) goes to (2, 6, 4). [c2]

**Composition order.** Let Q1 be the rotor about the z axis with sin φ = 3/5, cos φ = 4/5, and Q2 the translator by (2, 4, −2). Turning (1, 0, 0) first gives (7/25, 24/25, 0), then moving gives (57/25, 124/25, −2). Moving first gives (3, 4, −2), then turning gives (−3, 4, −2). compose(Q2, Q1) = Q2 ⟇ Q1 gives (57/25, 124/25, −2): Q1, the right operand, acts first. [c2]

## 5. Rigid objects

**A line through two points.** p = (1, 0, 0, 1) and q = (1, 0, 5, 1): direction q − p = (0, 0, 5), moment p × q = (0 · 5 − 0 · 0, 0 · 1 − 1 · 5, 1 · 0 − 0 · 1) = (0, −5, 0). [s5]

**A plane through three points.** (0, 0, 1), (1, 0, 1), (0, 1, 1) run counterclockwise seen from above. Normal (q − p) × (r − p) = (1, 0, 0) × (0, 1, 0) = (0, 0, 1); offset −p · n = −1. The plane is (0, 0, 1, −1), and (0, 0, 1, 1) ∨ it = 1 − 1 = 0: the point lies on it. [s5]

**Meets and their signs.** The planes x = 0 and y = 0, g = (1, 0, 0, 0) and h = (0, 1, 0, 0), meet in g ∨ h, whose direction is n_h × n_g = (0, 1, 0) × (1, 0, 0) = (0, 0, −1): the z axis, pointing down. Swapping the operands reverses it. The planes x = 1, y = 2, z = 3, (1, 0, 0, −1), (0, 1, 0, −2), (0, 0, 1, −3), meet in the point (1, 2, 3, 1); its weight is det[n_g; n_h; n_k] = 1, and a weight of 0 would mean the three normals are dependent (no single point). [s5, golden `rigid_meets`]

**A translator.** e23 + 2 e31 − e12 + 𝟙 has t = (1, 2, −1) and moves points by 2t: (5, 7, −1, 1) ↦ (7, 11, −3, 1). [c2]

**A rotor.** With sin φ = 3/5 and cos φ = 4/5, cos 2φ = 7/25 and sin 2φ = 24/25. The rotor (3/5) e43 + (4/5) 𝟙 about the z axis takes (1, 0, 0) to (7/25, 24/25, 0): a turn by 2φ, counterclockwise seen from +z. About the parallel axis through (1, 0, 0) (direction (0, 0, 1), moment (1, 0, 0) × (0, 0, 1) = (0, −1, 0)) it takes (2, 0, 0) to (32/25, 24/25, 0) and leaves (1, 0, 5) on the axis in place. [c2, golden `rigid_rotations`]

**The constructors take whole angles.** rotation(l, θ) builds the rotor with φ = θ/2, so a raw coefficient holds sin(θ/2). With l along z given as (0, 0, 5 | 0, 0, 0), the constructor divides by the length 5 and builds the same motor as (0, 0, 1 | 0, 0, 0). translation(1, −2, 4) puts (½, −1, 2) on e23, e31, e12, and moves (x, y, z) to (x + 1, y − 2, z + 4). [s5]

**A reflection keeps the weight.** The flector e412 is the plane z = 0. −F ⟇ x̂ ⟇ F̰ takes (1, 2, 3, 1) to (1, 2, −3, 1). Without the grade involution, −F ⟇ x ⟇ F̰ would give (−1, −2, 3, −1): the same place with weight −1. The plane z = 1, (0, 0, 1, −1), reflects to z = −1 with the same normal, (0, 0, 1, 1); the single-sign form would give (0, 0, −1, −1). [s5, c2]

**Motors and flectors close.** A motor has the eight even-grade blades and a flector the eight odd ones. In rigid(3) the antiproduct adds antigrades mod 2 and n = 4 is even, so grade parity and antigrade parity agree: motor ⟇ motor is a motor, and motor ⟇ flector a flector. [s5]

**A crystal class in two settings.** The class 2/m is a twofold axis with a mirror perpendicular to it. With the axis along z or along x, the two groups are different sets of matrices, but the half-turn about (1, 0, −1)/√2 swaps x and −z and carries one group onto the other. Same order, same traces, and an explicit conjugating motor: that is what "differs only by setting" means. [c3]

## 6. The round point (cga3, not yet a preset)

The round point with center (1, 2, 3) and r² = 4 is p + e4 + ½(14 + 4) e5 = e1 + 2 e2 + 3 e3 + e4 + 9 e5. Its square is p · p + 2 · 9 · (e4 · e5) = 14 − 18 = −4 = −r². With r = 0 it would be null. [s67]

## 7. Operators

`a ^ b == c` parses as `a ^ (b == c)`, because `^` binds more loosely than `==`; so does `&`. Every expression that mixes them with other operators needs parentheses. [s67, both compilers]
