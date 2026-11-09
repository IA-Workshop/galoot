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
// Each group of changes is checked in a source of its own,
// basis_change_<group>_test.cpp, so that the groups, whose products are
// most of the test's compile cost, compile in parallel.

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

#include <galoot/core/basis_change.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>
#include <galoot/core/product.hpp>

#include <doctest/doctest.h>

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

} // namespace

} // namespace galoot_test::basis_changes

#endif // GALOOT_TEST_CORE_BASIS_CHANGE_CHECKS_HPP
