// The identities of galoot/core/transform.hpp over many signatures:
// diagonal, degenerate, indefinite, and not diagonal.
//
// A versor here is the antiproduct of one to three random hyperplanes, each
// a random element of grade n - 1, which is always a blade, so its antigrade
// is 1. For such a V, V ⟇ antireverse(V) is lambda times the antiscalar, and
// when lambda is not zero the transform by V is lambda times an
// outermorphism: lambda^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) for
// vectors a1 to ak; and transforms compose, T_(W ⟇ V) = T_W after T_V. Draws
// with lambda = 0 are skipped. Every value is an exact rational with
// integer entries, so both sides of each identity must be equal.
//
// The test cases check every group of forms in turn, from one seeded source;
// the checks are in transform_identities_checks.hpp and each group is
// checked in a source of its own, transform_identities_<group>_test.cpp.
#include <random>

#include <doctest/doctest.h>

#include "transform_identities_checks.hpp"

namespace {

namespace identities = galoot_test::transform_identities;

// The identities over every group of forms, in order, drawn from `source`.
void check_all_forms(std::mt19937_64& source) {
  identities::forms_up_to_three(source);
  identities::named_forms_of_four(source);
  identities::gram_forms_of_four(source);
  identities::rigid_and_conformal_forms_of_five(source);
  identities::diagonal_dense_and_large_forms(source);
}

} // namespace

// Reference: identity: for V the antiproduct of one to three random
//   hyperplanes, V ⟇ antireverse(V) is lambda times the antiscalar, and when
//   lambda is not zero the transform by V is lambda times an
//   outermorphism, lambda^(k-1) T(a1 ^ ... ^ ak) = T(a1) ^ ... ^ T(ak) at
//   every grade k from 2 to n, and transforms compose,
//   transform(x, compose(W, V)) = transform(transform(x, V), W) at every
//   grade, over the test rational with integer entries, for each form of two
//   to five generators, a dense random form of five, where the grades stop
//   at 3, the versors at two hyperplanes, and the compositions at pairs of
//   single hyperplanes, and R(8,0,0) and rigid(7) with
//   sparse hyperplanes at grades up to 2
TEST_CASE("versors of hyperplanes act as outermorphisms and compose over "
          "many forms") {
  const unsigned seed = doctest::getContextOptions()->rand_seed;
  INFO("seed: ", seed);
  std::mt19937_64 source(seed);
  check_all_forms(source);
}

// Reference: identity: the outermorphism and composition identities of the
//   case above, at the fixed seeds 8, 23, 27, 37, 38, 39, and 43, at which
//   hyperplane entries from -2 to 2 made the grade-four check over the
//   random form of four generators overflow the test rational
TEST_CASE("versors of hyperplanes act as outermorphisms and compose at the "
          "seeds that once overflowed the test rational") {
  for (const unsigned seed : {8U, 23U, 27U, 37U, 38U, 39U, 43U}) {
    CAPTURE(seed);
    std::mt19937_64 source(seed);
    check_all_forms(source);
  }
}
