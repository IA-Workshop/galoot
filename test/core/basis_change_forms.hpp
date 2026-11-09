#ifndef GALOOT_TEST_CORE_BASIS_CHANGE_FORMS_HPP
#define GALOOT_TEST_CORE_BASIS_CHANGE_FORMS_HPP

// The changes of basis that the identity tests of basis_change.hpp run
// over, as values and as tags: each form against a diagonal counterpart by
// the exact diagonalization of support/diagonalize.hpp, conformal(d)
// against R(d+1, 1, 0), the Witt forms against R(m, m, 0), and rigid(3)
// against itself with its null generator doubled.
//
// A change from a form G to its diagonal counterpart D: the diagonalization
// gives T with T G T^T = D, so P = T^-1 satisfies P D P^T = G, and row i of
// P writes e_i of G in the generators of D.

#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/metric.hpp>

#include <support/diagonalize.hpp>
#include <support/rational.hpp>

namespace galoot_test::basis_changes {

namespace core = galoot::core;

/// The change whose row i is row i of p: its entries times the least common
/// multiple of their denominators, over that multiple.
///
/// Requires: every entry and multiple fits core::entry_type, as for the
/// changes of the tests.
template <std::size_t N>
constexpr core::basis_change<N>
change_of(const test_support::rational_matrix<N>& p) {
  core::basis_change<N> c{};
  for (std::size_t i = 0; i < N; ++i) {
    std::int64_t d = 1;
    for (const test_support::rational& x : p[i]) {
      d = std::lcm(d, x.denominator);
    }
    for (std::size_t j = 0; j < N; ++j) {
      c.rows[i][j] = static_cast<core::entry_type>(p[i][j].numerator *
                                                   (d / p[i][j].denominator));
    }
    c.denominators[i] = static_cast<core::entry_type>(d);
  }
  return c;
}

/// The diagonal form with these squares.
template <std::size_t N>
constexpr core::metric<N>
diagonal_metric(const std::array<std::int64_t, N>& squares) {
  core::metric<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    m.gram[i][i] = static_cast<core::entry_type>(squares[i]);
  }
  return m;
}

/// A form's diagonal counterpart and the change of basis from the form to
/// it.
template <std::size_t N>
struct diagonal_change {
  core::metric<N> target;
  core::basis_change<N> change;
};

/// The diagonal counterpart of g and the change P = T^-1 to it.
template <std::size_t N>
constexpr diagonal_change<N> to_diagonal(const core::metric<N>& g) {
  const auto d = test_support::diagonalize(g.gram);
  return {
      .target = diagonal_metric(d.squares),
      .change = change_of(test_support::inverse(d.rows)),
  };
}

/// The change of the scaled Witt form of M modes, e(2j-1) . e(2j) = 1 and
/// every other product 0, to R(M, M, 0): e(2j-1) = (e'j + e'(M+j)) / 2 and
/// e(2j) = e'j - e'(M+j), where e'1 to e'M square to +1 and e'(M+1) to e'2M
/// to -1. The creation operator c_j = e(2j-1) goes to (e'j + e'(M+j)) / 2
/// and the annihilation operator c_j† = e(2j) / 2 to (e'j - e'(M+j)) / 2.
template <std::size_t M>
constexpr core::basis_change<2 * M> witt_change() {
  core::basis_change<2 * M> c{};
  for (std::size_t j = 0; j < M; ++j) {
    c.rows[2 * j][j] = 1;
    c.rows[2 * j][M + j] = 1;
    c.denominators[2 * j] = 2;
    c.rows[(2 * j) + 1][j] = 1;
    c.rows[(2 * j) + 1][M + j] = -1;
    c.denominators[(2 * j) + 1] = 1;
  }
  return c;
}

/// The scaled Witt forms of one and two modes, as the exhaustive tests list
/// them.
inline constexpr auto witt_one = core::from_gram({
    {0, 1},
    {1, 0},
});
inline constexpr auto witt_two = core::from_gram({
    {0, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
});

/// rigid(3) to itself with e4 = 2 e'4: e4 squares to 0 on both sides, so the
/// change keeps every inner product, and its determinant is 2.
inline constexpr core::basis_change<4> doubled_null_change{
    .rows = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 2}}},
    .denominators = {1, 1, 1, 1},
};

// Tags. An algebra whose metric is the value G.
template <auto G>
struct form {
  static constexpr auto value = G;
};

// The change from the algebra of G to its diagonal counterpart.
template <auto G>
struct to_diagonal_tag {
  using source = form<G>;
  using target = form<to_diagonal(G).target>;
  static constexpr auto value = to_diagonal(G).change;
};

// conformal(D) to R(D+1, 1, 0) by conformal_change<D>().
template <std::size_t D>
struct conformal_tag {
  using source = form<core::conformal<D>()>;
  using target = form<core::R<D + 1, 1, 0>()>;
  static constexpr auto value = core::conformal_change<D>();
};

// The Witt forms of one and two modes to R(1, 1, 0) and R(2, 2, 0).
struct witt_one_tag {
  using source = form<witt_one>;
  using target = form<core::R<1, 1, 0>()>;
  static constexpr auto value = witt_change<1>();
};
struct witt_two_tag {
  using source = form<witt_two>;
  using target = form<core::R<2, 2, 0>()>;
  static constexpr auto value = witt_change<2>();
};

// rigid(3) to itself with its null generator doubled.
struct doubled_null_tag {
  using source = form<core::rigid<3>()>;
  using target = form<core::rigid<3>()>;
  static constexpr auto value = doubled_null_change;
};

} // namespace galoot_test::basis_changes

#endif // GALOOT_TEST_CORE_BASIS_CHANGE_FORMS_HPP
