#ifndef GALOOT_PRESETS_RGA3_MOTIONS_HPP
#define GALOOT_PRESETS_RGA3_MOTIONS_HPP

// The motions of rigid(3) built from whole angles and distances: a rotation
// about a line and a translation by a vector. A motor's coefficients hold
// half of each, since the rotor l sin(phi) + e1234 cos(phi) turns by 2 phi
// about the unitized line l and the translator tx e23 + ty e31 + tz e12 +
// e1234 moves points by 2t; these functions take theta and d and store
// theta/2 and d/2, and rotation unitizes the line it is given:
//
//   namespace rga3 = galoot::presets::rga3;
//   rga3::Line<double> z_axis{0, 0, 1, 0, 0, 0};
//   auto turn = rotation(z_axis, 1.5707963267948966);  // a quarter turn
//   auto move = rga3::translation(1.0, 2.0, 3.0);
//   auto both = galoot::core::compose(move, turn);      // turn, then move
//
// rotation is found from any namespace by argument-dependent lookup, through
// its Line argument. translation takes plain scalars, which name no
// namespace of the model, so it must be called as
// galoot::presets::rga3::translation.

#include <cmath>
#include <concepts>
#include <type_traits>

#include <galoot/core.hpp> // IWYU pragma: export
#include <galoot/core/assert.hpp>
#include <galoot/presets/rga3/objects.hpp>

namespace galoot::presets::rga3 {

/// The rotor that turns by the angle theta, in radians, about the line l:
/// (l / |v|) sin(theta/2) + e1234 cos(theta/2), with v the direction of l
/// (its coefficients on e41, e42, e43) and the half angle in the
/// coefficients. Dividing all six coefficients of l by |v| unitizes the
/// line, so the result is a unitized motor, up to rounding, for a direction
/// of any positive length. So the join of two points needs no division by
/// its length; it has a layout of its own, the wedge's, and is passed
/// converted, as Line{p ^ q}. The turn is counterclockwise when seen from
/// the tip of l's direction, so a positive theta about (0, 0, 1) takes the x
/// axis toward the y axis. theta has the line's scalar type exactly: 0.5
/// over double, 0.5f over float.
///
/// Cost: besides std::sin, std::cos, and halving theta, one std::sqrt, one
/// division, by the length, nine multiplications, and two additions; with
/// GALOOT_CHECKING 1, also the std::isnormal test of the length and a branch.
///
/// Requires: T is a floating type, since the coefficients need std::sin,
/// std::cos, and std::sqrt. l must be a line, its direction and moment
/// orthogonal (v . m = 0, as every join of two points is); this is not
/// checked, and about an l that is not a line the result distorts what it
/// moves. The length of the direction must lie between about 1e-150 and
/// 1e150 in double, or 1e-18 and 1e18 in float, so that the sum of its
/// squares neither underflows nor overflows. A computed length that is zero
/// or not finite, as for the zero direction of p ^ p or one whose squares
/// underflow to zero or overflow, is checked by GALOOT_ASSERT at run time
/// when GALOOT_CHECKING is 1, which then aborts with the message. With
/// GALOOT_CHECKING 0 the length is not tested, and an optimizing build drops
/// the check; without optimization a call and a branch remain. A zero
/// direction then gives infinite or NaN coefficients on the line's blades,
/// and one whose squares overflow a motor holding only cos(theta/2) on
/// e1234. These are not failure values, and rotation returns a plain Motor
/// rather than a std::optional on purpose: a zero direction is a misuse for
/// the check to report, not an outcome for the caller to handle, and an
/// optional result would change the return type of this function, while no
/// other function of the library returns one. A length whose squares fall
/// among the subnormal numbers passes the check and loses accuracy; no test
/// covers that case.
///
/// Rounding: when the computed length is a power of 2, as for (0, 0, 1) or
/// (0, 0, 2), the result is bit for bit the rotor of l divided by that
/// length; otherwise the square root and the division add a rounding each,
/// so for a direction of length 1 not along an axis the result may differ in
/// the last bit from l sin(theta/2) + e1234 cos(theta/2).
///
/// Evaluated at run time only. std::sin and std::cos are not usable in a
/// constant expression on every compiler, so this function is not constexpr;
/// for a motor in a constant expression, write its coefficients.
template <std::floating_point T>
[[nodiscard]] inline Motor<T> rotation(const Line<T>& l, T theta) {
  const auto& v = l.coefficients;
  const T length = std::sqrt(((v[0] * v[0]) + (v[1] * v[1])) + (v[2] * v[2]));
  // A square root is never subnormal, so a length that is not normal is
  // zero, infinite, or NaN.
  GALOOT_ASSERT(std::isnormal(length), "rotation: the direction of the axis "
                                       "must have a finite nonzero length");

  // The sine divided by the length, so that the six products below hold the
  // unitized line times sin(theta/2) with one division.
  const T half = theta / T{2};
  const T s = std::sin(half) / length;
  const T c = std::cos(half);
  return Motor<T>{
      v[0] * s, v[1] * s, v[2] * s, c, v[3] * s, v[4] * s, v[5] * s, T{0},
  };
}

/// Rejects a rotation about a line whose scalar type is not a floating type,
/// such as an integer or an exact rational, with one error: the sine and
/// cosine of the angle are not values of that type. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask std::floating_point of the scalar type instead.
template <class T, class S>
  requires(!std::floating_point<T>)
Motor<T> rotation(const Line<T>& /*l*/, const S& /*theta*/) {
  static_assert(std::floating_point<T>,
                "rotation: sin and cos of the angle do not give values of this "
                "scalar type, which is not a floating type; use Line<double> "
                "or Line<float>, or build the Motor from its coefficients, the "
                "unitized line times sin(theta/2) and cos(theta/2) on e1234");
  return Motor<T>{};
}

/// Rejects a rotation whose angle is not of the line's floating scalar type,
/// such as 1 or 0.5 for a line over float, with one error. A
/// requires-expression that asks whether such a call is valid is told yes;
/// ask whether the two types are the same instead.
template <std::floating_point T, class S>
  requires(!std::same_as<S, T>)
Motor<T> rotation(const Line<T>& /*l*/, const S& /*theta*/) {
  static_assert(std::is_same_v<S, T>,
                "rotation: the angle is not of the line's scalar type, the "
                "second of the two types shown; give it in radians as that "
                "type, as std::numbers::pi / 2 over double or "
                "std::numbers::pi_v<float> / 2 over float");
  return Motor<T>{};
}

/// Rejects a rotation about a multivector that is not an rga3 Line, such as
/// a Point or the join p ^ q, whose layout is the wedge's, with one error.
/// The assertion has no condition, so that the error line does not print
/// the axis's type, which the compilers spell by its blade masks. A
/// requires-expression that asks whether such a call is valid is told yes
/// for any multivector; it is told no for an axis that is not a multivector.
template <core::multivector X, class S>
  requires(!std::same_as<X, Line<typename X::scalar_type>>)
Motor<typename X::scalar_type> rotation(const X& /*l*/, const S& /*theta*/) {
  static_assert(false, "rotation: the axis is not an rga3 Line; give a "
                       "Line<T>, and convert a join such as p ^ q with "
                       "Line<T>{p ^ q}");
  return Motor<typename X::scalar_type>{};
}

/// The translator that moves points by d = (dx, dy, dz):
/// (dx/2) e23 + (dy/2) e31 + (dz/2) e12 + e1234, with half of d in the
/// coefficients. A point of weight w moves by w d, so a direction (weight 0)
/// is unchanged. The result is unitized for every d.
///
/// Requires: T is not a built-in integer type, since halving d would
/// truncate there, and T has division. Over a floating type the halves are
/// exact unless a component is subnormal; over an exact rational type they
/// are exact. dx, dy, and dz have one type; a mix, such as
/// translation(1.0, 0, 2), is rejected.
///
/// The arguments are plain scalars, which name no namespace of the model, so
/// an unqualified call does not find this function: write
/// galoot::presets::rga3::translation(dx, dy, dz).
template <core::ring_scalar T>
  requires(!std::integral<T>)
[[nodiscard]] constexpr Motor<T> translation(T dx, T dy, T dz) {
  return Motor<T>{
      T{0}, T{0}, T{0}, T{1}, dx / T{2}, dy / T{2}, dz / T{2}, T{0},
  };
}

/// Rejects a translation over a built-in integer type, such as
/// translation(1, 2, 3), with one error: halving d would truncate. A
/// requires-expression that asks whether such a call is valid is told yes;
/// ask std::integral of the scalar type instead.
template <std::integral T>
Motor<T> translation(T /*dx*/, T /*dy*/, T /*dz*/) {
  static_assert(!std::integral<T>,
                "translation: halving d would truncate in an integer scalar "
                "type; use a floating or exact rational scalar type");
  return Motor<T>{};
}

/// Rejects a translation whose three scalars are not all of one type, such
/// as translation(1.0, 0, 2), with one error. A requires-expression that asks
/// whether such a call is valid is told yes for any three ring scalars; ask
/// whether their types are the same instead.
template <core::ring_scalar X, core::ring_scalar Y, core::ring_scalar Z>
  requires(!(std::same_as<X, Y> && std::same_as<Y, Z>))
Motor<X> translation(const X& /*dx*/, const Y& /*dy*/, const Z& /*dz*/) {
  static_assert(std::is_same_v<X, Y> && std::is_same_v<Y, Z>,
                "translation: dx, dy, and dz are not all of one scalar type, "
                "the types shown; give all three the same type, as "
                "translation(1.0, 0.0, 2.0) over double");
  return Motor<X>{};
}

} // namespace galoot::presets::rga3

#endif // GALOOT_PRESETS_RGA3_MOTIONS_HPP
