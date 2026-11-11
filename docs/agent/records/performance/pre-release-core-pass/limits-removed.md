# Limit text removed from the header comments

- Unit: plan 0040 (the pre-release core pass), part D1, by Claude (agent, Opus 5.5), implementer 2, on `feature/pre-release-core-pass--comments`.
- Branch point: `8058589` on `develop`. No file under `include/` differs between it and `77cd947`, the revision plan 0041 writes the limits page from, or `b400fac`, where plan 0040 read its line numbers (`git diff --stat 77cd947 8058589 -- include` is empty), so the line numbers here hold at all three.
- Scope: the compiler-version limit catalogues of plan 0040 section 3.5. Part D1 trimmed `plan.hpp`, `product.hpp`, `dual.hpp` and `blade_set.hpp` (rows 1, 2, 6, 8 and 9), and its sections give lines at `8058589`. Part D2 trimmed `mv.hpp`, `linear.hpp`, `transform.hpp` and `basis_change.hpp` (rows 3, 4, 5, 7, 10 and 11) after the code of parts A and B and the comments of part C had moved their lines, so its sections give lines at `12f8018`, the revision before D2 on `feature/pre-release-core-pass--comments`. The last section holds the one measured figure part C removed.
- The page's headings, checked against `docs/guide/limits.md` at `6c278df` on `feature/documentation-lane`: every heading cited below is one of its headings, and its subsection "Steps and operations of each plan" passes `cost.py comments` with both figures files.
- Where the text goes: `docs/guide/limits.md`, which the documentation lane writes (plan 0041 section 5.3 fixes its headings). Each region below names the heading its figures belong under.

## How to read this record

Each region gives the comment lines at the branch point, verbatim and whole, and the lines that replace them now. A region holds every line of the header's diff (`git diff 8058589 -- include/galoot/core/<header>` for D1's headers, `git diff 12f8018 -- ...` for D2's) and may hold a few unchanged lines between two hunks of one comment, so that each catalogue reads as one passage. A line that kept part of its text, such as the first line of a region, is shown whole in both blocks.

For each region, "Removed" lists what left the header and "Kept" what stayed because it holds on any compiler: what the cost grows with, that the compilers' limits on constant evaluation bound the sizes, and the requirements and guarantees. The one sentence each catalogue leaves behind names the flags that raise the limits (Clang's `-fconstexpr-steps`, GCC's `-fconstexpr-ops-limit`, and for plans GCC's `-fconstexpr-loop-limit`) and the page and heading.

The counts of the plans and maps, which the cost model checks, are under the heading "Steps and operations of each plan" below, the name of the subsection the documentation lane gives them on the limits page, so the same command reads both:

```sh
python3 -I docs/agent/snippets/plan_cost_model/cost.py comments \
  docs/agent/records/performance/pre-release-core-pass/limits-removed.md \
  --section "Steps and operations of each plan" \
  docs/agent/records/performance/plan-cost-model/out/figures-gcc.txt \
  docs/agent/records/performance/plan-cost-model/out/figures-clang.txt
```

With `--section "include/galoot/core/plan.hpp"` it reads every count removed from `plan.hpp`, the sizes and flag values of `NON_COST` and the compilers' default limits among them.

## include/galoot/core/plan.hpp

### product_plan: sizes, flags, and what the counts depend on

Lines 618 to 644 at the branch point, the first part of one region (618 to 667) of `product_plan`'s `///` comment; the region's replacement, lines 618 to 623 now, follows the second part.

```cpp
/// product of two large sets. The geometric product of the scalar with
/// full<10> in R(10,0,0), 1,024 terms on 1,024 blades, fails at GCC 16.2.1's
/// default limit, though full<6> times full<6>, 4,096 terms on 64 blades,
/// builds. The largest evaluation of the scalar's plan, the typing of its
/// terms, needs about 89 million operations, measured with
/// _GLIBCXX_ASSERTIONS and GCC's cache of constant calls on, and
/// -fconstexpr-ops-limit=134217728 builds the product at -O0 with
/// _GLIBCXX_ASSERTIONS and at -O2 without. On Clang 23 the scalar
/// times full<8> fails at the default steps; -fconstexpr-steps sets Clang's
/// limit. GCC may report that it ran out of operations as libstdc++'s
/// std::span bounds check, a __builtin_expect in <span>, being "not a
/// constant expression", although every index is in range. GCC also stops
/// at a loop of 262,144 iterations, and so at a plan of that many terms.
/// The counts below are exact for Clang 23.1, whose classic constant
/// evaluator allows 1,048,576 steps by default, and for GCC 16.2, which
/// allows 33,554,432 operations, each with libstdc++ 16 and
/// _GLIBCXX_ASSERTIONS defined, which libstdc++ 16 defines by itself in a
/// build without optimization unless _GLIBCXX_NO_ASSERTIONS is defined.
/// GCC's counts are taken with its cache of constant calls turned off; with
/// the cache on, as GCC builds by default, the count is never higher, so a
/// GCC count over the limit does not mean that the plan fails to build.
/// Without _GLIBCXX_ASSERTIONS, libstdc++ checks each subscript through a
/// function call, which both compilers count as more; GCC's sanitizers add
/// checks that GCC counts too, about 2% to 4% more; and another compiler
/// version, evaluator, or standard library counts differently. The counts are
/// those of the plan's own evaluations: a translation unit also evaluates
/// other things, such as the check of an algebra's metric, done once for each
```

Removed: from "The geometric product of the scalar with full<10>" on line 618: the scalar times full<10> in R(10,0,0) failing at GCC 16.2.1's default limit while full<6> times full<6> builds; the 89 million operations of its typing and the flag value that builds it; the scalar times full<8> failing on Clang 23; GCC's report of running out of operations as a `std::span` bounds check; GCC's loop limit of 262,144 iterations; the compilers, versions, default limits, and standard library the counts are exact for; GCC's call cache; what changes the counts without `_GLIBCXX_ASSERTIONS`, under GCC's sanitizers, or on another compiler; and that a unit also evaluates the check of an algebra's metric.

Kept: lines 602 to 617 and the start of 618, which say that the plan is a constant evaluation bounded by the compiler's limits and what its cost grows with.

Heading: "Compile-time limits: products and plans".

### Steps and operations of each plan

The two lists of counts, which `cost.py figures` predicts (its `FIGURES`), and the counts of the sparse sets.

#### product_plan, lines 645 to 667

The second part of the region of `product_plan`. Line 645 begins with the end of the previous sentence ("algebra.").

```cpp
/// algebra. For full<n> with full<n>, in steps on Clang and operations on
/// GCC:
/// - geometric product, R(n,0,0): n = 5, 655,239 and 3,611,426; n = 6,
///   5,110,503 (over Clang's limit) and 25,853,002; n = 7, 203,069,770 on
///   GCC (over its limit).
/// - geometric product, the form with 2 on the diagonal and 1 elsewhere:
///   n = 4, 681,823 and 3,645,825; n = 5, 8,319,054 and 44,407,055 (over
///   both limits).
/// - wedge, any metric: n = 5, 216,396 and 1,093,326; n = 6, 1,309,448
///   (over Clang's limit) and 6,558,946; n = 7, 39,732,810 on GCC (over
///   its limit).
/// - geometric antiproduct, R(n,0,0): n = 5, 883,839 and 4,768,546 (in
///   conformal(3), 947,839 steps on Clang); n = 6, 5,076,962 (over Clang's
///   limit) and 25,775,777.
/// - antiwedge, any metric: n = 5, 271,653 and 1,393,084; n = 6, 1,108,607
///   (over Clang's limit) and 5,695,232; n = 7, 23,415,628 on GCC.
/// Sets of a few blades cost what they use, except through an antiproduct,
/// which multiplies the complements, whose grades grow with n: {1, e1, e_n,
/// e1 e_n} with {e1, e2, e_n} in R(n-1,0,1) cost 3,136 steps and 18,252
/// operations at every n from 4 to 16 through the geometric product, and
/// 9,137 and 49,347 at n = 4 but 77,513 and 443,211 at n = 16 through the
/// geometric antiproduct. At n = 16 the check of the algebra's metric costs
/// more than the geometric product's plan of these sets.
```

Removed: the counts for full<n> with full<n> under each product and form, in steps on Clang 23.1 and operations on GCC 16.2; the counts of {1, e1, e_n, e1 e_n} with {e1, e2, e_n} in R(n-1,0,1) from n = 4 to 16 through the geometric product and antiproduct; and that at n = 16 the check of the algebra's metric costs more than the geometric product's plan of these sets.

Kept: "Sets of a few blades cost what they use, except through an antiproduct, which multiplies the complements, whose grades grow with n", the general statement that the counts illustrated.

The region's replacement, lines 618 to 623 now:

```cpp
/// product of two large sets. Sets of a few blades cost what they use,
/// except through an antiproduct, which multiplies the complements, whose
/// grades grow with n. Clang's -fconstexpr-steps and GCC's
/// -fconstexpr-ops-limit and -fconstexpr-loop-limit raise those limits, and
/// the sizes measured on each compiler, with how each reports a failure, are
/// in docs/guide/limits.md, under 'Compile-time limits: products and plans'.
```

Heading: "Compile-time limits: products and plans", in its subsection "Steps and operations of each plan".

#### unary_map, lines 974 to 981

From `unary_map`'s `///` comment; replaced by lines 930 to 932 now.

```cpp
/// practice. Counted as for product_plan, for the map of every blade,
/// full<n>, in steps on Clang 23.1 and operations on GCC 16.2:
/// - the reverse, the antireverse, and the grade involution: n = 7, 313,635
///   and 1,577,666; n = 8, 1,251,192 (over Clang's limit) and 6,270,700;
///   n = 9, 25,009,582 on GCC; n = 10, 99,912,902 on GCC (over its limit).
/// - the right and the left complement: n = 8, 813,090 and 5,717,129;
///   n = 9, 3,199,010 (over Clang's limit) and 22,509,577; n = 10,
///   89,321,225 on GCC (over its limit).
```

Now:

```cpp
/// practice. The flags that raise them are those of product_plan, and the
/// sizes measured on each compiler are in docs/guide/limits.md, under
/// 'Compile-time limits: products and plans'.
```

Removed: from "Counted as for product_plan" on line 974: the counts of the maps of full<n> for the reverse, the antireverse and the grade involution (n = 7 to 10) and for the two complements (n = 8 to 10), in steps on Clang 23.1 and operations on GCC 16.2.

Kept: that typing the map costs at most a fixed amount times the square of the size of S, so the compilers' limits bound S.

Heading: "Compile-time limits: products and plans", in its subsection "Steps and operations of each plan".

## include/galoot/core/product.hpp

### The header comment, lines 47 to 52

Replaced by lines 47 to 50 now.

```cpp
// or a motor are far below them. Two full multivectors of five generators
// are near them on Clang 23 with libstdc++: the geometric antiproduct of two
// full<5> multivectors of conformal(3) builds only while libstdc++'s
// assertions are on, and in a dense form of five generators no geometric
// product or antiproduct of two full<5> multivectors builds; the comment of
// geometric_antiproduct gives the details and the workaround.
```

Now:

```cpp
// or a motor are far below them. Clang's -fconstexpr-steps and GCC's
// -fconstexpr-ops-limit raise those limits, and the sizes measured on each
// compiler are in docs/guide/limits.md, under
// 'Compile-time limits: products and plans'.
```

Removed: from "Two full multivectors of five generators" on line 47: that two full<5> multivectors are near the limits on Clang 23 with libstdc++; that the geometric antiproduct of two full<5> multivectors of conformal(3) builds only while libstdc++'s assertions are on; that in a dense form of five generators no geometric product or antiproduct of two full<5> multivectors builds; and the pointer to `geometric_antiproduct`'s comment for the details.

Kept: lines 44 to 46 and the start of 47: each pair of layouts builds one plan, bounded by the compiler's limits, and typed layouts are far below them.

Heading: "Compile-time limits: products and plans".

### geometric_antiproduct, lines 339 to 360

The end of the "Cost" paragraph and the whole "Limit" paragraph of `geometric_antiproduct`'s `///` comment; line 345, the blank `///` between them, is unchanged. Replaced by lines 337 to 343 now.

```cpp
/// so its compile cost grows with n even when x and y use few generators:
/// four products of rigid(3) types (point and point, line and plane, motor
/// and point, point and motor) embedded in 16 generators cost GCC 16 about
/// twice, and Clang 23 about four and a half times, what they cost in 4
/// generators, while the same four geometric products cost the same at both
/// sizes.
///
/// Limit: on Clang 23 with libstdc++, the plan of two full<5> multivectors
/// of conformal(3) takes about 950,000 of Clang's default 1,048,576 steps of
/// constant evaluation while libstdc++'s assertions are on
/// (_GLIBCXX_ASSERTIONS, which libstdc++ defines by itself only in a build
/// without optimization), and more than the limit without them, since each
/// array subscript then costs a function call. So an optimized build, or one
/// with _GLIBCXX_NO_ASSERTIONS, fails with "constexpr evaluation hit maximum
/// step limit" inside the plan's generation, followed by errors that do not
/// name the product. Raising the limit builds it: -fconstexpr-steps=1200000
/// is enough, measured. So do smaller layouts, such as full<5> with the
/// vectors, Clang with libc++, and GCC 16 at its default limits. In a dense
/// form of five generators, where every pair of generators is coupled, Clang
/// 23 cannot build the plan of two full<5> multivectors in any
/// configuration at its default limits, for the geometric product as well;
/// GCC 16 can.
```

Now:

```cpp
/// so its compile cost grows with n even when x and y use few generators.
///
/// Limit: the plan is a constant evaluation, so the compiler's limits on
/// constant evaluation bound the layouts. Clang's -fconstexpr-steps and
/// GCC's -fconstexpr-ops-limit raise those limits, and the sizes measured on
/// each compiler, with how a failure is reported, are in
/// docs/guide/limits.md, under 'Compile-time limits: products and plans'.
```

Removed: the growth figures (four products of rigid(3) types embedded in 16 generators cost GCC 16 about twice, and Clang 23 about four and a half times, what they cost in 4 generators, while the four geometric products cost the same); and the whole "Limit" paragraph: the share of Clang's default steps the conformal(3) full<5> plan takes with and without libstdc++'s assertions, when libstdc++ defines them, Clang's failure message and the errors after it, the flag value that builds it, the configurations that build it (smaller layouts, libc++, GCC 16), and the dense form of five generators on Clang 23 and GCC 16.

Kept: that the plan is built through the complements in all n generators, so its compile cost grows with n even when x and y use few generators. The new "Limit" paragraph says that the plan is a constant evaluation bounded by the compiler's limits and then gives the one pointer sentence.

Heading: "Compile-time limits: products and plans".

### operator%, lines 418 and 419

Replaced by lines 401 and 402 now.

```cpp
/// rejected with a message that names x % y. The limit on Clang that
/// geometric_antiproduct states applies here too.
```

Now:

```cpp
/// rejected with a message that names x % y. The limits that
/// geometric_antiproduct states apply here too.
```

Removed: "on Clang", since the limits `geometric_antiproduct` now states are those of every compiler.

Heading: none; it refers to `geometric_antiproduct`'s comment.

## include/galoot/core/dual.hpp

### The header comment, lines 78 to 95

Replaced by lines 78 to 84 now.

```cpp
// point, a line, a plane, or a motor are far below them. Measured on GCC 16
// and Clang 23 with libstdc++, at the default limits, the same with and
// without libstdc++'s assertions, for full<n> multivectors of R(n,0,0): the
// bulk, the weight, and the four duals build up to full<7> on Clang, where
// full<8> is past the reverse's limit, and up to full<9>, the largest tried,
// on GCC; the dot, the antidot, and the projection build up to full<5> on
// Clang and full<6> on GCC, the limits of the geometric product's and the
// wedge's plans, and the antiprojection the same. Every operation builds for
// full<5> of conformal(3). In a dense form of five generators, where every pair
// of generators is coupled, the dot and the antidot of two full<5> multivectors
// do not build on Clang; GCC builds them. A failure there is reported inside
// the plan's generation, on Clang with the note "constexpr evaluation hit
// maximum step limit" and on GCC as an operation count over the limit (without
// optimization, sometimes as a libstdc++ assertion that "is not a constant
// expression"), followed by errors that do not name the operation. The weight,
// the weight duals, the antidot, and the projection pass through the
// complements, whose grades grow with n, so their compile cost grows with n
// even when x uses few generators.
```

Now:

```cpp
// point, a line, a plane, or a motor are far below them. The weight, the
// weight duals, the antidot, and the projection pass through the
// complements, whose grades grow with n, so their compile cost grows with n
// even when x uses few generators. Clang's -fconstexpr-steps and GCC's
// -fconstexpr-ops-limit raise those limits, and the sizes measured on each
// compiler, with how a failure is reported, are in docs/guide/limits.md,
// under 'Compile-time limits: duals, dot products and projections'.
```

Removed: from "Measured on GCC 16" on line 78 to "do not name the operation." on line 92: the largest full<n> of R(n,0,0) for which the bulk, the weight, the four duals, the dot, the antidot, the projection and the antiprojection build on GCC 16 and Clang 23 with libstdc++, the same with and without its assertions; that every operation builds for full<5> of conformal(3); that in a dense form of five generators the dot and the antidot of two full<5> multivectors do not build on Clang but do on GCC; and how each compiler reports the failure. Plan 0040's section 3.5 gives this catalogue as lines 78 to 91; its last sentence ends on line 92.

Kept: the start of line 78 (typed layouts are far below the limits) and lines 92 to 95 from "The weight," (which operations pass through the complements, so their cost grows with n).

Heading: "Compile-time limits: duals, dot products and projections".

## include/galoot/core/blade_set.hpp

### full, lines 576 and 577

Replaced by lines 576 to 579 now.

```cpp
/// practice well below mask_bits: at their default limits, Clang 23 builds
/// full<13> but not full<14>, and GCC 16 builds full<15> but not full<16>.
```

Now:

```cpp
/// practice well below mask_bits. Clang's -fconstexpr-steps and GCC's
/// -fconstexpr-ops-limit raise those limits, and the sizes measured on each
/// compiler are in docs/guide/limits.md, under
/// 'Compile-time limits: blade sets'.
```

Removed: from the colon on line 576: Clang 23 builds full<13> but not full<14>, and GCC 16 builds full<15> but not full<16>, at their default limits.

Kept: the set holds 2^N blades, so the compilers' limits bound N in practice well below `mask_bits`.

Heading: "Compile-time limits: blade sets".

### of_grade, lines 586 to 588

Replaced by lines 588 to 590 now.

```cpp
/// The limits on constant evaluation bound the size as for full: at its
/// default limits, Clang 23 builds of_grade<32, 3>, with 4,960 blades, but
/// not of_grade<16, 8> or of_grade<32, 4>, with 12,870 and 35,960.
```

Now:

```cpp
/// The limits on constant evaluation bound the size as for full, and the
/// sizes measured on each compiler are in docs/guide/limits.md, under
/// 'Compile-time limits: blade sets'.
```

Removed: from the colon on line 586: Clang 23 builds of_grade<32, 3>, with 4,960 blades, but not of_grade<16, 8> or of_grade<32, 4>, with 12,870 and 35,960, at its default limits.

Kept: the limits bound the size as for `full`.

Heading: "Compile-time limits: blade sets".

### merge, lines 601 to 605

Replaced by lines 603 to 606 now.

```cpp
/// grows with the square of the sizes and depends on their order. Measured at
/// the default limits on constant evaluation: Clang 23 merges full<7> with
/// itself and two disjoint sets of 64 blades, but not two disjoint sets of
/// 128; GCC 16 merges full<9> with itself and two disjoint sets of 256, but
/// not two disjoint sets of 512, nor full<9> listed in reverse with itself.
```

Now:

```cpp
/// grows with the square of the sizes and depends on their order. The
/// compiler's limits on constant evaluation bound the sizes it merges, and
/// the sizes measured on each compiler are in docs/guide/limits.md, under
/// 'Compile-time limits: blade sets'.
```

Removed: from "Measured at" on line 601: Clang 23 merges full<7> with itself and two disjoint sets of 64 blades but not of 128; GCC 16 merges full<9> with itself and two disjoint sets of 256 but not of 512, nor full<9> listed in reverse with itself; at the default limits.

Kept: each blade is inserted into a list in canonical order, so the cost grows with the square of the sizes and depends on their order. The new sentence adds that the compilers' limits bound the sizes merged, which the removed one said only through "Measured at the default limits".

Heading: "Compile-time limits: blade sets".

## include/galoot/core/mv.hpp

Lines at `12f8018`. The figures belong under "Compile-time limits: products and plans".

### mv, lines 489 to 494

From `mv`'s `///` comment; replaced by lines 489 to 493 now.

```cpp
/// the size of a layout. Measured at the default limits, alike at -O0 and -O2
/// and with or without _GLIBCXX_ASSERTIONS: mv<R(n,0,0), full<n>, double>
/// builds for n up to 8 (256 blades) on Clang 23 and up to 10 (1,024 blades) on
/// GCC 16. A product with such a multivector can fail where its layout
/// builds: in R(n,0,0), the geometric product of the scalar with full<10>
/// fails on GCC 16 at its default limits, and with full<8> on Clang 23.
```

Now:

```cpp
/// the size of a layout, and a product can fail on a layout that builds.
/// Clang's -fconstexpr-steps and GCC's -fconstexpr-ops-limit raise those
/// limits, and the sizes measured on each compiler, for a layout and for a
/// product with it, are in docs/guide/limits.md, under
/// 'Compile-time limits: products and plans'.
```

Removed: from "Measured at the default limits" on line 489: the largest full<n> of R(n,0,0) that `mv` builds, at -O0 and -O2 and with or without `_GLIBCXX_ASSERTIONS`, on Clang 23 (n = 8, 256 blades) and GCC 16 (n = 10, 1,024 blades); and the geometric product of the scalar with full<10> failing on GCC 16 and with full<8> on Clang 23.

Kept: the repeated-mask check compares every pair of blades, so the compilers' limits bound the size of a layout. "A product with such a multivector can fail where its layout builds", which led the removed example, stays as "a product can fail on a layout that builds".

Heading: "Compile-time limits: products and plans".

### The binary apply, lines 883 to 905

From the `///` comment of `apply` with a target layout; replaced by lines 882 to 888 now. Line 883 is the end of the sentence part C rewrapped (`12f8018`).

```cpp
/// the terms, and
/// Clang 23 stops a fold at 2,048 terms; no plan that Clang 23 builds at its
/// default limits has more than 1,024 terms. Measured at the default limits, at
/// -O0 and -O2, with and without _GLIBCXX_ASSERTIONS: apply builds wherever its
/// plan builds. A plan's cost grows with its terms times the blades of its
/// result, not with its terms alone, so the size of a plan that builds
/// depends on its sets. In R(n,0,0), the plan of the geometric product of the
/// scalar with full<10> has 1,024 terms on 1,024 blades and fails at GCC
/// 16.2.1's default limit, and that of the scalar with full<8> fails at Clang
/// 23's, although mv on full<10> builds on GCC and mv on full<8> on Clang.
/// -fconstexpr-ops-limit=134217728 builds the product with full<10> on GCC,
/// and GCC may report the failure as libstdc++'s std::span bounds check
/// being "not a constant expression" rather than as its operation limit.
/// For a full layout times itself, the plans build up to full<6> times
/// full<6> (4,096 terms, on 64 blades) on GCC 16, and up to full<5> times
/// full<5> on Clang 23, with two exceptions there: dense five-generator
/// forms, whose products do not build on Clang at all, and the geometric
/// antiproduct of full<5> in conformal(3), which builds
/// only with _GLIBCXX_ASSERTIONS, which libstdc++ defines by itself only
/// without optimization, because without it each subscript is a function call
/// that Clang counts, and the plan then needs more than Clang's default steps
/// (-fconstexpr-steps=1200000 is enough). product_plan's comment gives the
/// counts, taken with _GLIBCXX_ASSERTIONS.
```

Now:

```cpp
/// the terms, so a compiler's limit on the length of a fold bounds it as
/// well as the limits that bound its plan. A plan's cost grows with its
/// terms times the blades of its result, not with its terms alone, so the
/// size of a plan that builds depends on its sets. The sizes measured on each
/// compiler, the fold's limit, how a failure is reported, and the flags that
/// raise the limits are in docs/guide/limits.md, under
/// 'Compile-time limits: products and plans'.
```

Removed: Clang 23's fold limit of 2,048 terms and that no plan Clang 23 builds at its default limits has more than 1,024; that `apply` builds wherever its plan builds, measured at the default limits, -O0 and -O2, with and without `_GLIBCXX_ASSERTIONS`; the scalar times full<10> in R(n,0,0), 1,024 terms on 1,024 blades, failing at GCC 16.2.1's default limit and times full<8> at Clang 23's, while `mv` on those layouts builds; the flag value that builds it on GCC; GCC's report of the failure as a `std::span` bounds check; the full layouts times themselves up to full<6> (4,096 terms on 64 blades) on GCC 16 and full<5> on Clang 23, with the dense five-generator forms and the conformal(3) antiproduct as exceptions, why, and the flag value that builds the latter; and the pointer to `product_plan`'s comment for the counts, which are no longer there.

Kept: that the code is one fold expression over the terms, now with what follows from it on any compiler (a compiler's limit on the length of a fold bounds it); and that a plan's cost grows with its terms times the blades of its result, so the size of a plan that builds depends on its sets.

Heading: "Compile-time limits: products and plans".

### The unary apply, lines 953 to 955

From the `///` comment of `apply` for a map with a target layout; replaced by lines 936 and 937 now.

```cpp
/// compile time with its message. Measured as for a product: apply builds
/// wherever the map builds, up to full<8> for the complements and full<7>
/// for the other maps on Clang 23, and full<9> on GCC 16.
```

Now:

```cpp
/// compile time with its message. The sizes measured on each compiler are in
/// docs/guide/limits.md, under 'Compile-time limits: products and plans'.
```

Removed: from "Measured as for a product" on line 953: `apply` builds wherever the map builds, up to full<8> for the complements and full<7> for the other maps on Clang 23, and full<9> on GCC 16.

Heading: "Compile-time limits: products and plans".

## include/galoot/core/linear.hpp

Lines at `12f8018`. The page states the sums under "Compile-time limits: blade sets", since a sum's limit is that of the merge of its layouts (plan 0040 section 1 item 8, K2).

### The header comment, lines 61 to 64

Replaced by lines 61 to 64 now.

```cpp
// of a multivector: on Clang 23, full<8> plus the bivectors of eight
// generators does not build. sum_layout's comment gives the measured sizes
// and the workaround. A sum of one type, a scaling, and a negation cost no
// constant evaluation beyond the multivector's own.
```

Now:

```cpp
// of a multivector. sum_layout's comment gives the workaround, and the sizes
// measured on each compiler are in docs/guide/limits.md, under
// 'Compile-time limits: blade sets'. A sum of one type, a scaling, and a
// negation cost no constant evaluation beyond the multivector's own.
```

Removed: from the colon on line 61: on Clang 23, full<8> plus the bivectors of eight generators does not build; and "the measured sizes" from the pointer to `sum_layout`'s comment, which keeps only the workaround.

Heading: "Compile-time limits: blade sets".

### sum_layout, lines 105 to 111

Replaced by lines 105 to 108 now.

```cpp
/// a sum of two layouts below the size of a multivector. Measured at the
/// default limits, at -O0 and -O2, with and without _GLIBCXX_ASSERTIONS, in
/// R(n,0,0): full<n> plus the bivectors of n generators, and full<n-1> plus
/// the blades that hold e_n, build for n up to 7 on Clang 23 and up to 9 on
/// GCC 16, while mv builds full<8> and full<10>. A larger sum fails inside
/// merge (Clang: "constexpr variable 'u' must be initialized by a constant
/// expression"; GCC: "constexpr evaluation operation count exceeds limit").
```

Now:

```cpp
/// a sum of two layouts below the size of a multivector. Clang's
/// -fconstexpr-steps and GCC's -fconstexpr-ops-limit raise those limits, and
/// the sizes measured on each compiler, with how a failure is reported, are
/// in docs/guide/limits.md, under 'Compile-time limits: blade sets'.
```

Removed: from "Measured at the default limits" on line 105 to the end of line 111: full<n> plus the bivectors, and full<n-1> plus the blades that hold e_n, build in R(n,0,0) for n up to 7 on Clang 23 and 9 on GCC 16, at -O0 and -O2, with and without `_GLIBCXX_ASSERTIONS`, while `mv` builds full<8> and full<10>; and each compiler's message when a larger sum fails inside merge.

Kept: the blades are merged by insertion, so the cost grows with the square of the two sizes and the compilers' limits bound a sum below the size of a multivector; and the workaround (convert the smaller operand first).

Heading: "Compile-time limits: blade sets".

## include/galoot/core/transform.hpp

### The header comment, lines 70 to 82

Lines at `12f8018`; replaced by lines 70 to 73 now.

```cpp
// line, a plane, a motor, or a flector are far below them. Measured on GCC 16
// and Clang 23 with libstdc++, at the default limits, the same with and
// without libstdc++'s assertions, for x a full<n> multivector of R(n,0,0)
// and V on every blade of even antigrade, the largest layout of one parity:
// transform builds up to n = 5 on Clang and n = 6 on GCC, and compose of two
// such versors up to n = 5 on Clang and n = 7, the largest tried, on GCC.
// Both build for full<5> of conformal(3). In a dense form of five generators,
// where every pair of generators is coupled, neither builds on Clang, and
// both build on GCC. A failure is reported inside a plan's generation, on
// Clang as "constexpr variable 'count' must be initialized by a constant
// expression" and on GCC as an operation count over the limit or a
// libstdc++ assertion that is not a constant expression, followed by errors
// that do not name the operation.
```

Now:

```cpp
// line, a plane, a motor, or a flector are far below them. Clang's
// -fconstexpr-steps and GCC's -fconstexpr-ops-limit raise those limits, and
// the sizes measured on each compiler, with how a failure is reported, are
// in docs/guide/limits.md, under 'Compile-time limits: transform and compose'.
```

Removed: from "Measured on GCC 16" on line 70: the largest full<n> of R(n,0,0), with V on every blade of even antigrade, for which `transform` and `compose` build on GCC 16 and Clang 23 with libstdc++, the same with and without its assertions; that both build for full<5> of conformal(3); that in a dense form of five generators neither builds on Clang and both build on GCC; and how each compiler reports a failure.

Kept: compose is one antiproduct and transform two, each a plan built once per pair of layouts, so they have the antiproduct's limits; typed layouts are far below them.

Heading: "Compile-time limits: transform and compose".

## include/galoot/core/basis_change.hpp

### The header comment, lines 111 to 124

Lines at `12f8018`; replaced by lines 111 to 115 now.

```cpp
// generator they hold. Measured on GCC 16.2 and Clang 23.1 with libstdc++
// 16, at the default limits, without optimization and with
// _GLIBCXX_ASSERTIONS, which libstdc++ 16 defines by itself in such a build,
// both ways for each:
// - conformal_change<D> maps full<D+2> up to D = 5 on Clang and D = 6 on
//   GCC.
// - a change whose every entry is nonzero maps full<n> up to n = 5 on Clang
//   and n = 7 on GCC, the bivectors up to n = 8 on Clang (n = 7 with
//   _GLIBCXX_NO_ASSERTIONS) and n = 12 on GCC, and the vectors at every n
//   up to 16, the largest that the factories build by default.
// A failure there is reported inside the generation, on Clang as a constexpr
// variable that "must be initialized by a constant expression" with the note
// "constexpr evaluation hit maximum step limit", and on GCC as an operation
// count over the limit, followed by errors that do not name the change.
```

Now:

```cpp
// generator they hold. The compilers' limits on constant evaluation bound
// these costs; Clang's -fconstexpr-steps and GCC's -fconstexpr-ops-limit
// raise those limits, and the sizes measured on each compiler, with how a
// failure is reported, are in docs/guide/limits.md, under
// 'Compile-time limits: change of basis'.
```

Removed: from "Measured on GCC 16.2" on line 111: the compilers, standard library and build the sizes were measured with; the largest full<D+2> that `conformal_change<D>` maps on Clang and GCC; the largest full<n>, bivectors (with and without `_GLIBCXX_NO_ASSERTIONS`) and vectors that a change with every entry nonzero maps on each; and how each compiler reports a failure.

Kept: everything before line 111 (the checks' cost in checked multiplications, what the image of a blade costs, conformal_change's two entries per row). The new sentence adds that the compilers' limits on constant evaluation bound these costs, which the removed text said only through "at the default limits".

Heading: "Compile-time limits: change of basis".

## Part C: the measured figure in the first TODO

`coefficient_storage`'s one-value constructor in `mv.hpp`, lines 392 to 396 at `c6efd1f`, the feature branch's tip that `--comments` merged; replaced by lines 392 to 397 now. Not a limit catalogue, but it held one figure measured on one compiler, "114 lines on GCC" (the length of GCC 16's error for `Scalar<rational>{1}`), which the new comment does not keep: it says only that the compiler's own error is a long one.

```cpp
  // TODO: a one-blade value built from a source that converts only
  // explicitly gets the compiler's own error, 114 lines on GCC. A catch-all
  // with a short message would fix that, but it changes three
  // std::is_constructible answers that the tests pin, so it waits until the
  // long message becomes a problem for a user.
```

Now:

```cpp
  // A one-blade value built from a source that converts to T only
  // explicitly, such as an int for a rational whose constructor from an
  // integer is explicit, matches no constructor and gets the compiler's own
  // error, a long one. No catch-all gives it a short message, as the other
  // misuses of mv have, because a catch-all would make std::is_constructible
  // true for such a source, which it now reports as false.
```

Heading: none. The page need not carry this figure; the release record's known-friction item for the one-blade message may.
