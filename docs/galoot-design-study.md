# Galoot: Design Study for a Modern Header-Only Geometric Algebra Library

Oct 30, 2026 · @Lord Voldemort

## Executive summary

Galoot can be a header-only C++23 library whose whole core is one compile-time "plan compiler": it turns a metric and two blade sets into a flat, exact table of multiply-add terms. Everything else in Versor's stack (named types, generators, constructions) becomes a thin layer over that one mechanism. Lengyel's antiproducts, complements and bulk/weight split are derived from the same tables, so they cost nothing extra at run time.

**Terms used throughout.** A *blade* is a wedge product of basis vectors, stored as a bitmask (Fontijne encoding). A *multivector* is a linear combination of blades. The *metric signature* R(p,q,r) means p basis vectors square to +1, q to −1 and r to 0.

The main decisions this study recommends:

| Decision | Recommendation | Why |
| --- | --- | --- |
| Language baseline | C++23, with optional C++26 reflection behind a feature-test macro | C++26 failed its DIS ballot on 2026-10-23; reflection ships only in GCC 16 behind `-freflection` |
| Type representation | Signatures and blade sets as constexpr values (class-type template arguments), not type lists | Removes Versor's recursive template instantiation, its 9-dimension practical cap and its `-ftemplate-depth-1200` flag |
| Product generation | `consteval` term tables with exact dyadic coefficients, including non-diagonal (null-basis) metrics | Versor kept only the sign of null-basis products; Galoot keeps the exact value |
| Mathematical model | Lengyel's RGA (rigid, R(3,0,1)) and CGA (R(4,1) with e4 = origin, e5 = infinity) | Duals are signed permutations, so no inverse pseudoscalar is needed; meet and join are symmetric |
| Scalars | A `scalar` concept: float, double, complex, interval, expansion, dual number, SIMD lane pack | One kernel serves graphics, robust predicates, autodiff and batched Monte Carlo |
| Robustness | Predicates are signs of single top-grade coefficients, compiled into Shewchuk-style filtered kernels | orient3d and insphere are exactly wedge products of points; VoroCrust's coverage tests are rational |
| Coupling | Core → models → {robust, geom, mesh, alg}; graphics in separate adapter packages | Matches Versor's compositional layering without its `form/` and `draw/` coupling |

The largest open risks are compile time for big blade sets, sign-convention drift between Lengyel's wiki and his Terathon code, and square roots in meshing. Each has a concrete mitigation in the sections below.

## Versor: what to keep and what to replace

Versor's lasting contribution is its compositional idea: a few bitmask rules generate every product, and named geometric types are *derived* from products rather than listed by hand. Its weaknesses are C++11 mechanics, not the design.

### History

- **Versor 1.0 (2011, MS thesis).** Lua scripts generated C functions for 3D CGA only: 167 types and 83,667 functions, about 50 MB compiled ([thesis](http://wolftype.com/versor/colapinto_masters_final_02.pdf)).
- **Versor 2 (2014–2015).** A rewrite as C++11 template metaprogramming for any `metric<P,Q,R,conformal>`, presented at C++Now 2014 and 2015. Appendix A of the [PhD dissertation](https://escholarship.org/uc/item/5m76n8tg) documents it.
- **Status.** The [repo](https://github.com/wolftype/versor) is BSD-2, last commit 2025-11-02 (a clang 20 fix), with 10 open issues. The author states it handles up to about 9 dimensions in practice (issue #24).

### Architecture, bottom to top

| Layer | Files (`include/vsr/`) | Role |
| --- | --- | --- |
| Bits | `detail/vsr_basis.h` | Blade = `short` bitmask; constexpr `grade`, `signFlip` (reordering sign), inner/outer existence tests, conformal bit positions |
| Type lists | `detail/vsr_xlists.h` | `Basis<short...>` blade lists, insert-sort, set difference; `metric<P,Q,R,conf>`; metric sign `MSign` |
| Instructions | `detail/vsr_instructions.h` | `Inst<sign, A, B, idxA, idxB>`: one multiply term; sign 0 drops degenerate terms |
| Conformal split | `detail/vsr_split_met.h` | Converts null-basis blades to the diagonal e+/e− basis, multiplies, converts back |
| Products | `detail/vsr_products.h` | Euclidean, diagonal and conformal families of geometric, outer, inner and scalar products |
| Algebra | `detail/vsr_algebra.h` | `algebra<metric, T>`; named types per algebra |
| Multivector | `detail/vsr_multivector.h` | `Multivector<algebra, basis>` holding `T val[N]`; operators, duals, norms |
| Generic ops | `detail/vsr_generic_op.h` | N-dimensional `Gen`, `Round`, `Flat`, `Tangent`, `Op` |
| Spaces | `space/vsr_cga3D_*.h`, `pga3D`, `sta` | 3D CGA vocabulary; functions defined in `src/*.cpp`, so CGA3D is not header-only |
| Forms | `form/` (about 14k lines) | Frames, kinematic chains, Bennett linkages, twists, knots, symmetry groups, folding, cyclides |
| Graphics | `draw/`, `util/`, `ext/gfx` | Separate directory and CMake flag; `vsr.h` is math-only |

### How the product generator works

1. A nested template recursion visits every pair of blades (A, B) in the two input lists and emits `Inst<±, A, B, i, j>` with result blade A XOR B.
2. For a diagonal metric, the sign is the reordering sign times the squares of the shared basis vectors; a zero square removes the term (this is how PGA works).
3. `Reduce` insert-sorts the result blades into a deduplicated, grade-ordered `Basis<...>`: the sparse return type.
4. Execution expands a parameter pack into straight-line code: one sum of signed products per output blade.
5. For CGA, each null-basis blade pair is pushed to the diagonal basis, multiplied, popped back and compressed. Only the **sign** of each result is kept, which is correct only because these coefficients happen to be ±1. That invariant is undocumented.
6. Named types are computed: `Par = op(Pnt, Pnt)`, `Cir = op(Par, Pnt)`, `Mot = gp(Rot, Trs)`.

### Vocabulary worth carrying forward

- The **round / flat / tangent / direction** taxonomy, with `center`, `radius`, `carrier`, `surround`, `split` and flat `direction`, `location`, `weight`.
- **Generators as 2-blades:** bivector → rotor, direction → translator, dual line → motor (screw), tangent → transversion, flat point → dilation, and **point pair → boost**. Boosts "bend lines into circles and planes into spheres" ("Boosted Surfaces", AACA 24(1):71–78).
- **Logs and ratios** of versors and rotor interpolation ("Composing Surfaces with Conformal Rotors", AACA 2017), plus splitting a general conformal bivector into commuting point pairs (Dorst–Valkenburg).
- **Higher-level forms:** cyclidic nets and volumes, kinematic chains with FABRIK, closed-form Bennett 4-bars, crystallographic space groups as versor products of mirrors, folding with fold lines as normalized lines.

### Pain points Galoot should fix

| Problem in Versor | Galoot remedy |
| --- | --- |
| Recursive type lists, `-ftemplate-depth-1200`, deep error messages | consteval tables over constexpr values; concepts for errors |
| `short` masks; about 9 dimensions in practice | `smallest_uint<N>` masks up to 32 compile-time, multiword at run time |
| Structural zeros only (v·v yields scalar + bivector) | Symbolic like-term collection at compile time (as in gal) |
| `spin` silently projects to the input type | Explicit `sandwich` that states its projection; debug check that dropped parts vanish |
| Implicit lossy conversions (issue #20); `Rot` wrong above 3D (issue #22) | `explicit` conversions; rotor types derived from even subalgebra per dimension |
| One precision per build (`VSR_PRECISION` macro) | Scalar type as a template parameter constrained by a concept |
| Exact `== 0` tests, fixed `FPERROR` 1e-6 | Robust predicate layer and tolerance policies |
| Mixed half-angle conventions across `Gen::` | One convention, documented and tested |
| `^` binds looser than `<=` and `*` | Named functions are canonical; operators are thin sugar |
| CGA3D needs linking `libvsr` | Fully header-only |

In [ga-benchmark](https://github.com/ga-developers/ga-benchmark), Versor was among the fastest libraries. Fernandes notes its eager evaluation misses cross-expression optimization in inverse kinematics, where lazy GATL wins ([chapter](https://profs.ic.uff.br/~laffernandes/content/publications/book_chapter/2021_sema_13/fernandes-sema-13-2021-chapter.pdf)).

## Lengyel's model and how it changes the generator

Lengyel's model needs only two extra tables beyond the geometric product: a signed complement permutation and a bulk/weight mask. Every "anti" operation then follows mechanically from De Morgan laws. Sources: the [RGA wiki](https://rigidgeometricalgebra.org/wiki/index.php?title=Main_Page), the [CGA wiki](https://conformalgeometricalgebra.org/wiki/index.php?title=Main_Page), [projectivegeometricalgebra.org](https://projectivegeometricalgebra.org/) and the [Terathon Math Library](https://github.com/EricLengyel/Terathon-Math-Library).

### Rigid GA, R(3,0,1)

- **Basis.** e1, e2, e3 square to +1 and e4 squares to 0. Vectors are points p = (x, y, z, w); e4 is the projective direction.
- **Basis order** is chosen so objects split cleanly: lines use e41, e42, e43 (direction v) and e23, e31, e12 (moment m); planes use e423, e431, e412 (normal) and e321 (offset).
- **Scalar and antiscalar.** **1** is the identity of the geometric product ⟑; the antiscalar 𝟙 = e1234 is the identity of the geometric antiproduct ⟇.
- **Antigrade** of a k-blade in n dimensions is n − k. Wedge adds grades; antiwedge adds antigrades.

### Complements replace the inverse pseudoscalar

The right complement ū and left complement u̲ of a basis blade u are defined by:

```latex
u \wedge \overline{u} = \mathbb{1}, \qquad \underline{u} \wedge u = \mathbb{1}, \qquad \underline{u} = (-1)^{\operatorname{gr}(u)\,\operatorname{ag}(u)}\,\overline{u}
```

A complement is a signed permutation of coefficients, with no division. Hestenes duality multiplies by the inverse pseudoscalar I⁻¹, which does not exist when I² = 0, as in R(3,0,1). The table below was computed independently and matches Terathon's hard-coded signs.

| Blade | Right / left complement | Blade | Right / left complement |
| --- | --- | --- | --- |
| **1** | 𝟙 / 𝟙 | e41 | −e23 / −e23 |
| e1 | e423 / −e423 | e42 | −e31 / −e31 |
| e2 | e431 / −e431 | e43 | −e12 / −e12 |
| e3 | e412 / −e412 | e423 | −e1 / +e1 |
| e4 | e321 / −e321 | e431 | −e2 / +e2 |
| e23 | −e41 / −e41 | e412 | −e3 / +e3 |
| e31 | −e42 / −e42 | e321 | −e4 / +e4 |
| e12 | −e43 / −e43 | 𝟙 | **1** / **1** |

### Bulk, weight, duals and norms

- **Metric exomorphism** G is the 16×16 matrix the metric induces on all blades; the **antiexomorphism** 𝔾 is its counterpart, with G𝔾 = det(𝔤)·I. In RGA both are 0/1 diagonal masks.
- **Bulk** u● = Gu holds the components without e4 (position relative to the origin). **Weight** u○ = 𝔾u holds the components with e4 (attitude).
- **Bulk dual** u★ = complement(u●); **weight dual** u☆ = complement(u○).
- **Dot** a•b = aᵀGb and **antidot** a∘b = aᵀ𝔾b. Bulk norm ‖u‖● = √(u•u), weight norm ‖u‖○ = √(u∘u); the geometric norm is their sum, and an object is *unitized* when ‖u‖○ = 𝟙.
- **Reverse and antireverse:** ũ = (−1)^{gr(gr−1)/2} u and the antireverse uses antigrade in place of grade.

### Products follow from De Morgan laws

```latex
\overline{a \wedge b} = \overline{a} \vee \overline{b}, \qquad \overline{a \mathbin{\unicode{x27D1}} b} = \overline{a} \mathbin{\unicode{x27C7}} \overline{b}, \qquad a \mathbin{\unicode{x27C7}} b = \underline{\,\overline{a} \mathbin{\unicode{x27D1}} \overline{b}\,}, \qquad a \vee b = \underline{\,\overline{a} \wedge \overline{b}\,}
```

For a generator this is the key fact: the antiwedge ∨ (meet) and antiproduct ⟇ are the ordinary tables conjugated by the complement permutation. Galoot folds the permutation into the consteval term table, so `antiwedge` runs exactly as fast as `wedge`.

Other operations are compositions:

- Contractions a∨b★, a∨b☆ and expansions a∧b★, a∧b☆.
- Projection of a onto b: b ∨ (a ∧ b☆); antiprojection: b ∧ (a ∨ b☆).
- Attitude att(u) = u ∨ e321.
- Motor sandwich x′ = Q ⟇ x ⟇ Q̰ (Q̰ = antireverse); flector sandwich x′ = −F ⟇ x ⟇ F̰.

A rotation motor is R = l sinφ + 𝟙 cosφ and rotates by 2φ about line l. A translation motor T = tₓe23 + tᵧe31 + t\_z e12 + 𝟙 translates by 2t. Galoot should keep this half-angle convention everywhere.

### Conformal GA, R(4,1)

- **Basis.** e1, e2, e3 Euclidean; e4 = ½(e₋ − e₊) is the origin and e5 = e₋ + e₊ is the point at infinity. Metric: e4·e4 = e5·e5 = 0 and e4·e5 = −1. The metric is **not diagonal** in the stored basis.
- **Rounds:** round point (e1..e5), dipole (point pair, grade 2), circle (grade 3), sphere (grade 4). A round point with center p and radius r is p + e4 + ½(p² + r²) e5.
- **Flats:** the RGA objects wedged with e5. A flat point is on e15, e25, e35, e45; a line and a plane follow the same pattern. RGA coefficients carry over unchanged.
- **Operations:** carrier car(u) = u ∧ e5, cocarrier ccr(u) = u★ ∧ e5, center cen(u) = ccr(u) ∨ u, container, partner, attitude, radius. Imaginary rounds have r² < 0.
- **Dipole endpoints** are p± = cen(d) ± ‖d‖\_R · att(d). The square root here is the only irrational step in sphere-intersection meshing (see the predicates section).
- **Norm partitions** in CGA use four masks: round bulk (no e4, e5), round weight (e4 only), flat bulk (e5 only), flat weight (both).

### Lengyel versus Hestenes-style conventions

| Aspect | Hestenes / Doran–Lasenby / Dorst | Lengyel |
| --- | --- | --- |
| Dual | Multiply by I⁻¹ (fails for degenerate metrics) | Complement: signed permutation, always defined |
| Meet | (A\* ∧ B\*)^{−\*} or contraction tricks | Antiwedge ∨, symmetric with ∧ |
| PGA object for a point | Trivector (plane-based PGA) | Vector (point-based RGA) |
| Rigid motion | Q x Q̃ with the geometric product | Q ⟇ x ⟇ Q̰ with the antiproduct |
| Inner products | Several (left/right contraction, Hestenes, scalar) | Dot, antidot, bulk/weight contractions and expansions |
| CGA null vectors | n₀, n∞ (or e₀, e∞) | e4, e5, with RGA objects embedding via ∧ e5 |

Gunn and Dorst–De Keninck argue for plane-based PGA, and a bivector.net thread notes that Lengyel dualizes operators rather than objects with the same arithmetic. Lengyel's own 2026 post says both forms give identical results ([Dual approaches to PGA](https://terathon.com/blog/dual-pga.html)). For a library the practical gain is uniformity: one set of rules works for degenerate, conformal and Euclidean metrics.

### Convention traps found in the sources

- **Dual sign.** Terathon's `BulkDual` and `WeightDual` for points and planes return the negation of the wiki's right duals (they match the left duals); CGA `Dual(RoundPoint3D)` is also negated. The difference is left versus right complement, which differ by (−1)^{gr·ag}: that is −1 only for odd grades in 4D, so lines agree. Recommendation: right-complement duals, as on the wiki. With them a ∧ b★ = (a•b)𝟙 and the projection g ∨ (p ∧ g☆) returns a positive weight; left duals negate both (checked numerically for this study).
- **Possible Terathon bug.** `Partner(Circle3D)` in `TSConformal3D.cpp` appears to be missing parentheses relative to the wiki formula.
- **RGA double duals vanish:** u★★ = 0 in R(3,0,1), so only complements, not bulk/weight duals, are invertible.

## Lessons from other GA libraries

No surveyed library combines a signature-driven compile-time generator, Lengyel's conventions and robust predicates; no C++26-reflection GA library was found on GitHub. The table lists what each one teaches Galoot.

| Library | Language, license | Representation and generation | Lesson for Galoot |
| --- | --- | --- | --- |
| [Versor](https://github.com/wolftype/versor) | C++11, BSD-2 | Type-list blade sets, eager straight-line code | Keep derived named types; replace type lists |
| [gal](https://github.com/jeremyong/gal) | C++17, MIT | Expression compiler to rational polynomials, exact cancellation | Like-term collection pays off (about 2–2.5× over Versor), but its IK benchmark took about 50 s to compile ([forum](https://discourse.bivector.net/t/gal-c-library/65)) |
| [GATL](https://github.com/laffernandes/gatl) | C++17, GPL-3 | Lazy expression templates; `general_metric_space` for non-diagonal metrics | Lazy evaluation wins in long chains; study only (GPL) |
| [Klein](https://github.com/jeremyong/klein) | C++, MIT, archived 2024 | Hand-written SSE for R(3,0,1) | Hand-packed SIMD wins for one fixed algebra and does not generalize |
| [Garamon](https://github.com/vincentnozick/garamon) | Generator → C++/Eigen, MIT | Precomputed low-dimension products; recursive prefix-tree products in high dimension | Need a second product path above about n = 10 |
| [TbGAL](https://github.com/Prograf-UFF/TbGAL) | C++17, GPL-3 | Factorized blades and versors (n > 256) | Factorized forms for very high dimensions; slow at low n |
| Gaigen 2.5 (Fontijne) | Generator | Bitmap blades; non-diagonal metrics by eigen-diagonalization | Origin of the bitmask encoding Galoot already uses |
| [Gaalop](https://github.com/CallForSanity/Gaalop) | Java precompiler, LGPL-3 | Symbolic optimization to coefficient code (C++, CUDA, Verilog) | Whole-expression symbolic optimization is a separate tool, not a header library |
| [GA-FuL](https://www.mdpi.com/2227-7390/12/14/2272/html) | C#, data-oriented | Scalar-processor abstraction (float, arbitrary precision, symbolic); code generation | One engine for numeric and symbolic work |
| [kingdon](https://github.com/tBuLi/kingdon) | Python, MIT | Per-key-set code generation with CSE, cached | "Multivector of arrays" was about 470× faster than "array of multivectors" ([paper](https://arxiv.org/html/2503.10451v1)) |
| [ganja.js](https://github.com/enkimute/ganja.js) | JS, MIT | `Algebra(p,q,r)` returns a generated class; emits C++, Rust and more | Signature as the single input is the right user model |
| [Grassmann.jl](https://github.com/chakravala/Grassmann.jl) | Julia, AGPL-3 | Types parameterized by vector space; diagonal, degenerate and null-basis metrics | Degenerate and conformal bases as first-class options |
| [Terathon](https://github.com/EricLengyel/Terathon-Math-Library) | C++, MIT | Fixed classes per object; no general geometric product; `Transform()` sandwiches; SSE/AVX/NEON | Golden test values for Lengyel's model; sandwiches as first-class fused operations |
| [gafro](https://arxiv.org/pdf/2310.19090) | C++20, Eigen | Compile-time blade-index lists, lazy expression templates, CGA only | Confirms C++20 compile-time blade lists are competitive |
| [ga-benchmark](https://github.com/ga-developers/ga-benchmark) | GPL-3 | Euclidean, homogeneous, Minkowski and conformal models; 22 operations plus IK | Last published run is 2020-02-05; a refreshed run with Galoot would be useful in its own right |

Three lessons recur:

1. **Values, not types.** Represent signatures and blade sets as constexpr values and memoize the tables per signature.
2. **Sparse result types beat generic SIMD.** Hand SIMD only the hot fixed algebras (RGA3, CGA3), ideally by making the scalar a SIMD lane pack.
3. **License hygiene.** GATL, TbGAL, Grassmann.jl and ga-benchmark are GPL or AGPL, so a permissively licensed Galoot can learn from them but not copy code. Decision (2026-10-31): Galoot is MIT-licensed and implemented clean-room from the mathematics; other libraries serve only as black-box cross-checks after Galoot's own tests pass.

## Which C++ standard

Target C++23 for the core and treat C++26 reflection as an optional convenience layer. The newest standard the feature set allows is C++26, but as of today it is neither published nor portable.

**C++26 status.** Technical work finished in March 2026 ([Sutter](https://herbsutter.com/2026/03/)). The DIS ballot then **failed** on 2026-10-23: 12 of 20 P-members approved, short of the 14 required, with 9 No votes ([train-it.eu](https://train-it.eu/blog/cpp26-contracts-the-ballot-result/)). The objections are believed to concern contracts; WG21 meets in Búzios on 16–21 November 2026, and the name may become C++27. The current standard is still C++23 (ISO/IEC 14882:2024).

| Feature | Use in Galoot | GCC | Clang | MSVC |
| --- | --- | --- | --- | --- |
| Class-type template arguments (C++20) | `metric` and `blade_set` as values | yes | yes | yes |
| `consteval`, `constexpr std::array`, `std::popcount` (C++20) | Building term tables | yes | yes | yes |
| Concepts (C++20) | `scalar`, `multivector`, `round`, `flat` constraints and readable errors | yes | yes | yes |
| Deducing `this`, static `operator()`, `if consteval` (C++23) | Unified member/free operations; constexpr-vs-runtime kernels | 14 | 18–19 | 19.43–19.44 |
| Pack indexing (C++26, P2662) | Index blade packs without recursion | 15 | 19 | no |
| `std::mdspan` (C++23) | Batched SoA storage views | 16 | libc++ 18 | 19.39 |
| Reflection (P2996) | Named aggregate members (`p.x`, `l.e41`) | 16 with `-freflection` | Bloomberg fork only | no |
| `template for` (P1306) | Unrolled term loops | 16 | 23, partial | no |
| `define_static_array` (P3491) | Hand consteval vectors to run time | 16 | fork only | no |
| `std::simd` (P1928) | SIMD lane packs as scalars | 16, partial | no | no |
| Contracts (P2900) | Avoid in public headers | 16 | no | no |

Compiler data are from the [GCC status page](https://gcc.gnu.org/projects/cxx-status.html), [Clang status page](https://clang.llvm.org/cxx_status.html) and cppreference's [C++23](https://en.cppreference.com/w/cpp/compiler_support/23) and [C++26](https://en.cppreference.com/w/cpp/compiler_support/26) tables. GCC 16.1 was released 2026-04-30.

### Compile-time cost rules

- **Memoize tables per signature** in a `static constexpr` variable template, so each (metric, blade-set pair, product) is evaluated once per translation unit.
- **Prefer constexpr evaluation over type instantiation.** Instantiating types and function templates is the most expensive template operation (the "Rule of Chiel"); evaluating a loop in `consteval` is far cheaper.
- **Keep `<meta>` out of the core header.** Romeo measured about 155 ms per translation unit just to include it on GCC 16, versus about 1–2 ms per reflected type ([measurements](https://vittorioromeo.com/index/blog/refl_compiletime.html)).
- **Watch constexpr step limits** for big tables: a dense Cayley table for n = 10 has about one million entries. Galoot computes only the sparse pairs the program uses.

### Portability fallback

Without reflection, named access uses a fixed-string template argument: `get<"e41">(line)`. With reflection, `galoot/reflect.hpp` generates `struct Line { T e41, e42, e43, e23, e31, e12; }` views. Without `std::simd`, the scalar concept accepts any SIMD wrapper such as xsimd or Eigen packets.

## Galoot core architecture

The core is six small parts in one dependency chain: bits → metric → blade sets → plan compiler → multivector → operations. Only the plan compiler is non-trivial, and it is ordinary `consteval` code rather than template recursion.

### 1. Bits

Constexpr functions on blade masks, usable both at compile time and at run time (the `alg` module reuses them):

- `grade(a) = popcount(a)`; antigrade `n − grade(a)`.
- **Reordering sign** of e\_A e\_B: count the swaps needed to sort the concatenated indices.

```cpp
constexpr int reorder_sign(mask_t a, mask_t b) {
  int swaps = 0;
  for (a >>= 1; a; a >>= 1) swaps += std::popcount(a & b);
  return (swaps & 1) ? -1 : 1;
}
```

- **Complement** of a basis blade: mask `~a & full`, with the sign fixed by u ∧ ū = 𝟙 (one `reorder_sign` call). Reverse and antireverse signs from grade and antigrade.

### 2. Metric descriptor

A `metric` is a structural value (usable as a template argument) holding the dimension, a symmetric bilinear form as a small integer matrix, and Lengyel's basis names. Factory functions cover the common cases:

```cpp
struct metric {
  std::uint8_t n;
  std::array<std::array<std::int8_t, 16>, 16> g;   // bilinear form on basis vectors
  basis_names names;                               // e.g. "e1 e2 e3 e4"
};
consteval metric R(int p, int q, int r);   // diagonal: R(3,0,1), R(1,3,0), R(4,1,0)
consteval metric rigid(int d);             // R(d,0,1), projective vector last (e4 in 3D)
consteval metric conformal(int d);         // e(d+1) = origin, e(d+2) = infinity, g(d+1,d+2) = -1
```

`R(4,1,0)` (diagonal e+, e−) and `conformal(3)` (null e4, e5) describe the same algebra in different bases. Galoot supports both and offers exact conversion between them.

### 3. Oriented blade sets

A type's storage layout is an ordered list of **oriented blades**: a mask plus a sign. Lengyel's e31 is stored as mask {1,3} with sign −1 and e321 as {1,2,3} with sign −1, so coefficients match his formulas with no hidden negations.

```cpp
struct oriented_blade { mask_t mask; std::int8_t sign; };
template <std::size_t K> struct blade_set { std::array<oriented_blade, K> b; };

// Parsed at compile time from Lengyel names:
constexpr auto line_blades = blades<rigid(3)>("e41 e42 e43 e23 e31 e12");
```

### 4. The plan compiler

A **plan** is the compiled form of one operation on two (or three) input blade sets: a result blade set and a flat list of terms `{out, ia, ib, coef}`. It is built once per signature and input pair and stored in a `static constexpr` variable.

1. **Diagonal metrics:** for each blade pair, one term with coefficient `reorder_sign × ∏ squares of shared vectors`; zero squares drop the term.
2. **Non-diagonal metrics (CGA null basis):** map each blade to the orthogonal basis by its outermorphism, multiply there, map back and collect. Coefficients are exact dyadic rationals (e.g. ±½), stored as numerator and power of two. This removes Versor's sign-only shortcut.
3. **Anti-operations:** apply the complement permutation to inputs and the inverse complement to outputs, inside the table. Antiwedge, geometric antiproduct and antidot then run at the same cost as their direct versions.
4. **Products with filters:** wedge, dot, contractions and expansions are the geometric product followed by a grade filter or composed with complements, all folded at compile time.
5. **Fused compound plans:** sandwiches (Q ⟇ x ⟇ Q̰), projections and joins of three points are expanded into polynomials in the input coefficients. Like monomials are collected exactly, as gal does, so cancellations such as the vanishing trivector part of a motor acting on a point disappear from the generated code.
6. **Result typing:** the result blade set holds only blades with at least one surviving term. A sandwich declares its output type; the plan records any dropped components so a debug build can check they are zero.

In the final kernel each term becomes `r[out] += coef * a[ia] * b[ib]` with `coef` a compile-time constant, so ±1 and ½ multiply away:

```cpp
template <class A, auto SA, auto SB>
constexpr auto operator*(mv<A, SA> const& a, mv<A, SB> const& b) {
  using P = plan<op::geometric, A::metric, SA, SB>;
  mv<A, P::result> r{};
  [&]<std::size_t... I>(std::index_sequence<I...>) {
    ((r.c[P::terms[I].out] += P::template coef<I, typename A::scalar>()
                             * a.c[P::terms[I].ia] * b.c[P::terms[I].ib]), ...);
  }(std::make_index_sequence<P::terms.size()>{});
  return r;
}
```

With C++26 the fold becomes a `template for` loop over `P::terms`. Above about n = 10 the dense pair loop grows too large; there Galoot should switch to Garamon-style recursive products or the runtime `alg` module.

### 5. Multivector and scalar concept

```cpp
template <class S>
concept scalar = requires(S a, S b) { a + b; a - b; a * b; S(0); S(1); };

template <metric M, scalar T = double> struct algebra { /* metric, scalar, names */ };

template <class A, auto Blades>
struct mv { std::array<typename A::scalar, Blades.size()> c{}; };
```

- `mv` is an aggregate: trivially copyable, no virtual functions, no heap.
- Conversions between blade sets are `explicit`, and a lossy conversion requires `project<T>(x)`.
- Scalars include `float`, `double`, `std::complex`, interval types, Shewchuk expansions, dual numbers for autodiff, symbolic expressions for code generation and SIMD lane packs. A SIMD-pack scalar gives the "multivector of arrays" layout for free.

### 6. Operations and naming

- **Canonical API is named functions:** `gp`, `antigp`, `wedge`, `antiwedge`, `dot`, `antidot`, `bulk`, `weight`, `bulk_dual`, `weight_dual`, `right_complement`, `left_complement`, `reverse`, `antireverse`, `sandwich`, `project`, `antiproject`, `unitize`, `exp`, `log`.
- **Operators are sugar** (recommended set): `*` geometric product ⟑ in every algebra; `%` geometric antiproduct ⟇ (same precedence as `*`); `^` wedge; `&` antiwedge (meet); `~` reverse; `!` right complement; `+`, `-`, scalar `*` and `/`. Motor composition and transforms also have named forms, `compose(Q2, Q1)` and `transform(x, Q)`, which run fused plans. Because `^` and `&` bind looser than `==` and `+`, examples always parenthesize.
- **Why not Terathon's scheme:** Terathon defines no general product operators. Its `*` between motors is the antiproduct, `~` is the antireverse, `!` is the right complement, and one `^` means wedge or antiwedge depending on argument types. In a general algebra both products exist for most pairs (in RGA, point ∧ plane and point ∨ plane are both nonzero), so Galoot needs two operators, and `*` must mean the same product in every model for generic code to work.
- **Models** declare named types by blade lists, as Versor did, but in Lengyel's vocabulary: `rga3::Point`, `Line`, `Plane`, `Motor`, `Flector`, `Magnitude`; `cga3::RoundPoint`, `Dipole`, `Circle`, `Sphere`, `FlatPoint`, `Line`, `Plane`.
- **Concepts per object class**, such as `round<T>`, `flat<T>` and `versor<T>`, replace Versor's "caller is responsible for the category" rule.
- **Generators and closed forms** for the fixed models use Lengyel's formulas (motor from line and angle, screw exponentials, sqrt of a motor). Generic `exp` uses a closed form for simple bivectors and invariant decomposition for general bivectors.
- **Errors** use concept failures and `static_assert` messages that print blade names, not template stacks.

### Layering

The core has no dependencies beyond the standard library. Models depend on the core only. Robust predicates, the modeling kernel, meshing and general algebras each depend on the core and models, never on each other except where noted. Graphics adapters live in separate packages.

&#91;embedded content: Galoot module architecture · core chain and opt-in modules\]

Only the plan compiler (highlighted) holds real logic; geom and mesh share robust predicates and models, while alg reuses only the core's bit rules.

## Geometric modeling kernel (`galoot/geom`)

The kernel should be a CSG and boundary-topology layer whose surfaces are canonical shapes placed by motors. GA supplies the placement, incidence and intersection algebra; the kernel supplies topology and queries. This mirrors how Monte Carlo transport codes already describe geometry (surfaces, senses, cells and transformations), with motors replacing transformation matrices.

### Primitives

| Surface family | GA representation | Native in |
| --- | --- | --- |
| Plane, half-space | RGA plane (x, y, z, w); sense = sign of g•p + g\_w p\_w | RGA, CGA |
| Sphere | CGA sphere; sense = sign of round point · sphere (power of the point) | CGA |
| Circle, point pair | CGA circle and dipole from meets of spheres and planes | CGA |
| Line, ray | RGA line (Plücker v, m) built as p ∧ d | RGA, CGA |
| Cylinder, cone, torus, general quadric | Canonical implicit form in a local frame, placed by a motor | Not native to CGA |
| Dupin cyclide patches | Colapinto's cyclidic nets from tangent frames and rotors | CGA (Versor `form/`) |

Cylinders, cones and tori are not CGA blades, and quadric and torus support is a concept only, not a requirement (decision, 2026-10-31). If it becomes one, canonical implicit forms with motor placement are the default; double conformal R(8,2) or quadric conformal R(9,6) (2^15 = 32,768 basis blades) would be a research track for the sparse generator.

### Operations

1. **Placement.** Every surface carries a unitized motor; a ray (line) is moved into the surface's local frame by one sandwich, which transforms direction and moment together.
2. **Intersections.** Line ∨ plane gives a flat point; sphere ∨ line gives a dipole whose reality test is a sign; sphere ∨ sphere gives a circle. These are the Join and meet formulas on Lengyel's CGA wiki.
3. **Cell membership.** A cell is a Boolean expression of surface senses, as in MCNP or OpenMC. Each sense is a sign of a GA scalar, so it routes through the robust predicate layer when a point is near a surface.
4. **Distance to boundary.** For transport, the kernel returns the smallest positive ray parameter over the cell's surfaces. Batches of particles use a SIMD-pack scalar, so one call advances many particles (event-based transport).
5. **Parametric construction.** Versor-style forms (frames, kinematic chains, twists, boosts, symmetry groups) live in `geom/form`, built only from public core operations.

### FEA-ready output

- A boundary representation with explicit vertex, edge and face incidence, where edges between spheres and planes are exact circles or lines.
- Tagged regions and interfaces for material assignment and boundary conditions, exported to the meshing module.
- Writers for OpenMC-style CSG, Gmsh `.geo` and VTK are adapters outside the kernel.

The design choice that matters most is **homogeneous coordinates throughout**. A point from the meet of three planes has polynomial coordinates (x, y, z, w) with no division, so later predicates on it stay polynomial and exact-evaluable.

## Mesh generation (`galoot/mesh`)

VoroCrust's seed placement is CGA in disguise: every decision in it is a sign of a Gram determinant of dual spheres, which is rational and exactly evaluable. Only the output seed coordinates need a square root. That makes a GA-native, robust VoroCrust feasible, and the same primitives cover Delaunay tetrahedra and regular (power) triangulations.

### VoroCrust in GA terms

VoroCrust ([TOG 2020](https://arxiv.org/abs/1902.08767), [SoCG 2018](https://arxiv.org/abs/1803.06078)) covers the surface with balls, takes the corner points of the union of balls as Voronoi seeds on either side of the surface, adds interior seeds, and outputs the unclipped Voronoi cells. Sandia released [code](https://github.com/sandialabs/vorocrust-meshing) under a BSD-style 3-clause license in 2022.

Define the normalized dual sphere Sᵢ = Pᵢ − ½rᵢ²e∞ for center Pᵢ (a CGA round point) and radius rᵢ, and the Gram matrix Gᵢⱼ = Sᵢ·Sⱼ (e∞ is Lengyel's e5; Lengyel's sphere is the complement of this vector, so the signs carry over up to a fixed factor):

```latex
S_i \cdot S_j = \tfrac{1}{2}\left(r_i^2 + r_j^2 - \lVert c_i - c_j \rVert^2\right), \qquad S_i \cdot S_i = r_i^2
```

For a k-blade B = S₁ ∧ … ∧ Sₖ, B² = (−1)^{k(k−1)/2} det Gₖ, so each test below is a sign of a single wedge product's square.

| VoroCrust step | GA operation | Exact test | Degree |
| --- | --- | --- | --- |
| Do two balls overlap? | Circle S₁ ∧ S₂ | det G₂ > 0 | 4 |
| Do three spheres meet in two points (a guide pair)? | Dipole S₁ ∧ S₂ ∧ S₃ | det G₃ > 0 | 6 |
| Is exactly one guide point inside a fourth ball (half-covered, sliver signal)? | S₁ ∧ S₂ ∧ S₃ ∧ S₄ | det G₄ > 0 | 8 |
| Are both guides covered, or both uncovered? | Sign of X₊·S₄ + X₋·S₄ when det G₄ < 0 | Rational, from the dipole | — |
| Is an interior seed inside a ball? | Round point · sphere | Sign of the power | 2 |
| Output a seed's coordinates | Dipole endpoints p± = cen ± ‖d‖·att | Needs a square root | — |

The second and third rows were checked numerically for this study: on 20,000 random sphere configurations, det G₃ > 0 matched "three spheres meet in two points" every time, and det G₄ > 0 matched "exactly one of the two points lies inside the fourth sphere" in all 6,363 cases with a real pair. The derivation (a Schur complement of G₄ against G₃) came from this study's research, not from the VoroCrust papers.

### Existing CGA Voronoi work

- **Romero, Barrón-Fernández, Godoy-Calderón, "Constructing Voronoi Diagrams from Hollow Spheres Using Conformal Geometric Algebra",** AACA 27(3):1997–2017 (2017), [doi:10.1007/s00006-017-0787-x](https://link.springer.com/article/10.1007/s00006-017-0787-x). It is 2D Bowyer–Watson on empty circles ("hollow spheres"), using P·S for inside/outside. Only the abstract and its two open precursors were readable; those show least-squares circle fits and exact `== 0` tests on doubles, so robustness is unaddressed.
- **Chomicki, Breuils, Biri, Nozick, "Pencils of CGA for Voronoi and Power Diagrams",** LNCS 2025: metadata only was reachable.
- **Edwards, Payne, Schafer, ["Generalized Voronoi Diagrams and Lie Sphere Geometry"](https://arxiv.org/abs/2408.09279)** expresses classical, Apollonius, farthest-point, order-2 and power diagrams, and polyhedral medial skeletons, as linear inequalities on an unknown sphere. It is the clearest algebraic recipe for generalizing Galoot's Voronoi module.

### Other element types

| Mesh type | Method | GA objects and predicates |
| --- | --- | --- |
| Tetrahedral (Delaunay) | Bowyer–Watson or incremental insertion, as in TetGen and CGAL Mesh\_3 | orient3d = top coefficient of P₁∧P₂∧P₃∧P₄∧e5; insphere = top coefficient of five round points |
| Weighted (regular) Delaunay, power diagrams | Protection balls for sharp features | Replace round points by dual spheres in the same 5-blade |
| Tetrahedral (envelope-based) | fTetWild-style insertion with exact inversion checks ([paper](https://arxiv.org/abs/1908.03581)) | Planes P₁∧P₂∧P₃∧e5, orient3d |
| Polyhedral | VoroCrust Voronoi cells, or duals of tetrahedral meshes | Bisector planes, dual spheres |
| Quad and hex | Frame-field guided meshing | An octahedral frame is a rotor modulo the 48-element binary octahedral group; smoothness energies use the degree-4 spherical-harmonic embedding ([Palmer et al.](https://arxiv.org/abs/1908.05411)) |
| Hybrid (hex-dominant + tets, pyramids, prisms) | Frame fields plus Delaunay fill and transition elements | All of the above; element Jacobians are orient-type wedges |
| Curved structured blocks | Colapinto's cyclidic volumes | Tangent frames, circles and rotor interpolation |

### Recommended module layout

1. `mesh/sizing`: local feature size and Lipschitz sizing fields, independent of GA.
2. `mesh/balls`: sphere covers using `cga3::Sphere`, with the exact overlap and pair tests above.
3. `mesh/seeds`: VoroCrust seed extraction and sliver elimination on exact predicates; seeds rounded to doubles with a recorded error bound.
4. `mesh/delaunay`: regular and Delaunay triangulation with filtered orient and insphere, plus Edelsbrunner–Mücke symbolic perturbation for ties.
5. `mesh/voronoi`: Voronoi and power cells as duals.
6. `mesh/frames`: rotor frame fields for quad and hex.
7. `mesh/quality`: Jacobians, dihedral angles from plane antiwedges and dots, aspect ratios.

## Robust predicates (`galoot/robust`)

Because Galoot's plan compiler already turns any GA expression into an explicit polynomial in the input coordinates, it can also generate the predicate's error bound at compile time. A robust predicate is then just "the sign of one coefficient", compiled into Shewchuk-style stages. No GA-specific robust-predicate literature turned up in this research, so this is a genuine gap Galoot can fill.

### Classical predicates are wedge products

| Predicate | GA form | Equivalent determinant |
| --- | --- | --- |
| orient2d / orient3d | Top coefficient of p₁ ∧ … ∧ p\_{d+1} in RGA | det\[xᵢ 1\] |
| incircle (2D CGA) | Top coefficient of P₁ ∧ P₂ ∧ P₃ ∧ P₄ | ½ det\[xᵢ yᵢ 1 ‖xᵢ‖²\] |
| insphere (3D CGA) | Top coefficient of P₁ ∧ … ∧ P₅ | ½ det\[xᵢ yᵢ zᵢ 1 ‖xᵢ‖²\] (lifting map) |
| Regular-triangulation (power) test | Same 5-blade with dual spheres in place of points | Lifted coordinate ‖x‖² − r² |
| Point vs. sphere | Round point · sphere | −½(‖x − c‖² − r²) |
| Sphere-cover tests | Square of S₁ ∧ … ∧ Sₖ | ±det of the Gram matrix |

The insphere row was checked numerically: for five random round points the Gram determinant equals −(½ det lift)², consistent with the 5-blade's coefficient being ½ det lift. Orientation sign conventions must still be pinned by unit tests against Galoot's chosen basis order.

### Predicate generation

The user writes the predicate as ordinary GA code. The library evaluates it once at compile time with a symbolic scalar to extract the polynomial, then builds the filter:

```cpp
inline constexpr auto insphere = galoot::predicate<cga3>(
  [](auto a, auto b, auto c, auto d, auto e) {
    return antiscalar(a ^ b ^ c ^ d ^ e);    // coefficient of the top blade
  });

int s = insphere(pa, pb, pc, pd, pe);        // -1, 0 or +1, always correct
```

The generated predicate runs four stages:

1. **Static or semi-static filter.** Evaluate in `double` and compare with an error bound derived from the polynomial's structure at compile time, in the manner of Meyer–Pion FPG and Bartels–Fisikopoulos–Weiser ([arXiv 2208.00497](https://arxiv.org/abs/2208.00497)). Translation to differences relative to the first point is applied first, as Shewchuk recommends.
2. **Interval arithmetic.** The same kernel with an interval scalar, as in CGAL's `Filtered_kernel`.
3. **Exact expansions.** The same kernel with a Shewchuk expansion scalar (TWO-SUM, TWO-PRODUCT via FMA).
4. **Symbolic perturbation.** Edelsbrunner–Mücke simulation of simplicity, as in Geogram's PCK, so exact zeros resolve consistently.

Shewchuk's published constants (ε = 2⁻⁵³) give the targets the generator must reproduce for the four standard predicates ([paper](https://www.wias-berlin.de/people/si/course/files/Shewchuk96-predicates.pdf)):

| Predicate | Stage A bound | Stage B bound | Stage C bound |
| --- | --- | --- | --- |
| orient2d | (3 + 16ε)ε | (2 + 12ε)ε | (9 + 64ε)ε² |
| orient3d | (7 + 56ε)ε | (3 + 28ε)ε | (26 + 288ε)ε² |
| incircle | (10 + 96ε)ε | (4 + 48ε)ε | (44 + 576ε)ε² |
| insphere | (16 + 224ε)ε | (5 + 72ε)ε | (71 + 1408ε)ε² |

Shewchuk reports about 8% overhead for 2D Delaunay and 35% for 3D on random points, which sets the performance goal.

### Constructed objects stay polynomial

Attene's indirect predicates ([arXiv 2105.09772](https://arxiv.org/abs/2105.09772)) represent a constructed point as polynomial numerators over a denominator, and evaluate predicates on it without division. Homogeneous GA already stores points that way: a meet of planes or lines is a polynomial (x, y, z, w). Galoot can therefore treat any flat built by wedges and antiwedges as an **implicit object**: it keeps its generating inputs and a cached magnitude bound, and predicates compose its plan with the predicate's plan. Attene reports 3.9–11.7× speedups over CGAL's lazy-exact kernel with this approach.

### Square roots appear only in rounds

Splitting a dipole into its two points needs √‖d‖². A single test on such a point has the form a + b√c with c ≥ 0, which has an exact sign rule:

```latex
\operatorname{sign}(a + b\sqrt{c}) = \begin{cases} \operatorname{sign}(a) & \text{if } \operatorname{sign}(a) = \operatorname{sign}(b) \text{ or } b = 0 \\ \operatorname{sign}(a)\,\operatorname{sign}(a^2 - b^2 c) & \text{otherwise} \end{cases}
```

VoroCrust's coverage logic avoids even this, as the meshing section shows. The hard case is the Voronoi diagram *of* the seeds: an insphere test on five seeds can involve five different square roots. Three options:

1. **Round seeds to doubles** and record a per-seed error bound; run exact Delaunay on the rounded seeds. This is effectively what VoroCrust does with Voro++ and is the recommended default.
2. **Exact real expression DAGs** (LEDA real, CORE) with separation bounds, for certification runs.
3. **Perturbed exact constructions** in the style of Irving and Green ([arXiv 1308.1986](https://arxiv.org/abs/1308.1986)).

## General algebras (`galoot/alg`)

The exact statement is narrower than "GA maps to the C\*-algebras of QFT": complex Clifford algebras are exactly the finite-mode **CAR algebras** (canonical anticommutation relations) of free fermions, and they are dense in the CAR C\*-algebra. Bosons, interacting fields and local algebras of algebraic QFT need other structures. Within that scope, a runtime Clifford engine sharing the core's bitmask rules is both exact and efficient.

**Terms.** A C\*-algebra is a complete normed algebra with an involution \* satisfying ‖a\*a‖ = ‖a‖². *Majorana operators* cμ are Hermitian generators with {cμ, cν} = 2δμν, i.e. orthonormal Clifford vectors. A *Witt basis* is a basis of null vectors fⱼ, fⱼ† with fⱼ² = 0 and fⱼ fₖ† + fₖ† fⱼ = δⱼₖ; these are fermion annihilation and creation operators.

### Where the mapping holds and where it does not

| Claim | Status | Detail |
| --- | --- | --- |
| Cl(2n, ℂ) ≅ M\_{2^n}(ℂ) | Holds | Jordan–Wigner gives the unique representation up to unitary equivalence ([Jozsa–Miyake](https://arxiv.org/abs/0804.4050)) |
| Fock space = spinor module | Holds | Minimal left ideal Cl·I with vacuum idempotent I = ∏ fⱼ fⱼ† ([Hrdina–Návrat–Vašík](https://arxiv.org/abs/2201.02246)) |
| Infinite-mode CAR C\*-algebra | Holds as a limit | UHF algebra of type 2^∞; Araki's self-dual form is a C\*-Clifford algebra over a real Hilbert space |
| Bogoliubov transformations | Holds | Orthogonal maps R act as automorphisms; implementability in a given representation needs Hilbert–Schmidt conditions ([Carey 1984](https://www.numdam.org/item/AIHPA_1984__40_2_141_0.pdf)) |
| \*-structure from the metric | Fails in general | In Cl(1,3), γ⁰ is Hermitian and γⁱ anti-Hermitian, so the Hilbert adjoint is not the GA reverse; the involution must be a separate input |
| Bosons (CCR, Weyl algebra) | Not Clifford | No finite-dimensional representation; uses an antisymmetric (symplectic) form; no bitmask basis |
| Algebraic QFT (Haag–Kastler nets) | Beyond scope | Local algebras are type III₁ factors with no trace; fields at spacelike separation anticommute and need a ℤ₂ grading |
| Interacting 4D QFT | No known Clifford presentation | Clifford covers free (quasi-free) fermions and Gaussian dynamics |

### Quantum computing uses

- **Pauli strings are bitmask blades.** Jordan–Wigner is an invertible linear map over GF(2) from Majorana masks to (x, z) Pauli masks; both products are XOR plus a phase from `popcount`. This is the same arithmetic as Galoot's core.
- **Matchgates are Spin(2n) rotors.** A quadratic Hamiltonian is a bivector in Majorana generators; its exponential acts on generators as R ∈ SO(2n). Such circuits are classically simulable in polynomial time (Valiant; [Terhal–DiVincenzo](https://arxiv.org/abs/quant-ph/0108010); Jozsa–Miyake).
- **Gaussian expectations are Pfaffians** of submatrices of the covariance matrix Mₐᵦ = (i/2) Tr(ρ\[cₐ, cᵦ\]) ([Bravyi](https://arxiv.org/abs/quant-ph/0404180)).
- **Three things are called "Clifford":** the algebra, the Clifford–Lipschitz (Pin/Spin) group, and the quantum-computing Clifford group (normalizer of the Pauli group). Galoot should use distinct names, for example `SpinRotor` and `PauliTableau`.
- **GA circuit formalisms** exist: Havel–Doran's multiparticle spacetime algebra and the Quantum Register Algebra of Hildenbrand and coauthors ([arXiv 2208.02608](https://arxiv.org/abs/2208.02608)). In orthonormal blades a 2-qubit basis ket needs 4 coefficients, but only 1 in the Witt basis, so basis choice decides sparsity.

### Interface for the `alg` module

1. **Algebra descriptor:** a scalar ring (ℝ, ℂ or a Grassmann ring), a full bilinear form split into symmetric part (metric, possibly degenerate) and antisymmetric part (ordering), a separate \*-involution, a runtime dimension with multiword masks, and a ℤ₂ grading flag.
2. **Bases with exact conversions:** orthonormal (Majorana and Pauli), Witt matrix units (Fock), and Grassmann symbols. The Grassmann case reuses the same storage with a zero metric, which also gives Berezin integration as top-coefficient extraction.
3. **Storage:** sparse hash-map multivectors; dense bit-packed Pauli sets in the style of [Stim](https://arxiv.org/abs/2103.02202); a dense matrix backend when products are dense; structured objects `SpinRotor{R, sign}`, `GaussianState{M}`, `PauliTableau`.
4. **Operations:** products, reverse, adjoint, grade projection; normal ordering and Wick contraction (changing the antisymmetric part of the form); `expect(state, x)`; graded and ungraded tensor products; Jordan–Wigner and Bravyi–Kitaev maps; bivector → SO(2n) and Givens decomposition into nearest-neighbor gates.
5. **Interoperability and export:** OpenFermion `FermionOperator` and `QuadraticHamiltonian`, Qiskit `SparsePauliOp`, Stim `PauliString` and `Tableau`, and [OpenQASM 3](https://openqasm.com/language/gates.html) text output. Pauli-product rotations must be synthesized (basis change, CNOT ladder, rz) because OpenQASM has no native form; QIR is reachable from OpenQASM through existing toolchains.
6. **Out of scope, delegated:** dimensional-regularization gamma traces in non-integer dimension D go to FORM or FeynCalc; bosonic Gaussian states go to a separate symplectic module.

**Scaling.** A uint64 mask caps the algebra at 64 generators (32 fermionic modes), and a dense complex128 Cl(30) needs about 17 GB. Gaussian operations stay polynomial (O(n³) per rotor composition); expanding a rotor into blades can take 2^n terms, so rotors stay in structured form.

## Repository layout, testing and roadmap

Ship the core and the RGA3/CGA3 models first, with conventions frozen by tests, before any application module. Every later module is a client of the public core API, which keeps the core small.

### Layout

This single-tree layout is superseded by the multi-repository structure later in this document. Each repository keeps the `include/galoot/<module>/` path, so headers from different repositories compose in one include tree.

Within each repository the default header (for example `<galoot/core.hpp>`) never pulls in optional headers such as reflection.

### Testing

- **Algebraic identities** as property tests over random signatures R(p,q,r) with n ≤ 6: associativity of ⟑ and ⟇, all four De Morgan laws, complement involution (left of right is identity), u ∧ ū = 𝟙, reverse and antireverse relations.
- **Golden values derived from the mathematics:** expected results for transforms, joins, meets and CGA centers and radii come from Lengyel's published formulas, evaluated by an independent symbolic script, not from another library's code.
- **Black-box cross-checks, late:** once Galoot's own tests pass, compare outputs (never source) with Terathon and kingdon or ganja.js on random inputs, and document any disagreement such as the dual sign.
- **Compile-time budgets** in CI: time and memory to compile the full RGA3 and CGA3 test suites on GCC and Clang, failing on regressions.
- **Predicate torture tests:** Shewchuk-style near-degenerate inputs (points on a grid perturbed by one ulp), with exact answers from rational arithmetic.
- **Benchmarks** on ga-benchmark's operations and models, plus Monte Carlo ray throughput and mesh-generation throughput.

### Phased roadmap

1. **Conventions spec.** Freeze basis order, names, dual signs, half-angle convention, operator set and precedence guidance. Gate: a written spec and a golden-value test file.
2. **Core for diagonal metrics.** bits, metric, blade sets, plan compiler, `mv`, products, antiproducts, complements, norms. Gate: RGA2/RGA3 pass identity and golden tests on GCC 15+ and Clang 19+.
3. **Non-diagonal metrics.** Exact dyadic coefficients for null bases; CGA2/CGA3 models with Lengyel's round and flat operations. Gate: CGA golden tests and diagonal/null-basis equivalence tests.
4. **Fused plans and performance.** Symbolic like-term collection for sandwiches and projections; SIMD lane-pack scalars. Gate: within 10% of Terathon's hand-written motor transforms (a proposed target, not a measured one).
5. **Robust predicates.** Predicate generator, expansions, intervals, simulation of simplicity. Gate: matches Shewchuk's orient and insphere on torture tests.
6. **Modeling kernel.** Surfaces with motor placement, CSG senses, ray distances, B-rep export. Gate: agreement with an independent CSG ray tracer on benchmark geometries.
7. **Meshing.** Sphere covers, VoroCrust seeds, Delaunay and Voronoi, quality metrics; frame fields after that. Gate: watertight conforming meshes on standard CAD test models.
8. **General algebras.** Runtime Clifford engine, Pauli and Gaussian types, OpenFermion and Qiskit interop, OpenQASM 3 export. Gate: round-trip tests against OpenFermion and Stim.

## Twistors and further core extensions

Twistors need nothing new in the core beyond two generic features worth having anyway: complex scalars and spinor modules. Colapinto's twistor-flavored constructions (Hopf fibers, torus knots, families of twisting circles) are ordinary CGA3 modeling and belong in `geom/form`.

### Where twistors fit

- **Definition.** A Penrose twistor is an element of ℂ⁴ carrying the spinor representation of the conformal group of Minkowski spacetime, SU(2,2) ≅ Spin(4,2). In GA terms twistors live in the six-dimensional conformal spacetime algebra R(4,2) ([Arcaute, Lasenby, Doran](https://arxiv.org/abs/math-ph/0603037); [AACA 2008](https://link.springer.com/article/10.1007/s00006-008-0083-x)).
- **Connection to Galoot's CGA3.** The even subalgebra of R(4,2) is isomorphic to R(4,1) ≅ M₄(ℂ), because Cl⁺(p,q) ≅ Cl(p,q−1). Twistor space is a minimal left ideal of that algebra, so CGA3's product tables serve, given complex scalars and an idempotent projection.
- **Geometric picture.** Penrose draws a twistor as the Robinson congruence, a family of twisting light rays whose spatial picture is nested Villarceau circles on tori (a Hopf fibration). Arcaute et al. derive the congruence inside conformal GA ([follow-up paper](https://arxiv.org/abs/math-ph/0604048)). This is most likely the link to Colapinto's Hopf-fiber knots in `vsr_knot.h`; the exact dissertation passages were not available to confirm.
- **For Galoot, two separate tracks:**
  1. Modeling: Hopf fibrations, Villarceau circles and twisting circle families as `geom/form` constructions from CGA3 circles and conformal rotors. No core change.
  2. Physics: a later `models/cst` for R(4,2) with the twistor incidence relation, feeding the `alg` module for light-ray geometry and massless fields. Momentum-twistor amplitude methods are out of scope for now.

### Extensions that make Galoot a generative algebra tool

The first three turn Galoot from a library into a generator, and each reuses the plan compiler instead of adding new machinery.

| Extension | What it adds | Location | Priority |
| --- | --- | --- | --- |
| Plans as public data with code-generation backends | Export term tables to C, CUDA, Julia or SystemVerilog (hardware GA kernels), and to a proof assistant such as Lean 4 to check identities and tables | core export + external tools | High |
| General bilinear forms and a separate involution | Non-orthogonal and non-symmetric forms at compile time: Clifford algebras of arbitrary bilinear forms, normal ordering and Wick contraction | core `metric` | High |
| Outermorphisms | Extend any linear map on vectors to all grades; determinants as top-grade coefficients; exact change of basis between R(4,1,0) and conformal(3); deformation gradients for FEA | core | High |
| Spinor modules and matrix representations | Primitive idempotents, minimal left ideals, compile-time isomorphism between Cl(p,q) and its matrix algebra (the periodicity table) | core | Medium |
| Algebra composition | Graded tensor products Cl(p,q) ⊗̂ Cl(r,s) ≅ Cl(p+r, q+s), even subalgebras, embeddings between models (RGA3 → CGA3 by ∧ e5) | core | Medium |
| Lie layer for bivectors | Commutator algebra; generic exp and log by invariant decomposition into commuting simple bivectors ([Roelfs, De Keninck](https://arxiv.org/abs/2107.03771)); composition rules | core ops | Medium |
| More scalar rings | Dual numbers for autodiff, exact rationals, multiprecision, Grassmann-valued coefficients for superalgebras | separate scalar headers | Medium |
| Geometric calculus and discrete exterior calculus | Vector derivative with autodiff scalars; discrete forms, Whitney elements and Hodge stars on Galoot meshes, linking `mesh` to FEA (finite element exterior calculus) | new `calc` module | Medium, after `mesh` |
| Mother algebra R(n,n) | General linear maps represented as versors (Doran–Hestenes) | model | Low |

Kept out deliberately: non-associative algebras such as octonions, and the bosonic Weyl algebra, which has no bitmask basis.

## Anyonic and braided extensions

The compile-time signature generalizes in two tiers. **Tier 1** keeps Galoot's single-term plan compiler: replace the blade bitmask with a mixed-radix digit vector and the sign (−1)^popcount with a root-of-unity phase. That one change covers qudit Pauli algebras, Z\_n parafermions, q-exterior algebras and abelian anyon braidings. **Tier 2** covers non-abelian anyons (Fibonacci, SU(2)\_k, Chern–Simons), which are not algebras with a monomial basis but fusion-category data. They get their own generator, `galoot/anyon`, with signatures such as `su2_level<k>`. Ising anyons sit in both tiers, because their braids are already Clifford rotors.

**Terms.** A *bicharacter* on an abelian group G is a map ε: G×G → ℂ\* that is multiplicative in each argument. A *twisted group algebra* has basis {e\_g : g ∈ G} and product e\_g e\_h = σ(g,h) e\_{g+h}; it is associative exactly when σ is a 2-cocycle. A *braiding* is a natural swap Ψ: X⊗Y → Y⊗X; it is *symmetric* when Ψ² = id, and *anyonic* otherwise. A *fusion category* is a set of particle labels with fusion rules and the F and R matrices described below.

### Tier 1: the graded signature

Albuquerque and Majid show that every Clifford algebra is the group algebra of Z₂ⁿ twisted by the cocycle F(x,y) = (−1)^{Σ\_{j\<i} x\_i y\_j} ∏ q\_i^{x\_i y\_i}, which is exactly Galoot's reordering sign times the metric ([math/0011040](https://arxiv.org/pdf/math/0011040)). Generalizing the group to Z\_{N₁}×…×Z\_{N\_n} gives the rule:

```latex
e_a\, e_b \;=\; \zeta_m^{\,a^{\mathsf T} S\, b} \prod_i \kappa_i(a_i, b_i)\; e_{(a+b) \bmod N}, \qquad \kappa_i = \begin{cases} 1 & a_i + b_i < N_i \\ \lambda_i & a_i + b_i \ge N_i \end{cases}
```

Here a and b are digit vectors (aᵢ ∈ {0, …, Nᵢ−1}), ζ\_m = e^{2πi/m}, S is a strictly lower-triangular integer matrix with e\_k e\_j = ζ\_m^{S\_kj} e\_j e\_k for k > j, and λᵢ is the power eᵢ^{Nᵢ} (0 means nilpotent). With Nᵢ = 2, m = 2 and all S\_kj = 1, the phase equals the popcount reordering sign; this was checked for all 256 blade pairs with n = 4.

```cpp
struct graded_signature {                               // structural: a template argument
  std::uint8_t n;                                       // number of generators
  std::array<std::uint8_t, 32> radix;                   // N_i
  std::array<exact_scalar, 32> power;                   // lambda_i = e_i^{N_i}; 0 => nilpotent
  std::array<std::array<std::uint16_t, 32>, 32> phase;  // S_kj for k > j
  std::uint16_t m;                                      // phases are powers of zeta_m
};
consteval graded_signature clifford(metric g);          // N_i = 2, m = 2: today's Galoot
consteval graded_signature clock_shift(int n, int N);   // qudit Pauli (Weyl-Heisenberg) algebra
consteval graded_signature parafermions(int L, int N);  // 2L Fradkin-Kadanoff generators
consteval graded_signature quantum_exterior(auto q);    // x_i x_j = q_ij x_j x_i, x_i^{N_i} = 0
consteval graded_signature braided_product(graded_signature A, graded_signature B, int n);
```

| Algebra | Radix Nᵢ | Power λᵢ | Phases | Use |
| --- | --- | --- | --- | --- |
| Clifford (current Galoot) | 2 | metric square: +1, −1 or 0 | −1 | Geometry, Dirac, Majorana |
| Grassmann | 2 | 0 | −1 | Differential forms, Berezin integration |
| Generalized Clifford, clock and shift | N | 1 | ω = e^{2πi/N} | Qudit Pauli and Weyl–Heisenberg operators ([Jagannathan](https://arxiv.org/abs/1005.4300)) |
| Z\_n parafermions | n | 1 | ω, in a fixed generator order | Clock chains and parafermion zero modes ([Fendley](https://arxiv.org/abs/1209.0472)) |
| Quantum linear spaces, q-exterior algebras | ord(qᵢᵢ) | 0 | qᵢⱼ with qᵢⱼqⱼᵢ = 1 for the Nichols (braided Hopf) structure; any q as a plain algebra | Braided exterior algebras ([Andruskiewitsch–Angiono](https://ar5iv.labs.arxiv.org/html/2002.11087)) |
| Color algebras (Scheunert) | grading group | — | bicharacter with ε(a,a) = ±1 | Color Lie superalgebras; still symmetric, not anyonic |
| Majid's anyonic Z\_n spaces | n | per factor | e^{2πi ab/n} between factors | "Anyspace" and anyonic Grassmann variables ([Majid–Rodríguez-Plaza](https://arxiv.org/abs/hep-th/9408056)) |

**Composition.** The braided tensor product A ⊗̲ B, with (x⊗y)(w⊗z) = x Ψ(y⊗w) z, generalizes the super tensor product Cl(V⊕W) ≅ Cl(V) ⊗̂ Cl(W). Its signature is the block matrix of S\_A, S\_B and the braiding phases. Parafermions are N copies of ℂ\[ψ\]/(ψⁿ − 1) composed this way.

**Correctness conditions, checked at compile time:**

1. If λᵢ ≠ 0 then every phase involving generator i must satisfy cᵢⱼ^{Nᵢ} = 1. A test with cube-root phases on binary digits and λ = 1 failed associativity in 826 of 3,000 random triples; with λ = 0 it never failed.
2. Phases must form a bicharacter (equivalently σ is a 2-cocycle). Without it the result is a *quasialgebra*: the octonions arise from Z₂³ this way. Galoot can offer this as an explicit opt-in, since kernels then stop composing associatively.
3. A braiding whose self-phase is not of bicharacter form, such as the semion with c(1,1) = ±i, needs a non-trivial associator (a 3-cocycle) and leaves Tier 1.
4. In a non-symmetric braiding, fully braided-commutative generators force q² = 1, so the generator order is part of the signature, not a convention to normalize away.

**Exact numbers.** Each plan term stores its phase as an integer mod m and its power factors as a monomial in λ, so the compiler never does field arithmetic. Arithmetic in the cyclotomic field ℚ(ζ\_m) is needed only for summed expressions and transforms: m = 4 gives ℤ\[i\], m = 3 or 6 the Eisenstein integers, m = 8 contains √2 (Ising), and m = 5 or 10 contains the golden ratio.

**Performance rule.** When every radix is 2, the plan compiler keeps today's bitmask path, selected with `if constexpr`, so Clifford users pay nothing for the generalization.

### What Tier 1 cannot express

- **Nichols algebras with qᵢⱼqⱼᵢ ≠ 1:** they have PBW bases of root-vector powers, but products are multi-term. Heckenberger classified the finite ones ([math/0605795](https://arxiv.org/abs/math/0605795)).
- **Non-symmetric bilinear forms:** Fauser and Abłamowicz's quantum Clifford algebras are Wick-isomorphic to ordinary ones as ℤ₂-graded algebras but mix grades ([math/9911180](https://arxiv.org/pdf/math/9911180)). Fauser builds Hecke algebra representations from bivectors when the form has an antisymmetric part ([q-alg/9710020](https://arxiv.org/abs/q-alg/9710020)). Galoot handles these through the plan compiler's exact change-of-basis path, with multi-term kernels.
- **Associator-twisted braidings** (semions) and the bosonic Weyl algebra at generic q (infinite-dimensional).
- **Non-abelian fusion:** Fibonacci and SU(2)\_k with k ≥ 3. Kauffman and Lomonaco note that the Fibonacci model is not directly related to the Clifford algebra ([1710.04650](https://ar5iv.arxiv.org/html/1710.04650)).

### Ising anyons are already in the core

Ivanov's exchange of Majorana vortices is the rotor τ\_k = exp(π/4 · c\_{k+1}c\_k) = (1 + c\_{k+1}c\_k)/√2 ([cond-mat/0005069](https://ar5iv.labs.arxiv.org/html/cond-mat/0005069)), matching Nayak and Wilczek's spinor representation of SO(2n) on 2n quasiholes ([cond-mat/9605145](https://arxiv.org/abs/cond-mat/9605145)). With Jordan–Wigner Majoranas these satisfy τ₁τ₂τ₁ = τ₂τ₁τ₂, far commutation and τ⁸ = 1, checked numerically. Bravyi shows Ising braids give only Clifford-group gates, so universality needs magic states ([quant-ph/0511178](https://arxiv.org/abs/quant-ph/0511178)).

### Tier 2: `galoot/anyon`, signatures for fusion categories

An anyon model (a unitary modular tensor category) is the data: labels with a vacuum and duals; fusion multiplicities N\_ab^c; F-symbols satisfying the pentagon equation; R-symbols satisfying the hexagon equations; and derived twists θ\_a, quantum dimensions d\_a and modular matrices S and T ([Kitaev, App. E](https://arxiv.org/abs/cond-mat/0506438)). Ocneanu rigidity makes solutions discrete up to gauge, so a small specification fixes the model up to finitely many choices. Braid generators on a fusion-tree basis are local basis changes:

```latex
(\sigma_i)_{x'x} = \sum_y \left[\big(F^{\,x_{i-1}\,a\,a}_{x_{i+1}}\big)^{-1}\right]_{x'y} R^{aa}_y \left[F^{\,x_{i-1}\,a\,a}_{x_{i+1}}\right]_{yx}
```

| Signature | What it generates | Data source |
| --- | --- | --- |
| `su2_level<k>` | Rank k+1; fusion by truncated Clebsch–Gordan; d\_a = \[a+1\]\_q; θ\_a = q^{a(a+2)/4}; F from Kirillov–Reshetikhin quantum 6j symbols; closed-form R and S; q = e^{2πi/(k+2)} | Closed forms in [Ardonne–Slingerland, App. E](https://arxiv.org/abs/1004.5456) |
| `so3_level<k>` | Integer-spin restriction; Fibonacci = `so3_level<3>` | Same |
| `zn<N, w>` | Abelian Z\_N^{(w)} anyons, F nontrivial for half-integer w | [Bonderson–Shtengel–Slingerland](https://arxiv.org/abs/0707.4206) |
| `abelian_K<K>` | U(1)^r Chern–Simons / K-matrix anyons: labels ℤʳ/Kℤʳ, θ\_l = e^{iπ lᵀK⁻¹l} | [Wen](https://arxiv.org/abs/cond-mat/9506066); F and R by a Smith-normal-form construction |
| `ising<nu>` | Ising^{(ν)}, θ\_σ = e^{iπν/8}, ν odd; bridge `to_rotor()` into the Clifford core | Kitaev |
| `quantum_double<G, omega>` | Kitaev models D(G), twisted D^ω(G); labels are (conjugacy class, centralizer irrep) | [Coste–Gannon–Ruelle](https://arxiv.org/abs/hep-th/0001158) |
| `tabulated<N, F, R>` | Any model from exact tables | [Rowell–Stong–Wang](https://arxiv.org/abs/0712.1377) (rank ≤ 4), [AnyonWiki](https://anyonwiki.github.io), [Anyon Atlas](https://arxiv.org/abs/2610.25439) |
| `deligne_product<A, B>`, `restrict<A, labels...>` | Products and sub-theories, e.g. Moore–Read from Ising × Z₈ | Constructions |

Generated for every signature: labels and duals, N, d, θ, S, T, central charge mod 8, F, R, Frobenius–Schur indicators, fusion spaces `fusion_space<a, n, c>` and braid matrices `braid<i>`.

Two results came from this study's own numerical checks rather than the literature. The SU(2)\_k closed forms passed the pentagon (residual ≤ 9×10⁻¹⁶, k = 2–4) and both hexagons (≤ 2×10⁻¹⁵, k = 2–5). For `abelian_K`, the naive carry cocycle fails the pentagon for the A₃ Cartan matrix (SU(4)₁). A construction that diagonalizes K by Smith normal form, uses Z\_N^{(w)} data per cyclic factor and adds a triangular bicharacter to R passed pentagon, hexagons and twists for ten test matrices, including A₂–A₄ and D₄. Odd K (fermionic theories) need a supermodular treatment.

**Exact numbers.** Phases θ and R are roots of unity in the cyclotomic gauge, and S and T lie in cyclotomic fields: SU(2)\_k uses ℚ(ζ\_{4(k+2)}). The *unitary* gauge can leave them: the unitary Fibonacci F needs φ^{−1/2}, whose splitting field has a non-abelian Galois group (D₄), so it lies in no cyclotomic field. Galoot should store data exactly in the cyclotomic gauge and generate the unitary gauge as a diagonal transform by square roots, using three number types shared with Tier 1: `phase<p, q>`, `cyclo<M>` and `radical<M>` (Σ√P).

**Compile-time cost.** A single F-symbol costs O(k) q-number products. The number of admissible F-symbols for SU(2)\_k is 36 at k = 2, 784 at k = 5, about 17,700 at k = 10 and about 643,000 at k = 20, so tables are generated lazily. Hexagon, ribbon, Verlinde and S-unitarity checks run as `static_assert`s; the full pentagon sweep is O(rank⁹) and belongs in one dedicated test translation unit. Fusion-space dimensions grow like d\_aⁿ (Fibonacci numbers for τ), so braid matrices are built on demand.

**Prior art for input design.** SageMath's `FusionRing("A1", k)` computes fusion, twists and S exactly and solves for F with Gröbner methods; TensorKitSectors.jl exposes `Nsymbol`, `Fsymbol`, `Rsymbol` per sector type; TensorCategories.jl computes Drinfeld centers exactly. Galoot's difference is generating the data as constexpr from a template argument.

### Chern–Simons theory

Chern–Simons enters Galoot in two separate ways:

1. **Quantum (topological) side.** Chern–Simons theory with group G at level k corresponds to a modular tensor category through the Reshetikhin–Turaev construction ([nLab](https://ncatlab.org/nlab/show/Chern-Simons+theory)). SU(2)\_k Wilson loops give the Jones polynomial at q = e^{2πi/(k+2)}, which is `su2_level<k>`. Abelian U(1)^r theories are `abelian_K<K>`. Link invariants then come from closing braid words with the quantum trace, using the same braid matrices.
2. **Classical and lattice field theory.** The action (k/4π) ∫ tr(A∧dA + ⅔ A∧A∧A) needs Lie-algebra-valued differential forms: Galoot's Grassmann algebra tensored with a Lie algebra, in the planned `calc` module. Lattice formulations need cochains with cup and cup-1 products, which are not graded-commutative ([Jacobson–Sulejmanpasic](https://arxiv.org/abs/2303.06160)), or a vertex–face bijection on the lattice ([Sun–Kumar–Fradkin](https://arxiv.org/abs/1502.00641)). That is a cochain module over Galoot's mesh complexes, separate from the Clifford product. Non-abelian lattice Chern–Simons remains an open research problem.

### Placement

- **Core:** the bitmask generalizes to a digit vector; `graded_signature` joins `metric`; Clifford stays the zero-cost special case.
- **Shared numbers:** `phase`, `cyclo`, `radical`, usable at compile time.
- **`galoot/anyon`:** fusion-category signatures, independent of the core except for numbers and the Ising `to_rotor()` bridge.
- **`galoot/calc` and a cochain module:** classical and lattice Chern–Simons on mesh complexes.
- **`alg`:** qudit and parafermion circuits from Tier 1, and braid-word compilation from Tier 2 (Fibonacci synthesis over ℤ\[e^{iπ/5}\], [Kliuchnikov–Bocharov–Svore](https://arxiv.org/abs/1310.4150)).

## Non-Hermitian systems

Non-Hermitian physics needs no new algebra. It needs three small core facilities: involutions as data, spectral operations on multivectors, and a superoperator doubling of the algebra. PT phases, exceptional points, biorthogonal states, Lindbladians and the 38-fold symmetry classification are then generated from those.

**Terms.** A *non-Hermitian* operator has H ≠ H†. It is *pseudo-Hermitian* when H† = ηHη⁻¹ for some invertible η, and *PT-symmetric* when it commutes with combined parity P and antilinear time reversal T. An *exceptional point* (EP) is a parameter value where eigenvalues and eigenvectors coalesce, so H cannot be diagonalized. *Biorthogonal* quantum mechanics pairs left and right eigenvectors instead of using H†.

### "Hermitian" is a choice of involution

- **In Cl(3,0) ≅ M₂(ℂ) with real coefficients,** Hermitian conjugation is reversion, complex conjugation is x ↦ σ₂ x̂ σ₂ (x̂ the grade involution), and transposition is x ↦ σ₂ x̂̃ σ₂. All three were checked numerically on random complex matrices. The imaginary unit is the pseudoscalar, so antilinear symmetries such as T and PT become ordinary real-linear (anti)automorphisms. The core needs no antilinear machinery.
- **In the spacetime algebra,** Hestenes' Hermitian adjoint is A† = γ₀ Ã γ₀: reversion twisted by the unit vector γ₀. In general a Hilbert-space adjoint is reversion (or Clifford conjugation) composed with conjugation by a unit element η. Pseudo-Hermiticity and Krein spaces (indefinite inner products) are a different η, or a signature with q > 0.
- **Core facility:** `involution<kind, eta>`, where `kind` is grade involution, reversion, Clifford conjugation or one of Shirokov's further sign operations. Each is a per-blade sign table folded into plans at zero cost; the η-twist compiles as a fused sandwich. This generalizes the earlier decision to give the \*-involution separately from the metric.
- **Symmetry classification.** Without Hermiticity, transposition and complex conjugation become distinct, which adds Hermitian-conjugate (AZ†) symmetries and raises the count to 38 classes with point and line gaps ([Kawabata, Shiozaki, Ueda, Sato](https://www.arxiv.org/abs/1812.09133)). With involutions as data, each symmetry operator is a concrete versor composed with an involution. The standard Hermitization H ↦ \[\[0, H\], \[H†, 0\]\] adds one anticommuting (chiral) generator, which is a Tier 1 signature composition.

### Exceptional points are nilpotent elements

Write any two-level H in Cl(3,0) as H = α + N, with N = a + Ib its vector and bivector part. Then N² = a² − b² + 2I(a·b) is a complex scalar, the eigenvalues are α ± √(N²), and an EP is exactly N² = 0 with N ≠ 0: N is **nilpotent**.

The PT-symmetric example H = \[\[r e^{iθ}, s\], \[s, r e^{−iθ}\]\] is r cosθ + s σ₁ + r sinθ σ₁σ₂, with N² = s² − r² sin²θ real:

| Sign of N² | Phase | Check at r = 1, θ = 0.7 |
| --- | --- | --- |
| N² > 0 | PT unbroken, real eigenvalues | s = 1: eigenvalues 1.530 and 0 |
| N² = 0, N ≠ 0 | Exceptional point, N nilpotent | s = sin 0.7 = 0.644: double eigenvalue 0.765 |
| N² < 0 | PT broken, complex-conjugate pair | s = 0.3: eigenvalues 0.765 ± 0.570i |

So PT phase boundaries and EPs are sign and zero tests of a single GA scalar, which is exactly what the robust predicate layer evaluates. This matters because eigensolvers are ill-conditioned at EPs, where eigenvalues move like the square root of a perturbation.

For larger algebras, Shirokov computes the determinant, characteristic polynomial and inverse of a multivector without matrices, using products of sign-table conjugations: U⁻¹ = F(U)/N(U) with N(U) = U F(U) a scalar, and explicit forms through n = 6 ([arXiv 2005.04015](https://arxiv.org/pdf/2005.04015)). Galoot can compile these as fused plans and add the Jordan–Chevalley split U = S + N (semisimple plus nilpotent) by polynomial arithmetic. EPs are where the nilpotent part is nonzero. General inverses are needed throughout the core anyway, so this is not a niche addition.

### Biorthogonal states

Left and right eigenvectors are elements of minimal right and left ideals. Their pairing ⟨φ̃ ψ⟩₀ uses reversion, not †, so it exists whether or not H is Hermitian. It comes with the spinor-module extension already proposed.

### Open systems and Liouvillians

A Lindblad generator is a non-Hermitian superoperator. Superoperators are left and right multiplications on the algebra, i.e. elements of A ⊗̂ A^op, and for Clifford algebras Cl(2n) ⊗̂ Cl(2n) ≅ Cl(4n). Prosen's third quantization makes this explicit: the 4ⁿ-dimensional operator space carries 4n adjoint fermionic maps with canonical anticommutation relations, quadratic Lindbladians become quadratic forms in them, and normal master modes come from a 4n×4n antisymmetric complex matrix ([arXiv 0801.1257](https://ar5iv.arxiv.org/html/0801.1257)). Gaussian open-system dynamics is therefore a complex bivector in Cl(4n), handled by the same rotor and covariance machinery as the `alg` module, with complex orthogonal SO(4n, ℂ) in place of SO(2n). This also raises the priority of the mother algebra R(n,n), which uses the same doubling to represent linear maps as versors.

### Minimal core additions

| Facility | Implementation | What it generates |
| --- | --- | --- |
| Involutions as data | Per-blade sign tables plus an η-twist sandwich | Hermitian, pseudo-Hermitian and Krein structures; PT; AZ and AZ† symmetry operators |
| Spectral operations | Shirokov's conjugation formulas compiled as plans; Jordan–Chevalley split | Determinants, inverses, characteristic polynomials, EPs, PT phases |
| Superoperator doubling A ⊗̂ A^op | Tier 1 signature composition with left and right multiplication maps | Liouvillians, third quantization, quantum channels |

Complex scalars are already in the scalar concept, and Cl(3,0) needs none because its pseudoscalar is central and squares to −1. Kept out of the core: non-Hermitian band topology (point-gap winding numbers, the skin effect), which is numerics in `alg`, and non-Gaussian Lindbladians, which remain exponential in the number of modes.

## Lattice and open quantum systems: `galoot/qsys` on `galoot/complex`

Non-Hermitian band topology and interacting Lindbladians belong in one higher-level module, `qsys`, built on a shared topology substrate, `complex`. `complex` is a cell-complex and cochain library that also serves boundary representations, meshes, discrete exterior calculus, lattice Chern–Simons and discretized Brillouin zones. `alg` shrinks to the runtime algebra engine, and the earlier `calc` module merges into `complex`.

### `galoot/complex`: one substrate for topology

**Data.** A cell complex is cells of each dimension with oriented boundary (incidence) matrices, plus optional geometry from the core (points, flats, rounds) and an optional metric for Hodge stars. A *cochain* assigns a coefficient to every oriented k-cell; coefficients can be scalars, core multivectors, group elements (U(1) or U(N) link variables, ℤ\_k), operators from `alg`, or anyon labels.

**Operations.** Coboundary d, cup and cup-1 products, Hodge star, Whitney interpolation, holonomy along cycles (Wilson loops) and plaquette fluxes.

| Client | Complex | Cochains |
| --- | --- | --- |
| `geom` B-rep | Vertices, edges and faces of a solid | Exact geometry attached to cells |
| `mesh` | Tetrahedra, hexahedra, polyhedra | Quality and sizing fields |
| FEA, discrete exterior calculus | Mesh cells | Discrete forms, Hodge stars, Whitney elements |
| Lattice Chern–Simons | Cubic lattice | ℝ, ℤ or ℤ\_k cochains with cup and cup-1 products |
| Berry phases, Chern numbers | Discretized Brillouin torus | U(1) or U(N) link variables |
| `qsys` lattices | Finite cluster with boundary conditions | Hoppings as operator-valued 1-cochains; Peierls phases as U(1) 1-cochains; flux as their coboundary |

The Fukui–Hatsugai–Suzuki method computes a Chern number from link variables on a discretized Brillouin zone: the sum of plaquette phases is an integer by construction and gauge invariant, and equals the Chern number once the mesh resolves the Berry curvature ([cond-mat/0503172](https://arxiv.org/abs/cond-mat/0503172)). It is a direct client of `complex`.

### `galoot/qsys`: system description

1. **Lattice.** `lattice<d>` gives a unit cell, basis sites and a finite cluster built as a `complex`. Boundary conditions: periodic, open, twisted by a flux angle, or an imaginary gauge transform with radius r for non-Hermitian work.
2. **Local algebra.** Each site carries a Tier 1 `graded_signature` (fermion modes, qudits, parafermions) or an anyon model from `galoot/anyon` (anyon chains).
3. **Operators.** Sparse sums of local monomials with exact or floating coefficients. Translation-invariant operators are hopping tables R ↦ local operator. A Lindbladian is a Hamiltonian plus jump operators {L\_μ}; Hermiticity is never assumed.
4. **Symmetries.** Versors plus involution data from the core, which fix the AZ or AZ† class; the gap type (point or line) is declared or detected.

### Representations chosen from structure

`qsys` picks a representation from what the operator is, so cost is paid only where the physics requires it:

| Structure | Representation | Cost |
| --- | --- | --- |
| Quadratic, translation-invariant | Matrix- or multivector-valued Laurent polynomial H(z) on the unit cell | Polynomial in the unit-cell size |
| Quadratic, no translation symmetry | One-body matrix; for Lindbladians, Prosen's 4n×4n third-quantization matrix | O(n³) |
| Clifford circuits | Stabilizer tableau | O(n²) per measurement |
| General interacting, closed | Sparse many-body matrix in the bitmask (digit-vector) basis, symmetry-reduced | 2ⁿ, or Nⁿ for qudits |
| General Lindbladian | Sparse superoperator from the A ⊗̂ A^op doubling | 4ⁿ |
| Lindbladian, larger n | Quantum trajectories (stochastic unraveling) | 2ⁿ per trajectory; statistical error falls as one over the square root of the trajectory count |
| Low entanglement in 1D | Matrix product states and operators, exported to ITensor or quimb | Polynomial in bond dimension |

### Non-Hermitian band theory

- **Laurent form.** A translation-invariant quadratic Hamiltonian is H(z) = Σ\_R h\_R z^R with z\_j = e^{ik\_j}. In 1D, f\_E(z) = det(H(z) − E) is a Laurent polynomial with exact coefficients. The determinant comes from the core's spectral operations when H(z) is multivector-valued, so no matrix representation is needed.
- **Point-gap winding** W(E) = (1/2πi) ∮ d log det(H(z) − E) around |z| = 1 equals the number of zeros minus poles of f\_E inside the unit circle (argument principle). Counting roots inside the unit disk is decidable in exact arithmetic with the Schur–Cohn test, so W(E) is a robust predicate, not a numerical integral.
- **Skin effect.** A nonzero point-gap winding forces the non-Hermitian skin effect under open boundaries ([Okuma, Kawabata, Shiozaki, Sato](https://ar5iv.labs.arxiv.org/html/1910.02878)). Check on the Hatano–Nelson chain with left and right hoppings 1 and 0.5: root counting gives W(0) = 1, and a 60-site open chain has a real spectrum spanning ±1.412 against the predicted ±2√(t\_L t\_R) = ±1.414, with all eigenvector weight in the left quarter of the chain.
- **Generalized Brillouin zone.** Order the 2M roots of det(H(β) − E) = 0 by modulus; open-chain continuum bands lie where |β\_M| = |β\_{M+1}| ([Yokomizo–Murakami](https://arxiv.org/pdf/1902.10958)). For Hatano–Nelson this is the circle of radius √(0.5/1) = 0.707. Sweeping the GBZ gives open-boundary spectra without diagonalizing large finite matrices, which are exponentially ill-conditioned in the chain length.
- **Pseudospectra and finite open chains** use the scalar concept's multiprecision and interval types when direct diagonalization is unavoidable.
- **Line-gap invariants** reduce to Hermitian or unitary flattening, then Chern numbers (Fukui–Hatsugai–Suzuki on `complex`), chiral windings, or ℤ₂ invariants from Wilson loops. Point gaps in higher dimensions use Hermitization. The class table of Kawabata et al. selects the invariant from the symmetry data.
- **Open research:** the skin effect and generalized Brillouin zones in two or more dimensions; `qsys` exposes the Laurent and root data rather than claiming a general algorithm.

### Open systems beyond Gaussian

Interacting Lindbladians stay exponential; `qsys` makes the cost explicit and lets the structure pick the method:

1. **Exact sparse Liouvillian** built from the A ⊗̂ A^op doubling, with Krylov (Arnoldi) methods for the spectral gap and steady state. At dimension 4ⁿ, about a dozen qubits is the practical limit on a workstation (4¹² ≈ 1.7×10⁷).
2. **Symmetry reduction.** Strong symmetries block-diagonalize the Liouvillian and weak symmetries constrain steady states ([Buča–Prosen](https://arxiv.org/pdf/1203.0943)); the symmetries are the same versor and involution data used for classification.
3. **Quantum trajectories** for larger systems, sharing the module's stochastic sampling infrastructure.
4. **Tensor networks** through export of vectorized density matrices and Liouvillian MPOs.
5. **Liouvillian exceptional points** and their relation to non-Hermitian effective Hamiltonians and postselected trajectories ([Minganti et al.](https://arxiv.org/pdf/1909.11619)) use the core's spectral operations directly.

Exports carry over from the earlier `alg` plan: OpenQASM 3, OpenFermion, Qiskit and Stim, plus ITensor and quimb for tensor networks.

### Revised module map

- **`alg`** keeps only the runtime algebra engine: runtime-dimension sparse and dense multivectors, Pauli and Majorana strings, and structured Gaussian, rotor and tableau objects. Physics semantics move to `qsys`.
- **`calc`** merges into `complex` for the discrete parts; continuous geometric calculus needs no module, because dual-number scalars give vector derivatives through the core.
- **`num`** names the shared exact number types: `phase`, `cyclo`, `radical`, expansions and intervals.
- **Dependency rule:** each layer uses only layers below it.

&#91;embedded content: Revised Galoot module map · five layers\]

`complex` sits below every application, so B-reps, meshes, lattice gauge fields and Brillouin-zone grids share one data structure; `qsys` is the only new application module.

## CPT symmetry and export to linear algebra, tensor networks and ZX

Two more core primitives close both gaps: **two-sided actions** (left and right versors plus an involution) and a **representation functor** that maps every generator to a Kronecker product of small matrices. Everything else is one small module, `galoot/sym`, and thin adapters.

### What general CPT needs

In the spacetime algebra, CPT is a *linear* operation. Write the Dirac–Hestenes equation as ∇ψ Iσ₃ − eAψ = mψγ₀, with I the pseudoscalar. Then

```latex
\psi(x) \mapsto I\,\psi(-x), \qquad A(x) \mapsto -A(-x)
```

maps solutions to solutions with the same charge e (derived for this study: ∇ anticommutes with I and x ↦ −x flips the sign of ∇, and the two signs cancel). Composing the usual forms P: ψ ↦ γ₀ψ(x̄)γ₀, C: ψ ↦ ψσ₁ and T: ψ ↦ Iγ₀ψ(−x̄)γ₁ gives the same map up to sign (these individual forms follow Doran and Lasenby's textbook as recalled here, not re-checked against the text). Four things are missing from the current design:

1. **Two-sided actions.** C acts by right multiplication and T by both sides, so a symmetry is x ↦ L·ι(x)·R, with L and R versors and ι an involution. Right multiplication already exists in the A ⊗̂ A^op doubling; the core needs only a `two_sided<L, R, iota>` action that compiles as a fused plan.
2. **Action on the base space.** Symmetries also move the argument of a field (x ↦ −x, x ↦ x̄ = γ₀xγ₀) and transform background fields (A ↦ −A(−x)). A symmetry is therefore a pair: an outermorphism or versor on the base space and a two-sided action on the fiber. On lattices the base action is an automorphism of the `complex`.
3. **Pin± is a signature choice.** The two double covers of the full Lorentz group differ in the squares of reflections: in Pin(1,3) a one-axis parity squares to −1 and T to +1, in Pin(3,1) the reverse ([Berg, DeWitt-Morette, Gwo, Kramer](https://ar5iv.labs.arxiv.org/html/math-ph/0012006)). Galoot already keeps R(1,3) and R(3,1) distinct, so this falls out of the signature; models should name both.
4. **Symmetry-group data, generated.** Freed and Moore describe any quantum symmetry group by (G, φ, t, c, τ): φ marks unitary versus antiunitary elements, t time reversal, c = φt whether an element commutes or anticommutes with H, and τ the projective twist; the ten-fold way and its Clifford-algebra modules are special cases ([Freed–Moore](https://ar5iv.arxiv.org/html/1208.5055)). Given generator versors, Galoot can close the group at compile time and read off the multiplication table, the twist (for example T² = ±1) and the symmetry class, instead of asking the user to state them.

Wick rotation fits the same pattern: an outermorphism with complex coefficients maps R(1,3) to a Euclidean signature, where CPT becomes a reflection-type symmetry. It needs only complex scalars and outermorphisms, both already planned.

### `galoot/sym`: one symmetry module

`sym` sits in the structures layer and closes finite groups of two-sided actions with Freed–Moore data. It unifies three earlier needs:

| Client | Groups | Use |
| --- | --- | --- |
| `geom/form` | Point groups, crystallographic space groups as versor products of mirrors (Colapinto's construction) | Symmetric modeling, lattice generation |
| `qsys` | AZ and AZ† classes, magnetic and crystalline groups | Selecting invariants, block-diagonalizing Hamiltonians and Liouvillians |
| Field theories | C, P, T, CPT on spinor and gauge fields, Pin± | Checking invariance; generating allowed terms |

### Export: one representation functor

A representation functor ρ assigns each generator a Kronecker product of small local matrices, chosen from the signature at compile time:

- **Clifford R(p,q,r):** nondegenerate generators in pairs per qubit, Z⊗…⊗Z⊗X or Y (times i for negative squares), and each degenerate generator on its own qubit as Z⊗…⊗Z⊗σ₊. This satisfied all relations and was faithful over the reals (all 2ⁿ blades independent) for R(3,0,1), R(4,1,0), R(1,3,0), R(3,1,0), R(3,0,0) and R(2,0,1), using 4×4 or 8×8 matrices.
- **Tier 1 graded signatures:** clock and shift matrices with Fradkin–Kadanoff strings, the qudit analogue of the same construction.

Every blade then maps to a single Kronecker product, so three exports follow from one primitive:

| Target | What ρ gives | Remaining work (adapter) |
| --- | --- | --- |
| General linear algebra | Dense or sparse matrices of any multivector; outermorphisms as matrices and back; determinants to cross-check the core's spectral operations | `mdspan` views and Eigen, BLAS or `std::linalg` interop |
| Tensor networks | Each blade is a product operator (bond dimension 1); a multivector with K terms is an MPO of bond dimension at most K, compressed by the finite-automaton construction ([Hubig, McCulloch, Schollwöck](https://arxiv.org/pdf/1611.02498)) | Writers for ITensor, quimb and TeNPy; Tier 1 phases and Tier 2 F and R symbols exported as sector data, so fermionic and anyonic networks keep their sign structure without Jordan–Wigner strings |
| Quantum compilation and ZX | A Pauli string is a blade, and exp(−iθP/2) is a single-blade rotor, which is a ZX phase gadget conjugated by single-qubit Cliffords ([Cowtan et al.](https://arxiv.org/pdf/1906.01734)); Clifford elements come from tableaux | PyZX or quizx graph writers; ZXW diagrams for sums and exponentials of Hamiltonians ([Shaikh, Wang, Yeung](https://arxiv.org/abs/2212.04462)); fermionic ZW calculus for native fermionic circuits ([de Felice, Hadzihasanovic, Ng](https://arxiv.org/abs/1801.01231v4)); OpenQASM 3 |

**Circuit representation.** The `alg` module holds a Pauli-based circuit form: a list of (blade, angle) rotations, a Clifford tableau and measurements. All export writers read that one structure.

**Exact phases.** Clifford+T angles are multiples of π/4, and exact synthesis works over ℤ\[1/√2, i\], a subring of ℚ(ζ₈). That is the same field the Ising anyons use, so `num` covers both.

### Core changes, summarized

| Addition | Size | Generates |
| --- | --- | --- |
| `two_sided<L, R, iota>` actions | One fused-plan pattern | C, P, T, CPT; gauge actions; superoperators |
| Representation functor ρ | Generator tables per signature kind | Matrices, MPOs, Pauli strings, ZX gadgets |

The module map gains `sym` in the structures layer, and the adapters layer gains `linalg`, `tn`, `zx` and `qasm` writers.

## Multi-repository structure

Galoot splits into fourteen repositories with dependencies that point only downward. Two roots stand alone: `galoot-core` (standard library only) and `galoot-num` (exact numbers). Every higher repository extends the core through public extension points and never requires a core change to add a model, scalar type or operation.

&#91;embedded content: Galoot repositories · layers and direct dependencies\]

The two highlighted roots build with nothing but a compiler; `galoot-num` reaches the core only through the `scalar` concept, and `galoot-conformance` is data that every repository's CI reads.

### Repositories

| Repository | Contents | Direct dependencies | Created at roadmap phase |
| --- | --- | --- | --- |
| `galoot-core` | Bits, metric and graded signatures, oriented blade sets, plan compiler, `mv`, named operations, complements, involutions, two-sided actions, spectral operations, representation functor, `scalar` concept; optional reflection header | Standard library only | 1–2 |
| `galoot-conformance` | Conventions spec, golden test vectors (language-neutral JSON), plans-as-data schema | None (data and spec) | 1 |
| `galoot-models` | Named models: RGA2/3, CGA2/3, STA in both signatures, conformal spacetime; graded-signature factories (clock and shift, parafermions, quantum exterior) | core | 2–3 |
| `galoot-num` | Cyclotomic, radical, expansion, interval and multiprecision scalar types | None; satisfies the core's `scalar` concept structurally | 3–5 |
| `galoot-robust` | Predicate generator, filters, symbolic perturbation | core, num | 5 |
| `galoot-sym` | Group closure of two-sided actions, Freed–Moore data, point, space and magnetic groups | core, num | 5–6 |
| `galoot-complex` | Cell complexes, cochains, coboundary, cup products, Hodge stars, holonomy | core | 6 |
| `galoot-anyon` | Fusion-category signatures and generated data | num; core only for the Ising rotor bridge | 8 |
| `galoot-alg` | Runtime algebra engine, Pauli and Majorana strings, Gaussian and tableau objects, circuit representation | core, num | 8 |
| `galoot-geom` | CSG, B-rep, motor-placed surfaces, parametric forms | models, robust, complex, sym | 6 |
| `galoot-mesh` | Sphere covers, VoroCrust seeds, Delaunay and Voronoi, frame fields, quality | geom, robust, complex | 7 |
| `galoot-qsys` | Lattices, Hamiltonians and Lindbladians, band theory, invariants, dynamics backends | alg, anyon, complex, sym, robust | 8 |
| `galoot-adapters` | viz, io, linalg, tn, zx and qasm components, each optional | The module it adapts, plus its external library | As needed |
| `galoot` | Umbrella: release trains, integration tests, examples, aggregated docs | All of the above | 2 onward |

Create each repository when its roadmap phase starts, so empty repositories never accumulate. Adapters share one repository because each is thin and changes with an external library rather than with Galoot.

### Rules

1. **Downward only.** A repository may depend only on repositories in lower layers; CI builds each one against its declared dependencies alone, so an undeclared include fails.
2. **The core is standalone.** It needs only a C++23 compiler and standard library, and works by adding one include directory, with no build system required.
3. **Extension without modification.** Higher repositories extend the core through public points, and the core never names a downstream type:
   - new models: `consteval` factories returning a `metric` or `graded_signature`, plus named blade sets;
   - new scalars: any type satisfying `galoot::scalar` (which is why `galoot-num` needs no core dependency);
   - new operations: compositions of the public plan API, involution tables and two-sided actions;
   - exports: plans-as-data and the representation tables.
4. **Uniform packaging.** Each repository installs headers under `include/galoot/<module>/`, uses namespace `galoot::<module>`, and exports a CMake target `galoot::<module>` with a package config. Consumers use `find_package`, `FetchContent`, CPM or plain include paths; vcpkg and Conan ports are optional. Nothing requires Nix or containers.
5. **Versioning.** Each repository uses semantic versioning. `galoot-core` 1.0 freezes the conventions spec; the plans-as-data schema carries its own version in `galoot-conformance`. The umbrella publishes calendar-versioned release trains that pin a known-good set, and each repository declares minimum versions of its dependencies.
6. **CI.** Each repository tests against the minimum and latest versions of its dependencies on GCC and Clang. A core change triggers downstream builds through the umbrella. Compile-time budgets run in core and models CI, and conformance vectors are pinned by tag.
7. **Governance.** Core changes go through written design decision records because every repository depends on them; higher tiers iterate freely within their own semantic versions.

## Risks and open questions

The biggest technical risk is compile time for large fused plans; the biggest correctness risk is sign-convention drift. Both are testable early, in phases 2–4.

| Risk | Effect | Mitigation |
| --- | --- | --- |
| C++26 delayed or renamed after the failed ballot | Reflection-based features unavailable on Clang and MSVC for an unknown period | C++23 baseline; reflection only in `reflect.hpp`; avoid contracts |
| Compile time of fused symbolic plans | Slow builds, as gal's 50 s IK benchmark showed | Memoized per-signature tables; fuse only declared hot paths; CI compile budgets |
| Constexpr step and memory limits for n ≥ 10 | Compile failures in large algebras | Sparse pair loops only; runtime `alg` module above about n = 10 |
| Lengyel wiki and Terathon disagree on dual signs | Silent sign errors in user code | Conventions spec in phase 1; explicit tests for each disagreement |
| Operator precedence of `^` and `&` | Misparsed expressions | Named functions canonical; parenthesized examples; optional clang-tidy check |
| Sandwich projection hides non-versor inputs | Wrong results for non-unit motors | Debug-mode check that dropped components vanish; `unitize` helpers |
| Square roots in seed coordinates | Non-conforming Voronoi in rare cases | Rounded seeds with recorded bounds; exact DAG option for certification |
| GPL references (GATL, TbGAL, Grassmann.jl, ga-benchmark) | License contamination | Clean-room implementation from the mathematics under MIT; other libraries used only as black-box cross-checks |
| Scope creep in `alg` toward full QFT | Core bloat | Keep CAR/Clifford only; delegate D-dimensional traces and bosons |

Decisions and recommendations as of 2026-10-31:

| Question | Status | Resolution |
| --- | --- | --- |
| Dual sign convention | Recommended | Right-complement duals (wiki convention); left variants available only by explicit name |
| Meaning of `*` | Recommended | Geometric product in every algebra; `%` for the antiproduct; `compose` and `transform` for motors |
| License | Decided | MIT; clean-room implementation from the mathematics |
| Quadric and torus surfaces | Decided | Concept only, not a requirement |

## References

Sources opened during this study, grouped by topic. Items marked "abstract only" or "metadata only" were not read in full.

**Versor and Colapinto**

- [wolftype/versor repository](https://github.com/wolftype/versor) and [issues](https://github.com/wolftype/versor/issues)
- [Colapinto, MS thesis, "Versor: Spatial Computing with Conformal Geometric Algebra" (2011)](http://wolftype.com/versor/colapinto_masters_final_02.pdf)
- [Colapinto, PhD dissertation, "Articulating Space" (UCSB)](https://escholarship.org/uc/item/5m76n8tg) (text read through Chapter 2 and the table of contents)
- [AGACSE 2018 proceedings, "The Design of Cyclidic Volumes"](https://upcommons.upc.edu/bitstream/2117/134258/1/9788498807035.pdf)
- [Fernandes, ga-benchmark analysis chapter (SEMA 2021)](https://profs.ic.uff.br/~laffernandes/content/publications/book_chapter/2021_sema_13/fernandes-sema-13-2021-chapter.pdf)

**Lengyel's model**

- [projectivegeometricalgebra.org](https://projectivegeometricalgebra.org/)
- [Rigid Geometric Algebra wiki](https://rigidgeometricalgebra.org/wiki/index.php?title=Main_Page)
- [Conformal Geometric Algebra wiki](https://conformalgeometricalgebra.org/wiki/index.php?title=Main_Page)
- [Terathon Math Library](https://github.com/EricLengyel/Terathon-Math-Library)
- Lengyel blog: [PGA Done Right](https://terathon.com/blog/pga-done-right.html), [Symmetries in PGA](https://terathon.com/blog/symmetries-pga.html), [Dual Approaches to PGA](https://terathon.com/blog/dual-pga.html)
- [Dorst and De Keninck, "A Guided Tour to the Plane-Based Geometric Algebra PGA"](https://geometricalgebra.org/downloads/PGA4CS.pdf)

**C++ and other libraries**

- [GCC C++ status](https://gcc.gnu.org/projects/cxx-status.html), [Clang C++ status](https://clang.llvm.org/cxx_status.html), [cppreference C++26 support](https://en.cppreference.com/w/cpp/compiler_support/26)
- [GCC 16.1 announcement](https://gcc.gnu.org/pipermail/gcc-announce/2026/000190.html); [C++26 ballot result](https://train-it.eu/blog/cpp26-contracts-the-ballot-result/); [P2996R13](https://isocpp.org/files/papers/P2996R13.html)
- [gal](https://github.com/jeremyong/gal), [GATL](https://github.com/laffernandes/gatl), [Klein](https://github.com/jeremyong/klein), [Garamon](https://github.com/vincentnozick/garamon), [Gaalop](https://github.com/CallForSanity/Gaalop), [kingdon paper](https://arxiv.org/html/2503.10451v1), [ganja.js](https://github.com/enkimute/ganja.js), [Grassmann.jl](https://github.com/chakravala/Grassmann.jl), [GA-FuL](https://www.mdpi.com/2227-7390/12/14/2272/html), [gafro](https://arxiv.org/pdf/2310.19090), [ga-benchmark](https://github.com/ga-developers/ga-benchmark)

**Meshing and robust predicates**

- [VoroCrust, ACM TOG 2020](https://arxiv.org/abs/1902.08767); [VoroCrust sampling conditions, SoCG 2018](https://arxiv.org/abs/1803.06078); [sandialabs/vorocrust-meshing](https://github.com/sandialabs/vorocrust-meshing)
- [Romero et al., "Constructing Voronoi Diagrams from Hollow Spheres Using CGA", AACA 2017](https://link.springer.com/article/10.1007/s00006-017-0787-x) (abstract only)
- [Edwards, Payne, Schafer, "Generalized Voronoi Diagrams and Lie Sphere Geometry"](https://arxiv.org/abs/2408.09279); [Hitzer, CGA introduction](https://arxiv.org/abs/1306.1358)
- [Shewchuk, adaptive precision predicates](https://www.wias-berlin.de/people/si/course/files/Shewchuk96-predicates.pdf); [Bartels et al., semi-static filters](https://arxiv.org/abs/2208.00497); [Attene, indirect predicates](https://arxiv.org/abs/2105.09772); [Irving and Green, black-box perturbation](https://arxiv.org/abs/1308.1986)
- [CGAL Filtered\_kernel](https://doc.cgal.org/4.8/Kernel_23/classCGAL_1_1Filtered__kernel.html); [CGAL Sqrt\_extension](https://doc.cgal.org/6.0/Number_types/classCGAL_1_1Sqrt__extension.html); [Geogram PCK](https://brunolevy.github.io/geogram/namespaceGEO_1_1PCK.html)
- [fTetWild](https://arxiv.org/abs/1908.03581); [Palmer, Bommes, Solomon, octahedral frames](https://arxiv.org/abs/1908.05411); [Beaufort et al., frame invariants](https://arxiv.org/abs/1910.06240)

**General algebras**

- [Jozsa and Miyake, matchgates](https://arxiv.org/abs/0804.4050); [Terhal and DiVincenzo](https://arxiv.org/abs/quant-ph/0108010); [Bravyi, Gaussian fermionic states](https://arxiv.org/abs/quant-ph/0404180); [Bravyi and Kitaev](https://arxiv.org/abs/quant-ph/0003137)
- [Hrdina, Návrat, Vašík, quantum computing in the Witt basis](https://arxiv.org/abs/2201.02246); [Quantum Register Algebra](https://arxiv.org/abs/2208.02608); [Havel and Doran](https://arxiv.org/abs/quant-ph/0004031)
- [Carey, Bogoliubov implementability (1984)](https://www.numdam.org/item/AIHPA_1984__40_2_141_0.pdf); [Halvorson, algebraic QFT](https://arxiv.org/abs/math-ph/0602036); [Buchholz and Grundling, resolvent algebra](https://arxiv.org/abs/0705.1988)
- [Stim](https://arxiv.org/abs/2103.02202); [OpenQASM 3 gates](https://openqasm.com/language/gates.html); [Qiskit SparsePauliOp](https://quantum.cloud.ibm.com/docs/en/api/qiskit/qiskit.quantum_info.SparsePauliOp.md)
