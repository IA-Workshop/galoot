// Fixture for tools/test_references.py markers: every marker is well formed,
// so the check must pass. Not compiled.
#include <doctest/doctest.h>

// Reference: identity: a C3 rotation composed three times is the identity,
//   and so is a C2 rotation composed twice
TEST_CASE("point-group names are mathematics, not pointers") {}

// Reference: proof: the S2 sphere and the Klein group V4 = C2 x C2
TEST_CASE("more mathematics that looks like a numbered rule") {}

// Reference: identity: Cl(1,1) is isomorphic to M2(R), the 2 by 2 real
//   matrices, through an explicit matrix representation
TEST_CASE("a matrix algebra named like a milestone") {}

// Reference: regression guard: the current output of the formatter
TEST_CASE("a regression guard that stands alone") {}

// Reference: contract: GALOOT_ASSERT aborts on a false condition, as its
//   comment in galoot/core/assert.hpp states
TEST_CASE("a contract of a documented behavior") {}
