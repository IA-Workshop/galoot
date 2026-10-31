// Fixture for tools/test_references.py golden and trace, with
// golden_fixture.hpp: every use of golden values states its fact, so both
// checks must pass. Not compiled.
#include <doctest/doctest.h>

namespace b = golden::beta;
namespace g = golden;

// A comment that names golden::alpha::value is not a use.

// Reference: independent value: The Fact Of Alpha, matched ignoring case
TEST_CASE("a direct use whose marker states the fact") {
  CHECK(golden::alpha::value == 1);
}

// Reference: independent value: e1 ∧ e2 = −e2 ∧ e1, with the wedge sign
TEST_CASE("a use through a namespace alias") {
  CHECK(b::value == 2);
  CHECK(std::string_view("golden::alpha is text, not a use").size() > 0);
}

// Reference: independent value: the fact of gamma
static_assert(g::gamma::value == 3);
