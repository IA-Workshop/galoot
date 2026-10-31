// Fixture for tools/test_references.py golden, with golden_bad_header.hpp:
// each use of golden values has one defect. Not compiled.
#include <doctest/doctest.h>

namespace b = golden::beta;
using namespace golden::gamma;

constexpr int twice() { return 2 * golden::alpha::value; }

// Reference: independent value: values from test/reference/golden.py
TEST_CASE("a direct use whose marker does not state the fact") {
  CHECK(golden::alpha::value == 1);
}

// Reference: independent value: values from test/reference/golden.py
TEST_CASE("a use through an alias whose marker does not state the fact") {
  CHECK(b::value == 2);
}

static_assert(golden::gamma::value == 3);
