// The identities that relate the products, the antiproducts, the
// involutions, and the complements of multivectors, checked through the
// public functions and operators of galoot/core/product.hpp and
// galoot/core/unary.hpp over many signatures: diagonal, degenerate,
// indefinite, and not diagonal.
//
// Each test case checks one identity over every group of forms in turn,
// from one seeded source; the checks and the groups are in
// identities_checks.hpp and the sources identities_<group>_test.cpp.
#include <random>

#include <doctest/doctest.h>

#include "identities_checks.hpp"

namespace {

using galoot_test::identities::groups;
using galoot_test::identities::Identity;

// Checks the identity over every group, in order, from the seed of the run.
void check_every_group(Identity identity) {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  for (const auto check_group : groups) {
    check_group(identity, source);
  }
}

} // namespace

// Reference: identity: the De Morgan laws right(a ⟑ b) = right(a) ⟇ right(b)
//   and right(a ^ b) = right(a) v right(b), the same with left, and their
//   converses right(a ⟇ b) = right(a) ⟑ right(b) and
//   right(a v b) = right(a) ^ right(b), with left likewise, which follow
//   from a ⟇ b = left(right(a) ⟑ right(b)) and a v b = left(right(a) ^
//   right(b)) because left and right are inverses, on random integer
//   multivectors of every blade for each form of up to four generators and
//   R(4,1,0), R(4,0,1), and conformal(3), on bivectors for two dense random
//   forms of five, on vectors and bivectors for six, and on sparse sets for
//   8, 12, and 16
TEST_CASE("the De Morgan laws hold on multivectors over many forms") {
  check_every_group(Identity::de_morgan);
}

// Reference: identity: the geometric antiproduct and the antiwedge are
//   associative, and the antiscalar is the identity of the geometric
//   antiproduct on both sides, because right(𝟙) is the scalar 1, on random
//   integer multivectors of every blade for each form of up to four
//   generators and R(4,1,0), R(4,0,1), and conformal(3), on vectors for two
//   dense random forms of five, on vectors and bivectors for six, and on
//   sparse sets for 8, 12, and 16
TEST_CASE("the antiproducts are associative over many forms") {
  check_every_group(Identity::antiproduct_laws);
}

// Reference: identity: the reverse reverses products,
//   reverse(a ⟑ b) = reverse(b) ⟑ reverse(a) and
//   reverse(a ^ b) = reverse(b) ^ reverse(a), the antireverse reverses
//   antiproducts, antireverse(a ⟇ b) = antireverse(b) ⟇ antireverse(a),
//   and the grade involution keeps the order,
//   grade_involution(a ⟑ b) = grade_involution(a) ⟑ grade_involution(b), on
//   random integer multivectors of every blade for each form of up to four
//   generators and R(4,1,0), R(4,0,1), and conformal(3), on bivectors for
//   two dense random forms of five, on vectors and bivectors for six, and on
//   sparse sets for 8, 12, and 16
TEST_CASE("the involutions follow their product rules over many forms") {
  check_every_group(Identity::product_rules);
}
