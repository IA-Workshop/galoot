#ifndef GALOOT_TEST_CORE_IDENTITIES_ROUNDING_CHECKS_HPP
#define GALOOT_TEST_CORE_IDENTITIES_ROUNDING_CHECKS_HPP

// The comparison of the two sides of an identity over double with the same
// computation over test_support::tracked, and the laws of associativity and
// the De Morgan laws that identities_rounding_test.cpp checks on the
// full multivectors of rigid(3) and conformal(3) it draws.
//
// The test case, in identities_rounding_test.cpp, draws every input from one
// seeded source, in turn, and passes each draw to the laws. Each law is a
// function of its own, defined for both algebras in one of the sources
// identities_rounding_<part>_test.cpp, so that no function holds the code of
// two laws of conformal(3): Clang, optimizing with the products inlined,
// spends several times as much work on one function that holds all the
// laws of a check as on the same laws one function each. The sources also
// compile in parallel.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <string>
#include <type_traits>

#include <galoot/core/bits.hpp>
#include <galoot/core/blade_set.hpp>
#include <galoot/core/metric.hpp>
#include <galoot/core/mv.hpp>

#include <doctest/doctest.h>

#include <support/rational.hpp>
#include <support/tracked.hpp>

namespace galoot_test::identities_rounding {

namespace core = galoot::core;
using test_support::tracked;

struct rga3 {
  static constexpr auto value = core::rigid<3>();
};
struct cga3 {
  static constexpr auto value = core::conformal<3>();
};

// The number of generators of an algebra.
template <class Alg>
inline constexpr std::size_t generators =
    std::remove_cvref_t<decltype(Alg::value)>::generator_count;

// A full multivector of Alg over tracked, and the same over double.
template <class Alg>
using full_tracked = core::mv<Alg, core::full<generators<Alg>>, tracked>;
template <class Alg>
using full_double = core::mv<Alg, core::full<generators<Alg>>, double>;

// What the comparisons found, over every coefficient of every identity.
struct tally {
  int coefficients = 0;
  int problems = 0;
  int equal_sides = 0;
  // The largest ratio of |L - R| to the sum of the two bounds
  // gamma_(K_L) M_L + gamma_(K_R) M_R, computed in double.
  double largest_ratio = 0.0;
};

// The laws checked on each draw a, b, c of tracked inputs, with da, db, dc
// the same values over double, for Alg either rga3 or cga3. The name of the
// form starts the name of each identity in a failure.

// (a ⟑ b) ⟑ c = a ⟑ (b ⟑ c), in identities_rounding_product_test.cpp.
template <class Alg>
void product_associativity(const full_tracked<Alg>& a,
                           const full_tracked<Alg>& b,
                           const full_tracked<Alg>& c,
                           const full_double<Alg>& da,
                           const full_double<Alg>& db,
                           const full_double<Alg>& dc, const std::string& form,
                           tally& out);

// (a ⟇ b) ⟇ c = a ⟇ (b ⟇ c), in identities_rounding_antiproduct_test.cpp.
template <class Alg>
void antiproduct_associativity(const full_tracked<Alg>& a,
                               const full_tracked<Alg>& b,
                               const full_tracked<Alg>& c,
                               const full_double<Alg>& da,
                               const full_double<Alg>& db,
                               const full_double<Alg>& dc,
                               const std::string& form, tally& out);

// The De Morgan laws on each draw a, b, as above.

// right(a ⟑ b) = right(a) ⟇ right(b) and left(a ⟑ b) = left(a) ⟇ left(b),
// in identities_rounding_complement_product_test.cpp.
template <class Alg>
void right_complement_of_product(const full_tracked<Alg>& a,
                                 const full_tracked<Alg>& b,
                                 const full_double<Alg>& da,
                                 const full_double<Alg>& db,
                                 const std::string& form, tally& out);
template <class Alg>
void left_complement_of_product(const full_tracked<Alg>& a,
                                const full_tracked<Alg>& b,
                                const full_double<Alg>& da,
                                const full_double<Alg>& db,
                                const std::string& form, tally& out);

// right(a ⟇ b) = right(a) ⟑ right(b) and left(a ⟇ b) = left(a) ⟑ left(b),
// in identities_rounding_complement_antiproduct_test.cpp.
template <class Alg>
void right_complement_of_antiproduct(const full_tracked<Alg>& a,
                                     const full_tracked<Alg>& b,
                                     const full_double<Alg>& da,
                                     const full_double<Alg>& db,
                                     const std::string& form, tally& out);
template <class Alg>
void left_complement_of_antiproduct(const full_tracked<Alg>& a,
                                    const full_tracked<Alg>& b,
                                    const full_double<Alg>& da,
                                    const full_double<Alg>& db,
                                    const std::string& form, tally& out);

// right(a ^ b) = right(a) v right(b), left(a ^ b) = left(a) v left(b),
// right(a v b) = right(a) ^ right(b), and left(a v b) = left(a) ^ left(b),
// in identities_rounding_complement_wedge_test.cpp.
template <class Alg>
void right_complement_of_wedge(const full_tracked<Alg>& a,
                               const full_tracked<Alg>& b,
                               const full_double<Alg>& da,
                               const full_double<Alg>& db,
                               const std::string& form, tally& out);
template <class Alg>
void left_complement_of_wedge(const full_tracked<Alg>& a,
                              const full_tracked<Alg>& b,
                              const full_double<Alg>& da,
                              const full_double<Alg>& db,
                              const std::string& form, tally& out);
template <class Alg>
void right_complement_of_antiwedge(const full_tracked<Alg>& a,
                                   const full_tracked<Alg>& b,
                                   const full_double<Alg>& da,
                                   const full_double<Alg>& db,
                                   const std::string& form, tally& out);
template <class Alg>
void left_complement_of_antiwedge(const full_tracked<Alg>& a,
                                  const full_tracked<Alg>& b,
                                  const full_double<Alg>& da,
                                  const full_double<Alg>& db,
                                  const std::string& form, tally& out);

// The helpers have internal linkage, as they had in a single source, so
// that each source keeps its own copy of what it instantiates.
// NOLINTNEXTLINE(misc-anonymous-namespace-in-header): one copy per source
namespace {

// The name of the canonical blade of a mask: e followed by the indices of
// its generators, or 1.
inline std::string blade_name(core::mask_t mask) {
  if (mask == 0) {
    return "1";
  }
  std::string name = "e";
  for (unsigned i = 0; i < 32; ++i) {
    if (((mask >> i) & 1U) != 0) {
      name += std::to_string(i + 1);
    }
  }
  return name;
}

// The coefficient of x on the canonical blade of this mask, or zero, with
// K = 0 and M = 0 for a tracked one, when the layout lacks the blade.
template <class X>
X::scalar_type value_on(const X& x, core::mask_t mask) {
  for (std::size_t k = 0; k < X::component_count; ++k) {
    const core::oriented_blade blade = X::layout.blades.at(k);
    if (blade.mask == mask) {
      return blade.sign < 0 ? -x.coefficients.at(k) : x.coefficients.at(k);
    }
  }
  return typename X::scalar_type{};
}

inline double gamma_of(int k) {
  const double ku = k * test_support::unit_roundoff;
  return ku / (1.0 - ku);
}

// Compares the sides of one identity, blade by blade over both layouts:
// tl and tr over tracked, dl and dr the same over double.
template <class TL, class TR, class DL, class DR>
void compare(const std::string& identity, const TL& tl, const TR& tr,
             const DL& dl, const DR& dr, tally& out) {
  const auto at = [&](core::mask_t mask) {
    const tracked l = value_on(tl, mask);
    const tracked r = value_on(tr, mask);
    const double ld = value_on(dl, mask);
    const double rd = value_on(dr, mask);
    ++out.coefficients;
    const bool exact_equal = l.value == r.value;
    const bool left_within = test_support::within_rounding_bound(
        ld, l.value, l.roundings, l.magnitude);
    const bool right_within = test_support::within_rounding_bound(
        rd, r.value, r.roundings, r.magnitude);
    const bool sides_within = test_support::within_identity_bound(
        ld, l.roundings, l.magnitude, rd, r.roundings, r.magnitude);
    if (!(exact_equal && left_within && right_within && sides_within)) {
      ++out.problems;
      const std::string problem = std::format(
          "{} on {}: exact sides equal {}, left within {}, right within {}, "
          "sides within {}; L = {} with K = {}, R = {} with K = {}",
          identity, blade_name(mask), exact_equal, left_within, right_within,
          sides_within, ld, l.roundings, rd, r.roundings);
      FAIL_CHECK(problem);
    }
    if (l.roundings <= 1 && r.roundings <= 1) {
      ++out.equal_sides;
    } else if (ld != rd) {
      const double bound =
          (gamma_of(l.roundings) * test_support::nearest_double(l.magnitude)) +
          (gamma_of(r.roundings) * test_support::nearest_double(r.magnitude));
      out.largest_ratio =
          std::max(out.largest_ratio, std::abs(ld - rd) / bound);
    }
  };
  for (const core::oriented_blade& blade : TL::layout.blades) {
    at(blade.mask);
  }
  for (const core::oriented_blade& blade : TR::layout.blades) {
    bool in_left = false;
    for (const core::oriented_blade& other : TL::layout.blades) {
      in_left = in_left || other.mask == blade.mask;
    }
    if (!in_left) {
      at(blade.mask);
    }
  }
}

} // namespace

} // namespace galoot_test::identities_rounding

#endif // GALOOT_TEST_CORE_IDENTITIES_ROUNDING_CHECKS_HPP
