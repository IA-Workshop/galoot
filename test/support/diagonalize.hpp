#ifndef GALOOT_TEST_SUPPORT_DIAGONALIZE_HPP
#define GALOOT_TEST_SUPPORT_DIAGONALIZE_HPP

// An exact diagonalization of a symmetric integer form for tests: rows T and
// an integer diagonal D with T G T^T = D, so that a test can pair any form G
// with a diagonal counterpart and the change of basis between them. Row k of
// T holds the coordinates, in the generators of G, of the k-th vector of an
// orthogonal basis.
//
// The method is symmetric Gaussian elimination by congruence (an LDL^T
// factorization without the square roots), over the test rational, so it
// never rounds. Each step k makes the vector f_k orthogonal to every later
// one by f_i <- f_i - (g(f_i, f_k) / g(f_k, f_k)) f_k. A zero pivot
// g(f_k, f_k) is replaced, first by swapping f_k with the next f_j of
// nonzero square, and otherwise by f_k <- f_k + f_j for the next f_j with
// g(f_k, f_j) nonzero, whose square is then 2 g(f_k, f_j). A vector with
// nothing left to pair with is in the radical of the form and keeps the
// square zero. Last, each square d = p / q in lowest terms is made an
// integer with no square factor: with p q = s k^2 for the squarefree s,
// scaling f_k by q / k gives the square d q^2 / k^2 = p q / k^2 = s, with
// p's sign.
//
// It is a test oracle, not part of the library, and it uses no header of
// the library. Its rationals stop rather than wrap when a value does not fit
// std::int64_t. Every function is constexpr, so a test can diagonalize a
// form in a constant expression and at run time alike.

#include <array>
#include <cstddef>
#include <cstdint>

#include <support/rational.hpp>

namespace test_support {

/// A square matrix of exact fractions, row by row.
template <std::size_t N>
using rational_matrix = std::array<std::array<rational, N>, N>;

/// The result of diagonalize: T and D with T G T^T = D.
///
/// Guarantees: rows is invertible; each square is zero or an integer with no
/// square factor other than 1, with the sign of the square it scales.
template <std::size_t N>
struct diagonal_form {
  /// Row k is the k-th orthogonal vector in the generators of G.
  rational_matrix<N> rows;

  /// The square of row k under G.
  std::array<std::int64_t, N> squares;
};

/// The largest r with r * r <= x, for x >= 0.
[[nodiscard]] constexpr std::int64_t integer_square_root(std::int64_t x) {
  std::int64_t low = 0;
  std::int64_t high = (x / 2) + 1;
  while (low < high) {
    const std::int64_t mid = low + ((high - low + 1) / 2);
    if (mid <= x / mid) {
      low = mid;
    } else {
      high = mid - 1;
    }
  }
  return low;
}

/// The squarefree part of x > 0: the product of the primes that divide x an
/// odd number of times, so x is the squarefree part times a square.
[[nodiscard]] constexpr std::int64_t squarefree_part(std::int64_t x) {
  std::int64_t part = 1;
  for (std::int64_t f = 2; f <= x / f; ++f) {
    while (x % (f * f) == 0) {
      x /= f * f;
    }
    if (x % f == 0) {
      part *= f;
      x /= f;
    }
  }
  return part * x;
}

/// T and the integer diagonal D with T G T^T = D, by the elimination above.
///
/// Requires: gram is symmetric, and every value the elimination forms fits
/// the test rational; otherwise the evaluation fails or the program aborts.
/// Guarantees: as for diagonal_form. Two calls on one form give the same
/// result, so a test can recompute it.
template <std::size_t N, class Entry>
[[nodiscard]] constexpr diagonal_form<N>
diagonalize(const std::array<std::array<Entry, N>, N>& gram) {
  rational_matrix<N> a{};
  rational_matrix<N> t{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      a[i][j] = rational{static_cast<std::int64_t>(gram[i][j])};
    }
    t[i][i] = rational{1};
  }

  // Exchanges f_i and f_j: rows and columns i and j of the form, and rows i
  // and j of T.
  const auto swap = [&](std::size_t i, std::size_t j) {
    const auto row = a[i];
    a[i] = a[j];
    a[j] = row;
    for (auto& r : a) {
      const rational x = r[i];
      r[i] = r[j];
      r[j] = x;
    }
    const auto t_row = t[i];
    t[i] = t[j];
    t[j] = t_row;
  };

  // f_k <- f_k + f f_j, a congruence: row k of the form, then column k, then
  // row k of T.
  const auto add_to = [&](std::size_t k, std::size_t j, rational f) {
    for (std::size_t c = 0; c < N; ++c) {
      a[k][c] = a[k][c] + (f * a[j][c]);
    }
    for (auto& r : a) {
      r[k] = r[k] + (f * r[j]);
    }
    for (std::size_t c = 0; c < N; ++c) {
      t[k][c] = t[k][c] + (f * t[j][c]);
    }
  };

  const rational zero{};
  for (std::size_t k = 0; k < N; ++k) {
    if (a[k][k] == zero) {
      std::size_t j = k + 1;
      while (j < N && a[j][j] == zero) {
        ++j;
      }
      if (j < N) {
        swap(k, j);
      } else {
        j = k + 1;
        while (j < N && a[k][j] == zero) {
          ++j;
        }
        if (j == N) {
          // f_k is orthogonal to every later vector and to itself.
          continue;
        }
        add_to(k, j, rational{1});
      }
    }
    for (std::size_t i = k + 1; i < N; ++i) {
      if (a[i][k] != zero) {
        add_to(i, k, -(a[i][k] / a[k][k]));
      }
    }
  }

  // Each square p / q to the squarefree part s of p q, by scaling its row by
  // q / k where p q = s k^2.
  diagonal_form<N> out{.rows = t, .squares = {}};
  for (std::size_t k = 0; k < N; ++k) {
    const rational d = a[k][k];
    if (d == zero) {
      continue;
    }
    const std::int64_t pq =
        detail::checked_multiply(detail::magnitude(d.numerator), d.denominator);
    const std::int64_t s = squarefree_part(pq);
    const std::int64_t root = integer_square_root(pq / s);
    const rational scale{d.denominator, root};
    for (rational& x : out.rows[k]) {
      x = x * scale;
    }
    out.squares[k] = d.numerator < 0 ? -s : s;
  }
  return out;
}

/// The product of two matrices.
template <std::size_t N>
[[nodiscard]] constexpr rational_matrix<N>
multiply(const rational_matrix<N>& x, const rational_matrix<N>& y) {
  rational_matrix<N> out{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      for (std::size_t k = 0; k < N; ++k) {
        out[i][j] = out[i][j] + (x[i][k] * y[k][j]);
      }
    }
  }
  return out;
}

/// The transpose of a matrix.
template <std::size_t N>
[[nodiscard]] constexpr rational_matrix<N>
transpose(const rational_matrix<N>& x) {
  rational_matrix<N> out{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      out[i][j] = x[j][i];
    }
  }
  return out;
}

/// The inverse of an invertible matrix, by Gauss-Jordan elimination with a
/// row swap at a zero pivot.
///
/// Requires: x is invertible; otherwise the evaluation fails or the program
/// aborts, with a message.
template <std::size_t N>
[[nodiscard]] constexpr rational_matrix<N> inverse(rational_matrix<N> x) {
  rational_matrix<N> out{};
  for (std::size_t i = 0; i < N; ++i) {
    out[i][i] = rational{1};
  }
  const rational zero{};
  for (std::size_t k = 0; k < N; ++k) {
    std::size_t pivot = k;
    while (pivot < N && x[pivot][k] == zero) {
      ++pivot;
    }
    if (pivot == N) {
      detail::rational_failed("inverse: the matrix is singular");
    }
    const auto x_row = x[k];
    x[k] = x[pivot];
    x[pivot] = x_row;
    const auto out_row = out[k];
    out[k] = out[pivot];
    out[pivot] = out_row;

    const rational p = x[k][k];
    for (std::size_t c = 0; c < N; ++c) {
      x[k][c] = x[k][c] / p;
      out[k][c] = out[k][c] / p;
    }
    for (std::size_t i = 0; i < N; ++i) {
      const rational f = x[i][k];
      if (i == k || f == zero) {
        continue;
      }
      for (std::size_t c = 0; c < N; ++c) {
        x[i][c] = x[i][c] - (f * x[k][c]);
        out[i][c] = out[i][c] - (f * out[k][c]);
      }
    }
  }
  return out;
}

} // namespace test_support

#endif // GALOOT_TEST_SUPPORT_DIAGONALIZE_HPP
