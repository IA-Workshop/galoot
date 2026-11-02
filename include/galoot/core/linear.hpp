#ifndef GALOOT_CORE_LINEAR_HPP
#define GALOOT_CORE_LINEAR_HPP

// The operations of the vector space of multivectors: the sum and the
// difference of two multivectors of one algebra, as the operators + and -.
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//   constexpr auto line_layout =
//       galoot::core::blades<rga3, "e41 e42 e43 e23 e31 e12">;
//   constexpr auto antiscalar_layout = galoot::core::blades<rga3, "e1234">;
//
//   using Line = galoot::core::mv<rga3, line_layout, double>;
//   using Antiscalar = galoot::core::mv<rga3, antiscalar_layout, double>;
//   Line a{0, 0, 1, 0, 0, 0};
//   Line b{1, 0, 0, 0, 0, 0};
//   Line c = a + b;  // a Line: 1 on e41 and 1 on e43
//   auto m = a + Antiscalar{1};  // e12 e13 e14 e23 e24 e34 e1234
//   double on_e34 = galoot::core::get<"e34">(m);  // -1, since e43 = -e34
//
// Result layouts: a sum or a difference of two values of one type is that
// type, so a + b above is a Line, with the line's orientations. Of two
// different layouts, it is the canonical set of the blades either holds,
// each once with orientation +1, in order of grade and then of index list,
// as for a product: its type is mv<Alg, sum_layout<Alg, A, B>, T>. Convert
// it to a named type that holds those blades, Type{m}, to read it under
// that type's orientations.
//
// Both operands must be mv types of the same algebra and the same scalar
// type; two mv types that differ in either are rejected at compile time with
// a message that names the mistake. The rejection is an overload of its own,
// so a requires-expression that asks whether such a sum or difference is
// valid is told yes; ask the operand types instead. A multivector and a
// number have no sum. A class derived from mv is accepted as its mv base,
// and the result is the mv type, not the derived class.
//
// Each result coefficient is computed by the scalar's own + and -, at most
// one of them per blade, so these operations round only as the scalar does.

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

namespace galoot::core {

namespace detail {

// The blades of `set` with orientation +1.
template <std::size_t K>
[[nodiscard]] constexpr blade_set<K>
with_plus_signs(const blade_set<K>& set) noexcept {
  blade_set<K> out = set;
  for (oriented_blade& blade : out.blades) {
    blade.sign = 1;
  }
  return out;
}

} // namespace detail

/// The layout of x + y and x - y for x on the layout A and y on a different
/// layout B in the algebra Alg: every blade that A or B holds, each once with
/// orientation +1, in order of grade and, within a grade, of index list
/// (e12, e13, e14, e23, ...). A rigid(3) line and the antiscalar give e12,
/// e13, e14, e23, e24, e34, and e1234. A sum of two values of one layout
/// keeps that layout instead, which this set holds in canonical order.
///
/// Requires: Alg is metric_like, and neither layout holds a mask twice. The
/// blades are merged by insertion, so the compile cost grows with the square
/// of the two sizes; the limits that merge states for its constant
/// evaluation apply.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto sum_layout =
    merge<detail::with_plus_signs(A), detail::with_plus_signs(B)>;

namespace detail {

// The terms that add y, on the layout B, to x, on the layout A, both written
// onto Target: one for each blade of B, with the coefficient Sign s_b s_t,
// where Sign is +1 for a sum and -1 for a difference, s_b is the blade's
// orientation in B, and s_t its orientation in Target. A term on a blade
// that A also holds follows x's term there and is added; a term on a blade
// that only B holds is the first there and is assigned.
template <blade_set A, blade_set B, blade_set Target, coefficient_type Sign>
[[nodiscard]] consteval auto make_second_operand_kernel() noexcept {
  std::array<kernel_term, B.size()> terms{};
  for (std::size_t k = 0; k < B.size(); ++k) {
    const std::size_t at = position_of(Target, B.blades[k].mask);
    terms[k] = {
        .a = k,
        .b = 0,
        .out = at,
        .coefficient = Sign * B.blades[k].sign * Target.blades[at].sign,
        .first = position_of(A, B.blades[k].mask) == A.size(),
    };
  }
  return terms;
}

template <blade_set A, blade_set B, blade_set Target, coefficient_type Sign>
inline constexpr auto second_operand_kernel =
    make_second_operand_kernel<A, B, Target, Sign>();

// The R coefficients of x + y or x - y: the terms of KX copy x onto the
// result, each the first on its blade, and then the terms of KY add or
// subtract y, or assign it where x has no blade. One statement per term,
// with every position and coefficient a constant, as for run_map.
// NOLINTBEGIN(readability-redundant-parentheses): as for run_map.
template <const auto& KX, const auto& KY, std::size_t R, class T,
          std::size_t NX, std::size_t NY, std::size_t... I, std::size_t... J>
constexpr std::array<T, R> run_sum(const std::array<T, NX>& x,
                                   const std::array<T, NY>& y,
                                   std::index_sequence<I...> /*x_terms*/,
                                   std::index_sequence<J...> /*y_terms*/) {
  std::array<T, R> out{};
  (accumulate<KX[I].coefficient, KX[I].first>(out[KX[I].out], x[KX[I].a]), ...);
  (accumulate<KY[J].coefficient, KY[J].first>(out[KY[J].out], y[KY[J].a]), ...);
  return out;
}
// NOLINTEND(readability-redundant-parentheses)

// x + y (Sign +1) or x - y (Sign -1) on the layout Target, which holds every
// blade of A and of B. A blade that only one operand holds takes that
// operand's coefficient, or its negation, by assignment and not by an
// addition to zero, so that over IEEE 754 a -0.0 stays -0.0. A blade that
// both hold takes x's coefficient plus or minus y's, each times its
// orientation.
template <coefficient_type Sign, blade_set Target, metric_like Alg, blade_set A,
          blade_set B, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, Target, T> sum(const mv<Alg, A, T>& x,
                                               const mv<Alg, B, T>& y) {
  mv<Alg, Target, T> out;
  out.coefficients =
      run_sum<identity_kernel<A, Target>,
              second_operand_kernel<A, B, Target, Sign>, Target.size()>(
          x.coefficients, y.coefficients,
          std::make_index_sequence<identity_kernel<A, Target>.size()>{},
          std::make_index_sequence<B.size()>{});
  return out;
}

// Whether two layouts are the same template argument.
template <blade_set A, blade_set B>
inline constexpr bool same_layout_v = false;

template <blade_set A>
inline constexpr bool same_layout_v<A, A> = true;

/// The ways a sum is written, so that a message names the operation as the
/// user wrote it.
// An int, so that a compiler prints its values and not a byte.
// NOLINTNEXTLINE(performance-enum-size)
enum class SumSpelling {
  plus_operator,
  minus_operator,
};

/// The only error of a mismatched sum or difference: the message names the
/// operation as written and the cause. Each condition compares the two
/// algebras or the two scalar types themselves, so that the compiler prints
/// them. Instantiated only by the catch-all overloads, whose result type is
/// that of the first operand so that a variable initialized from the call
/// adds no second error.
template <SumSpelling Spelling, class X, class Y>
constexpr std::remove_cvref_t<X> reject_sum() {
  using R = std::remove_cvref_t<X>;
  using S = std::remove_cvref_t<Y>;
  if constexpr (!std::is_same_v<typename R::algebra_type,
                                typename S::algebra_type>) {
    if constexpr (Spelling == SumSpelling::plus_operator) {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a + b: the operands are in different algebras; give both types the "
          "same algebra tag");
    } else {
      static_assert(
          std::is_same_v<typename R::algebra_type, typename S::algebra_type>,
          "a - b: the operands are in different algebras; give both types the "
          "same algebra tag");
    }
  } else {
    if constexpr (Spelling == SumSpelling::plus_operator) {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a + b: the operands have different scalar types; give both types "
          "the same scalar type");
    } else {
      static_assert(
          std::is_same_v<typename R::scalar_type, typename S::scalar_type>,
          "a - b: the operands have different scalar types; give both types "
          "the same scalar type");
    }
  }
  return R{};
}

} // namespace detail

/// x + y for two values of one type: the coefficient of each blade is the
/// sum of the two. The result has the operands' type.
template <metric_like Alg, blade_set A, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, A, T> operator+(const mv<Alg, A, T>& x,
                                                const mv<Alg, A, T>& y) {
  return detail::sum<1, A>(x, y);
}

/// x + y for two layouts of one algebra and scalar type: on each blade of
/// sum_layout<Alg, A, B>, the coefficients the two operands hold there, each
/// times its orientation, added, and a blade that one operand alone holds
/// takes its coefficient alone. A rigid(3) line plus the antiscalar holds
/// the line's -vx on e14, since e41 = -e14, and the antiscalar's on e1234.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
  requires(!detail::same_layout_v<A, B>)
[[nodiscard]] constexpr mv<Alg, sum_layout<Alg, A, B>, T>
operator+(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return detail::sum<1, sum_layout<Alg, A, B>>(x, y);
}

/// Rejects x + y for multivectors that are not in one algebra with one
/// scalar type, with one error that names the cause. Because it exists, a
/// requires-expression that asks whether x + y is valid for them is told
/// yes; ask the operand types instead.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator+(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_sum<detail::SumSpelling::plus_operator, X, Y>();
}

/// x - y for two values of one type: the coefficient of each blade is x's
/// minus y's. The result has the operands' type.
template <metric_like Alg, blade_set A, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, A, T> operator-(const mv<Alg, A, T>& x,
                                                const mv<Alg, A, T>& y) {
  return detail::sum<-1, A>(x, y);
}

/// x - y for two layouts of one algebra and scalar type, on
/// sum_layout<Alg, A, B> as for x + y: a blade that both hold takes x's
/// coefficient minus y's, each times its orientation, a blade that x alone
/// holds takes x's, and one that y alone holds takes the negation of y's.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
  requires(!detail::same_layout_v<A, B>)
[[nodiscard]] constexpr mv<Alg, sum_layout<Alg, A, B>, T>
operator-(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  return detail::sum<-1, sum_layout<Alg, A, B>>(x, y);
}

/// Rejects x - y for multivectors that are not in one algebra with one
/// scalar type, as for x + y: a requires-expression is told yes.
template <class X, class Y>
  requires detail::mismatched_operands<X, Y>
constexpr std::remove_cvref_t<X> operator-(const X& /*x*/, const Y& /*y*/) {
  return detail::reject_sum<detail::SumSpelling::minus_operator, X, Y>();
}

} // namespace galoot::core

#endif // GALOOT_CORE_LINEAR_HPP
