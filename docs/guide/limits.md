# The limits

Galoot works out every product and operation at compile time, as constant
evaluation, so how large an algebra and a layout can be depends on the
compiler's limits on constant evaluation as well as on the library's own
bounds. This page gives both: the bounds the library sets and checks, and
the sizes measured with the compilers Galoot is tested with, with the flags
that raise their limits and how each reports a failure. The types of the
rga3 preset, and layouts of a similar size, are far below every limit here.

Unless a section says otherwise, a figure was measured with GCC 16.2.1 and
Clang 23.1.4, each with libstdc++ 16, at the compilers' default limits, and
for full<n>, the layout of every blade of n generators, in R(n, 0, 0). A
compiler of another version, another constant evaluator, or another
standard library counts differently.

## Number of generators

An algebra has from 1 to 16 generators, `max_generators`, unless its
factory is given a larger bound `MaxN` as its last template argument:
`R<P, Q, Z, MaxN>()`, `rigid<D, MaxN>()`, `conformal<D, MaxN>()`, and
`from_gram<N, MaxN>({...})`. `MaxN` may be at most `mask_bits`, 32, the
number of generators a blade mask holds. A count of 0, a count above `MaxN`,
and a `MaxN` above 32 are each rejected at compile time with a message that
names it. An algebra above 16 generators is accepted by every later part of
the library, but nothing above 16 is tested.

The tests run the identities of the core on sparse layouts of 8, 12, and 16
generators, in R(8, 0, 0), R(6, 6, 0), R(15, 0, 1), and R(8, 8, 0). A layout
of many blades reaches the compilers' limits well before 16 generators, as
the sections below measure: a sparse layout costs what it uses, and a dense
one grows as 2^n.

## Blade names

Blade names have one decimal digit per index, so they exist only for
algebras of up to 9 generators, `max_compact_generators`: in a larger
algebra, e123 could mean e1 e23 or e12 e3. Above 9 generators, `blades<Tag,
"...">`, `get<"...">`, and printing are each rejected with a message, and
`has_name` answers false. There, a layout is written as a `blade_set` of masks, in which bit
i − 1 stands for e_i, or built by `full`, `of_grade`, and `merge`, and a
coefficient is read by its position in `coefficients`. A product's result is
in canonical order, by grade and then lexicographically by index list.

```cpp
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct r12 {
  static constexpr auto value = core::R<12, 0, 0>();
};

// The generators e1, e2, and the twelfth, g12, as masks with sign +1: no
// name such as e12 can stand for the twelfth generator, since e12 is e1 e2.
constexpr core::oriented_blade e1{.mask = 1U << 0U, .sign = 1};
constexpr core::oriented_blade e2{.mask = 1U << 1U, .sign = 1};
constexpr core::oriented_blade g12{.mask = 1U << 11U, .sign = 1};
constexpr core::blade_set<3> vectors{.blades = {{e1, e2, g12}}};
using Vector = core::mv<r12, vectors, int>;

constexpr Vector a{1, 2, 0};
constexpr Vector b{0, 0, 3};

// The wedge of two such vectors is on the three bivectors they can reach,
// in canonical order: e1 e2, then e1 g12, then e2 g12. Here
// a ^ b = 3 (e1 ^ g12) + 6 (e2 ^ g12), with 0 on e1 e2.
// The core names that layout wedge_layout.
constexpr auto ab = a ^ b;
constexpr auto bivectors = core::wedge_layout<r12, vectors, vectors>;
static_assert(bivectors.size() == 3 && ab.coefficients.size() == 3);
static_assert(bivectors.blades[0].mask == (e1.mask | e2.mask));
static_assert(bivectors.blades[1].mask == (e1.mask | g12.mask));
static_assert(ab.coefficients[0] == 0 && ab.coefficients[1] == 3 &&
              ab.coefficients[2] == 6);

// The vectors of twelve generators, built rather than listed.
static_assert(core::of_grade<12, 1>.size() == 12);
static_assert(core::max_compact_generators == 9);

} // namespace

int main() {}
```

```cpp rejected
// rejected: reach the coefficient by its position
#include <galoot/core.hpp>

namespace core = galoot::core;

namespace {

struct r12 {
  static constexpr auto value = core::R<12, 0, 0>();
};
using Vector = core::mv<r12, core::of_grade<12, 1>, int>;

} // namespace

int first(const Vector& v) { return core::get<"e1">(v); }

int main() { return first(Vector{}); }
```

## Coefficient range

A Gram matrix holds `entry_type`, `int`, entries within `max_form_entry` in
magnitude: the largest k for which a 2 × 2 minor of entries of magnitude k,
at most 2k², fits in `int`. With a 32-bit `int` that is 32,767. `from_gram`
rejects a larger entry at compile time with a message. Larger minors can
still overflow, and that fails the constant evaluation that forms them, so it
cannot pass silently.

The exact coefficients of the generated products are `coefficient_type`,
`std::int64_t`. Each one is checked as it is formed: one that would leave the
range fails with the message "a product coefficient does not fit
coefficient_type", in a constant evaluation as a compile error and at run
time, in checking mode, as an abort. With `GALOOT_CHECKING` 0, such an
overflow at run time is undefined behavior. A scalar type that cannot hold a
product's coefficients, as its `numeric_limits` tell, is rejected at compile
time with a message that names the product. A change of basis checks its
inner products and its inverse in the same way, each with a message.

```cpp
#include <concepts>
#include <cstdint>
#include <limits>

#include <galoot/core.hpp>

namespace core = galoot::core;

// 2 k^2 <= 2^31 - 1 holds for k = 32767, since 2 * 32767^2 = 2147352578,
// and fails for k = 32768.
static_assert(std::numeric_limits<core::entry_type>::max() == 2147483647);
static_assert(core::max_form_entry == 32767);
static_assert(std::same_as<core::coefficient_type, std::int64_t>);

int main() {}
```

## Compile-time limits: products and plans

Each pair of operand layouts builds one plan, the list of terms a product
computes, by constant evaluation, so the compilers' limits on constant
evaluation bound the layouts a product accepts. Generating the terms costs a
fixed amount for each pair of blades, plus the work of the pair's product,
which grows with the grades of the two blades (for an antiproduct, with the
grades of their complements) and with the number of terms the metric gives
the pair. Placing the T terms on the R blades of the result costs at most a
fixed amount times T times R, and is the largest part for large sets. For a
small set times a large one, both T and R grow with the large set, so the
placement grows as its square and can reach the limits with fewer terms than
a product of two large sets.

The multivector type itself builds for full<n> up to n = 8 (256 blades) on
Clang and up to n = 10 (1,024 blades) on GCC, alike at -O0 and -O2 and with
or without `_GLIBCXX_ASSERTIONS`; its check that no mask repeats compares
every pair of blades. A product with such a multivector can fail where the
layout builds:

- The geometric product of the scalar with full<10> in R(10,0,0), 1,024
  terms on 1,024 blades, fails at GCC's default limit, though full<6> times full<6>, 4,096
  terms on 64 blades, builds. The largest evaluation of the scalar's plan,
  the typing of its terms, needs about 89 million operations, measured with
  `_GLIBCXX_ASSERTIONS` and GCC's cache of constant calls on, and
  `-fconstexpr-ops-limit=134217728` builds the product at -O0 with
  `_GLIBCXX_ASSERTIONS` and at -O2 without.
- On Clang, the scalar times full<8> fails at the default steps.
- For a full layout times itself, the plans build up to full<6> times
  full<6> on GCC and up to full<5> times full<5> on Clang, with two
  exceptions on Clang: dense forms of five generators, where every pair of
  generators is coupled, whose products of two full<5> multivectors do not
  build in any configuration at the default limits, for the geometric
  product as for the antiproduct; and the geometric antiproduct of two
  full<5> multivectors of conformal(3), which takes about 950,000 of Clang's
  1,048,576 default steps while libstdc++'s assertions are on and more than
  the limit without them, since each array subscript then costs a function
  call. libstdc++ 16 defines `_GLIBCXX_ASSERTIONS` by itself only in a build
  without optimization, unless `_GLIBCXX_NO_ASSERTIONS` is defined, so an
  optimized build of that antiproduct fails. `-fconstexpr-steps=1200000`
  builds it, and so do smaller layouts, such as full<5> with the vectors,
  and GCC at its default limits. It was also seen to build on Clang with
  libc++, a standard library Galoot is not tested with, so that is not a
  claim. GCC builds the dense five-generator products too.
- The code that applies a plan is one fold expression over its terms, and
  Clang stops a fold at 2,048 terms; no plan that Clang builds at its default
  limits has more than 1,024 terms. The code builds wherever its plan builds,
  at -O0 and -O2, with and without `_GLIBCXX_ASSERTIONS`.
- GCC also stops at a loop of 262,144 iterations, and so at a plan of that
  many terms.
- The antiproducts are built through the complements in all n generators, so
  their compile cost grows with n even when the operands use few generators:
  four products of rigid(3) types (point and point, line and plane, motor and
  point, point and motor) embedded in 16 generators cost GCC about twice, and
  Clang about four and a half times, what they cost in 4 generators, while the
  same four geometric products cost the same at both sizes.

A failure is reported inside the plan's generation, followed by errors that
do not name the product: on Clang with the note "constexpr evaluation hit
maximum step limit", and on GCC as an operation count over the limit. GCC may
instead report it as libstdc++'s `std::span` bounds check, a
`__builtin_expect` in `<span>`, being "not a constant expression", although
every index is in range.

The involutions and complements are maps of one operand, with one term for
each blade; typing a map costs at most a fixed amount times the square of the
size of its layout. The maps build up to full<8> for the complements and
full<7> for the reverse, the antireverse, and the grade involution on Clang,
and up to full<9> on GCC.

### Steps and operations of each plan

These are the counts of each plan's own evaluations, exact for Clang 23.1,
whose classic constant evaluator allows 1,048,576 steps by default, and for
GCC 16.2, which allows 33,554,432 operations, each with libstdc++ 16 and
`_GLIBCXX_ASSERTIONS` defined. GCC's counts are taken with its cache of
constant calls turned off; with the cache on, as GCC builds by default, the
count is never higher, so a GCC count over the limit does not mean that the
plan fails to build. Without `_GLIBCXX_ASSERTIONS`, libstdc++ checks each
subscript through a function call, which both compilers count as more; GCC's
sanitizers add checks that GCC counts too, about 2% to 4% more. A
translation unit also evaluates other things, such as the check of an
algebra's metric, done once for each algebra.

For full<n> with full<n>, in steps on Clang and operations on GCC:

- geometric product, R(n,0,0): n = 5, 655,239 and 3,611,426; n = 6,
  5,110,503 (over Clang's limit) and 25,853,002; n = 7, 203,069,770 on GCC
  (over its limit).
- geometric product, the form with 2 on the diagonal and 1 elsewhere:
  n = 4, 681,823 and 3,645,825; n = 5, 8,319,054 and 44,407,055 (over both
  limits).
- wedge, any metric: n = 5, 216,396 and 1,093,326; n = 6, 1,309,448 (over
  Clang's limit) and 6,558,946; n = 7, 39,732,810 on GCC (over its limit).
- geometric antiproduct, R(n,0,0): n = 5, 883,839 and 4,768,546 (in
  conformal(3), 947,839 steps on Clang); n = 6, 5,076,962 (over Clang's
  limit) and 25,775,777.
- antiwedge, any metric: n = 5, 271,653 and 1,393,084; n = 6, 1,108,607
  (over Clang's limit) and 5,695,232; n = 7, 23,415,628 on GCC.

Sets of a few blades cost what they use, except through an antiproduct,
which multiplies the complements, whose grades grow with n: {1, e1, e_n,
e1 e_n} with {e1, e2, e_n} in R(n-1,0,1) cost 3,136 steps and 18,252
operations at every n from 4 to 16 through the geometric product, and 9,137
and 49,347 at n = 4 but 77,513 and 443,211 at n = 16 through the geometric
antiproduct. At n = 16 the check of the algebra's metric costs more than the
geometric product's plan of these sets.

For the map of every blade, full<n>, counted in the same way:

- the reverse, the antireverse, and the grade involution: n = 7, 313,635
  and 1,577,666; n = 8, 1,251,192 (over Clang's limit) and 6,270,700;
  n = 9, 25,009,582 on GCC; n = 10, 99,912,902 on GCC (over its limit).
- the right and the left complement: n = 8, 813,090 and 5,717,129;
  n = 9, 3,199,010 (over Clang's limit) and 22,509,577; n = 10,
  89,321,225 on GCC (over its limit).

## Compile-time limits: blade sets

`full<N>` holds 2^N blades, and the compilers' limits bound N well below
`mask_bits`: at their default limits, Clang builds full<13> but not
full<14>, and GCC builds full<15> but not full<16>. `of_grade` is bounded the
same way: Clang builds of_grade<32, 3>, with 4,960 blades, but not
of_grade<16, 8> or of_grade<32, 4>, with 12,870 and 35,960.

`merge` inserts each blade into a list kept in canonical order, so its cost
grows with the square of the sizes and depends on their order. Clang merges
full<7> with itself and two disjoint sets of 64 blades, but not two disjoint
sets of 128; GCC merges full<9> with itself and two disjoint sets of 256, but
not two disjoint sets of 512, nor full<9> listed in reverse with itself.

A sum or a difference of two different layouts merges them, so it is
bounded below the size of a multivector. At -O0 and -O2, with and without
`_GLIBCXX_ASSERTIONS`, full<n> plus the bivectors of n generators, and
full<n−1> plus the blades that hold e_n, build for n up to 7 on Clang and up
to 9 on GCC, while the multivector type builds full<8> and full<10>. A larger
sum fails inside the merge, on Clang as "constexpr variable 'u' must be
initialized by a constant expression" and on GCC as "constexpr evaluation
operation count exceeds limit". Where one layout holds every blade of the
other, convert the smaller operand to the larger one's type first, as in
`x + X{y}` for x of type X: a sum of one type merges nothing.

## Compile-time limits: duals, dot products and projections

Each of these operations is a composition of the products, the reverse, and
the complements, so it costs what its parts cost, and the smallest of their
limits bounds it. The weight, the weight duals, the antidot, and the
projection pass through the complements, whose grades grow with n, so their
compile cost grows with n even when the operand uses few generators.

Measured the same with and without libstdc++'s assertions: the bulk, the
weight, and the four duals build up to full<7> on Clang, where full<8> is
past the reverse's limit, and up to full<9>, the largest tried, on GCC; the
dot, the antidot, and the projection build up to full<5> on Clang and full<6>
on GCC, the limits of the geometric product's and the wedge's plans, and the
antiprojection the same. Every operation builds for full<5> of
conformal(3). In a dense form of five generators, where every pair of
generators is coupled, the dot and the antidot of two full<5> multivectors
do not build on Clang; GCC builds them.

A failure is reported inside the plan's generation, on Clang with the note
"constexpr evaluation hit maximum step limit" and on GCC as an operation
count over the limit (without optimization, sometimes as a libstdc++
assertion that "is not a constant expression"), followed by errors that do
not name the operation.

## Compile-time limits: transform and compose

`compose` is one geometric antiproduct and `transform` two, each a plan
built once for each pair of layouts, so they have the antiproduct's limits.
Measured the same with and without libstdc++'s assertions, for x a full<n>
multivector of R(n,0,0) and the versor on every blade of even antigrade, the
largest layout of one parity: `transform` builds up to n = 5 on Clang and
n = 6 on GCC, and `compose` of two such versors up to n = 5 on Clang and
n = 7, the largest tried, on GCC. Both build for full<5> of conformal(3). In
a dense form of five generators neither builds on Clang, and both build on
GCC.

A failure is reported inside a plan's generation, on Clang as "constexpr
variable 'count' must be initialized by a constant expression" and on GCC as
an operation count over the limit or a libstdc++ assertion that is not a
constant expression, followed by errors that do not name the operation.

## Compile-time limits: change of basis

The checks of a change run once per change tag, in about 2n³ checked
multiplications for the inner products and 5n³ for the inverse and its
check, whatever the layouts. The terms of a layout cost what the images of
its blades cost: the image of a blade of grade k is built by wedging the rows
of its generators one at a time, so its cost grows with the number of nonzero
entries in those rows and, for a dense change, with the binomial coefficients
C(n, j) for j up to k. `conformal_change` has at most two entries per row, so
its images have at most two terms for each null generator they hold.

Measured without optimization and with `_GLIBCXX_ASSERTIONS`, which
libstdc++ 16 defines by itself in such a build, both ways for each:

- `conformal_change<D>` maps full<D+2> up to D = 5 on Clang and D = 6 on
  GCC.
- A change whose every entry is nonzero maps full<n> up to n = 5 on Clang
  and n = 7 on GCC, the bivectors up to n = 8 on Clang (n = 7 with
  `_GLIBCXX_NO_ASSERTIONS`) and n = 12 on GCC, and the vectors at every n up
  to 16, the largest that the factories build by default.

A failure is reported inside the generation, on Clang as a constexpr
variable that "must be initialized by a constant expression" with the note
"constexpr evaluation hit maximum step limit", and on GCC as an operation
count over the limit, followed by errors that do not name the change.

## Raising the limits

Each compiler takes a flag that raises its limit on constant evaluation:

| Compiler | Limit | Default | Flag |
| --- | --- | --- | --- |
| Clang 23 | steps of constant evaluation | 1,048,576 | `-fconstexpr-steps=<n>` |
| GCC 16 | operations of constant evaluation | 33,554,432 | `-fconstexpr-ops-limit=<n>` |
| GCC 16 | iterations of one loop | 262,144 | `-fconstexpr-loop-limit=<n>` |

Two measured cases: `-fconstexpr-steps=1200000` builds the geometric
antiproduct of two full<5> multivectors of conformal(3) on Clang without
libstdc++'s assertions, and `-fconstexpr-ops-limit=134217728` builds the
geometric product of the scalar with full<10> on GCC. Defining
`_GLIBCXX_ASSERTIONS` lowers both compilers' counts of a plan, since
libstdc++ then checks a subscript without a function call. The other remedy
is a smaller layout: name the blades an object can hold rather than taking
every blade, and convert to the larger type before a sum.

## The rounding check

In [checking mode](core.md#checking-mode), `transform` checks that each
coefficient it drops, zero for a versor, is zero within the operation's
forward error bound when the scalar is a rounding type:
|d| ≤ fl(fl(γ_(K+1)) P), where γ_k = k u / (1 − k u) for the unit roundoff
u, K is the number of roundings on d's path, and P the same computation on
the absolute values of the versor and the object, with a small padding (the
smallest normal number divided by u) added after each product to cover a
product that underflows. On failure the message is "transform: v is not a
versor on x within rounding". A NaN or infinite dropped coefficient, or one
whose P overflows, fails the check.

Which scalar types it checks:

- the IEEE 754 binary floating-point types that round to nearest, those
  whose `std::numeric_limits` give `is_iec559` and `round_to_nearest`.
  `float` and `double` are tested; `long double` is neither tested nor
  claimed.
- Not a type of that kind whose precision is too coarse for the bound's
  margin, (K + 1)(2K + 1) u < 1 for every dropped coefficient, as
  `std::bfloat16_t` is for most transforms.
- Not any other scalar that is not exact, such as `std::complex<double>`.
- `truncate` never checks a rounding scalar: it has no error bound to test
  against.

The padding puts a floor under the bound, near 10^−306 in `double` and 10^−36
in `float`: a dropped part below it is not seen, so in `float` the check of a
non-versor whose two operands are both below about 2^−41 (4.5e−13), or of a
1% error in a conformal(3) motor below about 2^−35, is lost.

The bound assumes that each operation rounds once to the scalar's own
precision, to nearest, with gradual underflow, and with IEEE 754's NaN and
infinity. A build that flushes subnormal numbers to zero is outside it, and a
checking build under `-ffast-math` or `-ffinite-math-only` does not compile.
Options the compiler does not report, such as `-fassociative-math` alone, are
outside this test. The tests run on x86-64, and Galoot's presets build for
baseline x86-64, where nothing is fused, with `-ffp-contract=off`.

The rounding check's bound holds with or without multiply-add contraction (`-ffp-contract=off`, `on`, or `fast`), provided each fused multiply-add rounds once to the scalar's own precision: fusing a multiplication into an addition removes a rounding and never adds one, so the check never rejects what a versor drops. This is derived, and was checked with GCC 16.2.1 and Clang 23.1.4 at `-O2 -march=x86-64-v3`, where no check fired on exact versors and every rigid(3) result stayed within its bound.

Contraction does change the bits. Under `-ffp-contract=fast`, which GCC uses by default for C++ when the target has fused multiply-add instructions (on x86-64, with `-mfma` or `-march=x86-64-v3` and above; other targets were not tested), a transform's result can differ in its last bits between a checking and a non-checking build (10% to 14% of coefficients in our measurements), and between a constant evaluation and run time (43% to 49%). With `-ffp-contract=off` or `on` both compilers tested gave the same bits in every case. Baseline x86-64 builds contain no fused operations.

The bound takes the versor and the object as exact. A versor with rounding
error of its own, from its construction, is only nearly a versor, and where
what `transform` drops is zero only because the versor is one, as for the
antiscalar part of a conformal(3) vector, that error can exceed the bound and
the check aborts. Composing conformal(3) motors in floating point lets the
defect grow with the chain: for motors of two random flat planes each,
composed in `double` in a build without fused multiply-add, the check fired
for 0.2% of chains of 2 motors, 2.2% of
8, 11.4% of 64, and 22.7% of 128, while versors whose components were each
rounded once never fired. Rebuild or renormalize such a motor, or turn
checking off. In rigid(3), for a point, a line, a plane, a motor, or a
flector, what `transform` drops is zero for every versor of one parity, so
this cannot happen there.

## Toolchains

Galoot is tested with GCC 16.2.1 and Clang 23.1.4, each with libstdc++ 16,
on x86-64 Linux, in C++23 with compiler extensions off. Other compilers,
standard libraries, and platforms are not tested. The headers need no build
system: with `include/` on the include path, `-std=c++23 -pedantic-errors`
compiles a program that includes the core or the rga3 preset, and the core,
the preset, and the printing header compile with `-fno-exceptions` too. The
core throws no exceptions; Galoot's own test runner uses them. The CMake
routes of the [README](../../README.md) need CMake 4.4 or later, the version
Galoot's build requires; no older version is tested.

## The rga3 preset

The objects of the rga3 preset are small layouts of four generators, far
below every compile-time limit on this page, and every operation on them
builds at the compilers' default limits. In rigid(3), for those objects,
what `transform` drops is zero for every motor and flector, so the rounding
check cannot reject a correct result there.

`rotation` takes the length of its axis's direction as the square root of
the sum of its squares, so that length must lie between about 1e−150 and
1e150 in `double`, or 1e−18 and 1e18 in `float`, for the squares neither to
underflow nor to overflow. A length that is zero or not finite is checked in
checking mode; a length whose squares fall among the subnormal numbers
passes the check and loses accuracy.
