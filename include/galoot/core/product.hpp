#ifndef GALOOT_CORE_PRODUCT_HPP
#define GALOOT_CORE_PRODUCT_HPP

// The geometric product, the wedge, and their antiproducts of two
// multivectors of one algebra, as functions and as the operators * ^ % &.
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
//   constexpr auto plane_layout =
//       galoot::core::blades<rga3, "e423 e431 e412 e321">;
//
//   using Point = galoot::core::mv<rga3, point_layout, double>;
//   using Plane = galoot::core::mv<rga3, plane_layout, double>;
//   Point p{1, 0, 0, 1};
//   Point q{0, 1, 0, 1};
//   Plane g{0, 0, 1, 0};  // e412, the plane z = 0
//   auto line = galoot::core::wedge(p, q);  // or p ^ q
//   auto m = galoot::core::geometric_product(p, q);  // or p * q
//   auto s = galoot::core::antiwedge(p, g);  // or p & g
//   double at_p = galoot::core::get<"1">(s);  // 0: p lies on g
//   auto n = galoot::core::geometric_antiproduct(p, g);  // or p % g
//
// The result holds exactly the blades the product can reach, each in the
// canonical order of its grade with orientation +1, so its type follows from
// the operands' types and is not a layout the user named: the wedge of two
// vectors is an mv on the bivector blades in canonical order, and its type
// is mv<Alg, wedge_layout<Alg, A, B>, T>. Convert to a named layout
// (Line{p ^ q}) to read the blades under the orientations that layout gives;
// comparing two values with == needs them to have one layout. The operators
// ^ and & bind below ==, so parenthesize them: (p ^ q) == line. The
// operator % binds as * does.
//
// Both operands must be mv types of the same algebra and the same scalar
// type; two mv types that differ in either are rejected at compile time with
// a message that names the mistake. The rejection is an overload of its own,
// so a requires-expression that asks whether such a call or operator is
// valid is told yes; ask the operand types instead. A scalar operand is not
// accepted here.
// A class derived from mv is accepted when the product is valid, and gets
// the compiler's own error when it is not.
//
// Compile cost: each pair of operand layouts builds one plan, a constant
// evaluation, so the compiler's limits on constant evaluation bound the
// layouts a product accepts. Typed layouts such as a point, a line, a plane,
// or a motor are far below them. Clang's -fconstexpr-steps and GCC's
// -fconstexpr-ops-limit raise those limits, and the sizes measured on each
// compiler are in docs/guide/limits.md, under
// 'Compile-time limits: products and plans'.

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
  geometric_antiproduct_function,
  antiwedge_function,
  percent_operator,
  ampersand_operator,
};

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
    } else if constexpr (Spelling == caret_operator) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a ^ b: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else if constexpr (Spelling == geometric_antiproduct_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "geometric_antiproduct: the operands are in different algebras; give "
          "both types the same algebra tag");
    } else if constexpr (Spelling == antiwedge_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "antiwedge: the operands are in different algebras; give both types "
          "the same algebra tag");
    } else if constexpr (Spelling == percent_operator) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a % b: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a & b: the operands are in different algebras; give both types the "
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
    } else if constexpr (Spelling == caret_operator) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a ^ b: the operands have different scalar types; give both types "
          "the same scalar type");
    } else if constexpr (Spelling == geometric_antiproduct_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "geometric_antiproduct: the operands have different scalar types; "
          "give both types the same scalar type");
    } else if constexpr (Spelling == antiwedge_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "antiwedge: the operands have different scalar types; give both "
          "types the same scalar type");
    } else if constexpr (Spelling == percent_operator) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a % b: the operands have different scalar types; give both types "
          "the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a & b: the operands have different scalar types; give both types "
          "the same scalar type");
    }
  }
  return R{};
}

} // namespace detail

// The layouts of the products' results. A product's declared result type
// names its layout through these, so that a program can name the result type
// of a product and a compiler prints it in these terms, and not through the
// plan that computes it. clang-tidy reads the operation, once substituted, as
// a C-style cast of its value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of geometric_product(x, y) for x on the layout A and y on the
/// layout B in the algebra Alg: the blades the product can reach, each with
/// orientation +1, in order of grade and, within a grade, of index list
/// (e12, e13, e14, e23, ...). Two vectors of rigid(3) give the scalar and the
/// six bivectors.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto geometric_product_layout =
    detail::product_plan<detail::BinaryOperation::geometric_product, Alg, A, B>.result;

/// The layout of wedge(x, y), as for geometric_product_layout: two vectors
/// give the bivectors, mv<Alg, wedge_layout<Alg, A, B>, T>.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto wedge_layout =
    detail::product_plan<detail::BinaryOperation::wedge, Alg, A, B>.result;

/// The layout of geometric_antiproduct(x, y), as for
/// geometric_product_layout: two points of rigid(3) give e12, e13, e23, and
/// e1234.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto geometric_antiproduct_layout =
    detail::product_plan<detail::BinaryOperation::geometric_antiproduct, Alg,
                         A, B>.result;

/// The layout of antiwedge(x, y), as for geometric_product_layout: a point
/// and a plane of rigid(3) give the scalar.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto antiwedge_layout =
    detail::product_plan<detail::BinaryOperation::antiwedge, Alg, A, B>.result;

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
/// type, as for geometric_product: a requires-expression is told yes.
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
/// type, with one error that names the cause; a requires-expression that asks
/// whether x * y is valid for them is told yes.
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
/// type, with one error that names the cause; a requires-expression that asks
/// whether x ^ y is valid for them is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator^(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::caret_operator, X,
                                Y>();
}

/// The geometric antiproduct x ⟇ y of two multivectors of one algebra:
/// left(right(x) ⟑ right(y)), with right and left the complements fixed by
/// u ∧ right(u) = 𝟙 and left(u) ∧ u = 𝟙 for every basis blade u, on the
/// blades the antiproduct can reach. Its identity is the antiscalar 𝟙 =
/// e12...n, as 1 is the geometric product's.
///
/// Requires: as for geometric_product, with the message naming
/// geometric_antiproduct.
///
/// Cost: one plan for each pair of operand layouts, as for the geometric
/// product. The plan is built through the complements in all n generators,
/// so its compile cost grows with n even when x and y use few generators.
///
/// Limit: the plan is a constant evaluation, so the compiler's limits on
/// constant evaluation bound the layouts. Clang's -fconstexpr-steps and
/// GCC's -fconstexpr-ops-limit raise those limits, and the sizes measured on
/// each compiler, with how a failure is reported, are in
/// docs/guide/limits.md, under 'Compile-time limits: products and plans'.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, geometric_antiproduct_layout<Alg, A, B>, T>
geometric_antiproduct(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  constexpr bool scalar_holds_every_coefficient_of_the_antiproduct =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_antiproduct,
                "geometric_antiproduct: the scalar type cannot hold one of the "
                "antiproduct's exact coefficients; use a scalar type that "
                "holds larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_antiproduct) {
    return detail::apply<detail::BinaryOperation::geometric_antiproduct,
                         geometric_antiproduct_layout<Alg, A, B>>(x, y);
  } else {
    return {};
  }
}

/// Rejects a geometric_antiproduct whose operands are not in one algebra
/// with one scalar type, as for geometric_product: a requires-expression is
/// told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> geometric_antiproduct(const X& /*x*/,
                                                       const Y& /*y*/) {
  return detail::reject_product<
      detail::ProductSpelling::geometric_antiproduct_function, X, Y>();
}

/// The antiwedge x ∨ y of two multivectors of one algebra:
/// left(right(x) ∧ right(y)), with the complements as for
/// geometric_antiproduct, on the blades the antiwedge can reach. It does not
/// depend on the metric. In rigid(3) it is the meet: a point and a plane
/// meet in a scalar that is zero when the point lies on the plane.
///
/// Requires: as for wedge. Every coefficient of the antiwedge of two blades
/// is +1, -1, or 0, so every scalar type holds them.
///
/// Cost: as for geometric_antiproduct.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, antiwedge_layout<Alg, A, B>, T>
antiwedge(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return detail::apply<detail::BinaryOperation::antiwedge,
                       antiwedge_layout<Alg, A, B>>(x, y);
}

/// Rejects an antiwedge whose operands are not in one algebra with one
/// scalar type, as for geometric_product: a requires-expression is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> antiwedge(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::antiwedge_function, X,
                                Y>();
}

/// x % y is geometric_antiproduct(x, y). It binds as * does, left to right.
/// A scalar that cannot hold one of the antiproduct's coefficients is
/// rejected with a message that names x % y. The limits that
/// geometric_antiproduct states apply here too.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, geometric_antiproduct_layout<Alg, A, B>, T>
operator%(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  constexpr bool scalar_holds_every_coefficient_of_the_antiproduct =
      detail::coefficients_exact<detail::BinaryOperation::geometric_antiproduct,
                                 Alg, A, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_antiproduct,
                "a % b: the scalar type cannot hold one of the antiproduct's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_antiproduct) {
    return geometric_antiproduct(x, y);
  } else {
    return {};
  }
}

/// Rejects x % y for operands that are not in one algebra with one scalar
/// type, with one error that names the cause; a requires-expression that asks
/// whether x % y is valid for them is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator%(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::percent_operator, X,
                                Y>();
}

/// x & y is antiwedge(x, y). Like ^, it binds below ==, so parenthesize it:
/// (p & g) == s.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, antiwedge_layout<Alg, A, B>, T>
operator&(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return antiwedge(x, y);
}

/// Rejects x & y for operands that are not in one algebra with one scalar
/// type, with one error that names the cause; a requires-expression that asks
/// whether x & y is valid for them is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator&(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_product<detail::ProductSpelling::ampersand_operator, X,
                                Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_PRODUCT_HPP
