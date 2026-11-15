// The change of basis of galoot/core/basis_change.hpp as an algebra
// isomorphism, checked on its generated terms at run time: for every form of
// the exhaustive tests against its exact diagonal counterpart, and for the
// named changes of conformal(d), the Witt forms, and rigid(3) with its null
// generator doubled.
//
// For a change P from an algebra with Gram matrix G to one with G', with
// P G' P^T = G, the map phi sends e_A to the sum over the target's blades
// e'_B of A's grade of det P[A, B] e'_B. The library generates, for each
// blade, the minors of its integer rows M, det M[A, B], so that det P[A, B]
// is that over the product of the denominators of A's rows. Because P keeps
// every inner product, phi is an algebra map: phi(e_A e_B) =
// phi(e_A) phi(e_B) for every pair of blades, the geometric product on each
// side taken in its own algebra, phi(1) = 1, every image has its blade's
// grade, and the antiscalar goes to det(P) times the target's. The inverse
// change, which the library computes from the rows, undoes phi on every
// blade both ways. Each check runs over every blade or pair of blades of
// the form, so it is a proof over that form. The sums of terms are held
// densely by mask over the test rational, so every value is exact.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <utility>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_product.hpp>
#include <galoot/core/metric.hpp>

#include <doctest/doctest.h>

#include "basis_change_forms.hpp"
#include "signatures.hpp"
#include <support/diagonalize.hpp>
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace forms = galoot_test::basis_changes;
using test_support::rational;

// The forms checked here have at most six generators, so a product of two
// blades has at most 2^5 terms and the image of a blade at most C(6, 3) = 20.
constexpr std::size_t capacity = 32;
using list = core::detail::term_list<capacity>;

constexpr std::size_t blade_count(std::size_t n) { return std::size_t{1} << n; }

// A sum of blades, by mask.
template <std::size_t N>
using dense = std::array<rational, blade_count(N)>;

template <std::size_t N>
dense<N> unit(core::mask_t blade) {
  dense<N> x{};
  x.at(blade) = rational{1};
  return x;
}

// The image of every blade under the rows, as the library generates it:
// its minors over the product of the denominators of the blade's rows. The
// rows are those of a change or of the inverse the library computes.
template <std::size_t N, class Rows>
std::array<dense<N>, blade_count(N)> images_of(const Rows& rows) {
  std::array<dense<N>, blade_count(N)> images{};
  for (core::mask_t a = 0; a < blade_count(N); ++a) {
    std::int64_t denominator = 1;
    for (std::size_t i = 0; i < N; ++i) {
      if (((a >> i) & 1U) != 0) {
        denominator *= rows.denominators.at(i);
      }
    }
    const list image = core::detail::blade_image<capacity>(rows, a);
    for (std::size_t t = 0; t < image.size(); ++t) {
      images.at(a).at(image.terms.at(t).blade) =
          rational{image.terms.at(t).coefficient, denominator};
    }
  }
  return images;
}

// The number of terms of the images whose blade has another grade than the
// blade they are the image of.
template <std::size_t N>
int grade_mismatches(const std::array<dense<N>, blade_count(N)>& images) {
  int mismatches = 0;
  for (core::mask_t a = 0; a < blade_count(N); ++a) {
    for (core::mask_t b = 0; b < blade_count(N); ++b) {
      if (images.at(a).at(b) != rational{} &&
          std::popcount(a) != std::popcount(b)) {
        ++mismatches;
      }
    }
  }
  return mismatches;
}

// The linear map with these images, applied to x.
template <std::size_t N>
dense<N> apply(const std::array<dense<N>, blade_count(N)>& images,
               const dense<N>& x) {
  dense<N> out{};
  for (core::mask_t a = 0; a < blade_count(N); ++a) {
    if (x.at(a) == rational{}) {
      continue;
    }
    for (core::mask_t b = 0; b < blade_count(N); ++b) {
      out.at(b) = out.at(b) + (x.at(a) * images.at(a).at(b));
    }
  }
  return out;
}

// The geometric product of two sums of blades in the metric g, from the
// library's product of blades.
template <std::size_t N>
dense<N> product(const core::metric<N>& g, const dense<N>& x,
                 const dense<N>& y) {
  dense<N> out{};
  for (core::mask_t a = 0; a < blade_count(N); ++a) {
    if (x.at(a) == rational{}) {
      continue;
    }
    for (core::mask_t b = 0; b < blade_count(N); ++b) {
      if (y.at(b) == rational{}) {
        continue;
      }
      const list terms = core::detail::blade_product<capacity>(g, a, b);
      for (std::size_t t = 0; t < terms.size(); ++t) {
        const core::mask_t c = terms.terms.at(t).blade;
        out.at(c) = out.at(c) + (x.at(a) * y.at(b) *
                                 rational{terms.terms.at(t).coefficient});
      }
    }
  }
  return out;
}

// The rows of a change as a matrix of fractions.
template <std::size_t N>
test_support::rational_matrix<N> matrix_of(const core::basis_change<N>& c) {
  test_support::rational_matrix<N> p{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      p.at(i).at(j) = rational{c.rows.at(i).at(j), c.denominators.at(i)};
    }
  }
  return p;
}

// det of a matrix of fractions, by elimination with row swaps.
template <std::size_t N>
rational determinant(test_support::rational_matrix<N> m) {
  rational det{1};
  for (std::size_t k = 0; k < N; ++k) {
    std::size_t pivot = k;
    while (pivot < N && m.at(pivot).at(k) == rational{}) {
      ++pivot;
    }
    if (pivot == N) {
      return rational{};
    }
    if (pivot != k) {
      std::swap(m.at(pivot), m.at(k));
      det = -det;
    }
    det = det * m.at(k).at(k);
    for (std::size_t i = k + 1; i < N; ++i) {
      const rational f = m.at(i).at(k) / m.at(k).at(k);
      for (std::size_t j = k; j < N; ++j) {
        m.at(i).at(j) = m.at(i).at(j) - (f * m.at(k).at(j));
      }
    }
  }
  return det;
}

// Whether P G' P^T = G, computed here, so that a failure below is the
// library's and not the pairing's.
template <std::size_t N>
bool keeps_inner_products(const core::metric<N>& source,
                          const core::metric<N>& target,
                          const core::basis_change<N>& change) {
  const auto p = matrix_of(change);
  test_support::rational_matrix<N> g{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      g.at(i).at(j) = rational{target.gram.at(i).at(j)};
    }
  }
  const auto pgp = test_support::multiply(test_support::multiply(p, g),
                                          test_support::transpose(p));
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      if (pgp.at(i).at(j) != rational{source.gram.at(i).at(j)}) {
        return false;
      }
    }
  }
  return true;
}

// What the checks of one change found: the number of pairs of blades on
// which phi is not multiplicative, of image terms of the wrong grade, of
// blades that a round trip does not return, and whether the scalar and the
// antiscalar go where they must.
struct findings {
  int products = 0;
  int grades = 0;
  int forward_round_trips = 0;
  int back_round_trips = 0;
  bool scalar = false;
  bool antiscalar = false;
};

template <std::size_t N>
findings check_change(const core::metric<N>& source,
                      const core::metric<N>& target,
                      const core::basis_change<N>& change) {
  const auto phi = images_of<N>(change);
  const auto checked = core::detail::check_change(change, source, target);
  REQUIRE(checked.fault == core::detail::ChangeFault::none);
  const auto back = images_of<N>(checked.inverse);
  constexpr core::mask_t all = blade_count(N) - 1;

  findings f;
  f.grades = grade_mismatches<N>(phi) + grade_mismatches<N>(back);
  f.scalar = phi.at(0) == unit<N>(0) && back.at(0) == unit<N>(0);
  dense<N> antiscalar{};
  antiscalar.at(all) = determinant(matrix_of(change));
  f.antiscalar = phi.at(all) == antiscalar;
  for (core::mask_t a = 0; a < blade_count(N); ++a) {
    if (apply<N>(back, phi.at(a)) != unit<N>(a)) {
      ++f.forward_round_trips;
    }
    if (apply<N>(phi, back.at(a)) != unit<N>(a)) {
      ++f.back_round_trips;
    }
    for (core::mask_t b = 0; b < blade_count(N); ++b) {
      const dense<N> left =
          apply<N>(phi, product<N>(source, unit<N>(a), unit<N>(b)));
      const dense<N> right = product<N>(target, phi.at(a), phi.at(b));
      if (left != right) {
        ++f.products;
      }
    }
  }
  return f;
}

template <std::size_t N>
void check(const char* name, const core::metric<N>& source,
           const core::metric<N>& target, const core::basis_change<N>& change) {
  INFO(name);
  REQUIRE(keeps_inner_products(source, target, change));
  const findings f = check_change(source, target, change);
  CHECK(f.products == 0);
  CHECK(f.grades == 0);
  CHECK(f.forward_round_trips == 0);
  CHECK(f.back_round_trips == 0);
  CHECK(f.scalar);
  CHECK(f.antiscalar);
}

} // namespace

// Reference: proof: a change P with P G' P^T = G is an algebra map of the
//   Clifford algebras of G and G', since each vector's image squares to its
//   square and the wedge basis is the antisymmetrized product, so
//   phi(e_A e_B) = phi(e_A) phi(e_B) on every pair of blades, phi(1) = 1,
//   phi keeps grades, as each minor pairs sets of one size, phi sends the
//   antiscalar to det(P) times the target's, its top minor, and the change
//   by P^-1 undoes it both ways, checked on every blade and pair of blades
//   of every form of the exhaustive tests against the diagonal counterpart
//   D = T G T^T of its exact diagonalization, with P = T^-1
TEST_CASE("each form's change to its diagonal counterpart is an "
          "isomorphism") {
  galoot_test::for_each_signature([](const char* name, const auto& g) {
    const auto d = forms::to_diagonal(g);
    check(name, g, d.target, d.change);
  });
}

// Reference: proof: as for the diagonal counterparts, an isometry is an
//   algebra map, here conformal_change<d>() from conformal(d), with its null
//   pair e(d+1) . e(d+2) = -1, to R(d+1, 1, 0), d = 1 to 4, of determinant
//   -1, the Witt changes e(2j-1) = (e'j + e'(m+j)) / 2,
//   e(2j) = e'j - e'(m+j) of one and two modes to R(m, m, 0), each of
//   determinant -1, and rigid(3) to itself with e4 = 2 e'4, of
//   determinant 2, each checked on every blade and pair of blades
TEST_CASE("the conformal, Witt, and doubled changes are isomorphisms") {
  check("conformal(1)", core::conformal<1>(), core::R<2, 1, 0>(),
        core::conformal_change<1>());
  check("conformal(2)", core::conformal<2>(), core::R<3, 1, 0>(),
        core::conformal_change<2>());
  check("conformal(3)", core::conformal<3>(), core::R<4, 1, 0>(),
        core::conformal_change<3>());
  check("conformal(4)", core::conformal<4>(), core::R<5, 1, 0>(),
        core::conformal_change<4>());
  check("Witt, one mode", forms::witt_one, core::R<1, 1, 0>(),
        forms::witt_change<1>());
  check("Witt, two modes", forms::witt_two, core::R<2, 2, 0>(),
        forms::witt_change<2>());
  check("rigid(3), e4 doubled", core::rigid<3>(), core::rigid<3>(),
        forms::doubled_null_change);
}
