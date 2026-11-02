#ifndef GALOOT_CORE_UNARY_HPP
#define GALOOT_CORE_UNARY_HPP

// The involutions and the complements of a multivector, as functions and as
// the operators ~ (the reverse) and ! (the right complement).
//
//   using Line = galoot::core::mv<rga3, line_layout, double>;
//   Line l{0, 0, 1, 0, 0, 0};
//   Line r = galoot::core::reverse(l);  // or ~l: a Line, -l
//   auto c = galoot::core::right_complement(l);  // or !l
//
// The reverse, the antireverse, and the grade involution multiply each blade
// by a sign that depends only on its grade, so they return the input's type:
// ~line is a Line, with the line's orientations. The complements send each
// blade to another blade, so their result is the canonical set of the blades
// they reach, each with orientation +1, in order of grade and then of index
// list, as for a product: convert it to a named layout (Plane{!point}) to
// read it under that layout's orientations.
//
// Each operation reads only the number of generators of the algebra, never
// its metric. Compile cost: one map for each layout an operation is applied
// to, with one term for each blade of the layout.

#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/plan.hpp>

namespace galoot::core {

/// The layout of the right or the left complement of an mv on the layout S
/// in the algebra Alg: the complements of the blades of S, each with
/// orientation +1, in order of grade and then of index list. The complements
/// declare their result types through these, so a program can name them.
// clang-tidy reads the operation, once substituted, as a C-style cast of its
// value, which the source does not have.
// NOLINTBEGIN(modernize-avoid-c-style-cast)
template <metric_like Alg, blade_set S>
inline constexpr auto right_complement_layout =
    detail::unary_map<detail::UnaryOperation::right_complement, Alg, S>.result;
template <metric_like Alg, blade_set S>
inline constexpr auto left_complement_layout =
    detail::unary_map<detail::UnaryOperation::left_complement, Alg, S>.result;
// NOLINTEND(modernize-avoid-c-style-cast)

/// The reverse of x: each blade of grade k times (-1)^(k(k-1)/2), which
/// reverses the order of its generators. On the input's layout.
///
/// Under `using namespace std::ranges`, an unqualified reverse(x) finds the
/// function object std::ranges::reverse, which turns off argument-dependent
/// lookup, so it does not compile; write galoot::core::reverse(x) or ~x.
/// Under `using namespace std`, reverse(x) finds this function, since
/// std::reverse takes two iterators.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, S, T> reverse(const mv<Alg, S, T>& x) {
  return detail::apply<detail::UnaryOperation::reverse, S>(x);
}

/// The antireverse of x: each blade of antigrade m = n - k times
/// (-1)^(m(m-1)/2), the reverse with the antigrade in place of the grade. On
/// the input's layout.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, S, T> antireverse(const mv<Alg, S, T>& x) {
  return detail::apply<detail::UnaryOperation::antireverse, S>(x);
}

/// The grade involution of x: each blade of grade k times (-1)^k. On the
/// input's layout.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, S, T> grade_involution(const mv<Alg, S, T>& x) {
  return detail::apply<detail::UnaryOperation::grade_involution, S>(x);
}

/// The right complement of x: each basis blade u goes to the blade right(u)
/// fixed by u ∧ right(u) = 𝟙, the antiscalar e12...n, extended linearly. On
/// right_complement_layout<Alg, S>. Applied twice it gives each blade times
/// (-1)^(gr ag), its grade times its antigrade, not x; left_complement
/// undoes it.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, right_complement_layout<Alg, S>, T>
right_complement(const mv<Alg, S, T>& x) {
  return detail::apply<detail::UnaryOperation::right_complement,
                       right_complement_layout<Alg, S>>(x);
}

/// The left complement of x: each basis blade u goes to the blade left(u)
/// fixed by left(u) ∧ u = 𝟙, extended linearly, which is (-1)^(gr ag)
/// right(u); the two agree when n is odd. On left_complement_layout<Alg, S>.
/// left_complement(right_complement(x)) and right_complement(
/// left_complement(x)) are x.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, left_complement_layout<Alg, S>, T>
left_complement(const mv<Alg, S, T>& x) {
  return detail::apply<detail::UnaryOperation::left_complement,
                       left_complement_layout<Alg, S>>(x);
}

/// ~x is reverse(x).
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, S, T> operator~(const mv<Alg, S, T>& x) {
  return reverse(x);
}

/// !x is right_complement(x), a multivector, not a bool: if (!x) does not
/// compile. !!x is x with each blade times (-1)^(gr ag), not x.
template <metric_like Alg, blade_set S, ring_scalar T>
[[nodiscard]] constexpr mv<Alg, right_complement_layout<Alg, S>, T>
operator!(const mv<Alg, S, T>& x) {
  return right_complement(x);
}

} // namespace galoot::core

#endif // GALOOT_CORE_UNARY_HPP
