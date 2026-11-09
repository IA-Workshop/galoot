// Validation of the change of basis against published statements of what it
// must be: conformal(d) against its orthonormal counterpart R(d+1, 1, 0), as
// the conformal model writes its null pair, and the Witt forms against
// R(m, m, 0), whose images of the creation and annihilation operators are
// the published Witt basis and satisfy the canonical anticommutation
// relations. Over the test rational, so every value is exact.
#include <array>
#include <cstddef>
#include <string>
#include <utility>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
// The sums of multivectors, found by argument-dependent lookup, which the
// include check does not see.
#include <galoot/core/linear.hpp> // IWYU pragma: keep
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

#include "basis_change_forms.hpp"
#include <support/rational.hpp>

namespace {

namespace core = galoot::core;
namespace bc = galoot_test::basis_changes;
using test_support::rational;

// The coefficient of x on the canonical blade of this mask: the stored
// coefficient times the orientation of the blade in the layout, or zero when
// the layout lacks the blade.
template <class X>
rational value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return rational{blade.sign} * x.coefficients.at(k);
    }
  }
  return rational{};
}

// The number of the n-generator algebra's blades on which x and the sum of
// the listed terms differ.
template <class X, std::size_t K>
int mismatches(const X& x, std::size_t n,
               const std::array<std::pair<core::mask_t, rational>, K>& terms) {
  int count = 0;
  for (core::mask_t b = 0; b < (core::mask_t{1} << n); ++b) {
    rational expected{};
    for (const auto& [blade, coefficient] : terms) {
      if (blade == b) {
        expected = expected + coefficient;
      }
    }
    if (value_on(x, b) != expected) {
      ++count;
    }
  }
  return count;
}

// The vector of the algebra Alg with the coefficient c on e_i, i from 1.
template <class Alg>
auto vector(std::size_t i, rational c) {
  constexpr std::size_t n = Alg::value.generator_count;
  core::mv<Alg, core::of_grade<n, 1>, rational> v{};
  v.coefficients.at(i - 1) = c;
  return v;
}

constexpr core::mask_t generator(std::size_t i) {
  return core::mask_t{1} << (i - 1);
}

// For conformal_change<D>(): the origin o = e(D+1) and the point at
// infinity n = e(D+2) map to (e- - e+) / 2 and e- + e+, with e+ = e'(D+1)
// and e- = e'(D+2); E = o ^ n maps to e- ^ e+ = -e'(D+1)(D+2); and both
// o n = -1 + E and E E = 1 hold in each algebra, the target's computed from
// the images.
template <std::size_t D>
void check_null_pair() {
  using C = bc::conformal_tag<D>;
  using Source = C::source;
  constexpr std::size_t n = D + 2;
  INFO("d: ", std::to_string(D));
  const auto o = vector<Source>(D + 1, rational{1});
  const auto infinity = vector<Source>(D + 2, rational{1});
  const core::mask_t plus = generator(D + 1);
  const core::mask_t minus = generator(D + 2);

  const auto o_image = core::change_basis<C>(o);
  const auto infinity_image = core::change_basis<C>(infinity);
  CHECK(mismatches(o_image, n,
                   std::array{std::pair{minus, rational{1, 2}},
                              std::pair{plus, rational{-1, 2}}}) == 0);
  CHECK(mismatches(infinity_image, n,
                   std::array{std::pair{minus, rational{1}},
                              std::pair{plus, rational{1}}}) == 0);

  const auto e = core::wedge(o, infinity);
  const auto e_image = core::change_basis<C>(e);
  CHECK(mismatches(e_image, n,
                   std::array{std::pair{plus | minus, rational{-1}}}) == 0);
  CHECK(mismatches(core::geometric_product(o, infinity), n,
                   std::array{std::pair{core::mask_t{0}, rational{-1}},
                              std::pair{plus | minus, rational{1}}}) == 0);
  CHECK(mismatches(core::geometric_product(o_image, infinity_image), n,
                   std::array{std::pair{core::mask_t{0}, rational{-1}},
                              std::pair{plus | minus, rational{-1}}}) == 0);
  CHECK(mismatches(core::geometric_product(e, e), n,
                   std::array{std::pair{core::mask_t{0}, rational{1}}}) == 0);
  CHECK(mismatches(core::geometric_product(e_image, e_image), n,
                   std::array{std::pair{core::mask_t{0}, rational{1}}}) == 0);
}

// The anticommutator a b + b a.
template <class A, class B>
auto anticommutator(const A& a, const B& b) {
  return core::geometric_product(a, b) + core::geometric_product(b, a);
}

// The canonical anticommutation relations of M modes in the algebra of the
// operators c and c_dagger: {c_j, c_k†} = delta_jk, and {c_j, c_k} =
// {c_j†, c_k†} = 0, for every j and k.
template <std::size_t M, class X>
void check_car(const std::array<X, M>& c, const std::array<X, M>& c_dagger) {
  constexpr std::size_t n = 2 * M;
  const std::array one{std::pair{core::mask_t{0}, rational{1}}};
  const std::array zero{std::pair{core::mask_t{0}, rational{}}};
  for (std::size_t j = 0; j < M; ++j) {
    for (std::size_t k = 0; k < M; ++k) {
      CAPTURE(j);
      CAPTURE(k);
      CHECK(mismatches(anticommutator(c.at(j), c_dagger.at(k)), n,
                       j == k ? one : zero) == 0);
      CHECK(mismatches(anticommutator(c.at(j), c.at(k)), n, zero) == 0);
      CHECK(mismatches(anticommutator(c_dagger.at(j), c_dagger.at(k)), n,
                       zero) == 0);
    }
  }
}

// For the Witt form of M modes and its change to R(M, M, 0): c_j = e(2j-1)
// and c_j† = e(2j) / 2 satisfy the relations; their images are
// w_j = (e'j + e'(M+j)) / 2 and w*_j = (e'j - e'(M+j)) / 2; and the images
// satisfy the relations too.
template <std::size_t M, class C>
void check_witt() {
  using Source = C::source;
  using X = decltype(vector<Source>(1, rational{}));
  using Y = decltype(core::change_basis<C>(X{}));
  INFO("modes: ", std::to_string(M));
  std::array<X, M> c{};
  std::array<X, M> c_dagger{};
  std::array<Y, M> w{};
  std::array<Y, M> w_star{};
  for (std::size_t j = 1; j <= M; ++j) {
    c.at(j - 1) = vector<Source>((2 * j) - 1, rational{1});
    c_dagger.at(j - 1) = vector<Source>(2 * j, rational{1, 2});
    w.at(j - 1) = core::change_basis<C>(c.at(j - 1));
    w_star.at(j - 1) = core::change_basis<C>(c_dagger.at(j - 1));
    CHECK(mismatches(w.at(j - 1), 2 * M,
                     std::array{std::pair{generator(j), rational{1, 2}},
                                std::pair{generator(M + j), rational{1, 2}}}) ==
          0);
    CHECK(mismatches(
              w_star.at(j - 1), 2 * M,
              std::array{std::pair{generator(j), rational{1, 2}},
                         std::pair{generator(M + j), rational{-1, 2}}}) == 0);
  }
  check_car<M>(c, c_dagger);
  check_car<M>(w, w_star);
}

} // namespace

// Reference: independent value: the conformal model adds to R^d two vectors
//   e+ and e- with e+ . e+ = 1 and e- . e- = -1 and defines the origin as
//   (e- - e+) / 2 and the point at infinity as e- + e+, so that both are null
//   with o . n = -1, and then e- ^ e+ = o ^ n (E. Lengyel, Conformal
//   Geometric Algebra wiki, Main Page, revision 209, Introduction), the same
//   pair as P. Colapinto, Articulating Space, 2016, the caption of Table
//   2.6, where E = n_o ^ n_inf has n_o n_inf = -1 + E (equation 2.41) and
//   E^2 = 1 (equation 2.43), so conformal_change<d>() with e+ = e'(d+1) and
//   e- = e'(d+2) must send the origin and the point at infinity to those
//   vectors, E to e- ^ e+ = -e'(d+1)(d+2), and o n and E E must keep their
//   values through it, for d = 1 to 4
TEST_CASE("conformal_change sends the null pair to the published pair") {
  check_null_pair<1>();
  check_null_pair<2>();
  check_null_pair<3>();
  check_null_pair<4>();
}

// Reference: independent value: with c_j = e(2j-1) and c_j† = e(2j) / 2 in
//   the scaled Witt form, c_j . c_k† = delta_jk / 2 and c_j . c_k =
//   c_j† . c_k† = 0, which with a . b = (a b + b a) / 2 gives the canonical
//   anticommutation relations {c_j, c_k†} = delta_jk and
//   {c_j, c_k} = {c_j†, c_k†} = 0 (C. Doran, D. Hestenes, F. Sommen, and
//   N. Van Acker, Lie groups as spin groups, J. Math. Phys. 34 (1993) 3642,
//   equations 2.5, 2.7, and 3.7, with the relation of equation 3.34 read
//   as w_i w*_j + w*_j w_i = delta_ij, which those equations give), and the
//   Witt basis of R(m, m, 0) is w_j = (e_j + ebar_j) / 2 and
//   w*_j = (e_j - ebar_j) / 2, inverting e_j = w_j + w*_j and
//   ebar_j = w_j - w*_j (equation 3.16), with e_j = e'j and
//   ebar_j = e'(m+j), so the Witt change must send c_j and c_j† to w_j and
//   w*_j and keep the relations, for one and two modes
TEST_CASE("the Witt change sends the fermion operators to the Witt basis") {
  check_witt<1, bc::witt_one_tag>();
  check_witt<2, bc::witt_two_tag>();
}
