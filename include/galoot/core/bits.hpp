#ifndef GALOOT_CORE_BITS_HPP
#define GALOOT_CORE_BITS_HPP

// Functions on blade masks: the grade, the sign of reordering a product of
// basis blades, complements, and the signs of the grade involutions. Each is
// constexpr, so the same function serves generation at compile time and use
// at run time.
//
// A blade is named by its mask, in which bit i - 1 stands for the generator
// e_i, and the mask's canonical blade lists its generators in increasing
// order (CONVENTIONS.md, "Generators and blades"). A function that takes the
// number of generators n requires 1 <= n <= mask_bits and a mask with no bit
// at or above n, checked by GALOOT_ASSERT. The assertion messages state the
// precondition but not the function, because a message must fit on the line of
// its GALOOT_ASSERT to appear in a compile-time diagnostic; the diagnostic's
// call trace, or the file and line of a run-time report, names the function.
//
// The library's own limit on n is enforced where an algebra is declared, so it
// can be raised without changing these functions.

#include <bit>
#include <cstdint>
#include <limits>

#include <galoot/core/assert.hpp>

namespace galoot::core {

/// The mask of a basis blade: bit i - 1 stands for the generator e_i.
///
/// Guarantees: unsigned, so shifts and complements are defined for every bit.
/// Conventions: CONVENTIONS.md, "Generators and blades".
using mask_t = std::uint32_t;

/// The number of generators a mask can hold.
inline constexpr int mask_bits = std::numeric_limits<mask_t>::digits;

// The assertion messages below are literals that name this bound.
static_assert(mask_bits == 32, "update the messages that say 1 to 32");

namespace detail {

/// The mask of e_1 to e_n, that is, of the antiscalar.
///
/// Requires: 1 <= n <= mask_bits; the caller checks it.
[[nodiscard]] constexpr mask_t low_bits(int n) noexcept {
  // A right shift of all ones, because 1 << n is undefined at n = mask_bits.
  return ~mask_t{0} >> static_cast<unsigned>(mask_bits - n);
}

/// Whether n is a number of generators these functions accept.
[[nodiscard]] constexpr bool valid_count(int n) noexcept {
  return n >= 1 && n <= mask_bits;
}

/// Whether every generator of `a` is one of e_1 to e_n.
///
/// Requires: valid_count(n).
[[nodiscard]] constexpr bool in_algebra(mask_t a, int n) noexcept {
  return (a & ~low_bits(n)) == 0;
}

} // namespace detail

/// The grade of the blade of `a`: its number of generators.
///
/// Requires: nothing; every mask is a blade of some algebra.
/// Guarantees: 0 <= grade(a) <= mask_bits.
/// Conventions: CONVENTIONS.md, "Generators and blades".
[[nodiscard]] constexpr int grade(mask_t a) noexcept {
  return std::popcount(a);
}

/// The antigrade of the blade of `a` in an algebra of n generators: n minus its
/// grade.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT.
/// Guarantees: 0 <= antigrade(a, n) <= n.
/// Conventions: the notation of CONVENTIONS.md.
[[nodiscard]] constexpr int antigrade(mask_t a, int n) noexcept {
  GALOOT_ASSERT(detail::valid_count(n), "n must be from 1 to 32");
  GALOOT_ASSERT(detail::in_algebra(a, n), "blade has a generator beyond e_n");
  return n - grade(a);
}

/// The sign of reordering e_A e_B into the canonical order: -1 when the number
/// of pairs of a generator e_i of A and a generator e_j of B with i > j is
/// odd, and +1 when it is even.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1. For A and B with no common generator,
/// e_A ∧ e_B = reorder_sign(a, b) e_(A ∪ B). For common generators, the pairs
/// with i = j are not counted, so in a diagonal metric
/// e_A e_B = reorder_sign(a, b) times the squares of the common generators
/// times e_(A xor B).
/// Conventions: CONVENTIONS.md, "Products", for the wedge.
[[nodiscard]] constexpr int reorder_sign(mask_t a, mask_t b) noexcept {
  // Shifting `a` right by one more step each time and intersecting with `b`
  // counts, for each generator of A, the generators of B below it.
  int swaps = 0;
  for (a >>= 1U; a != 0; a >>= 1U) {
    swaps += std::popcount(a & b);
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
/// Conventions: CONVENTIONS.md, "Complements and duals".
[[nodiscard]] constexpr mask_t complement_mask(mask_t a, int n) noexcept {
  GALOOT_ASSERT(detail::valid_count(n), "n must be from 1 to 32");
  GALOOT_ASSERT(detail::in_algebra(a, n), "blade has a generator beyond e_n");
  return ~a & detail::low_bits(n);
}

/// The sign s of the right complement s e_C of the blade u = e_A of `a` in an
/// algebra of n generators, where C is complement_mask(a, n).
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in complement_mask.
/// Guarantees: returns -1 or +1, with u ∧ (s e_C) equal to the antiscalar
/// +e_12...n.
/// Conventions: CONVENTIONS.md, "Complements and duals", which fixes the right
/// complement by u ∧ right(u) = antiscalar, and "Generators and blades", which
/// fixes the antiscalar's sign.
[[nodiscard]] constexpr int right_complement_sign(mask_t a, int n) noexcept {
  // e_A ∧ e_C = reorder_sign(A, C) e_12...n, and the sign is its own inverse.
  return reorder_sign(a, complement_mask(a, n));
}

/// The sign s of the left complement s e_C of the blade u = e_A of `a` in an
/// algebra of n generators, where C is complement_mask(a, n).
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in complement_mask.
/// Guarantees: returns -1 or +1, with (s e_C) ∧ u equal to the antiscalar
/// +e_12...n.
/// Conventions: CONVENTIONS.md, "Complements and duals", which fixes the left
/// complement by left(u) ∧ u = antiscalar.
[[nodiscard]] constexpr int left_complement_sign(mask_t a, int n) noexcept {
  return reorder_sign(complement_mask(a, n), a);
}

/// The sign by which the reverse multiplies the blade of `a`:
/// (-1)^(k(k-1)/2) for its grade k.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1.
/// Conventions: CONVENTIONS.md, "Grade operations".
[[nodiscard]] constexpr int reverse_sign(mask_t a) noexcept {
  const int k = grade(a);
  return (k * (k - 1) / 2) % 2 == 0 ? 1 : -1;
}

/// The sign by which the antireverse multiplies the blade of `a` in an algebra
/// of n generators: (-1)^(m(m-1)/2) for its antigrade m.
///
/// Requires: 1 <= n <= mask_bits, and `a` names only e_1 to e_n; checked by
/// GALOOT_ASSERT in antigrade.
/// Guarantees: returns -1 or +1.
/// Conventions: CONVENTIONS.md, "Grade operations".
[[nodiscard]] constexpr int antireverse_sign(mask_t a, int n) noexcept {
  const int m = antigrade(a, n);
  return (m * (m - 1) / 2) % 2 == 0 ? 1 : -1;
}

/// The sign by which the grade involution multiplies the blade of `a`:
/// (-1)^k for its grade k.
///
/// Requires: nothing.
/// Guarantees: returns -1 or +1.
/// Conventions: CONVENTIONS.md, "Grade operations".
[[nodiscard]] constexpr int grade_involution_sign(mask_t a) noexcept {
  return grade(a) % 2 == 0 ? 1 : -1;
}

} // namespace galoot::core

#endif // GALOOT_CORE_BITS_HPP
