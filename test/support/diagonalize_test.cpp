// Tests of the exact diagonalization in support/diagonalize.hpp: T G T^T = D
// on every form the exhaustive tests of the core run over, the integer and
// squarefree squares within the range of a Gram entry, and one hand case of
// each rule of the elimination: a swap at a zero pivot, the pairing of two
// null vectors, the radical, and the scaling of a square to its squarefree
// part. The checks multiply the matrices with the header's own product; that
// product is checked against hand values first.
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include <core/signatures.hpp>
#include <support/diagonalize.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
using test_support::diagonalize;
using test_support::rational;
using test_support::rational_matrix;

// Rows of integers, as the hand values are written.
template <std::size_t N>
constexpr rational_matrix<N>
integer_rows(const std::array<std::array<std::int64_t, N>, N>& rows) {
  rational_matrix<N> out{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      out[i][j] = rational{rows[i][j]};
    }
  }
  return out;
}

// The number of entries in which T G T^T differs from diag(D), for the
// result of diagonalize on G.
template <std::size_t N, class Entry>
constexpr int
diagonal_mismatches(const std::array<std::array<Entry, N>, N>& gram,
                    const test_support::diagonal_form<N>& result) {
  rational_matrix<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g[i][j] = rational{static_cast<std::int64_t>(gram[i][j])};
    }
  }
  const rational_matrix<N> d =
      test_support::multiply(test_support::multiply(result.rows, g),
                             test_support::transpose(result.rows));
  int mismatches = 0;
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      const rational expected{i == j ? result.squares[i] : 0};
      mismatches += d[i][j] == expected ? 0 : 1;
    }
  }
  return mismatches;
}

// Whether x has no square factor other than 1, by trial division.
constexpr bool is_squarefree(std::int64_t x) {
  x = x < 0 ? -x : x;
  for (std::int64_t f = 2; f <= x / f; ++f) {
    if (x % (f * f) == 0) {
      return false;
    }
  }
  return true;
}

// The Witt form of one mode, diagonalized in a constant expression.
constexpr std::array<std::array<int, 2>, 2> witt_one = {{{0, 1}, {1, 0}}};
constexpr auto witt_one_diagonal = diagonalize(witt_one);
static_assert(witt_one_diagonal.squares == std::array<std::int64_t, 2>{2, -2});
static_assert(witt_one_diagonal.rows == integer_rows<2>({{{1, 1}, {-1, 1}}}));
static_assert(diagonal_mismatches(witt_one, witt_one_diagonal) == 0);

} // namespace

// Reference: hand values: [[1, 2], [3, 4]] [[5, 6], [7, 8]] = [[19, 22],
//   [43, 50]], its transpose is [[1, 3], [2, 4]], and the inverse of
//   [[2, 1], [1, 1]] is [[1, -1], [-1, 2]] and that of [[0, 1], [2, 0]],
//   which needs a row swap, is [[0, 1/2], [1, 0]]
TEST_CASE("the matrix product, transpose, and inverse on hand values") {
  const auto x = integer_rows<2>({{{1, 2}, {3, 4}}});
  const auto y = integer_rows<2>({{{5, 6}, {7, 8}}});
  CHECK(test_support::multiply(x, y) ==
        integer_rows<2>({{{19, 22}, {43, 50}}}));
  CHECK(test_support::transpose(x) == integer_rows<2>({{{1, 3}, {2, 4}}}));
  CHECK(test_support::inverse(integer_rows<2>({{{2, 1}, {1, 1}}})) ==
        integer_rows<2>({{{1, -1}, {-1, 2}}}));
  const rational_matrix<2> swapped{
      {
          {rational{0}, rational{1, 2}},
          {rational{1}, rational{0}},
      },
  };
  CHECK(test_support::inverse(integer_rows<2>({{{0, 1}, {2, 0}}})) == swapped);
}

// Reference: identity: T G T^T = D with D diagonal, on every form of the
//   exhaustive tests, the diagonal, conformal, Witt, degenerate, and seeded
//   random ones; each nonzero square is an integer with no square factor,
//   within the largest magnitude of a Gram entry, so that the diagonal is a
//   form the library accepts; T is invertible, its inverse undoing it; and
//   the number of zero squares is the dimension of the radical, n minus the
//   rank, which for the forms listed with a radical is 1 for R(1,0,1),
//   R(2,0,1), R(1,1,1), R(4,0,1), rigid(3) (both orders), the degenerate
//   form, and the form of rank 2 with its radical off the axes, 2 for
//   R(0,0,2), R(2,0,2), and the form with every entry 1, 3 for R(0,0,3), 4
//   for R(0,0,4), 1 for R(0,0,1), and 0 for every other
TEST_CASE("diagonalize gives T G T^T = D on every listed form") {
  galoot_test::for_each_signature([](const char* name, const auto& g) {
    INFO(name);
    constexpr std::size_t n = std::remove_cvref_t<decltype(g)>::generator_count;
    const auto result = diagonalize(g.gram);
    CHECK(diagonal_mismatches(g.gram, result) == 0);
    int zeros = 0;
    for (const std::int64_t s : result.squares) {
      CAPTURE(s);
      CHECK(is_squarefree(s));
      CHECK(s >= -core::max_form_entry);
      CHECK(s <= core::max_form_entry);
      zeros += s == 0 ? 1 : 0;
    }
    rational_matrix<n> identity{};
    for (std::size_t i = 0; i < n; ++i) {
      identity[i][i] = rational{1};
    }
    CHECK(test_support::multiply(
              result.rows, test_support::inverse(result.rows)) == identity);

    // The radical of the listed forms, counted by hand from their matrices.
    const std::string_view form = name;
    int radical = 0;
    if (form == "R(0,0,1)" || form == "R(1,0,1)" || form == "R(2,0,1)" ||
        form == "R(1,1,1)" || form == "R(4,0,1)" || form == "rigid(3)" ||
        form == "rigid(3), null first" || form == "degenerate" ||
        form == "radical off the axes") {
      radical = 1;
    } else if (form == "R(0,0,2)" || form == "R(2,0,2)" || form == "all ones") {
      radical = 2;
    } else if (form == "R(0,0,3)") {
      radical = 3;
    } else if (form == "R(0,0,4)") {
      radical = 4;
    }
    CHECK(zeros == radical);
  });
}

// Reference: hand values: in [[0, 1], [1, 1]] the first pivot is zero and
//   e2 squares to 1, so the two are swapped, f1 = e2, and then f2 = e1 - e2,
//   whose square is 0 - 2 + 1 = -1
TEST_CASE("a zero pivot is swapped with the next nonzero square") {
  const std::array<std::array<int, 2>, 2> g = {{{0, 1}, {1, 1}}};
  const auto result = diagonalize(g);
  CHECK(result.rows == integer_rows<2>({{{0, 1}, {1, -1}}}));
  CHECK(result.squares == std::array<std::int64_t, 2>{1, -1});
}

// Reference: hand values: in the degenerate form [[0, 1, 0], [1, 0, 0],
//   [0, 0, 0]] no generator has a nonzero square, so f1 = e1 + e2, of
//   square 2; then f2 = e2 - f1 / 2 = (e2 - e1) / 2, of square -1/2, scaled
//   by 2 to e2 - e1, of square -2; and e3 pairs with nothing and stays,
//   of square 0, the radical
TEST_CASE("two null vectors are paired, and the radical stays") {
  const std::array<std::array<int, 3>, 3> g = {
      {
          {0, 1, 0},
          {1, 0, 0},
          {0, 0, 0},
      },
  };
  const auto result = diagonalize(g);
  CHECK(result.rows == integer_rows<3>({{{1, 1, 0}, {-1, 1, 0}, {0, 0, 1}}}));
  CHECK(result.squares == std::array<std::int64_t, 3>{2, -2, 0});
}

// Reference: hand values: in [[2, 1], [1, 2]], f2 = e2 - e1 / 2 has square
//   2 - 1 + 1/2 = 3/2, and 3 2 = 6 is squarefree, so f2 is scaled by 2 to
//   2 e2 - e1, of square 6; in [[8]] and [[-12]], 8 = 2 2^2 and
//   12 = 3 2^2, so e1 is scaled by 1/2, to squares 2 and -3; and in
//   [[3, 1], [1, -2]], f2 = e2 - e1 / 3 has square -2 - 1/3 = -7/3, and
//   7 3 = 21, so f2 is scaled by 3 to 3 e2 - e1, of square -21
TEST_CASE("each square is scaled to the squarefree part of p q") {
  const std::array<std::array<int, 2>, 2> g = {{{2, 1}, {1, 2}}};
  const auto result = diagonalize(g);
  CHECK(result.rows == integer_rows<2>({{{1, 0}, {-1, 2}}}));
  CHECK(result.squares == std::array<std::int64_t, 2>{2, 6});

  const std::array<std::array<int, 1>, 1> eight = {{{8}}};
  const auto halved = diagonalize(eight);
  CHECK(halved.rows[0][0] == rational{1, 2});
  CHECK(halved.squares[0] == 2);
  const std::array<std::array<int, 1>, 1> minus_twelve = {{{-12}}};
  const auto negative = diagonalize(minus_twelve);
  CHECK(negative.rows[0][0] == rational{1, 2});
  CHECK(negative.squares[0] == -3);

  const std::array<std::array<int, 2>, 2> random_4 = {{{3, 1}, {1, -2}}};
  const auto thirds = diagonalize(random_4);
  CHECK(thirds.rows == integer_rows<2>({{{1, 0}, {-1, 3}}}));
  CHECK(thirds.squares == std::array<std::int64_t, 2>{3, -21});
}

// Reference: hand values: the squarefree part of 1, 2, 12 = 3 2^2,
//   72 = 2 6^2, 1770 = 2 3 5 59, and 2^2 3^2 5^2 = 900, and the integer
//   square roots of 0, 1, 15, 16, and 2^62 - 1, whose root is 2^31 - 1
//   since (2^31)^2 = 2^62
TEST_CASE("the squarefree part and the integer square root on hand values") {
  CHECK(test_support::squarefree_part(1) == 1);
  CHECK(test_support::squarefree_part(2) == 2);
  CHECK(test_support::squarefree_part(12) == 3);
  CHECK(test_support::squarefree_part(72) == 2);
  CHECK(test_support::squarefree_part(1770) == 1770);
  CHECK(test_support::squarefree_part(900) == 1);
  CHECK(test_support::integer_square_root(0) == 0);
  CHECK(test_support::integer_square_root(1) == 1);
  CHECK(test_support::integer_square_root(15) == 3);
  CHECK(test_support::integer_square_root(16) == 4);
  CHECK(test_support::integer_square_root(4'611'686'018'427'387'903) ==
        2'147'483'647);
}
