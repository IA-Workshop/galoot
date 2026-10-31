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
#include <type_traits>
#include <utility>

#include <galoot/core/assert.hpp>

namespace galoot::core {

// Gram entries are indexed by generator. Every index is bounded by N in its
// loop or by a stated precondition, a constant evaluation rejects an access
// out of bounds, and the test builds check each access at run time, so the
// bounds checks that ask for constant or checked indices are suppressed here.
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
// NOLINTBEGIN(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)

/// The largest number of generators that the factories and metric_like
/// accept when no MaxN is given. Galoot is tested up to 16 generators; a
/// caller that passes a larger MaxN gets an algebra that is not tested.
inline constexpr std::size_t max_generators = 16;

/// The largest magnitude of an entry of a Gram matrix. With it, a product of
/// two entries and a 2 x 2 minor of the form (the Gram determinant of a pair
/// of generators) fit in an int, whose range is at least -(2^31 - 1) to
/// 2^31 - 1.
inline constexpr int max_form_entry = 32767;

/// The symmetric bilinear form of an algebra with N generators.
///
/// Generators are named e1 to en, counting from 1, and bit i of a blade mask
/// stands for e_(i+1). So `gram[i][j]` is the inner product e_(i+1) . e_(j+1),
/// and row and column i belong to the generator of mask bit i. The factories
/// below produce only valid metrics; an aggregate written by hand is checked
/// by metric_like before an algebra can use it.
///
/// Guarantees: a structural type, so a metric can be a template argument;
/// trivially copyable, and never allocates.
template <std::size_t N>
struct metric {
  std::array<std::array<int, N>, N> gram;

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
    // A loop reads as plainly as std::ranges::all_of here, and keeps
    // <algorithm> out of a header that every algebra includes.
    // NOLINTNEXTLINE(readability-use-anyofallof)
    for (const auto& row : gram) {
      for (const int g : row) {
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
  /// Requires: i < N. A null generator need not make the form degenerate:
  /// in conformal(d) both e(d+1) and e(d+2) are null, and they pair to -1.
  [[nodiscard]] constexpr bool is_null(std::size_t i) const noexcept {
    return gram[i][i] == 0;
  }

  /// Two metrics are equal when their Gram matrices are equal entry by
  /// entry.
  [[nodiscard]] constexpr bool operator==(const metric&) const = default;
};

namespace detail {

// Every factory accepts 1 <= n <= MaxN. The check is a static_assert, because
// n is a template argument, and its message names the misuse.
template <std::size_t N, std::size_t MaxN>
consteval void check_generator_count() {
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

/// The diagonal signature R(p, q, r) with N = p + q + r generators.
///
/// Requires: 1 <= N <= MaxN; p, q, and r are not negative; p + q + r == N.
/// Each is checked at compile time with a message that names it.
/// Guarantees: e1 to ep square to +1, the next q generators to -1, and the
/// last r to 0, and distinct generators are orthogonal. Null generators come
/// last, as in the rigid algebra, where some texts put a null e0 first.
template <std::size_t N, std::size_t MaxN = max_generators>
// R(p, q, r) is the notation of the literature, so it keeps its capital.
// NOLINTNEXTLINE(readability-identifier-naming)
[[nodiscard]] consteval metric<N> R(int p, int q, int r) {
  detail::check_generator_count<N, MaxN>();
  const bool counts = p >= 0 && q >= 0 && r >= 0;
  GALOOT_ASSERT(counts, "R<N>(p, q, r): p, q, and r cannot be negative");
  const bool sums = std::cmp_equal(p + q + r, N);
  GALOOT_ASSERT(sums, "R<N>(p, q, r): p + q + r must equal N");

  const auto positive = static_cast<std::size_t>(p);
  const auto nonzero = positive + static_cast<std::size_t>(q);
  metric<N> m{};
  for (std::size_t i = 0; i < nonzero; ++i) {
    m.gram[i][i] = i < positive ? 1 : -1;
  }
  return m;
}

/// The rigid algebra rigid(D), which is R(D, 0, 1) with D + 1 generators.
///
/// Requires: 1 <= D + 1 <= MaxN.
/// Guarantees: e1 to eD are Euclidean and e(D+1) is the null projective
/// generator, e4 when D = 3.
template <std::size_t D, std::size_t MaxN = max_generators>
[[nodiscard]] consteval metric<D + 1> rigid() {
  return R<D + 1, MaxN>(static_cast<int>(D), 0, 1);
}

/// The conformal algebra conformal(D), with D + 2 generators.
///
/// Requires: 1 <= D + 2 <= MaxN.
/// Guarantees: e1 to eD are Euclidean; e(D+1), the origin, and e(D+2), the
/// point at infinity, are both null, with e(D+1) . e(D+2) = -1. The form is
/// not diagonal. Its diagonal counterpart R(D+1, 1, 0) is a different
/// signature, related by the exact change of basis
/// origin = (1/2)(e(D+2) - e(D+1)) and infinity = e(D+2) + e(D+1), in which
/// e(D+1) squares to +1 and e(D+2) to -1.
template <std::size_t D, std::size_t MaxN = max_generators>
[[nodiscard]] consteval metric<D + 2> conformal() {
  detail::check_generator_count<D + 2, MaxN>();
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
/// from_gram({{0, 1, 0}, {1, 0, 0}, {0, 0, 0}}).
///
/// Requires: 1 <= N <= MaxN; `gram` is symmetric; every entry lies in
/// -max_form_entry to max_form_entry. Each is checked at compile time with a
/// message that names it.
/// Guarantees: the result's gram equals `gram` entry by entry.
// A reference to a C array lets N be deduced from a braced matrix, which a
// std::array parameter does not.
template <std::size_t MaxN = max_generators, std::size_t N>
[[nodiscard]] consteval metric<N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
from_gram(const int (&gram)[N][N]) {
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
/// Requires of Alg: a static constexpr data member `value` of type
/// metric<N>, with 1 <= N <= MaxN, a symmetric Gram matrix, and every entry
/// within max_form_entry in magnitude. The concept tests the data only.
template <class Alg, std::size_t MaxN = max_generators>
concept metric_like =
    detail::is_metric_v<std::remove_cvref_t<decltype(Alg::value)>> &&
    (Alg::value.generator_count >= 1) && (Alg::value.generator_count <= MaxN) &&
    Alg::value.is_symmetric() && Alg::value.entries_in_range();

// NOLINTEND(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
// NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)

} // namespace galoot::core

#endif // GALOOT_CORE_METRIC_HPP
