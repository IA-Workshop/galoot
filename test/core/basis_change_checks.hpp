#ifndef GALOOT_TEST_CORE_BASIS_CHANGE_CHECKS_HPP
#define GALOOT_TEST_CORE_BASIS_CHANGE_CHECKS_HPP

// The checks of a change of basis through the public functions of
// galoot/core/basis_change.hpp, on seeded random multivectors over the test
// rational, for the kernel groups of core.basis_change_identities.
//
// Each check draws integer coefficients from -3 to 3, so that no value of
// either side, of the change, or of its inverse leaves the range of the test
// rational for any of the changes listed in basis_change_forms.hpp, and
// compares the two sides exactly. The two sides may hold their blades in
// different layouts, so they are compared blade by blade, on the canonical
// blade of each mask.
//
// What a change preserves, with d = det P and phi the change: the geometric
// product, the wedge, the dot, the reverse, the antireverse, and the grade
// involution with the factor 1; phi(a v b) = phi(a) v phi(b) / d for the
// antiwedge; and phi(x*) = d phi(x)* for the bulk duals, for every change.
// When the contragredient P^-T keeps the inner products too,
// P^-T G' P^-1 = G, also the geometric antiproduct, the weight duals, the
// antidot, and compose, each with 1 / d, and transform, before its result
// is cut to x's blades, with 1 / d^2. That condition holds for every change
// between forms whose Gram matrices are their own inverses, such as
// conformal(d) and R(d+1, 1, 0) or the Witt forms and R(m, m, 0), and for a
// change of a degenerate form that only rescales a null generator, such as
// rigid(3) with e4 = 2 e'4; for a nondegenerate form it makes d = 1 or -1.
//
// Each group of changes is checked in a source of its own,
// basis_change_<group>_test.cpp, so that the groups, whose products are
// most of the test's compile cost, compile in parallel.

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <utility>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/dual.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>
#include <galoot/core/transform.hpp>
#include <galoot/core/unary.hpp>

#include <doctest/doctest.h>

#include <support/diagonalize.hpp>
#include <support/rational.hpp>

namespace galoot_test::basis_changes {

// The checks have internal linkage, as they would in a single source, so
// that the compiler may inline each one into the one function that calls it.
// NOLINTNEXTLINE(misc-anonymous-namespace-in-header): one copy per source
namespace {

namespace core = galoot::core;
using test_support::rational;

// The number of random values each identity is checked on, for each change
// and layout.
constexpr int samples = 3;

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

inline std::string text(const rational& r) {
  return std::to_string(r.numerator) + "/" + std::to_string(r.denominator);
}

// The name of the canonical blade of a mask: e followed by the indices of
// its generators, separated by dots, or 1.
inline std::string blade_name(core::mask_t mask) {
  if (mask == 0) {
    return "1";
  }
  std::string name = "e";
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += (name.size() > 1 ? "." : "") + std::to_string(i + 1);
    }
  }
  return name;
}

// Empty when x and y have the same coefficient on every canonical blade that
// either layout holds; otherwise the first blade where they differ, with
// both values.
template <class X, class Y>
std::string difference(const X& x, const Y& y) {
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + text(value_on(x, mask)) + " against " +
           text(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (value_on(x, blade.mask) != value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

// A multivector of X's type with integer coefficients from -3 to 3.
template <class X>
X random_integers(std::mt19937_64& source) {
  std::uniform_int_distribution<std::int64_t> digit(-3, 3);
  X x{};
  for (auto& c : x.coefficients) {
    c = rational{digit(source)};
  }
  return x;
}

// P as a matrix of fractions, P_ij = rows[i][j] / denominators[i].
template <std::size_t N>
constexpr test_support::rational_matrix<N>
matrix_of(const core::basis_change<N>& c) {
  test_support::rational_matrix<N> p{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      p[i][j] = rational{c.rows[i][j], c.denominators[i]};
    }
  }
  return p;
}

// A Gram matrix as a matrix of fractions.
template <std::size_t N>
constexpr test_support::rational_matrix<N> gram_of(const core::metric<N>& g) {
  test_support::rational_matrix<N> m{};
  for (std::size_t i = 0; i < N; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
      m[i][j] = rational{g.gram[i][j]};
    }
  }
  return m;
}

// det of a matrix of fractions, by elimination with row swaps.
template <std::size_t N>
constexpr rational determinant(test_support::rational_matrix<N> m) {
  rational det{1};
  for (std::size_t k = 0; k < N; ++k) {
    std::size_t pivot = k;
    while (pivot < N && m[pivot][k] == rational{}) {
      ++pivot;
    }
    if (pivot == N) {
      return rational{};
    }
    if (pivot != k) {
      std::swap(m[pivot], m[k]);
      det = -det;
    }
    det = det * m[k][k];
    for (std::size_t i = k + 1; i < N; ++i) {
      const rational f = m[i][k] / m[k][k];
      for (std::size_t j = k; j < N; ++j) {
        m[i][j] = m[i][j] - (f * m[k][j]);
      }
    }
  }
  return det;
}

// det P of the change C.
template <class C>
inline constexpr rational determinant_of = determinant(matrix_of(C::value));

// Whether P^-T G' P^-1 = G for the change C, so that its contragredient
// keeps the inner products too.
template <class C>
constexpr bool contragredient_keeps_inner_products() {
  const auto q =
      test_support::transpose(test_support::inverse(matrix_of(C::value)));
  return test_support::multiply(
             test_support::multiply(q, gram_of(C::target::value)),
             test_support::transpose(q)) == gram_of(C::source::value);
}
template <class C>
inline constexpr bool dual_isometry = contragredient_keeps_inner_products<C>();

// Empty when x and factor times y have the same coefficient on every
// canonical blade that either layout holds; otherwise the first blade where
// they differ, with both values.
template <class X, class Y>
std::string scaled_difference(const X& x, const rational& factor, const Y& y) {
  const auto report = [&](core::mask_t mask) {
    return blade_name(mask) + ": " + text(value_on(x, mask)) + " against " +
           text(factor) + " times " + text(value_on(y, mask));
  };
  for (const core::oriented_blade& blade : X::layout.blades) {
    if (value_on(x, blade.mask) != factor * value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  for (const core::oriented_blade& blade : Y::layout.blades) {
    if (value_on(x, blade.mask) != factor * value_on(y, blade.mask)) {
      return report(blade.mask);
    }
  }
  return "";
}

// A multivector of the source algebra of the change C on the layout S.
template <class C, core::blade_set S>
using in_source = core::mv<typename C::source, S, rational>;

// A multivector of the target algebra of the change C on the layout S.
template <class C, core::blade_set S>
using in_target = core::mv<typename C::target, S, rational>;

// change_basis(x y) = change_basis(x) change_basis(y) and the same for the
// wedge, on random x on A and y on B of C's source.
template <class C, core::blade_set A, core::blade_set B>
void check_products(std::mt19937_64& source, const char* name) {
  INFO("change: ", name);
  for (int k = 0; k < samples; ++k) {
    const auto x = random_integers<in_source<C, A>>(source);
    const auto y = random_integers<in_source<C, B>>(source);
    const auto x_image = core::change_basis<C>(x);
    const auto y_image = core::change_basis<C>(y);
    CHECK(difference(core::change_basis<C>(core::geometric_product(x, y)),
                     core::geometric_product(x_image, y_image)) == "");
    CHECK(difference(core::change_basis<C>(core::wedge(x, y)),
                     core::wedge(x_image, y_image)) == "");
  }
}

// truncate<S>(change_basis_back(change_basis(x))) = x for random x on S of
// C's source, and the same through the target for random y on S of C's
// target.
template <class C, core::blade_set S>
void check_round_trips(std::mt19937_64& source, const char* name) {
  INFO("change: ", name);
  using X = in_source<C, S>;
  using Y = in_target<C, S>;
  for (int k = 0; k < samples; ++k) {
    const X x = random_integers<X>(source);
    CHECK(core::truncate<X>(
              core::change_basis_back<C>(core::change_basis<C>(x))) == x);
    const Y y = random_integers<Y>(source);
    CHECK(core::truncate<Y>(
              core::change_basis<C>(core::change_basis_back<C>(y))) == y);
  }
}

// The operations a change preserves, with their factors, on random a on A
// and b on B of C's source: the dot, the involutions, the antiwedge, and the
// bulk duals for every change, and the antiproduct family where the
// contragredient keeps the inner products too.
template <class C, core::blade_set A, core::blade_set B>
void check_agreement(std::mt19937_64& source, const char* name) {
  INFO("change: ", name);
  const rational d = determinant_of<C>;
  const rational inverse_d = rational{1} / d;
  INFO("det P: ", text(d));
  for (int k = 0; k < samples; ++k) {
    const auto a = random_integers<in_source<C, A>>(source);
    const auto b = random_integers<in_source<C, B>>(source);
    const auto a_image = core::change_basis<C>(a);
    const auto b_image = core::change_basis<C>(b);
    CHECK(difference(core::change_basis<C>(core::dot(a, b)),
                     core::dot(a_image, b_image)) == "");
    CHECK(difference(core::change_basis<C>(core::reverse(a)),
                     core::reverse(a_image)) == "");
    CHECK(difference(core::change_basis<C>(core::antireverse(a)),
                     core::antireverse(a_image)) == "");
    CHECK(difference(core::change_basis<C>(core::grade_involution(a)),
                     core::grade_involution(a_image)) == "");
    CHECK(scaled_difference(core::change_basis<C>(core::antiwedge(a, b)),
                            inverse_d,
                            core::antiwedge(a_image, b_image)) == "");
    CHECK(scaled_difference(core::change_basis<C>(core::bulk_dual(a)), d,
                            core::bulk_dual(a_image)) == "");
    CHECK(scaled_difference(core::change_basis<C>(core::left_bulk_dual(a)), d,
                            core::left_bulk_dual(a_image)) == "");
    if constexpr (dual_isometry<C>) {
      CHECK(scaled_difference(
                core::change_basis<C>(core::geometric_antiproduct(a, b)),
                inverse_d,
                core::geometric_antiproduct(a_image, b_image)) == "");
      CHECK(scaled_difference(core::change_basis<C>(core::antidot(a, b)),
                              inverse_d,
                              core::antidot(a_image, b_image)) == "");
      CHECK(scaled_difference(core::change_basis<C>(core::weight_dual(a)),
                              inverse_d, core::weight_dual(a_image)) == "");
      CHECK(scaled_difference(core::change_basis<C>(core::left_weight_dual(a)),
                              inverse_d,
                              core::left_weight_dual(a_image)) == "");
    }
  }
}

// A random hyperplane of C's source, of grade n - 1, with entries from -1
// to 1, so that the versors built from two of them keep every product of a
// transform within the range of the test rational.
template <class C>
auto random_hyperplane(std::mt19937_64& source) {
  constexpr std::size_t n = C::value.generator_count;
  using H = in_source<C, core::of_grade<n, static_cast<int>(n) - 1>>;
  std::uniform_int_distribution<std::int64_t> digit(-1, 1);
  H h{};
  for (auto& c : h.coefficients) {
    c = rational{digit(source)};
  }
  return h;
}

// For a change whose contragredient keeps the inner products: compose and
// transform, on versors V and W that are each the antiproduct of two random
// hyperplanes and on random x on X of C's source, X of whole grades, so
// that transform drops nothing: phi(compose(W, V)) = compose(phi(W),
// phi(V)) / d and phi(transform(x, V)) = transform(phi(x), phi(V)) / d^2.
template <class C, core::blade_set X>
void check_transform(std::mt19937_64& source, const char* name) {
  static_assert(dual_isometry<C>);
  INFO("change: ", name);
  const rational d = determinant_of<C>;
  INFO("det P: ", text(d));
  for (int k = 0; k < samples; ++k) {
    const auto v = core::geometric_antiproduct(random_hyperplane<C>(source),
                                               random_hyperplane<C>(source));
    const auto w = core::geometric_antiproduct(random_hyperplane<C>(source),
                                               random_hyperplane<C>(source));
    const auto x = random_integers<in_source<C, X>>(source);
    const auto v_image = core::change_basis<C>(v);
    CHECK(scaled_difference(
              core::change_basis<C>(core::compose(w, v)), rational{1} / d,
              core::compose(core::change_basis<C>(w), v_image)) == "");
    CHECK(scaled_difference(
              core::change_basis<C>(core::transform(x, v)),
              rational{1} / (d * d),
              core::transform(core::change_basis<C>(x), v_image)) == "");
  }
}

} // namespace

} // namespace galoot_test::basis_changes

#endif // GALOOT_TEST_CORE_BASIS_CHANGE_CHECKS_HPP
