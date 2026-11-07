#ifndef GALOOT_CORE_TRANSFORM_HPP
#define GALOOT_CORE_TRANSFORM_HPP

// Versors: the composition of two versors, in the sense of Lengyel's rigid
// and conformal geometric algebra, where a motor and a flector act through
// the geometric antiproduct ⟇.
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto motor_layout = galoot::core::blades<
//       rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
//
//   using Motor = galoot::core::mv<rga3, motor_layout, double>;
//   Motor turn{0, 0, 0.6, 0.8, 0, 0, 0, 0};  // a rotation about the z axis
//   Motor move{0, 0, 0, 1, 1, 2, -1, 0};     // a translation by (2, 4, -2)
//   Motor both{galoot::core::compose(move, turn)};  // turn, then move
//
// The parity of a versor is the parity of the antigrades n - k of its
// blades, where n is the number of generators and k a blade's grade: a
// motor is even and a flector odd. In rigid(3) the grade and the antigrade
// have the same parity; where n is odd they do not, and a motor of rigid(2)
// or conformal(3) has blades of odd grade.
//
// compose(Q2, Q1) is Q2 ⟇ Q1, the versor that applies Q1 first and then Q2.
// Its result is on compose_layout<Alg, A, B>, the canonical set of the blades
// the antiproduct reaches, each with orientation +1, in order of grade and
// then of index list, as for a product: convert it to a named layout
// (Motor{compose(q2, q1)}) to read it under that layout's orientations.
//
// Compile cost: compose is one geometric antiproduct, with its limits; the
// comment of geometric_antiproduct gives them.

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>
#include <galoot/core/product.hpp>

namespace galoot::core {

namespace detail {

/// The parity of the antigrades of a layout's blades: even when every blade
/// has an even antigrade, which an empty layout has vacuously; odd when
/// every blade has an odd one; mixed otherwise.
enum class Parity : std::uint8_t {
  even,
  odd,
  mixed,
};

// The parity of the antigrades n - k of the blades of `set`.
template <std::size_t K>
[[nodiscard]] constexpr Parity parity_of(const blade_set<K>& set,
                                         std::size_t n) noexcept {
  bool has_even = false;
  bool has_odd = false;
  for (const oriented_blade& blade : set.blades) {
    if (antigrade(blade.mask, n) % 2 == 0) {
      has_even = true;
    } else {
      has_odd = true;
    }
  }
  if (has_even && has_odd) {
    return Parity::mixed;
  }
  return has_odd ? Parity::odd : Parity::even;
}

/// The parity of the antigrades of the blades of S in the algebra Alg.
template <metric_like Alg, blade_set S>
inline constexpr Parity antigrade_parity =
    parity_of(S, std::remove_cvref_t<decltype(Alg::value)>::generator_count);

/// The ways an operation of this header is written, so that a message names
/// the operation as the user wrote it.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class TransformSpelling {
  compose_function,
};

/// The only error of a composition of mismatched operands: the message
/// names the operation and the cause. Each condition compares the two
/// algebras or the two scalar types themselves, so that the compiler prints
/// them. Its result type is that of the first operand, so that a variable
/// initialized from the call adds no second error.
template <TransformSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_transform_operands() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    static_assert(
        std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
        "compose: the operands are in different algebras; give both types "
        "the same algebra tag");
  } else {
    static_assert(
        std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
        "compose: the operands have different scalar types; give both types "
        "the same scalar type");
  }
  return R{};
}

} // namespace detail

// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of compose(q2, q1) for q2 on the layout A and q1 on the layout
/// B in the algebra Alg: the blades of the geometric antiproduct of A and B,
/// each with orientation +1, in order of grade and then of index list. Two
/// rigid(3) motors give 1, e12, e13, e14, e23, e24, e34, and e1234.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto compose_layout = geometric_antiproduct_layout<Alg, A, B>;

// NOLINTEND(modernize-avoid-c-style-cast)

/// The composition of two versors, q2 ⟇ q1: the versor that applies q1 first
/// and then q2, so that transform(x, compose(q2, q1)) is
/// transform(transform(x, q1), q2). The composition of two motors is a
/// motor, of a motor and a flector a flector, and of two flectors a motor,
/// since antigrades add modulo 2 under the antiproduct.
///
/// Requires: q2 and q1 have the same algebra and the same scalar type; each
/// has one parity, every blade of even antigrade or every blade of odd
/// antigrade; and the scalar holds every exact coefficient of the
/// antiproduct where its numeric_limits can tell, as for
/// geometric_antiproduct. Otherwise the call is rejected at compile time
/// with a message that names compose. For the antiproduct of anything, write
/// geometric_antiproduct or %.
/// Guarantees: on compose_layout<Alg, A, B>. Never checks: it drops nothing.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, compose_layout<Alg, A, B>, T>
compose(const mv<Alg, A, T>& q2, const mv<Alg, B, T>& q1) {
  constexpr bool each_operand_has_one_parity =
      detail::antigrade_parity<Alg, A> != detail::Parity::mixed &&
      detail::antigrade_parity<Alg, B> != detail::Parity::mixed;
  static_assert(each_operand_has_one_parity,
                "compose: an operand holds blades of both parities, even and "
                "odd antigrade; compose versors that are each even or odd");
  constexpr bool scalar_holds_every_coefficient_of_the_composition =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_composition,
                "compose: the scalar type cannot hold one of the "
                "composition's exact coefficients; use a scalar type that "
                "holds larger integers exactly");
  if constexpr (each_operand_has_one_parity &&
                scalar_holds_every_coefficient_of_the_composition) {
    return detail::apply<detail::BinaryOperation::geometric_antiproduct,
                         compose_layout<Alg, A, B>>(q2, q1);
  } else {
    return {};
  }
}

/// Rejects a composition whose operands are not in one algebra with one
/// scalar type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> compose(const X& /*q2*/, const Y& /*q1*/) {
  return detail::reject_transform_operands<
      detail::TransformSpelling::compose_function, X, Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_TRANSFORM_HPP
