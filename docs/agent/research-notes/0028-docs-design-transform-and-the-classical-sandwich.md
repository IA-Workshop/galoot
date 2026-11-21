# 0028 The documentation design: what `transform` gives for a classical rotor, the route from rotors to motors, what 1.0.0 lacks for teaching, and what the two research cycles change

Written by: Claude (agent, Fable 5.2, in the math-expert and hci-expert roles; the Fable level was authorized by the owner for this unit, decision 0253 item 32), 2026-11-21, for unit U1 of plan 0049, on `records/docs-design` from `main` at `01dce7f`. Not yet reviewed. The five design records this note supports are in [`records/docs/`](../records/docs/): `learner-contracts.md`, `concept-inventory.md`, `misconceptions.md`, `authoring-rules.md`, and `notation-key.md`.

## Departures and open points first (C20)

1. **The plan's statement about `transform` is wrong, and its probe was a coincidence.** Plan 0049 section 4.2 point 2 reports that `r * v * ~r` and `transform(v, r)` gave the same vector for the rotor r = b a of chapter 5. They do for that vector and that rotor only. In R(3, 0, 0), `transform(x, r)` is the reverse of r applied to the grade involution of x (section 2.3): for a vector, the opposite turn, negated. It equals the classical turn on a vector exactly when the rotor turns that vector's plane by a quarter turn, which is the case the probe happened to try. On (0, 0, 1) the two differ in sign, and for any other angle they differ in the plane too. The correct relation needs a complement: `transform(x, !r)` is `r * x * ~r` (section 2.4).
2. **This is a hazard for users, not only for the plan.** A reader who knows rotors from any other source will write `transform(v, r)`, get a wrong vector with no message, and see a test with a quarter turn pass. It goes to the owner as open item 1 (section 7).
3. **The tutorial is cut into ten chapters differently from the plan's table** (section 3.3): the plan's chapters 1 and 2 become one, and a new chapter 6 teaches the antiproduct on Euclidean ground before the rigid algebra appears. The page count of plan section 4.1 does not change. Open item 5 gives the alternatives.
4. **Three ruled choices have evidence against them**, recorded as open items and not designed around: the `///` contract moving out of the headers (decision 0253 items 14 and 25; the complexity cycle's census, open item 2), language-model cold readers as the default (item 31; open item 3), and the two antiproduct glyphs (item 27; a font probe of this machine, open item 4). The third is my own finding, not the research notes'.
5. **Proofs here are mine and unreviewed.** Each theorem of section 2 is proved in this note and also checked exactly by a script that shares no code with the library; the checks and the proofs come from one author, so they are evidence of the kind VVUQ calls self-checked until the unit's independent math review has run. That review is the coordinator's to arrange; it has not run.
6. **Claims about other authors' conventions are limited to what the knowledge base records as read.** I read no outside source in this unit and made no outbound request. Section 5 says what may and may not be written on a user page.
7. **No decision record was written.** Sibling units on other branches take decision numbers at the same time, so the choices are recorded here and in the five records with their alternatives, for the coordinator to file. This note takes the number 0028, the next free one on `main` and on the four sibling branches I could see; a sibling may take it first.
8. **The commit message of the checks says 28 forms; the count is 20** (section 2.8). History is not rewritten, so the message stands with this correction.
9. **Small compiles ran outside the job pool**, as the brief allows: two probe files, each compiled with `g++` and `clang++` directly (0.36 to 0.55 s each), and one scratch program in the session scratchpad. `tools/jobpool status` read "running" with 16 to 22 of 23 slots free; nothing was started or stopped. No test of the suite was built, no timing measurement was taken, and no lock was needed.

## 1. Sources and tools

Read in full: `AGENTS.md`, the constitution, WORKFLOW, STYLE, VVUQ, CONVENTIONS 1.18, decision 0253, plan 0049, the two role files, the README, the guide's first page, `transform.hpp`, and every note and exercise of the two research cycles (`knowledge-base-worktrees/doc-pedagogy` at `c4607f6`, 9 notes and 5 exercises; `knowledge-base-worktrees/complexity-reduction` at `56daebb`, 12 notes and 4 exercises). Read in part: `core.md` (through "Scalars", and the sections on products, complements, duals, the dot, transform, and checking mode), `rga3.md`, `unary.hpp`, `motions.hpp`, the misuse catalog's tables, and the knowledge base's notes on sign conventions, antiproducts, antiproduct identities, involutions over antiproducts, rigid motors and Colapinto's rotors, and the Clifford groups. The owner's pedagogical-writing and writing-style skills were loaded and applied.

Tools: CPython 3.14.10 (standard library only) for `transform_sandwich_check.py`, run with math-tools' `toolset/env/py/.venv/bin/python -I` and again with the system `python3 -I`; GCC 16.2.1 and Clang 23.1.5 for the probes. SymPy 1.14.0, galgebra 0.6.0, and python-flint 0.9.0 are in the environment and were not needed: every identity is checked over the integers on basis blades.

Evidence labels: **Proved** (derived here, written out), **Checked** (an exact computation, with its script and scope), **Read** (in a source, at the place cited), **Inferred** (my conclusion, not checked).

## 2. What `transform` gives for a classical rotor

### 2.1 Setting

Galoot's conventions throughout (CONVENTIONS 2.1, 3.1, 3.2, 4.1, 4.2, 4.4). n is the number of generators, 𝟙 = e12…n, x̃ the reverse, x̂ the grade involution, x̌ the antigrade involution, right and left the complements, ⟑ the geometric product (written by juxtaposition in the derivations), ⟇ the antiproduct, and V̰ the antireverse. For a multivector V whose blades all have grades of one parity, write g for that parity.

**The classical action.** For V of one grade parity,

    C_V(x) = V x Ṽ      when g is even,
    C_V(x) = V x̂ Ṽ      when g is odd.

This is what texts that act with the geometric product write for a rotor (g even) and for a product of an odd number of mirror normals (g odd). For one vector a and a vector v it is −a v a = (a · a) v − 2 (a · v) a, the reflection of v in the plane perpendicular to a, times a · a. (Proved: a v = 2 (a · v) − v a, so −a v a = −2 (a · v) a + v a a.) It is written with the reverse, not the inverse, so that it is a polynomial in the coefficients of V, as `transform` is.

**Galoot's transform** (CONVENTIONS 4.4), for V whose blades all have antigrades of one parity:

    transform(x, V) = V ⟇ x ⟇ V̰          antigrades even,
    transform(x, V) = −V ⟇ x̌ ⟇ V̰         antigrades odd.

A blade of grade k has antigrade n − k, so the two parities agree when n is even and are opposite when n is odd. In R(3, 0, 0) a classical rotor, on 1, e12, e13, e23, has antigrades 3 and 1: it is odd in Galoot's sense.

### 2.2 Two lemmas in a nondegenerate diagonal form

Let the form be diagonal with every square +1 or −1, q of them −1. G is the metric exomorphism, which multiplies each basis blade by the product of its generators' squares (3.2).

Facts used, each a line (Proved):

- For a basis blade u, u ũ = ũ u = G(u), a sign, since reversing brings equal generators together.
- The geometric product of two basis blades with no common generator is their wedge (4.1).
- G is an automorphism of the geometric product, G G = identity, and G commutes with the reverse. (For basis blades, e_A e_B is a sign times the squares of the shared generators times the blade of the symmetric difference, and G(A) G(B) is G of the symmetric difference times the squares of the shared generators twice.)
- G(𝟙) = (−1)^q 𝟙 = (𝟙 𝟙̃) 𝟙, so 𝟙⁻¹ = (−1)^q 𝟙̃.
- right(x) = G(x̃) 𝟙 and left(x) = 𝟙 G(x̃). (For a basis blade u, c = G(u) ũ 𝟙 is a multiple of the blade of the generators u lacks, so u ∧ c = u c = G(u)² 𝟙 = 𝟙; likewise c′ = G(u) 𝟙 ũ gives c′ ∧ u = 𝟙.)
- 𝟙 y 𝟙⁻¹ = y when n is odd and ŷ when n is even, because a generator passes the other n − 1 generators of 𝟙 with the sign (−1)^(n−1). Write this as 𝟙 y 𝟙⁻¹ = α(y), with α the identity for odd n and the grade involution for even n; 𝟙⁻¹ y 𝟙 = α(y) too.
- V̰ = left(reverse(right(V))), because on a basis blade of antigrade m the complement has grade m, the reverse multiplies it by (−1)^(m(m−1)/2), and left undoes right.

**Lemma 1.** a ⟇ b = b 𝟙⁻¹ a. (Proved; Checked on 14 forms.)

    a ⟇ b
    =   { definition, 4.2 }
    left(right(a) right(b))
    =   { right(x) = G(x̃) 𝟙 }
    left(G(ã) 𝟙 G(b̃) 𝟙)
    =   { left(y) = 𝟙 G(ỹ); the reverse reverses the order of a product }
    𝟙 G(𝟙̃ G(b) 𝟙̃ G(a))
    =   { G is an automorphism and G G = identity }
    𝟙 G(𝟙̃) b G(𝟙̃) a
    =   { G(𝟙̃) = (−1)^q 𝟙̃, twice }
    (𝟙 𝟙̃) b 𝟙̃ a
    =   { 𝟙 𝟙̃ = (−1)^q and 𝟙̃ = (−1)^q 𝟙⁻¹ }
    b 𝟙⁻¹ a

So the antiproduct reverses the order of its factors and puts 𝟙⁻¹ between them. Its identity is 𝟙, as it must be.

**Lemma 2.** V̰ = (−1)^q 𝟙 Ṽ 𝟙. (Proved; Checked on the same 14 forms.) From V̰ = left(reverse(right(V))) = 𝟙 G(right(V)) = 𝟙 G(G(Ṽ) 𝟙) = 𝟙 Ṽ G(𝟙).

**The antiproduct sandwich without its sign rule.** For any V, x, and W, by Lemma 1 twice and Lemma 2:

    V ⟇ x ⟇ W̰ = W̰ 𝟙⁻¹ x 𝟙⁻¹ V = (−1)^q 𝟙 W̃ x 𝟙⁻¹ V = (−1)^q α(W̃ x) V.        Eq. (1)

### 2.3 Theorem A: what `transform(x, V)` is, in a Euclidean form

**Theorem A.** In R(n, 0, 0), for V of one grade parity,

    transform(x, V) = Ṽ x̂ V     when g is even,
    transform(x, V) = Ṽ x V     when g is odd,

that is, transform(x, V) = C_Ṽ(x̂): the classical action of the reverse of V, on the grade involution of x. (Proved; Checked for n = 1 to 5.)

Proof. Take q = 0 and W = V in Eq. (1), and x̌ = (−1)^n x̂ (2.1). The four cases:

| n | g | antigrades | transform by 4.4 | value |
| --- | --- | --- | --- | --- |
| odd | odd | even | α(Ṽ x) V with α the identity | Ṽ x V |
| even | even | even | (Ṽ x)^ V | Ṽ x̂ V, since Ṽ is even |
| odd | even | odd | −(Ṽ x̌) V = −Ṽ (−x̂) V | Ṽ x̂ V |
| even | odd | odd | −(Ṽ x̌)^ V = −(Ṽ x̂)^ V = −(−Ṽ) x V | Ṽ x V, since Ṽ is odd |

For a rotor r with r r̃ = 1 and a vector v, this reads transform(v, r) = −r̃ v r: the opposite turn, then a point reflection. For one unit vector a it reads transform(v, a) = a v a, the negative of the reflection in the plane perpendicular to a; that is the reflection in the line along a, which keeps a and negates what is perpendicular to it. In Galoot's terms a vector of R(3, 0, 0) is not a mirror at all: the mirrors are the antivectors, here the bivectors.

### 2.4 Theorem B: the complement turns the classical action into `transform`

**Theorem B.** In a nondegenerate diagonal form with squares ±1, for V of one grade parity and every x,

    transform(x, bulk_dual(V)) = C_V(x).

In a Euclidean form the bulk dual is the right complement, so there

    transform(x, right(V)) = C_V(x):   `transform(x, !V)` is `V * x * ~V` for a rotor.

(Proved; Checked on 14 forms with the bulk dual, and on 8 definite forms with the complement.)

Proof. Let W = bulk_dual(V) = right(G V) = Ṽ 𝟙, so W̃ = 𝟙̃ V = (−1)^q 𝟙⁻¹ V. A blade of grade k has a complement of antigrade k, so the antigrades of W have the parity g. By Eq. (1) with V and W both replaced by this W:

    W ⟇ x ⟇ W̰
    =   { Eq. (1) }
    (−1)^q α(W̃ x) W
    =   { W̃ = (−1)^q 𝟙⁻¹ V and W = Ṽ 𝟙 }
    α(𝟙⁻¹ V x) Ṽ 𝟙
    =   { α(𝟙⁻¹) = 𝟙⁻¹: the grade involution multiplies 𝟙 by (−1)^n, and α applies it n − 1 times }
    𝟙⁻¹ α(V x) Ṽ 𝟙
    =   { 𝟙⁻¹ y 𝟙 = α(y), and α α = identity }
    V x α(Ṽ)
    =   { Ṽ has grade parity g }
    (−1)^((n−1) g) V x Ṽ.

For g even, 4.4 gives transform(x, W) = W ⟇ x ⟇ W̰ = V x Ṽ. For g odd it gives −W ⟇ x̌ ⟇ W̰ = −(−1)^(n−1) V x̌ Ṽ = −(−1)^(n−1) (−1)^n V x̂ Ṽ = V x̂ Ṽ. Both are C_V(x).

**With the complement in an indefinite form the statement fails.** right(V) = bulk_dual(G V), so transform(x, right(V)) = C_{G V}(x). In a definite form G V is V or its grade involution, and C does not change; in an indefinite form G V is another multivector. (Checked: it fails in each of the six indefinite forms tried and holds in R(0, 2, 0), R(0, 3, 0), and R(0, 4, 0).) The tutorial stays in R(2, 0, 0) and R(3, 0, 0) until the rigid algebra, so it needs the complement only; the conventions chapter states the general form with `bulk_dual`.

### 2.5 Theorem C: the same statement read the other way, in every form

**Theorem C.** In every form the conventions cover, degenerate and non-diagonal ones included, with W = right(V) and X = right(x) and the geometric product on the right-hand sides:

    right(transform(x, V)) = W X W̃          antigrades of V even,
    right(transform(x, V)) = −W X̂ W̃         antigrades of V odd.

(Proved from the definitions for every form; Checked on 12 diagonal forms, four of them degenerate, R(3, 0, 1) among them.)

Proof. right(a ⟇ b) = right(a) right(b) by the definition of ⟇ and right(left(y)) = y. right(V̰) = reverse(right(V)), since V̰ = left(reverse(right(V))). right(x̌) is the grade involution of right(x) (2.1). Apply right to the two forms of 4.4.

This is the exact relation between Galoot's rigid algebra and the description in which the planes are the vectors and motions act with the geometric product: complement the motor and the object, and Galoot's antiproduct sandwich becomes the geometric-product sandwich. For an odd versor the complemented form carries a minus sign and a grade involution. Section 5 uses it.

### 2.6 Theorem D: a Euclidean versor carried into rigid(d)

**Theorem D.** In rigid(d), let V be on the blades of e1 to ed only, of one grade parity, and let x be any multivector of rigid(d). Then

    transform(x, right(V)) = C_V(x),

with the complement and the geometric product of rigid(d). (Proved; Checked for d = 1 to 4.)

Proof. Write n = d + 1 and W = right(V).

1. right(W) = right(right(V)) = σ V with the one sign σ = (−1)^(g (n − g)), since two right complements multiply a blade of grade k by (−1)^(k (n − k)) (3.1) and V has one grade parity. The antigrades of W have parity g.
2. By Theorem C, right(transform(x, W)) is V X Ṽ for g even and −V X̂ Ṽ for g odd; σ appears twice and cancels.
3. The complements read only n, never the metric, so right is the same map in rigid(d) and in E = R(d + 1, 0, 0). A product of basis blades one of which lies on e1 to ed uses only the squares of shared generators, which are among e1 to ed and are +1 in both algebras. So V y Ṽ is the same multivector in rigid(d) and in E, for every y.
4. In E, right(y) = ỹ 𝟙 (section 2.2 with G the identity), so for V1 and V2 of grade parity g,
   right(V1 x Ṽ2) = V2 x̃ Ṽ1 𝟙 = V2 x̃ 𝟙 α(Ṽ1) = (−1)^((n−1) g) V2 right(x) Ṽ1.
   Summed over the blades of V this is right(V x Ṽ) = (−1)^((n−1) g) V right(x) Ṽ.
5. For g even, step 4 says right(C_V(x)) = V X Ṽ. For g odd, right(x̂) = (−1)^n times the grade involution of right(x), so right(C_V(x)) = right(V x̂ Ṽ) = (−1)^(n−1) (−1)^n V X̂ Ṽ = −V X̂ Ṽ. Both equal step 2's value, and left undoes right.

So the rotor of a Euclidean chapter, complemented in rigid(3), is the motor that does to the point (x, y, z, w) what the rotor does to the vector (x, y, z), and it fixes e4 up to the factor V Ṽ. The same holds for a mirror normal and the plane through the origin that is its complement. The knowledge base records the rotor case, for Colapinto's rotors, as self-checked ("Derived translation of Colapinto's rotors" in `geometric-algebra/rigid-motors-flectors-and-rational-point-groups.md`); this note adds the proof, the odd case, and every d.

### 2.7 When the two agree without a complement

The plan asked for which algebras and versors `transform` equals the classical sandwich. By Theorem A the condition is C_Ṽ(x̂) = C_V(x).

- As an identity in x it holds in no form tried, Euclidean or not, for n = 1 to 5. (Checked: the row N of the script.)
- For one versor and every vector: write C_V(v) = λ R v with R orthogonal and λ = V Ṽ, the classical correspondence (Inferred here from the background literature, not derived). Then transform(v, V) = −λ R⁻¹ v, and the two agree on every vector exactly when R R = −1. A mirror has R R = +1, so never. A rotor in odd n has determinant +1 while −1 has determinant −1, so never. In even n a rotor with R R = −1 qualifies, and in the plane that is the quarter turn. (Checked: in R(2, 0, 0), `transform(v, 1 − e12)` is the classical turn on every vector, and `transform(v, 2 − e12)` is not.)
- For one versor and one vector the two agree exactly when R R v = −v. A quarter turn in R(3, 0, 0) does this to the vectors of its own plane and to no other. The plan's probe used the quarter turn 1 − e12 and the vector (1, 0, 0).

### 2.8 Worked values and checks

The numbers the pages will use, each derived by hand and confirmed three ways (the derivation, the script from the definitions, the library).

With mirrors a = (1, 0, 0) and b = (1, 1, 0) in R(3, 0, 0): r = b a = (e1 + e2) e1 = 1 + e2 e1 = 1 − e12, and r r̃ = 2. The mirrors are 45 degrees apart, so r turns by 90 degrees from e1 toward e2.

    r e1 r̃ = (1 − e12) e1 (1 + e12) = (e1 + e2)(1 + e12) = e1 + e2 + e2 − e1 = 2 e2
    r e3 r̃ = 2 e3            (e3 commutes with e12)
    −r̃ e1 r = −(1 + e12) e1 (1 − e12) = −(e1 − e2)(1 − e12) = −(e1 − e2 − e2 − e1) = 2 e2
    −r̃ e3 r = −2 e3

| Expression | (1, 0, 0) | (0, 1, 0) | (0, 0, 1) | (2, −3, 5) |
| --- | --- | --- | --- | --- |
| `r * v * ~r`, classical | (0, 2, 0) | (−2, 0, 0) | (0, 0, 2) | (6, 4, 10) |
| `transform(v, r)` | (0, 2, 0) | (−2, 0, 0) | (0, 0, −2) | (6, 4, −10) |
| `transform(v, !r)`, with `!r` = e123 − e3 | (0, 2, 0) | (−2, 0, 0) | (0, 0, 2) | (6, 4, 10) |

With mirrors a = (1, 0, 0) and c = (2, 1, 0): s = c a = 2 − e12, s s̃ = 5, the cosine and sine of the turn are 3/5 and 4/5, and nothing coincides.

    s e1 s̃ = (2 − e12) e1 (2 + e12) = (2 e1 + e2)(2 + e12) = 4 e1 + 2 e2 + 2 e2 − e1 = 3 e1 + 4 e2

| Expression | (1, 0, 0) | (0, 1, 0) | (0, 0, 1) |
| --- | --- | --- | --- |
| `s * v * ~s`, classical | (3, 4, 0) | (−4, 3, 0) | (0, 0, 5) |
| `transform(v, s)` | (−3, 4, 0) | (−4, −3, 0) | (0, 0, −5) |
| `transform(v, !s)` | (3, 4, 0) | (−4, 3, 0) | (0, 0, 5) |

One mirror, a = (1, 0, 0), on (1, 2, 3): the classical reflection −a v a is (−1, 2, 3); `transform(v, a)` is (1, −2, −3); `transform(v, !a)` is (−1, 2, 3).

In rigid(3): `!` of 1 − e12 is e1234 − e34 = e1234 + e43, the motor (0, 0, 1, 1, 0, 0, 0, 0) on e41, e42, e43, e1234, e23, e31, e12, 1, which is the README's integer quarter turn about the z axis. `!` of the mirror normal e1 is e234 = e423, the plane x = 0. The motor sends the point (1, 0, 0, 1) to (0, 2, 0, 2), (0, 0, 1, 1) to (0, 0, 2, 2), and (2, −3, 5, 1) to (6, 4, 10, 2); the plane, as a flector, sends (1, 2, 3, 1) to (−1, 2, 3, 1); and `compose` of the two mirror planes of b and a is that motor.

**Checks** (all in [`records/docs/checks/`](../records/docs/checks/)):

| Check | What it shows | Scope | Result |
| --- | --- | --- | --- |
| `transform_sandwich_check.py`, rows A | Theorem A | R(n, 0, 0), n = 1 to 5 | holds |
| rows B | Theorem B with the complement | 5 Euclidean and 3 negative-definite forms | holds |
| rows B, indefinite | the complement form fails | 6 indefinite forms | fails in each |
| rows B′ | Theorem B with the bulk dual | 14 nondegenerate forms | holds |
| rows N | transform(x, V) = C_V(x) | the same 14 forms | fails in each |
| rows L | Lemmas 1 and 2 | the same 14 forms | hold |
| rows C | Theorem C | 12 diagonal forms, 4 degenerate | holds |
| rows D | Theorem D | rigid(1) to rigid(4) | holds |
| row W | the values of this section | by both actions | hold |
| `transform_sandwich_probe.cpp` | the library computes the values above, in R(3, 0, 0), R(2, 0, 0), and rigid(3) | 44 `static_assert`s over `int`, GCC 16.2.1 and Clang 23.1.5 | compiles; three mutants each fail on both compilers |
| `design_values_probe.cpp` | the test values of the misconception record and of sections 3 and 5 | 35 `static_assert`s, both compilers | compiles |

Twenty distinct diagonal forms are covered in all: five Euclidean, three negative definite, six indefinite, and six degenerate. Each side of each identity is linear in x and a quadratic form in V restricted to one parity, and the script compares the symmetrized coefficients on every triple of basis blades, so a pass is a proof for that algebra and a failure is a counterexample. The script's first version compared unsymmetrized terms, which made true identities look false; the output committed is from the corrected script. The script is written from the definitions and shares no code with the library or with `test/reference/conventions_check.py`; it and the proofs have one author. Not checked: non-diagonal forms (Theorem C is proved for them, the others are not claimed), and n above 5.

### 2.9 What this changes on the pages

1. The chapter on rotors writes the sandwich with `*` and `~` and never with `transform`. It says once, in words the reader has, that Galoot's `transform` is a different operation, and shows the vector (0, 0, 1) on which the two differ.
2. `transform` enters only after the complement, as "the sandwich of the complemented versor": `transform(v, !r)`.
3. The first-things page gains a tenth fact, the conventions chapter a row with the test values of 2.8, and the porting recipe for Euclidean rotors and quaternions rests on Theorems B and D.
4. No example or test may show that two actions agree with a quarter turn alone, or with vectors of the rotor's plane alone (authoring rule A2).

## 3. The bridge from rotors to motors (design point 1)

### 3.1 The idea

A mirror can be given by its normal vector or by its own plane. In R(3, 0, 0) the plane of the mirror with normal a is the bivector right(a), and by Theorem B everything the reader did with normals and the geometric product can be done with planes and the antiproduct, with the same numbers. The second description is the one that survives when planes stop passing through the origin: a plane of rigid(3) has four numbers and no normal vector that describes it. So the bridge has three steps, and each changes one thing.

| | Chapter 4: normals, ⟑, R(3, 0, 0) | Chapter 6: planes, ⟇, R(3, 0, 0) | Chapter 8: planes, ⟇, rigid(3) |
| --- | --- | --- | --- |
| First mirror | a = (1, 0, 0) | g = `!a` = e23 | the plane x = 0, (1, 0, 0, 0) |
| Second mirror | b = (1, 1, 0) | h = `!b` = e23 + e31 | the plane x + y = 0, (1, 1, 0, 0) |
| One reflection | −a v a | `transform(v, g)` | `transform(p, Flector{g})` |
| (1, 2, 3) reflected in the first | (−1, 2, 3) | (−1, 2, 3) | (−1, 2, 3, 1) |
| Two reflections | r = b a = 1 − e12 | V = h ⟇ g = `!r` = e123 − e3 | Q = `compose` of the planes = (0, 0, 1, 1, 0, 0, 0, 0) |
| The motion | `r * v * ~r` | `transform(v, V)` | `transform(p, Q)` |
| (1, 0, 0) moved | (0, 2, 0) | (0, 2, 0) | (0, 2, 0, 2) |
| (0, 0, 1) moved | (0, 0, 2) | (0, 0, 2) | (0, 0, 2, 2) |
| The other side | the reverse, `~r` | the antireverse | the antireverse |
| The factor left | r r̃ = 2 | V ⟇ V̰ = 2 𝟙 | λ = 2, and the weight doubles |
| Second motion after first | r2 r1 | `compose(V2, V1)` | `compose(Q2, Q1)` |
| What is new | | the mirror is its plane | planes need not pass through the origin |

Every entry is Checked in the two probes. The comparison is made once the reader knows the first method, with corresponding steps aligned and one question asked ("what changed from the column on the left, and what did not?"), as the pedagogical skill's comparison rule asks.

### 3.2 What chapter 8 adds, in exact numbers

Once planes carry an offset, two mirrors give every rigid motion:

- **Parallel mirrors move by twice their distance, from the first toward the second.** Reflecting in z = 0 sends z to −z, and reflecting in z = 1 sends z to 2 − z, so the two send z to z + 2. In coefficients: the planes (0, 0, 1, 0) and (0, 0, 1, −1) compose to the motor (0, 0, 0, 1, 0, 0, 1, 0), half of the distance 2 on e12, and (1, 2, 3, 1) goes to (1, 2, 5, 1).
- **Crossing mirrors turn by twice their angle about their common line, which is their meet.** Reflecting in x = 1 and then in y = 1 sends (x, y) to (2 − x, 2 − y), a half turn about the vertical line through (1, 1, 0). The planes (1, 0, 0, −1) and (0, 1, 0, −1) meet in the line (0, 0, −1, −1, 1, 0) and compose to the motor (0, 0, 1, 0, 1, −1, 0, 0), with λ = 1; the origin goes to (2, 2, 0), (1, 1, 5) stays, and (2, 1, 0) goes to (0, 1, 0). This is a turn about a line that misses the origin, exact in integers and unitized, which no Euclidean rotor can express.
- **Parallel planes still meet**, in a line with zero direction: z = 0 and z = 1 meet in (0, 0, 0, 0, 0, −1).

(All Checked in `design_values_probe.cpp`.)

### 3.3 The chapters

The order is the topological order of the concept inventory (`records/docs/concept-inventory.md`, checked by `checks/inventory_check.py`).

| Ch. | Title | Algebra | Against the plan's table |
| --- | --- | --- | --- |
| 1 | A first program: vectors and dot products | R(2, 0, 0), then R(3, 0, 0), `int` | the plan's chapters 1 and 2 |
| 2 | The wedge | R(3, 0, 0) | the plan's 3 |
| 3 | The geometric product | R(2, 0, 0), R(3, 0, 0) | the plan's 4 |
| 4 | Reflections and rotors | R(3, 0, 0) | the plan's 5, with the warning about `transform` |
| 5 | Complements and the meet | R(3, 0, 0), and R(2, 0, 0) for the contrast | the plan's 6 |
| 6 | Mirrors as planes: the antiproduct | R(3, 0, 0) | new: the antiproduct, the antireverse, `transform`, `compose`, moved here from the plan's 8 |
| 7 | Points, lines, and planes | rigid(3), `int` | the plan's 7 |
| 8 | Motions | rigid(3), `int` then `double` | the plan's 8 without what moved to 6 |
| 9 | Exact and rounded | rigid(3) | the plan's 9 |
| 10 | A robot arm | rigid(3) | the plan's 10 |

Reasons. Chapter 1 as merged teaches only Galoot's C++ on mathematics the reader already has, so the first chapter has one kind of new thing. The new chapter 6 separates the two hard ideas of the plan's chapter 8, a second product and a fourth coordinate, and puts the first on ground where Theorem B gives it an exact meaning in numbers the reader has already computed; plan risk 1 names that chapter as the one most likely to fail a cold read. One correction of fact: the plan's chapter 6 was to show "why `!!x` is not always x" in R(3, 0, 0), where `!!x` is always x, because the grade times the antigrade is even for every blade of three generators. The contrast needs R(2, 0, 0), where `!!e1` is −e1, or rigid(3).

## 4. What the 1.0.0 release lacks for teaching (design point 3)

Decision 0253 item 35 rules that the tutorial teaches around these and records the list. Each row says where the reader pays and what the page does instead. The last column ranks the item as a backlog candidate by how much teaching it would remove: A removes a hand-written helper or a paragraph from several chapters, B from one, C is a convenience.

| # | Missing | Where the reader pays | What the page does instead | Rank |
| --- | --- | --- | --- | --- |
| 1 | The classical sandwich as an operation: no function gives `V * x * ~V` on x's layout, and `transform(x, V)` is a different thing (section 2) | Chapter 4, every rotation: `r * v * ~r` is on e1, e2, e3, e123, so each use is `truncate<Vector>(r * v * ~r)`, and the reader needs `truncate` two chapters early; chapters 4 and 6: the trap of open item 1 | A four-line helper written by the reader in chapter 4; the warning; chapter 6's `transform(v, !r)` | A |
| 2 | `unitize`, for a point, a plane, a line, a motor | Chapter 7: (2, 4, 6, 2) and (1, 2, 3, 1) are one point and compare unequal with `==`, and nothing makes them equal; chapter 8: a motor written in integers leaves a factor; chapter 10: every printed position is divided by its weight by hand | Division by the weight written out; comparison by cross-multiplying, as a helper | A |
| 3 | A norm (length, bulk norm, weight norm) | Chapter 1: lengths are compared by their squares; chapter 4: a unit rotor is described, not made; chapters 8 and 10: distances | `dot(x, x)` and `antidot(x, x)` read with `get`, and a square root taken by the reader over `double` | A |
| 4 | An inverse, of a vector or of a versor | Chapter 3: the inverse of a vector is v / (v · v) by hand; chapter 4: the reflection is written −a v a with the factor a · a carried, and "undo a rotation" is the reverse divided by the factor | The reverse and the factor, stated once in chapter 3 and used after | B |
| 5 | A number from a one-blade result: `dot(a, b)` is a multivector on the scalar | Chapter 1, the first dot product: `int d = dot(a, b)` does not compile | `get<"1">(dot(a, b))`, taught as the first case of "a result has its own layout" | B |
| 6 | Exponential and logarithm | Chapter 4: a rotor for an angle is written cos(θ/2) − sin(θ/2) e12 by hand over `double`, since `rotation` exists only in the rga3 preset; chapter 10: no interpolation of a motion | The half-angle form written out | B |
| 7 | `rotation` in a constant expression, and motions over integers | Chapters 8 and 10: every exact check writes a motor by its eight coefficients, with half quantities scaled to integers | The coefficient table of the motor; quarter turns and the factor 2 | B |
| 8 | Comparison within a tolerance | Chapter 9: a loop over `coefficients` written by the reader | The loop, as that chapter's worked example | B |
| 9 | Equality of points, lines, or planes up to a factor | Chapters 7 and 8, with item 2 | The cross-multiplying helper | B |
| 10 | Distance and angle between objects | Chapters 7 and 10: not taught | Left out; named under "not in this release" | C |
| 11 | A grade part by number | Chapter 3: `part<Type>` needs a declared type for the part | A named layout for each part | C |
| 12 | The antigrade involution as a function | The conventions chapter: the odd form of `transform` cannot be written as the specification writes it | `grade_involution` with the sign (−1)^n | C |
| 13 | A motor as a 4 × 4 matrix | The rigid-geometry recipe: built by moving four points | The recipe | C |
| 14 | The conformal preset | Conventions and tutorial leave section 6 of the specification out | Said once on the map | C |

Rows 1 to 5 were found by writing the chapter's program before reading the header, as the hci role file asks; rows 1, 2, and 5 are the ones a newcomer hits in the first hour. (Checked for rows 1, 2, and 5 in `design_values_probe.cpp`; Read in the guide for rows 3, 6, 7, and 14; Inferred for the others from the list of public names in `core.md`'s reference, which I did not compare with the headers name by name.)

## 5. The other convention (design point 4)

What is established here (Theorem C, Proved and Checked for R(3, 0, 1)): complement the motor and the object, and Galoot's antiproduct sandwich with the antireverse becomes a geometric-product sandwich with the reverse. Under the complement a point, a vector in Galoot, becomes a trivector, and a plane, a trivector in Galoot, becomes a vector. That is the description in which planes are the vectors. For an odd versor, a reflection, the complemented form is −W X̂ W̃: it differs from the twisted form W X̂ W̃ by the sign −1 on every object, which is Galoot's choice that a reflected point keeps a positive weight (CONVENTIONS 5.6 and the first row of its section 8).

What the knowledge base records as read about other authors: Gunn (arXiv:1411.6502) names the null generator e0, puts it first, and argues for the algebra in which planes are the vectors; Lengyel puts the null generator last, keeps points as vectors, and uses antiproducts (`geometric-algebra/sign-convention-taxonomy.md`, rows "Signature order" and "PGA vectors"). It records the flector sign of other sources as untested. Nothing else about other authors is established, and I read none of them in this unit.

So for the pages:

1. Chapter 8 says, in two sentences, that other introductions, Gunn's among them, call the null generator e0 and put it first, make the planes the vectors, and act with the geometric product, and that the right complement carries each description to the other. It points to the conventions chapter, which is in milestone D1. It cannot point to the porting recipe, which is in D3 and not yet authorized (decision 0253 item 23).
2. The conventions chapter states Theorem C with a compiled example, and says that names change too: a blade named with e0 first has its own orientation, so a coefficient is carried over by recomputing it, never by editing signs.
3. No page says "most" introductions, names a library's convention, or states the sign another source gives a reflection, until someone has read that source (VVUQ V9).

## 6. What the two research cycles change in plan 0049

The plan's author could read only headings (plan section 1 item 4). Read in full, the notes confirm most of the plan. Tags in brackets are the notes' own. "Change" rows are built into the five records; "Owner" rows are open items of section 7.

| # | The plan's choice | What the notes say | Outcome |
| --- | --- | --- | --- |
| 1 | The tutorial starts with a first program and reaches a rotation in chapter 5 (4.2) | "Payoff first, theory on demand": Klein and kingdon open with a task the reader wants done [derived, from read-by-assistant sources]; a realistic task early (minimalism, through secondary accounts) | Change, within the ordering rule: the README's example is the payoff and the setup check; the tutorial's opening page shows the destination in the reader's own words and one figure, and the first rotation moves up one chapter (now chapter 4) |
| 2 | Dot product and metric before the wedge (chapters 2 and 3) | Outer product first; "inner products and contractions last, because this is where sources disagree most"; the metric "by the squares of the basis vectors, not by a bilinear form" [derived] | Change in part: chapter 1 uses the dot of two vectors only, which the reader knows, and the metric as the table of the generators' dot products; the dot of higher blades and the contractions stay out of the tutorial; the conventions card states which dot Galoot has |
| 3 | A long conventions chapter, each rule with a compiled example (4.6) | "One page", "each line states the choice and a small test value the reader can reproduce", "a card that only names the choices is not enough" [derived] | Change: the chapter opens with a one-screen card, one row per choice with a one-line expression and its value, each linked to its section (template T5) |
| 4 | Worked examples in Problem, Plan, Solution, Check form; exercises near to far (4.2) | Fading: fully worked, then one step left to the reader, then the goal only; a "predict, then run" prompt in each [read for the effect, derived for the form] | Change: the chapter template fixes the faded sequence and the prompt (template T1); the `cpp exercise` scaffold of plan 5.3 carries the middle step |
| 5 | Chapters are teaching texts with derivations in calculational chains (5.4) | Diataxis: "a tutorial is not the place for explanation"; a derivation in a tutorial step moves to explanation, with one sentence and a link [read, assistant] | Change, with a reading: the pedagogical skill forbids a step on trust, and the concepts pages a chapter would link are in D3. So a chapter's main line holds worked numbers, and a general argument that the exercises do not need sits in one closing section, "Why it works", in the chapter itself |
| 6 | Figures with labels of defined terms (5.5) | Spatial contiguity, d = 0.82: labels at the part they describe, no legend; output beside the code that printed it [read] | Change: rules F2 and E5 |
| 7 | Headings unspecified | Signaling, d = 0.70: a heading states the step's result ("Join two points into a line"), not the tool ("Using join") [read for the effect, derived for the rule] | Change: rule W3 |
| 8 | Reference entry: Requires, Guarantees, Result layout, Cost, Rejected with, example (4.5) | An accuracy contract per operation; failure sections with "must", what happens on violation, and degenerate inputs; design rationale reachable from reference (Robillard 2009) [read, derived] | Change: the entry gains Exactness, On violation, Degenerate inputs, Asking whether a call is valid, and Why (template T4) |
| 9 | Diagnostics page quotes each message (4.7) | Each entry: the exact text, the misuse as a program, what the reader meant, the fix, the concept; what each compiler prints, trimmed, with the line that carries the library's text; "keep a line naming the old text" when a message is reworded [derived] | Change: template T6. The messages that plan 0048's E1 and E2 reword shipped in 1.0.0, so their entries keep the old text. A stable tag in each message is a matter for E1 and E2, noted for their author |
| 10 | Cold reader: one persona, the newcomer (9.1) | Three personas, one of whom knows the mathematics from another source; "report every term or convention it had to supply from its own knowledge"; an agent is a pre-check, "not a substitute for human readers" [derived] | Change: the reader's table gains that column (rule R3). A second persona for the conventions chapter is open item 6. The human reader is open item 3 |
| 11 | Every example a whole program (decision 0242) | The include route is the usual one for a compiled language; a fragment that needs unstated context is a defect; examples "illustrate usage", edge cases stay in the test suite; exact printed values [read, assistant; derived] | No change to the mechanism. Rules E2, E6, E7. The cost of whole programs to a tutorial's readability is open item 7 |
| 12 | Cost pages: measured sizes on the limits page | "Never an adjective alone"; a measurement with compiler, version, flags, date, machine, and command; Eigen's ordinal ratings as the anti-model [read, assistant; derived] | Change: rule C4 |
| 13 | One operation, two spellings, both shown | mp-units' quick start "shows three ways to create a quantity at once" as an anti-model; each operator spelling is interface to learn [read, assistant; derived] | Change: rule W5, one way first |
| 14 | The reference documents every public name | The interface of a template library is also its constraints, diagnostics, compile cost and limits, headers, and companion names; 20 of 110 public names are `_layout` companions [checked-self] | Change: the tutorial brings in 28 of the public names and no `_layout` companion (the inventory counts them); T4 covers the other kinds |
| 15 | The frame creates about 20 pages at D1 (6, U3) | "Do not build empty sections to fill later" [read, assistant] | No change, with a caution for U3: with D3 not authorized, a page exists at D1 only if it has content moved from today's guide |
| 16 | One sentence of `///` and the contract in the reference (O3, ruled) | "Interface comments: complete"; the contract of a template library "cannot be learned from the signature"; 75% of interface-comment lines are contract; "deleting comments is not where its simplification lies" [derived; checked-self for the census] | Owner: open item 2 |
| 17 | Language-model readers by default (O9, ruled) | Row 10 | Owner: open item 3 |
| 18 | Examples run in every configuration until measured (O8, ruled) | A product test compiles in 2.4 s (Clang) to 3.9 s (GCC); Clang's step margin on the product tests is 1.49 [checked-self] | No change. For U2: both probes of this unit, about 40 assertions each over typed layouts, compile in 0.36 to 0.55 s, so the examples' cost is dominated by their count, and an example on a dense layout can meet Clang's step limit (rule E8) |
| 19 | The comparison with other libraries (4.6) | The prior-art note describes other libraries from pages read by an assistant, and gives no convention as read by its author | No change: plan 9.2 already requires a source someone has read; none of that note may be quoted on a user page as it stands |

Nothing in the notes argues against the ruled layout, figures, hand-written reference, restated conventions chapter, or the release that carries the pages (decision 0253 items 24, 26, 28, 29, 34).

## 7. Open items

For the owner:

1. **`transform` and the classical sandwich.** A user who knows rotors gets a wrong result with no message (section 2). Options: (a) documentation only: the warning in chapter 4, the bridge in chapter 6, a tenth fact on the first-things page, a row of the conventions card, and a porting recipe; nothing in the library changes, and the trap stays for anyone who does not read. (b) Also add the classical action under its own name, such as `sandwich(x, V)`, in a later minor release: it removes row 1 of section 4 and makes the right call findable, at the price of a public name and its tests. (c) Also say it in the `///` sentence of `transform`. Decision 0253 item 35 already rules (a) for the tutorial. Recommendation: (a) now, in D1 so that the first-things page and the conventions chapter carry it before the tutorial exists; (c) when plan 0048 next touches that comment; (b) recorded as the first backlog candidate.
2. **The `///` contract (decision 0253 items 14 and 25).** The complexity cycle argues for complete interface comments and measured that about three quarters of the headers' `///` lines are contract. Options: (a) the ruling as it stands: one sentence and a pointer, the contract in the reference, a precondition that is undefined behavior kept in full; one copy, and a reader of a header or of an editor's hover must follow the pointer. (b) S11 as ratified: two copies, kept in step by review alone. (c) The ruling, with the Requires line kept on the declaration as well as the undefined-behavior precondition. The ruling's own sequencing already protects the text: nothing leaves a header until the receiving page has passed review, and that page is in D3, which is not authorized. Recommendation: keep (a), and weigh the cycle's evidence when the amended wording of S11 is put for confirmation; nothing needs to change before then.
3. **A human cold reader (item 31).** The pedagogy cycle treats an agent's read as a pre-check and asks for people, three to five per kind of reader. The ruling waits on a name. Options: (a) name one reader for chapters 1 to 4 at D2, about an hour; (b) name two, the second one who knows rotors from elsewhere, for chapters 4 and 6 and the conventions card, the pages the trap of item 1 concerns; (c) none. Recommendation: (b) if two people are at hand, otherwise (a).
4. **The two antiproduct glyphs (item 27).** Of the font families installed on this machine, one (Noto Sans Math) has ⟑ (U+27D1) and ⟇ (U+27C7) and no monospaced family has either; ∧ and ∨ are in 105 families, 15 of them monospaced. The plan sets derivations in `text` blocks, which are monospaced. Options: (a) as ruled: the symbols everywhere, with a fallback line in the notation key; a reader without that font sees empty boxes in the derivations of the two products the library is about. (b) As ruled in running text, and inside `text` and code blocks the C++ spellings `*` and `%` for those two products only; every other symbol unchanged. (c) TeX. The notation key is written for (a), as ruled, with the fallback column that (b) would use. Recommendation: (b); it is the ruling's "symbols and words" applied to two glyphs.

For the coordinator:

5. **The chapter cut.** Options: (A) the plan's ten chapters, with the whole bridge inside chapter 8, which then introduces about twelve ideas; (B) eleven chapters, the new one added and nothing merged, which adds two pages to plan section 4.1 and needs the owner's word (plan risk 7); (C) the cut of section 3.3. Recommendation: (C). U5 then writes chapters 1 to 5 and U6 chapters 6 to 10, as planned by number.
6. **A second kind of cold reader**, one told it knows Euclidean rotors and quaternions, given the conventions chapter, the first-things page, and three porting tasks, at U4. It is the only test of open item 1's documentation. About one Sonnet read.
7. **Tutorial examples as whole programs.** Each of about 130 examples repeats its includes, namespace aliases, tag, and type aliases. Options: (a) as today, with one fixed preamble per chapter, identical in every example of the chapter (rule E2); (b) U2 adds a visible page preamble that later fragments of the page are compiled with. (b) reads better and changes decision 0242's mechanism; (a) keeps every block copyable alone. Recommendation: (a), and ask the cold readers whether the preamble cost them anything.
8. **D2 without D3.** The tutorial may link only pages that exist at D2: the conventions chapter, the glossary, the first-things page, the limits page, and what U3 moves. The design assumes this; plan section 4.2 point 4's pointer to the porting recipe becomes a pointer to the conventions chapter.
9. **The first-things page gains a fact** in U3 or U4, which plan section 4.3 has as "unchanged in substance". Its text and example are in `records/docs/misconceptions.md` under M15.
10. **The checkers.** `techlint.py` and `pedlint.py` are still not on this machine: the two skill directories under `~/.claude/skills/synced/` hold `SKILL.md` alone, and a search of `~/.claude/skills` and `~/projects/utils` to depth six found neither. Decision 0253 item 33 stands.
11. **The independent math review** that closes this unit: the proofs of section 2, the worked values, and the inventory's order. `checks/inventory_check.py` shows that the order has no cycle and no forward reference; it cannot show that no concept is missing.

## 8. Next step

The coordinator arranges the review of item 11 and puts items 1 to 4 to the owner. U2 and U3 can start from the five records as they stand; U4 needs item 1 ruled or defaulted to (a).
