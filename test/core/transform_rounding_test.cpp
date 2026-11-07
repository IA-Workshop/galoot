// Tests of the rounding check of transform over double and float.
//
// transform(x, V) computes V ⟇ x ⟇ antireverse(V), with the sign of an odd
// V, in two stages, and keeps the blades of x. Over a scalar whose
// numeric_limits say is_iec559 and round_to_nearest, in a checking build
// (every constant evaluation, and run time with GALOOT_CHECKING 1), it also
// computes each coefficient d that it drops and checks |d| <= tol and
// tol <= the largest finite number, where tol bounds the rounding error of
// d's computation: for a versor the exact dropped part is zero, so d is all
// rounding error. In the standard model, fl(a op b) = (a op b)(1 + delta)
// with |delta| <= u, the unit roundoff, 2^-53 for double and 2^-24 for
// float, and d is then within gamma_K M of its exact value, where K is the
// number of roundings on its path, M the same computation on absolute
// values, and gamma_K = K u / (1 - K u) (Higham, Accuracy and Stability of
// Numerical Algorithms, 2nd ed., SIAM, 2002, Lemma 3.1 and equation (3.5)).
// The check computes its tolerance in the scalar itself, so it adds one
// rounding: tol is at least gamma_K M and, for inputs of ordinary size,
// below gamma_(K+2) M, each rounding of its computation raising it by at
// most a factor 1 + u. When products fall below the smallest normal number
// lambda, gradual underflow adds an absolute error of at most lambda u to
// each (Higham, equation (2.8)), which gamma_K M misses; the check covers it
// by adding lambda to each rounded product of M's computation.
//
// The oracle is test_support::tracked (support/tracked.hpp): the same
// kernels run over it give, for each dropped coefficient, its exact value,
// its magnitude M, and its count K, by the rules a product adds the counts
// and 1, a sum takes the larger count and adds 1, and a negation keeps the
// count. The comparisons of tolerances with gamma_K M are exact, made in
// integers of 128 bits, since tol, K, and M are known exactly and gamma_K
// is not a binary fraction.
//
// Inputs are exact in the scalar and in the test rational. Each term of
// the transform multiplies three inputs, x once and V twice, so inputs
// p / 2^27, as for a product of two, would give exact values with
// denominators up to 2^81, beyond the rational's std::int64_t. Each input
// is instead p / 2^b with |p| < 2^b and b = 18 for double and 9 for float,
// so that a product of three needs up to 54 and 27 bits and rounds, while
// the exact values keep denominators of at most 2^54 and magnitudes of a
// few units. Where the structure constants are larger than 1, in the form
// below that is not diagonal, x has three bits fewer, and where V is an
// antiproduct of hyperplanes, whose coefficients are sums with a few bits
// more, four fewer, so that the magnitudes stay inside the rational. The
// versors of conformal(2) and conformal(3) are antiproducts of one to three
// hyperplanes whose coefficients split the b bits among the factors, so
// that the antiproduct is exact in the scalar, checked against the tracked
// run. In rigid(3), what a motor or a flector drops from a point, a line,
// or a plane is zero for every motor or flector, so V there is any value of
// one parity; so it is in the form with Gram matrix [[2, 0, -3],
// [0, 1, -3], [-3, -3, -2]], of three generators, for a vector or a
// bivector, since a transform by a V of one parity keeps x's antigrades
// modulo 4, and a form of three generators has no two antigrades 4 apart.
// Each family reports the largest ratio of |d| to the tolerance and to
// gamma_K M that it saw.
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <limits>
#include <random>
#include <string>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include "transform_rounding_cases.hpp"
#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

namespace core = galoot::core;
namespace cases = transform_rounding_cases;
using test_support::rational;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga2 {
  static constexpr auto value = core::conformal<2>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};
// A form that is not diagonal and whose antiproducts have coefficients other
// than 1 and -1, so that a term multiplies by its coefficient and rounds
// once more.
struct random1 {
  static constexpr auto value = core::from_gram({
      {2, 0, -3},
      {0, 1, -3},
      {-3, -3, -2},
  });
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;
// The flector on the canonical blades of odd antigrade.
inline constexpr auto canonical_flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e123 e124 e134 e234">;

// ---------------------------------------------------------------------------
// Exact comparisons of a scalar value with gamma_k times a magnitude.

// An unsigned integer of up to 128 bits, as two halves.
struct wide {
  std::uint64_t high = 0;
  std::uint64_t low = 0;
};

// a b, exactly, from the four products of 32-bit halves.
wide product(std::uint64_t a, std::uint64_t b) {
  constexpr std::uint64_t half = 0xFFFF'FFFFU;
  const std::uint64_t a0 = a & half;
  const std::uint64_t a1 = a >> 32U;
  const std::uint64_t b0 = b & half;
  const std::uint64_t b1 = b >> 32U;
  const std::uint64_t p00 = a0 * b0;
  const std::uint64_t p01 = a0 * b1;
  const std::uint64_t p10 = a1 * b0;
  const std::uint64_t middle = (p00 >> 32U) + (p01 & half) + (p10 & half);
  return {
      .high = (a1 * b1) + (p01 >> 32U) + (p10 >> 32U) + (middle >> 32U),
      .low = (middle << 32U) | (p00 & half),
  };
}

// The number of bits of w, 0 for zero.
int width(wide w) {
  if (w.high != 0) {
    return 128 - std::countl_zero(w.high);
  }
  return 64 - std::countl_zero(w.low);
}

// w times 2^s, for 0 <= s with no bit shifted out.
wide shifted(wide w, int s) {
  if (s == 0) {
    return w;
  }
  const auto k = static_cast<unsigned>(s);
  if (k >= 64U) {
    return {.high = w.low << (k - 64U), .low = 0};
  }
  return {.high = (w.high << k) | (w.low >> (64U - k)), .low = w.low << k};
}

// The sign of a 2^p - b 2^q: -1, 0, or 1.
int compare(wide a, int p, wide b, int q) {
  const int wa = width(a);
  const int wb = width(b);
  if (wa == 0 || wb == 0) {
    if (wa == wb) {
      return 0;
    }
    return wa == 0 ? -1 : 1;
  }
  // Values whose leading bits differ in place compare by that place;
  // otherwise the one with the larger exponent is moved to the other's,
  // which keeps it within 128 bits since both lead at the same place.
  if (wa + p != wb + q) {
    return wa + p < wb + q ? -1 : 1;
  }
  if (p > q) {
    a = shifted(a, p - q);
  } else {
    b = shifted(b, q - p);
  }
  if (a.high != b.high) {
    return a.high < b.high ? -1 : 1;
  }
  if (a.low != b.low) {
    return a.low < b.low ? -1 : 1;
  }
  return 0;
}

// A nonnegative value as significand times 2^exponent.
struct scaled {
  std::uint64_t significand = 0;
  int exponent = 0;
};

// A finite nonnegative double, exactly; subnormal numbers too.
scaled scaled_of(double t) {
  if (t == 0.0) {
    return {};
  }
  int exponent = 0;
  const double fraction = std::frexp(t, &exponent);
  return {
      .significand = static_cast<std::uint64_t>(std::ldexp(fraction, 53)),
      .exponent = exponent - 53,
  };
}

// A nonnegative rational whose denominator is a power of two, times
// 2^shift.
scaled scaled_of(const rational& m, int shift) {
  return {
      .significand = static_cast<std::uint64_t>(m.numerator),
      .exponent =
          shift - std::countr_zero(static_cast<std::uint64_t>(m.denominator)),
  };
}

// The sign of f t - g gamma_k M, for t >= 0 finite, M = m 2^shift, small
// integers f, g > 0 and k > 0, and gamma_k = k u / (1 - k u) with
// u = 2^-precision: after multiplying by 2^precision (1 - k u) > 0, the sign
// of f t (2^precision - k) - g k M, in integers.
int compare_with_gamma(double t, std::uint64_t f, int k, int precision,
                       const rational& m, std::uint64_t g, int shift = 0) {
  const scaled ts = scaled_of(t);
  const scaled ms = scaled_of(m, shift);
  const auto kk = static_cast<std::uint64_t>(k);
  const std::uint64_t left_factor =
      f * ((std::uint64_t{1} << static_cast<unsigned>(precision)) - kk);
  return compare(product(ts.significand, left_factor), ts.exponent,
                 product(ms.significand, g * kk), ms.exponent);
}

// gamma_k M <= t, exactly.
bool gamma_at_most(int k, int precision, const rational& m, double t) {
  return compare_with_gamma(t, 1, k, precision, m, 1) >= 0;
}

// t < gamma_k M, exactly.
bool below_gamma(double t, int k, int precision, const rational& m) {
  return compare_with_gamma(t, 1, k, precision, m, 1) < 0;
}

// gamma_k for the unit roundoff 2^-precision, computed in double: k u and
// 1 - k u are exact, so it rounds once.
double gamma_in_double(int k, int precision) {
  const double ku = std::ldexp(static_cast<double>(k), -precision);
  return ku / (1.0 - ku);
}

// The largest value of |d| / (gamma_K M), computed in double, for d within
// gamma_K M: the computation rounds four times, forming gamma_K, converting
// M, their product, and the quotient, so it gives the exact ratio, at most
// 1, times at most 1 + gamma_4 < 1 + 5u with u = 2^-53, and the largest
// double at most that is 1 + 4u.
constexpr double bound_ratio_limit = 1.0 + 0x1p-51;

// ---------------------------------------------------------------------------
// Families of dropped coefficients.

// What a family of dropped coefficients showed.
struct tally {
  int coefficients = 0;
  int problems = 0;
  // Coefficients the rounded computation left nonzero, so that the check
  // had something to compare.
  int nonzero = 0;
  // Coefficients of exact magnitude zero, whose tolerance is not of ordinary
  // size and is not compared with gamma_(K+2) M.
  int zero_magnitude = 0;
  // Underflow: coefficients beyond gamma_K M, which only the underflow
  // term of the tolerance admits.
  int beyond_plain_bound = 0;
  double largest_to_tolerance = 0.0;
  double largest_to_bound = 0.0;
  std::string first_problem;

  void note(const std::string& what) {
    ++problems;
    if (first_problem.empty()) {
      first_problem = what;
    }
  }
};

std::string text(const rational& x) {
  return std::format("{}/{}", x.numerator, x.denominator);
}

template <class T>
std::uint64_t bits_of(T t) {
  if constexpr (std::same_as<T, float>) {
    return std::bit_cast<std::uint32_t>(t);
  } else {
    return std::bit_cast<std::uint64_t>(t);
  }
}

// x as a value of T, which must hold it exactly.
template <class T>
T exactly(const rational& x, tally& out) {
  const auto t = static_cast<T>(test_support::nearest_double(x));
  if (test_support::exact_rational(static_cast<double>(t)) != x) {
    out.note("the input " + text(x) + " is not exact in the scalar");
  }
  return t;
}

// One dropped coefficient: |d|, its tolerance, the core's count, the
// scalar's precision, and the binary exponent by which the inputs' scaling
// multiplies the magnitude, 0 for inputs of ordinary size.
struct dropped_coefficient {
  double d = 0.0;
  double tolerance = 0.0;
  int roundings = 0;
  int precision = 0;
  int shift = 0;
};

// Compares one dropped coefficient with tracked's run e of the same
// computation on the unscaled inputs, as check_dropped says.
void compare_dropped(const dropped_coefficient& c, const tracked& e,
                     const std::string& where, tally& out) {
  const bool ordinary = c.shift == 0;
  ++out.coefficients;
  out.nonzero += c.d != 0.0 ? 1 : 0;
  if (e.value != rational{}) {
    out.note(where + ": the exact dropped value is " + text(e.value));
  }
  if (c.roundings != e.roundings) {
    out.note(where + std::format(": the core counts {} roundings, the "
                                 "tracked run {}",
                                 c.roundings, e.roundings));
  }
  if (!(c.d <= c.tolerance)) {
    out.note(where + std::format(": |d| = {} exceeds the tolerance {}", c.d,
                                 c.tolerance));
  }
  if (c.d != 0.0) {
    out.largest_to_tolerance =
        std::max(out.largest_to_tolerance, c.d / c.tolerance);
  }
  const bool within_plain_bound =
      compare_with_gamma(c.d, 1, e.roundings, c.precision, e.magnitude, 1,
                         c.shift) <= 0;
  if (!ordinary) {
    out.beyond_plain_bound += within_plain_bound ? 0 : 1;
    return;
  }
  if (!within_plain_bound) {
    out.note(where + std::format(": |d| = {} exceeds gamma_K M", c.d));
  }
  if (!gamma_at_most(e.roundings, c.precision, e.magnitude, c.tolerance)) {
    out.note(where +
             std::format(": the tolerance {} is below gamma_{} M, M = ",
                         c.tolerance, e.roundings) +
             text(e.magnitude));
  }
  if (e.magnitude == rational{}) {
    ++out.zero_magnitude;
  } else if (!below_gamma(c.tolerance, e.roundings + 2, c.precision,
                          e.magnitude)) {
    out.note(where +
             std::format(": the tolerance {} is not below gamma_{} M, M = ",
                         c.tolerance, e.roundings + 2) +
             text(e.magnitude));
  }
  if (c.d != 0.0) {
    out.largest_to_bound =
        std::max(out.largest_to_bound,
                 c.d / (gamma_in_double(e.roundings, c.precision) *
                        test_support::nearest_double(e.magnitude)));
  }
}

// Compares every coefficient that transform drops from x on XL, for V on
// VL, over T, with its tolerance and with tracked's run of the same
// kernels. With exponents 0, the inputs are of ordinary size: the core's
// count must equal tracked's, and its tolerance lie in
// [gamma_K M, gamma_(K+2) M). With exponents ev and ex > 0, V is scaled by
// 2^-ev and x by 2^-ex, so that products underflow, and the tolerance must
// still hold d. In both, d must be within its tolerance and within the
// exact bound gamma_K M plus, for underflow, what the scaling shows, and
// transform(x, V), which runs the check in a checking build, must give the
// coefficients of the unchecked computation, bit for bit.
template <class T, class Alg, core::blade_set VL, core::blade_set XL>
void check_dropped(const std::array<rational, VL.size()>& v,
                   const std::array<rational, XL.size()>& x, int ev, int ex,
                   const std::string& what, tally& out) {
  using V = core::mv<Alg, VL, T>;
  using X = core::mv<Alg, XL, T>;
  constexpr int precision = std::numeric_limits<T>::digits;

  // The inputs over T, scaled, and over tracked, unscaled: magnitudes scale
  // exactly, by 2^-(2 ev + ex), since each term holds V twice and x once.
  V vd;
  X xd;
  std::array<tracked, VL.size()> vt{};
  std::array<tracked, XL.size()> xt{};
  for (std::size_t k = 0; k < VL.size(); ++k) {
    vd.coefficients.at(k) = std::ldexp(exactly<T>(v.at(k), out), -ev);
    vt.at(k) = tracked::input(v.at(k));
  }
  for (std::size_t k = 0; k < XL.size(); ++k) {
    xd.coefficients.at(k) = std::ldexp(exactly<T>(x.at(k), out), -ex);
    xt.at(k) = tracked::input(x.at(k));
  }
  const int shift = -((2 * ev) + ex);

  const auto exact = core::detail::transform_outer<Alg, VL, XL, false>(
      core::detail::transform_middle<Alg, VL, XL, true>(vt, xt), vt);
  const auto dropped = core::detail::transform_outer<Alg, VL, XL, false>(
      core::detail::transform_middle<Alg, VL, XL, true>(vd.coefficients,
                                                        xd.coefficients),
      vd.coefficients);
  constexpr const auto& roundings =
      core::detail::transform_dropped_roundings<Alg, VL, XL>;
  const auto tolerances =
      core::detail::transform_dropped_tolerances<Alg, VL, XL>(vd.coefficients,
                                                              xd.coefficients);
  static_assert(roundings.size() == dropped.size());
  static_assert(tolerances.size() == dropped.size());

  for (std::size_t j = 0; j < dropped.size(); ++j) {
    compare_dropped(
        {
            .d = std::abs(static_cast<double>(dropped.at(j))),
            .tolerance = static_cast<double>(tolerances.at(j)),
            .roundings = roundings.at(j),
            .precision = precision,
            .shift = shift,
        },
        exact.at(j), std::format("{}, dropped coefficient {}", what, j), out);
  }

  // transform itself, checked or not, keeps what the unchecked computation
  // keeps.
  const X kept = core::transform(xd, vd);
  const auto unchecked = core::detail::transform_outer<Alg, VL, XL, true>(
      core::detail::transform_middle<Alg, VL, XL, false>(vd.coefficients,
                                                         xd.coefficients),
      vd.coefficients);
  for (std::size_t k = 0; k < XL.size(); ++k) {
    if (bits_of(kept.coefficients.at(k)) != bits_of(unchecked.at(k))) {
      out.note(what + std::format(": transform keeps {} on coefficient {}, "
                                  "the unchecked computation {}",
                                  static_cast<double>(kept.coefficients.at(k)),
                                  k, static_cast<double>(unchecked.at(k))));
    }
  }
}

// The exact rational value of a double that is an integer or a binary
// fraction small enough for the rational: test_support::exact_rational
// accepts powers of two only up to 2^8, and the cases near 2^25 need more.
rational rational_of(double t) {
  int exponent = 0;
  const double fraction = std::frexp(t, &exponent);
  auto significand = static_cast<std::int64_t>(std::ldexp(fraction, 53));
  int power = exponent - 53;
  while (significand != 0 && significand % 2 == 0) {
    significand /= 2;
    ++power;
  }
  if (power >= 0) {
    return rational{significand * test_support::power_of_two(power)};
  }
  return rational{significand, test_support::power_of_two(-power)};
}

// The seed source, its seed logged on failure.
std::mt19937_64 seeded_source() {
  return std::mt19937_64(doctest::getContextOptions()->rand_seed);
}

// p / 2^bits with 0 < |p| < 2^limit.
rational draw_dyadic(std::mt19937_64& source, int bits, int limit) {
  std::uniform_int_distribution<std::int64_t> numerator(
      1, test_support::power_of_two(limit) - 1);
  std::bernoulli_distribution negative(0.5);
  const std::int64_t p = numerator(source);
  return rational{negative(source) ? -p : p, test_support::power_of_two(bits)};
}

template <std::size_t N>
std::array<rational, N> draw_values(std::mt19937_64& source, int bits,
                                    int limit) {
  std::array<rational, N> out{};
  for (rational& c : out) {
    c = draw_dyadic(source, bits, limit);
  }
  return out;
}

// The bits of each factor of the transform's inputs: three factors of 18
// bits round in double, and of 9 in float.
template <class T>
constexpr int factor_bits = std::same_as<T, float> ? 9 : 18;

// Underflow exponents of a family, or none.
struct scaling {
  int versor = 0;
  int object = 0;
};

// samples of V on VL and x on XL, each coefficient p / 2^b for a seeded p
// with |p| < 2^b, b the scalar's factor bits, but |p| < 2^(b - narrower)
// for x, so that a form whose coefficients are large keeps its magnitudes
// within the rational.
template <class T, class Alg, core::blade_set VL, core::blade_set XL>
void random_family(std::mt19937_64& source, int samples, scaling s,
                   const std::string& what, tally& out, int narrower = 0) {
  constexpr int b = factor_bits<T>;
  for (int i = 0; i < samples; ++i) {
    const auto v = draw_values<VL.size()>(source, b, b);
    const auto x = draw_values<XL.size()>(source, b, b - narrower);
    check_dropped<T, Alg, VL, XL>(v, x, s.versor, s.object, what, out);
  }
}

// The antiproduct of the hyperplanes h, in order.
template <class H, std::size_t Count>
auto antiproduct_of(const std::array<H, Count>& h) {
  static_assert(Count >= 1 && Count <= 3);
  if constexpr (Count == 1) {
    return h[0];
  } else if constexpr (Count == 2) {
    return h[0] % h[1];
  } else {
    return (h[0] % h[1]) % h[2];
  }
}

// samples of V, the antiproduct of Count seeded hyperplanes on HL, and x on
// XL. Each hyperplane coefficient is p / 2^h with |p| < 2^h, h the factor
// bits divided among the Count factors, so that V is exact in T, which the
// tracked run confirms, and V's coefficients have about the factor bits and
// a few more from the sums of the antiproduct; x has four bits fewer, so
// that a product of three still needs more bits than the scalar has while
// the magnitudes stay within the rational.
template <class T, class Alg, core::blade_set HL, std::size_t Count,
          core::blade_set XL>
void versor_family(std::mt19937_64& source, int samples, scaling s,
                   const std::string& what, tally& out) {
  constexpr int b = factor_bits<T>;
  constexpr int hb = b / static_cast<int>(Count);
  using H = core::mv<Alg, HL, T>;
  using HT = core::mv<Alg, HL, tracked>;
  for (int i = 0; i < samples; ++i) {
    std::array<H, Count> hd{};
    std::array<HT, Count> ht{};
    for (std::size_t c = 0; c < Count; ++c) {
      const auto h = draw_values<HL.size()>(source, hb, hb);
      for (std::size_t k = 0; k < HL.size(); ++k) {
        hd.at(c).coefficients.at(k) = exactly<T>(h.at(k), out);
        ht.at(c).coefficients.at(k) = tracked::input(h.at(k));
      }
    }
    const auto vd = antiproduct_of(hd);
    const auto vt = antiproduct_of(ht);
    using VD = std::remove_cvref_t<decltype(vd)>;
    std::array<rational, VD::component_count> v{};
    for (std::size_t k = 0; k < v.size(); ++k) {
      v.at(k) = vt.coefficients.at(k).value;
      if (test_support::exact_rational(
              static_cast<double>(vd.coefficients.at(k))) != v.at(k)) {
        out.note(what + ": the versor is not exact in the scalar");
      }
    }
    const auto x = draw_values<XL.size()>(source, b, b - 4);
    check_dropped<T, Alg, VD::layout, XL>(v, x, s.versor, s.object, what, out);
  }
}

// Every family over T, with the underflow exponents s.
template <class T>
void run_families(std::mt19937_64& source, int samples, scaling s, tally& out) {
  random_family<T, rga3, motor_layout, point_layout>(
      source, samples, s, "rigid(3) point, motor", out);
  random_family<T, rga3, motor_layout, line_layout>(
      source, samples, s, "rigid(3) line, motor", out);
  random_family<T, rga3, motor_layout, plane_layout>(
      source, samples, s, "rigid(3) plane, motor", out);
  random_family<T, rga3, flector_layout, point_layout>(
      source, samples, s, "rigid(3) point, flector", out);
  random_family<T, rga3, flector_layout, line_layout>(
      source, samples, s, "rigid(3) line, flector", out);
  random_family<T, rga3, flector_layout, plane_layout>(
      source, samples, s, "rigid(3) plane, flector", out);
  random_family<T, rga3, canonical_flector_layout, point_layout>(
      source, samples, s, "rigid(3) point, canonical flector", out);
  random_family<T, rga3, canonical_flector_layout, line_layout>(
      source, samples, s, "rigid(3) line, canonical flector", out);
  random_family<T, rga3, canonical_flector_layout, plane_layout>(
      source, samples, s, "rigid(3) plane, canonical flector", out);
  random_family<T, random1, core::blades<random1, "1 e12 e13 e23">,
                core::of_grade<3, 1>>(source, samples, s,
                                      "random form vector, even", out, 3);
  random_family<T, random1, core::blades<random1, "e1 e2 e3 e123">,
                core::of_grade<3, 2>>(source, samples, s,
                                      "random form bivector, odd", out, 3);
  versor_family<T, cga2, core::of_grade<4, 3>, 2, core::blades<cga2, "1">>(
      source, samples, s, "conformal(2) scalar, two hyperplanes", out);
  versor_family<T, cga2, core::of_grade<4, 3>, 3, core::of_grade<4, 1>>(
      source, samples, s, "conformal(2) vector, three hyperplanes", out);
  versor_family<T, cga3, core::of_grade<5, 4>, 2, core::of_grade<5, 1>>(
      source, samples, s, "conformal(3) vector, two hyperplanes", out);
  versor_family<T, cga3, core::of_grade<5, 4>, 1, core::of_grade<5, 1>>(
      source, samples, s, "conformal(3) vector, one hyperplane", out);
  versor_family<T, cga3, core::of_grade<5, 4>, 3, core::of_grade<5, 1>>(
      source, samples, s, "conformal(3) vector, three hyperplanes", out);
}

void report(const tally& t, const std::string& family) {
  MESSAGE(std::format("{}: {} dropped coefficients, {} nonzero, {} of "
                      "magnitude zero, largest |d| / tolerance {:.6f}, "
                      "largest |d| / (gamma_K M) {:.6f}",
                      family, t.coefficients, t.nonzero, t.zero_magnitude,
                      t.largest_to_tolerance, t.largest_to_bound));
}

void report_underflow(const tally& t, const std::string& family) {
  MESSAGE(std::format("{}: {} dropped coefficients, {} nonzero, {} beyond "
                      "gamma_K M, largest |d| / tolerance {:.6f}",
                      family, t.coefficients, t.nonzero, t.beyond_plain_bound,
                      t.largest_to_tolerance));
}

} // namespace

// Reference: proof: transform checks a rounding scalar's dropped part
//   against its forward error bound, where the dropped part of a versor is
//   zero, so that a computed dropped coefficient d is its rounding error,
//   within gamma_K M with gamma_K = K u / (1 - K u) (Higham, Lemma 3.1), the
//   count K following the rules that a product adds its operands' counts
//   and 1, a sum takes the larger count and adds 1, and a negation or a
//   coefficient of 1 or -1 adds nothing, and a tolerance computed in the
//   scalar with a margin of one rounding lies in [gamma_K M, gamma_(K+2) M)
//   for inputs of ordinary size, since each of its own roundings moves it
//   by a factor of at most 1 + u, and transform keeps what it keeps without
//   the check, bit for bit; independent value: K, M, and the exact dropped
//   value from test_support::tracked over the same kernels, for seeded exact
//   inputs in rigid(3), conformal(2), conformal(3), and a form that is not
//   diagonal
TEST_CASE("transform's tolerance of each dropped coefficient is between "
          "gamma_K M and gamma_(K+2) M") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally over_double;
  run_families<double>(source, 100, scaling(), over_double);
  report(over_double, "double");
  INFO("first problem over double: ", over_double.first_problem);
  CHECK(over_double.coefficients > 0);
  CHECK(over_double.nonzero > 0);
  CHECK(over_double.problems == 0);
  CHECK(over_double.largest_to_tolerance <= 1.0);
  CHECK(over_double.largest_to_bound <= bound_ratio_limit);

  tally over_float;
  run_families<float>(source, 100, scaling(), over_float);
  report(over_float, "float");
  INFO("first problem over float: ", over_float.first_problem);
  CHECK(over_float.coefficients > 0);
  CHECK(over_float.nonzero > 0);
  CHECK(over_float.problems == 0);
  CHECK(over_float.largest_to_tolerance <= 1.0);
  CHECK(over_float.largest_to_bound <= bound_ratio_limit);
}

// Reference: proof: with gradual underflow a product's computed value is
//   a b (1 + delta) + eta with |eta| <= lambda u, lambda the smallest normal
//   number, and a sum below lambda is exact (Higham, equation (2.8)), so a
//   bound that adds lambda to each rounded product of the magnitude covers
//   every eta, which later factors multiply as they multiply lambda;
//   independent value: K and M from test_support::tracked on the unscaled
//   inputs, scaled exactly by the powers of two, which show that some
//   dropped coefficients exceed gamma_K M, a bound that ignores underflow
TEST_CASE("transform's tolerance holds dropped coefficients whose products "
          "underflow") {
  INFO("seed: ", doctest::getContextOptions()->rand_seed);
  std::mt19937_64 source = seeded_source();
  tally over_double;
  run_families<double>(source, 40,
                       {
                           .versor = cases::versor_underflow_exponent<double>,
                           .object = cases::object_underflow_exponent<double>,
                       },
                       over_double);
  report_underflow(over_double, "double, underflow");
  INFO("first problem over double: ", over_double.first_problem);
  CHECK(over_double.problems == 0);
  CHECK(over_double.beyond_plain_bound > 0);
  CHECK(over_double.largest_to_tolerance <= 1.0);

  tally over_float;
  run_families<float>(source, 40,
                      {
                          .versor = cases::versor_underflow_exponent<float>,
                          .object = cases::object_underflow_exponent<float>,
                      },
                      over_float);
  report_underflow(over_float, "float, underflow");
  INFO("first problem over float: ", over_float.first_problem);
  CHECK(over_float.problems == 0);
  CHECK(over_float.beyond_plain_bound > 0);
  CHECK(over_float.largest_to_tolerance <= 1.0);
}

// Reference: independent value: by hand in R(4,0,0), where 1 ⟇ 1 = e1234
//   and 1 ⟇ e1234 = e1234 ⟇ 1 = 1, for x = 1 on the scalar and
//   V = 1 + e1234, the first stage gives V ⟇ x = e1234 + 1, each blade from
//   one product, and the second reaches the dropped e1234 from two
//   products, e1234 ⟇ e1234 and 1 ⟇ 1, added, so K is 1 + 0 + 1 for each
//   product and one more for the sum, 3, every padded magnitude of a
//   product is 1 plus the smallest normal number, which rounds to 1, so the
//   tolerance is 2 fl(gamma_4), with gamma_4 = 4u / (1 - 4u) rounded once,
//   since 4u and 1 - 4u are exact, and the dropped value is 2
TEST_CASE("the count and the tolerance of a transform in R(4,0,0) are those "
          "computed by hand") {
  using cases::pair_layout;
  using cases::r400;
  using cases::scalar_layout;
  static_assert(
      core::detail::dropped_blade_count<r400, pair_layout, scalar_layout>() ==
      1);
  static_assert(core::detail::transform_dropped_roundings<r400, pair_layout,
                                                          scalar_layout> ==
                std::array<int, 1>{3});

  constexpr double u = 0x1p-53;
  constexpr double gamma_4 = (4 * u) / (1 - (4 * u));
  constexpr auto tol =
      core::detail::transform_dropped_tolerances<r400, pair_layout,
                                                 scalar_layout>(
          std::array<double, 2>{1.0, 1.0}, std::array<double, 1>{1.0});
  static_assert(tol[0] == 2 * gamma_4);
  CHECK(tol[0] == 2 * gamma_4);

  constexpr float uf = 0x1p-24F;
  constexpr float gamma_4f = (4 * uf) / (1 - (4 * uf));
  constexpr auto tolf =
      core::detail::transform_dropped_tolerances<r400, pair_layout,
                                                 scalar_layout>(
          std::array<float, 2>{1.0F, 1.0F}, std::array<float, 1>{1.0F});
  static_assert(tolf[0] == 2 * gamma_4f);
  CHECK(tolf[0] == 2 * gamma_4f);

  const auto dropped = core::detail::transform_outer<r400, pair_layout,
                                                     scalar_layout, false>(
      core::detail::transform_middle<r400, pair_layout, scalar_layout, true>(
          cases::not_a_versor(1.0).coefficients, std::array<double, 1>{1.0}),
      cases::not_a_versor(1.0).coefficients);
  CHECK(dropped[0] == 2.0);
  CHECK(dropped[0] > tol[0]);
}

// Reference: contract: the inputs of the program that runs transform in
//   each checking mode are what its tests say, which are a perturbed versor
//   whose dropped part is at least ten times gamma_(K+2) M, so above any
//   tolerance of the check, and another whose dropped part is at most a
//   tenth of gamma_K M, so below, the unperturbed one a versor, with
//   V ⟇ antireverse(V) a nonzero multiple of e1234, a versor near the
//   overflow threshold whose dropped coefficients are finite while a
//   tolerance is infinite, a NaN and an infinite input that reach the
//   dropped coefficient, and an underflowing motor and point that leave a
//   dropped coefficient nonzero and beyond gamma_K M; independent value: K,
//   M, and the exact values from test_support::tracked, compared exactly
TEST_CASE("the inputs of the checking program are as its tests say") {
  using cases::even_layout;
  using cases::r400;
  using cases::scalar_layout;
  using Even = cases::Even<tracked>;

  // The perturbed versors, from the same coefficients over tracked: the
  // exact value, count, and magnitude of the part on e1234, the last of the
  // dropped blades, since the second plan's result is in order of grade,
  // and its computed value. The other dropped coefficients, on the
  // bivectors, are zero.
  struct dropped_part {
    tracked exact;
    double computed = 0.0;
  };
  const auto perturbed = [](double m) {
    CAPTURE(m);
    const cases::Even<double> vd = cases::perturbed_versor(m);
    Even vt;
    for (std::size_t k = 0; k < even_layout.size(); ++k) {
      vt.coefficients.at(k) =
          tracked::input(rational_of(vd.coefficients.at(k)));
    }
    const std::array<tracked, 1> xt{tracked::input(rational{1})};
    const auto exact =
        core::detail::transform_outer<r400, even_layout, scalar_layout, false>(
            core::detail::transform_middle<r400, even_layout, scalar_layout,
                                           true>(vt.coefficients, xt),
            vt.coefficients);
    const auto dropped = core::detail::transform_outer<r400, even_layout,
                                                       scalar_layout, false>(
        core::detail::transform_middle<r400, even_layout, scalar_layout, true>(
            vd.coefficients, std::array<double, 1>{1.0}),
        vd.coefficients);
    for (std::size_t j = 0; j + 1 < exact.size(); ++j) {
      CHECK(exact.at(j).value == rational{});
    }
    return dropped_part{
        .exact = exact.back(),
        .computed = std::abs(dropped.back()),
    };
  };
  CHECK(perturbed(0.0).exact.value == rational{});
  // V ⟇ antireverse(V) = lambda e1234 with lambda the sum of the squares of
  // the coefficients, exactly over std::int64_t, for the unperturbed versor
  // and for the pattern of the versor near overflow.
  const auto antiscalar_multiple = [](const cases::Even<std::int64_t>& v) {
    const auto square = core::geometric_antiproduct(v, core::antireverse(v));
    std::int64_t on_antiscalar = 0;
    bool elsewhere = false;
    for (std::size_t k = 0; k < square.coefficients.size(); ++k) {
      if (decltype(square)::layout.blades.at(k).mask == 0b1111U) {
        on_antiscalar = square.coefficients.at(k);
      } else {
        elsewhere = elsewhere || square.coefficients.at(k) != 0;
      }
    }
    CHECK(!elsewhere);
    return on_antiscalar;
  };
  CHECK(antiscalar_multiple(cases::perturbed_versor(std::int64_t{0})) ==
        8 * test_support::power_of_two(50));
  CHECK(antiscalar_multiple(
            cases::Even<std::int64_t>{1, 1, 0, 0, 0, 0, 1, 1}) == 4);
  const dropped_part ten_times = perturbed(cases::ten_times_perturbation);
  CAPTURE(ten_times.computed);
  CAPTURE(ten_times.exact.roundings);
  CHECK(compare_with_gamma(ten_times.computed, 1, ten_times.exact.roundings + 2,
                           53, ten_times.exact.magnitude, 10) >= 0);
  const dropped_part tenth = perturbed(cases::tenth_perturbation);
  CAPTURE(tenth.computed);
  CAPTURE(tenth.exact.roundings);
  CHECK(tenth.computed > 0.0);
  CHECK(compare_with_gamma(tenth.computed, 10, tenth.exact.roundings, 53,
                           tenth.exact.magnitude, 1) <= 0);

  // Near overflow: every dropped coefficient zero, so within its tolerance,
  // and one tolerance infinite, so that only the test of the tolerance
  // itself rejects it.
  {
    const cases::Even<double> v = cases::near_overflow_versor<double>();
    const std::array<double, 1> x{cases::near_overflow_scalar};
    const auto dropped =
        core::detail::transform_outer<r400, even_layout, scalar_layout, false>(
            core::detail::transform_middle<r400, even_layout, scalar_layout,
                                           true>(v.coefficients, x),
            v.coefficients);
    const auto tolerances = core::detail::transform_dropped_tolerances<
        r400, even_layout, scalar_layout>(v.coefficients, x);
    bool some_infinite = false;
    for (std::size_t j = 0; j < dropped.size(); ++j) {
      CAPTURE(j);
      CHECK(dropped.at(j) == 0.0);
      CHECK(std::abs(dropped.at(j)) <= tolerances.at(j));
      some_infinite = some_infinite || std::isinf(tolerances.at(j));
    }
    CHECK(some_infinite);
  }

  // A NaN or an infinite coefficient of V reaches the dropped coefficient.
  for (const double bad : {
           std::numeric_limits<double>::quiet_NaN(),
           std::numeric_limits<double>::infinity(),
       }) {
    CAPTURE(bad);
    const auto dropped = core::detail::transform_outer<r400, cases::pair_layout,
                                                       scalar_layout, false>(
        core::detail::transform_middle<r400, cases::pair_layout, scalar_layout,
                                       true>(std::array<double, 2>{1.0, bad},
                                             std::array<double, 1>{1.0}),
        std::array<double, 2>{1.0, bad});
    CHECK(!std::isfinite(dropped[0]));
  }

  // Underflow: the motor and the point, each scaled, leave a dropped
  // coefficient nonzero and beyond gamma_K M, M scaled exactly from the
  // unscaled tracked run; and within the tolerance.
  const auto check_underflow = []<class T>(T /*scalar*/) {
    using V = cases::Motor<T>;
    using X = cases::Point<T>;
    const V v = cases::tiny_motor<T>();
    const X x = cases::tiny_point<T>();
    std::array<tracked, V::component_count> vt{};
    std::array<tracked, X::component_count> xt{};
    for (std::size_t k = 0; k < vt.size(); ++k) {
      vt.at(k) = tracked::input(test_support::exact_rational(
          static_cast<double>(cases::underflow_motor<T>().coefficients.at(k))));
    }
    for (std::size_t k = 0; k < xt.size(); ++k) {
      xt.at(k) = tracked::input(test_support::exact_rational(
          static_cast<double>(cases::underflow_point<T>().coefficients.at(k))));
    }
    constexpr auto vl = cases::motor_layout;
    constexpr auto xl = cases::point_layout;
    using A = cases::rga3;
    const auto exact = core::detail::transform_outer<A, vl, xl, false>(
        core::detail::transform_middle<A, vl, xl, true>(vt, xt), vt);
    const auto dropped = core::detail::transform_outer<A, vl, xl, false>(
        core::detail::transform_middle<A, vl, xl, true>(v.coefficients,
                                                        x.coefficients),
        v.coefficients);
    const auto tolerances =
        core::detail::transform_dropped_tolerances<A, vl, xl>(v.coefficients,
                                                              x.coefficients);
    const int shift = -((2 * cases::versor_underflow_exponent<
                                 T>)+cases::object_underflow_exponent<T>);
    bool beyond = false;
    for (std::size_t j = 0; j < dropped.size(); ++j) {
      const double d = std::abs(static_cast<double>(dropped.at(j)));
      CHECK(exact.at(j).value == rational{});
      CHECK(d <= static_cast<double>(tolerances.at(j)));
      beyond =
          beyond || compare_with_gamma(d, 1, exact.at(j).roundings,
                                       std::numeric_limits<T>::digits,
                                       exact.at(j).magnitude, 1, shift) > 0;
    }
    CHECK(beyond);
  };
  check_underflow(0.0);
  check_underflow(0.0F);
}

// Reference: proof: a constant evaluation always runs the check, and the
//   tolerance holds the dropped coefficients of a motor applied to a point,
//   whose exact dropped part is zero for every motor, both for coefficients
//   of ordinary size and for coefficients whose second-stage products
//   underflow, and the values equal those of the same transform at run
//   time
TEST_CASE("transform passes its rounding check in a constant evaluation") {
  constexpr cases::Point<double> ordinary = core::transform(
      cases::underflow_point<double>(), cases::underflow_motor<double>());
  constexpr cases::Point<double> tiny =
      core::transform(cases::tiny_point<double>(), cases::tiny_motor<double>());
  constexpr cases::Point<float> ordinary_float = core::transform(
      cases::underflow_point<float>(), cases::underflow_motor<float>());
  constexpr cases::Point<float> tiny_float =
      core::transform(cases::tiny_point<float>(), cases::tiny_motor<float>());
  CHECK(ordinary == core::transform(cases::underflow_point<double>(),
                                    cases::underflow_motor<double>()));
  CHECK(tiny == core::transform(cases::tiny_point<double>(),
                                cases::tiny_motor<double>()));
  CHECK(ordinary_float == core::transform(cases::underflow_point<float>(),
                                          cases::underflow_motor<float>()));
  CHECK(tiny_float == core::transform(cases::tiny_point<float>(),
                                      cases::tiny_motor<float>()));
}

// Reference: contract: the rounding check covers a ring scalar whose
//   numeric_limits say is_iec559 and round_style round_to_nearest, and no
//   other: float and double, and not an integer, an exact rational, a
//   scalar without numeric_limits, or a complex number
TEST_CASE("the rounding check covers the IEEE 754 binary types") {
  using core::detail::rounding_checked_scalar;
  static_assert(rounding_checked_scalar<double>);
  static_assert(rounding_checked_scalar<float>);
  static_assert(!rounding_checked_scalar<std::int64_t>);
  static_assert(!rounding_checked_scalar<rational>);
  static_assert(!rounding_checked_scalar<tracked>);
  static_assert(!rounding_checked_scalar<std::complex<double>>);
  CHECK(rounding_checked_scalar<double>);
  CHECK(!rounding_checked_scalar<tracked>);
}
