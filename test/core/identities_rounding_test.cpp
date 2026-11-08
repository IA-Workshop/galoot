// Identities over double: associativity of the geometric product and of the
// geometric antiproduct, the De Morgan laws, and the composition of
// transforms, in rigid(3) and conformal(3), each side within its forward
// error bound and the two sides within the sum of their bounds.
//
// The two sides L and R of an identity are computed over double and again
// over test_support::tracked, on the same inputs, which are exact in both.
// The tracked run gives, for each coefficient, the exact value e, the
// magnitude M of the same computation on absolute values, and the count of
// roundings K (support/tracked.hpp), so that the double result d is within
// gamma_K M of e. For every blade of either side's layout, a blade a side
// lacks holding 0 with K = 0 and M = 0, the test requires:
//   e_L = e_R exactly, since the identity holds in exact arithmetic;
//   each side within its own bound of e, by within_rounding_bound, which
//     also catches an error common to both sides;
//   the two doubles equal when K_L <= 1 and K_R <= 1, as each is then the
//     double nearest e, and otherwise |L - R| within the sum of the two
//     bounds, computed with one rounding of margin a side, by
//     within_identity_bound.
// The tolerances follow from the counts alone, so they are fixed before any
// double result is seen, and none is a literal.
//
// Inputs are seeded random dyadic rationals p / 2^b, |p| < 2^b, sized so
// that double must round and the tracked rationals stay within
// std::int64_t, with one to three bits to spare, so that another seed may
// stop the test with an overflow of the oracle, never a silent pass:
//   b = 18 for associativity in rigid(3), b = 17 in conformal(3);
//   b = 27 for the De Morgan laws;
//   b = 7 for the rigid(3) motors and flectors and b = 26 for what they
//     move;
//   in conformal(3), versors h1 ⟇ h2 of two hyperplanes with integer
//     coefficients from -2 to 2, exact in double, so that transform's own
//     check of its dropped part never trips on a versor carrying rounding,
//     and b = 42 for what they move.
// The test build compiles without contraction of multiply-add, and the
// bounds would hold with it.
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <random>
#include <string>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace {

namespace core = galoot::core;
using test_support::rational;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto line_layout =
    core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
inline constexpr auto plane_layout = core::blades<rga3, "e423 e431 e412 e321">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
inline constexpr auto flector_layout =
    core::blades<rga3, "e1 e2 e3 e4 e423 e431 e412 e321">;

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// The name of the canonical blade of a mask: e followed by the indices of
// its generators, or 1.
std::string blade_name(core::mask_t mask) {
  if (mask == 0) {
    return "1";
  }
  std::string name = "e";
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// The coefficient of x on the canonical blade of this mask, or zero, with
// K = 0 and M = 0 for a tracked one, when the layout lacks the blade.
template <class X>
X::scalar_type value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign < 0 ? -x.coefficients.at(k) : x.coefficients.at(k);
    }
  }
  return typename X::scalar_type{};
}

// p / 2^bits with p uniform in -(2^bits - 1) to 2^bits - 1, exact in double.
rational draw(std::mt19937_64& source, int bits) {
  const std::int64_t bound = test_support::power_of_two(bits);
  std::uniform_int_distribution<std::int64_t> p(-(bound - 1), bound - 1);
  return rational{p(source), bound};
}

// A tracked multivector of X's layout whose coefficients are inputs drawn
// with this many bits.
template <class X>
X random_input(std::mt19937_64& source, int bits) {
  X x{};
  for (auto& c : x.coefficients) {
    c = tracked::input(draw(source, bits));
  }
  return x;
}

// The tracked multivector x made an input: the same exact values, with
// K = 0.
template <class X>
X as_input(const X& x) {
  X y{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    y.coefficients.at(k) = tracked::input(x.coefficients.at(k).value);
  }
  return y;
}

// The multivector over double on the layout of x with x's exact values,
// which are doubles.
template <class X>
auto doubles_of(const X& x) {
  core::mv<typename X::algebra_type, X::layout, double> d{};
  for (std::size_t k = 0; k < X::component_count; ++k) {
    d.coefficients.at(k) =
        test_support::nearest_double(x.coefficients.at(k).value);
  }
  return d;
}

// What the comparisons found, over every coefficient of every identity.
struct tally {
  int coefficients = 0;
  int problems = 0;
  int equal_sides = 0;
  // The largest ratio of |L - R| to the sum of the two bounds
  // gamma_(K_L) M_L + gamma_(K_R) M_R, computed in double.
  double largest_ratio = 0.0;
};

double gamma_of(int k) {
  const double ku = k * test_support::unit_roundoff;
  return ku / (1.0 - ku);
}

// Compares the sides of one identity, blade by blade over both layouts:
// tl and tr over tracked, dl and dr the same over double.
template <class TL, class TR, class DL, class DR>
void compare(const std::string& identity, const TL& tl, const TR& tr,
             const DL& dl, const DR& dr, tally& out) {
  const auto at = [&](core::mask_t mask) {
    const tracked l = value_on(tl, mask);
    const tracked r = value_on(tr, mask);
    const double ld = value_on(dl, mask);
    const double rd = value_on(dr, mask);
    ++out.coefficients;
    const bool exact_equal = l.value == r.value;
    const bool left_within = test_support::within_rounding_bound(
        ld, l.value, l.roundings, l.magnitude);
    const bool right_within = test_support::within_rounding_bound(
        rd, r.value, r.roundings, r.magnitude);
    const bool sides_within = test_support::within_identity_bound(
        ld, l.roundings, l.magnitude, rd, r.roundings, r.magnitude);
    if (!(exact_equal && left_within && right_within && sides_within)) {
      ++out.problems;
      const std::string problem = std::format(
          "{} on {}: exact sides equal {}, left within {}, right within {}, "
          "sides within {}; L = {} with K = {}, R = {} with K = {}",
          identity, blade_name(mask), exact_equal, left_within, right_within,
          sides_within, ld, l.roundings, rd, r.roundings);
      FAIL_CHECK(problem);
    }
    if (l.roundings <= 1 && r.roundings <= 1) {
      ++out.equal_sides;
    } else if (ld != rd) {
      const double bound =
          (gamma_of(l.roundings) * test_support::nearest_double(l.magnitude)) +
          (gamma_of(r.roundings) * test_support::nearest_double(r.magnitude));
      out.largest_ratio =
          std::max(out.largest_ratio, std::abs(ld - rd) / bound);
    }
  };
  for (const core::oriented_blade& blade : TL::layout.blades) {
    at(blade.mask);
  }
  for (const core::oriented_blade& blade : TR::layout.blades) {
    bool in_left = false;
    for (const core::oriented_blade& other : TL::layout.blades) {
      in_left = in_left || other.mask == blade.mask;
    }
    if (!in_left) {
      at(blade.mask);
    }
  }
}

// Associativity of ⟑ and ⟇ on random full multivectors of Alg.
template <class Alg>
void check_associativity(std::mt19937_64& source, int samples, int bits,
                         const std::string& form, tally& out) {
  using X = core::mv<Alg, core::full<generators<Alg>>, tracked>;
  for (int k = 0; k < samples; ++k) {
    const X a = random_input<X>(source, bits);
    const X b = random_input<X>(source, bits);
    const X c = random_input<X>(source, bits);
    const auto da = doubles_of(a);
    const auto db = doubles_of(b);
    const auto dc = doubles_of(c);
    compare(form + " (a ⟑ b) ⟑ c = a ⟑ (b ⟑ c)", (a * b) * c, a * (b * c),
            (da * db) * dc, da * (db * dc), out);
    compare(form + " (a ⟇ b) ⟇ c = a ⟇ (b ⟇ c)", (a % b) % c, a % (b % c),
            (da % db) % dc, da % (db % dc), out);
  }
}

// The four De Morgan laws and their converses on random full multivectors
// of Alg.
template <class Alg>
void check_de_morgan(std::mt19937_64& source, int samples,
                     const std::string& form, tally& out) {
  using core::left_complement;
  using core::right_complement;
  using X = core::mv<Alg, core::full<generators<Alg>>, tracked>;
  for (int k = 0; k < samples; ++k) {
    const X a = random_input<X>(source, 27);
    const X b = random_input<X>(source, 27);
    const auto da = doubles_of(a);
    const auto db = doubles_of(b);
    compare(form + " right(a ⟑ b)", right_complement(a * b),
            right_complement(a) % right_complement(b),
            right_complement(da * db),
            right_complement(da) % right_complement(db), out);
    compare(form + " right(a ^ b)", right_complement(a ^ b),
            right_complement(a) & right_complement(b),
            right_complement(da ^ db),
            right_complement(da) & right_complement(db), out);
    compare(form + " left(a ⟑ b)", left_complement(a * b),
            left_complement(a) % left_complement(b), left_complement(da * db),
            left_complement(da) % left_complement(db), out);
    compare(form + " left(a ^ b)", left_complement(a ^ b),
            left_complement(a) & left_complement(b), left_complement(da ^ db),
            left_complement(da) & left_complement(db), out);
    compare(form + " right(a ⟇ b)", right_complement(a % b),
            right_complement(a) * right_complement(b),
            right_complement(da % db),
            right_complement(da) * right_complement(db), out);
    compare(form + " right(a v b)", right_complement(a & b),
            right_complement(a) ^ right_complement(b),
            right_complement(da & db),
            right_complement(da) ^ right_complement(db), out);
    compare(form + " left(a ⟇ b)", left_complement(a % b),
            left_complement(a) * left_complement(b), left_complement(da % db),
            left_complement(da) * left_complement(db), out);
    compare(form + " left(a v b)", left_complement(a & b),
            left_complement(a) ^ left_complement(b), left_complement(da & db),
            left_complement(da) ^ left_complement(db), out);
  }
}

// transform(x, compose(q2, q1)) = transform(transform(x, q1), q2) for
// tracked q2, q1, and x, and the same over double.
template <class Q2, class Q1, class X>
void compare_composition(const std::string& identity, const Q2& q2,
                         const Q1& q1, const X& x, tally& out) {
  const auto d2 = doubles_of(q2);
  const auto d1 = doubles_of(q1);
  const auto dx = doubles_of(x);
  compare(identity, core::transform(x, core::compose(q2, q1)),
          core::transform(core::transform(x, q1), q2),
          core::transform(dx, core::compose(d2, d1)),
          core::transform(core::transform(dx, d1), d2), out);
}

// Composition in rigid(3) on random versors of the layouts Q2 and Q1 and
// random x of X. Every motor and flector layout of rigid(3) drops only
// parts that are zero as polynomials, so any coefficients are versors for
// transform's check.
template <auto Q2L, auto Q1L, auto XL>
void check_rigid_composition(std::mt19937_64& source, int samples,
                             const std::string& identity, tally& out) {
  using Q2 = core::mv<rga3, Q2L, tracked>;
  using Q1 = core::mv<rga3, Q1L, tracked>;
  using X = core::mv<rga3, XL, tracked>;
  for (int k = 0; k < samples; ++k) {
    const Q2 q2 = random_input<Q2>(source, 7);
    const Q1 q1 = random_input<Q1>(source, 7);
    const X x = random_input<X>(source, 26);
    compare_composition("rigid(3) " + identity, q2, q1, x, out);
  }
}

// Composition in conformal(3) of motors h1 ⟇ h2 of two hyperplanes with
// integer coefficients, made inputs, on random x of X.
template <auto XL>
void check_conformal_composition(std::mt19937_64& source, int samples,
                                 const std::string& identity, tally& out) {
  using H = core::mv<cga3, core::of_grade<5, 4>, tracked>;
  using X = core::mv<cga3, XL, tracked>;
  std::uniform_int_distribution<std::int64_t> digit(-2, 2);
  const auto hyperplane = [&] {
    H h{};
    for (auto& c : h.coefficients) {
      c = tracked::input(rational{digit(source)});
    }
    return h;
  };
  for (int k = 0; k < samples; ++k) {
    const H h1 = hyperplane();
    const H h2 = hyperplane();
    const H h3 = hyperplane();
    const H h4 = hyperplane();
    const auto q1 = as_input(h1 % h2);
    const auto q2 = as_input(h3 % h4);
    const X x = random_input<X>(source, 42);
    compare_composition("conformal(3) " + identity, q2, q1, x, out);
  }
}

} // namespace

// Reference: identity: the geometric product and the geometric
//   antiproduct are associative, the De Morgan laws right(a ⟑ b) =
//   right(a) ⟇ right(b) and right(a ^ b) = right(a) v right(b) hold with
//   left likewise and with their converses, and compose(Q2, Q1) applies Q1
//   first, as transform(x, compose(Q2, Q1)) = transform(transform(x, Q1), Q2),
//   in rigid(3) and conformal(3), each side in exact arithmetic from the
//   tracked run and both equal there, and each side over double within its
//   forward error bound gamma_K M from the tracked counts (Higham, Accuracy
//   and Stability of Numerical Algorithms, 2nd ed., Lemma 3.1), and the two
//   doubles equal when each side rounds at most once and otherwise within
//   the sum of the two bounds, with one rounding of margin a side for the
//   check's own arithmetic, on seeded random inputs exact in double
TEST_CASE("the identities hold over double within their forward error "
          "bounds") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  tally out;

  check_associativity<rga3>(source, 10, 18, "rigid(3)", out);
  check_associativity<cga3>(source, 4, 17, "conformal(3)", out);
  check_de_morgan<rga3>(source, 10, "rigid(3)", out);
  check_de_morgan<cga3>(source, 4, "conformal(3)", out);
  check_rigid_composition<motor_layout, motor_layout, point_layout>(
      source, 10, "motor and motor on a point", out);
  check_rigid_composition<motor_layout, motor_layout, line_layout>(
      source, 10, "motor and motor on a line", out);
  check_rigid_composition<motor_layout, motor_layout, plane_layout>(
      source, 10, "motor and motor on a plane", out);
  check_rigid_composition<flector_layout, flector_layout, point_layout>(
      source, 10, "flector and flector on a point", out);
  check_rigid_composition<motor_layout, flector_layout, point_layout>(
      source, 10, "motor and flector on a point", out);
  check_rigid_composition<flector_layout, motor_layout, line_layout>(
      source, 10, "flector and motor on a line", out);
  check_conformal_composition<core::of_grade<5, 1>>(source, 10, "on a vector",
                                                    out);
  check_conformal_composition<core::of_grade<5, 2>>(source, 10, "on a bivector",
                                                    out);

  MESSAGE(std::format("{} coefficients, {} of them with at most one rounding "
                      "a side; largest ratio of |L - R| to the sum of the "
                      "bounds {}",
                      out.coefficients, out.equal_sides, out.largest_ratio));
  CHECK(out.coefficients > 0);
  CHECK(out.problems == 0);
}
