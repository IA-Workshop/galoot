// Fixture for tools/test_references.py trace, with trace_table.md,
// trace_conventions.md, and trace_golden.hpp. Not compiled.
#include <doctest/doctest.h>

// Reference: independent value: values from test/reference/golden.py
TEST_CASE("uses golden values without stating their fact") {
  CHECK(alpha::value == 1);
}

// Reference: independent value: the fact of alpha, from golden.py
TEST_CASE("uses golden values and states their fact") {
  CHECK(alpha::value == 1);
}
