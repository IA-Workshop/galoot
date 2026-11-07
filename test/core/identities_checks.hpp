#ifndef GALOOT_TEST_CORE_IDENTITIES_CHECKS_HPP
#define GALOOT_TEST_CORE_IDENTITIES_CHECKS_HPP

// The checks of the identities that relate the products, the antiproducts,
// the involutions, and the complements of multivectors, through the public
// functions and operators of galoot/core/product.hpp and
// galoot/core/unary.hpp, and the groups of signatures they run over.
//
// Each identity is checked on seeded random integer multivectors, on which
// every operation is exact, so both sides must be equal. The two sides of an
// identity may hold their blades in different layouts, so they are compared
// blade by blade, on the canonical blade of each mask.
//
// The test cases, in identities_test.cpp, each check one identity over every
// group of forms in turn, drawing from one seeded source. Each group is
// checked in a source of its own, identities_<group>_test.cpp, so that the
// groups, whose products are most of the test's compile cost, compile in
// parallel.

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

namespace galoot_test::identities {

// The identities a test case checks.
enum class Identity : std::uint8_t {
  de_morgan,
  antiproduct_laws,
  product_rules,
};

// The checks have internal linkage, as they would in a single source, so
// that the compiler may inline each one into the one function that calls
// it and drop its own copy. With external linkage, GCC spent half again as
// many instructions compiling these sources at -O1 with the sanitizers.
// NOLINTNEXTLINE(misc-anonymous-namespace-in-header): one copy per source
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

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// Sparse sets of an algebra of N generators, by mask, where bit i - 1 stands
// for e_i: {1, e1, e_N, e1 e_N} and {e1, e2, e_N}.
template <std::size_t N>
inline constexpr core::mask_t last_generator = core::mask_t{1} << (N - 1);
template <std::size_t N>
inline constexpr core::blade_set<4> sparse_a{
    {
        {
            {.mask = 0, .sign = 1},
            {.mask = 0x1, .sign = 1},
            {.mask = last_generator<N>, .sign = 1},
            {.mask = core::mask_t{0x1} | last_generator<N>, .sign = 1},
        },
    },
};
template <std::size_t N>
inline constexpr core::blade_set<3> sparse_b{
    {
        {
            {.mask = 0x1, .sign = 1},
            {.mask = 0x2, .sign = 1},
            {.mask = last_generator<N>, .sign = 1},
        },
    },
};

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
    text += ']';
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
inline std::string blade_name(core::mask_t mask) {
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
  using Alg = X::algebra_type;
  constexpr std::size_t n = generators<Alg>;
  constexpr core::mask_t all = (core::mask_t{1} << n) - 1;
  constexpr core::blade_set<1> layout{{{{.mask = all, .sign = 1}}}};
  core::mv<Alg, layout, std::int64_t> one{};
  one.coefficients.at(0) = 1;
  return one;
}

// The number of random triples each identity is checked on, in each form.
inline constexpr int samples = 3;

// The four De Morgan laws and their converses on random a of A and b of B.
template <class A, class B>
void check_de_morgan(std::mt19937_64& source) {
  using core::left_complement;
  using core::right_complement;
  INFO("form: ", gram_text<typename A::algebra_type>());
  for (int k = 0; k < samples; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    CHECK(difference(right_complement(core::geometric_product(a, b)),
                     right_complement(a) % right_complement(b)) == "");
    CHECK(difference(right_complement(core::wedge(a, b)),
                     right_complement(a) & right_complement(b)) == "");
    CHECK(difference(left_complement(a * b),
                     left_complement(a) % left_complement(b)) == "");
    CHECK(difference(left_complement(a ^ b),
                     left_complement(a) & left_complement(b)) == "");
    CHECK(difference(right_complement(core::geometric_antiproduct(a, b)),
                     right_complement(a) * right_complement(b)) == "");
    CHECK(difference(right_complement(core::antiwedge(a, b)),
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

// The layouts the groups are checked on: every blade, vectors and
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

// One identity on random a of A<G> and b of B<G>, for each form G of the
// list in its order.
template <template <auto> class A, template <auto> class B, auto... G>
void check_each(Identity identity, forms<G...> /*list*/,
                std::mt19937_64& source) {
  switch (identity) {
  case Identity::de_morgan:
    (check_de_morgan<A<G>, B<G>>(source), ...);
    return;
  case Identity::antiproduct_laws:
    (check_antiproduct_laws<A<G>, B<G>>(source), ...);
    return;
  case Identity::product_rules:
    (check_product_rules<A<G>, B<G>>(source), ...);
    return;
  }
}

} // namespace

// Each group checks an identity over its forms, on the layouts its source
// states, drawing from source in the order of its forms.
void forms_up_to_three(Identity identity, std::mt19937_64& source);
void forms_of_four(Identity identity, std::mt19937_64& source);
void diagonal_forms_of_five(Identity identity, std::mt19937_64& source);
void conformal_three(Identity identity, std::mt19937_64& source);
void dense_forms_of_five(Identity identity, std::mt19937_64& source);
void forms_of_six(Identity identity, std::mt19937_64& source);
void large_forms(Identity identity, std::mt19937_64& source);

// The groups in the order each test case checks them, so that one seed
// gives the same multivectors in every build.
inline constexpr std::array groups{
    &forms_up_to_three, &forms_of_four,       &diagonal_forms_of_five,
    &conformal_three,   &dense_forms_of_five, &forms_of_six,
    &large_forms,
};

} // namespace galoot_test::identities

#endif // GALOOT_TEST_CORE_IDENTITIES_CHECKS_HPP
