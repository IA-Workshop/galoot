// Tests of change_basis and change_basis_back over the scalar types they
// serve: std::int64_t where no division is needed, and double, float, and
// std::complex<double>, against the same call over the test rational and
// over the tracking scalar of support/tracked.hpp.
//
// Each result coefficient is an integer sum of the input's coefficients
// divided once by a positive integer q, where the change's denominators
// reach that blade. Two classes of inputs fix how a floating result is
// checked, before any result is seen:
//
// Small dyadic inputs, n 2^-b with |n| <= 2^20 and one b from 0 to 20 for
// every coefficient of an input. Every
// integer coefficient sum C of the changes used here is at most 18,612 (3
// for conformal_change, 12 for its round trip), so every term and partial
// sum is an integer multiple of 2^-b below C 2^20 2^-b <= 2^(53-b) in
// magnitude, which double holds exactly, and for conformal_change below
// 2^(24-b), which float holds too. Only the division can round: by a power
// of two it is exact, so the result is the exact value, and otherwise it
// is the correctly rounded quotient of two exact operands, the double
// nearest the exact value.
//
// Wide integer inputs, |n| <= N with N = 2^53, or 2^50 where C exceeds 2^7,
// so that C N <= 2^60. Products and sums then round in double, and the
// result must lie within the forward error bound gamma_K M of the exact
// value, with K the count of roundings on its path, the division's
// included, and M the magnitude, both from the tracked run of the same
// call: equal to the exact value for K = 0, the nearest double for K = 1,
// and otherwise the ratio of the exact error to gamma_K M at most
// 1 + 2^-51, the limit of the ratio's own five roundings. The output
// denominators of these changes are below 2^9, which keeps every scaled
// value of that check within the rational.
#include <algorithm>
#include <bit>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <format>
#include <random>
#include <string>
#include <type_traits>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include "basis_change_forms.hpp"
#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;
using test_support::rational;
using test_support::tracked;

using random_2 = bc::to_diagonal_tag<core::from_gram({
    {-2, 0, -2, 2},
    {0, -2, -3, 3},
    {-2, -3, 1, 3},
    {2, 3, 3, 3},
})>;
using random_7 = bc::to_diagonal_tag<core::from_gram({
    {-3, 1, 2, -1, -2},
    {1, -3, 0, -3, -1},
    {2, 0, 0, 3, -3},
    {-1, -3, 3, 1, -2},
    {-2, -1, -3, -2, 2},
})>;
using null_first = bc::to_diagonal_tag<core::from_gram({
    {0, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
})>;

// The number of random inputs each case is checked on.
constexpr int samples = 20;

// The bound of the numerators of the small dyadic inputs, 2^20, and of
// their exponent: every coefficient of an input is n 2^-b, |n| <= 2^20,
// with one b, 0 <= b <= 20, for the whole input.
constexpr std::int64_t dyadic_numerator = test_support::power_of_two(20);
constexpr int dyadic_exponent = 20;

// The change C, or its inverse when Back, applied to x.
template <class C, bool Back, class X>
auto apply(const X& x) {
  if constexpr (Back) {
    return core::change_basis_back<C>(x);
  } else {
    return core::change_basis<C>(x);
  }
}

// The algebra a change C maps from, forward or back.
template <class C, bool Back>
using from_algebra =
    std::conditional_t<Back, typename C::target, typename C::source>;

// One input drawn three ways: the scalar under test, the test rational, and
// the tracking scalar, all of one exact value.
template <class C, bool Back, core::blade_set S, class T>
struct inputs {
  core::mv<from_algebra<C, Back>, S, T> value;
  core::mv<from_algebra<C, Back>, S, rational> exact;
  core::mv<from_algebra<C, Back>, S, tracked> counted;
};

template <class C, bool Back, core::blade_set S, class T>
inputs<C, Back, S, T> small_dyadic(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> numerator(-dyadic_numerator,
                                                        dyadic_numerator);
  std::uniform_int_distribution<int> exponent(0, dyadic_exponent);
  const int b = exponent(source);
  inputs<C, Back, S, T> in{};
  for (std::size_t k = 0; k < S.size(); ++k) {
    const std::int64_t n = numerator(source);
    const rational x{n, test_support::power_of_two(b)};
    in.value.coefficients.at(k) =
        static_cast<T>(static_cast<double>(n) /
                       static_cast<double>(test_support::power_of_two(b)));
    in.exact.coefficients.at(k) = x;
    in.counted.coefficients.at(k) = tracked::input(x);
  }
  return in;
}

template <class C, bool Back, core::blade_set S>
inputs<C, Back, S, double> wide_integers(std::mt19937_64& source,
                                         std::int64_t bound) {
  std::uniform_int_distribution<std::int64_t> digit(-bound, bound);
  inputs<C, Back, S, double> in{};
  for (std::size_t k = 0; k < S.size(); ++k) {
    const std::int64_t n = digit(source);
    in.value.coefficients.at(k) = static_cast<double>(n);
    in.exact.coefficients.at(k) = rational{n};
    in.counted.coefficients.at(k) = tracked::input(rational{n});
  }
  return in;
}

// Whether the denominator of x is a power of two.
bool is_dyadic(const rational& x) {
  return std::has_single_bit(static_cast<std::uint64_t>(x.denominator));
}

// The exact value of a float or double result.
template <class T>
rational exact_of(T d) {
  return test_support::exact_rational(static_cast<double>(d));
}

// Small dyadic inputs of C (or its inverse) on S over T, float or double:
// the tracked run's value equals the rational run's, and each result
// coefficient is the exact value where its denominator is a power of two and
// otherwise the double nearest it, which only double is asked for.
template <class C, bool Back, core::blade_set S, class T>
void check_small_dyadic(std::mt19937_64& source, const char* name) {
  INFO("change: ", std::string{name} + (Back ? ", back" : ""));
  for (int s = 0; s < samples; ++s) {
    const auto in = small_dyadic<C, Back, S, T>(source);
    const auto value = apply<C, Back>(in.value);
    const auto exact = apply<C, Back>(in.exact);
    const auto counted = apply<C, Back>(in.counted);
    for (std::size_t k = 0; k < exact.coefficients.size(); ++k) {
      const rational e = exact.coefficients.at(k);
      const T d = value.coefficients.at(k);
      CAPTURE(k);
      INFO("result: ", std::format("{}", d));
      CHECK(counted.coefficients.at(k).value == e);
      if (is_dyadic(e)) {
        CHECK(exact_of(d) == e);
      } else {
        CHECK(d == test_support::correctly_rounded(e));
      }
    }
  }
}

// The round trip of small dyadic inputs through C and back over T: every
// intermediate is exact, so the result is the input on its blades and zero
// on any other blade it holds.
template <class C, core::blade_set S, class T>
void check_dyadic_round_trip(std::mt19937_64& source, const char* name) {
  INFO("change: ", std::string{name});
  for (int s = 0; s < samples; ++s) {
    const auto in = small_dyadic<C, false, S, T>(source);
    const auto back =
        core::change_basis_back<C>(core::change_basis<C>(in.value));
    using Back = std::remove_cvref_t<decltype(back)>;
    for (std::size_t k = 0; k < Back::component_count; ++k) {
      const core::mask_t mask = Back::layout.blades.at(k).mask;
      rational expected{};
      for (std::size_t j = 0; j < S.size(); ++j) {
        if (S.blades.at(j).mask == mask) {
          expected =
              rational{S.blades.at(j).sign} * in.exact.coefficients.at(j);
        }
      }
      CAPTURE(k);
      CHECK(rational{Back::layout.blades.at(k).sign} *
                exact_of(back.coefficients.at(k)) ==
            expected);
    }
  }
}

// Small dyadic inputs of C (or its inverse) on S over std::complex<double>,
// real and imaginary parts drawn apart: each part is the exact value, since
// a product with the integer coefficient, (a + bi)(c + 0i), and a division
// by q + 0i with q a power of two round nothing under these inputs.
template <class C, bool Back, core::blade_set S>
void check_complex(std::mt19937_64& source, const char* name) {
  INFO("change: ", std::string{name} + (Back ? ", back" : ""));
  for (int s = 0; s < samples; ++s) {
    const auto real = small_dyadic<C, Back, S, double>(source);
    const auto imaginary = small_dyadic<C, Back, S, double>(source);
    core::mv<from_algebra<C, Back>, S, std::complex<double>> z{};
    for (std::size_t k = 0; k < S.size(); ++k) {
      z.coefficients.at(k) = std::complex<double>(
          real.value.coefficients.at(k), imaginary.value.coefficients.at(k));
    }
    const auto result = apply<C, Back>(z);
    const auto real_part = apply<C, Back>(real.exact);
    const auto imaginary_part = apply<C, Back>(imaginary.exact);
    for (std::size_t k = 0; k < result.coefficients.size(); ++k) {
      CAPTURE(k);
      CHECK(exact_of(result.coefficients.at(k).real()) ==
            real_part.coefficients.at(k));
      CHECK(exact_of(result.coefficients.at(k).imag()) ==
            imaginary_part.coefficients.at(k));
    }
  }
}

// Wide integer inputs, |n| <= bound, of C (or its inverse) on S over
// double: the tracked run's value equals the rational run's, and each
// coefficient is within its forward error bound. Returns the largest ratio
// of an error to its bound over the coefficients with two or more
// roundings.
template <class C, bool Back, core::blade_set S>
double check_wide(std::mt19937_64& source, const char* name,
                  std::int64_t bound) {
  INFO("change: ", std::string{name} + (Back ? ", back" : ""));
  double largest = 0.0;
  for (int s = 0; s < samples; ++s) {
    const auto in = wide_integers<C, Back, S>(source, bound);
    const auto value = apply<C, Back>(in.value);
    const auto exact = apply<C, Back>(in.exact);
    const auto counted = apply<C, Back>(in.counted);
    for (std::size_t k = 0; k < exact.coefficients.size(); ++k) {
      const tracked& t = counted.coefficients.at(k);
      const double d = value.coefficients.at(k);
      CAPTURE(k);
      CAPTURE(t.roundings);
      INFO("result: ", std::format("{}", d));
      CHECK(t.value == exact.coefficients.at(k));
      CHECK(test_support::within_quotient_bound(d, t.value, t.roundings,
                                                t.magnitude));
      if (t.roundings >= 2) {
        largest = std::max(largest, test_support::scaled_bound_ratio(
                                        d, t.value, t.roundings, t.magnitude));
      }
    }
  }
  MESSAGE(std::format("{}{}: largest ratio of error to bound {}", name,
                      Back ? ", back" : "", largest));
  return largest;
}

} // namespace

// Reference: proof: a change whose coefficients on a layout are integers
//   needs no division there, so an integer scalar computes it exactly: the
//   permutation of rigid(3) with its null generator first, and
//   conformal_change<3>() on e1, e2, e3, e45, and e12345, whose images e'1,
//   e'2, e'3, -e'45, and -e'12345 have integer coefficients once each
//   output's denominator is reduced with its terms, equal the results over
//   the test rational, and the inverse of conformal_change<3>() keeps the
//   Euclidean generators, e'i = e_i over the denominator 1, so it maps
//   e'1, e'2, e'3 back without a division
TEST_CASE("change_basis over std::int64_t where no division is needed") {
  using Full = core::mv<null_first::source, core::full<4>, std::int64_t>;
  Full x{};
  for (std::size_t k = 0; k < Full::component_count; ++k) {
    x.coefficients.at(k) = static_cast<std::int64_t>(k) - 7;
  }
  core::mv<null_first::source, core::full<4>, rational> r{};
  for (std::size_t k = 0; k < Full::component_count; ++k) {
    r.coefficients.at(k) = rational{x.coefficients.at(k)};
  }
  const auto y = core::change_basis<null_first>(x);
  const auto exact = core::change_basis<null_first>(r);
  for (std::size_t k = 0; k < y.coefficients.size(); ++k) {
    CAPTURE(k);
    CHECK(rational{y.coefficients.at(k)} == exact.coefficients.at(k));
  }
  const auto back = core::change_basis_back<null_first>(y);
  CHECK(core::truncate<Full>(back) == x);

  using cga3 = bc::conformal_tag<3>::source;
  constexpr auto layout = core::blades<cga3, "e1 e2 e3 e45 e12345">;
  const core::mv<cga3, layout, std::int64_t> p{1, 2, 3, 4, 5};
  const auto q = core::change_basis<bc::conformal_tag<3>>(p);
  CHECK(q.coefficients.size() == 5);
  CHECK(q.coefficients.at(0) == 1);
  CHECK(q.coefficients.at(1) == 2);
  CHECK(q.coefficients.at(2) == 3);
  CHECK(q.coefficients.at(3) == -4);
  CHECK(q.coefficients.at(4) == -5);

  using r410 = bc::conformal_tag<3>::target;
  const core::mv<r410, core::blades<r410, "e1 e2 e3">, std::int64_t> u{1, 2, 3};
  const auto v = core::change_basis_back<bc::conformal_tag<3>>(u);
  CHECK(v.coefficients.size() == 3);
  CHECK(v.coefficients.at(0) == 1);
  CHECK(v.coefficients.at(1) == 2);
  CHECK(v.coefficients.at(2) == 3);
}

// Reference: proof: with inputs n 2^-b, |n| <= 2^20, and the integer
//   coefficient sums of conformal_change<d>() at most 3 forward, 2 back, and
//   12 for the round trip, every term and partial sum is an integer multiple
//   of 2^-b that double and float hold exactly, and the divisions are by 1
//   or 2, which are exact, so for d = 1 to 4 every coefficient over double
//   and float, forward, back, and through the round trip, is the exact
//   value, and over std::complex<double> each part is, the products by an
//   integer coefficient and the divisions by 2 + 0i rounding nothing
TEST_CASE("conformal_change over double, float, and complex is exact on "
          "small dyadic inputs") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  using c1 = bc::conformal_tag<1>;
  using c2 = bc::conformal_tag<2>;
  using c3 = bc::conformal_tag<3>;
  using c4 = bc::conformal_tag<4>;
  check_small_dyadic<c1, false, core::full<3>, double>(source, "conformal(1)");
  check_small_dyadic<c1, true, core::full<3>, double>(source, "conformal(1)");
  check_small_dyadic<c2, false, core::full<4>, double>(source, "conformal(2)");
  check_small_dyadic<c2, true, core::full<4>, double>(source, "conformal(2)");
  check_small_dyadic<c3, false, core::full<5>, double>(source, "conformal(3)");
  check_small_dyadic<c3, true, core::full<5>, double>(source, "conformal(3)");
  check_small_dyadic<c4, false, core::full<6>, double>(source, "conformal(4)");
  check_small_dyadic<c4, true, core::full<6>, double>(source, "conformal(4)");
  check_dyadic_round_trip<c1, core::full<3>, double>(source, "conformal(1)");
  check_dyadic_round_trip<c2, core::full<4>, double>(source, "conformal(2)");
  check_dyadic_round_trip<c3, core::full<5>, double>(source, "conformal(3)");
  check_dyadic_round_trip<c4, core::full<6>, double>(source, "conformal(4)");

  check_small_dyadic<c1, false, core::full<3>, float>(source, "conformal(1)");
  check_small_dyadic<c1, true, core::full<3>, float>(source, "conformal(1)");
  check_small_dyadic<c2, false, core::full<4>, float>(source, "conformal(2)");
  check_small_dyadic<c2, true, core::full<4>, float>(source, "conformal(2)");
  check_small_dyadic<c3, false, core::full<5>, float>(source, "conformal(3)");
  check_small_dyadic<c3, true, core::full<5>, float>(source, "conformal(3)");
  check_small_dyadic<c4, false, core::full<6>, float>(source, "conformal(4)");
  check_small_dyadic<c4, true, core::full<6>, float>(source, "conformal(4)");
  check_dyadic_round_trip<c1, core::full<3>, float>(source, "conformal(1)");
  check_dyadic_round_trip<c2, core::full<4>, float>(source, "conformal(2)");
  check_dyadic_round_trip<c3, core::full<5>, float>(source, "conformal(3)");
  check_dyadic_round_trip<c4, core::full<6>, float>(source, "conformal(4)");

  check_complex<c3, false, core::full<5>>(source, "conformal(3)");
  check_complex<c3, true, core::full<5>>(source, "conformal(3)");
}

// Reference: proof: with inputs n 2^-b, |n| <= 2^20, and integer coefficient
//   sums of at most 867 (random form 7 to its diagonal), 774 (its inverse),
//   and 42 (random form 2), every term and partial sum is exact in double,
//   and the one division, by output denominators up to 72, 20, and 30 that
//   are not all powers of two, rounds once from exact operands, so each
//   coefficient is the double nearest the exact value, and the exact value
//   itself where its denominator is a power of two
TEST_CASE("a diagonalizing change over double rounds only its division on "
          "small dyadic inputs") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_small_dyadic<random_7, false, core::full<5>, double>(source,
                                                             "random 7");
  check_small_dyadic<random_7, true, core::full<5>, double>(source, "random 7");
  check_small_dyadic<random_2, false, core::full<4>, double>(source,
                                                             "random 2");
}

// Reference: proof: in the standard model of floating-point arithmetic each
//   product, sum, and division rounds once, so a result is within
//   gamma_K M of the exact value, with K the roundings on its path and M the
//   magnitude, as the tracked run counts them (N. J. Higham, Accuracy and
//   Stability of Numerical Algorithms, 2nd ed., 2002, equation (2.4) and
//   Lemma 3.1), checked on wide integer inputs, up to 2^50 for random form
//   7 to its diagonal and back and up to 2^53 for random form 2 and for
//   conformal_change<3>() both ways
TEST_CASE("a change of basis over double is within its forward error bound "
          "on wide integer inputs") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  constexpr std::int64_t two_50 = test_support::power_of_two(50);
  constexpr std::int64_t two_53 = test_support::power_of_two(53);
  const double largest = std::max({
      check_wide<random_7, false, core::full<5>>(source, "random 7", two_50),
      check_wide<random_7, true, core::full<5>>(source, "random 7", two_50),
      check_wide<random_2, false, core::full<4>>(source, "random 2", two_53),
      check_wide<bc::conformal_tag<3>, false, core::full<5>>(
          source, "conformal(3)", two_53),
      check_wide<bc::conformal_tag<3>, true, core::full<5>>(
          source, "conformal(3)", two_53),
  });
  CHECK(largest > 0.0);
}
