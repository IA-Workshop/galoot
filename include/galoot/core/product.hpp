#ifndef GALOOT_CORE_PRODUCT_HPP
#define GALOOT_CORE_PRODUCT_HPP

// The geometric product and the wedge of two multivectors of one algebra, as
// functions and as the operators * and ^.
//
//   using Line = galoot::core::mv<rga3, line_layout, double>;
//   auto m = galoot::core::geometric_product(a, b);  // or a * b
//   auto w = galoot::core::wedge(a, b);              // or a ^ b
//
// The result holds exactly the blades the product can reach, so its type
// follows from the operands' types. The operator ^ binds below ==, so
// parenthesize it: (a ^ b) == c.
//
// Both operands must be in the same algebra and have the same scalar type;
// anything else is rejected at compile time with a message that names the
// mistake. A scalar operand is not accepted here.

#include <concepts>
#include <cstdint>
#include <type_traits>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace galoot::core {

namespace detail {

/// The ways a product is written, so that a message names the operation as
/// the user wrote it.
enum class ProductSpelling : std::uint8_t {
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
/// written and the cause. Instantiated only by the catch-all overloads.
template <ProductSpelling Spelling, class X, class Y>
constexpr void reject_product() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  constexpr bool algebras = same_algebra_v<R, S>;
  using enum ProductSpelling;
  if constexpr (!algebras) {
    static_assert(Spelling != geometric_product_function,
                  "geometric_product: the operands are in different algebras; "
                  "both must be in the same one");
    static_assert(Spelling != wedge_function,
                  "wedge: the operands are in different algebras; both must "
                  "be in the same one");
    static_assert(Spelling != times_operator,
                  "operator*: the operands are in different algebras; both "
                  "must be in the same one");
    static_assert(Spelling != caret_operator,
                  "operator^: the operands are in different algebras; both "
                  "must be in the same one");
  } else {
    static_assert(Spelling != geometric_product_function,
                  "geometric_product: the operands have different scalar "
                  "types; convert one explicitly first");
    static_assert(Spelling != wedge_function,
                  "wedge: the operands have different scalar types; convert "
                  "one explicitly first");
    static_assert(Spelling != times_operator,
                  "operator*: the operands have different scalar types; "
                  "convert one explicitly first");
    static_assert(Spelling != caret_operator,
                  "operator^: the operands have different scalar types; "
                  "convert one explicitly first");
  }
}

} // namespace detail

/// The geometric product x y of two multivectors of one algebra, on the
/// blades that the product can reach. The coefficient of each blade is the
/// sum over the pairs of blades whose product lands on it, evaluated as
/// detail::apply evaluates it.
///
/// Requires: x and y have the same algebra and the same scalar type, and
/// the scalar holds every coefficient of the product exactly where its
/// numeric_limits can tell; otherwise the call is rejected at compile time.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr auto geometric_product(const mv<Alg, A, T>& x,
                                               const mv<Alg, B, T>& y) {
  return detail::apply<detail::BinaryOperation::geometric_product>(x, y);
}

/// Rejects a geometric_product whose operands are not in one algebra with
/// one scalar type, with one error that names the cause.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr void geometric_product([[maybe_unused]] const X& x,
                                 [[maybe_unused]] const Y& y) {
  detail::reject_product<detail::ProductSpelling::geometric_product_function, X,
                         Y>();
}

/// The wedge x ∧ y of two multivectors of one algebra, on the blades that
/// the product can reach. It does not depend on the metric.
///
/// Requires: as for geometric_product.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr auto wedge(const mv<Alg, A, T>& x,
                                   const mv<Alg, B, T>& y) {
  return detail::apply<detail::BinaryOperation::wedge>(x, y);
}

/// Rejects a wedge whose operands are not in one algebra with one scalar
/// type, as for geometric_product.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr void wedge([[maybe_unused]] const X& x, [[maybe_unused]] const Y& y) {
  detail::reject_product<detail::ProductSpelling::wedge_function, X, Y>();
}

/// x * y is geometric_product(x, y).
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr auto operator*(const mv<Alg, A, T>& x,
                                       const mv<Alg, B, T>& y) {
  return geometric_product(x, y);
}

/// Rejects x * y for operands that are not in one algebra with one scalar
/// type.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr void operator*([[maybe_unused]] const X& x,
                         [[maybe_unused]] const Y& y) {
  detail::reject_product<detail::ProductSpelling::times_operator, X, Y>();
}

/// x ^ y is wedge(x, y). Parenthesize it next to ==, which binds tighter.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr auto operator^(const mv<Alg, A, T>& x,
                                       const mv<Alg, B, T>& y) {
  return wedge(x, y);
}

/// Rejects x ^ y for operands that are not in one algebra with one scalar
/// type.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr void operator^([[maybe_unused]] const X& x,
                         [[maybe_unused]] const Y& y) {
  detail::reject_product<detail::ProductSpelling::caret_operator, X, Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_PRODUCT_HPP
