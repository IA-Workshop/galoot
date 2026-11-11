#ifndef GALOOT_CORE_LINEAR_HPP
#define GALOOT_CORE_LINEAR_HPP

// The operations of the vector space of multivectors: the sum and the
// difference of two multivectors of one algebra, as the operators + and -,
// the product and quotient of a multivector by a scalar, as * and /, and
// the negation of a multivector, as unary -.
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
//   Line d = 2.0 * a;  // or a * 2.0: a Line, 2 on e43
//   Line e = a / 4.0;  // 0.25 on e43
//   Line f = -a;  // -1 on e43
//
// Result layouts: a sum or a difference of two values of one type is that
// type, so a + b above is a Line, with the line's orientations, and so is a
// multiple, a quotient, or the negation of a. A sum or a difference of two
// different layouts is the canonical set of the blades either holds, each
// once with orientation +1, in order of grade and then of index list, as
// for a product: its type is mv<Alg, sum_layout<Alg, A, B>, T>. Convert it
// to a named type that holds those blades, Type{m}, to read it under that
// type's orientations.
//
// Both operands must be mv types of the same algebra and the same scalar
// type; two mv types that differ in either are rejected at compile time with
// a message that names the mistake. The rejection is an overload of its own,
// so a requires-expression that asks whether such a sum or difference is
// valid is told yes; ask the operand types instead. A multivector and a
// number have no sum: x + 1.0 and 1.0 - x are rejected the same way, by an
// overload of their own, so a requires-expression that asks is told yes.
//
// The scalar of s * a, a * s, and a / s must have the multivector's scalar
// type exactly: 2 * a for a multivector over double is rejected with a
// message that says to write 2.0, and 2.5 * a for one over std::int64_t is
// rejected rather than truncated. The rejection is again an overload of its
// own, for any ring scalar of another type, so a requires-expression that
// asks whether 2 * a is valid is told yes.
//
// A class derived from mv is accepted as its mv base, and the result is the
// mv type, not the derived class.
//
// Each result coefficient is computed by the scalar's own operations, one
// per blade (at most one + or - for a sum, one * for a multiple, one / for a
// quotient), so these operations round only as the scalar does.
//
// Compile cost: a sum or a difference of two layouts merges them once for
// each pair of layouts, a constant evaluation whose cost grows with the
// square of their sizes, so the compilers' limits bound it below the size
// of a multivector. Clang's -fconstexpr-steps and GCC's -fconstexpr-ops-limit
// raise those limits, sum_layout's comment gives the workaround, and the
// sizes measured on each compiler are in docs/guide/limits.md, under
// 'Compile-time limits: blade sets'. A sum of one type, a scaling, and a
// negation cost no constant evaluation beyond the multivector's own.

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>
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

/// The layout of x + y and x - y for x on the layout A and y on the layout B
/// in the algebra Alg. For two different layouts: every blade that A or B
/// holds, each once with orientation +1, in order of grade and, within a
/// grade, of index list (e12, e13, e14, e23, ...); a rigid(3) line and the
/// antiscalar give e12, e13, e14, e23, e24, e34, and e1234. For one layout,
/// A itself, with its order and orientations, so a sum of two lines is a
/// line, and nothing is merged.
///
/// Requires: Alg is metric_like, and neither layout holds a mask twice. The
/// blades are merged by insertion, so the compile cost grows with the square
/// of the two sizes, and the compilers' limits on constant evaluation bound
/// a sum of two layouts below the size of a multivector. Clang's
/// -fconstexpr-steps and GCC's -fconstexpr-ops-limit raise those limits, and
/// the sizes measured on each compiler, with how a failure is reported, are
/// in docs/guide/limits.md, under 'Compile-time limits: blade sets'.
/// Where one layout holds every blade of the other, convert the smaller
/// operand to the larger one's type first, as in x + X{y} for x of type X:
/// a sum of one type merges nothing.
template <metric_like Alg, blade_set A, blade_set B>
inline constexpr auto sum_layout =
    merge<detail::with_plus_signs(A), detail::with_plus_signs(B)>;

/// sum_layout of one layout: that layout.
template <metric_like Alg, blade_set A>
inline constexpr auto sum_layout<Alg, A, A> = A;

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
          std::size_t NX, std::size_t NY, std::size_t... Xs, std::size_t... Ys>
GALOOT_ALWAYS_INLINE constexpr std::array<T, R>
run_sum(const std::array<T, NX>& x, const std::array<T, NY>& y,
        std::index_sequence<Xs...> /*x_terms*/,
        std::index_sequence<Ys...> /*y_terms*/) {
  // Not out{}, which GCC can compile wrongly; zero_places says why.
  std::array<T, R> out = zero_places<T>(std::make_index_sequence<R>{});
  (accumulate<KX[Xs].coefficient, KX[Xs].first>(out[KX[Xs].out], x[KX[Xs].a]),
   ...);
  (accumulate<KY[Ys].coefficient, KY[Ys].first>(out[KY[Ys].out], y[KY[Ys].a]),
   ...);
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
GALOOT_ALWAYS_INLINE [[nodiscard]] constexpr mv<Alg, Target, T>
sum(const mv<Alg, A, T>& x, const mv<Alg, B, T>& y) {
  mv<Alg, Target, T> out;
  out.coefficients =
      run_sum<identity_kernel<A, Target>,
              second_operand_kernel<A, B, Target, Sign>, Target.size()>(
          x.coefficients, y.coefficients,
          std::make_index_sequence<identity_kernel<A, Target>.size()>{},
          std::make_index_sequence<B.size()>{});
  return out;
}

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

/// x + y for two multivectors of one algebra and scalar type, on
/// sum_layout<Alg, A, B>: on each of its blades, the coefficients the two
/// operands hold there, each times its orientation, added, and a blade that
/// one operand alone holds takes its coefficient alone. Two values of one
/// type give that type, place by place. A rigid(3) line plus the antiscalar
/// holds the line's -vx on e14, since e41 = -e14, and the antiscalar's on
/// e1234.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
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

/// x - y for two multivectors of one algebra and scalar type, on
/// sum_layout<Alg, A, B> as for x + y: a blade that both hold takes x's
/// coefficient minus y's, each times its orientation, a blade that x alone
/// holds takes x's, and one that y alone holds takes the negation of y's.
/// Two values of one type give that type, place by place.
template <metric_like Alg, blade_set A, blade_set B, ring_scalar T>
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

namespace detail {

// Whether S is a number: a ring scalar that is not a multivector or derived
// from one. What the rejecting sums below take beside a multivector.
template <class S>
concept number_operand = !multivector_or_derived<S> && ring_scalar<S>;

} // namespace detail

/// Rejects x + s and s + x for a multivector x, or a class derived from one,
/// and a number s of any ring scalar type, with one error: a multivector and
/// a number have no sum. To add s, make it a multivector on the scalar
/// blade, mv<Alg, blades<Alg, "1">, T>{s}, and add that. Because these exist,
/// a requires-expression that asks whether x + 1.0 is valid is told yes.
template <metric_like Alg, blade_set B, ring_scalar T, class S>
  requires detail::number_operand<S>
constexpr mv<Alg, B, T> operator+(const mv<Alg, B, T>& /*x*/, const S& /*s*/) {
  static_assert(false, "a + b: a multivector and a number have no sum; make "
                       "the number a multivector on the scalar blade 1 and "
                       "add that");
  return {};
}

/// Rejects s + x, as x + s above.
template <class S, metric_like Alg, blade_set B, ring_scalar T>
  requires detail::number_operand<S>
constexpr mv<Alg, B, T> operator+(const S& /*s*/, const mv<Alg, B, T>& /*x*/) {
  static_assert(false, "a + b: a multivector and a number have no sum; make "
                       "the number a multivector on the scalar blade 1 and "
                       "add that");
  return {};
}

/// Rejects x - s and s - x for a multivector x and a number s, as for x + s.
template <metric_like Alg, blade_set B, ring_scalar T, class S>
  requires detail::number_operand<S>
constexpr mv<Alg, B, T> operator-(const mv<Alg, B, T>& /*x*/, const S& /*s*/) {
  static_assert(false, "a - b: a multivector and a number have no "
                       "difference; make the number a multivector on the "
                       "scalar blade 1 and subtract that");
  return {};
}

/// Rejects s - x, as x - s above.
template <class S, metric_like Alg, blade_set B, ring_scalar T>
  requires detail::number_operand<S>
constexpr mv<Alg, B, T> operator-(const S& /*s*/, const mv<Alg, B, T>& /*x*/) {
  static_assert(false, "a - b: a multivector and a number have no "
                       "difference; make the number a multivector on the "
                       "scalar blade 1 and subtract that");
  return {};
}

namespace detail {

// Whether S, a scalar that is not a multivector or derived from one, is a
// ring scalar of another type than the scalar type of the multivector type
// X: what the rejecting overloads of the scalings take.
template <class S, class X>
concept scalar_of_another_type =
    multivector<X> && !multivector_or_derived<S> && ring_scalar<S> &&
    !std::same_as<S, typename std::remove_cvref_t<X>::scalar_type>;

// Whether a / b is a T for two values of T.
template <class T>
concept scalar_has_division = requires(const T& a, const T& b) {
  { a / b } -> std::convertible_to<T>;
};

} // namespace detail

/// s * x: each coefficient of x multiplied by s on the left, s x_i, so that
/// a scalar whose product does not commute, such as a matrix, keeps its
/// order. The result has x's type.
///
/// Requires: s has x's scalar type T exactly; a scalar of another type is
/// rejected, by the overload below.
template <metric_like Alg, blade_set A, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, A, T> operator*(const T& s,
                                                const mv<Alg, A, T>& x) {
  mv<Alg, A, T> out;
  for (std::size_t i = 0; i < A.size(); ++i) {
    out.coefficients[i] = static_cast<T>(s * x.coefficients[i]);
  }
  return out;
}

/// x * s: each coefficient of x multiplied by s on the right, x_i s. The
/// result has x's type.
///
/// Requires: s has x's scalar type T exactly, as for s * x.
template <metric_like Alg, blade_set A, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, A, T> operator*(const mv<Alg, A, T>& x,
                                                const T& s) {
  mv<Alg, A, T> out;
  for (std::size_t i = 0; i < A.size(); ++i) {
    out.coefficients[i] = static_cast<T>(x.coefficients[i] * s);
  }
  return out;
}

/// x / s: each coefficient of x divided by s with T's own division, x_i / s,
/// never multiplied by a reciprocal of s, which would round twice: exact for
/// an exact rational type, correctly rounded per coefficient for double, and
/// truncated toward zero for a built-in integer type, so 7 / 2 is 3 and -7 /
/// 2 is -3. The result has x's type.
///
/// Requires: s has x's scalar type T exactly, as for s * x, and T has a /
/// whose result converts to T; for a T without one, a requires-expression
/// that asks whether x / s is valid answers no, and a call is the
/// compiler's own error. For an integer T, s is not zero, whatever its sign
/// or width: checked by GALOOT_ASSERT in every constant evaluation, which
/// then fails, and at run time when GALOOT_CHECKING is 1, which then aborts
/// with the message; with GALOOT_CHECKING 0 the division by zero is
/// undefined behavior. An integer T here is one whose std::numeric_limits
/// specialization describes an integer type with its range (is_specialized
/// and is_integer, with is_signed and digits, and min() and max() that
/// convert implicitly to std::int64_t and std::uint64_t) and which has ==.
/// A type whose limits say less, whose min() and max() convert only
/// explicitly, as a big-integer class's often do, or which has no ==, is
/// not checked, and its own / decides what a zero divisor does. A floating
/// T is not checked: at run time IEEE 754 defines its division by zero, and
/// in a constant evaluation a division of a built-in floating type by zero
/// is not a constant expression, so a constant x / 0.0 fails with the
/// compiler's own error; a scalar of the user's decides for itself.
/// Otherwise the scalar's own arithmetic decides: over a signed integer
/// type at least as wide as int, min() / -1 overflows, which is undefined
/// at run time and fails a constant evaluation, as an overflowing sum does;
/// over short or signed char the quotient is computed in int and converted
/// back, giving min() again.
template <metric_like Alg, blade_set A, ring_scalar T>
  requires detail::scalar_has_division<T>
[[nodiscard]] constexpr mv<Alg, A, T> operator/(const mv<Alg, A, T>& x,
                                                const T& s) {
  if constexpr (detail::integer_limits<T> && std::equality_comparable<T>) {
    GALOOT_ASSERT(s != T{}, "a / s: an integer scalar cannot divide by zero");
  }
  mv<Alg, A, T> out;
  for (std::size_t i = 0; i < A.size(); ++i) {
    out.coefficients[i] = static_cast<T>(x.coefficients[i] / s);
  }
  return out;
}

/// -x: each coefficient of x negated by T's unary minus, so that over IEEE
/// 754 a +0.0 becomes -0.0, which 0.0 - x_i would not give. The result has
/// x's type.
template <metric_like Alg, blade_set A, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, A, T> operator-(const mv<Alg, A, T>& x) {
  mv<Alg, A, T> out;
  for (std::size_t i = 0; i < A.size(); ++i) {
    out.coefficients[i] = static_cast<T>(-x.coefficients[i]);
  }
  return out;
}

/// Rejects s * x for a ring scalar s of another type than x's scalar type,
/// such as 2 for a multivector over double, with one error. Because it
/// exists, a requires-expression that asks whether s * x is valid is told
/// yes for any ring scalar; ask whether the two types are the same instead.
/// A volatile scalar is of another type, volatile T; copy it to a T first.
template <class S, class X>
  requires detail::scalar_of_another_type<S, X>
constexpr X operator*(const S& /*s*/, const X& x) {
  static_assert(std::is_same_v<S, typename X::scalar_type>,
                "s * a: the scalar is not of the multivector's scalar type, "
                "the second of the two types shown; convert it to that type "
                "first, as 2.0 for 2 over double or std::int64_t{2} over "
                "std::int64_t");
  return x;
}

/// Rejects x * s for a ring scalar s of another type than x's scalar type,
/// as for s * x: a requires-expression is told yes.
template <class X, class S>
  requires detail::scalar_of_another_type<S, X>
constexpr X operator*(const X& x, const S& /*s*/) {
  static_assert(std::is_same_v<S, typename X::scalar_type>,
                "a * s: the scalar is not of the multivector's scalar type, "
                "the second of the two types shown; convert it to that type "
                "first, as 2.0 for 2 over double or std::int64_t{2} over "
                "std::int64_t");
  return x;
}

/// Rejects x / s for a ring scalar s of another type than x's scalar type,
/// as for s * x: a requires-expression is told yes.
template <class X, class S>
  requires detail::scalar_of_another_type<S, X>
constexpr X operator/(const X& x, const S& /*s*/) {
  static_assert(std::is_same_v<S, typename X::scalar_type>,
                "a / s: the scalar is not of the multivector's scalar type, "
                "the second of the two types shown; convert it to that type "
                "first, as 2.0 for 2 over double or std::int64_t{2} over "
                "std::int64_t");
  return x;
}

} // namespace galoot::core

#endif // GALOOT_CORE_LINEAR_HPP
