#ifndef GALOOT_CORE_METRIC_HPP
#define GALOOT_CORE_METRIC_HPP

// The signature of an algebra as data: the symmetric bilinear form of its
// generators, held as an integer Gram matrix in a structural value.
//
// An algebra is named by a tag type that carries its metric as a static
// member `value`, so that compiler diagnostics and symbol names show the name
// of the algebra rather than its whole matrix:
//
//   struct rga3 {
//     static constexpr auto value = galoot::core::rigid<3>();
//   };
//
// metric_like<rga3> then holds. The metric holds no basis names; names belong
// to the blade sets built on it.

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>

#include <galoot/core/assert.hpp>
#include <galoot/core/bits.hpp>

namespace galoot::core {

/// The type of an entry of a Gram matrix.
using entry_type = int;

namespace detail {

// The largest k with k * k <= x, for x >= 0. Bisection keeps it cheap for a
// wide entry type, and comparing k with x / k avoids forming k * k.
consteval entry_type isqrt(entry_type x) {
  entry_type low = 0;
  entry_type high = (x / 2) + 1;
  while (low < high) {
    const entry_type mid = low + ((high - low + 1) / 2);
    if (mid <= x / mid) {
      low = mid;
    } else {
      high = mid - 1;
    }
  }
  return low;
}

} // namespace detail

/// The number of generators that the factories accept unless their MaxN
/// says otherwise. Galoot is tested up to 16 generators; a factory given a
/// larger MaxN, up to mask_bits, builds an algebra that every later part of
/// the library accepts but that is not tested.
inline constexpr std::size_t max_generators = 16;

/// The largest magnitude of an entry of a Gram matrix: the largest k for
/// which the 2 x 2 minor g_ii g_jj - g_ij^2 of entries of magnitude k, at most
/// 2 k^2, fits in entry_type. Larger minors can still overflow; that fails the
/// constant evaluation that forms them, so it cannot pass silently.
inline constexpr entry_type max_form_entry =
    detail::isqrt(std::numeric_limits<entry_type>::max() / 2);

/// The symmetric bilinear form of an algebra with N generators.
///
/// Generators are named e1 to en, counting from 1, and bit i of a blade mask
/// stands for e_(i+1). The factories below produce only valid metrics; an
/// aggregate written by hand is checked by metric_like before an algebra can
/// use it.
///
/// Requires: N <= mask_bits, the number of generators a blade mask holds.
/// Guarantees: a structural type, so a metric can be a template argument;
/// trivially copyable, and never allocates.
template <std::size_t N>
struct metric {
  static_assert(N <= mask_bits,
                "metric: n is larger than the number of bits in a blade mask");

  /// gram[i][j] is the inner product e_(i+1) . e_(j+1), so row and column i
  /// belong to the generator of mask bit i.
  std::array<std::array<entry_type, N>, N> gram;

  /// The number of generators, n.
  static constexpr std::size_t generator_count = N;

  /// Whether gram[i][j] == gram[j][i] for every i and j.
  [[nodiscard]] constexpr bool is_symmetric() const noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = 0; j < i; ++j) {
        if (gram[i][j] != gram[j][i]) {
          return false;
        }
      }
    }
    return true;
  }

  /// Whether every entry lies in -max_form_entry to max_form_entry.
  [[nodiscard]] constexpr bool entries_in_range() const noexcept {
    // NOLINTNEXTLINE(readability-use-anyofallof): avoids <algorithm> here.
    for (const auto& row : gram) {
      for (const entry_type g : row) {
        if (g < -max_form_entry || g > max_form_entry) {
          return false;
        }
      }
    }
    return true;
  }

  /// Whether every entry off the diagonal is zero, so that distinct
  /// generators are orthogonal.
  [[nodiscard]] constexpr bool is_diagonal() const noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = 0; j < N; ++j) {
        if (i != j && gram[i][j] != 0) {
          return false;
        }
      }
    }
    return true;
  }

  /// Whether the generator e_(i+1) squares to zero.
  ///
  /// Requires: i < N, checked by GALOOT_ASSERT. A null generator need not
  /// make the form degenerate: in conformal(d) both e(d+1) and e(d+2) are
  /// null, and they pair to -1.
  [[nodiscard]] constexpr bool is_null(std::size_t i) const noexcept {
    GALOOT_ASSERT(i < N, "metric::is_null: the index must be below n");
    return gram[i][i] == 0;
  }

  /// Two metrics are equal when their Gram matrices are equal entry by
  /// entry.
  [[nodiscard]] constexpr bool operator==(const metric&) const = default;
};

namespace detail {

// Every factory accepts 1 <= n <= MaxN, with MaxN no wider than a blade mask.
// The checks are static_asserts, because n and MaxN are template arguments,
// and each message names its misuse.
template <std::size_t N, std::size_t MaxN>
consteval void check_generator_count() {
  static_assert(MaxN <= mask_bits,
                "metric: MaxN is larger than the number of bits in a blade "
                "mask");
  static_assert(N >= 1, "metric: an algebra has at least one generator, so "
                        "n = 0 is rejected");
  static_assert(N <= MaxN, "metric: n is larger than MaxN, the largest number "
                           "of generators accepted (max_generators unless "
                           "MaxN is given)");
}

template <class T>
inline constexpr bool is_metric_v = false;

template <std::size_t N>
inline constexpr bool is_metric_v<metric<N>> = true;

} // namespace detail

/// The diagonal signature R(p, q, z) with n = P + Q + Z generators. The third
/// count is named Z, for the generators that square to zero, because the
/// function itself is R.
///
/// Requires: 1 <= n <= MaxN, and MaxN <= mask_bits, each checked at compile
/// time with a message that names it. A negative count does not convert to
/// std::size_t, so it fails to compile.
/// Guarantees: e1 to eP square to +1, the next Q generators to -1, and the
/// last Z to 0, and distinct generators are orthogonal. Null generators come
/// last, as in the rigid algebra, where some texts put a null e0 first.
template <std::size_t P, std::size_t Q, std::size_t Z,
          std::size_t MaxN = max_generators>
// NOLINTNEXTLINE(readability-identifier-naming): the notation R(p, q, r).
[[nodiscard]] consteval metric<P + Q + Z> R() {
  // Each count is checked against MaxN before the sum is relied on, so that a
  // count near the top of std::size_t cannot wrap the sum back into range.
  static_assert(P <= MaxN && Q <= MaxN && Z <= MaxN,
                "metric: n is larger than MaxN, the largest number of "
                "generators accepted (max_generators unless MaxN is given)");
  // Checking a wrapped sum would add a second, misleading message.
  if constexpr (P <= MaxN && Q <= MaxN && Z <= MaxN) {
    detail::check_generator_count<P + Q + Z, MaxN>();
  }

  metric<P + Q + Z> m{};
  for (std::size_t i = 0; i < P + Q; ++i) {
    m.gram[i][i] = i < P ? 1 : -1;
  }
  return m;
}

/// The rigid algebra rigid(D), which is R(D, 0, 1) with D + 1 generators.
///
/// Requires: 1 <= D + 1 <= MaxN, checked as for R.
/// Guarantees: e1 to eD are Euclidean and e(D+1) is the null projective
/// generator, e4 when D = 3.
template <std::size_t D, std::size_t MaxN = max_generators>
[[nodiscard]] consteval metric<D + 1> rigid() {
  return R<D, 0, 1, MaxN>();
}

/// The conformal algebra conformal(D), with D + 2 generators.
///
/// Requires: 1 <= D + 2 <= MaxN, checked as for R.
/// Guarantees: e1 to eD are Euclidean; e(D+1), the origin, and e(D+2), the
/// point at infinity, are both null, with e(D+1) . e(D+2) = -1. The form is
/// not diagonal. Its diagonal counterpart R(D+1, 1, 0) is a different
/// signature, related by the exact change of basis
/// origin = (1/2)(e(D+2) - e(D+1)) and infinity = e(D+2) + e(D+1), in which
/// e(D+1) squares to +1 and e(D+2) to -1.
template <std::size_t D, std::size_t MaxN = max_generators>
[[nodiscard]] consteval metric<D + 2> conformal() {
  // D is checked before D + 2 is relied on, as R checks its counts.
  static_assert(D <= MaxN,
                "metric: n is larger than MaxN, the largest number of "
                "generators accepted (max_generators unless MaxN is given)");
  if constexpr (D <= MaxN) {
    detail::check_generator_count<D + 2, MaxN>();
  }

  metric<D + 2> m{};
  for (std::size_t i = 0; i < D; ++i) {
    m.gram[i][i] = 1;
  }
  m.gram[D][D + 1] = -1;
  m.gram[D + 1][D] = -1;
  return m;
}

/// The metric whose Gram matrix is `gram`, for a form that no other factory
/// builds, such as a non-diagonal or degenerate one:
/// from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}}). The parameter is a C array
/// so that N is deduced from the braces. N comes first, so raising MaxN means
/// writing N as well: from_gram<3, 32>({...}).
///
/// Requires: `gram` is square, with N rows of N entries, or the call does not
/// match; 1 <= N <= MaxN, checked as for R; `gram` is symmetric, and every
/// entry lies in -max_form_entry to max_form_entry, each checked at compile
/// time with a message that names it.
/// Guarantees: the result's gram equals `gram` entry by entry.
template <std::size_t N, std::size_t MaxN = max_generators>
[[nodiscard]] consteval metric<N>
// NOLINTNEXTLINE(*-avoid-c-arrays): a C array lets N be deduced from braces.
from_gram(const entry_type (&gram)[N][N]) {
  detail::check_generator_count<N, MaxN>();
  metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m.gram[i][j] = gram[i][j];
    }
  }

  // Each message stays on the line of its GALOOT_ASSERT, because a compiler
  // shows only that line of a failed constant evaluation.
  const bool in_range = m.entries_in_range();
  GALOOT_ASSERT(m.is_symmetric(), "from_gram: the form must be symmetric");
  GALOOT_ASSERT(in_range, "from_gram: an entry is beyond max_form_entry");
  return m;
}

/// A tag type that names an algebra and carries its metric.
///
/// Requires of Alg: a static data member `value`, usable in a constant
/// expression, of type metric<N> with N >= 1, a symmetric Gram matrix, and
/// every entry within max_form_entry in magnitude. Any N that metric<N>
/// allows is accepted, whatever MaxN the factory was given. The concept tests
/// the data only, and is false rather than an error for a type that has no
/// such member. The guard reads every entry, so that it fails for a value
/// that is not usable at compile time even when n = 1.
template <class Alg>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    requires { typename std::bool_constant<Alg::value.entries_in_range()>; } &&
    (Alg::value.generator_count >= 1) && Alg::value.is_symmetric() &&
    Alg::value.entries_in_range();

} // namespace galoot::core

#endif // GALOOT_CORE_METRIC_HPP
