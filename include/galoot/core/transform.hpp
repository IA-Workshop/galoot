#ifndef GALOOT_CORE_TRANSFORM_HPP
#define GALOOT_CORE_TRANSFORM_HPP

// Versors: the transform of a multivector by a versor, and the composition
// of two versors, in the sense of Lengyel's rigid and conformal geometric
// algebra, where a motor and a flector act through the geometric
// antiproduct ⟇.
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
//   constexpr auto motor_layout = galoot::core::blades<
//       rga3, "e41 e42 e43 e1234 e23 e31 e12 1">;
//
//   using Point = galoot::core::mv<rga3, point_layout, double>;
//   using Motor = galoot::core::mv<rga3, motor_layout, double>;
//   Motor turn{0, 0, 0.6, 0.8, 0, 0, 0, 0};  // a rotation about the z axis
//   Motor move{0, 0, 0, 1, 1, 2, -1, 0};     // a translation by (2, 4, -2)
//   Motor both{galoot::core::compose(move, turn)};  // turn, then move
//   Point p{1, 0, 0, 1};
//   Point q = galoot::core::transform(p, both);  // (2.28, 4.96, -2, 1)
//
// The parity of a versor is the parity of the antigrades n - k of its
// blades, where n is the number of generators and k a blade's grade: a
// motor is even and a flector odd. In rigid(3) the grade and the antigrade
// have the same parity; where n is odd they do not, and a motor of rigid(2)
// or conformal(3) has blades of odd grade.
//
// transform(x, V) applies the versor V to x. With antireverse(V) written V̰,
// which multiplies a blade of antigrade m by (-1)^(m(m-1)/2):
//
//   even V   V ⟇ x ⟇ V̰
//   odd V    -V ⟇ x̌ ⟇ V̰, where x̌, the antigrade involution of x, multiplies
//            a blade of antigrade m by (-1)^m
//
// When n is even x̌ is the grade involution of x, so an odd V gives
// -V ⟇ x̂ ⟇ V̰; when n is odd x̌ is minus the grade involution, so an odd V
// gives V ⟇ x̂ ⟇ V̰. In rigid(3) a flector F then reflects a point and keeps
// its weight positive, where the single form -F ⟇ x ⟇ F̰ would negate it.
// For a versor, an antiproduct of hyperplanes, with V ⟇ V̰ = λ𝟙 and λ not
// zero, the transform T satisfies λ^(k-1) T(a1 ∧ ... ∧ ak) = T(a1) ∧ ... ∧
// T(ak): lines and planes move as the points that span them. A unitized
// motor or flector has λ = 1; transform does not unitize, so a versor with
// λ = 2 doubles the weight of a point it moves, which is the same point.
//
// The result has x's type, its layout and orientations: transform(line, V)
// is a Line. The part of V ⟇ x ⟇ V̰ on other blades is dropped. For a versor
// it is zero, and for an exact scalar transform checks that it is; see the
// comment of transform.
//
// compose(Q2, Q1) is Q2 ⟇ Q1, the versor that applies Q1 first and then Q2.
// Its result is on compose_layout<Alg, A, B>, the canonical set of the blades
// the antiproduct reaches, each with orientation +1, in order of grade and
// then of index list, as for a product: convert it to a named layout
// (Motor{compose(q2, q1)}) to read it under that layout's orientations.
//
// Under `using namespace std::ranges` or `using namespace std::views`, an
// unqualified transform(x, v) finds the function object std::ranges::transform
// or std::views::transform, which turns off argument-dependent lookup, so it
// does not compile; write galoot::core::transform(x, v). Under
// `using namespace std`, transform(x, v) finds this function, since
// std::transform takes iterators.
//
// Compile cost: compose is one geometric antiproduct, and transform two,
// V ⟇ x and then that with V, each a plan built once for each pair of
// layouts a program uses, so they have the antiproduct's limits, which the
// comment of geometric_antiproduct gives. Typed layouts such as a point, a
// line, a plane, a motor, or a flector are far below them.

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
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
  transform_function,
  compose_function,
};

/// The only error of a transform or a composition of mismatched operands:
/// the message
/// names the operation and the cause. Each condition compares the two
/// algebras or the two scalar types themselves, so that the compiler prints
/// them. Its result type is that of the first operand, so that a variable
/// initialized from the call adds no second error.
template <TransformSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_transform_operands() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  using enum TransformSpelling;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    if constexpr (Spelling == transform_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "transform: the operands are in different algebras; give both "
          "types the same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "compose: the operands are in different algebras; give both types "
          "the same algebra tag");
    }
  } else {
    if constexpr (Spelling == transform_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "transform: the operands have different scalar types; give both "
          "types the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "compose: the operands have different scalar types; give both "
          "types the same scalar type");
    }
  }
  return R{};
}

// The two plans of a transform of x on X by V on V: the geometric
// antiproduct of V with x, on its result layout, the middle layout; and
// the antiproduct of that with V, whose result holds the blades that the
// transform reaches.
template <metric_like Alg, blade_set V, blade_set X>
inline constexpr const auto& sandwich_first_plan =
    product_plan<BinaryOperation::geometric_antiproduct, Alg, V, X>;

template <metric_like Alg, blade_set V, blade_set X>
inline constexpr auto sandwich_middle_layout =
    sandwich_first_plan<Alg, V, X>.result;

template <metric_like Alg, blade_set V, blade_set X>
inline constexpr const auto& sandwich_second_plan =
    product_plan<BinaryOperation::geometric_antiproduct, Alg,
                 sandwich_middle_layout<Alg, V, X>, V>;

// Whether the blade at position `out` of the second plan's result is a
// blade of X, which the transform keeps.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval bool kept_output(std::size_t out) {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  return position_of(X, second.result.blades[out].mask) < X.size();
}

// The number of terms of the second plan that reach a blade of X, when
// Kept, or another blade, when not.
template <metric_like Alg, blade_set V, blade_set X, bool Kept>
[[nodiscard]] consteval std::size_t second_term_count() {
  std::size_t count = 0;
  for (const typed_term& t : sandwich_second_plan<Alg, V, X>.terms) {
    count += kept_output<Alg, V, X>(t.out) == Kept ? 1 : 0;
  }
  return count;
}

// The number of blades of the second plan's result that X lacks: the
// blades the transform drops.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval std::size_t dropped_blade_count() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::size_t count = 0;
  for (std::size_t out = 0; out < second.result.size(); ++out) {
    count += kept_output<Alg, V, X>(out) ? 0 : 1;
  }
  return count;
}

// The position of each blade of the second plan's result among the dropped
// blades, in the order of that result; kept blades have none.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval auto make_dropped_places() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::array<std::size_t, second.result.size()> places{};
  std::size_t k = 0;
  for (std::size_t out = 0; out < second.result.size(); ++out) {
    if (!kept_output<Alg, V, X>(out)) {
      places[out] = k;
      ++k;
    }
  }
  return places;
}

// The sign that the second stage of the transform folds into each term: the
// antireverse's sign of V's blade, and -1 more for an odd V.
template <metric_like Alg, blade_set V>
[[nodiscard]] consteval coefficient_type second_stage_sign(std::size_t b) {
  constexpr std::size_t n =
      std::remove_cvref_t<decltype(Alg::value)>::generator_count;
  const int odd = antigrade_parity<Alg, V> == Parity::odd ? -1 : 1;
  return static_cast<coefficient_type>(antireverse_sign(V.blades[b].mask, n) *
                                       odd);
}

// The terms of the second stage, (V ⟇ x) ⟇ V̰ with the odd sign, that reach
// the blades of X, when Kept, moved to their places in X and multiplied by
// the blade's orientation there, since c e_C = (c s) (s e_C); or the terms
// that reach the other blades, when not, at their places among the dropped
// blades.
template <metric_like Alg, blade_set V, blade_set X, bool Kept>
[[nodiscard]] consteval auto make_second_kernel() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  constexpr auto dropped_places = make_dropped_places<Alg, V, X>();
  std::array<kernel_term, second_term_count<Alg, V, X, Kept>()> terms{};
  std::size_t k = 0;
  for (const typed_term& t : second.terms) {
    if (kept_output<Alg, V, X>(t.out) != Kept) {
      continue;
    }
    std::size_t out = 0;
    coefficient_type sign = second_stage_sign<Alg, V>(t.b);
    if constexpr (Kept) {
      out = position_of(X, second.result.blades[t.out].mask);
      sign *= X.blades[out].sign;
    } else {
      out = dropped_places[t.out];
    }
    terms[k] = {
        .a = t.a,
        .b = t.b,
        .out = out,
        .coefficient = sign * t.coefficient,
        .first = false,
    };
    ++k;
  }
  if constexpr (Kept) {
    mark_first_terms<X.size()>(terms);
  } else {
    mark_first_terms<dropped_blade_count<Alg, V, X>()>(terms);
  }
  return terms;
}

// The blades of the middle layout that the kept terms of the second stage
// read.
template <metric_like Alg, blade_set V, blade_set X>
[[nodiscard]] consteval auto make_middle_read() {
  constexpr const auto& second = sandwich_second_plan<Alg, V, X>;
  std::array<bool, sandwich_middle_layout<Alg, V, X>.size()> read{};
  for (const typed_term& t : second.terms) {
    if (kept_output<Alg, V, X>(t.out)) {
      read[t.a] = true;
    }
  }
  return read;
}

// The number of terms of the first plan that the first stage keeps: every
// term, when All, or those whose output the kept terms of the second stage
// read.
template <metric_like Alg, blade_set V, blade_set X, bool All>
[[nodiscard]] consteval std::size_t first_term_count() {
  constexpr auto read = make_middle_read<Alg, V, X>();
  std::size_t count = 0;
  for (const typed_term& t : sandwich_first_plan<Alg, V, X>.terms) {
    count += All || read[t.out] ? 1 : 0;
  }
  return count;
}

// The terms of the first stage, V ⟇ x̌ for an odd V and V ⟇ x for an even
// one, the involution's sign of x's blade folded into each term: every term
// of the first plan, when All, or those the kept terms of the second stage
// read, in the plan's order. A term's output is its place in the middle
// layout, so both selections give each blade they reach the same sum in the
// same order.
template <metric_like Alg, blade_set V, blade_set X, bool All>
[[nodiscard]] consteval auto make_first_kernel() {
  constexpr std::size_t n =
      std::remove_cvref_t<decltype(Alg::value)>::generator_count;
  constexpr bool odd = antigrade_parity<Alg, V> == Parity::odd;
  constexpr const auto& first = sandwich_first_plan<Alg, V, X>;
  constexpr auto read = make_middle_read<Alg, V, X>();
  std::array<kernel_term, first_term_count<Alg, V, X, All>()> terms{};
  std::size_t k = 0;
  for (const typed_term& t : first.terms) {
    if (!All && !read[t.out]) {
      continue;
    }
    const coefficient_type sign =
        odd && antigrade(X.blades[t.b].mask, n) % 2 != 0 ? -1 : 1;
    terms[k] = {
        .a = t.a,
        .b = t.b,
        .out = t.out,
        .coefficient = sign * t.coefficient,
        .first = false,
    };
    ++k;
  }
  mark_first_terms<sandwich_middle_layout<Alg, V, X>.size()>(terms);
  return terms;
}

// Each kernel is evaluated once for each pair of layouts a program uses. As
// for coefficients_exact.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
template <metric_like Alg, blade_set V, blade_set X, bool All>
inline constexpr auto first_kernel = make_first_kernel<Alg, V, X, All>();

template <metric_like Alg, blade_set V, blade_set X, bool Kept>
inline constexpr auto second_kernel = make_second_kernel<Alg, V, X, Kept>();
// NOLINTEND(modernize-avoid-c-style-cast)

// The middle coefficients of a transform, by the first kernel of All.
template <metric_like Alg, blade_set V, blade_set X, bool All, class T>
[[nodiscard]] constexpr auto
transform_middle(const std::array<T, V.size()>& v,
                 const std::array<T, X.size()>& x) {
  constexpr const auto& kernel = first_kernel<Alg, V, X, All>;
  return run_product<kernel, sandwich_middle_layout<Alg, V, X>.size()>(
      v, x, std::make_index_sequence<kernel.size()>{});
}

// The coefficients that the second kernel of Kept gives from the middle
// coefficients, on X when Kept and on the dropped blades when not.
template <metric_like Alg, blade_set V, blade_set X, bool Kept, class T,
          std::size_t M>
[[nodiscard]] constexpr auto transform_outer(const std::array<T, M>& middle,
                                             const std::array<T, V.size()>& v) {
  constexpr const auto& kernel = second_kernel<Alg, V, X, Kept>;
  constexpr std::size_t size =
      Kept ? X.size() : dropped_blade_count<Alg, V, X>();
  return run_product<kernel, size>(middle, v,
                                   std::make_index_sequence<kernel.size()>{});
}

// Checks that every coefficient a transform drops is zero, one comparison
// each. The message is on the line of the check, which a failed constant
// evaluation shows.
template <class T, std::size_t D>
constexpr void check_transform_dropped(const std::array<T, D>& dropped) {
  for (const T& d : dropped) {
    GALOOT_ASSERT(d == T{}, "transform: the result leaves the blades of x");
  }
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

/// The transform of x by the versor v: v ⟇ x ⟇ antireverse(v) when v is
/// even, every blade of even antigrade, and -v ⟇ x̌ ⟇ antireverse(v) when v
/// is odd, where x̌ multiplies a blade of antigrade m by (-1)^m; the header's
/// comment gives the forms for even and odd n. The result is on x's layout,
/// with x's orientations, so transform(point, motor) is a point; the part of
/// the antiproduct on blades x lacks is dropped. An empty v gives zero. The
/// coefficients are computed by straight-line code from the terms of the two
/// antiproducts that reach x's blades, with the signs of the antireverse, of
/// the involution, and of an odd v folded into the terms' coefficients;
/// under IEEE 754 a negation is exact, so this is the value of the
/// antiproducts written out, bit for bit. transform does not unitize: for
/// v ⟇ antireverse(v) = λ𝟙 it multiplies a point's weight by λ.
///
/// Requires: x and v have the same algebra and the same scalar type; v has
/// one parity, every blade of even antigrade or every blade of odd antigrade;
/// and the scalar holds every exact coefficient of the two antiproducts the
/// transform composes, v ⟇ x and that with v, where its numeric_limits can
/// tell, as for geometric_antiproduct. Otherwise the call is rejected at
/// compile time with a message that names transform.
///
/// Checks, for an exact_scalar T with ==, that every coefficient it drops is
/// zero, which holds when v acts as a versor on x: in rigid(3) for any v of
/// one parity and x a point, a line, a plane, a motor, or a flector, and in
/// any algebra for a versor and an x of whole grades. A v that is not a
/// versor may leave the blades of x, as v = 1 + 𝟙 does for the scalar in
/// R(4,0,0). The check is a GALOOT_ASSERT in every constant evaluation,
/// which then fails, and at run time when GALOOT_CHECKING is 1, which then
/// aborts with the message; it compares each dropped coefficient with zero
/// once, after computing the terms that reach the dropped blades. With
/// GALOOT_CHECKING 0 those terms are not computed at run time. For a scalar
/// that is not exact, such as double or std::complex<double>, nothing is
/// checked.
/// Guarantees: on x's layout; with an exact scalar the result is exact, and
/// the result is the same with and without the check.
template <metric_like Alg, blade_set X, blade_set V, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, X, T> transform(const mv<Alg, X, T>& x,
                                                const mv<Alg, V, T>& v) {
  constexpr bool versor_has_one_parity =
      detail::antigrade_parity<Alg, V> != detail::Parity::mixed;
  static_assert(versor_has_one_parity,
                "transform: the versor holds blades of both parities, even "
                "and odd antigrade; a versor is even, as a motor, or odd, as "
                "a flector");
  constexpr bool scalar_holds_every_coefficient_of_the_transform =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, V, X, T> &&
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, detail::sandwich_middle_layout<Alg, V, X>,
                                 V, T>;
  static_assert(scalar_holds_every_coefficient_of_the_transform,
                "transform: the scalar type cannot hold one of the "
                "transform's exact coefficients; use a scalar type that holds "
                "larger integers exactly");

  mv<Alg, X, T> out;
  if constexpr (versor_has_one_parity &&
                scalar_holds_every_coefficient_of_the_transform) {
    if constexpr (exact_scalar<T> && std::equality_comparable<T>) {
      if (detail::checking_now()) {
        const auto middle = detail::transform_middle<Alg, V, X, true>(
            v.coefficients, x.coefficients);
        out.coefficients =
            detail::transform_outer<Alg, V, X, true>(middle, v.coefficients);
        detail::check_transform_dropped(
            detail::transform_outer<Alg, V, X, false>(middle, v.coefficients));
        return out;
      }
    }
    out.coefficients = detail::transform_outer<Alg, V, X, true>(
        detail::transform_middle<Alg, V, X, false>(v.coefficients,
                                                   x.coefficients),
        v.coefficients);
  }
  return out;
}

/// Rejects a transform whose operands are not in one algebra with one scalar
/// type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> transform(const X& /*x*/, const Y& /*v*/) {
  return detail::reject_transform_operands<
      detail::TransformSpelling::transform_function, X, Y>();
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
