#ifndef GALOOT_CORE_BLADE_PRODUCT_HPP
#define GALOOT_CORE_BLADE_PRODUCT_HPP

// The geometric product of two basis blades as an exact list of terms,
// generated from the metric by one constexpr function, so that the same code
// serves generation at compile time and at run time.
//
// Generators are e1 to en, counting from 1, and bit i - 1 of a mask stands
// for e_i. The blade of a mask is the wedge product of its generators in
// increasing order, e_a1 ∧ ... ∧ e_ak, not their geometric product; the two
// differ when the metric is not diagonal. In conformal(3), e45 = e4 ∧ e5, and
// so e4 e5 = -1 + e45.
//
// The products follow from the defining relations e_i e_j + e_j e_i = 2 g_ij
// by two identities. For a generator e_i and any x,
//
//   e_i x = e_i ⌋ x + e_i ∧ x,
//
// where the left contraction of a blade is
//
//   e_i ⌋ (e_b1 ∧ ... ∧ e_bk) = Σ_p (-1)^(p-1) g(e_i, e_bp) e_(B without b_p),
//
// and e_i ∧ e_B is zero when e_i is in B, and otherwise e_(B with i) times
// (-1) to the number of generators of B below e_i. For a blade A whose lowest
// generator is e_a, with the rest A', e_A = e_a ∧ e_A' = e_a e_A' - e_a ⌋ e_A',
// so
//
//   e_A e_B = e_a (e_A' e_B) - (e_a ⌋ e_A') e_B,
//
// a recursion on the grade of A in which every step multiplies and adds
// integers. So every coefficient is an integer polynomial in the entries of
// the Gram matrix, of degree (|A| + |B| - |C|) / 2 on the blade e_C, and the
// generation needs no division and no rounding.

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>

namespace galoot::core {

/// The type of the exact coefficient of a generated term.
///
/// Guarantees: a signed integer of 64 bits. Generation keeps every
/// coefficient within -max to max, where max is
/// std::numeric_limits<coefficient_type>::max(), and a coefficient that
/// would leave that range fails GALOOT_ASSERT where it is formed, so no
/// coefficient wraps silently.
using coefficient_type = std::int64_t;

namespace detail {

/// x + y, checked to lie within -max to max, where max is the largest
/// coefficient_type.
///
/// Requires: x and y lie within -max to max.
/// Guarantees: returns x + y when it lies in that range; otherwise fails
/// GALOOT_ASSERT, which ends a constant evaluation and aborts a checking
/// build at run time.
[[nodiscard]] constexpr coefficient_type
add_checked(coefficient_type x, coefficient_type y) noexcept {
  // Neither bound can overflow: max - y for 0 < y <= max, and -max - y for
  // -max <= y <= 0, both lie within -max to max.
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  const bool fits = y > 0 ? x <= max - y : x >= -max - y;
  GALOOT_ASSERT(fits, "a product coefficient does not fit coefficient_type");
  return x + y;
}

/// x y, checked to lie within -max to max, where max is the largest
/// coefficient_type.
///
/// Requires: x and y lie within -max to max, so that their magnitudes are
/// defined.
/// Guarantees: returns x y when it lies in that range; otherwise fails
/// GALOOT_ASSERT, which ends a constant evaluation and aborts a checking
/// build at run time.
[[nodiscard]] constexpr coefficient_type
multiply_checked(coefficient_type x, coefficient_type y) noexcept {
  // For |x| > 0, |x| |y| <= max exactly when |y| <= max / |x| rounded down.
  constexpr coefficient_type max = std::numeric_limits<coefficient_type>::max();
  const coefficient_type magnitude_x = x < 0 ? -x : x;
  const coefficient_type magnitude_y = y < 0 ? -y : y;
  const bool fits = magnitude_x == 0 || magnitude_y <= max / magnitude_x;
  GALOOT_ASSERT(fits, "a product coefficient does not fit coefficient_type");
  return x * y;
}

/// One term of a generated product: coefficient times the canonical blade of
/// the mask `blade`.
///
/// Guarantees: a structural aggregate, trivially copyable; two terms are
/// equal when blade and coefficient are.
struct term {
  /// The generators of the blade: bit i - 1 stands for e_i.
  mask_t blade;
  /// The exact coefficient of the blade.
  coefficient_type coefficient;

  /// Equal when blade and coefficient are equal.
  [[nodiscard]] constexpr bool operator==(const term&) const = default;
};

/// A sum of at most Capacity terms, each on a different blade, none with a
/// zero coefficient, kept in canonical order: by grade and, within a grade,
/// lexicographically by index list, the order of canonical_less.
///
/// Requires: the list starts empty, as term_list<Capacity>{}, and changes
/// only through add.
/// Guarantees: a structural, trivially copyable aggregate. The first size()
/// places of `terms` hold the terms and the rest hold {0, 0}, so two lists of
/// one sum compare equal with ==, in whatever order its terms were added.
template <std::size_t Capacity>
struct term_list {
  /// The terms, in canonical order of their blades, then {0, 0}.
  std::array<term, Capacity> terms;
  /// The number of terms.
  std::size_t length;

  /// The number of terms.
  [[nodiscard]] constexpr std::size_t size() const noexcept { return length; }

  /// Adds coefficient times the blade of `blade` to the sum.
  ///
  /// Requires: coefficient lies within -max to max, the range of
  /// coefficient_type that generation keeps.
  /// Guarantees: a blade already listed gets the sum of the two
  /// coefficients, and is removed if that sum is zero; a new blade is
  /// inserted at its place in canonical order; a zero coefficient changes
  /// nothing. A list that would hold more than Capacity terms, or a sum that
  /// does not fit coefficient_type, fails GALOOT_ASSERT.
  constexpr void add(mask_t blade, coefficient_type coefficient) noexcept {
    if (coefficient == 0) {
      return;
    }
    std::size_t at = 0;
    while (at < length && canonical_less(terms[at].blade, blade)) {
      ++at;
    }

    if (at < length && terms[at].blade == blade) {
      terms[at].coefficient = add_checked(terms[at].coefficient, coefficient);
      if (terms[at].coefficient == 0) {
        for (std::size_t j = at; j + 1 < length; ++j) {
          terms[j] = terms[j + 1];
        }
        --length;
        terms[length] = {};
      }
      return;
    }

    GALOOT_ASSERT(length < Capacity, "more product terms than the capacity");
    for (std::size_t j = length; j > at; --j) {
      terms[j] = terms[j - 1];
    }
    terms[at] = {.blade = blade, .coefficient = coefficient};
    ++length;
  }

  /// Equal when the same terms are listed.
  [[nodiscard]] constexpr bool operator==(const term_list&) const = default;
};

/// The largest number of terms that e_A e_B can have in any metric, for the
/// masks `a` and `b` of A and B: 2^(|A ∪ B| - 1), or 1 when both are the
/// scalar.
///
/// Requires: nothing.
/// Guarantees: every term of e_A e_B lies on a subset of A ∪ B, because each
/// step of the recursion above either removes a generator or adds one of A;
/// and every term has a grade of the parity of |A| + |B|, because each step
/// changes the grade by one and the correction by two. So at most half the
/// 2^|A ∪ B| subsets appear. The same holds for every partial sum formed
/// while generating the product, so a term_list of this capacity never
/// overflows.
[[nodiscard]] constexpr std::size_t product_capacity(mask_t a,
                                                     mask_t b) noexcept {
  const int generators = grade(a | b);
  return generators == 0
             ? 1
             : std::size_t{1} << static_cast<unsigned>(generators - 1);
}

// Adds scale e_i x to `out`, by e_i x = e_i ⌋ x + e_i ∧ x, term by term.
template <std::size_t Capacity, std::size_t N>
constexpr void add_generator_product(term_list<Capacity>& out,
                                     const metric<N>& g, std::size_t i,
                                     const term_list<Capacity>& x,
                                     coefficient_type scale) {
  const mask_t generator = mask_t{1} << i;
  for (std::size_t k = 0; k < x.size(); ++k) {
    const mask_t blade = x.terms[k].blade;
    const coefficient_type c = multiply_checked(x.terms[k].coefficient, scale);

    // e_i ∧ e_B: e_i moves past the generators of B below it, each one
    // transposition, which is the count reorder_sign takes.
    if ((blade & generator) == 0) {
      out.add(blade | generator,
              multiply_checked(reorder_sign(generator, blade), c));
    }

    // e_i ⌋ e_B: the p-th generator e_j of B, counting from 1 in increasing
    // order, contributes (-1)^(p-1) g(e_i, e_j) e_(B without j).
    coefficient_type sign = 1;
    for (std::size_t j = 0; j < N; ++j) {
      if (((blade >> j) & 1U) == 0U) {
        continue;
      }
      const coefficient_type entry = g.gram[i][j];
      if (entry != 0) {
        out.add(blade & ~(mask_t{1} << j),
                multiply_checked(multiply_checked(sign, entry), c));
      }
      sign = -sign;
    }
  }
}

// Adds scale e_A e_B to `out`, by e_A e_B = e_a (e_A' e_B) - (e_a ⌋ e_A') e_B
// for the lowest generator e_a of A and the rest A'. The recursion is on the
// grade of `a`, which falls by one or two at each call, so its depth is at
// most mask_bits.
template <std::size_t Capacity, std::size_t N>
// NOLINTNEXTLINE(misc-no-recursion): the depth is bounded, as stated above.
constexpr void add_blade_product(term_list<Capacity>& out, const metric<N>& g,
                                 mask_t a, mask_t b, coefficient_type scale) {
  if (a == 0) {
    out.add(b, scale);
    return;
  }
  const auto low = static_cast<std::size_t>(std::countr_zero(a));
  const mask_t rest = a & (a - 1);

  term_list<Capacity> rest_product{};
  add_blade_product(rest_product, g, rest, b, 1);
  add_generator_product(out, g, low, rest_product, scale);

  // -(e_a ⌋ e_A') e_B: the p-th generator e_j of A', counting from 1 in
  // increasing order, contributes -(-1)^(p-1) g(e_a, e_j) e_(A' without j)
  // e_B.
  coefficient_type sign = -1;
  for (std::size_t j = 0; j < N; ++j) {
    if (((rest >> j) & 1U) == 0U) {
      continue;
    }
    const coefficient_type entry = g.gram[low][j];
    if (entry != 0) {
      add_blade_product(out, g, rest & ~(mask_t{1} << j), b,
                        multiply_checked(multiply_checked(sign, entry), scale));
    }
    sign = -sign;
  }
}

/// The terms of the geometric product e_A e_B of the blades of the masks `a`
/// and `b` in the metric `g`, each blade the wedge product of its generators.
/// The metric is a value, so the same call runs on a metric built at compile
/// time or read at run time.
///
/// Requires: `g` is symmetric with every entry within max_form_entry in
/// magnitude, as the factories of metric.hpp guarantee and metric_like
/// checks; this is not checked here. `a` and `b` name only e_1 to e_N,
/// checked by GALOOT_ASSERT. No list of terms formed while generating the
/// product holds more than Capacity terms; Capacity >= product_capacity(a, b)
/// always suffices, and a list that would outgrow Capacity fails
/// GALOOT_ASSERT. Every coefficient formed fits coefficient_type, checked by
/// GALOOT_ASSERT.
/// Guarantees: the result is exact, in canonical order, with no zero
/// coefficient, so two equal products compare equal with ==.
template <std::size_t Capacity, std::size_t N>
[[nodiscard]] constexpr term_list<Capacity>
blade_product(const metric<N>& g, mask_t a, mask_t b) noexcept {
  // N == mask_bits is tested first, because a shift by the width of mask_t is
  // undefined.
  const bool in_range = N == mask_bits || ((a | b) >> N) == 0;
  GALOOT_ASSERT(in_range, "blade has a generator beyond e_n");

  term_list<Capacity> out{};
  add_blade_product(out, g, a, b, 1);
  return out;
}

} // namespace detail

} // namespace galoot::core

#endif // GALOOT_CORE_BLADE_PRODUCT_HPP
