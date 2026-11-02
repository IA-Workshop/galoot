#ifndef GALOOT_CORE_PRODUCT_HPP
#define GALOOT_CORE_PRODUCT_HPP

// The geometric product and the wedge of two multivectors of one algebra, as
// functions and as the operators * and ^.
//
//   using Point = galoot::core::mv<rga3, point_layout, double>;  // e1 e2 e3 e4
//   Point p{1, 0, 0, 1};
//   Point q{0, 1, 0, 1};
//   auto line = galoot::core::wedge(p, q);  // or p ^ q
//   auto m = galoot::core::geometric_product(p, q);  // or p * q
//
// The result holds exactly the blades the product can reach, each in the
// canonical order of its grade with orientation +1, so its type follows from
// the operands' types and is not a layout the user named: the wedge of two
// vectors is an mv on the bivector blades in canonical order. Convert to a
// named layout (Line{p ^ q}) to read the blades under the orientations that
// layout gives; comparing two values with == needs them to have one layout.
// The operator ^ binds below ==, so parenthesize it: (p ^ q) == line.
//
// Both operands must be mv types of the same algebra and the same scalar
// type; two mv types that differ in either are rejected at compile time with
// a message that names the mistake. A scalar operand is not accepted here.
// A class derived from mv is accepted when the product is valid, and gets
// the compiler's own error when it is not. Compile cost: each pair of operand
// layouts builds one plan, within the limits that detail::apply states.

#include <concepts>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace galoot::core {

namespace detail {

/// The ways a product is written, so that a message names the operation as
/// the user wrote it.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class ProductSpelling {
  geometric_product_function,
  wedge_function,
  times_operator,
  caret_operator,
};

template <class X, class Y>
inline constexpr bool same_algebra_v = false;

template <class Alg, blade_set A, blade_set B, class T, class U>
inline constexpr bool same_algebra_v<mv<Alg, A, T>, mv<Alg, B, U>> = true;

template <class X, class Y>
inline constexpr bool same_scalar_v = false;

template <class Alg, class Beta, blade_set A, blade_set B, class T>
inline constexpr bool same_scalar_v<mv<Alg, A, T>, mv<Beta, B, T>> = true;

/// Two multivectors that a product accepts: one algebra, one scalar type.
template <class X, class Y>
concept product_operands =
    multivector<X> && multivector<Y> &&
    same_algebra_v<std::remove_cvref_t<X>, std::remove_cvref_t<Y>> &&
    same_scalar_v<std::remove_cvref_t<X>, std::remove_cvref_t<Y>>;

/// Two multivectors that a product rejects.
template <class X, class Y>
concept mismatched_operands =
    multivector<X> && multivector<Y> && !product_operands<X, Y>;

/// The only error of a mismatched product: the message names the product as
/// written and the cause. Each condition compares the two algebras or the two
/// scalar types themselves, so that the compiler prints them. Instantiated
/// only by the catch-all overloads, whose result type is that of the first
/// operand so that a variable initialized from the call adds no second error.
template <ProductSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_product() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  using enum ProductSpelling;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    if constexpr (Spelling == geometric_product_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "geometric_product: the operands are in different algebras; give "
          "both types the same algebra tag");
    } else if constexpr (Spelling == wedge_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "wedge: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else if constexpr (Spelling == times_operator) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a * b: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a ^ b: the operands are in different algebras; give both types the "
          "same algebra tag");
    }
  } else {
    if constexpr (Spelling == geometric_product_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "geometric_product: the operands have different scalar types; give "
          "both types the same scalar type");
    } else if constexpr (Spelling == wedge_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "wedge: the operands have different scalar types; give both types "
          "the same scalar type");
    } else if constexpr (Spelling == times_operator) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a * b: the operands have different scalar types; give both types "
          "the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a ^ b: the operands have different scalar types; give both types "
          "the same scalar type");
    }
  }
  return R{};
}

} // namespace detail

/// The layout of the result of each product of an mv on the layout A by an
/// mv on the layout B in the algebra Alg: the blades the product can reach,
/// each with orientation +1, in order of grade and, within a grade, of index
/// list (e12, e13, e14, e23, ...). The products declare their result types
/// through these, mv<Alg, wedge_layout<Alg, A, B>, T> for the wedge, so a
/// program can name the result type of a product, and a compiler prints it in
/// these terms.
// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto geometric_product_layout =
    detail::product_plan<detail::BinaryOperation::geometric_product, Alg, A, B>.result;
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto wedge_layout =
    detail::product_plan<detail::BinaryOperation::wedge, Alg, A, B>.result;
// NOLINTEND(modernize-avoid-c-style-cast)

/// The geometric product x y of two multivectors of one algebra, on the
/// blades that the product can reach. The coefficient of each blade is the
/// sum, over the pairs of blades of x and y whose product reaches it, of the
/// two coefficients times the exact coefficient of that blade product.
///
/// Requires: x and y have the same algebra and the same scalar type, and
/// the scalar holds every exact coefficient of the product where its
/// numeric_limits can tell (an integer type within its range, a binary
/// floating type within its digits); otherwise the call is rejected at
/// compile time with a message that names it.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, geometric_product_layout<Alg, A, B>, T>
geometric_product(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  constexpr bool scalar_holds_every_coefficient_of_the_product =
      detail::coefficients_exact<detail::BinaryOperation::geometric_product,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_product,
                "geometric_product: the scalar type cannot hold one of the "
                "product's exact coefficients; use a scalar type that holds "
                "larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_product) {
    return detail::apply<detail::BinaryOperation::geometric_product,
                         geometric_product_layout<Alg, A, B>>(x, y);
  } else {
    return {};
  }
}

/// Rejects a geometric_product whose operands are not in one algebra with
/// one scalar type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> geometric_product(const X& /*x*/,
                                                   const Y& /*y*/) {
  return detail::reject_product<
      detail::ProductSpelling::geometric_product_function, X, Y>();
}

/// The wedge x ∧ y of two multivectors of one algebra, on the blades that
/// the product can reach. It does not depend on the metric.
///
/// Requires: x and y have the same algebra and the same scalar type;
/// otherwise the call is rejected at compile time. Every coefficient of the
/// wedge of two blades is +1, -1, or 0, so every scalar type holds them.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, wedge_layout<Alg, A, B>, T>
wedge(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return detail::apply<detail::BinaryOperation::wedge, wedge_layout<Alg, A, B>>(
      x, y);
}

/// Rejects a wedge whose operands are not in one algebra with one scalar
/// type, as for geometric_product.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> wedge(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::wedge_function, X,
                                Y>();
}

/// x * y is geometric_product(x, y). A scalar that cannot hold one of the
/// product's coefficients is rejected with a message that names x * y.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, geometric_product_layout<Alg, A, B>, T>
operator*(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  constexpr bool scalar_holds_every_coefficient_of_the_product =
      detail::coefficients_exact<detail::BinaryOperation::geometric_product,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_product,
                "a * b: the scalar type cannot hold one of the product's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_product) {
    return geometric_product(x, y);
  } else {
    return {};
  }
}

/// Rejects x * y for operands that are not in one algebra with one scalar
/// type.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator*(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::times_operator, X,
                                Y>();
}

/// x ^ y is wedge(x, y). Parenthesize it next to ==, which binds tighter.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, wedge_layout<Alg, A, B>, T>
operator^(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return wedge(x, y);
}

/// Rejects x ^ y for operands that are not in one algebra with one scalar
/// type.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator^(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::caret_operator, X,
                                Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_PRODUCT_HPP
