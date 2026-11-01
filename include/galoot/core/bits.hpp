#ifndef GALOOT_CORE_BITS_HPP
#define GALOOT_CORE_BITS_HPP

// Functions on blade masks: the grade, the sign of reordering a product of
// basis blades, complements, and the signs of the grade involutions. Each is
// constexpr, so the same function serves generation at compile time and use
// at run time.
//
// Generators are e_1 to e_n, counting from 1. A basis blade is named by its
// mask, in which bit i - 1 stands for e_i, and the mask's blade lists its
// generators in increasing order: the mask 0b1011 is e_124. The antiscalar is
// +e_12...n.
//
// A function that takes the number of generators n requires
// 1 <= n <= mask_bits and a mask with no bit at or above n, checked by
// GALOOT_ASSERT. The assertion messages state the precondition but not the
// function, because a message must fit on the line of its GALOOT_ASSERT to
// appear in a compile-time diagnostic; the diagnostic's call trace, or the file
// and line of a run-time report, names the function. A smaller limit on n
// belongs where an algebra is declared, so that raising it leaves these
// functions unchanged.

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

#include <galoot/core/assert.hpp>

namespace galoot::core {

/// The mask of a basis blade: bit i - 1 stands for the generator e_i.
///
/// Guarantees: unsigned, so shifts and complements are defined for every bit.
using mask_t = std::uint32_t;

/// The number of generators a mask can hold.
inline constexpr std::size_t mask_bits = std::numeric_limits<mask_t>::digits;

// The assertion messages below are literals that name this bound.
static_assert(mask_bits == 32, "update the messages that say 1 to 32");

namespace detail {

/// The mask of e_1 to e_n, that is, of the antiscalar.
///
/// Requires: 1 <= n <= mask_bits; the caller checks it.
[[nodiscard]] constexpr mask_t low_bits(std::size_t n) noexcept {
  // A right shift of all ones, because 1 << n is undefined at n = mask_bits.
  return ~mask_t{0} >> (mask_bits - n);
}

/// Whether n is a number of generators these functions accept.
[[nodiscard]] constexpr bool valid_count(std::size_t n) noexcept {
  return n >= 1 && n <= mask_bits;
}

/// Whether every generator of `a` is one of e_1 to e_n.
///
/// Requires: valid_count(n).
[[nodiscard]] constexpr bool in_algebra(mask_t a, std::size_t n) noexcept {
  return (a & ~low_bits(n)) == 0;
}

} // namespace detail

/// The grade of the blade of `a`: its number of generators.
///
/// Requires: nothing; every mask is a blade of some algebra.
/// Guarantees: 0 <= grade(a) <= mask_bits. Grades are int, the type of
/// std::popcount, so that sign formulas such as k(k - 1)/2 and differences of
/// grades need no unsigned arithmetic.
[[nodiscard]] constexpr int grade(mask_t a) noexcept {
  return std::popcount(a);
}

/// The antigrade of the blade of `a` in an algebra of n generators: n minus its
/// grade.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT.
/// Guarantees: 0 <= antigrade(a, n) <= n.
[[nodiscard]] constexpr int antigrade(mask_t a, std::size_t n) noexcept {
  GALOOT_ASSERT(detail::valid_count(n), "n must be from 1 to 32");
  GALOOT_ASSERT(detail::in_algebra(a, n), "blade has a generator beyond e_n");
  // Exact, because n <= mask_bits once the checks hold.
  return static_cast<int>(n) - grade(a);
}

/// The sign of reordering e_A e_B into increasing order: -1 when the number of
/// pairs of a generator e_i of A and a generator e_j of B with i > j is odd,
/// and +1 when it is even.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1. For A and B with no common generator, the
/// exterior product is e_A ∧ e_B = reorder_sign(a, b) e_(A ∪ B). Pairs with
/// i = j are not counted, so in a diagonal metric the geometric product is
/// e_A e_B = reorder_sign(a, b) times the squares of the common generators
/// times e_(A xor B). The cost is one step per generator of A, whatever the
/// width of the masks.
[[nodiscard]] constexpr int reorder_sign(mask_t a, mask_t b) noexcept {
  // For each generator e_i of A, lowest first, count the generators of B
  // below it.
  int swaps = 0;
  for (; a != 0; a &= a - 1) {
    const auto i = static_cast<unsigned>(std::countr_zero(a));
    swaps += std::popcount(b & ((mask_t{1} << i) - 1U));
  }
  return swaps % 2 == 0 ? 1 : -1;
}

/// The mask of the right and of the left complement of the blade of `a` in an
/// algebra of n generators: the generators of e_1 to e_n that `a` lacks.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT.
/// Guarantees: a | complement_mask(a, n) is the antiscalar's mask and
/// a & complement_mask(a, n) is 0. Applied twice, it returns `a`.
[[nodiscard]] constexpr mask_t complement_mask(mask_t a,
                                               std::size_t n) noexcept {
  GALOOT_ASSERT(detail::valid_count(n), "n must be from 1 to 32");
  GALOOT_ASSERT(detail::in_algebra(a, n), "blade has a generator beyond e_n");
  return ~a & detail::low_bits(n);
}

/// The sign s of the right complement s e_C of the blade u = e_A of `a` in an
/// algebra of n generators, where C is complement_mask(a, n). The right
/// complement is defined by u ∧ right(u) = +e_12...n.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in complement_mask.
/// Guarantees: returns -1 or +1, with u ∧ (s e_C) = +e_12...n.
[[nodiscard]] constexpr int right_complement_sign(mask_t a,
                                                  std::size_t n) noexcept {
  // e_A ∧ e_C = reorder_sign(A, C) e_12...n, and the sign is its own inverse.
  return reorder_sign(a, complement_mask(a, n));
}

/// The sign s of the left complement s e_C of the blade u = e_A of `a` in an
/// algebra of n generators, where C is complement_mask(a, n). The left
/// complement is defined by left(u) ∧ u = +e_12...n.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in complement_mask.
/// Guarantees: returns -1 or +1, with (s e_C) ∧ u = +e_12...n. It equals
/// (-1)^(gr ag) times the right complement's sign, and the left complement of
/// the right complement of u is u.
[[nodiscard]] constexpr int left_complement_sign(mask_t a,
                                                 std::size_t n) noexcept {
  return reorder_sign(complement_mask(a, n), a);
}

/// The sign by which the reverse multiplies the blade of `a`:
/// (-1)^(k(k-1)/2) for its grade k.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1.
[[nodiscard]] constexpr int reverse_sign(mask_t a) noexcept {
  const int k = grade(a);
  return (k * (k - 1) / 2) % 2 == 0 ? 1 : -1;
}

/// The sign by which the antireverse multiplies the blade of `a` in an algebra
/// of n generators: (-1)^(m(m-1)/2) for its antigrade m, the reverse's sign
/// with the antigrade in place of the grade.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in antigrade.
/// Guarantees: returns -1 or +1.
[[nodiscard]] constexpr int antireverse_sign(mask_t a, std::size_t n) noexcept {
  const int m = antigrade(a, n);
  return (m * (m - 1) / 2) % 2 == 0 ? 1 : -1;
}

/// The sign by which the grade involution multiplies the blade of `a`:
/// (-1)^k for its grade k.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1.
[[nodiscard]] constexpr int grade_involution_sign(mask_t a) noexcept {
  return grade(a) % 2 == 0 ? 1 : -1;
}

} // namespace galoot::core

#endif // GALOOT_CORE_BITS_HPP
