#ifndef GALOOT_CORE_DUAL_HPP
#define GALOOT_CORE_DUAL_HPP

// The bulk and the weight of a multivector, in the sense of Lengyel's rigid
// and conformal geometric algebra:
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto point_layout = galoot::core::blades<rga3, "e1 e2 e3 e4">;
//
//   using Point = galoot::core::mv<rga3, point_layout, double>;
//   Point p{1, 2, 3, 1};
//   auto b = galoot::core::bulk(p);  // e1 e2 e3: 1, 2, 3
//   auto w = galoot::core::weight(p);  // e4: 1
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
//
// In rigid(3), where e1, e2, e3 square to 1 and e4 to 0, the bulk is the part
// without e4 and the weight the part with it, so bulk(x) + weight(x) is x.
// In other metrics the bulk and the weight need not split x:
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
// line's orientations. Each result type is mv<Alg, bulk_layout<Alg, S>, T>, and
// likewise for the other operations.
//
// Nothing here checks anything: the bulk and the weight drop blades by
// definition.
//
// The names bulk and weight are common words. A variable of one of
// these names in scope stops an unqualified call of the function, as for any
// function found by argument-dependent lookup; write galoot::core::bulk(x).
// C++26's std::execution::bulk is a function object, so under
// `using namespace std::execution` an unqualified bulk(x) finds it and does
// not compile.

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/unary.hpp>

namespace galoot::core {

namespace detail {

// The antiscalar e12...n of the algebra Alg, as a layout of one blade.
template <metric_like Alg>
inline constexpr blade_set<1> antiscalar_layout{
    {{{.mask = low_bits(Alg::value.generator_count), .sign = 1}}}};

// The antiscalar with coefficient 1, the identity of the geometric
// antiproduct.
template <metric_like Alg, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, antiscalar_layout<Alg>, T> unit_antiscalar() {
  return mv<Alg, antiscalar_layout<Alg>, T>{
      static_cast<T>(coefficient_type{1})};
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
[[nodiscard]] constexpr mv<Alg, weight_layout<Alg, S>, T>
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

} // namespace galoot::core

#endif // GALOOT_CORE_DUAL_HPP
