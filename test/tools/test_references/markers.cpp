// Fixture for tools/test_references.py markers: every test case but the last
// has one defect in its reference marker. Not compiled.
#include <doctest/doctest.h>

TEST_CASE("a test case with no comment above it") {}

// Reference: proof: a comment separated from its test case by a blank line

TEST_CASE("a test case whose marker is cut off by a blank line") {}

// Reference: u ^ right(u) = antiscalar
TEST_CASE("a source that names no kind of evidence") {}

// Reference: regression guard: the current output; identity: (ab)c = a(bc)
TEST_CASE("a regression guard with a second source") {}

// Reference: identity: (ab)c = a(bc)
TEST_CASE("regression of the product table, not labeled as a guard") {}

// Reference: proof: as decision 0034 explains
TEST_CASE("a marker that points to a record instead of a reason") {}

// Reference: independent value: right complements from
//   u ^ right(u) = antiscalar, computed by test/reference/golden.py
TEST_CASE("a well-formed marker continued on a second line") {}
