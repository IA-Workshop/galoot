// The identities that relate the bulk, the weight, the duals, the dot, and
// the antidot of galoot/core/dual.hpp to each other and to the metric,
// checked through the public functions over many signatures: diagonal,
// degenerate, indefinite, and not diagonal.
//
// G is the metric exomorphism and antiG = left(G right(u)) the
// antiexomorphism. Each identity is checked on seeded random integer
// multivectors, on which every operation is exact, so both sides must be
// equal. The two sides may hold their blades in different layouts, so they
// are compared blade by blade, on the canonical blade of each mask. The
// determinant of each Gram matrix is computed here by exact fraction-free
// elimination.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <type_traits>
#include <utility>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
// The sums, multiples, and negations of multivectors, found by
// argument-dependent lookup, which the include check does not see.
#include <galoot/core/linear.hpp> // IWYU pragma: keep
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
// nonzero: the duals are checked on every blade, and the dot and the antidot
// on vectors and on bivectors, since the geometric product of two full
// multivectors of such a form, which the dot reads, is beyond what one
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

// The forms of six generators: the duals on every blade, and the dot and
// the antidot on vectors and on bivectors, for the same reason.
using six_forms =
    forms<core::R<4, 2, 0>(), core::R<3, 3, 0>(), core::conformal<4>()>;

// The forms of 8, 12, and 16 generators, checked on sparse sets.
using large_forms = forms<core::R<8, 0, 0>(), core::R<6, 6, 0>(),
                          core::R<15, 0, 1>(), core::R<8, 8, 0>()>;

// conformal(1) to conformal(4), where G is its own inverse and
// det g = -1, so that antiG = det(g) G^-1 = -G.
using conformal_forms = forms<core::conformal<1>(), core::conformal<2>(),
                              core::conformal<3>(), core::conformal<4>()>;

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

// det g of the algebra's Gram matrix, by fraction-free (Bareiss)
// elimination: after step k every entry below row k is a k+1 by k+1 minor,
// so each division is exact, and a zero pivot is replaced by a row below it
// with the sign of the swap.
template <class Alg>
std::int64_t determinant() {
  constexpr std::size_t n = generators<Alg>;
  std::array<std::array<std::int64_t, n>, n> m{};
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      m.at(i).at(j) = Alg::value.gram.at(i).at(j);
    }
  }
  std::int64_t sign = 1;
  std::int64_t previous = 1;
  for (std::size_t k = 0; k < n; ++k) {
    std::size_t pivot = k;
    while (pivot < n && m.at(pivot).at(k) == 0) {
      ++pivot;
    }
    if (pivot == n) {
      return 0;
    }
    if (pivot != k) {
      std::swap(m.at(pivot), m.at(k));
      sign = -sign;
    }
    for (std::size_t i = k + 1; i < n; ++i) {
      for (std::size_t j = k + 1; j < n; ++j) {
        m.at(i).at(j) = ((m.at(i).at(j) * m.at(k).at(k)) -
                         (m.at(i).at(k) * m.at(k).at(j))) /
                        previous;
      }
    }
    previous = m.at(k).at(k);
  }
  return sign * m.at(n - 1).at(n - 1);
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

// x with each coefficient multiplied by `factor` and by (-1)^(gr ag) of its
// blade, the sign that turns a right complement into a left one.
template <class X>
X by_complement_parity(X x, std::int64_t factor) {
  constexpr int n = static_cast<int>(generators<typename X::algebra_type>);
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const int grade = std::popcount(X::layout.blades.at(k).mask);
    const int parity = (grade * (n - grade)) % 2 == 0 ? 1 : -1;
    x.coefficients.at(k) *= factor * parity;
  }
  return x;
}

// The number of random values each identity is checked on, in each form.
constexpr int samples = 3;

// G antiG = antiG G = det(g) times the identity, antiG = right G left as well
// as left G right, each double dual is (-1)^(gr ag) det(g) x, and the left
// duals are the right duals times (-1)^(gr ag), on random x of X.
template <class X>
void check_dual_identities(std::mt19937_64& source) {
  using core::bulk;
  using core::weight;
  using Alg = X::algebra_type;
  const std::int64_t det = determinant<Alg>();
  INFO("form: ", gram_text<Alg>());
  INFO("det g: ", det);
  for (int k = 0; k < samples; ++k) {
    const X x = random_integers<X>(source);
    const X scaled = x * det;
    CHECK(difference(bulk(weight(x)), scaled) == "");
    CHECK(difference(weight(bulk(x)), scaled) == "");
    CHECK(difference(weight(x), core::right_complement(
                                    bulk(core::left_complement(x)))) == "");
    const X twice = by_complement_parity(x, det);
    CHECK(difference(core::bulk_dual(core::bulk_dual(x)), twice) == "");
    CHECK(difference(core::weight_dual(core::weight_dual(x)), twice) == "");
    CHECK(difference(core::left_bulk_dual(core::left_bulk_dual(x)), twice) ==
          "");
    CHECK(difference(core::left_weight_dual(core::left_weight_dual(x)),
                     twice) == "");
    CHECK(difference(core::left_bulk_dual(x),
                     by_complement_parity(core::bulk_dual(x), 1)) == "");
    CHECK(difference(core::left_weight_dual(x),
                     by_complement_parity(core::weight_dual(x), 1)) == "");
  }
}

// The dot and the antidot are symmetric, right(antidot(a, b)) =
// dot(right(a), right(b)), and the antiscalar parts of a ^ bulk_dual(b) and
// a ^ weight_dual(b) are the dot and the antidot, on random a of A and b of
// B. The last holds for operands of any grades: e_A ^ right(e_C) is the
// antiscalar when A = C, has a lower grade when e_C has the higher one, and
// is zero otherwise, and G keeps grades.
template <class A, class B>
void check_dot_identities(std::mt19937_64& source) {
  using core::antidot;
  using core::dot;
  using core::right_complement;
  INFO("form: ", gram_text<typename A::algebra_type>());
  for (int k = 0; k < samples; ++k) {
    const A a = random_integers<A>(source);
    const B b = random_integers<B>(source);
    CHECK(difference(dot(a, b), dot(b, a)) == "");
    CHECK(difference(antidot(a, b), antidot(b, a)) == "");
    CHECK(difference(right_complement(antidot(a, b)),
                     dot(right_complement(a), right_complement(b))) == "");
    constexpr core::mask_t antiscalar =
        (core::mask_t{1} << generators<typename A::algebra_type>)-1;
    CHECK(value_on(core::wedge(a, core::bulk_dual(b)), antiscalar) ==
          value_on(dot(a, b), 0));
    CHECK(value_on(core::wedge(a, core::weight_dual(b)), antiscalar) ==
          value_on(antidot(a, b), antiscalar));
  }
}

// In a diagonal form, the bulk of a blade is the product of the squares of
// its generators times the blade and the weight the product of the squares
// of the generators it lacks, on random x of X; forms that are not diagonal
// are skipped.
template <class X>
void check_diagonal_parts(std::mt19937_64& source) {
  using Alg = X::algebra_type;
  constexpr std::size_t n = generators<Alg>;
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      if (i != j && Alg::value.gram.at(i).at(j) != 0) {
        return;
      }
    }
  }
  INFO("form: ", gram_text<Alg>());
  for (int k = 0; k < samples; ++k) {
    const X x = random_integers<X>(source);
    X kept = x;
    X lacking = x;
    for (std::size_t c = 0; c < X::component_count; ++c) {
      const core::mask_t mask = X::layout.blades.at(c).mask;
      for (std::size_t i = 0; i < n; ++i) {
        const std::int64_t square = Alg::value.gram.at(i).at(i);
        if (((mask >> i) & 1U) != 0) {
          kept.coefficients.at(c) *= square;
        } else {
          lacking.coefficients.at(c) *= square;
        }
      }
    }
    CHECK(difference(core::bulk(x), kept) == "");
    CHECK(difference(core::weight(x), lacking) == "");
  }
}

// The layouts each list of forms is checked on: every blade, vectors,
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

// Reference: identity: G antiG = antiG G = det(g) times the identity, as
//   bulk(weight(x)) and weight(bulk(x)), antiG = right G left as well as
//   left G right, and each double dual, bulk_dual(bulk_dual(x)) and the
//   other three, is (-1)^(gr ag) det(g) x, which follows from those because
//   right(right(u)) = (-1)^(gr ag) u, so in a degenerate metric the double
//   bulk dual is zero, and the left duals are the right duals times
//   (-1)^(gr ag), on random integer multivectors of every blade for
//   each form of up to five generators, the dense random forms of five, and
//   the forms of six, and on sparse sets for 8, 12, and 16
TEST_CASE("the duals compose to det g over many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dual_identities<full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dual_identities<full_mv<G>>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dual_identities<full_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dual_identities<sparse_a_mv<G>>(s), ...);
    (check_dual_identities<sparse_b_mv<G>>(s), ...);
  }(large_forms{}, source);
}

// Reference: identity: the dot and the antidot are symmetric, since G and
//   antiG are symmetric, and right(antidot(a, b)) = dot(right(a), right(b)),
//   since antiG = left G right and the right complement is a signed
//   permutation whose inverse, the left complement, is its transpose, and
//   a ^ bulk_dual(b) = (a . b) antiscalar for a and b of equal grade, which
//   for operands of any grades holds on the antiscalar part, and likewise
//   for the weight dual and the antidot, on
//   random integer multivectors of every blade for each form of up to five
//   generators, on vectors and on bivectors for the dense random forms of
//   five and the forms of six, and on sparse sets for 8, 12, and 16
TEST_CASE("the dot and the antidot are symmetric De Morgan pairs over many "
          "forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dot_identities<full_mv<G>, full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dot_identities<vectors_mv<G>, vectors_mv<G>>(s), ...);
    (check_dot_identities<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(dense_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dot_identities<vectors_mv<G>, vectors_mv<G>>(s), ...);
    (check_dot_identities<bivectors_mv<G>, bivectors_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_dot_identities<sparse_a_mv<G>, sparse_b_mv<G>>(s), ...);
    (check_dot_identities<sparse_a_mv<G>, sparse_a_mv<G>>(s), ...);
  }(large_forms{}, source);
}

// Reference: identity: in a diagonal form the bulk of a blade is the
//   product of the squares of its generators times it and the weight the
//   product of the squares of the generators it lacks, so in rigid(d) the
//   bulk and the weight split x, in R(n, 0, 0) both are x, and with two or
//   more null generators a blade with some but not all of them is in
//   neither, on random integer multivectors of every blade for each diagonal
//   form of up to six generators and on sparse sets for 8, 12, and 16, and
//   in conformal(1) to conformal(4), where G is its own inverse and
//   det g = -1, the weight is the negated bulk
TEST_CASE("the bulk and the weight in diagonal and conformal forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_diagonal_parts<full_mv<G>>(s), ...);
  }(small_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_diagonal_parts<full_mv<G>>(s), ...);
  }(six_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    (check_diagonal_parts<sparse_a_mv<G>>(s), ...);
    (check_diagonal_parts<sparse_b_mv<G>>(s), ...);
  }(large_forms{}, source);
  []<auto... G>(forms<G...>, std::mt19937_64& s) {
    const auto check = [&]<class X>(std::type_identity<X>) {
      INFO("form: ", gram_text<typename X::algebra_type>());
      for (int k = 0; k < samples; ++k) {
        const X x = random_integers<X>(s);
        CHECK(difference(core::weight(x), -core::bulk(x)) == "");
      }
    };
    (check(std::type_identity<full_mv<G>>{}), ...);
  }(conformal_forms{}, source);
}
