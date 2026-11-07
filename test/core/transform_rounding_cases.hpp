#ifndef GALOOT_TEST_CORE_TRANSFORM_ROUNDING_CASES_HPP
#define GALOOT_TEST_CORE_TRANSFORM_ROUNDING_CASES_HPP

// Inputs of transform over double and float for the rounding check of what
// it drops, shared by the program that runs them in each checking mode
// (transform_rounding_assert_fail.cpp) and by the test that shows they are
// what that program's tests say (transform_rounding_test.cpp).
//
// The rounding check compares each coefficient d that transform drops with
// a tolerance at least gamma_K M, where K is the number of roundings on the
// coefficient's path, M the same computation on absolute values, and
// gamma_K = K u / (1 - K u) with u the unit roundoff, and below
// gamma_(K+2) M for inputs of ordinary size (Higham, Accuracy and Stability
// of Numerical Algorithms, 2nd ed., SIAM, 2002, Lemma 3.1).

#include <limits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace transform_rounding_cases {

namespace core = galoot::core;

struct r400 {
  static constexpr auto value = core::R<4, 0, 0>();
};
struct rga3 {
  static constexpr auto value = core::rigid<3>();
};

inline constexpr auto scalar_layout = core::blades<r400, "1">;
inline constexpr auto pair_layout = core::blades<r400, "1 e1234">;
inline constexpr auto even_layout =
    core::blades<r400, "1 e12 e13 e14 e23 e24 e34 e1234">;

template <class T>
using Scalar = core::mv<r400, scalar_layout, T>;
template <class T>
using Pair = core::mv<r400, pair_layout, T>;
template <class T>
using Even = core::mv<r400, even_layout, T>;

inline constexpr auto point_layout = core::blades<rga3, "e1 e2 e3 e4">;
inline constexpr auto motor_layout =
    core::blades<rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;

template <class T>
using Point = core::mv<rga3, point_layout, T>;
template <class T>
using Motor = core::mv<rga3, motor_layout, T>;

/// 2^k for k >= 0, exact in T while k is below its largest exponent.
template <class T>
[[nodiscard]] constexpr T power_of_two(int k) {
  T p{1};
  for (int i = 0; i < k; ++i) {
    p *= T{2};
  }
  return p;
}

/// 2^-k for k >= 0, exact in T down to its smallest subnormal number.
template <class T>
[[nodiscard]] constexpr T inverse_power_of_two(int k) {
  T p{1};
  for (int i = 0; i < k; ++i) {
    p /= T{2};
  }
  return p;
}

/// In R(4,0,0), 1 + e1234 times s, which is not a versor: with x = 1 on the
/// scalar, V ⟇ x ⟇ antireverse(V) = 2 s^2 + 2 s^2 e1234, since 1 ⟇ 1 = e1234
/// and 1 ⟇ e1234 = e1234 ⟇ 1 = 1 there, and transform keeps the scalar, so
/// it drops 2 s^2, far above any rounding of its four products.
template <class T>
[[nodiscard]] constexpr Pair<T> not_a_versor(T s) {
  return Pair<T>{s, s};
}

/// In R(4,0,0), the versor a (1 + e12 + e13 + e14 + e23 + e24 + e34 + e1234)
/// with a = 2^25, whose V ⟇ antireverse(V) is 8 a^2 e1234, perturbed by m on
/// e12 and by -m on e34. On x = 1 on the scalar, the part transform drops on
/// e1234 is a sum of eight products of the coefficients of complementary
/// blades, a^2 each but for (a + m)(a - m) = a^2 - m^2, with signs that
/// cancel for the versor: 2 m^2 against a magnitude of 8 a^2, with
/// K = 9 roundings.
template <class T>
[[nodiscard]] constexpr Even<T> perturbed_versor(T m) {
  const T a = power_of_two<T>(25);
  return Even<T>{a, a + m, a, a, a, a, a - m, a};
}

/// The perturbation m of the case whose dropped part is at least ten times
/// gamma_(K+2) M, so at least ten times any tolerance of the check.
inline constexpr double ten_times_perturbation = 16.0;

/// The perturbation m of the case whose dropped part is at most a tenth of
/// gamma_K M, so at most a tenth of any tolerance of the check.
inline constexpr double tenth_perturbation = 0.5;

/// In R(4,0,0), the versor c (1 + e12 + e34 + e1234), whose
/// V ⟇ antireverse(V) is 4 c^2 e1234, with c = 2^(E/2 - 1) for E the
/// largest binary exponent of T, so c^2 = 2^(E - 2). On x = 1.25 on the
/// scalar, each product of the second stage is 0 or 1.25 c^2 = 0.3125 2^E
/// in magnitude, below the largest number 2^E (1 - u), and no dropped
/// coefficient adds more than two of one sign, so each sums to zero without
/// overflow, while the magnitude of the four on e1234, 1.25 2^E, overflows.
template <class T>
[[nodiscard]] constexpr Even<T> near_overflow_versor() {
  const T c = power_of_two<T>((std::numeric_limits<T>::max_exponent / 2) - 1);
  return Even<T>{c, c, T{}, T{}, T{}, T{}, c, c};
}

inline constexpr double near_overflow_scalar = 1.25;

/// The binary exponents by which the underflow case scales V and x: the
/// first stage, V times x, stays normal, near 2^-700 for double and 2^-92
/// for float, and the second, those products times V, falls below the
/// smallest normal number, 2^-1022 and 2^-126, near 2^-1060 and 2^-140,
/// deep enough that a product of three inputs of nine significant bits,
/// with its lowest bit near 2^-1087 or 2^-167, rounds to the subnormal
/// numbers, spaced 2^-1074 and 2^-149.
template <class T>
inline constexpr int versor_underflow_exponent = 0;
template <>
inline constexpr int versor_underflow_exponent<double> = 360;
template <>
inline constexpr int versor_underflow_exponent<float> = 48;
template <class T>
inline constexpr int object_underflow_exponent = 0;
template <>
inline constexpr int object_underflow_exponent<double> = 340;
template <>
inline constexpr int object_underflow_exponent<float> = 44;

/// The rigid(3) motor and point of the underflow case, before scaling: the
/// dyadic coefficients below, with nine significant bits each, so that a
/// product of three needs up to 27 bits and rounds in float, and the
/// subnormal products of the second stage round in either type. What a
/// motor drops from a point is zero for every motor; with these
/// coefficients the rounded computation leaves one dropped coefficient
/// nonzero in each type, which few choices do.
template <class T>
[[nodiscard]] constexpr Motor<T> underflow_motor() {
  const T s = inverse_power_of_two<T>(9);
  return Motor<T>{T{-300} * s, T{-341} * s, T{420} * s, T{261} * s,
                  T{-436} * s, T{-486} * s, T{328} * s, T{-457} * s};
}

template <class T>
[[nodiscard]] constexpr Point<T> underflow_point() {
  const T s = inverse_power_of_two<T>(9);
  return Point<T>{T{-400} * s, T{346} * s, T{-402} * s, T{-289} * s};
}

/// The underflow case's motor and point, scaled by 2^-E for the exponents
/// above.
template <class T>
[[nodiscard]] constexpr Motor<T> tiny_motor() {
  Motor<T> v = underflow_motor<T>();
  const T scale = inverse_power_of_two<T>(versor_underflow_exponent<T>);
  for (T& c : v.coefficients) {
    c *= scale;
  }
  return v;
}

template <class T>
[[nodiscard]] constexpr Point<T> tiny_point() {
  Point<T> x = underflow_point<T>();
  const T scale = inverse_power_of_two<T>(object_underflow_exponent<T>);
  for (T& c : x.coefficients) {
    c *= scale;
  }
  return x;
}

} // namespace transform_rounding_cases

#endif // GALOOT_TEST_CORE_TRANSFORM_ROUNDING_CASES_HPP
