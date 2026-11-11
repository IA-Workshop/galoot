#ifndef GALOOT_CORE_DUAL_HPP
#define GALOOT_CORE_DUAL_HPP

// The bulk and the weight of a multivector, its duals, the dot and the
// antidot of two multivectors, and the projection and the antiprojection of
// one onto another, in the sense of Lengyel's rigid and conformal geometric
// algebra:
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
//   Point p{1, 2, 3, 1};
//   auto b = galoot::core::bulk(p);  // e1 e2 e3: 1, 2, 3
//   auto w = galoot::core::weight(p);  // e4: 1
//   Plane g{galoot::core::bulk_dual(p)};  // the plane (1, 2, 3, 0)
//   auto d = galoot::core::dot(p, p);  // the scalar 14
//   double pp = galoot::core::get<"1">(d);
//
// The metric exomorphism G sends a blade e_A to the sum over the blades e_B
// of its grade of det g[A, B] e_B, the Gram determinant of the generators of
// A against those of B, and extends linearly; in a diagonal metric it
// multiplies each blade by the product of its generators' squares. The
// antiexomorphism is left(G right(x)), with right and left the complements
// fixed by u ∧ right(u) = 𝟙 and left(u) ∧ u = 𝟙 for each basis blade u,
// where 𝟙 = e12...n is the antiscalar. Then:
//
//   bulk(x)              G x
//   weight(x)            left(G right(x))
//   bulk_dual(x)         right(G x)
//   weight_dual(x)       right(weight(x)), which is G right(x)
//   left_bulk_dual(x)    left(G x)
//   left_weight_dual(x)  left(weight(x))
//   dot(a, b)            the sum of a_A G_AB b_B, on the scalar
//   antidot(a, b)        the sum of a_A (left G right)_AB b_B, times 𝟙
//   project(a, b)        antiwedge(b, wedge(a, weight_dual(b)))
//   antiproject(a, b)    wedge(b, antiwedge(a, weight_dual(b)))
//
// In rigid(3), where e1, e2, e3 square to 1 and e4 to 0, the bulk is the part
// without e4 and the weight the part with it, so bulk(x) + weight(x) is x,
// and a ∧ bulk_dual(b) is dot(a, b) times 𝟙 for a and b of one grade. In
// other metrics the bulk and the weight need not split x:
//
// - where no generator squares to 0, as in R(n,0,0), both keep every blade;
// - in conformal(3), with e4 and e5 null and e4 · e5 = -1, the weight is the
//   negated bulk, and both move blades: the bulk of e4 is -e5;
// - in a diagonal metric with a negative square, the bulk keeps a blade times
//   the product of the squares of its generators, as -e2 for e2 in R(1,1,0);
// - with two or more null generators, as in R(2,0,2), a blade that holds some
//   but not all of them, such as e3, is in neither.
//
// The four parts of an object of the conformal model (round and flat, bulk
// and weight) are the model's own sets of blades, not these.
//
// Result layouts: every operation here can send a blade to another blade, or
// to none, so its result is the canonical set of the blades it reaches, each
// with orientation +1, in order of grade and then of index list, as for a
// product. bulk of a rigid(3) line holds e12, e13, e23, where the line's
// moment on e31 appears negated on e13; Line{bulk(l)} reads it back under the
// line's orientations. The dot holds the scalar alone when the metric pairs
// some blade of one operand with some blade of the other, and no blade
// otherwise, so dot(point, line) is a value of an empty layout, and the type
// says it is zero; the antidot holds 𝟙 likewise. Each result type is
// mv<Alg, bulk_layout<Alg, S>, T>, and likewise for the other operations.
//
// Nothing here checks anything: the bulk and the weight drop blades by
// definition, the dot and the antidot compute one coefficient, and the duals
// and the projections drop nothing.
//
// Compile cost: each operation is a composition of the products, the reverse,
// and the complements, so it costs what its parts cost, and the smallest of
// their limits on constant evaluation bounds it. Typed layouts such as a
// point, a line, a plane, or a motor are far below them. The weight, the
// weight duals, the antidot, and the projection pass through the
// complements, whose grades grow with n, so their compile cost grows with n
// even when x uses few generators. Clang's -fconstexpr-steps and GCC's
// -fconstexpr-ops-limit raise those limits, and the sizes measured on each
// compiler, with how a failure is reported, are in docs/guide/limits.md,
// under 'Compile-time limits: duals, dot products and projections'.
//
// The names bulk, weight, and dot are common words. A variable of one of
// these names in scope stops an unqualified call of the function, as for any
// function found by argument-dependent lookup; write galoot::core::bulk(x).
// C++26's std::execution::bulk is a function object, so under
// `using namespace std::execution` an unqualified bulk(x) finds it and does
// not compile.

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/unary.hpp>

namespace galoot::core {

namespace detail {

// The antiscalar e12...n of n generators, as a layout of one blade.
[[nodiscard]] constexpr blade_set<1> antiscalar_of(std::size_t n) noexcept {
  blade_set<1> set{};
  set.blades[0] = {.mask = low_bits(n), .sign = 1};
  return set;
}

template <metric_like Alg>
inline constexpr blade_set<1> antiscalar_layout =
    antiscalar_of(std::remove_cvref_t<decltype(Alg::value)>::generator_count);

// The antiscalar with coefficient 1, the identity of the geometric
// antiproduct.
template <metric_like Alg, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, antiscalar_layout<Alg>, T> unit_antiscalar() {
  return mv<Alg, antiscalar_layout<Alg>, T>{
      static_cast<T>(coefficient_type{1}),
  };
}

// The number of terms of the geometric product of A and B that land on the
// scalar.
template <metric_like Alg, blade_set A, blade_set B>
[[nodiscard]] consteval std::size_t scalar_term_count() {
  constexpr const auto& plan =
      product_plan<BinaryOperation::geometric_product, Alg, A, B>;
  std::size_t count = 0;
  for (const typed_term& t : plan.terms) {
    count += plan.result.blades[t.out].mask == 0 ? 1 : 0;
  }
  return count;
}

// The terms of the geometric product of A and B that land on the scalar,
// written onto position 0 of a result of one blade: the plan of the product
// with a mask on its outputs. Applied to a and reverse(b), they compute the
// scalar part of a b~ and nothing else.
template <metric_like Alg, blade_set A, blade_set B>
[[nodiscard]] consteval auto make_scalar_part_kernel() {
  constexpr const auto& plan =
      product_plan<BinaryOperation::geometric_product, Alg, A, B>;
  std::array<kernel_term, scalar_term_count<Alg, A, B>()> terms{};
  std::size_t k = 0;
  for (const typed_term& t : plan.terms) {
    if (plan.result.blades[t.out].mask == 0) {
      terms[k] = {
          .a = t.a,
          .b = t.b,
          .out = 0,
          .coefficient = t.coefficient,
          .first = false,
      };
      ++k;
    }
  }
  mark_first_terms<1>(terms);
  return terms;
}

template <metric_like Alg, blade_set A, blade_set B>
// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTNEXTLINE(modernize-avoid-c-style-cast)
inline constexpr auto scalar_part_kernel = make_scalar_part_kernel<Alg, A, B>();

// The scalar alone when some term of the product of A and B lands on it, and
// no blade otherwise.
template <metric_like Alg, blade_set A, blade_set B>
[[nodiscard]] consteval auto make_scalar_part_layout() {
  if constexpr (scalar_term_count<Alg, A, B>() == 0) {
    return blade_set<0>{};
  } else {
    return blade_set<1>{{{{.mask = 0, .sign = 1}}}};
  }
}

// Whether every coefficient of the terms converts to T exactly, as far as
// std::numeric_limits<T> tells.
template <class T, std::size_t N>
[[nodiscard]] constexpr bool
kernel_held_exactly(const std::array<kernel_term, N>& terms) noexcept {
  // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
  for (const kernel_term& t : terms) {
    if (!holds_exactly<T>(t.coefficient)) {
      return false;
    }
  }
  return true;
}

// Whether T holds every exact coefficient of the bulk of a multivector on S:
// those of the geometric product of S with the antiscalar.
template <metric_like Alg, blade_set S, class T>
inline constexpr bool bulk_coefficients_exact =
    coefficients_exact<BinaryOperation::geometric_product, Alg, S,
                       antiscalar_layout<Alg>, T>;

// Whether T holds every exact coefficient of the weight of a multivector on
// S: those of the bulk of its right complement.
template <metric_like Alg, blade_set S, class T>
inline constexpr bool weight_coefficients_exact = bulk_coefficients_exact<
    Alg, unary_map<UnaryOperation::right_complement, Alg, S>.result, T>;

/// The ways a binary operation of this header is written, so that a message
/// names the operation as the user wrote it.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class DualSpelling {
  dot_function,
  antidot_function,
  project_function,
  antiproject_function,
};

/// The only error of a dot, an antidot, or a projection of mismatched
/// operands: the message names the operation and the cause. Each condition
/// compares the two algebras or the two scalar types themselves, so that the
/// compiler prints them. Its result type is that of the first operand, so
/// that a variable initialized from the call adds no second error.
template <DualSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_dual_operands() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  using enum DualSpelling;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    if constexpr (Spelling == dot_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "dot: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else if constexpr (Spelling == antidot_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "antidot: the operands are in different algebras; give both types "
          "the same algebra tag");
    } else if constexpr (Spelling == project_function) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "project: the operands are in different algebras; give both types "
          "the same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "antiproject: the operands are in different algebras; give both "
          "types the same algebra tag");
    }
  } else {
    if constexpr (Spelling == dot_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "dot: the operands have different scalar types; give both types "
          "the same scalar type");
    } else if constexpr (Spelling == antidot_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "antidot: the operands have different scalar types; give both "
          "types the same scalar type");
    } else if constexpr (Spelling == project_function) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "project: the operands have different scalar types; give both "
          "types the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "antiproject: the operands have different scalar types; give both "
          "types the same scalar type");
    }
  }
  return R{};
}

} // namespace detail

// The layouts of the results, named so that a program can name a result
// type and a compiler prints it in these terms. Each is the layout its
// composition gives. clang-tidy reads the operation, once substituted, as a
// C-style cast of its value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)

/// The layout of bulk(x) for x on the layout S in the algebra Alg: the
/// blades that G reaches from S, each with orientation +1, in order of grade
/// and then of index list. A rigid(3) line gives e12, e13, and e23, and a
/// point e1, e2, and e3.
template <metric_like Alg, blade_set S>
inline constexpr auto bulk_layout = left_complement_layout<
    Alg, geometric_product_layout<Alg, S, detail::antiscalar_layout<Alg>>>;

/// The layout of weight(x), as for bulk_layout: the blades the
/// antiexomorphism reaches from S. A rigid(3) line gives e14, e24, and e34.
template <metric_like Alg, blade_set S>
inline constexpr auto weight_layout =
    left_complement_layout<Alg,
                           bulk_layout<Alg, right_complement_layout<Alg, S>>>;

/// The layout of bulk_dual(x), as for bulk_layout: the complements of the
/// blades of bulk_layout<Alg, S>. A rigid(3) point gives e124, e134, and
/// e234.
template <metric_like Alg, blade_set S>
inline constexpr auto bulk_dual_layout =
    geometric_product_layout<Alg, S, detail::antiscalar_layout<Alg>>;

/// The layout of weight_dual(x), as for bulk_layout. A rigid(3) point gives
/// e123 alone.
template <metric_like Alg, blade_set S>
inline constexpr auto weight_dual_layout =
    bulk_layout<Alg, right_complement_layout<Alg, S>>;

/// The layout of left_bulk_dual(x), the same blades as bulk_dual_layout.
template <metric_like Alg, blade_set S>
inline constexpr auto left_bulk_dual_layout =
    left_complement_layout<Alg, bulk_layout<Alg, S>>;

/// The layout of left_weight_dual(x), the same blades as
/// weight_dual_layout.
template <metric_like Alg, blade_set S>
inline constexpr auto left_weight_dual_layout =
    left_complement_layout<Alg, weight_layout<Alg, S>>;

/// The layout of dot(a, b) for a on the layout A and b on the layout B: the
/// scalar alone when the metric pairs some blade of A with some blade of B,
/// and no blade otherwise. Two rigid(3) points give the scalar; a point and
/// a line give the empty layout.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto dot_layout = detail::make_scalar_part_layout<Alg, A, B>();

/// The layout of antidot(a, b): the antiscalar alone when the
/// antiexomorphism pairs some blade of A with some blade of B, and no blade
/// otherwise. Two rigid(3) points give e1234.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto antidot_layout =
    left_complement_layout<Alg, dot_layout<Alg, right_complement_layout<Alg, A>,
                                           right_complement_layout<Alg, B>>>;

/// The layout of project(a, b) for a on the layout A and b on the layout B:
/// the blades of the antiwedge of b with the wedge of a and the weight dual
/// of b. A rigid(3) point onto a plane gives e1, e2, e3, and e4.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto project_layout =
    antiwedge_layout<Alg, B, wedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;

/// The layout of antiproject(a, b) for a on the layout A and b on the layout
/// B: the blades of the wedge of b with the antiwedge of a and the weight
/// dual of b. A rigid(3) plane onto a point gives e123, e124, e134, and
/// e234, the plane's blades in canonical order.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto antiproject_layout =
    wedge_layout<Alg, B, antiwedge_layout<Alg, A, weight_dual_layout<Alg, B>>>;

// NOLINTEND(modernize-avoid-c-style-cast)

/// The bulk of x, G x: the metric exomorphism, which sends each blade e_A to
/// the sum of det g[A, B] e_B over the blades e_B of its grade. Computed as
/// left(x~ 𝟙), the left complement of the geometric product of the reverse
/// of x with the antiscalar 𝟙 = e12...n, since x~ 𝟙 is right(G x) in every
/// metric. On bulk_layout<Alg, S>. In rigid(3) it is the part of x without
/// e4; the header's comment lists what it is in other metrics. Never checks:
/// dropping the blades that G sends to zero is what it does.
///
/// Requires: the scalar holds every exact coefficient of x~ 𝟙 where its
/// numeric_limits can tell (an integer type within its range, a binary
/// floating type within its digits); otherwise the call is rejected at
/// compile time with a message that names bulk. The coefficients are minors
/// of the metric's matrix; in rigid(d) and conformal(d) each is +1 or -1,
/// which every scalar holds.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, bulk_layout<Alg, S>, T>
bulk(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_bulk =
      detail::bulk_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_bulk,
                "bulk: the scalar type cannot hold one of the bulk's exact "
                "coefficients; use a scalar type that holds larger integers "
                "exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_bulk) {
    return left_complement(
        geometric_product(reverse(x), detail::unit_antiscalar<Alg, T>()));
  } else {
    return {};
  }
}

/// The weight of x, left(G right(x)): the antiexomorphism. Computed as
/// left(bulk(right(x))). On weight_layout<Alg, S>. In rigid(3) it is the part
/// of x with e4: weight(p) of a point is a multivector on e4, not a number,
/// and get<"e4">(weight(p)) reads it. Never checks.
///
/// Requires: the scalar holds every exact coefficient of the bulk of the
/// right complement of x, as for bulk; otherwise the call is rejected at
/// compile time with a message that names weight.
template <metric_like Alg, blade_set S, ring_scalar T>
GALOOT_ALWAYS_INLINE [[nodiscard]] constexpr mv<Alg, weight_layout<Alg, S>, T>
weight(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_weight =
      detail::weight_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_weight,
                "weight: the scalar type cannot hold one of the weight's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_weight) {
    return left_complement(bulk(right_complement(x)));
  } else {
    return {};
  }
}

/// The bulk dual of x, right(G x): the right complement of the metric
/// exomorphism of x. Computed as the geometric product of the reverse of x
/// with the antiscalar, x~ 𝟙, which equals right(G x) in every metric. On
/// bulk_dual_layout<Alg, S>. In rigid(3) the bulk dual of the point
/// (x, y, z, w) is the plane (x, y, z, 0), and a ∧ bulk_dual(b) is
/// dot(a, b) times 𝟙 for a and b of one grade. Applied twice it gives x
/// times (-1)^(gr ag) det g, so zero in a degenerate metric. Never checks.
///
/// Requires: as for bulk, with the message naming bulk_dual.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, bulk_dual_layout<Alg, S>, T>
bulk_dual(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_bulk_dual =
      detail::bulk_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_bulk_dual,
                "bulk_dual: the scalar type cannot hold one of the bulk "
                "dual's exact coefficients; use a scalar type that holds "
                "larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_bulk_dual) {
    return geometric_product(reverse(x), detail::unit_antiscalar<Alg, T>());
  } else {
    return {};
  }
}

/// The weight dual of x, right(left(G right(x))), which is G right(x):
/// computed as bulk(right_complement(x)). On weight_dual_layout<Alg, S>. In
/// rigid(3) the weight dual of the point (x, y, z, w) is the plane
/// (0, 0, 0, w). Never checks.
///
/// Requires: as for weight, with the message naming weight_dual.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, weight_dual_layout<Alg, S>, T>
weight_dual(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_weight_dual =
      detail::weight_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_weight_dual,
                "weight_dual: the scalar type cannot hold one of the weight "
                "dual's exact coefficients; use a scalar type that holds "
                "larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_weight_dual) {
    return bulk(right_complement(x));
  } else {
    return {};
  }
}

/// The left bulk dual of x, left(G x): the bulk dual with the left complement
/// in place of the right, which is bulk_dual(x) times (-1)^(gr ag) for each
/// blade of grade gr and antigrade ag of the result. In rigid(3) the left
/// bulk dual of the point (x, y, z, w) is the plane (-x, -y, -z, 0). On
/// left_bulk_dual_layout<Alg, S>. Never checks.
///
/// Requires: as for bulk, with the message naming left_bulk_dual.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, left_bulk_dual_layout<Alg, S>, T>
left_bulk_dual(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_left_bulk_dual =
      detail::bulk_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_left_bulk_dual,
                "left_bulk_dual: the scalar type cannot hold one of the left "
                "bulk dual's exact coefficients; use a scalar type that "
                "holds larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_left_bulk_dual) {
    return left_complement(bulk(x));
  } else {
    return {};
  }
}

/// The left weight dual of x, left(weight(x)): the weight dual with the left
/// complement in place of the right. In rigid(3) the left weight dual of the
/// point (x, y, z, w) is the plane (0, 0, 0, -w). On
/// left_weight_dual_layout<Alg, S>. Never checks.
///
/// Requires: as for weight, with the message naming left_weight_dual.
template <metric_like Alg, blade_set S, ring_scalar T>
GALOOT_ALWAYS_INLINE
[[nodiscard]] constexpr mv<Alg, left_weight_dual_layout<Alg, S>, T>
left_weight_dual(const mv<Alg, S, T>& x) {
  constexpr bool scalar_holds_every_coefficient_of_the_left_weight_dual =
      detail::weight_coefficients_exact<Alg, S, T>;
  static_assert(scalar_holds_every_coefficient_of_the_left_weight_dual,
                "left_weight_dual: the scalar type cannot hold one of the "
                "left weight dual's exact coefficients; use a scalar type "
                "that holds larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_left_weight_dual) {
    return left_complement(weight(x));
  } else {
    return {};
  }
}

/// The dot a • b, the sum of a_A G_AB b_B over the blades of a and b, on the
/// scalar: the scalar part of a b~, the geometric product of a with the
/// reverse of b, computed from the terms of that product that land on the
/// scalar and no others. Blades of different grades contribute nothing. This
/// is Lengyel's dot: in a Euclidean metric e12 • e12 is +1, where the scalar
/// part of e12 e12, which many libraries call the scalar or inner product,
/// is -1. dot(x, x) is the squared bulk norm of x. Never checks.
///
/// Requires: a and b have the same algebra and the same scalar type, and the
/// scalar holds every exact coefficient of the terms the dot keeps, as for
/// bulk; otherwise the call is rejected at compile time with a message
/// that names dot. A scalar that holds those but not every coefficient of
/// the whole product is accepted.
/// Guarantees: on dot_layout<Alg, A, B>, the scalar or no blade. The first
/// term is assigned, not added to zero, so a lone term of -0.0 gives -0.0.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, dot_layout<Alg, A, B>, T>
dot(const mv<Alg, A, T>& a, const mv<Alg, B, T>& b) {
  constexpr const auto& kernel = detail::scalar_part_kernel<Alg, A, B>;
  constexpr bool scalar_holds_every_coefficient_of_the_dot =
      detail::kernel_held_exactly<T>(kernel);
  static_assert(scalar_holds_every_coefficient_of_the_dot,
                "dot: the scalar type cannot hold one of the dot's exact "
                "coefficients; use a scalar type that holds larger integers "
                "exactly");

  mv<Alg, dot_layout<Alg, A, B>, T> out;
  if constexpr (scalar_holds_every_coefficient_of_the_dot) {
    out.coefficients =
        detail::run_product<detail::scalar_part_kernel<Alg, A, B>,
                            dot_layout<Alg, A, B>.size()>(
            a.coefficients, reverse(b).coefficients,
            std::make_index_sequence<kernel.size()>{});
  }
  return out;
}

/// Rejects a dot whose operands are not in one algebra with one scalar type,
/// with one error that names the cause. Because it exists, a
/// requires-expression that asks whether such a call is valid is told yes;
/// ask the operand types instead. It takes only multivectors, so a dot of a
/// multivector and a number is not a valid call, and another type's own dot
/// is left alone.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> dot(const X& /*a*/, const Y& /*b*/) {
  return detail::reject_dual_operands<detail::DualSpelling::dot_function, X,
                                      Y>();
}

/// The antidot a ∘ b, the sum of a_A (left G right)_AB b_B over the blades
/// of a and b, times the antiscalar 𝟙: computed as
/// left(dot(right(a), right(b))), since the right complement is a signed
/// permutation of the blades whose inverse is the left complement.
/// antidot(x, x) is the squared weight norm of x times 𝟙; for a rigid(3)
/// point it is w² 𝟙. Never checks.
///
/// Requires: as for dot, with the message naming antidot; the check reads
/// only the terms the dot of the complements keeps.
/// Guarantees: on antidot_layout<Alg, A, B>, the antiscalar or no blade.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
GALOOT_ALWAYS_INLINE
[[nodiscard]] constexpr mv<Alg, antidot_layout<Alg, A, B>, T>
antidot(const mv<Alg, A, T>& a, const mv<Alg, B, T>& b) {
  constexpr bool scalar_holds_every_coefficient_of_the_antidot =
      detail::kernel_held_exactly<T>(
          detail::scalar_part_kernel<Alg, right_complement_layout<Alg, A>,
                                     right_complement_layout<Alg, B>>);
  static_assert(scalar_holds_every_coefficient_of_the_antidot,
                "antidot: the scalar type cannot hold one of the antidot's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_antidot) {
    return left_complement(dot(right_complement(a), right_complement(b)));
  } else {
    return {};
  }
}

/// Rejects an antidot whose operands are not in one algebra with one scalar
/// type, as for dot: a requires-expression is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> antidot(const X& /*a*/, const Y& /*b*/) {
  return detail::reject_dual_operands<detail::DualSpelling::antidot_function, X,
                                      Y>();
}

/// The projection of a onto b, b ∨ (a ∧ weight_dual(b)): the antiwedge of b
/// with the wedge of a and the weight dual of b. In rigid(3) it projects a
/// point onto a plane or a line along the plane's or the line's normal
/// direction: the point (1, 1, 7, 1) onto the plane z = 2 gives
/// (1, 1, 2, 1), and a point projected onto a unitized plane, one whose
/// normal has length 1, keeps its weight. The object comes first, as in
/// project(point, plane). Never checks.
///
/// Requires: a and b have the same algebra and the same scalar type, and the
/// scalar holds every exact coefficient of the weight dual of b, as for
/// weight_dual (the wedge and the antiwedge have coefficients +1, -1, and 0,
/// which every scalar holds); otherwise the call is rejected at compile time
/// with a message that names project.
/// Guarantees: on project_layout<Alg, A, B>.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
GALOOT_ALWAYS_INLINE
[[nodiscard]] constexpr mv<Alg, project_layout<Alg, A, B>, T>
project(const mv<Alg, A, T>& a, const mv<Alg, B, T>& b) {
  constexpr bool scalar_holds_every_coefficient_of_the_projection =
      detail::weight_coefficients_exact<Alg, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_projection,
                "project: the scalar type cannot hold one of the projection's "
                "exact coefficients; use a scalar type that holds larger "
                "integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_projection) {
    return antiwedge(b, wedge(a, weight_dual(b)));
  } else {
    return {};
  }
}

/// Rejects a projection whose operands are not in one algebra with one
/// scalar type, as for dot: a requires-expression is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> project(const X& /*a*/, const Y& /*b*/) {
  return detail::reject_dual_operands<detail::DualSpelling::project_function, X,
                                      Y>();
}

/// The antiprojection of a onto b, b ∧ (a ∨ weight_dual(b)): the wedge of b
/// with the antiwedge of a and the weight dual of b. In rigid(3) it carries
/// a onto an object through b: a plane antiprojected onto a unitized point
/// is the plane through the point with the same normal, and a line
/// antiprojected onto a unitized point is the line through the point with
/// the same direction. The object comes first, as in antiproject(plane,
/// point). The result is not unitized: onto a point of weight w it is w²
/// times the result onto the unitized point. Never checks.
///
/// Requires: as for project, with the message naming antiproject.
/// Guarantees: on antiproject_layout<Alg, A, B>, canonical; read it back
/// under a named layout, as Plane{antiproject(g, p)}.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
GALOOT_ALWAYS_INLINE
[[nodiscard]] constexpr mv<Alg, antiproject_layout<Alg, A, B>, T>
antiproject(const mv<Alg, A, T>& a, const mv<Alg, B, T>& b) {
  constexpr bool scalar_holds_every_coefficient_of_the_antiprojection =
      detail::weight_coefficients_exact<Alg, B, T>;
  static_assert(scalar_holds_every_coefficient_of_the_antiprojection,
                "antiproject: the scalar type cannot hold one of the "
                "antiprojection's exact coefficients; use a scalar type that "
                "holds larger integers exactly");
  if constexpr (scalar_holds_every_coefficient_of_the_antiprojection) {
    return wedge(b, antiwedge(a, weight_dual(b)));
  } else {
    return {};
  }
}

/// Rejects an antiprojection whose operands are not in one algebra with one
/// scalar type, as for dot: a requires-expression is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> antiproject(const X& /*a*/, const Y& /*b*/) {
  return detail::reject_dual_operands<
      detail::DualSpelling::antiproject_function, X, Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_DUAL_HPP
