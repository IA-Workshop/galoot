// The identities that relate the products, the antiproducts, the
// involutions, and the complements of multivectors, checked through the
// public functions and operators of galoot/core/product.hpp and
// galoot/core/unary.hpp over many signatures: diagonal, degenerate,
// indefinite, and not diagonal.
//
// Each identity is checked on seeded random integer multivectors, on which
// every operation is exact, so both sides must be equal. The two sides of an
// identity may hold their blades in different layouts, so they are compared
// blade by blade, on the canonical blade of each mask.
#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

namespace {

namespace core = galoot::core;

// An algebra whose metric is the value G.
template <auto G>
struct form {
  static constexpr auto value = G;
};

// A list of metrics.
template <auto... G>
struct forms {};

// The forms of up to five generators that the identities are checked over on
// all blades: each diagonal R(p, q, r) with null generators last up to four
// generators, and R(4,1,0) and R(4,0,1); conformal(1) to conformal(3); the
// Witt forms of one and two modes; a degenerate form that is not diagonal;
// rigid(3) with its null generator first; and seeded random symmetric forms
// of two to four generators with entries from -3 to 3.
using small_forms = forms<
    // n = 1
    core::R<1, 0, 0>(), core::R<0, 1, 0>(), core::R<0, 0, 1>(),
    // n = 2
    core::R<2, 0, 0>(), core::R<0, 2, 0>(), core::R<1, 1, 0>(),
    core::R<1, 0, 1>(), core::R<0, 0, 2>(),
    core::from_gram({
        {0, 1},
        {1, 0},
    }),
    core::from_gram({
        {-1, -2},
        {-2, 3},
    }),
    core::from_gram({
        {3, 1},
        {1, -2},
    }),
    core::from_gram({
        {3, -2},
        {-2, -2},
    }),
    // n = 3
    core::R<3, 0, 0>(), core::R<2, 1, 0>(), core::R<2, 0, 1>(),
    core::R<1, 1, 1>(), core::R<0, 0, 3>(), core::conformal<1>(),
    core::from_gram({
        {0, 1, 0},
        {1, 0, 0},
        {0, 0, 0},
    }),
    core::from_gram({
        {2, 0, -3},
        {0, 1, -3},
        {-3, -3, -2},
    }),
    core::from_gram({
        {-3, -3, 0},
        {-3, -2, 2},
        {0, 2, 0},
    }),
    core::from_gram({
        {-3, 1, -2},
        {1, -3, -2},
        {-2, -2, 1},
    }),
    // n = 4
    core::R<4, 0, 0>(), core::rigid<3>(), core::R<1, 3, 0>(),
    core::R<3, 1, 0>(), core::R<2, 2, 0>(), core::R<2, 0, 2>(),
    core::R<0, 0, 4>(), core::conformal<2>(),
    core::from_gram({
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 0, 1},
        {0, 0, 1, 0},
    }),
    core::from_gram({
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }),
    core::from_gram({
        {-2, 0, -2, 2},
        {0, -2, -3, 3},
        {-2, -3, 1, 3},
        {2, 3, 3, 3},
    }),
    core::from_gram({
        {0, 1, -1, 0},
        {1, -1, 0, -1},
        {-1, 0, 2, 0},
        {0, -1, 0, -1},
    }),
    // n = 5
    core::R<4, 1, 0>(), core::R<4, 0, 1>(), core::conformal<3>()>;

// The random forms of five generators, whose entries are almost all
// nonzero, checked on bivectors, and on vectors for the products of three
// factors: the products of all their blades, the antiproduct of a vector
// with a bivector, and a product of three bivectors are beyond what one
// compiler evaluates at its default limits.
using dense_forms = forms<core::from_gram({
                              {3, 1, -3, -1, -2},
                              {1, -3, -2, 1, 0},
                              {-3, -2, -2, 3, 0},
                              {-1, 1, 3, 3, 2},
                              {-2, 0, 0, 2, -1},
                          }),
                          core::from_gram({
                              {-3, 1, 2, -1, -2},
                              {1, -3, 0, -3, -1},
                              {2, 0, 0, 3, -3},
                              {-1, -3, 3, 1, -2},
                              {-2, -1, -3, -2, 2},
                          })>;

// The forms of six generators, checked on vectors and bivectors, since the
// products of all their blades are beyond what one compiler evaluates at its
// default limits.
using six_forms =
    forms<core::R<4, 2, 0>(), core::R<3, 3, 0>(), core::conformal<4>()>;

// The forms of 8, 12, and 16 generators, checked on sparse sets.
using large_forms = forms<core::R<8, 0, 0>(), core::R<6, 6, 0>(),
                          core::R<15, 0, 1>(), core::R<8, 8, 0>()>;

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// Sparse sets of an algebra of N generators, by mask, where bit i - 1 stands
// for e_i: {1, e1, e_N, e1 e_N} and {e1, e2, e_N}.
template <std::size_t N>
inline constexpr core::blade_set<4> sparse_a{{{
    {.mask = 0, .sign = 1},
    {.mask = 0x1, .sign = 1},
    {.mask = core::mask_t{1} << (N - 1), .sign = 1},
    {.mask = 0x1 | (core::mask_t{1} << (N - 1)), .sign = 1},
}}};
template <std::size_t N>
inline constexpr core::blade_set<3> sparse_b{{{
    {.mask = 0x1, .sign = 1},
    {.mask = 0x2, .sign = 1},
    {.mask = core::mask_t{1} << (N - 1), .sign = 1},
}}};

// The Gram matrix of the algebra as text, which names the form in a failure.
template <class Alg>
std::string gram_text() {
  std::string text = "[";
  for (std::size_t i = 0; i < generators<Alg>; ++i) {
    text += i == 0 ? "[" : ", [";
    for (std::size_t j = 0; j < generators<Alg>; ++j) {
      text +=
          (j == 0 ? "" : ", ") + std::to_string(Alg::value.gram.at(i).at(j));
    }
    text += "]";
  }
  return text + "]";
}

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
std::int64_t value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign * x.coefficients.at(k);
    }
  }
  return 0;
}

// The name of the canonical blade of a mask: e followed by the indices of
// its generators, separated by dots, or 1.
std::string blade_name(core::mask_t mask) {
  if (mask == 0) {
    return "1";
  }
  std::string name = "e";
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += (name.size() > 1 ? "." : "") + std::to_string(i + 1);
    }
  }
  return name;
}

// Empty when x and y have the same coefficient on every canonical blade that
// either layout holds; otherwise the first blade where they differ, with
// both values.
template <class X, class Y>
std::string difference(const X& x, const Y& y) {
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + std::to_string(value_on(x, mask)) +
           " against " + std::to_string(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

template <class X>
X random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-3, 3);
  X x{};
  for (auto& c : x.coefficients) {
    c = digit(source);
  }
  return x;
}

// The antiscalar e12...n of the algebra of X with coefficient 1, on the
// layout that holds it alone.
template <class X>
auto antiscalar() {
  using Alg = typename X::algebra_type;
  constexpr core::mask_t all = (core::mask_t{1} << generators<Alg>)-1;
  constexpr core::blade_set<1> layout{{{{.mask = all, .sign = 1}}}};
  core::mv<Alg, layout, std::int64_t> one{};
  one.coefficients.at(0) = 1;
  return one;
}

// The number of random triples each identity is checked on, in each form.
constexpr int samples = 3;

// The four De Morgan laws and their converses on random a of A and b of B.
template <class A, class B>
void check_de_morgan(std::mt19937_64& source) {
  using core::left_complement;
  using core::right_complement;
  INFO("form: ", gram_text<typename A::algebra_type>());
  for (int k = 0; k < samples; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    CHECK(difference(right_complement(a * b),
                     right_complement(a) % right_complement(b)) == "");
    CHECK(difference(right_complement(a ^ b),
                     right_complement(a) & right_complement(b)) == "");
    CHECK(difference(left_complement(a * b),
                     left_complement(a) % left_complement(b)) == "");
    CHECK(difference(left_complement(a ^ b),
                     left_complement(a) & left_complement(b)) == "");
    CHECK(difference(right_complement(a % b),
                     right_complement(a) * right_complement(b)) == "");
    CHECK(difference(right_complement(a & b),
                     right_complement(a) ^ right_complement(b)) == "");
    CHECK(difference(left_complement(a % b),
                     left_complement(a) * left_complement(b)) == "");
    CHECK(difference(left_complement(a & b),
                     left_complement(a) ^ left_complement(b)) == "");
  }
}

// Associativity of the antiproducts on random a, c of A and b of B, and the
// antiscalar as the identity of the geometric antiproduct.
template <class A, class B>
void check_antiproduct_laws(std::mt19937_64& source) {
  INFO("form: ", gram_text<typename A::algebra_type>());
  const auto one = antiscalar<A>();
  for (int k = 0; k < samples; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    const A c = random_integers<A>(source);
    CHECK(difference((a % b) % c, a % (b % c)) == "");
    CHECK(difference((a & b) & c, a & (b & c)) == "");
    CHECK(difference(one % a, a) == "");
    CHECK(difference(b % one, b) == "");
  }
}

// The rules of the involutions over the products on random a of A and b of
// B.
template <class A, class B>
void check_product_rules(std::mt19937_64& source) {
  using core::antireverse;
  using core::grade_involution;
  using core::reverse;
  INFO("form: ", gram_text<typename A::algebra_type>());
  for (int k = 0; k < samples; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    CHECK(difference(reverse(a * b), reverse(b) * reverse(a)) == "");
    CHECK(difference(reverse(a ^ b), reverse(b) ^ reverse(a)) == "");
    CHECK(difference(antireverse(a % b), antireverse(b) % antireverse(a)) ==
          "");
    CHECK(difference(grade_involution(a * b),
                     grade_involution(a) * grade_involution(b)) == "");
  }
}

// The layouts each list of forms is checked on: every blade, vectors and
// bivectors, and the sparse sets.
template <auto G>
using full_mv =
    core::mv<form<G>, core::full<generators<form<G>>>, std::int64_t>;
template <auto G>
using vectors_mv =
    core::mv<form<G>, core::of_grade<generators<form<G>>, 1>, std::int64_t>;
template <auto G>
using bivectors_mv =
    core::mv<form<G>, core::of_grade<generators<form<G>>, 2>, std::int64_t>;
template <auto G>
using sparse_a_mv =
    core::mv<form<G>, sparse_a<generators<form<G>>>, std::int64_t>;
template <auto G>
using sparse_b_mv =
    core::mv<form<G>, sparse_b<generators<form<G>>>, std::int64_t>;

} // namespace

// Reference: identity: the De Morgan laws right(a ⟑ b) = right(a) ⟇ right(b)
//   and right(a ^ b) = right(a) v right(b), the same with left, and their
//   converses right(a ⟇ b) = right(a) ⟑ right(b) and
//   right(a v b) = right(a) ^ right(b), with left likewise, which follow
//   from a ⟇ b = left(right(a) ⟑ right(b)) and a v b = left(right(a) ^
//   right(b)) because left and right are inverses, on random integer
//   multivectors of every blade for each form of up to four generators and
//   R(4,1,0), R(4,0,1), and conformal(3), on bivectors for two dense random
//   forms of five, on vectors and bivectors for six, and on sparse sets for
//   8, 12, and 16
TEST_CASE("the De Morgan laws hold on multivectors over many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_de_morgan<full_mv<G>, full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_de_morgan<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_de_morgan<vectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_de_morgan<sparse_a_mv<G>, sparse_b_mv<G>>(s), ...);
  }(large_forms{}, source);
}

// Reference: identity: the geometric antiproduct and the antiwedge are
//   associative, and the antiscalar is the identity of the geometric
//   antiproduct on both sides, because right(𝟙) is the scalar 1, on random
//   integer multivectors of every blade for each form of up to four
//   generators and R(4,1,0), R(4,0,1), and conformal(3), on vectors for two
//   dense random forms of five, on vectors and bivectors for six, and on
//   sparse sets for 8, 12, and 16
TEST_CASE("the antiproducts are associative over many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_antiproduct_laws<full_mv<G>, full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_antiproduct_laws<vectors_mv<G>, vectors_mv<G>>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_antiproduct_laws<vectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_antiproduct_laws<sparse_a_mv<G>, sparse_b_mv<G>>(s), ...);
  }(large_forms{}, source);
}

// Reference: identity: the reverse reverses products,
//   reverse(a ⟑ b) = reverse(b) ⟑ reverse(a) and
//   reverse(a ^ b) = reverse(b) ^ reverse(a), the antireverse reverses
//   antiproducts, antireverse(a ⟇ b) = antireverse(b) ⟇ antireverse(a),
//   and the grade involution keeps the order,
//   grade_involution(a ⟑ b) = grade_involution(a) ⟑ grade_involution(b), on
//   random integer multivectors of every blade for each form of up to four
//   generators and R(4,1,0), R(4,0,1), and conformal(3), on bivectors for
//   two dense random forms of five, on vectors and bivectors for six, and on
//   sparse sets for 8, 12, and 16
TEST_CASE("the involutions follow their product rules over many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_product_rules<full_mv<G>, full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_product_rules<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_product_rules<vectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_product_rules<sparse_a_mv<G>, sparse_b_mv<G>>(s), ...);
  }(large_forms{}, source);
}
