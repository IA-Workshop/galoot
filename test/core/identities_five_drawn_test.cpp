// The identities over two diagonal forms of five generators drawn at
// random, R(0,3,2) and R(0,2,3), on every blade.
//
// Diagonal forms R(p, q, r) drawn with Python's random.Random(20261108):
// for n = 5 and then n = 6, of the (p, q, r) with p + q + r = n, ordered
// by p and then q, both descending, one chosen with q >= 2 and r >= 2,
// then one with r >= 3 other than the first: R(0,3,2), R(0,2,3),
// R(1,3,2), R(1,1,4).
#include <random>

#include <galoot/core/metric.hpp>

#include "identities_checks.hpp"

namespace galoot_test::identities {

void drawn_forms_of_five(Identity identity, std::mt19937_64& source) {
  check_each<full_mv, full_mv>(
      identity, forms<core::R<0, 3, 2>(), core::R<0, 2, 3>()>{}, source);
}

} // namespace galoot_test::identities
